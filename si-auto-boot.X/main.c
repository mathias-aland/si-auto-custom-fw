/* 
 * File:   main.c
 * Author: mathias
 *
 * Created on den 24 juni 2012, 18:44
 */

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "nvm.h"
#include "modbus.h"

//CONFIG1L
#pragma config FEXTOSC = XT     // External Oscillator Mode Selection->XT (crystal oscillator) above 100 kHz, below 8 MHz; PFM set to medium power
#pragma config RSTOSC = EXTOSC_4PLL     // Power-Up Default Value for COSC->EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits

//CONFIG1H
#pragma config CLKOUTEN = OFF     // Clock Out Enable->CLKOUT function is disabled
#pragma config FCMEN = OFF     // Fail-Safe Clock Monitor Enable->Fail-Safe Clock Monitor disabled
#pragma config CSWEN = ON     // Clock Switch Enable->Writing to NOSC and NDIV is allowed

//CONFIG2L
#pragma config MCLRE = EXTMCLR     // Master Clear (MCLR) Enable->MCLR pin (RE3) is MCLR
#pragma config BOREN = SBORDIS     // Brown-Out Reset Enable->Brown-out Reset enabled , SBOREN bit is ignored
#pragma config PWRTE = OFF     // Power-Up Timer Enable->Power up timer disabled
#pragma config LPBOREN = OFF     // Low-Power BOR Enable->Low power BOR is disabled

//CONFIG2H
#pragma config XINST = OFF     // Extended Instruction Set Enable->Extended Instruction Set and Indexed Addressing Mode disabled
#pragma config ZCD = OFF     // ZCD Disable->ZCD disabled. ZCD can be enabled by setting the ZCDSEN bit of ZCDCON
#pragma config STVREN = ON     // Stack Overflow/Underflow Reset Enable->Stack full/underflow will cause Reset
#pragma config BORV = VBOR_285     // Brown-out Reset Voltage Selection->Brown-out Reset Voltage (VBOR) set to 2.85V
#pragma config PPS1WAY = OFF     // PPSLOCKED bit One-Way Set Enable->PPSLOCK bit can be set and cleared repeatedly (subject to the unlock sequence)

//CONFIG3L
#pragma config WDTCPS = WDTCPS_31     // WDT Period Select->Divider ratio 1:65536; software control of WDTPS
#pragma config WDTE = SWDTEN     // WDT Operating Mode->WDT enabled/disabled by SWDTEN bit

//CONFIG3H
#pragma config WDTCWS = WDTCWS_7     // WDT Window Select->window always open (100%); software control; keyed access not required
#pragma config WDTCCS = SC     // WDT Input Clock Selector->Software Control

//CONFIG4L
#pragma config WRT1 = OFF     // User NVM Self-Write Protection Block 1->Block 1 (004000-007FFFh) not write-protected
#pragma config WRT0 = OFF     // User NVM Self-Write Protection Block 0->Block 0 (000800-003FFFh) not write-protected
#pragma config WRT2 = OFF     // User NVM Self-Write Protection Block 2->Block 2 (008000-00BFFFh) not write-protected
#pragma config WRT3 = OFF     // User NVM Self-Write Protection Block 3->Block 3 (00C000-00FFFFh) not write-protected

//CONFIG4H
#pragma config WRTB = OFF     // Boot Block Write Protection->Boot Block (000000-0007FFh) not write-protected
#pragma config WRTC = OFF     // Configuration Register Write Protection->Configuration registers (300000-30000Bh) not write-protected
#pragma config WRTD = OFF     // Data EEPROM Write Protection->Data EEPROM not write-protected
#pragma config LVP = ON     // Low Voltage Programming Enable->Low voltage programming enabled. MCLR/VPP pin function is MCLR. MCLRE configuration bit is ignored
#pragma config SCANE = ON     // Scanner Enable->Scanner module is available for use, SCANMD bit can control the module

//CONFIG5L
#pragma config CPD = OFF     // Data NVM (DFM) Memory Code Protection->DataNVM code protection disabled
#pragma config CP = OFF     // User NVM Program Memory Code Protection->UserNVM code protection disabled

//CONFIG6L
#pragma config EBTR1 = OFF     // Table Read Protection Block 1->Block 1 (004000-007FFFh) not protected from table reads executed in other blocks
#pragma config EBTR0 = OFF     // Table Read Protection Block 0->Block 0 (000800-003FFFh) not protected from table reads executed in other blocks
#pragma config EBTR3 = OFF     // Table Read Protection Block 3->Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks
#pragma config EBTR2 = OFF     // Table Read Protection Block 2->Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks

//CONFIG6H
#pragma config EBTRB = OFF     // Boot Block Table Read Protection->Boot Block (000000-0007FFh) not protected from table reads executed in other blocks

//#define _XTAL_FREQ 64000000
#define _XTAL_FREQ 16777216

#define CMD_DISPLAY_OFF    0b00001000 //Display off
#define CMD_DISPLAY_ON_N   0b00001100 //Display on, no cursor
#define CMD_DISPLAY_ON_C   0b00001110 //Display on, cursor
#define CMD_DISPLAY_ON_B   0b00001101 //Display on, blink
#define CMD_DISPLAY_ON_CB  0b00001111 //Display on, blink, cursor
#define CMD_DISPLAY_CLEAR  0b00000001 //Clear display
#define CMD_DISPLAY_HOME   0b00000010 //Return home
#define CMD_DISPLAY_MODE   0b00000110 //Entry mode set (increment, no shift)
#define CMD_INIT           0b00111000 //Function set (8-bit 2-line)
#define CMD_SET_DDRAM_ADDR 0b10000000 //Set DDRAM address
#define CMD_SET_CGRAM_ADDR 0b01000000 //Set DDRAM address

#define LCD_LINE_1         0x00       //Line 1 address
#define LCD_LINE_2         0x40       //Line 2 address

#define LCD_RS      (LATCbits.LATC0)
#define LCD_RW      (LATCbits.LATC1)
#define LCD_E       (LATCbits.LATC2)
#define LCD_DPORT   (LATB)

#define OUTPUT_ENABLE	(LATAbits.LATA4)

#define PROG_START  0x800
 
asm( "PSECT HiVector,class=CODE,delta=1,abs "); 
asm( "ORG 0x08 "); 
asm ( "GOTO  " ___mkstr(PROG_START+0x8));
asm( "PSECT LoVector,class=CODE,delta=1,abs "); 
asm( "ORG 0x18 "); 
asm ( "GOTO  " ___mkstr(PROG_START+0x18));

void lcd_write(unsigned char data, bool isCmd)
{
    LCD_RW = 0;     // write
    
    if (isCmd)
        LCD_RS = 0;
    else
        LCD_RS = 1;
    
    LCD_DPORT = data;
    
    TRISB = 0x00;   // Enable port drivers
    
    __delay_us(1);
    
    // Strobe
    LCD_E = 1;
    __delay_us(1);
    LCD_E = 0;
    __delay_us(1);   
    
    TRISB = 0xFF;   // Disable port drivers
    
    __delay_us(100);
}

void lcd_init()
{
    lcd_write(CMD_INIT, true); // init
    __delay_ms(5);
    lcd_write(CMD_INIT, true); // init
    __delay_ms(5);
    lcd_write(CMD_INIT, true); // init
    __delay_ms(5);
    lcd_write(CMD_DISPLAY_OFF, true);
    __delay_ms(5);
    lcd_write(CMD_DISPLAY_CLEAR, true);
    __delay_ms(5);
    lcd_write(CMD_DISPLAY_MODE, true);
    __delay_ms(5);
    lcd_write(CMD_DISPLAY_ON_N, true);
    __delay_ms(5);

}

void lcd_putrs(const char *s)
{
   while(*s)
      lcd_write(*s++, false);
}

// OUTPUT CONTROL

/* state1:
 * bit 7: TEMP SEL
 * bit 6: LCD BL OFF
 * bit 5: Ultrasonics ON
 * bit 4: LID LOCK
 * bit 3: DRY LED
 * bit 2: RINSE LED
 * bit 1: WASH LED
 * bit 0: Heater ON
 * 
 * state2:
 * bit 7: RINSE
 * bit 6: WASH
 * bit 5: DRY
 * bit 4: PUMP
 * bit 3: BOILER TRIGGER
 * bit 2: DRAIN
 * bit 1: COLD
 * bit 0: HOT
 */

void setOutputs(uint8_t state1, uint8_t state2)
{
    TRISB = 0x00;
    LATB = state1;
    __delay_us(10);
    LATAbits.LATA1 = 1;
    __delay_us(10);
    LATAbits.LATA1 = 0;
    LATB = state2;
    __delay_us(10);
    LATAbits.LATA2 = 1;
    __delay_us(10);
    LATAbits.LATA2 = 0;
    TRISB = 0xFF;
    __delay_us(10);
}

/* return value:
 * bit 15: N/C
 * bit 14: N/C
 * bit 13: N/C
 * bit 12: N/C
 * bit 11: LID SENSOR
 * bit 10: UNKNOWN (water level?)
 * bit 9: UNKNOWN (water level?)
 * bit 8: UNKNOWN (water level?)
 * bit 7: DIL2
 * bit 6: DIL1
 * bit 5: SW B (EDIT)
 * bit 4: SW A (FUNCTION)
 * bit 3: RESET
 * bit 2: SRT/STP
 * bit 1: ENTER
 * bit 0: SELECT
 */

uint16_t readInputs()
{
    uint8_t in1;
    uint8_t in2;
    
    TRISB = 0xFF;   // make sure data bus is hi-Z
    LATAbits.LATA3 = 0; // Select first bank of inputs
    LATCbits.LATC5 = 0; // Enable readout    
    __delay_us(10);
    in1 = PORTB;        // Read
    LATAbits.LATA3 = 1; // Select second bank of inputs
    __delay_us(10);
    in2 = PORTB;        // Read
    LATCbits.LATC5 = 1; // Disable readout
    __delay_us(10);
    
    return (((uint16_t)in1 << 8) | in2);
}

/*
 * 
 */
int main(int argc, char** argv) {

    GIE = 0;

    /**
    LATx registers
    */
    LATA = 0x0;
    LATB = 0x0;
    LATC = 0x38;
    /**
    ODx registers
    */
    ODCONA = 0x0;
    ODCONB = 0x0;
    ODCONC = 0x0;

    /**
    TRISx registers
    */
    TRISA = 0xE1;
    TRISB = 0xFF;
    TRISC = 0x98;

    /**
    ANSELx registers
    */
    ANSELA = 0xC1;
    ANSELB = 0x00;
    ANSELC = 0x00;

    /**
    WPUx registers
    */
    WPUA = 0x0;
    WPUB = 0xFF;
    WPUC = 0x18;
    WPUE = 0x0;

    /**
    SLRCONx registers
    */
    SLRCONA = 0xFF;
    SLRCONB = 0xFF;
    SLRCONC = 0xFF;

    /**
    INLVLx registers
    */
    INLVLA = 0xFF;
    INLVLB = 0xFF;
    INLVLC = 0xFF;
    INLVLE = 0x8;
    
    setOutputs(0x40,0x00);
    
    OUTPUT_ENABLE = 1;
    

   /**
    RxyI2C | RxyFEAT registers   
    */
    /**
    PPS registers
    */
    RX1PPS = 0x17; //RC7->EUSART1:RX1;
    RC6PPS = 0x09;  //RC6->EUSART1:TX1;
    SSP1CLKPPS = 0x13;  //RC3->MSSP1:SCL1;
    RC3PPS = 0x0F;  //RC3->MSSP1:SCL1;
    SSP1DATPPS = 0x14;  //RC4->MSSP1:SDA1;
    RC4PPS = 0x10;  //RC4->MSSP1:SDA1;
    
    //ABDEN disabled; WUE disabled; BRG16 16bit_generator; SCKP Non-Inverted; 
    BAUD1CON = 0x48; 
    //ADDEN disabled; CREN enabled; SREN disabled; RX9 8-bit; SPEN enabled; 
    RC1STA = 0x90; 
    //TX9D 0x0; BRGH hi_speed; SENDB sync_break_complete; SYNC asynchronous; TXEN enabled; TX9 8-bit; CSRC client; 
    TX1STA = 0x26;
    
    // 57600 baud
    SP1BRGL = 0x48;
    SP1BRGH = 0x0;
    
    NVMCON0bits.NVMERR = 0;
    
    __delay_ms(100);
    
    
    // Check if SELECT is pressed   
    
    if ((readInputs() & 0x1) == 0)
    {
        // no, go to application
        // check for valid code (not FF))
        if (FLASH_Read(PROG_START+1) != 0xFF)
        {
            asm("goto " ___mkstr(PROG_START));
        }
    }
    
    modbus_start();
    lcd_init();
    
    lcd_putrs("BOOTLOADER MODE");
    
    for (;;)
    {
        
        if(PIE3bits.TX1IE == 1 && PIR3bits.TX1IF == 1)
        {
            modbus_tx_isr();
        } 
        if(PIE3bits.RC1IE == 1 && PIR3bits.RC1IF == 1)
        {
            modbus_rx_isr();
        } 
        
        modbus_state_machine();
        
        
        if (checkRstPend())
        {
            if ((PIE3bits.TX1IE == 0) && (TX1STAbits.TRMT == 1))
            {
                // reset
                RESET();
            }
        }
    }
}

/* 
 * File:   main.c
 * Author: mathias
 *
 * Created on den 24 juni 2012, 18:44
 */

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "systick.h"
#include "modbus.h"
#include "globals.h"

//CONFIG1L
#pragma config FEXTOSC = XT     // External Oscillator Mode Selection->XT (crystal oscillator) above 100 kHz, below 8 MHz; PFM set to medium power
#pragma config RSTOSC = HFINTOSC_64MHZ     // Power-Up Default Value for COSC->HFINTOSC with HFFRQ = 64 MHz and CDIV = 1:1

//CONFIG1H
#pragma config CLKOUTEN = OFF     // Clock Out Enable->CLKOUT function is disabled
#pragma config FCMEN = ON     // Fail-Safe Clock Monitor Enable->Fail-Safe Clock Monitor enabled
#pragma config CSWEN = ON     // Clock Switch Enable->Writing to NOSC and NDIV is allowed

//CONFIG2L
#pragma config MCLRE = EXTMCLR     // Master Clear (MCLR) Enable->MCLR pin (RE3) is MCLR
#pragma config BOREN = SBORDIS     // Brown-Out Reset Enable->Brown-out Reset enabled , SBOREN bit is ignored
#pragma config PWRTE = ON     // Power-Up Timer Enable->Power up timer enabled
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





#define _XTAL_FREQ 16777216


#define DELAY_BLINK     0
#define DELAY_BTN       1
#define DELAY_BEEP      2
#define DELAY_TMEAS     3
#define DELAY_TWAIT     4




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
#define LCD_LINE_3         0x14       //Line 3 address
#define LCD_LINE_4         0x54       //Line 4 address

#define LCD_RS      (LATCbits.LATC0)
#define LCD_RW      (LATCbits.LATC1)
#define LCD_E       (LATCbits.LATC2)
#define LCD_DPORT   (LATB)


#define OUTPUT_ENABLE (LATAbits.LATA4)




#define TEMP_INTERNAL   0
#define TEMP_EXTERNAL   1


const uint8_t ntc_ext_table[] =    { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  
                                    0,  0,  0,  0,  0,  0,  0,  2,  3,  4,  6,  7,  8,  9,  10, 11, 
                                    13, 14, 14, 15, 16, 17, 18, 19, 20, 20, 21, 22, 23, 23, 24, 25, 
                                    25, 26, 27, 27, 28, 28, 29, 30, 30, 31, 31, 32, 32, 33, 33, 34, 
                                    34, 35, 35, 36, 36, 37, 37, 37, 38, 38, 39, 39, 40, 40, 41, 41, 
                                    41, 42, 42, 43, 43, 44, 44, 44, 45, 45, 46, 46, 47, 47, 47, 48, 
                                    48, 49, 49, 50, 50, 50, 51, 51, 52, 52, 52, 53, 53, 54, 54, 55, 
                                    55, 55, 56, 56, 57, 57, 57, 58, 58, 59, 59, 59, 60, 60, 61, 61, 
                                    61, 62, 62, 63, 63, 63, 64, 64, 65, 65, 65, 66, 66, 67, 67, 67, 
                                    68, 68, 69, 69, 69, 70, 70, 71, 71, 71, 72, 72, 73, 73, 73, 74, 
                                    74, 75, 75, 75, 76, 76, 77, 77, 78, 78, 78, 79, 79, 80, 80, 80, 
                                    81, 81, 82, 82, 83, 83, 84, 84, 84, 85, 85, 86, 86, 87, 87, 88, 
                                    88, 89, 89, 90, 90, 90, 91, 91, 92, 92, 93, 93, 94, 94, 95, 95, 
                                    96, 96, 97, 97, 98, 98, 98, 99, 99, 99, 99, 99, 99, 99, 99, 99, 
                                    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 
                                    99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99};


const uint8_t char_Aring[8] = {
	0b00100,
	0b01010,
	0b00100,
	0b01110,
	0b10001,
	0b11111,
	0b10001,
	0b10001
};

const uint8_t char_Auml[8] = {
	0b01010,
	0b00000,
	0b00100,
	0b01110,
	0b10001,
	0b11111,
	0b10001,
	0b10001
};

const uint8_t char_Ouml[8] = {
	0b01010,
	0b00000,
	0b01110,
	0b10001,
	0b10001,
	0b10001,
	0b10001,
	0b01110
};


// Empty

const uint8_t char_wls_empty[8] = {
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b11111
};

// WLS Bottom

const uint8_t char_wls_bottom[8] = {
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b10001,
    0b11111,
    0b11111,
    0b11111
};

// WLS Bottom + Top

const uint8_t char_wls_top[8] = {
    0b10001,
    0b10001,
    0b10001,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111
};

// WLS Bottom + Top + Over:

const uint8_t char_wls_over[8] = {
    0b10001,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111
};

const uint8_t char_degreeSign[8] = {
	0b00010,
	0b00101,
	0b00010,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000
};


const uint8_t char_degreeSign1[8] = {
    0b00010,
    0b00101,
    0b00010,
    0b11000,
    0b00100,
    0b01000,
    0b10000,
    0b11100
};

const uint8_t char_degreeSign2[8] = {
    0b00010,
    0b00101,
    0b00010,
    0b01000,
    0b11000,
    0b01000,
    0b01000,
    0b11100
};


//float Setpoint, Input, Output;



typedef enum beep_e
{
    BEEP_NONE,
    BEEP_LOW,
    BEEP_HIGH
} beep_t;

// systick counter vars (1 ms)
//uint8_t systick_tickcnt = 0;
//volatile uint16_t systick_cnt_local = 0;
//volatile uint16_t systick_cnt = 0;

// locks
//volatile __bit systick_cnt_lock = 0;

typedef enum mode_e
{
    SCREEN_NORMAL,
    SCREEN_NORMAL_SHIFT,
    SCREEN_MENU,
    SCREEN_MENU_AUTODRAIN_TIME,
    SCREEN_MENU_AUTODEGAS_TIME  
} screen_t;

uint8_t screen = SCREEN_NORMAL;




//volatile uint16_t aDegasTime = 600;

char lcd_buf1[32];  // larger than needed to avoid crashes in case of too long strings returned by sprintf()
char lcd_buf2[32];  // larger than needed to avoid crashes in case of too long strings returned by sprintf()

//bool auto_degas_enable = false;
//volatile bool degas_enable = false;
//volatile bool air_enable = true;
//volatile bool recirc_enable = true;


//volatile bool drain_enable = false;
//bool auto_drain_enable = false;



__bit wls_led_state = 0;

typedef enum status_page_e
{
    STATUS_PAGE_MAIN,
    STATUS_PAGE_AUTO_DEGAS,   
    STATUS_PAGE_RECIRC,
    STATUS_PAGE_AIR
} status_page_t;

volatile uint8_t curr_status_page = STATUS_PAGE_MAIN;








#define SAVE_TIMEOUT    10  // Time betweeen last change and saving to EEPROM, (1-254 seconds)

bool savePend = false;

uint32_t savePendTimer = 0;


typedef enum water_level_e
{
    WATER_LEVEL_EMPTY,
    WATER_LEVEL_BASE,   
    WATER_LEVEL_TOP,
    WATER_LEVEL_OVER,
    WATER_LEVEL_ERROR
} water_level_t;


uint8_t water_level = WATER_LEVEL_EMPTY;


__bit temp_sel = 0;

__bit debug_tx = 0;

void putch(char dat)
{
    if (!debug_tx)
        return;
    
    // if modbus TX is active, skip
    if (PIE3bits.TX1IE)
        return;
    
    while (!PIR3bits.TX1IF);    // wait until TXREG is ready
    TXREG1 = dat;
}


//uint16_t systickGetTicks()
//{
//    uint16_t ticks_tmp;
//    systick_cnt_lock = 1;
//    ticks_tmp = systick_cnt;
//    systick_cnt_lock = 0;
//    return ticks_tmp;
//}


//__bit systickDelayElapsed(uint8_t delayNum, uint16_t delayTime)
//{
//    if (!delayEnabled[delayNum]) return 0;

//    if ((systickGetTicks() - delayCnt[delayNum]) >= delayTime)
//    {
 //       delayEnabled[delayNum] = 0;
 //       return 1;
 //   }

 //   return 0;
//}

//void systickDelayStart(uint8_t delayNum)
//{
//    delayCnt[delayNum] = systickGetTicks();
//    delayEnabled[delayNum] = 1;
//}



uint8_t EEPROM_ReadByte(uint16_t address)
{
    //Load NVMADR with the desired byte address
    NVMADRU = 0x31;
    NVMADRH = (uint8_t) (address >> 8);
    NVMADRL = (uint8_t) address;

    //Enable NVM access
    NVMCON0bits.NVMEN = 1;

    //Start DFM byte read
    NVMCON1bits.RD = 1;

    //Disable NVM access
    NVMCON0bits.NVMEN = 0;

    return NVMDATL;
}








bool EEPROM_WriteByte(uint16_t address, uint8_t data)
{
    if (address > 1023)
        return false;
    
    //Load NVMADR with the target address of the byte
    NVMADRU = 0x31;
    NVMADRH = (uint8_t) (address >> 8);
    NVMADRL = (uint8_t) address;

    //Load NVMDAT with the desired value
    NVMDATL = data;

    
    //Enable NVM access
    NVMCON0bits.NVMEN = 1;
    
    // Clear error bit
    NVMCON0bits.NVMERR = 0;

    //Disable global interrupt
    INTCONbits.GIE = 0;
    
    //Perform the unlock sequence
    NVMCON2 = 0x55;
    NVMCON2 = 0xAA;
    NVMCON1bits.WR = 1;
    

    //Perform the unlock sequence
    //asm("asmopt push"); //save the current selection of optimizations
    //asm("asmopt off"); //turn off assembler optimizations
    //asm("banksel(NVMCON2)"); //select the bank of the NVMCON2 register
    //asm("MOVLW (0x55)"); //load 'unlockKeyLow' into the W register
    //asm("MOVWF (NVMCON2&0xFF),b"); //move the W register to NVMCON2
    //asm("MOVLW (0xAA)"); //load 'unlockKeyHigh' into the W register
    //asm("MOVWF (NVMCON2&0xFF),b"); //move the W register to NVMCON2

    //Start byte write
    //asm("bsf (_NVMCON1bits&0xFF)," ___mkstr(_NVMCON1_WR_POSN) ",b"); //set WR bit in the NVMCON1 register
    //asm("asmopt pop"); //restore assembler optimization settings

    //Restore global interrupt enable bit value
    INTCONbits.GIE = 1;
    
    while (NVMCON1bits.WR);
    
    //Disable NVM access
    NVMCON0bits.NVMEN = 0;    
    
    if (NVMCON0bits.NVMERR == 1)
    {
        return false;
    }
    else
    {
        return true;
    }
}





uint8_t lastValidBlock = 0;
uint8_t lastSignature = 0;




void eepromClear()
{
    lastValidBlock = 0;
    lastSignature = 0;
    
    for (uint16_t i = 0; i<1024; i++)
    {
        if (!EEPROM_WriteByte(i, 0xFF))
        {
            // failed
            printf("EEPROM write failed, addr=0x%04X, data=0x%02X.\r\n", i, 0xFF);
        }
    }
}



void loadDefaults()
{
    
    memset(&settings, 0, sizeof(settings));
    
    settings.set_temp = 5;
    settings.degas_time = 5;
    settings.set_time = 5;
    settings.autodrain_time = 5;
    settings.degas_pulse_off = 10,
    settings.degas_pulse_on = 5,
    settings.wlp_enable = 1;
    settings.autodrain_enable = 0;    
}


void loadSettings()
{
    lastValidBlock = 0;
    lastSignature = 0;

    
    // 16 bytes per block * 64 blocks = 1024 bytes
    for (uint8_t i = 0; i<64; i++)
    {
        uint8_t rdByte = EEPROM_ReadByte(i*16);
        if (rdByte == 0xFF)
        {
            // empty block -> most recent data was the last block read
            break;
        }
        else if (rdByte == 0x55)
        {
            // Used block
            
            if (lastSignature == 0xAA)
            {
                // signature changed -> most recent data was the last block read
                break;
            }
            
            lastValidBlock = i;
            lastSignature = 0x55;
        }
        else if (rdByte == 0xAA)
        {
            // used block
            
            if (lastSignature == 0x55)
            {
                // signature changed -> most recent data was the last block read
                break;
            }
            
            lastValidBlock = i;
            lastSignature = 0xAA;
        }
    }
    
    // if no signature change was detected, then the most recent data must be the last block
        
    if (lastSignature == 0x00)
    {
        // data not found, load defaults
        printf("No valid EEPROM block found, loading defaults!\r\n");
        
        loadDefaults();     
    }
    else
    {
        // data found, load settings
        printf("Most recent EEPROM data is at block %u.\r\n", lastValidBlock);
        
        for (uint8_t i = 0; i<15; i++)
        {
            ((uint8_t*)&settings)[i] = EEPROM_ReadByte(lastValidBlock * 16 + i + 1);
        }
        
        
        if (settings.set_temp > 95)
            settings.set_temp = 95;
        else if (settings.set_temp < 5)
            settings.set_temp = 5;
        
        if (settings.degas_time > 95)
            settings.degas_time = 95;
        else if (settings.degas_time < 5)
            settings.degas_time = 5;
        
        if (settings.set_time > 599)
            settings.set_time = 599;
        
        if (settings.autodrain_time > 95)
            settings.autodrain_time = 95;        
        else if (settings.autodrain_time < 5)
            settings.autodrain_time = 5;
        
        if (settings.degas_pulse_off > 99)
            settings.degas_pulse_off = 99;        
        else if (settings.degas_pulse_off < 1)
            settings.degas_pulse_off = 1;
        
        if (settings.degas_pulse_on > 99)
            settings.degas_pulse_on = 99;        
        else if (settings.degas_pulse_on < 1)
            settings.degas_pulse_on = 1;
        
        
        
    }
    
    
    
}






void saveSettings()
{
    uint8_t blockToWrite = lastValidBlock + 1;
    
    
     if (lastSignature == 0x00)
     {
        blockToWrite = 0;
        lastSignature = 0x55;
     }
    
    
    if (blockToWrite == 64)
    {
        // wrap-around, flip signature
        if (lastSignature == 0x55)
            lastSignature = 0xAA;
        else
            lastSignature = 0x55;
        
        blockToWrite = 0;
    }
    
    printf("Saving settings to EEPROM block %u.\r\n", blockToWrite);
    
    if (!EEPROM_WriteByte(blockToWrite * 16, lastSignature))
    {
        // failed
        printf("EEPROM write failed, addr=0x%04X, data=0x%02X.\r\n", blockToWrite * 16, lastSignature);
        return;
    }
    
    printf("EEPROM write OK, addr=0x%04X, data=0x%02X.\r\n", blockToWrite * 16, lastSignature);
    
    
    for (uint8_t i = 0; i<15; i++)
    {
        if (!EEPROM_WriteByte(blockToWrite * 16 + i + 1, ((uint8_t*)&settings)[i]))
        {
            // failed
            printf("EEPROM write failed, addr=0x%04X, data=0x%02X.\r\n", blockToWrite * 16 + i + 1, ((uint8_t*)&settings)[i]);
            return;
        }
        
        printf("EEPROM write OK, addr=0x%04X, data=0x%02X.\r\n", blockToWrite * 16 + i + 1, ((uint8_t*)&settings)[i]);
        
    }
    
    lastValidBlock = blockToWrite;
    
    printf("Save OK!\r\n");
    
}







void lcd_strobe()
{
    LCD_E = 1;
    __delay_us(1);
    LCD_E = 0;
    __delay_us(1);
}

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
    lcd_strobe();
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



void lcd_set_ddram_addr(char address)
{
    lcd_write(CMD_SET_DDRAM_ADDR | address, true);
}


void lcd_puts(char *s)
{
   while(*s)
      lcd_write(*s++, false);
}

void lcd_customchar(uint8_t pos, const uint8_t *custom_char)
{
    
    pos &= 0b111;
    
    lcd_write(CMD_SET_CGRAM_ADDR | (uint8_t)((pos & 0b111) << 3), true);
    
    for (uint8_t i=0; i<8; i++)
    {
        lcd_write(custom_char[i], false);
    }
    
    lcd_set_ddram_addr(LCD_LINE_1);
}


#define OUTPUT1_TEMPSEL_BIT         7
#define OUTPUT1_BL_BIT              6
#define OUTPUT1_ULTRASONICS_BIT     5
#define OUTPUT1_LIDLOCK_BIT         4
#define OUTPUT1_DRYLED_BIT          3
#define OUTPUT1_RINSELED_BIT        2
#define OUTPUT1_WASHLED_BIT         1
#define OUTPUT1_HEATER_ON_BIT       0



// OUTPUT CONTROL

/*
 * bit 7: TEMP SEL
 * bit 6: LCD BL OFF
 * bit 5: Ultrasonics ON
 * bit 4: Lid lock (?)
 * bit 3: DRY LED
 * bit 2: RINSE LED
 * bit 1: WASH LED
 * bit 0: Heater ON
 */


void setOutputs1(uint8_t state, uint8_t mask)
{
    uint8_t newState = out1State & (~mask);  // keep bits with 0 in mask
    
    newState |= state & mask;   // change bits with 1 in mask
    
    out1State = newState;
    
    TRISB = 0x00;
    LATB = newState;
    __delay_us(10);
    LATAbits.LATA1 = 1;
    __delay_us(10);
    LATAbits.LATA1 = 0;
    TRISB = 0xFF;
    __delay_us(10);
}

#define OUTPUT2_RINSE_BIT   7
#define OUTPUT2_WASH_BIT    6
#define OUTPUT2_DRY_BIT     5
#define OUTPUT2_PUMP_BIT    4
#define OUTPUT2_BOILER_BIT  3
#define OUTPUT2_DRAIN_BIT   2
#define OUTPUT2_COLD_BIT    1
#define OUTPUT2_HOT_BIT     0



/* 
 * bit 7: RINSE (not required)
 * bit 6: WASH (not required)
 * bit 5: DRY (not required)
 * bit 4: PUMP
 * bit 3: BOILER TRIGGER (not required)
 * bit 2: DRAIN
 * bit 1: COLD (not required)
 * bit 0: HOT (not required)
 */

void setOutputs2(uint8_t state, uint8_t mask)
{
    uint8_t newState = out2State & (~mask);  // keep bits with 0 in mask
    
    newState |= state & mask;   // change bits with 1 in mask
    
    out2State = newState;
    
    TRISB = 0x00;
    LATB = newState;
    __delay_us(10);
    LATAbits.LATA2 = 1;
    __delay_us(10);
    LATAbits.LATA2 = 0;
    TRISB = 0xFF;
    __delay_us(10);
}


#define INPUT_LID_SENSOR_BIT    11
#define INPUT_WLVL_OVER_BIT    10
#define INPUT_WLVL_BASE_BIT    9
#define INPUT_WLVL_TOP_BIT    8
#define INPUT_DIL2_BIT    7
#define INPUT_DIL1_BIT    6
#define INPUT_BTN_EDIT_BIT    5
#define INPUT_BTN_FUNC_BIT    4
#define INPUT_BTN_RESET_BIT    3
#define INPUT_BTN_START_BIT    2
#define INPUT_BTN_ENTER_BIT   1
#define INPUT_BTN_SELECT_BIT    0

/* return value:
 * bit 15: N/C
 * bit 14: N/C
 * bit 13: N/C
 * bit 12: N/C
 * bit 11: LID SENSOR
 * bit 10: Water level OVER (0=dry, 1=wet)
 * bit 9:  Water level BASE (0=dry, 1=wet)
 * bit 8:  Water level TOP (0=dry, 1=wet)
 * bit 7:  DIL2 (not populated)
 * bit 6:  DIL1 (not populated)
 * bit 5:  SW B (EDIT)
 * bit 4:  SW A (FUNCTION)
 * bit 3:  RESET
 * bit 2:  SRT/STP
 * bit 1:  ENTER
 * bit 0:  SELECT
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

bool lockoutCheck(void)
{
    // Check water level
    if ((settings.wlp_enable) && (inputState & (1 << INPUT_WLVL_BASE_BIT)))
    {
        return false;   // no water
    }
    
    return true;
}

void setHeater(bool enabled)
{
    if ((heater_enabled != enabled) && (run_mode != RUN_MODE_DRAIN))
    {
        if (enabled && lockoutCheck())
        {
            // enable heat
            setOutputs1(1 << OUTPUT1_RINSELED_BIT, 1 << OUTPUT1_RINSELED_BIT);
            heater_enabled = 1;
            printf("Heater enabled.\r\n");
        }
        else
        {
            // disable heat
            setOutputs1(0, (1 << OUTPUT1_RINSELED_BIT) | (1 << OUTPUT1_HEATER_ON_BIT));
            heat_on = 0;
            heater_enabled = 0;
            printf("Heater disabled.\r\n");
        }
    }
}


void setRecirc(bool enabled)
{
    if (recirc_enabled != enabled)
    {
        if (enabled && lockoutCheck())
        {
            // enable recirc
            setOutputs2(1 << OUTPUT2_PUMP_BIT, 1 << OUTPUT2_PUMP_BIT);
            recirc_enabled = 1;
            printf("Recirc enabled.\r\n");
        }
        else
        {
            // disable recirc
            setOutputs2(0, (1 << OUTPUT2_PUMP_BIT));
            recirc_enabled = 0;
            printf("Heater disabled.\r\n");
        }
    }
}


void setAir(bool enabled)
{
    if (air_enabled != enabled)
    {
        if (enabled)
        {
            // enable air
            setOutputs2(1 << OUTPUT2_DRY_BIT, 1 << OUTPUT2_DRY_BIT);
            air_enabled = 1;
            printf("Air enabled.\r\n");
        }
        else
        {
            // disable air
            setOutputs2(0, (1 << OUTPUT2_DRY_BIT));
            air_enabled = 0;
            printf("Air enabled.\r\n");
        }
    }
}

void setMode(uint8_t new_mode)
{
    if (new_mode != run_mode)
    {
        // mode changed
        
        if (new_mode == RUN_MODE_IDLE)
        {
            // switch to IDLE mode
            ultrasonic_on = 0;
            setHeater(0);
            setRecirc(0);
            setAir(0);
            setOutputs1(0, 0x37);
            setOutputs2(0, 0xFF);
            run_mode = RUN_MODE_IDLE;
            printf("IDLE mode.\r\n");
        }
        else if ((new_mode == RUN_MODE_NORMAL) && (run_mode == RUN_MODE_IDLE))
        {
            // Start normal wash cycle
            if (lockoutCheck())
            {
                // safe to proceed
                elapsedTime = 0;
                ultrasonic_on = 1;
                setOutputs1(0x22, 0x22);
                run_mode = RUN_MODE_NORMAL;
                printf("Normal cycle started.\r\n");
            }
        }  
        else if ((new_mode == RUN_MODE_DEGAS) && (run_mode == RUN_MODE_IDLE))
        {
            // Start degas cycle
            if (lockoutCheck())
            {
                // safe to proceed
                elapsedTime = 0;
                ultrasonic_on = 1;
                setOutputs1(0x22, 0x22);
                degas_pulse_timer = settings.degas_pulse_on;
                run_mode = RUN_MODE_DEGAS;
                printf("Degas cycle started.\r\n");
            }
        }
        else if ((new_mode == RUN_MODE_DRAIN) && (run_mode == RUN_MODE_IDLE))
        {
            if (!heater_enabled)
            {
                // Start drain cycle
                run_mode = RUN_MODE_DRAIN;
                autodrain_timer = settings.autodrain_time;
                setOutputs2(1 << OUTPUT2_DRAIN_BIT, 1 << OUTPUT2_DRAIN_BIT);
                printf("Drain cycle started.\r\n");
            }
        }
        
        
    }
}







void resetSaveTimer(void)
{
    //save_timer = SAVE_TIMEOUT + 1;
    
    savePendTimer = SysTick_GetTicks();
    savePend = true;
    
}

const char str_ready[] = "READY";
const char str_fill[] =  "FILL ";
const char str_degas[] = "DEGAS";
const char str_run[] =   "RUN  ";
const char str_drain[] = "DRAIN";

void updateLCD_normal()
{
    
    //static uint32_t blinkTimer = 0;
    //static __bit blinkState = 0;
    
    uint8_t hours, minutes;
    uint8_t hoursE, minutesE;    
    
    hours = ((run_mode == RUN_MODE_DEGAS)?settings.degas_time:settings.set_time) / 60;
    minutes = ((run_mode == RUN_MODE_DEGAS)?settings.degas_time:settings.set_time) % 60;
    
    if (hours > 9)
        hours = 9;
    
    if (minutes > 99)
        minutes = 99;
        
    hoursE = elapsedTime / 3600;
    minutesE = (elapsedTime % 3600) / 60;
    
    if (hoursE > 9)
        hoursE = 9;
    
    if (minutesE > 99)
        minutesE = 99;
    
    const char *pModeStr;
    
    switch (run_mode)
    {
        case RUN_MODE_IDLE:
            if ((settings.wlp_enable) && (inputState & (1 << INPUT_WLVL_BASE_BIT)))
                pModeStr = str_fill;
            else
                pModeStr = str_ready;
            break;
        case RUN_MODE_NORMAL:
            pModeStr = str_run;
            break;
        case RUN_MODE_DEGAS:
            pModeStr = str_degas;
            break;
        case RUN_MODE_DRAIN:
            pModeStr = str_drain;
            break;
    }
    
    
    
    if ((run_mode == RUN_MODE_DRAIN) && (settings.autodrain_enable))
    {
        
        minutes = autodrain_timer / 60;
        hours = autodrain_timer % 60;       // seconds
        
        if (minutes > 9)
        {
            minutes = 9;
        }
        
        sprintf(lcd_buf1, "AUTO DRAIN  %u:%02u", minutes, hours);
    }
    else
    {
        sprintf(lcd_buf1, "%2u\014C %s  %u:%02u", settings.set_temp, pModeStr, hours, minutes);  
    }

    
    sprintf(lcd_buf2, "%2u\014C %2u\014C   %u:%02u", temperatures[TEMP_INTERNAL], temperatures[TEMP_EXTERNAL], hoursE, minutesE);

    
    if (water_level == WATER_LEVEL_OVER)
    {
        // water level OVER
        lcd_buf2[10] = '\013';
    }
    else if (water_level == WATER_LEVEL_TOP)
    {
        // water level TOP
        lcd_buf2[10] = '\012';
    }
    else if (water_level == WATER_LEVEL_BASE)
    {
        // water level BASE
        lcd_buf2[10] = '\011';
    }
    else if (water_level == WATER_LEVEL_EMPTY)
    {
        // water level EMPTY
        lcd_buf2[10] = '\010';
    }
    else
    {
        // water level ERROR
        lcd_buf2[10] = 'E';
    }
    
    
    if ((run_mode == RUN_MODE_NORMAL) || (run_mode == RUN_MODE_DEGAS))
    {
        if (elapsedTime & 0x1)
        {
            // colon off
            lcd_buf2[13] = ' ';
        }
        
        
        
        
        
        //if (SysTick_CheckElapsed(blinkTimer, 512))
        //{
        //    blinkState = !blinkState;
         //   blinkTimer = SysTick_GetTicks();
        //}
    }
    //else
    //{
    //    blinkState = 0;
    //    blinkTimer = SysTick_GetTicks();
    //}
    
    //if (blinkState)
    //{
        // colon off
    //    lcd_buf2[13] = ' ';
    //}
    
     
}


void updateLCD_shifted()
{
    uint8_t hours, minutes;
    
    //udiv_t d;
    
    
    //d = udiv(setTime + 59, 3600);
    //hours = d.quot + (d.rem ? 1 : 0);
    //hours = d.quot;
    //minutes = d.rem / 60;
    
    //hours = (settings.set_time + 59) / 3600;
    //minutes = ((settings.set_time + 59) % 3600) / 60;
    
    
    hours = ((run_mode == RUN_MODE_DEGAS)?settings.degas_time:settings.set_time) / 60;
    minutes = ((run_mode == RUN_MODE_DEGAS)?settings.degas_time:settings.set_time) % 60;
    
    if (hours > 9)
        hours = 9;
    
    if (minutes > 99)
        minutes = 99;
    
    sprintf(lcd_buf1, "%u:%02u         AIR", hours, minutes);
    
    
    //d = udiv(remainTime + 59, 3600);
    //hours = d.quot + (d.rem ? 1 : 0);
    //hours = d.quot;
    //minutes = d.rem / 60;
    
    //hours = (elapsedTime + 59) / 3600;
    //minutes = ((elapsedTime + 59) % 3600) / 60;
    
    hours = elapsedTime / 3600;
    minutes = (elapsedTime % 3600) / 60;
    
    if (hours > 9)
        hours = 9;
    
    if (minutes > 99)
        minutes = 99;
    
    sprintf(lcd_buf2, "%u:%02u  RECIRC    ", hours, minutes);
  
    if (air_enabled)
    {
        lcd_buf1[12] = '>';
    }
    
    if (recirc_enabled)
    {
        lcd_buf2[5] = '>';
    }
}



void updateLCD_menu()
{
    sprintf(lcd_buf1, "%s   W-LVL PROT", settings.wlp_enable?"ON ":"OFF");
    sprintf(lcd_buf2, "%s   AUTO DRAIN", settings.autodrain_enable?"ON ":"OFF");
} 

void updateLCD_menu_autodrain()
{
    sprintf(lcd_buf1, "AUTO DRAIN TIME ");
    sprintf(lcd_buf2, "%2u SEC      BACK", settings.autodrain_time);
} 

void updateLCD_menu_autodegas()
{
    sprintf(lcd_buf1, "---AUTO DEGAS---");
    sprintf(lcd_buf2, "%2u MIN    CANCEL", settings.degas_time);
} 



void handleButtons()
{
    static uint32_t btnTimer = 0;
    static uint32_t repeatTimer = 0;
    static uint8_t lastBtnState = 0;
    static uint16_t repeatInterval = 0;    
    static bool longPressTriggered = 0;
    uint8_t btnState;
    uint8_t numButtons = 0;

    
    btnState = inputState & 0xFF;
    
    if(btnState & (1 << INPUT_BTN_ENTER_BIT))
        numButtons++;
    if(btnState & (1 << INPUT_BTN_FUNC_BIT))
        numButtons++;
    if(btnState & (1 << INPUT_BTN_RESET_BIT))
        numButtons++;
    if(btnState & (1 << INPUT_BTN_SELECT_BIT))
        numButtons++;
    if(btnState & (1 << INPUT_BTN_START_BIT))
        numButtons++;
    if(btnState & (1 << INPUT_BTN_EDIT_BIT))
        numButtons++;

    
    if ((numButtons != 1) || ((lastBtnState != 0) && (lastBtnState != btnState)))
    {
        // no button pressed, or more than one pressed, or different button pressed than last time, reset states
        
        if ((btnState == 0) && (lastBtnState != 0))
        {
            // button released
            printf("Button released: %02X\r\n", lastBtnState);
            
            
            if (lastBtnState & (1 << INPUT_BTN_START_BIT))
            {
                if ((screen == SCREEN_NORMAL) && (!longPressTriggered))
                {
                    // Ultrasonic on
                    if (run_mode == RUN_MODE_IDLE)
                        setMode(RUN_MODE_NORMAL);
                }          
            }
            

            
        }
        
        
        
        btnTimer = SysTick_GetTicks();  // reset button timer
        repeatTimer = btnTimer;         // reset repeat timer
        repeatInterval = 350;           // initial delay for key repeat
        lastBtnState = 0;
        longPressTriggered = 0;
        
    }
    else if (lastBtnState == 0)
    {
        // numButtons == 1, lastBtnState != btnState, lastBtnState == 0
        // state transition no button pressed -> button pressed

        if (SysTick_CheckElapsed(btnTimer, 50))
        {
            // debounce timer elapsed, register as pressed
            
            
            printf("Button pressed: %02X\r\n", btnState);
            
            
            if (btnState & (1 << INPUT_BTN_SELECT_BIT))
            {
                // Temp+ or time+
                
                if (screen == SCREEN_NORMAL)
                {
                    // Temp+
                    if (settings.set_temp <= 90)
                    {
                        settings.set_temp += 5;
                        resetSaveTimer();
                    }
                    else if (settings.set_temp != 95)
                    {
                        settings.set_temp = 95;
                        resetSaveTimer();
                    }
                }
                else if (screen == SCREEN_NORMAL_SHIFT)
                {
                    if (run_mode == RUN_MODE_DEGAS)
                    {
                        // Auto degas time+
                        if (settings.degas_time <= 90)
                        {
                            settings.degas_time += 5;
                            resetSaveTimer();
                        }
                        else if (settings.degas_time != 95)
                        {
                            settings.degas_time = 95;
                            resetSaveTimer();
                        }
                    }
                    else
                    {
                        // Time+
                        if (settings.set_time < 599)
                        {
                            settings.set_time++;
                            resetSaveTimer();
                        }
                        else if (settings.set_time != 599)
                        {
                            settings.set_time = 599;
                            resetSaveTimer();
                        }
                    }
                }
                else if (screen == SCREEN_MENU)
                {
                    // WL protect
                    settings.wlp_enable = !settings.wlp_enable;
                    resetSaveTimer();
                    
                    // Auto degas toggle
                   // settings.autodegas_enable = !settings.autodegas_enable;
                    //resetSaveTimer();
                }
                else if (screen == SCREEN_MENU_AUTODEGAS_TIME)
                {
                    // Auto degas time+
                    if (settings.degas_time <= 90)
                    {
                        settings.degas_time += 5;
                        resetSaveTimer();
                    }
                    else if (settings.degas_time != 95)
                    {
                        settings.degas_time = 95;
                        resetSaveTimer();
                    }
                }
                else if (screen == SCREEN_MENU_AUTODRAIN_TIME)
                {
                    // Auto drain time+
                    if (settings.autodrain_time <= 90)
                    {
                        settings.autodrain_time += 5;
                        resetSaveTimer();
                    }
                    else if (settings.autodrain_time != 95)
                    {
                        settings.autodrain_time = 95;
                        resetSaveTimer();
                    }
                }
                
            }
            
            
            if (btnState & (1 << INPUT_BTN_FUNC_BIT))
            {
                // Temp- or time-
                
                if (screen == SCREEN_NORMAL)
                {
                    // Temp-
                    if (settings.set_temp >= 10)
                    {
                        settings.set_temp -= 5;
                        resetSaveTimer();
                    }
                    else if (settings.set_temp != 5)
                    {
                        settings.set_temp = 5;
                        resetSaveTimer();
                    }
                }
                else if (screen == SCREEN_NORMAL_SHIFT)
                {
                    if (run_mode == RUN_MODE_DEGAS)
                    {
                        // Auto degas time-
                        if (settings.degas_time >= 10)
                        {
                            settings.degas_time -= 5;
                            resetSaveTimer();
                        }
                        else if (settings.degas_time != 5)
                        {
                            settings.degas_time = 5;
                            resetSaveTimer();
                        }
                    }
                    else
                    {
                        // Time-
                        if (settings.set_time > 0)
                        {
                            settings.set_time--;
                            resetSaveTimer();
                        }
                    }
                    

                }
                else if (screen == SCREEN_MENU)
                {
                    // Auto drain toggle
                    settings.autodrain_enable = !settings.autodrain_enable;
                    resetSaveTimer();
                }
                else if (screen == SCREEN_MENU_AUTODEGAS_TIME)
                {
                    // Auto degas time-
                    if (settings.degas_time >= 10)
                    {
                        settings.degas_time -= 5;
                        resetSaveTimer();
                    }
                    else if (settings.degas_time != 5)
                    {
                        settings.degas_time = 5;
                        resetSaveTimer();
                    }
                }
                else if (screen == SCREEN_MENU_AUTODRAIN_TIME)
                {
                    // Auto drain time-
                    if (settings.autodrain_time >= 10)
                    {
                        settings.autodrain_time -= 5;
                        resetSaveTimer();
                    }
                    else if (settings.autodrain_time != 5)
                    {
                        settings.autodrain_time = 5;
                        resetSaveTimer();
                    }
                }
                
            }
            
            
            if (btnState & (1 << INPUT_BTN_EDIT_BIT))
            {
                // Shift/select
                if (screen == SCREEN_NORMAL)
                    screen = SCREEN_NORMAL_SHIFT;
                else if (screen == SCREEN_NORMAL_SHIFT)
                    screen = SCREEN_NORMAL;
                else if (screen == SCREEN_MENU)
                    screen = SCREEN_NORMAL; // Exit menu
                else if (screen == SCREEN_MENU_AUTODRAIN_TIME)
                    screen = SCREEN_MENU;
                else if (screen == SCREEN_MENU_AUTODEGAS_TIME)
                    screen = SCREEN_NORMAL;
            }
            
            if (btnState & (1 << INPUT_BTN_START_BIT))
            {
                if (screen == SCREEN_NORMAL)
                {
                    // Ultrasonic off
                    if ((run_mode == RUN_MODE_NORMAL) || (run_mode == RUN_MODE_DEGAS))
                    {
                        setMode(RUN_MODE_IDLE);
                        longPressTriggered = 1; // make sure long press and button release events are ignored

                    
                    }
                }
                else if (screen == SCREEN_NORMAL_SHIFT)
                {
                    // Degas toggle
                    //if (degas_on)
                    //{
                    //    degas_on = 0;
                    //}
                    //else
                    //{
                    //    degas_on = 1;
                    //    if (settings.autodegas_enable)
                    //        autodegas_timer = (uint16_t)settings.autodegas_time * 60;
                    //}
                }
                else if (screen == SCREEN_MENU)
                {
                    // Degas time
                    //screen = SCREEN_MENU_AUTODEGAS_TIME;
                }
                else if (screen == SCREEN_MENU_AUTODEGAS_TIME)
                {
                    // Start auto degas
                    setMode(RUN_MODE_DEGAS);
                    
                    screen = SCREEN_NORMAL;
                    longPressTriggered = 1; // make sure long press and button release events are ignored
                }
            }
            
            if (btnState & (1 << INPUT_BTN_ENTER_BIT))
            {
                if (screen == SCREEN_NORMAL)
                {
                    // Drain toggle
                    if (run_mode == RUN_MODE_DRAIN)
                    {
                        setMode(RUN_MODE_IDLE);
                    }
                    else if (run_mode == RUN_MODE_IDLE)
                    {
                        setMode(RUN_MODE_DRAIN);
                    }
                    
                    
                }
                else if (screen == SCREEN_NORMAL_SHIFT)
                {
                    // Recirc toggle
                    setRecirc(!recirc_enabled);
                }
                else if (screen == SCREEN_MENU)
                {
                    // Drain time
                    screen = SCREEN_MENU_AUTODRAIN_TIME;
                }
            }
            
            if (btnState & (1 << INPUT_BTN_RESET_BIT))
            {
                if (screen == SCREEN_NORMAL)
                {
                    // Heat toggle
                    setHeater(!heater_enabled);
                }
                else if (screen == SCREEN_NORMAL_SHIFT)
                {
                    // Air toggle
                    setAir(!air_enabled);
                }
                else if (screen == SCREEN_MENU)
                {
                    // WL protect
                    //settings.wlp_enable = !settings.wlp_enable;
                    //resetSaveTimer();
                }
            }
            
            
            lastBtnState = btnState;
            
        }
    }
    else
    {
        // numButtons == 1, lastBtnState != 0, lastBtnState == btnState
        // button pressed -> same button still pressed
        if ((SysTick_CheckElapsed(btnTimer, 1024)) && (!longPressTriggered))
        {
            
            printf("Button long press: %02X\r\n", btnState);
            
            // button hold action
            if (btnState & (1 << INPUT_BTN_EDIT_BIT))
            {
                // Shift/select
                
                //shiftFuncActive = !shiftFuncActive;
                
                // long press -> enter menu
                if ((screen == SCREEN_NORMAL) || (screen == SCREEN_NORMAL_SHIFT))
                    screen = SCREEN_MENU;
                
            }
            
            
            if (btnState & (1 << INPUT_BTN_START_BIT))
            {
                if ((screen == SCREEN_NORMAL) && (run_mode == RUN_MODE_IDLE))
                {
                    // Degas time
                    screen = SCREEN_MENU_AUTODEGAS_TIME;
                }
            }
            
            
            longPressTriggered = 1;
            
        }
        
        if (SysTick_CheckElapsed(repeatTimer, repeatInterval))
        {
            // button key repeat trigger
            
            
            printf("Button repeat: %02X\r\n", btnState);
            
            
            
            
            if (btnState & (1 << INPUT_BTN_SELECT_BIT))
            {
                // Temp+ or time+
                
                if (screen == SCREEN_NORMAL)
                {
                    // Temp+
                    if (settings.set_temp <= 90)
                    {
                        settings.set_temp += 5;
                        resetSaveTimer();
                    }
                    else if (settings.set_temp != 95)
                    {
                        settings.set_temp = 95;
                        resetSaveTimer();
                    }
                }
                else if (screen == SCREEN_NORMAL_SHIFT)
                {
                    if (run_mode == RUN_MODE_DEGAS)
                    {
                        // Auto degas time+
                        if (settings.degas_time <= 90)
                        {
                            settings.degas_time += 5;
                            resetSaveTimer();
                        }
                        else if (settings.degas_time != 95)
                        {
                            settings.degas_time = 95;
                            resetSaveTimer();
                        }
                    }
                    else
                    {
                        // Time+
                        if (settings.set_time <= 589)
                        {
                            settings.set_time += 10;
                            resetSaveTimer();
                        }
                        else if (settings.set_time != 599)
                        {
                            settings.set_time = 599;
                            resetSaveTimer();
                        }
                    }
                }
                else if (screen == SCREEN_MENU_AUTODEGAS_TIME)
                {
                    // Auto degas time+
                    if (settings.degas_time <= 90)
                    {
                        settings.degas_time += 5;
                        resetSaveTimer();
                    }
                    else if (settings.degas_time != 95)
                    {
                        settings.degas_time = 95;
                        resetSaveTimer();
                    }
                }
                else if (screen == SCREEN_MENU_AUTODRAIN_TIME)
                {
                    // Auto drain time+
                    if (settings.autodrain_time <= 90)
                    {
                        settings.autodrain_time += 5;
                        resetSaveTimer();
                    }
                    else if (settings.autodrain_time != 95)
                    {
                        settings.autodrain_time = 95;
                        resetSaveTimer();
                    }
                }
                
            }
            
            
            if (btnState & (1 << INPUT_BTN_FUNC_BIT))
            {
                // Temp- or time-
                
                if (screen == SCREEN_NORMAL)
                {
                    // Temp-
                    if (settings.set_temp >= 10)
                    {
                        settings.set_temp -= 5;
                        resetSaveTimer();
                    }
                    else if (settings.set_temp != 5)
                    {
                        settings.set_temp = 5;
                        resetSaveTimer();
                    }
                }
                else if (screen == SCREEN_NORMAL_SHIFT)
                {
                    if (run_mode == RUN_MODE_DEGAS)
                    {
                        // Auto degas time-
                        if (settings.degas_time >= 10)
                        {
                            settings.degas_time -= 5;
                            resetSaveTimer();
                        }
                        else if (settings.degas_time != 5)
                        {
                            settings.degas_time = 5;
                            resetSaveTimer();
                        }
                    }
                    else
                    {
                        // Time-
                        if (settings.set_time >= 10)
                        {
                            settings.set_time -= 10;
                            resetSaveTimer();
                        }
                        else if (settings.set_time != 0)
                        {
                            settings.set_time = 0;
                            resetSaveTimer();
                        }
                    }
                }
                else if (screen == SCREEN_MENU_AUTODEGAS_TIME)
                {
                    // Auto degas time-
                    if (settings.degas_time >= 10)
                    {
                        settings.degas_time -= 5;
                        resetSaveTimer();
                    }
                    else if (settings.degas_time != 5)
                    {
                        settings.degas_time = 5;
                        resetSaveTimer();
                    }
                }
                else if (screen == SCREEN_MENU_AUTODRAIN_TIME)
                {
                    // Auto drain time-
                    if (settings.autodrain_time >= 10)
                    {
                        settings.autodrain_time -= 5;
                        resetSaveTimer();
                    }
                    else if (settings.autodrain_time != 5)
                    {
                        settings.autodrain_time = 5;
                        resetSaveTimer();
                    }
                }
                
            }
            
            repeatTimer = SysTick_GetTicks();         // reset repeat timer
            repeatInterval = 150;           // key repeat rate
        }
        
        
    }
    
    
  
    
}





uint32_t everySecondTimer = 0;
uint32_t adcTimer = 0;

/*
 * 
 */
void main(void)
{
    


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

   /**
    IOCx registers 
    */
    IOCAP = 0x0;
    IOCAN = 0x0;
    IOCAF = 0x0;
    IOCBP = 0x0;
    IOCBN = 0x0;
    IOCBF = 0x0;
    IOCCP = 0x0;
    IOCCN = 0x0;
    IOCCF = 0x0;
    IOCEP = 0x0;
    IOCEN = 0x0;
    IOCEF = 0x0;

    
    
    
    
    setOutputs1(0, 0xFF);
    setOutputs2(0, 0xFF);
    
    OUTPUT_ENABLE = 1;
    
    IPEN = 1;
    
    //ABDEN disabled; WUE disabled; BRG16 16bit_generator; SCKP Non-Inverted; 
    BAUD1CON = 0x48; 
    //ADDEN disabled; CREN enabled; SREN disabled; RX9 8-bit; SPEN enabled; 
    RC1STA = 0x90; 
    //TX9D 0x0; BRGH hi_speed; SENDB sync_break_complete; SYNC asynchronous; TXEN enabled; TX9 8-bit; CSRC client; 
    TX1STA = 0x26; 
    //SPBRGL 130; 
    //SP1BRGL = 0x82; 
    //SPBRGH 6; 
   // SP1BRGH = 0x6; 
    
    
    // 57600 baud
    //SPBRGL 72; 
    SP1BRGL = 0x48; 
    //SPBRGH 0; 
    SP1BRGH = 0x0;
    
    
    TX1STAbits.TXEN = 1; // enable transmitter    
    RC1STAbits.CREN = 1; // enable receiver
    
    IPR3bits.RC1IP = 0; // low priority
    IPR3bits.TX1IP = 0; // low priority
    
    PIE3bits.RC1IE = 1;  // enable RX interrupt
    
    
    // init ADC
    
    ADCON0bits.ADON = 0;
    PIE1bits.ADIE = 0;
    PIE1bits.ADTIE = 0;
    PIR1bits.ADIF = 0;
    PIR1bits.ADTIF = 0;
    
    ADRPT = 64; // Repeat 64 times
    ADPCH = 0;  // RA0
    ADACQ = 5;  // Acquisition time = 10 us
    ADCAP = 0;  // No additional capacitance
    ADPRE = 0;  // No pre-charge
        
    ADCON1 = (0 << _ADCON1_ADDSEN_POSITION)     /* ADDSEN disabled(0) */
			|(0 << _ADCON1_ADGPOL_POSITION)     /* ADGPOL digital_low(0) */
			|(0 << _ADCON1_ADIPEN_POSITION)     /* ADIPEN disabled(0) */
			|(0 << _ADCON1_ADPPOL_POSITION);    /* ADPPOL VSS(0) */
    ADCON2 = (6 << _ADCON2_ADCRS_POSITION)      /* ADCRS 0x6(6) */
			|(3 << _ADCON2_ADMD_POSITION)       /* ADMD Burst_average_mode(3) */
			|(1 << _ADCON2_ADACLR_POSITION)     /* ADACLR enabled(1) */
			|(0 << _ADCON2_ADPSIS_POSITION);	/* ADPSIS ADRES(0) */
    ADCON3 = (0 << _ADCON3_ADCALC_POSITION)     /* ADCALC First derivative of single measurement(0) */
			|(7 << _ADCON3_ADTMD_POSITION)      /* ADTMD Interrupt regardless of threshold test results(7) */
			|(0 << _ADCON3_ADSOI_POSITION);     /* ADSOI ADGO not cleared(0) */
    ADSTAT = (0 << _ADSTAT_ADAOV_POSITION);     /* ADAOV ACC or ADERR not Overflowed(0) */
    ADREF = (0 << _ADREF_ADNREF_POSITION)       /* ADNREF VSS(0) */
			|(0 << _ADREF_ADPREF_POSITION);     /* ADPREF VDD(0) */
    ADCLK = (8 << _ADCLK_ADCS_POSITION);        /* ADCS FOSC/18(8) */
    ADACT = (0 << _ADACT_ADACT_POSITION);       /* ADACT disabled(0) */
    
    //PIE1bits.ADIE  = 1;
    //PIE1bits.ADTIE = 1;
    
    ADCON0 = (0 << _ADCON0_ADGO_POSITION)	/* ADGO stop(0) */
			|(1 << _ADCON0_ADFM_POSITION)	/* ADFM right(1) */
			|(1 << _ADCON0_ADON_POSITION)	/* ADON enabled(1) */
			|(0 << _ADCON0_ADCONT_POSITION)	/* ADCONT disabled(0) */
			|(1 << _ADCON0_ADCS_POSITION);	/* ADCS ADCRC(1) */;
    

    
            
            
            
     __delay_ms(100);        
            
            
    // LCD init
    
    lcd_init();
    
    lcd_customchar(0, char_wls_empty);
    lcd_customchar(1, char_wls_bottom);
    lcd_customchar(2, char_wls_top);
    lcd_customchar(3, char_wls_over);
    lcd_customchar(4, char_degreeSign);
    lcd_customchar(5, char_degreeSign1);
    lcd_customchar(6, char_degreeSign2);
    
    
    
    
    
        
    if (readInputs() & (1 << INPUT_BTN_FUNC_BIT))
    {
        // enable debug TX
        debug_tx = 1;
        
        lcd_set_ddram_addr(LCD_LINE_1);
        __delay_ms(5);
        lcd_puts("DEBUG TX ENABLED");
        while (readInputs() & (1 << INPUT_BTN_FUNC_BIT));   // wait until button is released         
    }
    
    printf("*** Startup ***\r\n");
    
    
    
    
    if (readInputs() & (1 << INPUT_BTN_EDIT_BIT))
    {
        // EEPROM erase
        
        lcd_set_ddram_addr(LCD_LINE_1);
        __delay_ms(5);
        lcd_puts("!!EEPROM ERASE!!");
        
        
        uint8_t eraseCnt = 10;
        
        
        while (eraseCnt)
        {
            
            lcd_set_ddram_addr(LCD_LINE_2);
            __delay_ms(5);
            sprintf(lcd_buf2, "%2u", eraseCnt);
            lcd_puts(lcd_buf2);
            
            __delay_ms(1000);
            
            if (readInputs() & (1 << INPUT_BTN_EDIT_BIT))
            {
                eraseCnt--;
            }
            else
            {
                break;
            }        
        }
        
        if (eraseCnt == 0)
        {
            // Perform erase
            lcd_set_ddram_addr(LCD_LINE_2);
            __delay_ms(5);
            lcd_puts("ERASING... ");
            
            eepromClear();
            
            lcd_puts("DONE!");
        }
        
        while (readInputs() & (1 << INPUT_BTN_EDIT_BIT));
        
    }
    
    
    // load settings from EEPROM
    printf("Loading settings.\r\n");
    loadSettings();     
    
    
    // Timer0 (systick))
    TMR0H = 0xFE;                    // Systick rate = 1024 Hz
    TMR0L = 0x0;
    
    T0CON1 = (2 << _T0CON1_T0CS_POSN)   // T0CS FOSC/4
        | (4 << _T0CON1_T0CKPS_POSN)   // T0CKPS 1:16
        | (1 << _T0CON1_T0ASYNC_POSN);  // T0ASYNC not_synchronised

    IPR0bits.TMR0IP = 1;    // High priority
    PIR0bits.TMR0IF = 0;	   
    PIE0bits.TMR0IE = 1;

    T0CON0 = (0 << _T0CON0_T0OUTPS_POSN)   // T0OUTPS 1:1
        | (1 << _T0CON0_T0EN_POSN)   // T0EN enabled
        | (0 << _T0CON0_T016BIT_POSN);  // T016BIT 8-bit
 

    
    printf("Interrupt enable.\r\n");
    
  
    GIEL = 1;
    GIEH = 1;
    
    //PEIE = 1;
    //GIE = 1;
    
    
    //SysTick_Delay(100);
    
    //printf("LCD init.\r\n");
    

    
    //TEMP_SEL = 1;
    
    // start ADC
    //ADCON0bits.ADCONT = 1;
    printf("ADC start.\r\n");
    adcTimer = SysTick_GetTicks();
    ADCON0bits.ADGO = 1;
    
     // WWDT software enable. 
    printf("WDT enable.\r\n");
    WDTCON0bits.SEN=1;   
    
    printf("Startup complete.\r\n");
    
    for (;;)
    {
        modbus_state_machine();
        
        inputState = readInputs();
        
        
        handleButtons();
        
        
        
        if ((!(inputState & (1 << INPUT_WLVL_BASE_BIT))) && (!(inputState & (1 << INPUT_WLVL_TOP_BIT))) && (!(inputState & (1 << INPUT_WLVL_OVER_BIT))))
        {
            // water level OVER

            water_level = WATER_LEVEL_OVER;
        }
        else if ((!(inputState & (1 << INPUT_WLVL_BASE_BIT))) && (!(inputState & (1 << INPUT_WLVL_TOP_BIT))) && ((inputState & (1 << INPUT_WLVL_OVER_BIT))))
        {
            // water level TOP
            water_level = WATER_LEVEL_TOP;
        }
        else if ((!(inputState & (1 << INPUT_WLVL_BASE_BIT))) && ((inputState & (1 << INPUT_WLVL_TOP_BIT))) && ((inputState & (1 << INPUT_WLVL_OVER_BIT))))
        {
            // water level BASE
            water_level = WATER_LEVEL_BASE;
        }
        else if (((inputState & (1 << INPUT_WLVL_BASE_BIT))) && ((inputState & (1 << INPUT_WLVL_TOP_BIT))) && ((inputState & (1 << INPUT_WLVL_OVER_BIT))))
        {
            // water level EMPTY
            water_level = WATER_LEVEL_EMPTY;
        }
        else
        {
            // water level ERROR
            water_level = WATER_LEVEL_ERROR;
        }
        
        if (water_level == WATER_LEVEL_TOP)
        {
            if (wls_led_state == 0)
            {
                setOutputs1((1 << OUTPUT1_DRYLED_BIT), (1 << OUTPUT1_DRYLED_BIT));
                wls_led_state = 1;
            }
        }
        else
        {
            if (wls_led_state == 1)
            {
                setOutputs1(0, (1 << OUTPUT1_DRYLED_BIT));
                wls_led_state = 0;
            }
        }
        
        
        
        // handle ADC
        if (PIR1bits.ADTIF)
        {
            // ADC conversion complete

            if (temp_sel == 1)
            {
                // Ext sensor
                temp_sel = 0;
                setOutputs1(0, (1 << OUTPUT1_TEMPSEL_BIT));   // Select internal sensor
                
                temperatures[TEMP_EXTERNAL] = ntc_ext_table[(uint8_t)(ADFLTR >> 2)];
                
                //adcValExt = ADFLTR >> 2;
            }
            else
            {
                // Int sensor
                temp_sel = 1;
                setOutputs1((1 << OUTPUT1_TEMPSEL_BIT), (1 << OUTPUT1_TEMPSEL_BIT));    // Select external sensor
                
                temperatures[TEMP_INTERNAL] = ntc_ext_table[(uint8_t)(ADFLTR >> 2)];
                //adcValInt = ADFLTR >> 2;
            }
            
            PIR1bits.ADTIF = 0;
        }
        
        
        
        if (SysTick_CheckElapsed(adcTimer, 256))
        {
            adcTimer = SysTick_GetTicks();
            ADCON0bits.ADGO = 1;
        }
        
        
        
        
        
        
        
        
        if (run_mode == RUN_MODE_IDLE)
        {
            // do not run timer in IDLE
            everySecondTimer = SysTick_GetTicks();            
        }
        else if (SysTick_CheckElapsed(everySecondTimer, 1024))
        {
            // 1024 ticks = 1 second
            everySecondTimer = SysTick_GetTicks();
            
            
            if (run_mode == RUN_MODE_NORMAL)
            {
                if (++elapsedTime >= (settings.set_time * 60))
                {
                    printf("Normal cycle complete.\r\n");
                    setMode(RUN_MODE_IDLE);
                }
            }
            else if (run_mode == RUN_MODE_DEGAS)
            {
                if (++elapsedTime >= (settings.degas_time * 60))
                {
                    printf("Degas cycle complete.\r\n");
                    setMode(RUN_MODE_IDLE);
                }
                
                
                
                if (degas_pulse_timer > 0)
                {
                    degas_pulse_timer--;
                    
                    if (degas_pulse_timer == 0)
                    {
                        if (ultrasonic_on)
                        {
                            degas_pulse_timer = settings.degas_pulse_off;
                            ultrasonic_on = 0;
                            setOutputs1(0x00, 0x20);
                            printf("Ultrasonics OFF\r\n");
                        }
                        else
                        {
                            degas_pulse_timer = settings.degas_pulse_on;
                            ultrasonic_on = 1;
                            setOutputs1(0x20, 0x20);
                            printf("Ultrasonics ON\r\n");
                        }
                    }
                }
                
                // Blink status LED when in degas mode
                if (elapsedTime & 0x1)
                {
                    setOutputs1(0x00, 0x02);
                }
                else
                {
                    setOutputs1(0x02, 0x02);
                }
                
                
                
                
                
            }
            else if (run_mode == RUN_MODE_DRAIN)
            {
                // check if we need to transition to AUTODRAIN mode
                
                // water level check
                
                if ((water_level == WATER_LEVEL_EMPTY) && (settings.autodrain_enable))
                {
                 
                    // Empty
                    if (autodrain_timer > 0)
                    {
                        autodrain_timer--;
                        
                        if (autodrain_timer == 0)
                        {
                            // stop
                            printf("Auto drain complete.\r\n");
                            setMode(RUN_MODE_IDLE);
                        }
                        
                    }
                    
                }
                else
                {
                    // Not empty
                    autodrain_timer = settings.autodrain_time;
                    
                }
                
            }
            
        }
        
        
        
        
        if (!lockoutCheck())
        {
            // lockout            
            setHeater(0);   // heat off
            setRecirc(0);   // recirc off 
            
            if ((run_mode == RUN_MODE_NORMAL) || (run_mode == RUN_MODE_DEGAS))
            {
                setMode(RUN_MODE_IDLE);
            }
            
        }
        
        
        
        
        
        if (heater_enabled)
        {
            if ((heat_on) && (temperatures[TEMP_EXTERNAL] > settings.set_temp))
            {
                // heat off
                heat_on = 0;
                printf("Heater off\r\n");
                setOutputs1(0, (1 << OUTPUT1_HEATER_ON_BIT));
            }
            else if ((!heat_on) && (temperatures[TEMP_EXTERNAL] < settings.set_temp))
            {
                // heat on
                heat_on = 1;
                printf("Heater on\r\n");
                setOutputs1((1 << OUTPUT1_HEATER_ON_BIT), (1 << OUTPUT1_HEATER_ON_BIT));
            }
        }
        
        

        
        
        
        
        
        
        
        
        
        if (savePend && SysTick_CheckElapsed(savePendTimer, 10240))
        {
            // 10240 ticks = 10 seconds
            
            // save settings to EEPROM
                
            printf("EEPROM save!\r\n");
            
            
            //for (uint8_t i2=0; i2<100; i2++)
            //{
                saveSettings();
             //   CLRWDT();
            //}
            
            savePend = false;
        }
        
        
        
        
        
        
        
        
        
        
        //I2C1_WriteRead(0x68, &reg, 1, i2cReadBuf, 8);
    
        //while (I2C1_IsBusy());
        
        //uint8_t sec = bcd2dec(i2cReadBuf[0]);
        //uint8_t min = bcd2dec(i2cReadBuf[1]);
        //uint8_t hour = bcd2dec(i2cReadBuf[2]);
        //uint8_t day = bcd2dec(i2cReadBuf[4]);
        //uint8_t month = bcd2dec(i2cReadBuf[5]);
        //uint8_t year = bcd2dec(i2cReadBuf[6]);
        
       //float tempInt = adcValInt;
        
        //tempInt *= 0.4508;
        //tempInt += 2.7831;
        
        //float tempExt = adcValExt;
        
        //tempExt = 1E-05*powf(tempExt,3) - 0.0046*powf(tempExt,2) + 0.9924*tempExt - 14.094;
        
        
        //tempExt *= 0.4508;
        //tempExt += 2.7831;
        
        
        //sprintf(lcd_buf1, "C=%02X I=%+04d", ADCNT, ntc_ext_table[(uint8_t)adcValInt]);
        //sprintf(lcd_buf2, "S=%02X E=%+04d", ADSTAT, ntc_ext_table[(uint8_t)adcValExt]);
        //sprintf(lcd_buf1, "20%02u-%02u-%02u", year, month, day);
        //sprintf(lcd_buf2, "%02u:%02u:%02u", hour, min, sec);
        
        switch (screen)
        {
            case SCREEN_NORMAL:
                updateLCD_normal();
                break;
            case SCREEN_NORMAL_SHIFT:
                updateLCD_shifted();
                break;
            case SCREEN_MENU:
                updateLCD_menu();
                break;
            case SCREEN_MENU_AUTODRAIN_TIME:
                updateLCD_menu_autodrain();
                break;
            case SCREEN_MENU_AUTODEGAS_TIME:
                updateLCD_menu_autodegas();
                break;
                
        }
        
        
        lcd_set_ddram_addr(LCD_LINE_1);
        SysTick_Delay(5);
        lcd_puts(lcd_buf1);
        lcd_set_ddram_addr(LCD_LINE_2);
        SysTick_Delay(5);
        lcd_puts(lcd_buf2);
        
        
        
       // TRISB = 0x00;
       // LATB = 0x08;
        //__delay_ms(2);
        
        //SysTick_Delay(2);
        //LATAbits.LATA1 = 1;
        //SysTick_Delay(2);
        //__delay_ms(2);
        //LATAbits.LATA1 = 0;
        //TRISB = 0xFF;
       // SysTick_Delay(250);
        // __delay_ms(500);
        
        //TRISB = 0x00;
       // LATB = 0x00;
       // SysTick_Delay(2);
         //__delay_ms(2);
       // LATAbits.LATA1 = 1;
        //SysTick_Delay(2);
        // __delay_ms(2);
        //LATAbits.LATA1 = 0;
        //TRISB = 0xFF;
        //SysTick_Delay(250);
         //__delay_ms(500);
        
        CLRWDT();

    }


}




//void TMR0_ISR()
//{    
    //static uint8_t adcTmr = 0;
    
   // PIR0bits.TMR0IF = 0;
    
    //if (++adcTmr == 0)
    //{
        // Trigger ADC every 256 ticks
    //    ADCON0bits.ADGO = 1;
    //}
    
  //  systick_isr();
    
    
//}

void __interrupt(__low_priority) isr_low(void)
{
    // interrupt handler
    if(TX1IE == 1 && TX1IF == 1)
    {
        modbus_tx_isr();
    } 
    else if(RC1IE == 1 && RC1IF == 1)
    {
        modbus_rx_isr();
    }
}

//void __interrupt(__flags(PEIE,TX1IE,TX1IF), __low_priority) isr_tx(void)
//{
     
//}

void __interrupt(__high_priority) isr_high(void)
{
    
    // Only TMR0 enabled as high priority interrupt, no need to determine source
    PIR0bits.TMR0IF = 0;
    systick_isr();
    
    // interrupt handler
    //if(INTCONbits.PEIE == 1)
    //{
        //if(PIE1bits.ADIE == 1 && PIR1bits.ADIF == 1)
        //{
            //ADC_ISR();
        //} 
        //else if(PIE1bits.ADTIE == 1 && PIR1bits.ADTIF == 1)
        //{
        //    ADC_ThresholdISR();
        //} 
        //if(PIE0bits.TMR0IE == 1 && PIR0bits.TMR0IF == 1)
        //{
        //    TMR0_ISR();
        //} 
        //else
        //{
            //Unhandled Interrupt
        //}
    //}      
    //else
    //{
        //Unhandled Interrupt
    //}
    
}




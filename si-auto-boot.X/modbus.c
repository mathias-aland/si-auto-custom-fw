/*
 * modbus.c
 *
 * Created: 10-09-2023 22:50:40
 *  Author: Mathias
 */ 
#include <xc.h>
#include <limits.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include "modbus.h"
#include "rs232.h"
#include "nvm.h"


bool rstPend = false;

volatile uint8_t modbusState;


uint8_t modbus_addr = 0;	// slave address

uint8_t modbus_recvSum;
uint8_t modbus_buf[255] = {0};	// Max PDU is 253 bytes + 1 byte LRC + 1 byte station address = 255 bytes buffer size
uint8_t modbus_buflen = 0;

bool checkRstPend(void)
{
    return rstPend;
}


// the following function assumes the character is a valid HEX digit, eg. 0-9, A-F or a-f
uint8_t hexToNibble(char digit)
{
	if (digit >= 'a')
	{
		return digit - 'a' + 10;
	}
	else if (digit >= 'A')
	{
		return digit - 'A' + 10;
	}
	else
	{
		return digit - '0';
	}
}

char nibbleToHex(uint8_t nibble)
{
	char c;
	
	if (nibble < 10)
	{
		c = nibble + '0';
	}
	else
	{
		c = nibble - 10 + 'A';
	}
	
	return c;
}

void modbus_tx_isr()
{
	// Data Register Empty
	static uint8_t lrc;
	static uint8_t bufPtr;
	
	switch (modbusState)
	{
		case MODBUS_STATE_SENDSTART:
			rs232TxByte(':');	// send start character
			bufPtr = 0;
			lrc = 0;
			modbusState = MODBUS_STATE_SENDDATA1;
			break;
		case MODBUS_STATE_SENDDATA1:
			if (bufPtr == modbus_buflen)
			{
				// all data sent, proceed with LRC
				lrc = -lrc;
				rs232TxByte(nibbleToHex(lrc >> 4));		// send LRC byte (first hex digit in byte)
				modbusState = MODBUS_STATE_SENDLRC2;
			}
			else
			{
				rs232TxByte(nibbleToHex(modbus_buf[bufPtr] >> 4));	// send data byte (first hex digit in byte)
				modbusState = MODBUS_STATE_SENDDATA2;				
			}
			break;
		case MODBUS_STATE_SENDDATA2:
			rs232TxByte(nibbleToHex(modbus_buf[bufPtr] & 0x0f));	// send data byte (last hex digit in byte)
			lrc += modbus_buf[bufPtr++];								// update LRC
			modbusState = MODBUS_STATE_SENDDATA1;
			break;
		case MODBUS_STATE_SENDLRC2:
			rs232TxByte(nibbleToHex(lrc & 0x0f));		// send LRC byte (last hex digit in byte)
			modbusState = MODBUS_STATE_SENDCR;
			break;
		case MODBUS_STATE_SENDCR:
			rs232TxByte('\r');					// send CR
			modbusState = MODBUS_STATE_SENDLF;
			break;	
		case MODBUS_STATE_SENDLF:
			rs232TxByte('\n');					// send LF
            PIE3bits.TX1IE = 0;                 // disable TX interrupt
			modbusState = MODBUS_STATE_WAITSTART;
			break;           
		default:
            PIE3bits.TX1IE = 0;  // disable TX interrupt
			modbusState = MODBUS_STATE_WAITSTART;
	}	
}


void modbus_rx_isr() {
	uint8_t rxData;
	uint8_t rxFlags;
    
    if (RC1STAbits.OERR)
    {
        // overrun error, reset receiver
        RC1STAbits.CREN = 0;
        RC1STAbits.CREN = 1;
        return;
    }
	
	// read bytes from FIFO
	rxData = rs232RxByte();
    
    
	
	if (RC1STAbits.FERR)
	{
		// framing error
		return;	// ignore data
	}

    if (rxData & 0x80)
	{
		// parity error
		return;	// ignore data
	}
    
    // RX OK

    if (PIE3bits.TX1IE)
    {
        // TX interrupt enabled -> TX busy (ignore RX while transmitting)
        return;
    }

    if (modbusState > MODBUS_STATE_WAITEND)
    {
        // do nothing
        return;
    }

    if (rxData == ':')
    {
        // start of frame, reset buffer and start waiting for address
        modbus_buflen = 0;
        modbus_recvSum = 0;	// clear LRC
        modbusState = MODBUS_STATE_WAITDATA1;
        return;

    }
    else if (rxData == '\r')
    {
        // end of packet 1

        if ((modbusState != MODBUS_STATE_WAITDATA1) || (modbus_buflen < 3))
        {
            // unexpected end of packet char received, start over
            modbusState = MODBUS_STATE_WAITSTART;
        }
        else
        {
            modbusState = MODBUS_STATE_WAITEND;
        }
    }
    else if (rxData == '\n')
    {
        // end of packet 2
        if (modbusState != MODBUS_STATE_WAITEND)
        {
            // unexpected end of packet char received, start over
            modbusState = MODBUS_STATE_WAITSTART;
        }
        else
        {
            // entire packet received, do LRC check
            // in case of correct reception the sum of all data bytes will be zero
            if (modbus_recvSum != 0)
            {
                // LRC check failed, discard data and start over
                modbusState = MODBUS_STATE_WAITSTART;
            }
            else
            {
                // data OK, strip LRC
                modbus_buflen--;

                modbusState = MODBUS_STATE_WAITPROC;
                //modbus_pend = true;
            }
        }			
    }
    else if (isxdigit(rxData))
    //else if (((rxData >= '0') && (rxData <= '9')) || ((rxData >= 'A') && (rxData <= 'F')) || ((rxData >= 'a') && (rxData <= 'f')))
    {
        // valid hex char
        if (modbusState == MODBUS_STATE_WAITDATA1)
        {	
            // high nibble received
            if (modbus_buflen >= sizeof(modbus_buf))
            {
                // buffer overflow, discard and start over
                modbusState = MODBUS_STATE_WAITSTART;
            }
            else
            {
                modbus_buf[modbus_buflen] = hexToNibble(rxData) << 4;	// high nibble
                modbusState = MODBUS_STATE_WAITDATA2;
            }
        }
        else if (modbusState == MODBUS_STATE_WAITDATA2)
        {
            // low nibble received
            modbus_buf[modbus_buflen] |= hexToNibble(rxData);	// low nibble

            if ((modbus_buflen == 0) && (modbus_buf[0] != modbus_addr))
            {
                // not our address, start over
                modbusState = MODBUS_STATE_WAITSTART;
            }
            else
            {
                modbus_recvSum += modbus_buf[modbus_buflen++];	// update LRC
                modbusState = MODBUS_STATE_WAITDATA1;
            }
        }
    }
}

void sendReply(uint8_t exception)
{
	if (exception != 0)
	{
		// exception
		modbus_buf[1] |= 0x80;	// set highest bit in FC to indicate exception status
		modbus_buf[2] = exception;
		modbus_buflen = 3;
	}
	
	// start TX

	modbusState = MODBUS_STATE_SENDSTART;
    PIE3bits.TX1IE = 1;  // enable TX interrupt, this will trigger the TX sequence
	
}





void modbus_start()
{
    TX1STAbits.TXEN = 1; // enable transmitter    
    RC1STAbits.CREN = 1; // enable receiver
    PIE3bits.RC1IE = 1;  // enable RX interrupt
}



void modbus_state_machine()
{
	//uint16_t devID;
    uint16_t flashAddr;
    uint16_t unlockKey;
    uint16_t flashData;
    uint8_t numBytes;
    uint8_t secNum;
    uint8_t *pBuf;
    
	if (modbusState == MODBUS_STATE_WAITPROC)
	{
		// message waiting
		
		// packet received
		uint8_t exception = 0;
        
		// check function code
        if (modbus_buf[1] == MODBUS_FUNC_BOOTLOADER)
        {
            // check sub-function code
            switch (modbus_buf[2])
            {
                case MODBUS_BOOT_SUB_IDENTIFY:
                    modbus_buf[3] = FLASH_Read(0x3FFFFFU);  // Device ID MSB
                    modbus_buf[4] = FLASH_Read(0x3FFFFEU);  // Device ID LSB
                    modbus_buf[5] = FLASH_Read(0x3FFFFDU);  // Revision ID MSB
                    modbus_buf[6] = FLASH_Read(0x3FFFFCU);  // Revision ID LSB
                    modbus_buflen = 7;
                    break;
                case MODBUS_BOOT_SUB_RESET:
                    rstPend = true;
                    break;
                case MODBUS_BOOT_SUB_READ:
                    flashAddr  = (uint16_t)modbus_buf[4] << 8;  // flash address MSB
                    flashAddr |= modbus_buf[5];                 // flash address LSB
                    numBytes = modbus_buf[6];                   // number of bytes to read
                    
                    modbus_buflen = numBytes + 7;
                    
                    if (numBytes > 246)
                    {
                        exception = MODBUS_ERR_ILLEGAL_DATA_VAL;
                    }
                    else
                    {
                        // Fill buffer with data
                        pBuf = &modbus_buf[7];
                        
                        while (numBytes)
                        {
                            *pBuf++ = FLASH_Read(flashAddr++);
                            numBytes--;
                        }                        
                    }
                    
                    break;    
                case MODBUS_BOOT_SUB_ERASE:
                    secNum  = modbus_buf[4];                    // sector number to erase
                    unlockKey = modbus_buf[5];                 // unlock key 1
                    unlockKey |= (uint16_t)modbus_buf[6] << 8;  // unlock key 2
                    
                    modbus_buflen = 7;
                    
                    if (secNum < 8)
                    {
                        // Bootloader area
                        exception = MODBUS_ERR_ILLEGAL_DATA_VAL;
                    }
                    else
                    {
                        NVM_StatusClear();
                        NVM_UnlockKeySet(unlockKey);
                        
                        if (FLASH_PageErase((uint16_t)secNum*256) == NVM_ERROR)
                        {
                            exception = MODBUS_ERR_ILLEGAL_DATA_VAL;
                        }
                        
                        NVM_UnlockKeyClear();
                    }
                    
                    unlockKey = 0;
                    
                    break;    
                case MODBUS_BOOT_SUB_WRITE:
                    flashAddr  = (uint16_t)modbus_buf[4] << 8;  // flash address MSB
                    flashAddr |= modbus_buf[5];                 // flash address LSB
                    numBytes = modbus_buf[6];                   // number of bytes to write
                    unlockKey = modbus_buf[7];                 // unlock key 1                    
                    unlockKey |= (uint16_t)modbus_buf[8] << 8;  // unlock key 2

                    
                    modbus_buflen = 9;
                    
                    if ((numBytes > 244) || (numBytes & 0x01) || (flashAddr & 0x01) || (flashAddr < 0x800))
                    {
                        exception = MODBUS_ERR_ILLEGAL_DATA_VAL;
                    }
                    else
                    {
                        NVM_UnlockKeySet(unlockKey);
                        
                        // Write flash
                        
                        pBuf = &modbus_buf[9];
                        
                        while (numBytes)
                        {
                            
                            flashData  = *pBuf++;                 // flash data LSB
                            flashData |= (uint16_t)*pBuf++ << 8;  // flash data MSB
                            
                            
                            NVM_StatusClear();
                            
                            if (FLASH_Write(flashAddr, flashData) == NVM_ERROR)
                            {
                                exception = MODBUS_ERR_ILLEGAL_DATA_VAL;
                                break;
                            }
                            
                            flashAddr += 2;
                            numBytes -= 2;
                        }
                        
                        NVM_UnlockKeyClear();
                    }
                    
                    unlockKey = 0;
                    
                    break;
                default:
				exception = MODBUS_ERR_ILLEGAL_FUNC;
            }
        }
        else
        {
            exception = MODBUS_ERR_ILLEGAL_FUNC;
        }
						
		sendReply(exception);
		
	}
	
}

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
#include "globals.h"

volatile uint8_t modbusState;

uint8_t modbus_addr = 0;	// slave address
uint8_t modbus_recvSum;
uint8_t modbus_buf[255] = {0};	// Max PDU is 253 bytes + 1 byte LRC + 1 byte station address = 255 bytes buffer size
uint8_t modbus_buflen = 0;

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

uint8_t writeCoil(uint16_t addr, bool data)
{
	
	switch (addr)
	{
		case MODBUS_COIL_NORMAL_CYCLE_STATE:
            if (data)
            {
                // start normal cycle
                if (run_mode == RUN_MODE_IDLE)
                    setMode(RUN_MODE_NORMAL);
            }
            else
            {
                if (run_mode == RUN_MODE_NORMAL)
                    setMode(RUN_MODE_IDLE);
            }
			break;
        case MODBUS_COIL_DEGAS_CYCLE_STATE:
            if (data)
            {
                // start degas cycle
                if (run_mode == RUN_MODE_IDLE)
                    setMode(RUN_MODE_DEGAS);
            }
            else
            {
                if (run_mode == RUN_MODE_DEGAS)
                    setMode(RUN_MODE_IDLE);
            }
            break; 
        case MODBUS_COIL_DRAIN_CYCLE_STATE:
            if (data)
            {
                // start drain cycle
                if (run_mode == RUN_MODE_IDLE)
                    setMode(RUN_MODE_DRAIN);
            }
            else
            {
                if (run_mode == RUN_MODE_DRAIN)
                    setMode(RUN_MODE_IDLE);
            }
            break;
        case MODBUS_COIL_HEATER_ENABLED:
            setHeater(data);
			break;
        case MODBUS_COIL_RECIRC_ENABLED:
            setRecirc(data);
            break; 
        case MODBUS_COIL_AIR_ENABLED:
            setAir(data);
            break;    
        case MODBUS_COIL_WLP_ENABLED:
            settings.wlp_enable = data;
            resetSaveTimer();
            break; 
        case MODBUS_COIL_AUTODRAIN_ENABLED:
            settings.autodrain_enable = data;
            resetSaveTimer();
            break;	
		default:
			return MODBUS_ERR_ILLEGAL_DATA_ADDR;	// address not found
	}
	return 0;
	
}

uint8_t readCoil(uint16_t addr, bool *data)
{
	
	switch (addr)
	{
		case MODBUS_COIL_NORMAL_CYCLE_STATE:
            *data = run_mode == RUN_MODE_NORMAL;
			break;
        case MODBUS_COIL_DEGAS_CYCLE_STATE:
            *data = run_mode == RUN_MODE_DEGAS;
            break; 
        case MODBUS_COIL_DRAIN_CYCLE_STATE:
            *data = run_mode == RUN_MODE_DRAIN;
            break;
        case MODBUS_COIL_HEATER_ENABLED:
            *data = heater_enabled;
			break;
        case MODBUS_COIL_RECIRC_ENABLED:
            *data = recirc_enabled;
            break; 
        case MODBUS_COIL_AIR_ENABLED:
            *data = air_enabled;
            break;    
        case MODBUS_COIL_WLP_ENABLED:
            *data = settings.wlp_enable;
            break; 
        case MODBUS_COIL_AUTODRAIN_ENABLED:
            *data = settings.autodrain_enable;
            break;
		default:
			return MODBUS_ERR_ILLEGAL_DATA_ADDR;	// address not found
	}
	return 0;
	
}



uint8_t readCoils(uint16_t addr, uint16_t numCoils)
{
	if ((numCoils > 1000))
	{
		return MODBUS_ERR_ILLEGAL_DATA_ADDR;	// too many coils requested
	}
	
	uint8_t bytes = (numCoils + 7) / 8;	// Round upwards
	
	modbus_buf[2] = bytes;	// number of data bytes to follow
	modbus_buflen = 3 + (bytes);
	
	uint8_t modbus_buf_idx = 3;	// first data byte at pos 3
	
	bool coilValue;
	uint8_t retVal;
	
	while (bytes)
	{
		
		modbus_buf[modbus_buf_idx] = 0;
		
		for (uint8_t bitpos = 0; bitpos < 8; bitpos++)
		{
			if (numCoils == 0)
			{
				break;
			}
			
			retVal = readCoil(addr, &coilValue);
			
			if (retVal != 0)
			{
				return retVal;
			}
			
			modbus_buf[modbus_buf_idx] |= coilValue << bitpos;
			
			addr++;
			numCoils--;
		}
		
		bytes--;
		modbus_buf_idx++;
		
	}

	return 0;
	
}

uint8_t readHoldingRegs(uint16_t addr, uint16_t numRegs)
{
	if (numRegs > 125)
	{
		return MODBUS_ERR_ILLEGAL_DATA_ADDR;	// too many regs requested
	}
	
	
	modbus_buf[2] = numRegs * 2;	// number of data bytes to follow
	modbus_buflen = 3 + (numRegs * 2);
	
	uint8_t modbus_buf_idx = 3;	// first data byte at pos 3
	
	while (numRegs)
	{	
        
        if (addr == MODBUS_HOLDING_SET_TIME)
		{
			modbus_buf[modbus_buf_idx++] = settings.set_time >> 8;		// MSB
			modbus_buf[modbus_buf_idx++] = settings.set_time & 0xFF;	// LSB	
		}
        else if (addr == MODBUS_HOLDING_DEGAS_TIME)
		{
			modbus_buf[modbus_buf_idx++] = settings.degas_time >> 8;		// MSB
			modbus_buf[modbus_buf_idx++] = settings.degas_time & 0xFF;      // LSB	
		}
        else if (addr == MODBUS_HOLDING_AUTODRAIN_TIME)
		{
			modbus_buf[modbus_buf_idx++] = 0;                               // MSB
			modbus_buf[modbus_buf_idx++] = settings.autodrain_time & 0xFF;	// LSB	
		}
        else if (addr == MODBUS_HOLDING_SET_TEMP)
		{
			modbus_buf[modbus_buf_idx++] = 0;                           // MSB
			modbus_buf[modbus_buf_idx++] = settings.set_temp & 0xFF;	// LSB	
		}
        else if (addr == MODBUS_HOLDING_DEGAS_PULSE_OFF)
		{
			modbus_buf[modbus_buf_idx++] = 0;                               // MSB
			modbus_buf[modbus_buf_idx++] = settings.degas_pulse_off & 0xFF;	// LSB	
		}
        else if (addr == MODBUS_HOLDING_DEGAS_PULSE_ON)
		{
			modbus_buf[modbus_buf_idx++] = 0;                               // MSB
			modbus_buf[modbus_buf_idx++] = settings.degas_pulse_on & 0xFF;	// LSB	
		}
        else
        {
            return MODBUS_ERR_ILLEGAL_DATA_ADDR;	// address not found            
        }
		
		numRegs--;
		addr++;
	}

	return 0;
	
}


uint8_t writeHoldingRegs(uint16_t addr, uint16_t numRegs)
{
	if ((numRegs > 125) || (numRegs * 2 != modbus_buf[6]))
	{
		return MODBUS_ERR_ILLEGAL_DATA_ADDR;	// too many registers or invalid byte count
	}
	
	modbus_buflen = 6;
	
	uint8_t modbus_buf_idx = 7;	// first data byte at pos 7
	
	while (numRegs)
	{
        if (addr == MODBUS_HOLDING_SET_TIME)
		{
            uint16_t tempParam = modbus_buf[modbus_buf_idx++];	// MSB
			tempParam <<= 8;									// Shift MSB
			tempParam |= modbus_buf[modbus_buf_idx++];			// LSB
            
            if (tempParam > 599)
                tempParam = 599;
            
			settings.set_time = tempParam;
            resetSaveTimer();
		}
        else if (addr == MODBUS_HOLDING_DEGAS_TIME)
		{
			uint16_t tempParam = modbus_buf[modbus_buf_idx++];	// MSB
			tempParam <<= 8;									// Shift MSB
			tempParam |= modbus_buf[modbus_buf_idx++];			// LSB
            
            if (tempParam > 95)
                tempParam = 95;
            else if (tempParam < 5)
                tempParam = 5;
            
			settings.degas_time = tempParam;
            resetSaveTimer();
		}
        else if (addr == MODBUS_HOLDING_AUTODRAIN_TIME)
		{            
            modbus_buf_idx++;	// MSB
            uint8_t tempParam = modbus_buf[modbus_buf_idx++]; // LSB
            
            if (tempParam > 95)
                tempParam = 95;
            else if (tempParam < 5)
                tempParam = 5;
            
			settings.autodrain_time = tempParam;
            resetSaveTimer();
		}
        else if (addr == MODBUS_HOLDING_SET_TEMP)
		{
            modbus_buf_idx++;	// MSB
            uint8_t tempParam = modbus_buf[modbus_buf_idx++]; // LSB
            
            if (tempParam > 95)
                tempParam = 95;
            else if (tempParam < 5)
                tempParam = 5;
            
			settings.set_temp = tempParam;	
            resetSaveTimer();
		}
        else if (addr == MODBUS_HOLDING_DEGAS_PULSE_OFF)
		{
            modbus_buf_idx++;	// MSB
            uint8_t tempParam = modbus_buf[modbus_buf_idx++]; // LSB
            
            if (tempParam > 99)
                tempParam = 99;
            else if (tempParam < 1)
                tempParam = 1;
            
			settings.degas_pulse_off = tempParam; // LSB
            resetSaveTimer();
		}
        else if (addr == MODBUS_HOLDING_DEGAS_PULSE_ON)
		{
            modbus_buf_idx++;	// MSB
            uint8_t tempParam = modbus_buf[modbus_buf_idx++]; // LSB
            
            if (tempParam > 99)
                tempParam = 99;
            else if (tempParam < 1)
                tempParam = 1;
            
			settings.degas_pulse_on = tempParam; // LSB
            resetSaveTimer();
		}
        else if (addr == MODBUS_HOLDING_SET_OUTPUTS1)
		{
            uint8_t state = modbus_buf[modbus_buf_idx++];
            uint8_t mask = modbus_buf[modbus_buf_idx++];
            setOutputs1(state, mask);
		}
        else if (addr == MODBUS_HOLDING_SET_OUTPUTS2)
		{
            uint8_t state = modbus_buf[modbus_buf_idx++];
            uint8_t mask = modbus_buf[modbus_buf_idx++];
            setOutputs2(state, mask);
		}
        else
        {
            return MODBUS_ERR_ILLEGAL_DATA_ADDR;	// address not found            
        }
        
		
		numRegs--;
		addr++;
	}
	

	return 0;
	
}

uint8_t readRegs(uint16_t addr, uint16_t numRegs)
{
	if (numRegs > 125)
	{
		return MODBUS_ERR_ILLEGAL_DATA_ADDR;	// too many regs requested
	}
	
	
	modbus_buf[2] = numRegs * 2;	// number of data bytes to follow
	modbus_buflen = 3 + (numRegs * 2);
	
	uint8_t modbus_buf_idx = 3;	// first data byte at pos 3
	
	while (numRegs)
	{
		
		if (addr == MODBUS_INPUT_ELAPSED_TIME)
		{
            modbus_buf[modbus_buf_idx++] = elapsedTime >> 8;		// MSB
			modbus_buf[modbus_buf_idx++] = elapsedTime & 0xFF;		// LSB
		}
        else if (addr == MODBUS_INPUT_AUTODRAIN_TIMER)
		{
            modbus_buf[modbus_buf_idx++] = 0;		// MSB
			modbus_buf[modbus_buf_idx++] = autodrain_timer;		// LSB
		}
        else if (addr == MODBUS_INPUT_DEGAS_PULSE_TIMER)
		{
            modbus_buf[modbus_buf_idx++] = 0;		// MSB
			modbus_buf[modbus_buf_idx++] = degas_pulse_timer;		// LSB
		}
        else if (addr == MODBUS_INPUT_TEMP)
		{
            modbus_buf[modbus_buf_idx++] = temperatures[0];		// MSB
			modbus_buf[modbus_buf_idx++] = temperatures[1];		// LSB
		}
        else if (addr == MODBUS_INPUT_INPUTSTATE)
		{
            modbus_buf[modbus_buf_idx++] = inputState >> 8;         // MSB
			modbus_buf[modbus_buf_idx++] = inputState & 0xFF;		// LSB
		}
        else if (addr == MODBUS_INPUT_OUTPUTSTATE)
		{
            modbus_buf[modbus_buf_idx++] = out1State;       // MSB
			modbus_buf[modbus_buf_idx++] = out2State;		// LSB
		}
		else
		{
			return MODBUS_ERR_ILLEGAL_DATA_ADDR;	// address not found
		}
		
		numRegs--;
		addr++;
	}

	return 0;
	
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



void modbus_state_machine()
{
	
	if (modbusState == MODBUS_STATE_WAITPROC)
	{
		// message waiting
		
		// packet received
		uint8_t exception = 0;
						
		// check function code
		switch (modbus_buf[1])
		{
			case MODBUS_FUNC_WRITE_COIL:
							
				if (modbus_buflen == 6)
				{
					if ((modbus_buf[4] == 0xFF) && (modbus_buf[5] == 0x00))
					{
						// coil value = 1
						exception = writeCoil(((uint16_t)modbus_buf[2] << 8) | modbus_buf[3], true);
					}
					else if ((modbus_buf[4] == 0x00) && (modbus_buf[5] == 0x00))
					{
						// coil value = 0
						exception = writeCoil(((uint16_t)modbus_buf[2] << 8) | modbus_buf[3], false);
					}
					else
					{
						// illegal value
						exception = MODBUS_ERR_ILLEGAL_DATA_VAL;
					}
				}
				else
				{
					// Send "Illegal Data Value" exception
					exception = MODBUS_ERR_ILLEGAL_DATA_VAL;
				}
				break;
			case MODBUS_FUNC_READ_COILS:

				if (modbus_buflen == 6)
				{
					exception = readCoils(((uint16_t)modbus_buf[2] << 8) | modbus_buf[3], ((uint16_t)modbus_buf[4] << 8) | modbus_buf[5]);
				}
				else
				{
					// Send "Illegal Data Value" exception
					exception = MODBUS_ERR_ILLEGAL_DATA_VAL;
				}
				break;
			case MODBUS_FUNC_READ_INPUT_REGS:

				if (modbus_buflen == 6)
				{
					exception = readRegs(((uint16_t)modbus_buf[2] << 8) | modbus_buf[3], ((uint16_t)modbus_buf[4] << 8) | modbus_buf[5]);
				}
				else
				{
					// Send "Illegal Data Value" exception
					exception = MODBUS_ERR_ILLEGAL_DATA_VAL;
				}
				break;
						
						
			case MODBUS_FUNC_READ_HOLDING_REGS:
				if (modbus_buflen == 6)
				{
					exception = readHoldingRegs(((uint16_t)modbus_buf[2] << 8) | modbus_buf[3], ((uint16_t)modbus_buf[4] << 8) | modbus_buf[5]);
				}
				else
				{
					// Send "Illegal Data Value" exception
					exception = MODBUS_ERR_ILLEGAL_DATA_VAL;
				}
				break;
			
			case MODBUS_FUNC_WRITE_HOLDING_REGS:
				if (modbus_buflen >= 9)
				{
					exception = writeHoldingRegs(((uint16_t)modbus_buf[2] << 8) | modbus_buf[3], ((uint16_t)modbus_buf[4] << 8) | modbus_buf[5]);
				}
				else
				{
					// Send "Illegal Data Value" exception
					exception = MODBUS_ERR_ILLEGAL_DATA_VAL;
				}
				break;
			
			
			default:
				exception = MODBUS_ERR_ILLEGAL_FUNC;
		}
						
		sendReply(exception);
		
	}
	
}

/*
 * modbus.h
 *
 * Created: 10-09-2023 22:50:50
 *  Author: Mathias
 */ 


#ifndef MODBUS_H_
#define MODBUS_H_

#define MODBUS_FLAG_RXDONE	0x01

// bootloader function
#define MODBUS_FUNC_BOOTLOADER			0x6C

// exceptions
#define MODBUS_ERR_ILLEGAL_FUNC			1
#define MODBUS_ERR_ILLEGAL_DATA_ADDR	2
#define MODBUS_ERR_ILLEGAL_DATA_VAL		3

// bootloader sub-functions
#define MODBUS_BOOT_SUB_IDENTIFY		0
#define MODBUS_BOOT_SUB_RESET   		1
#define MODBUS_BOOT_SUB_READ            2
#define MODBUS_BOOT_SUB_ERASE           3
#define MODBUS_BOOT_SUB_WRITE           4


enum {
	MODBUS_STATE_WAITSTART = 0,			// waiting for start character (':')
	MODBUS_STATE_WAITDATA1,				// waiting for data (first hex digit in byte) or end of packet (CR)
	MODBUS_STATE_WAITDATA2,				// waiting for data (last hex digit in byte)
	MODBUS_STATE_WAITEND,				// waiting for end of packet (LF)
	MODBUS_STATE_WAITPROC,				// waiting for processing
	MODBUS_STATE_SENDSTART,				// send start character (':')
	MODBUS_STATE_SENDDATA1,				// send data byte (first hex digit in byte)
	MODBUS_STATE_SENDDATA2,				// send data byte (last hex digit in byte)
	MODBUS_STATE_SENDLRC1,				// send LRC (first hex digit in byte)
	MODBUS_STATE_SENDLRC2,				// send LRC (last hex digit in byte)
	MODBUS_STATE_SENDCR,				// send CR
	MODBUS_STATE_SENDLF                 // send LF
};

bool checkRstPend(void);

void modbus_start(void);

void modbus_state_machine(void);

void modbus_tx_isr(void);
void modbus_rx_isr(void);

uint8_t getModBusState(void);
void setModBusState(uint8_t state);

#endif /* MODBUS_H_ */
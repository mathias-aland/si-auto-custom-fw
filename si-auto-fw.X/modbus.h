/*
 * modbus.h
 *
 * Created: 10-09-2023 22:50:50
 *  Author: Mathias
 */ 


#ifndef MODBUS_H_
#define MODBUS_H_

#define MODBUS_FLAG_RXDONE	0x01

// coils and discrete inputs (bit access)
#define MODBUS_FUNC_READ_COILS			1
#define MODBUS_FUNC_READ_DINPUTS		2
#define MODBUS_FUNC_WRITE_COIL			5
#define MODBUS_FUNC_WRITE_COILS			15

// input and holding registers (16-bit access)
#define MODBUS_FUNC_READ_HOLDING_REGS	3
#define MODBUS_FUNC_READ_INPUT_REGS		4
#define MODBUS_FUNC_WRITE_HOLDING_REG	6
#define MODBUS_FUNC_WRITE_HOLDING_REGS	16

// file record access
#define MODBUS_FUNC_READFILE	20
#define MODBUS_FUNC_WRITEFILE	21


// exceptions
#define MODBUS_ERR_ILLEGAL_FUNC			1
#define MODBUS_ERR_ILLEGAL_DATA_ADDR	2
#define MODBUS_ERR_ILLEGAL_DATA_VAL		3


// modbus address map
#define MODBUS_COIL_NORMAL_CYCLE_STATE		100
#define MODBUS_COIL_DEGAS_CYCLE_STATE		101
#define MODBUS_COIL_DRAIN_CYCLE_STATE		102
#define MODBUS_COIL_HEATER_ENABLED          103
#define MODBUS_COIL_RECIRC_ENABLED          104
#define MODBUS_COIL_AIR_ENABLED             105
#define MODBUS_COIL_WLP_ENABLED             106
#define MODBUS_COIL_AUTODRAIN_ENABLED       107

#define MODBUS_HOLDING_SET_TIME             2000
#define MODBUS_HOLDING_DEGAS_TIME           2001
#define MODBUS_HOLDING_AUTODRAIN_TIME       2002
#define MODBUS_HOLDING_SET_TEMP             2003
#define MODBUS_HOLDING_DEGAS_PULSE_OFF      2004
#define MODBUS_HOLDING_DEGAS_PULSE_ON       2005
#define MODBUS_HOLDING_SET_OUTPUTS1         2100
#define MODBUS_HOLDING_SET_OUTPUTS2         2101




#define MODBUS_INPUT_ELAPSED_TIME           20000
#define MODBUS_INPUT_AUTODRAIN_TIMER        20001
#define MODBUS_INPUT_DEGAS_PULSE_TIMER      20002
#define MODBUS_INPUT_TEMP                   20003
#define MODBUS_INPUT_INPUTSTATE             20004
#define MODBUS_INPUT_OUTPUTSTATE            20005


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

void modbus_start(void);

void modbus_state_machine(void);

void modbus_tx_isr(void);
void modbus_rx_isr(void);

uint8_t getModBusState(void);
void setModBusState(uint8_t state);

#endif /* MODBUS_H_ */
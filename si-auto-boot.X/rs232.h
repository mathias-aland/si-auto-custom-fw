/* 
 * File:   rs232.h
 * Author: Mathias
 *
 * Created on den 8 februari 2025, 22:38
 */

#ifndef RS232_H
#define	RS232_H

void rs232TxByte(uint8_t byte);
void rs232TxData(uint8_t *data, uint8_t len);
uint8_t rs232RxByte(void);

#endif	/* RS232_H */


/*
 * uart.h
 *
 *  Created on: 10 May 2024
 *      Author: ali berk
 */

#ifndef UART_H_
#define UART_H_

void uart_init(void);
void uart_send_byte(uint8_t data);
void uart_send_string(uint8_t * cmdstr, uint8_t nstr);

#endif /* UART_H_ */

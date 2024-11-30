#ifndef UART_HAL_H_
#define UART_HAL_H_

#include "config.h"
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define ASYNC_NORMAL_MODE 16
#define ASYNC_DOUBLE_SPEED_MODE 8 
#define SYNC_MASTER_MODE 2

void uart_init(uint32_t baud, uint8_t mode);

void uart_send_byte(uint8_t data);
void uart_send_array(uint8_t *data, uint16_t data_length);
void uart_send_string(uint8_t *c);



#endif /* UART_HAL_H_*/
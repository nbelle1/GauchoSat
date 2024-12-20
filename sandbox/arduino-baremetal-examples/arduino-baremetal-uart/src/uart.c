#include "uart.h"
#include "usb.h"

static uint8_t rx_buffer[RX_BUFFER_SIZE] = {0};
volatile static uint16_t rx_count = 0;	
volatile static uint8_t uart_tx_busy = 1; // BUSY = 0 NOT BUSY = 1

char echo[2];

ISR(USART1_RX_vect){

    volatile static uint16_t rx_write_pos = 0;
	
	rx_buffer[rx_write_pos] = UDR1;
	rx_count++;
	rx_write_pos++;
	if(rx_write_pos >= RX_BUFFER_SIZE){
		rx_write_pos = 0;
	}
}

ISR(USART1_TX_vect){
    uart_tx_busy = 1;
}
//atmega32u4 datasheet p.191
void uart_init(uint32_t baud, uint8_t mode){
    uint8_t speed = ASYNC_NORMAL_MODE;
    if(mode != 0) {
        speed = ASYNC_DOUBLE_SPEED_MODE;
    }

    baud = (uint8_t)(F_CPU/(speed*baud)) - 1; 

    //set baud 
    UBRR1H = (baud && 0x0F00) >> 8;
    UBRR1L = (baud && 0x0FF);

    //Enable TX/RX and TX/RX Interrupt
    UCSR1B |= (1 << RXCIE1) | (1 << TXCIE1) | (1 << RXEN1) | (1 << TXEN1);

    /* Set frame format: 8data, 2stop bit */ 
    UCSR1C = (1<<USBS1)|(3<<UCSZ10);

    sei();
}

void uart_send_byte(uint8_t data){
    while(uart_tx_busy == 0); // busy
    uart_tx_busy = 0;
    UDR1 = data; //UART Data Register
}
void uart_send_array(uint8_t *data, uint16_t data_length){
    for(uint16_t i = 0; i < data_length; i++){
        uart_send_byte(data[i]);
    }
}
void uart_send_string(uint8_t *c){
    uint16_t i = 0;
    do{
        uart_send_byte(c[i]);
        i++;
    }while(c[i] != '\0');
    uart_send_byte(c[i]);
}

uint16_t uart_read_count(void){
	return rx_count;
}
uint8_t uart_read(void){
	static uint16_t rx_read_pos = 0;
	uint8_t data = 0;
	
	data = rx_buffer[rx_read_pos];
	rx_read_pos++;
	rx_count--;
	if(rx_read_pos >= RX_BUFFER_SIZE){
		rx_read_pos = 0;
	}
	return data;
}
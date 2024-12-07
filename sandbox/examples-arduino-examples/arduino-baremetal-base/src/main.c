#include "config.h"

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "uart.h"
#include "usb.h"



void init(void){ 
    serial.begin();
    uart_init(F_BUAD_RATE, 0);
}

int main() {
    init();
    serial.print("Hello World! \n");

    uint8_t data = 'A';
    char echo[2];

    while (1) {
        uart_send_byte(data);
        _delay_ms(50);
        data++;
        if(data > 'Z'){
            data = 'A';
        }

        /*  The serial echo is slower than the UART update rate, 
            causing some echo data to be missed when there is a delay. */
        serial.print("Echo: ");
        echo[0] = (char)uart_read(); 
        echo[1] = '\0';        
        serial.print(echo);
        serial.println();
    }

    return 0;
}

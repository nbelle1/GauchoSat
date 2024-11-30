
#include "config.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "uart_hal.h"

int main() {
    // // LED -> BUILTIN : PC7
    DDRC |= (1 << PC7); // Set PC7 as output (on Port C)

    uint8_t data = 'A';
    uart_init(115200, 0);
    sei(); //enable global interrupts
    while (1) {
        // PORTC ^= (1 << PC7); // Toggle PC7 (on Port C)

        uart_send_byte(data);
        data++;
        if(data > 'Z'){
            data = 'A';
        }

        _delay_ms(500);
    }
}

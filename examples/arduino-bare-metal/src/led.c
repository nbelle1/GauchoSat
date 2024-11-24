#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define CPU_ClOCK 16000000UL

int main() {
    //LED->BUILTIN : PC7
    DDRB |= (1 << PC7); // Set PC7 as output

    while (1) {
        PORTB ^= (1 << PC7); // Toggle PB0
        _delay_ms(500);
    }
}

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define CPU_CLOCK 16000000UL

int main() {
    // LED -> BUILTIN : PC7
    DDRC |= (1 << PC7); // Set PC7 as output (on Port C)

    while (1) {
        PORTC ^= (1 << PC7); // Toggle PC7 (on Port C)
        _delay_ms(500);
    }
}

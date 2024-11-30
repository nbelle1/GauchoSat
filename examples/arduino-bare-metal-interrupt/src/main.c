
#include "config.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

ISR(TIMER1_COMPA_vect) {
    // Toggle LED to confirm interrupt is working
    PORTC ^= (1 << PC7);  // Toggle LED on PC7
}

void timer_init() {
    // Set Timer1 to CTC mode (Clear Timer on Compare Match)
    TCCR1B |= (1 << WGM12);  // CTC mode
    TIMSK1 |= (1 << OCIE1A);  // Enable interrupt on compare match for OCR1A
    OCR1A = 15624;  // Set compare value for 1 Hz interrupt (1 second)
    
    // Set prescaler to 1024 (common value for slower timer speeds)
    TCCR1B |= (1 << CS12) | (1 << CS10);  // Prescaler 1024
    
    sei();  // Enable global interrupts
}

int main(void) {
    // Set PC7 as output for LED
    DDRC |= (1 << PC7); 
    
    // Initialize Timer
    timer_init();
    
    // Main loop
    while (1) {
        // Main loop does nothing, just wait for interrupts
    }
}
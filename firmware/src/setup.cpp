#include <Arduino.h>
#include "qpn.h"            /* QP/C framework API */
#include "bsp.h"            /* Board Support Package interface */

// Interrupt for Timer1
ISR(TIMER1_COMPA_vect) {
    QACTIVE_POST_ISR((QActive *)&AO_CubeSat, Q_BATTERY_SIG, 0U);
    QACTIVE_POST_ISR((QActive *)&AO_CubeSat, Q_TICK_SIG, 0U);

    // QF_tickXISR(0);         // Process time events for tick rate 0
}

void timer1_init(void){
    // Configure Timer1 for CTC mode, prescaler 1024
    TCCR1A = 0U;                                                    // Normal port operation (no PWM)
    TCCR1B = (1U << WGM12) | (1U << CS12) | (1U << CS10);           // CTC mode, 1024 prescaler
    TIMSK1 = (1U << OCIE1A);                                        // Enable TIMER1 compare interrupt
    TCNT1 = 0U;                                                     // Clear the timer counter

    // Set the compare match value for 100 Hz (10 ms interval)
    OCR1A = (F_CPU / BSP_TICKS_PER_SEC / 1024U) - 1U;
}
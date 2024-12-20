#include <Arduino.h>
#include "qpn.h"    /* QP/C framework API */
#include "bsp.h"  /* Board Support Package interface */

void BSP_init(void) {
    Serial.print("Simple Blinky example\n");
    Serial.print("QP-nano version: ");
    Serial.println(QP_VERSION_STR);
    Serial.println("Press Ctrl-C to quit...");
}

void BSP_ledOff(void) {
    digitalWrite(LED_L, LOW);
    Serial.println("LED OFF");
}

void BSP_ledOn(void) {
    digitalWrite(LED_L, HIGH);
    Serial.println("LED ON");
}

// Interrupt for Timer1 Compare A Match
ISR(TIMER1_COMPA_vect) {
    QF_tickXISR(0);  // Process time events for tick rate 0
}

// QF callbacks
void QF_onStartup(void) {
    // Set Timer1 in CTC mode, 1/1024 prescaler, and start the timer ticking...

    // Configure Timer1 for CTC mode, prescaler 1024
    TCCR1A = 0U;  // Normal port operation (no PWM)
    TCCR1B = (1U << WGM12) | (1U << CS12) | (1U << CS10);  // CTC mode, 1024 prescaler
    TIMSK1 = (1U << OCIE1A);  // Enable TIMER1 compare interrupt
    TCNT1 = 0U;  // Clear the timer counter

    // Set the compare match value for 100 Hz (10 ms interval)
    OCR1A = (F_CPU / BSP_TICKS_PER_SEC / 1024U) - 1U;
}

void QV_onIdle(void) {  // Called with interrupts DISABLED
    // Put the CPU and peripherals to the low-power mode
    SMCR = (0 << SM2) | (0 << SM1) | (0 << SM0) | (1 << SE);  // Idle mode
    QV_CPU_SLEEP();  // Atomically go to sleep and enable interrupts
}

void QF_onCleanup(void) {}

Q_NORETURN Q_onAssert(char const Q_ROM * const module, int location) {
    // Error-handling policy for your application
    (void)module;
    (void)location;
    QF_INT_DISABLE();  // Disable all interrupts
    QF_RESET();  // Reset the CPU
    for (;;) {}
}

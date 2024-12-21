#include <Arduino.h>
#include "qpn.h"        /* QP/C framework API */
#include "bsp.h"        /* Board Support Package interface */
#include "setup.h"

void BSP_init(void) {
    Serial.print("Simple CubeSat example\n");
    Serial.print("QP-nano version: ");
    Serial.println(QP_VERSION_STR);
    Serial.println("Press Ctrl-C to quit...");
    
    timer1_init();
}

void BSP_ledOff(void) {
    digitalWrite(LED_L, LOW);
    // PORTC &= ~(1 << PC7);
}

void BSP_ledOn(void) {
    digitalWrite(LED_L, HIGH);
    // PORTC |= (1 << PC7);
}

// QF callbacks
void QF_onStartup(void) {
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

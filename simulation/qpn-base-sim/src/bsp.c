#include <stdio.h>

#include "qpn.h"        /* QP/C framework API */
#include "../lib/bsp.h"        /* Board Support Package interface */

void BSP_init(void) {
    printf("Simple CubeSat example\n");
    printf("QP-nano version: ");
    printf(QP_VERSION_STR);
    printf("\nPress Ctrl-C to quit... \n");
    
}

void BSP_ledOff(void) {
    printf("LED OFF");
}

void BSP_ledOn(void) {
    printf("LED ON");
}

// QF callbacks
void QF_onStartup(void) {
}

void QV_onIdle(void) {  // Called with interrupts DISABLED

}

void QF_onCleanup(void) {}

Q_NORETURN Q_onAssert(char const Q_ROM * const module, int location) {
    (void)module;
    (void)location;
    for (;;) {}
}

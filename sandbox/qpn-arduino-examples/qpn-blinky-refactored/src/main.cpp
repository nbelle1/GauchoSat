#include <Arduino.h>
#include "qpn.h"    /* QP-nano framework API */
#include "bsp.h"  /* Board Support Package interface */

// Q_DEFINE_THIS_FILE

/* Local-scope objects -----------------------------------------------------*/
static QEvt l_blinkyQSto[10]; /* Event queue storage for Blinky */

/* QF_active[] array defines all active object control blocks --------------*/
QActiveCB const Q_ROM QF_active[] = {
    { (QActive *)0,           (QEvt *)0,        0U                      },
    { (QActive *)&AO_Blinky,  l_blinkyQSto,     Q_DIM(l_blinkyQSto)     }
};

void setup() {
    Serial.begin(9600);
    while (!Serial);  // Wait for serial connection
    Serial.println("QF_INIT");

    // Initialize the QF-nano framework
    QF_init(Q_DIM(QF_active));
    BSP_init();

    Blinky_ctor();  // Initialize Blinky AO
}

void loop() {
  QF_run();  // Run the QF-nano framework
}

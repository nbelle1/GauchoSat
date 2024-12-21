#include <Arduino.h>

#include "config.h"
#include "qpn.h"    /* QP-nano framework API */
#include "bsp.h"  /* Board Support Package interface */

// Q_DEFINE_THIS_FILE

/* Local-scope objects -----------------------------------------------------*/
static QEvt l_CubeSatQSto[10]; /* Event queue storage for CubeSat */

/* QF_active[] array defines all active object control blocks --------------*/
QActiveCB const Q_ROM QF_active[] = {
    { (QActive *)0,           (QEvt *)0,        0U                      },
    { (QActive *)&AO_CubeSat,  l_CubeSatQSto,     Q_DIM(l_CubeSatQSto)     }
};

void setup() {
    Serial.begin(BAUD_RATE);
    while (!Serial);  // Wait for serial connection
    Serial.println("QF_INIT");

    // Initialize the QF-nano framework
    QF_init(Q_DIM(QF_active));
    BSP_init();

    CubeSat_ctor();  // Initialize CubeSat AO
}

void loop() {
  QF_run();  // Run the QF-nano framework
}

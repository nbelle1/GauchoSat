#include <Arduino.h>
#include "qpn.h"    /* QP-nano framework API */
#include "bsp.h"  /* Board Support Package interface */

/* Declare the CubeSat class --------------------------------------*/
typedef struct CubeSat {
    QActive super;
} CubeSat;

static QState CubeSat_initial(CubeSat * const me);
static QState CubeSat_off(CubeSat * const me);
static QState CubeSat_on(CubeSat * const me);

/* The single instance of the CubeSat active object -------------------------*/
CubeSat AO_CubeSat;

/* Define the CubeSat class ---------------------------------------*/
void CubeSat_ctor(void) {
    CubeSat * const me = &AO_CubeSat;
    QActive_ctor(&me->super, Q_STATE_CAST(&CubeSat_initial));
}

static QState CubeSat_initial(CubeSat * const me) {
    QActive_armX(&me->super, 0U, BSP_TICKS_PER_SEC / 2U, BSP_TICKS_PER_SEC / 2U);
    return Q_TRAN(&CubeSat_off);
}

static QState CubeSat_off(CubeSat * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            Serial.println("LED OFF");
            BSP_ledOff();
            status_ = Q_HANDLED();
            break;
        }
        case Q_TIMEOUT_SIG: {
            status_ = Q_TRAN(&CubeSat_on);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

static QState CubeSat_on(CubeSat * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            Serial.println("LED ON");
            BSP_ledOn();
            status_ = Q_HANDLED();
            break;
        }
        case Q_TIMEOUT_SIG: {
            status_ = Q_TRAN(&CubeSat_off);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

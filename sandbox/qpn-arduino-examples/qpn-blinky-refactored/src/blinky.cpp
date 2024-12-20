#include "qpn.h"    /* QP-nano framework API */
#include "bsp.h"  /* Board Support Package interface */

/* Declare the Blinky class --------------------------------------*/
typedef struct Blinky {
    QActive super;
} Blinky;

static QState Blinky_initial(Blinky * const me);
static QState Blinky_off(Blinky * const me);
static QState Blinky_on(Blinky * const me);

/* The single instance of the Blinky active object -------------------------*/
Blinky AO_Blinky;

/* Define the Blinky class ---------------------------------------*/
void Blinky_ctor(void) {
    Blinky * const me = &AO_Blinky;
    QActive_ctor(&me->super, Q_STATE_CAST(&Blinky_initial));
}

static QState Blinky_initial(Blinky * const me) {
    QActive_armX(&me->super, 0U, BSP_TICKS_PER_SEC / 2U, BSP_TICKS_PER_SEC / 2U);
    return Q_TRAN(&Blinky_off);
}

static QState Blinky_off(Blinky * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_ledOff();
            status_ = Q_HANDLED();
            break;
        }
        case Q_TIMEOUT_SIG: {
            status_ = Q_TRAN(&Blinky_on);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

static QState Blinky_on(Blinky * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            BSP_ledOn();
            status_ = Q_HANDLED();
            break;
        }
        case Q_TIMEOUT_SIG: {
            status_ = Q_TRAN(&Blinky_off);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

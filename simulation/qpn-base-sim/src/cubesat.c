#include <stdio.h>


#include "qpn.h"    /* QP-nano framework API */
#include "../lib/bsp.h"  /* Board Support Package interface */

/* Declare the CubeSat class --------------------------------------*/
typedef struct CubeSat {
    QActive super;
} CubeSat;

static QState CubeSat_initial(CubeSat * const me);
static QState CubeSat_launch(CubeSat * const me);

static QState CubeSat_leo(CubeSat * const me);
static QState CubeSat_charge(CubeSat * const me);
static QState CubeSat_active(CubeSat * const me);

/* The single instance of the CubeSat active object -------------------------*/
CubeSat AO_CubeSat;

/* Define the CubeSat class ---------------------------------------*/
void CubeSat_ctor(void) {
    CubeSat * const me = &AO_CubeSat;
    QActive_ctor(&me->super, Q_STATE_CAST(&CubeSat_initial));
}

static QState CubeSat_initial(CubeSat * const me) {
    QActive_armX(&me->super, 0U, BSP_TICKS_PER_SEC / 2U, BSP_TICKS_PER_SEC / 2U);
    return Q_TRAN(&CubeSat_launch);
}

static QState CubeSat_launch(CubeSat * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("Cubesat in Launch State\n");
            /* ALL SYSTEM IDLE/OFF CHECK*/


            status_ = Q_HANDLED();
            break;
        }
        case Q_LEO_SIG: {
            printf("LEO Signal from Launch State\n");
            status_ = Q_TRAN(&CubeSat_leo);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
static QState CubeSat_leo(CubeSat * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("Cubesat in LEO State\n");
            /* CHARGE BATTERIES */
            status_ = Q_TRAN(&CubeSat_charge);
            break;
        }
        case Q_ACTIVE_SIG: {
            printf("Active Signal from LEO State\n");
            status_ = Q_TRAN(&CubeSat_active);
            break;
        }
        case Q_CHARGE_SIG: {
            printf("Charge Signal from LEO State\n");
            status_ = Q_TRAN(&CubeSat_charge);
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

static QState CubeSat_charge(CubeSat * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("Cubesat in Charge State\n");
            /* TURN OFF ALL COMPONENTS/ PUT THEM IN IDLE */

            status_ = Q_HANDLED();
            break;
        }
        case Q_TICK_SIG: {
            printf("Titck Signal from Charge State\n");
            /* CHECK BATTERY POWER PERIODICALLY  */

            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

static QState CubeSat_active(CubeSat * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("Cubesat in Active State\n");

            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
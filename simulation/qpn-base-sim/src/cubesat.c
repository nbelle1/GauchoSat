#include <stdio.h>


#include "qpn.h"    /* QP-nano framework API */
#include "../lib/bsp.h"  /* Board Support Package interface */

/* Define the CubeSat Variables  --------------------------------------*/
float battery_watt_h = 0.0f;
int active = 0;
static void dispatch(QSignal sig);


/* Declare the CubeSat class --------------------------------------*/
typedef struct CubeSat {
    QActive super;
} CubeSat;

static QState CubeSat_initial(CubeSat * const me);
static QState CubeSat_launch(CubeSat * const me);

static QState CubeSat_leo(CubeSat * const me);
static QState CubeSat_deployment(CubeSat * const me);
static QState CubeSat_detumble(CubeSat * const me);
static QState CubeSat_telemetry(CubeSat * const me);

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
            status_ = Q_TRAN(&CubeSat_deployment);
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
            status_ = Q_HANDLED();
            break;
        }
        case Q_CHARGE_SIG: {
            printf("Charge Signal from LEO State\n");
            /*DISABLE THINGS*/

            status_ = Q_HANDLED();
            break;
        }
        case Q_RADIO_SIG: {
            printf("Radio Signal from LEO State\n");
            battery_watt_h -= 1.5;
            status_ = Q_HANDLED();
            break;
        }
        case Q_DEORBIT_SIG: {
            printf("Deorbit Signal from LEO State\n");
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

static QState CubeSat_deployment(CubeSat * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("Cubesat in Deployment State\n");
            status_ = Q_HANDLED();
            break;
        }
        case Q_TICK_SIG: {
            printf("Tick Signal from Deployment State\n");
            battery_watt_h -= .01;

            if ((battery_watt_h < BATTERY_MAX_W * 0.65)) {
                if (battery_watt_h < BATTERY_MAX_W * 0.50) {
                    if (active == 1) { // Reset charge mode
                        printf("Battery below 50%%\n");
                        active = 0; // Disable systems and prepare to charge
                    }
                }
                if (active == 0) { // Start charging if not active
                    dispatch(Q_CHARGE_SIG);
                }
            } else if (battery_watt_h >= BATTERY_MAX_W * 0.65 || active == 1) {
                if (active == 0) { // Systems running mode
                    active = 1;
                }
                dispatch(Q_SYSTEMS_SIG);
                status_ = Q_TRAN(&CubeSat_detumble);
                break;
            } 

            status_ = Q_HANDLED();
            break;
        }
        case Q_SYSTEMS_SIG: {
            printf("Systems Signal from Deployment State\n");
            battery_watt_h -= .5;
            status_ =  Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&CubeSat_leo);
            break;
        }
    }
    return status_;
}

static QState CubeSat_detumble(CubeSat * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("Cubesat in Detumble State\n");
            status_ = Q_HANDLED();
            break;
        }
        case Q_TICK_SIG: {
            printf("Tick Signal from Detumble State\n");
            battery_watt_h -= .01;
            
            if ((battery_watt_h < BATTERY_MAX_W * 0.65)) {
                if (battery_watt_h < BATTERY_MAX_W * 0.50) {
                    if (active == 1) { // Reset charge mode
                        printf("Battery below 50%%\n");
                        active = 0; // Disable systems and prepare to charge
                    }
                }
                if (active == 0) { // Start charging if not active
                    dispatch(Q_CHARGE_SIG);
                }
            
            } else if (battery_watt_h >= BATTERY_MAX_W * 0.65 || active == 1) {
                if (active == 0) { // Systems running mode
                    active = 1;
                }
                dispatch(Q_TELEMETRY_SIG);
                dispatch(Q_RADIO_SIG);
                dispatch(Q_DETUMBLE_SIG);
            }  

            status_ = Q_HANDLED();
            break;
        }
        case Q_TELEMETRY_SIG: {
            printf("Telemetry Signal from Detumble State\n");
            status_ = Q_HANDLED();
            break;
        }
        case Q_DETUMBLE_SIG: {
            printf("Detumble Signal from Detumble State\n");
            battery_watt_h -= 1.3;
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&CubeSat_leo);
            break;
        }
    }
    return status_;
}

static QState CubeSat_telemetry(CubeSat * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            printf("Cubesat in Telemetry State\n");
            status_ = Q_HANDLED();
            break;
        }
        case Q_TICK_SIG: {
            printf("Tick Signal from Telemetry State\n");
            status_ = Q_HANDLED();
            break;
        }
        case Q_TELEMETRY_SIG: {
            printf("Telemetry Signal from Telemetry State\n");
            status_ = Q_HANDLED();
            
        } 
        default: {
            status_ = Q_SUPER(&CubeSat_leo);
            break;
        }
    }
    return status_;
}

static void dispatch(QSignal sig) {
    Q_SIG((QHsm *)&AO_CubeSat) = sig;
    QHsm_dispatch_((QHsm *)&AO_CubeSat);              /* dispatch the event */
}
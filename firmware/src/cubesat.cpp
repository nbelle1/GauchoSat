#include <Arduino.h>
#include "qpn.h"    /* QP-nano framework API */
#include "bsp.h"  /* Board Support Package interface */

/* Define CubeSat Variables & Functions --------------------------------------*/
float battery_watt_h = 0.0f;
int active = 1;
int r_to_transmit = 0;

// static void dispatch(QSignal sig);

/* Declare the CubeSat class --------------------------------------*/
typedef struct CubeSat {
    QActive super;
} CubeSat;

static QState CubeSat_initial(CubeSat * const me);
static QState CubeSat_launch(CubeSat * const me);
static QState CubeSat_charge(CubeSat * const me);
static QState CubeSat_active(CubeSat * const me);
static QState CubeSat_payload(CubeSat * const me);
static QState CubeSat_detumble(CubeSat * const me);
static QState CubeSat_telemetry(CubeSat * const me);
static QState CubeSat_radio(CubeSat * const me);
static QState CubeSat_transmit(CubeSat * const me);
static QState CubeSat_receive(CubeSat * const me);


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
            Serial.print("Entry Signal from Launch State\n");
            /* ALL SYSTEM IDLE/OFF CHECK*/
            QACTIVE_POST_ISR((QActive *)&AO_CubeSat, Q_LEO_SIG, 0U);
            status_ = Q_HANDLED();
            break;
        }
        case Q_LEO_SIG: {
            Serial.print("LEO Signal from Launch State\n");
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
static QState CubeSat_leo(CubeSat * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            Serial.print("Entry Signal from LEO State\n");
            status_ = Q_HANDLED();
            break;
        }
        case Q_BATTERY_SIG: {
            Serial.println(battery_watt_h);
            Serial.print("Battery Signal from LEO State\n");
            battery_watt_h -= .01;

            if (battery_watt_h > BATTERY_MAX_W * 0.5 && active == 0) {
                Serial.print("Battery level high, transitioning to Active State\n");
                active = 1;
                status_ = Q_TRAN(&CubeSat_active);
                break;
            }
            if (battery_watt_h < BATTERY_MAX_W * 0.3 && active == 1) {
                Serial.print("Battery level low, transitioning to Charge State\n");
                active = 0;
                status_ = Q_TRAN(&CubeSat_charge);
                break;
            }
        
            status_ = Q_HANDLED();
            break;
        }
        case Q_DEORBIT_SIG: {
            Serial.print("Deorbit Signal from LEO State\n");
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

static QState CubeSat_charge(CubeSat * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            Serial.print("Entry Signal from Charge State\n");
            Serial.print("TURN OFF/IDLE ALL SYSTEMS\n");
            status_ = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG: {
            Serial.print("Exit Signal from Charge State\n");
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

static QState CubeSat_active(CubeSat * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            Serial.print("Entry Signal from Active State\n");
            status_ = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            Serial.print("Init Signal from Active State\n");
            if (r_to_transmit == 1) {
                status_ = Q_TRAN(&CubeSat_transmit);
            } else {
                status_ = Q_TRAN(&CubeSat_detumble);
            }
            break;
        }
        case Q_EXIT_SIG: {
            Serial.print("Exit Signal from Active State\n");
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

static QState CubeSat_payload(CubeSat * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            Serial.print("Entry Signal from Payload State\n");
            status_ = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG: {
            Serial.print("Exit Signal from Payload State\n");
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&CubeSat_active);
            break;
        }
    }
    return status_;
}

static QState CubeSat_detumble(CubeSat * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            Serial.print("Entry Signal from Detumble State\n");
            Serial.print("TURN ON ADCS\n");
            status_ = Q_HANDLED();
            break;
        }
        case Q_TICK_SIG: {
            Serial.print("Tick Signal from Detumble State\n");
            battery_watt_h -= .15;
            /*WRITE DETUMBLE CODE IN HERE*/
            status_ = Q_TRAN(&CubeSat_telemetry);
            // status_ = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG: {
            Serial.print("Exit Signal from Detumble State\n");
            Serial.print("TURN OFF ADCS\n");
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&CubeSat_payload);
            break;
        }
    }
    return status_;
}

static QState CubeSat_telemetry(CubeSat * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            Serial.print("Entry Signal from Telemetry State\n");
            Serial.print("TURN ON Telemetry\n");
            status_ = Q_HANDLED();
            break;
        }
        case Q_TICK_SIG: {
            battery_watt_h -= .21;
            Serial.print("Tick Signal from Telemetry State\n");
            /*WRITE Telemetry CODE IN HERE*/
            r_to_transmit = 1;
            status_ = Q_TRAN(&CubeSat_active);
            break;
        }
        case Q_EXIT_SIG: {
            Serial.print("Exit Signal from Telemetry State\n");
            Serial.print("TURN OFF Telemetry\n");
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&CubeSat_payload);
            break;
        }
    }
    return status_;
}

static QState CubeSat_radio(CubeSat * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            Serial.print("Entry Signal from Radio State\n");
            Serial.print("TURN ON RADIO\n");
            battery_watt_h -= 1.5;
            status_ = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG: {
            Serial.print("Exit Signal from Radio State\n");
            Serial.print("TURN OFF RADIO\n");
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&CubeSat_active);
            break;
        }
    }
    return status_;
}

static QState CubeSat_transmit(CubeSat * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            Serial.print("Entry Signal from Transmit State\n");
            status_ = Q_HANDLED();
            break;
        }
        case Q_TICK_SIG: {
            Serial.print("Tick Signal from Transmit State\n");
            /*WRITE Telemetry CODE IN HERE*/
            status_ = Q_TRAN(&CubeSat_receive);
            break;
        }
        case Q_EXIT_SIG: {
            Serial.print("Exit Signal in Transmit State\n");
            r_to_transmit = 0;
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&CubeSat_radio);
            break;
        }
    }
    return status_;
}

static QState CubeSat_receive(CubeSat * const me) {
    QState status_;
    switch (Q_SIG(me)) {
        case Q_ENTRY_SIG: {
            Serial.print("Entry Signal from Receive State\n");
            status_ = Q_HANDLED();
            break;
        }
        case Q_TICK_SIG: {
            Serial.print("Tick Signal from Recieve State\n");
            /*WRITE Telemetry CODE IN HERE*/
            status_ = Q_TRAN(&CubeSat_active);
            break;
        }
        case Q_EXIT_SIG: {
            Serial.print("Exit Signal in Transmit State\n");
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&CubeSat_radio);
            break;
        }
    }
    return status_;
}
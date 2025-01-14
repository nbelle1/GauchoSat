#ifndef BSP_H
#define BSP_H


/* a very simple Board Support Package (BSP) -------------------------------*/
enum {
    BSP_TICKS_PER_SEC = 100, // number of system clock ticks in one second
    LED_L = 13               // the pin number of the on-board LED (L)
};
void BSP_init(void);
void BSP_ledOff(void);
void BSP_ledOn(void);

/* define the event signals used in the application ------------------------*/
enum CubeSatSignals {
    DUMMY_SIG = Q_USER_SIG,
    Q_LEO_SIG,
    Q_ACTIVE_SIG,
    Q_CHARGE_SIG,
    Q_TICK_SIG
};

/* active object(s) used in this application -------------------------------*/

#define BATTERY_MAX_W 48            /* 48 Wh 4.5A max*/
#define BATTERY_MAX_A 4.5

extern double current_total_power_min;
extern float battery_watt_h;
extern int MOVE_TIME_F;

extern struct CubeSat AO_CubeSat;   /* opaque struct */
void CubeSat_ctor(void);
#endif /* BSP_H */
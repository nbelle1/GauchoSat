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
enum BlinkySignals {
    DUMMY_SIG = Q_USER_SIG,
    MAX_SIG /* the last signal */
};

/* active object(s) used in this application -------------------------------*/
extern struct Blinky AO_Blinky; /* opaque struct */
void Blinky_ctor(void);

#endif /* BSP_H */
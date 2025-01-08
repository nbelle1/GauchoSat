#include <stdio.h>

#include "config.h"
#include "qpn.h"    /* QP-nano framework API */
#include "../lib/bsp.h"  /* Board Support Package interface */

// Q_DEFINE_THIS_FILE


/* Local-scope objects -----------------------------------------------------*/
static QEvt l_CubeSatQSto[10]; /* Event queue storage for CubeSat */

/* QF_active[] array defines all active object control blocks --------------*/
QActiveCB const Q_ROM QF_active[] = {
    { (QActive *)0,           (QEvt *)0,        0U                      },
    { (QActive *)&AO_CubeSat,  l_CubeSatQSto,     Q_DIM(l_CubeSatQSto)     }
};

/* local objects -----------------------------------------------------------*/
static void dispatch(QSignal sig);
int seed;

#define CALLTIME 20 //200 or 100 or 50 or 20 or 10
#define TOTAL_SIM_TIME 100000
#define TRUE 1
#define FALSE 0

int main() {
    printf("QF_INIT \n");
    // Initialize the QF-nano framework
    QF_init(Q_DIM(QF_active));
    BSP_init();

    CubeSat_ctor();  // Initialize CubeSat AO
    QHsm_init_((QHsm *)&AO_CubeSat);

    dispatch(Q_LEO_SIG);
    dispatch(Q_ACTIVE_SIG);
    // while(1){
    //     QF_run();
    // }

    return 0;
}

// void loop() {
//   QF_run();  // Run the QF-nano framework
// }

static void dispatch(QSignal sig) {
    Q_SIG((QHsm *)&AO_CubeSat) = sig;
    QHsm_dispatch_((QHsm *)&AO_CubeSat);              /* dispatch the event */
}
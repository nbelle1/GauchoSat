#include <stdio.h>
#include <stdlib.h>
#include "time.h"

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
static FILE *l_outFile = (FILE *)0;
static void dispatch(QSignal sig);
static int outf;
int simTime = 0;            /* TIME MINUTES */
int seed;

#define TOTAL_SIM_TIME 1700
#define TRUE 1
#define FALSE 0

void readCSV(const char *filename);
double total_power_min[TOTAL_SIM_TIME];
double current_total_power_min;

int main(int argc, char *argv[]) {
    if (argc > 1) {             /* file name provided? */
        outf = TRUE;            /* write output trace */
        l_outFile = fopen(argv[1], "w");
    } else  				    /* just do the stats */
        outf = FALSE;

    if (outf) fprintf(l_outFile, "QHsmTst example for CubeSat, QP-nano %s\n",
            QP_getVersion());

    readCSV(argv[2]);

    
    // Initialize the QF-nano framework
    printf("QF_INIT \n");
    QF_init(Q_DIM(QF_active));
    BSP_init();

    CubeSat_ctor();  // Initialize CubeSat AO
    QHsm_init_((QHsm *)&AO_CubeSat);

    dispatch(Q_LEO_SIG);
    dispatch(Q_ACTIVE_SIG);

    // while (simTime < TOTAL_SIM_TIME) {
    //     if (outf) fprintf(l_outFile, "total power minute %d:, %lf\n",
    //             simTime + 1, total_power_min[simTime]);

    //     current_total_power_min = total_power_min[simTime];

    //     /* CHECK BATTERY POWER PERIODICALLY  */
    //     if (battery_watt_h <= BATTERY_MAX_W) {
    //         battery_watt_h += current_total_power_min / 60;
    //     }
    //     printf("Total power in battery: %.2f\n", battery_watt_h);  // Debug print

    //     // dispatch(Q_TICK_SIG);
    //     simTime++;

    //     printf("Simulation time: %d minutes\n", simTime);  // Debug print
    // }
    printf("done");
    if (outf) fclose(l_outFile);

    return 0;
}

// void loop() {
//   QF_run();  // Run the QF-nano framework
// }

static void dispatch(QSignal sig) {
    Q_SIG((QHsm *)&AO_CubeSat) = sig;
    QHsm_dispatch_((QHsm *)&AO_CubeSat);              /* dispatch the event */
}

void readCSV(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[1024];
    int lineNumber = 0;

    printf("Reading values from the CSV:\n");
    while (fgets(line, sizeof(line), file)) {
        double value;
        if (sscanf(line, "%lf", &value) == 1) { // Parse the line as a double
            if(lineNumber > TOTAL_SIM_TIME){
                break;
            }
            printf("Line %d: %lf\n", lineNumber, value);
            total_power_min[lineNumber] = value;
            lineNumber++;
        } else {
            fprintf(stderr, "Invalid line format at line %d: %s", ++lineNumber, line);
        }
    }

    fclose(file);
    printf("Finished reading CSV file.\n");
}

#include <system.h>
#include <msp430.h>


void system_init(void){
    if (CLOCK_CALBC1 == 0xFF) LPM4;    // Calibration erased. Sleep forever.

    DCOCTL = 0;
    BCSCTL1 = CLOCK_CALBC1;             // BCSCTL1 calibration
    BCSCTL1 |= XT2OFF;                  // Disable XT2 (on applicable chips)
    BCSCTL2 |= SELS;                    // SMCLK derived from DOC
    BCSCTL2 |= DIVS_2;                  // SMCLK divided by 4
    BCSCTL2 |= SELM_0;                  // MCLK derived from DCO
    BCSCTL2 |= DIVM_0;                  // MCLK divided by 1
    DCOCTL = CLOCK_CALDCO;              // Configure DCO frequency
}

/**
 * @file system.c
 * @author Marcus Behel (mgbehel@ncsu.edu)
 */

#include <system.h>
#include <msp430.h>
#include <g2553support.h>

// Make sure these are set to the same frequency
// Valid frequencies are 1MHz, 8MHz, 12MHz, 16MHz
#define CLOCK_CALBC1      CALBC1_8MHZ
#define CLOCK_CALDCO      CALDCO_8MHZ


void system_init(void){
    if (CLOCK_CALBC1 == 0xFF) LPM4;     // Calibration erased. Sleep forever.

    DCOCTL = 0;
    BCSCTL1 = CLOCK_CALBC1;             // BCSCTL1 calibration
    BCSCTL1 |= XT2OFF;                  // Disable XT2 (on applicable chips)
    BCSCTL2 = 0;                        // Start with known config
    BCSCTL2 |= SELS__DCO;               // SMCLK derived from DOC
    BCSCTL2 |= DIVS__8;                 // SMCLK divided by 8
    BCSCTL2 |= SELM__DCO;               // MCLK derived from DCO
    BCSCTL2 |= DIVM__1;                 // MCLK divided by 1
    DCOCTL = CLOCK_CALDCO;              // Configure DCO frequency
}

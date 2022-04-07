
#include <clocks.h>
#include <msp430.h>

// Make sure these are set to the same frequency
// Valid frequencies are 1MHz, 8MHz, 12MHz, 16MHz
#define CALBC1_USE      CALBC1_16MHZ
#define CALDCO_USE      CALDCO_16MHZ

void clocks_init(void){
    if (CALBC1_USE == 0xFF) while(1);   // Calibration erased. Kill program.

    DCOCTL = 0;
    BCSCTL1 = CALBC1_USE;               // BCSCTL1 calibration
    BCSCTL1 |= XT2OFF;                  // Disable XT2 (on applicable chips)
    BCSCTL2 |= SELS;                    // SMCLK derived from DOC
    BCSCTL2 |= DIVS_2;                  // SMCLK divided by 4
    BCSCTL2 |= SELM_0;                  // MCLK derived from DCO
    BCSCTL2 |= DIVM_0;                  // MCLK divided by 1
    DCOCTL = CALDCO_USE;                // Configure DCO frequency
}

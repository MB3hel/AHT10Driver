
#include <clocks.h>
#include <msp430.h>

void clocks_init(void){
    // TODO: Add calibration constant erase detection

    DCOCTL = 0;
    BCSCTL1 = CALBC1_16MHZ;             // BCSCTL1 calibration for 16MHz DCO
    BCSCTL1 |= XT2OFF;                  // Disable XT2 (on applicable chips)
    BCSCTL2 |= SELS;                    // SMCLK derived from DOC
    BCSCTL2 |= DIVS_2;                  // SMCLK divided by 4
    BCSCTL2 |= SELM_0;                  // MCLK derived from DCO
    BCSCTL2 |= DIVM_0;                  // MCLK divided by 1
    DCOCTL = CALDCO_16MHZ;              // Configure DCO frequency to 16MHz
}

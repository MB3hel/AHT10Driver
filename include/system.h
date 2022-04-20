/**
 * @file system.h
 * @brief System configuration and control functions / macros
 * @author Marcus Behel (mgbehel@ncsu.edu)
 * @version 1.0.0
 */

#pragma once

#include <msp430.h>


////////////////////////////////////////////////////////////////////////////////
/// Macros
////////////////////////////////////////////////////////////////////////////////

#define ENABLE_INTERRUPTS       __bis_SR_register(GIE);
#define DISABLE_INTERRUPTS      __bic_SR_register(GIE);
#define DISABLE_WDT             WDTCTL = WDTPW + WDTHOLD;

// Make sure these are set to the same frequency
// Valid frequencies are 1MHz, 8MHz, 12MHz, 16MHz
#define CLOCK_CALBC1      CALBC1_8MHZ
#define CLOCK_CALDCO      CALDCO_8MHZ


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

/**
 * System level initialization. Initialize / configure things that affect other
 * subsystems such as clocks.
 */
void system_init(void);

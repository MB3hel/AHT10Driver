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


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

/**
 * System level initialization. Initialize / configure things that affect other
 * subsystems such as clocks.
 */
void system_init(void);

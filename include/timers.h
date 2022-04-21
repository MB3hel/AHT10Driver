/**
 * @file timers.h
 * @brief Timer configuration and control
 * @author Marcus Behel (mgbehel@ncsu.edu)
 * @version 1.0.0
 */

#pragma once


////////////////////////////////////////////////////////////////////////////////
/// Macros
////////////////////////////////////////////////////////////////////////////////

#define TA1CCR0_OFFSET      1250    // 125kHz / 1250  = 100Hz int rate (10ms)
#define TA1CCR1_OFFSET      12500   // 125kHz / 12500 = 10Hz int rate (100ms)
#define TA1CCR2_OFFSET      62500   // 125kHz / 62500 = 2Hz int rate (500ms)


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

/**
 * Initialize & configure all timers
 */
void timers_init(void);

/**
 * Delay for the configured time then transition to the next bbi2c state
 */
void timers_bbi2c_delay(void);

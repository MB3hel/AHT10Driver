/**
 * @file timers.h
 * @brief Timer configuration and control
 * @author Marcus Behel (mgbehel@ncsu.edu)
 * @version 1.0.0
 */

#pragma once

#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
/// Globals
////////////////////////////////////////////////////////////////////////////////

// Increment in smallest period timer ISR. Tracks time since start.
// Precision is smallest period (10ms)
extern volatile uint32_t timers_now;

// Counts 250ms interrupts. Used to derive 500ms and 1sec timings.
extern volatile unsigned int timers_250_count;


////////////////////////////////////////////////////////////////////////////////
/// Macros
////////////////////////////////////////////////////////////////////////////////

#define TA1CCR0_OFFSET      2500    // 250kHz / 2500  = 100Hz int rate (10ms)
#define TA1CCR1_OFFSET      25000   // 250kHz / 25000 = 10Hz int rate (100ms)
#define TA1CCR2_OFFSET      62500   // 250kHz / 62500 = 4Hz int rate (250ms)


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

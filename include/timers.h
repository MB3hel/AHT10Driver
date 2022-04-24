/**
 * @file timers.h
 * @brief Timer configuration and control
 * @author Marcus Behel (mgbehel@ncsu.edu)
 * @version 1.0.0
 */

/*
 * MIT License
 *
 * Copyright (c) 2022 Marcus Behel
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
/// Globals
////////////////////////////////////////////////////////////////////////////////

// Increment in smallest period timer ISR. Tracks time since start.
// Precision is smallest period (10ms)
extern volatile uint32_t timers_now;

// Counts 500ms interrupts. Used to derive 1sec timing
extern volatile unsigned int timers_500_count;


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

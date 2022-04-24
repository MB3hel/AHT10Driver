/**
 * @file system.c
 * @author Marcus Behel (mgbehel@ncsu.edu)
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

#include <system.h>
#include <msp430.h>
#include <msp430helper.h>

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


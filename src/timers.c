/**
 * @file timers.c
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

#include <timers.h>
#include <msp430.h>
#include <msp430helper.h>
#include <stdint.h>


////////////////////////////////////////////////////////////////////////////////
/// Globals
////////////////////////////////////////////////////////////////////////////////
volatile uint32_t timers_now = 0;
volatile unsigned int timers_500_count = 0;


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

void timers_init_a0(void){
    TA0CTL = TASSEL__SMCLK;         // Derive timer clock from SMCLK = 1MHz
    TA0CTL |= TACLR;                // Reset timer configuration
    TA0CTL |= MC__CONTINUOUS;       // Timer in continuous mode
    TA0CTL |= ID__1;                // Divide timer clock by 1 = 1MHz

    // Used for bbi2c
    TA0CCTL0 &= ~CCIFG;             // Clear CCR0 IFG
    TA0CCTL0 &= ~CCIE;              // Disable CCR0 interrupt (for now)

    // Unused
    TA0CCTL1 &= ~CCIFG;             // Clear CCR1 IFG
    TA0CCTL1 &= ~CCIE;              // Disable CCR1 interrupt

    // Unused
    TA0CCTL2 &= ~CCIFG;             // Clear CCR2 IFG
    TA0CCTL2 &= ~CCIE;              // Disable CCR2 interrupt

    TA0CTL &= ~TAIFG;               // Clear overflow IFG
    TA0CTL &= ~TAIE;                // Disable overflow interrupt
}

void timers_init_a1(void){
    TA1CTL = TASSEL__SMCLK;         // Derive timer clock from SMCLK = 1MHz
    TA1CTL |= TACLR;                // Reset timer configuration
    TA1CTL |= MC__CONTINUOUS;       // Timer in continuous mode
    TA1CTL |= ID__8;                // Divide timer clock by 8 = 125kHz

    // Used for 10ms timing
    TA1CCTL0 &= ~CCIFG;             // Clear CCR0 IFG
    TA1CCR0 = TA1CCR0_OFFSET;       // Set initial interrupt count
    TA1CCTL0 |= CCIE;               // Enable CCR0 interrupt

    // Used for 100ms timing
    TA1CCTL1 &= ~CCIFG;             // Clear CCR1 IFG
    TA1CCR1 = TA1CCR1_OFFSET;       // Set initial interrupt count
    TA1CCTL1 |= CCIE;               // Enable CCR1 interrupt

    // Used for 500ms timing
    TA1CCTL2 &= ~CCIFG;             // Clear CCR2 IFG
    TA1CCR2 = TA1CCR2_OFFSET;       // Set initial interrupt count
    TA1CCTL2 |= CCIE;               // Enable CCR2 interrupt

    TA1CTL &= ~TAIFG;               // Clear overflow IFG
    TA1CTL &= ~TAIE;                // Disable overflow interrupt
}

void timers_init(void){
    timers_init_a0();
    timers_init_a1();
}

void timers_bbi2c_delay(void){
    // TA0 counts at 1MHz = TimerFreq (see timer steup above)
    // I2CDataRate (up to 100kHz is normal mode)
    // I2CDataRate = TimerFreq / (2 * period)
    // Configured for approx 50kHz
    const uint16_t period = 10;

    TA0CCTL0 &= ~CCIFG;             // Clear CCR0 IFG
    TA0CCR0 = TA0R + period;        // Set time of next interrupt
    TACCTL0 |= CCIE;                // Enable interrupt
}

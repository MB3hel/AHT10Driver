/**
 * @file ports.c
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

#include <ports.h>
#include <msp430.h>


/**
 * Configure all pins on port 1
 */
void ports_init1(void){
    // Start with known initial state
    // All pins GPIO mode, configured as inputs with no resistors or interrupts
    P1SEL  = 0x00;
    P1SEL2 = 0x00;
    P1DIR  = 0x00;
    P1REN  = 0x00;
    P1IE   = 0x00;
    P1IFG  = 0x00;
    P1IES  = 0x00;
    P1OUT  = 0x00;
    ADC10AE0 = 0x00;

    P1SEL &= ~GRN_LED;      // GPIO function
    P1SEL2 &= ~GRN_LED;     // GPIO function
    P1DIR |= GRN_LED;       // Output direction
    P1IE &= ~GRN_LED;       // Disable interrupt
    P1OUT &= ~GRN_LED;      // Initially low

    P1SEL |= UCA0RXD;       // UCA0 function
    P1SEL2 |= UCA0RXD;      // UCA0 function

    P1SEL |= UCA0TXD;       // UCA0 function
    P1SEL2 |= UCA0TXD;      // UCA0 function

    P1SEL &= ~RED_LED;      // GPIO function
    P1SEL2 &= ~RED_LED;     // GPIO function
    P1DIR |= RED_LED;       // Output direction
    P1IE &= ~RED_LED;       // Disable interrupt
    P1OUT &= ~RED_LED;      // Initially low
}

/**
 * Configures all pins on port 2
 */
void ports_init2(void){
    // Start with known initial state
    // All pins GPIO mode, configured as inputs with no resistors or interrupts
    P2SEL  = 0x00;
    P2SEL2 = 0x00;
    P2DIR  = 0x00;
    P2REN  = 0x00;
    P2IE   = 0x00;
    P2IFG  = 0x00;
    P2IES  = 0x00;
    P2OUT  = 0x00;

    P2SEL &= ~SCL;          // GPIO function
    P2SEL2 &= ~SCL;         // GPIO function
    P2DIR |= SCL;           // Output direction (initially)
    P2IE &= ~SCL;           // Disable interrupt
    P2OUT &= ~SCL;          // Output low

    P2SEL &= ~SDA;          // GPIO function
    P2SEL2 &= ~SDA;         // GPIO function
    P2DIR |= SDA;           // Output direction (initially)
    P2IE &= ~SDA;           // Disable interrupt
    P2OUT &= ~SDA;          // Initially low
}

void ports_init(void){
    ports_init1();
    ports_init2();
}

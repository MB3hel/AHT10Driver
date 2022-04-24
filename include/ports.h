/**
 * @file ports.h
 * @brief Ports configuration and control
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

#include <msp430.h>

////////////////////////////////////////////////////////////////////////////////
/// Pin "Name" Macros
////////////////////////////////////////////////////////////////////////////////

// Port 1
#define GRN_LED                 BIT0
#define UCA0RXD                 BIT1
#define UCA0TXD                 BIT2
#define RED_LED                 BIT6

// Port 2
#define SCL                     BIT1
#define SDA                     BIT2


////////////////////////////////////////////////////////////////////////////////
/// Function-like macros for external use
////////////////////////////////////////////////////////////////////////////////

#define GRN_LED_ON              P1OUT |= GRN_LED
#define GRN_LED_OFF             P1OUT &= ~GRN_LED
#define GRN_LED_TOGGLE          P1OUT ^= GRN_LED

#define RED_LED_ON              P1OUT |= RED_LED
#define RED_LED_OFF             P1OUT &= ~RED_LED
#define RED_LED_TOGGLE          P1OUT ^= RED_LED

// PxOUT is configured so pin is low in output mode
// In input mode, external (I2C) pullups take over making the line high
// Changing pin direction changes high vs low
#define PORTS_SDA_LOW           P2DIR |= SDA
#define PORTS_SDA_HIGH          P2DIR &= ~SDA
#define PORTS_SDA_READ          (P2IN & SDA)
#define PORTS_SCL_LOW           P2DIR |= SCL
#define PORTS_SCL_HIGH          P2DIR &= ~SCL
#define PORTS_SCL_READ          (P2IN & SCL)


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

/**
 * Initialize (configure) ports. Configures all pins on all ports
 */
void ports_init(void);

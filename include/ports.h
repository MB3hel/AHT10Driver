/**
 * @file ports.h
 * @brief Ports configuration and control
 * @author Marcus Behel (mgbehel@ncsu.edu)
 * @version 1.0.0
 */

#pragma once

#include <msp430.h>

////////////////////////////////////////////////////////////////////////////////
/// Pin "Name" Macros
////////////////////////////////////////////////////////////////////////////////

// Port 1
#define GRN_LED                 BIT0
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

#define PORTS_SDA_LOW           P2OUT &= ~SDA
#define PORTS_SDA_HIGH          P2OUT |= SDA
#define PORTS_SDA_READ          P2IN & SDA

#define PORTS_SCL_LOW           P2OUT &= ~SCL
#define PORTS_SCL_HIGH          P2OUT |= SCL
#define PORTS_SCL_READ          P2OUT & SCL


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

/**
 * Initialize (configure) ports. Configures all pins on all ports
 */
void ports_init(void);

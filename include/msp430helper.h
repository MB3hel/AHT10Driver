/**
 * @file g2553support.h
 * @brief Macro definitions to help with code readability and helper functions
 * to perform common operations
 * @author Marcus Behel (mgbehel@ncsu.edu)
 * @version 1.0.0
 */

#pragma once

#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
/// Helper functions
////////////////////////////////////////////////////////////////////////////////

/**
 * Integer to string representation
 * @param value Value to convert to string
 * @param data String to write into. MUST BE AT LEAST 12 CHARS WIDE
 */
unsigned int int_to_str(int32_t value, char *data);

/**
 * Unsigned divide by 10 using bitwise operations. Based on method from
 * http://web.archive.org/web/20180517023231/http://www.hackersdelight.org/divcMore.pdf
 * Modifies provided number and returns remainder
 * @param n Number to divide by 10
 * @param q Quotient
 * @param r Remainder
 */
void udiv10(uint32_t n, uint32_t *q, uint32_t *r);


////////////////////////////////////////////////////////////////////////////////
/// Macros
////////////////////////////////////////////////////////////////////////////////

#if defined (__MSP430G2553__)

////////////////////////////////////////////////////////////////////////////////
/// More readable names for clock settings.
////////////////////////////////////////////////////////////////////////////////

// MCLK Source settings
#define SELM__DCO               SELM_0
#define SELM__XT2LXFT           SELM_2
#define SELM__LFXT              SELM_3

// MCLK Divider settings
#define DIVM__1                 DIVM_0
#define DIVM__2                 DIVM_1
#define DIVM__4                 DIVM_2
#define DIVM__8                 DIVM_3

// SMCLK Source settings
#define SELS__DCO               0x00
#define SELS__XT2LXFT           SELS

// SMCLK Divider settings
#define DIVS__1                 DIVS_0
#define DIVS__2                 DIVS_1
#define DIVS__4                 DIVS_2
#define DIVS__8                 DIVS_3

////////////////////////////////////////////////////////////////////////////////
/// More readable names for the timer settings. Some newer MSP430's headers
/// include macros defined this way by default, but older ones do not always.
/// The specific meaning of each numbered source is determined from datasheet
////////////////////////////////////////////////////////////////////////////////

// Timer clock sources
#define TASSEL__TACLK           TASSEL_0
#define TASSEL__ACLK            TASSEL_1
#define TASSEL__SMCLK           TASSEL_2
#define TASSEL__INCLK           TASSEL_3

// Timer modes
#define MC__STOP                MC_0
#define MC__UP                  MC_1
#define MC__CONTINUOUS          MC_2
#define MC__UPDOWN              MC_3

// Input divider settings
#define ID__1                   ID_0
#define ID__2                   ID_1
#define ID__4                   ID_2
#define ID__8                   ID_3

// Bits in the interrupt vector
#define TAIV__NONE              0x00
#define TAIV__TACCR1            0x02
#define TAIV__TACCR2            0x04
#define TAIV__TAIFG             0x0A


////////////////////////////////////////////////////////////////////////////////
/// Redefining the names of timer interrupts
/// In the block for the G2553 as the number and type of timers available is
/// chip dependent.
/// This redefinition is intended to improve readability of the code
/// as the name of interrupt vectors for timers is often confusing
////////////////////////////////////////////////////////////////////////////////

// Timer A0
#define TIMER_A0_CCR0_VECTOR    TIMER0_A0_VECTOR    // For CCR0
#define TIMER_A0_CCRN_VECTOR    TIMER0_A1_VECTOR    // For CCR1, 2, ..., and OF

// Timer A1
#define TIMER_A1_CCR0_VECTOR    TIMER1_A0_VECTOR    // For CCR0
#define TIMER_A1_CCRN_VECTOR    TIMER1_A1_VECTOR    // For CCR1, 2, ..., and OF

#endif

/**
 * @file g2553support.h
 * @brief Macro definitions to help with code readability for the MSP430G2553
 * @author Marcus Behel (mgbehel@ncsu.edu)
 * @version 1.0.0
 */

#pragma once

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

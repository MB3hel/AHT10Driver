
#ifndef INCLUDE_G2553SUPPORT_H_
#define INCLUDE_G2553SUPPORT_H_


#if defined (__MSP430G2553__)


////////////////////////////////////////////////////////////////////////////////
/// More readable names for the timer settings. Some newer MSP430's headers
/// include macros defined this way by default, but older ones do not always.
/// The specific meaning of each numbered source is determined from the datasheet
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
#define MC_UPDOWN               MC_3

// Input divider settings
#define ID__1                   ID_0
#define ID__2                   ID_1
#define ID__4                   ID_2
#define ID__8                   ID_8

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
#define TIMER_A0_CCRn_VECTOR    TIMER0_A1_VECTOR    // For CCR1, 2, ..., and OF

// Timer A1
#define TIMER_A1_CCR0_VECTOR    TIMER1_A0_VECTOR    // For CCR0
#define TIMER_A1_CCRn_VECTOR    TIMER1_A1_VECTOR    // For CCR1, 2, ..., and OF

#endif


#endif /* INCLUDE_G2553SUPPORT_H_ */

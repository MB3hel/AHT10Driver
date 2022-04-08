
#include <timers.h>
#include <msp430.h>
#include <stdint.h>
#include <g2553support.h>


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

void timers_init_a0(void){
    TA0CTL = TASSEL__SMCLK;         // Derive timer clock from SMCLK = 4MHz
    TA0CTL |= TACLR;                // Reset timer configuration
    TA0CTL |= MC__CONTINUOUS;       // Timer in continuous mode
    TA0CTL |= ID__4;                // Divide timer clock by 4 = 1MHz

    // CCR0 used for BBI2C timings
    TA0CCTL0 &= ~CCIFG;             // Clear CCR0 IFG
    TA0CCTL0 &= ~CCIE;              // Disable CCR0 interrupt

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
    TA1CTL = TASSEL__SMCLK;         // Derive timer clock from SMCLK = 4MHz
    TA1CTL |= TACLR;                // Reset timer configuration
    TA1CTL |= MC__CONTINUOUS;       // Timer in continuous mode
    TA1CTL |= ID__4;                // Divide timer clock by 4 = 1MHz

    // Unused
    TA1CCTL0 &= ~CCIFG;             // Clear CCR0 IFG
    TA1CCTL0 &= ~CCIE;              // Disable CCR0 interrupt

    // Unused
    TA1CCTL1 &= ~CCIFG;             // Clear CCR1 IFG
    TA1CCTL1 &= ~CCIE;              // Disable CCR1 interrupt

    // Unused
    TA1CCTL2 &= ~CCIFG;             // Clear CCR2 IFG
    TA1CCTL2 &= ~CCIE;              // Disable CCR2 interrupt

    TA1CTL &= ~TAIFG;               // Clear overflow IFG
    TA1CTL &= ~TAIE;                // Disable overflow interrupt
}

void timers_init(void){
    timers_init_a0();
    timers_init_a1();
}

inline void timers_bbi2c_delay(void){
    // TA0 counts at 1MHz = TimerFreq
    // I2CDataRate (100kHz to 800kHz generally)
    // I2CDataRate = TimerFreq / (2 * period)
    // Configured for 100kHz
    const uint16_t period = 5;

    TA0CCTL0 &= ~CCIFG;             // Clear CCR0 IFG
    TA0CCR0 = TA0R + period;        // Set time of next interrupt
    TA0CCTL0 |= CCIE;               // Enable CCR0 interrupt

    // Wait until interrupt flag set to continue
    while(!(TA0CCTL0 & CCIFG));
}


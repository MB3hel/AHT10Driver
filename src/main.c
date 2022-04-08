
#include <msp430.h>
#include <g2553support.h>

#include <stdint.h>
#include <stdbool.h>

#include <system.h>
#include <ports.h>
#include <timers.h>
#include <bbi2c.h>

// TODO: Blink LEDs in a pattern to demo timing using flags from ISR
// TODO: UART comm with PC for temperature values

////////////////////////////////////////////////////////////////////////////////
/// Program Entry Point
////////////////////////////////////////////////////////////////////////////////

int main(void){
    DISABLE_WDT;                // Disable watchdog timer
    ENABLE_INTERRUPTS;          // Enable interrupts (global)
    system_init();              // System initialization (clocks)
    ports_init();               // Ports initialization & config
    timers_init();              // Timer initialization
    // bbi2c_init();            // Initialize SW I2C subsystem
    // aht10_init();            // Initialize & configure AHT10 sensor

    while(true){
        LPM0;                   // Enter low power mode 0
    }
}


////////////////////////////////////////////////////////////////////////////////
/// Interrupt Service Routines
////////////////////////////////////////////////////////////////////////////////

#pragma vector=TIMER_A0_CCR0_VECTOR
__interrupt void isr_timera0_ccr0(void){
    // CCR0: bbi2c timing
    // Nothing to do in ISR
}

#pragma vector=TIMER_A0_CCRN_VECTOR
__interrupt void isr_timera0_ccrn(void){
    switch(__even_in_range(TA0IV, TAIV__TAIFG)){
       case TAIV__NONE:                 // No interrupt
           break;
       case TAIV__TACCR1:               // CCR1
           break;
       case TAIV__TACCR2:               // CCR2
           break;
       case TAIV__TAIFG:                // Overflow
           break;
    }
}

#pragma vector=TIMER_A1_CCR0_VECTOR
__interrupt void isr_timera1_ccr0(void){
    // CCR0
}

#pragma vector=TIMER_A1_CCRN_VECTOR
__interrupt void isr_timera1_ccrn(void){
    switch(__even_in_range(TA1IV, TAIV__TAIFG)){
       case TAIV__NONE:                 // No interrupt
           break;
       case TAIV__TACCR1:               // CCR1
           break;
       case TAIV__TACCR2:               // CCR2
           break;
       case TAIV__TAIFG:                // Overflow
           break;
    }
}


#include <msp430.h>
#include <g2553support.h>

#include <stdint.h>
#include <stdbool.h>

#include <clocks.h>
#include <ports.h>


////////////////////////////////////////////////////////////////////////////////
/// Program Entry Point
////////////////////////////////////////////////////////////////////////////////

int main(void){
    clocks_init();
    ports_init();
    timers_init();

    while(true){

    }
}


////////////////////////////////////////////////////////////////////////////////
/// Interrupt Service Routines
////////////////////////////////////////////////////////////////////////////////

#pragma vector=TIMER_A0_CCR0_VECTOR
__interrupt void isr_timera0_ccr0(void){

}

#pragma vector=TIMER_A0_CCRn_VECTOR
__interrupt void isr_timera0_ccrn(void){

}

#pragma vector=TIMER_A1_CCR0_VECTOR
__interrupt void isr_timera1_ccr0(void){

}

#pragma vector=TIMER_A1_CCRn_VECTOR
__interrupt void isr_timera1_ccrn(void){

}

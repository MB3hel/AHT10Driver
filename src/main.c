/**
 * @file main.c
 * @brief Program entry point, main tree, and ISRs
 * @author Marcus Behel (mgbehel@ncsu.edu)
 */

#include <msp430.h>
#include <g2553support.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <system.h>
#include <ports.h>
#include <timers.h>
#include <bbi2c.h>


////////////////////////////////////////////////////////////////////////////////
/// Other globals
////////////////////////////////////////////////////////////////////////////////
volatile unsigned int counter_500ms = 0;


////////////////////////////////////////////////////////////////////////////////
/// Flags from interrupt service routines
////////////////////////////////////////////////////////////////////////////////

// Can make this wider if necessary
volatile uint8_t flags = 0;

#define TIMING_10MS         BIT0
#define TIMING_100MS        BIT1
#define TIMING_500MS        BIT2
#define TIMING_1S           BIT3
#define I2C_DONE            BIT4

#define SET_FLAG(x)         flags |= x
#define CHECK_FLAG(x)       (flags & x)
#define CLEAR_FLAG(x)       flags &= ~x


////////////////////////////////////////////////////////////////////////////////
/// Program Entry Point
////////////////////////////////////////////////////////////////////////////////

void i2c_callback(bbi2c_transaction *trans, bool success){
    SET_FLAG(I2C_DONE);
}

int main(void){
    DISABLE_WDT;                        // Disable watchdog timer
    ENABLE_INTERRUPTS;                  // Enable interrupts (global)
    system_init();                      // System initialization (clocks)
    ports_init();                       // Ports initialization & config
    timers_init();                      // Timer initialization
    bbi2c_init(&i2c_callback);          // Initialize SW I2C

    __delay_cycles(8e6);


    GRN_LED_ON;
    RED_LED_OFF;

    uint8_t aht10_command[3] = {0xE1, 0x08, 0x00};
    bbi2c_transaction i2c_trans;

    i2c_trans.address = 0x38;
    i2c_trans.write_buf = aht10_command;
    i2c_trans.write_count = 3;
    i2c_trans.read_count = 0;
    i2c_trans.repeated_start = false;

    while(true){
        bbi2c_perform(&i2c_trans);
        while(!CHECK_FLAG(I2C_DONE));
        __delay_cycles(1e6);
    }

    /*while(true){
        if(CHECK_FLAG(TIMING_10MS)){
            CLEAR_FLAG(TIMING_10MS);
            // -----------------------------------------------------------------
            // Run every 10ms
            // -----------------------------------------------------------------

            // -----------------------------------------------------------------
        }else if(CHECK_FLAG(TIMING_100MS)){
            CLEAR_FLAG(TIMING_100MS);
            // -----------------------------------------------------------------
            // Run every 100ms
            // -----------------------------------------------------------------

            // -----------------------------------------------------------------
        }else if(CHECK_FLAG(TIMING_500MS)){
            CLEAR_FLAG(TIMING_500MS);
            // -----------------------------------------------------------------
            // Run every 500ms
            // -----------------------------------------------------------------
            GRN_LED_TOGGLE;
            // -----------------------------------------------------------------
        }else if(CHECK_FLAG(TIMING_1S)){
            CLEAR_FLAG(TIMING_1S);
            // -----------------------------------------------------------------
            // Run every 1sec
            // -----------------------------------------------------------------
            RED_LED_TOGGLE;
            // -----------------------------------------------------------------
        }else{
            // No flags set. Enter LPM0. Interrupts will exit LPM0 when flag set
            LPM0;
        }
    }*/
}


////////////////////////////////////////////////////////////////////////////////
/// Interrupt Service Routines
////////////////////////////////////////////////////////////////////////////////

// Timer A0

#pragma vector=TIMER_A0_CCR0_VECTOR
__interrupt void isr_timera0_ccr0(void){
    // CCR0: bbi2c timing
    TA0CCTL0 &= ~CCIE;                  // Disable interrupt
    bbi2c_next();                       // Move to next state
}

#pragma vector=TIMER_A0_CCRN_VECTOR
__interrupt void isr_timera0_ccrn(void){
    switch(__even_in_range(TA0IV, TAIV__TAIFG)){
       case TAIV__NONE:                 // No interrupt
           break;
       case TAIV__TACCR1:               // CCR1: Unused
           break;
       case TAIV__TACCR2:               // CCR2: Unused
           break;
       case TAIV__TAIFG:                // Overflow
           break;
    }
}


// Timer A1

#pragma vector=TIMER_A1_CCR0_VECTOR
__interrupt void isr_timera1_ccr0(void){
    // TA1 CCR0: 10ms timing
    SET_FLAG(TIMING_10MS);              // Set correct flag
    LPM0_EXIT;                          // Flag needs handling; exit LPM0
}

#pragma vector=TIMER_A1_CCRN_VECTOR
__interrupt void isr_timera1_ccrn(void){
    switch(__even_in_range(TA1IV, TAIV__TAIFG)){
       case TAIV__NONE:                 // No interrupt
           break;
       case TAIV__TACCR1:               // CCR1: 100ms timing
           SET_FLAG(TIMING_100MS);      // Set correct flag
           LPM0_EXIT;                   // Flag needs handling; exit LPM0
           break;
       case TAIV__TACCR2:               // CCR2: 500ms timing
           SET_FLAG(TIMING_500MS);      // Set correct flag
           counter_500ms++;             // Increment counter (used for 1s)
           if(counter_500ms == 2){
               SET_FLAG(TIMING_1S);     // Set correct flag
               counter_500ms = 0;       // Reset counter
           }
           LPM0_EXIT;                   // Flag needs handling; exit LPM0
           break;
       case TAIV__TAIFG:                // Overflow
           break;
    }
}

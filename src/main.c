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
#include <aht10.h>
#include <pc_comm.h>


////////////////////////////////////////////////////////////////////////////////
/// Flags from interrupt service routines
////////////////////////////////////////////////////////////////////////////////

// Can make this wider if necessary
volatile uint8_t flags = 0;

#define TIMING_10MS         BIT0        // Set every 10ms
#define TIMING_100MS        BIT1        // Set every 100ms
#define TIMING_250MS        BIT2        // Set every 250ms
#define TIMING_500MS        BIT3        // Set every 500ms
#define TIMING_1S           BIT4        // Set every 1s
#define AHT10_DONE          BIT5        // AHT10 I2C done (success or fail)
#define AHT10_FAIL          BIT6        // AHT10 I2C failed

#define SET_FLAG(x)         flags |= (x)
#define CHECK_FLAG(x)       (flags & x)
#define CLEAR_FLAG(x)       flags &= ~(x)

////////////////////////////////////////////////////////////////////////////////
/// Program Entry Point
////////////////////////////////////////////////////////////////////////////////

int main(void){
    // -------------------------------------------------------------------------
    // Initialization
    // -------------------------------------------------------------------------

    DISABLE_WDT;                        // Disable watchdog timer
    ENABLE_INTERRUPTS;                  // Enable interrupts (global)
    system_init();                      // System initialization (clocks)
    ports_init();                       // Ports initialization & config
    timers_init();                      // Timer initialization
    bbi2c_init();                       // Initialize SW I2C
    aht10_init();                       // Initialize AHT10 state machine

    // -------------------------------------------------------------------------
    // Setup initial state
    // -------------------------------------------------------------------------

    GRN_LED_OFF;
    RED_LED_OFF;


    // -------------------------------------------------------------------------
    // Main loop
    // -------------------------------------------------------------------------

    while(true){
        if(CHECK_FLAG(TIMING_10MS)){
            CLEAR_FLAG(TIMING_10MS);
            // -----------------------------------------------------------------
            // Run every 10ms
            // -----------------------------------------------------------------
            // Nothing here
            // -----------------------------------------------------------------
        }else if(CHECK_FLAG(TIMING_100MS)){
            CLEAR_FLAG(TIMING_100MS);
            // -----------------------------------------------------------------
            // Run every 100ms
            // -----------------------------------------------------------------
            // Attempt to read AHT10 every 500ms
            if(timers_now - aht10_last_read > 500){
                aht10_read();           // Request new sensor data
            }
            // -----------------------------------------------------------------
        }else if(CHECK_FLAG(TIMING_250MS)){
            CLEAR_FLAG(TIMING_250MS);
            // -----------------------------------------------------------------
            // Run every 250ms
            // -----------------------------------------------------------------
            // Nothing here
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
        }else if(CHECK_FLAG(AHT10_DONE)){
            CLEAR_FLAG(AHT10_DONE);
            if(aht10_i2c_done(!CHECK_FLAG(AHT10_FAIL))){
                // Function returned true. New data available
                __no_operation();
            }
            CLEAR_FLAG(AHT10_FAIL);
        }else{
            // No flags set. Enter LPM0. Interrupts will exit LPM0 when flag set
            LPM0;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
/// Interrupt Service Routines
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// Timer A0
// -----------------------------------------------------------------------------

#pragma vector=TIMER_A0_CCR0_VECTOR
__interrupt void isr_timera0_ccr0(void){
    // CCR0: bbi2c timing
    TA0CCTL0 &= ~CCIE;                  // Disable interrupt
    unsigned int res = bbi2c_next();    // Move to next state

    if(res == BBI2C_DONE && bbi2c_trans == &aht10_trans){
        SET_FLAG(AHT10_DONE);
        LPM0_EXIT;
    }else if(res == BBI2C_FAIL && bbi2c_trans == &aht10_trans){
        SET_FLAG(AHT10_DONE | AHT10_FAIL);
        LPM0_EXIT;
    }
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


// -----------------------------------------------------------------------------
// Timer A1
// -----------------------------------------------------------------------------

#pragma vector=TIMER_A1_CCR0_VECTOR
__interrupt void isr_timera1_ccr0(void){
    // TA1 CCR0: 10ms timing
    TA1CCR0 += TA1CCR0_OFFSET;          // Configure to interrupt in 10ms
    timers_now += 10;                   // Increment current time counter
    SET_FLAG(TIMING_10MS);              // Set correct flag
    LPM0_EXIT;                          // Flag needs handling; exit LPM0
}

#pragma vector=TIMER_A1_CCRN_VECTOR
__interrupt void isr_timera1_ccrn(void){
    switch(__even_in_range(TA1IV, TAIV__TAIFG)){
       case TAIV__NONE:                 // No interrupt
           break;
       case TAIV__TACCR1:               // CCR1: 100ms timing
           TA1CCR1 += TA1CCR1_OFFSET;   // Configure to interrupt in 100ms
           SET_FLAG(TIMING_100MS);      // Set correct flag
           LPM0_EXIT;                   // Flag needs handling; exit LPM0
           break;
       case TAIV__TACCR2:               // CCR2: 250ms timing
           TA1CCR2 += TA1CCR2_OFFSET;   // Configure to interrupt in 250ms
           SET_FLAG(TIMING_250MS);      // Set correct flag
           timers_250_count++;          // Increment counter (used for 1s)
           if(timers_250_count == 2){
               SET_FLAG(TIMING_500MS);  // Set correct flags
           }
           if(timers_250_count == 4){
               SET_FLAG(TIMING_1S);     // Set correct flags
               SET_FLAG(TIMING_500MS);  // Set correct flags
               timers_250_count = 0;    // Reset counter
           }
           LPM0_EXIT;                   // Flag needs handling; exit LPM0
           break;
       case TAIV__TAIFG:                // Overflow
           break;
    }
}

// -----------------------------------------------------------------------------
// USCI0 (ISRs shared for A0 and B0)
// -----------------------------------------------------------------------------

#pragma vector=USCIAB0RX_VECTOR
__interrupt void usci0_rx_isr(void){
    if(IFG2 & UCA0RXIFG){
        IFG2 &= !UCA0RXIFG;             // Clear RX flag for UCA0
        pc_comm_handle_read();          // Handle pc_comm receive
    }
}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void usci0_tx_isr(void){
    if(IFG2 & UCA0TXIFG){
        IFG2 &= ~UCA0TXIFG;             // Clear TX flag for UCA0
        pc_comm_handle_write();         // Handle pc_comm transmit
    }
}


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
uint32_t i2c_idle_time = 0;
bool i2c_should_send = false;


////////////////////////////////////////////////////////////////////////////////
/// Flags from interrupt service routines
////////////////////////////////////////////////////////////////////////////////

// Can make this wider if necessary
volatile uint8_t flags = 0;

#define TIMING_10MS         BIT0
#define TIMING_100MS        BIT1
#define TIMING_250MS        BIT2
#define TIMING_500MS        BIT3
#define TIMING_1S           BIT4
#define I2C_DONE            BIT5
#define I2C_FAIL            BIT6
#define TIMING_CUSTOM       BIT7

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

    uint8_t aht10_command[3] = {0xE1, 0x08, 0x00};
    bbi2c_transaction i2c_trans;
    uint8_t read_buf[1];

    i2c_trans.address = 0x38;
    i2c_trans.write_count = 0;
    i2c_trans.read_buf = read_buf;
    i2c_trans.read_count = 1;


    // -------------------------------------------------------------------------
    // Setup initial state
    // -------------------------------------------------------------------------

    GRN_LED_OFF;
    RED_LED_OFF;
    bbi2c_perform(&i2c_trans);


    GRN_LED_ON;

    while(true){
        // bbi2c_perform(&i2c_trans);
        while(!CHECK_FLAG(I2C_DONE));
        CLEAR_FLAG(I2C_DONE);
        __delay_cycles(1e6);
    }

    // -------------------------------------------------------------------------
    // Main loop
    // -------------------------------------------------------------------------

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
            if(i2c_should_send && (timers_now - i2c_idle_time >= 1000)){
                i2c_should_send = false;
                bbi2c_perform(&i2c_trans);
            }
            // -----------------------------------------------------------------
        }else if(CHECK_FLAG(TIMING_250MS)){
            CLEAR_FLAG(TIMING_250MS);
            // -----------------------------------------------------------------
            // Run every 250ms
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
            // RED_LED_TOGGLE;
            // -----------------------------------------------------------------
        }else if(CHECK_FLAG(I2C_DONE)){
            // -----------------------------------------------------------------
            // Run when BBI2C done with a transaction
            // -----------------------------------------------------------------
            CLEAR_FLAG(I2C_DONE);
            if(CHECK_FLAG(I2C_FAIL)){
                CLEAR_FLAG(I2C_FAIL);
                RED_LED_ON;
            }else{
                RED_LED_OFF;
            }
            i2c_idle_time = timers_now;
            i2c_should_send = true;
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

// -----------------------------------------------------------------------------
// Timer A0
// -----------------------------------------------------------------------------

#pragma vector=TIMER_A0_CCR0_VECTOR
__interrupt void isr_timera0_ccr0(void){
    // CCR0: bbi2c timing
    TA0CCTL0 &= ~CCIE;                  // Disable interrupt
    unsigned int res = bbi2c_next();    // Move to next state

    if(res == BBI2C_DONE){
        SET_FLAG(I2C_DONE);             // Transaction done. Set flag.
        LPM0_EXIT;                      // Flag needs handling; exit LPM0
    }else if(res == BBI2C_FAIL){
        SET_FLAG(I2C_DONE);             // Transaction done. Set flag.
        SET_FLAG(I2C_FAIL);             // Transaction failed. Set flag.
        LPM0_EXIT;                      // Flags need handling; exit LPM0
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

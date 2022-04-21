/**
 * @file bbi2c.c
 * @author Marcus Behel (mgbehel@ncsu.edu)
 */

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <timers.h>
#include <ports.h>


////////////////////////////////////////////////////////////////////////////////
/// Macros
////////////////////////////////////////////////////////////////////////////////

#define OPERATION_IDLE      0
#define OPERATION_START     1
#define OPERATION_WRITE     2
#define OPERATION_READ      3
#define OPERATION_STOP      4


////////////////////////////////////////////////////////////////////////////////
/// Globals
////////////////////////////////////////////////////////////////////////////////

unsigned int bbi2c_operation = OPERATION_IDLE;
unsigned int bbi2c_state = 0;
uint8_t bbi2c_address;
uint8_t bbi2c_tx_buf;


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

/**
 * Send start condition
 */
void bbi2c_start(void){
    bbi2c_state = 0;
    bbi2c_operation = OPERATION_START;
    // TODO: bbi2c_next();
}

// TODO: Other similar functions to above


/**
 * Start condition state machine
 */
void bbi2c_start_next(void){
    uint8_t addr = (bbi2c_address << 1) | BIT0;
    uint8_t bits = 8;
    switch(bbi2c_state){
    case 0:
        // Start condition
        PORTS_SDA_LOW;
        PORTS_SCL_LOW;
        bbi2c_state = 1;
        break;
    case 1:
        // Transmit next bit
        if(addr & BIT7)
            PORTS_SDA_HIGH;
        else
            PORTS_SDA_LOW;
        PORTS_SCL_HIGH;
        bbi2c_state = 2;
        break;
    case 2:
        // Increment to next bit
        addr <<= 1;
        bits--;
        PORTS_SCL_LOW;
        if(bits > 0)
            bbi2c_state = 1;
        else
            bbi2c_state = 3;
        break;
    case 3:
        // Start done
        bbi2c_operation = OPERATION_IDLE;
        break;
    }
}

/**
 * Stop condition state machine
 */
void bbi2c_stop_next(void){
    switch(bbi2c_state){
    case 0:
        // Make sure SCL low before SDA transitions to low
        PORTS_SCL_LOW;
        bbi2c_state = 1;
        break;
    case 1:
        // Transition SDA to low
        PORTS_SDA_LOW;
        bbi2c_state = 2;
        break;
    case 2:
        // Raise  SDA while SCL high = stop condition
        PORTS_SCL_HIGH;
        PORTS_SDA_HIGH;
        bbi2c_operation = OPERATION_IDLE;
        break;
    }
}

/**
 * Transmit byte state machine
 */
void bbi2c_tx_byte_next(void){
    uint8_t bits = 8;
    // TODO
}

/**
 * @file bbi2c.c
 * @author Marcus Behel (mgbehel@ncsu.edu)
 */

#include <bbi2c.h>
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
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

#define STATUS_FAIL         0
#define STATUS_BUSY         1
#define STATUS_DONE         2


////////////////////////////////////////////////////////////////////////////////
/// Globals
////////////////////////////////////////////////////////////////////////////////

void (*bbi2c_callback)(void) = NULL;
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
void bbi2c_start(uint8_t address){
    bbi2c_address = address;
    bbi2c_state = 0;
    bbi2c_operation = OPERATION_START;
    bbi2c_next();
}

void bbi2c_write_byte(uint8_t data){
    bbi2c_tx_buf = data;
    bbi2c_state = 0;
    bbi2c_operation = OPERATION_WRITE;
    bbi2c_next();
}

/**
 * Send stop condition
 */
void bbi2c_stop(void){
    bbi2c_state = 0;
    bbi2c_operation = OPERATION_STOP;
    bbi2c_next();
}


/**
 * Start condition state machine
 * @return STATUS_DONE, STATUS_BUSY, STATUS_FAIL
 */
unsigned int bbi2c_start_next(void){
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
        // Pull both lines high and wait for ACK
        PORTS_SDA_HIGH;
        PORTS_SCL_HIGH;
        while(!PORTS_SCL_READ);     // Wait for potential clock stretching
        bbi2c_state = 4;
        break;
    case 4:
        if(PORTS_SDA_READ){
            // Still high = NACK
            return STATUS_FAIL;
        }
        PORTS_SCL_LOW;
        bbi2c_state = 5;
        break;
    case 5:
        return STATUS_DONE;
    }
    return STATUS_BUSY;
}

/**
 * Stop condition state machine
 * @return STATUS_DONE, STATUS_BUSY, STATUS_FAIL
 */
unsigned int bbi2c_stop_next(void){
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
        return STATUS_DONE;
    }
    return STATUS_BUSY;
}

/**
 * Transmit byte state machine
 * @return STATUS_DONE, STATUS_BUSY, STATUS_FAIL
 */
unsigned int bbi2c_tx_byte_next(void){
    uint8_t bits = 8;
    switch(bbi2c_state){
    case 0:
        // Set bit on SDA
        if(bbi2c_tx_buf & BIT7)
            PORTS_SDA_HIGH;
        else
            PORTS_SDA_LOW;
        PORTS_SCL_HIGH;
        bbi2c_state = 1;
        break;
    case 1:
        // Move to next bit
        bbi2c_tx_buf <<= 1;
        bits--;
        PORTS_SCL_LOW;
        bbi2c_state = 2;
        break;
    case 2:
        // Continue to next bit or finish
        if(bits > 0)
            bbi2c_state = 0;
        else
            bbi2c_state = 3;
        break;
    case 3:
        // Prep to detect NACK
        PORTS_SDA_HIGH;
        PORTS_SCL_HIGH;
        while(!PORTS_SCL_READ);     // In case slave is clock stretching
        bbi2c_state = 4;
        break;
    case 4:
        if(PORTS_SDA_READ){
            // Still high = NACK
            return STATUS_FAIL;
        }
        PORTS_SCL_LOW;
        bbi2c_state = 5;
        break;
    case 5:
        return STATUS_DONE;
    }
    return STATUS_BUSY;
}

void bbi2c_next(void){
    switch(bbi2c_operation){
    case OPERATION_IDLE:
        break;
    case OPERATION_START:
        break;
    case OPERATION_WRITE:
        break;
    case OPERATION_STOP:
        break;
    }
}

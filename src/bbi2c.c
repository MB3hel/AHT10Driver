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
/// Globals
////////////////////////////////////////////////////////////////////////////////

// bbi2c_callback(unsigned int operation, unsigned int result)
void (*bbi2c_callback)(unsigned int, unsigned int) = NULL;

unsigned int bbi2c_operation = BBI2C_OPERATION_IDLE;
unsigned int bbi2c_state = 0;
uint8_t bbi2c_address;
uint8_t bbi2c_tx_buf;
uint8_t bbi2c_bitcount;


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

void bbi2c_init(void (*cb)(unsigned int, unsigned int)){
    bbi2c_callback = cb;
    bbi2c_operation = BBI2C_OPERATION_IDLE;
    bbi2c_state = 0;
    PORTS_SDA_HIGH;
    PORTS_SCL_HIGH;
}

void bbi2c_start(uint8_t address){
    bbi2c_address = address;
    bbi2c_state = 0;
    bbi2c_operation = BBI2C_OPERATION_START;
    bbi2c_next();
}

void bbi2c_write(uint8_t data){
    bbi2c_tx_buf = data;
    bbi2c_state = 0;
    bbi2c_operation = BBI2C_OPERATION_WRITE;
    bbi2c_next();
}

void bbi2c_stop(void){
    bbi2c_state = 0;
    bbi2c_operation = BBI2C_OPERATION_STOP;
    bbi2c_next();
}


/**
 * Start condition state machine
 * @return BBI2C_STATUS_DONE, BBI2C_STATUS_BUSY, BBI2C_STATUS_FAIL
 */
unsigned int bbi2c_start_next(void){
    switch(bbi2c_state){
    case 0:
        // Start condition
        bbi2c_bitcount = 7;
        bbi2c_tx_buf = (bbi2c_address << 1);
        PORTS_SDA_LOW;
        PORTS_SCL_LOW;
        bbi2c_state = 1;
        break;
    case 1:
        // Transmit next bit
        if(bbi2c_tx_buf & BIT7)
            PORTS_SDA_HIGH;
        else
            PORTS_SDA_LOW;
        PORTS_SCL_HIGH;
        bbi2c_state = 2;
        break;
    case 2:
        // Increment to next bit
        bbi2c_tx_buf <<= 1;
        bbi2c_bitcount--;
        PORTS_SCL_LOW;
        if(bbi2c_bitcount > 0)
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
            bbi2c_state = 6;
        }
        PORTS_SCL_LOW;
        bbi2c_state = 5;
        break;
    case 5:
        return BBI2C_STATUS_DONE;
    case 6:
        PORTS_SCL_LOW;
        bbi2c_state = 7;
        break;
    case 7:
        PORTS_SDA_LOW;
        bbi2c_state = 8;
        break;
    case 8:
        PORTS_SCL_HIGH;
        PORTS_SDA_HIGH;
        return BBI2C_STATUS_FAIL;
    }
    return BBI2C_STATUS_BUSY;
}

/**
 * Stop condition state machine
 * @return BBI2C_STATUS_DONE, BBI2C_STATUS_BUSY, BBI2C_STATUS_FAIL
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
        return BBI2C_STATUS_DONE;
    }
    return BBI2C_STATUS_BUSY;
}

/**
 * Transmit byte state machine
 * @return BBI2C_STATUS_DONE, BBI2C_STATUS_BUSY, BBI2C_STATUS_FAIL
 */
unsigned int bbi2c_write_next(void){
    switch(bbi2c_state){
    case 0:
        bbi2c_bitcount = 7;
        bbi2c_tx_buf = bbi2c_address;
        bbi2c_state = 1;
        // Case fallthrough intentional
    case 1:
        // Set bit on SDA
        if(bbi2c_tx_buf & BIT7)
            PORTS_SDA_HIGH;
        else
            PORTS_SDA_LOW;
        PORTS_SCL_HIGH;
        bbi2c_state = 2;
        break;
    case 2:
        // Move to next bit
        bbi2c_tx_buf <<= 1;
        bbi2c_bitcount--;
        PORTS_SCL_LOW;
        bbi2c_state = 3;
        break;
    case 3:
        // Continue to next bit or finish
        if(bbi2c_bitcount > 0)
            bbi2c_state = 1;
        else
            bbi2c_state = 4;
        break;
    case 4:
        // Prep to detect NACK
        PORTS_SDA_HIGH;
        PORTS_SCL_HIGH;
        while(!PORTS_SCL_READ);     // In case slave is clock stretching
        bbi2c_state = 5;
        break;
    case 5:
        if(PORTS_SDA_READ){
            // Still high = NACK
            return BBI2C_STATUS_FAIL;
        }
        PORTS_SCL_LOW;
        bbi2c_state = 6;
        break;
    case 6:
        return BBI2C_STATUS_DONE;
    }
    return BBI2C_STATUS_BUSY;
}

void bbi2c_next(void){
    unsigned int result;

    switch(bbi2c_operation){
    case BBI2C_OPERATION_START:
        result = bbi2c_start_next();
        if(result != BBI2C_STATUS_BUSY){
            bbi2c_operation = BBI2C_OPERATION_IDLE;
            if(bbi2c_callback != NULL)
                bbi2c_callback(BBI2C_OPERATION_START, result);
        }
        break;

    case BBI2C_OPERATION_WRITE:
        result = bbi2c_write_next();
        if(result != BBI2C_STATUS_BUSY){
            bbi2c_operation = BBI2C_OPERATION_IDLE;
            if(bbi2c_callback != NULL)
                bbi2c_callback(BBI2C_OPERATION_WRITE, result);
        }
        break;

    case BBI2C_OPERATION_STOP:
        result = bbi2c_stop_next();
        if(result != BBI2C_STATUS_BUSY){
            bbi2c_operation = BBI2C_OPERATION_IDLE;
            if(bbi2c_callback != NULL)
                bbi2c_callback(BBI2C_OPERATION_STOP, result);
        }
        break;
    }

    if(bbi2c_operation != BBI2C_OPERATION_IDLE)
        timers_bbi2c_delay();
}

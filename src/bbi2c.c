/**
 * @file bbi2c.c
 * @author Marcus Behel (mgbehel@ncsu.edu)
 */

#include <bbi2c.h>
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ports.h>
#include <timers.h>





////////////////////////////////////////////////////////////////////////////////
/// Globals
////////////////////////////////////////////////////////////////////////////////

void (*bbi2c_callback)(bbi2c_transaction*, bool);
unsigned int bbi2c_state;
unsigned int bbi2c_bits;
unsigned int bbi2c_pos;
uint8_t bbi2c_buf;
bbi2c_transaction *bbi2c_trans;


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

void bbi2c_init(void (*cb)(bbi2c_transaction*, bool)){
    bbi2c_callback = cb;
    PORTS_SDA_HIGH;
    PORTS_SCL_HIGH;
}

void bbi2c_perform(bbi2c_transaction *trans){
    bbi2c_trans = trans;
    bbi2c_state = 0;
    bbi2c_next();
}

void bbi2c_next(void){
    // -------------------------------------------------------------------------
    // Write portion of transaction
    // -------------------------------------------------------------------------
    switch(bbi2c_state){
    // Write guard
    case 0:
        if(bbi2c_trans->write_count == 0){
            bbi2c_state = 50;
            break;
        }
        // Fallthrough intentional

    // Start bit
    case 1:
        PORTS_SDA_LOW;
        __no_operation();
        PORTS_SCL_LOW;
        bbi2c_state = 2;
        break;

    // Control byte
    case 2:
        bbi2c_bits = 8;
        bbi2c_buf = bbi2c_trans->address << 1;
        //Fallthrough intentional
    case 3:
        if(bbi2c_buf & BIT7)
            PORTS_SDA_HIGH;
        else
            PORTS_SDA_LOW;
        PORTS_SCL_HIGH;
        bbi2c_state = 4;
        break;
    case 4:
        bbi2c_buf <<= 1;
        bbi2c_bits--;
        if(bbi2c_bits > 0)
            bbi2c_state = 3;
        else
            bbi2c_state = 5;
        PORTS_SCL_LOW;
        break;
    case 5:
        PORTS_SDA_HIGH;
        PORTS_SCL_HIGH;
        while(!PORTS_SCL_READ);     // In case slave is clock stretching
        bbi2c_state = 6;
        break;
    case 6:
        PORTS_SCL_LOW;
        if(PORTS_SDA_READ){
            bbi2c_state = 7;
            break;
        }
        bbi2c_state = 9;
        break;
    case 7:
        PORTS_SDA_LOW;
        bbi2c_state = 8;
        break;
    case 8:
        PORTS_SCL_HIGH;
        __no_operation();
        PORTS_SDA_HIGH;
        bbi2c_callback(bbi2c_trans, false);
        return;

    // Write bytes
    case 9:
        bbi2c_pos = 0;
        PORTS_SCL_LOW;
        bbi2c_state = 10;
        break;
    case 10:
        bbi2c_buf = bbi2c_trans->write_buf[bbi2c_pos];
        bbi2c_bits = 8;
        // Fallthrough intentional
    case 11:
        if(bbi2c_buf & BIT7)
            PORTS_SDA_HIGH;
        else
            PORTS_SDA_LOW;
        PORTS_SCL_HIGH;
        bbi2c_state = 12;
        break;
    case 12:
        bbi2c_buf <<= 1;
        bbi2c_bits--;
        if(bbi2c_bits > 0)
            bbi2c_state = 11;
        else
            bbi2c_state = 13;
        PORTS_SCL_LOW;
        break;
    case 13:
        PORTS_SDA_HIGH;
        PORTS_SCL_HIGH;
        while(!PORTS_SCL_READ);     // In case slave is clock stretching
        bbi2c_state = 14;
        break;
    case 14:
        PORTS_SCL_LOW;
        if(PORTS_SDA_READ){
            bbi2c_state = 15;
            break;
        }
        bbi2c_state = 17;
        break;
    case 15:
        PORTS_SDA_LOW;
        bbi2c_state = 16;
        break;
    case 16:
        PORTS_SCL_HIGH;
        __no_operation();
        PORTS_SDA_HIGH;
        bbi2c_callback(bbi2c_trans, false);
        return;
    case 17:
        bbi2c_pos++;
        if(bbi2c_pos < bbi2c_trans->write_count){
            PORTS_SCL_LOW;
            bbi2c_state = 10;
        }else{
            if(!(bbi2c_trans->repeated_start && bbi2c_trans->read_count > 0)){
                PORTS_SCL_LOW;
                bbi2c_state = 18;
            }else{
                bbi2c_state = 50;
                PORTS_SCL_HIGH;
                __no_operation();
                __no_operation();
                __no_operation();
                PORTS_SDA_HIGH;
            }
        }
        break;
    case 18:
        PORTS_SDA_LOW;
        bbi2c_state = 19;
        break;
    case 19:
        bbi2c_state = 50;
        PORTS_SCL_HIGH;
        __no_operation();
        PORTS_SDA_HIGH;
    }

    // -------------------------------------------------------------------------
    // Read portion of transaction
    // -------------------------------------------------------------------------
    switch(bbi2c_state){
    case 50:
        // For now, read is unimplemented, so complete successfully
        bbi2c_callback(bbi2c_trans, true);
        return;
    }

    // Called unless state machine exists function with "return"
    // Ensures state transitions happen until transaction finished
    timers_bbi2c_delay();
}

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


// Note: __no_operation() does nothing, but has a purpose
//       It is a function that is called, causing a small delay
//       This is used to ensure SDA or SCL changes first


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

void bbi2c_write_byte(uint8_t data){
    unsigned int i;
    for(i = 8; i > 0; --i){
        // Set SDA line
        if(data & BIT7)
            PORTS_SDA_HIGH;
        else
            PORTS_SDA_LOW;
        __no_operation();

        // Generate rising clock edge
        PORTS_SCL_HIGH;
        timers_bbi2c_delay();

        // Move to next bit (and next clock cycle)
        data <<= 1;
        PORTS_SCL_LOW;
        timers_bbi2c_delay();
    }
}

uint8_t bbi2c_read_byte(void){
    uint8_t data = 0;
    unsigned int i;
    for(i = 8; i > 0; --i){
        // Shift data
        data <<= 1;

        // Next clock cycle
        PORTS_SCL_HIGH;
        timers_bbi2c_delay();

        // Read data line
        if(PORTS_SDA_READ)
            data |= 0x01;

        // Next clock cycel
        PORTS_SCL_LOW;
        timers_bbi2c_delay();
    }
    return data;
}

bool bbi2c_write(bbi2c_transaction *trans){
    unsigned int i = 0;
    bool sendStop;

    // Start configuration
    PORTS_SDA_LOW;
    __no_operation();
    PORTS_SCL_LOW;

    // Send control byte
    bbi2c_write_byte(trans->address << 1);

    // Check for ACK
    PORTS_SDA_HIGH;
    PORTS_SCL_HIGH;
    while(!PORTS_SCL_READ);     // In case slave is clock stretching
    timers_bbi2c_delay();

    if(!PORTS_SDA_READ){
        // SDA low = ACK. Can continue transmitting

        // Cannot change SDA until SCL low for a cycle
        PORTS_SCL_LOW;
        timers_bbi2c_delay();

        for(i = 0; i < trans->write_count; ++i){
            bbi2c_write_byte(trans->write_buf[i]);

            // Check for ACK
            PORTS_SDA_HIGH;
            PORTS_SCL_HIGH;
            while(!PORTS_SCL_READ);     // In case slave is clock stretching
            timers_bbi2c_delay();

            if(PORTS_SDA_READ){
                // SDA high = NACK. Stop transmitting
                break;
            }

            PORTS_SCL_LOW;
            timers_bbi2c_delay();
        }
    }

    // Send stop if necessary
    sendStop = !(trans->repeated_start && trans->read_count > 0);
    if((sendStop && i == trans->write_count) || (i != trans->write_count)){
        PORTS_SCL_LOW;
        timers_bbi2c_delay();
        PORTS_SDA_LOW;
        timers_bbi2c_delay();
        PORTS_SCL_HIGH;
        __no_operation();
        PORTS_SDA_HIGH;
    }else{
        PORTS_SCL_HIGH;
        PORTS_SDA_HIGH;
    }

    return (i == trans->write_count);
}

bool bbi2c_read(bbi2c_transaction *trans){
    unsigned int i = 0;

    // Send start
    PORTS_SDA_LOW;
    __no_operation();
    PORTS_SCL_LOW;
    timers_bbi2c_delay();

    // Send control byte
    bbi2c_write_byte((trans->address << 1) | BIT0);

    // Check for ACK
    PORTS_SDA_HIGH;
    PORTS_SCL_HIGH;
    timers_bbi2c_delay();

    if(!PORTS_SDA_READ){
        // SDA low = ACK. Can continue reading

        // Read all requested bytes
        for(i = 0; i < trans->read_count; ++i){
            while(!PORTS_SCL_READ);     // In case slave is clock stretching

            // Another clock cycle
            PORTS_SCL_LOW;
            timers_bbi2c_delay();
            PORTS_SDA_HIGH;

            trans->read_buf[i] = bbi2c_read_byte();

            // Send ACK
            PORTS_SDA_LOW;
            PORTS_SCL_HIGH;

            while(!PORTS_SCL_READ);     // In case slave is clock stretching
            timers_bbi2c_delay();
        }
    }

    // Cleanup
    PORTS_SCL_LOW;
    timers_bbi2c_delay();
    PORTS_SDA_LOW;
    timers_bbi2c_delay();

    PORTS_SCL_HIGH;
    __no_operation();
    PORTS_SDA_HIGH;
    timers_bbi2c_delay();

    return i == trans->read_count;
}

void bbi2c_init(void){
    PORTS_SDA_HIGH;
    PORTS_SCL_HIGH;
}

bool bbi2c_perform(bbi2c_transaction *trans){
    if(trans->write_count > 0){
        if(!bbi2c_write(trans))
            return false;
    }
    if(trans->read_count > 0){
        if(!bbi2c_read(trans))
            return false;
    }
    return true;
}

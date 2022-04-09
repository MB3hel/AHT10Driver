/**
 * @file bbi2c.c
 * @author Marcus Behel (mgbehel@ncsu.edu)
 */

#include <bbi2c.h>
#include <timers.h>
#include <ports.h>


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

void bbi2c_write_byte(uint8_t b){
    uint_fast8_t i;
    for(i = 8; i > 0; --i){
        if(b & BIT7)
            PORTS_SDA_HIGH;
        else
            PORTS_SDA_LOW;
        PORTS_SCL_HIGH;
        timers_bbi2c_delay();
        b <<= 1;
        PORTS_SCL_LOW;
        timers_bbi2c_delay();
    }
}

uint8_t bbi2c_read_byte(void){
    uint_fast8_t i;
    uint8_t res = 0;
    for(i = 8; i > 0; --i){
        res <<= 1;
        PORTS_SCL_HIGH;
        timers_bbi2c_delay();
        if(PORTS_SDA_READ)
            res |= 0x01;
        PORTS_SCL_LOW;
        timers_bbi2c_delay();
    }
    return res;
}

bool bbi2c_write(bbi2c_transaction *trans){
    size_t i = 0;

    // Send start bit
    PORTS_SDA_LOW;
    PORTS_SCL_LOW;
    timers_bbi2c_delay();

    // Send address (lower 7-bits then a zero)
    bbi2c_write_byte(trans->addr << 1);

    // Check for ACK (ACK = SDA pulled low; NACK = no ACK = SDA stays high)
    PORTS_SDA_HIGH;
    PORTS_SCL_HIGH;

    // Wait for clock to go high (in case slave is stretching)
    while(!PORTS_SCL_READ);

    timers_bbi2c_delay();

    // Actually check for ACK response (ACK = SDA low)
    if(!PORTS_SDA_READ){
        // Got ACK continue with transmission
        // Write as many bytes as were requested

        // Complete clock cycle from ACK read
        PORTS_SCL_LOW;
        timers_bbi2c_delay();

        // Send each byte to be written
        for(i = 0; i < trans->write_count; ++i){
            // Transmit all bits of this byte
            bbi2c_write_byte(trans->write_buf[i]);

            // Check for ACK. If no ACK (NACK) tx failure break out of loop
            PORTS_SDA_HIGH;
            PORTS_SCL_HIGH;

            // Wait for clock to go high (in case slave is stretching)
            while(!PORTS_SCL_READ);
            timers_bbi2c_delay();

            // Actual NACK check (high = NACK)
            if(PORTS_SDA_READ)
                break;

            // Complete clock cycle
            PORTS_SCL_LOW;
            timers_bbi2c_delay();
        }
    }

    // Send stop bit, unless requested to skip
    bool send_stop = trans->repeat_start && trans->read_count > 0;
    if(send_stop && i == trans->write_count || i != trans->write_count){
        PORTS_SCL_LOW;
        timers_bbi2c_delay();
        PORTS_SDA_LOW;
        timers_bbi2c_delay();
    }
    PORTS_SCL_HIGH;
    PORTS_SDA_HIGH;

    // If i == write_count all bytes were written (success)
    // If 0 < i < write_count some bytes were written (failure)
    // If i == 0 no bytes were written (failure)
    return i == trans->write_count;
}

bool bbi2c_read(bbi2c_transaction *trans){
    size_t i = 0;

    // Send start bit
    PORTS_SDA_LOW;
    PORTS_SCL_LOW;
    timers_bbi2c_delay();

    // Send address (lower 7-bits then a zero)
    bbi2c_write_byte(trans->addr << 1);

    // Check for ACK (ACK = SDA pulled low; NACK = no ACK = SDA stays high)
    PORTS_SDA_HIGH;
    PORTS_SCL_HIGH;
    timers_bbi2c_delay();
    if(!PORTS_SDA_READ){
        // Got ACK continue with transmission
        // Read as many bytes as were requested
        for(i = 0; i < trans->read_count; ++i){
            // Wait for clock to go high (in case slave is stretching)
            while(!PORTS_SCL_READ);

            // One more clock cycle
            PORTS_SCL_LOW;
            timers_bbi2c_delay();
            PORTS_SCL_HIGH;

            // Read a byte
            trans->read_buf[i] = bbi2c_read_byte();

            // Send ACK on last byte received
            if(i == trans->read_count - 1)
                PORTS_SDA_HIGH;
            else
                PORTS_SDA_LOW;
            PORTS_SCL_HIGH;

            // Wait for clock to go high (in case slave is stretching)
            while(!PORTS_SCL_READ);
            timers_bbi2c_delay();
        }
    }

    // Send stop bit
    PORTS_SCL_LOW;
    PORTS_SDA_LOW;
    timers_bbi2c_delay();
    PORTS_SCL_HIGH;
    PORTS_SDA_HIGH;
    timers_bbi2c_delay();

    // If i == read_count all bytes were read (success)
    // If i == 0 no bytes were read (failure)
    return i != 0;
}

void bbi2c_init(void){
    // Nothing to do here for now.
    // Exists in API in case something should be done later
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

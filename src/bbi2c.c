
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

    // Send address
    bbi2c_write_byte(trans->addr);

    // Check for ACK (ACK = SDA pulled low; NACK = no ACK = SDA stays high)
    PORTS_SDA_HIGH;
    PORTS_SCL_HIGH;
    timers_bbi2c_delay();
    if(!PORTS_SDA_READ){

    }
}

bool bbi2c_read(bbi2c_transaction *trans){
    size_t i = 0;

    // Send start bit
    PORTS_SDA_LOW;
    PORTS_SCL_LOW;
    timers_bbi2c_delay();

    // Send address
    bbi2c_write_byte(trans->addr);

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

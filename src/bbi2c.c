
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

bool bbi2c_write(bbi2c_transaction *trans){

}

bool bbi2c_read(bbi2c_transaction *trans){
    size_t count = 0;

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
        // TODO
    }

    // Send stop bit
    PORTS_SCL_LOW;
    PORTS_SDA_LOW;
    timers_bbi2c_delay();
    PORTS_SCL_HIGH;
    PORTS_SDA_HIGH;
    timers_bbi2c_delay();

    // Success if all requested bytes were read
    return count == trans->read_count;
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

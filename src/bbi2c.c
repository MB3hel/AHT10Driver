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
volatile unsigned int bbi2c_state;
volatile unsigned int bbi2c_bits;
volatile unsigned int bbi2c_pos;
volatile uint8_t bbi2c_buf;
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

/*
 * The bbi2c_next function implements a state machine that performs I2C
 * transactions. Calling the function causes it to perform the actions for the
 * current state. It then changes the state variable (bbi2c_state) to the next
 * state before enabling the timer-based delay and exiting the function. The
 * timer delay will configure the timer interrupt to occur after some amount of
 * time. The ISR then calls this function again. When the transaction is
 * complete (either successfully or not) the function returns without enabling
 * the timer delay. When complete, the callback function is called. Since
 * bbi2c_next is likely invoked from an isr, the end user must treat the
 * callback function like an isr.
 *
 * The state machine implements the following algorithm. The algorithm is
 * written in pseudocode. Note that in the following pseudocode, delay() is a
 * blocking delay. In the state machine, the delay occurs between states.
 *
 * Note that for(i = 0:7) means i = 0 to 7 (both ends inclusive)
 *
 * Also note that SDA or SCL low means the master drives the line low, but
 * SDA or SCL high means the master lets the line float. The pullup resistors
 * pull it high.
 *
 * The delay duration determines the I2C data rate. Changes to SCL and SDA are
 * offset by half a clock period, thus delay should be half of the clock period.
 * As such, between delays sets of SDA and SCL should alternate
 *
 * When no transaction is in progress, both SDA and SCL are floating (high).
 *
 * function i2c_transaction(bbi2c_transaction *transaction)
 *     if (transaction->write_count > 0)
 *         // Start bit (SDA goes low followed by SCL)
 *         SDA_LOW
 *         delay()
 *         SCL_LOW
 *         delay()
 *
 *         // Control byte (7-bits address, 1-bit write/read = 0/1)
 *         cb = transaction->address << 1
 *         for(bit = 0:7)
 *             if(cb & 0x80) SDA_HIGH
 *             else SDA_LOW
 *             cb <<= 1
 *             delay()
 *             SCL_HIGH
 *             delay()
 *             // No change to SDA here (slave latches data)
 *             delay()
 *             SCL_LOW
 *             delay()
 *         endfor
 *
 *         // Check for NACK
 *         SDA_HIGH
 *         delay()
 *         SCL_HIGH
 *         while(!SCL_READ) // Slave may be clock stretching
 *         delay()
 *         ack = !SDA_READ
 *         delay()
 *         SCL_LOW
 *         delay()
 *         if(ack)
 *             // SDA Low is ACK
 *             for(i = 0:transaction->write_count)
 *                 data = transaction->write_buf[i]
 *                 for(bit = 0:7)
 *                     if(data & 0x80) SDA_HIGH
 *                     else SDA_LOW
 *                     data <<= 1
 *                     delay()
 *                     SCL_HIGH
 *                     delay()
 *                     // No change to SDA here (slave latches data)
 *                     delay()
 *                     SCL_LOW
 *                     delay()
 *                 end
 *
 *                 // NACK check
 *                 SDA_HIGH
 *                 delay()
 *                 SCL_HIGH
 *                 while(!SCL_READ) // Slave may be clock stretching
 *                 delay()
 *                 ack = !SDA_READ
 *                 delay()
 *                 SCL_LOW
 *                 delay()
 *                 if(!ack)
 *                     break;    // SDA still high = NACK
 *                 endif
 *             endfor
 *         endif
 *
 *         // Stop bit (SCL goes high followed by SDA)
 *         SDA_LOW
 *         delay()
 *         SCL_HIGH
 *         delay()
 *         SDA_HIGH
 *         if(i != transaction->write_count)
 *             return false;     // Failed to write all bytes
 *     endif
 *
 *     if(transaction->read_count > 0)
 *         // TODO: Implement this...
 *     endif
 *     return true;
 * endfunction
 */

void bbi2c_next(void){

    // -------------------------------------------------------------------------
    // Write portion of transaction
    // -------------------------------------------------------------------------
    switch(bbi2c_state){
    case 0:
        if(bbi2c_trans->write_count == 0){
            bbi2c_state = 50;       // Skip to read state machine
            break;
        }
        bbi2c_pos = 0;
        // fallthrough

    // Start bit
    case 1:
        PORTS_SDA_LOW;
        bbi2c_state = 2;
        break;
    case 2:
        PORTS_SCL_LOW;
        bbi2c_state = 3;
        break;

    // Control byte
    case 3:
        bbi2c_buf = bbi2c_trans->address << 1;
        bbi2c_bits = 0;
        // fallthrough
    case 4:
        if(bbi2c_buf & BIT7) PORTS_SDA_HIGH;
        else PORTS_SDA_LOW;
        bbi2c_state = 5;
        break;
    case 5:
        PORTS_SCL_HIGH;
        bbi2c_state = 6;
    case 6:
        // No change to SDA. Slave is latching data.
        ++bbi2c_bits;
        bbi2c_buf <<= 1;
        bbi2c_state = 7;
        break;
    case 7:
        PORTS_SCL_LOW;
        if(bbi2c_bits < 8)
            bbi2c_state = 4;
        else
            bbi2c_state = 8;
        break;
    case 8:
        PORTS_SDA_HIGH;
        bbi2c_state = 9;
        break;
    case 9:
        PORTS_SCL_HIGH;
        while(!PORTS_SCL_READ);     // In case slave is clock stretching
        bbi2c_state = 10;
        break;
    case 10:
        bbi2c_buf = !PORTS_SDA_READ;    // SDA low = ACK (bbi2c_buf == 1 if ACK)
        bbi2c_state = 11;
        break;
    case 11:
        PORTS_SCL_LOW;
        if(bbi2c_buf)
            bbi2c_state = 12;   // Got ack.
        else
            bbi2c_state = 22;   // No ack. Skip to stop
        break;

    // Write bytes
    case 12:
        bbi2c_pos = 0;
        // fallthrough
    case 13:
        bbi2c_buf = bbi2c_trans->write_buf[bbi2c_pos];
        bbi2c_bits = 0;
        // fallthrough
    case 14:
        if(bbi2c_buf & BIT7) PORTS_SDA_HIGH;
        else PORTS_SDA_LOW;
        bbi2c_state = 15;
        break;
    case 15:
        PORTS_SCL_HIGH;
        bbi2c_state = 16;
        break;
    case 16:
        // No change to SDA. Slave is latching data.
        ++bbi2c_bits;
        bbi2c_buf <<= 1;
        bbi2c_state = 17;
        break;
    case 17:
        PORTS_SCL_LOW;
        if(bbi2c_bits < 8)
            bbi2c_state = 14;
        else
            bbi2c_state = 18;
        break;
    case 18:
        PORTS_SDA_HIGH;
        bbi2c_state = 19;
        break;
    case 19:
        PORTS_SCL_HIGH;
        while(!PORTS_SCL_READ); // In case slave is clock stretching
        bbi2c_state = 20;
        break;
    case 20:
        bbi2c_buf = !PORTS_SDA_READ;    // SDA low = ACK (bbi2c_buf == 1 if ACK)
        bbi2c_state = 21;
        break;
    case 21:
        PORTS_SCL_LOW;
        if(bbi2c_buf){
            ++bbi2c_pos;
            if(bbi2c_pos < bbi2c_trans->write_count)
                bbi2c_state = 13;   // Got ack.
            else
                bbi2c_state = 22;   // No ack. Skip to stop
        }else{
            bbi2c_state = 22;       // No ack. Skip to stop
        }
        break;

    // Stop bit
    case 22:
        PORTS_SDA_LOW;
        bbi2c_state = 23;
        break;
    case 23:
        PORTS_SCL_HIGH;
        bbi2c_state = 24;
        break;
    case 24:
        PORTS_SDA_HIGH;
        if(bbi2c_pos != bbi2c_trans->write_count){
            bbi2c_callback(bbi2c_trans, false);
            return; // Failed to write all bytes
        }
        // Write successful. Move on to read
        bbi2c_state = 50;
        break;
    }
    // -------------------------------------------------------------------------

    // -------------------------------------------------------------------------
    // Read portion of transaction
    // -------------------------------------------------------------------------
    switch(bbi2c_state){
    case 50:
        // TODO: Implement read portion of transaction
        // For now, any read is "successful"
        // If got here, write was successful
        bbi2c_callback(bbi2c_trans, true);
        return;
    }
    // -------------------------------------------------------------------------


    // Not done, so enable timer to move to next state
    timers_bbi2c_delay();
}

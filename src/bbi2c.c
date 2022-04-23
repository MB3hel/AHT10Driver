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

// Should be approx 1 micro second
// NumCycles = MCLKRateHz * DelaySec
// NumCycles = 16e6 * 1e-6 = 16 cycles
#define SMALL_DELAY __delay_cycles(16)

////////////////////////////////////////////////////////////////////////////////
/// Globals
////////////////////////////////////////////////////////////////////////////////

volatile unsigned int bbi2c_state;
volatile unsigned int bbi2c_bits;
volatile unsigned int bbi2c_pos;
volatile uint8_t bbi2c_buf;
bbi2c_transaction *bbi2c_trans;


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

void bbi2c_init(void){
    bbi2c_trans = NULL;
    PORTS_SDA_HIGH;
    PORTS_SCL_HIGH;
}

void bbi2c_perform(bbi2c_transaction *trans){
    bbi2c_trans = trans;
    bbi2c_state = 0;

    // Don't call bbi2c_next directly. Only timer ISR should call it.
    timers_bbi2c_delay();
}

/*
 * The following pseudocode describes the algorithm implemented by the
 * bbi2c_next state machine. The pseudocode description is a blocking algorithm.
 * State machine states are separated by "delay()"s. small_delay is just enough
 * to offset changes of SDA and SCL.
 *
 * SDA or SCL low requires the master drive the line low
 * SDA or SCL high requires the master release the line (floating). Pullup
 *     resistors pull the line high.
 *
 * function bbi2c_perform(trans)
 *     if(trans->write_count > 0)
 *         // Start bit
 *         SDA_LOW
 *         small_delay()
 *         SCL_LOW
 *         delay()
 *         pos = 0
 *
 *         // Control byte
 *         data = trans->adddress << 1
 *         for(bit = 0; bit < 8; ++bit)
 *             if(data & BIT7) SDA_HIGH
 *             else SDA_LOW
 *             small_delay()
 *             SCL_HIGH;
 *             delay()
 *             data <<= 1
 *             SCL_LOW
 *             delay()
 *         endfor
 *         SDA_HIGH
 *         small_delay()
 *         SCL_HIGH
 *         while(!SCL_READ) // Slave may be clock stretching
 *         delay()
 *         ack = !SDA_READ  // SDA low = ack
 *         SCL_LOW
 *         delay()
 *
 *         // Write bytes
 *         if(ack)
 *             for(pos = 0; pos < trans->write_count; ++pos)
 *                 data = trans->write_buf[pos]
 *                 for(bit = 0; bit < 8; ++bit)
 *                     if(data & BIT7) SDA_HIGH
 *                     else SDA_LOW
 *                     small_delay()
 *                     SCL_HIGH
 *                     delay()
 *                     data <<= 1
 *                     SCL_LOW
 *                     delay()
 *                 endfor
 *                 SDA_HIGH
 *                 small_delay()
 *                 SCL_HIGH
 *                 while(!SCL_READ)
 *                 delay()
 *                 ack = !SDA_READ
 *                 SCL_LOW
 *                 delay()
 *                 if(!ack)
 *                     break
 *                 endif
 *             endfor
 *         endif
 *
 *         // Stop Bit
 *         SDA_LOW
 *         delay()
 *         SCL_HIGH
 *         small_delay()
 *         SDA_HIGH
 *
 *         if(pos != trans->write_count)
 *             return false;    // Transaction failed
 *         endif
 *     endif
 *
 *     if(trans->read_count > 0)
 *         // TODO: Implement this
 *     endif
 *
 *     return true;
 * endfunction
 */

unsigned int bbi2c_next(void){

    // -------------------------------------------------------------------------
    // Write portion of transaction
    // -------------------------------------------------------------------------
    switch(bbi2c_state){
    case 0:
        if(bbi2c_trans->write_count == 0){
            bbi2c_state = 50;   // Skip to read portion
            break;
        }
        // fallthrough

    // Start bit
    case 1:
        PORTS_SDA_LOW;
        SMALL_DELAY;
        PORTS_SCL_LOW;
        bbi2c_pos = 0;
        bbi2c_state = 2;
        break;

    // Control byte
    case 2:
        bbi2c_buf = bbi2c_trans->address << 1;
        bbi2c_bits = 0;
        // fallthrough
    case 3:
        if(bbi2c_buf & BIT7) PORTS_SDA_HIGH;
        else PORTS_SDA_LOW;
        SMALL_DELAY;
        PORTS_SCL_HIGH;
        bbi2c_state = 4;
        break;
    case 4:
        bbi2c_buf <<= 1;
        bbi2c_bits++;
        PORTS_SCL_LOW;
        if(bbi2c_bits < 8)
            bbi2c_state = 3;
        else
            bbi2c_state = 5;
        break;
    case 5:
        PORTS_SDA_HIGH;
        SMALL_DELAY;
        PORTS_SCL_HIGH;
        while(!PORTS_SCL_READ);
        bbi2c_state = 6;
        break;
    case 6:
        if(!PORTS_SDA_READ)
            bbi2c_state = 7;
        else
            bbi2c_state = 12;
        PORTS_SCL_LOW;
        break;
    case 7:
        bbi2c_buf = bbi2c_trans->write_buf[bbi2c_pos];
        bbi2c_bits = 0;
        // fallthrough
    case 8:
        if(bbi2c_buf & BIT7) PORTS_SDA_HIGH;
        else PORTS_SDA_LOW;
        SMALL_DELAY;
        PORTS_SCL_HIGH;
        bbi2c_state = 9;
        break;
    case 9:
        bbi2c_buf <<= 1;
        bbi2c_bits++;
        PORTS_SCL_LOW;
        if(bbi2c_bits < 8)
            bbi2c_state = 8;
        else
            bbi2c_state = 10;
        break;
    case 10:
        PORTS_SDA_HIGH;
        SMALL_DELAY;
        PORTS_SCL_HIGH;
        while(!PORTS_SCL_READ);
        bbi2c_state = 11;
        break;
    case 11:
        bbi2c_pos++;
        if(!PORTS_SDA_READ){
            if(bbi2c_pos < bbi2c_trans->write_count)
                bbi2c_state = 7;
            else
                bbi2c_state = 12;
        }else{
            bbi2c_state = 12;
        }
        PORTS_SCL_LOW;
        break;

    // Stop bit
    case 12:
        PORTS_SDA_LOW;
        bbi2c_state = 13;
        break;
    case 13:
        PORTS_SCL_HIGH;
        SMALL_DELAY;
        PORTS_SDA_HIGH;
        if(bbi2c_pos != bbi2c_trans->write_count){
            return BBI2C_FAIL;
        }
        bbi2c_state = 50; // Move to read portion
        break;
    }
    // -------------------------------------------------------------------------

    // -------------------------------------------------------------------------
    // Read portion of transaction
    // -------------------------------------------------------------------------
    switch(bbi2c_state){
    case 50:
        if(bbi2c_trans->read_count == 0){
            return BBI2C_DONE;
        }
        //fallthrough

    // Start bit
    case 51:
        bbi2c_pos = 0;
        PORTS_SDA_LOW;
        SMALL_DELAY;
        PORTS_SCL_LOW;
        bbi2c_state = 52;
        break;

    // Control byte
    case 52:
        bbi2c_buf = (bbi2c_trans->address << 1) | BIT0;
        bbi2c_bits = 8;
        // fallthrough
    case 53:
        if(bbi2c_buf & BIT7) PORTS_SDA_HIGH;
        else PORTS_SDA_LOW;
        SMALL_DELAY;
        PORTS_SCL_HIGH;
        bbi2c_state = 54;
        break;
    case 54:
        bbi2c_buf <<= 1;
        bbi2c_bits--;
        PORTS_SCL_LOW;
        if(bbi2c_bits > 0)
            bbi2c_state = 53;
        else
            bbi2c_state = 55;
        break;
    case 55:
        PORTS_SDA_HIGH;
        SMALL_DELAY;
        PORTS_SCL_HIGH;
        while(!PORTS_SCL_READ); // In case slave is clock stretching
        bbi2c_state = 56;
        break;
    case 56:
        if(PORTS_SDA_READ){
            // NACK. Start the stop sequence
            PORTS_SCL_LOW;
            SMALL_DELAY;
            PORTS_SDA_LOW;
            bbi2c_state = 63;
            break;
        }
        bbi2c_pos = 0;
        // fallthrough
    case 57:
        while(!PORTS_SCL_READ); // In case slave is clock stretching
        bbi2c_buf = 0;
        bbi2c_bits = 8;
        PORTS_SCL_LOW;
        bbi2c_state = 58;
        break;
    case 58:
        PORTS_SDA_HIGH;
        SMALL_DELAY;
        // fallthrough
    case 59:
        bbi2c_buf <<= 1;
        PORTS_SCL_HIGH;
        bbi2c_state = 60;
        break;
    case 60:
        if(PORTS_SDA_READ) bbi2c_buf |= 0x01;
        bbi2c_bits--;
        if(bbi2c_bits > 0)
            bbi2c_state = 59;
        else
            bbi2c_state = 61;
        PORTS_SCL_LOW;
        break;
    case 61:
        bbi2c_trans->read_buf[bbi2c_pos] = bbi2c_buf;
        ++bbi2c_pos;
        if(bbi2c_pos == bbi2c_trans->read_count){
            PORTS_SDA_HIGH;
            bbi2c_state = 62;
        }else{
            PORTS_SDA_LOW;
            bbi2c_state = 57;
        }
        SMALL_DELAY;
        PORTS_SCL_HIGH;
        while(!PORTS_SCL_READ); // In case slave is clock stretching
        break;

    // Stop bits
    case 62:
        PORTS_SCL_LOW;
        SMALL_DELAY;
        PORTS_SDA_LOW;
        bbi2c_state = 63;
        break;
    case 63:
        PORTS_SCL_HIGH;
        SMALL_DELAY;
        PORTS_SDA_HIGH;
        bbi2c_state = 64;
        break;
    case 64:
        if(bbi2c_pos == bbi2c_trans->read_count)
            return BBI2C_DONE;
        return BBI2C_FAIL;
    }
    // -------------------------------------------------------------------------


    // Not done, so enable timer to move to next state
    timers_bbi2c_delay();

    return BBI2C_BUSY;
}

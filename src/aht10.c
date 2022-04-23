/**
 * @file aht10.c
 * @author Marcus Behel (mgbehel@ncsu.edu)
 */

#include <aht10.h>

/*
 * Basic usage outline (pseudocode; blocking)
 * Note: Does not check for errors after each i2c operation
 *
 * This is implemented using a state machine below.
 *
 * default_address = 0x38
 *
 * function begin()
 *     i2c_init();
 *
 *     // Reset
 *     i2c_write(address, 0xBA);
 *
 *     // Make sure slave is connected
 *     if(i2c_failure())
 *         return false;
 *
 *     // Calibrate
 *     i2c_write(address, [0xE1, 0x08, 0x00]);
 *
 *     // Read status and wait until sensor not busy
 *     while(true)
 *         status = i2c_read(address, 1);
 *         if(!(status & 0x80)) break;
 *     endwhile
 *
 *     // Make sure calibration successful
 *     status = i2c_read(address, 1);
 *     if(!(status & 0x08)) return false;
 *
 *     return true;
 * endfunction
 *
 * function read()
 *     // Trigger read data
 *     i2c_write(address, [0xAC, 0x33, 0x00]);
 *
 *     // Wait until AHT10 not busy
 *     while(true)
 *         status = i2c_read(address, 1);
 *         if(!(status & 0x80)) break;
 *     endwhile
 *
 *     // Read raw data
 *     data = i2c_read(address, 6);
 *
 *     // TODO: Temp and humidity calculations
 *
 * endfunction
 *
 */

////////////////////////////////////////////////////////////////////////////////
/// Macros
////////////////////////////////////////////////////////////////////////////////

#define STATE_NONE              0           // No state
#define STATE_RST               1           // Send reset command
#define STATE_CAL               2           // Send calibrate command
#define STATE_CALWAIT           3           // Wait until not busy
#define STATE_IDLE              4           // Wait until aht10_read() call
#define STATE_TRG               5           // Send read trigger
#define STATE_TRGWAIT           6           // Wait until not busy
#define STATE_READ              7           // Read data
#define STATE_CALC              8           // Calculate temp & humidity


////////////////////////////////////////////////////////////////////////////////
/// Globals
////////////////////////////////////////////////////////////////////////////////
unsigned int aht10_temperature;
unsigned int aht10_humidity;
unsigned int aht10_status;

bbi2c_transaction aht10_trans;

unsigned int aht10_current_state;
unsigned int aht10_next_state;
uint8_t aht10_wb[3];
uint8_t aht10_rb[6];


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

void aht10_init(void){
    aht10_trans.address = AHT10_ADDR;
    ath10_trans.write_buf = aht10_wb;
    aht10_read_buf = aht10_rb;

    aht10_status = AHT10_NOINIT;
    aht10_temperature = 0;
    aht10_humidity = 0;
    aht10_current_state = STATE_NONE;
    aht10_next_state = STATE_RST;
}

void aht10_process(void){
    if(aht10_status == AHT10_NODEV)
        return;         // No device connected

    // -------------------------------------------------------------------------
    // Actions to be performed on transition to a new state
    // -------------------------------------------------------------------------
    if(aht10_next_state != STATE_NONE){
        switch(aht10_next_state){
        case STATE_RST:
            // Send reset command
            aht10_wb[0] = 0xBA;
            aht10_trans.write_count = 1;
            aht10_trans.read_count = 0;
            bbi2c_perform(&aht10_trans);
            break;
        case STATE_CAL:
            // Send calibrate command
            aht10_wb[0] = 0x08;
            aht10_wb[1] = 0x08;
            aht10_wb[2] = 0x00;
            aht10_trans.write_count = 3;
            aht10_trans.read_count = 0;
            bbi2c_perform(aht10_trans);
            break;
        case STATE_CALWAIT:
            // Read status to determine if busy
            aht10_trans.write_count = 0;
            aht10_trans.read_count = 1;
            bbi2c_perform(aht10_trans);
            break;
        case STATE_IDLE:
            aht10_status = AHT10_IDLE;
            break;
        case
        }
        aht10_current_state = aht10_next_state;
        aht10_next_state = STATE_NONE;
    }


    // -------------------------------------------------------------------------
    // Actions to be performed periodically while in the current state
    // -------------------------------------------------------------------------
    switch(aht10_current_state){

    }
}

void aht10_read(void){
    // Cannot start a reading if AHT10 not initialized and idle
    if(aht10_status != AHT10_IDLE)
        return;
}

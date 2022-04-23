/**
 * @file aht10.c
 * @author Marcus Behel (mgbehel@ncsu.edu)
 */

#include <aht10.h>
#include <timers.h>

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

#define STATE_W_RST         0       // Write reset command
#define STATE_W_CAL         1       // Write calibrate command
#define STATE_R_STACAL      2       // Read status until not busy
#define STATE_W_TRG         3       // Write trigger for read data
#define STATE_R_STATRG      4       // Read status until not busy
#define STATE_R_DATA        5       // Read actual data


////////////////////////////////////////////////////////////////////////////////
/// Globals
////////////////////////////////////////////////////////////////////////////////
unsigned int aht10_temperature;
unsigned int aht10_humidity;
unsigned int aht10_status;
uint32_t aht10_last_read;
bbi2c_transaction aht10_trans;

unsigned int aht10_state;
uint8_t aht10_wb[3];
uint8_t aht10_rb[6];


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

/**
 * Called when state is changed. Runs new state's actions
 */
void aht10_actions(void){

}

void aht10_init(void){
    aht10_last_read = 0;
    aht10_status = AHT10_NOINIT;
    aht10_trans.address = 0x38;
    aht10_trans.write_buf = aht10_wb;
    aht10_trans.read_buf = aht10_rb;
    aht10_state = 0;
}

void aht10_read(void){

}

void aht10_i2c_done(bool success){

}


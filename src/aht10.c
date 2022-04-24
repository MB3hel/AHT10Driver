/**
 * @file aht10.c
 * @author Marcus Behel (mgbehel@ncsu.edu)
 */

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
 *     uint32_t tmp;
 *
 *     // Calculate temperature
 *     tmp = data[1];
 *     tmp <<= 8;
 *     tmp |= data[2];
 *     tmp <<= 4;
 *     tmp |= data[3] >> 4;
 *     humidity = ((float)tmp * 100) / 0x100000;
 *
 *     // Calculate humidity
 *     tmp = data[3] & 0x0F;
 *     tmp <<= 8;
 *     tmp |= data[4];
 *
 *
 * endfunction
 *
 */

/*
 * AHT10 State machine diagram (generated using asciiflow.com)
 *                      │ init
 *                  ┌───▼───┐
 *                  │  RST  │
 *                  └───┬───┘
 *                      │ i2c_done
 *                  ┌───▼───┐
 *                  │  CAL  │
 *                  └───┬───┘                       (from any)
 *                      │ i2c_done                       │ i2c_fail
 *                ┌►┌───▼───┐                        ┌───▼───┐
 *  i2c_done(busy)│ │CAL_STA├────────────────────────►  ERR  │
 *                └─┴───┬───┘   i2c_done(!busy;!cal) └───────┘
 *                      │ i2c_done(!busy;cal)
 *                  ┌───▼───┐
 *                  │  TRG  │◄─────────┐
 *                  └───┬───┘          │
 *                      │ i2c_done     │
 *                ┌►┌───▼───┐          │
 *           busy │ │TRG_STA│          │
 *                └─┴───┬───┘          │
 *                      │ !busy        │ read_requested
 *                  ┌───▼───┐          │
 *                  │ READ  │          │
 *                  └───┬───┘          │
 *                      │ i2c_done     │
 *                  ┌───▼───┐          │
 *                  │ IDLE  ├──────────┘
 *                  └───────┘
 */

#include <aht10.h>
#include <timers.h>


////////////////////////////////////////////////////////////////////////////////
/// Macros
////////////////////////////////////////////////////////////////////////////////

// AHT10 State machine states
#define STATE_RST           0               // Send reset command
#define STATE_CAL           1               // Send calibrate command
#define STATE_CAL_STA       2               // Read status from sensor
#define STATE_TRG           3               // Send trigger command
#define STATE_TRG_STA       4               // Read status from sensor
#define STATE_READ          5               // Read data from sensor
#define STATE_IDLE          6               // Calculate. Wait for read request
#define STATE_ERR           7               // Error occurred. Stop.


// I2C definitions
#define ADDR_DEF            0x38            // Default address
#define CMD_CALIBRATE       0xE1            // Calibrate command
#define CMD_TRIGGER         0xAC            // Trigger read command
#define CMD_RESET           0xBA            // Reset command
#define STATUS_BUSY         0x80            // Busy bit in status byte
#define STATUS_CAL          0x08            // Calibrate bit in status byte


////////////////////////////////////////////////////////////////////////////////
/// Globals
////////////////////////////////////////////////////////////////////////////////
float aht10_temperature;
float aht10_humidity;
unsigned int aht10_ec;
uint32_t aht10_last_read;
bbi2c_transaction aht10_trans;

unsigned int aht10_state;                   // Current state
uint8_t aht10_wb[3];                        // Write buffer
uint8_t aht10_rb[6];                        // Read buffer


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

/**
 * Called when state is changed. Runs new state's actions.
 * Should be called after state changes.
 */
void aht10_actions(void){
    uint32_t tmp;
    switch(aht10_state){
    case STATE_RST:
        // Send reset command
        aht10_wb[0] = CMD_RESET;
        aht10_trans.write_count = 1;
        aht10_trans.read_count = 0;
        bbi2c_perform(&aht10_trans);
        break;
    case STATE_CAL:
        // Send calibrate command
        aht10_wb[0] = CMD_CALIBRATE;
        aht10_wb[1] = 0x08;
        aht10_wb[2] = 0x00;
        aht10_trans.write_count = 3;
        aht10_trans.read_count = 0;
        bbi2c_perform(&aht10_trans);
        break;
    case STATE_CAL_STA:
        // Read status byte. Wait until not busy and check calibrate fail
        aht10_trans.write_count = 0;
        aht10_trans.read_count = 1;
        bbi2c_perform(&aht10_trans);
        break;
    case STATE_TRG:
        // Send trigger command
        aht10_wb[0] = CMD_TRIGGER;
        aht10_wb[1] = 0x33;
        aht10_wb[2] = 0x00;
        aht10_trans.write_count = 3;
        aht10_trans.read_count = 0;
        bbi2c_perform(&aht10_trans);
        break;
    case STATE_TRG_STA:
        // Read status byte. Wait until not busy
        aht10_trans.write_count = 0;
        aht10_trans.read_count = 1;
        bbi2c_perform(&aht10_trans);
        break;
    case STATE_READ:
        // Read raw sensor data
        aht10_trans.write_count = 0;
        aht10_trans.read_count = 6;
        bbi2c_perform(&aht10_trans);
        break;
    case STATE_IDLE:
        // Waiting for user to request a read (aht10_read)
        // Perform calculations using raw data that was read

        // TODO: Do this without floating point calculations

        // Calculate humidity
        tmp = aht10_rb[1];
        tmp <<= 8;
        tmp |= aht10_rb[2];
        tmp <<= 4;
        tmp |= aht10_rb[3] >> 4;
        aht10_humidity = ((float)tmp * 100) / 0x100000;

        // Calculate temperature
        tmp = aht10_rb[3] & 0x0F;
        tmp <<= 8;
        tmp |= aht10_rb[4];
        tmp <<= 8;
        tmp |= aht10_rb[5];
        aht10_temperature = ((float)tmp * 200 / 0x100000) - 50;
        break;
    }
}

void aht10_init(void){
    aht10_trans.address = ADDR_DEF;         // Set device address
    aht10_trans.write_buf = aht10_wb;       // Configure write buffer
    aht10_trans.read_buf = aht10_rb;        // Configure read buffer

    aht10_ec = AHT10_EC_NONE;               // No error (yet)

    aht10_state = STATE_RST;                // Set initial state
    aht10_actions();                        // State has changed. Run actions.
}

void aht10_read(void){
    // read_requested transition trigger
    if(aht10_state != STATE_IDLE)
        return;                             // No effect if not in idle state
    aht10_state = STATE_TRG;                // Transition to trigger state
    aht10_actions();                        // State changed. Run state actions
}

bool aht10_i2c_done(bool success){
    // i2c_fail transition trigger
    if(!success){
        aht10_ec = AHT10_EC_NODEV;          // Set error code for I2C failure
        aht10_state = STATE_ERR;            // Move to error state
        // aht10_actions();                 // No actions for error state
        return false;
    }

    // i2c_done transition trigger
    switch(aht10_state){
    case STATE_RST:
        aht10_state = STATE_CAL;
        break;
    case STATE_CAL:
        aht10_state = STATE_CAL_STA;
        break;
    case STATE_CAL_STA:
        if(aht10_rb[0] & STATUS_BUSY){
            // busy
            aht10_state = STATE_CAL_STA;
        }else{
            // !busy
            if(aht10_rb[0] & STATUS_CAL){
                // cal
                aht10_state = STATE_TRG;
            }else{
                // !cal
                aht10_ec = AHT10_EC_NOCAL;
                aht10_state = STATE_ERR;
            }
        }
        break;
    case STATE_TRG:
        aht10_state = STATE_TRG_STA;
        break;
    case STATE_TRG_STA:
        if(aht10_rb[0] & STATUS_BUSY){
            // busy
            aht10_state = STATE_TRG_STA;
        }else{
            // !busy
            aht10_state = STATE_READ;
        }
        break;
    case STATE_READ:
        aht10_state = STATE_IDLE;
        break;
    }

    aht10_actions();                        // State changed. Run state actions

    return aht10_state == STATE_IDLE;       // New data if now in idle state
}


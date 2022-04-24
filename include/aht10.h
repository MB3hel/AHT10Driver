/**
 * @file aht10.h
 * @brief AHT10 temperature and humidity sensor driver.
 * Implemented as a statemachine.
 * Implemented using bbi2c (SW I2C driver)
 *
 * @author Marcus Behel (mgbehel@ncsu.edu)
 * @version 1.0.0
 */

#pragma once

#include <stdint.h>
#include <bbi2c.h>

////////////////////////////////////////////////////////////////////////////////
/// Macros
////////////////////////////////////////////////////////////////////////////////

// AHT10 Error Codes
#define AHT10_EC_NONE               0       // No error
#define AHT10_EC_NODEV              1       // Device not connected (I2C fail)
#define AHT10_EC_NOCAL              2       // Device calibration failed


////////////////////////////////////////////////////////////////////////////////
/// Globals
////////////////////////////////////////////////////////////////////////////////
extern float aht10_temperature;             // Last read temperature (deg C)
extern float aht10_humidity;                // Last read humidity (%)
extern unsigned int aht10_ec;               // Current error code for AHT10
extern bbi2c_transaction aht10_trans;       // Transaction var for AHT10


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

/**
 * Initialize AHT10 subsystem and state machine
 */
void aht10_init(void);

/**
 * Request a read of data. Only works if status is AHT10_IDLE.
 */
void aht10_read(void);

/**
 * Indicate that I2C transaction finished. Triggers state changes.
 * @param success true if I2C transaction successful; false if not.
 * @return true if there are new temp / humidity values available else false
 */
bool aht10_i2c_done(bool success);

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

// AHT10 Status
#define AHT10_NOINIT            0           // Not initialized
#define AHT10_IDLE              1           // Idle (can request read)
#define AHT10_BUSY              2           // Busy (cannot request read)
#define AHT10_NODEV             3           // Device not found


////////////////////////////////////////////////////////////////////////////////
/// Globals
////////////////////////////////////////////////////////////////////////////////
extern unsigned int aht10_temperature;      // Last read temperature (deg C)
extern unsigned int aht10_humidity;         // Last read humidity (%)
extern bbi2c_transaction aht10_trans;       // Transaction var for AHT10
extern uint32_t aht10_last_read;            // Time of last read completion
extern unsigned int aht10_status;           // Current status


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
 */
void aht10_i2c_done(bool success);

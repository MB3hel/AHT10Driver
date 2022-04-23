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
#define AHT10_IDLE      0                   // Idle. Ready to read
#define AHT10_BUSY      1                   // Reading in progress
#define AHT10_NODEV     2                   // No I2C device found
#define AHT10_NOINIT    3                   // Not initialized.


////////////////////////////////////////////////////////////////////////////////
/// Globals
////////////////////////////////////////////////////////////////////////////////
extern unsigned int aht10_temperature;      // Last read temperature (deg C)
extern unsigned int aht10_humidity;         // Last read humidity (%)
extern unsigned int aht10_status;           // Current AHT10 status


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

/**
 * Initialize AHT10 state machine and subsystem
 */
void aht10_init(void);

/**
 * Handle whatever needs to be done for the AHT10
 */
void aht10_process(void);

/**
 * Start another reading of data (only if in IDLE state)
 */
void aht10_read(void);

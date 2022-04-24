/**
 * @file aht10.h
 * @brief AHT10 temperature and humidity sensor driver.
 * Implemented as a statemachine.
 * Implemented using bbi2c (SW I2C driver)
 *
 * @author Marcus Behel (mgbehel@ncsu.edu)
 * @version 1.0.0
 */

/*
 * MIT License
 *
 * Copyright (c) 2022 Marcus Behel
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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

// Last read temperature (deg C)
// Last two digits are after decimal point
extern unsigned int aht10_temperature;


// Last read humidity (%)
// Last two digits are after decimal point
extern unsigned int aht10_humidity;

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

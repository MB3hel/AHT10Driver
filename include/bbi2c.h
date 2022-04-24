/**
 * @file bbi2c.h
 * @brief Bitbanged (software) I2C (master only)
 *
 * Implemented using state machine.
 *
 * Data rate determined by timer configuration (see timers.h)
 * Pins determined by port configuration (see ports.h)
 *
 * Depending on MCLK rate, achieving a high data rate may not be possible or may
 * result in a rapid interrupt rate during the transaction effectively
 * preventing the rest of the program from running. If the system clock rate is
 * changed (see system.h) change the SMALL_DELAY macro in bbi2c.c
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
#include <stdbool.h>

////////////////////////////////////////////////////////////////////////////////
/// Typedefs
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    uint8_t address;
    uint8_t *write_buf;
    unsigned int write_count;
    uint8_t *read_buf;
    unsigned int read_count;
} bbi2c_transaction;

////////////////////////////////////////////////////////////////////////////////
/// Globals
////////////////////////////////////////////////////////////////////////////////

bbi2c_transaction *bbi2c_trans;


////////////////////////////////////////////////////////////////////////////////
/// Macros
////////////////////////////////////////////////////////////////////////////////

#define BBI2C_FAIL  0       // Transaction failed
#define BBI2C_DONE  1       // Transaction completed successfully
#define BBI2C_BUSY  2       // Transaction in progress


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

void bbi2c_init(void);

void bbi2c_perform(bbi2c_transaction *trans);

unsigned int bbi2c_next(void);

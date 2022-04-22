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

#pragma once

#include <stdint.h>
#include <stdbool.h>

////////////////////////////////////////////////////////////////////////////////
/// Macros
////////////////////////////////////////////////////////////////////////////////

#define BBI2C_FAIL  0       // Transaction failed
#define BBI2C_DONE  1       // Transaction completed successfully
#define BBI2C_BUSY  2       // Transaction in progress


////////////////////////////////////////////////////////////////////////////////
/// Typedefs
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    uint8_t address;
    uint8_t *write_buf;
    unsigned int write_count;
    uint8_t *read_buf;
    unsigned int read_count;
    bool repeated_start;
} bbi2c_transaction;


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

void bbi2c_init(void);

void bbi2c_perform(bbi2c_transaction *trans);

unsigned int bbi2c_next(void);

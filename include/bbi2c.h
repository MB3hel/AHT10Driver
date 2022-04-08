/**
 * @file bbi2c.h
 * @brief Bit banged I2C master implementation
 * Algorithm adapted from
 * https://www.ti.com/lit/an/slaa703a/slaa703a.pdf?ts=1649382877460
 *
 * TODO: Convert to a non-blocking state machine based implementation
 * @author Marcus Behel (mgbehel@ncsu.edu)
 * @version 1.0.0
 */

#pragma once


#include <msp430.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <ports.h>
#include <timers.h>

////////////////////////////////////////////////////////////////////////////////
/// Typedefs
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    uint8_t addr;
    uint8_t *write_buf;
    size_t write_count;
    uint8_t *read_buf;
    size_t read_count;
    bool repeat_start;
} bbi2c_transaction;


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

bool bbi2c_perform(bbi2c_transaction *transaction);

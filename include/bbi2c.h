/**
 * @file bbi2c.h
 * @brief Bitbanged (software) I2C (master only)
 * Implemented blocking (not as state machine)
 * State machine implementation would be of minimal use as maintaining correct
 * data rate would leave minimal time for non bbi2c tasks during transactions
 *
 * Based on TI example code https://www.ti.com/lit/an/slaa703a/slaa703a.pdf
 *
 * See ports.h for SDA and SCL configuration / control
 * See timers.h for timing and clock rate control
 *
 * @author Marcus Behel (mgbehel@ncsu.edu)
 * @version 1.0.0
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
    bool repeated_start;
} bbi2c_transaction;


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

void bbi2c_init(void);

bool bbi2c_perform(bbi2c_transaction *trans);

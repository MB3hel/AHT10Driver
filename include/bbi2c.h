/**
 * @file bbi2c.h
 * @brief Bitbanged (software) I2C (master only)
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

void bbi2c_init(void (*cb)(bbi2c_transaction*, bool));

void bbi2c_perform(bbi2c_transaction *trans);

void bbi2c_next(void);

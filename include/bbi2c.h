/**
 * @file bbi2c.h
 * @brief Bit banged I2C master implementation
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
/// Macros
////////////////////////////////////////////////////////////////////////////////

#define BBI2C_WB_SIZE       16


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

void bbi2c_init(void (*read_callback)(void));

void bbi2c_write(uint8_t address, uint8_t *data, size_t count);

void bbi2c_service(void);

void bbi2c_delay_done(void);

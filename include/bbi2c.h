/**
 * @file bbi2c.h
 * @brief Bitbanged (software) I2C (master only)
 * @author Marcus Behel (mgbehel@ncsu.edu)
 * @version 1.0.0
 */

#pragma once

#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
/// Macros
////////////////////////////////////////////////////////////////////////////////

#define BBI2C_OPERATION_IDLE      0
#define BBI2C_OPERATION_START     1
#define BBI2C_OPERATION_WRITE     2
#define BBI2C_OPERATION_READ      3
#define BBI2C_OPERATION_STOP      4

#define BBI2C_STATUS_FAIL         0
#define BBI2C_STATUS_BUSY         1
#define BBI2C_STATUS_DONE         2


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

/**
 * Initialize bbi2c state machine
 */
void bbi2c_init(void (*cb)(unsigned int, unsigned int));

/**
 * Send start condition and address of slave for following operations
 * @param address Address of slave
 */
void bbi2c_start(uint8_t address);

/**
 * Write a byte
 * @param data The byte to write
 */
void bbi2c_write(uint8_t data);

/**
 * Send stop condition
 */
void bbi2c_stop(void);

/**
 * High-level state machine. Call from timer ISR.
 */
void bbi2c_next(void);

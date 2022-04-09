/**
 * @file bbi2c.h
 * @brief Bit banged I2C master implementation
 * Algorithm adapted from
 * https://www.ti.com/lit/an/slaa703a/slaa703a.pdf?ts=1649382877460
 *
 * TODO: Convert to a non-blocking state machine based implementation
 *
 * Note: For now, this is a blocking implementation. Calling bbi2c_perform
 *       will block until transaction is complete. As such it is recommended
 *       to call no more frequently than necessary.
 *
 * Configuration notes:
 *   ports.h/.c  configure the SDA and SCL pins and provide macros to use them
 *               See macros in ports.h to change and read SDA, SCL
 *               See ports initialization for config of pins
 *   timers.h/.c configure the timer used by this subsystem and provide a
 *               function to wait for a certain duration. The configuration
 *               of the timer and duration control the I2C data rate.
 *               See timers_bbi2c_delay function. See timers initialization
 *               for general timer configuration. Timer configuration depends
 *               on clock configuration done in system.h/.c
 *
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
    uint8_t addr;                   // I2C slave address
    uint8_t *write_buf;             // Data to write
    size_t write_count;             // Number of bytes to write
    uint8_t *read_buf;              // Buffer to read into
    size_t read_count;              // Number of bytes to read
    bool repeat_start;              // Skip stop bit under correct circumstances
} bbi2c_transaction;


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

/**
 * Initialize bbi2c subsystem
 */
void bbi2c_init(void);

/**
 * Perform an I2C transaction.
 * WARNING: THIS FUNCTION WILL NOT RETURN UNTIL THE ENTIRE TRANSACTION IS DONE
 *          IT IS RECOMMENDED TO MINIMIZE FREQUENCY OF COMMUNICATION WITH BBI2C
 * @param transaction Pointer to I2C transaction to perform
 * @return true on success (read and write completed successfully) else false
 */
bool bbi2c_perform(bbi2c_transaction *transaction);

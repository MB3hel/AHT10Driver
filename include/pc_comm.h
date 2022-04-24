/**
 * @file pc_comm.h
 * @brief UART communication with a PC via USCI.
 *
 * Must configure pins correctly (see ports.h)
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

// Possible baud rates
#define PC_COMM_BUAD_9600   0


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

/**
 * Initialize pc comm subsystem including USCI port in use
 * @param baud Baud rate to initialize at
 */
void pc_comm_init(unsigned int baud);

/**
 * Set baud rate
 * @param baud Baud rate
 */
void pc_comm_set_buad(unsigned int baud);

/**
 * Copy a string into the internal write buffer (transmitted later)
 * @param str Null terminated string to copy
 * @return Number of bytes written
 */
unsigned int pc_comm_write_str(char *str);

/**
 * Write a byte into the internal write buffer (transmitted later)
 * @param b Byte to write
 * @return true on success else false
 */
bool pc_comm_write_byte(uint8_t b);

/**
 * Copy bytes into the internal write buffer (transmitted later)
 * @param data Buffer to copy from
 * @param len Number of bytes to copy from buffer
 * @return Number of bytes written
 */
unsigned int pc_comm_write_bytes(uint8_t *data, unsigned int len);

/**
 * Read one byte from the internal read buffer
 * @param dest Where to move read byte
 * @return true if byte read. false if none available to read
 */
bool pc_comm_read_byte(uint8_t *dest);

/**
 * Read multiple bytes from the internal read buffer
 * @param dest Where to move the read bytes
 * @param count Max number of bytes to read
 * @return Number of bytes read (up to count)
 */
unsigned int pc_comm_read_bytes(uint8_t *dest, unsigned int count);

/**
 * Call from ISR when byte written
 */
inline void pc_comm_handle_write(void);

/**
 * Call from ISR when byte read
 */
inline void pc_comm_handle_read(void);

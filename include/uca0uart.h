/**
 * @file uca0uart.h
 * @brief UART communication via UCA0
 *
 * Must configure pins correctly (see ports.h)
 * uca0uart_set_baud must change based on SMCLK rate (see system.h)
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
#define uca0uart_BUAD_9600   0


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

/**
 * Initialize pc comm subsystem including USCI port in use
 * @param baud Baud rate to initialize at
 */
void uca0uart_init(unsigned int baud);

/**
 * Set baud rate
 * @param baud Baud rate
 */
void uca0uart_set_buad(unsigned int baud);

/**
 * Copy a string into the internal write buffer (transmitted later)
 * @param str Null terminated string to copy
 * @return Number of bytes written
 */
unsigned int uca0uart_write_str(char *str);

/**
 * Write a byte into the internal write buffer (transmitted later)
 * @param b Byte to write
 * @return true on success else false
 */
bool uca0uart_write_byte(uint8_t b);

/**
 * Copy bytes into the internal write buffer (transmitted later)
 * @param data Buffer to copy from
 * @param len Number of bytes to copy from buffer
 * @return Number of bytes written
 */
unsigned int uca0uart_write_bytes(uint8_t *data, unsigned int len);

/**
 * Read one byte from the internal read buffer
 * @param dest Where to move read byte
 * @return true if byte read. false if none available to read
 */
bool uca0uart_read_byte(uint8_t *dest);

/**
 * Read multiple bytes from the internal read buffer
 * @param dest Where to move the read bytes
 * @param count Max number of bytes to read
 * @return Number of bytes read (up to count)
 */
unsigned int uca0uart_read_bytes(uint8_t *dest, unsigned int count);

/**
 * Call from ISR when byte written
 */
void uca0uart_handle_write(void);

/**
 * Call from ISR when byte read
 */
void uca0uart_handle_read(void);

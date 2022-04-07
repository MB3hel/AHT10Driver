/**
 * @file circular_buffer.h
 * @brief Ciruclar (ring) buffer implementation for comm protocols
 * @author Marcus Behel (mgbehel@ncsu.edu)
 * @version 1.0.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>


////////////////////////////////////////////////////////////////////////////////
/// Typedefs
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    uint8_t *data;              // Backing array for the buffer
    size_t length;              // Length of backing array
    size_t read_pos;            // Position to remove data from buffer at
    size_t write_pos;           // Position to insert data into buffer at
    bool full;                  // Used to distinguish between full and empty
} circular_buffer;


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

/**
 * Initialize a circular buffer for use. A circular buffer must not be used
 * before it is initialized (may cause memory overwrites).
 * @param cb Pointer to the circular buffer to initialize
 * @param backing_array Pointer to array to back the circular buffer
 * @param length Length of the array backing the circular buffer
 */
void cb_init(circular_buffer *cb, uint8_t *backing_array, size_t length);

/**
 * Write a single byte into the circular buffer
 * @param cb Pointer to the circular buffer to write into
 * @param value The value to write into the buffer
 * @return true on success; false on failure (buffer full)
 */
inline bool cb_write_byte(circular_buffer *cb, uint8_t value);

/**
 * Write multiple bytes into the circular buffer
 * @param cb Pointer to the circular buffer to write into
 * @param src Array of bytes to write into the buffer
 * @param length Length of the src array
 * @return Number of bytes written (may be less than length if buffer fills up)
 */
inline size_t cb_write(circular_buffer *cb, uint8_t *src, size_t length);

/**
 * Read a single byte from the buffer
 * @param cb Pointer to the circular buffer to read from
 * @param dest Pointer to the variable to store the read value in
 * @return true on success; false on failure (buffer empty)
 */
inline bool cb_read_byte(circular_buffer *cb, uint8_t *dest);

/**
 * Read multiple bytes from the buffer
 * @param cb Pointer to the circular buffer to read from
 * @param dest Pointer to the array to store read values in
 * @param length Max number of bytes to read
 * @return Number of bytes read (may be less than length if buffer empties)
 */
inline size_t cb_read(circular_buffer *cb, uint8_t *dest, size_t length);

/**
 * Check if the buffer is empty
 * @param cb Pointer to the buffer to check
 * @return true if empty; false if not empty
 */
inline bool cb_empty(circular_buffer *cb);

/**
 * Check if the buffer is full
 * @param cb Pointer to the buffer to check
 * @return true if full; false if not full
 */
inline bool cb_full(circular_buffer *cb);

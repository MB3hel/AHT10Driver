
#include <circular_buffer.h>

////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

void cb_init(circular_buffer *cb, uint8_t *backing_array, size_t length){
    cb->data = backing_array;
    cb->length = length;
    cb->read_pos = 0;
    cb->write_pos = 0;
    cb->full = false;
}

inline bool cb_write_byte(circular_buffer *cb, uint8_t value){
    if(!cb_full(cb)){
        cb->data[cb->write_pos] = value;            // Write data
        cb->write_pos++;                            // Increment write pos
        if(cb->write_pos == cb->length)             // Write pos rollover
            cb->write_pos = 0;
        if(cb->read_pos == cb->write_pos)           // Now full if equal poses
            cb->full = true;
        return true;                                // Write success
    }
    return false;                                   // Write failure
}

inline size_t cb_write(circular_buffer *cb, uint8_t *src, size_t length){
    size_t i;
    if(cb_full(cb)){
        return 0;                                   // Buffer full; no write
    }
    for(i = 0; i < length; ++i){
        cb->data[cb->write_pos] = src[i];           // Write byte
        cb->write_pos++;                            // Increment write pos
        if(cb->write_pos == cb->length)             // Write pos rollover
            cb->write_pos = 0;
        if(cb->read_pos == cb->write_pos){          // Now full if equal poses
            cb->full = true;
            return i;                               // Wrote some bytes
        }
    }
    return length;                                  // Wrote all bytes
}

inline bool cb_read_byte(circular_buffer *cb, uint8_t *dest){
    if(!cb_empty(cb)){
        *dest = cb->data[cb->read_pos];             // Read one byte
        cb->read_pos++;                             // Increment read pos
        if(cb->read_pos == cb->length)              // Read pos rollover
            cb->read_pos = 0;
        cb->full = false;                           // Cannot be full now
        return true;                                // Read success
    }
    return false;                                   // Read failure
}

inline size_t cb_read(circular_buffer *cb, uint8_t *dest, size_t length){
    size_t i;
    for(i = 0; i < length; ++i){
        if(cb_empty(cb)){
            return i;                               // No more data to read
        }
        dest[i] = cb->data[cb->read_pos];           // Read byte
        cb->read_pos++;                             // Increment read pos
        if(cb->read_pos == cb->length)              // Read pos rollover
            cb->read_pos = 0;
        cb->full = false;                           // Cannot be full now
    }
    return length;                                  // Read all requested data
}

inline bool cb_empty(circular_buffer *cb){
    return (cb->read_pos == cb->write_pos) && !cb->full;
}

inline bool cb_full(circular_buffer *cb){
    return (cb->write_pos == cb->read_pos) && cb->full;
}

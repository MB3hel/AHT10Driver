
#include <bbi2c.h>
#include <circular_buffer.h>


////////////////////////////////////////////////////////////////////////////////
/// Macros
////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// Byte transmit states
// -----------------------------------------------------------------------------

// These three states are looped over 8 times (once per bit)
#define BBI2C_TX_SET_DATA           0
#define BBI2C_TX_CLK_HIGH           1
#define BBI2C_TX_CLK_LOW            2

// These happen once each after transmit
#define BBI2C_TX_ACK_CLK_HIGH       3
#define BBI2C_TX_ACK_CLK_LOW        4
#define BBI2C_TX_ACK_DATA_LOW       5

// -----------------------------------------------------------------------------


// TODO: Byte receive states


////////////////////////////////////////////////////////////////////////////////
/// Globals
////////////////////////////////////////////////////////////////////////////////

uint8_t bbi2c_tx_array[BBI2C_TX_BUF_SIZE];  // Backing array for transit buffer
circular_buffer bbi2c_tx_cb;                // Transmit buffer
uint8_t bbi2c_state;                        // Transmit state machine state
uint8_t bbi2c_tx_bit;                       // Which bit in the current TX byte

void (*bbi2c_read_callback)(void);


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

inline void bbi2c_next_state(void){
    // I2C transaction example: Transmit 0b1010_0110
    //        -1----      -1----            -1-----1----      --
    // SDA:         -0----      -0-----0----            -0----  --ACK--
    //
    //         ----  ----  ----  ----  ----  ----  ----  ----  ----
    // SCL: ----  ----  ----  ----  ----  ----  ----  ----  ----  ----
    //
    // First 8 rising edges of clock a bit is tx'd to slave
    // Last rising edge slave should pull SDA low to ack before falling edge
    //

    // Pseudocode for blocking method of transmitting a byte:
    // data = 0b10100110
    // for i = 7...0
    //     SDA = bit i of data (DIO write)
    //     SCL_HIGH
    //     wait(clock_period)
    //     SCL_LOW
    //     wait(clock_period)
    // endfor
    // SDA_HIGH
    // SCL_HIGH
    // wait(clock_period)
    // ack = SDA (DIO read)
    // SCL_LOW
    // wait(clock_period)
    // SDA_LOW
    // if ack == 0 success else failure

    // The following state machine implements the same algorithm non-blocking
    // This function (bbi2c_handle_timeout) should be called when a wait is
    // finished.

}






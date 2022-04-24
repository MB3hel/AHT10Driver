/**
 * @file pc_comm.c
 * @author Marcus Behel (mgbehel@ncsu.edu)
 */

#include <pc_comm.h>
#include <msp430.h>
#include <circular_buffer.h>

////////////////////////////////////////////////////////////////////////////////
/// Macros
////////////////////////////////////////////////////////////////////////////////

#define WB_SIZE             32              // Write buffer size
#define RB_SIZE             32              // Read buffer size


#define ENABLE_TX_INT       IE2 |= UCA0TXIE
#define DISABLE_TX_INT      IE2 &= ~UCA0TXIE

////////////////////////////////////////////////////////////////////////////////
/// Globals
////////////////////////////////////////////////////////////////////////////////
volatile uint8_t pc_comm_rb_array[WB_SIZE]; // Backing array for read buffer
volatile uint8_t pc_comm_wb_array[RB_SIZE]; // Backing array for write buffer
volatile circular_buffer pc_comm_rb;        // Read circular (ring) buffer
volatile circular_buffer pc_comm_wb;        // Write circular (write) buffer


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

void pc_comm_init(unsigned int baud){
    // Initialize circular buffers
    cb_init(&pc_comm_rb, pc_comm_rb_array, RB_SIZE);
    cb_init(&pc_comm_wb, pc_comm_wb_array, WB_SIZE);

    // Configure UCA0 for UART
    UCA0CTL1 = UCSWRST;                 // Put USCI module in reset state
    UCA0CTL0 = 0x00;

    UCA0CTL0 &= ~UCPEN;                 // Disable parity
    UCA0CTL0 &= ~UCMSB;                 // LSB first
    UCA0CTL0 &= ~UC7BIT;                // 8-bit data mode
    UCA0CTL0 &= ~UCSPB;                 // One stop bit
    UCA0CTL0 |= UCMODE_0;               // UART mode
    UCA0CTL0 &= ~UCSYNC;                // Asynchronous mode
    UCA0CTL1 |= UCSSEL_2;               // SMCLK as BRCLK src (1MHz)

    pc_comm_set_buad(baud);             // Baud rate configuration

    IE2 &= ~UCA0TXIE;                   // Disable TX interrupt
    IE2 &= ~UCA0RXIE;                   // Disable RX interrupt

    UCA0CTL1 &= ~UCSWRST;               // Take out of reset

    IFG2 &= ~UCA0RXIFG;                 // Clear RX interrupt flag
    IE2 |= UCA0RXIE;                    // Enable RX interrupt

    IFG2 |= UCA0TXIFG;                  // Set TX interrupt flag
    // Leave TX interrupt disabled
}

void pc_comm_set_buad(unsigned int baud){
    // For details on where the baud rate settings come from see
    // https://www.ti.com/lit/ug/slau144j/slau144j.pdf
    // Calculations described on page 421
    // Page 424 has a helpful table
    switch(baud){
    default:
    case PC_COMM_BUAD_9600:
        UCA0BR1 = 0x00;                 // UCBRx = 104 (upper byte)
        UCA0BR0 = 0x68;                 // UCBRx = 104 (lower byte)
        UCA0MCTL = 0x02;                // UCBRFx = 0, UCBRSx = 1, UCOS16 = 0
        break;
    }
}

unsigned int pc_comm_write_str(char *str){
    unsigned int pos = 0;
    while(true){
        if(str[pos] == '\0')
            break;
        if(!cb_write_byte(&pc_comm_wb, (uint8_t)str[pos]))
            break;
        ++pos;
    }
    ENABLE_TX_INT;
    return pos;
}

bool pc_comm_write_byte(uint8_t b){
    bool res = cb_write_byte(&pc_comm_wb, b);
    ENABLE_TX_INT;
    return res;
}

unsigned int pc_comm_write_bytes(uint8_t *data, unsigned int len){
    unsigned int res = cb_write(&pc_comm_wb, data, len);
    ENABLE_TX_INT;
    return res;
}

bool pc_comm_read_byte(uint8_t *dest){
    return cb_read_byte(&pc_comm_rb, dest);
}

unsigned int pc_comm_read_bytes(uint8_t *dest, unsigned int count){
    return cb_read(&pc_comm_rb, dest, count);
}

inline void pc_comm_handle_write(void){
    uint8_t b;
    cb_read_byte(&pc_comm_wb, &b);      // Read next byte
    if(cb_empty(&pc_comm_wb))
        DISABLE_TX_INT;                 // Nothing left. Don't run ISR next time
                                        // IFG gets set when this byte done
                                        // So next time write is called and ISR
                                        // enabled, IFG will be set to start tx
    UCA0TXBUF = b;                      // Put next byte in TXBUF
}

inline void pc_comm_handle_read(void){
    cb_write_byte(&pc_comm_rb, UCA0RXBUF);
}

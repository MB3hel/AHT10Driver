/**
 * @file uca0uart.c
 * @author Marcus Behel (mgbehel@ncsu.edu)
 */

/*
 * MIT License
 *
 * Copyright (c) 2022 Marcus Behel
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <uca0uart.h>
#include <msp430.h>
#include <msp430helper.h>
#include <circular_buffer.h>

////////////////////////////////////////////////////////////////////////////////
/// Macros
////////////////////////////////////////////////////////////////////////////////

#define WB_SIZE             32              // Write buffer size
#define RB_SIZE             32              // Read buffer size

// Note: Which IE and IFG registers change if not using UCA0
#define ENABLE_TX_INT       IE2 |= UCA0TXIE
#define DISABLE_TX_INT      IE2 &= ~UCA0TXIE
#define ENABLE_RX_INT       IE2 |= UCA0RXIE
#define DISABLE_RX_INT      IE2 &= ~UCA0RXIE
#define SET_TX_IFG          IFG2 |= UCA0TXIFG
#define CLEAR_TX_IFG        IFG2 &= ~UCA0TXIFG
#define SET_RX_IFG          IFG2 |= UCA0RXIFG
#define CLEAR_RX_IFG        IFG2 &= ~UCA0RXIFG


////////////////////////////////////////////////////////////////////////////////
/// Globals
////////////////////////////////////////////////////////////////////////////////
volatile uint8_t uca0uart_rb_array[WB_SIZE]; // Backing array for read buffer
volatile uint8_t uca0uart_wb_array[RB_SIZE]; // Backing array for write buffer
volatile circular_buffer uca0uart_rb;        // Read circular (ring) buffer
volatile circular_buffer uca0uart_wb;        // Write circular (write) buffer


////////////////////////////////////////////////////////////////////////////////
/// Functions
////////////////////////////////////////////////////////////////////////////////

void uca0uart_init(unsigned int baud){
    // Initialize circular buffers
    cb_init(&uca0uart_rb, uca0uart_rb_array, RB_SIZE);
    cb_init(&uca0uart_wb, uca0uart_wb_array, WB_SIZE);

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

    uca0uart_set_buad(baud);             // Baud rate configuration

    DISABLE_TX_INT;                     // Disable TX interrupt
    DISABLE_RX_INT;                     // Disable RX interrupt

    UCA0CTL1 &= ~UCSWRST;               // Take out of reset

    CLEAR_RX_IFG;                       // Clear RX interrupt flag
    ENABLE_RX_INT;                      // Enable RX interrupt

    SET_TX_IFG;                         // Set TX interrupt flag
    // Leave TX interrupt disabled
}

void uca0uart_set_buad(unsigned int baud){
    // For details on where the baud rate settings come from see
    // https://www.ti.com/lit/ug/slau144j/slau144j.pdf
    // Calculations described on page 421
    // Page 424 has a helpful table
    switch(baud){
    default:
    case uca0uart_BUAD_9600:
        UCA0BR1 = 0x00;                 // UCBRx = 104 (upper byte)
        UCA0BR0 = 0x68;                 // UCBRx = 104 (lower byte)
        UCA0MCTL = 0x02;                // UCBRFx = 0, UCBRSx = 1, UCOS16 = 0
        break;
    }
}

unsigned int uca0uart_write_str(char *str){
    unsigned int pos = 0;
    while(true){
        if(str[pos] == '\0')
            break;
        if(!cb_write(&uca0uart_wb, (uint8_t)str[pos]))
            break;
        ++pos;
    }
    ENABLE_TX_INT;
    return pos;
}

bool uca0uart_write_byte(uint8_t b){
    bool res = cb_write(&uca0uart_wb, b);
    ENABLE_TX_INT;
    return res;
}

unsigned int uca0uart_write_bytes(uint8_t *data, unsigned int len){
    unsigned int pos = 0;
    while(true){
        if(pos == len)
            break;
        if(!cb_write(&uca0uart_wb, data[pos]))
            break;
        ++pos;
    }
    ENABLE_TX_INT;
    return pos;
}

bool uca0uart_read_byte(uint8_t *dest){
    return cb_read(&uca0uart_rb, dest);
}

unsigned int uca0uart_read_bytes(uint8_t *dest, unsigned int count){
    unsigned int pos = 0;
    while(true){
        if(pos == count)
            break;
        if(!cb_read(&uca0uart_rb, &dest[pos]))
            break;
        ++pos;
    }
    return pos;
}

void uca0uart_handle_write(void){
    uint8_t b;
    cb_read(&uca0uart_wb, &b);          // Read next byte
    if(CB_EMPTY(&uca0uart_wb))
        DISABLE_TX_INT;                 // Nothing left. Don't run ISR next time
                                        // IFG gets set when this byte done
                                        // So next time write is called and ISR
                                        // enabled, IFG will be set to start tx
    UCA0TXBUF = b;                      // Put next byte in TXBUF
}

void uca0uart_handle_read(void){
    cb_write(&uca0uart_rb, UCA0RXBUF);
}

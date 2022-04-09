/**
 * @file ports.c
 * @author Marcus Behel (mgbehel@ncsu.edu)
 */

#include <ports.h>
#include <msp430.h>


/**
 * Configure all pins on port 1
 */
void ports_init1(void){
    // Start with known initial state
    // All pins GPIO mode, configured as inputs with no resistors or interrupts
    P1SEL  = 0x00;
    P1SEL2 = 0x00;
    P1DIR  = 0x00;
    P1REN  = 0x00;
    P1IE   = 0x00;
    P1IFG  = 0x00;
    P1IES  = 0x00;
    P1OUT  = 0x00;
    ADC10AE0 = 0x00;
}

/**
 * Configures all pins on port 2
 */
void ports_init2(void){
    // Start with known initial state
    // All pins GPIO mode, configured as inputs with no resistors or interrupts
    P2SEL  = 0x00;
    P2SEL2 = 0x00;
    P2DIR  = 0x00;
    P2REN  = 0x00;
    P2IE   = 0x00;
    P2IFG  = 0x00;
    P2IES  = 0x00;
    P2OUT  = 0x00;

    P2SEL &= ~SCL;          // GPIO mode
    P2SEL2 &= ~SCL;         // GPIO mode
    P2DIR |= SCL;           // Output direction
    P2IE &= ~SCL;           // Disable interrupt
    P2OUT &= ~SCL;          // Initially low

    P2SEL &= ~SDA;          // GPIO mode
    P2SEL2 &= ~SDA;         // GPIO mode
    P2DIR |= SDA;           // Output direction
    P2IE &= ~SDA;           // Disable interrupt
    P2OUT &= ~SDA;          // Initially low
}

void ports_init(void){
    ports_init1();
    ports_init2();
}

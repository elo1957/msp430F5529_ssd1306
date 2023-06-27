/*
 * i2c.c
 */

#include "i2c.h"
#include <msp430f5529.h>
#include <msp430.h>
#include <stdint.h>


#define SDA BIT0                                                        // i2c sda pin
#define SCL BIT1                                                        // i2c scl pin
unsigned char TXByteCtr;
//4.1 SDA
//4.2 scl
void i2c_init(void) {
 //   PMAPRECFG=1;
   // P4MAP45|=SCL + SDA;                                  // Assign I2C pins to USCI_B0
   P3SEL|= SCL + SDA;
   // Assign I2C pins to USCI_B0
  // UCSCTL3 = SELREF__XT2CLK;   // select XT2 as FLL reference clock
  //  UCSCTL4|=SELS_5;
    UCB0CTL1 |= UCSWRST;                                                // Enable SW reset
    UCB0CTL0  |= UCMST + UCMODE_3 + UCSYNC;                              // I2C Master, synchronous mode
    UCB0CTL1  = UCSSEL_2 + UCSWRST;                                     // Use SMCLK, keep SW reset
    UCB0BR0   = 2;                                                     // fSCL = SMCLK/10 = ~100kHz with SMCLK = 1MHz
    UCB0BR1   = 0;
    UCB0CTL1 &= ~UCSWRST;
    UCB0I2CSA=0x78;// Clear SW reset, resume operation
   // UCTXIE      |= UCB0TXIE;                                               // Enable TX interrupt
   // UCB0IE = UCNACKIE;
    UCB0IE|=UCTXIE;
    UCB0IE |= UCSTPIE + UCSTTIE;


  /*  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
     P3SEL |= 0x03;                            // Assign I2C pins to USCI_B0
     UCB0CTL1 |= UCSWRST;                      // Enable SW reset
     UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
     UCB0CTL0&=~UCSLA10;   //7 bit address
     UCB0CTL0&=~UCA10;   //7 bit address
     UCB0CTL1 = UCSSEL_2;// + UCSWRST;            // Use SMCLK, keep SW reset
     UCB0BR0 = 10;                             // fSCL = SMCLK/12 = ~100kHz  12
     UCB0BR1 = 0;
    // UCB0I2CSA = 0x78;                         // Slave Address is 048h
     UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
     UCB0IE |= UCTXIE;                         // Enable TX interrupt
*/
} // end i2c_init

void TI_USCI_I2C_transmitinit(unsigned char slave_address,
                            unsigned char prescale){
    P4SEL |= SDA + SCL;                 // Assign I2C pins to USCI_B0
    UCB0CTL1 |= UCSWRST;                        // Enable SW reset
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;       // I2C Master, synchronous mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST;              // Use SMCLK, keep SW reset
    UCB0BR0 = prescale;                         // set prescaler
    UCB0BR1 = 0;
    UCB0I2CSA = slave_address;                  // Set slave address
    UCB0CTL1 &= ~UCSWRST;                       // Clear SW reset, resume operation
    UCB0IE = UCNACKIE;
    UCB0IE |= UCTXIE;                            // Enable TX ready interrupt
  }

void i2c_write(unsigned char slave_address, unsigned char *DataBuffer, unsigned char ByteCtr) {
    UCB0I2CSA = slave_address;
    UCB0I2CSA = slave_address;

      PTxData = DataBuffer;
      TxByteCtr = ByteCtr;

      while (UCB0CTL1 & UCTXSTP);                                         // Ensure stop condition got sent
      UCB0CTL1 |= UCTR + UCTXSTT;                                         // I2C TX, start condition
      while (UCB0CTL1 & UCTXSTP);                                         // Ensure stop condition got sent
      __bis_SR_register(CPUOFF + GIE);                                    // Enter LPM0 w/ interrupts
                                                                          // Remain in LPM0 until all data is TX'd
  }

  /* =============================================================================
   * The USCIAB0TX_ISR is structured such that it can be used to transmit any
   * number of bytes by pre-loading TXByteCtr with the byte count. Also, TXData
   * points to the next byte to transmit.
   * ============================================================================= */
  #pragma vector = USCI_B0_VECTOR
  __interrupt void USCI_B0_ISR(void) {
      if (TxByteCtr) {                                                    // Check TX byte counter
          UCB0TXBUF = *PTxData++;                                         // Load TX buffer
          TxByteCtr--;                                                    // Decrement TX byte counter
      } else {
          UCB0CTL1 |= UCTXSTP;                                            // I2C stop condition
          UCB0IFG &= ~UCTXIFG;                                             // Clear USCI_B0 TX int flag
          __bic_SR_register_on_exit(CPUOFF);                              // Exit LPM0
    }
}




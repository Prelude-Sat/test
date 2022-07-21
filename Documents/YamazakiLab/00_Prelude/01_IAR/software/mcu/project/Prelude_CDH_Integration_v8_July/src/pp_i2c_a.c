/***************************************************************************************
* @file     i2c_a.c
* @version  V1.4
* @date     2022 March 28
*
* @note
* Interrupt function / Reizy
*
* @par
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND BY 
* ALL THE TERMS AND CONDITIONS OF THE VORAGO TECHNOLOGIES END USER LICENSE AGREEMENT. 
* THIS SOFTWARE IS PROVIDED "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, 
* INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
* VORAGO TECHNOLOGIES SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
* INCIDENTAL, OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
*
****************************************************************************************/

#include "pp_adc.h"
#include "pp_i2c_a.h"

void I2CA_TC(uint8_t* I2C_DATA_TX, uint8_t I2C_WORDS_TX){
  init_i2ca();
  i2ca_write(I2C_DATA_TX,I2C_WORDS_TX);
}

void I2CA_TLM(uint8_t* I2C_DATA_RX, uint8_t* I2C_DATA_TX, uint8_t I2C_WORDS_RX){
  init_i2ca();
  i2ca_write(I2C_DATA_TX,1);  
  i2ca_read(I2C_DATA_RX, I2C_WORDS_RX);
}

uint32_t init_i2ca() {
  //  Steps: 
  //     0: Enable clocks to I2CA
  //     1: Load CLKSCALE to set bit rate  
  //     2: Clear Rx and Tx FIFO
  //     3: Load CTRL with parameters (dig filter enabled & enable bit) 
  
  VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |= CLK_ENABLE_I2CA  ;  
  VOR_I2CA -> CLKSCALE = I2CA_CLKSCALE_VAL  ;  // default to normal mode, divide by 25 for 50 MHz SysClk  
  VOR_I2CA -> FIFO_CLR =(I2CA_FIFO_CLR_RXFIFO_Msk | I2CA_FIFO_CLR_TXFIFO_Msk) ;// clear both Rx and Tx FIFO 
  VOR_I2CA -> CTRL =  (I2CA_CTRL_DLGFILTER_Msk | I2CA_CTRL_ENABLE_Msk)  ;   // enable filter and module 
  return(VOR_I2CA -> STATUS)  ; 
}

void i2ca_read(uint8_t* I2C_DATA, uint8_t I2C_WORDS){
  volatile int32_t count = 0, for_count = 0;
  VOR_I2CA -> WORDS   = I2C_WORDS  ;  
  VOR_I2CA -> ADDRESS = I2C_ADDR_RX  ;
  VOR_I2CA -> CMD     = I2C_START_STOP_CMD  ;  // start with stop
  while(( VOR_I2CA -> STATUS & I2CA_STATUS_IDLE_Msk ) == 0) // wait for transaction to complete
  {
    count++ ;  
    if (count > 0x100000) 
      break   ; 
  }
  for(for_count = 0; for_count < I2C_WORDS; for_count++){
    I2C_DATA[for_count] = VOR_I2CA -> DATA ;
  }
}

void i2ca_write(uint8_t* I2C_DATA, uint8_t I2C_WORDS){
  volatile int32_t count = 0, for_count;
  VOR_I2CA -> WORDS   = I2C_WORDS;
  VOR_I2CA -> ADDRESS = I2C_ADDR_TX;
  for(for_count = 0; for_count < I2C_WORDS; for_count++){
    VOR_I2CA -> DATA = I2C_DATA[for_count];
  }
  VOR_I2CA -> CMD     = I2C_START_STOP_CMD  ;  // start with stop
  while(( VOR_I2CA -> STATUS & I2CA_STATUS_IDLE_Msk ) == 0){ // wait for transaction to complete
    count++ ;  
    if (count > 0x100000) 
      break; 
  }
}
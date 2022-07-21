/***************************************************************************************
* @file     pp_spi.c
* @version  V4.1

* @date     2022 March 7
*
* @note
* Interrupt function /TOMMY 
*
* @par
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND BY 
ALL THE TERMS AND CONDITIONS OF THE VORAGO TECHNOLOGIES END USER LICENSE AGREEMENT. 
* THIS SOFTWARE IS PROVIDED "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, 
INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
* VORAGO TECHNOLOGIES SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
INCIDENTAL, OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.



*
****************************************************************************************/
#include "pp_spi.h"

volatile uint8_t bounce_cnt = 0; 
//volatile int _Cnt,  SPI_ISR_RXCNT ;                                            // Intterupt service routine
volatile int SPI_ISR_RXCNT ;
//unsigned char bf[32], spi_bf[132], RX_trg_remainder   ;                        // buffer_size , trigger
volatile signed int RX_trg_remainder;

uint8_t spi_bf[],*spi_bp  ;
//uint8_t *spi_bp  ;
volatile signed int spi_tx_cnt, spi_rx_cnt ;                                   // 送信・受信のカウント
int32_t systick_old, elapsed_ticks1, elapsed_ticks2, main_cnt, retVal; 
/**********************************************************************************************************************/
uint32_t  __attribute__((align_t(32))) READ_APB(volatile const uint32_t *ptr) {
  return *ptr;
}
void  __attribute__((align_t(32))) WRITE_APB(volatile uint32_t *ptr, uint32_t data) {
  *ptr=data;
}

//Set up SPIB
void REB1_SPIB_setup(int CSB){
  // enable clks to PORTIO, IOMGR and SPIB peripherals 
  VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |=(1<<PERIPH_CLK_ENAB_PORTIO|1<<PERIPH_CLK_ENAB_IOCONFIG|1<<PERIPH_CLK_ENAB_SPIB);
  
  VOR_IOCONFIG->PORTA[CSB] |= (FUNCSEL2 << IOCONFIG_PORTA_FUNSEL_Pos); // Set pin function for SPIB_SSEL0
  VOR_IOCONFIG->PORTA[18] |= (FUNCSEL2 << IOCONFIG_PORTA_FUNSEL_Pos); // Set pin function for SPIB_MISO
  VOR_IOCONFIG->PORTA[19] |= (FUNCSEL2 << IOCONFIG_PORTA_FUNSEL_Pos); // Set pin function for SPIB_MOSI
  VOR_IOCONFIG->PORTA[20] |= (FUNCSEL2 << IOCONFIG_PORTA_FUNSEL_Pos); // Set pin function for SPIB_SCLK
  
  //IOCONFIG_PORTA_FUNSEL_Pos = IOCONFIG_PORTB_FUNSEL_Pos = 13
  
//  VOR_IRQSEL->SPI[1] =   0xE ;  // assign SPIA int to NVIC entry E = 14decimal
//  NVIC_SetPriority (OC14_IRQn, 1) ; 
//  SPI_ISR_RXCNT = 0  ;          //  debug entry to help track entries into ISR
//  NVIC_EnableIRQ(OC14_IRQn)  ; // IRQ14
}
//Set up SPIA
void REB1_SPIA_setup(int CSA){ 
  // enable clks to PORTIO, IOMGR and SPIA peripherals 
  VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |= ((1<<PERIPH_CLK_ENAB_PORTIO)|(1<<PERIPH_CLK_ENAB_IOCONFIG)|(1<<PERIPH_CLK_ENAB_SPIA)); 
  
  VOR_IOCONFIG->PORTA[CSA] |= (FUNCSEL1 << IOCONFIG_PORTA_FUNSEL_Pos); // Set pin function for SPIA_SSEL0
  VOR_IOCONFIG->PORTA[30] |= (FUNCSEL1 << IOCONFIG_PORTA_FUNSEL_Pos); // Set pin function for SPIA_MISO
  VOR_IOCONFIG->PORTA[29] |= (FUNCSEL1 << IOCONFIG_PORTA_FUNSEL_Pos); // Set pin function for SPIA_MOSI
  VOR_IOCONFIG->PORTA[31] |= (FUNCSEL1 << IOCONFIG_PORTA_FUNSEL_Pos); // Set pin function for SPIA_SCLK
  
  //IOCONFIG_PORTA_FUNSEL_Pos = IOCONFIG_PORTB_FUNSEL_Pos = 13
  
}

void spi_frequency(int SELECT_PORT,uint8_t PRESCALE,uint8_t SCRDV,int slave){
  //SELECT_PORT;0->A,1->B?,2->C
  //slave==0 is master / slave ==1 is slave
  if(slave == 0){//master
    // enable clks to PORTIO, IOCONFIG and SPI peripherals
    if(SELECT_PORT == 0){
      VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |= ((1<<PERIPH_CLK_ENAB_PORTIO)|(1<<PERIPH_CLK_ENAB_IOCONFIG) 
                                               | (1<<PERIPH_CLK_ENAB_PORTA)|(1<<PERIPH_CLK_ENAB_PORTB)
                                                 | (1<<PERIPH_CLK_ENAB_IRQSEL)|(1<<PERIPH_CLK_ENAB_SPIA));// enable clks to PORTIO, IOCONFIG and SPIA peripherals 
      //VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |=((1 << PERIPH_CLK_ENAB_PORTIO)|(1 << PERIPH_CLK_ENAB_IOCONFIG)|(1 << PERIPH_CLK_ENAB_SPIA));

    }
    
    else if(SELECT_PORT == 1){
      VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |= ((1<<PERIPH_CLK_ENAB_PORTIO)|(1<<PERIPH_CLK_ENAB_IOCONFIG) 
                                               | (1<<PERIPH_CLK_ENAB_PORTA)|(1<<PERIPH_CLK_ENAB_PORTB)
                                                 | (1<<PERIPH_CLK_ENAB_IRQSEL)|(1<<PERIPH_CLK_ENAB_SPIC));
    }
    
    else if(SELECT_PORT == 2){
      VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |= ((1<<PERIPH_CLK_ENAB_PORTIO)|(1<<PERIPH_CLK_ENAB_IOCONFIG) 
                                               | (1<<PERIPH_CLK_ENAB_PORTA)|(1<<PERIPH_CLK_ENAB_PORTB)
                                                 | (1<<PERIPH_CLK_ENAB_IRQSEL)|(1<<PERIPH_CLK_ENAB_SPIC));
    }
    
    VOR_IRQSEL->SPI[SELECT_PORT] =   0xE ;  // assign SPIA int to NVIC entry E = 14decimal
    // NVICブロックはCPUに割込み信号を送り，どのサブルーチンか記憶させて置く必要があるため
    // →送信される固有の値を記憶させておく設定???
    
//    Name Address       offset  Description
//    SPI0 0x0000-0x0FFC SPI 0 ? SPIA
//    SPI1 0x1000-0x1FFC SPI 1 ? SPIB
//    SPI2 0x2000-0x2FFC SPI 2 ? SPIC
//         0x3000-0xFFFC Reserved
    
    NVIC_SetPriority (OC14_IRQn, 1) ; // 割込み定義名と優先度
    SPI_ISR_RXCNT = 0  ;          //  debug entry to help track entries into ISR
    NVIC_EnableIRQ(OC14_IRQn)  ; // 割込み定義名の有効化　今回はIRQ14
    
    VOR_SPI->BANK[SELECT_PORT].CLKPRESCALE = PRESCALE ; /*  PRESCALE -> Hz   */
    
    VOR_SPI->BANK[SELECT_PORT].CTRL0 = 
      (SCRDV<<SPI_PERIPHERAL_CTRL0_SCRDV_Pos) | (0x7 << SPI_PERIPHERAL_CTRL0_SIZE_Pos); //SCRDV = input, Size = 7

    //
    VOR_SPI->BANK[SELECT_PORT].CTRL0 &= ~(SPI_PERIPHERAL_CTRL0_SPO_Msk | SPI_PERIPHERAL_CTRL0_SPH_Msk); // Polarity and phase = 0x0	
    //  SPI clock equation = SYSCLK / (CLKPRESCALE * (SCRDV +1))

  }
  else if(slave == 1){//slave
    VOR_SPI->BANK[SELECT_PORT].CLKPRESCALE = PRESCALE ; /*  PRESCALE -> Hz   */
    
    VOR_SPI->BANK[SELECT_PORT].CTRL0 = 
      (SCRDV<<SPI_PERIPHERAL_CTRL0_SCRDV_Pos) | (0x7 << SPI_PERIPHERAL_CTRL0_SIZE_Pos); //SCRDV = input, Size = 7
    //SPI_PERIPHERAL_CTRL0_SIZE_Pos = 0x00                (0x07 <<)      0000 0000 0000 0000 0111
    
    VOR_SPI->BANK[SELECT_PORT].CTRL0 &= ~(SPI_PERIPHERAL_CTRL0_SPO_Msk | SPI_PERIPHERAL_CTRL0_SPH_Msk); // Polarity and phase = 0x0	
    //  SPI clock equation = SYSCLK / (CLKPRESCALE * (SCRDV +1))
  }
}

/* Set up SPI Frequency*/
void spi_frequency_2byte(int SELECT_PORT,uint8_t PRESCALE,uint8_t SCRDV,int slave){
  //SELECT_PORT;0->A,1->B?,2->C
  //slave==0 is master / slave ==1 is slave
  if(slave == 0){//master
    // enable clks to PORTIO, IOCONFIG and SPI peripherals
    if(SELECT_PORT == 0){
      VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |= ((1<<PERIPH_CLK_ENAB_PORTIO)|(1<<PERIPH_CLK_ENAB_IOCONFIG) 
                                               | (1<<PERIPH_CLK_ENAB_PORTA)|(1<<PERIPH_CLK_ENAB_PORTB)
                                                 | (1<<PERIPH_CLK_ENAB_IRQSEL)|(1<<PERIPH_CLK_ENAB_SPIA));// enable clks to PORTIO, IOCONFIG and SPIA peripherals 
      //VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |=((1 << PERIPH_CLK_ENAB_PORTIO)|(1 << PERIPH_CLK_ENAB_IOCONFIG)|(1 << PERIPH_CLK_ENAB_SPIA));

    }
    
    else if(SELECT_PORT == 1){
      VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |= ((1<<PERIPH_CLK_ENAB_PORTIO)|(1<<PERIPH_CLK_ENAB_IOCONFIG) 
                                               | (1<<PERIPH_CLK_ENAB_PORTA)|(1<<PERIPH_CLK_ENAB_PORTB)
                                                 | (1<<PERIPH_CLK_ENAB_IRQSEL)|(1<<PERIPH_CLK_ENAB_SPIC));
    }
    
    else if(SELECT_PORT == 2){
      VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |= ((1<<PERIPH_CLK_ENAB_PORTIO)|(1<<PERIPH_CLK_ENAB_IOCONFIG) 
                                               | (1<<PERIPH_CLK_ENAB_PORTA)|(1<<PERIPH_CLK_ENAB_PORTB)
                                                 | (1<<PERIPH_CLK_ENAB_IRQSEL)|(1<<PERIPH_CLK_ENAB_SPIC));
    }
    
    VOR_IRQSEL->SPI[SELECT_PORT] =   0xE ;  // assign SPIA int to NVIC entry E = 14decimal
    // NVICブロックはCPUに割込み信号を送り，どのサブルーチンか記憶させて置く必要があるため
    // →送信される固有の値を記憶させておく設定???
    
//    Name Address       offset  Description
//    SPI0 0x0000-0x0FFC SPI 0 ? SPIA
//    SPI1 0x1000-0x1FFC SPI 1 ? SPIB
//    SPI2 0x2000-0x2FFC SPI 2 ? SPIC
//         0x3000-0xFFFC Reserved
    
    NVIC_SetPriority (OC14_IRQn, 1) ; // 割込み定義名と優先度
    SPI_ISR_RXCNT = 0  ;          //  debug entry to help track entries into ISR
    NVIC_EnableIRQ(OC14_IRQn)  ; // 割込み定義名の有効化　今回はIRQ14
    
    VOR_SPI->BANK[SELECT_PORT].CLKPRESCALE = PRESCALE ; /*  PRESCALE -> Hz   */
    
    VOR_SPI->BANK[SELECT_PORT].CTRL0 = 
      (SCRDV<<SPI_PERIPHERAL_CTRL0_SCRDV_Pos) | (0xf << SPI_PERIPHERAL_CTRL0_SIZE_Pos); //SCRDV = input, Size = 7

    //
    VOR_SPI->BANK[SELECT_PORT].CTRL0 &= ~(SPI_PERIPHERAL_CTRL0_SPO_Msk | SPI_PERIPHERAL_CTRL0_SPH_Msk); // Polarity and phase = 0x0	
    //  SPI clock equation = SYSCLK / (CLKPRESCALE * (SCRDV +1))

  }
  else if(slave == 1){//slave
    VOR_SPI->BANK[SELECT_PORT].CLKPRESCALE = PRESCALE ; /*  PRESCALE -> Hz   */
    
    VOR_SPI->BANK[SELECT_PORT].CTRL0 = 
      (SCRDV<<SPI_PERIPHERAL_CTRL0_SCRDV_Pos) | (0xf << SPI_PERIPHERAL_CTRL0_SIZE_Pos); //SCRDV = input, Size = 7
    //SPI_PERIPHERAL_CTRL0_SIZE_Pos = 0x00                (0x07 <<)      0000 0000 0000 0000 0111
    
    VOR_SPI->BANK[SELECT_PORT].CTRL0 &= ~(SPI_PERIPHERAL_CTRL0_SPO_Msk | SPI_PERIPHERAL_CTRL0_SPH_Msk); // Polarity and phase = 0x0	
    //  SPI clock equation = SYSCLK / (CLKPRESCALE * (SCRDV +1))
  }
}

/*
:: SELECT_PORT -> 0->A,1->B,2->C
:: SS(Slave Select) -> 0,1,2,~,7
:: Slave -> Enable = 1
*/


void SPI_CTRL(int SELECT_PORT,int SS,int Slave){
  //int32_t i, wrt_buf_cnt=0, read_buf_cnt=0, busy_loop_cnt=0  ;
  if(Slave==0){
    VOR_SPI->BANK[SELECT_PORT].CTRL1 = (SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk | SS<<SPI_PERIPHERAL_CTRL1_SS_Pos 
                                        | SPI_PERIPHERAL_CTRL1_BLOCKMODE_Msk | SPI_PERIPHERAL_CTRL1_BMSTALL_Msk) ; 
    /* comment out
    //(SPI_PERIPHERAL_CTRL1_ENABLE_Msk | SS<<SPI_PERIPHERAL_CTRL1_SS_Pos | SPI_PERIPHERAL_CTRL1_BLOCKMODE_Msk) ; 
    //SPI_PERIPHERAL_CTRL1_ENABLE_Msk = 0x01UL | 0x00 << SPI_PERIPHERAL_CTRL1_SS_Pos = 4
    //    if(DS == 0){  
    //      VOR_SPI->BANK[SELECT_PORT].FIFO_CLR = (SPI_PERIPHERAL_FIFO_CLR_TXFIFO_Msk 
    //                                             | SPI_PERIPHERAL_FIFO_CLR_RXFIFO_Msk);// clear Tx & RX fifo
    //      VOR_SPI->BANK[SELECT_PORT].RXFIFOIRQTRG = 0x0C ;   //  setup RX half full to 12 bytes
    //      VOR_SPI->BANK[SELECT_PORT].TXFIFOIRQTRG = 0x10 ;   //  setup TX half empty to 16 bytes
    //      unsigned long sz = 32;
    //      sz = (sz + 3) & ~3 ;  // adjust size for words  (add 3 then clear 2 least sig bits)
    //      spi_tx_cnt = sz ;
    //      spi_rx_cnt = sz + 3 ;  //  rx cnt will have to account for wrt command + 2/3 address bytes	
    //      RX_trg_remainder = spi_rx_cnt%12 ;  // store remainder for use in ISR. volatile int32_t  busy_loop_cnt = 0  ;
    */
  }
  else if(Slave==1){
    //int32_t i, wrt_buf_cnt=0, read_buf_cnt=0, busy_loop_cnt=0  ;
    VOR_SPI->BANK[SELECT_PORT].CTRL1 = (1<<SPI_PERIPHERAL_CTRL1_MS_Pos |SS<<SPI_PERIPHERAL_CTRL1_SS_Pos | SPI_PERIPHERAL_CTRL1_BLOCKMODE_Msk);
//    //VOR_SPI->BANK[SELECT_PORT].CTRL1 |= SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk;  /* set pause bit  */ 
//    VOR_SPI->BANK[SELECT_PORT].CTRL1 |= (SPI_PERIPHERAL_CTRL1_ENABLE_Msk ) ;  // enable block  
//    //VOR_SPI->BANK[SELECT_PORT].CTRL1 &=   ~(SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk)  ;  // clr pause bit
//    //SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk = 0x01UL
//    VOR_SPI->BANK[SELECT_PORT].IRQ_ENB |= (SPI_PERIPHERAL_IRQ_ENB_RXIM_Msk | SPI_PERIPHERAL_IRQ_ENB_TXIM_Msk) ; 
//    // Enable RXIM and TXIM interrupts// Prepare for interrupt driven SPI data transfer★
//    //}
  }

  VOR_SPI->BANK[SELECT_PORT].FIFO_CLR = 
    (SPI_PERIPHERAL_FIFO_CLR_TXFIFO_Msk | SPI_PERIPHERAL_FIFO_CLR_RXFIFO_Msk);// clear Tx & RX fifo
  
  //printf("_PLEASE SET DATA_\r\n");
  // enable block and set stall bit
  // 割込み発生条件
  VOR_SPI->BANK[SELECT_PORT].RXFIFOIRQTRG = 0x0C ;   //  setup RX half full to 12 bytes
  VOR_SPI->BANK[SELECT_PORT].TXFIFOIRQTRG = 0x10 ;   //  setup TX half empty to 16 bytes
  
}

//SPI.WRITE(Input uint8_t DATA);
uint8_t spi_write(int SELECT_PORT,uint8_t data){
  VOR_SPI->BANK[SELECT_PORT].DATA = data;
//  return VOR_SPI->BANK[SELECT_PORT].DATA;
}

uint16_t spi_write_2byte(int SELECT_PORT,uint16_t data){
  VOR_SPI->BANK[SELECT_PORT].DATA = data;
//  return VOR_SPI->BANK[SELECT_PORT].DATA;
}

/*
:: SELECT_PORT -> 0->A,1->B,2->C
:: DS -> Low(0)is data set,High(1)is start com..
*/
void SPI_DATASET(int SELECT_PORT,unsigned long sz){
  volatile int32_t  busy_loop_cnt = 0  ; 
  
  //unsigned long sz ; 送信サイズを決定する定義
  sz = (sz + 3) & ~3 ;  // adjust size for words  (add 3 then clear 2 least sig bits)
  /* adjust
  ex. sz = 100;
      sz = (100 + 3) & ~3
      ~3 -> 11111100
     103 -> 01100111
            01100100 -> 100
  MAX sz = 252 
 */
  spi_tx_cnt = sz ;
  spi_rx_cnt = sz ;
  
  RX_trg_remainder = spi_rx_cnt%12 ;  // store remainder for use in ISR. volatile int32_t  busy_loop_cnt = 0  ;

  VOR_SPI->BANK[SELECT_PORT].CTRL1 |= (SPI_PERIPHERAL_CTRL1_ENABLE_Msk ) ;  // enable block

  VOR_SPI->BANK[SELECT_PORT].CTRL1 &=   ~(SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk)  ;  // clr pause bit
  
  //VOR_SPI->BANK[SELECT_PORT].IRQ_ENB |= (SPI_PERIPHERAL_IRQ_ENB_RXIM_Msk | SPI_PERIPHERAL_IRQ_ENB_TXIM_Msk) ; 
  // Enable RXIM and TXIM interrupts// Prepare for interrupt driven SPI data transfer★
  VOR_SPI->BANK[SELECT_PORT].CTRL1 &= ~(SPI_PERIPHERAL_CTRL1_BMSTALL_Msk);
  
  while((VOR_SPI->BANK[SELECT_PORT].STATUS & SPI_PERIPHERAL_STATUS_BUSY_Msk) !=0) {
    busy_loop_cnt ++ ; 
  }
}


void SPI_DATASET_SLAVE(int SELECT_PORT,unsigned long sz){
  volatile int32_t  busy_loop_cnt = 0  ; 
  
  //unsigned long sz ; 送信サイズを決定する定義
  sz = (sz + 3) & ~3 ;  // adjust size for words  (add 3 then clear 2 least sig bits)
  /* adjust
  ex. sz = 100;
      sz = (100 + 3) & ~3
      ~3 -> 11111100
     103 -> 01100111
            01100100 -> 100
  MAX sz = 252 
 */
  spi_tx_cnt = sz ;
  spi_rx_cnt = sz ;
  
  RX_trg_remainder = spi_rx_cnt%12 ;  // store remainder for use in ISR. volatile int32_t  busy_loop_cnt = 0  ;

  VOR_SPI->BANK[SELECT_PORT].CTRL1 |= (SPI_PERIPHERAL_CTRL1_ENABLE_Msk ) ;  // enable block
  
/** MASTERの場合 **/
//  VOR_SPI->BANK[SELECT_PORT].CTRL1 &=   ~(SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk)  ;  // clr pause bit
  
  /** 割込有効化 **/
  //VOR_SPI->BANK[SELECT_PORT].IRQ_ENB |= (SPI_PERIPHERAL_IRQ_ENB_RXIM_Msk | SPI_PERIPHERAL_IRQ_ENB_TXIM_Msk) ; 
  
  // Enable RXIM and TXIM interrupts// Prepare for interrupt driven SPI data transfer★
  VOR_SPI->BANK[SELECT_PORT].CTRL1 &= ~(SPI_PERIPHERAL_CTRL1_BMSTALL_Msk);
  
  while((VOR_SPI->BANK[SELECT_PORT].STATUS & SPI_PERIPHERAL_STATUS_BUSY_Msk) !=0) {
    busy_loop_cnt ++ ; 
  }
}


//SPI.READ(Input unsigned char *Read_Buffer);
uint8_t spi_read(int SELECT_PORT,uint8_t* rbp,int bytes){
  for(int r=0;r < bytes+1 ;r++){
    *rbp = VOR_SPI->BANK[SELECT_PORT].DATA ;
    rbp++;
  }
}

uint16_t spi_read_2byte(int SELECT_PORT,uint16_t* rbp,int bytes){
  for(int r=0;r < bytes+1 ;r++){
    *rbp = VOR_SPI->BANK[SELECT_PORT].DATA ;
    rbp++;
  }
}

//spi_interrupts function
void OC14_IRQHandler(void)
{
  int32_t  i ;
  if(VOR_SPI->BANK[0].IRQ_END & SPI_PERIPHERAL_IRQ_END_RXIM_Msk)  
    // Receive FIFO half full
    // IRQ_END <- Enable IRQ Status
    // SPI_PERIPHERAL_IRQ_END_RXIM_Msk <- Receive FIFO is at least half full.
    // Half full is defined as FIFO count >= RXFIFOIRQTRG.
    // 12byte のFIFOがたまったらこの条件に当てはまる.
  {
    printf("R11\n");// ここまでの確認OK
    
    if(spi_rx_cnt <= 0)    // disable interrupts when cnt = 0 
    {
      VOR_SPI->BANK[0].IRQ_ENB &= ~(SPI_PERIPHERAL_IRQ_ENB_RXIM_Msk); // Disable TXIM interrupts
      printf("FINISH RX PROCESS\n");
    }
    /*********************************************************************************************/
    //このループ処理に入いるけど*spi_bpにデータを入れる所で止まっているのが問題.
    for(i=0; i<12 ; i++)
    {
      *spi_bp = VOR_SPI->BANK[0].DATA ;
      spi_bp ++ ;
      spi_rx_cnt -- ;
    }
    if (spi_rx_cnt <= 15)  // When rx_cnt < 12,  adjust water mark to remainder value 
    {
      VOR_SPI->BANK[0].RXFIFOIRQTRG = RX_trg_remainder ;   //set RX half full to 11 bytes
      printf("R44\n");
    }
  }  // foot of RX portion 
  /*************************************************************************************************/
  if((VOR_SPI->BANK[0].IRQ_END & SPI_PERIPHERAL_IRQ_END_TXIM_Msk) && (spi_tx_cnt > 0))//Tx FIFO empty  
  {
    VOR_SPI->BANK[0].CTRL1 |= (SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk); //pause tx while FIFO updated
    for(i=0; i<16 ; i++)   //  load transmit buffer with 16 bytes 
    {
      if (spi_tx_cnt == 1)     // this is last byte to be sent, must set BMSTOP bit 
      {
        VOR_SPI->BANK[0].DATA = (SPI_PERIPHERAL_DATA_BMSTOP_Msk | i) ; 
        spi_tx_cnt --  ; 
        printf("PHASE:2.3:%d\n",spi_tx_cnt);
        printf("***** FINISH TRANS DATA ***** \r\n");
        VOR_SPI->BANK[0].IRQ_ENB &= ~(SPI_PERIPHERAL_IRQ_ENB_TXIM_Msk); // Disable TXIM interrupts
        spi_tx_cnt --  ;
        printf("PHASE:2.3:%d\n",spi_tx_cnt);
        break;
      }
      else if(spi_tx_cnt > 1){
        VOR_SPI->BANK[0].DATA = (i) ;
        spi_tx_cnt --  ;
      }
      else
      {
//        VOR_SPI->BANK[0].DATA = (i) ;
//        spi_tx_cnt --  ;
        printf("***** NO DATA TRANS ***** \r\n");
      }
    }
    VOR_SPI->BANK[0].CTRL1 &= ~(SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk) ;// clear tx pause bit
    printf("TTT \n");
  }
  VOR_SPI->BANK[0].IRQ_CLR = 0x3  ;  //clear the receive overrun and receive timeout bits
}		//  foot of ISR
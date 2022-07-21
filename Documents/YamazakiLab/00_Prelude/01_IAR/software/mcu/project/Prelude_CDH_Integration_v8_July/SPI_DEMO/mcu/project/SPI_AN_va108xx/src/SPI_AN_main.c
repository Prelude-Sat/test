/***************************************************************************************
 * @file     SPI_AN_main.c
 * @version  V1.0.9
 * @date     29 April 2016
 *
 * @note
 * VORAGO Technologies 
 *
 * @note
 * Copyright (c) 2013-2016 VORAGO Technologies. 
 *
 * @par
 * BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND BY 
 * ALL THE TERMS AND CONDITIONS OF THE VORAGO TECHNOLOGIES END USER LICENSE AGREEMENT. 
 * THIS SOFTWARE IS PROVIDED "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. VORAGO TECHNOLOGIES 
 * SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL 
 * DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ****************************************************************************************/
#include <stdio.h>
#include "va108xx.h"
#include "reb_log.h"
#include "reb_board.h"
#include "reb_adt75.h"
#include "driver_common.h"
#include "reb_max11619.h"
#include "reb_timer.h"
#include "gpio_va108xx.h"
#include "segger_rtt.h" 
#include "irq_va108xx.h"

volatile uint8_t bounce_cnt = 0; 

//**************************************************************
//  Global variables declared here  
//
//**************************************************************

volatile int _Cnt,  SPI_ISR_RXCNT ;
unsigned char bf[32], spi_bf[132], RX_trg_remainder   ; 
unsigned char *spi_bp  ; 
volatile signed int spi_tx_cnt, spi_rx_cnt ;  

//**** global variable defs

#define PERIPH_CLK_ENAB_PORTA 0x00 
#define PERIPH_CLK_ENAB_PORTB 0x01
#define PERIPH_CLK_ENAB_SPIA 0x04
#define PERIPH_CLK_ENAB_SPIB 0x05
#define PERIPH_CLK_ENAB_SPIC 0x06
#define PERIPH_CLK_ENAB_UARTA 0x8
#define PERIPH_CLK_ENAB_UARTB 0x9
#define PERIPH_CLK_ENAB_I2CA 0x10
#define PERIPH_CLK_ENAB_I2CB 0x11
#define PERIPH_CLK_ENAB_IRQSEL 0x15
#define PERIPH_CLK_ENAB_IOCONFIG 0x16
#define PERIPH_CLK_ENAB_UTILITY 0x17
#define PERIPH_CLK_ENAB_PORTIO 0x18 
#define FUNCSEL0 00
#define FUNCSEL1 01 
#define FUNCSEL2 02 
		//  *** Cypress FM25V10 (SPI FRAM) device command set: 40 MHz access  ****		
#define FRAM_WREN 0x06
#define FRAM_WRDI 0x04
#define FRAM_RDSR 0x05
#define FRAM_WRSR 0x01
#define FRAM_READ 0x03
#define FRAM_WRITE 0x02
#define FRAM_RDID 0x9F 
		
		// Address masks and macros for creating byte by byte address declarations
#define FADDR_MSB_MASK   (uint32_t)0xFF0000
#define FADDR_MID_MASK   (uint32_t)0x00FF00
#define FADDR_LSB_MASK   (uint32_t)0x0000FF
#define FMSB_ADDR_BYTE(addr)   ((uint8_t)((addr & FADDR_MSB_MASK)>>16))
#define FMID_ADDR_BYTE(addr)   ((uint8_t)((addr & FADDR_MID_MASK)>>8))
#define FLSB_ADDR_BYTE(addr)   ((uint8_t)(addr & FADDR_LSB_MASK))



#define SPI_PERIPHERAL_CTRL1_BLOCKMODE_Pos     7                                                       /*!< SPI_PERIPHERAL CTRL1: BYTEMODE Position */
#define SPI_PERIPHERAL_CTRL1_BLOCKMODE_Msk     (0x01UL << SPI_PERIPHERAL_CTRL1_BLOCKMODE_Pos)
   
  // *********************

uint32_t  __attribute__((aligned(32))) READ_APB(volatile const uint32_t *ptr) {
	 return *ptr;
}

void  __attribute__((aligned(32))) WRITE_APB(volatile uint32_t *ptr, uint32_t data) {
	 *ptr=data;
}










int Single_Frame_TX_SPIA (void) 
{
	//  ******  Single frame    *********     
	
		// 
		//   Basic flow is this:
		//      1) init clocks for peripherals
		//			2) init SPI control registers (0 & 1)  
		//      3) send Write enable command (single byte)  {wait for transmit complete} 
		//      4) wait for frame to complete
		  
volatile		 int32_t i, busy_loop_cnt, FRAM_SR  ; 
	  
			
		VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |= 
	       ((1<<PERIPH_CLK_ENAB_PORTIO) | (1<<PERIPH_CLK_ENAB_IOCONFIG) 
	       | (1<<PERIPH_CLK_ENAB_SPIA)); // enable clks to PORTIO, IOCONFIG and SPIA peripherals  
	
		VOR_SPI->BANK[0].CLKPRESCALE = 0x2 ; /*  PRESCALE = 0x2   */  

	  VOR_SPI->BANK[0].CTRL0 = 
	     (0x4<<SPI_PERIPHERAL_CTRL0_SCRDV_Pos) | (0x7<<SPI_PERIPHERAL_CTRL0_SIZE_Pos); //SCRDV = 4, Size = 7
     VOR_SPI->BANK[0].CTRL0 &= 	~(SPI_PERIPHERAL_CTRL0_SPO_Msk | SPI_PERIPHERAL_CTRL0_SPH_Msk); // Polarity and phase = 0x0	
	//  SPI clock equation = SYSCLK / (CLKPRESCALE * (SCRDV +1)) = 50MHz / (2 * ( 4+ 1)) = 5 MHz
	
	  VOR_SPI->BANK[0].CTRL1 = 
	   (SPI_PERIPHERAL_CTRL1_ENABLE_Msk | 0x00<<SPI_PERIPHERAL_CTRL1_SS_Pos) ; //  set Enable=1 and SS=0x0. 
 	
		VOR_SPI->BANK[0].FIFO_CLR = 
				(SPI_PERIPHERAL_FIFO_CLR_TXFIFO_Msk | SPI_PERIPHERAL_FIFO_CLR_RXFIFO_Msk);// clear Tx & RX fifo 	
	
			// ** Send Write enable command - single byte    *** 

		VOR_SPI->BANK[0].CTRL1 |= SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk;  /* set pause bit  */  	
		VOR_SPI->BANK[0].DATA =   FRAM_WREN   ; // Write enable = 0x06
		VOR_SPI->BANK[0].CTRL1 &=   ~(SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk)  ;  /* clr pause bit  */

while((VOR_SPI->BANK[0].STATUS & SPI_PERIPHERAL_STATUS_BUSY_Msk) !=0) 
				{
					busy_loop_cnt ++ ;  
				}
	   i = VOR_SPI->BANK[0].DATA  ; 			
//  ***   Transmit 16 bit frame	(size = 0xF) to read FRAM Status Register   ***  	
				VOR_SPI->BANK[0].CTRL0 = 
	     (0x4<<SPI_PERIPHERAL_CTRL0_SCRDV_Pos) | (0xF<<SPI_PERIPHERAL_CTRL0_SIZE_Pos); //SCRDV = 9, Size = 7
     
				// ** Send Write enable command - single byte    *** 
				
				VOR_SPI->BANK[0].FIFO_CLR = 
				(SPI_PERIPHERAL_FIFO_CLR_TXFIFO_Msk | SPI_PERIPHERAL_FIFO_CLR_RXFIFO_Msk);// clear Tx & RX fifo 	

				VOR_SPI->BANK[0].CTRL1 |= (SPI_PERIPHERAL_CTRL1_ENABLE_Msk
				   | SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk) ;  /* enable block and set pause bit  */  	
				VOR_SPI->BANK[0].DATA =   (FRAM_RDSR<<8)   ; // Put Read Status Register in MSB 
				VOR_SPI->BANK[0].CTRL1 &=   ~(SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk)  ;  /* clr pause bit  */
	  
				while((VOR_SPI->BANK[0].STATUS & SPI_PERIPHERAL_STATUS_BUSY_Msk) !=0) 
				{
					busy_loop_cnt ++ ;  
				}
				
				FRAM_SR = VOR_SPI->BANK[0].DATA  ;  
				busy_loop_cnt --  ;  
		return(FRAM_SR) ;  
	}

	//*************************************************************************
	
#define SPIC_DATA_BMSTOP_Pos                31                                                       /*!< SPIC DATA: BMSTOP Position            */
#define SPIC_DATA_BMSTOP_Msk                (0x01UL << SPIC_DATA_BMSTOP_Pos)                         /*!< SPIC DATA: BMSTOP Mask                */

#define FRAM_RDSR_WIP_Pos                   0                                                        /*!<            */
#define FRAM_RDSR_WIP_Msk                   (0x01UL << FRAM_RDSR_WIP_Pos)                            /*!<              */


/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */


//*************************************************************************
	
int ProgramPage_SPIA (unsigned long adr, unsigned long sz, unsigned char *buf) 
{
	//  ******   Code for FRAM write   *********     
	
		// specific code for data to FRAM  bytes to SPI-C is contained here
		//  Note: SPIC had dedicated pins and there is not pin setup code required 
		//   Basic flow is this:
		//      1) init clocks for peripherals
		//			2) init SPIC control registers (0 & 1)  
		//      3) send Write enable command (single byte)  {wait for transmit complete} 
		//      4) send Write command (1byte), Address (3Bytes), Data (4 bytes) and wait for data to be transmitted.  {wait for transmit complete} 
		  
 
			
		VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |= 
	       (PERIPH_CLK_ENAB_PORTIO | PERIPH_CLK_ENAB_IOCONFIG | PERIPH_CLK_ENAB_SPIC); // enable clks to PORTIO, IOMGR and SPIB peripherals  
	
		VOR_SPI->BANK[0].CLKPRESCALE = 0x1 ; /*  PRESCALE = 0x1   */  

	  VOR_SPI->BANK[0].CTRL0 = 
	     (0x9<<SPI_PERIPHERAL_CTRL0_SCRDV_Pos) | (0x7<<SPI_PERIPHERAL_CTRL0_SIZE_Pos); //SCRDV = 9, Size = 7
     VOR_SPI->BANK[0].CTRL0 &= 	~(SPI_PERIPHERAL_CTRL0_SPO_Msk | SPI_PERIPHERAL_CTRL0_SPH_Msk); // Polarity and phase = 0x0	
	//  SPI clock equation = SYSCLK / (CLKPRESCALE * (SCRDV +1)) = 50MHz / (1 * ( 9+ 1)) = 5 MHz
	
	  VOR_SPI->BANK[0].CTRL1 = 
	   (SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk | 0x00<<SPI_PERIPHERAL_CTRL1_SS_Pos | SPI_PERIPHERAL_CTRL1_BLOCKMODE_Msk) ; // set blockmode, set MTXPAUSE and SS=0x0. 
 	
		VOR_SPI->BANK[0].FIFO_CLR = 
				(SPI_PERIPHERAL_FIFO_CLR_TXFIFO_Msk | SPI_PERIPHERAL_FIFO_CLR_RXFIFO_Msk)  ;  // clear Tx & RX fifo 	

// #include "RTE_Components.h"             // Component selection
	
    sz = (sz + 3) & ~3 ;  // adjust size for words  (add 3 then clear 2 least sig bits)		
		
		while(sz) 
		{
			// ** Send Write enable command    *** 
				// configure SPI for:  Block mode, Enabled and block mode stall  	
				VOR_SPI->BANK[0].CTRL1|= (SPI_PERIPHERAL_CTRL1_BLOCKMODE_Msk 
			                           | SPI_PERIPHERAL_CTRL1_ENABLE_Msk
			                           | SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk);  /* enable block and set stall bit  */  
				VOR_SPI->BANK[0].FIFO_CLR = (SPIC_FIFO_CLR_RXFIFO_Msk | SPIC_FIFO_CLR_TXFIFO_Msk)  ;  // clear Tx & RX fifo 
				//  Send single "Write enable" byte  	
				VOR_SPI->BANK[0].DATA =  FRAM_WREN   ; // Write enable = 0x06
				VOR_SPI->BANK[0].CTRL1 &= ~(SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk)  ;  /* clr pause bit  */  	
		 
		while (VOR_SPI->BANK[0].STATUS & SPI_PERIPHERAL_STATUS_BUSY_Msk ) { }    ; //wait for transaction to complete  
			// send write command (1byte) , address (3bytes) and data (4 bytes)	
				VOR_SPI->BANK[0].CTRL1 |=  (SPI_PERIPHERAL_CTRL1_BLOCKMODE_Msk 
		                             | SPI_PERIPHERAL_CTRL1_ENABLE_Msk
		                             | SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk) ;  /* enable block and set stall bit  */  
				VOR_SPI->BANK[0].FIFO_CLR = (SPIC_FIFO_CLR_RXFIFO_Msk | SPIC_FIFO_CLR_TXFIFO_Msk)  ;  // clear Tx & RX fifo 
		
				VOR_SPI->BANK[0].DATA = FRAM_WRITE  ; //  Write command 
//			VOR_SPI->BANK[0].DATA, FMSB_ADDR_BYTE(adr))   ; //  Address high byte (need this for mem >64k
				VOR_SPI->BANK[0].DATA = FMID_ADDR_BYTE(adr)  ; //  Address mid byte 
				VOR_SPI->BANK[0].DATA = FLSB_ADDR_BYTE(adr)   ; //  Address low byte
				VOR_SPI->BANK[0].DATA =  *buf   ; //  byte 1
						buf ++ ;  
				VOR_SPI->BANK[0].DATA =  *buf    ; //  byte 2
						buf ++ ; 
				VOR_SPI->BANK[0].DATA =  *buf    ; //  byte 3
						buf ++ ; 
				VOR_SPI->BANK[0].DATA =  *buf    ; //  byte 4
						buf ++ ; 
				VOR_SPI->BANK[0].DATA =  *buf   ; //  byte 5
						buf ++ ;  
				VOR_SPI->BANK[0].DATA =  *buf    ; //  byte 6
						buf ++ ; 
				VOR_SPI->BANK[0].DATA =  *buf    ; //  byte 7
						buf ++ ; 
				VOR_SPI->BANK[0].DATA =  *buf    ; //  byte 8
						buf ++ ; 
			
				VOR_SPI->BANK[0].CTRL1 &=   ~(SPIC_CTRL1_MTXPAUSE_Msk);  /* clr stall bit  */  	
					
				while (READ_APB(&VOR_SPI->BANK[0].STATUS) & SPI_PERIPHERAL_STATUS_BUSY_Msk ) {  }    ;  // wait here until 12 bytes are fully transmitted.

				adr += 8 ;  
				sz -= 8 ; 
		
	   }
		
		/*   ***** This is end of EE Program section  ****    */ 
 // 
  return (0);                                  
}

//*************************************************************************
	
unsigned long Read_32bytes_from_EE(unsigned long adr, unsigned long sz, unsigned char *bp) // Verify Function
{
	//  This routine will read 32 bytes from SPI memory and place the contents in a buffer 
	//      Input to routine:
	//          adr = address from which to start the read process
	//          sz = size of memory read (for our case it will be 32)
	//  				*bp = pointer to buffer
	//      Data will be transmitted in one block transfer with Tx FIFO filled four times with writes of 16, 8, 8 and 4 bytes
	//         - the RxFiFo is read in 5 reads of 8, 8, 8, 8 and 4 bytes. 
	//      Data transmitted will be the read command followed by 3 byte address and  32 data bytes 
	//      RxFIFO is read when the RXhalf full flag is set and when the SPI_Busy flag is cleared. 
	
	int32_t i, wrt_buf_cnt=0, read_buf_cnt=0, busy_loop_cnt=0  ; 
	
		VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |= 
	       ((1<<PERIPH_CLK_ENAB_PORTIO) | (1<<PERIPH_CLK_ENAB_IOCONFIG) 
	        | (1<<PERIPH_CLK_ENAB_SPIC)); // enable clks to PORTIO, IOMGR and SPIB peripherals  
	
		VOR_SPI->BANK[2].CLKPRESCALE = 0x2 ; /*  PRESCALE = 0x2   */  

	    VOR_SPI->BANK[2].CTRL0 = 
	     (0x4<<SPI_PERIPHERAL_CTRL0_SCRDV_Pos) | (0x7<<SPI_PERIPHERAL_CTRL0_SIZE_Pos); //SCRDV = 4, Size = 7
        VOR_SPI->BANK[2].CTRL0 &= ~(SPI_PERIPHERAL_CTRL0_SPO_Msk | SPI_PERIPHERAL_CTRL0_SPH_Msk);//Polarity and phase = 0x0	
	//  SPI clock equation = SYSCLK / (CLKPRESCALE * (SCRDV +1)) = 50MHz / (2 * ( 4+ 1)) = 5 MHz
	
	    VOR_SPI->BANK[2].CTRL1 = 
                            (SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk | 0x00<<SPI_PERIPHERAL_CTRL1_SS_Pos 
                            | SPI_PERIPHERAL_CTRL1_BLOCKMODE_Msk) ; // set blockmode, set MTXPAUSE and SS=0x0. 
 	
		VOR_SPI->BANK[2].FIFO_CLR = 
                        (SPI_PERIPHERAL_FIFO_CLR_TXFIFO_Msk | SPI_PERIPHERAL_FIFO_CLR_RXFIFO_Msk)  ;  // clear Tx & RX fifo 	

        sz = (sz + 3) & ~3 ;  // adjust size for words  (add 3 then clear 2 least sig bits)		
		
		// Load TxFIFO with read command (1byte) , address (3bytes) and 12 dummy bytes data  {16 bytes in total}		
		VOR_SPI->BANK[2].DATA = FRAM_READ   ; 		//  Read command 
		VOR_SPI->BANK[2].DATA = FMSB_ADDR_BYTE(adr) ; //  Address high byte
		VOR_SPI->BANK[2].DATA =  FMID_ADDR_BYTE(adr); //  Address mid byte 
		VOR_SPI->BANK[2].DATA = FLSB_ADDR_BYTE(adr) ; //  Address low byte
			
		for(i=0; i<12 ; i++)   //  load transmit buffer with 12 bytes 
		{
			VOR_SPI->BANK[2].DATA = i; //  byte 1  (value of data is meaningless, just need to apply clocks for read) 
		}					 
		VOR_SPI->BANK[2].CTRL1 |= (SPI_PERIPHERAL_CTRL1_ENABLE_Msk ) ;  /* enable block  */  
		VOR_SPI->BANK[2].CTRL1 &= ~(SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk) ; // clear pause bit to allow transaction to start 
				
		while (VOR_SPI->BANK[2].STATUS & SPI_PERIPHERAL_STATUS_BUSY_Msk )  // stay in this while loop until full packet is sent
		{
			busy_loop_cnt ++; 
			if(VOR_SPI->BANK[2].STATUS & SPI_PERIPHERAL_STATUS_TXTRIGGER_Msk )  // TX FIFO half empty, shift out 8 more bytes 
				{
					if(wrt_buf_cnt < 3)  //  need to load TX_FIFO with 8 bytes , three times 
					{
						wrt_buf_cnt ++ ;  
						for(i=0; i<8 ; i++)   //  load transmit buffer with 8 more bytes 
						{
							WRITE_APB(&VOR_SPI->BANK[2].DATA, i); //(value is meaningless, just need to apply clocks for read) 
						}
					}
				}
			if(VOR_SPI->BANK[2].STATUS & SPI_PERIPHERAL_STATUS_RXTRIGGER_Msk )  // read 8 bytes from TX buffer
				{
					if(read_buf_cnt < 4) //  needs to read Rx FIFO
					{
						read_buf_cnt ++ ;
						for(i=0; i<8 ; i++)   
						{
                            *bp = VOR_SPI->BANK[2].DATA   ;
                            bp ++ ; 	
						}
					}
				}								
		}    ; //foot of while loop : wait for transaction to complete then read final 4 bytes in RX FIFO							
			for(i=0; i<4 ; i++)   
			{
				*bp = VOR_SPI->BANK[2].DATA   ;
				bp ++ ; 	
			}
						
  return (0);                                  
}   //  this is end of the read 32 byte read EEPROM routine 


//*************************************************************************


unsigned long Read_32bytes_wint(unsigned long adr, unsigned long sz, unsigned char *bp) // Read EE with Int for SPI
{
	//  This routine will read 32 bytes from SPI memory and place the contents in a buffer.
  //     Interrupts will be used to refill the TX FIFO and read the Rx FIFO
  //     The transmit buffer will be filled in 16 byte bursts (reload when TxFIFO empty)
  //     The receive buffer will be read in 12 byte bursts (reload when RxFIFO half empty = 4)	
	//      Input to routine:
	//          adr = address from which to start the read process
	//          sz = size of memory read (for our case it will be 32)
	//  				*bp = pointer to buffer
	//      This routine sets up the SPI and loads the address.  
	//      Data transmitted will be the read command followed by 3 byte address and  32 data bytes 
	//      RxFIFO is read when the RXhalf full flag is set and when the SPI_Busy flag is cleared. 
	
volatile int32_t  busy_loop_cnt = 0  ;  
	VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |= 
	       ((1<<PERIPH_CLK_ENAB_PORTIO) | (1<<PERIPH_CLK_ENAB_IOCONFIG) 
	        | (1<<PERIPH_CLK_ENAB_PORTA) | (1<<PERIPH_CLK_ENAB_PORTB)
	        | (1<<PERIPH_CLK_ENAB_IRQSEL) |(1<<PERIPH_CLK_ENAB_SPIA));// enable clks to PORTIO, IOCONFIG and SPIA peripherals  
	
	VOR_IRQSEL->SPI[0] =   0xE ;  // assign SPIA int to NVIC entry E = 14decimal
    NVIC_SetPriority (OC14_IRQn, 1) ; 
	SPI_ISR_RXCNT = 0  ;          //  debug entry to help track entries into ISR
	NVIC_EnableIRQ(OC14_IRQn)  ; // IRQ14  

	VOR_SPI->BANK[0].CLKPRESCALE = 0x2 ; /*  PRESCALE = 0x1   */  

	VOR_SPI->BANK[0].CTRL0 = 
	     (0x4<<SPI_PERIPHERAL_CTRL0_SCRDV_Pos) | (0x7<<SPI_PERIPHERAL_CTRL0_SIZE_Pos); //SCRDV = 9, Size = 7
    VOR_SPI->BANK[0].CTRL0 &= 	~(SPI_PERIPHERAL_CTRL0_SPO_Msk | SPI_PERIPHERAL_CTRL0_SPH_Msk); // Polarity and phase = 0x0	
	//  SPI clock equation = SYSCLK / (CLKPRESCALE * (SCRDV +1)) = 50MHz / (1 * ( 9+ 1)) = 5 MHz
	
	  VOR_SPI->BANK[0].CTRL1 = 
	     (SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk | 0x00<<SPI_PERIPHERAL_CTRL1_SS_Pos | 
		    SPI_PERIPHERAL_CTRL1_BLOCKMODE_Msk | SPI_PERIPHERAL_CTRL1_BMSTALL_Msk) ; // set blockmode, set MTXPAUSE and SS=0x0. 
 	
		VOR_SPI->BANK[0].FIFO_CLR = 
				(SPI_PERIPHERAL_FIFO_CLR_TXFIFO_Msk | SPI_PERIPHERAL_FIFO_CLR_RXFIFO_Msk)  ;  // clear Tx & RX fifo 	
  
	  VOR_SPI->BANK[0].RXFIFOIRQTRG = 0xC ;   //  setup RX half full to 12 bytes
	  VOR_SPI->BANK[0].TXFIFOIRQTRG = 0x1 ;   //  setup TX half empty to 16 bytes	

    sz = (sz + 3) & ~3 ;  // adjust size for words  (add 3 then clear 2 least sig bits)		
	  
			// Load TxFIFO with read command (1byte) , address (2/3bytes) 
				VOR_SPI->BANK[0].FIFO_CLR= (SPIC_FIFO_CLR_RXFIFO_Msk | SPIC_FIFO_CLR_TXFIFO_Msk);  // clear Tx & RX fifo 
		
				VOR_SPI->BANK[0].DATA= FRAM_READ   ; //  Read command 
//				WRITE_APB(&VOR_SPI->BANK[0].DATA, FMSB_ADDR_BYTE(adr)); //  Address high byte (need this line for memories over 64kbytes)
				VOR_SPI->BANK[0].DATA=  FMID_ADDR_BYTE(adr); //  Address mid byte 
				VOR_SPI->BANK[0].DATA=  FLSB_ADDR_BYTE(adr); //  Address low byte

// Prepare for interrupt driven SPI data transfer
				spi_tx_cnt = sz   ;  
				spi_rx_cnt = sz + 3     ;  //  rx cnt will have to account for wrt command + 2/3 address bytes
        RX_trg_remainder = spi_rx_cnt%12 ;  // store remainder for use in ISR.  
				
				VOR_SPI->BANK[0].CTRL1 |= (SPI_PERIPHERAL_CTRL1_ENABLE_Msk ) ;  // enable block    	
		    VOR_SPI->BANK[0].CTRL1 &= ~(SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk) ; // clear pause bit to allow transaction to start 
				VOR_SPI->BANK[0].IRQ_ENB |= (SPI_PERIPHERAL_IRQ_ENB_RXIM_Msk 
				                         | SPI_PERIPHERAL_IRQ_ENB_TXIM_Msk)  ; // Enable RXIM and TXIM interrupts

				while((VOR_SPI->BANK[0].STATUS & SPI_PERIPHERAL_STATUS_BUSY_Msk) !=0) 
				{
					busy_loop_cnt ++ ;  
				}
  return (0);                                  
}   //  this is end of the EERead32_wint routine  .  

//  ***************************************************************************
//   OC14_IRQHandler - aka SPI interrupt handler  
//       - will use the following global variables to determine activity
//            - spi_tx_cnt: when tx_cnt = 1, set "last data" bit in TX word 
//						- spi_rx_cnt: tracks received words
//						- RX_trg_remainder: adjusts trigger level for last Rx batch read 
 

#define SPI_PERIPHERAL_DATA_BMSTOP_Msk 0x80000000   // 

//**************************************************************
void OC14_IRQHandler(void)
{
int32_t  i    ;
   
	if(VOR_SPI->BANK[0].IRQ_END & SPI_PERIPHERAL_IRQ_END_RXIM_Msk)  // Receive FIFO half full
		{ 
			if(spi_rx_cnt <= 0)    // disable interrupts when cnt = 0 
			{
				VOR_SPI->BANK[0].IRQ_ENB &= ~(SPI_PERIPHERAL_IRQ_ENB_RXIM_Msk); // Disable TXIM interrupts
			}
			for(i=0; i<12 ; i++)   
			{
				*spi_bp = VOR_SPI->BANK[0].DATA   ;
				spi_bp ++ ; 
				spi_rx_cnt -- ;								
			}
			if (spi_rx_cnt <= 15)  // When rx_cnt < 12,  adjust water mark to remainder value 
			{
				VOR_SPI->BANK[0].RXFIFOIRQTRG = RX_trg_remainder ;   //set RX half full to 11 bytes
            }				 
		}  // foot of RX portion 
	if((VOR_SPI->BANK[0].IRQ_END & SPI_PERIPHERAL_IRQ_END_TXIM_Msk) && (spi_tx_cnt > 0))//Tx FIFO empty  
	{
		VOR_SPI->BANK[0].CTRL1 |= (SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk); //pause tx while FIFO updated
		 for(i=0; i<16 ; i++)   //  load transmit buffer with 16 bytes 
		{
			if (spi_tx_cnt == 1)     // this is last byte to be sent, must set BMSTOP bit 
			{
                VOR_SPI->BANK[0].DATA = (SPI_PERIPHERAL_DATA_BMSTOP_Msk | i) ; 
				spi_tx_cnt --  ;  
				VOR_SPI->BANK[0].IRQ_ENB &= ~(SPI_PERIPHERAL_IRQ_ENB_TXIM_Msk); // Disable TXIM interrupts
				spi_tx_cnt --  ;
			}
			else
			{
				VOR_SPI->BANK[0].DATA = (i) ;					 
				spi_tx_cnt --  ;  
			}
		}
        VOR_SPI->BANK[0].CTRL1 &= ~(SPI_PERIPHERAL_CTRL1_MTXPAUSE_Msk) ;// clear tx pause bit		
	}
	VOR_SPI->BANK[0].IRQ_CLR = 0x3  ;  //clear the receive overrun and receive timeout bits
 }		//  foot of ISR

// *************************************************************
void REB1_SPIB_setup(void) { 
	
	VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |= 
                    (PERIPH_CLK_ENAB_PORTIO | PERIPH_CLK_ENAB_IOCONFIG 
                    | PERIPH_CLK_ENAB_SPIB); // enable clks to PORTIO, IOMGR and SPIB peripherals 
	VOR_IOCONFIG->PORTA[17] |=  (FUNCSEL2 <<  IOCONFIG_PORTA_FUNSEL_Pos); // Set pin function for SPIB_SSEL0
	VOR_IOCONFIG->PORTA[18] |=  (FUNCSEL2 <<  IOCONFIG_PORTA_FUNSEL_Pos); // Set pin function for SPIB_MISO
	VOR_IOCONFIG->PORTA[19] |=  (FUNCSEL2 <<  IOCONFIG_PORTA_FUNSEL_Pos); // Set pin function for SPIB_MOSI
	VOR_IOCONFIG->PORTA[20] |=  (FUNCSEL2 <<  IOCONFIG_PORTA_FUNSEL_Pos); // Set pin function for SPIB_SCLK
}

void REB1_SPIA_setup(void) { 
	VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |= 
	                    ((1<<PERIPH_CLK_ENAB_PORTIO) | (1<<PERIPH_CLK_ENAB_IOCONFIG)
        	             |(1<<PERIPH_CLK_ENAB_SPIA)); // enable clks to PORTIO, IOMGR and SPIA peripherals 
	VOR_IOCONFIG->PORTA[28] |= (FUNCSEL1 << IOCONFIG_PORTA_FUNSEL_Pos); // Set pin function for SPIA_SSEL0
	VOR_IOCONFIG->PORTA[30] |= (FUNCSEL1 << IOCONFIG_PORTA_FUNSEL_Pos); // Set pin function for SPIA_MISO
	VOR_IOCONFIG->PORTA[29] |= (FUNCSEL1 << IOCONFIG_PORTA_FUNSEL_Pos); // Set pin function for SPIA_MOSI
	VOR_IOCONFIG->PORTA[31] |= (FUNCSEL1 << IOCONFIG_PORTA_FUNSEL_Pos); // Set pin function for SPIA_SCLK
}

void Output_data_RTT(unsigned long adr, unsigned long sz, unsigned char *buf) 
{
	int32_t i=0, jj, kk ;  

   for(jj=0; jj<sz/32; jj++ ) 
		 {
 			  SEGGER_RTT_printf(0, "ADR %5X : ", adr  )  ;
			
				SEGGER_RTT_printf(0, " %2X %2X %2X %2X", buf[i],buf[i+1],buf[i+2], buf[i+3] )  ;
				i+=4 ; 
				SEGGER_RTT_printf(0, " %2X %2X %2X %2X", buf[i],buf[i+1],buf[i+2], buf[i+3] )  ;
				i+=4 ;  
				SEGGER_RTT_printf(0, " %2X %2X %2X %2X", buf[i],buf[i+1],buf[i+2], buf[i+3] )  ;
				i+=4 ;  
				SEGGER_RTT_printf(0, " %2X %2X %2X %2X  \r\n", buf[i],buf[i+1],buf[i+2], buf[i+3] )  ;
			  i+=4 ;
				VOR_GPIO->BANK[0].TOGOUT |= (0x01UL << 10)   ; //drive 1 LED on 
			  adr += 16  ;  
			 	SEGGER_RTT_printf(0, "ADR %5X : ", adr  )  ;
			
				SEGGER_RTT_printf(0, " %2X %2X %2X %2X", buf[i],buf[i+1],buf[i+2], buf[i+3] )  ;
				i+=4 ; 
				SEGGER_RTT_printf(0, " %2X %2X %2X %2X", buf[i],buf[i+1],buf[i+2], buf[i+3] )  ;
				i+=4 ;  
				SEGGER_RTT_printf(0, " %2X %2X %2X %2X", buf[i],buf[i+1],buf[i+2], buf[i+3] )  ;
				i+=4 ;  
				SEGGER_RTT_printf(0, " %2X %2X %2X %2X  \r\n", buf[i],buf[i+1],buf[i+2], buf[i+3] )  ;
				i+=4 ;
			  adr += 16  ;
			  for(kk=0;kk<0x8200;kk++)  {   }    //  wait a while to let buffer clear 
		 }
 }

//------------------------------------------------------------------------------
// Main Test Program
//  Setup LED and ADC  
//  continually convert ADC   
//------------------------------------------------------------------------------

uint32_t ctr = 0, tmp_RESET_STATUS, EE_STAT;
 
char abDataIn[128], abDataOut[128] ;   //  RTT (real time terminal) buffer allocation

#define SEGGER_RTT_IN_RAM 1
int main(void) 
{
		int32_t i =0, kk; 
		int32_t systick_old, elapsed_ticks1, elapsed_ticks2, main_cnt, retVal; 
		unsigned long adr, sz ;
	  unsigned char buf[256],*ibuf, *iread_buf, first_pass ; 
	
 	 VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE |= 
	       ((1<<PERIPH_CLK_ENAB_PORTIO) | (1<<PERIPH_CLK_ENAB_IOCONFIG) 
	        | (1<<PERIPH_CLK_ENAB_PORTA) | (1<<PERIPH_CLK_ENAB_PORTB)
	        | (1<<PERIPH_CLK_ENAB_IRQSEL) |(1<<PERIPH_CLK_ENAB_SPIA)); // enable clks to PORTIO, IOMGR and SPIB peripherals 
	
	for(i=0; i<32; i++  )   //  config all port pins with internal pull resistor enabled 
		{
	    VOR_IOCONFIG->PORTA[i] |= IOCONFIG_PORTA_PEN_Msk  ;
			if(i<24) 
			{ 
				VOR_IOCONFIG->PORTB[i] |= IOCONFIG_PORTB_PEN_Msk  ;
			}
		}
		VOR_GPIO->BANK[0].DIR |= ( (0x01UL << PORTA_6_D4) |(0x01UL << PORTA_7_D3) | (0x01UL << PORTA_10_D2) ) ; // setup REB1 LED pins  : PORTA 6,7,10
		VOR_GPIO->BANK[0].DATAOUT &= ~((0x01UL << PORTA_6_D4) |(0x01UL << PORTA_7_D3)  )  ; //drive  2 LEDs off
		VOR_GPIO->BANK[0].DATAOUT |= (0x01UL << PORTA_10_D2)   ; //drive 1 LED on 

		SysTick -> CTRL = (SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk) ; /*  enable Systick counter  */

		SysTick -> LOAD =  0x7fffff ; 
    systick_old = SysTick ->VAL ; 
		
		SEGGER_RTT_Init() ; //  Must call this for RAM based apps 
	  retVal = SEGGER_RTT_ConfigUpBuffer(1, "DataOut", &abDataOut, sizeof(abDataOut), SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
		retVal = SEGGER_RTT_ConfigDownBuffer(1, "DataIn", &abDataIn[0], sizeof(abDataIn), SEGGER_RTT_MODE_NO_BLOCK_SKIP);
 
		
		SEGGER_RTT_printf(0, "\n\n*****  AN1201 Example code terminal output  *****\n\n")  ; 	

		
// **  Example 1 of AN1201 starts here 
		SEGGER_RTT_printf(0, "\n\n>>>>>  Example 1 - Read of FRAM Status Register\n")  ; 
    REB1_SPIA_setup() ;   // setup port pins for SPIA on PORTA28 (SS0), 29-31  (connection to FRAM)
	  kk = Single_Frame_TX_SPIA() ;   // call subroutine to transmit single frame (AN exercise 1)
    if(kk!= 0)
		{
      SEGGER_RTT_printf(0, "FRAM Status register = %5X \n\n", kk  )  ;
		}
//  **  Example 2 of AN starts here   **  
		SEGGER_RTT_printf(0, "\n\n>>>>>  Example 2 - 32 byte read from EEPROM (polling method) \n")  ; 
		adr = 0x0000  ;  
		sz = 32  ;  
		ibuf = buf ;  
		Read_32bytes_from_EE(adr, sz, ibuf)  ; 
		SEGGER_RTT_printf(0, "\n   +++ 32bytes from Boot EEPROM (polling method)  +++\n")  ; 
		SEGGER_RTT_printf(0, "\n  Note: First four bytes account for READ command and 3 address  bytes. \n\n")  ; 
     sz = 64   ;   
   	Output_data_RTT(adr, sz,  ibuf)  ;

    SEGGER_RTT_WriteString(0, ">>> Setup for example 3 - Memory pattern being written to SPI FRAM  \n\n");
		SEGGER_RTT_SetTerminal(0) ;	 
		
    ibuf = buf ;  // set pointer to first entry of buf[] array

//  setup array for call to ProgramPage
		adr = 0x1000 ; 
		sz = 256  ; 
		for(i=0; i< 256 ; i ++ )  { buf[i] = (0xFF - i) ; }  //  load buf array with non-zero value
		
		REB1_SPIA_setup() ;   // setup port pins for SPIA on PORTA28 (SS0), 29-31  (connection to FRAM)
		ProgramPage_SPIA (adr, sz, ibuf)  ; 
		SEGGER_RTT_printf(0, ">>  PROGRAM PAGE DATA\n\n")  ;
		Output_data_RTT(adr, sz,  ibuf)  ;
				
// ** Example 3 of AN1201 starts here  **  
		SEGGER_RTT_printf(0, "\n\n>>>>> Example 3 - 32 byte read from FRAM (interrupt driven)\n")  ; 			
		spi_bp = spi_bf  ;  //   setup global buffer for SPI reads from EEPROM
		sz = 32 ;  
		adr = 0x1010  ;  
		Read_32bytes_wint(adr, sz, iread_buf)  ;  
		sz = 64 ;  
		adr = 0x1010  ; 
		SEGGER_RTT_printf(0, "\n+++  READ 32bytes from FRAM DATA  +++\n")  ;
		SEGGER_RTT_printf(0, "   Note: First three bytes account for READ command and 2 address  bytes. \n\n")  ; 
    
		spi_bp = spi_bf  ; 
		Output_data_RTT(adr, sz,spi_bp)  ;  

     first_pass = 1  ;  
		  
		 while(1)     //  This is infinite loop  (reads FRAM and EEPROM from Examples 2 & 3 continuously) 
		 {
		 systick_old = SysTick ->VAL ;   // time stamp of system tick counter
		 spi_bp = spi_bf  ;              // setup global buffer for SPI reads from EEPROM 
		 adr = 0x1020  ;  
		 sz = 32  ;  
     Read_32bytes_wint( adr, sz, ibuf);
		 VOR_GPIO->BANK[0].TOGOUT = (0x01UL << 6) ; // toggle LEDs each time through 

		 elapsed_ticks1 = systick_old - SysTick ->VAL   ;
		 systick_old = SysTick ->VAL ;   // time stamp of system tick counter		 
		
     Read_32bytes_from_EE(adr, sz, ibuf)  ; 
		 elapsed_ticks2 = systick_old - SysTick ->VAL   ;
			 
		 if(first_pass == 1) 
			{
				SEGGER_RTT_printf(0, "\n\nDuration of 32 byte FRAM read (clock cycles)   %5d \n", elapsed_ticks1  )  ;
				SEGGER_RTT_printf(0, "Duration of 32 byte EEPROM read (clock cycles) %5d \n\n\n", elapsed_ticks2  )  ;
				SEGGER_RTT_printf(0, "*****  End of AN1201 Terminal output  **** \n\n"  )  ;
			
				first_pass = 0 ; 
			}
		
		 main_cnt ++ ; 
		}  //  end of while (1)   
		
				
	}  // end of main routine 
	
	
 
	






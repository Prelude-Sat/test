/***************************************************************************************
* @file     integration_main.c
* @version  V1.0
* @date     8. June 2022
* @note     Produced by Ryusuke Iwata 
* @note     add timer

****************************************************************************************/
#include <stdio.h>
#include "va108xx.h"
#include "reb_log.h"
#include "reb_board.h"
#include "driver_common.h"
#include "pp_timer_v2.0.h"
#include "gpio_va108xx.h"


#include "pp_spi_v2.0.h"
#include "pp_adc.h"
#include "pp_timer_v2.0.h"
/**** Global variebles defs ****/
int eps_get_HK_isr_flag = 0;
int adc_get_HK_isr_flag = 0;

#define SUB_CDH_IO_PIN 15
// SPI Settings
#define EPS_on 0 
#define EPS_off 5
//// Sellect Port
#define PORT_SPIA 0
#define PORT_SPIB 1
#define PORT_SPIC 2

#define EPS_MOSI 30
#define EPS_MISO 29
#define EPS_CLK 31
#define EPS_CS 28

#define COM_CS 27


#define MASTER 0
#define SLAVE 1

ADC_t adc;

/*************** Functions ***************/
// Latching Relay Swich Function
void LRsw(int pin_num){
  VOR_GPIO->BANK[0].DIR |= (1 << pin_num);
  VOR_GPIO->BANK[0].DATAOUT |= (1 << pin_num); 
  VOR_Sleep(200);
//  VOR_GPIO->BANK[0].DATAOUT &= ~(1 << pin_num);
}

// SubマイコンにIOパルスを送信
void WDT_pulse(int pin_num){
  VOR_GPIO->BANK[0].DIR |= (1 << pin_num);
  VOR_GPIO->BANK[0].TOGOUT |= (1 << pin_num);
}

uint8_t spi_send_cmd(uint8_t SELLECT_PORT, uint8_t CS, uint8_t cmd){
  uint8_t ack;
  //printf(">>>>>> cmd: %x\n", cmd);
  cs(CS,0);
  ack = spi_write(SELLECT_PORT, cmd);
  cs(CS,1);
  return ack;
}

// EPS helth check
void helth_check(uint8_t SELLECT_PORT, uint8_t CS, uint8_t cmd){
  printf(">>>>>> Health check\n");
  uint16_t ack[128], *i_ack; 
  i_ack = ack;
  int cnt = 0;
  
  while(cnt<5){ 
    //    cs(EPS_CS,0);
    //    ack[cnt] = spi_write(PORT_SPIA, cmd);
    //    cs(EPS_CS,1);
    ack[cnt] = spi_send_cmd(SELLECT_PORT, CS, cmd);
    
    if(ack[cnt] == 0xEF){
      printf(">>>>>> cnt: %d, Status: %x, EPS Health Check Success. \n",cnt, ack[cnt]);
      break;
    }
    else{
      printf(">>>>>> cnt: %d, Status: %x, EPS Health Check False. \n", cnt, ack[cnt]);
      // Latching Relay Switch off
      LRsw(EPS_off);
      VOR_Sleep(100);
      // Latching Relay Switch on
      LRsw(EPS_on);
      VOR_Sleep(1000);
    }
    cnt++;
  }
}


/****************************** Timer ******************************/

#define STAT_SEL_1CYC  0 
#define IRQ_TIM23_IRQn     OC30_IRQn  
#define IRQ_TIM23_IRQ_PRIORITY  0  

uint32_t CONFIG_TIM23_periodic_int(void){
    VOR_TIM23->CTRL = (TIM23_CTRL_IRQ_ENB_Msk | ( STAT_SEL_1CYC << TIM1_CTRL_STATUS_SEL_Pos))  ;  //  
    VOR_TIM23->RST_VALUE = (10e-3 * 50e6) ;   // setup RST value for 10 msec  
    VOR_TIM23->CNT_VALUE = (10e-3 * 50e6) ; 	
    VOR_TIM23->ENABLE = 0x1   ;    // enable TIM23
    VOR_IRQSEL->TIM[23]  = IRQ_TIM23_IRQn;   // IRQSEL redirects TIM23 to NVIC IRQ input 30 
    NVIC_SetPriority(IRQ_TIM23_IRQn,IRQ_TIM23_IRQ_PRIORITY);  // set priority in NVIC for IRQ30
    NVIC_EnableIRQ(IRQ_TIM23_IRQn);	  //  enable NVIC IRQ30 
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
    SysTick_CTRL_TICKINT_Msk   |
    SysTick_CTRL_ENABLE_Msk;   
    VOR_IOCONFIG->PORTA[5] |= IOCONFIG_PORTA_IEWO_Msk   ; 
    VOR_IOCONFIG->PORTA[6] |= IOCONFIG_PORTA_IEWO_Msk   ;
    VOR_GPIO->BANK[0].DIR |= (1 << 6);    // set PA[6] to an output.  this is LED4 on REB1 board, toggle every 1 sec   
    VOR_GPIO->BANK[0].DIR |= (1 << 5);    // set PA[5] to an output.  this is available I/O on REB1, toggle every 1 sec
    VOR_GPIO->BANK[0].DIR |= (1 << 4);   //  set PA[4] to an output.  This will Toggle every 10 msec 
    return (0)   ;  
}

//  *****  Global variables defined here  ******  
uint32_t  cnt_10ms, cnt_sec ; //  global variable for keeping track of 10 msec  
uint32_t  old_SYSTICK    ;    //  global variable for checking TIM derived 10 msec 
uint32_t  new_SYSTICK    ;    //  global variable for checking TIM derived 10 msec
uint32_t  elapsed_time_array[16], i ; // global variable for checking TIM derived 10 msec
//*****************************************************
//  Interrupt Subroutine for TIM23 (NVIC IRQ30)  
//******************************************************
void print_hi(){
  printf("HK\n");
}

void OC30_IRQHandlerX(void)   {
    uint32_t elapsed_time  ;

    if(cnt_10ms++ >= 99)
    {
    cnt_sec ++  ;  
//    print_hi();
    WDT_pulse(SUB_CDH_IO_PIN);
    cnt_10ms = 0 ;  
    }

    new_SYSTICK = SysTick->VAL  ;    //   Use Cortex-M0 systick to check TIM accuracy
    elapsed_time = ((old_SYSTICK - new_SYSTICK) & 0xFFFFFF)   ;  // systick only has 24 bits, need to ignore top 8 bits
    old_SYSTICK = new_SYSTICK  ;  

    if(i>16)  i = 0  ;  //  keeping rolling array of elapsed time intervals 
    elapsed_time_array[i++] = elapsed_time  ;    //   store elapse tick count for the last 16 interrupts 
}


uint32_t  cnt_SysTick   ;    //  global variable 
int SysTick_Handler () {
    cnt_SysTick  ++  ;
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
    SysTick_CTRL_TICKINT_Msk   |
    SysTick_CTRL_ENABLE_Msk;    
    SysTick->VAL = 0xFFFFFF   ;  
    SysTick->LOAD = 0xFFFFFF   ;  
    return(0)   ;  
}

/****************************** Timer end ******************************/



int main(void){
  /*** Settings ***/
  // CDH-EPS spi settings
  VOR_SYSCONFIG->PERIPHERAL_CLK_ENABLE = ( CLK_ENABLE_PORTA | CLK_ENABLE_PORTB | CLK_ENABLE_SPIA | CLK_ENABLE_SPIB | CLK_ENABLE_SPIC |
                                          CLK_ENABLE_UARTA | CLK_ENABLE_UARTB | CLK_ENABLE_I2CA | CLK_ENABLE_I2CB | CLK_ENABLE_IRQSEL |
                                            CLK_ENABLE_IOMGR | CLK_ENABLE_UTILITY | CLK_ENABLE_PORTIO | CLK_ENABLE_SYSTEM );
  spi(PORT_SPIA, EPS_MOSI, EPS_MISO, EPS_CLK, MASTER);
  init_cs(EPS_CS);
  frequency(PORT_SPIA,0x05,0x09,MASTER,0x07);
  
  /*** Main Code ***/
  printf("\n>> CDH Main Start \n");
  printf(">> Auto Sequence Start \n\n");
  
  // Power on Phase 
  printf(">> Release Detection & Power on\n\n");
  
  //timer on
  
  VOR_SYSCONFIG->TIM_CLK_ENABLE |= 1<<23   ;  //  enable TIM23 clock 
  i =  CONFIG_TIM23_periodic_int();
  

  // 6. EPS Power ON 
  printf(">>>> 6. EPS Power ON \n");
  printf(">>>> wait");
  for(int i=0; i<5; i++){
    printf(". ");
    VOR_Sleep(1000);
  }
  printf(">>>>\n");
  LRsw(EPS_on);       // Latching Relay Switch ON
  printf(">>>> EPS Power ON Done !!\n\n");
  VOR_Sleep(1000);    // Wait for power on
  
  // 7. EPS Helth Check 
  printf(">>>> 7. EPS Helth Check\n");
  helth_check(PORT_SPIA, EPS_CS, 0x02);
  VOR_Sleep(1000);
  printf("\n");
  for(int i=0; i<100; i++){
    printf("helth check ");
    helth_check(PORT_SPIA, EPS_CS, 0x02);
    VOR_Sleep(1000);
  }
  // 10. EPS HK Sensings
  printf(">>>> 10. EPS First HK Sensing\n");
  int ack = spi_send_cmd(PORT_SPIA, EPS_CS, 0x04);
  VOR_Sleep(1000);
  if(ack == 0xEF){
    eps_get_HK_isr_flag = 1;
    printf(">>>> eps_get_HK_isr_flag = %d\n", eps_get_HK_isr_flag);
    VOR_Sleep(1);
    for(;;){
      printf(">>>> HK command Sending\n");
      ack = 0;
      ack = spi_send_cmd(PORT_SPIA, EPS_CS, 0x05);
      if(ack != 0xEF){
        break;
      }
      VOR_Sleep(1);
    }
    VOR_Sleep(10000);
    printf(">>>> HK data Recieving\n");
    
    uint8_t EPS_HK[128];
    for(int i = 0; i<40;i++){
      EPS_HK[i] = spi_send_cmd(PORT_SPIA, EPS_CS, 0x00);
    }
    for(int i = 0; i<40;i++){
      printf(">>>>EPS_HK[%d] = %x\n",i, EPS_HK[i]);
    }
  }
  printf("\n");
  


#if 0
  
  // 17. ADC Power ON
  printf(">>>> 17. ADC Power On\n");
  int ack = spi_send_cmd(PORT_SPIA, EPS_CS, 0x09);
  
  if(ack == 0xEF){
    adc_get_HK_isr_flag = 1;
    printf(">>>> ADC Power On Done !!\n");
    
    int time = 1;
    printf(">>>> ADC Waking Up Now. waiting...");
    
    while(time < 21){
      VOR_Sleep(500);
      printf(".");
      time++;
    }
    printf("\n");
    
    //*** ADC
    PowerON_ADC(&adc);
    printf(" POWER ON STATUS = %d\n\n",adc.STATUS);
  }
  
  // 19. ADC Healt Check
  printf(">>>> 19. ADC HK Check. (^_^)v\n");
  printf(">>>> 19.1. ADC HK Configuration.\n");
  HKInitial_ADC(&adc);  // ADC HK configuration
  printf(" ADC HK Configuration STATUS = %d\n\n",adc.STATUS);
  
  printf(">>>> 19.2. GET ADC HK.\n");
  adc_get_HK(&adc);     // GET ADC HK
  HK_print(1, &adc);    // PRINT ADC HK
  
  printf(">>>> adc_get_HK_isr_flag = %d\n", adc_get_HK_isr_flag);
#endif
}  // end of main routine !
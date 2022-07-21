/***************************************************************************************
 * @file     pp_timer_v2.0.c
 * @version  V1.0
 * @date     8. June 2022
 * @name     Ryusuke Iwata
 *
 * @note
 * This program comes from AN1202_3phase_pwm
 * the link is here: https://drive.google.com/drive/u/0/folders/1O_iyGungMABeHryT_yyC42h4wV35NvPH
 *
 ****************************************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "va108xx.h"
#include "reb_timer.h"
#include "reb_board.h"
#include "irq_va108xx.h"

#define FUNCSEL1 1
#define OUTPUT 0
#define TIMER_DEBUG true 

VOR_TIM_ALLOCTIMERS alloc_timers; 

static void VOR_TIM_PortPinConfig(uint8_t port_num, uint8_t pin_num) {
	uint32_t regVal = 0;
	
	if( port_num == 0 )
		regVal = VOR_IOCONFIG->PORTA[pin_num]; 
	else
		regVal = VOR_IOCONFIG->PORTB[pin_num]; 
		
	regVal = (regVal & ~IOCONFIG_PORTA_FUNSEL_Msk) | (FUNCSEL1 <<  IOCONFIG_PORTA_FUNSEL_Pos); 
	regVal = (regVal & ~IOCONFIG_PORTA_FLTTYPE_Msk) | (OUTPUT <<  IOCONFIG_PORTA_FLTTYPE_Pos); 
		
	if( port_num == 0 )
		VOR_IOCONFIG->PORTA[pin_num] = regVal; 
	else
		VOR_IOCONFIG->PORTB[pin_num] = regVal; 
	
	/* Configure caller specified port pin */ 
	VOR_GPIO->BANK[port_num].DIR |= (1 << pin_num);  
	VOR_GPIO->BANK[port_num].DATAMASK |= (1 << pin_num); 	
}

static int32_t VOR_TIM_CreateTrigger(VOR_TIM_SIGNAL signal) {
	int32_t ret_code = 0;
	
	VOR_SYSCONFIG->TIM_CLK_ENABLE |= 1<<signal.trig.timer_instance;  
	VOR_TIM->BANK[signal.trig.timer_instance].CTRL = 0x0;
	
	/* Disable IRQ for trigger timer */ 
	VOR_Disable_Irq(VOR_IRQ_TIM, signal.trig.timer_instance); 
	
	VOR_TIM->BANK[signal.trig.timer_instance].RST_VALUE = signal.trig.cycle*(SystemCoreClock/1000000);;  
	
	VOR_TIM->BANK[signal.trig.timer_instance].CTRL &= ~TIM_PERIPHERAL_CTRL_STATUS_SEL_Msk; 
	VOR_TIM->BANK[signal.trig.timer_instance].CTRL = (0x1 << TIM_PERIPHERAL_CTRL_STATUS_SEL_Pos) | TIM_PERIPHERAL_CTRL_IRQ_ENB_Msk;

	/* Enable IRQ for trigger module */ 
	VOR_Enable_Irq(VOR_IRQ_TIM, signal.trig.timer_instance);

#if TIMER_DEBUG	
	VOR_TIM_PortPinConfig(signal.trig.port_num, signal.trig.pin_num); 
#endif 
	return ret_code; 
}

static int32_t VOR_TIM_CreatePulse(VOR_TIM_SIGNAL signal) {
	int32_t ret_code = 0;
	
	VOR_SYSCONFIG->TIM_CLK_ENABLE |= 1<<signal.pulse.timer_instance;  
	VOR_TIM->BANK[signal.pulse.timer_instance].CTRL = 0x0;
	
	/* Initialize TIM w/ specified cycle period */ 	
	VOR_TIM->BANK[signal.pulse.timer_instance].RST_VALUE = signal.pulse.cycle*(SystemCoreClock/1000000);  
	VOR_TIM->BANK[signal.pulse.timer_instance].CNT_VALUE = 0;  
			
	/* Configure PWMA and PWMB values */ 
	VOR_TIM->BANK[signal.pulse.timer_instance].PWMA_VALUE = (signal.pulse.cycle*(SystemCoreClock/1000000)) - (signal.pulse.start_offset*(SystemCoreClock/1000000));
	VOR_TIM->BANK[signal.pulse.timer_instance].PWMB_VALUE = (signal.pulse.cycle*(SystemCoreClock/1000000)) - (signal.pulse.end_offset*(SystemCoreClock/1000000));
	
	/* Enable Cascade Control 0 */ 
	VOR_TIM->BANK[signal.pulse.timer_instance].CSD_CTRL |= TIM_PERIPHERAL_CSD_CTRL_CSDEN0_Msk | TIM_PERIPHERAL_CSD_CTRL_CSDTRG0_Msk;
	
	/* Configure Cascade Control 0 */ 
	VOR_TIM->BANK[signal.pulse.timer_instance].CASCADE0 = CASCADE_TIMER_INDEX + signal.trig.timer_instance; 
	
	/* Enable TIM and configure status select options based on caller selected options */ 
	VOR_TIM->BANK[signal.pulse.timer_instance].CTRL &= ~TIM_PERIPHERAL_CTRL_STATUS_SEL_Msk; 
	if( !signal.pulse.inverse )
		VOR_TIM->BANK[signal.pulse.timer_instance].CTRL = TIM_PERIPHERAL_CTRL_ENABLE_Msk | (0x4 << TIM_PERIPHERAL_CTRL_STATUS_SEL_Pos);
	else
		VOR_TIM->BANK[signal.pulse.timer_instance].CTRL = TIM_PERIPHERAL_CTRL_ENABLE_Msk | (0x4 << TIM_PERIPHERAL_CTRL_STATUS_SEL_Pos) | TIM_PERIPHERAL_CTRL_STATUS_INV_Msk;

#if TIMER_DEBUG	
	VOR_TIM_PortPinConfig(signal.pulse.port_num, signal.pulse.pin_num); 
#endif 
	return ret_code;
}

static int32_t VOR_TIM_CreatePWMPulse(VOR_TIM_SIGNAL signal) {
	int32_t ret_code = 0;
	
	VOR_TIM_CreatePulse(signal); 	
		
	VOR_SYSCONFIG->TIM_CLK_ENABLE |= 1<<signal.pwmpulse.timer_instance;  
	VOR_TIM->BANK[signal.pwmpulse.timer_instance].CTRL = 0x0;

	/* Initialize TIM w/ specified cycle period */ 	
	VOR_TIM->BANK[signal.pwmpulse.timer_instance].RST_VALUE = signal.pwmpulse.cycle*(SystemCoreClock/1000000);  
	VOR_TIM->BANK[signal.pwmpulse.timer_instance].CNT_VALUE = 0;  
		
	/* Configure PWMA value */ 
	VOR_TIM->BANK[signal.pwmpulse.timer_instance].PWMA_VALUE = ((signal.pwmpulse.cycle/2)*(SystemCoreClock/1000000)); 
		
	/* Enable Cascade Control 0 */ 
	VOR_TIM->BANK[signal.pwmpulse.timer_instance].CSD_CTRL |= TIM_PERIPHERAL_CSD_CTRL_CSDEN0_Msk | TIM_PERIPHERAL_CSD_CTRL_CSDEN1_Msk | TIM_PERIPHERAL_CSD_CTRL_CSDTRG1_Msk;
	
	/* Configure Cascade Control's*/ 
	VOR_TIM->BANK[signal.pwmpulse.timer_instance].CASCADE0 = CASCADE_TIMER_INDEX + signal.pulse.timer_instance; 
	VOR_TIM->BANK[signal.pwmpulse.timer_instance].CASCADE1 = CASCADE_TIMER_INDEX + signal.trig.timer_instance; 
	
	/* Enable TIM and configure status select options based on caller selected options */ 
	VOR_TIM->BANK[signal.pwmpulse.timer_instance].CTRL &= ~TIM_PERIPHERAL_CTRL_STATUS_SEL_Msk; 
	VOR_TIM->BANK[signal.pwmpulse.timer_instance].CTRL  = TIM_PERIPHERAL_CTRL_ENABLE_Msk | (0x3 << TIM_PERIPHERAL_CTRL_STATUS_SEL_Pos);

#if TIMER_DEBUG
	VOR_TIM_PortPinConfig(signal.pwmpulse.port_num, signal.pwmpulse.pin_num); 
#endif 
	return ret_code;
}

int32_t VOR_TIM_Initialize(void) {
	int32_t ret_code = 0;
	alloc_timers.num_timers = 0; 
	alloc_timers.trigger_timer_index = 0xFF; 
	return ret_code;
} 

int32_t VOR_TIM_Create(VOR_TIM_SIGNAL signal) {
	if( alloc_timers.trigger_timer_index == 0xFF ) {
		alloc_timers.trigger_timer_index = alloc_timers.num_timers; 
		alloc_timers.timers[alloc_timers.num_timers++] = signal.trig.timer_instance;
	}
	
	if( signal.type == SIGNAL_TYPE_TRIGGER ) {
		return VOR_TIM_CreateTrigger(signal); 
	} else if( signal.type == SIGNAL_TYPE_PULSE )	{	
		alloc_timers.timers[alloc_timers.num_timers++] = signal.pulse.timer_instance; 
		return VOR_TIM_CreatePulse(signal); 
	} else {
		alloc_timers.timers[alloc_timers.num_timers++] = signal.pulse.timer_instance; 
		alloc_timers.timers[alloc_timers.num_timers++] = signal.pwmpulse.timer_instance; 
		return VOR_TIM_CreatePWMPulse(signal); 
	}
}

int32_t VOR_TIM_Enable(void) {
	int32_t ret_code = 0; 
	VOR_TIM->BANK[alloc_timers.trigger_timer_index].CTRL |= TIM_PERIPHERAL_CTRL_ENABLE_Msk; 
	return ret_code; 
}

/*
     uint32_t  cnt_10ms    ;    //  global variable for keeping track of 10 msec  
     uint32_t  old_SYSTICK    ;    //  global variable for debuging 10 msec 
     uint32_t  new_SYSTICK    ;    //  global variable for keeping track of 10 msec 
     uint32_t  elapsed_time_array[16],  i   ;  
//   *****************************************************
//  Interrupt Subroutine for TIM23 (NVIC IRQ30)  

void OC30_IRQHandlerX(void)  
 
{
 	uint32_t elapsed_time  ;
//	VOR_TIM23->CTRL &= ~(TIM23_CTRL_IRQ_ENB_Msk )  ;  //   disable TIM 23 INT 
	cnt_10ms ++  ; 
	new_SYSTICK = SysTick->VAL  ;    //     
	elapsed_time = old_SYSTICK - new_SYSTICK   ;  
	old_SYSTICK = new_SYSTICK  ;  
//	VOR_TIM23->CTRL |= (TIM23_CTRL_IRQ_ENB_Msk )  ;  //   enable TIM 23 INT
	if(i>16)  i = 0  ;  
	elapsed_time_array[i++] = elapsed_time  ;  
}

*/
void OC0_IRQHandlerX(void) {
	uint8_t i; 
	/* Disable IRQ for TIM0 module */ 
  VOR_Disable_Irq(VOR_IRQ_TIM, 0); 
	
	/* Disable all allocated timers */ 
	for( i=0; i<alloc_timers.num_timers; i++ )
			VOR_TIM->BANK[alloc_timers.timers[i]].ENABLE = 0; 
	
	/* Clear all count values */ 
	for( i=0; i<alloc_timers.num_timers; i++ )
			VOR_TIM->BANK[alloc_timers.timers[i]].CNT_VALUE = 0; 
		
	/* Enable all allocated timers except trigger timer */ 
	for( i=0; i<alloc_timers.num_timers; i++ )
		if( alloc_timers.timers[i] != alloc_timers.trigger_timer_index)
			VOR_TIM->BANK[alloc_timers.timers[i]].ENABLE = 1; 
	
	/* Enable IRQ for TIM0 module */ 
	VOR_Enable_Irq(VOR_IRQ_TIM, 0); 
	
	VOR_TIM->BANK[alloc_timers.trigger_timer_index].ENABLE = 1;
}
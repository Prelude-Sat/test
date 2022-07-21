/***************************************************************************************
 * @file     pp_timer_v2.0.h
 * @version  V1.0
 * @date     8. June 2022
 * @name     Ryusuke Iwata
 *
 * @note
 * This program comes from AN1202_3phase_pwm
 * the link is here: https://drive.google.com/drive/u/0/folders/1O_iyGungMABeHryT_yyC42h4wV35NvPH

 ****************************************************************************************/
#ifndef __REB_TIMER_H
#define __REB_TIMER_H

#define CASCADE_TIMER_INDEX 64
#define MAX_TIMERS 24 

/**
\brief Signal type enum for timer.
*/
typedef enum {
	SIGNAL_TYPE_TRIGGER,								///< Trigger signal type.  
	SIGNAL_TYPE_PULSE,									///< Pulse signal type.   
	SIGNAL_TYPE_PWMPULSE								///< PWM pulse signal type.  
} VOR_TIM_SIGNALTYPE; 

/**
\brief Allocated timers structure for timer.
*/
typedef struct {
	uint8_t timers[MAX_TIMERS];					///< An array of allocated timers. 	
  uint8_t num_timers; 								///< Specifies number of timers allocated. 
	uint8_t trigger_timer_index; 				///< Specifies the trigger timer index in allocated timers array
} VOR_TIM_ALLOCTIMERS; 

/**
\brief Trigger signal structure for timer, where all attributes for that signal are specified.
*/
typedef struct {
	uint8_t timer_instance; 						///< Specifies trigger timer instance.  
	uint8_t port_num; 									///< Specifies the port number for the port pin used to route this signal. 
	uint8_t pin_num; 										///< Specifies the pin number to which the signal will be routed. 
	uint32_t cycle;											///< Specifies the cycle period in micro seconds. 
} VOR_TIM_TRIGSIGNAL; 

/**
\brief Pulse signal structure for timer, where all attributes for that signal are specified.
*/
typedef struct {
	uint8_t timer_instance; 						///< Specifies pulse timer instance. 
	uint8_t port_num;										///< Specifies the port number for the port pin used to route this signal. 
	uint8_t pin_num; 										///< Specifies the pin number to which the signal will be routed. 
	uint32_t cycle;											///< Specifies the cycle period in micro seconds. 
	uint32_t start_offset; 							///< Specifies start offset for this signal. 
	uint32_t end_offset;								///< Specifies the end offset for this signal. 
	bool inverse;												///< Specifies whether signal has to be inverted. 
} VOR_TIM_PULSESIGNAL; 

/**
\brief PWM pulse signal structure for timer, where all attributes for that signal are specified.
*/
typedef struct {
	uint8_t timer_instance; 						///< Specifies PWM pulse timer instance. 
	uint8_t port_num;										///< Specifies the port number for the port pin used to route this signal. 
	uint8_t pin_num;										///< Specifies the pin number to which the signal will be routed. 
	uint32_t cycle; 										///< Specifies the cycle period in micro seconds. 
} VOR_TIM_PWMPULSESIGNAL; 

/**
\brief Signal structure for timer, which encompases various other signal structures such as trigger, pulse and PWM pulse.
*/
typedef struct {
	VOR_TIM_TRIGSIGNAL trig; 						///< Trigger signal attributes 
	VOR_TIM_PULSESIGNAL pulse; 					///< Pulse signal attributes  
	VOR_TIM_PWMPULSESIGNAL pwmpulse; 		///< PWM pulse signal attributes 
	VOR_TIM_SIGNALTYPE type; 						///< Specifies signal type - trigger, pulse or PWM pulse 
} VOR_TIM_SIGNAL; 

/**
 \fn      int32_t VOR_TIM_Initialize(void)
 \brief   Initialize timer module. 
 \return  \ref execution_status
*/ 
int32_t VOR_TIM_Initialize(void); 

/**
 \fn       		int32_t VOR_TIM_Create(VOR_TIM_SIGNAL signal)
 \brief   		Creates a timer based on caller specified signal type and attributes.
 \param[in] 	signal Specifies the signal type and attriubutes. 
 \return  		\ref execution_status
*/
int32_t VOR_TIM_Create(VOR_TIM_SIGNAL signal); 

/**
 \fn      int32_t VOR_TIM_Enable(void)
 \brief   Enable timer module. This call will enable trigger timer that will start all other timers.  
 \return  \ref execution_status
*/
int32_t VOR_TIM_Enable(void); 

#endif /* __REB_TIMER_H */ 



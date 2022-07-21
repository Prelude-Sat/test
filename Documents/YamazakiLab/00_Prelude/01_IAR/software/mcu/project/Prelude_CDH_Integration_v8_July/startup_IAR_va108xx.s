;/******************** (C) COPYRIGHT 2016 VORAG0 Technologies ********************
;* File Name          : startup_va108xx.s
;* Date               : Sept 29, 2016
;* Description        : VA108xx devices vector table for EWARM toolchain.
;*                      This module performs:
;*                      - Set the initial SP
;*                      - Set the initial PC == _iar_program_start,
;*                      - Set the vector table entries with the exceptions ISR 
;*                        address.
;*                      - Branches to main in the C library (which eventually
;*                        calls main()).
;********************************************************************************
;

#pragma language=extended
#pragma segment="CSTACK"

//  EXTERN void __iar_program_start( void );

//  EXTERN void NMI_Handler( void );
//  EXTERN void HardFault_Handler( void );
//  EXTERN void MemManage_Handler( void );
//  EXTERN void BusFault_Handler( void );
//  EXTERN void UsageFault_Handler( void );
//  EXTERN void SVC_Handler( void );
//  EXTERN void DebugMon_Handler( void );
//  EXTERN void PendSV_Handler( void );
//  EXTERN void SysTick_Handler( void );

//  EXTERN void OC0_IRQHandler_Handler( void );
//  EXTERN void OC30_IRQHandler_Handler( void );
//  EXTERN void OC31_IRQHandler_Handler( void );

//  typedef void( *intfunc )( void );
//  typedef union { intfunc __fun; void * __ptr; } intvec_elem;

; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY, which
; is where to find the SP start value.
; If vector table is not located at address 0, the user has to initialize the  NVIC vector
; table register (VTOR) before using interrupts.
;
; Cortex-M version
;

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        EXTERN  SystemInit
        EXTERN  C_HardFault_Handler
        PUBLIC  __vector_table

        DATA
__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler             ; Reset Handler

        DCD     NMI_Handler               ; NMI Handler
        DCD     HardFault_Handler         ; Hard Fault Handler
        DCD     MemManage_Handler         ; MPU Fault Handler
        DCD     BusFault_Handler          ; Bus Fault Handler
        DCD     UsageFault_Handler        ; Usage Fault Handler
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     0                         ; Reserved
        DCD     SVC_Handler               ; SVCall Handler
        DCD     DebugMon_Handler          ; Debug Monitor Handler
        DCD     0                         ; Reserved
        DCD     PendSV_Handler            ; PendSV Handler
        DCD     SysTick_Handler           ; SysTick Handler

    ; External Interrupts
        DCD     OC0_IRQHandler   	  ;  0: TIM 0 ISR
        DCD     VOR_TIM1_IRQHandler 	  ;  1: TIM 1 ISR
        DCD     OC2_IRQHandler            ;  2: General
        DCD     VOR_UART0_IRQHandler      ;  3: UART A ISR
        DCD     VOR_UART1_IRQHandler      ;  4: UART B ISR
        DCD     VOR_GPIO_IRQHandler   	  ;  5: GPIO ISR
        DCD     VOR_SPI0_IRQHandler       ;  6: SPI A ISR
        DCD     VOR_SPI1_IRQHandler       ;  7: SPI B ISR
        DCD     VOR_SPI2_IRQHandler       ;  8: SPI C ISR
        DCD     VOR_I2C0_MS_IRQHandler    ;  9: I2C A Master ISR
        DCD     VOR_I2C0_SL_IRQHandler    ; 10: I2C A Slave ISR
        DCD     VOR_I2C1_MS_IRQHandler    ; 11: I2C B Master ISR
        DCD     VOR_I2C1_SL_IRQHandler    ; 12: I2C B Slave ISR
        DCD     OC13_IRQHandler           ; 13: General
        DCD     OC14_IRQHandler           ; 14: General
        DCD     OC15_IRQHandler           ; 15: General
        DCD     OC16_IRQHandler           ; 16: Reserved
        DCD     OC17_IRQHandler           ; 17: Reserved
        DCD     OC18_IRQHandler           ; 18: Reserved
        DCD     OC19_IRQHandler           ; 19: Reserved
        DCD     OC20_IRQHandler           ; 20: Reserved
        DCD     OC21_IRQHandler           ; 21: Reserved
        DCD     OC22_IRQHandler           ; 22: Reserved
        DCD     OC23_IRQHandler           ; 23: Reserved
        DCD     OC24_IRQHandler           ; 24: Reserved
        DCD     OC25_IRQHandler           ; 25: Reserved
        DCD     OC26_IRQHandler           ; 26: Reserved
        DCD     OC27_IRQHandler           ; 27: Reserved
        DCD     OC28_IRQHandler           ; 28: Reserved
        DCD     OC29_IRQHandler           ; 29: Reserved
        DCD	OC30_IRQHandlerX          ;  30   Reserved  
        DCD     OC31_IRQHandler           ; 31:  Reserved
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB
        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
Reset_Handler

        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0

        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler

    REQUIRE8              ; For API compatability used 8 byte stack frame
    PRESERVE8  
    MOVS    r3, #127	
    PUSH    {r3-r7,lr}    ; Save contents of other registers for display (R3 included to meet REQUIRE8)e
    MOV     R0,SP         ; Pass current SP as parameter
 //   BL      __cpp(C_HardFault_Handler)
    BL      C_HardFault_Handler
    POP     {r3-r7,pc}    ; Restore all registers
   



//        B HardFault_Handler

        PUBWEAK MemManage_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
MemManage_Handler
        B MemManage_Handler

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BusFault_Handler
        B BusFault_Handler

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
UsageFault_Handler
        B UsageFault_Handler

        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        B SVC_Handler

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
DebugMon_Handler
        B DebugMon_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B SysTick_Handler
        
 ; externaal interrupts start here   *****************
 
        PUBWEAK OC0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OC0_IRQHandler  
        B OC0_IRQHandler
                 
        PUBWEAK VOR_TIM1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
VOR_TIM1_IRQHandler  
        B VOR_TIM1_IRQHandler
        
                PUBWEAK OC2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OC2_IRQHandler  
        B OC2_IRQHandler
        
        
                
                PUBWEAK VOR_UART0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
VOR_UART0_IRQHandler  
        B VOR_UART0_IRQHandler
        
                PUBWEAK VOR_UART1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
VOR_UART1_IRQHandler  
        B VOR_UART1_IRQHandler
        
                PUBWEAK VOR_GPIO_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
VOR_GPIO_IRQHandler  
        B VOR_GPIO_IRQHandler
        
                PUBWEAK VOR_SPI0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
VOR_SPI0_IRQHandler  
        B VOR_SPI0_IRQHandler
        
                PUBWEAK VOR_SPI1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
VOR_SPI1_IRQHandler  
        B VOR_SPI1_IRQHandler
        
                PUBWEAK VOR_SPI2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
VOR_SPI2_IRQHandler  
        B VOR_SPI2_IRQHandler
        
                PUBWEAK VOR_I2C0_MS_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
VOR_I2C0_MS_IRQHandler 
        B VOR_I2C0_MS_IRQHandler
        
                PUBWEAK VOR_I2C0_SL_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
VOR_I2C0_SL_IRQHandler 
        B VOR_I2C0_SL_IRQHandler
                 
        PUBWEAK VOR_I2C1_MS_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
VOR_I2C1_MS_IRQHandler  
        B VOR_I2C1_MS_IRQHandler
        
                PUBWEAK VOR_I2C1_SL_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
VOR_I2C1_SL_IRQHandler 
        B VOR_I2C1_SL_IRQHandler
        
        
                PUBWEAK OC13_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OC13_IRQHandler  
        B OC13_IRQHandler
        
                PUBWEAK OC14_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OC14_IRQHandler  
        B OC14_IRQHandler
        
                PUBWEAK OC15_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OC15_IRQHandler  
        B OC15_IRQHandler
        
                PUBWEAK OC16_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OC16_IRQHandler  
        B OC16_IRQHandler
        
                PUBWEAK OC17_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OC17_IRQHandler  
        B OC17_IRQHandler
        
                PUBWEAK OC18_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OC18_IRQHandler  
        B OC18_IRQHandler
        
                PUBWEAK OC19_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OC19_IRQHandler  
        B OC19_IRQHandler
        
                      PUBWEAK OC20_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OC20_IRQHandler 
        B OC20_IRQHandler
                 
        PUBWEAK OC21_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OC21_IRQHandler  
        B OC21_IRQHandler
        
                PUBWEAK OC22_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OC22_IRQHandler  
        B OC22_IRQHandler
        
        
                PUBWEAK OC23_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OC23_IRQHandler  
        B OC23_IRQHandler
        
                PUBWEAK OC24_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OC24_IRQHandler  
        B OC24_IRQHandler
        
                PUBWEAK OC25_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OC25_IRQHandler  
        B OC25_IRQHandler
        
                PUBWEAK OC26_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OC26_IRQHandler  
        B OC26_IRQHandler
        
                PUBWEAK OC27_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OC27_IRQHandler  
        B OC27_IRQHandler
        
                PUBWEAK OC28_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OC28_IRQHandler  
        B OC28_IRQHandler
        
                PUBWEAK OC29_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OC29_IRQHandler  
        B OC29_IRQHandler
        
        PUBWEAK OC30_IRQHandlerX
        SECTION .text:CODE:REORDER:NOROOT(1)
OC30_IRQHandlerX 
        B OC30_IRQHandlerX
                 
        PUBWEAK OC31_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
OC31_IRQHandler  
        B OC31_IRQHandler

        END
        
        
/****END OF FILE****/

#include <stdio.h>
#include "va108xx.h"
#include "reb_log.h"
#include "reb_board.h"
#include "reb_adt75.h"
#include "driver_common.h"
#include "reb_max11619.h"
#include "gpio_va108xx.h"
#include "segger_rtt.h" 
#include "irq_va108xx.h"
#include "sleep.h"

//**** global variable defs *****//
#define PERIPH_CLK_ENAB_PORTA 0x00
#define PERIPH_CLK_ENAB_PORTB 0x01
#define PERIPH_CLK_ENAB_SPIA 0x04
#define PERIPH_CLK_ENAB_SPIB 0x05
#define PERIPH_CLK_ENAB_SPIC 0x06
#define PERIPH_CLK_ENAB_IRQSEL 0x15
#define PERIPH_CLK_ENAB_IOCONFIG 0x16
#define PERIPH_CLK_ENAB_UTILITY 0x17
#define PERIPH_CLK_ENAB_PORTIO 0x18 //GPIO
#define FUNCSEL0 00                                                            // 1つのピンに3つの役割が存在するため選択する必要がある
#define FUNCSEL1 01                                                             
#define FUNCSEL2 02
#define FUNCSEL3 03
#define SPI_PERIPHERAL_CTRL1_BLOCKMODE_Pos 7
#define SPI_PERIPHERAL_CTRL1_BLOCKMODE_Msk (0x01UL << SPI_PERIPHERAL_CTRL1_BLOCKMODE_Pos)

extern void spi(int sp,int mosi,int miso,int clk, int ms);
extern void init_cs(int cs);
extern void cs(int pin_num,int cs);
extern void frequency(int sp, uint8_t PRESCALE, uint8_t SCRDV, int ms,uint8_t size);
extern uint8_t spi_write(int sp,uint8_t data);
extern uint8_t spi_receive(int sp);
extern uint8_t spi_reply(int sp,uint8_t data);

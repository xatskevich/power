
#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f0xx.h"                  // Device header
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_can.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_iwdg.h"

#include "CAN.h"
#include "GPIO.h"
#include "rcc.h"
#include "timer.h"

#define sys_clock 24000
#define power_address 0x050

#define input_config_address 0x08007814
#define output_config_address 0x08007818

#define set_out1		GPIO_SetBits(GPIOB, GPIO_Pin_4)
#define reset_out1		GPIO_ResetBits(GPIOB, GPIO_Pin_4)
#define set_out2		GPIO_SetBits(GPIOB, GPIO_Pin_6)
#define reset_out2		GPIO_ResetBits(GPIOB, GPIO_Pin_6)
#define set_out3		GPIO_SetBits(GPIOB, GPIO_Pin_7)
#define reset_out3		GPIO_ResetBits(GPIOB, GPIO_Pin_7)
#define set_out4		GPIO_SetBits(GPIOA, GPIO_Pin_3)
#define reset_out4		GPIO_ResetBits(GPIOA, GPIO_Pin_3)
#define is_in1			(GPIOB->IDR & 0x1) == 0
#define is_in2			(GPIOB->IDR & 0x2) == 0

#define left_light_mask		1<<0
#define beam_light_mask		1<<2
#define inside_light_mask	1<<4
#define rear_light_mask		1<<3
#define right_light_mask	1<<1
#define strobe_light_mask	1<<5
#define tail_light_mask		1<<6

extern uint8_t tmr_cnt;
extern uint8_t can_on;
extern uint8_t input_config, output_config;
extern uint8_t inputs, out1_config, out2_config, out3_config, out4_config;	
extern uint8_t light_mask;		

void flash_erase(void);
void flash_prog_all(void);

extern CanTxMsg TxMessage;
extern GPIO_InitTypeDef GPIO_InitStructure;
extern NVIC_InitTypeDef NVIC_InitStructure;
extern CAN_InitTypeDef CAN_InitStructure;
extern CAN_FilterInitTypeDef CAN_FilterInitStructure;


#endif

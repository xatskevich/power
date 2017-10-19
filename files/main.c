
#include "main.h"

uint8_t tmr_cnt;
uint8_t can_on;
uint8_t input_config, output_config;
uint8_t inputs,	//0 - роллета
								//1 - ступень
out1_config, out2_config, out3_config, out4_config;	//0 - освещение слева
																										//1 - освещение справа
																										//2 - маяк	
																										//3 - задний прожектор
																										//4 - освещение отсеков
																										//5 - строб					
																										//6 - габариты
uint8_t light_mask;		//0 - освещение слева
											//1 - освещение справа
											//2 - маяк	
											//3 - задний прожектор
											//4 - освещение отсеков
											//5 - строб					
											//6 - габариты

CanTxMsg TxMessage;
GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
CAN_InitTypeDef CAN_InitStructure;
CAN_FilterInitTypeDef CAN_FilterInitStructure;



int main(void)
{

//	uint32_t temp;
	uint8_t tmp;
	
	Init_IWDG();			//

	input_config = 0x69;
	output_config = 0;

	FLASH->KEYR = 0x45670123;
	FLASH->KEYR = 0xCDEF89AB;

	//temp = (*(__IO uint32_t*) input_config_address);
	//if(temp != 0xffffffff) input_config = temp;
	//temp = (*(__IO uint32_t*) output_config_address);
	//if(temp != 0xffffffff) output_config = temp;

	switch (output_config & 3) {
	case 0:
		tmp = power_address;
		if ((tmp == 0x50) || (tmp == 0x51) || (tmp == 0x52) || (tmp == 0x53)) out1_config = left_light_mask;
		if ((tmp == 0x60) || (tmp == 0x61) || (tmp == 0x62) || (tmp == 0x63)) out1_config = right_light_mask;
		if (tmp == 0x56) out1_config = beam_light_mask;
		break;
	case 1:
		out1_config = beam_light_mask;
		break;
	case 2:
		out1_config = rear_light_mask;
		break;
	case 3:
		out1_config = inside_light_mask;
		break;
	}
	switch ((output_config & 0xC) >> 2) {
	case 0:
		out2_config = inside_light_mask;
		break;
	case 1:
		out2_config = beam_light_mask;
		break;
	case 2:
		out2_config = rear_light_mask;
		break;
	case 3:
		out2_config = tail_light_mask;
		break;
	}
	switch ((output_config & 0x30) >> 4) {
	case 0:
		out3_config = tail_light_mask;
		break;
	case 1:
		out3_config = beam_light_mask;
		break;
	case 2:
		out3_config = rear_light_mask;
		break;
	case 3:
		out3_config = inside_light_mask;
		break;
	}
	switch ((output_config & 0xC0) >> 6) {
	case 0:
		out4_config = strobe_light_mask;
		break;
	case 1:
		out4_config = beam_light_mask;
		break;
	case 2:
		out4_config = rear_light_mask;
		break;
	case 3:
		out4_config = inside_light_mask;
		break;
	}


	Init_RCC();			//тактирование
	Init_GPIO();		//порты
	Init_CAN();			//CAN
	Init_Timer();		//таймеры
	
	GPIO_SetBits(GPIOA, GPIO_Pin_15);
	
	NVIC_SystemLPConfig(NVIC_LP_SLEEPONEXIT, ENABLE);
	
	__WFI();

    while(1)
    {

    }
}

//стирание flash
void flash_erase(void){

	while (FLASH->SR & FLASH_SR_BSY);
	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR = FLASH_SR_EOP;
	}
    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = input_config_address;
    FLASH->CR |= FLASH_CR_STRT;
    while (FLASH->SR & FLASH_SR_BSY);
    FLASH->SR = FLASH_SR_EOP;
    FLASH->CR &= ~FLASH_CR_PER;
}

//запись flash
void flash_prog_all(void){

	while (FLASH->SR & FLASH_SR_BSY);
	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR = FLASH_SR_EOP;
	}
	FLASH->CR |= FLASH_CR_PG;
	*(__IO uint16_t*)input_config_address = input_config;
	while (!(FLASH->SR & FLASH_SR_EOP));
	FLASH->SR = FLASH_SR_EOP;
	*(__IO uint16_t*)output_config_address = output_config;
	while (!(FLASH->SR & FLASH_SR_EOP));
	FLASH->SR = FLASH_SR_EOP;

	FLASH->CR &= ~(FLASH_CR_PG);
}


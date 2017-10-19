
#include "timer.h"


void TIM17_IRQHandler(void){	//CAN

	static uint8_t can_cnt;
	uint8_t tmp, i, out;

	TIM17->SR &= ~TIM_SR_UIF;	//сброс флага

	IWDG_ReloadCounter();

//анализ CAN
	can_cnt++;
	if (can_cnt == 9) { //интервал 1,27 с
		can_cnt = 0;
		if (can_on == 0) { //CAN сообщений не было
			GPIOA->ODR ^= 0x400;
		}
		can_on = 0;
	}

//входа
	if (input_config & 0xF) { 				//вход 1
		if (input_config & 1) { 			//если вход настроен на роллету
			if (input_config & 4) { 		//открытое состояние активный
				if (is_in1) inputs |= 1; 	//включить сигнал
				else inputs &= ~1; 			//выключить сигнал
			}
			if (input_config & 8) { 		//открытое состояние неактивный
				if (is_in1) inputs &= ~1; 	//выключить сигнал
				else inputs |= 1; 			//включить сигнал
			}
		}
		if (input_config & 2) { 			//если вход настроен на ступень
			if (input_config & 4) { 		//открытое состояние активный
				if (is_in1) inputs |= 2; 	//включить сигнал
				else inputs &= ~2; 			//выключить сигнал
			}
			if (input_config & 8) { 		//открытое состояние неактивный
				if (is_in1) inputs &= ~2; 	//выключить сигнал
				else inputs |= 2; 			//включить сигнал
			}
		}
	}
	if (input_config & 0xF0) { 				//вход 2
		if (input_config & 0x10) { 			//если вход настроен на роллету
			if (input_config & 0x40) { 		//открытое состояние активный
				if (is_in2) inputs |= 1; 	//включить сигнал
				else inputs &= ~1; 			//выключить сигнал
			}
			if (input_config & 0x80) { 		//открытое состояние неактивный
				if (is_in2) inputs &= ~1; 	//выключить сигнал
				else inputs |= 1; 			//включить сигнал
			}
		}
		if (input_config & 0x20) { 			//если вход настроен на ступень
			if (input_config & 0x40) { 		//открытое состояние активный
				if (is_in2) inputs |= 2; 	//включить сигнал
				else inputs &= ~2; 			//выключить сигнал
			}
			if (input_config & 0x80) { 		//открытое состояние неактивный
				if (is_in2) inputs &= ~2; 	//выключить сигнал
				else inputs |= 2; 			//включить сигнал
			}
		}
	}

//выхода
	out = 0;
	if (out1_config) { //выход 1
		for (i = 0; i < 7; i++) {
			if ((light_mask & 1 << i) && (out1_config & 1 << i)){
				out = 1; //если выход задействован и включен
				break;
			}
		}
		if (out) { //если нужно включить выход
			if (out1_config & inside_light_mask) { //если освещение отсеков
				if (inputs & 1) set_out1; //проверяем роллету
				else reset_out1;
			} else set_out1; //если не освещение отсеков, включаем выход
		} else reset_out1;
	}

	out = 0;
	if (out2_config) { //выход 2
		for (i = 0; i < 7; i++) {
			if ((light_mask & 1 << i) && (out2_config & 1 << i)){
				out = 1; //если выход задействован и включен
				break;
			}
		}
		if (out) { //если нужно включить выход
			if (out2_config & inside_light_mask) { //если освещение отсеков
				if (inputs & 1) set_out2; //проверяем роллету
				else reset_out2;
			} else set_out2; //если не освещение отсеков, включаем выход
		} else reset_out2;
	}

	out = 0;
	if (out3_config) { //выход 3
		for (i = 0; i < 7; i++) {
			if ((light_mask & 1 << i) && (out3_config & 1 << i)){
				out = 1; //если выход задействован и включен
				break;
			}
		}
		if (out) { //если нужно включить выход
			if (out3_config & inside_light_mask) { //если освещение отсеков
				if (inputs & 1) set_out3; //проверяем роллету
				else reset_out3;
			} else set_out3; //если не освещение отсеков, включаем выход
		} else reset_out3;
	}

	out = 0;
	if (out4_config) { //выход 3
		for (i = 0; i < 7; i++) {
			if ((light_mask & 1 << i) && (out4_config & 1 << i)){
				out = 1; //если выход задействован и включен
				break;
			}
		}
		if (out) { //если нужно включить выход
			if (out4_config & inside_light_mask) { //если освещение отсеков
				if (inputs & 1) set_out4; //проверяем роллету
				else reset_out4;
			} else set_out4; //если не освещение отсеков, включаем выход
		} else reset_out4;
	}

	tmp = 0xF0;
	if(inputs & 1) tmp |= 1;
	if(inputs & 2) tmp |= 4;
	TxMessage.IDE = CAN_Id_Standard;
	TxMessage.StdId = power_address;	//адрес модуля
	TxMessage.DLC=1;
	TxMessage.Data[0] = tmp;
	GPIO_ResetBits(GPIOA, GPIO_Pin_15);
	CAN_Transmit(CAN, &TxMessage);
	if(CAN_GetLastErrorCode(CAN)){		//ошибка отправки
		//GPIO_SetBits(GPIOA, GPIO_Pin_9);
	}

}



void Init_Timer(){

	//таймер CAN
	TIM17->PSC = 239;
	TIM17->ARR = 16000;		//период 160 мс
	TIM17->DIER |= TIM_DIER_UIE;
	TIM17->CR1 |= TIM_CR1_CEN;


	//вектор
	NVIC_InitStructure.NVIC_IRQChannel = TIM17_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}


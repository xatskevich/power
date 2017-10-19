
#include "main.h"

void USART1_IRQHandler(void){

	uint8_t tmp;

	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){

		tmp = USART_ReceiveData(USART1);

		if(tmp == 0x02){	//пришло начало посылки
			uart_cnt = 1;
		}
		else if(uart_cnt != 0){
			if(tmp != 0x03){			//прием данных
			uart_rcv[uart_cnt] = tmp;
			}
			if(tmp == 0x03){		//окончание посылки
				if(uart_rcv[1] == 0x31){		//концевики
					inputs = uart_rcv[2];
				}
				if(uart_rcv[1] == 0x32){		//обороты
					rpm = (uart_rcv[2] << 8) | uart_rcv[3];
				}
				if(uart_rcv[1] == 0x35){		//давление перегрев
					engine = uart_rcv[2];
					if(engine & 1){				//давление
						can.motor3 &= ~3;
						can.motor3 |= 1;
					} else {
						can.motor3 &= ~3;
					}
					if(engine & 2){				//перегрев
						can.motor3 &= ~0xC;
						can.motor3 |= 4;
					} else {
						can.motor3 &= ~0xC;
					}
				}
			}
		}
	}
}

void Init_uart(){

	USART_StructInit(&USART_InitStructure);

	USART_InitStructure.USART_BaudRate = 250000;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_Even;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_Cmd(USART1, ENABLE);
}


//#include "main.h"
#include "CAN.h"

void CEC_CAN_IRQHandler (void)
{
	uint8_t tmp;

	if (CAN_GetITStatus(CAN,CAN_IT_TME))
	{
		CAN_ClearITPendingBit(CAN, CAN_IT_TME);
		//GPIOA->ODR ^= 0x400;
		GPIO_SetBits(GPIOA, GPIO_Pin_15);
	}

	
	if (CAN_GetITStatus(CAN,CAN_IT_FMP0))
	{
		CAN_ClearITPendingBit(CAN, CAN_IT_FMP0);

	can_on = 1;
	//GPIOA->ODR ^= 0x400;

	CanRxMsg msg_buf;
	CAN_Receive(CAN, CAN_FIFO0, &msg_buf);
	switch(msg_buf.FMI){
	case 0:		//0x028
		if((msg_buf.Data[0] & 0x30) == 0x10) light_mask |= strobe_light_mask;	//строб
		if((msg_buf.Data[0] & 0x30) == 0) light_mask &= ~(strobe_light_mask);

		if((msg_buf.Data[1] & 0x3) == 0x1) light_mask |= beam_light_mask;	//маяк
		if((msg_buf.Data[1] & 0x3) == 0) light_mask &= ~(beam_light_mask);

		if((msg_buf.Data[1] & 0xC) == 0x4) light_mask |= rear_light_mask;	//задний прожектор
		if((msg_buf.Data[1] & 0xC) == 0) light_mask &= ~(rear_light_mask);

		if((msg_buf.Data[2] & 0x3) == 0x1) light_mask |= left_light_mask;	//освещение слева
		if((msg_buf.Data[2] & 0x3) == 0) light_mask &= ~(left_light_mask);

		if((msg_buf.Data[2] & 0xC) == 0x4) light_mask |= right_light_mask;	//освещение справа
		if((msg_buf.Data[2] & 0xC) == 0) light_mask &= ~(right_light_mask);

		if((msg_buf.Data[2] & 0x30) == 0x10) light_mask |= inside_light_mask;	//освещение отсеков
		if((msg_buf.Data[2] & 0x30) == 0) light_mask &= ~(inside_light_mask);
	
		if((msg_buf.Data[2] & 0x3) == 0x1) GPIO_SetBits(GPIOA, GPIO_Pin_10);	//освещение слева
		if((msg_buf.Data[2] & 0x3) == 0) GPIO_ResetBits(GPIOA, GPIO_Pin_10);


		break;
	case 1:		//0x0AE
		TxMessage.IDE = CAN_Id_Standard;
		TxMessage.StdId = 0x0AE;
		TxMessage.DLC=1;
		TxMessage.Data[0] = power_address;	//адрес модуля
		GPIO_ResetBits(GPIOA, GPIO_Pin_15);
		CAN_Transmit(CAN, &TxMessage);
		if(CAN_GetLastErrorCode(CAN)){		//ошибка отправки
			//GPIO_SetBits(GPIOA, GPIO_Pin_9);
		}
		break;
	case 2:		//0x0AD
		input_config = msg_buf.Data[0];			//конфигурация входов
		output_config = msg_buf.Data[1];

		switch(output_config & 3){
		case 0:
			tmp = power_address;
			if((tmp == 0x50) || (tmp == 0x51) || (tmp == 0x52) || (tmp == 0x53)) out1_config = left_light_mask;
			if((tmp == 0x60) || (tmp == 0x61) || (tmp == 0x62) || (tmp == 0x63)) out1_config = right_light_mask;
			if(tmp == 0x56) out1_config = beam_light_mask;
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
		switch((output_config & 0xC) >> 2){
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

		//стирание flash
		flash_erase();
		//запись
		flash_prog_all();

		break;
		case 3:
			if((msg_buf.Data[4] & 0xC0) == 0x40) light_mask |= tail_light_mask;	//габарит
			if((msg_buf.Data[4] & 0xC0) == 0) light_mask &= ~(tail_light_mask);
			break;
	}
	}
}

void Init_CAN(){

	/* CAN register init */
	CAN_DeInit(CAN);

	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = ENABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;

	/* CAN Baudrate = 250 kBps (CAN clocked at 24 MHz) */
	CAN_InitStructure.CAN_BS1 = CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_7tq;
	CAN_InitStructure.CAN_Prescaler = 6;
	CAN_Init(CAN, &CAN_InitStructure);
	/* Enable FIFO 0 message pending Interrupt */
	CAN_ITConfig(CAN, CAN_IT_FMP0, ENABLE);
	CAN_ITConfig(CAN, CAN_IT_TME, ENABLE);
	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x028<<5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0ff<<5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 1;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0AE<<5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0ff<<5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 2;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0AD<<5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0ff<<5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_FilterInitStructure.CAN_FilterNumber = 3;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x011<<5;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0ff<<5;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	//вектор
	NVIC_InitStructure.NVIC_IRQChannel = CEC_CAN_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

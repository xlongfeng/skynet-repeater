/*
 * uart.c
 */

#include "stm32f10x.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "uart.h"

struct UartDevice halfDuplexUart;

void USART2_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		uint8_t uByte = USART_ReceiveData(USART2);
		xQueueSendFromISR(halfDuplexUart.rx, &uByte, &xHigherPriorityTaskWoken);
	}

	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET) {
		uint8_t uByte;

		if (xQueueReceiveFromISR(halfDuplexUart.tx, &uByte, &xHigherPriorityTaskWoken)) {
			USART_SendData(USART2, uByte);
		} else {
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
			USART_ITConfig(USART2, USART_IT_TC, ENABLE);
			halfDuplexUart.itTxEnable = DISABLE;
		}
	}

	if(USART_GetITStatus(USART2, USART_IT_TC) != RESET) {
		USART_ITConfig(USART2, USART_IT_TC, DISABLE);
		halfDuplexUart.tc = Bit_SET;
	}
}

typedef enum {
	MODE_TRANSMIT,
	MODE_RECEIVE
} HalfDuplexUartMode;

void halfDuplexUartModeSwitch(HalfDuplexUartMode mode)
{
	if (mode == MODE_RECEIVE) {
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET);
		GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET);
	} else {
		GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);
		GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);
	}
}

void halfDuplexUartInitialize(void)
{
	halfDuplexUart.tx = xQueueCreate(UART_QUEUE_SIZE, sizeof(uint8_t));
	halfDuplexUart.rx = xQueueCreate(UART_QUEUE_SIZE, sizeof(uint8_t));

	NVIC_InitTypeDef NVIC_InitStructure;
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	GPIO_InitTypeDef GPIO_InitStructure;
	/* Configure USART2 Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART2 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* MAX485 /RE */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* MAX485 DE */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	halfDuplexUartModeSwitch(MODE_RECEIVE);

	USART_InitTypeDef USART_InitStructure;
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = 38400;
	USART_Init(USART2, &USART_InitStructure);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	USART_Cmd(USART2, ENABLE);
}

int32_t halfDuplexUartWrite(uint8_t *ptr, int32_t len)
{
	int32_t i;

	for (i = 0; i < len; i++) {
		if (!xQueueSendToBack(halfDuplexUart.tx, ptr + i, UART_TRANSMIT_TIMEOUT))
			break;
	}

	halfDuplexUartModeSwitch(MODE_TRANSMIT);

	halfDuplexUart.itTxEnable = ENABLE;
	halfDuplexUart.tc = Bit_RESET;
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);

	while (halfDuplexUart.itTxEnable || !halfDuplexUart.tc) {
		taskYIELD();
	}

	halfDuplexUartModeSwitch(MODE_RECEIVE);

	return i;
}

int32_t halfDuplexUartRead(uint8_t *ptr, int32_t len)
{
	int32_t i;

	for (i = 0; i < len; i++) {
		if (!xQueueReceive(halfDuplexUart.rx, ptr + i, UART_TRANSMIT_TIMEOUT))
			break;
	}

	return i;
}

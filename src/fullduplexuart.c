/*
 * uart.c
 */

#include "stm32f10x.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "uart.h"

struct UartDevice fullDuplexUart;

void USART1_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		uint8_t uByte = USART_ReceiveData(USART1);
		xQueueSendFromISR(fullDuplexUart.rx, &uByte, &xHigherPriorityTaskWoken);
	}

	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET) {
		uint8_t uByte;

		if (xQueueReceiveFromISR(fullDuplexUart.tx, &uByte, &xHigherPriorityTaskWoken)) {
			USART_SendData(USART1, uByte);
		} else {
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
			fullDuplexUart.itTxEnable = DISABLE;
		}
	}
}

void fullDuplexUartInitialize(void)
{
	fullDuplexUart.tx = xQueueCreate(UART_QUEUE_SIZE, sizeof(uint8_t));
	fullDuplexUart.rx = xQueueCreate(UART_QUEUE_SIZE, sizeof(uint8_t));

	NVIC_InitTypeDef NVIC_InitStructure;
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	/* Configure USART1 Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure USART1 Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	USART_InitTypeDef USART_InitStructure;
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = 38400;
	USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	USART_Cmd(USART1, ENABLE);
}

int32_t fullDuplexUartWrite(uint8_t *ptr, int32_t len)
{
	int32_t i;

	for (i = 0; i < len; i++) {
		if (!xQueueSendToBack(fullDuplexUart.tx, ptr + i, UART_TRANSMIT_TIMEOUT))
			break;
	}

	taskENTER_CRITICAL();
	if (!fullDuplexUart.itTxEnable && !xQueueIsQueueEmptyFromISR(fullDuplexUart.tx)) {
		fullDuplexUart.itTxEnable = ENABLE;
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	}
	taskEXIT_CRITICAL();

	return i;
}

int32_t fullDuplexUartRead(uint8_t *ptr, int32_t len)
{
	int32_t i;

	for (i = 0; i < len; i++) {
		if (!xQueueReceive(fullDuplexUart.rx, ptr + i, UART_TRANSMIT_TIMEOUT))
			break;
	}

	return i;
}

int stdoutWrite(uint8_t *ptr, int32_t len)
{
	for (int i = 0; i < len; i++) {
		if (ptr[i] == '\n') {
			static const uint8_t cr = '\r';
			fullDuplexUartWrite((uint8_t *)&cr, 1);
		}
		fullDuplexUartWrite(ptr + i, 1);
	}
	return len;
}

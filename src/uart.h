/*
 * uart.h
 */

#ifndef _UART_H_
#define _UART_H_

#define UART_QUEUE_SIZE		64

// baudrate 19200, 2byte/ms
#define UART_TRANSMIT_TIMEOUT	(pdMS_TO_TICKS(5))

struct UartDevice {
	QueueHandle_t tx;
	QueueHandle_t rx;
	FunctionalState itTxEnable;
	BitAction tc;
};

void fullDuplexUartInitialize(void);
int32_t fullDuplexUartWrite(uint8_t *ptr, int32_t len);
int32_t fullDuplexUartRead(uint8_t *ptr, int32_t len);

void halfDuplexUartInitialize(void);
int32_t halfDuplexUartWrite(uint8_t *ptr, int32_t len);
int32_t halfDuplexUartRead(uint8_t *ptr, int32_t len);

#endif /* _UART_H_ */

/*************************************************************************
	> File Name: fifo.h
	> Author: Yan Li 
	> Mail: yan.li@apple.com 
	> Created Time: Sun Apr 19 14:29:22 2020
 ************************************************************************/
#include "main.h"
#include <stdio.h>
//#include "stm32l0xx_hal_uart.h"

extern UART_HandleTypeDef testUartHandle;
extern TIM_HandleTypeDef htim3;
uint32_t* ADDR;

// uart fifo type define
typedef struct
{
	uint8_t buf[128]; // length of the fifo is 128 bytes
	volatile uint8_t WrIdx; // Write pointer index
	volatile uint8_t RdIdx; // Read pointer index
}fifo_typedef;

extern fifo_typedef aidfifo;
extern fifo_typedef SRF_fifo;

// test command type define
typedef struct 
{
	char command[128];
	char* cmd;
	char* param1;
	char* param2;
	char* param3;
}testCmd_typedef;


void fifoReset(fifo_typedef *fifo);
void fifoWrite(fifo_typedef *fifo, uint8_t data);
uint8_t fifoRead(fifo_typedef *fifo);
void fifoParser(fifo_typedef *fifo);

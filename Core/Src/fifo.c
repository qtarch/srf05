/*************************************************************************
	> File Name: fifo.c
	> Author: Yan Li 
	> Mail: yan.li@apple.com 
	> Created Time: Sun Apr 19 14:28:53 2020
 ************************************************************************/

#include "fifo.h"
#include "user_func.h"
#include <string.h>
#include <stdlib.h>
#include "gpio.h"
#include "tim.h"

void fifoReset(fifo_typedef *fifo)
{
	fifo->RdIdx = fifo->WrIdx = 0;
}

void fifoWrite(fifo_typedef *fifo, uint8_t data)
{
	fifo->buf[fifo->WrIdx] = data;
	fifo->WrIdx==127? (fifo->WrIdx=0): fifo->WrIdx++;
}

uint8_t fifoRead(fifo_typedef *fifo)
{
	uint8_t data;

	if(fifo->WrIdx>fifo->RdIdx)
	{
		data = fifo->buf[fifo->RdIdx];
		fifo->RdIdx==127? (fifo->RdIdx=0) : fifo->RdIdx++;
	}
	else data = 0;
	return data;
}

void fifoParser(fifo_typedef *fifo)
{
	uint8_t cmd_length = 0;
	uint8_t* cmd;
	uint8_t i=0;
	char response[128]="";
	uint8_t res_length = 0; // length of the response in bytes
	testCmd_typedef testCmd;
	uint32_t data=0;

	testCmd.cmd="";
	memset(testCmd.command,0,128); // Clean the command buffer; Otherwise there will be gabage to hang MCU
    
	if((fifo->buf[fifo->WrIdx-1]==0x0A)&&(fifo->buf[fifo->WrIdx-2]==0x0D))  // if current input is 0x0A, and previous input is 0x0D (return)
	{ 
		if (fifo->WrIdx > fifo-> RdIdx)   // if writer pointer is ahead of read pointer
		{
			cmd_length = fifo->WrIdx - fifo->RdIdx-2;  // Remove \r\n characters
			cmd = (uint8_t *) (fifo->buf+fifo->RdIdx); // cmd start from the address (fifo->buf + RdIdx+1)
			fifo->WrIdx = fifo->RdIdx; // back off writer pointer to read pointer
			if (cmd_length>0)
			{ 
				// Parsing the input command and parameters 
				while(i<cmd_length) {testCmd.command[i]=cmd[i];i++;}
				testCmd.cmd = strtok(testCmd.command, " "); 
				if(testCmd.command!=NULL) testCmd.param1= strtok(NULL," "); else testCmd.param1=NULL; 
				if(testCmd.command!=NULL) testCmd.param2= strtok(NULL," "); else testCmd.param2=NULL; 
				if(testCmd.command!=NULL) testCmd.param3= strtok(NULL," "); else testCmd.param3=NULL;
			}
			sprintf(response, "]%s",cmd);
			res_length = cmd_length+2;
			HAL_UART_Transmit(&testUartHandle, (uint8_t *) response,res_length,1000); // output, the length of output is considered 

		// memory read function - mr32(addr)
		if (strcmp(testCmd.cmd,"mr32")==0) 
			{
				if(testCmd.param1!=NULL) 
				{
					data = mr32((uint32_t) strtol(testCmd.param1,NULL,16)); // input format is hex like 0x40004400 or FFFFFFFFFF
					sprintf(response,"<OK:%0#10x\n",(unsigned int) data); // output 4bytes 
					res_length = 15;
				}else
				{
					sprintf(response,"<Error:mr32 address\n"); // help
					res_length = 20;
				}
				HAL_UART_Transmit(&testUartHandle, (uint8_t *) response,res_length,1000); // output, the length of output is considered 
			}

		// memory write function - mw32(addr, data)
		if (strcmp(testCmd.cmd,"mw32")==0) 
			{
				if((testCmd.param1!=NULL)&&(testCmd.param2!=NULL))
				{
					int32_t address = (uint32_t) strtol(testCmd.param1,NULL,16);
					mw32(address, (uint32_t) strtol(testCmd.param2,NULL,16)); // input format is hex like 0x40004400 or FFFFFFFFFF
					data = mr32((uint32_t) address); // input format is hex like 0x40004400 or FFFFFFFFFF
					sprintf(response,"<OK:%0#10x, %0#10x\n", (unsigned int) address,(unsigned int) data); // output 4bytes 
					res_length = 27;
				}else
				{
					sprintf(response,"<Error:mw32 address data\n"); // help
					res_length = 25;
				}
				HAL_UART_Transmit(&testUartHandle, (uint8_t *) response,res_length,1000); // output, the length of output is considered 
			}

		// SRF measurement function - srf_go()
		if (strcmp(testCmd.cmd,"srf_go")==0) 
			{
				testUartHandle.Instance->CR1 = 0x00; // Disable testUart
				HAL_GPIO_WritePin(GPIOB, SRF_TRIG, GPIO_PIN_SET); // Generate a high pulse on Trigger pin
				HAL_Delay(1);
				HAL_GPIO_WritePin(GPIOB, SRF_TRIG, GPIO_PIN_RESET);
				testUartHandle.Instance->CR1 = 0x2d; // Enable testUart
				while(SRF_fifo.RdIdx==SRF_fifo.WrIdx){}; // Wait until data is updated
				uint8_t msb_byte=fifoRead(&SRF_fifo);
				uint8_t lsb_byte=fifoRead(&SRF_fifo);
				unsigned int data= (unsigned int) ((msb_byte<<8)+lsb_byte);
				double distance = data*17/2000;
				fifoReset(&SRF_fifo);

				sprintf(response,"<OK:%0#6x, Distance=%3d cm\n", (unsigned int) data, (unsigned int) distance); // output 4bytes 
				res_length = 11+17;
				HAL_UART_Transmit(&testUartHandle, (uint8_t *) response,res_length,1000); // output, the length of output is considered 
			}

		if (strcmp(testCmd.cmd,"timer")==0) 
			{
				if((testCmd.param1!=NULL)&&(testCmd.param2!=NULL))
				{
					uint16_t TimerCnt = htim3.Instance->CNT; // Base timer counter
					uint16_t TimerCC3Cnt = htim3.Instance->CCR3; // input capture 1 register
					uint16_t TimerCC4Cnt = htim3.Instance->CCR4; // input capture 2 register
					uint16_t TimerSR = htim3.Instance->SR; // Status register

					sprintf(response,"<OK:%0#6x, %0#6x, %0#6x, %0#6x\n", (unsigned int) TimerCnt,(unsigned int) TimerCC3Cnt, (unsigned int) TimerCC4Cnt, (unsigned int) TimerSR); // output 4bytes 
					res_length = 35;
				}
				if(strcmp(testCmd.param1,"go")==0)
				{
					MX_TIMER_Enable();
					sprintf(response,"<OK: Timer go\n"); // help
					res_length = 14;
				}
				if(strcmp(testCmd.param1,"stop")==0)
				{
					MX_TIMER_Disable();
					sprintf(response,"<OK: Timer stop\n"); // help
					res_length = 16;
				}
				HAL_UART_Transmit(&testUartHandle, (uint8_t *) response,res_length,1000); // output, the length of output is considered 
			}

		// When execution is over, print a new ] for notification
		sprintf(response,"]\n"); // output 4bytes 
		res_length = 2;
		HAL_UART_Transmit(&testUartHandle, (uint8_t *) response,res_length,1000); // output, the length of output is considered 
		}
	}
}
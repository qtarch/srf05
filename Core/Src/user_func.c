/* Includes ------------------------------------------------------------------*/
#include "user_func.h"
#include <stdbool.h>
#include "stm32l0xx_hal_uart.h"

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&testUartHandle, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

int _write(int file, char *data, int len)
{
   if ((file != STDOUT_FILENO) && (file != STDERR_FILENO))
   {
      errno = EBADF;
      return -1;
   }

   // arbitrary timeout 1000
   HAL_StatusTypeDef status =
      HAL_UART_Transmit(&testUartHandle, (uint8_t*)data, len, 1000);

   // return # of bytes written - as best we can tell
   return (status == HAL_OK ? len : 0);
}

// Print a floating number
void printfloat(float num)
{
	char *tmpSign = (num < 0) ? "-" : "";
	float tmpVal = (num < 0) ? -num : num;

	int tmpInt1 = tmpVal;                  // Get the integer (678).
	float tmpFrac = tmpVal - tmpInt1;      // Get fraction (0.0123).
	int tmpInt2 = trunc(tmpFrac * 1000000);  // Turn into integer (123).

	// Print as parts, note that you need 0-padding for fractional bit.
	printf ("%s%d.%06d\r\n", tmpSign, tmpInt1, tmpInt2);
}

// The hash function is used to translate a string to hash number
int hash(char *str)
{
    unsigned long hash = 0;
    int n; 

    while ((n = *str++))
//        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    	  hash = (hash<<8) + n;
    return hash;
}

uint16_t I2CAddress(char *addr)
{
	uint16_t DevAddress=255; //initialize i2c slave address to 7'b1111111

	if (strcmp(addr, "io2")==0){ 
		DevAddress=ADDR_PCAIO2;
	}
	if (strcmp(addr, "fuel")==0){ 
		DevAddress=ADDR_FUELGAUGE;
	}
	if (strcmp(addr, "led")==0){ 
		DevAddress=ADDR_LEDDRIVER;
	}
	if (strcmp(addr, "beryl")==0){ 
		DevAddress=ADDR_BERYL;
	}
	return (DevAddress<<1); //STM32 needs to shift 1bit for r/w control bit
}

// I2C bus scan from 0x00 to 0x7F
void I2CScan(char* DeviceList)
{
	uint16_t I2CAddr = 1;
	HAL_StatusTypeDef I2C_Status;
	char i2c_addr[5]; 

	sprintf(DeviceList,"I2C slaves: ");

	while(I2CAddr<=0x7F) //Scan I2C address from 0x01 to 0x7F
	{
		I2C_Status=HAL_I2C_IsDeviceReady(&hi2c1, I2CAddr, 2, 1000);
		if (I2C_Status!=0x03) 
		{
			sprintf(i2c_addr, "%x", I2CAddr);
			strcat(DeviceList, i2c_addr);
		}
		I2CAddr++;
	}
}

HAL_StatusTypeDef CheckI2CStatus(char *addr)
{
	HAL_StatusTypeDef I2C_Status=HAL_OK;
	uint16_t DevAddress;

	DevAddress=I2CAddress(addr);

	I2C_Status=HAL_I2C_IsDeviceReady(&hi2c1, DevAddress, 2, 1000);
	return I2C_Status;
}

HAL_StatusTypeDef I2CRead(char *addr, uint8_t *pData, uint16_t Size)
{
	HAL_StatusTypeDef I2C_Status=HAL_OK;
	uint16_t DevAddress;

	DevAddress=I2CAddress(addr);

	I2C_Status=HAL_I2C_Master_Receive(&hi2c1, DevAddress, pData, Size, 2);
	return I2C_Status;
}

HAL_StatusTypeDef I2CReadReg(char *addr, uint16_t RegAdd, uint16_t RegSize, uint8_t *pData, uint16_t Size)
{
	HAL_StatusTypeDef I2C_Status=HAL_OK;
	uint16_t DevAddress;

	if(RegSize==8){RegSize=I2C_MEMADD_SIZE_8BIT;}
	if(RegSize==16){RegSize=I2C_MEMADD_SIZE_16BIT;}

	DevAddress=I2CAddress(addr);

	I2C_Status=HAL_I2C_Mem_Read(&hi2c1, DevAddress, RegAdd, RegSize, pData, Size, 2);
	return I2C_Status;
}

HAL_StatusTypeDef I2CWrite(char *addr, uint8_t *pData, uint16_t Size)
{
	HAL_StatusTypeDef I2C_Status=HAL_OK;
	uint16_t DevAddress;

	DevAddress=I2CAddress(addr);

	I2C_Status=HAL_I2C_Master_Transmit(&hi2c1, DevAddress, pData, Size, 2);
	return I2C_Status;
}

HAL_StatusTypeDef I2CWriteReg(char *addr, uint16_t RegAdd, uint16_t RegSize, uint8_t *pData, uint16_t Size)
{
	HAL_StatusTypeDef I2C_Status=HAL_OK;
	uint16_t DevAddress;

	if(RegSize==8){RegSize=I2C_MEMADD_SIZE_8BIT;}
	if(RegSize==16){RegSize=I2C_MEMADD_SIZE_16BIT;}

	DevAddress=I2CAddress(addr);

	I2C_Status=HAL_I2C_Mem_Write(&hi2c1, DevAddress, RegAdd, RegSize, pData, Size, 2);
	return I2C_Status;
}

uint32_t mr32(uint32_t address)
{
	ADDR = (uint32_t *) address;
	return *ADDR;
}

void mw32(uint32_t address, uint32_t data)
{
	ADDR = (uint32_t *) address; // Change the data type of address to pointer -- use address of the pointer
	*ADDR = data;  // assign the data to the address
}
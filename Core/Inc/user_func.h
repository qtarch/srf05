// User function definitions
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <sys/unistd.h> // STDOUT_FILENO, STDERR_FILENO
#include "stm32l0xx_hal.h"

/* Private variables ---------------------------------------------------------*/
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#define ADDR_BERYL 0x3C
#define ADDR_BORON2 0x19
#define ADDR_ROSWELL 0x10
#define ADDR_ADAMS1 0x34
#define ADDR_ADAMS2 0x31
#define ADDR_TRISTAR 0x1A
#define ADDR_FUELGAUGE 0x55
#define ADDR_LEDDRIVER 0x45 //DEV, secondary
#define ADDR_PCAIO 0x24
#define ADDR_PCAIO2 0x43 //DEV, Secondary

extern UART_HandleTypeDef testUartHandle;
extern I2C_HandleTypeDef hi2c1;
extern uint32_t* ADDR;

PUTCHAR_PROTOTYPE;
int _write(int file, char *data, int len);
void printfloat(float num);
int hash(char *str);

uint16_t I2CAddress(char *addr);
void I2CScan(char* DeviceList);

HAL_StatusTypeDef CheckI2CStatus(char *addr);
HAL_StatusTypeDef I2CRead(char *addr, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef I2CReadReg(char *addr, uint16_t RegAdd, uint16_t RegSize, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef I2CWrite(char *addr, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef I2CWriteReg(char *addr, uint16_t RegAdd, uint16_t RegSize, uint8_t *pData, uint16_t Size);

uint32_t mr32(uint32_t address);
void mw32(uint32_t address, uint32_t data);
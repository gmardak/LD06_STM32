# LD06
This is the driver for affordable LD06 lidar when using STM32 boards. You need to copy two files: ld06.h header file that will go to core/Inc folder and ld06.c that goes to the core/Src folder. To start receiving data from lidar, You have to initialize an UART channel and set it to DMA mode. The header file contains the structure of the parsed lidar data. Thus you have to declare LiDARFrameTypeDef, which will store lidar data, in your main function. Here is the example of the code for Nucleo-H723ZG board, after you copied the files:
```
#include "main.h"
#include "ld06.h"
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */
uint8_t lidar_data = 0;
uint8_t data_buffer[47] = {0};
uint8_t count = 0;
uint8_t crc_check_sum; 
/* USER CODE END PV */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN 0 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(lidar_data == 0x54 && data_buffer[0] == 0)
	{
		data_buffer[count] = lidar_data;
		count++;
	}
	else if (count <= 46 && data_buffer[0] == 0x54)
	{
		data_buffer[count] = lidar_data;
		count++;
	}
	return;
}
/* USER CODE END 0 */
int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  LiDARFrameTypeDef lf;
  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(count <= 46)
	  	  {
	  		  HAL_UART_Receive_DMA(&huart1, &lidar_data, 1);
	  	  }
	  	  else
	  	  {
	  			crc_check_sum = CalCRC8(data_buffer, 46);
	  			if(crc_check_sum == data_buffer[46])
	  			{
	  				lf = AssignValues(data_buffer);
	  			}
	  			data_buffer[0] = 0;
	  			crc_check_sum = 0;	//to hold return value of crc function
	  			count = 0;
	  	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
```
The lidar send data in 47 bytes. The first byte is the start byte and the last is CRC check byte. Every lidar data frame contains 12 points. At the default speed, the lidar's start angle and end angle are 8-9 degrees apart. Here is the LiDARFrameTypeDef struct:
```
typedef struct __attribute__ ((packed))
{
	float angle;
	uint16_t distance;
	uint16_t confidence;
}LidarPointStructDef;

typedef struct __attribute__((packed))
{
	uint8_t header;
	uint8_t ver_len;
	uint32_t speed;
	uint16_t start_angle;
	LidarPointStructDef point[12];
	uint16_t end_angle;
	uint16_t timestamp;
	uint8_t crc8;
	uint32_t distance_average;
}LiDARFrameTypeDef;
```
If you want to print it, you will get the following results:
start-byte: 54 
data-length: 2C 
speed: 3584 
start_angle: 143 
distance: 1225 
confidence: 237 
end_angle: 152 
time_stamp: 7940 
cs: F7 
Please email me if you need nay clarification. 

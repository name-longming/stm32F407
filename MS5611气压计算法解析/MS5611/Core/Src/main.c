/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define  MS561101BA_SlaveAddress 	(0x77)  //定义器件在IIC总线中的从地址
#define  MS561101BA_D1 				(0x40) 
#define  MS561101BA_D2 				(0x50)
#define  MS561101BA_RST 			(0x1E)
#define  MS561101BA_PROM_RD 		(0xA0) 	//出厂校准值起始地址

uint16_t Cal_C[8];

uint32_t D1_Pres,D2_Temp; 	// 存放数字压力和温度
double dT , Temperature , T2;	//实际和参考温度之间的差异,实际温度,中间值

double Pressure;					//温度补偿大气压
double OFF , SENS;  			//实际温度抵消,实际温度灵敏度
double Aux , OFF2 , SENS2;  		//温度校验值
double MS5611_Pressure;
static double Alt_offset_Pa=1013.25;
double paOffsetNum = 0;
uint16_t  paInitCnt=0;
uint8_t paOffsetInited=0;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t MS5611_Write(uint8_t addr,uint8_t commnd,uint8_t len)
{
     HAL_I2C_Master_Transmit(&hi2c1, (addr<<1), &commnd, len, 0xfff);
     return 0;
}

uint8_t MS5611_Read_Len(uint8_t addr, uint8_t *buf, uint8_t len)
{ 
    HAL_I2C_Master_Receive(&hi2c1, (addr<<1), buf, len, 0xfff);
    return 0;       
}



uint8_t MS561101BA_RESET(void)
{	
	uint8_t res;
	MS5611_Write(MS561101BA_SlaveAddress,MS561101BA_RST,1);
	return res;
}


uint8_t MS561101BA_READ_PROM(void)
{
	uint8_t buf[2],i;
	
	for(i=0;i<8;i++)
	{
		MS5611_Write(MS561101BA_SlaveAddress,MS561101BA_PROM_RD+i*2,1);
		MS5611_Read_Len(MS561101BA_SlaveAddress,buf,2);
		Cal_C[i]=buf[0]<<8 | buf[1];
		printf("Cal_C[%d]=%d\r\n",i,Cal_C[i]);
	}
	return (Cal_C[0]);
}

uint8_t MS5611_CRC(uint16_t *prom)
{
    int32_t i, j;
    uint32_t res = 0;
    uint8_t zero = 1;
    uint8_t crc = prom[7] & 0xF;
    prom[7] &= 0xFF00;
 
    // if eeprom is all zeros, we're probably fucked - BUT this will return valid CRC lol
    for (i = 0; i < 8; i++) 
	{
        if (prom[i] != 0)
            zero = 0;
    }
 
    if (zero)
        return 1;
 
    for (i = 0; i < 16; i++) {
        if (i & 1)
            res ^= ((prom[i >> 1]) & 0x00FF);
        else
            res ^= (prom[i >> 1] >> 8);
        for (j = 8; j > 0; j--) {
            if (res & 0x8000)
                res ^= 0x1800;
            res <<= 1;
        }
    }
 
    prom[7] |= crc;
    if (crc == ((res >> 12) & 0xF))
        return 0;
	return 1;
}


uint32_t MS561101BA_DO_CONVERSION(uint8_t command)
{
	uint32_t conversion = 0x00;
	uint8_t conv[3];
	
	MS5611_Write(MS561101BA_SlaveAddress,command,1);
	HAL_Delay(12);
	MS5611_Write(MS561101BA_SlaveAddress,0x00,1);
	
	MS5611_Read_Len(MS561101BA_SlaveAddress,conv,3);
	conversion = (conv[0] << 16) + (conv[1] << 8) + conv[2];
	
	return conversion;
}

//读取数字温度AD转换值,计算温度
void MS561101BA_GetTemperature(uint8_t OSR_Temp)
{   
	D2_Temp = MS561101BA_DO_CONVERSION(OSR_Temp);
	HAL_Delay(12);		//8497614
		
 
//警告：当温度低于20度，计算的大气压力不正确的解决方法	

	//上面2行代码，当温度值高于等于20度时，计算的压力值正确。
	//但是上面2行代码，当温度值低于20度时，有问题。因为D2_Temp和Cal_C[5] )都是无符号数，
	//无符号数之间使用减法，没法得到负值，所以必须要按照下述代码修改一下。
	
	if (D2_Temp > (((uint32_t)Cal_C[5]) << 8 ))
	{
		dT	= D2_Temp - (((uint32_t)Cal_C[5]) << 8 );
//		T2 = 0;
	}
	else
	{
		dT	= ((( uint32_t)Cal_C[5]) << 8) - D2_Temp;
//		T2 = (dT * dT)/(double)2147483648.0;
		dT *= -1;
	}

	
	Temperature =  (double)(2000 + dT*((uint32_t)Cal_C[6])/(double)8388608.0);
//	Temperature = Temperature - T2;
	//printf("Temperature = %lf\r\n",Temperature);
	//Temperature = (float)(2000 + dT*((uint32_t)Cal_C[6])/(float)8388608.0);//算出温度值的100倍，2001表示20.01°
}


void MS561101BA_GetPressure(uint8_t OSR_Pres)
{	
	D1_Pres = MS561101BA_DO_CONVERSION(OSR_Pres);
	//MS5611_delay_ms(10);
	HAL_Delay(12);
	OFF =  (uint32_t)(Cal_C[2] << 16) + ((uint32_t)Cal_C[4] * dT) / 128;
	SENS = (uint32_t)(Cal_C[1] << 15) + ((uint32_t)Cal_C[3] * dT) / 256;
	
	//温度补偿
	if (Temperature < 2000)	// second order temperature compensation when under 20 degrees C
	{
		T2 = (dT*dT) / 0x80000000;
		Aux = (Temperature - 2000)*(Temperature - 2000);
		OFF2 = (double)(2.5)*Aux;
		SENS2 = (double)(1.25)*Aux;
		if (Temperature < -1500)
		{
			Aux = (Temperature + 1500)*(Temperature + 1500);
			OFF2 = OFF2 + 7 * Aux;
			SENS2 = SENS + (float)(5.5)*Aux;
		}
	}
	else //(Temperature > 2000)
	{
		T2 = 0;
		OFF2 = 0;
		SENS2 = 0;
	}
	
	Temperature -= T2;
	OFF = OFF - OFF2;
	SENS = SENS - SENS2;
	Pressure = (D1_Pres * SENS / 2097152 - OFF)/32768;
	MS5611_Pressure = Pressure / (double)(100.0);
	printf("Temperature=%lf\r\n",Temperature);
	printf("Pressure = %lf\r\n",MS5611_Pressure);
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint8_t Result;
	double Altitude;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	MS561101BA_RESET();
	HAL_Delay(100);
	
	MS561101BA_READ_PROM();
	HAL_Delay(100);
	Result=MS5611_CRC(Cal_C);
	if(Result==0)
	{
		printf("气压计校准成功\r\n");
	}
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		//temp = MS561101BA_DO_CONVERSION(0x48);
		//HAL_Delay(500);
		//mol = MS561101BA_DO_CONVERSION(0x58);
		HAL_Delay(500);
		MS561101BA_GetTemperature(0x58);
		MS561101BA_GetPressure(0x48);
		
    Altitude = 4433000.0f * (1 - powf((((double) MS5611_Pressure) / Alt_offset_Pa), 0.190295f));
		printf("Altitude = %lfcm\r\n",Altitude);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/********************************************
主控芯片：STM32 Controller
模块型号：MLX90614
通讯方式：SPI串口通信
作者：苏夏雨
授权：未经作者允许，禁止转载
********************************************/
//定义模块头文件名称
#ifndef _MLX90614_H
#define _MLX90614_H
//定义模块使用头文件
#include "tim.h"
#include "usart.h" 
//定义模块使用的引脚
#define SDA_R HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)
#define SDA_H HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET)
#define SDA_L HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET)
#define SCL_H HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET)
#define SCL_L HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET)

//定义模块使用的函数
float Tempinput(void);//读取温度函数
void MLX90614_Init(void);//初始化MLX90614
uint8_t PEC_Calculation(uint8_t*);
#endif

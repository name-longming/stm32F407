//定义模块头文件名称
#ifndef _LMT70_H
#define _LMT70_H
//定义模块使用头文件
#include "stm32f4xx_hal.h"
//定义模块使用的引脚
extern uint16_t ADC_ConvertedValue[20];
extern double Temperature;

float Get_LMT70_Temperature(void);
#endif

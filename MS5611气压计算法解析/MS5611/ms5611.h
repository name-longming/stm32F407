#ifndef _MS5611_H_
#define _MS5611_H_	 
#include "sys.h"
/***********宏定义****************/
//定义器件在IIC总线中的从地址,根据CSB引脚不同修改

//#define MS561101BA_Device_Address  0xec   //CBR=1 0x76 I2C address when CSB is connected to HIGH (VCC) 
#define  MS561101BA_Device_Address   0xee   //CBR=0  0x77 I2C address when CSB is connected to LOW (GND)

// 定义MS561101BA内部寄存器地址 

#define MS561101BA_D1             0x40  
#define MS561101BA_D2             0x50  
#define MS561101BA_RESET          0x1E //软复位指令
#define CMD_ADC_READ              0x00
// D1 and D2 result size (bytes)  
#define MS561101BA_D1D2_SIZE      3
// OSR (Over Sampling Ratio) constants

#define MS561101BA_OSR_256        0x00  
#define MS561101BA_OSR_512        0x02  
#define MS561101BA_OSR_1024       0x04  
#define MS561101BA_OSR_2048       0x06  
#define MS561101BA_OSR_4096       0x08  
//#define  MS561101BA_D1_OSR_256 0x40   
//#define  MS561101BA_D1_OSR_512 0x42   
//#define  MS561101BA_D1_OSR_1024 0x44   
//#define  MS561101BA_D1_OSR_2048 0x46   
#define  MS561101BA_Press_D1_OSR_4096   0x48   //12位精度（4096）的压强采集
  
//#define  MS561101BA_D2_OSR_256 0x50   
//#define  MS561101BA_D2_OSR_512 0x52   
//#define  MS561101BA_D2_OSR_1024 0x54   
//#define  MS561101BA_D2_OSR_2048 0x56   
#define  MS561101BA_Temp_D2_OSR_4096   0x58  //12位精度（4096）的温度采集

#define MS561101BA_PROM_BASE_ADDR 0xA0 // by adding ints from 0 to 6 we can read all the prom configuration values. 
// C1 will be at 0xA2 and all the subsequent are multiples of 2 
//prom出厂参数存储的基地址（有6个）
#define MS561101BA_PROM_REG_COUNT  6 // number of registers in the PROM  
#define MS561101BA_PROM_REG_SIZE   2 // size in bytes of a prom registry.



/*函数声明----------------------------------------------------------------*/  
 void MS561101BA_Reset(void);  
 void MS561101BA_readPROM(void);  
 uint32_t MS561101BA_DO_CONVERSION(u8 command);  
 void MS561101BA_GetTemperature(u8 OSR_Temp);  
 void MS561101BA_GetPressure(u8 OSR_Pres);  
 void MS561101BA_Init(void);  
 void SampleANDExchange(void);  
void MS561101BA_data_transfer(void);
/*变量声明*--------------------------------------*/
extern uint16_t Cal_C[7];  //用于存放PROM中的6组数据      
extern uint32_t D1_Pres,D2_Temp; // 存放数字压力和温度  
extern float Pressure;             //温度补偿大气压  
extern float Temperature,Temperature2;//实际和参考温度之间的差异,实际温度,中间值  
extern double dT,OFF,SENS;  //实际温度抵消,实际温度灵敏度  
extern float Aux,OFF2,SENS2;  //温度校验值  
extern uint32_t ex_Pressure;           //串口读数转换值  
extern uint8_t  exchange_num[8]; 

void  get_ms5611_data(void);
#endif
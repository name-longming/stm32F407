//定义模块头文件名称
#ifndef _control_H
#define _control_H
//定义模块使用头文件
#include "stm32f4xx_hal.h"
#include "usart.h"

extern long int Motor_A,Motor_B,Motor_C,Motor_D;        //电机PWM变量
extern long int Target_A,Target_B,Target_C,Target_D;     //电机目标值
extern float Velocity_KP,Velocity_KI;	          //速度控制PID参数
extern int Encoder_A,Encoder_B,Encoder_C,Encoder_D;          //编码器的脉冲计数

void Kinematic_Analysis(float Vx,float Vy,float Vz);
void Xianfu_Pwm(int amplitude);
uint32_t myabs(long int a);
void Set_Pwm(int motor_a,int motor_b,int motor_c,int motor_d);
int Incremental_PI_A (int Encoder,int Target);
int Incremental_PI_B (int Encoder,int Target);
int Incremental_PI_C (int Encoder,int Target);
int Incremental_PI_D (int Encoder,int Target);
int Read_Encoder(uint8_t TIMX);
#endif

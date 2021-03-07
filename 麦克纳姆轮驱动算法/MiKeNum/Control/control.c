#include "control.h"

long int Motor_A,Motor_B,Motor_C,Motor_D;        //电机PWM变量
long int Target_A,Target_B,Target_C,Target_D;     //电机目标值
int Encoder_A,Encoder_B,Encoder_C,Encoder_D;          //编码器的脉冲计数

float Velocity_KP=-10,Velocity_KI=-10;	          //速度控制PID参数
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;

#define a_PARAMETER          (0.095f)               
#define b_PARAMETER          (0.086f)         
/**************************************************************************
函数功能：小车运动数学模型
入口参数：X Y Z 三轴速度或者位置
返回  值：无
**************************************************************************/
void Kinematic_Analysis(float Vx,float Vy,float Vz)
{
	Target_A   = -Vx+Vy-Vz*(a_PARAMETER+b_PARAMETER);
	Target_B   = +Vx+Vy+Vz*(a_PARAMETER+b_PARAMETER);
	Target_C   = -Vx+Vy+Vz*(a_PARAMETER+b_PARAMETER);
	Target_D   = +Vx+Vy-Vz*(a_PARAMETER+b_PARAMETER);
}

/**************************************************************************
函数功能：限制PWM赋值 
入口参数：幅值
返回  值：无
**************************************************************************/
void Xianfu_Pwm(int amplitude)
{	
    if(Motor_A<-amplitude) Motor_A=-amplitude;	
		if(Motor_A>amplitude)  Motor_A=amplitude;	
	  if(Motor_B<-amplitude) Motor_B=-amplitude;	
		if(Motor_B>amplitude)  Motor_B=amplitude;		
	  if(Motor_C<-amplitude) Motor_C=-amplitude;	
		if(Motor_C>amplitude)  Motor_C=amplitude;		
	  if(Motor_D<-amplitude) Motor_D=-amplitude;	
	  if(Motor_D>amplitude)  Motor_D=amplitude;		
}

/**************************************************************************
函数功能：绝对值函数
入口参数：long int
返回  值：unsigned int
**************************************************************************/
uint32_t myabs(long int a)
{ 		   
	  uint32_t temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}

/**************************************************************************
函数功能：增量PI控制器
入口参数：编码器测量值，目标速度
返回  值：电机PWM
根据增量式离散PID公式 
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)代表本次偏差 
e(k-1)代表上一次的偏差  以此类推 
pwm代表增量输出
在我们的速度控制闭环系统里面，只使用PI控制
pwm+=Kp[e（k）-e(k-1)]+Ki*e(k)
**************************************************************************/
int Incremental_PI_A (int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                //计算偏差
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
	 if(Pwm>8400)Pwm=8400;
	 if(Pwm<-8400)Pwm=-8400;
	 Last_bias=Bias;	                   //保存上一次偏差 
	 return Pwm;                         //增量输出
}
int Incremental_PI_B (int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                //计算偏差
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
	 if(Pwm>8400)Pwm=8400;
	 if(Pwm<-8400)Pwm=-8400;
	 Last_bias=Bias;	                   //保存上一次偏差 
	 return Pwm;                         //增量输出
}
int Incremental_PI_C (int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                                  //计算偏差
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
	 if(Pwm>8400)Pwm=8400;
	 if(Pwm<-8400)Pwm=-8400;
	 Last_bias=Bias;	                   //保存上一次偏差 
	 return Pwm;                         //增量输出
}
int Incremental_PI_D (int Encoder,int Target)
{ 	
	 static int Bias,Pwm,Last_bias;
	 Bias=Encoder-Target;                                  //计算偏差
	 Pwm+=Velocity_KP*(Bias-Last_bias)+Velocity_KI*Bias;   //增量式PI控制器
	 if(Pwm>8400)Pwm=8400;
	 if(Pwm<-8400)Pwm=-8400;
	 Last_bias=Bias;	                   //保存上一次偏差 
	 return Pwm;                         //增量输出
}

/**************************************************************************
函数功能：赋值给PWM寄存器
入口参数：PWM
返回  值：无
**************************************************************************/
void Set_Pwm(int motor_a,int motor_b,int motor_c,int motor_d)
{
		int siqu=0;
		if(motor_a>0)			
		{
			htim1.Instance->CCR1=motor_a+siqu;
			GPIOD->ODR &= ~(1<<0);
			GPIOD->ODR |= (1<<1);
		}
		else  	          
		{
			htim1.Instance->CCR1=myabs(motor_a)+siqu;
			GPIOD->ODR |= (1<<0);
			GPIOD->ODR &= ~(1<<1);
		}
	
		if(motor_b>0)			
		{
			htim1.Instance->CCR2=motor_b+siqu;
			GPIOD->ODR |= (1<<2);
			GPIOD->ODR &= ~(1<<3);
		}
		else 	            
		{
			htim1.Instance->CCR2=myabs(motor_b)+siqu;
			GPIOD->ODR &= ~(1<<2);
			GPIOD->ODR |= (1<<3);
		}

		if(motor_c>0)			
		{
			htim1.Instance->CCR3=motor_c+siqu;
			GPIOD->ODR |= (1<<4);
			GPIOD->ODR &= ~(1<<5);
		}
		else 	            
		{
			GPIOD->ODR &= ~(1<<4);
			GPIOD->ODR |= (1<<5);
			htim1.Instance->CCR3=myabs(motor_c)+siqu;
		}

	 if(motor_d>0)			
	 {
		 htim1.Instance->CCR4=motor_d+siqu;
		 GPIOD->ODR &= ~(1<<6);
		 GPIOD->ODR |= (1<<7);
	 }
	 else 	            
	 {
		 htim1.Instance->CCR4=myabs(motor_d)+siqu;
		 GPIOD->ODR |= (1<<6);
		 GPIOD->ODR &= ~(1<<7);

	 }
}

int Read_Encoder(uint8_t TIMX)
{
	 int Encoder_TIM;    
	 switch(TIMX)
	 {
		 case 2:  Encoder_TIM=( int16_t )__HAL_TIM_GET_COUNTER(&htim2);__HAL_TIM_SET_COUNTER(&htim2,0); break;
		 case 3:  Encoder_TIM=( int16_t )__HAL_TIM_GET_COUNTER(&htim3);__HAL_TIM_SET_COUNTER(&htim3,0); break;
		 case 4:  Encoder_TIM=( int16_t )__HAL_TIM_GET_COUNTER(&htim4);__HAL_TIM_SET_COUNTER(&htim4,0);  break;	
		 case 5:  Encoder_TIM=( int16_t )__HAL_TIM_GET_COUNTER(&htim5);__HAL_TIM_SET_COUNTER(&htim5,0);  break;	
		 default:  Encoder_TIM=0;
	 }
		return Encoder_TIM;
}

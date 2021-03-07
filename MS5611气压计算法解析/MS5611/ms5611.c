#include "main.h"


/*变量声明----------------------------------------------------------------*/  
uint16_t Cal_C[7];  //用于存放PROM中的6组数据      
uint32_t D1_Pres,D2_Temp; // 存放数字压力和温度  
float Pressure;             //温度补偿大气压  
double dT;
float Temperature,Temperature2;//实际和参考温度之间的差异,实际温度,中间值  
double OFF,SENS;  //实际温度抵消,实际温度灵敏度  
float Aux,OFF2,SENS2;  //温度校验值  
  
uint32_t ex_Pressure;           //串口读数转换值  
uint8_t  exchange_num[8]; 

/***初始化函数**/
void MS561101BA_Init(void)
{
        my_IIC_Init();
	MS561101BA_Reset();
	delay_ms(100);
	MS561101BA_readPROM();
	delay_ms(100);
} 

/***软复位函数****/
void MS561101BA_Reset(void)  
{  
    IIC_Start();
    IIC_Send_Byte(MS561101BA_Device_Address);
    while(IIC_Wait_Ack());
    delay_us(100);
    IIC_Send_Byte(MS561101BA_RESET);
    while(IIC_Wait_Ack());
    delay_us(100);
    IIC_Stop();
}


//从PROM读取出厂校准数据
void MS561101BA_readPROM(void)
{
   u16 data1,data2;
   u8  i=0;
   for(i=0;i<=6;i++)
   {
     /***********************************/
      IIC_Start();
      IIC_Send_Byte(MS561101BA_Device_Address);//写入从设备地址，并设定为写入模式
      while(IIC_Wait_Ack());
      delay_us(100);
      IIC_Send_Byte(MS561101BA_PROM_BASE_ADDR+i*2);//写入寄存器地址，准备读取数据
      while(IIC_Wait_Ack());
      delay_us(100);
      IIC_Stop();
      
      delay_us(100);
      IIC_Start();
      IIC_Send_Byte(MS561101BA_Device_Address+0x01);//写入从设备地址，并设定为读取模式
      while(IIC_Wait_Ack());
      delay_us(100);
      data1=IIC_Read_Byte(1);
      data2=IIC_Read_Byte(0);
      IIC_Stop();
      delay_ms(10);
      Cal_C[i]=(data1<<8)|data2;
   }
}


/******读取信息函数******/
uint32_t MS561101BA_DO_CONVERSION(u8 command)
{
    	u32 conversion=0;
	u16 conv1,conv2,conv3; 
        
        IIC_Start();
	IIC_Send_Byte(MS561101BA_Device_Address);//写入从设备地址，并设定为写入模式
	while(IIC_Wait_Ack());
	IIC_Send_Byte(command);	//写入寄存器地址（指令），准备读取数据
	while(IIC_Wait_Ack());
	IIC_Stop();
        
        delay_ms(20);
        
        IIC_Start();    
        IIC_Send_Byte(MS561101BA_Device_Address);
        while(IIC_Wait_Ack());
	IIC_Send_Byte(0x00);
	while(IIC_Wait_Ack());
	IIC_Stop();//I2C ADC read sequence
        
        delay_ms(10);
        IIC_Start();
	IIC_Send_Byte(MS561101BA_Device_Address+1);
	while(IIC_Wait_Ack());
        conv1=IIC_Read_Byte(1);
        conv2=IIC_Read_Byte(1);
        conv3=IIC_Read_Byte(0);
        IIC_Stop();
        
        delay_ms(10);
        conversion= (conv1<<16) + (conv2<<8) + conv3;
        
        return conversion;
        
}

//读取数字温度
void MS561101BA_GetTemperature(u8 OSR_Temp)
{
    
	D2_Temp= MS561101BA_DO_CONVERSION(OSR_Temp);	
	delay_ms(10);
	
	dT=D2_Temp - ((((unsigned long)Cal_C[5])<<8)*1.0);
	Temperature=2000+dT*((u32)Cal_C[6])/8388608.0;//2007等于20.07摄氏度

}

//读取大气压强
void MS561101BA_GetPressure(u8 OSR_Pres)
{ 
      float Aux,OFF2,SENS2;  //温度校验值
      D1_Pres=MS561101BA_DO_CONVERSION(OSR_Pres);
      delay_ms(10);
      OFF=(u32)(Cal_C[2]<<16)+((u32)Cal_C[4]*dT)/128.0;
      SENS=(u32)(Cal_C[1]<<15)+((u32)Cal_C[3]*dT)/256.0;
      //温度补偿
      	if(Temperature < 2000)// second order temperature compensation when under 20 degrees C
	{
		Temperature2 = (dT*dT) / 0x80000000;//(0x80000000是2的31次方)
		Aux = (Temperature-2000)*(Temperature-2000);
		OFF2 = 2.5*Aux;
		SENS2 = 1.25*Aux;
		if(Temperature < -1500)
		{
			Aux = (Temperature+1500)*(Temperature+1500);
			OFF2 = OFF2 + 7*Aux;
			SENS2 = SENS + 5.5*Aux;
		}
	}else  //(Temperature > 2000)
	{
		Temperature2 = 0;
		OFF2 = 0;
		SENS2 = 0;
	}
	
	Temperature = Temperature - Temperature2;
	OFF = OFF - OFF2;
	SENS = SENS - SENS2;	

	Pressure=(D1_Pres*SENS/2097152.0-OFF)/32768.0;//100009= 1000.09 mbar  
        //一个标准大气压的压强是1.013x10^3mbar
}


void MS561101BA_data_transfer(void)
{
  long temp_transfer=0;
  long press_transfer=0;
  unsigned char temp_exchange_number[]={'0'};
  unsigned char press_exchange_number[]={'0'};
  u8 temp_datalen=0,press_datalen=0,i=0;
  
  
  
  if(Temperature<0)
  {
    temp_transfer=(long)(-Temperature);//（注意此单位是100*摄氏度，需将温度转换成摄氏度为单位）
    temp_exchange_number[0]='-';
  }
  else
  {
    temp_transfer=(long)Temperature;//（注意此单位是100*摄氏度，需将温度转换成摄氏度为单位）
    temp_exchange_number[0]='\0';
  }
  
  if(temp_transfer>=0&&temp_transfer<=9999)
  {
    temp_datalen=6;
    temp_exchange_number[1]=((u16)(temp_transfer/1000.0f)+0x30);//十位
    temp_transfer=temp_transfer%1000;
    
    temp_exchange_number[2]=((u16)(temp_transfer/100.0f)+0x30);//个位
    temp_transfer=temp_transfer%100;
    
    temp_exchange_number[3]='.';
    
    temp_exchange_number[4]=((u16)(temp_transfer/10.0f)+0x30);
    temp_transfer=temp_transfer%10;
    
    temp_exchange_number[5]=((u16)(temp_transfer)+0x30);
    
    printf("当前温度是：");
    for(i=0;i<6;i++)
    {
      printf("%c",temp_exchange_number[i]);
    }
    printf("°C   \r \n");
  }
  if(temp_transfer>=10000)
  {//温度大于100度
    temp_datalen=7;
    temp_exchange_number[1]=((u16)(temp_transfer/10000.0f)+0x30);//百位
    temp_transfer=temp_transfer%10000;
    
    temp_exchange_number[2]=((u16)(temp_transfer/1000.0f)+0x30);//十位
    temp_transfer=temp_transfer%1000;
    
    temp_exchange_number[3]=((u16)(temp_transfer/100.0f)+0x30);//个位
    temp_transfer=temp_transfer%100;
    
    temp_exchange_number[4]='.';
    
    temp_exchange_number[5]=((u16)(temp_transfer/10.0f)+0x30);
    temp_transfer=temp_transfer%10;
    
    temp_exchange_number[6]=((u16)(temp_transfer)+0x30);
    
    printf("当前温度是：");
    for(i=0;i<temp_datalen;i++)
    {
      printf("%c",temp_exchange_number[i]);
    }
    printf("°C   \r \n");
  }
  
  /*********发送气压大小***********/
  if(Pressure<0)
  {
    press_transfer=(long)(-Pressure);//（注意此单位是100*摄氏度，需将温度转换成摄氏度为单位）
    press_exchange_number[0]='-';
  }
  else
  {
    press_transfer=(long)Pressure;//（注意此单位是100*摄氏度，需将温度转换成摄氏度为单位）
    press_exchange_number[0]='\0';
  }
  
  if(press_transfer>=0&&press_transfer<=99999)
  {
    press_datalen=7;
    press_exchange_number[1]=((u16)(press_transfer/10000.0f)+0x30);//百位
    press_transfer=press_transfer%10000;
    
    press_exchange_number[2]=((u16)(press_transfer/1000.0f)+0x30);//十位
    press_transfer=temp_transfer%1000;
    
    press_exchange_number[3]=((u16)(press_transfer/100.0f)+0x30);//个位
    press_transfer=temp_transfer%100;
    
    press_exchange_number[4]='.';
    
    press_exchange_number[5]=((u16)(press_transfer/10.0f)+0x30);
    press_transfer=temp_transfer%10;
    
    press_exchange_number[6]=((u16)(press_transfer)+0x30);
    
    printf("当前气压是：");
    for(i=0;i<press_datalen;i++)
    {
      printf("%c",press_exchange_number[i]);
    }
    printf("mbar   \r \n");
  }
  
  if(press_transfer>=100000)
  {
    press_datalen=8;
    press_exchange_number[1]=((u16)(press_transfer/100000.0f)+0x30);//千位
    press_transfer=press_transfer%100000;
    
    press_exchange_number[2]=((u16)(press_transfer/10000.0f)+0x30);//百位
    press_transfer=press_transfer%10000;
    
    press_exchange_number[3]=((u16)(press_transfer/1000.0f)+0x30);//十位
    press_transfer=press_transfer%1000;
    
    press_exchange_number[4]=((u16)(press_transfer/100.0f)+0x30);//个位
    press_transfer=press_transfer%100;
    
    press_exchange_number[5]='.';
    
    press_exchange_number[6]=((u16)(press_transfer/10.0f)+0x30);
    press_transfer=press_transfer%10;
    
    press_exchange_number[7]=((u16)(press_transfer)+0x30);
    
    printf("当前气压是：");
    for(i=0;i<press_datalen;i++)
    {
      printf("%c",press_exchange_number[i]);
    }
    printf("mbar   \r \n");
  }
  
  
}


/*****   brief     获取ms5611的数据并写出onenet平台要求的Http协议格式   *****/
void  get_ms5611_data(void)      
{
      char data[25]={0};
      MS561101BA_GetTemperature(MS561101BA_Temp_D2_OSR_4096);
      MS561101BA_GetPressure(MS561101BA_Press_D1_OSR_4096);
      memset(send_Buf,0,MAX_SEND_BUF_LEN);
      strcat(send_Buf, ",;");	
      strcat(send_Buf, "Temperature,");
      sprintf(data,"%f",Temperature);
      strcat(send_Buf, data);
      strcat(send_Buf, ";");
      
      strcat(send_Buf, "Pressure,");
      memset(data,0,25);
      sprintf(data,"%f",Pressure);
      strcat(send_Buf, data);
      strcat(send_Buf, ";");
  
}





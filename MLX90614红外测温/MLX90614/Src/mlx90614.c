/********************************************
主控芯片：STM32 Controller
模块型号：MLX90614
通讯方式：SPI串口通信
作者：苏夏雨
授权：未经作者允许，禁止转载
********************************************/
#include "mlx90614.h"
//mlx90614 端口定义 
GPIO_InitTypeDef GPIO_Initure;

/*******************************************************************************
* 函数名: PEC_calculation
* 功能 : 数据校验
* Input          : pec[]
* Output         : None
* Return         : pec[0]-this byte contains calculated crc value
*******************************************************************************/
uint8_t PEC_Calculation(uint8_t pec[])
{
    uint8_t 	crc[6];//存放多项式
    uint8_t	BitPosition=47;//存放所有数据最高位，6*8=48 最高位就是47位
    uint8_t	shift;
    uint8_t	i;
    uint8_t	j;
    uint8_t	temp;

    do
    {
        /*Load pattern value 0x00 00 00 00 01 07*/
        crc[5]=0;
        crc[4]=0;
        crc[3]=0;
        crc[2]=0;
        crc[1]=0x01;
        crc[0]=0x07;

        /*Set maximum bit position at 47 ( six bytes byte5...byte0,MSbit=47)*/
        BitPosition=47;

        /*Set shift position at 0*/
        shift=0;

        /*Find first "1" in the transmited message beginning from the MSByte byte5*/
        i=5;
        j=0;
        while((pec[i]&(0x80>>j))==0 && i>0)
        {
            BitPosition--;
            if(j<7)
            {
                j++;
            }
            else
            {
                j=0x00;
                i--;
            }
        }/*End of while */

        /*Get shift value for pattern value*/
        shift=BitPosition-8;

        /*Shift pattern value */
        while(shift)
        {
            for(i=5; i<0xFF; i--)
            {
                if((crc[i-1]&0x80) && (i>0))
                {
                    temp=1;
                }
                else
                {
                    temp=0;
                }
                crc[i]<<=1;
                crc[i]+=temp;
            }/*End of for*/
            shift--;
        }/*End of while*/

        /*Exclusive OR between pec and crc*/
        for(i=0; i<=5; i++)
        {
            pec[i] ^=crc[i];
        }/*End of for*/
    }
    while(BitPosition>8); /*End of do-while*/

    return pec[0];
}

void MLX90614_Init()//初始化MLX90614
{
    __HAL_RCC_GPIOB_CLK_ENABLE();					
    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7;			
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_OD;  				//推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;         					//上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  	//高速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);     				//初始化GPIO
		SCL_H;
		SDA_H;
}
void SMBstart()//SMB发送开始标志
{ 
   SDA_H; 
   delay_us(10); 
   SCL_H; 
   delay_us(10); 
   SDA_L; 
   delay_us(10); 
   SCL_L; 
   delay_us(10); 
} 
void SMBstop()//SMB发送停止标志
{ 

   SCL_L; 
   delay_us(10); 
   SDA_L; 
   delay_us(10); 
   SCL_H; 
   delay_us(10); 
   SDA_H; 
   delay_us(10); 
} 
void SMBsend(uint8_t buf)//SMB发送一个字节
{ 
		uint8_t i;
    for(i=0;i<8;i++)
    {     
        SCL_L;  		
			
        if((buf&0x80)==0)  
        {SDA_L;}  
        else  
        {SDA_H;};  
				
        buf<<=1;  
        delay_us(10);     
        SCL_H;
        delay_us(10);    
		}  
		SCL_L; 
		delay_us(10); 
		SDA_H; 				   
		delay_us(10);  
		SCL_H; 
		delay_us(10);  
		SCL_L;
} 
uint8_t   SMBread()//SMB接收一个字节 
{ 
		uint8_t i;
		uint8_t buf;
    for(i=0;i<8;i++)
    {     
        SCL_L;       
        delay_us(10);  			
        SDA_H; 
        delay_us(10);  
        SCL_H;  
        delay_us(10);  			
        buf<<=1;  
        if(SDA_R){buf|=0x01;};  
        delay_us(10);  
    }
		delay_us(10);
		SCL_L;	
		delay_us(10);
		SDA_L;
		delay_us(10);
		SCL_H;
		delay_us(10);
		SCL_L;	
		return buf;
} 
float Tempinput()//读取温度函数
{
//	uint8_t string[]={"000.00摄氏度\r\n"};
	uint8_t arr[6];			// Buffer for the sent bytes
	uint8_t SMBdataL;
	uint8_t SMBdataH;
	uint8_t Pec;				// PEC byte storage
	uint8_t PecReg;			// Calculated PEC byte storage
	uint32_t SMBdata;
	float temp;
	
	//摄氏温度计算公式（T*0.02）-273.15
  SMBstart();//开始起始标志（写）
  SMBsend(0x00);//从机地址
  SMBsend(0x07);//发送命令
	
  SMBstart();//重复起始标志（读）
  SMBsend(0x01);  
	SMBdataL=SMBread();//接收高位
	SMBdataH=SMBread();//接收低位
	Pec=SMBread();
	SMBsend(1);
	//SMBread();
  SMBstop(); 
	delay_us(10);
	arr[5] = 0x00;		//
	arr[4] = 0x07;			//
	arr[3] = 0x00+1;	//Load array arr
	arr[2] = SMBdataL;				//
	arr[1] = SMBdataH;				//
	arr[0] = 0;					//
	PecReg=PEC_Calculation(arr);//Calculate CRC 数据校验
	while(PecReg != Pec);//If received and calculated CRC are equal go out from do-while{}
	SMBdata=(SMBdataH*256)+SMBdataL;
	SMBdata=SMBdata*2;
	SMBdata=SMBdata-27315;
//	string[0]=(0x30+(SMBdata/10000));
//	string[1]=(0x30+((SMBdata%10000)/1000));
//	string[2]=(0x30+((SMBdata%1000)/100));
//	string[4]=(0x30+((SMBdata%100)/10));
//	string[5]=(0x30+((SMBdata%10)/1));
	
	temp=SMBdata*0.01;
//	printf("temp=%.2f\r\n", temp);
//	HAL_UART_Transmit(&huart1,string,sizeof(string),1000);
	return temp;
} 

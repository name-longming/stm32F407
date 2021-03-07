#ifndef __OLED_H
#define __OLED_H	 

#include "i2c.h"
 

#define OLED0561_ADD	0x78  // OLED的I2C地址（禁止修改）
#define COM				0x00  // OLED 指令（禁止修改）
#define DAT 			0x40  // OLED 数据（禁止修改）

void WriteCmd(unsigned char I2C_Command);//写命令
void WriteDat(unsigned char I2C_Data);//写数据
void OLED_Init(void);//初始化
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char fill_Data);//全屏填充
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);//显示字符串
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N);//显示汉字
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);//显示图片

void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size);
uint32_t oled_pow(uint8_t m,uint8_t n);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size2);//size2(16|12)




#endif

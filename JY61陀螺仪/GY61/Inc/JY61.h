#ifndef __JY61_H
#define __JY61_H 

#include "stm32f4xx_hal.h"
#include "string.h"
#include "usart.h"

struct SAcc
{
	short a[3];
	short T;
};
struct SGyro
{
	short w[3];
	short T;
};
struct SAngle
{
	short Angle[3];
	short T;
};

extern struct SAcc 		stcAcc;
extern struct SGyro 		stcGyro;
extern struct SAngle 	stcAngle;

extern char YAWCMD[3];
extern char ACCCMD[3];
extern char SLEEPCMD[3];
extern char UARTMODECMD[3];
extern char IICMODECMD[3];

void CopeSerial2Data(unsigned char ucData);
void Send_CMD_Calibration(void);
#endif




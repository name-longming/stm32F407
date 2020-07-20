#ifndef _ONENET_H_
#define _ONENET_H_





_Bool OneNet_DevLink(void);

void OneNet_SendData(char *DataStream,int data);

void OneNet_RevPro(unsigned char *cmd);

_Bool OneNet_PushData(const char* dst_devid, const char* data, unsigned int data_len);
	
#endif

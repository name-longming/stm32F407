#ifndef _EDPKIT_H_
#define _EDPKIT_H_


#include "Common.h"


//=============================配置==============================
//===========可以提供RTOS的内存管理方案，也可以使用C库的=========
#include <stdlib.h>

#define EDP_MallocBuffer	malloc

#define EDP_FreeBuffer		free
//==========================================================


#define MOSQ_MSB(A)         (uint8)((A & 0xFF00) >> 8)
#define MOSQ_LSB(A)         (uint8)(A & 0x00FF)


/*--------------------------------消息编号--------------------------------*/
#define MSG_ID_HIGH			0x55
#define MSG_ID_LOW			0xAA


/*--------------------------------消息类型--------------------------------*/
/* 连接请求 */
#define CONNREQ             0x10
/* 连接响应 */
#define CONNRESP            0x20
/* 转发(透传)数据 */
#define PUSHDATA            0x30
/* 存储(转发)数据 */
#define SAVEDATA            0x80
/* 存储确认 */
#define SAVEACK             0x90
/* 命令请求 */
#define CMDREQ              0xA0
/* 命令响应 */
#define CMDRESP             0xB0
/* 心跳请求 */
#define PINGREQ             0xC0
/* 心跳响应 */
#define PINGRESP            0xD0
/* 加密请求 */
#define ENCRYPTREQ          0xE0
/* 加密响应 */
#define ENCRYPTRESP         0xF0


#ifndef NULL
#define NULL (void*)0
#endif


/*--------------------------------SAVEDATA消息支持的格式类型--------------------------------*/
typedef enum
{
	
    kTypeFullJson = 0x01,
	
    kTypeBin = 0x02,
	
    kTypeSimpleJsonWithoutTime = 0x03,
	
    kTypeSimpleJsonWithTime = 0x04,
	
    kTypeString = 0x05
	
} SaveDataType;


/*--------------------------------内存分配方案标志--------------------------------*/
#define MEM_FLAG_NULL		0
#define MEM_FLAG_ALLOC		1
#define MEM_FLAG_STATIC		2


typedef struct Buffer
{
	
	uint8	*_data;		//协议数据
	
	uint32	_len;		//写入的数据长度
	
	uint32	_size;		//缓存总大小
	
	uint8	_memFlag;	//内存使用的方案：0-未分配	1-使用的动态分配		2-使用的固定内存
	
} EDP_PACKET_STRUCTURE;


/*--------------------------------删包--------------------------------*/
void EDP_DeleteBuffer(EDP_PACKET_STRUCTURE *edpPacket);

/*--------------------------------解包--------------------------------*/
uint8 EDP_UnPacketRecv(uint8 *dataPtr);

/*--------------------------------登录方式1组包--------------------------------*/
uint1 EDP_PacketConnect1(const int8 *devid, const int8 *apikey, uint16 cTime, EDP_PACKET_STRUCTURE *edpPacket);

/*--------------------------------登录方式2组包--------------------------------*/
uint1 EDP_PacketConnect2(const int8 *devid, const int8 *auth_key, uint16 cTime, EDP_PACKET_STRUCTURE *edpPacket);

/*--------------------------------连接回复解包--------------------------------*/
uint8 EDP_UnPacketConnectRsp(uint8 *rev_data);

/*--------------------------------数据点上传组包--------------------------------*/
uint8 EDP_PacketSaveData(const int8 *devid, int16 send_len, int8 *type_bin_head, SaveDataType type, EDP_PACKET_STRUCTURE *edpPacket);

/*--------------------------------PushData组包--------------------------------*/
uint8 EDP_PacketPushData(const int8 *devid, const int8 *msg, uint32 msg_len, EDP_PACKET_STRUCTURE *edpPacket);

/*--------------------------------PushData解包--------------------------------*/
uint8 EDP_UnPacketPushData(uint8 *rev_data, int8 **src_devid, int8 **req, uint32 *req_len);

/*--------------------------------命令下发解包--------------------------------*/
uint8 EDP_UnPacketCmd(uint8 *rev_data, int8 **cmdid, uint16 *cmdid_len, int8 **req, uint32 *req_len);

/*--------------------------------命令回复组包--------------------------------*/
uint1 EDP_PacketCmdResp(const int8 *cmdid, uint16 cmdid_len, const int8 *resp, uint32 resp_len, EDP_PACKET_STRUCTURE *edpPacket);

/*--------------------------------心跳请求组包--------------------------------*/
uint1 EDP_PacketPing(EDP_PACKET_STRUCTURE *edpPacket);


#endif

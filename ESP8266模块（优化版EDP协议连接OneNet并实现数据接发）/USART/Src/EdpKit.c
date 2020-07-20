/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	EdpKit.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-09-13
	*
	*	版本： 		V1.1
	*
	*	说明： 		EDP协议
	*
	*	修改记录：	V1.1：将strncpy替换为memcpy，解决潜在bug
	************************************************************
	************************************************************
	************************************************************
**/

//协议头文件
#include "EdpKit.h"
#include "main.h"

//C库
#include <string.h>

extern void USART2_Write(USART_TypeDef* USARTx, uint8_t *Data, uint8_t len);
//==========================================================
//	函数名称：	EDP_NewBuffer
//
//	函数功能：	申请内存
//
//	入口参数：	edpPacket：包结构体
//				size：大小
//
//	返回参数：	无
//
//	说明：		1.可使用动态分配来分配内存
//				2.可使用局部或全局数组来指定内存
//==========================================================
void EDP_NewBuffer(EDP_PACKET_STRUCTURE *edpPacket, uint32 size)
{
	
	uint32 i = 0;

	if(edpPacket->_data == NULL)
	{
		edpPacket->_memFlag = MEM_FLAG_ALLOC;
		
		edpPacket->_data = (uint8 *)EDP_MallocBuffer(size);
		if(edpPacket->_data != NULL)
		{
			edpPacket->_len = 0;
			
			edpPacket->_size = size;

			for(; i < edpPacket->_size; i++)
				edpPacket->_data[i] = 0;
		}
	}
	else
	{
		edpPacket->_memFlag = MEM_FLAG_STATIC;
		
		for(; i < edpPacket->_size; i++)
			edpPacket->_data[i] = 0;
		
		edpPacket->_len = 0;
		
		if(edpPacket->_size < size)
			edpPacket->_data = NULL;
	}

}

//==========================================================
//	函数名称：	EDP_DeleteBuffer
//
//	函数功能：	释放数据内存
//
//	入口参数：	edpPacket：包结构体
//
//	返回参数：	无
//
//	说明：		当使用的局部或全局数组时不释放内存
//==========================================================
void EDP_DeleteBuffer(EDP_PACKET_STRUCTURE *edpPacket)
{

	if(edpPacket->_memFlag == MEM_FLAG_ALLOC)
		EDP_FreeBuffer(edpPacket->_data);
	
	edpPacket->_data = NULL;
	edpPacket->_len = 0;
	edpPacket->_size = 0;
	edpPacket->_memFlag = MEM_FLAG_NULL;

}

//==========================================================
//	函数名称：	EDP_UnPacketRecv
//
//	函数功能：	EDP数据接收类型判断
//
//	入口参数：	dataPtr：接收的数据指针
//
//	返回参数：	0-成功		其他-失败原因
//
//	说明：		
//==========================================================
uint8 EDP_UnPacketRecv(uint8 *dataPtr)
{
	
	return dataPtr[0];

}

//==========================================================
//	函数名称：	EDP_PacketConnect1
//
//	函数功能：	登录方式1组包
//
//	入口参数：	devid：设备ID
//				apikey：APIKEY
//				cTime：连接保持时间
//				edpPacket：包指针
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
//==========================================================
uint1 EDP_PacketConnect1(const int8 *devid, const int8 *apikey, uint16 cTime, EDP_PACKET_STRUCTURE *edpPacket)
{
	
	uint8 devid_len = strlen(devid);
	uint8 apikey_len = strlen(apikey);

	//分配内存---------------------------------------------------------------------
	EDP_NewBuffer(edpPacket, 56);
	if(edpPacket->_data == NULL)
		return 1;
	
	//Byte0：连接类型--------------------------------------------------------------
	edpPacket->_data[0] = CONNREQ;
	edpPacket->_len++;
	
	//Byte1：剩余消息长度----------------------------------------------------------
	edpPacket->_data[1] = 13 + devid_len + apikey_len;
	edpPacket->_len++;
	
	//Byte2~3：协议名长度----------------------------------------------------------
	edpPacket->_data[2] = 0;
	edpPacket->_data[3] = 3;
	edpPacket->_len += 2;
	
	//Byte4~6：协议名--------------------------------------------------------------
	strncat((int8 *)edpPacket->_data + 4, "EDP", 3);
	edpPacket->_len += 3;
	
	//Byte7：协议版本--------------------------------------------------------------
	edpPacket->_data[7] = 1;
	edpPacket->_len++;
	
	//Byte8：连接标志--------------------------------------------------------------
	edpPacket->_data[8] = 0x40;
	edpPacket->_len++;
	
	//Byte9~10：连接保持时间-------------------------------------------------------
	edpPacket->_data[9] = MOSQ_MSB(cTime);
	edpPacket->_data[10] = MOSQ_LSB(cTime);
	edpPacket->_len += 2;
	
	//Byte11~12：DEVID长度---------------------------------------------------------
	edpPacket->_data[11] = MOSQ_MSB(devid_len);
	edpPacket->_data[12] = MOSQ_LSB(devid_len);
	edpPacket->_len += 2;
	
	//Byte13~13+devid_len：DEVID---------------------------------------------------
	strncat((int8 *)edpPacket->_data + 13, devid, devid_len);
	edpPacket->_len += devid_len;
	
	//Byte13+devid_len~13+devid_len+2：APIKEY长度----------------------------------
	edpPacket->_data[13 + devid_len] = MOSQ_MSB(apikey_len);
	edpPacket->_data[14 + devid_len] = MOSQ_LSB(apikey_len);
	edpPacket->_len += 2;
	
	//Byte15+devid_len~15+devid_len+apikey_len：APIKEY-----------------------------
	strncat((int8 *)edpPacket->_data + 15 + devid_len, apikey, apikey_len);
	edpPacket->_len += apikey_len;
	
	return 0;

}

//==========================================================
//	函数名称：	EDP_PacketConnect2
//
//	函数功能：	登录方式2组包
//
//	入口参数：	devid：设备ID
//				auth_key：鉴权信息
//				cTime：连接保持时间
//				edpPacket：包指针
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
//==========================================================
uint1 EDP_PacketConnect2(const int8 *proid, const int8 *auth_key, uint16 cTime, EDP_PACKET_STRUCTURE *edpPacket)
{
	
	uint8 proid_len = strlen(proid);
	uint8 authkey_len = strlen(auth_key);

	//分配内存---------------------------------------------------------------------
	EDP_NewBuffer(edpPacket, 56);
	if(edpPacket->_data == NULL)
		return 1;
	
	//Byte0：连接类型--------------------------------------------------------------
	edpPacket->_data[0] = CONNREQ;
	edpPacket->_len++;
	
	//Byte1：剩余消息长度----------------------------------------------------------
	edpPacket->_data[1] = 15 + proid_len + authkey_len;
	edpPacket->_len++;
	
	//Byte2~3：协议名长度----------------------------------------------------------
	edpPacket->_data[2] = 0;
	edpPacket->_data[3] = 3;
	edpPacket->_len += 2;
	
	//Byte4~6：协议名--------------------------------------------------------------
	strncat((int8 *)edpPacket->_data + 4, "EDP", 3);
	edpPacket->_len += 3;
	
	//Byte7：协议版本--------------------------------------------------------------
	edpPacket->_data[7] = 1;
	edpPacket->_len++;
	
	//Byte8：连接标志--------------------------------------------------------------
	edpPacket->_data[8] = 0xC0;
	edpPacket->_len++;
	
	//Byte9~10：连接保持时间-------------------------------------------------------
	edpPacket->_data[9] = MOSQ_MSB(cTime);
	edpPacket->_data[10] = MOSQ_LSB(cTime);
	edpPacket->_len += 2;
	
	//Byte11~12：DEVID长度---------------------------------------------------------
	edpPacket->_data[11] = 0;
	edpPacket->_data[12] = 0;
	edpPacket->_len += 2;
	
	//Byte13~14：PROID长度---------------------------------------------------------
	edpPacket->_data[13] = MOSQ_MSB(proid_len);
	edpPacket->_data[14] = MOSQ_LSB(proid_len);
	edpPacket->_len += 2;
	
	//Byte15~15+proid_len：RPOID---------------------------------------------------
	strncat((int8 *)edpPacket->_data + 15, proid, proid_len);
	edpPacket->_len += proid_len;
	
	//Byte15+devid_len~15+proid_len+1：APIKEY长度----------------------------------
	edpPacket->_data[15 + proid_len] = MOSQ_MSB(authkey_len);
	edpPacket->_data[16 + proid_len] = MOSQ_LSB(authkey_len);
	edpPacket->_len += 2;
	
	//Byte17+proid_len~17+proid_len+apikey_len：APIKEY-----------------------------
	strncat((int8 *)edpPacket->_data + 17 + proid_len, auth_key, authkey_len);
	edpPacket->_len += authkey_len;
	
	return 0;

}

//==========================================================
//	函数名称：	EDP_UnPacketConnectRsp
//
//	函数功能：	连接回复解包
//
//	入口参数：	rev_data：接收到的数据
//
//	返回参数：	登录结果
//
//	说明：		
//==========================================================
uint8 EDP_UnPacketConnectRsp(uint8 *rev_data)
{

	//0		连接成功
	//1		验证失败：协议错误
	//2		验证失败：设备ID鉴权失败
	//3		验证失败：服务器失败
	//4		验证失败：用户ID鉴权失败
	//5		验证失败：未授权
	//6		验证失败：授权码无效
	//7		验证失败：激活码未分配
	//8		验证失败：该设备已被激活
	//9		验证失败：重复发送连接请求包
	return rev_data[3];

}

int32 WriteRemainlen(uint8 *buf, uint32 len_val, uint16 write_pos)
{
	
	int32 remaining_count = 0;
	uint8 byte = 0;

	do
	{
		byte = len_val % 128;
		len_val = len_val >> 7;
		/* If there are more digits to encode, set the top bit of this digit */
		if (len_val > 0)
		{
			byte = byte | 0x80;
		}
		buf[write_pos++] = byte;
		remaining_count++;
	} while(len_val > 0 && remaining_count < 5);

	return --write_pos;
}

int32 ReadRemainlen(int8 *buf, uint32 *len_val, uint16 read_pos)
{
	
    uint32 multiplier = 1;
    uint32 len_len = 0;
    uint8 onebyte = 0;
	
    *len_val = 0;
	
    do
	{
        onebyte = buf[read_pos++];

        *len_val += (onebyte & 0x7f) * multiplier;
        multiplier <<= 7;

        len_len++;
        if (len_len > 4)
		{
            return -1;/*len of len more than 4;*/
        }
    } while((onebyte & 0x80) != 0);
	
    return read_pos;
}

//==========================================================
//	函数名称：	EDP_PacketSaveJson
//
//	函数功能：	封装协议头
//
//	入口参数：	devid：设备ID(可为空)
//				send_buf：json缓存buf
//				send_len：json总长
//				type_bin_head：bin文件的消息头
//				type：类型
//				edpPacket：包指针
//
//	返回参数：	0-成功		1-失败
//
//	说明：		当不为Type2的时候，type_bin_head可为NULL
//==========================================================
uint8 EDP_PacketSaveData(const int8 *devid, int16 send_len, int8 *type_bin_head, SaveDataType type, EDP_PACKET_STRUCTURE *edpPacket)
{

	int16 remain_len = 0;
	uint8 devid_len = strlen(devid);
	
	if(type == 2 && type_bin_head == NULL)
		return 1;
	
	if(type == 2)
		EDP_NewBuffer(edpPacket, strlen(type_bin_head));
	else
		EDP_NewBuffer(edpPacket, send_len + 24);
	if(edpPacket->_data == NULL)
		return 2;

	//Byte0：消息类型--------------------------------------------------------------
	edpPacket->_data[edpPacket->_len++] = SAVEDATA;
	
	if(devid)
	{
		if(type == 2)
			remain_len = 12 + strlen(type_bin_head) + send_len;
		else
			remain_len = 8 + send_len + devid_len;
		
		//剩余消息长度-------------------------------------------------------------
		edpPacket->_len += WriteRemainlen(edpPacket->_data, remain_len, edpPacket->_len);
		
		//标志--bit7:1-有devid，0-无devid		bit6:1-有消息编号，0-无消息编号----
		edpPacket->_data[edpPacket->_len++] = 0xC0;
		
		//DEVID长度---------------------------------------------------------------
		edpPacket->_data[edpPacket->_len++] = 0;
		edpPacket->_data[edpPacket->_len++] = devid_len;
		
		//DEVID------------------------------------------------------------------
		strncat((int8 *)edpPacket->_data + edpPacket->_len, devid, devid_len);
		edpPacket->_len += devid_len;
		
		//消息编号----------------------------------------------------------------
		edpPacket->_data[edpPacket->_len++] = MSG_ID_HIGH;
		edpPacket->_data[edpPacket->_len++] = MSG_ID_LOW;
	}
	else
	{
		if(type == 2)
			remain_len = 10 + strlen(type_bin_head) + send_len;
		else
			remain_len = 6 + send_len;
		 
		//剩余消息长度------------------------------------------------------------
		edpPacket->_len += WriteRemainlen(edpPacket->_data, remain_len, edpPacket->_len);
		
		//标志--bit7:1-有devid，0-无devid		bit6:1-有消息编号，0-无消息编号----
		edpPacket->_data[edpPacket->_len++] = 0x40;
		
		//消息编号----------------------------------------------------------------
		edpPacket->_data[edpPacket->_len++] = MSG_ID_HIGH;
		edpPacket->_data[edpPacket->_len++] = MSG_ID_LOW;
	}
	
	edpPacket->_data[edpPacket->_len++] = type;
	
	if(type == 2)
	{
		uint8 type_bin_head_len = strlen(type_bin_head);
		uint8 i = 0;
		
		//消息头长度---------------------------------------------------------------
		edpPacket->_data[edpPacket->_len++] = MOSQ_MSB(type_bin_head_len);
		edpPacket->_data[edpPacket->_len++] = MOSQ_LSB(type_bin_head_len);
		
		//消息头-------------------------------------------------------------------
		for(; i < type_bin_head_len; i++)
			edpPacket->_data[edpPacket->_len++] = type_bin_head[i];
		
		//图片长度-----------------------------------------------------------------
		edpPacket->_data[edpPacket->_len++] = (uint8)(send_len >> 24);
		edpPacket->_data[edpPacket->_len++] = (uint8)(send_len >> 16);
		edpPacket->_data[edpPacket->_len++] = (uint8)(send_len >> 8);
		edpPacket->_data[edpPacket->_len++] = (uint8)send_len;
	}
	else
	{
		//json长度-----------------------------------------------------------------
		edpPacket->_data[edpPacket->_len++] = MOSQ_MSB(send_len);
		edpPacket->_data[edpPacket->_len++] = MOSQ_LSB(send_len);
	}
	
	return 0;

}

//==========================================================
//	函数名称：	EDP_PacketPushData
//
//	函数功能：	PushData功能组包
//
//	入口参数：	devid：设备ID
//				msg：推送数据
//				msg_len：推送的数据长度
//				edpPacket：包指针
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
//==========================================================
uint8 EDP_PacketPushData(const int8 *devid, const int8 *msg, uint32 msg_len, EDP_PACKET_STRUCTURE *edpPacket)
{
	
	uint32 remain_len = 2 + strlen(devid) + msg_len;
	uint8 devid_len = strlen(devid);
	uint16 i = 0;
	uint16 size = 5 + strlen(devid) + msg_len;
	
	if(devid == NULL || msg == NULL || msg_len == 0)
		return 1;

	EDP_NewBuffer(edpPacket, size);
	if(edpPacket->_data == NULL)
		return 2;
	
	//Byte0：pushdata类型-----------------------------------------------------------
	edpPacket->_data[edpPacket->_len++] = PUSHDATA;
	
	//剩余长度----------------------------------------------------------------------
	edpPacket->_len += WriteRemainlen(edpPacket->_data, remain_len, edpPacket->_len);
	
	//DEVID长度---------------------------------------------------------------------
	edpPacket->_data[edpPacket->_len++] = MOSQ_MSB(devid_len);
	edpPacket->_data[edpPacket->_len++] = MOSQ_LSB(devid_len);
	
	//写入DEVID---------------------------------------------------------------------
	for(; i < devid_len; i++)
		edpPacket->_data[edpPacket->_len++] = devid[i];
	
	//写入数据----------------------------------------------------------------------
	for(i = 0; i < msg_len; i++)
		edpPacket->_data[edpPacket->_len++] = msg[i];
	
	return 0;

}

//==========================================================
//	函数名称：	EDP_UnPacketPushData
//
//	函数功能：	PushData功能解包
//
//	入口参数：	rev_data：收到的数据
//				src_devid：源devid缓存
//				req：命令缓存
//				req_len：命令长度
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
//==========================================================
uint8 EDP_UnPacketPushData(uint8 *rev_data, int8 **src_devid, int8 **req, uint32 *req_len)
{

	int32 read_pos = 0;
	uint32 remain_len = 0;
	uint16 devid_len = 0;
	
	//Byte0：PushData消息------------------------------------------------------------
	if(rev_data[read_pos++] != PUSHDATA)
		return 1;

	//读取剩余长度--------------------------------------------------------------------
	read_pos = ReadRemainlen((int8 *)rev_data, &remain_len, read_pos);
	if(read_pos == -1)
		return 2;
	
	//读取源devid长度-----------------------------------------------------------------
	devid_len = (uint16)rev_data[read_pos] << 8 | rev_data[read_pos + 1];
	read_pos += 2;

	//分配内存------------------------------------------------------------------------
	*src_devid = (int8 *)EDP_MallocBuffer(devid_len + 1);
	if(*src_devid == NULL)
		return 3;

	//读取源devid---------------------------------------------------------------------
	memset(*src_devid, 0, devid_len + 1);
	memcpy(*src_devid, (const int8 *)rev_data + read_pos, devid_len);
	read_pos += devid_len;

	remain_len -= 2 + devid_len;

	//分配内存------------------------------------------------------------------------
	*req = (int8 *)EDP_MallocBuffer(remain_len + 1);
	if(*req == NULL)
	{
		EDP_FreeBuffer(*src_devid);
		return 4;
	}

	//读取命令------------------------------------------------------------------------
	memset(*req, 0, remain_len + 1);
	memcpy(*req, (const int8 *)rev_data + read_pos, remain_len);
	read_pos += remain_len;

	*req_len = remain_len;
	
	return 0;

}

//==========================================================
//	函数名称：	EDP_UnPacketCmd
//
//	函数功能：	下发命令解包
//
//	入口参数：	rev_data：收到的数据
//				cmdid：cmdid
//				cmdid_len：cmdid长度
//				req：命令
//				req_len：命令长度
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
//==========================================================
uint8 EDP_UnPacketCmd(uint8 *rev_data, int8 **cmdid, uint16 *cmdid_len, int8 **req, uint32 *req_len)
{

	uint32 remain_len = 0;
	int32 read_pos = 0;
	
	//Byte0：PushData消息------------------------------------------------------------
	if(rev_data[read_pos++] != CMDREQ)
		return 1;
	
	//读取剩余长度--------------------------------------------------------------------
	read_pos = ReadRemainlen((int8 *)rev_data, &remain_len, read_pos);
	if(read_pos == -1)
		return 2;
	
	//读取cmdid长度-------------------------------------------------------------------
	*cmdid_len = (uint16)rev_data[read_pos] << 8 | rev_data[read_pos + 1];
	read_pos += 2;
	
	//分配内存------------------------------------------------------------------------
	*cmdid = (int8 *)EDP_MallocBuffer(*cmdid_len + 1);
	if(*cmdid == NULL)
		return 3;
	
	//读取cmdid-----------------------------------------------------------------------
	memset(*cmdid, 0, *cmdid_len + 1);
	memcpy(*cmdid, (const int8 *)rev_data + read_pos, *cmdid_len);
	read_pos += *cmdid_len;
	
	//读取req长度---------------------------------------------------------------------
	*req_len = (uint32)rev_data[read_pos] << 24 | (uint32)rev_data[read_pos + 1] << 16
					 | (uint32)rev_data[read_pos + 2] << 8 | (uint32)rev_data[read_pos + 3];
	read_pos += 4;
	
	//分配内存------------------------------------------------------------------------
	*req = (int8 *)EDP_MallocBuffer(*req_len + 1);
	if(*req == NULL)
	{
		EDP_FreeBuffer(*cmdid);
		return 4;
	}
	
	//读取req-------------------------------------------------------------------------
	memset(*req, 0, *req_len + 1);
	memcpy(*req, (const int8 *)rev_data + read_pos, *req_len);
	read_pos += *req_len;
	
	return 0;

}

//==========================================================
//	函数名称：	EDP_PacketCmdResp
//
//	函数功能：	命令回复组包
//
//	入口参数：	cmdid：命令的cmdid(随命令下发)
//				cmdid_len：cmdid长度
//				req：命令
//				req_len：命令长度
//				edpPacket：包指针
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
//==========================================================
uint1 EDP_PacketCmdResp(const int8 *cmdid, uint16 cmdid_len, const int8 *resp, uint32 resp_len, EDP_PACKET_STRUCTURE *edpPacket)
{
	
	uint32 remain_len = cmdid_len + resp_len + (resp_len ? 6 : 2);
	
	EDP_NewBuffer(edpPacket, remain_len + 5);
	if(edpPacket->_data == NULL)
		return 1;
	
	//Byte0：CMDRESP消息------------------------------------------------------------
	edpPacket->_data[edpPacket->_len++] = CMDRESP;
	
	//写入剩余长度------------------------------------------------------------------
	edpPacket->_len += WriteRemainlen(edpPacket->_data, remain_len, edpPacket->_len);
	
	//写入cmdid长度------------------------------------------------------------------
	edpPacket->_data[edpPacket->_len++] = cmdid_len >> 8;
	edpPacket->_data[edpPacket->_len++] = cmdid_len & 0x00FF;
	
	//写入cmdid----------------------------------------------------------------------
	memcpy((int8 *)edpPacket->_data + edpPacket->_len, cmdid, cmdid_len);
	edpPacket->_len += cmdid_len;
	
	if(resp_len)
	{
		//写入req长度-----------------------------------------------------------------
		edpPacket->_data[edpPacket->_len++] = (uint8)(resp_len >> 24);
		edpPacket->_data[edpPacket->_len++] = (uint8)(resp_len >> 16);
		edpPacket->_data[edpPacket->_len++] = (uint8)(resp_len >> 8);
		edpPacket->_data[edpPacket->_len++] = (uint8)(resp_len & 0x00FF);
		
		//写入req---------------------------------------------------------------------
		memcpy((int8 *)edpPacket->_data + edpPacket->_len, resp, resp_len);
		
		edpPacket->_len += resp_len;
	}

	return 0;

}

//==========================================================
//	函数名称：	EDP_PacketPing
//
//	函数功能：	心跳请求组包
//
//	入口参数：	edpPacket：包指针
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
//==========================================================
uint1 EDP_PacketPing(EDP_PACKET_STRUCTURE *edpPacket)
{


	EDP_NewBuffer(edpPacket, 2);
	if(edpPacket->_data == NULL)
		return 1;
	
	//Byte0：PINGREQ消息------------------------------------------------------------
	edpPacket->_data[edpPacket->_len++] = PINGREQ;    //装载C0
	
	//Byte1：0----------------------------------------------------------------------
	edpPacket->_data[edpPacket->_len++] = 0;          //装载00
	
  USART2_Write(USART2,edpPacket->_data, edpPacket->_len);	  //发送心跳包 
	return 0;

}

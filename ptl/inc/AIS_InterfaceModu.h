/******************************************************************
Copyright(c) 2010, TJU-TCB
All right reserved
File name:		AIS_InterfaceModu.h
Description:	Protocol related Interface process
Others:			void
Author:			WT
Version:		1.0
Date:			2010.7.21
Function List:  
*****************************************************************************
*  History:               				                                    *
*      <author>   <time>    <version >     <desc>                           *
*         WT     10/07/21     1.0       build this moudle                   *
****************************************************************************/
#ifndef AIS_INTERFACEMODU
#define AIS_INTERFACEMODU
#include "def.h"
#include "gentype.h"

#define MSG_6_MAX_LEN   115                                                     //lq 单位byte （920bits）
#define MSG_14_MAX_LEN  126                    //lq 单位Byte
#define MSG_18_MAX_LEN  21
#define MSG_19_MAX_LEN  39
#define MSG_1_MAX_LEN  MSG_18_MAX_LEN	/** message 1的长度与message 18相同 */
#define MSG_21_MAX_LEN  45

//全局变量声明
extern U16   AISMin; 
extern U16   AISSlot;

extern U32   Send18Time;		     //本次发送18号消息的时间
extern U32   Send19Time;		     //本次发送19号消息的时间

extern U16   SendSlotSelectedFlag;//wt 选取了消息发送时隙标记，1表示已经选取好了
extern U8   G_Msg1[];
extern U8   G_Msg6IFM0[];        //lq 消息6 
extern U8   G_Msg18[];			 //消息18模版，初始化后依照本模版生成18号消息
extern U8   G_Msg19[];			 //消息19模版，初始化后依照本模版生成19号消息
extern U8   G_Msg14[];			 //消息14存储位置
extern U8   G_Msg21[];
extern U32   Send14Time;
extern U16   Msg14NeedToSend;		 //有消息14需要发送
extern U8 Msg6AppDataText[];     //lq 6-bit ASCII中无小写字符

extern U32   SendInterval;	 //4503消息发送间隔指示		6分钟
extern U32   SendInterval_4501_dyn_low;	 //4501动态消息低速发送间隔指示	3分钟
extern U32   SendInterval_4501_dyn_high;	 //4501动态消息高速发送间隔指示	30秒 1125时隙
extern U32   SendInterval_4501_sta;	 //4501静态态消息发送间隔指示	6分钟
#define SPEEDFLAG 20		//高低航速的门限值

//函数声明
U16 InitInterfacePara(void);
void CreatMsgMana(void);
void CreatMsgMana_4501(void);
void CreatMsgMana_4503(void);
U16 Change4501Channel(void);

extern U16 ChangeChannel(void);

/**
	@ingroup 协议栈
	@brief 发送消息14到接口FIFO
	@param 无
	@retval 0 结束
*/
extern U16 SendMsg14(void);

/**
	@ingroup 协议栈
	@brief 发送消息18到接口FIFO
	@param 无
	@retval 0 结束
*/
extern U16 SendMsg18(U8 burstFlg);

/**
	@ingroup 协议栈
	@brief 发送消息19到接口FIFO
	@param 无
	@retval 0 结束
*/
extern U16 SendMsg19(U8 burstFlg);

extern U16 SendMsg14_2(U8 inChannelNum);
extern U16 SendMsg18_2(U8 inChannelNum);
extern U16 SendMsg19_2(U8 inChannelNum, U8 inTimeoutFlg);
extern U16 SendMsg1_SO(U8 ucBurstFlg, U8 ucTestFlg);
extern U16 SendMsg14_SO(U8 ucBurstFlg, U8 ucTestFlg);
#endif


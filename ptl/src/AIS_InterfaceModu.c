/******************************************************************
Copyright(c) 2010, TJU-TCB
All right reserved
File name:		AIS_InterfaceModu.c
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
#if 0
#include <stdio.h>
#include <stdlib.h>
#include "AIS_InterfaceModu.h"
//#include "AIS_TimeModu.h"
#include "Ptl_Interface.h"
#include "AISconfig.h"
#include "LinkPtlPort.h"
#include "AIS_MsgModu.h"
#include "AISMsgModule.h"

//全局变量声明
U8 G_Msg1[MSG_1_MAX_LEN + 1];
U8 G_Msg6IFM0[MSG_6_MAX_LEN + 1];                                                   //lq 消息6 
U8 G_Msg14[MSG_14_MAX_LEN + 1];	         	//消息14存储位置
U8 G_Msg18[MSG_18_MAX_LEN + 1];	         	//消息18模版，初始化后依照本模版生成18号消息
U8 G_Msg19[MSG_19_MAX_LEN + 1];	         	//消息19模版，初始化后依照本模版生成19号消息
U8 G_Msg21[MSG_21_MAX_LEN + 1];	         	//消息21模版，初始化后依照本模版生成21号消息

U8 Msg6AppDataText[MSG6_IFM0_TEXT_LEN] = "ATON_MSG6!";     //lq 6-bit ASCII中无小写字符

U16   AISMin = 0; 
U16   AISSlot = 0;

U32   Send18Time = 0;		     //本次发送18号消息的时间
U32   Send19Time = 0;		     //本次发送19号消息的时间
U32   Send14Time = 0;			 //本次发送14号消息的时间

U16 	SendSlotSelectedFlag = 0;//wt 选取了消息发送时隙标记，1表示已经选取好了
U16 	Msg14NeedToSend;			 //有消息14需要发送
	
extern U8 Channel;

U16 Send14Flag = 0;//lg 1表示需要发送消息14，0表示不需要发送消息14

U32   SendInterval = 0x6000;	 //4503消息发送间隔指示		6分钟
U32   SendInterval_4501_dyn_low = 0x3000;	 //4501动态消息低速发送间隔指示	3分钟
U32   SendInterval_4501_dyn_high = 0x465;	 //4501动态消息高速发送间隔指示	30秒 1125时隙
U32   SendInterval_4501_sta = 0x6000;	 //4501静态态消息发送间隔指示	6分钟

/******************************************
函数：InitInterfacePara
功能: 初始化消息管理结构体参数
输入:
输出:
返回值：
备注：
*******************************************/
U16 InitInterfacePara(void)
{
    mmMessCell.MsgID = 18;
    mmMessCell.FouceSendFlg = 0;
	mmMessCell.ChannelNum = Channel_4503;
	mmMessCell.DataLength = 0;
	mmMessCell.GPSvalid = 0;
	mmMessCell.MessState = 0;
	mmMessCell.pData = NULL;
	mmMessCell.Timer = 0;
	mmMessCell.TranSlotNum = 0;
	mmMessCell.CurrentOperationFlg = 1;	//上电初始有业务

	return 0;
}


/*******************************************************************************
* 名称  : Change4501Channel
* 描述  : 4501模式切换信道
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U16 Change4501Channel(void)
{
	if (Channel_4501 == CHANNEL_A)
	{
		Channel_4501 = CHANNEL_B;
	} 
	else if (Channel_4501 == CHANNEL_B)
	{
		Channel_4501 = CHANNEL_A;
	}
	return 0;
}

/*******************************************************************************
* 名称  : ChangeChannel
* 描述  : 切换信道
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U16 ChangeChannel(void)
{
	if (Channel == CHANNEL_A)
	{
		Channel = CHANNEL_B;
	} 
	else if (Channel == CHANNEL_B)
	{
		Channel = CHANNEL_A;
	}
	return 0;
}


/*******************************************************************************
* 名称  : SendMsg19_SO
* 描述  : 发送消息19 
* 输入  : 无	  		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U16 SendMsg6(void)
{
    InitMsg6();

	mmMessCell.pData = G_Msg6IFM0 + 1;

	mmMessCell.MsgID = 6;
	mmMessCell.FouceSendFlg = 0;

    if (GetBurstFlg() == TRUE)
    {
        mmMessCell.TranSlotNum = (GetBurstTxTimSlot() + 2) % 2250;  //lq 6号消息在21号消息的后两个时隙发送  
    }
    else
    {
        mmMessCell.TranSlotNum = (GetMsgTxTimSlot() + 2) % 2250;    
    }
    	
	mmMessCell.ChannelNum = GetTxChannel();
	mmMessCell.DataLength = G_Msg6IFM0[0];
	mmMessCell.MessState = 1; 
	
	LinkPtlPort(&mmMessCell);

	return 0;
}

#if 0
/*******************************************************************************
* 名称  : SendMsg14_SO
* 描述  : 发送消息14 
* 输入  : ucBurstFlg - 是否是组内第一条消息，
          ucTestFlg - 是否是测试态发送	  		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U16 SendMsg14_SO(U8 ucBurstFlg, U8 ucTestFlg)
{
	InitMsg14(ucTestFlg);

	mmMessCell.pData = G_Msg14 + 1;

	mmMessCell.MsgID = 14;
	mmMessCell.FouceSendFlg = 0;

    if (GetBurstFlg() == TRUE)
    {
        mmMessCell.TranSlotNum = GetBurstTxTimSlot();    
    }
    else
    {
        mmMessCell.TranSlotNum = GetMsgTxTimSlot();    
    }
	
	mmMessCell.ChannelNum = GetTxChannel();
	mmMessCell.DataLength = G_Msg14[0];
	mmMessCell.MessState = 1; 
	
	LinkPtlPort(&mmMessCell);

	return 0;
}
#endif

/*******************************************************************************
* 名称  : SendMsg14
* 描述  : 发送消息14 
* 输入  : 无  		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U16 SendMsg14(void)
{
	InitMsg14();

	mmMessCell.pData = G_Msg14 + 1;

	mmMessCell.MsgID = 14;
	mmMessCell.FouceSendFlg = 0;

    if (GetBurstFlg() == TRUE)
    {
        mmMessCell.TranSlotNum = (GetBurstTxTimSlot() + 4) % 2250;  //lq 14号消息在21号消息的后4个时隙发送    
    }
    else
    {
        mmMessCell.TranSlotNum = (GetMsgTxTimSlot() + 4) % 2250;    
    }
	
	mmMessCell.ChannelNum = GetTxChannel();
	mmMessCell.DataLength = G_Msg14[0];
	mmMessCell.MessState = 1; 
	
	LinkPtlPort(&mmMessCell);

	return 0;
}

/*******************************************************************************
* 名称  : SendMsg18
* 描述  : 发送消息18 
* 输入  : burstFlg - 是否是组内第一条消息，  		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U16 SendMsg18(U8 burstFlg)
{
    InitMsg18();

	mmMessCell.pData = AISMsgBinBuff_18 + 1;

	mmMessCell.MsgID = 18;
	mmMessCell.FouceSendFlg = 0;

    if (GetBurstFlg() == TRUE)
    {
        mmMessCell.TranSlotNum = GetBurstTxTimSlot();  //lq 18号消息  
    }
    else
    {
        mmMessCell.TranSlotNum = GetMsgTxTimSlot();    
    }
    	
	mmMessCell.ChannelNum = GetTxChannel();
	mmMessCell.DataLength = AISMsgBinBuff_18[0];
	mmMessCell.MessState = 1; 
	
	LinkPtlPort(&mmMessCell);

	return 0;
}

/*******************************************************************************
* 名称  : SendMsg19
* 描述  : 发送消息19 
* 输入  : burstFlg - 是否是组内第一条消息，
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U16 SendMsg19(U8 burstFlg)
{
    InitMsg19();

	mmMessCell.pData = AISMsgBinBuff_19 + 1;

	mmMessCell.MsgID = 19;
	mmMessCell.FouceSendFlg = 0;

    if (GetBurstFlg() == TRUE)
    {
        mmMessCell.TranSlotNum = (GetBurstTxTimSlot() + 1) % 2250;  //lq 19号消息在18号消息的下一时隙发送  
    }
    else
    {
        mmMessCell.TranSlotNum = (GetMsgTxTimSlot() + 1) % 2250;    
    }
    	
	mmMessCell.ChannelNum = GetTxChannel();
	mmMessCell.DataLength = AISMsgBinBuff_19[0];
	mmMessCell.MessState = 1; 
	
	LinkPtlPort(&mmMessCell);

	return 0;
}

/*******************************************************************************
* 名称  : SendMsg21
* 描述  : 发送消息21
* 输入  : 无 		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U16 SendMsg21(void)
{
    InitMsg21();

	mmMessCell.pData = G_Msg21 + 1;

	mmMessCell.MsgID = 21;
	mmMessCell.FouceSendFlg = 0;

    if (GetBurstFlg() == TRUE)
    {
        mmMessCell.TranSlotNum = GetBurstTxTimSlot();  //lq 18号消息  
    }
    else
    {
        mmMessCell.TranSlotNum = GetMsgTxTimSlot();    
    }
    	
	mmMessCell.ChannelNum = GetTxChannel();
	mmMessCell.DataLength = G_Msg21[0];
	mmMessCell.MessState = 1; 
	
	LinkPtlPort(&mmMessCell);

	return 0;
}
#endif

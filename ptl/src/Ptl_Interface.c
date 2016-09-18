/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: Ptl_Interface.c
  Author: lq       Version : V1.00        Date: 2012.04.24
  Description:     协议栈接口模块变量、函数定义，初始化初始化协议栈接口
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Ptl_Interface.h"
#include "AISconfig.h"
#include "initial.h"

/************************************************************************/
/* 协议栈和上层接口														*/
/************************************************************************/
GPSStruct	gpsStoreCell;	//GPS数据结构体		，GPS的信息由底层定时更新
AISStcStr	 aiAISCell;		//静态变量结构体
UCHAR SOS_PressFlg = 0;	//SOS按键状态 TRUE表示按下
UCHAR ModeFlag = 0;				//0:4503	1:4501


/************************************************************************/
/* 协议栈和底层接口														*/
/************************************************************************/
PTL_MAC_Interface Ptl_Mac_Interface;		//协议栈发送至底层接口
MAC_PTL_Interface Mac_Ptl_Interface;		//底层发送至协议栈接口
MessManaStr mmMessCell;		     //消息管理参数	
PTL_TIMER Ptl_Timer;		//协议栈使用的定时器 每1/2250分钟更新一次，由外部时钟程序调用

/************************************************************************/
/* 发送信道信息                                                                     */
/************************************************************************/
U8 Channel_4503;			//4503模式下的起始发送信道值
U8 Channel_4501;			//4501模式下的起始发送信道值
U8 Channel;

/*******************************************************************************
* 名称  : AISStcStructInit
* 描述  : 初始化AISStcStr结构体
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISStcStructInit(void)
{
	U8 len_1;
	U8 len_2;
	U8 len_3;

	aiAISCell.UserID = siStaticPara.MMSI;					    //MMSI
	aiAISCell.Dimension_Position[0] = siStaticPara.ShipA;		//安装位置
	aiAISCell.Dimension_Position[1] = siStaticPara.ShipB;		//安装位置
	aiAISCell.Dimension_Position[2] = siStaticPara.ShipC;		//安装位置
	aiAISCell.Dimension_Position[3] = siStaticPara.ShipD;		//安装位置
#if 0
	len_1 = strlen((char *)siStaticPara.NameofAtoN);
	len_2 = sizeof(aiAISCell.Name);
	len_3 = len_1 > len_2 ? len_2 : len_1;
	strncpy(aiAISCell.Name, (char *)siStaticPara.NameofAtoN, len_3);
#else
	strcpy(aiAISCell.Name, (char *)siStaticPara.NameofAtoN);
#endif

#if 0    
	len_1 = strlen((char *)siStaticPara.VendID);
	len_2 = sizeof(aiAISCell.VenderID);
	len_3 = len_1 > len_2 ? len_2 : len_1;
	strncpy((char*)aiAISCell.VenderID, (char *)siStaticPara.VendID, len_3);
#else
	strcpy((char*)aiAISCell.VenderID, (char *)siStaticPara.VendID);
#endif

	len_1 = strlen("MAYDAY MAYDAY");
	len_2 = sizeof(aiAISCell.saferelatedtext);
	len_3 = len_1 > len_2 ? len_2 : len_1;
	strncpy(aiAISCell.saferelatedtext, "MAYDAY MAYDAY", len_3);

	aiAISCell.SeqNum = siStaticPara.SerialNumber;				//不同的序列号对应的发送时隙不同
    aiAISCell.DestMMSI = siStaticPara.DestMMSI;                 // (xxxxxxxxx)
    aiAISCell.AtoNStatus = siStaticPara.AtoNStatus;       	    // (hh)
    aiAISCell.TypeofEPFS = siStaticPara.TypeofEPFS;		        // (x)
    aiAISCell.PositionAccuracy = siStaticPara.PositionAccuracy;	// (x)
    aiAISCell.PowerLevel = siStaticPara.PowerLevel;		        // (x)
    aiAISCell.TypeofAtoN = siStaticPara.TypeofAtoN;		        // (xx)
    aiAISCell.VirtualAtoNFlag = siStaticPara.VirtualAtoNFlag;	// (x)
}

/*******************************************************************************
* 名称  : InitPtl_Interface
* 描述  : 初始化协议栈接口
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U16 InitPtl_Interface(void)
{
	Channel_4501 = CHANNEL_A;
	Channel_4503 = CHANNEL_A;
	Channel = CHANNEL_A;
	//ModeFlag = 0;
    
    /*lq AIS消息中的静态字段初始化*/
    AISStcStructInit();
 
	mmMessCell.MsgID = 0;					    //消息号
	mmMessCell.MessState = 0;         			//消息发送状态
	mmMessCell.TranSlotNum = 0;         			//发送时隙号
	mmMessCell.ChannelNum = 0;		  			//信道标志0/1
	mmMessCell.Timer = 0;						//计时器
	mmMessCell.pData = NULL;             			//发送数据指针
	mmMessCell.DataLength = 0;		 			//发送数据长度
	mmMessCell.GPSvalid = 1;					    //GPS有效状态
	mmMessCell.CurrentOperationFlg = 1;          //当前业务标志

	return 0;
}



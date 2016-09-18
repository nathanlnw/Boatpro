/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: Ptl_Interface.h
  Author: lq       Version : V1.00        Date: 2012.04.24
  Description:     协议栈接口模块宏、结构体定义，变量、函数声明
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/

#ifndef PTL_INTERFACE_H
#define PTL_INTERFACE_H

#include "gentype.h"
#include "gpsctrl.h"
#include "initial.h"

//结构体定义
/*****************************************************************************
*     		 GPS struct   		 *
******************************************************************************/
/*
typedef struct {
	UCHAR GPSTimeBuf[6];
	CHAR GPSDateBuf[6];				//20110926 lg
	U16  GPS_PosFix;					//GPS定位状态
	U16 GPS_TimeY;
	U16 GPS_TimeMon;
	U16 GPS_TimeD;
	U16 GPS_TimeH;
	U16 GPS_TimeM;
	U16 GPS_TimeS;
	S16 GPS_LatiD;
	S32 GPS_Latitude;
	S32 GPS_Lati100;
	S32 GPS_Longitud;
	S32 GPS_Long100;
	U32 GPS_Sog;
	U32 GPS_Sog_Buf[18];
	U16 GPS_Sog_Index;
	U32 GPS_Cog;
	U16 GPS_State;
	U16 GPS_errlat;
	U16 GPS_errlon;
	U16 GPS_PosPrec;
}GPSStruct;
*/

/*****************************************************************************
*     		 AISStc struct   		 *
******************************************************************************/

/**
	@ingroup 协议栈 
	@brief 协议栈存储静态信息的结构体
*/
typedef struct {
	ULINT  UserID;					    //MMSI
    USINT Dimension_Position[4];		//安装位置
	char Name[EQ_ATON_NAME_LEN + 1];    //AtoN名,含‘\0’
	UCHAR VenderID[EQ_VEND_ID_LEN + 1];	//厂商ID,含‘\0’
	// char CallSign[7];			    //呼号
	char saferelatedtext[21];			//安全信息                            //lq 限定14号安全消息的安全相关内容的最大长度为21Bytes（不包含消息的其他内容40bits）
	USINT SeqNum;                       //序列号

    U32 DestMMSI;           // (xxxxxxxxx)
    U8  AtoNStatus;       	// (hh)
    U8	TypeofEPFS;		    // (x)
    U8	PositionAccuracy;	// (x)
    U8	PowerLevel;		    // (x)
    U8  TypeofAtoN;		    // (xx)
    U8	VirtualAtoNFlag;	// (x)

} AISStcStr;
/*****************************************************************************
*     		MessMana struct   		 *
******************************************************************************/

/**
	@ingroup 数据处理
	@brief 协议栈存放消息的结构体，包含消息和对应信道等
*/
typedef struct {
	U16 MsgID;					    //消息号	lg added 20110614
	U16 FouceSendFlg;				    //强制发送标志  lg added 20110617
	U16 MessState;         			//消息发送状态                              //lq 标识该消息是否已放入协议栈（TRUE-可以放入，但还未放入;FALSE-不可以放入）
	S16 TranSlotNum;         			//发送时隙号                            //lq 消息发射时隙号
	U8  ChannelNum;		  			//信道标志0/1
	U16 Timer;						//计时器
	U8 *pData;             			//发送数据指针
	U16 DataLength;		 			//发送数据长度                              //lq 单位为Byte
	U16 GPSvalid;					    //GPS有效状态
	U16 CurrentOperationFlg;          //当前业务标志
} MessManaStr;

/*****************************************************************************
*     		Interface struct   		 *
******************************************************************************/
typedef struct {
	USINT MsgID;					    //消息号
	USINT MessState;         			//消息发送状态
	USINT TranSlotNum;         			//发送时隙号
	USINT ChannelNum;		  			//信道标志0/1
	USINT Timer;						//计时器
	USINT pData[100];             			//发送数据指针
	USINT DataLength;		 			//发送数据长度
	USINT GPSvalid;					    //GPS有效状态
	USINT CurrentOperationFlg;          //当前业务标志
} PTL_MAC_Interface;

typedef struct {
	USINT WakeUpFlag;		//休眠唤醒标志(唤醒后置1)
	USINT ReportFlag;		//消息发送成功与否的反馈报告有效性标志
	USINT ReportType;		//消息发送成功与否的反馈报告类型
} MAC_PTL_Interface;

typedef struct {
	USINT Slot;
	USINT Min;
} PTL_TIMER;


extern PTL_TIMER Ptl_Timer;

extern GPSStruct	gpsStoreCell;	//GPS数据结构体
extern AISStcStr	 aiAISCell;		//静态变量结构体
extern UCHAR SOS_PressFlg;	//SOS按键状态 TRUE表示按下
extern UCHAR ModeFlag;				//0:4503	1:4501

extern PTL_MAC_Interface Ptl_Mac_Interface;		//协议栈发送至底层接口
extern MAC_PTL_Interface Mac_Ptl_Interface;		//底层发送至协议栈接口
extern MessManaStr mmMessCell;		     //消息管理参数	

extern U8 Channel_4503;			//4503模式下的起始发送信道值
extern U8 Channel_4501;			//4501模式下的起始发送信道值
extern U8 Channel;
#define SENDMSG_SPACE 4		//提前发送给底层的时隙值

USINT InitPtl_Interface(void);
USINT Timer_Run(void);

#endif

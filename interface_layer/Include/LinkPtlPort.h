/****************************************************************************
*  Copyright (C), 2010, 712.Co.,Ltd.                                        *
*  FileName: LinkPtlReport.h								   						*
*  Description:  底层与协议栈反馈接口函数相关变量定义及函数声明							*
*****************************************************************************
*  History:               				                                    *
*      <author>  <time>    <version >   <desc>                              *
*      vozhj   	 10/11/25     1.0       Build this file					    *
****************************************************************************/
#ifndef _LINKPTLPORT_H
#define _LINKPTLPORT_H

#define LINKPTL_DATABUF_LEN		65
#define LINKPTL_FIFOLEN			15

#include "Ptl_Interface.h"
/*****************************************************************************
***********************			结构体定义   		***********************
****************************************************************************/
typedef struct {
	U16 MessState;         			//消息发送状态                              //lq 标识协议栈中的该条消息是否可以发送（TRUE-可以发送，但还未发送）
	S16 TranSlotNum;         		//发送时隙号
	U8  ChannelNum;		  			//信道标志0/1
	U8  DataBuf[LINKPTL_DATABUF_LEN];             			//发送数据指针
	U16 DataLength;		 			//发送数据长度
} LinkPtlStruc;

typedef struct {
	LinkPtlStruc LinkPtlPara[LINKPTL_FIFOLEN + 1];                           //lq 队列中时钟空余一个元素不用，方便区分队列的空和满
	U8  FifoIn;
	U8  FifoOut;
} LinkPtlFifoStruc;

/*
typedef struct {
	U16 MessState;         			//消息发送状态
	U16 TranSlotNum;         			//发送时隙号
	U16 ChannelNum;		  			//信道标志0/1
	U16 Timer;						//计时器
	U16 *pData;             			//发送数据指针
	U16 DataLength;		 			//发送数据长度
	U16 GPSvalid;					    //GPS有效状态
	U16 CurrentOperationFlg;          //当前业务标志
} MessManaStr;
*/
extern LinkPtlFifoStruc LinkPtlFifoPara;
//extern MessManaStr mmMessCell;
/*****************************************************************************
***********************			函数声明    		***********************
****************************************************************************/
extern void InitaLinkPtlPortPara (void);
//extern void LinkPtlPort (void);

/**
	@ingroup 数据处理
	@brief 	将协议栈产生的消息放入接口FIFO
	@param mmMessCell 协议栈存放消息的结构体
	@retval 无
*/
extern void LinkPtlPort (MessManaStr *mmMessCell);
extern U8 GetPtlSta (void);
extern U8 ChkPtlFifoSta(void);

/**
	@ingroup 休眠
	@brief 查看协议栈接口FIFO是否为空
	@param 无
	@retval TRUE 消息发送完，空闲
	@retval FALSE FIFO中有消息未发送完
*/
extern U8 GetPtlFifoSleepSta(void);
extern U8 IsLinkPtlQueEmpty(void);

#endif












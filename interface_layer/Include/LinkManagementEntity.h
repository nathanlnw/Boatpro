/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: LinkManagementEntity.h
  Author: lq       Version : V1.00        Date: 2014.07.11
  Description:     AIS链路管理实体层
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _LINK_MANAGEMENT_ENTITY_H
#define _LINK_MANAGEMENT_ENTITY_H

/* Include ------------------------------------------------------------------*/
#include "Include.h"

/* Public define ------------------------------------------------------------*/
#define LME_BINMSG_FIFO_DEPTH              7
#define LME_BINMSG_MAX_LEN_BYTE            160  //lq 5个时隙总共256*5=1280bit=160byte

/* Public typedef -----------------------------------------------------------*/
#if 0
typedef struct BinMsgFifoStruct
{
    U8  FIFO[LME_BINMSG_FIFO_DEPTH][LME_BINMSG_MAX_LEN_BYTE + 1];  //lq DataBuff[0][]存放实际数据长度，5个时隙总共256*5=1280bit=160byte, 
    U16 Input;
    U16 Output;
}BinMsgFifoStruct;
#endif

typedef struct BinMsgRxCellStruct
{
	U8  Data[LME_BINMSG_MAX_LEN_BYTE + 1];
	U16 DataLen;                                        //lq 单位：1byte
	//lq U16 DCOffset;
	S32 ArrivalFrame;
    S16 ArrivalSlot;
    AisChannelEnum  RxChannel;	
}BinMsgRxCellStruct;

typedef struct BinMsgTxCellStruct
{
	U8  Data[LME_BINMSG_MAX_LEN_BYTE + 1];
	U16 DataLen;                                        //lq 单位：1byte
    S16 TxSlot;
    AisChannelEnum TxChannel;	
}BinMsgTxCellStruct;

typedef struct BinMsgRxFifoStruct
{
    BinMsgRxCellStruct FIFO[LME_BINMSG_FIFO_DEPTH];
    U16 Input;
    U16 Output;
}BinMsgRxFifoStruct;

typedef struct BinMsgTxFifoStruct
{
    BinMsgTxCellStruct FIFO[LME_BINMSG_FIFO_DEPTH];
    U16 Input;
    U16 Output;
}BinMsgTxFifoStruct;

/* Public variables ---------------------------------------------------------*/

/* Public function ----------------------------------------------------------*/

#endif

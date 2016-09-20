/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: TransceiverCtrl.h
  Author: lq       Version : V1.0.0        Date: 2014.08.06
  Description:     Transceiver控制
  Version:         V1.0.0
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _TRANSCEIVERCTRL_H_
#define _TRANSCEIVERCTRL_H_

/* Include -------------------------------------------------------------------*/
#include "DataLinkService.h"

/* Public define -------------------------------------------------------------*/
/*lq 测试状态定义 */
#define TRCTRL_TEST_TX_CTRL_STA_IDLE              0
#define TRCTRL_TEST_TX_CTRL_STA_TX                1
#define TRCTRL_TEST_TX_CTRL_STA_HALT              2

/*lq 时间定义 */
#define TRCTRL_TIME_PERIOD                        10      //lq unit: 5ms  发射持续时间

#if 0
#define TRCTRL_TEST_TX_TIMEOUT_TX                 10      //lq unit: 50ms 发射持续时间
#define TRCTRL_TEST_TX_TIMEOUT_HALT               10      //lq unit: 50ms 发射暂停时间
#endif
#define TRCTRL_TEST_TX_TIMEOUT_TX_1               2       //lq unit: 50ms 发射持续时间, 0表示不间断连续发射
#define TRCTRL_TEST_TX_TIMEOUT_HALT_1             2       //lq unit: 50ms 发射暂停时间
#define TRCTRL_TEST_TX_TIMEOUT_TX_2               2       //lq unit: 50ms 发射持续时间
#define TRCTRL_TEST_TX_TIMEOUT_HALT_2             2       //lq unit: 50ms 发射暂停时间
#define TRCTRL_TEST_TX_TIMEOUT_TX_3               2       //lq unit: 50ms 发射持续时间
#define TRCTRL_TEST_TX_TIMEOUT_HALT_3             2       //lq unit: 50ms 发射暂停时间
#define TRCTRL_TEST_TX_TIMEOUT_TX_4               2       //lq unit: 50ms 发射持续时间
#define TRCTRL_TEST_TX_TIMEOUT_HALT_4             2       //lq unit: 50ms 发射暂停时间
#define TRCTRL_TEST_TX_TIMEOUT_TX_5               2       //lq unit: 50ms 发射持续时间
#define TRCTRL_TEST_TX_TIMEOUT_HALT_5             2       //lq unit: 50ms 发射暂停时间
#define TRCTRL_TEST_TX_TIMEOUT_TX_6               2       //lq unit: 50ms 发射持续时间
#define TRCTRL_TEST_TX_TIMEOUT_HALT_6             2       //lq unit: 50ms 发射暂停时间
#define TRCTRL_TEST_TX_TIMEOUT_TX_7               2       //lq unit: 50ms 发射持续时间
#define TRCTRL_TEST_TX_TIMEOUT_HALT_7             2       //lq unit: 50ms 发射暂停时间

/*lq 发射数据缓存 */
#define TRCTRL_TEST_TX_BUFFER_LEN                 (DLS_FRAME_MAX_LEN_BYTE + 2)

/*lq 测试信号类型 */
#define TRCTRL_TEST_TX_SIGNAL_FLOOR               1
#define TRCTRL_TEST_TX_SIGNAL_NUM1                1
#define TRCTRL_TEST_TX_SIGNAL_NUM2                2
#define TRCTRL_TEST_TX_SIGNAL_NUM3                3
#define TRCTRL_TEST_TX_SIGNAL_CW                  4
#define TRCTRL_TEST_TX_SIGNAL_PN9                 5
#define TRCTRL_TEST_TX_SIGNAL_ALL0                6
#define TRCTRL_TEST_TX_SIGNAL_ALL1                7
#define TRCTRL_TEST_TX_SIGNAL_CEIL                7

/*lq 发射控制状态定义 */
#define TRCTRL_TX_CTRL_STATE_IDLE                 0
#define TRCTRL_TX_CTRL_STATE_TX_INIT              1
#define TRCTRL_TX_CTRL_STATE_TX_RUNNING           2

/*lq 发射数据控制状态定义 */
#define TRCTRL_TX_DATA_CTRL_STATE_IDLE            0
#define TRCTRL_TX_DATA_CTRL_STATE_START           1
#define TRCTRL_TX_DATA_CTRL_STATE_READY           2
#define TRCTRL_TX_DATA_CTRL_STATE_DATA            3
#define TRCTRL_TX_DATA_CTRL_STATE_END             4

/*lq 发射数据控制参数定义 */
#define TRCTRL_TX_DATA_BEGIN_BIT_THRES            8     //lq unit: 1bit，时隙起始时刻至数据开始发射的延时
#define TRCTRL_TX_DATA_END_BIT_THRES              8     //lq unit: 1bit，发射数据结束后的延时

/*lq 时间 */
#define TRCTRL_TIMEOUT_TX_DONE_WAITING            200         //lq 单位：5ms

/* Public typedef ------------------------------------------------------------*/
typedef enum
{
    TransceiverTypeA,
    TransceiverTypeB
}TransceiverTypeEnum;

typedef struct TransceiverTestStruct
{
	U32 TimeCnt;
	U16 TimePeriod;
	U16 TimeoutTx;
	U16 TimeoutHalt;
	U16 TimeoutCntTx;
	U16 TimeoutCntHalt;

    U8  TxCtrlSta;
    U8  TxTestFlg;
    
    AisChannelEnum TxChannel;
    U8  TxSignalType;
    U8  TxBuffer[TRCTRL_TEST_TX_BUFFER_LEN];      //lq TxBuffer[0]存放有效数据的整字节数（不包括非整字节），TxBuffer[1]存放有效数据的非整字节比特数
    U16 TxBufferIndex;
    U8  TxBufferOffset;
}TransceiverTestStruct;

typedef struct TransceiverTxCtrlStruct
{
	U32 TimeCnt;
	U16 TimePeriod;

    U8  TxCtrlState;    	
    U8  TxFlg;
    U8  TxDoneFlg;
    S16 TxSlot;
    AisChannelEnum  TxChannel;	
    U8  TxDataCtrlState;
    S16 TxDataStartBitCnt;
    S16 TxDataEndBitCnt;
        
    U8  TxBuffer[DLS_FRAME_MAX_LEN_BYTE + 2];      //lq TxBuffer[0]存放有效数据的整字节数（不包括非整字节），TxBuffer[1]存放有效数据的非整字节比特数
    U16 TxBufferIndex;
    U8  TxBufferOffset;
}TransceiverTxCtrlStruct;
/* Public variables ----------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/
//lnw add
extern U8 TRCTRL_InstTxTestGetSignalBit(TransceiverTypeEnum cs);

#endif

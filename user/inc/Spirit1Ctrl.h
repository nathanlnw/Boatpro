/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: Spirit1Ctrl.h
  Author: lq       Version : V1.0.0        Date: 2014.08.06
  Description:     SPIRIT1控制
  Version:         V1.0.0
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _SPIRIT1CTRL_H
#define _SPIRIT1CTRL_H

/* Include -------------------------------------------------------------------*/
#include "DataLinkService.h"

/* Public define -------------------------------------------------------------*/
/*lq 测试状态定义 */
#define SPRTCTRL_TEST_TX_CTRL_STA_IDLE              0
#define SPRTCTRL_TEST_TX_CTRL_STA_TX                1
#define SPRTCTRL_TEST_TX_CTRL_STA_HALT              2

/*lq 时间定义 */
#define SPRTCTRL_TIME_PERIOD                        10      //lq unit: 5ms  发射持续时间

#if 0
#define SPRTCTRL_TEST_TX_TIMEOUT_TX                 10      //lq unit: 50ms 发射持续时间
#define SPRTCTRL_TEST_TX_TIMEOUT_HALT               10      //lq unit: 50ms 发射暂停时间
#endif
#define SPRTCTRL_TEST_TX_TIMEOUT_TX_1               0       //lq unit: 50ms 发射持续时间, 0表示不间断连续发射
#define SPRTCTRL_TEST_TX_TIMEOUT_HALT_1             10      //lq unit: 50ms 发射暂停时间
#define SPRTCTRL_TEST_TX_TIMEOUT_TX_2               0       //lq unit: 50ms 发射持续时间
#define SPRTCTRL_TEST_TX_TIMEOUT_HALT_2             10      //lq unit: 50ms 发射暂停时间
#define SPRTCTRL_TEST_TX_TIMEOUT_TX_3               0       //lq unit: 50ms 发射持续时间
#define SPRTCTRL_TEST_TX_TIMEOUT_HALT_3             10      //lq unit: 50ms 发射暂停时间
#define SPRTCTRL_TEST_TX_TIMEOUT_TX_4               0       //lq unit: 50ms 发射持续时间
#define SPRTCTRL_TEST_TX_TIMEOUT_HALT_4             10      //lq unit: 50ms 发射暂停时间
#define SPRTCTRL_TEST_TX_TIMEOUT_TX_5               0       //lq unit: 50ms 发射持续时间
#define SPRTCTRL_TEST_TX_TIMEOUT_HALT_5             10      //lq unit: 50ms 发射暂停时间
#define SPRTCTRL_TEST_TX_TIMEOUT_TX_6               0       //lq unit: 50ms 发射持续时间
#define SPRTCTRL_TEST_TX_TIMEOUT_HALT_6             10      //lq unit: 50ms 发射暂停时间
#define SPRTCTRL_TEST_TX_TIMEOUT_TX_7               0       //lq unit: 50ms 发射持续时间
#define SPRTCTRL_TEST_TX_TIMEOUT_HALT_7             10      //lq unit: 50ms 发射暂停时间

/*lq 发射数据缓存 */
#define SPRTCTRL_TEST_TX_BUFFER_LEN                 (DLS_FRAME_MAX_LEN_BYTE + 2)

/*lq 测试信号类型 */
#define SPRTCTRL_TEST_TX_SIGNAL_FLOOR               1
#define SPRTCTRL_TEST_TX_SIGNAL_NUM1                1
#define SPRTCTRL_TEST_TX_SIGNAL_NUM2                2
#define SPRTCTRL_TEST_TX_SIGNAL_NUM3                3
#define SPRTCTRL_TEST_TX_SIGNAL_CW                  4
#define SPRTCTRL_TEST_TX_SIGNAL_PN9                 5
#define SPRTCTRL_TEST_TX_SIGNAL_ALL0                6
#define SPRTCTRL_TEST_TX_SIGNAL_ALL1                7
#define SPRTCTRL_TEST_TX_SIGNAL_CEIL                7

/*lq 发射控制状态定义 */
#define SPRTCTRL_TX_CTRL_STATE_IDLE                 0
#define SPRTCTRL_TX_CTRL_STATE_TX_WAITING           1
#define SPRTCTRL_TX_CTRL_STATE_TX_EXEC              2
#define SPRTCTRL_TX_CTRL_STATE_DONE_WAITING         3

/*lq 时间 */
#define SPRTCTRL_TIMEOUT_TX_DONE_WAITING            200         //lq 单位：5ms

/* Public typedef ------------------------------------------------------------*/
typedef struct SPIRIT1TestStruct
{
	U32 TimeCnt;
	U16 TimePeriod;
	U16 TimeoutTx;
	U16 TimeoutHalt;
	U16 TimeoutCntTx;
	U16 TimeoutCntHalt;

    U8  TxCtrlSta;
    U8  TxTestFlg;
    
    U8  TxSignalType;
    U8  TxBuffer[SPRTCTRL_TEST_TX_BUFFER_LEN];      //lq TxBuffer[0]存放有效数据的整字节数（不包括非整字节），TxBuffer[1]存放有效数据的非整字节比特数
    U16 TxBufferIndex;
    U8  TxBufferOffset;
}SPIRIT1TestStruct;

typedef struct SPIRIT1TxCtrlStruct
{
	U32 TimeCnt;
	U16 TimePeriod;

    U8  TxCtrlState;	
    U8  TxFlg;
    U16 TxSlot;
    U8  TxChannel;	
        
    U8  TxBuffer[DLS_FRAME_MAX_LEN_BYTE + 2];      //lq TxBuffer[0]存放有效数据的整字节数（不包括非整字节），TxBuffer[1]存放有效数据的非整字节比特数
    U16 TxBufferIndex;
    U8  TxBufferOffset;
}SPIRIT1TxCtrlStruct;
/* Public variables ----------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/

#endif

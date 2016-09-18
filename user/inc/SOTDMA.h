/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: SOTDMA.h
  Author: lq       Version : V1.00        Date: 2012.04.24
  Description:     SOTDMA接入方式消息发送控制模块宏、结构体定义，变量、函数声明
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _SOTDMA_H
#define _SOTDMA_H

/* Includes ------------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef struct SOTDMAPara
{
    S16 BurstTxTimSlot;
    S32 BurstTxTimFrame;
    S16 MsgTxTimSlot;
    S32 MsgTxTimFrame;
    
    S16 BurstSendTimSlot;
    S32 BurstSendTimFrame;
    S16 MsgSendTimSlot;
    S32 MsgSendTimFrame;
    
    S8  Timeout;
    S8  MsgCount;
    U8  BurstFlg;
    U8  TxChannel;
    U8  BurstFinsFlg;
    U8  BurstStarFlg;
    U16 SlotOffset;
}SOTDMAParaStruct;

#if 0
typedef struct SOCommState
{
    U8  ucSyncState;                                                            // 同步状态                                    
    S8  ucSlotTimeout;                                                          // 时隙超时
    U16 usSubMsg;                                                               // 子消息

    U16 usSlotNum;                                                              // 用于此次发射的时隙号
    U16 usSlotOffset;                                                           // 时隙偏差
    U16 usUTC_HourMin;                                                          // UTC小时和分钟
    U16 usRecStations;                                                          // 接收的台站数
}SOCommStateStruct;
#else
typedef struct ICommState
{
    U8  ucSyncState;                                                            // 同步状态                                    
    U16 usSlotIncrement;                                                        // 时隙增量
    U8  ucSlotNumbers;                                                          // 时隙数
    U8  ucKeepFlag;                                                             // 保持标记                                                         // 接收的台站数
}ICommStateStruct;
#endif
/* Private macro ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function  ---------------------------------------------------------*/
/* Public  macro ------------------------------------------------------------*/
#define SLOT_INTERVAL_MSG_IN_BURST  75
#define SLOT_INTERVAL_SEND_TO_TX    10
#define SLOT_INTERVAL_GPS_ON_TO_TX  450     //12s
#define SLOT_INTERVAL_MCU_WAKEUP_TO_TX  375 //10s

#define TIME_OFFSET_SEND_TO_TX      10      //lq 10slots
#define TIME_OFFSET_GPSON_TO_TX     750     //lq 563slots = 20s（15s无法完成同步）

#define COMM_STAT_SLOT_OFFSET_CEIL  2475
#define COMM_STAT_SLOT_OFFSET_FLOOR 2025

/*lq 默认消息周期和条数*/
#define ITDMA_MSG_SEND_INTERVAL_DEF_VAL     3       //lq 默认发送间隔3min
#define ITDMA_MSG_SEND_TIME_DEF_VAL         2       //lq 默认发送条数2

/*lq ITDMA Communication State*/
#define ITDMA_COM_STA_SLOT_INCRE_DEF_VAL    2250  //ITDMA通信状态 时隙增量默认值

/* Public  variables ---------------------------------------------------------*/
extern SOTDMAParaStruct struSOTDMAPara;
extern ICommStateStruct struICommState;

/* Public  function  ---------------------------------------------------------*/
extern void InitCommStateSOTDMA(void);
extern void InitSOTDMAPara(void);
extern U32 GetBurstSendTim(void);
extern U16 GetRandSlotOffset(S16 PeriodSlots);
extern void SetBurstTxTim(S32 usValue);
extern void SetMsgTxTim(S16 usValue);
extern void SetBurstSendTim(S16 usValue);
extern void SetMsgSendTim(S16 usValue);
extern void SetCommState(void);
extern U16 GetBurstTxTimSlot(void);
extern void SetCommStaSlotNum(S16 ssSlotNum);
extern void SetCommStaSubMsg(S8 ucTimeout);
extern void SetCommStaUTCHourMin(void);
extern void SetCommStaSlotOffset(S16 ssSlotOffset);
extern U16 GetCommStaSlotOffset(void);
extern U8 GetBurstFlg(void);


#endif

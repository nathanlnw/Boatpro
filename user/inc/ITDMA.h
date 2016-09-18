/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: ITDMA.h
  Author: lq       Version : V1.00        Date: 2012.11.30
  Description:     ITDMA接入方式消息发送控制模块宏、结构体定义，变量、函数声明
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _ITDMA_H
#define _ITDMA_H

/* Includes ------------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef struct ITDMAPara
{
    U8  ucSyncState;                                                            // 同步状态                                    
    U16 usSlotIncrement;                                                        // 时隙增量
    U8  ucSlotNumbers;                                                          // 时隙数
    U8  ucKeepFlag;                                                             // 保持标记

}ITDMAParaStruct;

typedef enum  ReportInterval
{
   ReportIntervalSlot_3min = 6750,
   ReportIntervalSlot_30s = 1125,
   ReportIntervalSlot_15s = 562,                                                //lq 562.5slots
   ReportIntervalSlot_5s = 187,                                                 //lq 187.5slots
   ReportIntervalSlot_1min = 2250,
   ReportIntervalSlot_6min = 13500

}ReportIntervalEnum;

/* Private macro ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function  ---------------------------------------------------------*/
/* Public  macro ------------------------------------------------------------*/
#define TMO_MIN 3
#define TMO_MAX 7

#define RR_MIN  2
#define RR_MAX  30

#define NI_MIN  75
#define NI_MAX  1225

#define ITINC_MIN 0
#define ITINC_MAX 8191

#define TIME_OFFSET_SEND_TO_TX      10      //lq 10slots
#define TIME_OFFSET_GPSON_TO_TX     750     //lq 563slots = 20s（15s无法完成同步）

/* Public  variables ---------------------------------------------------------*/
extern ITDMAParaStruct gstruITDMAPara;

extern ReportIntervalEnum genSendInterval;                                      //lq 正常模式Msg18的发送间隔
extern ReportIntervalEnum genSendIntervalDyn;                                   //lq 随GPS报文的更新而变化
extern U32 gusMsg18XmtSlotCnt_A;                                                //lq 信道A消息18发射定时
extern U32 gusMsg18XmtSlotCnt_B;                                                //lq 信道B消息18发射定时
extern U32 gusMsg18SendSlotCnt_A;                                               //lq 信道A消息18发送定时
extern U32 gusMsg18SendSlotCnt_B;                                               //lq 信道B消息18发送定时

// extern U32 gusChannelASlotCnt;                                               //lq 信道A消息18发送定时
// extern U32 gusChannelBSlotCnt;                                               //lq 信道B消息18发送定时
extern U32 gusMsg19XmtSlotCnt;
extern U32 gusMsg19SendSlotCnt;                                                 //lq 消息19发送定时
extern U8  gucMsg19SendFlg;
extern U32 gulMsg19XmtSlotCnt_Timeout;
extern U32 gusMsg14XmtSlotCnt;
extern U32 gulGpsOnSlotCnt_A;
extern U32 gulGpsOnSlotCnt_B;
/* Public  function  ---------------------------------------------------------*/
extern void InitCommStateITDMA(void);
extern void PreemptionSlotMsg18(ReportIntervalEnum usSendIntervalNew);
extern void CalculateSendInterval(U16 speed);
extern void CalculateSendInterval_2(U16 speed);
extern U8 UpdateSendInterval(void);
extern void UpdateCommStateITDMA(U8 inFlg);
extern void PreemptionSlotMsg19(void);
extern void SetMsg19SendFlg(void);
extern U8 IsPreemptionSlotMsg19(void);
extern U8 GetMsg19SendFlg(void);

extern void SetMsg18XmtTimeCnt(U8 inCHANNEL, U16 inTimeInterval);
extern void ResetMsg18XmtTimeCnt(U8 inCHANNEL);
extern void SetMsg18SendTimeCnt(U8 inCHANNEL);
extern void SetMsg19XmtTimeCnt(U16 inTimeInterval);
extern void SetMsg19SendTimeCnt(void);
extern void SetMsg14XmtTimeCnt(U32 inTimeCnt);
extern void SetGpsOnTimeCnt(U8 inCHANNEL);
extern void SetGpsOffEn(void);
extern void ResetGpsOffEn(void);
extern U8 GetGpsOffEn(void);
extern void SetGpsOffOk(void);
extern void ResetGpsOffOk(void);
extern U8 GetGpsOffOk(void);
extern void SetUpdateSendIntervalEn(void);
extern void ResetUpdateSendIntervalEn(void);
extern U8 GetUpdateSendIntervalEn(void);
#endif

/*******************************************************************************
  Copyright (C), 2012, 712.Co.,Ltd.
  FileName: adjust.h
  Author: 郭长勇      Version : V1.00        Date: 2012.07.28
  Description:    时间自动校对模块
  Version:         V1.00
  Function List:   // 主要函数及其功能
  History:         // 历史修改记录
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#ifndef _ADJUST_
#define _ADJUST_

#include "def.h"

#define ADJUST_MAX_DIFF_VALUE	(20000)//两次秒脉冲计数可能的最大偏差
#define ADJUST_MAX_WAIT_PLUS_VALUE (96000)//最多等待2秒的GPS脉冲，超过认为失败
#define ADJUST_CALC_PERIOD		(60)//统计的次数
#define ADJUST_PHASE_MAX_ADJUST	(100)//最大调整值 

#define ADJUST_TIM_CNT_1S_CEIL      (48000 + 1000)
#define ADJUST_TIM_CNT_1S_FLOOR     (48000 - 1000)

#define ADJUST_RTC_LSE_CNT_1S_CEIL      (512 + 100)
#define ADJUST_RTC_LSE_CNT_1S_FLOOR     (512 - 100)

#define ADJUST_RTC_LSI_CNT_1S_CEIL      (500 + 100)
#define ADJUST_RTC_LSI_CNT_1S_FLOOR     (500 - 100)

#define ADJUST_CTRL_STA_IDEL		(0)		//空闲状态
#define ADJUST_CTRL_STA_FRISTFLAG 	(1)		//等待第一个脉冲
#define ADJUST_CTRL_STA_COUNT 		(2)		//统计阶段
#define ADJUST_CTRL_STA_CALC		(3)		//计算阶段

#define ADJUST_RTC_SOURCE_LSI		(0)		//rtc时钟源为LSI
#define ADJUST_RTC_SOURCE_LSE		(1)		//rtc时钟源为LSE
#define ADJUST_WAIT_LSI_TIME		(400)	//等待开机后2秒选择RTC时钟源
	 										//此开机等待时间应设置小于经验GPS开机定位时间
typedef struct{
	U8 AdjustCtrlState;//状态机
	U32 TimPastCount;//Tim过去的计数
	U32 RtcPastCount;//Rtc过去的计数
	//U32 NowCount;//现在的计数
	S32 TimDiffCount;//Tim计数偏差
	S32 RtcDiffCount;//Rtc计数偏差
	S32 TimDiffSum;//Tim偏差合
	S32 RtcDiffSum;//Rtc偏差合
	U16 PeriodCount;//周期计数
	U8	CalcNowFlag;//立刻计算flag

    U32 TimCntLast;                                                                 //lq 上次保存的本地Tim的计数值（未校准）
    U32 RtcCntLast;                                                                 //lq 上次保存的本地Rtc的计数值（未校准）
    S32 TimCntSec1;                                                                 //lq UTC1s间隔内本地Tim的计数值
    S32 RtcCntSec1;                                                                 //lq UTC1s间隔内本地Rtc的计数值
    S32 TimCntSec1Sum;
    S32 RtcCntSec1Sum;

	U16 PeriodCountLast;//周期计数
	//rtc时钟选择相关
	U32 TimeCnt;
	U16 TimePeriod;	
	U8 	RtcClockSource;//时钟源
	U8	RtcClockChoice;//时钟选择flag  是否完成时钟选择

}TimeAdjustStruct;

extern TimeAdjustStruct TimAdjustPara;

extern void InitTimAdjust(void);
extern void ResetTimAdjust(void);
extern void ResetTimAdjustCtrlState(void);
extern void TimAdjustProcess(void);

extern void SetAdjustCalcNow(void);
extern U8 GetAdjustSleepState(void);

extern U8 IsRtcClockSourceLSE(void);

extern void RtcClockChoice(void);

#endif

/*******************************************************************************
  Copyright (C),  2012,  712.Co., Ltd.
  File name:	Tim.h
  Author: vozhj      Version: V1.00       Date: 2012.03.21 
  Description:  
  				主要功能： 
				1. 定义相关宏定义
				2. 变量声明
				3. 函数声明
				4. 定义结构体类型
				
  Others:         // 其它内容的说明
  Function List:  // 主要函数列表，每条记录应包括函数名及功能简要说明
  History:        // 修改历史记录列表，每条修改记录应包括修改日期、修改
                  // 者及修改内容简述  
    1. Date:
       Author:
       Modification:
*******************************************************************************/

#ifndef __TIM_H
#define __TIM_H

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
#define TIM_ADJUST_1S (48000)//定时器1秒
#define TIM_ADJUST_10S (480000)//定时器10秒
#define TIM_ADJUST_100S (4800000)//定时器100秒
extern S16 gsiPhaseCnt;
extern S16 gsiSlotCnt;
extern S32 gslFrameCnt;
extern U32 GpsTimCount;
extern U32 TimCnt;                                                                     //lq 本地Tim计数（未校准）                                                                
extern U32 TimCnt1s;                                                                   //lq 本地Tim在UTC1s间隔内的计数值
extern U32 TimCnt10s;                                                                  //lq 本地Tim在UTC10s间隔内的计数值
extern U32 TimCnt100s;                                                                 //lq 本地Tim在UTC100s间隔内的计数值
extern S16 TimAdjust1;//定时器每2秒调整phase数
extern S16 TimAdjust10;//定时器每10秒调整phase数
extern S16 TimAdjust100;//定时器每50秒调整phase数
extern U8 ucTestCnt;

extern U8  gucFSKDataXmtEn;
/* Public  function  ---------------------------------------------------------*/ 
extern void InitTim2(void);
extern void InitTim2Para_2(void);
extern void Tim2Interrupt(void);
extern S16 GetPhaseNum(void);
extern S16 GetSlotNum(void);
extern S32 GetFrameNum(void);
extern S32 GetSlotSum(void);
extern S32 SetSlotTimer(U16 inTimeInterval);
extern void SetTimAdjust1(S16 data);
extern void SetTimAdjust10(S16 data);
extern void SetTimAdjust100(S16 data);
extern void ResetTimeCount(void);
extern void SetFSKDataXmtEn(void);
extern void ResetFSKDataXmtEn(void);
extern U8 GetFSKDataXmtEn(void);
extern U8 GetFSKDataXmtSleepState(void);
extern void SetFetchFirstFskDataEn(void);
extern void ResetFetchFirstFskDataEn(void);
extern U8 GetFetchFirstFskDataEn(void);
extern void TransPhaseAdjust(S16 * const pAdjust1, S16 * const pAdjust10, S16 * const pAdjust100);
#endif

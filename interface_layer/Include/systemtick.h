/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	systemtick.h
  Author: 李斌      Version: V1.00       Date: 2012.03.22 
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
#ifndef _SYSTICK
#define _SYSTICK
#define LOCAL_CLOCK_MAX_PEIROD	(48000)
struct LocalClockTickStruct
{
	U8  State;
	U8  TimeIntFlg; 
	U16 TimeCnt;
	U16 TimePeriod;
	U8  Time5msCnt;	
};
extern struct LocalClockTickStruct LocalClockTick;
void LocalClockCtrlMainProcess(void);
U16 GetLocalClockTickProcess(void);
void LocalClockStateCtrlProcess(void);
U32 SetTimePeriodProcess(U32 Period);
void SystemTickKeyScan(void);
extern U32 testTimval;
#endif 

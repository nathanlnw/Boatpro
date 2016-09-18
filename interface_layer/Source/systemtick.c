/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: interfunc.c
  Author: 李斌      Version : V1.00        Date: 2012.03.21
  Description:    系统时钟
  Version:         V1.00
  Function List:   // 主要函数及其功能
  History:         // 历史修改记录
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
//#include "stm32f10x.h"
#include "def.h"
#include "systemtick.h"
//#include "Tim.h"
//#include "keyscan.h"

/* public  variables ---------------------------------------------------------*/
struct LocalClockTickStruct LocalClockTick;
U32 testTimval = 0; //定时计数器

/*******************************************************************************
* 名称  : init_systermtick
* 描述  : 初始化定时参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void init_systermtick(void)
{
	//初始化定时参数
	LocalClockStateCtrlProcess();			//关闭或打开本地时钟模块
}

/*******************************************************************************
* 名称  : GetLocalClockTickProcess
* 描述  : 获取当前本地时钟节拍处理函数(应用接口函数)
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U16 GetLocalClockTickProcess(void)
{		
	return (LocalClockTick.TimeCnt);
}

/*******************************************************************************
* 名称  : SetTimePeriodProcess
* 描述  : 设置定时时间处理(应用接口函数)
* 输入  : Period：定时周期
* 输出  : 无
* 返回  : 定时时间
*******************************************************************************/
U32 SetTimePeriodProcess(U32 Period)
{
	U32 TimePeirod;
	U32 TimeCnt;

	if (Period >= LOCAL_CLOCK_MAX_PEIROD)
	{
		Period = LOCAL_CLOCK_MAX_PEIROD - 1;
	}
	else if (Period == 0)
	{
		Period = 1;
	}
	
	TimePeirod = GetLocalClockTickProcess() + Period;
	if (TimePeirod >= LOCAL_CLOCK_MAX_PEIROD)
	{
		TimeCnt = TimePeirod - LOCAL_CLOCK_MAX_PEIROD;
	}
	else
	{
		TimeCnt = TimePeirod;
	}	
	
	return TimeCnt;
}

/*******************************************************************************
* 名称  : LocalClockStateCtrlProcess
* 描述  : 本地时钟状态控制处理(应用接口函数)
* 输入  : State：
			FALSE：关闭
			TRUE：开启
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void LocalClockStateCtrlProcess(void)
{	
	//5ms精度定时器
	LocalClockTick.TimeIntFlg = FALSE;
	LocalClockTick.TimeCnt = 0;
	LocalClockTick.TimePeriod = LOCAL_CLOCK_MAX_PEIROD;
	LocalClockTick.Time5msCnt = 0;
}

/*******************************************************************************
* 名称  : LocalClockCtrlMainProcess
* 描述  : 本地时钟控制主程序处理
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void LocalClockCtrlMainProcess(void)
{					
	if (LocalClockTick.TimeIntFlg == TRUE)                                      //zjw 在Time.c中有定义说明//lq 每5ms，本地时钟计数一次
	{
		LocalClockTick.TimeIntFlg = FALSE;
		
		LocalClockTick.TimeCnt++;                                               //lq 5ms计时
	    if (LocalClockTick.TimeCnt >= LOCAL_CLOCK_MAX_PEIROD)                   //lq LOCAL_CLOCK_MAX_PEIROD=4min
	    {                                                                       //lq 可以通过定时计数的方式进行任意时长的定时
	    	LocalClockTick.TimeCnt = 0;                                         //lq （定时到后马上进行下一次的定时，并使定时计数加1）
	    }					    
	}
}


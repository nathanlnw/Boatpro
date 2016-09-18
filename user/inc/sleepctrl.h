/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	sleepctrl.h
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
#ifndef _SLEEPCTRL_H
#define _SLEEPCTRL_H
/* Private define ------------------------------------------------------------*/
#define SLEEP_POWERSTA_POWERUP		0
#define SLEEP_POWERSTA_NORMAL		1

#define SLEEP_PRT_PERIOD			2		//10ms周期
#define SLEEP_1S_PRTTIME			100		//1s进入省电保护时间

#define SLEEP_USART_FLAG_CLEAR_PERIOD   200     //lq 单位 5ms

#define SLEEP_STA_IDLE				0				
#define SLEEP_STA_STEP1				1				
#define SLEEP_STA_STEP2				2				
#define SLEEP_STA_STEP3				3

#define SLEEP_SLEEP_TIMER			5		//省电时间，5min
#define SLEEP_WAKEUP_TIMER			1		//正常工作时间，1min				

/* Private typedef -----------------------------------------------------------*/
typedef struct {
	U8  PowerSta;
	
	S16 EnterSleepSlot;		//进入省电时隙
	S32 EnterSleepFrame;	//进入省电帧

	S16 WakeUpSlot;		   //退出省电时隙
	S32 WakeUpFrame;	   //退出省电帧
	
	U32 TimeCnt;
	U16 TimePeriod;

    U32 TimeCnt1;
	U16 TimePeriod1;

	U16 EnterSleepPrtTime;	//进入省电保护时间

	U8  SleepCtrlSta;		//省电管理状态机

    U8  ucMcuStopFlg;
    U8  ucMcuStandbyFlg;
    U8  ucGpsPulseFlg;      //lq GPS秒脉冲唤醒
    U8  ucMcuWakeupFlg;     //lq MCU唤醒标志
    U8  ucUsartFlg;        //lq USART1中断唤醒标志
    U8  ucStandbyAlarmSetFlg;//lq待机唤醒定时设置
    U8  ucStandbyAlarmSetOk;//lq 待机唤醒定时设置完成
    U8  ucTimToRtcSwitch;   //lq 相位计数方式切换
    U8  ucRtcToTimSwitch;
}SleepStruct;

extern U8 set_sleep_time_flag;//可以设置休眠时间
extern SleepStruct		SleepCtrlPara;
extern S32 gslTimToRtcCnt_2;                                                                
extern S32 gslRtcToTimCnt_2;
extern S32 gslTimToRtcIdleCnt_2; 
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/
extern void IniSleepCtrlPara(void);
/**
	@ingroup 休眠
	@brief 正常模式进入休眠控制函数

	当各个模块处于空闲状态，则函数控制系统进入休眠，并设定唤醒时间等
*/
extern void SleepCtrlHigFreq(void);
/**
	@ingroup 休眠
	@brief 休眠模式唤醒控制函数

	当休眠时间到或者按键时，系统唤醒进入正常模式
*/
extern void SleepCtrlLowFreq(void);
extern U8 IsMcuWakeupTimeout(void);
extern S32 GetMcuWakeupTim(void);
extern void SetMcuWakeupSlot(S16 value);
extern void SetMcuWakeupFrame(S32 value);
extern void SetMcuWakeupTim(S16 usValue);
extern void SaveBKP_DRPara(void);
extern U32 CalcuSleepSpan(void);
extern S32 CalcuSleepSlots_Sleep(void);
extern S32 CalcuSleepPhases_Sleep(void);
extern void ResetUsartFlg(void);
extern U8 GetUsartFlgRst(void);
extern U8 GetUsartFlg(void);
#endif

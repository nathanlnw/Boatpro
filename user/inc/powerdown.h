/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	powerdown.h
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
#ifndef _POWERDOWN_H
#define _POWERDOWN_H
/* Private typedef -----------------------------------------------------------*/

/**
	@ingroup MMI
	@brief 关机控制结构体
*/
typedef struct {
	U16 TimeCnt;
	U16 TimePeriod;	
    U16 TimeCnt1min;
    U16 TimePeriod1min;
    U16 TimeCntMin;

	U16 XDouCnt;
	U8 KeyVal;
	U8 KeyValPast;
	
	U8 Sta;

	U16 ShutDownTime;
} PowerDownStruct;
/* Private define ------------------------------------------------------------*/

#define POWERDOWN_STA_IDLE		0
#define POWERDOWN_STA_STEP1		1
#define POWERDOWN_STA_STEP2		2
//#define POWERDOWN_STA_STEP3		3

#define POWERDOWN_TIME_1min_PERIOD       12000
#define POWERDOWN_TIME_Min_PERIOD       1
#define POWERDOWN_POWERUP_PEROID	10		//开机保护时间，用于检测按键抬起，50ms定时
#define POWERDOWN_NORMAL_PEROID		1		//正常工作状态下检测关键按键周期，5ms定时
#define POWERDOWN_SHUTDOWN_TIME		600		//5ms定时，3s计数
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/
extern void IniPowerDownPara(void);
/**
	@ingroup MMI
	@brief 关机函数
	@param 无
	@retval 无

	检测POWER长按3秒进入关机过程
*/
extern void PowerDownProcess(void);

/**
	@ingroup 休眠
	@brief 查看关机模块是否忙
	@param 无
	@retval TRUE 关机模块空闲
	@retval FALSE 关机模块忙
*/
extern U8 GetPowerDownSleepSta(void);
extern void ExecShutdownAction(void);

#endif

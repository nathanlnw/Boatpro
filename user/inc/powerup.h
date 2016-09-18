/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	powerup.h
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
#ifndef _POWERUP_H
#define _POWERUP_H
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
#define VERSION		"V1.0.0"

/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/
#define KEY_ENABLE_VALUE (300)//3秒 10ms检测一次
extern void InitPowerUppara(void);

/**
   @ingroup MMI
   @brief 开机函数
   @param 无
   @retval 无

   两种开机模式，长按POWER和长按SOS
*/
extern void PowerUpProcess(void);
extern void InitBKP_DRPara(void);
extern void SleepWakeUpInit(void);
extern U32 CalcuSleepSlots_Wakeup(const U32 ulAlarmValue);
extern U8 ucStandbyWakeupFlg;
extern U8 ucStandbyWakeupDelayFlg;
//extern void PowerUpLedCtrl(void);
#endif

/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	mmi.h
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
#ifndef _MMI_H
#define _MMI_H
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#define MMI_CTRLSTA_IDLE	0	//状态机IDLE状态
#define MMI_CTRLSTA_STEP1	1	//状态机IDLE状态
#define MMI_CTRLSTA_STEP2	2	//状态机IDLE状态

#define MMI_STA_NONE		0	//MMI为空闲状态
#define MMI_STA_BUSY		1	//MMI为忙状态

#define MMI_100SECPEROID	20	//100ms定时器
#define MMI_3SECCNT			30	//3s计数器

#define MSG_CTRL_STA_POWERUP (0)//初始开机
#define MSG_CTRL_STA_SEND (1)//发送
#define MSG_CTRL_STA_IDLE (2)//空闲
#define MSG_CTRL_STA_WAIT_SEND (3)//等待定位

#define MSG_CTRL_WAITNUM (20)//等待20个循环

/**
	@ingroup MMI
	@brief SOS告警检测结构体
*/
typedef struct {
	U32 TimeCnt;
	U16 TimePeriod;	
	U16 Time100msCnt;	//100ms计时器

	U8  MMICtrlSta;		//MMI状态机
	U8  MMISta;			//MMI状态

	U8 	SOSConfirm;//确认SOS告警

	U16 MMIKeyValue;//SOS告警响应时间阈值
}MMIStruct;

typedef struct{
	U8 MsgCtrlState;
	S16 GpsStartFrame;
	S16 GpsStartSlot;
    S32 slGpsOnSlot;                                                            //lq GPS开启定时    
	U8 GpsPowerup;
	U8 EnterHighFreq;
	U16 WaitCount;//由于外部循环太快，等待下模式切换几个循环再执行操作
}MsgCtrlStruct;

/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
extern MsgCtrlStruct MsgCtrlPara;

/* Public  function  ---------------------------------------------------------*/

extern void IniMMIPara(void);
/**
	@ingroup MMI
	@brief SOS告警检测
	@param 无
	@retval 无

	长按SOS三秒触发SOS告警
	该告警在协议栈每分钟只会响应一次
*/
extern void MMIProcess(void);

/**
	@ingroup 休眠
	@brief 查询是否SOS按键按下状态
	@param 无
	@retval TRUE SOS按键空闲
	@retval FALSE SOS按键忙
*/
extern U8 GetMMISleepSta(void);

/**
	@ingroup MMI
	@brief 查询是否SOS告警函数
	@param 无
	@retval TRUE SOS告警
	@retval FALSE SOS不告警

	告警状态在查询第一次返回TRUE，再次查询将置为FALSE
*/
extern U8 GetMMISOSState(void);

extern void InitMsgCtrlPara(void);
extern U8 MsgSendErrorCheck(S16 Slot,S16 Frame);
extern void MsgCtrlProcess(void);
extern U8 GetMsgCtrlSleepState(void);
extern void MsgStartNew(void);
extern void ResetMsgGpsPowerup(void);
extern void SetMsgGpsPowerup(void);

extern U8 GetMsgGpsPowerup(void);
extern U8 GetMsgWakeupState(void);

#endif

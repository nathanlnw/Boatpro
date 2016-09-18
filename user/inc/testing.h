/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	interfunc.h
  Author: 李斌      Version: V1.00       Date: 2012.03.21 
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
#ifndef _TESTING_H
#define _TESTING_H

/* Private define ------------------------------------------------------------*/
#define TEST_200MS_PERIOD	40		//200ms定时

#define TEST_KEYSTA_IDLE	0		//按键扫描状态机
#define TEST_KEYSTA_STEP1	1		//按键扫描状态机
#define TEST_KEYSTA_STEP2	2		//按键扫描状态机
#define TEST_KEYSTA_STEP3	3		//按键扫描状态机

#define TEST_RFXMT_IDLE		0		//发射状态机
#define TEST_RFXMT_STEP1	1		//发射状态机
#define TEST_RFXMT_STEP2	2		//发射状态机
#define TEST_RFXMT_STEP3	3		//发射状态机

#define TEST_XMTBUF_LEN		100

#define TEST_XMTTYPE_CARRIER	0
#define TEST_XMTTYPE_MOD		1
#define TEST_XMTTYPE_SIG2		2
#define TEST_XMTTYPE_T2			3
#define TEST_XMTTYPE_T3			4
#define TEST_XMTTYPE_STSIGNAL2	5   //SART 标准测试信号2：00001111
/* Private typedef -----------------------------------------------------------*/

/**
	@ingroup DEBUG
	@brief 测试模式控制结构体
*/
typedef struct {
	U32 TimeCnt;
	U16 TimePeriod;			//200ms定时

	U8  KeyScanSta;			//按键扫描状态机
	U8  LEDMsgFlag;			//向LED控制模块传递消息标志

	U8  ucTestNum;			//测试项

	U8  ucRFXmtSta;
	U8  ucXmtEn;			//数据发送使能
	U8  ucXmtData;			//待调制数据
	U8  ucXmtType;			
	U8  ucXmtBuf[TEST_XMTBUF_LEN];		//待发送数据缓冲
	U16 uiXmtBufLen;

	U8 FirstGenFlag;

    U8  FirstEnterTestFlag; //lq 第一次进入测试态
} TestCtrlStruct;

//电池电量检测控制结构体
#define BATTERY_TEST_CLOCK_INTERVAL 75

#define BATTERY_TEST_STATE_POWERUP 		0//开机状态
#define BATTERY_TEST_STATE_HIGHFREQ		1//高频模式
#define BATTERY_TEST_STATE_LOWFREQ		2//低频模式
#define BATTERY_TEST_STATE_WAIT			3//等待模式

#define BATTERY_TEST_MIN_COUNT 			30//一分钟  == 30*2 s

#define BATTERY_TEST_GPS_HOT_POWERUP	5//GPS热启动时间10秒

#define BATTERY_TEST_WAIT_STEP1			0
#define BATTERY_TEST_WAIT_STEP2			1
typedef struct{
	//定时器相关
	U32 ClockCount;
	U32 ClockSlotPast;//用于根据slot定时
	
	U32 TimeCntGps;
	U32 TimeCntMessage;
	U32 TimeCntWait;
	U16 TimePeriod;	

	U8 CtrlState;//状态机

	U8 GpsPowerOn;

	S32 FrameCntPast;

	//U8 WaitForSleep;
	//U8 WaitState;
	//U16 WaitCount;
}BatteryTestCtrlStruct;

///////////////////////////
//消息部分控制

#define MSG_TEST_STATE_POWERUP 	0
#define MSG_TEST_STATE_WAIT 	1
#define MSG_TEST_STATE_GPS		2
#define MSG_TEST_STATE_SEND		3

typedef struct{
	U8 CtrlState;
	U16 TimePeriod; 
	U32 SendIntervalTimeCnt;
	U32	GpsPoweronTimeCnt;
	U32 GpsProtectTimeCnt;
	U8 	WakeupFlag;
	U8 GpsPowerOn;
	U8 GpsProtect;
	S32 FrameCntPast;
}MsgTestCtrlStruct;

//////////////////////////////////
//休眠控制部分
#define SLEEP_TEST_STATE_IDEL	0
#define SLEEP_TEST_STATE_STEP1	1

typedef struct{
	U8 CtrlState;
	U16 TimePeriod; 
	U32 TimeCnt;
	U32 WaitCount;
}SleepTestCtrlStruct;

typedef struct MsgTestStruct{
    U8 ucPowerDownEn;
}MsgTestStruct;

typedef struct RxTestStruct
{
    U8 RxTestEn;
}RxTestStruct;

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
#define TEST_TESTSTA_IDLE		0
#define TEST_TESTSTA_CARRIER	1
#define TEST_TESTSTA_MOD		2
#define TEST_TESTSTA_FREQADJEN	3
#define TEST_TESTSTA_MODADJEN	4
#define TEST_TESTSTA_FREQADJ	5
#define TEST_TESTSTA_MODADJ		6
#define TEST_TESTSTA_SIG2		7
#define TEST_TESTSTA_T2			8
#define TEST_TESTSTA_T3			9
#define TEST_TESTSTA_STSIGNAL_1 10
#define TEST_TESTSTA_STSIGNAL_2 11
#define TEST_TESTSTA_STSIGNAL_3 12
#define TEST_TESTSTA_GRIDADJEN  13
#define TEST_TESTSTA_GRIDADJ	14
#define TEST_TESTSTA_STSIGNAL2	15       //SART标准测试信号2：00001111

/* Public  variables ---------------------------------------------------------*/

extern TestCtrlStruct TestCtrlPara;
extern MsgTestStruct struMsgTestPara;
extern RxTestStruct RxTestInstance;

/* Public  function  ---------------------------------------------------------*/
extern void IniTestingPara(void);
extern void IniTesting(void);

/**
	@ingroup DEBUG
	@brief 测试模式主函数
	@param 无
	@retval 无
*/
extern void Testing(void);

/**
	@ingroup DEBUG
	@brief 测试模式按键扫描
	@param 无
	@retval 无

	测试模式下按SOS可切换信号发送模式
*/
extern void Testing_KeyScan(void);

/**
	@ingroup DEBUG
	@brief 测试模式主函数
	@param ucType 测试模式
	@param ulFreq 发送信号频率
	@retval TRUE 设置测试模式成功
	@retval FALSE 设置测试模式失败
*/
extern U8 Testing_SetTestType(U8 ucType, U32 ulFreq);

/**
	@ingroup DEBUG
	@brief 产生GMSK调制后的测试信号
	@param 无
	@retval 无

	根据不同的测试模式产生不同的调制后的测试信号
*/
extern void GMSKTest(void);

/**
	@ingroup DEBUG
	@brief 测试模式下定时产生测试信号
	@param 无
	@retval 无

	每200ms产生一次测试信号
*/
extern void Testing_TestMain(void);
extern void ResetTesting(void);

extern TestCtrlStruct TestCtrlPara;
extern void Testing_TestMain(void);

extern void BatteryTestCtrlLowFreq(void);
extern void InitBatteryTest(void);
//extern void BatteryTestCtrl(void);
extern void BatteryTestSendMsg(void);
extern void BattertTestClock(void);
extern U32 SetBatteryTestTimer(U32 period);
extern U32 GetBattertTestTick(void);

extern U8 GetMsgTestCtrlWakeupFlag(void);
extern void InitMsgTestCtrl(void);
extern void InitSleepTestCtrl(void);
extern void MsgTestCtrl(void);
extern void SleepTestCtrl(void);
extern void InitMsgTest(void);
#endif

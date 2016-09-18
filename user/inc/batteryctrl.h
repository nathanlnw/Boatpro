/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	batteryctrl.h
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
#ifndef _BATTERYCTRL_H
#define _BATTERYCTRL_H

/* Private define ------------------------------------------------------------*/
#define BATTERY_AVGBUF_LEN		15
#define BATTERY_20MS_PERIOD		4

#define BATTERY_STA_IDLE		0
#define BATTERY_STA_STEP1		1
#define BATTERY_STA_STEP2		2
#define BATTERY_STA_STEP3		3
#define BATTERY_STA_STEP4		4

#define BATTERY_LOW_THRESHOLD		1896		//6.8V电压设置
#define BATTERY_ULTRALOW_THRESHOLD	1723		//6.2V电压设置

#define BATTERY_BELOW_CNT			500        // （单位20ms）	

#define BATTERY_SELFCHECK_TIME_PERIOD               100     //lq unit:5ms
#define BATTERY_SELFCHECK_TIME_TIMEOUT              5       //lq unit:500ms
#define BATTERY_SELFCHECK_VOLT_CEIL                 740     //lq unit:0.01V 使用串口供电比使用电源插座供电低0.2V
#define BATTERY_SELFCHECK_VOLT_FLOOR                700     //lq unit:0.01V 误差为0.2V
#define BATTERY_SELFCHECK_VALID_CNT_THRESH          5       //lq unit:1

/* Private typedef -----------------------------------------------------------*/

/**
	@ingroup 电池
	@brief 电池电量检测结构体，包含电池电压平均等
*/
typedef struct {
	U32 TimeCnt;
	U16 TimePeriod;	
#if 0	
	U16 AvgBuf[BATTERY_AVGBUF_LEN + 1];	//平滑滤波器
	U8  AvgBufIndex;
	U32 AvgSum;
#endif
	U8 BatterySta;			//状态机状态

    U16 ucAboveCnt;                                                              //lq 由低或超低电量进入正常门限的次数
	U16 BellowCnt;			//进入到门限以下的次数
	U8 SaveCloseTime;

	U16 BatteryLowLevel;//电池低电压门限
	U16 BatteryUltraLowLevel;//电池超低电压门限
} BatteryCtrlStruct;

typedef struct BatterySelfCheckStruct
{
	U32 TimeCnt;
	U16 TimePeriod;	
    U16 TimeoutCnt;

    U8  ValidCnt;
    U8  StartFlg;
    U8  CompleteFlg;
    U8  SupplyVoltOkFlg;
}BatterySelfCheckStruct;

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
extern BatteryCtrlStruct BatteryCtrlPara;

/* Public  function  ---------------------------------------------------------*/
extern void InitBatteryCtrlPara(void);
/**
	@ingroup 电池
	@brief 电池电量检测
*/
extern U8 BatteryCtrlProcess(void);
#endif

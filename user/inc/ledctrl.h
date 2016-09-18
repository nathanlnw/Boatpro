/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	ledctrl.h
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
#ifndef _LEDCTRL_H
#define _LEDCTRL_H
/* Private typedef -----------------------------------------------------------*/

/**
	@ingroup MMI
	@brief LED结构体，包含LED计数器，优先级等
*/
typedef struct {
	U8  *pLEDGreen;			//LED绿灯控制指针
	U8  *pLEDRed;			//LED红灯控制指针
	U8  *pLEDTimer;			//LED控制定时器
	U8  LEDReCycleFlag;		//循环标志
	U8  LEDPriority;		//优先级	
	U8	LEDEn;				//使能标志
	U8  ucType;				//类型
	U8  ucBufLen;			//缓冲长度
	U8  ucLedFragmentTimer;	//子计数器，用于计数没段的时间
	U8  ucLedFragment;		//计数器，用于计数段的数目
}LEDStruct;

/**
	@ingroup MMI
	@brief LED控制结构体，用于控制两个LED
*/
typedef struct {
	U32 TimeCnt;
	U16 TimePeriod;			//用于250ms定时

	LEDStruct	LedGreenPara;	//单色LED1(Green)
	LEDStruct	LedYellowPara;	//单色LED2(Yellow)
	LEDStruct	LedRedPara;	//单色LED3(Red)
	LEDStruct	LedBluePara;	//单色LED4(Blue)
}LEDCtrlStruct;

/* Private define ------------------------------------------------------------*/
/*lq 以下定义LED控制优先级，0为最高优先级，3为最低优先级，优先级比较只限于同一个LED */
#define LED_CTRL_PRIORITY0		0
#define LED_CTRL_PRIORITY1		1
#define LED_CTRL_PRIORITY2		2
#define LED_CTRL_PRIORITY3		3
#define LED_CTRL_PRIORITY_H		(LED_CTRL_PRIORITY0)        //lq 最高优先级
#define LED_CTRL_PRIORITY_L		(LED_CTRL_PRIORITY3)        //lq 最低优先级

#define LED_CTRL_BUFF_DEPTH     12
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/*lq LED控制类型定义 */
#define LED_TYPE_NONE			        0
#define LED_TYPE_GPS_SEARCH		        1
#define LED_TYPE_GPS_FIX		        2
#define LED_TYPE_RF_XMT			        3
#define LED_TYPE_POWER_NORMAL	        4
#define LED_TYPE_POWER_TEST	            5
#define LED_TYPE_TEST_TEST1	            6
#define LED_TYPE_TEST_TEST2		        7
#define LED_TYPE_TEST_TEST3		        8
#define LED_TYPE_TEST_TEST4		        9
#define LED_TYPE_TEST_RX		        10
#define LED_TYPE_SELF_CHECK_IDLE        11
#define LED_TYPE_SELF_CHECK_RUNNING     12
#define LED_TYPE_SELF_CHECK_OK          13
#define LED_TYPE_SELF_CHECK_ERR         14
#define LED_TYPE_TIMEOUT                15
#define LED_TYPE_ERR                    16
#define LED_TYPE_GREEN_NONE			    17
#define LED_TYPE_YELLOW_NONE			18
#define LED_TYPE_RED_NONE			    19
#define LED_TYPE_BLUE_NONE			    20

/*lq LED控制循环定义 */
#define LED_RECYCLE_NONE		FALSE
#define LED_RECYCLE				TRUE

/* Public  variables ---------------------------------------------------------*/
extern LEDCtrlStruct LEDCtrlPara;
/* Public  function  ---------------------------------------------------------*/
/**
	@ingroup MMI
	@param ucType LED闪烁类型
	@retval 无
	@brief LED控制接口函数，指定LED闪烁规则
*/
extern void LEDInit(U8 ucType);
extern void InitaLEDCtrlPara(void);
/**
	@ingroup MMI
	@param 无
	@retval 无
	@brief LED控制主函数
*/
extern void LEDCtrlProcess(void);
extern void LEDAllLight(void);
extern void LED1AllDown(void);
extern void LED2AllDown(void);
extern void LEDAllDown(void);
#endif

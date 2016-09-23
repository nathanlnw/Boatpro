/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: ledctrl.c
  Author: vozhj      Version : V1.00        Date: 2012.03.21
  Description:    LED控制模块
  Version:         V1.00
  Function List:   // 主要函数及其功能
  History:         // 历史修改记录
    <author>    <time>      <version >  <desc>
    lq          2015.07.23              修改文件结构，重新定义LED控制缓存和LED初
                                        始化方式                                            
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "def.h"
#include "ledctrl.h"
#include "ioctrl.h"
#include "systemtick.h"

/* Private define ------------------------------------------------------------*/
#define LED_INFINIT_LEN		0xFF

#define LED1_GREEN_OFFSET	0
#define LED1_RED_OFFSET		1
#define LED2_GREEN_OFFSET	2
#define LED2_RED_OFFSET		3
#define LED3_GREEN_OFFSET	4
#define LED3_RED_OFFSET		5
#define LED4_GREEN_OFFSET	6
#define LED4_RED_OFFSET		7
#define LED1_TIMER_OFFSET	8
#define LED2_TIMER_OFFSET	9
#define LED3_TIMER_OFFSET	10
#define LED4_TIMER_OFFSET	11

/* Private typedef -----------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/
LEDCtrlStruct LEDCtrlPara;

/* Private variables ---------------------------------------------------------*/
/*lq 以下时间以250ms为单位 */
/*lq LED关闭缓冲 */
#define LED_NONE_LEN	    1
static const U8 LED_None_Buf[LED_CTRL_BUFF_DEPTH][LED_NONE_LEN] = 
{	
	{0},		//LED1_GREEN, 对于单色LED只需定义LED_GREEN的控制方式即可，LED_RED的定义对LED控制没有影响
	{0},		//LED1_RED
	{0},		//LED2_GREEN
	{0},		//LED2_RED
	{0},		//LED3_GREEN
	{0},		//LED3_RED
	{0},		//LED4_GREEN
	{0},		//LED4_RED
	{0},	    //LED1_TIME
	{0},		//LED2_TIME
	{0},		//LED3_TIME
	{0},	    //LED4_TIME
};

/*lq GPS搜索缓冲（同超时指示） */
#define LED_GPS_SEARCH_LEN		1
static const U8 LED_GPS_Search_Buf[LED_CTRL_BUFF_DEPTH][LED_GPS_SEARCH_LEN] = 
{	
	{0},		//LED1_GREEN, 对于单色LED只需定义LED_GREEN的控制方式即可，LED_RED的定义对LED控制没有影响
	{0},		//LED1_RED
	{1},		//LED2_GREEN
	{1},		//LED2_RED
	{0},		//LED3_GREEN
	{0},		//LED3_RED
	{0},		//LED4_GREEN
	{0},		//LED4_RED
	{0},	    //LED1_TIME
	{1},		//LED2_TIME
	{0},		//LED3_TIME
	{0},	    //LED4_TIME
};

/*lq GPS定位缓冲（循环，Blue LED 亮250ms，灭250ms） */
#define LED_GPS_FIX_LEN		2
static const U8 LED_GPS_Fix_Buf[LED_CTRL_BUFF_DEPTH][LED_GPS_FIX_LEN] = 
{	
	{0, 0},		//LED1_GREEN, 对于单色LED只需定义LED_GREEN的控制方式即可，LED_RED的定义对LED控制没有影响
	{0, 0},		//LED1_RED
	{0, 0},		//LED2_GREEN
	{0, 0},		//LED2_RED
	{0, 0},		//LED3_GREEN
	{0, 0},		//LED3_RED
	{1, 0},		//LED4_GREEN
	{1, 0},		//LED4_RED
	{0, 0},	    //LED1_TIME
	{0, 0},		//LED2_TIME
	{0, 0},		//LED3_TIME
	{1, 1},	    //LED4_TIME
};

/*lq 正常工作模式发射缓冲（单次，Red LED 亮1000ms，灭250ms） */
#define LED_RF_XMT_LEN		2
static const U8 LED_RF_Xmt_Buf[LED_CTRL_BUFF_DEPTH][LED_RF_XMT_LEN] = 
{	
	{0, 0},		//LED1_GREEN, 对于单色LED只需定义LED_GREEN的控制方式即可，LED_RED的定义对LED控制没有影响
	{0, 0},		//LED1_RED
	{0, 0},		//LED2_GREEN
	{0, 0},		//LED2_RED
	{1, 0},		//LED3_GREEN
	{1, 0},		//LED3_RED
	{0, 0},		//LED4_GREEN
	{0, 0},		//LED4_RED
	{0, 0},	    //LED1_TIME
	{0, 0},		//LED2_TIME
	{4, 1},		//LED3_TIME
	{0, 0},	    //LED4_TIME
};

/*lq 超时指示缓冲（循环，YELLOW LED 长亮） */
#define LED_TIMEOUT_LEN	    1
static const U8 LED_Timeout_Buf[LED_CTRL_BUFF_DEPTH][LED_TIMEOUT_LEN] = 
{	
	{0},		//LED1_GREEN, 对于单色LED只需定义LED_GREEN的控制方式即可，LED_RED的定义对LED控制没有影响
	{0},		//LED1_RED
	{1},		//LED2_GREEN
	{1},		//LED2_RED
	{0},		//LED3_GREEN
	{0},		//LED3_RED
	{0},		//LED4_GREEN
	{0},		//LED4_RED
	{0},	    //LED1_TIME
	{1},		//LED2_TIME
	{0},		//LED3_TIME
	{0},	    //LED4_TIME
};

/*lq 错误指示缓冲（循环，Red LED 长亮） */
#define LED_ERR_LEN	    1
static const U8 LED_Err_Buf[LED_CTRL_BUFF_DEPTH][LED_ERR_LEN] = 
{	
	{0},		//LED1_GREEN, 对于单色LED只需定义LED_GREEN的控制方式即可，LED_RED的定义对LED控制没有影响
	{0},		//LED1_RED
	{0},		//LED2_GREEN
	{0},		//LED2_RED
	{1},		//LED3_GREEN
	{1},		//LED3_RED
	{0},		//LED4_GREEN
	{0},		//LED4_RED
	{0},	    //LED1_TIME
	{0},		//LED2_TIME
	{1},		//LED3_TIME
	{0},	    //LED4_TIME
};

/*lq 正常工作模式电源指示缓冲（循环，Green LED 亮250ms，灭250ms） */
#define LED_POWER_NORMAL_LEN		2
static const U8 LED_PowerNormal_Buf[LED_CTRL_BUFF_DEPTH][LED_POWER_NORMAL_LEN] = 
{	
	{1, 0},		//LED1_GREEN, 对于单色LED只需定义LED_GREEN的控制方式即可，LED_RED的定义对LED控制没有影响
	{1, 0},		//LED1_RED
	{0, 0},		//LED2_GREEN
	{0, 0},		//LED2_RED
	{0, 0},		//LED3_GREEN
	{0, 0},		//LED3_RED
	{0, 0},		//LED4_GREEN
	{0, 0},		//LED4_RED
	{1, 1},	    //LED1_TIME
	{0, 0},		//LED2_TIME
	{0, 0},		//LED3_TIME
	{0, 0},	    //LED4_TIME
};

/*lq 测试工作模式电源指示缓冲（循环，Green LED 亮500ms，灭500ms） */
#define LED_POWER_TEST_LEN		2
static const U8 LED_PowerTest_Buf[LED_CTRL_BUFF_DEPTH][LED_POWER_TEST_LEN] = 
{	
	{1, 0},		//LED1_GREEN, 对于单色LED只需定义LED_GREEN的控制方式即可，LED_RED的定义对LED控制没有影响
	{1, 0},		//LED1_RED
	{0, 0},		//LED2_GREEN
	{0, 0},		//LED2_RED
	{0, 0},		//LED3_GREEN
	{0, 0},		//LED3_RED
	{0, 0},		//LED4_GREEN
	{0, 0},		//LED4_RED
	{2, 2},	    //LED1_TIME
	{0, 0},		//LED2_TIME
	{0, 0},		//LED3_TIME
	{0, 0},	    //LED4_TIME
};

/*lq 发射测试第1项缓冲（循环，Red LED 亮250ms，灭250ms） */
#define LED_TEST_TEST1_LEN		2
static const U8 LED_Test_Test1_Buf[LED_CTRL_BUFF_DEPTH][LED_TEST_TEST1_LEN] = 
{	
	{0, 0},		//LED1_GREEN, 对于单色LED只需定义LED_GREEN的控制方式即可，LED_RED的定义对LED控制没有影响
	{0, 0},		//LED1_RED
	{0, 0},		//LED2_GREEN
	{0, 0},		//LED2_RED
	{1, 0},		//LED3_GREEN
	{1, 0},		//LED3_RED
	{0, 0},		//LED4_GREEN
	{0, 0},		//LED4_RED
	{0, 0},	    //LED1_TIME
	{0, 0},		//LED2_TIME
	{1, 1},		//LED3_TIME
	{0, 0},	    //LED4_TIME
};

/*lq 发射测试第2项缓冲（循环，Red LED 亮250ms，灭750ms） */
#define LED_TEST_TEST2_LEN		2
static const U8 LED_Test_Test2_Buf[LED_CTRL_BUFF_DEPTH][LED_TEST_TEST2_LEN] = 
{	
	{0, 0},		//LED1_GREEN, 对于单色LED只需定义LED_GREEN的控制方式即可，LED_RED的定义对LED控制没有影响
	{0, 0},		//LED1_RED
	{0, 0},		//LED2_GREEN
	{0, 0},		//LED2_RED
	{1, 0},		//LED3_GREEN
	{1, 0},		//LED3_RED
	{0, 0},		//LED4_GREEN
	{0, 0},		//LED4_RED
	{0, 0},	    //LED1_TIME
	{0, 0},		//LED2_TIME
	{1, 3},		//LED3_TIME
	{0, 0},	    //LED4_TIME
};

/*lq 发射测试第3项缓冲（循环，Red LED 亮250ms，灭1250ms） */
#define LED_TEST_TEST3_LEN		2
static const U8 LED_Test_Test3_Buf[LED_CTRL_BUFF_DEPTH][LED_TEST_TEST3_LEN] = 
{	
	{0, 0},		//LED1_GREEN, 对于单色LED只需定义LED_GREEN的控制方式即可，LED_RED的定义对LED控制没有影响
	{0, 0},		//LED1_RED
	{0, 0},		//LED2_GREEN
	{0, 0},		//LED2_RED
	{1, 0},		//LED3_GREEN
	{1, 0},		//LED3_RED
	{0, 0},		//LED4_GREEN
	{0, 0},		//LED4_RED
	{0, 0},	    //LED1_TIME
	{0, 0},		//LED2_TIME
	{1, 5},		//LED3_TIME
	{0, 0},	    //LED4_TIME
};

/*lq 发射测试第4项缓冲（循环，Red LED 亮250ms，灭1750ms） */
#define LED_TEST_TEST4_LEN		2
static const U8 LED_Test_Test4_Buf[LED_CTRL_BUFF_DEPTH][LED_TEST_TEST4_LEN] = 
{	
	{0, 0},		//LED1_GREEN, 对于单色LED只需定义LED_GREEN的控制方式即可，LED_RED的定义对LED控制没有影响
	{0, 0},		//LED1_RED
	{0, 0},		//LED2_GREEN
	{0, 0},		//LED2_RED
	{1, 0},		//LED3_GREEN
	{1, 0},		//LED3_RED
	{0, 0},		//LED4_GREEN
	{0, 0},		//LED4_RED
	{0, 0},	    //LED1_TIME
	{0, 0},		//LED2_TIME
	{1, 7},		//LED3_TIME
	{0, 0},	    //LED4_TIME
};

/*lq 接收测试缓冲（循环，Yellow LED 亮500ms，灭500ms） */
#define LED_TEST_RX_LEN		2
static const U8 LED_TEST_RX_Buf[LED_CTRL_BUFF_DEPTH][LED_TEST_RX_LEN] = 
{	
	{0, 0},		//LED1_GREEN, 对于单色LED只需定义LED_GREEN的控制方式即可，LED_RED的定义对LED控制没有影响
	{0, 0},		//LED1_RED
	{1, 0},		//LED2_GREEN
	{1, 0},		//LED2_RED
	{0, 0},		//LED3_GREEN
	{0, 0},		//LED3_RED
	{0, 0},		//LED4_GREEN
	{0, 0},		//LED4_RED
	{0, 0},	    //LED1_TIME
	{2, 2},		//LED2_TIME
	{0, 0},		//LED3_TIME
	{0, 0},	    //LED4_TIME
};

/*lq 设备自检空闲缓冲（循环，Blue LED 亮250ms，灭4750ms） */
#define LED_SELF_CHECK_IDLE_LEN		2
static const U8 LED_SELF_CHECK_IDLE_Buf[LED_CTRL_BUFF_DEPTH][LED_SELF_CHECK_IDLE_LEN] = 
{	
	{0, 0},		//LED1_GREEN, 对于单色LED只需定义LED_GREEN的控制方式即可，LED_RED的定义对LED控制没有影响
	{0, 0},		//LED1_RED
	{0, 0},		//LED2_GREEN
	{0, 0},		//LED2_RED
	{0, 0},		//LED3_GREEN
	{0, 0},		//LED3_RED
	{1, 0},		//LED4_GREEN
	{1, 0},		//LED4_RED
	{0, 0},	    //LED1_TIME
	{0, 0},		//LED2_TIME
	{0, 0},		//LED3_TIME
	{1, 19},	//LED4_TIME
};

/*lq 设备自检中缓冲（循环，Blue LED 亮250ms，灭250ms） */
#define LED_SELF_CHECK_RUNNING_LEN		2
static const U8 LED_SELF_CHECK_RUNNING_Buf[LED_CTRL_BUFF_DEPTH][LED_SELF_CHECK_RUNNING_LEN] = 
{	
	{0, 0},		//LED1_GREEN, 对于单色LED只需定义LED_GREEN的控制方式即可，LED_RED的定义对LED控制没有影响
	{0, 0},		//LED1_RED
	{0, 0},		//LED2_GREEN
	{0, 0},		//LED2_RED
	{0, 0},		//LED3_GREEN
	{0, 0},		//LED3_RED
	{1, 0},		//LED4_GREEN
	{1, 0},		//LED4_RED
	{0, 0},	    //LED1_TIME
	{0, 0},		//LED2_TIME
	{0, 0},		//LED3_TIME
	{1, 1},	    //LED4_TIME
};

/*lq 设备自检正常缓冲（循环，Blue LED 长亮） */
#define LED_SELF_CHECK_OK_LEN		1
static const U8 LED_SELF_CHECK_OK_Buf[LED_CTRL_BUFF_DEPTH][LED_SELF_CHECK_OK_LEN] = 
{	
	{0},		//LED1_GREEN, 对于单色LED只需定义LED_GREEN的控制方式即可，LED_RED的定义对LED控制没有影响
	{0},		//LED1_RED
	{0},		//LED2_GREEN
	{0},		//LED2_RED
	{0},		//LED3_GREEN
	{0},		//LED3_RED
	{1},		//LED4_GREEN
	{1},		//LED4_RED
	{0},	    //LED1_TIME
	{0},		//LED2_TIME
	{0},		//LED3_TIME
	{1},	    //LED4_TIME
};

/*lq 设备自检错误缓冲（循环，Red LED 长亮） */
#define LED_SELF_CHECK_ERR_LEN		1
static const U8 LED_SELF_CHECK_ERR_Buf[LED_CTRL_BUFF_DEPTH][LED_SELF_CHECK_ERR_LEN] = 
{	
	{0},		//LED1_GREEN, 对于单色LED只需定义LED_GREEN的控制方式即可，LED_RED的定义对LED控制没有影响
	{0},		//LED1_RED
	{0},		//LED2_GREEN
	{0},		//LED2_RED
	{1},		//LED3_GREEN
	{1},		//LED3_RED
	{0},		//LED4_GREEN
	{0},		//LED4_RED
	{0},	    //LED1_TIME
	{0},		//LED2_TIME
	{1},		//LED3_TIME
	{0},	    //LED4_TIME
};

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* 名称  : InitaLEDPara
* 描述  : 初始化单LED控制参数
* 输入  : pLEDPara：LED参数缓冲
          pBuf：控制缓冲
          ucLen：缓冲长度
          ReCycle：是否循环输出
          priority: 优先级
          ucType：类型
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitaLEDPara(LEDStruct *pLEDPara, U8 *pBuf, U8 ucLen, U8 ReCycle,
                  U8 priority, U8 ucType)
{
	U8  ucGreenOffset;
	U8  ucRedOffset;
	U8  ucTimerOffset;

	if (pLEDPara == &LEDCtrlPara.LedGreenPara)
	{
		ucGreenOffset = LED1_GREEN_OFFSET * ucLen;
		ucRedOffset = LED1_RED_OFFSET * ucLen;
		ucTimerOffset = LED1_TIMER_OFFSET * ucLen;
	}
	else if (pLEDPara == &LEDCtrlPara.LedYellowPara)
	{
		ucGreenOffset = LED2_GREEN_OFFSET * ucLen;
		ucRedOffset = LED2_RED_OFFSET * ucLen;
		ucTimerOffset = LED2_TIMER_OFFSET * ucLen;
	}
	else if (pLEDPara == &LEDCtrlPara.LedRedPara)
	{
		ucGreenOffset = LED3_GREEN_OFFSET * ucLen;
		ucRedOffset = LED3_RED_OFFSET * ucLen;
		ucTimerOffset = LED3_TIMER_OFFSET * ucLen;
	}
	else
	{
		ucGreenOffset = LED4_GREEN_OFFSET * ucLen;
		ucRedOffset = LED4_RED_OFFSET * ucLen;
		ucTimerOffset = LED4_TIMER_OFFSET * ucLen;
	}

	pLEDPara->LEDPriority = priority;
	pLEDPara->LEDReCycleFlag = ReCycle;
	pLEDPara->pLEDGreen = &pBuf[ucGreenOffset];
	pLEDPara->pLEDRed = &pBuf[ucRedOffset];
	pLEDPara->pLEDTimer = &pBuf[ucTimerOffset];
	pLEDPara->ucType = ucType;
	pLEDPara->ucBufLen = ucLen;
	pLEDPara->ucLedFragmentTimer = 0;
	pLEDPara->ucLedFragment = 0;

	if (ucType == LED_TYPE_NONE)
	{
		pLEDPara->LEDEn = FALSE;
	}
	else
	{
		pLEDPara->LEDEn = TRUE;
	}
}

/*******************************************************************************
* 名称  : InitaLEDPara
* 描述  : 初始化单LED控制参数
* 输入  : pLEDPara：LED参数		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitaLEDCtrlPara(void)
{
	LEDCtrlPara.TimeCnt = 0;
	LEDCtrlPara.TimePeriod = 50;

	LEDCtrlPara.TimeCnt = SetTimePeriodProcess(LEDCtrlPara.TimePeriod);
	
	InitaLEDPara(&LEDCtrlPara.LedGreenPara, 
				(U8 *)LED_None_Buf, 
				LED_NONE_LEN, 
				LED_RECYCLE_NONE,
                LED_CTRL_PRIORITY_L, 
				LED_TYPE_NONE);
	InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
				(U8 *)LED_None_Buf, 
				LED_NONE_LEN, 
				LED_RECYCLE_NONE, 
                LED_CTRL_PRIORITY_L, 
				LED_TYPE_NONE);
	InitaLEDPara(&LEDCtrlPara.LedRedPara, 
				(U8 *)LED_None_Buf, 
				LED_NONE_LEN, 
				LED_RECYCLE_NONE, 
                LED_CTRL_PRIORITY_L, 
				LED_TYPE_NONE);
	InitaLEDPara(&LEDCtrlPara.LedBluePara, 
				(U8 *)LED_None_Buf, 
				LED_NONE_LEN, 
				LED_RECYCLE_NONE, 
                LED_CTRL_PRIORITY_L, 
				LED_TYPE_NONE);
}

/*******************************************************************************
* 名称  : LEDInit
* 描述  : 初始化LED控制必要的参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void LEDInit(U8 ucType)
{
    
	switch(ucType)
	{
		/*lq ---------------------- LED1 ---------------------------*/
		case LED_TYPE_GREEN_NONE :
		{
			/* 无条件执行 */
	    	IO_Ctrl(IO_LED_GREEN_TYPE, IO_VAL_OFF);
	    	InitaLEDPara(&LEDCtrlPara.LedGreenPara, 
	    				(U8 *)LED_None_Buf, 
	    				LED_NONE_LEN, 
	    				LED_RECYCLE_NONE,
	                    LED_CTRL_PRIORITY_L, 
	    				ucType);
			break;
		}	
	    case LED_TYPE_POWER_NORMAL :
		{
			/*lq 
	         * 之前运行的优先级低于或等于当前优先级，且控制类型与之前的不同，
	         * 则允许执行，否则抛弃
	         */
			if ((LEDCtrlPara.LedGreenPara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_POWER_NORMAL))
			{
	        	IO_Ctrl(IO_LED_GREEN_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedGreenPara, 
							(U8 *)LED_PowerNormal_Buf, 
							LED_POWER_NORMAL_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3,
							ucType);
			}
			break;
		}
		case LED_TYPE_POWER_TEST :
		{
			/*lq 
	         * 之前运行的优先级低于或等于当前优先级，且控制类型与之前的不同，
	         * 则允许执行，否则抛弃
	         */
			if ((LEDCtrlPara.LedGreenPara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_POWER_TEST))
			{
	        	IO_Ctrl(IO_LED_GREEN_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedGreenPara, 
							(U8 *)LED_PowerTest_Buf, 
							LED_POWER_TEST_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3,
							ucType);
			}
			break;
		}
	    
	    /*lq ---------------------- LED2 ---------------------------*/
		case LED_TYPE_YELLOW_NONE :
		{
			/* 无条件执行 */
	    	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
	    	InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
	    				(U8 *)LED_None_Buf, 
	    				LED_NONE_LEN, 
	    				LED_RECYCLE_NONE,
	                    LED_CTRL_PRIORITY_L, 
	    				ucType);
			break;
		}	
		case LED_TYPE_GPS_SEARCH :
		{
			/*lq 
	         * 之前运行的优先级低于或等于当前优先级，且控制类型与之前的不同，
	         * 则允许执行，否则抛弃
	         */
			if ((LEDCtrlPara.LedYellowPara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedYellowPara.ucType != LED_TYPE_GPS_SEARCH))                
			{
	        	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
							(U8 *)LED_GPS_Search_Buf, 
							LED_GPS_SEARCH_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3, 
							ucType);
			
	            if (LEDCtrlPara.LedBluePara.ucType == LED_TYPE_GPS_FIX)
	            {
	            	/*lq 关闭GPS定位指示 */
	                IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
	            	InitaLEDPara(&LEDCtrlPara.LedBluePara, 
	            				(U8 *)LED_None_Buf, 
	            				LED_NONE_LEN, 
	            				LED_RECYCLE_NONE,
	                            LED_CTRL_PRIORITY_L, 
	            				ucType);
	            }
	        }
			break;
		}
		case LED_TYPE_TIMEOUT :
		{
			/*lq 
	         * 之前运行的优先级低于或等于当前优先级，且控制类型与之前的不同，
	         * 则允许执行，否则抛弃
	         */
			if ((LEDCtrlPara.LedYellowPara.LEDPriority >= LED_CTRL_PRIORITY0)
	            && (LEDCtrlPara.LedYellowPara.ucType != LED_TYPE_TIMEOUT))
			{
	        	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
							(U8 *)LED_Timeout_Buf, 
							LED_TIMEOUT_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY0,
							ucType);
			}
			break;
		}
		case LED_TYPE_TEST_RX :
		{
			/*lq 
	         * 之前运行的优先级低于或等于当前优先级，且控制类型与之前的不同，
	         * 则允许执行，否则抛弃
	         */
			if ((LEDCtrlPara.LedYellowPara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedYellowPara.ucType != LED_TYPE_TEST_RX))
			{       	
	    		/* LED3关闭 */
	        	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedRedPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	    		/* LED4关闭 */
	        	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedBluePara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	        	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
							(U8 *)LED_TEST_RX_Buf, 
							LED_TEST_RX_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3,
							ucType);
			}
			break;
		}
	    
	    /*lq ---------------------- LED3 ---------------------------*/
		case LED_TYPE_RED_NONE :
		{
			/* 无条件执行 */
	    	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
	    	InitaLEDPara(&LEDCtrlPara.LedRedPara, 
	    				(U8 *)LED_None_Buf, 
	    				LED_NONE_LEN, 
	    				LED_RECYCLE_NONE,
	                    LED_CTRL_PRIORITY_L, 
	    				ucType);
			break;
		}	
		case LED_TYPE_RF_XMT :
		{
			/*lq 
	         * 单次，之前运行的优先级低于或等于当前优先级，则允许执行，否则抛弃
	         */
			if (LEDCtrlPara.LedRedPara.LEDPriority >= LED_CTRL_PRIORITY0)
			{
	        	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedRedPara, 
							(U8 *)LED_RF_Xmt_Buf, 
							LED_RF_XMT_LEN, 
							LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY0,
							ucType);
			}
			break;
		}
		case LED_TYPE_ERR :
		{
			/*lq 
	         * 之前运行的优先级低于或等于当前优先级，且控制类型与之前的不同，
	         * 则允许执行，否则抛弃
	         */
			if ((LEDCtrlPara.LedRedPara.LEDPriority >= LED_CTRL_PRIORITY0)
	            && (LEDCtrlPara.LedRedPara.ucType != LED_TYPE_ERR))
			{
	        	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedRedPara, 
							(U8 *)LED_Err_Buf, 
							LED_ERR_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY0,
							ucType);
			}
			break;
		}
		case LED_TYPE_TEST_TEST1 :
		{
			/*lq 
	         * 之前运行的优先级低于或等于当前优先级，且控制类型与之前的不同，
	         * 则允许执行，否则抛弃
	         */
			if ((LEDCtrlPara.LedRedPara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedRedPara.ucType != LED_TYPE_TEST_TEST1))
			{
	    		/*lq LED2关闭 */
	        	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	    		/*lq LED4关闭 */
	        	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedBluePara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	        	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedRedPara, 
							(U8 *)LED_Test_Test1_Buf, 
							LED_TEST_TEST1_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3,
							ucType);
			}
			break;
		}
		case LED_TYPE_TEST_TEST2 :
		{
			/*lq 
	         * 之前运行的优先级低于或等于当前优先级，且控制类型与之前的不同，
	         * 则允许执行，否则抛弃
	         */
			if ((LEDCtrlPara.LedRedPara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedRedPara.ucType != LED_TYPE_TEST_TEST2))
			{
	    		/*lq LED2关闭 */
	        	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	    		/*lq LED4关闭 */
	        	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedBluePara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	        	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedRedPara, 
							(U8 *)LED_Test_Test2_Buf, 
							LED_TEST_TEST2_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3,
							ucType);
			}
			break;
		}
		case LED_TYPE_TEST_TEST3 :
		{
			/*lq 
	         * 之前运行的优先级低于或等于当前优先级，且控制类型与之前的不同，
	         * 则允许执行，否则抛弃
	         */
			if ((LEDCtrlPara.LedRedPara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedRedPara.ucType != LED_TYPE_TEST_TEST3))
			{
	    		/*lq LED2关闭 */
	        	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	    		/*lq LED4关闭 */
	        	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedBluePara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	        	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedRedPara, 
							(U8 *)LED_Test_Test3_Buf, 
							LED_TEST_TEST3_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3,
							ucType);
			}
			break;
		}
		case LED_TYPE_TEST_TEST4 :
		{
			/*lq 
	         * 之前运行的优先级低于或等于当前优先级，且控制类型与之前的不同，
	         * 则允许执行，否则抛弃
	         */
			if ((LEDCtrlPara.LedRedPara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedRedPara.ucType != LED_TYPE_TEST_TEST4))
			{
	    		/*lq LED2关闭 */
	        	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	    		/*lq LED4关闭 */
	        	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedBluePara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	        	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedRedPara, 
							(U8 *)LED_Test_Test4_Buf, 
							LED_TEST_TEST4_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3,
							ucType);
			}
			break;
		}
		case LED_TYPE_SELF_CHECK_ERR :
		{
			/*lq 
	         * 之前运行的优先级低于或等于当前优先级，且控制类型与之前的不同，
	         * 则允许执行，否则抛弃
	         */
			if ((LEDCtrlPara.LedRedPara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedRedPara.ucType != LED_TYPE_SELF_CHECK_ERR))
			{
	    		/*lq LED2关闭 */
	        	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	    		/*lq LED4关闭 */
	        	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedBluePara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	        	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedRedPara, 
							(U8 *)LED_SELF_CHECK_ERR_Buf, 
							LED_SELF_CHECK_ERR_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3,
							ucType);
			}
			break;
		}
	    
	    /*lq ---------------------- LED4 ---------------------------*/
		case LED_TYPE_BLUE_NONE :
		{
			/* 无条件执行 */
	    	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
	    	InitaLEDPara(&LEDCtrlPara.LedBluePara, 
	    				(U8 *)LED_None_Buf, 
	    				LED_NONE_LEN, 
	    				LED_RECYCLE_NONE,
	                    LED_CTRL_PRIORITY_L, 
	    				ucType);
			break;
		}	
		case LED_TYPE_GPS_FIX :
		{
			/*lq 
	         * 之前运行的优先级低于或等于当前优先级，且控制类型与之前的不同，
	         * 则允许执行，否则抛弃
	         */
			if ((LEDCtrlPara.LedBluePara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedBluePara.ucType != LED_TYPE_GPS_FIX))
			{
	        	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedBluePara, 
							(U8 *)LED_GPS_Fix_Buf, 
							LED_GPS_FIX_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3, 
							ucType);
			
	            if (LEDCtrlPara.LedYellowPara.ucType == LED_TYPE_GPS_SEARCH)
	            {
	            	/*lq 关闭LED搜索指示 */
	                IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
	            	InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
	            				(U8 *)LED_None_Buf, 
	            				LED_NONE_LEN, 
	            				LED_RECYCLE_NONE,
	                            LED_CTRL_PRIORITY_L, 
	            				ucType);           
	            }
	        }
			break;
		}
		case LED_TYPE_SELF_CHECK_IDLE :
		{
			/*lq 
	         * 之前运行的优先级低于或等于当前优先级，且控制类型与之前的不同，
	         * 则允许执行，否则抛弃
	         */
			if ((LEDCtrlPara.LedBluePara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedBluePara.ucType != LED_TYPE_SELF_CHECK_IDLE))
			{
	    		/*lq LED2关闭 */
	        	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	    		/*lq LED3关闭 */
	        	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedRedPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	        	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedBluePara, 
							(U8 *)LED_SELF_CHECK_IDLE_Buf, 
							LED_SELF_CHECK_IDLE_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3,
							ucType);
			}
			break;
		}
		case LED_TYPE_SELF_CHECK_RUNNING :
		{
			/*lq 
	         * 之前运行的优先级低于或等于当前优先级，且控制类型与之前的不同，
	         * 则允许执行，否则抛弃
	         */
			if ((LEDCtrlPara.LedBluePara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedBluePara.ucType != LED_TYPE_SELF_CHECK_RUNNING))
			{
	    		/*lq LED2关闭 */
	        	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	    		/*lq LED3关闭 */
	        	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedRedPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	        	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedBluePara, 
							(U8 *)LED_SELF_CHECK_RUNNING_Buf, 
							LED_SELF_CHECK_RUNNING_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3,
							ucType);
			}
			break;
		}
		case LED_TYPE_SELF_CHECK_OK :
		{
			/*lq 
	         * 之前运行的优先级低于或等于当前优先级，且控制类型与之前的不同，
	         * 则允许执行，否则抛弃
	         */
			if ((LEDCtrlPara.LedBluePara.LEDPriority >= LED_CTRL_PRIORITY3)
	            && (LEDCtrlPara.LedBluePara.ucType != LED_TYPE_SELF_CHECK_OK))
			{
	    		/*lq LED2关闭 */
	        	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedYellowPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	    		/*lq LED3关闭 */
	        	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
	        	InitaLEDPara(&LEDCtrlPara.LedRedPara, 
	        				(U8 *)LED_None_Buf, 
	        				LED_NONE_LEN, 
	        				LED_RECYCLE_NONE,
	                        LED_CTRL_PRIORITY_L, 
	        				ucType);
	
	        	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
				InitaLEDPara(&LEDCtrlPara.LedBluePara, 
							(U8 *)LED_SELF_CHECK_OK_Buf, 
							LED_SELF_CHECK_OK_LEN, 
							LED_RECYCLE,
	                        LED_CTRL_PRIORITY3,
							ucType);
			}
			break;
		}
	
	    /*lq --------------- LED1/LED2/LED3/LED4 -------------------*/
		default:
		{
			/* 无条件执行 */
			InitaLEDCtrlPara();
			LEDAllDown();
			break;
		}
	}
}

/*******************************************************************************
* 名称  : LEDAllLight
* 描述  : LED全部开启
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void LEDAllLight(void)
{
#if !WIN32
	IO_Ctrl(IO_LED_GREEN_TYPE, IO_VAL_ON);
	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_ON);
	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_ON);
	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_ON);
#endif
}

/*******************************************************************************
* 名称  : LEDAllDown
* 描述  : LED全部关闭
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void LEDAllDown(void)
{
#if !WIN32
	IO_Ctrl(IO_LED_GREEN_TYPE, IO_VAL_OFF);
	IO_Ctrl(IO_LED_YELLOW_TYPE, IO_VAL_OFF);
	IO_Ctrl(IO_LED_RED_TYPE, IO_VAL_OFF);
	IO_Ctrl(IO_LED_BLUE_TYPE, IO_VAL_OFF);
#endif
}

/*******************************************************************************
* 名称  : LedCtrl
* 描述  : 初始化单LED控制参数
* 输入  : pLEDPara：LED参数缓冲
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void LedCtrl(LEDStruct *pLEDPara)
{
	U8 ucTempGreen;
//	U8 ucTempRed;

	if (pLEDPara->LEDEn == TRUE)
	{
		if (pLEDPara->pLEDGreen[pLEDPara->ucLedFragment] > 0)
		{
			ucTempGreen = IO_VAL_ON;
		}
		else
		{
			ucTempGreen = IO_VAL_OFF;
		}

		if (pLEDPara->pLEDRed[pLEDPara->ucLedFragment] > 0)
		{
			;//ucTempRed = IO_VAL_ON;
		}
		else
		{
			//;ucTempRed = IO_VAL_OFF;
		}

		pLEDPara->ucLedFragmentTimer++;

		if (pLEDPara->ucLedFragmentTimer >= pLEDPara->pLEDTimer[pLEDPara->ucLedFragment])
		{
			pLEDPara->ucLedFragmentTimer = 0;
			pLEDPara->ucLedFragment++;

			if (pLEDPara->ucLedFragment >= pLEDPara->ucBufLen)
			{
				pLEDPara->ucLedFragment = 0;

				if (pLEDPara->LEDReCycleFlag == LED_RECYCLE_NONE)
				{
					pLEDPara->LEDEn = FALSE;
                    pLEDPara->LEDPriority = LED_CTRL_PRIORITY_L;
				}
			}
		}
#if !WIN32
		if (pLEDPara == &LEDCtrlPara.LedGreenPara)
		{
			IO_Ctrl(IO_LED_GREEN_TYPE, ucTempGreen);    //lq 对于单色LED只需根据LED_GREEN的定义方式进行控制即可
		}
		else if (pLEDPara == &LEDCtrlPara.LedYellowPara)
		{
			IO_Ctrl(IO_LED_YELLOW_TYPE, ucTempGreen);
		}
		else if (pLEDPara == &LEDCtrlPara.LedRedPara)
		{
			IO_Ctrl(IO_LED_RED_TYPE, ucTempGreen);
		}
		else
		{
			IO_Ctrl(IO_LED_BLUE_TYPE, ucTempGreen);
		}		
#endif			
	}
}
/*******************************************************************************
* 名称  : LEDCtrlProcess
* 描述  : LED处理主函数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void LEDCtrlProcess(void)
{
    /* 250ms定时 */
	if (LEDCtrlPara.TimeCnt == GetLocalClockTickProcess())
	{
		LEDCtrlPara.TimeCnt = SetTimePeriodProcess(LEDCtrlPara.TimePeriod);

		/* LED控制 */
		LedCtrl(&LEDCtrlPara.LedGreenPara);
		LedCtrl(&LEDCtrlPara.LedYellowPara);
		LedCtrl(&LEDCtrlPara.LedRedPara);
		LedCtrl(&LEDCtrlPara.LedBluePara);
	}
}
#if 0
/*******************************************************************************
* 名称  : LEDCtrlTest
* 描述  : LED测试函数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 LEDTestFlag = 0;
void LEDCtrlTest(void)
{
	if (LEDTestFlag == 1)
	{
		LEDTestFlag = 0;
		LEDInit(LED_TYPE_GPS_SEARCH);	
	}
	else if (LEDTestFlag == 2)
	{
		LEDTestFlag = 0;
		LEDInit(LED_TYPE_GPS_FIX);	
	}
	else if (LEDTestFlag == 3)
	{
		LEDTestFlag = 0;
		LEDInit(LED_TYPE_RF_XMT);	
	}
	else if (LEDTestFlag == 7)
	{
		LEDTestFlag = 0;
		LEDInit(LED_TYPE_TEST_TEST1);	
	}
	else if (LEDTestFlag == 8)
	{
		LEDTestFlag = 0;
		LEDInit(LED_TYPE_TEST_TEST2);	
	}
}
#endif

/*******************************************************************************
* 名称  : LEDReadFileProcess
* 描述  : 读取SD卡文件LED处理函数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void LEDReadFileProcess(void)
{
	/* 250ms定时 */
	if (LEDCtrlPara.TimeCnt == GetLocalClockTickProcess())
	{
		LEDCtrlPara.TimeCnt = SetTimePeriodProcess(LEDCtrlPara.TimePeriod);

		/* LED控制,只对绿灯进行控制 */
		LedCtrl(&LEDCtrlPara.LedGreenPara);
	}
}

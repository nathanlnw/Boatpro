/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: RfLinkSelfCheck.h
  Author: lq       Version : V1.0.0        Date: 2015.07.22
  Description:     射频链路自检模块
  Version:         V1.0.0
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _RFLINKSELFCHECK_H_
#define _RFLINKSELFCHECK_H_

/* Include -------------------------------------------------------------------*/
#include "Include.h"

/* Public define -------------------------------------------------------------*/
/*lq 射频链路自检控制状态宏定义 */
#define RFL_SLFCHK_CTRL_STATE_IDLE      0
#define RFL_SLFCHK_CTRL_STATE_TX        1
#define RFL_SLFCHK_CTRL_STATE_RX        2

/*lq 射频链路自检时间宏定义 */
#define RFL_SLFCHK_TIME_PERIOD          200     //lq unit:5ms
#define RFL_SLFCHK_TIME_TIMEOUT_TX      1       //lq unit:1s
#define RFL_SLFCHK_TIME_TIMEOUT_RX      1       //lq unit:1s

/*lq 射频链路自检消息宏定义 */
#define RFL_SLFCHK_MSG6_AI_DAC          0       //lq test (DAC = 0), used for test purposes.
#define RFL_SLFCHK_MSG6_AI_FI           0
#define RFL_SLFCHK_MSG6_AD              0x55
#define RFL_SLFCHK_MSG6_AD_BIT_LEN      8
   
/* Public typedef ------------------------------------------------------------*/
typedef struct 
{
	U32 TimeCnt;
	U16 TimePeriod;	
    U16 Timeout;
    U16 TimeoutCnt;
    
    U8  CtrlState;
    U8  StartFlg;
    U8  CompleteFlg;

    U8  RfLinkOkFlg;
}RfLinkSelfCheckStruct;

/* Public variables ----------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/
//lnw add
extern void RFL_RfLinkSelfCheckStructInit(RfLinkSelfCheckStruct *pRfLinkSelfCheckStruct);
extern void RFL_SelfCheck(RfLinkSelfCheckStruct *pRfLinkSelfCheckStruct);


#endif
/*==============================================================================
------------------------------------End of File---------------------------------
==============================================================================*/

/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: LightBeaconProtocol.h
  Author: lq       Version : V1.00        Date: 2014.01.21
  Description:     Protocol between AIS and light beacon  
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _LIGHT_BEACON_PROTOCOL_H
#define _LIGHT_BEACON_PROTOCOL_H

/* Include ------------------------------------------------------------*/
#include "usart.h"

/* Private define ------------------------------------------------------------*/
/*lq 发送队列深度 */
#define LB_PRT_REPET_BUFF_LEN               10

/*lq 时间定时宏定义 */
#define LB_PRT_TIME_RESPONSE_INTERVAL       2   //lq 单位5ms
#define LB_PRT_TIME_REPETITION_INTERVAL     200 //lq 单位5ms

/*lq RDM格式命令类型*/
#define LB_PRT_RDM_DATE                     1
#define LB_PRT_RDM_UTC_TIME                 2
#define LB_PRT_RDM_ANCHOR                   3
#define LB_PRT_RDM_MMSI                     4
#define LB_PRT_RDM_LIGHT_BEACON_TYPE        5
#define LB_PRT_RDM_NAME                     6
#define LB_PRT_RDM_AIS_STATUE               7
#define LB_PRT_RDM_USART_SETTING            8

/*lq RDM格式命令类型*/
#define LB_PRT_WRM_DATE                     1
#define LB_PRT_WRM_UTC_TIME                 2
#define LB_PRT_WRM_ANCHOR                   3
#define LB_PRT_WRM_MMSI                     4
#define LB_PRT_WRM_LIGHT_BEACON_TYPE        5
#define LB_PRT_WRM_NAME                     6
#define LB_PRT_WRM_ATON_STATUE              7
#define LB_PRT_WRM_USART_SETTING            8

/*lq CDM格式命令类型*/
#define LB_PRT_CDM_START_INTERROGATION      1
#define LB_PRT_CDM_AIS_SLEEP                2
#define LB_PRT_CDM_STOP_INTERROGATION       3

/* Private typedef -----------------------------------------------------------*/

/*lq light beacon protocol 通信控制结构体*/
typedef struct{

	U32 TimeCnt;
	U16 TimePeriod;	

    U8  InterrogationFlag;
#if 0
    U8  SendFIFO[LB_PRT_SEND_FIFO_DEPTH][2];    //lq SendFIFO[][0]存放发送报文的类型号，SendFIFO[][1]存放发送报文重传的剩余次数 
    U16 SendFIFOIn;
    U16 SendFIFOOut;       
#endif
	U16 RepetBuff[LB_PRT_REPET_BUFF_LEN];      //lq RepetBuff[0]存放重传剩余次数，RepetBuff[1]存放要重传的命令个数，RepetBuff[2-]存放要重传的命令类型号 
    U8  LB_Stat;
    U16 LB_TimeY;
	U16 LB_TimeMon;
	U16 LB_TimeD;
	U16 LB_TimeH;
	U16 LB_TimeM;
	U16 LB_TimeS;
	S32 LB_Latitude;
	S32 LB_Longitud;

    //lq struct UartRxFIFOStruct Uart2RxFIFO2;

}LBPrtCommStruct;

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
extern LBPrtCommStruct LBPrtCommPara;
/* Public  function  ---------------------------------------------------------*/

#endif

/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: UbloxProtocol.h
  Author: lq       Version : V1.00        Date: 2014.01.13
  Description:     ublox protocol
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _UBLOXPROTOCOL_H
#define _UBLOXPROTOCOL_H

/* Include ------------------------------------------------------------*/
#include "usart.h"

/* Private define ------------------------------------------------------------*/
/*lq UBLOX消息类号宏定义 */
#define UBX_PRT_MSG_CLASS_ACK                   0x05
#define UBX_PRT_MSG_CLASS_CFG                   0x06

/*lq UBLOX消息号宏定义 */
#define UBX_PRT_MSG_ID_ACK_ACK                  0x01
#define UBX_PRT_MSG_ID_ACK_NAK                  0x00

#define UBX_PRT_MSG_ID_CFG_MSG                  0x01
#define UBX_PRT_MSG_ID_CFG_CFG                  0x09
#define UBX_PRT_MSG_ID_CFG_RXM                  0x11
#define UBX_PRT_MSG_ID_CFG_TP5                  0x31
#define UBX_PRT_MSG_ID_CFG_PM2                  0x3B

/*lq UBLOX消息缓存长度 */
#define UBX_PRT_MSG_BUFFER_DEPTH                16
#define UBX_PRT_MSG_BUFFER_LEN                  80

/*lq UBX消息长度*/
#define UBX_PRT_MSG_LEN_CFG_MSG                 14
#define UBX_PRT_MSG_LEN_CFG_TP5                 38
#define UBX_PRT_MSG_LEN_CFG_CFG                 19
#define UBX_PRT_MSG_LEN_CFG_PM2                 50
#define UBX_PRT_MSG_LEN_CFG_RXM                 8

/*lq 状态机状态宏定义*/
#define UBX_PRT_COMM_CTRL_STAT_IDLE             0
#define UBX_PRT_COMM_CTRL_STAT_WAKEUP           1
#define UBX_PRT_COMM_CTRL_STAT_SEND             2
#define UBX_PRT_COMM_CTRL_STAT_RESPONSE_WAIT    3

/*lq 时间定时宏定义 */
#define UBX_PRT_TIME_WAKEUP_WAIT                10      //lq 50ms
#define UBX_PRT_TIME_RESPONSE_WAIT              200     //lq 1000ms

/*lq GPS接收机唤醒消息*/
#define UBX_PRT_DUMMY_SEQUENCE                  0xFF

/*lq UBX每条消息尝试发送次数的最大值 */
#define UBX_PRT_SINGLE_MSG_SEND_CNT_THRES       3

/* Private typedef -----------------------------------------------------------*/

/*lq ublox protocol 通信控制结构体*/
typedef struct{

	U32 TimeCnt;
	U16 TimePeriod;	

    U8  MsgTxBuffer[UBX_PRT_MSG_BUFFER_DEPTH][UBX_PRT_MSG_BUFFER_LEN];
    U8  MsgTx[UBX_PRT_MSG_BUFFER_LEN];
    U16 TxFIFOIn;
    U16 TxFIFOOut;       
    U8  CtrlState;          //lq 通信状态机状态
    U8  SendRequireFlag;    //lq UBX消息发送请求标志
    U8  SendFlag;           //lq UBX消息发送标志
    U8  MsgClass;           //lq UBX消息类
    U8  MsgID;              //lq UBX消息号
    U8  RespFlag;           //lq UBX消息响应标志
    U8  AckFlag;            //lq 设置是否成功
    U8  SendCnt;            //lq 每条消息尝试发送的次数
    struct UartRxFIFOStruct Uart2RxFIFO2;

}UBXPrtCommStruct;

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
extern UBXPrtCommStruct UBXPrtCommPara;

/* Public  function  ---------------------------------------------------------*/

//lnw add
extern void UBX_ModuleInit(void); 
extern U16 UBX_GetCOMMMess(U8 COMM);
extern void UBX1_UBXPrtCommCtrlProcess(void);
extern void UBX_UartRxProcess(U8 COMM, U8 data);

#endif

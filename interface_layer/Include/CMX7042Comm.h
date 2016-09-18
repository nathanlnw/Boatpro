/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: CMX7042Comm.h
  Author: lq       Version : V1.00        Date: 2014.05.21
  Description:     CMX7042消息发送和接收通信
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _CMX7042COMM_H
#define _CMX7042COMM_H

/* Include ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define BUFF_FIFO_DEPTH         8
#define BUFF_FIFO_SIZE          128

#define BUFF_SIZE               176
#define AIS_PACKET_STARTFLAG    0x7E
#define AIS_PACKET_ENDFLAG      0x7E

/*lq CMX7042消息发射控制状态 */
#define CMXCOMM_TX_CTRL_STATE_IDLE          0
#define CMXCOMM_TX_CTRL_STATE_TX_WAITING    1
#define CMXCOMM_TX_CTRL_STATE_TX_EXEC       2
#define CMXCOMM_TX_CTRL_STATE_DONE_WAITING  3

/*lq 时间 */
#define CMXCOMM_TIMEOUT_TX_DONE_WAITING     2000        //lq 单位：5ms

#define AIS_FREQ_A                          161975000u  //lq 单位：1Hz
#define AIS_FREQ_B                          162025000u
#define AIS_CHANNEL_A                       'A'
#define AIS_CHANNEL_B                       'B'
/* Private typedef -----------------------------------------------------------*/
typedef struct BuffFIFO
{
	U8  FIFO[BUFF_FIFO_DEPTH][BUFF_FIFO_SIZE];
	U16 FIFOInput;
	U16 FIFOOutput;
	U16 BuffIndex;                                      //lq 列标
	
}BuffFIFOStruct;

typedef struct CMXRecMsgCell
{
	U8  Data[BUFF_SIZE];
	U16 BurstLen;                                       //lq 单位：1byte
	U16 DCOffset;
	U16 ArrivalTime;
    U16 ArrivalSLot;
    U8  RxChannel;	
}CMXRecMsgCellStruct;

typedef struct CMXTraMsgCell
{
	U8  Data[BUFF_SIZE];
	U16 DataLen;                                        //lq 单位：1byte
    U16 TxSlot;
    U8  TxChannel;	
}CMXTraMsgCellStruct;

typedef struct CMXTxCtrlCell
{
	U32 TimeCnt;
	U16 TimePeriod;	
	U8  Data[BUFF_SIZE];
	U16 DataLen;                                        //lq 单位：1byte
    U16 TxSlot;
    U8  TxChannel;
    U8  State;                                          //lq TRUE - 正在发射，FALSE - 发射结束	
}CMXTxCtrlCellStruct;

typedef struct CMXRecMsgFIFO
{
    CMXRecMsgCellStruct FIFO[BUFF_FIFO_DEPTH];
	U16 FIFOInput;
	U16 FIFOOutput;	
}CMXRecMsgFIFOStruct;

typedef struct CMXTraMsgFIFO
{
    CMXTraMsgCellStruct FIFO[BUFF_FIFO_DEPTH];
	U16 FIFOInput;
	U16 FIFOOutput;	
}CMXTraMsgFIFOStruct;

typedef struct CMX7042Comm
{
    CMXRecMsgFIFOStruct RxFIFO;
    //lq BuffFIFOStruct TxFIFO;
    CMXTraMsgFIFOStruct TxFIFO;
    CMXTxCtrlCellStruct TxCtrlCell;

}CMX7042CommStruct;

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
extern CMX7042CommStruct CMX7042CommInstance;

/* Public  function  ---------------------------------------------------------*/
extern U8 CMXCOMM_RxEnque(CMX7042CommStruct* pCMX7042CommStruct, CMXRecMsgCellStruct* pCMXRecMsgCellStruct);
extern U8 CMXCOMM_GetRxData(CMX7042CommStruct* pCMX7042CommStruct, U8 rxChannel);
extern void CMXCOMM_ReceiveProcess(CMX7042CommStruct* pCMX7042CommStruct);
extern U8 CMXCOMM_RxDeque(CMX7042CommStruct* pCMX7042CommStruct, CMXRecMsgCellStruct* pCMXRecMsgCellStruct);
extern U8 CMXCOMM_RxEnque(CMX7042CommStruct* pCMX7042CommStruct, CMXRecMsgCellStruct* pCMXRecMsgCellStruct);
extern U8 CMXCOMM_IsTxQueEmpty(CMX7042CommStruct* const pCMX7042CommStruct);
extern U8 CMXCOMM_TxDeque(CMX7042CommStruct* pCMX7042CommStruct, CMXTraMsgCellStruct* pCMXTraMsgCellStruct);
extern void CMXCOMM_TransmitProcess(CMX7042CommStruct* const pCMX7042CommStruct);
extern void CMXCOMM_TxTest(void);
extern void CMXCOMM_SimpleOperVeriTx(void);

#endif

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
#ifndef _GMSKMOD_H
#define _GMSKMOD_H

#include "math_rand.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define G_MOD_FILTER_SIZE           (21)
#define FSKDATAXMT_BUF_LEN			(3120)		//发送数据产度                  //lq 最小取值要保证不小于GMSK调制前的消息位数的5倍，原来取值为5120

#define FSKDATAXMT_FIFO_DEPTH		(3)
#define FSKDATAXMT_FIFODATA_LEN		(70)

#define FSKDATAXMTFIFO_DEPTH        (1) //lq (3)

#define FSKDATA_TRAN_IDLE			(0)		//空闲状态
#define FSKDATA_TRAN_START			(1)		//功率上升状态
#define FSKDATA_TRAN_DATA			(2)		//发送数据状态
#define FSKDATA_TRAN_END			(3)		//数据发送结束状态
#define FSKDATA_TRAN_STARTRDY		(4)		//功率上升状态

#define FSKDATA_NOWSLOT				(0x7FFF)

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/

/**
	@ingroup 数据处理
	@brief 发送控制结构体，包括发送的时隙，数据缓冲、长度等
*/
typedef struct {
	S16 siDataXmtBuf[FSKDATAXMT_BUF_LEN];	//发送数据缓冲                  //lq 存放调制后的数据(Msg18[和Msg19])
	U16 uiDataXmtIndex;						//数据发送索引                      //lq 用于发送时的计数(单位-字，16bit)
	U16 uiDataLen;							//发送数据长度                      //lq 要发送数据（调制后的数据长度[单位-字，16b]）的长度
	U16 uiTran_Sta;							//数据发送状态机状态
	U16 uiFirt_En;							//数据调制使能
	S16 siRxTx_Slot;						//收发切换时隙号
	S16 siTxRx_Slot;						//发收切换时隙号
	S16 siPowerUp_Slot;						//功率上升时隙
	S16 siPowerDown_Slot;					//功率下降时隙
	S16 siDataStart_Slot;					//数据开始发送时隙               //lq 数据开始发送相位
	S16 siTran_Slot;						//发送帧号，0～2249，为0xFFFF时使用当前帧
//lq-	S32 siTran_Slot;                        //调制后的消息的发射时隙号
    U16 uiModIndex;                                                             //lq 用于调制时的计数(单位-字，16bit)
	U16 uiServicNum;
	U32 uiXmtFreq;                          //当前调制信号的发射频率
}FSKDataXmtStruct;


typedef struct FSKDataXmtFIFO
{
    FSKDataXmtStruct  arrFIFOData[FSKDATAXMTFIFO_DEPTH];
    U8  ucFIFOIn;
    U8  ucFIFOOut;
    U8  ucFIFOLen;
}FSKDataXmtFIFOStruct;

extern FSKDataXmtStruct FSKDataXmtStru;
extern FSKDataXmtStruct *pFSKDataXmtStru2;  
extern FSKDataXmtFIFOStruct gstruFSKDataXmtFIFO;
extern struct math_rand send_seed;

/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/
extern void InitFSKDataTranPara(void);
extern U8 GMSKModMain(U8 *pInBuf, S16 uiSlotNum, U16 uiLen, U16 uiModAmp, U16 uiOffset);
extern U8 GMSKModTest(U8 *pInBuf, S16 uiSlotNum, U16 uiLen, U16 uiModAmp, U16 uiOffset, U8 ucTestMod);
extern U8 GMSKModTest2(U8 *pInBuf, S16 uiSlotNum, U16 uiLen, U16 uiModAmp, U16 uiOffset);
//extern U8 FillGMSKTranFIFO(U8 *pInBuf, U16 uiSlotNum, U16 uiLen);
extern void set_send_seed(U32 seed);

extern U8 GMSKModMain_ITDMA(U8 *pInBuf, S16 uiSlotNum, U16 uiLen, U16 uiModAmp, U16 uiOffset, 	U32 uiXmtFreq);
extern U8 FIFOInOfFSKDataXmt(void);
extern U8 FIFOOutOfFSKDataXmt(FSKDataXmtStruct **outFSKDataXmtPara);
extern U8 IsEmptyOfFSKDataXmt(void);
extern U8 IsFullOfFSKDataXmt(void);
extern void InitFSKDataXmtFIFO(FSKDataXmtFIFOStruct *pFSKDataXmtFIFO);
#endif

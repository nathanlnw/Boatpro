/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: CMX7042Comm.c
  Author: lq       Version : V1.00        Date: 2014.05.21
  Description:     CMX7042消息发送和接收通信
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "stm32f10x_lib.h"
#include "def.h"
#include "CMX7042.h"
#include "CMX7042Comm.h"
#include "sotdma.h"
#include "RxTxCtrl.h"

CMX7042CommStruct CMX7042CommInstance;
U8 CMX7042TestMsgFlg = FALSE;
U8 CMX7042SovTxFlg = FALSE;

/*******************************************************************************
* 名称  : CMXCOMM_InitStruct
* 描述  : 初始化结构体CMX7042CommStruct变量
* 输入  : pCMX7042CommStruct-结构体指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static CMXCOMM_InitStruct(CMX7042CommStruct* pCMX7042CommStruct)
{
    pCMX7042CommStruct->RxFIFO.FIFOInput = 0;
    pCMX7042CommStruct->RxFIFO.FIFOOutput = 0;

    pCMX7042CommStruct->TxFIFO.FIFOInput = 0;
    pCMX7042CommStruct->TxFIFO.FIFOOutput = 0;
    pCMX7042CommStruct->TxCtrlCell.TimePeriod = CMXCOMM_TIMEOUT_TX_DONE_WAITING;
    pCMX7042CommStruct->TxCtrlCell.TimeCnt = 0;
    pCMX7042CommStruct->TxCtrlCell.State = CMXCOMM_TX_CTRL_STATE_IDLE;
}

/*******************************************************************************
* 名称  : CMXCOMM_InitInstance
* 描述  : 初始化结构体CMX7042CommStruct实例
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMXCOMM_InitInstance(void)
{
    CMXCOMM_InitStruct(&CMX7042CommInstance);
}

/*******************************************************************************
* 名称  : CMXCOMM_CtrlProcess
* 描述  : CMX7042通信控制进程
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMXCOMM_CtrlProcess(void)
{
	U16	data[4];

	/*lq... CMX7042时隙更新，需在无发射和接收任务时进行（因需切换工作模式） */
	if (CMX7042_GetSlotCntCalibFlgRst() == TRUE)
	{
		CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_EnterConfigMode);
		data[0] = GetSlotNum();
		CMX7042_ExecCfgTask(CMX7042_CTASK_SetSlotCount, data, 1);
		CMX7042_ExecCfgTaskNoPara(CMX7042_CTASK_EXIT_CONFIG);				

	    /*lq 启动接收任务 */
	    CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_Rx1_RXB);
	    CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_Rx2_RXB);
	}

	/*lq 接收 */
	CMXCOMM_ReceiveProcess(&CMX7042CommInstance); 

    /*lq 发射 */
    CMXCOMM_TransmitProcess(&CMX7042CommInstance);

    if (CMX7042TestMsgFlg == TRUE)
    {
        CMX7042TestMsgFlg = FALSE;
        CMXCOMM_TxTest();
    }

    if (CMX7042SovTxFlg == TRUE)
    {
        CMX7042SovTxFlg = FALSE;
        CMXCOMM_SimpleOperVeriTx();
    }
}

/*******************************************************************************
* 名称  : CMXCOMM_ReceiveProcess
* 描述  : CMX7042消息接收
* 输入  : pCMX7042CommStruct - CMX7042CommStruct指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMXCOMM_ReceiveProcess(CMX7042CommStruct* pCMX7042CommStruct)
{
    /*lq 接收机1 */
    if (CMX7042_IsRx1Ready() == TRUE)
    {
        /*lq 接收数据 */ 
        if (CMXCOMM_GetRxData(pCMX7042CommStruct, CMX7042_RX_1) == TRUE)
        {
            //lq usart_printf("$CMX7042 RX1 has received!\r\n");                                
        }

        /*lq 启动下一次接收 */
        CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_Rx1_RXB);
    }

    /*lq 接收机2 */    
    if (CMX7042_IsRx2Ready() == TRUE)
    {
        /*lq 接收数据 */ 
        if (CMXCOMM_GetRxData(pCMX7042CommStruct, CMX7042_RX_2) == TRUE)
        {
            //lq usart_printf("$CMX7042 RX2 has received!\r\n");                                        
        }

        /*lq 启动下一次接收 */
        CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_Rx2_RXB);
    }

    if (CMX7042_IsRxStateChange() == TRUE)
    {
        /*lq 接收机1出错 */
        if (CMX7042_GetRxErrorRst(CMX7042_RX_1) == TRUE)
        {
            /*lq 启动下一次接收 */
            CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_Rx1_RXB);
    
            if (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_TwoLong)
            {
                usart_printf("$CMX7042 RX1 ERR: TwoLong!\r\n");                                
            }
            else if (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_CRC)
            {
                usart_printf("$CMX7042 RX1 ERR: CRC!\r\n");                                        
            }
            else if (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_NewFrameTwoBuffFull)
            {
                usart_printf("$CMX7042 RX1 ERR: NewFrameTwoBuffFull!\r\n");                                        
            } 
            else if (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_EndFlgNotByteBound)
            {
                usart_printf("$CMX7042 RX1 ERR: EndFlgNotByteBound!\r\n");                                        
            }                        
        }
    
        /*lq 接收机2出错 */
        if (CMX7042_GetRxErrorRst(CMX7042_RX_2) == TRUE)
        {
            /*lq 启动下一次接收 */
            CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_Rx2_RXB); 
            
            if (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_TwoLong)
            {
                usart_printf("$CMX7042 RX2 ERR: TwoLong!\r\n");                                
            }
            else if (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_CRC)
            {
                usart_printf("$CMX7042 RX2 ERR: CRC!\r\n");                                        
            }
            else if (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_NewFrameTwoBuffFull)
            {
                usart_printf("$CMX7042 RX2 ERR: NewFrameTwoBuffFull!\r\n");                                        
            } 
            else if (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_EndFlgNotByteBound)
            {
                usart_printf("$CMX7042 RX2 ERR: EndFlgNotByteBound!\r\n");                                        
            }                
        }    
    }    
}

/*******************************************************************************
* 名称  : CMXCOMM_TransmitProcess
* 描述  : CMX7042消息发送
* 输入  : pCMX7042CommStruct - CMX7042CommStruct指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMXCOMM_TransmitProcess(CMX7042CommStruct* const pCMX7042CommStruct)
{
    CMXTraMsgCellStruct* pTraMsgCell;
    CMXTxCtrlCellStruct* pTxCtrlCell;
    S32 currSlotSum;
    S16 SlotDiff1;
    S16 SlotDiff2;
    U16 i;
    U8  txStatus;

    pTxCtrlCell = &(pCMX7042CommStruct->TxCtrlCell);
    currSlotSum = GetSlotNum();

    switch (pTxCtrlCell->State)
    {
        case CMXCOMM_TX_CTRL_STATE_IDLE:

            /*lq 发送消息队列非空 */
            if (CMXCOMM_IsTxQueEmpty(pCMX7042CommStruct) == FALSE)
            {
                /*lq 出队 */
                CMXCOMM_TxDeque(pCMX7042CommStruct, pTraMsgCell);
    
                /*lq 发射控制单元赋值 */
                for (i = 0; i < pTraMsgCell->DataLen; i++)
                {
                    pTxCtrlCell->Data[i] = pTraMsgCell->Data[i]; 
                }
                pTxCtrlCell->DataLen = pTraMsgCell->DataLen;                     
                pTxCtrlCell->TxSlot = pTraMsgCell->TxSlot;                     
                pTxCtrlCell->TxChannel = pTraMsgCell->TxChannel;                     
                pTxCtrlCell->State = CMXCOMM_TX_CTRL_STATE_TX_WAITING; 
                usart_printf("$CMX7042 TX State: TX_WAITING!\r\n");                                                                   
            } 
                    
            break;    

        case CMXCOMM_TX_CTRL_STATE_TX_WAITING:

            SlotDiff1 = pTxCtrlCell->TxSlot - currSlotSum;
            SlotDiff2 = pTxCtrlCell->TxSlot + 2250 - currSlotSum;

            /*lq 预定时隙有效 */
            if (!((SlotDiff1 > TIME_OFFSET_SEND_TO_TX)
                || (SlotDiff1 <= 0 && SlotDiff2 > TIME_OFFSET_SEND_TO_TX)))
            {
                /*lq 发射时隙到 */
                if ((currSlotSum + 1) % 2250 == pTxCtrlCell->TxSlot)    //lq CMX7042在收到发射任务的时隙的下一时隙开始发射
                {
                    /*lq CMX7042 Tx buffer 可用 */       
                    if (CMX7042_IsTxBufferFree() == TRUE)
                    {
                        pTxCtrlCell->State = CMXCOMM_TX_CTRL_STATE_TX_EXEC;
                        usart_printf("$CMX7042 TX State: TX_EXEC!\r\n");                                                                                                                                       
                    }                                      
                }                
            }
            else
            {
                pTxCtrlCell->State = CMXCOMM_TX_CTRL_STATE_IDLE;
                usart_printf("$CMX7042 TX State: IDLE!\r\n");             
            } 
                       
            break;

        case CMXCOMM_TX_CTRL_STATE_TX_EXEC:

            /*lq 收发转换 */
            if (pTxCtrlCell->TxChannel == AIS_CHANNEL_A)
            {
                RxTxCtrl(RXTX_CTRL_TYPE_TX, 0, AIS_FREQ_A);            
            }
            else
            {
                RxTxCtrl(RXTX_CTRL_TYPE_TX, 0, AIS_FREQ_B);                        
            }

            /*lq 传输消息数据 */
            CMX7042_DTaskDataBulkWrite(pTxCtrlCell->Data, pTxCtrlCell->DataLen * 8);

    	    /*lq 启动发射任务 */
    	    CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_Tx_TXB);

    		/*lq 设置发射超时定时 */
            pTxCtrlCell->TimeCnt = SetTimePeriodProcess(pTxCtrlCell->TimePeriod);
            pTxCtrlCell->State = CMXCOMM_TX_CTRL_STATE_DONE_WAITING;
            usart_printf("$CMX7042 TX State: DONE_WAITING!\r\n");                                                                                                                                                                                               

            break;

        case CMXCOMM_TX_CTRL_STATE_DONE_WAITING:

            if (CMX7042_IsTxDone() == TRUE)
            {            
                /*lq 发射状态判断 */
                txStatus = CMX7042_GetTxState();
                if (txStatus == CMX7042_TX_STATE_TxAborted_CSTDMA
                    | txStatus == CMX7042_TX_STATE_TxAborted_BuffNotReady
                    | txStatus == CMX7042_TX_STATE_TxAborted_MsgTooLong)
                {
            	    /*lq 启动终止发射任务 */
            	    CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_Tx_Abort);                
                }

                /*lq 收发转换 */
                RxTxCtrl(RXTX_CTRL_TYPE_RX, 0, AIS_FREQ_B);                                        

                pTxCtrlCell->State = CMXCOMM_TX_CTRL_STATE_IDLE;
                usart_printf("$CMX7042 TX State: IDLE!\r\n");                                                                                                                                                                                               
            }
            else
            {
                /*lq 发射超时 */
                if (pTxCtrlCell->TimeCnt == GetLocalClockTickProcess())
            	{
            	    /*lq 启动终止发射任务 */
            	    CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_Tx_Abort); 

                    /*lq 收发转换 */
                    RxTxCtrl(RXTX_CTRL_TYPE_RX, 0, AIS_FREQ_B);                                        
    
                    pTxCtrlCell->State = CMXCOMM_TX_CTRL_STATE_IDLE;
                    usart_printf("$CMX7042 TX timeout!\r\n");                                            
                    usart_printf("$CMX7042 TX State: IDLE!\r\n");                                            
            	}
            }

            break;

        default :
            
            /*lq 默认接收 */
            RxTxCtrl(RXTX_CTRL_TYPE_RX, 0, AIS_FREQ_B);
            pTxCtrlCell->State = CMXCOMM_TX_CTRL_STATE_IDLE;                                                
            usart_printf("$CMX7042 TX State: IDLE from default case!\r\n");                                                                                                                                                                                               
            
            break;
    }
}

/*******************************************************************************
* 名称  : CMXCOMM_GetRxData
* 描述  : 获取CMX7042接收消息
* 输入  : pCMX7042CommStruct - CMX7042CommStruct指针
          rxChannel - 接收机	  
* 输出  : 无
* 返回  : TRUE - 获取消息成功， FALSE - 获取消息失败
*******************************************************************************/
U8 CMXCOMM_GetRxData(CMX7042CommStruct* pCMX7042CommStruct, U8 rxChannel)
{
    CMXRecMsgCellStruct msgCell;
    U16 data[4];
    U16 data2[88];                  //lq CMX7042单个data buffer为88words
    U16 i;
    U16 temp;

    /*lq 读起始四个字 */
    CMX7042_DTaskDataRead(data, 64, TRUE, rxChannel);
    msgCell.BurstLen = data[0];
    msgCell.DCOffset = data[1];
    msgCell.ArrivalTime = data[2];
    msgCell.ArrivalSLot = data[3];
    msgCell.RxChannel = rxChannel;

    if (data[0] == 0)
    {
        /*lq 接收字节数为0 */
        return FALSE;    
    }

    temp = data[0] * 8;             //lq 总的比特数，（24bit SyncSeq + 8bit FrameHead + xbit data + 16bit FCS + 8bit FrameTail）

    /*lq 读内容 */    
    CMX7042_DTaskDataBulkRead(data2, temp, FALSE, rxChannel);

    temp = (data[0] - 1) / 2;           //lq 除去FrameTail
    for (i = 2; i < temp; i++)          //lq 前两个字为同步序列和帧头
    {
        msgCell.Data[(i - 2) * 2] = data2[i] & 0xFF;            //lq CMX7042word寄存器中存放字节的顺序为小端模式（靠前的字节数据存放在低字节）
        msgCell.Data[(i - 2) * 2 + 1] = (data2[i] >> 8) & 0xFF;
    }
    if ((data[0] - 1) % 2 != 0)//lqif ((data[0] - 3) % 2 != 0)
    {
        msgCell.Data[(i - 2) * 2] = data2[i] & 0xFF;
    }

    msgCell.BurstLen = data[0] - 7;     //lq 包中数据的长度

    temp = (msgCell.Data[data[0] - 5 - 2] << 8) | msgCell.Data[data[0] - 5 - 1];    //lq 接收到的校验码
    
    if (temp == AIS_Cal_CRC16(&msgCell.Data[0], msgCell.BurstLen))
    {
        CMXCOMM_RxEnque(pCMX7042CommStruct, &msgCell);
        
        return TRUE;
    }
    else
    {
        return FALSE;
    }            
}

/*******************************************************************************
* 名称  : CMXCOMM_IsRxQueFull
* 描述  : 接收消息队列是否已满
* 输入  : pCMX7042CommStruct - CMX7042CommStruct指针
* 输出  : 无
* 返回  : True - 已满，FALSE - 未满
*******************************************************************************/
U8 CMXCOMM_IsRxQueFull(CMX7042CommStruct* pCMX7042CommStruct)
{
    CMXRecMsgFIFOStruct* pFIFO;

    pFIFO = &(pCMX7042CommStruct->RxFIFO);
    if (((pFIFO->FIFOInput + 1) % BUFF_FIFO_DEPTH) == pFIFO->FIFOOutput)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : CMXCOMM_IsTxQueFull
* 描述  : 发送消息队列是否已满
* 输入  : pCMX7042CommStruct - CMX7042CommStruct指针
* 输出  : 无
* 返回  : True - 已满，FALSE - 未满
*******************************************************************************/
U8 CMXCOMM_IsTxQueFull(CMX7042CommStruct* const pCMX7042CommStruct)
{
    CMXTraMsgFIFOStruct* pFIFO;

    pFIFO = &(pCMX7042CommStruct->TxFIFO);
    if (((pFIFO->FIFOInput + 1) % BUFF_FIFO_DEPTH) == pFIFO->FIFOOutput)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : CMXCOMM_IsRxQueEmpty
* 描述  : 接收消息队列是否为空
* 输入  : pCMX7042CommStruct - CMX7042CommStruct指针
* 输出  : 无
* 返回  : True - 为空，FALSE - 非空
*******************************************************************************/
U8 CMXCOMM_IsRxQueEmpty(CMX7042CommStruct* pCMX7042CommStruct)
{
    CMXRecMsgFIFOStruct* pFIFO;

    pFIFO = &(pCMX7042CommStruct->RxFIFO);
    if (pFIFO->FIFOInput == pFIFO->FIFOOutput)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : CMXCOMM_IsTxQueEmpty
* 描述  : 发送消息队列是否为空
* 输入  : pCMX7042CommStruct - CMX7042CommStruct指针
* 输出  : 无
* 返回  : True - 为空，FALSE - 非空
*******************************************************************************/
U8 CMXCOMM_IsTxQueEmpty(CMX7042CommStruct* const pCMX7042CommStruct)
{
    CMXTraMsgFIFOStruct* pFIFO;

    pFIFO = &(pCMX7042CommStruct->TxFIFO);
    if (pFIFO->FIFOInput == pFIFO->FIFOOutput)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : CMXCOMM_RxEnque
* 描述  : 接收消息入队
* 输入  : pCMX7042CommStruct - CMX7042CommStruct指针
          pCMXRecMsgCellStruct - 消息单元指针	  
* 输出  : 无
* 返回  : True - 入队成功，FALSE - 入队失败
*******************************************************************************/
U8 CMXCOMM_RxEnque(CMX7042CommStruct* pCMX7042CommStruct, CMXRecMsgCellStruct* pCMXRecMsgCellStruct)
{
    CMXRecMsgFIFOStruct* pFIFO;
    U16 i;

    pFIFO = &(pCMX7042CommStruct->RxFIFO);
           
    if (CMXCOMM_IsRxQueFull(pCMX7042CommStruct) == FALSE)    //lq 队列不满
    {
    	pFIFO->FIFO[pFIFO->FIFOInput].BurstLen = pCMXRecMsgCellStruct->BurstLen;
    	pFIFO->FIFO[pFIFO->FIFOInput].DCOffset = pCMXRecMsgCellStruct->DCOffset;
    	pFIFO->FIFO[pFIFO->FIFOInput].ArrivalTime = pCMXRecMsgCellStruct->ArrivalTime;
        pFIFO->FIFO[pFIFO->FIFOInput].ArrivalSLot = pCMXRecMsgCellStruct->ArrivalSLot;
        pFIFO->FIFO[pFIFO->FIFOInput].RxChannel = pCMXRecMsgCellStruct->RxChannel;                
        for (i = 0; i < pCMXRecMsgCellStruct->BurstLen; i++)
        {
            pFIFO->FIFO[pFIFO->FIFOInput].Data[i] = pCMXRecMsgCellStruct->Data[i];        
        }

        pFIFO->FIFOInput = (pFIFO->FIFOInput + 1) % BUFF_FIFO_DEPTH;

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : CMXCOMM_TxEnque
* 描述  : 发送消息入队
* 输入  : pCMX7042CommStruct - CMX7042CommStruct指针
          pCMXTraMsgCellStruct - 发送消息单元指针	  
* 输出  : 无
* 返回  : True - 入队成功，FALSE - 入队失败
*******************************************************************************/
U8 CMXCOMM_TxEnque(CMX7042CommStruct* pCMX7042CommStruct, CMXTraMsgCellStruct* pCMXTraMsgCellStruct)
{
    CMXTraMsgFIFOStruct* pFIFO;
    U16 i;

    pFIFO = &(pCMX7042CommStruct->TxFIFO);
           
    if (CMXCOMM_IsTxQueFull(pCMX7042CommStruct) == FALSE)    //lq 队列不满
    {
    	pFIFO->FIFO[pFIFO->FIFOInput].DataLen = pCMXTraMsgCellStruct->DataLen;
    	pFIFO->FIFO[pFIFO->FIFOInput].TxSlot = pCMXTraMsgCellStruct->TxSlot;
    	pFIFO->FIFO[pFIFO->FIFOInput].TxChannel = pCMXTraMsgCellStruct->TxChannel;                       
        for (i = 0; i < pCMXTraMsgCellStruct->DataLen; i++)
        {
            pFIFO->FIFO[pFIFO->FIFOInput].Data[i] = pCMXTraMsgCellStruct->Data[i];        
        }

        pFIFO->FIFOInput = (pFIFO->FIFOInput + 1) % BUFF_FIFO_DEPTH;

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : CMXCOMM_RxDeque
* 描述  : 接收消息入队
* 输入  : pCMX7042CommStruct - CMX7042CommStruct指针
          pCMXRecMsgCellStruct - 消息单元指针	  
* 输出  : 无
* 返回  : True - 出队成功，FALSE - 出队失败
*******************************************************************************/
U8 CMXCOMM_RxDeque(CMX7042CommStruct* pCMX7042CommStruct, CMXRecMsgCellStruct* pCMXRecMsgCellStruct)
{
    CMXRecMsgFIFOStruct* pFIFO;
    U16 i;

    pFIFO = &(pCMX7042CommStruct->RxFIFO);
           
    if (CMXCOMM_IsRxQueEmpty(pCMX7042CommStruct) == FALSE)    //lq 队列非空
    {
    	pCMXRecMsgCellStruct->BurstLen = pFIFO->FIFO[pFIFO->FIFOOutput].BurstLen;
    	pCMXRecMsgCellStruct->DCOffset = pFIFO->FIFO[pFIFO->FIFOOutput].DCOffset;
    	pCMXRecMsgCellStruct->ArrivalTime = pFIFO->FIFO[pFIFO->FIFOOutput].ArrivalTime;
        pCMXRecMsgCellStruct->ArrivalSLot = pFIFO->FIFO[pFIFO->FIFOOutput].ArrivalSLot;
        pCMXRecMsgCellStruct->RxChannel = pFIFO->FIFO[pFIFO->FIFOOutput].RxChannel;
        for (i = 0; i < pFIFO->FIFO[pFIFO->FIFOOutput].BurstLen; i++)
        {
            pCMXRecMsgCellStruct->Data[i] = pFIFO->FIFO[pFIFO->FIFOOutput].Data[i]; 
        }

        pFIFO->FIFOOutput = (pFIFO->FIFOOutput + 1) % BUFF_FIFO_DEPTH;

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : CMXCOMM_TxDeque
* 描述  : 发送消息出队
* 输入  : pCMX7042CommStruct - CMX7042CommStruct指针
          pCMXTraMsgCellStruct - 发送消息单元指针	  
* 输出  : 无
* 返回  : True - 出队成功，FALSE - 出队失败
*******************************************************************************/
U8 CMXCOMM_TxDeque(CMX7042CommStruct* pCMX7042CommStruct, CMXTraMsgCellStruct* pCMXTraMsgCellStruct)
{
    CMXTraMsgFIFOStruct* pFIFO;
    U16 i;

    pFIFO = &(pCMX7042CommStruct->TxFIFO);
           
    if (CMXCOMM_IsTxQueEmpty(pCMX7042CommStruct) == FALSE)    //lq 队列非空
    {
    	pCMXTraMsgCellStruct->DataLen = pFIFO->FIFO[pFIFO->FIFOOutput].DataLen;
    	pCMXTraMsgCellStruct->TxSlot = pFIFO->FIFO[pFIFO->FIFOOutput].TxSlot;
    	pCMXTraMsgCellStruct->TxChannel = pFIFO->FIFO[pFIFO->FIFOOutput].TxChannel;
        for (i = 0; i < pFIFO->FIFO[pFIFO->FIFOOutput].DataLen; i++)
        {
            pCMXTraMsgCellStruct->Data[i] = pFIFO->FIFO[pFIFO->FIFOOutput].Data[i]; 
        }

        pFIFO->FIFOOutput = (pFIFO->FIFOOutput + 1) % BUFF_FIFO_DEPTH;

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : CMXCOMM_TxTest
* 描述  : 发送消息测试
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMXCOMM_TxTest(void)
{
    static U8 channel = AIS_CHANNEL_A;
    CMXTraMsgCellStruct traMsgCell;
    U16 i;
#if 1
    U8  data[53] = { 0x14,0x38
                    ,0x26,0x34
                    ,0xE0,0x02
                    ,0x33,0x9A
                    ,0x59,0xA2
                    ,0x51,0x53
                    ,0x30,0x00
                    ,0x14,0x41
                    ,0x19,0x4D
                    ,0x43,0xCE
                    ,0x14,0x00
                    ,0x00,0x00
                    ,0x00,0x00
                    ,0x00,0x00
                    ,0x00,0x4F
                    ,0x09,0xAB
                    ,0x4D,0x34
                    ,0x59,0xC6
                    ,0x00,0xE4
                    ,0x13,0x94
                    ,0x5C,0x54
                    ,0x90,0x00
                    ,0x00,0x00
                    ,0x00,0x00
                    ,0x00,0x00
                    ,0x00,0x00
                    ,0x00 };
#else
    U8  data[53] = { 0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55,0x55
                    ,0x55 };
#endif
    traMsgCell.DataLen = 53;
    for (i = 0; i < traMsgCell.DataLen; i++)
    {
        traMsgCell.Data[i] = data[i];
    }

    traMsgCell.TxSlot = (GetSlotNum() + TIME_OFFSET_SEND_TO_TX) % 2250;
    traMsgCell.TxChannel = channel;
#if 1
    channel = (channel == AIS_CHANNEL_A) ? AIS_CHANNEL_B : AIS_CHANNEL_A;
#else    
    channel = AIS_CHANNEL_A;
#endif
    CMXCOMM_TxEnque(&CMX7042CommInstance, &traMsgCell);
}

/*******************************************************************************
* 名称  : CMXCOMM_SimpleOperVeriTx
* 描述  : 随机发射测试
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMXCOMM_SimpleOperVeriTx(void)
{
    CMX7042_ExecModemTaskNoPara(0x002C);
    usart_printf("$CMX7042 : SimpleOperVeriTx!\r\n");                                                
}

/*******************************************************************************
* 名称  : CMXCOMM_SetTxTestFlg
* 描述  : 使能发送消息测试
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMXCOMM_SetTxTestFlg(void)
{
    CMX7042TestMsgFlg = TRUE;    
}

/*******************************************************************************
* 名称  : CMXCOMM_SetSovTxFlg
* 描述  : 使能随机发射测试
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMXCOMM_SetSovTxFlg(void)
{
    CMX7042SovTxFlg = TRUE;    
}

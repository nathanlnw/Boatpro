/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: LinkManagementEntity.c
  Author: lq       Version : V1.00        Date: 2014.07.11
  Description:     AIS链路管理实体层
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "Include.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static BinMsgRxFifoStruct BinMsgFifoRx1;    
static BinMsgRxFifoStruct BinMsgFifoRx2;    //lq 接收有两路，使用两个消息接收队列，避免冲突
static BinMsgTxFifoStruct BinMsgFifoTx;     //lq 发射只有一路，不会冲突
 
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* 名称  : LME_BinMsgRxFifoStructInit
* 描述  : 初始化结构体
* 输入  : pBinMsgRxFifoStruct - BinMsgRxFifoStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void LME_BinMsgRxFifoStructInit(BinMsgRxFifoStruct *pBinMsgRxFifoStruct)
{
    pBinMsgRxFifoStruct->Input = 0;
    pBinMsgRxFifoStruct->Output = 0;    
}

/*******************************************************************************
* 名称  : LME_BinMsgTxFifoStructInit
* 描述  : 初始化结构体
* 输入  : pBinMsgTxFifoStruct - BinMsgTxFifoStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void LME_BinMsgTxFifoStructInit(BinMsgTxFifoStruct *pBinMsgTxFifoStruct)
{
    pBinMsgTxFifoStruct->Input = 0;
    pBinMsgTxFifoStruct->Output = 0;    
}

/*******************************************************************************
* 名称  : LME_InstBinMsgFifoStructInit
* 描述  : 初始化结构体实例
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void LME_InstBinMsgFifoStructInit(void)
{
    LME_BinMsgRxFifoStructInit(&BinMsgFifoRx1);
    LME_BinMsgRxFifoStructInit(&BinMsgFifoRx2);
    LME_BinMsgTxFifoStructInit(&BinMsgFifoTx);
}

/*******************************************************************************
* 名称  : DLS_ModuleInit
* 描述  : 模块初始化
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void LME_ModuleInit(void)
{
    LME_InstBinMsgFifoStructInit();
}

/*******************************************************************************
* 名称  : LME_BinMsgRxFifoIsEmpty
* 描述  : 队列是否为空
* 输入  : pBinMsgRxFifoStruct - BinMsgRxFifoStruct结构体指针		  
* 输出  : 无
* 返回  : TRUE - 为空，FALSE - 非空
*******************************************************************************/
static U8 LME_BinMsgRxFifoIsEmpty(BinMsgRxFifoStruct *pBinMsgRxFifoStruct)
{
    if (pBinMsgRxFifoStruct->Input == pBinMsgRxFifoStruct->Output)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* 名称  : LME_BinMsgTxFifoIsEmpty
* 描述  : 队列是否为空
* 输入  : pBinMsgTxFifoStruct - BinMsgTxFifoStruct结构体指针		  
* 输出  : 无
* 返回  : TRUE - 为空，FALSE - 非空
*******************************************************************************/
static U8 LME_BinMsgTxFifoIsEmpty(BinMsgTxFifoStruct *pBinMsgTxFifoStruct)
{
    if (pBinMsgTxFifoStruct->Input == pBinMsgTxFifoStruct->Output)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* 名称  : LME_BinMsgRxFifoIsFull
* 描述  : 队列是否已满
* 输入  : pBinMsgRxFifoStruct - BinMsgRxFifoStruct结构体指针		  
* 输出  : 无
* 返回  : TRUE - 已满，FALSE - 未满
*******************************************************************************/
static U8 LME_BinMsgRxFifoIsFull(BinMsgRxFifoStruct *pBinMsgRxFifoStruct)
{
    if ((pBinMsgRxFifoStruct->Input + 1) % LME_BINMSG_FIFO_DEPTH
        == pBinMsgRxFifoStruct->Output)
    {
        return TRUE;
        
        debug_printf("$[%6d:%4d] %s : BinMsg rx fifo [%s]is full!\r\n",
            GetFrameNum(), GetSlotNum(), __func__,
            pBinMsgRxFifoStruct == &BinMsgFifoRx1 ? "A" : "B");
    }
    else
    {
        return FALSE;
    }     
}

/*******************************************************************************
* 名称  : LME_BinMsgTxFifoIsFull
* 描述  : 队列是否已满
* 输入  : pBinMsgTxFifoStruct - BinMsgTxFifoStruct结构体指针		  
* 输出  : 无
* 返回  : TRUE - 已满，FALSE - 未满
*******************************************************************************/
static U8 LME_BinMsgTxFifoIsFull(BinMsgTxFifoStruct *pBinMsgTxFifoStruct)
{
    if ((pBinMsgTxFifoStruct->Input + 1) % LME_BINMSG_FIFO_DEPTH
        == pBinMsgTxFifoStruct->Output)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }     
}

/*******************************************************************************
* 名称  : LME_BinMsgRxFifoEnque
* 描述  : 队列入队
* 输入  : pBinMsgRxFifoStruct - BinMsgRxFifoStruct结构体指针
          pBinMsgRxCellStruct - 指向入队数据单元的指针         		  
* 输出  : 无
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
static U8 LME_BinMsgRxFifoEnque(BinMsgRxFifoStruct *pBinMsgRxFifoStruct, const BinMsgRxCellStruct *pBinMsgRxCellStruct)
{
    BinMsgRxCellStruct *pCell;
    U16 i;
       
    if (LME_BinMsgRxFifoIsFull(pBinMsgRxFifoStruct) == FALSE)
    {      
        pCell = &(pBinMsgRxFifoStruct->FIFO[pBinMsgRxFifoStruct->Input]);
        
        pCell->DataLen = pBinMsgRxCellStruct->DataLen;
        for (i = 0; i < pBinMsgRxCellStruct->DataLen; i++)        
        {
            pCell->Data[i] = pBinMsgRxCellStruct->Data[i];         
        }
        pCell->RxChannel = pBinMsgRxCellStruct->RxChannel;
        pCell->ArrivalFrame = pBinMsgRxCellStruct->ArrivalFrame;
        pCell->ArrivalSlot = pBinMsgRxCellStruct->ArrivalSlot;
                
        pBinMsgRxFifoStruct->Input = (pBinMsgRxFifoStruct->Input + 1) % LME_BINMSG_FIFO_DEPTH;
        
        return TRUE;    
    }
    else
    {
        return FALSE;
    }   
}

/*******************************************************************************
* 名称  : LME_BinMsgTxFifoEnque
* 描述  : 队列入队
* 输入  : pBinMsgTxFifoStruct - BinMsgTxFifoStruct结构体指针
          pBinMsgTxCellStruct - 指向入队数据单元的指针        		  
* 输出  : 无
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
static U8 LME_BinMsgTxFifoEnque(BinMsgTxFifoStruct *pBinMsgTxFifoStruct,
                                const BinMsgTxCellStruct *pBinMsgTxCellStruct)
{
    BinMsgTxCellStruct *pCell;
    U16 i;
       
    if (LME_BinMsgTxFifoIsFull(pBinMsgTxFifoStruct) == FALSE)
    {      
        pCell = &(pBinMsgTxFifoStruct->FIFO[pBinMsgTxFifoStruct->Input]);
        
        pCell->DataLen = pBinMsgTxCellStruct->DataLen;
        for (i = 0; i < pBinMsgTxCellStruct->DataLen; i++)        
        {
            pCell->Data[i] = pBinMsgTxCellStruct->Data[i];         
        }
        pCell->TxChannel = pBinMsgTxCellStruct->TxChannel;
        pCell->TxSlot = pBinMsgTxCellStruct->TxSlot;
                
        pBinMsgTxFifoStruct->Input = (pBinMsgTxFifoStruct->Input + 1) % LME_BINMSG_FIFO_DEPTH;
        
        return TRUE;    
    }
    else
    {
        return FALSE;
    }   
}

/*******************************************************************************
* 名称  : LME_Inst1BinMsgRxFifoEnque
* 描述  : 队列入队
* 输入  : pBinMsgRxCellStruct - 指向入队数据单元的指针         		  
* 输出  : 无
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
U8 LME_Inst1BinMsgRxFifoEnque(const BinMsgRxCellStruct *pBinMsgRxCellStruct)
{
    return LME_BinMsgRxFifoEnque(&BinMsgFifoRx1, pBinMsgRxCellStruct);
}

/*******************************************************************************
* 名称  : LME_Inst2BinMsgRxFifoEnque
* 描述  : 队列入队
* 输入  : pBinMsgRxCellStruct - 指向入队数据单元的指针         		  
* 输出  : 无
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
U8 LME_Inst2BinMsgRxFifoEnque(const BinMsgRxCellStruct *pBinMsgRxCellStruct)
{
    return LME_BinMsgRxFifoEnque(&BinMsgFifoRx2, pBinMsgRxCellStruct);
}

/*******************************************************************************
* 名称  : LME_InstBinMsgTxFifoEnque
* 描述  : 队列入队
* 输入  : pBinMsgTxCellStruct - 指向入队数据单元的指针         		  
* 输出  : 无
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
U8 LME_InstBinMsgTxFifoEnque(const BinMsgTxCellStruct *pBinMsgTxCellStruct)
{
    return LME_BinMsgTxFifoEnque(&BinMsgFifoTx, pBinMsgTxCellStruct);
}

/*******************************************************************************
* 名称  : LME_BinMsgRxFifoDeque
* 描述  : 队列出队
* 输入  : pBinMsgRxFifoStruct - BinMsgRxFifoStruct结构体指针          
* 输出  : pBinMsgRxCellStruct - 指向出队数据单元的指针
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
static U8 LME_BinMsgRxFifoDeque(BinMsgRxFifoStruct *pBinMsgRxFifoStruct,
                                BinMsgRxCellStruct *pBinMsgRxCellStruct)
{
    U16 i;
    BinMsgRxCellStruct *pCell;

    if (LME_BinMsgRxFifoIsEmpty(pBinMsgRxFifoStruct) == FALSE)
    {        
        pCell = &(pBinMsgRxFifoStruct->FIFO[pBinMsgRxFifoStruct->Output]);
        
        pBinMsgRxCellStruct->DataLen = pCell->DataLen;
        for (i = 0; i < pCell->DataLen; i++)        
        {
            pBinMsgRxCellStruct->Data[i] = pCell->Data[i];         
        }
        pBinMsgRxCellStruct->RxChannel = pCell->RxChannel;
        pBinMsgRxCellStruct->ArrivalFrame = pCell->ArrivalFrame;
        pBinMsgRxCellStruct->ArrivalSlot = pCell->ArrivalSlot;

        pBinMsgRxFifoStruct->Output = (pBinMsgRxFifoStruct->Output + 1) % LME_BINMSG_FIFO_DEPTH;
        
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* 名称  : LME_BinMsgTxFifoDeque
* 描述  : 队列出队
* 输入  : pBinMsgTxFifoStruct - BinMsgTxFifoStruct结构体指针          
* 输出  : pBinMsgTxCellStruct - 指向出队数据单元的指针
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
static U8 LME_BinMsgTxFifoDeque(BinMsgTxFifoStruct *pBinMsgTxFifoStruct,
                                BinMsgTxCellStruct *pBinMsgTxCellStruct)
{
    U16 i;
    BinMsgTxCellStruct *pCell;

    if (LME_BinMsgTxFifoIsEmpty(pBinMsgTxFifoStruct) == FALSE)
    {        
        pCell = &(pBinMsgTxFifoStruct->FIFO[pBinMsgTxFifoStruct->Output]);
        
        pBinMsgTxCellStruct->DataLen = pCell->DataLen;
        for (i = 0; i < pCell->DataLen; i++)        
        {
            pBinMsgTxCellStruct->Data[i] = pCell->Data[i];         
        }
        pBinMsgTxCellStruct->TxChannel = pCell->TxChannel;
        pBinMsgTxCellStruct->TxSlot = pCell->TxSlot;

        pBinMsgTxFifoStruct->Output = (pBinMsgTxFifoStruct->Output + 1) % LME_BINMSG_FIFO_DEPTH;
        
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* 名称  : LME_Inst1BinMsgRxFifoDeque
* 描述  : 队列出队
* 输入  : 无
* 输出  : pBinMsgRxCellStruct - 指向出队数据单元的指针
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
U8 LME_Inst1BinMsgRxFifoDeque(BinMsgRxCellStruct *pBinMsgRxCellStruct)
{
    return LME_BinMsgRxFifoDeque(&BinMsgFifoRx1, pBinMsgRxCellStruct);
}

/*******************************************************************************
* 名称  : LME_Inst2BinMsgRxFifoDeque
* 描述  : 队列出队
* 输入  : 无
* 输出  : pBinMsgRxCellStruct - 指向出队数据单元的指针
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
U8 LME_Inst2BinMsgRxFifoDeque(BinMsgRxCellStruct *pBinMsgRxCellStruct)
{
    return LME_BinMsgRxFifoDeque(&BinMsgFifoRx2, pBinMsgRxCellStruct);
}

/*******************************************************************************
* 名称  : LME_InstBinMsgTxFifoDeque
* 描述  : 队列出队
* 输入  : 无
* 输出  : pBinMsgTxCellStruct - 指向出队数据单元的指针
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
U8 LME_InstBinMsgTxFifoDeque(BinMsgTxCellStruct *pBinMsgTxCellStruct)
{
    return LME_BinMsgTxFifoDeque(&BinMsgFifoTx, pBinMsgTxCellStruct);
}

/*******************************************************************************
* 名称  : LME_BinMsgRxFifoPortIn
* 描述  : 接收帧队列中的数据转入接收消息队列
* 输入  : pBinMsgRxFifoStruct - BinMsgRxFifoStruct结构体指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void LME_BinMsgRxFifoPortIn(BinMsgRxFifoStruct *pBinMsgRxFifoStruct)
{
    FrameRxCellStruct *pFrameRxCell;
    BinMsgRxCellStruct *pBinMsgRxCell;        
    U16 temp16;
    U16 i;
    U8 state;
#if 0
    U8 temp[2 * DLS_FRAME_MAX_LEN_BYTE + 30];
    U8 temp2[3];
#endif
    pFrameRxCell = (FrameRxCellStruct *)malloc(sizeof(FrameRxCellStruct));
    memset((FrameRxCellStruct *)pFrameRxCell, 0, sizeof(FrameRxCellStruct));

    if (pBinMsgRxFifoStruct == &BinMsgFifoRx1)
    {
        state = DLS_Inst1FrameRxFifoDeque(pFrameRxCell);
    }
    else if (pBinMsgRxFifoStruct == &BinMsgFifoRx2)
    {
        state = DLS_Inst2FrameRxFifoDeque(pFrameRxCell);    
    }        
    else
    {
        state = FALSE;
    }

    if (state == TRUE)
    {
        if (pFrameRxCell->DataLen >= 3)   //lq 帧长至少为3，1byte of data and 2bytes of CRC
        {
            temp16 = ((pFrameRxCell->Data[pFrameRxCell->DataLen - 2] << 8) & 0xFF00)
                      | (pFrameRxCell->Data[pFrameRxCell->DataLen - 1] & 0x00FF);    //lq CRC的高位在前 
            
            if (temp16 == AIS_Cal_CRC16(&pFrameRxCell->Data[0], pFrameRxCell->DataLen - 2))
            {
                pBinMsgRxCell = (BinMsgRxCellStruct *)malloc(sizeof(BinMsgRxCellStruct));
                memset((BinMsgRxCellStruct *)pBinMsgRxCell, 0, sizeof(BinMsgRxCellStruct));         
                       
                pBinMsgRxCell->DataLen = pFrameRxCell->DataLen - 2;                  //lq 不含CRC
                for (i = 0; i < pBinMsgRxCell->DataLen; i++)
                {
                    pBinMsgRxCell->Data[i] = pFrameRxCell->Data[i];
                }
                pBinMsgRxCell->RxChannel = pFrameRxCell->RxChannel;
                pBinMsgRxCell->ArrivalFrame = pFrameRxCell->ArrivalFrame;
                pBinMsgRxCell->ArrivalSlot = pFrameRxCell->ArrivalSlot;

                LME_BinMsgRxFifoEnque(pBinMsgRxFifoStruct, pBinMsgRxCell);                   
                
                free((BinMsgRxCellStruct *)pBinMsgRxCell);

                /*lq 接收测试响应报文 */
                if (RxTestInstance.RxTestEn == TRUE)
                {
                    if (pFrameRxCell->RxChannel == ENUM_AIS_CHANNEL_A)
                    {
                        UartResponseMsg("$P712,A,OK!"); 
                    }
                    else if (pFrameRxCell->RxChannel == ENUM_AIS_CHANNEL_B)
                    {
                        UartResponseMsg("$P712,B,OK!");                     
                    }
                }
            }
            else
            {
#if 0
                usart_printf("$FrameProcess: CRC error!\r\n");
#endif
            }
#if 0
            sprintf(temp, "$FrameProcess: ");            
            for (i = 0; i < pFrameRxCell->DataLen; i++)
            {
                sprintf(temp2, "%02X", pFrameRxCell->Data[i]);
                strcat(temp, temp2);
            }
            strcat(temp, "\r\n");
            usart_printf(temp);
#endif                                
        }               
    }
    
    free((FrameRxCellStruct *)pFrameRxCell);
}

/*******************************************************************************
* 名称  : LME_InstBinMsgRxFifoPortIn
* 描述  : 接收帧队列中的数据转入接收消息队列
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void LME_InstBinMsgRxFifoPortIn(void)
{
    LME_BinMsgRxFifoPortIn(&BinMsgFifoRx1);
    LME_BinMsgRxFifoPortIn(&BinMsgFifoRx2);
}

/*******************************************************************************
* 名称  : LME_BinMsgRxFifoPortOut
* 描述  : 将二进制消息接收队列中的消息转入消息接收列表
* 输入  : pBinMsgRxFifoStruct - BinMsgRxFifoStruct结构体指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void LME_BinMsgRxFifoPortOut(BinMsgRxFifoStruct *pBinMsgRxFifoStruct)
{
    BinMsgRxCellStruct *pBinMsgRxCell;        
    MsgCellStruct *pMsgCellStruct;
    U8  channel;

    pBinMsgRxCell = (BinMsgRxCellStruct *)malloc(sizeof(BinMsgRxCellStruct));
    memset((BinMsgRxCellStruct *)pBinMsgRxCell, 0, sizeof(BinMsgRxCellStruct));

    if (LME_BinMsgRxFifoDeque(pBinMsgRxFifoStruct, pBinMsgRxCell) == TRUE)
    {
        /*lq 输出VDM语句 */
        if (pBinMsgRxCell->RxChannel == ENUM_AIS_CHANNEL_A)
        {
            channel = AIS_CHANNEL_CHAR_A;
        }
        else
        {
            channel = AIS_CHANNEL_CHAR_B;
        }

        AISDI_BuildVDM(AIS_DI_TALKER_ID_DEFAULT, pBinMsgRxCell->Data,
            pBinMsgRxCell->DataLen, channel);                

        /*lq 将二进制消息接收队列中的消息转入消息接收列表 */
        pMsgCellStruct = (MsgCellStruct *)malloc(sizeof(MsgCellStruct));
        memset((MsgCellStruct *)pMsgCellStruct, 0, sizeof(MsgCellStruct));

        pMsgCellStruct->Channel = pBinMsgRxCell->RxChannel;

        pMsgCellStruct->Frame = pBinMsgRxCell->ArrivalFrame;
        pMsgCellStruct->Slot = pBinMsgRxCell->ArrivalSlot;

        memcpy(&(pMsgCellStruct->Data[1]), &(pBinMsgRxCell->Data[0]),
            pBinMsgRxCell->DataLen);

        pMsgCellStruct->Data[0] = pBinMsgRxCell->DataLen;
        pMsgCellStruct->DataLength = pMsgCellStruct->Data[0] + 1;
        pMsgCellStruct->MsgPriority = ENUM_AIS_MSG_PRIORITY_DEFAULT;  //lq 收到的消息的优先级相同，按接收顺序处理

        MSGMAN_RxMsgListInsert(pMsgCellStruct);
    
        free((MsgCellStruct *)pMsgCellStruct);
    }

    free((BinMsgRxCellStruct *)pBinMsgRxCell);
}

/*******************************************************************************
* 名称  : LME_InstBinMsgRxFifoPortOut
* 描述  : 将二进制消息发射队列中的消息转入消息发射列表
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void LME_InstBinMsgRxFifoPortOut(void)
{
    LME_BinMsgRxFifoPortOut(&BinMsgFifoRx1);
    LME_BinMsgRxFifoPortOut(&BinMsgFifoRx2);
}

/*******************************************************************************
* 名称  : LME_BinMsgTxFifoPortOut
* 描述  : 发射消息队列中的数据经过组帧处理后转入发射帧队列
* 输入  : pBinMsgTxFifoStruct - BinMsgTxFifoStruct结构体指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void LME_BinMsgTxFifoPortOut(BinMsgTxFifoStruct *pBinMsgTxFifoStruct)
{
    FrameTxCellStruct *pFrameTxCell;
    BinMsgTxCellStruct *pBinMsgTxCell;        
    U8 *pBuff;
    U8 *pBuff2;
    U16 temp16;
    U16 i;
    U8  channel;

    pBinMsgTxCell = (BinMsgTxCellStruct *)malloc(sizeof(BinMsgTxCellStruct));
    memset((BinMsgTxCellStruct *)pBinMsgTxCell, 0, sizeof(BinMsgTxCellStruct));
        
    if (LME_BinMsgTxFifoDeque(pBinMsgTxFifoStruct, pBinMsgTxCell) == TRUE)
    {
        if (pBinMsgTxCell->DataLen > 0)   //lq 帧长至少为3，1byte of data and 2bytes of CRC
        {
            /*lq 中间缓存申请 */
            pBuff = (U8 *)malloc((pBinMsgTxCell->DataLen + 2) * sizeof(U8));
            memset((U8 *)pBuff, 0, (pBinMsgTxCell->DataLen + 2) * sizeof(U8));
            pBuff[0] = pBinMsgTxCell->DataLen;
            pBuff[1] = 0;
            memcpy(&pBuff[2], &pBinMsgTxCell->Data[0], pBinMsgTxCell->DataLen);                
            
            pFrameTxCell = (FrameTxCellStruct *)malloc(sizeof(FrameTxCellStruct));
            memset((FrameTxCellStruct *)pFrameTxCell, 0, sizeof(FrameTxCellStruct));
            DLS_FrameBuild(&pBuff[0], &pFrameTxCell->Data[0]);      //lq 组帧
            free((U8 *)pBuff);
            pFrameTxCell->TxSlot = pBinMsgTxCell->TxSlot;
            pFrameTxCell->TxChannel = pBinMsgTxCell->TxChannel;

            if (DLS_InstFrameTxFifoEnque(pFrameTxCell) == TRUE)
            {
                /*lq 输出VDO语句 */
                if (pBinMsgTxCell->TxChannel == ENUM_AIS_CHANNEL_A)
                {
                    channel = AIS_CHANNEL_CHAR_A;
                }
                else
                {
                    channel = AIS_CHANNEL_CHAR_B;
                }

                AISDI_BuildVDO(AIS_DI_TALKER_ID_DEFAULT, pBinMsgTxCell->Data,
                    pBinMsgTxCell->DataLen, channel);                

            }
            free((FrameTxCellStruct *)pFrameTxCell);
        }               
    }
    
    free((BinMsgTxCellStruct *)pBinMsgTxCell);
}

/*******************************************************************************
* 名称  : LME_InstBinMsgTxFifoPortOut
* 描述  : 发射消息队列中的数据经过组帧处理后转入发射帧队列
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void LME_InstBinMsgTxFifoPortOut(void)
{
    LME_BinMsgTxFifoPortOut(&BinMsgFifoTx);
}

/*******************************************************************************
* 名称  : LME_BinMsgTxFifoPortIn
* 描述  : 将消息发射列表中的消息转入二进制消息发射队列
* 输入  : pBinMsgTxFifoStruct - BinMsgTxFifoStruct结构体指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void LME_BinMsgTxFifoPortIn(BinMsgTxFifoStruct *pBinMsgTxFifoStruct)
{
    BinMsgTxCellStruct *pBinMsgTxCell;        
    MsgCellStruct *pMsgCellStruct;

    pMsgCellStruct = (MsgCellStruct *)malloc(sizeof(MsgCellStruct));
    memset((MsgCellStruct *)pMsgCellStruct, 0, sizeof(MsgCellStruct));

    if (MSGMAN_TxMsgListGetElem(pMsgCellStruct) == TRUE)
    {
        pBinMsgTxCell = (BinMsgTxCellStruct *)malloc(sizeof(BinMsgTxCellStruct));
        memset((BinMsgTxCellStruct *)pBinMsgTxCell, 0, sizeof(BinMsgTxCellStruct));

        pBinMsgTxCell->TxChannel = pMsgCellStruct->Channel;
        pBinMsgTxCell->TxSlot = pMsgCellStruct->Slot;

        memcpy(&pBinMsgTxCell->Data[0], &(pMsgCellStruct->Data[1]),
            pMsgCellStruct->Data[0]);

        pBinMsgTxCell->DataLen = pMsgCellStruct->Data[0];
        
        LME_BinMsgTxFifoEnque(pBinMsgTxFifoStruct, pBinMsgTxCell);

        free((BinMsgTxCellStruct *)pBinMsgTxCell);
    }

    free((MsgCellStruct *)pMsgCellStruct);
}

/*******************************************************************************
* 名称  : LME_InstBinMsgTxFifoPortIn
* 描述  : 发射消息队列中的数据经过组帧处理后转入发射帧队列
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void LME_InstBinMsgTxFifoPortIn(void)
{
    LME_BinMsgTxFifoPortIn(&BinMsgFifoTx);
}

/*******************************************************************************
* 名称  : LME_BinMsgPort
* 描述  : 实现消息数据在接收/发射帧队列、接收/发射消息队列和接收/发射消息列表之
          间的转移
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void LME_BinMsgPort(void)
{
    LME_InstBinMsgTxFifoPortIn();
    LME_InstBinMsgTxFifoPortOut();
    LME_InstBinMsgRxFifoPortIn();
    LME_InstBinMsgRxFifoPortOut();
}

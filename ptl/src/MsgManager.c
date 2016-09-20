/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: MsgManager.c
  Author: lq       Version : V1.0.0        Date: 2015.06.23
  Description:     消息管理模块
  Version:         V1.0.0
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "Include.h"

/* Private typedef -----------------------------------------------------------*/
static MsgStaticListStruct TxMsgList;
static MsgStaticListStruct RxMsgList;
static PeriodSchedStruct PeriodSched;
static BurstSchedStruct BurstSched;

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* 名称  : MSGMAN_MsgCellStructInit
* 描述  : 初始化结构体
* 输入  : pMsgCellStruct - MsgCellStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void MSGMAN_MsgCellStructInit(MsgCellStruct *pMsgCellStruct)
{
    pMsgCellStruct->MsgType = AISMsgTypeError;
    pMsgCellStruct->MsgPriority = ENUM_AIS_MSG_PRIORITY_DEFAULT;
    pMsgCellStruct->Channel = ENUM_AIS_CHANNEL_A;
    pMsgCellStruct->Slot = SLOT_INVALID_VALUE;
    pMsgCellStruct->Frame = FRAME_INVALID_VALUE;
    pMsgCellStruct->MsgState = FALSE;
	pMsgCellStruct->DataLength = 0;
}

/*******************************************************************************
* 名称  : MSGMAN_MsgStaticListElemStructInit
* 描述  : 初始化结构体
* 输入  : pMsgStaticListElemStruct - MsgStaticListElemStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void MSGMAN_MsgStaticListElemStructInit(
    MsgStaticListElemStruct *pMsgStaticListElemStruct)
{
    MSGMAN_MsgCellStructInit(&pMsgStaticListElemStruct->MsgCell);
    pMsgStaticListElemStruct->NextIndex = STATIC_LIST_INDEX_NULL;
}

/*******************************************************************************
* 名称  : MSGMAN_MsgTxTaskStructInit
* 描述  : 初始化结构体
* 输入  : pMsgTxTaskStruct - MsgTxTaskStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void MSGMAN_MsgTxTaskStructInit(MsgTxTaskStruct *pMsgTxTaskStruct)
{
    pMsgTxTaskStruct->Channel = ENUM_AIS_CHANNEL_A;
    pMsgTxTaskStruct->NominalFrame = FRAME_INVALID_VALUE;
    pMsgTxTaskStruct->NominalSlot = SLOT_INVALID_VALUE;
    pMsgTxTaskStruct->TxFrame = FRAME_INVALID_VALUE;
    pMsgTxTaskStruct->TxSlot = SLOT_INVALID_VALUE;
    pMsgTxTaskStruct->TxSlotInterval = AIS_SLOT_INTERVAL_FIXED_PERIOD_TX_PERIOD;    //lq 双信道6min
    pMsgTxTaskStruct->ValidFlag = FALSE;
}

/*******************************************************************************
* 名称  : MSGMAN_PeriodSchedStructInit
* 描述  : 初始化结构体
* 输入  : PeriodSchedStruct - pPeriodSchedStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void MSGMAN_PeriodSchedStructInit(PeriodSchedStruct *pPeriodSchedStruct)
{
    MSGMAN_MsgTxTaskStructInit(&(pPeriodSchedStruct->MsgTxTaskMsg19));
    MSGMAN_MsgTxTaskStructInit(&(pPeriodSchedStruct->MsgTxTaskMsg24A));
    MSGMAN_MsgTxTaskStructInit(&(pPeriodSchedStruct->MsgTxTaskMsg24B));
	MSGMAN_MsgTxTaskStructInit(&(pPeriodSchedStruct->MsgTxTaskMsg5));
    pPeriodSchedStruct->CtrlState = ENUM_PERIOD_SCHED_CTRL_STATE_IDLE;
}

/*******************************************************************************
* 名称  : MSGMAN_AisAckCellStructInit
* 描述  : 初始化结构体
* 输入  : pAisAckCellStruct - AisAckCellStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void MSGMAN_AisAckCellStructInit(AisAckCellStruct *pAisAckCellStruct)
{
    pAisAckCellStruct->DestinationID = 0;
    pAisAckCellStruct->SequenceNumber = 0;    
}

/*******************************************************************************
* 名称  : MSGMAN_AisAckCellStructCopy
* 描述  : 结构体复制
* 输入  : pDst - 目的指针
          pSrc - 源指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void MSGMAN_AisAckCellStructCopy(AisAckCellStruct *pDst,
                                        const AisAckCellStruct *pSrc)
{
    pDst->DestinationID = pSrc->DestinationID;
    pDst->SequenceNumber = pSrc->SequenceNumber;    
}

/*******************************************************************************
* 名称  : MSGMAN_AisAckFifoStructInit
* 描述  : 初始化结构体
* 输入  : pAisAckFifoStruct - AisAckFifoStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void MSGMAN_AisAckFifoStructInit(AisAckFifoStruct *pAisAckFifoStruct)
{
    S16 i;

    for (i = 0; i < MSG_MAN_ACK_FIFO_LEN; i++)
    {
        MSGMAN_AisAckCellStructInit(&(pAisAckFifoStruct->FIFO[i]));
    }

    pAisAckFifoStruct->Input = 0;
    pAisAckFifoStruct->Output = 0;    
}

/*******************************************************************************
* 名称  : MSGMAN_BurstSchedStructInit
* 描述  : 初始化结构体
* 输入  : pBurstSchedStruct - BurstSchedStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void MSGMAN_BurstSchedStructInit(BurstSchedStruct *pBurstSchedStruct)
{
    MSGMAN_AisAckFifoStructInit(&(pBurstSchedStruct->Msg6AckA));
    MSGMAN_AisAckFifoStructInit(&(pBurstSchedStruct->Msg6AckB));
    MSGMAN_AisAckFifoStructInit(&(pBurstSchedStruct->Msg12AckA));
    MSGMAN_AisAckFifoStructInit(&(pBurstSchedStruct->Msg12AckB));
}

/*******************************************************************************
* 名称  : MSGMAN_MsgTxTaskTxCfg
* 描述  : 周期消息发射配置
* 输入  : msgType - 消息类型
          channel - 发射信道
          frame - 发射帧号
          slot - 发射时隙号
* 输出  : 无
* 返回  : 配置是否成功
*******************************************************************************/
void MSGMAN_MsgTxTaskTxCfg(AISMsgTypeEnum msgType, AisChannelEnum channel,
                           S32 frame, S16 slot)
{
    MsgTxTaskStruct *pMsgTxTaskStruct;
    
    switch (msgType)
    {
        case AISMsgType19 :

            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg19);

            break;

        case AISMsgType24A :
            
            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg24A);

            break;

        case AISMsgType24B :
            
            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg24B);

            break;

		case AISMsgType5 :
            
            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg5);

            break;

        default :
            
            return;
    }

    pMsgTxTaskStruct->Channel = channel;
    pMsgTxTaskStruct->NominalFrame = frame;
    pMsgTxTaskStruct->NominalSlot = slot;
    pMsgTxTaskStruct->TxFrame = frame;
    pMsgTxTaskStruct->TxSlot = slot;
    pMsgTxTaskStruct->ValidFlag = TRUE;
}

/*******************************************************************************
* 名称  : MSGMAN_MsgTxTaskNextTxCfg
* 描述  : 下次发射配置
* 输入  : msgType - 消息类型
* 输出  : 无
* 返回  : 配置是否成功
*******************************************************************************/
U8 MSGMAN_MsgTxTaskNextTxCfg(AISMsgTypeEnum msgType)
{
    MsgTxTaskStruct *pMsgTxTaskStruct;
    S32 randValue;

    switch (msgType)
    {
        case AISMsgType19 :

            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg19);

            break;

        case AISMsgType24A :
            
            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg24A);

            break;

        case AISMsgType24B :
            
            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg24B);

            break;

		case AISMsgType5 :
            
            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg5);

            break;

        default :
            
            return FALSE;
    }

    if (pMsgTxTaskStruct->ValidFlag == FALSE)
    {
        return FALSE;
    }

    if (pMsgTxTaskStruct->Channel == ENUM_AIS_CHANNEL_A)
    {
        pMsgTxTaskStruct->Channel = ENUM_AIS_CHANNEL_B;    
    }
    else
    {
        pMsgTxTaskStruct->Channel = ENUM_AIS_CHANNEL_A;    
    }

    /*lq 计算下一标称时隙 */
    pMsgTxTaskStruct->NominalFrame =
        CalcFrameNum(pMsgTxTaskStruct->NominalFrame,
            pMsgTxTaskStruct->NominalSlot,
            pMsgTxTaskStruct->TxSlotInterval);
    
    pMsgTxTaskStruct->NominalSlot =
        CalcSlotNum(pMsgTxTaskStruct->NominalFrame,
            pMsgTxTaskStruct->NominalSlot,
            pMsgTxTaskStruct->TxSlotInterval);

    /*lq 计算下一发射时隙 */
    randValue = Random(-AIS_SLOT_INTERVAL_SLOT_SELECT_OFFSET_MAX,
        AIS_SLOT_INTERVAL_SLOT_SELECT_OFFSET_MAX);

    pMsgTxTaskStruct->TxFrame =
        CalcFrameNum(pMsgTxTaskStruct->NominalFrame,
            pMsgTxTaskStruct->NominalSlot,
            randValue);
    
    pMsgTxTaskStruct->TxSlot =
        CalcSlotNum(pMsgTxTaskStruct->NominalFrame,
            pMsgTxTaskStruct->NominalSlot,
            randValue);

    return TRUE;
}

/*******************************************************************************
* 名称  : MSGMAN_MsgTxTaskIsTxTimExpired
* 描述  : 发射定时是否超时
* 输入  : msgType - 消息类型
          frame - 当前帧号
          slot - 当前时隙号
* 输出  : 无
* 返回  : 是否超时
*******************************************************************************/
U8 MSGMAN_MsgTxTaskIsTxTimExpired(AISMsgTypeEnum msgType, S32 frame, S16 slot)
{
    MsgTxTaskStruct *pMsgTxTaskStruct;

    switch (msgType)
    {
        case AISMsgType19 :

            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg19);

            break;

        case AISMsgType24A :
            
            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg24A);

            break;

        case AISMsgType24B :
            
            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg24B);

            break;

		case AISMsgType5 :
            
            pMsgTxTaskStruct = &(PeriodSched.MsgTxTaskMsg5);

            break;

        default :
            
            return FALSE;
    }

    if (pMsgTxTaskStruct->ValidFlag == FALSE)
    {
        return FALSE;
    }

    if (CalcSlotOffset(pMsgTxTaskStruct->TxFrame, pMsgTxTaskStruct->TxSlot,
        frame, slot) >= 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : MSGMAN_MsgTxTaskIsSendTimExpired
* 描述  : 发送（送入发射队列）定时是否超时
* 输入  : msgType - 消息类型
          frame - 当前帧号
          slot - 当前时隙号
* 输出  : 无
* 返回  : 是否超时
*******************************************************************************/
U8 MSGMAN_MsgTxTaskIsSendTimExpired(AISMsgTypeEnum msgType, S32 frame, S16 slot)
{
    return MSGMAN_MsgTxTaskIsTxTimExpired(msgType,
            CalcFrameNum(frame, slot, AIS_SLOT_INTERVAL_SEND_TO_TX),
            CalcSlotNum(frame, slot, AIS_SLOT_INTERVAL_SEND_TO_TX));
}

/*******************************************************************************
* 名称  : MSGMAN_PeriodSchedule
* 描述  : 固定周期类消息（msg19/msg24A/msg24B）发送 管理
* 输入  : pPeriodSchedStruct - PeriodSchedStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void MSGMAN_PeriodSchedule(PeriodSchedStruct *pPeriodSchedStruct)
{
    MsgTxTaskStruct *pMsgTxTaskMsg5;
	MsgTxTaskStruct *pMsgTxTaskMsg19;
    MsgTxTaskStruct *pMsgTxTaskMsg24A;
    MsgTxTaskStruct *pMsgTxTaskMsg24B;
    S32 currFrame;
    S16 currSlot;

	if (DevStaticPara.AisClass == AIS_CLASS_B)    
    {
	    pMsgTxTaskMsg19 = &(pPeriodSchedStruct->MsgTxTaskMsg19);
	    pMsgTxTaskMsg24A = &(pPeriodSchedStruct->MsgTxTaskMsg24A);
	    pMsgTxTaskMsg24B = &(pPeriodSchedStruct->MsgTxTaskMsg24B);
	}
	else
	{
		pMsgTxTaskMsg5 = &(pPeriodSchedStruct->MsgTxTaskMsg5);
	}

    currFrame = GetFrameNum();
    currSlot = GetSlotNum();

    switch (pPeriodSchedStruct->CtrlState)
    {
        case ENUM_PERIOD_SCHED_CTRL_STATE_IDLE :
            
            if (GetGpsPowerupFixState() == TRUE)
            {  
				if (DevStaticPara.AisClass == AIS_CLASS_B)    
				{          
	                /*lq 19号消息首次发射定时 */
	                MSGMAN_MsgTxTaskTxCfg(AISMsgType19, ENUM_AIS_CHANNEL_A,
	                    CalcFrameNum(currFrame, currSlot,
	                        AIS_SLOT_INTERVAL_SEND_TO_TX),
	                    CalcSlotNum(currFrame, currSlot,
	                        AIS_SLOT_INTERVAL_SEND_TO_TX));
	
	                /*lq 24A号消息首次发射定时 */
	                MSGMAN_MsgTxTaskTxCfg(AISMsgType24A, ENUM_AIS_CHANNEL_A,
	                    CalcFrameNum(pMsgTxTaskMsg19->NominalFrame,
	                        pMsgTxTaskMsg19->NominalSlot,
	                        AIS_SLOT_INTERVAL_MSG24A_TO_MSG19
	                        + AIS_SLOT_INTERVAL_SEND_TO_TX),
	                    CalcSlotNum(pMsgTxTaskMsg19->NominalFrame,
	                        pMsgTxTaskMsg19->NominalSlot,
	                        AIS_SLOT_INTERVAL_MSG24A_TO_MSG19
	                        + AIS_SLOT_INTERVAL_SEND_TO_TX));
	                
	                /*lq 24B号消息首次发射定时 */
	                MSGMAN_MsgTxTaskTxCfg(AISMsgType24B, ENUM_AIS_CHANNEL_A,
	                    CalcFrameNum(pMsgTxTaskMsg24A->NominalFrame,
	                        pMsgTxTaskMsg24A->NominalSlot,
	                        AIS_SLOT_INTERVAL_MSG24B_TO_MSG24A
	                        + AIS_SLOT_INTERVAL_SEND_TO_TX),
	                    CalcSlotNum(pMsgTxTaskMsg24A->NominalFrame,
	                        pMsgTxTaskMsg24A->NominalSlot,
	                        AIS_SLOT_INTERVAL_MSG24B_TO_MSG24A
	                        + AIS_SLOT_INTERVAL_SEND_TO_TX));
				}
				else
				{
					/*lq 5号消息首次发射定时 */
	                MSGMAN_MsgTxTaskTxCfg(AISMsgType5, ENUM_AIS_CHANNEL_A,
	                    CalcFrameNum(currFrame, currSlot,
	                        AIS_SLOT_INTERVAL_SEND_TO_TX),
	                    CalcSlotNum(currFrame, currSlot,
	                        AIS_SLOT_INTERVAL_SEND_TO_TX));			                                    
	        	}
				pPeriodSchedStruct->CtrlState =
	                    ENUM_PERIOD_SCHED_CTRL_STATE_RUNNIG;
	                    
                debug_printf("$[%6d:%4d] %s : Ctrl state to RUNNING!\r\n",
                    GetFrameNum(), GetSlotNum(), __func__);
			}

            break;

        case ENUM_PERIOD_SCHED_CTRL_STATE_RUNNIG:
            
            if (DevStaticPara.AisClass == AIS_CLASS_B)    
			{ 
				/*lq 19号消息发射判断 */
	            if (MSGMAN_MsgTxTaskIsSendTimExpired(AISMsgType19, currFrame, currSlot)
	                == TRUE)
	            {
	                SendMsg19(pMsgTxTaskMsg19->Channel, pMsgTxTaskMsg19->TxSlot);
	                MSGMAN_MsgTxTaskNextTxCfg(AISMsgType19);
	            }
	
	            /*lq 24A号消息发射判断 */
	            if (MSGMAN_MsgTxTaskIsSendTimExpired(AISMsgType24A, currFrame, currSlot)
	                == TRUE)
	            {
	                SendMsg24A(pMsgTxTaskMsg24A->Channel, pMsgTxTaskMsg24A->TxSlot);
	                MSGMAN_MsgTxTaskNextTxCfg(AISMsgType24A);
	            }
	
	            /*lq 24B号消息发射判断 */
	            if (MSGMAN_MsgTxTaskIsSendTimExpired(AISMsgType24B, currFrame, currSlot)
	                == TRUE)
	            {
	                SendMsg24B(pMsgTxTaskMsg24B->Channel, pMsgTxTaskMsg24B->TxSlot);
	                MSGMAN_MsgTxTaskNextTxCfg(AISMsgType24B);
	            }
			}
			else
			{	
				/*lq 5号消息发射判断 */
				if (MSGMAN_MsgTxTaskIsSendTimExpired(AISMsgType5, currFrame, currSlot)
	                == TRUE)
	            {
	                SendMsg5(pMsgTxTaskMsg5->Channel, pMsgTxTaskMsg5->TxSlot);
	                MSGMAN_MsgTxTaskNextTxCfg(AISMsgType5);
				}
			}

            break;
        
        default :
            
            MSGMAN_PeriodSchedStructInit(pPeriodSchedStruct);
            
            break;
    }
        
}

/*******************************************************************************
* 名称  : MSGMAN_MsgStaticListStructInit
* 描述  : 初始化结构体
* 输入  : pMsgStaticListStruct - MsgStaticListStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void MSGMAN_MsgStaticListStructInit(
    MsgStaticListStruct *pMsgStaticListStruct)
{
    S16 i;

    for (i = 0; i < MSG_MAN_MSG_STATIC_LIST_LEN; i++)
    {
        MSGMAN_MsgCellStructInit(&(pMsgStaticListStruct->StaticList[i].MsgCell));
        pMsgStaticListStruct->StaticList[i].NextIndex = i + 1;
    }
    pMsgStaticListStruct->StaticList[MSG_MAN_MSG_STATIC_LIST_LEN - 1].NextIndex
        = STATIC_LIST_INDEX_NULL;

    pMsgStaticListStruct->Head = STATIC_LIST_INDEX_NULL;
    pMsgStaticListStruct->FreeHead = 0;
}

/*******************************************************************************
* 名称  : MSGMAN_MsgCellStructCopy
* 描述  : 结构体复制
* 输入  : pMsgCellDst - 目的
          pMsgCellSrc - 源
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void MSGMAN_MsgCellStructCopy(MsgCellStruct *pMsgCellDst,
                                     const MsgCellStruct *pMsgCellSrc)
{
    S16 i;

    pMsgCellDst->MsgType = pMsgCellSrc->MsgType;
    pMsgCellDst->MsgPriority = pMsgCellSrc->MsgPriority;
    pMsgCellDst->Channel = pMsgCellSrc->Channel;
    pMsgCellDst->Slot = pMsgCellSrc->Slot;
    pMsgCellDst->Frame = pMsgCellSrc->Frame;
    pMsgCellDst->MsgState = pMsgCellSrc->MsgState;
	pMsgCellDst->DataLength = pMsgCellSrc->DataLength;   
    for (i = 0; i < pMsgCellSrc->DataLength; i++)
    {
        pMsgCellDst->Data[i] = pMsgCellSrc->Data[i];
    }
}

/*******************************************************************************
* 名称  : MSGMAN_MsgStaticListIsEmpty
* 描述  : 静态链表是否为空
* 输入  : pMsgStaticListStruct - MsgStaticListStruct结构体指针	  
* 输出  : 无
* 返回  : 是否为空
*******************************************************************************/
static U8 MSGMAN_MsgStaticListIsEmpty(MsgStaticListStruct *pMsgStaticListStruct)
{
    if (pMsgStaticListStruct->Head == STATIC_LIST_INDEX_NULL)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : MSGMAN_MsgStaticListIsFull
* 描述  : 静态链表是否为空
* 输入  : pMsgStaticListStruct - MsgStaticListStruct结构体指针	  
* 输出  : 无
* 返回  : 是否已满
*******************************************************************************/
static U8 MSGMAN_MsgStaticListIsFull(MsgStaticListStruct *pMsgStaticListStruct)
{
    if (pMsgStaticListStruct->FreeHead == STATIC_LIST_INDEX_NULL)
    {
        return TRUE;

        debug_printf("$[%6d:%4d] %s : Msg static list is full!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : MSGMAN_MsgStaticListInsert
* 描述  : 静态链表插入元素，按消息优先级排序
* 输入  : pMsgStaticListStruct - 静态链表指针	  
          pMsgCellStruct - 指向存放插入元素指针
* 输出  : 无
* 返回  : 是否插入成功
*******************************************************************************/
static U8 MSGMAN_MsgStaticListInsert(MsgStaticListStruct *pMsgStaticListStruct,
                                     const MsgCellStruct *pMsgCellStruct)
{
    S16 prior;
    S16 curr;
    S16 temp;

    if (MSGMAN_MsgStaticListIsFull(pMsgStaticListStruct) == FALSE)
    {
        prior = STATIC_LIST_INDEX_NULL;
        curr = pMsgStaticListStruct->Head;
        
        /*lq 根据优先级选择插入点 */
        while (curr != STATIC_LIST_INDEX_NULL)
        {
            if (pMsgCellStruct->MsgPriority
                >= pMsgStaticListStruct->StaticList[curr].MsgCell.MsgPriority)
            {
                prior = curr;
                curr = pMsgStaticListStruct->StaticList[curr].NextIndex;
            }
            else
            {
                /*lq 插入点为curr之前 */
                break;
            }            
        }

        /*lq 链表插入元素赋值 */
        MSGMAN_MsgCellStructCopy(&(pMsgStaticListStruct->StaticList[
            pMsgStaticListStruct->FreeHead].MsgCell),
            pMsgCellStruct);
        
        /*lq 更新链表索引 */
        if (curr == STATIC_LIST_INDEX_NULL)
        {
            /*lq 在尾部插入 */
            if (prior == STATIC_LIST_INDEX_NULL)
            {
                /*lq 链表为空 */
                pMsgStaticListStruct->Head = pMsgStaticListStruct->FreeHead;
                
                pMsgStaticListStruct->FreeHead
                    = pMsgStaticListStruct->StaticList[
                    pMsgStaticListStruct->FreeHead].NextIndex;
                
                pMsgStaticListStruct->StaticList[pMsgStaticListStruct->Head].NextIndex
                    = STATIC_LIST_INDEX_NULL;
            }
            else
            {
                /*lq 链表非空 */
                pMsgStaticListStruct->StaticList[prior].NextIndex
                    = pMsgStaticListStruct->FreeHead;
                
                pMsgStaticListStruct->FreeHead
                    = pMsgStaticListStruct->StaticList[
                    pMsgStaticListStruct->FreeHead].NextIndex;
                
                pMsgStaticListStruct->StaticList[
                    pMsgStaticListStruct->StaticList[prior].NextIndex].NextIndex
                    = STATIC_LIST_INDEX_NULL;
            }
        }
        else
        {
            /*lq 在链表中插入 */ 
            if (prior == STATIC_LIST_INDEX_NULL)
            {
                /*lq 在链表头之前插入 */
                temp = pMsgStaticListStruct->FreeHead;
                pMsgStaticListStruct->FreeHead = pMsgStaticListStruct->StaticList[temp].NextIndex;
                pMsgStaticListStruct->StaticList[temp].NextIndex = pMsgStaticListStruct->Head;
                pMsgStaticListStruct->Head = temp;
            }
            else
            {
                temp = pMsgStaticListStruct->FreeHead;
                pMsgStaticListStruct->FreeHead = pMsgStaticListStruct->StaticList[temp].NextIndex;
                pMsgStaticListStruct->StaticList[temp].NextIndex = curr;
                pMsgStaticListStruct->StaticList[prior].NextIndex = temp;               
            }
        }

        return TRUE;
    } 
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : MSGMAN_MsgStaticListInsert
* 描述  : 获取并删除表头元素
* 输入  : pMsgStaticListStruct - 静态链表指针
* 输出  : pMsgCellStruct - 指向存放返回元素的指针
* 返回  : 是否获取成功
*******************************************************************************/
static U8 MSGMAN_MsgStaticListGetHead(MsgStaticListStruct *pMsgStaticListStruct,
                                      MsgCellStruct *pMsgCellStruct)
{
    S16 temp;

    if (MSGMAN_MsgStaticListIsEmpty(pMsgStaticListStruct) == FALSE)
    {
        /*lq 获取元素赋值 */
        MSGMAN_MsgCellStructCopy(pMsgCellStruct,
            &(pMsgStaticListStruct->StaticList[
            pMsgStaticListStruct->Head].MsgCell));

        /*lq 更新静态链表索引 */
        temp = pMsgStaticListStruct->Head;
        pMsgStaticListStruct->Head = pMsgStaticListStruct->StaticList[temp].NextIndex;
        pMsgStaticListStruct->StaticList[temp].NextIndex = pMsgStaticListStruct->FreeHead;
        pMsgStaticListStruct->FreeHead = temp;
        
        return TRUE;
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* 名称  : MSGMAN_TxMsgListIsEmpty
* 描述  : 消息发射链表是否为空
* 输入  : 无
* 输出  : 无
* 返回  : 是否为空
*******************************************************************************/
U8 MSGMAN_TxMsgListIsEmpty(void)
{
    return MSGMAN_MsgStaticListIsEmpty(&TxMsgList);
}

/*******************************************************************************
* 名称  : MSGMAN_RxMsgListIsEmpty
* 描述  : 消息接收链表是否为空
* 输入  : 无
* 输出  : 无
* 返回  : 是否为空
*******************************************************************************/
U8 MSGMAN_RxMsgListIsEmpty(void)
{
    return MSGMAN_MsgStaticListIsEmpty(&RxMsgList);
}

/*******************************************************************************
* 名称  : MSGMAN_TxMsgListIsFull
* 描述  : 消息发射链表是否已满
* 输入  : 无
* 输出  : 无
* 返回  : 是否已满
*******************************************************************************/
U8 MSGMAN_TxMsgListIsFull(void)
{
    return MSGMAN_MsgStaticListIsFull(&TxMsgList);
}

/*******************************************************************************
* 名称  : MSGMAN_RxMsgListIsFull
* 描述  : 消息接收链表是否已满
* 输入  : 无
* 输出  : 无
* 返回  : 是否已满
*******************************************************************************/
U8 MSGMAN_RxMsgListIsFull(void)
{
    return MSGMAN_MsgStaticListIsFull(&RxMsgList);
}

/*******************************************************************************
* 名称  : MSGMAN_TxMsgListInsert
* 描述  : 消息发射链表插入元素，按消息优先级排序
* 输入  : pMsgCellStruct - 指向存放插入元素指针
* 输出  : 无
* 返回  : 是否插入成功
*******************************************************************************/
U8 MSGMAN_TxMsgListInsert(const MsgCellStruct *pMsgCellStruct)
{
    return MSGMAN_MsgStaticListInsert(&TxMsgList, pMsgCellStruct);
}

/*******************************************************************************
* 名称  : MSGMAN_RxMsgListInsert
* 描述  : 消息接收链表插入元素
* 输入  : pMsgCellStruct - 指向存放插入元素指针
* 输出  : 无
* 返回  : 是否插入成功
*******************************************************************************/
U8 MSGMAN_RxMsgListInsert(const MsgCellStruct *pMsgCellStruct)
{
    return MSGMAN_MsgStaticListInsert(&RxMsgList, pMsgCellStruct);
}

/*******************************************************************************
* 名称  : MSGMAN_TxMsgListGetElem
* 描述  : 消息发射链表获取元素
* 输入  : 无
* 输出  : pMsgCellStruct - 指向存放返回元素的指针
* 返回  : 是否插入成功
*******************************************************************************/
U8 MSGMAN_TxMsgListGetElem(MsgCellStruct *pMsgCellStruct)
{
    return MSGMAN_MsgStaticListGetHead(&TxMsgList, pMsgCellStruct);
}

/*******************************************************************************
* 名称  : MSGMAN_RxMsgListGetElem
* 描述  : 消息接收链表获取元素
* 输入  : 无
* 输出  : pMsgCellStruct - 指向存放返回元素的指针
* 返回  : 是否插入成功
*******************************************************************************/
U8 MSGMAN_RxMsgListGetElem(MsgCellStruct *pMsgCellStruct)
{
    return MSGMAN_MsgStaticListGetHead(&RxMsgList, pMsgCellStruct);
}

/*******************************************************************************
* 名称  : MSGMAN_AisAckFifoIsEmpty
* 描述  : 队列是否为空
* 输入  : pAisAckFifoStruct - AisAckFifoStruct结构体指针		  
* 输出  : 无
* 返回  : TRUE - 为空，FALSE - 非空
*******************************************************************************/
static U8 MSGMAN_AisAckFifoIsEmpty(AisAckFifoStruct *pAisAckFifoStruct)
{
    if (pAisAckFifoStruct->Input == pAisAckFifoStruct->Output)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* 名称  : MSGMAN_AisAckFifoIsFull
* 描述  : 队列是否已满
* 输入  : pAisAckFifoStruct - AisAckFifoStruct结构体指针		  
* 输出  : 无
* 返回  : TRUE - 已满，FALSE - 未满
*******************************************************************************/
static U8 MSGMAN_AisAckFifoIsFull(AisAckFifoStruct *pAisAckFifoStruct)
{
    if ((pAisAckFifoStruct->Input + 1) % MSG_MAN_ACK_FIFO_LEN
        == pAisAckFifoStruct->Output)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }     
}

/*******************************************************************************
* 名称  : MSGMAN_AisAckFifoEnque
* 描述  : 队列入队
* 输入  : pAisAckFifoStruct - AisAckFifoStruct结构体指针
          pAisAckCellStruct - 指向入队数据单元的指针         		  
* 输出  : 无
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
static U8 MSGMAN_AisAckFifoEnque(AisAckFifoStruct *pAisAckFifoStruct,
                                 const AisAckCellStruct *pAisAckCellStruct)
{
    AisAckCellStruct *pCell;
//    U16 i;
       
    if (MSGMAN_AisAckFifoIsFull(pAisAckFifoStruct) == FALSE)
    {      
        pCell = &(pAisAckFifoStruct->FIFO[pAisAckFifoStruct->Input]);
        
        pCell->DestinationID = pAisAckCellStruct->DestinationID;
        pCell->SequenceNumber = pAisAckCellStruct->SequenceNumber;
                
        pAisAckFifoStruct->Input = (pAisAckFifoStruct->Input + 1)
            % MSG_MAN_ACK_FIFO_LEN;
        
        return TRUE;    
    }
    else
    {
        return FALSE;
    }   
}

/*******************************************************************************
* 名称  : MSGMAN_AisAckFifoDeque
* 描述  : 队列出队
* 输入  : pAisAckFifoStruct - AisAckFifoStruct结构体指针          
* 输出  : pAisAckCellStruct - 指向出队数据单元的指针
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
static U8 MSGMAN_AisAckFifoDeque(AisAckFifoStruct *pAisAckFifoStruct,
                                 AisAckCellStruct *pAisAckCellStruct)
{
//    U16 i;
    AisAckCellStruct *pCell;

    if (MSGMAN_AisAckFifoIsEmpty(pAisAckFifoStruct) == FALSE)
    {        
        pCell = &(pAisAckFifoStruct->FIFO[pAisAckFifoStruct->Output]);
        
        pAisAckCellStruct->DestinationID = pCell->DestinationID;
        pAisAckCellStruct->SequenceNumber = pCell->SequenceNumber;

        pAisAckFifoStruct->Output = (pAisAckFifoStruct->Output + 1)
            % MSG_MAN_ACK_FIFO_LEN;
        
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* 名称  : MSGMAN_BurstSchedule
* 描述  : 突发类消息（msg6/msg7/msg8/msg12/msg13/msg14）发送管理
* 输入  : pBurstSchedStruct - BurstSchedStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void MSGMAN_BurstSchedule(BurstSchedStruct *pBurstSchedStruct)
{
    AisAckFifoStruct *pMsg6AckA;
    AisAckFifoStruct *pMsg6AckB;
    AisAckFifoStruct *pMsg12AckA;
    AisAckFifoStruct *pMsg12AckB;
    AisAckCellStruct ackCell;
    U32 DestIDs[AIS_ACK_ONCE_QUANTITY_MAX];
    U8  SeqNumber[AIS_ACK_ONCE_QUANTITY_MAX];
    U8  ackType;
    S16 i;

    pMsg6AckA = &(pBurstSchedStruct->Msg6AckA);
    pMsg6AckB = &(pBurstSchedStruct->Msg6AckB);
    pMsg12AckA = &(pBurstSchedStruct->Msg12AckA);
    pMsg12AckB = &(pBurstSchedStruct->Msg12AckB);

    /*lq 对信道A上的6号消息进行确认 */
    if (MSGMAN_AisAckFifoIsEmpty(pMsg6AckA) != TRUE)
    {        
        /*lq 获取确认信息 */
        for (i = 0; i < AIS_ACK_ONCE_QUANTITY_MAX; i++)
        {
            if (MSGMAN_AisAckFifoDeque(pMsg6AckA, &ackCell) == TRUE)
            {
                DestIDs[i] = ackCell.DestinationID;
                SeqNumber[i] = ackCell.SequenceNumber;
            } 
            else
            {
                break;
            }
        }

        /*lq 判断确认类型（数量） */
        if (i == 1)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_1;
        }
        else if (i == 2)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_2;        
        }
        else if (i == 3)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_3;        
        }
        else
        {
            ackType = AIS_MSG7_13_ACK_TYPE_4;        
        }

        /*lq 发送确认消息 */
        SendMsg7(ENUM_AIS_CHANNEL_A, SLOT_FORCE_TX_VALUE, ackType,
            DestIDs, SeqNumber);
    }

    /*lq 对信道B上的6号消息进行确认 */
    if (MSGMAN_AisAckFifoIsEmpty(pMsg6AckB) != TRUE)
    {        
        /*lq 获取确认信息 */
        for (i = 0; i < AIS_ACK_ONCE_QUANTITY_MAX; i++)
        {
            if (MSGMAN_AisAckFifoDeque(pMsg6AckB, &ackCell) == TRUE)
            {
                DestIDs[i] = ackCell.DestinationID;
                SeqNumber[i] = ackCell.SequenceNumber;
            } 
            else
            {
                break;
            }
        }

        /*lq 判断确认类型（数量） */
        if (i == 1)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_1;
        }
        else if (i == 2)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_2;        
        }
        else if (i == 3)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_3;        
        }
        else
        {
            ackType = AIS_MSG7_13_ACK_TYPE_4;        
        }

        /*lq 发送确认消息 */
        SendMsg7(ENUM_AIS_CHANNEL_B, SLOT_FORCE_TX_VALUE, ackType,
            DestIDs, SeqNumber);
    }

    /*lq 对信道A上的12号消息进行确认 */
    if (MSGMAN_AisAckFifoIsEmpty(pMsg12AckA) != TRUE)
    {        
        /*lq 获取确认信息 */
        for (i = 0; i < AIS_ACK_ONCE_QUANTITY_MAX; i++)
        {
            if (MSGMAN_AisAckFifoDeque(pMsg12AckA, &ackCell) == TRUE)
            {
                DestIDs[i] = ackCell.DestinationID;
                SeqNumber[i] = ackCell.SequenceNumber;
            } 
            else
            {
                break;
            }
        }

        /*lq 判断确认类型（数量） */
        if (i == 1)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_1;
        }
        else if (i == 2)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_2;        
        }
        else if (i == 3)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_3;        
        }
        else
        {
            ackType = AIS_MSG7_13_ACK_TYPE_4;        
        }

        /*lq 发送确认消息 */
        SendMsg13(ENUM_AIS_CHANNEL_A, SLOT_FORCE_TX_VALUE, ackType,
            DestIDs, SeqNumber);
    }

    /*lq 对信道B上的12号消息进行确认 */
    if (MSGMAN_AisAckFifoIsEmpty(pMsg12AckB) != TRUE)
    {        
        /*lq 获取确认信息 */
        for (i = 0; i < AIS_ACK_ONCE_QUANTITY_MAX; i++)
        {
            if (MSGMAN_AisAckFifoDeque(pMsg12AckB, &ackCell) == TRUE)
            {
                DestIDs[i] = ackCell.DestinationID;
                SeqNumber[i] = ackCell.SequenceNumber;
            } 
            else
            {
                break;
            }
        }

        /*lq 判断确认类型（数量） */
        if (i == 1)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_1;
        }
        else if (i == 2)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_2;        
        }
        else if (i == 3)
        {
            ackType = AIS_MSG7_13_ACK_TYPE_3;        
        }
        else
        {
            ackType = AIS_MSG7_13_ACK_TYPE_4;        
        }

        /*lq 发送确认消息 */
        SendMsg13(ENUM_AIS_CHANNEL_B, SLOT_FORCE_TX_VALUE, ackType,
            DestIDs, SeqNumber);
    }

    /*lq 发送通过ABM语句设置的消息（Msg6和Msg12） */
    if (AISDI_ABMIsValid(&ABMPara) == TRUE)
    {
        ABMPara.ValidFlg = FALSE;

        if (AISMSG_ConvertToMsgType(ABMPara.MsgID, 0) == AISMsgType6)
        {
            SendMsg6(ABMPara.Channel, SLOT_FORCE_TX_VALUE);
        }
        else if (AISMSG_ConvertToMsgType(ABMPara.MsgID, 0) == AISMsgType12)
        {
            SendMsg12(ABMPara.Channel, SLOT_FORCE_TX_VALUE);
        }
    }

    /*lq 发送通过BBM语句设置的消息（Msg8和Msg14） */
    if (AISDI_BBMIsValid(&BBMPara) == TRUE)
    {
        BBMPara.ValidFlg = FALSE;

        if (AISMSG_ConvertToMsgType(BBMPara.MsgID, 0) == AISMsgType8)
        {
            SendMsg8(BBMPara.Channel, SLOT_FORCE_TX_VALUE);
        }
        else if (AISMSG_ConvertToMsgType(BBMPara.MsgID, 0) == AISMsgType14)
        {
            SendMsg14(BBMPara.Channel, SLOT_FORCE_TX_VALUE);
        }
    }
}

/*******************************************************************************
* 名称  : SendMsg1
* 描述  : 发送1号消息
* 输入  : channel - 发射信道
          txSlot - 发射时隙
          commState - 通信状态
* 输出  : 无
* 返回  : 是否发送成功
*******************************************************************************/
U8 SendMsg1(AisChannelEnum channel, S16 txSlot, AISMsgCommStateUnion commState)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq 初始化消息 */
    InitMsg1(commState);

    /*lq 初始化消息单元 */
    msgCell.MsgType = AISMsgType1;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_1;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq 发射时无用
    //lq msgCell.MsgState = ;   //lq 发射时无用
	for (i = 0; i < AISMsgBinBuff_1[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_1[i];
    }
	msgCell.DataLength = AISMsgBinBuff_1[0] + 1;

    /*lq 加入消息发射列表 */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SendMsg3
* 描述  : 发送3号消息
* 输入  : channel - 发射信道
          txSlot - 发射时隙
          commState - 通信状态
* 输出  : 无
* 返回  : 是否发送成功
*******************************************************************************/
U8 SendMsg3(AisChannelEnum channel, S16 txSlot, AISMsgCommStateUnion commState)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq 初始化消息 */
    InitMsg3(commState);

    /*lq 初始化消息单元 */
    msgCell.MsgType = AISMsgType3;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_1;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq 发射时无用
    //lq msgCell.MsgState = ;   //lq 发射时无用
	for (i = 0; i < AISMsgBinBuff_3[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_3[i];
    }
	msgCell.DataLength = AISMsgBinBuff_3[0] + 1;

    /*lq 加入消息发射列表 */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SendMsg5
* 描述  : 发送5号消息
* 输入  : channel - 发射信道
          txSlot - 发射时隙
* 输出  : 无
* 返回  : 是否发送成功
*******************************************************************************/
U8 SendMsg5(AisChannelEnum channel, S16 txSlot)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq 初始化消息 */
    InitMsg5();

    /*lq 初始化消息单元 */
    msgCell.MsgType = AISMsgType5;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_4;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq 发射时无用
    //lq msgCell.MsgState = ;   //lq 发射时无用
	for (i = 0; i < AISMsgBinBuff_5[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_5[i];
    }
	msgCell.DataLength = AISMsgBinBuff_5[0] + 1;

    /*lq 加入消息发射列表 */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SendMsg6
* 描述  : 发送6号消息
* 输入  : channel - 发射信道
          txSlot - 发射时隙
* 输出  : 无
* 返回  : 是否发送成功
*******************************************************************************/
U8 SendMsg6(AisChannelEnum channel, S16 txSlot)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq 初始化消息 */
    InitMsg6();

    /*lq 初始化消息单元 */
    msgCell.MsgType = AISMsgType6;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_4;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq 发射时无用
    //lq msgCell.MsgState = ;   //lq 发射时无用
	for (i = 0; i < AISMsgBinBuff_6[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_6[i];
    }
	msgCell.DataLength = AISMsgBinBuff_6[0] + 1;

    /*lq 加入消息发射列表 */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SendMsg6Test
* 描述  : 发送6号测试消息
* 输入  : channel - 发射信道
          txSlot - 发射时隙
* 输出  : 无
* 返回  : 是否发送成功
*******************************************************************************/
U8 SendMsg6Test(AisChannelEnum channel, S16 txSlot)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq 初始化消息 */
    InitMsg6Test();

    /*lq 初始化消息单元 */
    msgCell.MsgType = AISMsgType6;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_4;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq 发射时无用
    //lq msgCell.MsgState = ;   //lq 发射时无用
	for (i = 0; i < AISMsgBinBuff_6[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_6[i];
    }
	msgCell.DataLength = AISMsgBinBuff_6[0] + 1;

    /*lq 加入消息发射列表 */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SendMsg7
* 描述  : 发送7号消息
* 输入  : channel - 发射信道
          txSlot - 发射时隙
          ackType - 确认类型
          pDestinationID - 确认消息的目的ID数组指针
          pSequenceNumber - 确认消息的序号数组指针
* 输出  : 无
* 返回  : 是否发送成功
*******************************************************************************/
U8 SendMsg7(AisChannelEnum channel, S16 txSlot, U8 ackType, U32 *pDestinationID,
            U8 *pSequenceNumber)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq 初始化消息 */
    InitMsg7(ackType, pDestinationID, pSequenceNumber);

    /*lq 初始化消息单元 */
    msgCell.MsgType = AISMsgType7;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_1;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq 发射时无用
    //lq msgCell.MsgState = ;   //lq 发射时无用
	for (i = 0; i < AISMsgBinBuff_7[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_7[i];
    }
	msgCell.DataLength = AISMsgBinBuff_7[0] + 1;

    /*lq 加入消息发射列表 */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SendMsg8
* 描述  : 发送8号消息
* 输入  : channel - 发射信道
          txSlot - 发射时隙
* 输出  : 无
* 返回  : 是否发送成功
*******************************************************************************/
U8 SendMsg8(AisChannelEnum channel, S16 txSlot)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq 初始化消息 */
    InitMsg8();

    /*lq 初始化消息单元 */
    msgCell.MsgType = AISMsgType8;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_4;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq 发射时无用
    //lq msgCell.MsgState = ;   //lq 发射时无用
	for (i = 0; i < AISMsgBinBuff_8[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_8[i];
    }
	msgCell.DataLength = AISMsgBinBuff_8[0] + 1;

    /*lq 加入消息发射列表 */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SendMsg12
* 描述  : 发送12号消息
* 输入  : channel - 发射信道
          txSlot - 发射时隙
* 输出  : 无
* 返回  : 是否发送成功
*******************************************************************************/
U8 SendMsg12(AisChannelEnum channel, S16 txSlot)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq 初始化消息 */
    InitMsg12();

    /*lq 初始化消息单元 */
    msgCell.MsgType = AISMsgType12;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_2;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq 发射时无用
    //lq msgCell.MsgState = ;   //lq 发射时无用
	for (i = 0; i < AISMsgBinBuff_12[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_12[i];
    }
	msgCell.DataLength = AISMsgBinBuff_12[0] + 1;

    /*lq 加入消息发射列表 */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SendMsg13
* 描述  : 发送13号消息
* 输入  : channel - 发射信道
          txSlot - 发射时隙
          ackType - 确认类型
          pDestinationID - 确认消息的目的ID数组指针
          pSequenceNumber - 确认消息的序号数组指针
* 输出  : 无
* 返回  : 是否发送成功
*******************************************************************************/
U8 SendMsg13(AisChannelEnum channel, S16 txSlot, U8 ackType, U32 *pDestinationID,
            U8 *pSequenceNumber)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq 初始化消息 */
    InitMsg13(ackType, pDestinationID, pSequenceNumber);

    /*lq 初始化消息单元 */
    msgCell.MsgType = AISMsgType13;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_1;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq 发射时无用
    //lq msgCell.MsgState = ;   //lq 发射时无用
	for (i = 0; i < AISMsgBinBuff_13[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_13[i];
    }
	msgCell.DataLength = AISMsgBinBuff_13[0] + 1;

    /*lq 加入消息发射列表 */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SendMsg14
* 描述  : 发送14号消息
* 输入  : channel - 发射信道
          txSlot - 发射时隙
* 输出  : 无
* 返回  : 是否发送成功
*******************************************************************************/
U8 SendMsg14(AisChannelEnum channel, S16 txSlot)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq 初始化消息 */
    InitMsg14();

    /*lq 初始化消息单元 */
    msgCell.MsgType = AISMsgType14;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_2;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq 发射时无用
    //lq msgCell.MsgState = ;   //lq 发射时无用
	for (i = 0; i < AISMsgBinBuff_14[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_14[i];
    }
	msgCell.DataLength = AISMsgBinBuff_14[0] + 1;

    /*lq 加入消息发射列表 */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SendMsg18
* 描述  : 发送18号消息
* 输入  : channel - 发射信道
          txSlot - 发射时隙
          commStateType - 通信状态类型
          commState - 通信状态 
* 输出  : 无
* 返回  : 是否发送成功
*******************************************************************************/
U8 SendMsg18(AisChannelEnum channel, S16 txSlot,
             AISMsgCommStateEnum commStateType, AISMsgCommStateUnion commState)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq 初始化消息 */
    InitMsg18(commStateType, commState);

    /*lq 初始化消息单元 */
    msgCell.MsgType = AISMsgType18;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_1;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq 发射时无用
    //lq msgCell.MsgState = ;   //lq 发射时无用
	for (i = 0; i < AISMsgBinBuff_18[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_18[i];
    }
	msgCell.DataLength = AISMsgBinBuff_18[0] + 1;

    /*lq 加入消息发射列表 */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded at channel %s , tx slot [%4d]!\r\n",
            GetFrameNum(), GetSlotNum(), __func__,
            channel == ENUM_AIS_CHANNEL_A ? AIS_CHANNEL_NAME_A : AIS_CHANNEL_NAME_B,
            txSlot);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed at channel %s , tx slot [%4d]!\r\n",
            GetFrameNum(), GetSlotNum(), __func__,
            channel == ENUM_AIS_CHANNEL_A ? AIS_CHANNEL_NAME_A : AIS_CHANNEL_NAME_B,
            txSlot);
    
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SendMsg18
* 描述  : 发送18号消息
* 输入  : channel - 发射信道
          txSlot - 发射时隙
          commStateType - 通信状态类型
          commState - 通信状态 
* 输出  : 无
* 返回  : 是否发送成功
*******************************************************************************/
U8 SendMsg18Test(AisChannelEnum channel, S16 txSlot)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq 初始化消息 */
    InitMsg18Test();

    /*lq 初始化消息单元 */
    msgCell.MsgType = AISMsgType18;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_1;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq 发射时无用
    //lq msgCell.MsgState = ;   //lq 发射时无用
	for (i = 0; i < AISMsgBinBuff_18[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_18[i];
    }
	msgCell.DataLength = AISMsgBinBuff_18[0] + 1;

    /*lq 加入消息发射列表 */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded at channel %s , tx slot [%4d]!\r\n",
            GetFrameNum(), GetSlotNum(), __func__,
            channel == ENUM_AIS_CHANNEL_A ? AIS_CHANNEL_NAME_A : AIS_CHANNEL_NAME_B,
            txSlot);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed at channel %s , tx slot [%4d]!\r\n",
            GetFrameNum(), GetSlotNum(), __func__,
            channel == ENUM_AIS_CHANNEL_A ? AIS_CHANNEL_NAME_A : AIS_CHANNEL_NAME_B,
            txSlot);
    
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SendMsg19
* 描述  : 发送19号消息
* 输入  : channel - 发射信道
          txSlot - 发射时隙
* 输出  : 无
* 返回  : 是否发送成功
*******************************************************************************/
U8 SendMsg19(AisChannelEnum channel, S16 txSlot)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq 初始化消息 */
    InitMsg19();

    /*lq 初始化消息单元 */
    msgCell.MsgType = AISMsgType19;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_1;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq 发射时无用
    //lq msgCell.MsgState = ;   //lq 发射时无用
	for (i = 0; i < AISMsgBinBuff_19[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_19[i];
    }
	msgCell.DataLength = AISMsgBinBuff_19[0] + 1;

    /*lq 加入消息发射列表 */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SendMsg24A
* 描述  : 发送24A号消息
* 输入  : channel - 发射信道
          txSlot - 发射时隙
* 输出  : 无
* 返回  : 是否发送成功
*******************************************************************************/
U8 SendMsg24A(AisChannelEnum channel, S16 txSlot)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq 初始化消息 */
    InitMsg24A();

    /*lq 初始化消息单元 */
    msgCell.MsgType = AISMsgType24A;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_4;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq 发射时无用
    //lq msgCell.MsgState = ;   //lq 发射时无用
	for (i = 0; i < AISMsgBinBuff_24A[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_24A[i];
    }
	msgCell.DataLength = AISMsgBinBuff_24A[0] + 1;

    /*lq 加入消息发射列表 */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SendMsg24B
* 描述  : 发送24B号消息
* 输入  : channel - 发射信道
          txSlot - 发射时隙
* 输出  : 无
* 返回  : 是否发送成功
*******************************************************************************/
U8 SendMsg24B(AisChannelEnum channel, S16 txSlot)
{
    MsgCellStruct msgCell;
    S16 i;

    /*lq 初始化消息 */
    InitMsg24B();

    /*lq 初始化消息单元 */
    msgCell.MsgType = AISMsgType24B;
    msgCell.MsgPriority = ENUM_AIS_MSG_PRIORITY_4;
    msgCell.Channel = channel;
    msgCell.Slot = txSlot;
    //lq msgCell.Frame = ;      //lq 发射时无用
    //lq msgCell.MsgState = ;   //lq 发射时无用
	for (i = 0; i < AISMsgBinBuff_24B[0] + 1; i++)
    {
        msgCell.Data[i] = AISMsgBinBuff_24B[i];
    }
	msgCell.DataLength = AISMsgBinBuff_24B[0] + 1;

    /*lq 加入消息发射列表 */
    if (MSGMAN_TxMsgListInsert(&msgCell) == TRUE)
    {
        debug_printf("$[%6d:%4d] %s : Send succeeded!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return TRUE;
    }
    else
    {
        debug_printf("$[%6d:%4d] %s : Send failed!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : MSGMAN_MsgTxProcess
* 描述  : 消息发送管理模块
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void MSGMAN_MsgTxProcess(void) 
{
    /*lq 可变周期类消息（msg18）发送 */
    TDMA_InstSchedule();
    
    /*lq 固定周期类消息（msg19/msg24A/msg24B）发送 */
    MSGMAN_PeriodSchedule(&PeriodSched);

    /*lq 突发类消息（msg6/msg7/msg8/msg12/msg13/msg14）发送 */
    MSGMAN_BurstSchedule(&BurstSched);
}

/*******************************************************************************
* 名称  : MSGMAN_MsgRxProcess
* 描述  : 消息接收管理模块
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void MSGMAN_MsgRxProcess(void) 
{
    MsgCellStruct msgCell;
    AisAckCellStruct aisAckCell;
    AISMsgCommStateEnum commStateType;

    if (MSGMAN_RxMsgListGetElem(&msgCell) == TRUE)
    {
        switch (AISMSG_ParseMsg(&msgCell.Data[0]))
        {
            case AISMsgType1 :
                
                /*lq 时隙预占管理 */
                TDMA_SetSlotState(msgCell.Channel, AIS_MSG_COMMSTATE_SOTDMA,
                    AISMsg_1.CommunicationState, msgCell.Frame, msgCell.Slot,
                    SLOT_STATE_EXTER_ALLOC);

                break;

            case AISMsgType2 :
                
                /*lq 时隙预占管理 */
                TDMA_SetSlotState(msgCell.Channel, AIS_MSG_COMMSTATE_SOTDMA,
                    AISMsg_2.CommunicationState, msgCell.Frame, msgCell.Slot,
                    SLOT_STATE_EXTER_ALLOC);

                break;

            case AISMsgType3 :
                
                /*lq 时隙预占管理 */
                TDMA_SetSlotState(msgCell.Channel, AIS_MSG_COMMSTATE_ITDMA,
                    AISMsg_3.CommunicationState, msgCell.Frame, msgCell.Slot,
                    SLOT_STATE_EXTER_ALLOC);

                break;

            case AISMsgType4 :
                
                /*lq 时隙预占管理 */
                TDMA_SetSlotState(msgCell.Channel, AIS_MSG_COMMSTATE_SOTDMA,
                    AISMsg_4.CommunicationState, msgCell.Frame, msgCell.Slot,
                    SLOT_STATE_EXTER_ALLOC);

                break;

            case AISMsgType5 :
                
                /*lq 接收船台计数 */
                AISINFO_InstRecStationQuantityIncrease();

                break;

            case AISMsgType6 :
                
                /*lq 消息确认 */
                if (AISMsg_6.DestinationID == AisStaticPara.MMSI)
                {
                    aisAckCell.DestinationID = AISMsg_6.SourceID;                
                    aisAckCell.SequenceNumber = AISMsg_6.SequenceNumber;
                    
                    if (msgCell.Channel == ENUM_AIS_CHANNEL_A)
                    {
                        MSGMAN_AisAckFifoEnque(&BurstSched.Msg6AckA,
                            &aisAckCell);                
                    }
                    else
                    {
                        MSGMAN_AisAckFifoEnque(&BurstSched.Msg6AckB,
                            &aisAckCell);                
                    }
                }
                
                break;

            case AISMsgType9 :
                
                /*lq 时隙预占管理 */
                if (AISMsg_9.CommStateSelectFlag == 0)
                {
                    commStateType = AIS_MSG_COMMSTATE_SOTDMA;
                }
                else
                {
                    commStateType = AIS_MSG_COMMSTATE_ITDMA;
                }

                TDMA_SetSlotState(msgCell.Channel, commStateType,
                    AISMsg_9.CommunicationState, msgCell.Frame, msgCell.Slot,
                    SLOT_STATE_EXTER_ALLOC);

                break;

            case AISMsgType11 :
                
                /*lq 时隙预占管理 */
                TDMA_SetSlotState(msgCell.Channel, AIS_MSG_COMMSTATE_SOTDMA,
                    AISMsg_11.CommunicationState, msgCell.Frame, msgCell.Slot,
                    SLOT_STATE_EXTER_ALLOC);

                break;

            case AISMsgType12 :
                
                /*lq 消息确认 */
                if (AISMsg_12.DestinationID == AisStaticPara.MMSI)
                {
                    aisAckCell.DestinationID = AISMsg_12.SourceID;                
                    aisAckCell.SequenceNumber = AISMsg_12.SequenceNumber;
                    
                    if (msgCell.Channel == ENUM_AIS_CHANNEL_A)
                    {
                        MSGMAN_AisAckFifoEnque(&BurstSched.Msg12AckA,
                            &aisAckCell);                
                    }
                    else
                    {
                        MSGMAN_AisAckFifoEnque(&BurstSched.Msg12AckB,
                            &aisAckCell);                
                    }
                }

                break;

            case AISMsgType18 :
                
                /*lq 时隙预占管理 */
                if (AISMsg_18.CommStateSelectFlag == 0)
                {
                    commStateType = AIS_MSG_COMMSTATE_SOTDMA;
                }
                else
                {
                    commStateType = AIS_MSG_COMMSTATE_ITDMA;
                }

                TDMA_SetSlotState(msgCell.Channel, commStateType,
                    AISMsg_18.CommunicationState, msgCell.Frame, msgCell.Slot,
                    SLOT_STATE_EXTER_ALLOC);

                break;

            case AISMsgType24A :
                
                /*lq 接收船台计数 */
                AISINFO_InstRecStationQuantityIncrease();

                break;

            case AISMsgType26 :
                
                /*lq 时隙预占管理 */
                if (AISMsg_26.CommStateSelectFlag == 0)
                {
                    commStateType = AIS_MSG_COMMSTATE_SOTDMA;
                }
                else
                {
                    commStateType = AIS_MSG_COMMSTATE_ITDMA;
                }

                TDMA_SetSlotState(msgCell.Channel, commStateType,
                    AISMsg_26.CommunicationState, msgCell.Frame, msgCell.Slot,
                    SLOT_STATE_EXTER_ALLOC);

                break;

            default :
                break;            
        }
    }
}

/*******************************************************************************
* 名称  : MSGMAN_ModuleInit
* 描述  : 模块初始化
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void MSGMAN_ModuleInit(void) 
{
    MSGMAN_MsgStaticListStructInit(&TxMsgList);    
    MSGMAN_MsgStaticListStructInit(&RxMsgList);    
    MSGMAN_PeriodSchedStructInit(&PeriodSched);
    MSGMAN_BurstSchedStructInit(&BurstSched);
}

/*==============================================================================
------------------------------------End of File---------------------------------
==============================================================================*/

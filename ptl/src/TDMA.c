/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: TDMA.c
  Author: lq       Version : V1.0.0        Date: 2015.01.16
  Description:     TDMA 管理模块
  Version:         V1.0.0
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "Include.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static TdmaSchedStruct TdmaSched;

/* Private function prototypes -----------------------------------------------*/
static U8 TDMA_SendMsg(AisChannelEnum channel, S16 txSlot,
             AISMsgCommStateEnum commStateType, AISMsgCommStateUnion commState);

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* 名称  : SOTDMA_SotdmaParaStructInit
* 描述  : SotdmaParaStruct结构体初始化
* 输入  : pSotdmaParaStruct - 指向SotdmaParaStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SOTDMA_SotdmaParaStructInit(SotdmaParaStruct *pSotdmaParaStruct)
{
    U16 i;

    pSotdmaParaStruct->NominalStartSlot = SLOT_INVALID_VALUE;
    pSotdmaParaStruct->ReportRate = SOTDMA_SC_RR_MIN;   //zjw 报告速率//lq 该初始值需与TdmaCtrlStruct.ReportIntervalSlot的初始值相匹配
    pSotdmaParaStruct->NominalIncrement = FRAME_SLOT_SIZE / (pSotdmaParaStruct->ReportRate);
    pSotdmaParaStruct->SelectIntervalOffset =
        SOTDMA_CalcSelectIntervalOffset(pSotdmaParaStruct->NominalIncrement);
    pSotdmaParaStruct->SelectIntervalLow = SLOT_INVALID_VALUE;
    pSotdmaParaStruct->SelectIntervalHigh = SLOT_INVALID_VALUE;
    for (i = 0; i < SOTDMA_NTS_BUFF_LEN; i++)
    {
        pSotdmaParaStruct->NominalSlots[i] = SLOT_INVALID_VALUE;
        pSotdmaParaStruct->NominalTxSlots[i] = SLOT_INVALID_VALUE;
        pSotdmaParaStruct->SlotTimeouts[i] = SOTDMA_SLOT_TMO_INVALID_VALUE;
    }
    pSotdmaParaStruct->NominalSlotIn = 0;
    pSotdmaParaStruct->NominalSlotOut = 0;
    pSotdmaParaStruct->NominalSlot = SLOT_INVALID_VALUE;
    pSotdmaParaStruct->NominalTxSlotTotal = 0;
    pSotdmaParaStruct->NominalTxSlotIn = 0;
    pSotdmaParaStruct->NominalTxSlotOut = 0;
    pSotdmaParaStruct->NominalTxSlot = SLOT_INVALID_VALUE;
    pSotdmaParaStruct->SlotTimeoutIn = 0;
    pSotdmaParaStruct->SlotTimeoutOut = 0;
    pSotdmaParaStruct->SlotTimeout = SOTDMA_SLOT_TMO_INVALID_VALUE;

    pSotdmaParaStruct->FrameReportCnt = 0;   
    pSotdmaParaStruct->SlotOffset = 0;

    pSotdmaParaStruct->LastTxFrame = FRAME_INVALID_VALUE;
    pSotdmaParaStruct->LastTxSlot = SLOT_INVALID_VALUE;
}

/*******************************************************************************
* 名称  : SOTDMA_SotdmaParaStructReset
* 描述  : 复位SotdmaParaStruct结构体除与报告速率直接相关的参数之外的参数
* 输入  : pSotdmaParaStruct - 指向SotdmaParaStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SOTDMA_SotdmaParaStructReset(SotdmaParaStruct *pSotdmaParaStruct)
{
    U16 i;

    pSotdmaParaStruct->NominalStartSlot = SLOT_INVALID_VALUE;
    pSotdmaParaStruct->SelectIntervalLow = SLOT_INVALID_VALUE;
    pSotdmaParaStruct->SelectIntervalHigh = SLOT_INVALID_VALUE;
    for (i = 0; i < SOTDMA_NTS_BUFF_LEN; i++)
    {
        pSotdmaParaStruct->NominalSlots[i] = SLOT_INVALID_VALUE;
        pSotdmaParaStruct->NominalTxSlots[i] = SLOT_INVALID_VALUE;
        pSotdmaParaStruct->SlotTimeouts[i] = SOTDMA_SLOT_TMO_INVALID_VALUE;
    }
    pSotdmaParaStruct->NominalSlotIn = 0;
    pSotdmaParaStruct->NominalSlotOut = 0;
    pSotdmaParaStruct->NominalSlot = SLOT_INVALID_VALUE;
    pSotdmaParaStruct->NominalTxSlotTotal = 0;
    pSotdmaParaStruct->NominalTxSlotIn = 0;
    pSotdmaParaStruct->NominalTxSlotOut = 0;
    pSotdmaParaStruct->NominalTxSlot = SLOT_INVALID_VALUE;
    pSotdmaParaStruct->SlotTimeoutIn = 0;
    pSotdmaParaStruct->SlotTimeoutOut = 0;
    pSotdmaParaStruct->SlotTimeout = SOTDMA_SLOT_TMO_INVALID_VALUE;

    pSotdmaParaStruct->FrameReportCnt = 0;   
    pSotdmaParaStruct->SlotOffset = 0;

    pSotdmaParaStruct->LastTxFrame = FRAME_INVALID_VALUE;
    pSotdmaParaStruct->LastTxSlot = SLOT_INVALID_VALUE;
}

/*******************************************************************************
* 名称  : SOTDMA_SotdmaParaStructInit
* 描述  : SotdmaParaStruct结构体复制
* 输入  : pSotdmaCtrlDst - 指向目的SotdmaCtrlStruct结构体的指针
          pSotdmaCtrlSrc - 指向源SotdmaCtrlStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SOTDMA_SotdmaParaStructCopy(SotdmaParaStruct *pSotdmaParaDst,
                                        SotdmaParaStruct *pSotdmaParaSrc)
{
    U16 i;

    pSotdmaParaDst->NominalStartSlot = pSotdmaParaSrc->NominalStartSlot;
    pSotdmaParaDst->ReportRate = pSotdmaParaSrc->ReportRate;
    pSotdmaParaDst->NominalIncrement = pSotdmaParaSrc->NominalIncrement;
    pSotdmaParaDst->SelectIntervalOffset = pSotdmaParaSrc->SelectIntervalOffset;
    pSotdmaParaDst->SelectIntervalLow = pSotdmaParaSrc->SelectIntervalLow;
    pSotdmaParaDst->SelectIntervalHigh = pSotdmaParaSrc->SelectIntervalHigh;
    for (i = 0; i < SOTDMA_NTS_BUFF_LEN; i++)
    {
        pSotdmaParaDst->NominalSlots[i] = pSotdmaParaSrc->NominalSlots[i];
        pSotdmaParaDst->NominalTxSlots[i] = pSotdmaParaSrc->NominalTxSlots[i];
        pSotdmaParaDst->SlotTimeouts[i] = pSotdmaParaSrc->SlotTimeouts[i];
    }
    pSotdmaParaDst->NominalSlotIn = pSotdmaParaSrc->NominalSlotIn;
    pSotdmaParaDst->NominalSlotOut = pSotdmaParaSrc->NominalSlotOut;
    pSotdmaParaDst->NominalSlot = pSotdmaParaSrc->NominalSlot;
    pSotdmaParaDst->NominalTxSlotTotal = pSotdmaParaSrc->NominalTxSlotTotal;
    pSotdmaParaDst->NominalTxSlotIn = pSotdmaParaSrc->NominalTxSlotIn;
    pSotdmaParaDst->NominalTxSlotOut = pSotdmaParaSrc->NominalTxSlotOut;
    pSotdmaParaDst->NominalTxSlot = pSotdmaParaSrc->NominalTxSlot;
    pSotdmaParaDst->SlotTimeoutIn = pSotdmaParaSrc->SlotTimeoutIn;
    pSotdmaParaDst->SlotTimeoutOut = pSotdmaParaSrc->SlotTimeoutOut;
    pSotdmaParaDst->SlotTimeout = pSotdmaParaSrc->SlotTimeout;

    pSotdmaParaDst->FrameReportCnt = pSotdmaParaSrc->FrameReportCnt;
    pSotdmaParaDst->SlotOffset = pSotdmaParaSrc->SlotOffset;

    pSotdmaParaDst->LastTxFrame = pSotdmaParaSrc->LastTxFrame;
    pSotdmaParaDst->LastTxSlot = pSotdmaParaSrc->LastTxSlot;
}

/*******************************************************************************
* 名称  : SOTDMA_SotdmaCtrlStructInit
* 描述  : SotdmaCtrlStruct结构体初始化
* 输入  : pSotdmaCtrlStruct - 指向SotdmaCtrlStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SOTDMA_SotdmaCtrlStructInit(SotdmaCtrlStruct *pSotdmaCtrlStruct)
{
    pSotdmaCtrlStruct->TimeCnt = 0;
    pSotdmaCtrlStruct->TimePeriod = SOTDMA_CTRL_TIM_PERIOD;
    pSotdmaCtrlStruct->Timeout = SOTDMA_CTRL_TIMEOUT_INIT_PHASE;
    pSotdmaCtrlStruct->TimeoutCnt = 0;
    pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_IDLE;
    pSotdmaCtrlStruct->SotdmaEn = FALSE;
}

#if 0 // lnw mask  noused
/*******************************************************************************
* 名称  : SOTDMA_SotdmaCtrlStructInit
* 描述  : SotdmaCtrlStruct结构体复制
* 输入  : pSotdmaCtrlDst - 指向目的SotdmaCtrlStruct结构体的指针
          pSotdmaCtrlSrc - 指向源SotdmaCtrlStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SOTDMA_SotdmaCtrlStructCopy(SotdmaCtrlStruct *pSotdmaCtrlDst,
                                        SotdmaCtrlStruct *pSotdmaCtrlSrc)
{
    pSotdmaCtrlDst->TimeCnt = pSotdmaCtrlSrc->TimeCnt;
    pSotdmaCtrlDst->TimePeriod = pSotdmaCtrlSrc->TimePeriod;
    pSotdmaCtrlDst->Timeout = pSotdmaCtrlSrc->Timeout;
    pSotdmaCtrlDst->TimeoutCnt = pSotdmaCtrlSrc->TimeoutCnt;
    pSotdmaCtrlDst->CtrlState = pSotdmaCtrlSrc->CtrlState;
    pSotdmaCtrlDst->SotdmaEn = pSotdmaCtrlSrc->SotdmaEn;
}
#endif
/*******************************************************************************
* 名称  : SOTDMA_SotdmaSchedStructInit
* 描述  : SotdmaSchedStruct结构体初始化
* 输入  : pSotdmaSchedStruct - 指向SotdmaSchedStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SOTDMA_SotdmaSchedStructInit(SotdmaSchedStruct *pSotdmaSchedStruct)
{
    SOTDMA_SotdmaParaStructInit(&(pSotdmaSchedStruct->SotdmaPara));
    SOTDMA_SotdmaParaStructInit(&(pSotdmaSchedStruct->SotdmaParaNew));
    SOTDMA_SotdmaCtrlStructInit(&(pSotdmaSchedStruct->SotdmaCtrl));
}

/*******************************************************************************
* 名称  : SOTDMA_SotdmaSchedDualStructInit
* 描述  : SotdmaSchedDualStruct结构体初始化
* 输入  : pSotdmaSchedDualStruct - 指向SotdmaSchedDualStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SOTDMA_SotdmaSchedDualStructInit(SotdmaSchedDualStruct *pSotdmaSchedDualStruct)
{
    SOTDMA_SotdmaSchedStructInit(&(pSotdmaSchedDualStruct->SotdmaSchedA));
    SOTDMA_SotdmaSchedStructInit(&(pSotdmaSchedDualStruct->SotdmaSchedB));
    SOTDMA_SotdmaCtrlStructInit(&(pSotdmaSchedDualStruct->SotdmaCtrl));
}

/*******************************************************************************
* 名称  : SOTDMA_SotdmaSchedEnable
* 描述  : SOTDMA调度使能配置
* 输入  : flag - 使能标志
          pSotdmaCtrlStruct - 指向SotdmaCtrlStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SOTDMA_SotdmaSchedEnable(U8 flag,
                                     SotdmaCtrlStruct *pSotdmaCtrlStruct)
{
    pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_IDLE;
    pSotdmaCtrlStruct->SotdmaEn = flag;
}

/*******************************************************************************
* 名称  : SOTDMA_SetRr
* 描述  : SOTDMA帧内报告次数计数
* 输入  : pSotdmaParaStruct - 指向SotdmaParaStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SOTDMA_FrameReportCntIncrease(SotdmaParaStruct *pSotdmaParaStruct)
{
    pSotdmaParaStruct->FrameReportCnt = (pSotdmaParaStruct->FrameReportCnt + 1)
        % pSotdmaParaStruct->ReportRate;
}

/*******************************************************************************
* 名称  : SOTDMA_SetRr
* 描述  : SOTDMA设置报告间隔，同时更新与报告间隔相关的参数
* 输入  : pSotdmaParaStruct - 指向SotdmaParaStruct结构体的指针
          reportRate - 报告速率
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static U8 SOTDMA_SetRr(SotdmaParaStruct *pSotdmaParaStruct, S16 reportRate)
{
    if ((reportRate >= SOTDMA_SC_RR_MIN) && (reportRate <= SOTDMA_SC_RR_MAX))
    {
        pSotdmaParaStruct->ReportRate = reportRate;

        pSotdmaParaStruct->NominalIncrement = FRAME_SLOT_SIZE
            / (pSotdmaParaStruct->ReportRate);

        pSotdmaParaStruct->SelectIntervalOffset =
            SOTDMA_CalcSelectIntervalOffset(pSotdmaParaStruct->NominalIncrement);
    
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SOTDMA_SetRrDual
* 描述  : SOTDMA实例设置报告速率
* 输入  : reportRate - 报告速率
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SOTDMA_SetRrDual(S16 reportRate)
{
    SOTDMA_SetRr(&(TdmaSched.SotdmaSched.SotdmaSchedA.SotdmaPara), reportRate);
    SOTDMA_SetRr(&(TdmaSched.SotdmaSched.SotdmaSchedB.SotdmaPara), reportRate);
}

/*******************************************************************************
* 名称  : SOTDMA_IsAtSi
* 描述  : SOTDMA判断是否位于选择间隔内
* 输入  : slot - 判定时隙
          ns - 标称时隙
          offset - 选择间隔偏移量
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static U8 SOTDMA_IsAtSi(S16 slot, S16 ns, S16 offset)
{     
    S16 tempS16;

    tempS16 = slot - ns;
    if (tempS16 < 0)
    {
        tempS16 = -tempS16; 
    }

    if (tempS16 <= offset)
    {
        return TRUE;
    }
    else
    {
        return FALSE;    
    }
}

/*******************************************************************************
* 名称  : SOTDMA_IsAtSi2
* 描述  : SOTDMA判断待判定时隙加上AIS_SLOT_INTERVAL_SEND_TO_TX后是否位于选择间隔
          内
* 输入  : slot - 判定时隙
          ns - 标称时隙
          offset - 选择间隔偏移量
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static U8 SOTDMA_IsAtSi2(S16 slot, S16 ns, S16 offset)
{     
    return SOTDMA_IsAtSi(CalcSlotNum(0, slot, AIS_SLOT_INTERVAL_SEND_TO_TX),
            ns, offset);
}

/*******************************************************************************
* 名称  : SOTDMA_GetAvailableNts
* 描述  : SOTDMA获取可用的发射时隙
* 输入  : pSotdmaParaStruct - 指向待检测的SotdmaParaStruct结构体的指针
          ns - 标称时隙
          offset - 选择间隔偏移量
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static U16 SOTDMA_GetAvailableNts(SotdmaParaStruct *pSotdmaParaStruct, S16 ns, S16 offset)
{     
    S16 i;

    for (i = 0; i < pSotdmaParaStruct->ReportRate; i++)
    {
        if (SOTDMA_IsAtSi(pSotdmaParaStruct->NominalTxSlots[i], ns, offset)
            == TRUE)
        {
            return pSotdmaParaStruct->NominalTxSlots[i];
        }
    }
    
    return SLOT_INVALID_VALUE;
}

#if   0  // lnw mask  no use
/*******************************************************************************
* 名称  : SOTDMA_IsAtNts
* 描述  : SOTDMA判断是否位于标称发射时隙
* 输入  : pSotdmaParaStruct - 指向SotdmaParaStruct结构体的指针
          slot - 判定时隙
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static U8 SOTDMA_IsAtNts(SotdmaParaStruct *pSotdmaParaStruct, S16 slot)
{     
    if (pSotdmaParaStruct->NominalTxSlot == slot)    
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#endif
/*******************************************************************************
* 名称  : SOTDMA_IsNtsArrived
* 描述  : SOTDMA判断是否到达标称发射时隙
* 输入  : pSotdmaParaStruct - 指向SotdmaParaStruct结构体的指针
          slot - 判定时隙
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static U8 SOTDMA_IsNtsArrived(SotdmaParaStruct *pSotdmaParaStruct, S16 slot)
{     
    if (((slot - pSotdmaParaStruct->NominalTxSlot + FRAME_SLOT_SIZE)
        % FRAME_SLOT_SIZE) < AIS_SLOT_INTERVAL_SEND_TO_TX)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SOTDMA_IsSendTimArrived
* 描述  : SOTDMA判断发送时隙（送入消息队列的时隙）是否到达
* 输入  : pSotdmaParaStruct - 指向SotdmaParaStruct结构体的指针
          frame - 判定帧（仅在pSotdmaParaStruct->ReportRate == 1时有用，解决
          ReportRate = 1时，因下一发射时隙与当前发射时隙相同而导致处于标称发射时
          隙时消息多次发送的问题。）
          slot - 判定时隙
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static U8 SOTDMA_IsSendTimArrived(SotdmaParaStruct *pSotdmaParaStruct,
                                  S32 frame, S16 slot)
{     
    if (SOTDMA_IsNtsArrived(pSotdmaParaStruct, CalcSlotNum(0, slot,
        AIS_SLOT_INTERVAL_SEND_TO_TX)) == TRUE)    
    {
        if (pSotdmaParaStruct->ReportRate == 1)
        {
            if ((pSotdmaParaStruct->LastTxFrame == FRAME_INVALID_VALUE)
                || (pSotdmaParaStruct->LastTxSlot == SLOT_INVALID_VALUE))
            {
                pSotdmaParaStruct->LastTxFrame = frame;
                pSotdmaParaStruct->LastTxSlot = slot;

                return TRUE;                
            }
            else
            {
                if (CalcSlotOffset(pSotdmaParaStruct->LastTxFrame,
                    pSotdmaParaStruct->LastTxSlot, frame, slot)
                    > (pSotdmaParaStruct->NominalIncrement / 2))
                {
                    pSotdmaParaStruct->LastTxFrame = frame;
                    pSotdmaParaStruct->LastTxSlot = slot;
    
                    return TRUE;                
                }
                else
                {
                    return FALSE;                
                }
            }

        }
        else
        {
            return TRUE;
        }
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SOTDMA_SetNss
* 描述  : SOTDMA设置标称起始时隙，同时指定第一个NS
* 输入  : pSotdmaParaStruct - 指向SotdmaParaStruct结构体的指针
          nss - 标称起始时隙
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static U8 SOTDMA_SetNss(SotdmaParaStruct *pSotdmaParaStruct, S16 nss)
{
    if (nss >= 0 && nss < FRAME_SLOT_SIZE)
    {
        pSotdmaParaStruct->NominalStartSlot = nss;                     
        
        return TRUE;
    }
    else
    {
        return FALSE;    
    }
}

/*******************************************************************************
* 名称  : SOTDMA_SetNextNs
* 描述  : 选择下一个标称时隙
* 输入  : pSotdmaParaStruct - 指向SotdmaParaStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SOTDMA_SetFirstNs(SotdmaParaStruct *pSotdmaParaStruct)
{
    pSotdmaParaStruct->NominalSlots[pSotdmaParaStruct->NominalSlotIn]
        = pSotdmaParaStruct->NominalStartSlot;

    pSotdmaParaStruct->NominalSlotIn = (pSotdmaParaStruct->NominalSlotIn + 1)
        % pSotdmaParaStruct->ReportRate;
}

#if   0  // lnw mask  no use
/*******************************************************************************
* 名称  : SOTDMA_SetNs
* 描述  : 设置标称时隙
* 输入  : pSotdmaParaStruct - 指向SotdmaParaStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SOTDMA_SetNs(SotdmaParaStruct *pSotdmaParaStruct, U16 index, U16 slot)
{
    pSotdmaParaStruct->NominalSlots[index] = slot;
}
#endif 
/*******************************************************************************
* 名称  : SOTDMA_SetNextNs
* 描述  : 选择下一个标称时隙
* 输入  : pSotdmaParaStruct - 指向SotdmaParaStruct结构体的指针
* 输出  : 无
* 返回  : 下一个标称时隙
*******************************************************************************/
static U16 SOTDMA_SetNextNs(SotdmaParaStruct *pSotdmaParaStruct)
{
    U16 tempU16;
    
    tempU16 = (pSotdmaParaStruct->NominalSlot + pSotdmaParaStruct->NominalIncrement)
        % FRAME_SLOT_SIZE;

    pSotdmaParaStruct->NominalSlots[pSotdmaParaStruct->NominalSlotIn] = tempU16;
    pSotdmaParaStruct->NominalSlotIn = (pSotdmaParaStruct->NominalSlotIn + 1)
        % pSotdmaParaStruct->ReportRate;

    return tempU16;
}

/*******************************************************************************
* 名称  : SOTDMA_SetNextNs
* 描述  : 选择下一个标称时隙
* 输入  : pSotdmaParaStruct - 指向SotdmaParaStruct结构体的指针
* 输出  : 无
* 返回  : 下一个标称时隙
*******************************************************************************/
static U16 SOTDMA_GetNextNs(SotdmaParaStruct *pSotdmaParaStruct)
{
    pSotdmaParaStruct->NominalSlot =
        pSotdmaParaStruct->NominalSlots[pSotdmaParaStruct->NominalSlotOut];
    
    pSotdmaParaStruct->NominalSlotOut =
        (pSotdmaParaStruct->NominalSlotOut + 1) % pSotdmaParaStruct->ReportRate;

    return pSotdmaParaStruct->NominalSlot;
}

/*******************************************************************************
* 名称  : SOTDMA_SetNts
* 描述  : 设置标称发射时隙
* 输入  : pSotdmaParaStruct - 指向SotdmaParaStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SOTDMA_SetNts(SotdmaParaStruct *pSotdmaParaStruct, U16 index, U16 slot)
{
    pSotdmaParaStruct->NominalTxSlots[index] = slot;
}

/*******************************************************************************
* 名称  : SOTDMA_SetNextNts
* 描述  : SOTDMA设置下一个标称发射时隙
* 输入  : pSotdmaParaStruct - 指向SotdmaParaStruct结构体的指针
          slot - 标称发射时隙
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static U8 SOTDMA_SetNextNts(SotdmaParaStruct *pSotdmaParaStruct, S16 slot)
{
    if (slot >= 0 && slot < FRAME_SLOT_SIZE)
    {
        pSotdmaParaStruct->NominalTxSlots[pSotdmaParaStruct->NominalTxSlotIn] = slot;
        pSotdmaParaStruct->NominalTxSlotIn = (pSotdmaParaStruct->NominalTxSlotIn + 1)
            %  pSotdmaParaStruct->ReportRate;
        
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SOTDMA_GetNextNts
* 描述  : SOTDMA获取下一标称发射时隙
* 输入  : pSotdmaParaStruct - 指向SotdmaParaStruct结构体的指针
* 输出  : 无
* 返回  : 下一标称发射时隙
*******************************************************************************/
static S16 SOTDMA_GetNextNts(SotdmaParaStruct *pSotdmaParaStruct)
{
    pSotdmaParaStruct->NominalTxSlot = 
        pSotdmaParaStruct->NominalTxSlots[pSotdmaParaStruct->NominalTxSlotOut];
    
    pSotdmaParaStruct->NominalTxSlotOut =
        (pSotdmaParaStruct->NominalTxSlotOut + 1) %  pSotdmaParaStruct->ReportRate;

    return pSotdmaParaStruct->NominalTxSlot;
}

/*******************************************************************************
* 名称  : SOTDMA_IsRrChanged
* 描述  : 报告速率是否改变
* 输入  : channel - 信道
* 输出  : 无
* 返回  : TRUE - 已改变；FALSE - 未改变
*******************************************************************************/
U8 SOTDMA_IsRrChanged(AisChannelEnum channel)
{
    if (channel == ENUM_AIS_CHANNEL_A)
    {
        if (TdmaSched.SotdmaSched.SotdmaSchedA.SotdmaParaNew.ReportRate
            != TdmaSched.SotdmaSched.SotdmaSchedA.SotdmaPara.ReportRate)
        {
            return TRUE;        
        }
        else
        {
            return FALSE;
        }
    }
    else if (channel == ENUM_AIS_CHANNEL_B)
    {
        if (TdmaSched.SotdmaSched.SotdmaSchedB.SotdmaParaNew.ReportRate
            != TdmaSched.SotdmaSched.SotdmaSchedB.SotdmaPara.ReportRate)
        {
            return TRUE;        
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }                
}

#if 0
/*******************************************************************************
* 名称  : SotdmaIsRrChanged
* 描述  : 报告速率是否改变
* 输入  : 无
* 输出  : 无
* 返回  : TRUE - 已改变；FALSE - 未改变
*******************************************************************************/
U8 SotdmaIsRrChanged(void)
{
    if ((SOTDMA_IsRrChanged(&(SotdmaSched.SotdmaParaA)) == TRUE)
        || (SOTDMA_IsRrChanged(&(SotdmaSched.SotdmaParaB)) == TRUE))
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }
}
#endif

/*******************************************************************************
* 名称  : SOTDMA_SetNssDual
* 描述  : SOTDMA设置两个信道的标称起始时隙
* 输入  : channel - nss对应的信道
          nss - 标称起始时隙
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 SOTDMA_SetNssDual(AisChannelEnum channel, S16 nss)
{
    if (nss >= 0 && nss < FRAME_SLOT_SIZE)
    {
        if (channel == ENUM_AIS_CHANNEL_A)
        {
            SOTDMA_SetNss(&(TdmaSched.SotdmaSched.SotdmaSchedA.SotdmaPara), nss);
            SOTDMA_SetNss(&(TdmaSched.SotdmaSched.SotdmaSchedB.SotdmaPara),
                (nss + (TdmaSched.SotdmaSched.SotdmaSchedB.SotdmaPara.NominalIncrement >> 1))   //lq 此处的NominalIncrement为单信道标称增量，为双信道标称增量的2倍
                % FRAME_SLOT_SIZE);        
        }
        else if (channel == ENUM_AIS_CHANNEL_B)
        {
            SOTDMA_SetNss(&(TdmaSched.SotdmaSched.SotdmaSchedB.SotdmaPara), nss);
            SOTDMA_SetNss(&(TdmaSched.SotdmaSched.SotdmaSchedA.SotdmaPara),
                (nss + (TdmaSched.SotdmaSched.SotdmaSchedA.SotdmaPara.NominalIncrement >> 1))
                % FRAME_SLOT_SIZE);
        }
        else
        {
            return FALSE;
        }                
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

#if   0  // lnw mask  no use
/*******************************************************************************
* 名称  : SOTDMA_SetTmo
* 描述  : SOTDMA设置时隙超时
* 输入  : pSotdmaParaStruct - 指向SotdmaParaStruct结构体的指针
          index - 索引值
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SOTDMA_SetTmo(SotdmaParaStruct *pSotdmaParaStruct, U16 index)
{ 
    pSotdmaParaStruct->SlotTimeouts[index] =
        SOTDMA_TMO_MIN + (rand() % (SOTDMA_TMO_MAX - SOTDMA_TMO_MIN + 1));
}
#endif
/*******************************************************************************
* 名称  : SOTDMA_GetTmo
* 描述  : SOTDMA获取时隙超时
* 输入  : pSotdmaParaStruct - 指向SotdmaParaStruct结构体的指针
          index - 索引值
* 输出  : 无
* 返回  : 时隙超时
*******************************************************************************/
static S8 SOTDMA_GetTmo(SotdmaParaStruct *pSotdmaParaStruct, U16 index)
{ 
    return pSotdmaParaStruct->SlotTimeouts[index];
}

/*******************************************************************************
* 名称  : SOTDMA_SetNextNts
* 描述  : SOTDMA设置下一个标称发射时隙
* 输入  : pSotdmaParaStruct - 指向SotdmaParaStruct结构体的指针
          nts - 标称发射时隙
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SOTDMA_SetNextTmo(SotdmaParaStruct *pSotdmaParaStruct)
{ 
    pSotdmaParaStruct->SlotTimeouts[pSotdmaParaStruct->SlotTimeoutIn] =
        SOTDMA_TMO_MIN + (rand() % (SOTDMA_TMO_MAX - SOTDMA_TMO_MIN + 1));

    pSotdmaParaStruct->SlotTimeoutIn = (pSotdmaParaStruct->SlotTimeoutIn + 1)
        %  pSotdmaParaStruct->ReportRate;
}

/*******************************************************************************
* 名称  : SOTDMA_GetNextTmo
* 描述  : 选择下一个时隙超时值，同时将缓存值减1，若时隙超时值减1后小于0，则重新
          选择初始值。
* 输入  : pSotdmaParaStruct - 指向SotdmaParaStruct结构体的指针
* 输出  : 无
* 返回  : 下一个时隙超时值
*******************************************************************************/
static S8 SOTDMA_GetNextTmo(SotdmaParaStruct *pSotdmaParaStruct)
{
    pSotdmaParaStruct->SlotTimeout =
        (pSotdmaParaStruct->SlotTimeouts[pSotdmaParaStruct->SlotTimeoutOut])--;

    /*lq 若时隙超时值减1后小于0，则重新选择初始值 */
    if (pSotdmaParaStruct->SlotTimeouts[pSotdmaParaStruct->SlotTimeoutOut] < 0)    
    {
        pSotdmaParaStruct->SlotTimeouts[pSotdmaParaStruct->SlotTimeoutOut] =
            SOTDMA_TMO_MIN + (rand() % (SOTDMA_TMO_MAX - SOTDMA_TMO_MIN + 1));        
    }

    pSotdmaParaStruct->SlotTimeoutOut =
        (pSotdmaParaStruct->SlotTimeoutOut + 1) % pSotdmaParaStruct->ReportRate;

    return pSotdmaParaStruct->SlotTimeout;
}

/*******************************************************************************
* 名称  : SOTDMA_GetSubMsg
* 描述  : 根据时隙超时值获取子消息。
* 输入  : slotTimeout - 时隙超时值；
          pSotdmaParaStruct - 指向SotdmaParaStruct结构体的指针；
* 输出  : 无
* 返回  : 子消息
*******************************************************************************/
static U16 SOTDMA_GetSubMsg(U8 slotTimeout, SotdmaParaStruct *pSotdmaParaStruct)
{
    U16 subMsg;
	U16 utcHour;
	U16 utcMinute;

    if (slotTimeout == 0)
    {
        /*lq Slot offset */
        subMsg = pSotdmaParaStruct->SlotOffset;   
    }
    else if (slotTimeout == 1)
    {
        /*lq UTC hour and minute */
        if (IsGpsFix() == TRUE)
        {
            /*lq 已定位 */
            utcHour = GpsPara.GPS_TimeH;
            utcMinute = GpsPara.GPS_TimeM;
        }
        else
        {
            /*lq 未定位 */
            utcHour = 24;
            utcMinute = 60;
        }

        subMsg = (utcHour << 19) || (utcMinute << 2);
    }
    else if ((slotTimeout == 2)
             || (slotTimeout == 4)
             || (slotTimeout == 6))
    {
        subMsg = pSotdmaParaStruct->NominalTxSlot;
    }
    else if ((slotTimeout == 3)
             || (slotTimeout == 5)
             || (slotTimeout == 7))
    {
        subMsg = AISINFO_InstRecStationGetQuantity();
    }

    return subMsg;
}

/*******************************************************************************
* 名称  : SOTDMA_CalcSelectIntervalOffset
* 描述  : 根据SOTDMA的标称时隙增量计算时隙选择区间偏移量
* 输入  : nominalIncrement - SOTDMA标称时隙增量
* 输出  : 无
* 返回  : 时隙选择区间偏移量
*******************************************************************************/
S16 SOTDMA_CalcSelectIntervalOffset(S16 nominalIncrement)
{
    return nominalIncrement * SOTDMA_SI_OFFSET_SCALE;
}

/*******************************************************************************
* 名称  : SOTDMA_SetSlotState
* 描述  : 根据通信状态、当前帧号及时隙号设置时隙映射表中相应时隙的状态
* 输入  : channel - 时隙所在的信道
          commState - SOTDMA通信状态
          currFrame - 当前帧号
          currSlot - 当前时隙号
          slotState - 时隙状态
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SOTDMA_SetSlotState(AisChannelEnum channel, SOTDMACommStateStruct commState,
                       S32 currFrame, S16 currSlot, U8 slotState)
{
//    S32 tempS32;
//    S16 tempS16;
    S16 i;

    if ((commState.SlotTimeout >= 1)
        && (commState.SlotTimeout <= 7))
    {
        for (i = 1; i <= commState.SlotTimeout; i++)
        {
            SetSlotState(channel, currSlot, slotState, currFrame + i);        
        }
    }
    else if (commState.SlotTimeout == 0)
    {
        if (commState.SubMessage.SlotOffset != 0)
        {
            //tempS32 =  //lnw mask
					    CalcFrameNum(currFrame, currSlot,
                commState.SubMessage.SlotOffset);
            
           // tempS16 =  //lnw mask
					     CalcSlotNum(currFrame, currSlot,
                commState.SubMessage.SlotOffset);
        }
    }
}

/*******************************************************************************
* 名称  : TDMA_GetSyncState
* 描述  : 获取同步方式
* 输入  : 无
* 输出  : 无
* 返回  : 同步状态
*******************************************************************************/
U8 TDMA_GetSyncState(void)
{
    return TDMA_SYNC_STATE_UTC_DIRECT;  //lq 当前同步方式只支持UTC direct方式
}

#if 0
/*******************************************************************************
* 名称  : SotdmaIsAtNts
* 描述  : SOTDMA是否处于NTS
* 输入  : channel - slot对应的信道
          slot - 当前时隙
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 SotdmaIsAtNts(AisChannelEnum channel, S16 slot)
{
    SotdmaParaStruct *pSotdmaPara;

    if (channel == ENUM_AIS_CHANNEL_A)
    {
        pSotdmaPara = &(SotdmaSched.SotdmaParaA);    
    }
    else if (channel == ENUM_AIS_CHANNEL_B)
    {
        pSotdmaPara = &(SotdmaSched.SotdmaParaB);    
    }
    else
    {
        return FALSE;
    }
    
    if (pSotdmaPara->NominalTxSlot == slot)    
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#endif

/*******************************************************************************
* 名称  : SOTDMA_Schedule
* 描述  : 单信道SOTDMA调度
* 输入  : pSotdmaSchedStruct - 指向SotdmaSchedStruct结构体的指针
          currFrame - 当前帧号
          currSlot - 当前时隙号
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SOTDMA_Schedule(SotdmaSchedStruct *pSotdmaSchedStruct, S32 currFrame, S16 currSlot)
{
    AisChannelEnum channel;
    SotdmaCtrlStruct *pSotdmaCtrlStruct;
    SotdmaParaStruct *pSotdmaPara;
    SotdmaParaStruct *pSotdmaParaNew;
    AISMsgCommStateUnion commState;
    U16 tempU16;
    U16 tempU16_1;
    S32 tempS32;

    if (pSotdmaSchedStruct == &(TdmaSched.SotdmaSched.SotdmaSchedA))
    {
        channel = ENUM_AIS_CHANNEL_A;
    }
    else if (pSotdmaSchedStruct == &(TdmaSched.SotdmaSched.SotdmaSchedB))
    {
        channel = ENUM_AIS_CHANNEL_B;
    }
    else
    {
        return ;
    }
    pSotdmaCtrlStruct = &(pSotdmaSchedStruct->SotdmaCtrl);
    pSotdmaPara = &(pSotdmaSchedStruct->SotdmaPara);
    pSotdmaParaNew = &(pSotdmaSchedStruct->SotdmaParaNew);

    switch (pSotdmaCtrlStruct->CtrlState)
    {
        case SOTDMA_STA_IDLE :

            if (pSotdmaCtrlStruct->SotdmaEn == TRUE)
            {
                pSotdmaCtrlStruct->SotdmaEn = FALSE;

                /*lq 转入开始状态 */
                pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_START;
            }

            break;

        case SOTDMA_STA_START :

            pSotdmaCtrlStruct->TimeCnt = SetTimePeriodProcess(pSotdmaCtrlStruct->TimePeriod);

            /*lq 转入初始化阶段 */
            pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_INIT_PHASE;            
            
            break;

        case SOTDMA_STA_INIT_PHASE :
            
            /*lq SOTDMA初始化阶段1min等待 */
            if (pSotdmaCtrlStruct->TimeCnt == GetLocalClockTickProcess())
            {
                pSotdmaCtrlStruct->TimeCnt = SetTimePeriodProcess(pSotdmaCtrlStruct->TimePeriod);                
                
                pSotdmaCtrlStruct->TimeoutCnt++;
                if (pSotdmaCtrlStruct->TimeoutCnt >= pSotdmaCtrlStruct->Timeout)
                {
                    pSotdmaCtrlStruct->TimeoutCnt = 0;

                    /*lq 转入网络进入阶段 */
                    pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_NETWORK_ENTRY_PHASE;                 
                }                 
            }

            break;

        case SOTDMA_STA_NETWORK_ENTRY_PHASE :
            
            /*lq 选择NSS */
            if (pSotdmaPara->NominalIncrement != AIS_SLOT_INTERVAL_SEND_TO_TX)
            {
                tempU16 = Random(pSotdmaPara->NominalIncrement,
                    AIS_SLOT_INTERVAL_SEND_TO_TX);
            }
            else
            {
                tempU16 = pSotdmaPara->NominalIncrement;
            }            
            tempU16 = (currSlot + tempU16) % FRAME_SLOT_SIZE;
            SOTDMA_SetNss(pSotdmaPara, tempU16);

            /*lq 选择信道的NTS */
            if (pSotdmaPara->NominalStartSlot < currSlot)
            {
                /*lq NSS位于当前帧的下一帧 */
                tempS32 = currFrame + 1;
            }
            else
            {
                /*lq NSS位于当前帧 */
                tempS32 = currFrame;
            }
            tempU16 = SelectCandidateSlot(channel, pSotdmaPara->NominalStartSlot,
                pSotdmaPara->SelectIntervalOffset, tempS32, SLOT_CAND_SELECT_UNI);      //lq 使用单边选择，避免选择的时隙小于当前时隙
            
            if ((IsSlotValid(tempU16) == TRUE))
            {
                /*lq 网络进入初始配置 */
                SOTDMA_SetFirstNs(pSotdmaPara);
                SOTDMA_GetNextNs(pSotdmaPara);
                SOTDMA_SetNextNts(pSotdmaPara, tempU16);
                SOTDMA_GetNextNts(pSotdmaPara);
                SOTDMA_SetNextTmo(pSotdmaPara);
                SOTDMA_GetNextTmo(pSotdmaPara);
                pSotdmaPara->FrameReportCnt = 0;
               
                /*lq 转入第一帧阶段 */
                pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_FIRST_FRAME_PHASE;            
            }

            break;

        case SOTDMA_STA_FIRST_FRAME_PHASE :
            
            /*lq 等待当前信道的NTS */
            if (pSotdmaPara->FrameReportCnt < pSotdmaPara->ReportRate)
            {
                if (SOTDMA_IsSendTimArrived(pSotdmaPara, currFrame, currSlot) == TRUE)
                {
                    /*lq 判断是否是一帧中的最后一次发射 */
                    if ((pSotdmaPara->FrameReportCnt + 1) >= pSotdmaPara->ReportRate)
                    {
                        pSotdmaPara->SlotOffset = 0;
                    }
                    else
                    {
                        /*lq 设置下一个NS */
                        tempU16_1 = SOTDMA_SetNextNs(pSotdmaPara);
    
                        /*lq 选择下一个NTS */
                        if (tempU16_1 < currSlot)
                        {
                            tempS32 = currFrame + 1;
                        }
                        else
                        {
                            tempS32 = currFrame;
                        }
                        tempU16 = SelectCandidateSlot(channel, tempU16_1,
                            pSotdmaPara->SelectIntervalOffset, tempS32, SLOT_CAND_SELECT_BI);
    
                        if (IsSlotValid(tempU16) == FALSE)
                        {
                            tempU16 = tempU16_1;
                        }
    
                        /*lq 计算slotOffset */
                        pSotdmaPara->SlotOffset = (tempU16 - pSotdmaPara->NominalTxSlot
                            + FRAME_SLOT_SIZE) % FRAME_SLOT_SIZE;                    
                        
                        /*lq 设置下一个NTS */
                        SOTDMA_SetNextNts(pSotdmaPara, tempU16);

                        /*lq 设置下一个slotTimeout*/
                        SOTDMA_SetNextTmo(pSotdmaPara);
                    }
    
                    /*lq 初始化18号消息 */
                    /*lq 使用ITDMA通信状态，SendMsg18()调用需在SOTDMA_GetNextNts()调用之前*/
                    commState.ITDMACommState.SyncState = TDMA_GetSyncState();
                    commState.ITDMACommState.SlotIncrement =
                        ITDMA_CalcCommStateSlotIncrement(pSotdmaPara->SlotOffset);
                    commState.ITDMACommState.NumberOfSlots =
                        ITDMA_CalcCommStateNumberOfSlots(1,
                        pSotdmaPara->SlotOffset);
                    commState.ITDMACommState.KeepFlag =
                        ITDMA_CalcCommStateKeepFlag(TRUE);
                    TDMA_SendMsg(channel, pSotdmaPara->NominalTxSlot,
                        AIS_MSG_COMMSTATE_ITDMA, commState);    //lq 调用参数需提供channel, txSlot, commStateType, commState
    
                    SOTDMA_GetNextNs(pSotdmaPara);
                    SOTDMA_GetNextNts(pSotdmaPara);
                    SOTDMA_GetNextTmo(pSotdmaPara);
                    SOTDMA_FrameReportCntIncrease(pSotdmaPara);
                }            
            }

            /*lq 判断第一帧阶段是否完成 */
            if (pSotdmaPara->FrameReportCnt >= pSotdmaPara->ReportRate)
            {
                pSotdmaPara->FrameReportCnt = 0;

                /*lq 转入连续工作阶段 */
                pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CONTINUOUS_PHASE;
            }

            break;

        case SOTDMA_STA_CONTINUOUS_PHASE :

            /*lq 等待当前信道的NTS */
            if (SOTDMA_IsSendTimArrived(pSotdmaPara, currFrame, currSlot) == TRUE)            
            {
                /*lq 判断是否为预占的最后一帧 */
                if (pSotdmaPara->SlotTimeout == 0)
                {
                    /*lq 为当前的NS选择新的NTS */
                    if (pSotdmaPara->NominalSlot < currSlot)
                    {
                        tempS32 = currFrame + 1;
                    }
                    else
                    {
                        tempS32 = currFrame;
                    }
                    tempU16 = SelectCandidateSlot(channel, pSotdmaPara->NominalSlot,
                        pSotdmaPara->SelectIntervalOffset, tempS32, SLOT_CAND_SELECT_BI);

                    if (IsSlotValid(tempU16) == FALSE)
                    {
                        tempU16 = pSotdmaPara->NominalSlot;                    
                    }

                    /*lq 计算slotOffset */
                    pSotdmaPara->SlotOffset = tempU16 - pSotdmaPara->NominalTxSlot
                        + FRAME_SLOT_SIZE;
                    
                    /*lq 更新NTS */                                            
                    SOTDMA_SetNts(pSotdmaPara, pSotdmaPara->FrameReportCnt,
                        tempU16);

                    /*lq 更新slotTimeout */                                            
                    pSotdmaPara->SlotTimeout = SOTDMA_GetTmo(pSotdmaPara,
                        pSotdmaPara->FrameReportCnt);
                }
                else
                {
                    pSotdmaPara->SlotOffset = 0;                
                }

                /*lq 初始化18号消息 */
                /*lq 使用SOTDMA通信状态，SendMsg18()调用需在SOTDMA_GetNextNts()调用之前*/
                commState.SOTDMACommState.SyncState = TDMA_GetSyncState();
                commState.SOTDMACommState.SlotTimeout = pSotdmaPara->SlotTimeout;
                commState.SOTDMACommState.SubMessage.SubMsg =
                    SOTDMA_GetSubMsg(commState.SOTDMACommState.SlotTimeout,
                    pSotdmaPara);
                TDMA_SendMsg(channel, pSotdmaPara->NominalTxSlot,
                    AIS_MSG_COMMSTATE_SOTDMA, commState);       //lq 调用参数需提供channel, txSlot, commStateType, commState

                SOTDMA_GetNextNs(pSotdmaPara);
                SOTDMA_GetNextNts(pSotdmaPara);
                SOTDMA_GetNextTmo(pSotdmaPara);

                /*lq 一帧中的报告次数计数加1 */
                SOTDMA_FrameReportCntIncrease(pSotdmaPara);
            }

            if (SOTDMA_IsRrChanged(channel) == TRUE)
            {
                SOTDMA_SotdmaParaStructReset(pSotdmaParaNew);

                /*lq 转入改变报告间隔状态1 */
                pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CHANGE_REPORT_INTERVAL_1;                             
            }

            break;

        case SOTDMA_STA_CHANGE_REPORT_INTERVAL_1 :

            /*lq 等待当前信道的NTS */
            if (SOTDMA_IsSendTimArrived(pSotdmaPara, currFrame, currSlot) == TRUE)
            {
                /*lq 新报告速率下，一帧中的第一次发射 */
                SOTDMA_SetNss(pSotdmaParaNew, pSotdmaPara->NominalSlot);
                SOTDMA_SetFirstNs(pSotdmaParaNew);
                SOTDMA_GetNextNs(pSotdmaParaNew);
                SOTDMA_SetNextNts(pSotdmaParaNew, pSotdmaPara->NominalTxSlot);
                SOTDMA_GetNextNts(pSotdmaParaNew);
                SOTDMA_SetNextTmo(pSotdmaParaNew);
                SOTDMA_GetNextTmo(pSotdmaParaNew);

                if (pSotdmaPara->SlotTimeout == 0)
                {
                    pSotdmaPara->SlotTimeout = 1;
                }

                /*lq 新报告速率下，一帧中的第二次发射 */
                SOTDMA_SetNextNs(pSotdmaParaNew);
                SOTDMA_GetNextNs(pSotdmaParaNew);

                tempU16 = SOTDMA_GetAvailableNts(pSotdmaPara,
                    pSotdmaParaNew->NominalSlot,
                    pSotdmaParaNew->SelectIntervalOffset);
                
                if (tempU16 != SLOT_INVALID_VALUE)
                {
                    /*lq 使用旧报告速率下已预占的时隙作为新报告速率下的发射时隙 */
                    SOTDMA_SetNextNts(pSotdmaParaNew, tempU16);
                    SOTDMA_SetNextTmo(pSotdmaParaNew);

                    if (pSotdmaParaNew->NominalSlot == pSotdmaParaNew->NominalStartSlot)
                    {
                        /*lq 转入改变报告间隔状态3 */
                        pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CHANGE_REPORT_INTERVAL_3;                                           
                    }
                    else
                    {                 
                        /*lq 初始化18号消息 */
                        /*lq 使用SOTDMA通信状态和参数SotdmaPara，SendMsg18()调用需在SOTDMA_GetNextNts()调用之前*/
                        commState.SOTDMACommState.SyncState = TDMA_GetSyncState();
                        commState.SOTDMACommState.SlotTimeout = pSotdmaPara->SlotTimeout;
                        commState.SOTDMACommState.SubMessage.SubMsg =
                            SOTDMA_GetSubMsg(commState.SOTDMACommState.SlotTimeout,
                            pSotdmaPara);
                        TDMA_SendMsg(channel, pSotdmaPara->NominalTxSlot,
                            AIS_MSG_COMMSTATE_SOTDMA, commState);       //lq 调用参数需提供channel, txSlot, commStateType, commState

                        /*lq 新报告速率和就报告速率下的发射时隙重合，合为一次发射 */
                        SOTDMA_GetNextNts(pSotdmaPara);
                        SOTDMA_GetNextTmo(pSotdmaPara);
                        SOTDMA_GetNextNts(pSotdmaParaNew);
                        SOTDMA_GetNextTmo(pSotdmaParaNew);
    
                        /*lq 一帧中的报告次数计数加1 */
                        SOTDMA_FrameReportCntIncrease(pSotdmaPara);
                        SOTDMA_FrameReportCntIncrease(pSotdmaParaNew);

                        /*lq 转入改变报告间隔状态2 */
                        pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CHANGE_REPORT_INTERVAL_2;
                    }
                }
                else
                {
                    /*lq 选择新的NTS */
                    if (pSotdmaParaNew->NominalSlot < currSlot)
                    {
                        tempS32 = currFrame + 1;
                    }
                    else
                    {
                        tempS32 = currFrame;
                    }
                    tempU16 = SelectCandidateSlot(channel, pSotdmaParaNew->NominalSlot,
                        pSotdmaParaNew->SelectIntervalOffset, tempS32, SLOT_CAND_SELECT_BI);
                    if (IsSlotValid(tempU16) == FALSE)
                    {
                        tempU16 = pSotdmaParaNew->NominalSlot;
                    }

                    /*lq 计算slotOffset */
                    pSotdmaPara->SlotOffset = tempU16 - pSotdmaPara->NominalTxSlot
                        + FRAME_SLOT_SIZE;

                    SOTDMA_SetNextNts(pSotdmaParaNew, tempU16);
                    SOTDMA_SetNextTmo(pSotdmaParaNew);
                   
                    /*lq 初始化18号消息 */
                    /*lq 使用ITDMA通信状态（keepFlag = 1）和参数SotdmaPara，SendMsg18()调用需在SOTDMA_GetNextNts()调用之前*/
                    commState.ITDMACommState.SyncState = TDMA_GetSyncState();
                    commState.ITDMACommState.SlotIncrement =
                        ITDMA_CalcCommStateSlotIncrement(pSotdmaPara->SlotOffset);
                    commState.ITDMACommState.NumberOfSlots =
                        ITDMA_CalcCommStateNumberOfSlots(1,
                        pSotdmaPara->SlotOffset);
                    commState.ITDMACommState.KeepFlag =
                        ITDMA_CalcCommStateKeepFlag(TRUE);
                    TDMA_SendMsg(channel, pSotdmaPara->NominalTxSlot,
                        AIS_MSG_COMMSTATE_ITDMA, commState);    //lq 调用参数需提供channel, txSlot, commStateType, commState

                    /*lq 新报告速率和就报告速率下的发射时隙重合，合为一次发射 */
                    SOTDMA_GetNextNts(pSotdmaPara);
                    SOTDMA_GetNextTmo(pSotdmaPara);
                    SOTDMA_GetNextNts(pSotdmaParaNew);
                    SOTDMA_GetNextTmo(pSotdmaParaNew);

                    /*lq 一帧中的报告次数计数加1 */
                    SOTDMA_FrameReportCntIncrease(pSotdmaPara);
                    SOTDMA_FrameReportCntIncrease(pSotdmaParaNew);

                    /*lq 转入改变报告间隔状态2 */
                    pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CHANGE_REPORT_INTERVAL_2;
                }
            }            
            
            break;

        case SOTDMA_STA_CHANGE_REPORT_INTERVAL_2 :

            /*lq 等待新报告速率下的下一SI */
            if (SOTDMA_IsAtSi2(currSlot, pSotdmaParaNew->NominalSlot,
                pSotdmaParaNew->SelectIntervalOffset) == TRUE)
            {
                /*lq 等待新报告速率下的下一NTS */
                if (SOTDMA_IsSendTimArrived(pSotdmaParaNew, currFrame, currSlot) == TRUE)
                {
                    /*lq 新报告速率下的下一个NS */
                    SOTDMA_SetNextNs(pSotdmaParaNew);
                    SOTDMA_GetNextNs(pSotdmaParaNew);
    
                    tempU16 = SOTDMA_GetAvailableNts(pSotdmaPara,
                        pSotdmaParaNew->NominalSlot,
                        pSotdmaParaNew->SelectIntervalOffset);

                    if (tempU16 != SLOT_INVALID_VALUE)
                    {
                        /*lq 使用旧报告速率下预占的时隙作为新报告速率下的发射时隙 */
                        SOTDMA_SetNextNts(pSotdmaParaNew, tempU16);
                        SOTDMA_SetNextTmo(pSotdmaParaNew);
                            
                        if (pSotdmaParaNew->NominalSlot == pSotdmaParaNew->NominalStartSlot)
                        {
                            /*lq 转入改变报告间隔状态3 */
                            pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CHANGE_REPORT_INTERVAL_3;                                           
                        }
                        else
                        {
                            /*lq 初始化18号消息 */
                            /*lq 使用SOTDMA通信状态和参数SotdmaParaNew，SendMsg18()调用需在SOTDMA_GetNextNts()调用之前*/
                            commState.SOTDMACommState.SyncState = TDMA_GetSyncState();
                            commState.SOTDMACommState.SlotTimeout = pSotdmaParaNew->SlotTimeout;
                            commState.SOTDMACommState.SubMessage.SubMsg =
                                SOTDMA_GetSubMsg(commState.SOTDMACommState.SlotTimeout,
                                pSotdmaParaNew);
                            TDMA_SendMsg(channel, pSotdmaParaNew->NominalTxSlot,
                                AIS_MSG_COMMSTATE_SOTDMA, commState);       //lq 调用参数需提供channel, txSlot, commStateType, commState

                            SOTDMA_GetNextNts(pSotdmaParaNew);
                            SOTDMA_GetNextTmo(pSotdmaParaNew);
        
                            /*lq 一帧中的报告次数计数加1 */
                            SOTDMA_FrameReportCntIncrease(pSotdmaParaNew);
                        }
                    }
                    else
                    {
                        /*lq 选择新的NTS */
                        if (pSotdmaParaNew->NominalSlot < currSlot)
                        {
                            tempS32 = currFrame + 1;
                        }
                        else
                        {
                            tempS32 = currFrame;
                        }
                        tempU16 = SelectCandidateSlot(channel, pSotdmaParaNew->NominalSlot,
                            pSotdmaParaNew->SelectIntervalOffset, tempS32, SLOT_CAND_SELECT_BI);
    
                        /*lq 计算slotOffset */
                        pSotdmaParaNew->SlotOffset = tempU16 - pSotdmaParaNew->NominalTxSlot
                            + FRAME_SLOT_SIZE;

                        SOTDMA_SetNextNts(pSotdmaParaNew, tempU16);
                        SOTDMA_SetNextTmo(pSotdmaParaNew);
                        
                        /*lq 初始化18号消息 */
                        /*lq 使用ITDMA通信状态（keepFlag = 1）和参数SotdmaParaNew，SendMsg18()调用需在SOTDMA_GetNextNts()调用之前*/
                        commState.ITDMACommState.SyncState = TDMA_GetSyncState();
                        commState.ITDMACommState.SlotIncrement =
                            ITDMA_CalcCommStateSlotIncrement(pSotdmaParaNew->SlotOffset);
                        commState.ITDMACommState.NumberOfSlots =
                            ITDMA_CalcCommStateNumberOfSlots(1,
                            pSotdmaPara->SlotOffset);
                        commState.ITDMACommState.KeepFlag =
                            ITDMA_CalcCommStateKeepFlag(TRUE);
                        TDMA_SendMsg(channel, pSotdmaParaNew->NominalTxSlot,
                            AIS_MSG_COMMSTATE_ITDMA, commState);    //lq 调用参数需提供channel, txSlot, commStateType, commState

                        SOTDMA_GetNextNts(pSotdmaParaNew);
                        SOTDMA_GetNextTmo(pSotdmaParaNew);
    
                        /*lq 一帧中的报告次数计数加1 */
                        SOTDMA_FrameReportCntIncrease(pSotdmaParaNew);       
                    }                    
                }

                /*lq 旧报告速率下的NTS */
                if (SOTDMA_IsSendTimArrived(pSotdmaPara, currFrame, currSlot) == TRUE)
                {
                    pSotdmaPara->SlotTimeout = 0;  //lq 不再预占
                    pSotdmaPara->SlotOffset = 0;

                    /*lq 初始化18号消息 */
                    //lq 此处不发送消息

                    SOTDMA_GetNextNs(pSotdmaPara);
                    SOTDMA_GetNextNts(pSotdmaPara);
                    SOTDMA_GetNextTmo(pSotdmaPara);

                    SOTDMA_FrameReportCntIncrease(pSotdmaPara);                  
                }
            }
            else
            {
                if (SOTDMA_IsSendTimArrived(pSotdmaPara, currFrame, currSlot) == TRUE)
                {
                    pSotdmaPara->SlotTimeout = 0;  //lq 不再预占
                    pSotdmaPara->SlotOffset = 0;

                    /*lq 初始化18号消息 */
                    /*lq 使用SOTDMA通信状态和参数SotdmaPara，SendMsg18()调用需在SOTDMA_GetNextNts()调用之前*/
                    commState.SOTDMACommState.SyncState = TDMA_GetSyncState();
                    commState.SOTDMACommState.SlotTimeout = pSotdmaPara->SlotTimeout;
                    commState.SOTDMACommState.SubMessage.SubMsg =
                        SOTDMA_GetSubMsg(commState.SOTDMACommState.SlotTimeout,
                        pSotdmaPara);
                    TDMA_SendMsg(channel, pSotdmaPara->NominalTxSlot,
                        AIS_MSG_COMMSTATE_SOTDMA, commState);       //lq 调用参数需提供channel, txSlot, commStateType, commState

                    SOTDMA_GetNextNs(pSotdmaPara);
                    SOTDMA_GetNextNts(pSotdmaPara);
                    SOTDMA_GetNextTmo(pSotdmaPara);

                    SOTDMA_FrameReportCntIncrease(pSotdmaPara);
                }            
            }

            break;

        case SOTDMA_STA_CHANGE_REPORT_INTERVAL_3 :

            /*lq 使用新报告速率下的参数 */
            SOTDMA_SotdmaParaStructCopy(pSotdmaPara, pSotdmaParaNew);
            SOTDMA_SotdmaParaStructReset(pSotdmaParaNew);

            /*lq 转入连续工作阶段 */
            pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CONTINUOUS_PHASE;
            
            break;

        default :
            
            SOTDMA_SotdmaSchedStructInit(pSotdmaSchedStruct);
            SOTDMA_SotdmaSchedEnable(FALSE, pSotdmaCtrlStruct);
            break;
    }
}

/*******************************************************************************
* 名称  : SOTDMA_ScheduleDual
* 描述  : 双信道SOTDMA调度
* 输入  : pSotdmaSchedDualStruct - 指向SotdmaSchedDualStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SOTDMA_ScheduleDual(SotdmaSchedDualStruct *pSotdmaSchedDualStruct)
{
    SotdmaSchedStruct *pSotdmaSchedA;
    SotdmaSchedStruct *pSotdmaSchedB;
    SotdmaParaStruct *pSotdmaParaA;
    SotdmaParaStruct *pSotdmaParaB;
    SotdmaCtrlStruct *pSotdmaCtrlA;
    SotdmaCtrlStruct *pSotdmaCtrlB;
    SotdmaCtrlStruct *pSotdmaCtrlStruct;
    S32 currFrame;
    S16 currSlot;
    U16 tempU16;
    U16 tempU16_1;
    S32 tempS32;

    pSotdmaCtrlStruct = &(pSotdmaSchedDualStruct->SotdmaCtrl);
    pSotdmaSchedA = &(pSotdmaSchedDualStruct->SotdmaSchedA);
    pSotdmaSchedB = &(pSotdmaSchedDualStruct->SotdmaSchedB);
    pSotdmaParaA = &(pSotdmaSchedDualStruct->SotdmaSchedA.SotdmaPara);
    pSotdmaParaB = &(pSotdmaSchedDualStruct->SotdmaSchedB.SotdmaPara);
    pSotdmaCtrlA = &(pSotdmaSchedDualStruct->SotdmaSchedA.SotdmaCtrl);
    pSotdmaCtrlB = &(pSotdmaSchedDualStruct->SotdmaSchedB.SotdmaCtrl);

    currFrame = GetFrameNum();
    currSlot = GetSlotNum();

    switch (pSotdmaCtrlStruct->CtrlState)
    {
        case SOTDMA_STA_IDLE :

            if (pSotdmaCtrlStruct->SotdmaEn == TRUE)
            {
                pSotdmaCtrlStruct->SotdmaEn = FALSE;

                /*lq 转入开始状态 */
                pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_START;
            }

            break;

        case SOTDMA_STA_START :

            pSotdmaCtrlStruct->TimeCnt = SetTimePeriodProcess(pSotdmaCtrlStruct->TimePeriod);

            /*lq 转入初始化阶段 */
            pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_INIT_PHASE;            
            
            break;

        case SOTDMA_STA_INIT_PHASE :
            
            /*lq SOTDMA初始化阶段1min等待 */
            if (pSotdmaCtrlStruct->TimeCnt == GetLocalClockTickProcess())
            {
                pSotdmaCtrlStruct->TimeCnt = SetTimePeriodProcess(pSotdmaCtrlStruct->TimePeriod);                
                
                pSotdmaCtrlStruct->TimeoutCnt++;
                if (pSotdmaCtrlStruct->TimeoutCnt >= pSotdmaCtrlStruct->Timeout)
                {
                    pSotdmaCtrlStruct->TimeoutCnt = 0;

                    /*lq 转入网络进入阶段 */
                    pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_NETWORK_ENTRY_PHASE;                 
                }                 
            }

            break;

        case SOTDMA_STA_NETWORK_ENTRY_PHASE :
            
            /*lq 选择NSS */
            SOTDMA_SetNssDual(ENUM_AIS_CHANNEL_A, CalcSlotNum(0, currSlot,
                AIS_SLOT_INTERVAL_SEND_TO_TX));

	        debug_printf("$[%6d:%4d] %s : NETWORK_ENTRY_PHASE, Channel A NSS is [%4d] , Channel B NSS is [%4d]!\r\n",
                currFrame, currSlot, __func__, pSotdmaParaA->NominalStartSlot,
                pSotdmaParaB->NominalStartSlot);                        
            
            /*lq 选择信道A的NTS */
            if (pSotdmaParaA->NominalStartSlot < currSlot)
            {
                /*lq NSS位于当前帧的下一帧 */
                tempS32 = currFrame + 1;
            }
            else
            {
                /*lq NSS位于当前帧 */
                tempS32 = currFrame;
            }
            tempU16 = SelectCandidateSlot(ENUM_AIS_CHANNEL_A, pSotdmaParaA->NominalStartSlot,
                pSotdmaParaA->SelectIntervalOffset, tempS32, SLOT_CAND_SELECT_UNI);     //lq 使用单边选择，避免选择的时隙小于当前时隙
            
            /*lq 选择信道B的NTS */
            if (pSotdmaParaB->NominalStartSlot < currSlot)
            {
                /*lq NSS位于当前帧的下一帧 */
                tempS32 = currFrame + 1;
            }
            else
            {
                /*lq NSS位于当前帧 */
                tempS32 = currFrame;
            }
            tempU16_1 = SelectCandidateSlot(ENUM_AIS_CHANNEL_B, pSotdmaParaB->NominalStartSlot,
                pSotdmaParaB->SelectIntervalOffset, tempS32, SLOT_CAND_SELECT_UNI);     //lq 使用单边选择，避免选择的时隙小于当前时隙

            /*lq 
             *  若无可用候选发射时隙，则使用标称时隙作为发射时隙，
             *  （之前的方式为等两个信道的候选发射时隙均有效后再转入下一阶段
             *   避免一直无可用的发射时隙）
             */
            if (IsSlotValid(tempU16) == FALSE)
            {
                tempU16 = pSotdmaParaA->NominalStartSlot;
            }

            if (IsSlotValid(tempU16_1) == FALSE)
            {
                tempU16_1 = pSotdmaParaB->NominalStartSlot;
            }

            SOTDMA_SetFirstNs(pSotdmaParaA);
            SOTDMA_GetNextNs(pSotdmaParaA);
            SOTDMA_SetNextNts(pSotdmaParaA, tempU16);
            SOTDMA_GetNextNts(pSotdmaParaA);
            SOTDMA_SetNextTmo(pSotdmaParaA);
            SOTDMA_GetNextTmo(pSotdmaParaA);
            pSotdmaParaA->FrameReportCnt = 0;

            SOTDMA_SetFirstNs(pSotdmaParaB);
            SOTDMA_GetNextNs(pSotdmaParaB);
            SOTDMA_SetNextNts(pSotdmaParaB, tempU16_1);
            SOTDMA_GetNextNts(pSotdmaParaB);
            SOTDMA_SetNextTmo(pSotdmaParaB);
            SOTDMA_GetNextTmo(pSotdmaParaB);
            pSotdmaParaB->FrameReportCnt = 0;
            
            /*lq 转入第一帧阶段 */
            pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CONTINUOUS_PHASE;
            pSotdmaCtrlA->CtrlState = SOTDMA_STA_FIRST_FRAME_PHASE;
            pSotdmaCtrlB->CtrlState = SOTDMA_STA_FIRST_FRAME_PHASE; 

            break;

        case SOTDMA_STA_CONTINUOUS_PHASE :

            /*lq 信道A/B调度 */
            SOTDMA_Schedule(pSotdmaSchedA, currFrame, currSlot);
            SOTDMA_Schedule(pSotdmaSchedB, currFrame, currSlot);

            break;

        default :
            
            SOTDMA_SotdmaSchedDualStructInit(pSotdmaSchedDualStruct);
            SOTDMA_SotdmaSchedEnable(FALSE, pSotdmaCtrlStruct);
            
            break;
    }

}

#if 0
/*******************************************************************************
* 名称  : SOTDMA_SetNss
* 描述  : SOTDMA设置标称起始时隙（）
* 输入  : pSotdmaParaStruct - 指向SotdmaParaStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SOTDMA_ScheduleDual(SotdmaSchedStruct *pSotdmaSchedStruct)
{
    SotdmaCtrlStruct *pSotdmaCtrlStruct;
    SotdmaParaStruct *pSotdmaParaA;
    SotdmaParaStruct *pSotdmaParaB;
    SotdmaParaStruct *pSotdmaParaANew;
    SotdmaParaStruct *pSotdmaParaBNew;
    S32 currFrame;
    S16 currSlot;
    U16 tempU16;
    U16 tempU16_1;
    S32 tempS32;

    pSotdmaCtrlStruct = &(pSotdmaSchedStruct->SotdmaCtrl);
    pSotdmaParaA = &(pSotdmaSchedStruct->SotdmaParaA);
    pSotdmaParaB = &(pSotdmaSchedStruct->SotdmaParaB);
    pSotdmaParaANew = &(pSotdmaSchedStruct->SotdmaParaANew);
    pSotdmaParaBNew = &(pSotdmaSchedStruct->SotdmaParaBNew);

    currFrame = GetFrameNum();
    currSlot = GetSlotNum();

    switch (pSotdmaCtrlStruct->CtrlState)
    {
        case SOTDMA_STA_IDLE :

            if (pSotdmaCtrlStruct->SotdmaEn == TRUE)
            {
                pSotdmaCtrlStruct->SotdmaEn = FALSE;

                /*lq 转入开始状态 */
                pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_START;
            }

            break;

        case SOTDMA_STA_START :

            pSotdmaCtrlStruct->TimeCnt = SetTimePeriodProcess(pSotdmaCtrlStruct->TimePeriod);

            /*lq 转入初始化阶段 */
            pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_INIT_PHASE;            
            
            break;

        case SOTDMA_STA_INIT_PHASE :
            
            /*lq SOTDMA初始化阶段1min等待 */
            if (pSotdmaCtrlStruct->TimeCnt == GetLocalClockTickProcess())
            {
                pSotdmaCtrlStruct->TimeCnt = SetTimePeriodProcess(pSotdmaCtrlStruct->TimePeriod);                
                
                pSotdmaCtrlStruct->TimeoutCnt++;
                if (pSotdmaCtrlStruct->TimeoutCnt >= pSotdmaCtrlStruct->Timeout)
                {
                    pSotdmaCtrlStruct->TimeoutCnt = 0;

                    /*lq 转入网络进入阶段 */
                    pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_NETWORK_ENTRY_PHASE;                 
                }                 
            }

            break;

        case SOTDMA_STA_NETWORK_ENTRY_PHASE :
            
            /*lq 选择NSS */
            tempU16 = (currSlot + (rand() % pSotdmaParaA->NominalIncrement)) % FRAME_SLOT_SIZE;
            SotdmaSetNssDual(ENUM_AIS_CHANNEL_A, tempU16);
            
            /*lq 选择信道A的NTS */
            tempU16 = SelectCandidateSlot(ENUM_AIS_CHANNEL_A, pSotdmaParaA->NominalStartSlot,
                pSotdmaParaA->SelectIntervalOffset, currFrame, SLOT_CAND_SELECT_UNI);   //lq 使用单边选择，避免选择的时隙小于当前时隙
            
            /*lq 选择信道B的NTS */
            tempU16_1 = SelectCandidateSlot(ENUM_AIS_CHANNEL_B, pSotdmaParaB->NominalStartSlot,
                pSotdmaParaB->SelectIntervalOffset, currFrame, SLOT_CAND_SELECT_UNI);   //lq 使用单边选择，避免选择的时隙小于当前时隙

            if ((IsSlotValid(tempU16) == TRUE) && (IsSlotValid(tempU16_1) == TRUE))
            {
                SOTDMA_SetFirstNs(pSotdmaParaA);
                SOTDMA_GetNextNs(pSotdmaParaA);
                SOTDMA_SetNextNts(pSotdmaParaA, tempU16);
                SOTDMA_GetNextNts(pSotdmaParaA);
                SOTDMA_SetNextTmo(pSotdmaParaA);
                SOTDMA_GetNextTmo(pSotdmaParaA);
                pSotdmaParaA->FrameReportCnt = 0;

                SOTDMA_SetFirstNs(pSotdmaParaB);
                SOTDMA_GetNextNs(pSotdmaParaB);
                SOTDMA_SetNextNts(pSotdmaParaB, tempU16_1);
                SOTDMA_GetNextNts(pSotdmaParaB);
                SOTDMA_SetNextTmo(pSotdmaParaB);
                SOTDMA_GetNextTmo(pSotdmaParaB);
                pSotdmaParaB->FrameReportCnt = 0;
                
                /*lq 转入第一帧阶段 */
                pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_FIRST_FRAME_PHASE;
            } 
            break;

        case SOTDMA_STA_FIRST_FRAME_PHASE :
            
            /*lq 等待信道A的NTS */
            if (pSotdmaParaA->FrameReportCnt < pSotdmaParaA->ReportRate)
            {
                if (SOTDMA_IsAtNts(pSotdmaParaA, currSlot) == TRUE)
                {
                    /*lq 判断是否是一帧中的最后一次发射 */
                    if ((pSotdmaParaA->FrameReportCnt + 1) >= pSotdmaParaA->ReportRate)
                    {
                        pSotdmaParaA->SlotOffset = 0;
                    }
                    else
                    {
                        /*lq 设置下一个NS */
                        tempU16_1 = SOTDMA_SetNextNs(pSotdmaParaA);
    
                        /*lq 选择下一个NTS */
                        if (tempU16_1 < currSlot)
                        {
                            tempS32 = currFrame + 1;
                        }
                        else
                        {
                            tempS32 = currFrame;
                        }
                        tempU16 = SelectCandidateSlot(ENUM_AIS_CHANNEL_A, tempU16_1,
                            pSotdmaParaA->SelectIntervalOffset, tempS32, SLOT_CAND_SELECT_BI);
    
                        if (IsSlotValid(tempU16) == FALSE)
                        {
                            tempU16 = tempU16_1;
                        }
    
                        /*lq 计算slotOffset */
                        pSotdmaParaA->SlotOffset = (tempU16 - pSotdmaParaA->NominalTxSlot
                            + FRAME_SLOT_SIZE) % FRAME_SLOT_SIZE;                    
                        
                        /*lq 设置下一个NTS */
                        SOTDMA_SetNextNts(pSotdmaParaA, tempU16);

                        /*lq 设置下一个slotTimeout*/
                        SOTDMA_SetNextTmo(pSotdmaParaA);
                    }
    
                    /*lq 初始化18号消息 */
                    /*lq 使用信道A和ITDMA方式，该调用需在SOTDMA_GetNextNts()调用之前*/
                    SendMsg18();    //lq... 接口SendMsg18()需重新实现，调用参数需提供channel(txSlot, slotOffset, slotTimeout), tdmaSchema
    
                    SOTDMA_GetNextNs(pSotdmaParaA);
                    SOTDMA_GetNextNts(pSotdmaParaA);
                    SOTDMA_GetNextTmo(pSotdmaParaA);
                    SOTDMA_FrameReportCntIncrease(pSotdmaParaA);
                }            
            }

            /*lq 等待信道B的NTS */
            if (pSotdmaParaB->FrameReportCnt < pSotdmaParaB->ReportRate)
            {
                if (SOTDMA_IsAtNts(pSotdmaParaB, currSlot) == TRUE)
                {
                    /*lq 判断是否是一帧中的最后一次发射 */
                    if ((pSotdmaParaB->FrameReportCnt + 1) >= pSotdmaParaB->ReportRate)
                    {
                        pSotdmaParaB->SlotOffset = 0;
                    }
                    else
                    {
                        /*lq 设置下一个NS */
                        tempU16_1 = SOTDMA_SetNextNs(pSotdmaParaB);
    
                        /*lq 选择下一个NTS */
                        if (tempU16_1 < currSlot)
                        {
                            tempS32 = currFrame + 1;
                        }
                        else
                        {
                            tempS32 = currFrame;
                        }
                        tempU16 = SelectCandidateSlot(ENUM_AIS_CHANNEL_B, tempU16_1,
                            pSotdmaParaB->SelectIntervalOffset, tempS32, SLOT_CAND_SELECT_BI);
    
                        if (IsSlotValid(tempU16) == FALSE)
                        {
                            tempU16 = tempU16_1;
                        }
    
                        /*lq 计算slotOffset */
                        pSotdmaParaB->SlotOffset = (tempU16 - pSotdmaParaB->NominalTxSlot
                            + FRAME_SLOT_SIZE) % FRAME_SLOT_SIZE;                    
                        
                        /*lq 设置下一个NTS */
                        SOTDMA_SetNextNts(pSotdmaParaB, tempU16);

                        /*lq 设置下一个slotTimeout*/
                        SOTDMA_SetNextTmo(pSotdmaParaB);
                    }
    
                    /*lq 初始化18号消息 */
                    /*lq 使用信道A和ITDMA方式，该调用需在SOTDMA_GetNextNts()调用之前*/
                    SendMsg18();    //lq... 接口SendMsg18()需重新实现，调用参数需提供channel(txSlot, slotOffset, slotTimeout), tdmaSchema
    
                    SOTDMA_GetNextNs(pSotdmaParaB);
                    SOTDMA_GetNextNts(pSotdmaParaB);
                    SOTDMA_GetNextTmo(pSotdmaParaB);
                    SOTDMA_FrameReportCntIncrease(pSotdmaParaB);
                }            
            }

            /*lq 判断第一帧阶段是否完成 */
            if ((pSotdmaParaA->FrameReportCnt >= pSotdmaParaA->ReportRate)
                && (pSotdmaParaB->FrameReportCnt >= pSotdmaParaB->ReportRate))
            {
                pSotdmaParaA->FrameReportCnt = 0;
                pSotdmaParaB->FrameReportCnt = 0;

                /*lq 转入连续工作阶段 */
                pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CONTINUOUS_PHASE;
            }

            break;

        case SOTDMA_STA_CONTINUOUS_PHASE :

            /*lq 等待信道A的NTS */
            if (SOTDMA_IsAtNts(pSotdmaParaA, currSlot) == TRUE)            
            {
                /*lq 判断是否为预占的最后一帧 */
                if (pSotdmaParaA->SlotTimeout == 0)
                {
                    /*lq 为当前的NS选择新的NTS */    
                    tempU16 = SelectCandidateSlot(ENUM_AIS_CHANNEL_A, pSotdmaParaA->NominalSlot,
                        pSotdmaParaA->SelectIntervalOffset, currFrame, SLOT_CAND_SELECT_BI);

                    if (IsSlotValid(tempU16) == FALSE)
                    {
                        tempU16 = pSotdmaParaA->NominalSlot;                    
                    }

                    /*lq 计算slotOffset */
                    pSotdmaParaA->SlotOffset = tempU16 - pSotdmaParaA->NominalTxSlot
                        + FRAME_SLOT_SIZE;
                    
                    /*lq 更新NTS */                                            
                    SOTDMA_SetNts(pSotdmaParaA, pSotdmaParaA->FrameReportCnt,
                        tempU16);

                    /*lq 更新slotTimeout */                                            
                    pSotdmaParaA->SlotTimeout = SOTDMA_GetTmo(pSotdmaParaA,
                        pSotdmaParaA->FrameReportCnt);
                }
                else
                {
                    pSotdmaParaA->SlotOffset = 0;                
                }

                /*lq 初始化18号消息 */
                SendMsg18();    //lq... 接口SendMsg18()需重新实现，使用信道A和SOTDMA方式

                SOTDMA_GetNextNs(pSotdmaParaA);
                SOTDMA_GetNextNts(pSotdmaParaA);
                SOTDMA_GetNextTmo(pSotdmaParaA);

                /*lq 一帧中的报告次数计数加1 */
                SOTDMA_FrameReportCntIncrease(pSotdmaParaA);
            }

            /*lq 等待信道B的NTS */
            if (SOTDMA_IsAtNts(pSotdmaParaB, currSlot) == TRUE)            
            {
                /*lq 判断是否为预占的最后一帧 */
                if (pSotdmaParaB->SlotTimeout == 0)
                {
                    /*lq 为当前的NS选择新的NTS */    
                    tempU16 = SelectCandidateSlot(ENUM_AIS_CHANNEL_B, pSotdmaParaB->NominalSlot,
                        pSotdmaParaB->SelectIntervalOffset, currFrame, SLOT_CAND_SELECT_BI);

                    if (IsSlotValid(tempU16) == FALSE)
                    {
                        tempU16 = pSotdmaParaB->NominalSlot;                    
                    }

                    /*lq 计算slotOffset */
                    pSotdmaParaB->SlotOffset = tempU16 - pSotdmaParaB->NominalTxSlot
                        + FRAME_SLOT_SIZE;
                                        
                    /*lq 更新NTS */                                            
                    SOTDMA_SetNts(pSotdmaParaB, pSotdmaParaB->FrameReportCnt,
                        tempU16);

                    /*lq 更新slotTimeout */                                            
                    pSotdmaParaB->SlotTimeout = SOTDMA_GetTmo(pSotdmaParaB,
                        pSotdmaParaB->FrameReportCnt);

                }
                else
                {
                    pSotdmaParaB->SlotOffset = 0;                
                }

                /*lq 初始化18号消息 */
                SendMsg18();    //lq... 接口SendMsg18()需重新实现，使用信道B和SOTDMA方式

                SOTDMA_GetNextNs(pSotdmaParaB);
                SOTDMA_GetNextNts(pSotdmaParaB);
                SOTDMA_GetNextTmo(pSotdmaParaB);

                /*lq 一帧中的报告次数计数加1 */
                SOTDMA_FrameReportCntIncrease(pSotdmaParaB);
            }

            if (SOTDMA_IsRrChanged(ENUM_AIS_CHANNEL_A) == TRUE)
            {
                SOTDMA_SotdmaParaStructReset(pSotdmaParaANew);

                /*lq 转入改变报告间隔状态1 */
                pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CHANGE_REPORT_INTERVAL_1;                             
            }

            break;

        case SOTDMA_STA_CHANGE_REPORT_INTERVAL_1 :

            /*lq 等待信道A的NTS */
            if (SOTDMA_IsAtNts(pSotdmaParaA, currSlot) == TRUE)            
            {
                /*lq 新报告速率下一帧中的第一次发射 */
                SOTDMA_SetNss(pSotdmaParaANew, pSotdmaParaA->NominalSlot);
                SOTDMA_SetFirstNs(pSotdmaParaANew);
                SOTDMA_GetNextNs(pSotdmaParaANew);
                SOTDMA_SetNextNts(pSotdmaParaANew, pSotdmaParaA->NominalTxSlot);
                SOTDMA_GetNextNts(pSotdmaParaANew);
                SOTDMA_SetNextTmo(pSotdmaParaANew);
                SOTDMA_GetNextTmo(pSotdmaParaANew);

                if (pSotdmaParaA->SlotTimeout == 0)
                {
                    pSotdmaParaA->SlotTimeout = 1;
                }

                /*lq 新报告速率下一帧中的第二次发射 */
                SOTDMA_SetNextNs(pSotdmaParaANew);
                SOTDMA_GetNextNs(pSotdmaParaANew);

                tempU16 = SOTDMA_GetAvailableNts(pSotdmaParaA,
                    pSotdmaParaANew->NominalSlot,
                    pSotdmaParaANew->SelectIntervalOffset);
                
                if (tempU16 != SLOT_INVALID_VALUE)
                {
                    /*lq 使用旧报告速率下已预占的时隙作为新报告速率下的发射时隙 */
                    SOTDMA_SetNextNts(pSotdmaParaANew, tempU16);
                    SOTDMA_SetNextTmo(pSotdmaParaANew);

                    if (pSotdmaParaANew->NominalSlot == pSotdmaParaANew->NominalStartSlot)
                    {
                        /*lq 转入改变报告间隔状态3 */
                        pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CHANGE_REPORT_INTERVAL_3;                                           
                    }
                    else
                    {                 
                        /*lq 初始化18号消息 */
                        SendMsg18();    //lq... 使用参数SotdmaParaA，使用SOTDMA方式

                        /*lq 新报告速率和就报告速率下的发射时隙重合，合为一次发射 */
                        SOTDMA_GetNextNts(pSotdmaParaA);
                        SOTDMA_GetNextTmo(pSotdmaParaA);
                        SOTDMA_GetNextNts(pSotdmaParaANew);
                        SOTDMA_GetNextTmo(pSotdmaParaANew);
    
                        /*lq 一帧中的报告次数计数加1 */
                        SOTDMA_FrameReportCntIncrease(pSotdmaParaA);
                        SOTDMA_FrameReportCntIncrease(pSotdmaParaANew);

                        /*lq 转入改变报告间隔状态2 */
                        pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CHANGE_REPORT_INTERVAL_2;
                    }
                }
                else
                {
                    /*lq 选择新的NTS */
                    if (pSotdmaParaANew->NominalSlot < currSlot)
                    {
                        tempS32 = currFrame + 1;
                    }
                    else
                    {
                        tempS32 = currFrame;
                    }
                    tempU16 = SelectCandidateSlot(ENUM_AIS_CHANNEL_A, pSotdmaParaANew->NominalSlot,
                        pSotdmaParaANew->SelectIntervalOffset, tempS32, SLOT_CAND_SELECT_BI);
                    if (IsSlotValid(tempU16) == FALSE)
                    {
                        tempU16 = pSotdmaParaANew->NominalSlot;
                    }

                    /*lq 计算slotOffset */
                    pSotdmaParaA->SlotOffset = tempU16 - pSotdmaParaA->NominalTxSlot
                        + FRAME_SLOT_SIZE;

                    SOTDMA_SetNextNts(pSotdmaParaANew, tempU16);
                    SOTDMA_SetNextTmo(pSotdmaParaANew);
                   
                    /*lq 初始化18号消息 */
                    SendMsg18();    //lq... 使用参数SotdmaParaA，使用ITDMA方式（keepFlag = 1）

                    /*lq 新报告速率和就报告速率下的发射时隙重合，合为一次发射 */
                    SOTDMA_GetNextNts(pSotdmaParaA);
                    SOTDMA_GetNextTmo(pSotdmaParaA);
                    SOTDMA_GetNextNts(pSotdmaParaANew);
                    SOTDMA_GetNextTmo(pSotdmaParaANew);

                    /*lq 一帧中的报告次数计数加1 */
                    SOTDMA_FrameReportCntIncrease(pSotdmaParaA);
                    SOTDMA_FrameReportCntIncrease(pSotdmaParaANew);

                    /*lq 转入改变报告间隔状态2 */
                    pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CHANGE_REPORT_INTERVAL_2;
                }
            }            
            
            break;

        case SOTDMA_STA_CHANGE_REPORT_INTERVAL_2 :

            /*lq 等待新报告速率下的下一SI */
            if (SOTDMA_IsAtSi(currSlot, pSotdmaParaANew->NominalSlot,
                pSotdmaParaANew->SelectIntervalOffset) == TRUE)
            {
                /*lq 等待新报告速率下的下一NTS */
                if (SOTDMA_IsAtNts(pSotdmaParaANew, currSlot) == TRUE)
                {
                    /*lq 新报告速率下的下一个NS */
                    SOTDMA_SetNextNs(pSotdmaParaANew);
                    SOTDMA_GetNextNs(pSotdmaParaANew);
    
                    tempU16 = SOTDMA_GetAvailableNts(pSotdmaParaA,
                        pSotdmaParaANew->NominalSlot,
                        pSotdmaParaANew->SelectIntervalOffset);

                    if (tempU16 != SLOT_INVALID_VALUE)
                    {
                        /*lq 使用旧报告速率下预占的时隙作为新报告速率下的发射时隙 */
                        SOTDMA_SetNextNts(pSotdmaParaANew, tempU16);
                        SOTDMA_SetNextTmo(pSotdmaParaANew);
                            
                        if (pSotdmaParaANew->NominalSlot == pSotdmaParaANew->NominalStartSlot)
                        {
                            /*lq 转入改变报告间隔状态3 */
                            pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CHANGE_REPORT_INTERVAL_3;                                           
                        }
                        else
                        {
                            /*lq 初始化18号消息 */
                            SendMsg18();    //lq... 使用参数SotdmaParaANew，使用SOTDMA方式

                            SOTDMA_GetNextNts(pSotdmaParaANew);
                            SOTDMA_GetNextTmo(pSotdmaParaANew);
        
                            /*lq 一帧中的报告次数计数加1 */
                            SOTDMA_FrameReportCntIncrease(pSotdmaParaANew);
                        }
                    }
                    else
                    {
                        /*lq 选择新的NTS */
                        if (pSotdmaParaANew->NominalSlot < currSlot)
                        {
                            tempS32 = currFrame + 1;
                        }
                        else
                        {
                            tempS32 = currFrame;
                        }
                        tempU16 = SelectCandidateSlot(ENUM_AIS_CHANNEL_A, pSotdmaParaANew->NominalSlot,
                            pSotdmaParaANew->SelectIntervalOffset, tempS32, SLOT_CAND_SELECT_BI);
    
                        SOTDMA_SetNextNts(pSotdmaParaANew, tempU16);
                        SOTDMA_SetNextTmo(pSotdmaParaANew);
                        
                        /*lq 初始化18号消息 */
                        SendMsg18();    //lq... 使用参数SotdmaParaANew，使用ITDMA方式

                        SOTDMA_GetNextNts(pSotdmaParaANew);
                        SOTDMA_GetNextTmo(pSotdmaParaANew);
    
                        /*lq 一帧中的报告次数计数加1 */
                        SOTDMA_FrameReportCntIncrease(pSotdmaParaANew);       
                    }                    
                }

                /*lq 旧报告速率下的NTS */
                if (SOTDMA_IsAtNts(pSotdmaParaA, currSlot) == TRUE)
                {
                    pSotdmaParaA->SlotTimeout = 0;  //lq 不再预占
                    pSotdmaParaA->SlotOffset = 0;

                    /*lq 初始化18号消息 */
                    //lq 此处不发送消息

                    SOTDMA_GetNextNs(pSotdmaParaA);
                    SOTDMA_GetNextNts(pSotdmaParaA);
                    SOTDMA_GetNextTmo(pSotdmaParaA);

                    SOTDMA_FrameReportCntIncrease(pSotdmaParaA);                  
                }
            }
            else
            {
                if (SOTDMA_IsAtNts(pSotdmaParaA, currSlot) == TRUE)
                {
                    pSotdmaParaA->SlotTimeout = 0;  //lq 不再预占
                    pSotdmaParaA->SlotOffset = 0;

                    /*lq 初始化18号消息 */
                    SendMsg18();    //lq... 使用参数SotdmaParaA，使用SOTDMA方式

                    SOTDMA_GetNextNs(pSotdmaParaA);
                    SOTDMA_GetNextNts(pSotdmaParaA);
                    SOTDMA_GetNextTmo(pSotdmaParaA);

                    SOTDMA_FrameReportCntIncrease(pSotdmaParaA);
                }            
            }

            break;

        case SOTDMA_STA_CHANGE_REPORT_INTERVAL_3 :

            /*lq 使用新报告速率下的参数 */
            SOTDMA_SotdmaParaStructCopy(pSotdmaParaA, pSotdmaParaANew);
            SOTDMA_SotdmaParaStructReset(pSotdmaParaANew);

            /*lq 转入连续工作阶段 */
            pSotdmaCtrlStruct->CtrlState = SOTDMA_STA_CONTINUOUS_PHASE;
            
            break;

        default :
            
            SOTDMA_SotdmaCtrlStructInit(pSotdmaCtrlStruct);

            break;
    }
}

#endif

/*******************************************************************************
* 名称  : ITDMA_ItdmaParaStructInit
* 描述  : ItdmaParaStruct结构体初始化
* 输入  : pItdmaParaStruct - 指向ItdmaParaStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void ITDMA_ItdmaParaStructInit(ItdmaParaStruct *pItdmaParaStruct)
{
    pItdmaParaStruct->NominalStartFrame = 0;
    pItdmaParaStruct->NominalStartSlot = 0;
    pItdmaParaStruct->NominalFrame = 0;
    pItdmaParaStruct->NominalSlot = 0;
    pItdmaParaStruct->NominalFrameNext = 0;
    pItdmaParaStruct->NominalSlotNext = 0;
    pItdmaParaStruct->NominalTxFrame = 0;
    pItdmaParaStruct->NominalTxSlot = 0;
    pItdmaParaStruct->NominalTxFrameNext = 0;
    pItdmaParaStruct->NominalTxSlotNext = 0;

    pItdmaParaStruct->NominalIncrement = ITDMA_REPORT_INTERVAL_SLOT_6M;
    pItdmaParaStruct->SelectIntervalOffset =
        ITDMA_CalcSelectIntervalOffset(pItdmaParaStruct->NominalIncrement);
    pItdmaParaStruct->SlotOffset = 0;
    pItdmaParaStruct->SlotQuantity = 0;
    pItdmaParaStruct->SlotKeep = FALSE;
}

/*******************************************************************************
* 名称  : ITDMA_ItdmaCtrlStructInit
* 描述  : ItdmaCtrlStruct结构体初始化
* 输入  : pItdmaCtrlStruct - 指向ItdmaCtrlStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void ITDMA_ItdmaCtrlStructInit(ItdmaCtrlStruct *pItdmaCtrlStruct)
{
    pItdmaCtrlStruct->TimeCnt = 0;
    pItdmaCtrlStruct->TimePeriod = ITDMA_CTRL_TIM_PERIOD;
    pItdmaCtrlStruct->Timeout = 0;
    pItdmaCtrlStruct->TimeoutCnt = 0;
    pItdmaCtrlStruct->CtrlState = ITDMA_STA_IDLE;
    pItdmaCtrlStruct->ItdmaEn = FALSE;
}

/*******************************************************************************
* 名称  : ITDMA_ItdmaSchedStructInit
* 描述  : ItdmaSchedStruct结构体初始化
* 输入  : pItdmaSchedStruct - 指向ItdmaSchedStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void ITDMA_ItdmaSchedStructInit(ItdmaSchedStruct *pItdmaSchedStruct)
{
    ITDMA_ItdmaParaStructInit(&(pItdmaSchedStruct->ItdmaPara));
    ITDMA_ItdmaParaStructInit(&(pItdmaSchedStruct->ItdmaParaNew));
    ITDMA_ItdmaCtrlStructInit(&(pItdmaSchedStruct->ItdmaCtrl));
}

/*******************************************************************************
* 名称  : ITDMA_ItdmaSchedDualStructInit
* 描述  : ItdmaSchedDualStruct结构体初始化
* 输入  : pItdmaSchedDualStruct - 指向ItdmaSchedDualStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void ITDMA_ItdmaSchedDualStructInit(ItdmaSchedDualStruct *pItdmaSchedDualStruct)
{
    ITDMA_ItdmaSchedStructInit(&(pItdmaSchedDualStruct->ItdmaSchedA));
    ITDMA_ItdmaSchedStructInit(&(pItdmaSchedDualStruct->ItdmaSchedB));
    ITDMA_ItdmaCtrlStructInit(&(pItdmaSchedDualStruct->ItdmaCtrl));
}

/*******************************************************************************
* 名称  : ITDMA_ItdmaSchedEnable
* 描述  : ITDMA调度使能配置
* 输入  : flag - 使能标志
          pItdmaCtrlStruct - 指向ItdmaCtrlStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void ITDMA_ItdmaSchedEnable(U8 flag, ItdmaCtrlStruct *pItdmaCtrlStruct)
{
    pItdmaCtrlStruct->CtrlState = ITDMA_STA_IDLE;
    pItdmaCtrlStruct->ItdmaEn = flag;
}

#if   0  // lnw mask  no use
/*******************************************************************************
* 名称  : ITDMA_IsAtNts
* 描述  : ITDMA判断是否位于标称发射时隙
* 输入  : pItdmaParaStruct - 指向ItdmaParaStruct结构体的指针
          frame - 判定帧
          slot - 判定时隙
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static U8 ITDMA_IsAtNts(ItdmaParaStruct *pItdmaParaStruct, S32 frame, S16 slot)
{     
    if ((pItdmaParaStruct->NominalTxFrame == frame)
        && (pItdmaParaStruct->NominalTxSlot == slot))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#endif
/*******************************************************************************
* 名称  : ITDMA_IsNtsArrived
* 描述  : ITDMA判断是否到达标称发射时隙
* 输入  : pItdmaParaStruct - 指向ItdmaParaStruct结构体的指针
          frame - 判定帧
          slot - 判定时隙
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static U8 ITDMA_IsNtsArrived(ItdmaParaStruct *pItdmaParaStruct, S32 frame,
                             S16 slot)
{     
    if (CalcSlotOffset(pItdmaParaStruct->NominalTxFrame,
        pItdmaParaStruct->NominalTxSlot, frame, slot) >= 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : ITDMA_IsSendTimArrived
* 描述  : ITDMA判断发送时隙（送入消息队列的时隙）是否到达
* 输入  : pItdmaParaStruct - 指向ItdmaParaStruct结构体的指针
          frame - 判定帧
          slot - 判定时隙
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static U8 ITDMA_IsSendTimArrived(ItdmaParaStruct *pItdmaParaStruct, S32 frame, S16 slot)
{     
    return ITDMA_IsNtsArrived(pItdmaParaStruct,
            CalcFrameNum(frame, slot, AIS_SLOT_INTERVAL_SEND_TO_TX),
            CalcSlotNum(frame, slot, AIS_SLOT_INTERVAL_SEND_TO_TX));
}

/*******************************************************************************
* 名称  : ITDMA_SetNi
* 描述  : ITDMA设置标称增量
* 输入  : pItdmaParaStruct - 指向ItdmaParaStruct结构体的指针
          slotIncrement - 标称增量
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void ITDMA_SetNi(ItdmaParaStruct *pItdmaParaStruct, S16 slotIncrement)
{     
    if (slotIncrement > ITDMA_REPORT_INTERVAL_SLOT_6M)
    {
        pItdmaParaStruct->NominalIncrement = ITDMA_REPORT_INTERVAL_SLOT_6M;
    }
    else
    {
        pItdmaParaStruct->NominalIncrement = slotIncrement;
    }
}

/*******************************************************************************
* 名称  : ITDMA_SetNi
* 描述  : ITDMA设置标称增量
* 输入  : pItdmaParaStruct - 指向ItdmaParaStruct结构体的指针
          slotIncrement - 标称增量
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void ITDMA_SetSelectIntervalOffset(ItdmaParaStruct *pItdmaParaStruct)
{
    pItdmaParaStruct->SelectIntervalOffset = 
        ITDMA_CalcSelectIntervalOffset(pItdmaParaStruct->NominalIncrement);
}

/*******************************************************************************
* 名称  : ITDMA_SetSlotIncrement
* 描述  : ITDMA设置时隙偏移量
* 输入  : pItdmaParaStruct - 指向ItdmaParaStruct结构体的指针
          slotOffset - 时隙偏移量
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void ITDMA_SetSlotOffset(ItdmaParaStruct *pItdmaParaStruct, S16 slotOffset)
{     
    if (slotOffset > ITDMA_SLOT_OFFSET_MAX)
    {
        pItdmaParaStruct->SlotOffset = ITDMA_SLOT_OFFSET_MAX;        
    }
    else
    {
        pItdmaParaStruct->SlotOffset = slotOffset;    
    }
}

/*******************************************************************************
* 名称  : ITDMA_SetSlotQuantity
* 描述  : ITDMA设置预占时隙数
* 输入  : pItdmaParaStruct - 指向ItdmaParaStruct结构体的指针
          slotQuantity - 预占时隙数
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void ITDMA_SetSlotQuantity(ItdmaParaStruct *pItdmaParaStruct, S16 slotQuantity)
{     
    if (slotQuantity > ITDMA_SLOT_QUANTITY_MAX)
    {
        pItdmaParaStruct->SlotQuantity = ITDMA_SLOT_QUANTITY_MAX;    
    }
    else
    {
        pItdmaParaStruct->SlotQuantity = slotQuantity;
    }
}

/*******************************************************************************
* 名称  : ITDMA_SetSlotKeep
* 描述  : ITDMA设置时隙保持标志
* 输入  : pItdmaParaStruct - 指向ItdmaParaStruct结构体的指针
          slotKeep - 保持标志
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void ITDMA_SetSlotKeep(ItdmaParaStruct *pItdmaParaStruct, U8 slotKeep)
{     
    if (slotKeep == FALSE)
    {
        pItdmaParaStruct->SlotKeep = FALSE;    
    }
    else
    {
        pItdmaParaStruct->SlotKeep = TRUE;
    }
}

/*******************************************************************************
* 名称  : ITDMA_CalcCommStateSlotIncrement
* 描述  : 根据时隙偏移量计算ITDMA通信状态中的时隙增量字段值
* 输入  : slotOffset - 时隙偏移量
* 输出  : 无
* 返回  : ITDMA通信状态中的时隙增量字段值
*******************************************************************************/
S16 ITDMA_CalcCommStateSlotIncrement(S16 slotOffset)
{
    S16 slotIncrement;

    if (slotOffset > ITDMA_COMMSTATE_SLOT_INCREMENT_MAX)
    {
        slotIncrement = slotOffset - ITDMA_COMMSTATE_SLOT_INCREMENT_MAX - 1; 

        if (slotOffset > ITDMA_COMMSTATE_SLOT_INCREMENT_MAX)
        {
            slotOffset = ITDMA_COMMSTATE_SLOT_INCREMENT_MAX;
        }
    }
    else
    {
        slotIncrement = slotOffset;
    }

    return slotIncrement;
}

/*******************************************************************************
* 名称  : ITDMA_CalcSlotOffset
* 描述  : 根据ITDMA通信状态中的时隙增量和时隙数量字段值计算时隙偏移量
* 输入  : slotIncrement - ITDMA通信状态中的时隙增量字段值
          numberOfSlots - ITDMA通信状态中的时隙数量字段值
* 输出  : 无
* 返回  : 时隙偏移量
*******************************************************************************/
S16 ITDMA_CalcSlotOffset(U16 slotIncrement, U8 numberOfSlots)
{
    S16 slotOffset;
    S16 tempS16;

    /*lq 有效性判断 */
    if (slotIncrement > ITDMA_COMMSTATE_SLOT_INCREMENT_MAX)
    {
        tempS16 = ITDMA_COMMSTATE_SLOT_INCREMENT_MAX;
    }
    else
    {
        tempS16 = slotIncrement;    
    }

    if (numberOfSlots < ITDMA_COMMSTATE_NUM_OF_SLOTS_SPLIT)
    {
        slotOffset = tempS16; 
    }
    else
    {
        slotOffset = tempS16 + ITDMA_COMMSTATE_SLOT_INCREMENT_MAX + 1;
    }

    return slotOffset;
}

/*******************************************************************************
* 名称  : ITDMA_CalcCommStateNumberOfSlots
* 描述  : 根据时隙数量和时隙偏移量计算ITDMA通信状态中的时隙数量字段值
* 输入  : numOfSlots - 时隙数量
          slotOffset - 时隙偏移量
* 输出  : 无
* 返回  : ITDMA通信状态中的时隙数量字段值
*******************************************************************************/
U8 ITDMA_CalcCommStateNumberOfSlots(U8 numOfSlots, S16 slotOffset)
{
    U8 numberOfSlots;

    if (slotOffset > ITDMA_COMMSTATE_SLOT_INCREMENT_MAX)
    {
        if (numOfSlots > ITDMA_CONSECUTIVE_SLOTS_QUANTITY_2_MAX)
        {
            numberOfSlots = ITDMA_CONSECUTIVE_SLOTS_QUANTITY_2_MAX + 4;
        }
        else
        {
            numberOfSlots = numOfSlots + 4;               
        }
    }
    else
    {
        if (numOfSlots > ITDMA_CONSECUTIVE_SLOTS_QUANTITY_1_MAX)
        {
            numberOfSlots = ITDMA_CONSECUTIVE_SLOTS_QUANTITY_1_MAX - 1;   
        }
        else
        {
            numberOfSlots = numOfSlots - 1;               
        }
    }

    return numberOfSlots;
}

/*******************************************************************************
* 名称  : ITDMA_CalcSlotQuantity
* 描述  : 根据ITDMA通信状态中的时隙数量字段值计算连续预占的时隙数量
* 输入  : numberOfSlots - ITDMA通信状态中的时隙数量字段值
* 输出  : 无
* 返回  : 连续预占的时隙数量
*******************************************************************************/
U8 ITDMA_CalcSlotQuantity(U8 numberOfSlots)
{
    S16 slotQuantity;
    S16 tempS16;

    /*lq 有效性判断 */
    if (numberOfSlots > ITDMA_COMMSTATE_NUM_OF_SLOTS_MAX)
    {
        tempS16 = ITDMA_COMMSTATE_NUM_OF_SLOTS_MAX;
    }
    else
    {
        tempS16 = numberOfSlots;    
    }

    if (numberOfSlots < ITDMA_COMMSTATE_NUM_OF_SLOTS_SPLIT)
    {
        slotQuantity = tempS16 + 1; 
    }
    else
    {
        slotQuantity = tempS16 - 4;
    }

    return slotQuantity;
}

/*******************************************************************************
* 名称  : ITDMA_CalcCommStateKeepFlag
* 描述  : 根据时隙保持标志计算ITDMA通信状态中的时隙保持字段值
* 输入  : slotKeep - 时隙保持标志
* 输出  : 无
* 返回  : ITDMA通信状态中的时隙保持字段值
*******************************************************************************/
U8 ITDMA_CalcCommStateKeepFlag(U8 slotKeep)
{
    if (slotKeep == FALSE)
    {
        return 0;    
    }
    else
    {
        return 1;
    }
}

/*******************************************************************************
* 名称  : ITDMA_CalcSlotKeep
* 描述  : 根据ITDMA通信状态中的时隙保持字段值计算时隙保持标志
* 输入  : keepFlag - ITDMA通信状态中的时隙保持字段值
* 输出  : 无
* 返回  : 时隙保持标志
*******************************************************************************/
U8 ITDMA_CalcSlotKeep(U8 keepFlag)
{
    if (keepFlag == 0)
    {
        return FALSE;    
    }
    else
    {
        return TRUE;
    }
}

/*******************************************************************************
* 名称  : ITDMA_CalcSelectIntervalOffset
* 描述  : 根据ITDMA的标称增量计算时隙选择区间偏移量
* 输入  : nominalIncrement - ITDMA标称增量
* 输出  : 无
* 返回  : 时隙选择区间偏移量
*******************************************************************************/
S16 ITDMA_CalcSelectIntervalOffset(S16 nominalIncrement)
{
    return nominalIncrement * ITDMA_SI_OFFSET_SCALE;
}

/*******************************************************************************
* 名称  : ITDMA_SetNss
* 描述  : ITDMA设置标称起始时隙
* 输入  : pItdmaParaStruct - 指向ItdmaParaStruct结构体的指针
          frame - 帧号
          slot - 时隙号
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void ITDMA_SetNss(ItdmaParaStruct *pItdmaParaStruct, S32 frame, S16 slot)
{      
    pItdmaParaStruct->NominalStartFrame = frame;
    pItdmaParaStruct->NominalStartSlot = slot;
}

#if   0  // lnw mask  no use
/*******************************************************************************
* 名称  : ITDMA_SetNss
* 描述  : ITDMA设置标称时隙
* 输入  : pItdmaParaStruct - 指向ItdmaParaStruct结构体的指针
          frame - 帧号
          slot - 时隙号
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void ITDMA_SetNs(ItdmaParaStruct *pItdmaParaStruct, S32 frame, S16 slot)
{      
    pItdmaParaStruct->NominalFrame = frame;
    pItdmaParaStruct->NominalSlot = slot;
}
#endif
/*******************************************************************************
* 名称  : ITDMA_GetFirstNs
* 描述  : ITDMA获取首个标称时隙
* 输入  : pItdmaParaStruct - 指向ItdmaParaStruct结构体的指针
          frame - 帧号
          slot - 时隙号
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void ITDMA_GetFirstNs(ItdmaParaStruct *pItdmaParaStruct)
{      
    pItdmaParaStruct->NominalFrame = pItdmaParaStruct->NominalStartFrame;
    pItdmaParaStruct->NominalSlot = pItdmaParaStruct->NominalStartSlot;
}

/*******************************************************************************
* 名称  : ITDMA_SetNextNs
* 描述  : ITDMA设置下一标称时隙
* 输入  : pItdmaParaStruct - 指向ItdmaParaStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void ITDMA_SetNextNs(ItdmaParaStruct *pItdmaParaStruct)
{      
    S32 tempS32;

    tempS32 = pItdmaParaStruct->NominalSlot + pItdmaParaStruct->NominalIncrement;
    pItdmaParaStruct->NominalFrameNext = pItdmaParaStruct->NominalFrame
        + tempS32 / FRAME_SLOT_SIZE;
    pItdmaParaStruct->NominalSlotNext = tempS32 % FRAME_SLOT_SIZE;
}

/*******************************************************************************
* 名称  : ITDMA_GetNextNs
* 描述  : ITDMA获取下一标称时隙
* 输入  : pItdmaParaStruct - 指向ItdmaParaStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void ITDMA_GetNextNs(ItdmaParaStruct *pItdmaParaStruct)
{      
    pItdmaParaStruct->NominalFrame = pItdmaParaStruct->NominalFrameNext;
    pItdmaParaStruct->NominalSlot = pItdmaParaStruct->NominalSlotNext;
}

/*******************************************************************************
* 名称  : ITDMA_SetNextNs
* 描述  : ITDMA设置下一标称时隙
* 输入  : pItdmaParaStruct - 指向ItdmaParaStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void ITDMA_SetNextNts(ItdmaParaStruct *pItdmaParaStruct, S32 frame, S16 slot)
{      
    pItdmaParaStruct->NominalTxFrameNext = frame;
    pItdmaParaStruct->NominalTxSlotNext = slot;
}

/*******************************************************************************
* 名称  : ITDMA_GetNextNs
* 描述  : ITDMA获取下一标称时隙
* 输入  : pItdmaParaStruct - 指向ItdmaParaStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void ITDMA_GetNextNts(ItdmaParaStruct *pItdmaParaStruct)
{      
    pItdmaParaStruct->NominalTxFrame = pItdmaParaStruct->NominalTxFrameNext;
    pItdmaParaStruct->NominalTxSlot = pItdmaParaStruct->NominalTxSlotNext;
}

/*******************************************************************************
* 名称  : ITDMA_Schedule
* 描述  : 单信道ITDMA调度。该调度方案适用于连续ITDMA接入方式，插入ITDMA接入方式
          不适用。
* 输入  : pItdmaSchedStruct - 指向ItdmaSchedStruct结构体的指针
          currFrame - 当前帧号
          currSlot - 当前时隙号
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void ITDMA_Schedule(ItdmaSchedStruct *pItdmaSchedStruct, S32 currFrame, S16 currSlot)
{
    AisChannelEnum channel;
    ItdmaCtrlStruct *pItdmaCtrlStruct;
    ItdmaParaStruct *pItdmaPara;
//    ItdmaParaStruct *pItdmaParaNew;
    AISMsgCommStateUnion commState;
    U16 tempU16;
//    U16 tempU16_1;
    S32 tempS32;

    if (pItdmaSchedStruct == &(TdmaSched.ItdmaSched.ItdmaSchedA))
    {
        channel = ENUM_AIS_CHANNEL_A;
    }
    else if (pItdmaSchedStruct == &(TdmaSched.ItdmaSched.ItdmaSchedB))
    {
        channel = ENUM_AIS_CHANNEL_B;
    }
    else
    {
        return ;
    }
    pItdmaCtrlStruct = &(pItdmaSchedStruct->ItdmaCtrl);
    pItdmaPara = &(pItdmaSchedStruct->ItdmaPara);
  //  pItdmaParaNew = &(pItdmaSchedStruct->ItdmaParaNew);

    switch (pItdmaCtrlStruct->CtrlState)
    {
        case ITDMA_STA_IDLE :

            if (pItdmaCtrlStruct->ItdmaEn == TRUE)
            {
                pItdmaCtrlStruct->ItdmaEn = FALSE;

                /*lq 转入开始状态 */
                pItdmaCtrlStruct->CtrlState = ITDMA_STA_CONTINUOUS_PHASE;
            
		        debug_printf("$[%6d:%4d] %s %s : State to ITDMA_STA_CONTINUOUS_PHASE!\r\n",
                    currFrame, currSlot, __func__,
                    channel == ENUM_AIS_CHANNEL_A ? AIS_CHANNEL_NAME_A : AIS_CHANNEL_NAME_B);                        
            }

            break;

        case ITDMA_STA_CONTINUOUS_PHASE :

            /*lq 等待当前信道的NTS */
            if (ITDMA_IsSendTimArrived(pItdmaPara, currFrame, currSlot) == TRUE)            
            {                
                ITDMA_SetNextNs(pItdmaPara);
                ITDMA_GetNextNs(pItdmaPara);

                /*lq 为当前的NS选择新的NTS */    
                tempU16 = SelectCandidateSlot(channel, pItdmaPara->NominalSlot,
                    pItdmaPara->SelectIntervalOffset, pItdmaPara->NominalFrame,
                    SLOT_CAND_SELECT_BI);

                if (IsSlotValid(tempU16) == FALSE)
                {
                    ITDMA_SetNextNts(pItdmaPara, pItdmaPara->NominalFrame,
                        pItdmaPara->NominalSlot);
                }
                else
                {
                    tempS32 = SelectCandidateSlotFrame(tempU16,
                        pItdmaPara->NominalSlot, pItdmaPara->NominalFrame);

                    ITDMA_SetNextNts(pItdmaPara, tempS32, tempU16);
                }

                /*lq 计算时隙偏移量 */
                tempS32 = CalcSlotOffset(pItdmaPara->NominalTxFrame,
                    pItdmaPara->NominalTxSlot, pItdmaPara->NominalTxFrameNext,
                    pItdmaPara->NominalTxSlotNext);

                ITDMA_SetSlotOffset(pItdmaPara, tempS32);
                ITDMA_SetSlotQuantity(pItdmaPara, 1);
                ITDMA_SetSlotKeep(pItdmaPara, TRUE);
                
                /*lq 初始化18号消息 */
                /*lq 使用ITDMA通信状态，SendMsg18()调用需在ITDMA_GetNextNts()调用之前*/
                commState.ITDMACommState.SyncState = TDMA_GetSyncState();
                commState.ITDMACommState.SlotIncrement =
                    ITDMA_CalcCommStateSlotIncrement(pItdmaPara->SlotOffset);
                commState.ITDMACommState.NumberOfSlots =
                    ITDMA_CalcCommStateNumberOfSlots(pItdmaPara->SlotQuantity,
                        pItdmaPara->SlotOffset);
                commState.ITDMACommState.KeepFlag =
                    ITDMA_CalcCommStateKeepFlag(pItdmaPara->SlotKeep);

                TDMA_SendMsg(channel, pItdmaPara->NominalTxSlot,
                    AIS_MSG_COMMSTATE_ITDMA, commState);    //lq 调用参数需提供channel, txSlot, commStateType, commState

                ITDMA_GetNextNts(pItdmaPara);
            }

            break;

        default :
            
            ITDMA_ItdmaSchedStructInit(pItdmaSchedStruct);
            ITDMA_ItdmaSchedEnable(FALSE, pItdmaCtrlStruct);
            break;
    }
}


/*******************************************************************************
* 名称  : ITDMA_ScheduleDual
* 描述  : 双信道ITDMA调度
* 输入  : pItdmaSchedDualStruct - 指向ItdmaSchedDualStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void ITDMA_ScheduleDual(ItdmaSchedDualStruct *pItdmaSchedDualStruct)
{
    ItdmaSchedStruct *pItdmaSchedA;
    ItdmaSchedStruct *pItdmaSchedB;
//    ItdmaParaStruct *pItdmaParaA;
//    ItdmaParaStruct *pItdmaParaB;
    ItdmaCtrlStruct *pItdmaCtrlA;
    ItdmaCtrlStruct *pItdmaCtrlB;
    ItdmaCtrlStruct *pItdmaCtrlStruct;
    S32 currFrame;
    S16 currSlot;
//    U16 tempU16;
//    U16 tempU16_1;
//    S32 tempS32;

    pItdmaCtrlStruct = &(pItdmaSchedDualStruct->ItdmaCtrl);
    pItdmaSchedA = &(pItdmaSchedDualStruct->ItdmaSchedA);
    pItdmaSchedB = &(pItdmaSchedDualStruct->ItdmaSchedB);
   // pItdmaParaA = &(pItdmaSchedDualStruct->ItdmaSchedA.ItdmaPara);
  //  pItdmaParaB = &(pItdmaSchedDualStruct->ItdmaSchedB.ItdmaPara);
    pItdmaCtrlA = &(pItdmaSchedDualStruct->ItdmaSchedA.ItdmaCtrl);
    pItdmaCtrlB = &(pItdmaSchedDualStruct->ItdmaSchedB.ItdmaCtrl);

    currFrame = GetFrameNum();
    currSlot = GetSlotNum();

    switch (pItdmaCtrlStruct->CtrlState)
    {
        case ITDMA_STA_IDLE :

            if (pItdmaCtrlStruct->ItdmaEn == TRUE)
            {
                pItdmaCtrlStruct->ItdmaEn = FALSE;

                ITDMA_ItdmaSchedEnable(TRUE, pItdmaCtrlA);
                ITDMA_ItdmaSchedEnable(TRUE, pItdmaCtrlB);

                /*lq 转入开始状态 */
                pItdmaCtrlStruct->CtrlState = ITDMA_STA_CONTINUOUS_PHASE;
            
		        debug_printf("$[%6d:%4d] %s : State to ITDMA_STA_CONTINUOUS_PHASE!\r\n",
                    currFrame, currSlot, __func__);
            }

            break;

        case ITDMA_STA_CONTINUOUS_PHASE :

            /*lq 信道A/B调度 */
            ITDMA_Schedule(pItdmaSchedA, currFrame, currSlot);
            ITDMA_Schedule(pItdmaSchedB, currFrame, currSlot);

            break;

        default :
            
            ITDMA_ItdmaSchedDualStructInit(pItdmaSchedDualStruct);
            ITDMA_ItdmaSchedEnable(FALSE, pItdmaCtrlStruct);
            
            break;
    }

}

/*******************************************************************************
* 名称  : ITDMA_SetSlotState
* 描述  : 根据通信状态、当前帧号及时隙号设置时隙映射表中相应时隙的状态
* 输入  : channel - 时隙所在的信道
          commState - SOTDMA通信状态
          currFrame - 当前帧号
          currSlot - 当前时隙号
          slotState - 时隙状态
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ITDMA_SetSlotState(AisChannelEnum channel, ITDMACommStateStruct commState,
                        S32 currFrame, S16 currSlot, U8 slotState)
{
    S32 tempS32;
    S16 tempS16;
    S16 tempS16_2;
    S16 tempS16_3;
    S16 tempS16_4;
    S16 i;
    U8  keepFlg;
    
    keepFlg = ITDMA_CalcSlotKeep(commState.KeepFlag);
    tempS16 = ITDMA_CalcSlotQuantity(commState.NumberOfSlots);
    tempS16_4 = ITDMA_CalcSlotOffset(commState.SlotIncrement,
            commState.NumberOfSlots);

    if (tempS16_4 != 0)
    {
        for (i = 0; i < tempS16; i++)
        {
            tempS16_2 = tempS16_4 + i;
    
            tempS32 = CalcFrameNum(currFrame, currSlot, tempS16_2);            
            tempS16_3 = CalcSlotNum(currFrame, currSlot, tempS16_2);
    
            SetSlotState(channel, tempS16_3, slotState, tempS32);
            
            if (keepFlg == TRUE)
            {
                SetSlotState(channel, tempS16_3, slotState, tempS32 + 1);
            }        
        }
    }
}

/*******************************************************************************
* 名称  : TDMA_TdmaCtrlStructInit
* 描述  : TdmaCtrlStruct结构体初始化
* 输入  : pTdmaCtrlStruct - 指向TdmaCtrlStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void TDMA_TdmaCtrlStructInit(TdmaCtrlStruct *pTdmaCtrlStruct)
{
    pTdmaCtrlStruct->TimePeriod = TDMA_CTRL_TIM_PERIOD;
    pTdmaCtrlStruct->TimeCnt = SetTimePeriodProcess(pTdmaCtrlStruct->TimePeriod);
    pTdmaCtrlStruct->Timeout = TDMA_CTRL_TIMEOUT_REPORT_INTERVAL_SAMPLE;
    pTdmaCtrlStruct->TimeoutCnt = 0;

    pTdmaCtrlStruct->CtrlState = TDMA_STA_IDLE;
    pTdmaCtrlStruct->TdmaEn = FALSE;
    pTdmaCtrlStruct->SchedSchema = ENUM_TDMA_SCHED_SCHEMA_SOTDMA;
    pTdmaCtrlStruct->ReportIntervalSlot = TDMA_REPORT_INTERVAL_SLOT_30S;    //lq 该初始值需与SotdmaParaStruct.ReportRate的初始值相匹配
    pTdmaCtrlStruct->ReportIntervalSlotSample = TDMA_REPORT_INTERVAL_SLOT_30S;
    pTdmaCtrlStruct->ReportIntervalChangedCnt = 0;

}

/*******************************************************************************
* 名称  : TDMA_TdmaSchedStructInit
* 描述  : TdmaSchedStruct结构体初始化
* 输入  : pTdmaSchedStruct - 指向TdmaSchedStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void TDMA_TdmaSchedStructInit(TdmaSchedStruct *pTdmaSchedStruct)
{
    SOTDMA_SotdmaSchedDualStructInit(&(pTdmaSchedStruct->SotdmaSched));
    ITDMA_ItdmaSchedDualStructInit(&(pTdmaSchedStruct->ItdmaSched));
    TDMA_TdmaCtrlStructInit(&(pTdmaSchedStruct->TdmaCtrl));
}

/*******************************************************************************
* 名称  : TDMA_TdmaSchedEnable
* 描述  : TDMA调度使能配置
* 输入  : flag - 使能标志
          pTdmaCtrlStruct - 指向TdmaCtrlStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void TDMA_TdmaSchedEnable(U8 flag, TdmaCtrlStruct *pTdmaCtrlStruct)
{
    pTdmaCtrlStruct->CtrlState = TDMA_STA_IDLE;
    pTdmaCtrlStruct->TdmaEn = flag;
}

/*******************************************************************************
* 名称  : TDMA_InstReportIntervalSample
* 描述  : TDMA实例报告间隔采样
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TDMA_InstReportIntervalSample(S16 reportInterval)
{
    TdmaSched.TdmaCtrl.ReportIntervalSlotSample = reportInterval;
}

/*******************************************************************************
* 名称  : TDMA_Schedule
* 描述  : TDMA调度，当前方案是当发现报告间隔已改变时会立即发送一条消息，而不是等
          到使用旧报速率预占的下一次发射
* 输入  : pTdmaSchedStruct - 指向TdmaSchedStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void TDMA_Schedule(TdmaSchedStruct *pTdmaSchedStruct)
{
    SotdmaSchedDualStruct *pSotdmaSched;
    ItdmaSchedDualStruct *pItdmaSched;
    TdmaCtrlStruct *pTdmaCtrl;
    S32 currFrame;
    S16 currSlot;

    pSotdmaSched = &(pTdmaSchedStruct->SotdmaSched);
    pItdmaSched = &(pTdmaSchedStruct->ItdmaSched);
    pTdmaCtrl = &(pTdmaSchedStruct->TdmaCtrl);

    /*lq 获取本地帧号和时隙号 */
    currFrame = GetFrameNum();
    currSlot = GetSlotNum();

    switch (pTdmaCtrl->CtrlState)
    {
        case TDMA_STA_IDLE :
            
            if (GetGpsPowerupFixState() == TRUE)
            {
                if (pTdmaCtrl->TdmaEn == TRUE)
                {
                    pTdmaCtrl->TdmaEn = FALSE;
                    
                    /*lq 首先使用SOTDMA调度方案 */
                    SOTDMA_SotdmaSchedDualStructInit(pSotdmaSched);
                    SOTDMA_SotdmaSchedEnable(TRUE, &(pSotdmaSched->SotdmaCtrl));
                    SOTDMA_SotdmaSchedEnable(FALSE, &(pSotdmaSched->SotdmaSchedA.SotdmaCtrl));
                    SOTDMA_SotdmaSchedEnable(FALSE, &(pSotdmaSched->SotdmaSchedB.SotdmaCtrl));
    
                    ITDMA_ItdmaSchedDualStructInit(pItdmaSched);
                    ITDMA_ItdmaSchedEnable(FALSE, &(pItdmaSched->ItdmaCtrl));
                    ITDMA_ItdmaSchedEnable(FALSE, &(pItdmaSched->ItdmaSchedA.ItdmaCtrl));
                    ITDMA_ItdmaSchedEnable(FALSE, &(pItdmaSched->ItdmaSchedB.ItdmaCtrl));
    
                    pTdmaCtrl->CtrlState = TDMA_STA_CONTINUOUS_PHASE;
                    
                    debug_printf("$[%6d:%4d] %s : State to CONTINUOUS_PHASE!\r\n",
                        currFrame, currSlot, __func__);                                       
                }
            }

            break;

        case TDMA_STA_CONTINUOUS_PHASE :
            
            /*lq 报告间隔采样定时 */
            if (pTdmaCtrl->TimeCnt == GetLocalClockTickProcess())
            {
                pTdmaCtrl->TimeCnt = SetTimePeriodProcess(pTdmaCtrl->TimePeriod);
                
                pTdmaCtrl->TimeoutCnt++;
                if (pTdmaCtrl->TimeoutCnt >= pTdmaCtrl->Timeout)
                {
                    /*lq 报告间隔采样定时到 */
                    pTdmaCtrl->TimeoutCnt = 0;
                
                    /*lq 进行报告间隔更新判断 */
                    if (pTdmaCtrl->ReportIntervalSlotSample
                        != pTdmaCtrl->ReportIntervalSlot)
                    {
                        pTdmaCtrl->ReportIntervalChangedCnt++;
                        if (pTdmaCtrl->ReportIntervalChangedCnt
                            >= TDMA_REPORT_INTERVAL_CHANGED_CNT_THRES)
                        {
                            /*lq 改变持续时间达到要求，确认改变报告间隔 */
                            pTdmaCtrl->ReportIntervalChangedCnt = 0;
        
                            if (pTdmaCtrl->ReportIntervalSlotSample
                                > TDMA_REPORT_INTERVAL_SLOT_30S)
                            {
                                /*lq
                                 *  单（双）信道报告间隔超过1（0.5）min ，
                                 *  使用ITDMA调度方式
                                 */    
                                ITDMA_ItdmaSchedDualStructInit(pItdmaSched);
                                
                                /*lq A信道参数配置 */
                                ITDMA_SetNi(&(pItdmaSched->ItdmaSchedA.ItdmaPara),
                                    pTdmaCtrl->ReportIntervalSlotSample * 2);
                                
                                ITDMA_SetSelectIntervalOffset(
                                    &(pItdmaSched->ItdmaSchedA.ItdmaPara));
                                
                                ITDMA_SetNss(&(pItdmaSched->ItdmaSchedA.ItdmaPara),
                                    CalcFrameNum(currFrame, currSlot,
                                        AIS_SLOT_INTERVAL_SEND_TO_TX),
                                    CalcSlotNum(currFrame, currSlot,
                                        AIS_SLOT_INTERVAL_SEND_TO_TX));
                                
                                ITDMA_GetFirstNs(&(pItdmaSched->ItdmaSchedA.ItdmaPara));
                                
                                ITDMA_SetNextNts(&(pItdmaSched->ItdmaSchedA.ItdmaPara),
                                    pItdmaSched->ItdmaSchedA.ItdmaPara.NominalFrame,
                                    pItdmaSched->ItdmaSchedA.ItdmaPara.NominalSlot);
                                
                                ITDMA_GetNextNts(&(pItdmaSched->ItdmaSchedA.ItdmaPara));
        
                                /*lq B信道参数配置 */
                                ITDMA_SetNi(&(pItdmaSched->ItdmaSchedB.ItdmaPara),
                                    pTdmaCtrl->ReportIntervalSlotSample * 2);
                                
                                ITDMA_SetSelectIntervalOffset(
                                    &(pItdmaSched->ItdmaSchedB.ItdmaPara));
                                
                                ITDMA_SetNss(&(pItdmaSched->ItdmaSchedB.ItdmaPara),
                                    CalcFrameNum(currFrame, currSlot,
                                        pTdmaCtrl->ReportIntervalSlotSample
                                        + AIS_SLOT_INTERVAL_SEND_TO_TX),
                                    CalcSlotNum(currFrame, currSlot,
                                        pTdmaCtrl->ReportIntervalSlotSample
                                        + AIS_SLOT_INTERVAL_SEND_TO_TX));
                                
                                ITDMA_GetFirstNs(&(pItdmaSched->ItdmaSchedB.ItdmaPara));
                                
                                ITDMA_SetNextNts(&(pItdmaSched->ItdmaSchedB.ItdmaPara),
                                    pItdmaSched->ItdmaSchedB.ItdmaPara.NominalFrame,
                                    pItdmaSched->ItdmaSchedB.ItdmaPara.NominalSlot);
                                
                                ITDMA_GetNextNts(&(pItdmaSched->ItdmaSchedB.ItdmaPara));
        
                                /*lq 使能ITDMA调度 */
                                pItdmaSched->ItdmaCtrl.CtrlState = ITDMA_STA_IDLE;
                                ITDMA_ItdmaSchedEnable(TRUE, &(pItdmaSched->ItdmaCtrl));
        
                                /*lq 关闭SOTDMA调度 */
                                SOTDMA_SotdmaSchedEnable(FALSE, &(pSotdmaSched->SotdmaCtrl));
                            
                                /*lq 改为ITDMA调度方式 */
                                pTdmaCtrl->SchedSchema = ENUM_TDMA_SCHED_SCHEMA_ITDMA;
                            
                                debug_printf("$[%6d:%4d] %s : TDMA has changed to ITDMA, Report interval is [%d] slots now!\r\n",
                                    currFrame, currSlot, __func__, pTdmaCtrl->ReportIntervalSlotSample);                                                                   
                            }
                            else
                            {
                                /*lq
                                 *  单（双）信道报告间隔未超过1（0.5）min，
                                 *  使用SOTDMA调度方式
                                 */    
                                if (pTdmaCtrl->SchedSchema == ENUM_TDMA_SCHED_SCHEMA_SOTDMA)
                                {
                                    /*lq 之前使用的接入方式为SOTDMA */
                                    SOTDMA_SetRr(&(pSotdmaSched->SotdmaSchedA.SotdmaParaNew),
                                        FRAME_SLOT_SIZE / (pTdmaCtrl->ReportIntervalSlotSample * 2));
                                    SOTDMA_SetRr(&(pSotdmaSched->SotdmaSchedB.SotdmaParaNew),
                                        FRAME_SLOT_SIZE / (pTdmaCtrl->ReportIntervalSlotSample * 2));

                                    debug_printf("$[%6d:%4d] %s : TDMA remains SOTDMA, Report interval is [%d] slots now!\r\n",
                                        currFrame, currSlot, __func__, pTdmaCtrl->ReportIntervalSlotSample);                                                                   
                                }
                                else
                                {
                                    /*lq 之前使用的接入方式为ITDMA */
                                    SOTDMA_SotdmaSchedDualStructInit(pSotdmaSched);
                                    
                                    SOTDMA_SetRrDual(FRAME_SLOT_SIZE
                                        / (pTdmaCtrl->ReportIntervalSlotSample * 2));
                                    
                                    SOTDMA_SetNssDual(ENUM_AIS_CHANNEL_A,
                                        CalcSlotNum(currFrame, currSlot,
                                            AIS_SLOT_INTERVAL_SEND_TO_TX));
        
                                    /*lq 新旧参数的航速保持一致 */
                                    SOTDMA_SotdmaParaStructCopy(
                                        &(pSotdmaSched->SotdmaSchedA.SotdmaParaNew),
                                        &(pSotdmaSched->SotdmaSchedA.SotdmaPara));
        
                                    SOTDMA_SotdmaParaStructCopy(
                                        &(pSotdmaSched->SotdmaSchedB.SotdmaParaNew),
                                        &(pSotdmaSched->SotdmaSchedB.SotdmaPara));
        
                                    /*lq 进入SOTDMA调度 */
                                    pSotdmaSched->SotdmaCtrl.CtrlState = SOTDMA_STA_NETWORK_ENTRY_PHASE;    //lq 直接进入网络进入阶段，绕过初始化阶段（初始化阶段仅在设备开机后执行一次）
            
                                    /*lq 关闭ITDMA调度 */
                                    ITDMA_ItdmaSchedEnable(FALSE, &(pItdmaSched->ItdmaCtrl));
        
                                    /*lq 改为SOTDMA调度方式 */
                                    pTdmaCtrl->SchedSchema = ENUM_TDMA_SCHED_SCHEMA_SOTDMA;

                                    debug_printf("$[%6d:%4d] %s : TDMA has changed to SOTDMA, Report interval is [%d] slots now!\r\n",
                                        currFrame, currSlot, __func__, pTdmaCtrl->ReportIntervalSlotSample);                                                                   
                                }
                            }
        
                            /*lq 更新报告间隔 */
                            pTdmaCtrl->ReportIntervalSlot = 
                                pTdmaCtrl->ReportIntervalSlotSample;
                        }
                    }
                    else
                    {
                        pTdmaCtrl->ReportIntervalChangedCnt = 0;
                    }
                }
            }
        
            /*lq 双信道SOTDMA调度 */
            if (pTdmaCtrl->SchedSchema == ENUM_TDMA_SCHED_SCHEMA_SOTDMA)
            {
                SOTDMA_ScheduleDual(pSotdmaSched);
            }
            else
            {
                ITDMA_ScheduleDual(pItdmaSched);
            }

            break;
        
        default :

            TDMA_TdmaSchedStructInit(pTdmaSchedStruct);
            TDMA_TdmaSchedEnable(TRUE, pTdmaCtrl);
            
            break;
    }

}

/*******************************************************************************
* 名称  : TDMA_Schedule
* 描述  : TDMA调度，当前方案是当发现报告间隔已改变时会立即发送一条消息，而不是等
          到使用旧报速率预占的下一次发射
* 输入  : pTdmaSchedStruct - 指向TdmaSchedStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TDMA_InstSchedule(void)
{
    TDMA_Schedule(&TdmaSched);
}

/*******************************************************************************
* 名称  : TDMA_SetSlotState
* 描述  : 根据通信状态、当前帧号及时隙号设置时隙映射表中相应时隙的状态
* 输入  : channel - 时隙所在的信道
          commStateType - 通信状态类型
          commState - SOTDMA通信状态
          currFrame - 当前帧号
          currSlot - 当前时隙号
          slotState - 时隙状态
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TDMA_SetSlotState(AisChannelEnum channel, AISMsgCommStateEnum commStateType,AISMsgCommStateUnion commState, S32 currFrame,S16 currSlot, U8 slotState)
{
    /*lq 开机同步过之后才可设置时隙状态 */
    if (GetGpsPowerupFixState() == TRUE)
    {
        if (commStateType == AIS_MSG_COMMSTATE_ITDMA)
        {
            ITDMA_SetSlotState(channel, commState.ITDMACommState, currFrame,
                currSlot, slotState);
        }
        else
        {
            SOTDMA_SetSlotState(channel, commState.SOTDMACommState, currFrame,
                currSlot, slotState);    
        }
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
static U8 TDMA_SendMsg(AisChannelEnum channel, S16 txSlot,
             AISMsgCommStateEnum commStateType, AISMsgCommStateUnion commState)
{
   U8   RTN_Value=0; // lnw add
   	
    if (DevStaticPara.AisClass == AIS_CLASS_B)    
    {
        /*lq Class B */
        RTN_Value=SendMsg18(channel, txSlot, commStateType, commState);   //lnw modify     
    }
    else
    {
        /*lq Class A */
        if (commStateType == AIS_MSG_COMMSTATE_SOTDMA)
        {
            /*lq 使用SOTDMA通信状态，发送Msg1 */
            RTN_Value=SendMsg1(channel, txSlot, commState); // lnw modify      
        }
        else
        {
            /*lq 使用ITDMA通信状态，发送Msg3 */
            RTN_Value=SendMsg3(channel, txSlot, commState);  //lnw modify      
        }
    }

	return  RTN_Value;  // lnw  add
}

/*******************************************************************************
* 名称  : TDMA_ModuleInit
* 描述  : 模块初始化
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TDMA_ModuleInit(void) 
{
    TDMA_TdmaSchedStructInit(&TdmaSched);
    TDMA_TdmaSchedEnable(TRUE, &(TdmaSched.TdmaCtrl));
}

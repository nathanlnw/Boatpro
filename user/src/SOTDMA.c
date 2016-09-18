/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: SOTDMA.c
  Author: lq       Version : V1.00        Date: 2013.04.24
  Description:     准SOTDMA消息发送控制模块变量、函数定义，
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include "def.h"
#include "SOTDMA.h"
#include "AIS_MsgModu.h"
#include "Tim.h"
#include "AISConfig.h"
#include "initial.h"
#include "gpsctrl.h"
#include "gmskmod.h"

SOTDMAParaStruct struSOTDMAPara;
ICommStateStruct struICommState;

/*******************************************************************************
* Function Name  : InitSOTDMAPara
* Description    : 初始化SOTDMA参数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitSOTDMAPara(void)
{
    struSOTDMAPara.BurstTxTimSlot = 0;
    struSOTDMAPara.BurstTxTimFrame = 0;
    struSOTDMAPara.MsgTxTimSlot = 0;
    struSOTDMAPara.MsgTxTimFrame = 0;
    
    struSOTDMAPara.BurstSendTimSlot = 0;
    struSOTDMAPara.BurstSendTimFrame = 0;
    struSOTDMAPara.MsgSendTimSlot = 0;
    struSOTDMAPara.MsgSendTimFrame = 0;
    
    struSOTDMAPara.Timeout = 0;
    struSOTDMAPara.MsgCount = 0;
    struSOTDMAPara.BurstFlg = TRUE;    
    struSOTDMAPara.TxChannel = 'A'; 
    struSOTDMAPara.BurstFinsFlg = FALSE; 
    struSOTDMAPara.BurstStarFlg = FALSE;
    
    struSOTDMAPara.SlotOffset = ITDMA_COM_STA_SLOT_INCRE_DEF_VAL;                          
}

/*******************************************************************************
* Function Name  : InitCommStateITDMA
* Description    : 初始化通信状态
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitCommStateITDMA(void)
{
    struICommState.ucSyncState = 3;                                            // 同步状态：                                    
    struICommState.usSlotIncrement = ITDMA_COM_STA_SLOT_INCRE_DEF_VAL;         // 时隙增量
    struICommState.ucSlotNumbers = 2;                                          // 时隙数：3个时隙（见1371_4）
    struICommState.ucKeepFlag = 0;                                             // 保持标记：不保持                                                        // 接收的台站数
}

/*******************************************************************************
* Function Name  : GetBurstSendTim
* Description    : 获取组内第一条消息发送定时
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
U32 GetBurstSendTim(void)
{
    return struSOTDMAPara.BurstSendTimFrame * 2250 + struSOTDMAPara.BurstSendTimSlot;                     
}

/*******************************************************************************
* Function Name  : GetMsgSendTim
* Description    : 获取组内消息发送定时
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
U32 GetMsgSendTim(void)
{
    return struSOTDMAPara.MsgSendTimFrame * 2250 + struSOTDMAPara.MsgSendTimSlot;                     
}
#if 0
/*******************************************************************************
* Function Name  : GetBurstSendTim
* Description    : 获取组定时
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
U16 GetRandSlotOffset(void)
{
    /*lq... 返回随机值[2025,2475]*/
#if 1
    return RandData(&send_seed, 2025, 2475);
#else
    return 2250;
#endif
}
#endif

/*******************************************************************************
* Function Name  : GetRandSlotOffset
* Description    : 获取时隙增量偏移
* Input          : PeriodSlots - 发送周期（单位:slot）
* Output         : None
* Return         : 加入随机偏差的时隙增量
*******************************************************************************/
U16 GetRandSlotOffset(S16 PeriodSlots)
{
    S16 randSlotOffset;
#if 0
    /*lq 随机时隙偏差值[-225,225]*/
    randSlotOffset = 225 - RandData(&send_seed, 0, 450);    

    return PeriodSlots + randSlotOffset;                    //lq 时隙增量值为发送周期+随机时隙偏差
#else
    return PeriodSlots;                                     //lq 测试用
#endif
}

/*******************************************************************************
* Function Name  : SetBurstTxTim
* Description    : 设置组发射定时
* Input          : usValue - 时隙增量
* Output         : None
* Return         : None
*******************************************************************************/
#if 0
void SetBurstTxTim(S16 usValue)
{
    if (usValue < 0)
    {
        usValue = 0;
    }

    struSOTDMAPara.BurstTxTimSlot += usValue;
    if (struSOTDMAPara.BurstTxTimSlot >= 2250)
    {
       struSOTDMAPara.BurstTxTimFrame += struSOTDMAPara.BurstTxTimSlot / 2250; 
       struSOTDMAPara.BurstTxTimSlot %= 2250;    
    } 
}
#else
void SetBurstTxTim(S32 usValue)
{
    S32 tempSlot;

    if (usValue < 0)
    {
        usValue = 0;
    }

    tempSlot = struSOTDMAPara.BurstTxTimSlot + usValue;
    if (tempSlot >= 2250)
    {
       struSOTDMAPara.BurstTxTimFrame += tempSlot / 2250; 
       struSOTDMAPara.BurstTxTimSlot = tempSlot % 2250;    
    }
    else
    {
       struSOTDMAPara.BurstTxTimSlot = tempSlot;
    } 
}
#endif
/*******************************************************************************
* Function Name  : SetMsgTxTim
* Description    : 设置消息发射定时
* Input          : usValue - 时隙增量
* Output         : None
* Return         : None
*******************************************************************************/
void SetMsgTxTim(S16 usValue)
{
    if (usValue < 0)
    {
        usValue = 0;
    }

    if (GetBurstFlg() == TRUE)
    {
        struSOTDMAPara.MsgTxTimFrame = struSOTDMAPara.BurstTxTimFrame;
        struSOTDMAPara.MsgTxTimSlot = struSOTDMAPara.BurstTxTimSlot + usValue;
        if (struSOTDMAPara.MsgTxTimSlot >= 2250)
        {
            struSOTDMAPara.MsgTxTimFrame += struSOTDMAPara.MsgTxTimSlot / 2250;
            struSOTDMAPara.MsgTxTimSlot %= 2250;
        }    
    }
    else
    {
        struSOTDMAPara.MsgTxTimSlot += usValue;
        if (struSOTDMAPara.MsgTxTimSlot >= 2250)
        {
           struSOTDMAPara.MsgTxTimFrame += struSOTDMAPara.MsgTxTimSlot / 2250; 
           struSOTDMAPara.MsgTxTimSlot %= 2250;
        }     
    }
}

/*******************************************************************************
* Function Name  : SetBurstSendTim
* Description    : 设置组发送定时
* Input          : usValue - 发送定时和发射定时的时隙差值
* Output         : None
* Return         : None
*******************************************************************************/
void SetBurstSendTim(S16 usValue)
{
    if (usValue < 0)
    {
        usValue = 0;
    }
    else if (usValue > 2250)
    {
        usValue = 2250;
    }

    struSOTDMAPara.BurstSendTimFrame = struSOTDMAPara.BurstTxTimFrame;
    struSOTDMAPara.BurstSendTimSlot = struSOTDMAPara.BurstTxTimSlot - usValue;
    if (struSOTDMAPara.BurstSendTimSlot < 0)
    {
        struSOTDMAPara.BurstSendTimSlot += 2250;
        struSOTDMAPara.BurstSendTimFrame--; 
        if (struSOTDMAPara.BurstSendTimFrame < 0)
        {
            struSOTDMAPara.BurstSendTimSlot = 0;
            struSOTDMAPara.BurstSendTimFrame = 0; 
        }
    } 
}

/*******************************************************************************
* Function Name  : SetMsgSendTim
* Description    : 设置组内消息发送定时
* Input          : usValue - 发送定时和发射定时的时隙差值
* Output         : None
* Return         : None
*******************************************************************************/
void SetMsgSendTim(S16 usValue)
{
    if (usValue < 0)
    {
        usValue = 0;
    }
    else if (usValue > 2250)
    {
        usValue = 2250;
    }

    struSOTDMAPara.MsgSendTimFrame = struSOTDMAPara.MsgTxTimFrame;
    struSOTDMAPara.MsgSendTimSlot = struSOTDMAPara.MsgTxTimSlot - usValue;
    if (struSOTDMAPara.MsgSendTimSlot < 0)
    {
        struSOTDMAPara.MsgSendTimSlot += 2250;
        struSOTDMAPara.MsgSendTimFrame--; 
        if (struSOTDMAPara.MsgSendTimFrame < 0)
        {
            struSOTDMAPara.MsgSendTimSlot = 0;
            struSOTDMAPara.MsgSendTimFrame = 0; 
        }
    } 
}
#if 0
/*******************************************************************************
* Function Name  : SetGpsOnTim
* Description    : 设置GPS开启定时
* Input          : usValue - GPS启动定时和发射定时的时隙差值
* Output         : None
* Return         : None
*******************************************************************************/
void SetGpsOnTim(S16 usValue)
{
    S32 slSlotNums;

    if (usValue < 0)
    {
        usValue = 0;
    }

    slSlotNums = struSOTDMAPara.BurstTxTimFrame * 2250 + struSOTDMAPara.BurstTxTimSlot;
    slSlotNums -= usValue;
    if (slSlotNums < 0)
    {
        slSlotNums = 0;
    }

    SetGpsOnTimFrame(slSlotNums / 2250);
    SetGpsOnTimSlot(slSlotNums % 2250);
}
#endif
#if 0
/*******************************************************************************
* Function Name  : SetCommState_Test
* Description    : 设置SOTDMA的通信状态
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommState_Test(void)
{
    /*lq 设置同步状态*/
    if (GetGpsNormalFixState() == TRUE)
    {
        struSOCommState.ucSyncState = 0;    
    } 
    else
    {
        struSOCommState.ucSyncState = 3;    
    }
    
    /*lq 设置时隙超时值*/
    struSOCommState.ucSlotTimeout = 0;

    /*lq 设置通信状态子消息*/
    struSOCommState.usSubMsg = 0;
}
#endif

#if 0
/*******************************************************************************
* Function Name  : SetCommState
* Description    : 设置SOTDMA的通信状态
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommState(void)
{
    /*lq 设置同步状态*/
    if (GetGpsNormalFixState() == TRUE)
    {
        struSOCommState.ucSyncState = 0;    
    } 
    else
    {
        struSOCommState.ucSyncState = 3;    
    }
    
    /*lq 根据时隙超时值，设置通信状态子消息*/
    SetCommStaSubMsg(struSOCommState.ucSlotTimeout);
}
#endif

/*******************************************************************************
* Function Name  : SetCommStateITDMA_Test
* Description    : 用户测试模式下设置ITDMA的通信状态
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommStateITDMA_Test(U8 synFlg)
{
    /*lq 设置同步状态*/
    if (synFlg == TRUE)
    {
        struICommState.ucSyncState = 0;    
    } 
    else
    {
        struICommState.ucSyncState = 3;    
    }

    /*lq 设置时隙增量和预占时隙数*/
    struICommState.usSlotIncrement = 0;         //lq 不预占时隙
    struICommState.ucSlotNumbers = 1;

    /*lq 设置保持标志*/
    struICommState.ucKeepFlag = 0;              // 保持标记：不保持                                                        // 接收的台站数
}

/*******************************************************************************
* Function Name  : SetCommStateITDMA
* Description    : 设置ITDMA的通信状态
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommStateITDMA(U8 synFlg, U16 slotIncr)
{
    /*lq 设置同步状态*/
    if (synFlg == TRUE)
    {
        struICommState.ucSyncState = 0;    
    } 
    else
    {
        struICommState.ucSyncState = 3;    
    }

    /*lq 设置时隙增量和预占时隙数*/
    if (slotIncr > 8191)                                  //lq 1371-4标准 时隙增量大于8191
    {
        slotIncr -= 8192;
        struICommState.usSlotIncrement = slotIncr < 8192 ? slotIncr : 8191; //lq 最大值8191
        struICommState.ucSlotNumbers = 7;
    }
    else
    {
        struICommState.usSlotIncrement = slotIncr; //lq 最大值8191
        struICommState.ucSlotNumbers = 2;
    }

    /*lq 设置保持标志*/
    struICommState.ucKeepFlag = 0;                                             // 保持标记：不保持                                                        // 接收的台站数
}

/*******************************************************************************
* Function Name  : GetSetCommStateSlotIncr
* Description    : 获取时隙增量实际值
* Input          : None
* Output         : None
* Return         : 时隙增量实际值
*******************************************************************************/
U16 GetSetCommStateSlotIncr(void)
{
    if (struICommState.ucSlotNumbers < 5)   
    {
        /*lq 预占时隙的增量小于8192*/
        return struICommState.usSlotIncrement;
    } 
    else
    {
        /*lq 预占时隙的增量大于等于8192*/
        return struICommState.usSlotIncrement + 8192;        
    }
}
#if 0
/*******************************************************************************
* Function Name  : SetCommStaSubMsg
* Description    : 根据时隙超时值，设置SOTDMA的通信状态子消息
* Input          : ucTimeout - 时隙超时值
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommStaSubMsg(S8 ucTimeout)
{
    if (ucTimeout < 0 || ucTimeout > 7)
    {
        ucTimeout = 0;
    }

    /*lq 根据时隙超时值，设置SOTDMA的通信状态子消息*/
    if (ucTimeout == 0)
    {
        struSOCommState.usSubMsg = struSOCommState.usSlotOffset;
    }
    else if (ucTimeout == 1)
    {
        struSOCommState.usSubMsg = struSOCommState.usUTC_HourMin;
    }
    else if (ucTimeout == 2 || ucTimeout == 4 ||ucTimeout == 6)
    {
        struSOCommState.usSubMsg = struSOCommState.usSlotNum;
    }
    else if (ucTimeout == 3 || ucTimeout == 5 ||ucTimeout == 7)
    {
        struSOCommState.usSubMsg = struSOCommState.usRecStations;
    }
}
#endif
/*******************************************************************************
* Function Name  : GetBurstTxTimSlot
* Description    : 获取组内第一条消息的发射时隙
* Input          : None
* Output         : None
* Return         : 组内第一条消息的发射时隙
*******************************************************************************/
U16 GetBurstTxTimSlot(void)
{
    return struSOTDMAPara.BurstTxTimSlot;
}

/*******************************************************************************
* Function Name  : GetMsgTxTimSlot
* Description    : 获取组内其他消息的发射时隙
* Input          : None
* Output         : None
* Return         : 组内组内其他消息的发射时隙
*******************************************************************************/
U16 GetMsgTxTimSlot(void)
{
    return struSOTDMAPara.MsgTxTimSlot;
}

/*******************************************************************************
* Function Name  : SetBurstFlg
* Description    : 设置组内第一条消息发射标志
* Input          : ucFlag - true:组内第一条消息发射,false:组内其他消息发射
* Output         : None
* Return         : None
*******************************************************************************/
void SetBurstFlg(U8 ucFlag)
{
    if (ucFlag == TRUE)
    {
        struSOTDMAPara.BurstFlg = TRUE;    
    }
    else
    {
        struSOTDMAPara.BurstFlg = FALSE;    
    }
}

/*******************************************************************************
* Function Name  : GetBurstFlg
* Description    : 获取组内第一条消息发射标志
* Input          : None
* Output         : None
* Return         : 组内第一条消息发射标志
*******************************************************************************/
U8 GetBurstFlg(void)
{
    return struSOTDMAPara.BurstFlg;    
}

#if 0
/*******************************************************************************
* Function Name  : SetTxChannel
* Description    : 设置发射信道,SART工作方式
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetTxChannel(void)
{
    if (struSOTDMAPara.MsgCount & 0x1 == 1)
    {
        struSOTDMAPara.TxChannel = 'A';                             
    }
    else
    {
        struSOTDMAPara.TxChannel = 'B';    
    }
}
#endif

/*******************************************************************************
* Function Name  : SetTxChannel
* Description    : 设置发射信道,SART工作方式
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetTxChannel(void)
{
    if (siStaticPara.TxChannel1 == 2087)
    {
        struSOTDMAPara.TxChannel = 'A';                             
    }
    else if (siStaticPara.TxChannel1 == 2088)
    {
        struSOTDMAPara.TxChannel = 'B';    
    }
    else
    {
        struSOTDMAPara.TxChannel = 'A';    
    }
}

/*******************************************************************************
* Function Name  : GetTxChannel
* Description    : 获取发射信道
* Input          : None
* Output         : None
* Return         : 发射信道
*******************************************************************************/
U8 GetTxChannel(void)
{
    return struSOTDMAPara.TxChannel;    
}
#if 0
/*******************************************************************************
* Function Name  : SetCommStaSlotNum
* Description    : 设置通信状态时隙号
* Input          : ssSlotNum - 通信状态时隙号
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommStaSlotNum(S16 ssSlotNum)
{
    if (ssSlotNum < 0 || ssSlotNum >= 2250)
    {
       ssSlotNum = 0; 
    }

    struSOCommState.usSlotNum = ssSlotNum;
}

/*******************************************************************************
* Function Name  : SetCommStaUTCHourMin
* Description    : 设置通信状态UTC小时和分钟
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommStaUTCHourMin(void)
{
    struSOCommState.usUTC_HourMin &= 0x0;

    if (IsGpsFix() == TRUE)
    {
       struSOCommState.usUTC_HourMin |= GpsPara.GPS_TimeH << 9;
       struSOCommState.usUTC_HourMin &= 0xFE00;
       struSOCommState.usUTC_HourMin |= GpsPara.GPS_TimeM << 2;
    }
}

/*******************************************************************************
* Function Name  : SetCommStaSlotOffset
* Description    : 设置通信状态时隙偏移值
* Input          : 通信状态时隙偏移值
* Output         : None
* Return         : None
*******************************************************************************/
void SetCommStaSlotOffset(S16 ssSlotOffset)
{
    if (ssSlotOffset < COMM_STAT_SLOT_OFFSET_FLOOR)
    {
        ssSlotOffset = COMM_STAT_SLOT_OFFSET_FLOOR;
    }
    else if (ssSlotOffset > COMM_STAT_SLOT_OFFSET_CEIL)
    {
        ssSlotOffset = COMM_STAT_SLOT_OFFSET_CEIL;
    }

    struSOCommState.usSlotOffset = ssSlotOffset;
}

/*******************************************************************************
* Function Name  : GetCommStaSlotOffset
* Description    : 获取通信状态时隙偏移值
* Input          : None
* Output         : None
* Return         : 通信状态时隙偏移值
*******************************************************************************/
U16 GetCommStaSlotOffset(void)
{
    return struSOCommState.usSlotOffset;
}

/*******************************************************************************
* Function Name  : GetCommStaSlotOffset
* Description    : 获取通信状态时隙超时值
* Input          : None
* Output         : None
* Return         : 通信状态时隙超时值
*******************************************************************************/
S8 GetCommStaSlotTimeout(void)
{
    return struSOCommState.ucSlotTimeout;
}
#endif
/*******************************************************************************
* Function Name  : GetMsgCnt
* Description    : 获取组内消息数
* Input          : None
* Output         : None
* Return         : 组内消息数
*******************************************************************************/
S8 GetSOParaMsgCnt(void)
{
    return struSOTDMAPara.MsgCount;
}

/*******************************************************************************
* Function Name  : SetBurstFinsFlg
* Description    : 置位组发射结束标志
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetBurstFinsFlg(void)
{
    struSOTDMAPara.BurstFinsFlg = TRUE;                            
}

/*******************************************************************************
* Function Name  : ResetBurstFinsFlg
* Description    : 复位组发射结束标志
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ResetBurstFinsFlg(void)
{
    struSOTDMAPara.BurstFinsFlg = FALSE;                            
}

/*******************************************************************************
* Function Name  : GetBurstFinsFlg
* Description    : 获取组发射结束标志
* Input          : None
* Output         : None
* Return         : 组发射结束标志
*******************************************************************************/
U8 GetBurstFinsFlg(void)
{
    return struSOTDMAPara.BurstFinsFlg;
}

/*******************************************************************************
* Function Name  : GetBurstFinsFlgRst
* Description    : 获取组发射结束标志,若已经置位则复位
* Input          : None
* Output         : None
* Return         : 组发射结束标志
*******************************************************************************/
U8 GetBurstFinsFlgRst(void)
{
    if (struSOTDMAPara.BurstFinsFlg == TRUE)
    {
        ResetBurstFinsFlg();

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* Function Name  : SetBurstStarFlg
* Description    : 置位组发送开始标志
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetBurstStarFlg(void)
{
    struSOTDMAPara.BurstStarFlg = TRUE;                            
}

/*******************************************************************************
* Function Name  : ResetBurstStarFlg
* Description    : 复位组发送开始标志
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ResetBurstStarFlg(void)
{
    struSOTDMAPara.BurstStarFlg = FALSE;                            
}

/*******************************************************************************
* Function Name  : GetBurstStarFlg
* Description    : 获取组发送开始标志
* Input          : None
* Output         : None
* Return         : 组发送开始标志
*******************************************************************************/
U8 GetBurstStarFlg(void)
{
    return struSOTDMAPara.BurstStarFlg;                            
}

/*******************************************************************************
* Function Name  : GetBurstStarFlgRst
* Description    : 获取组发送开始标志,并复位
* Input          : None
* Output         : None
* Return         : 组发送开始标志
*******************************************************************************/
U8 GetBurstStarFlgRst(void)
{
    if (struSOTDMAPara.BurstStarFlg == TRUE)
    {
        ResetBurstStarFlg();                            

        return TRUE;
    }
    else
    {
        return FALSE;
    }                            
}

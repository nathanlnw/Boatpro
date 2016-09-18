/*******************************************************************************
  Copyright (C), 2012, 712.Co.,Ltd.
  FileName: PTL_main.c
  Author: 王畅      Version : V1.00        Date: 2012.07.10
  Description:    实现协议栈消息组织与发送
  Version:         V1.00
  Function List:   // 主要函数及其功能
  History:         // 历史修改记录
      <author>  <time>   	<version >   <desc>
      lq        2013.09.06               add AISmain_4503_ITDMA()
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "AIS_MsgModu.h"
#include "Ptl_Interface.h"
#include "PTL_Main.h"
#include "AISconfig.h"
#include "initial.h"
#include "sleepctrl.h"
#include "SOTDMA.h"
#include "gpsctrl.h"
#include "testing.h"

#if 1
#include "tim.h"
#endif 
U8 state_4503 = INIT_4503;

U8 state_4501 = INIT_4501;
U16 timeout_4501_interval;
U16 time_init_4503;
U16 timestamp_sos;
U16 flag_sos;
U16 idle_4503 = 0;

U16 SosSendProtectTime = 0; 

U8 ucChannelCurr = CHANNEL_A;
extern S32 GetFrameNum(void);
extern S16 GetSlotNum(void);
extern U8 GetMMISOSState(void);
extern U16 Update_GpsData_To_Msg(void);

/******************************************
函数：Ptl_Init
功能: 协议栈初始化
输入:
输出:
返回值：
备注：
*******************************************/
U16 Ptl_Init(void)
{
#if 0
	InitInterfacePara();

	SendSlotSelectedFlag = 0;
#endif
	InitPtl_Interface();
#if 0
	InitMsgPara();
#endif
	state_4503 = INIT_4503;
	state_4501 = INIT_4501;
	GPS_valid = 0;
	flag_sos = 0;

//	ModeFlag = eqEquipPara.ucPowerSaveMode == EQ_MOD_POWERSAVE ? 0 : 1;

//	SosSendProtectTime = eqEquipPara.SosProtectTime * 375;//时隙10秒 = 375

	return 0;
}

/*******************************************************************************
* 名称  : get_timestamp
* 描述  : 获取时间戳
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U16 get_timestamp(void)
{
	S32 min;
	S16 slot;
	U16 timestamp;
	
	min = GetFrameNum();
	slot = GetSlotNum();
	timestamp = (((U8)(min & TIMESTAMP_MASK)) * FRAMETOSLOT) + (U16)slot;
	
	return timestamp;
}

/*******************************************************************************
* 名称  : IsTimeout
* 描述  : 验证是否超时
* 输入  : time1要等的时间
		  time2当前时间	
		  timeout超时时间	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U16 IsTimeout(U16 time1, U16 time2, U16 timeout)
{
	if(time2 >= time1)
	{
		return (time2 - time1 >= timeout);
	}
	else
	{
	//lq	return ((time2 - time1 + (TIMESTAMP_MASK * FRAMETOSLOT + FRAMETOSLOT - 1)) >= timeout);
        return ((time2 - time1 + (TIMESTAMP_MASK * FRAMETOSLOT + FRAMETOSLOT)) >= timeout);
	}
}

/*******************************************************************************
* 名称  : set_sos_timestamp
* 描述  : 设置SOS超时时间戳
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U16 set_sos_timestamp(void)
{
	timestamp_sos = get_timestamp();
	return 0;
}

/*******************************************************************************
* 名称  : get_timestamp
* 描述  : 获取时间戳
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
S32 get_timestamp_slot(void)
{
	S32 min;
	S16 slot;
	S32 timestamp;
	
	min = GetFrameNum();
	slot = GetSlotNum();
	timestamp = min * FRAMETOSLOT + (U16)slot;
	
	return timestamp;
}

/*******************************************************************************
* 名称  : IsTimeout
* 描述  : 验证是否超时
* 输入  : time1要等的时间
		  time2当前时间			  	  
* 输出  : 无
* 返回  : 是否超时
*******************************************************************************/
U8 IsTimeout_slot(S32 time1, S32 time2, S16 timeout)
{	
    return (time2 - time1 >= timeout);
}

/*******************************************************************************
* 名称  : set_sos_timestamp_slot
* 描述  : 设置SOS超时时间戳
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U16 set_sos_timestamp_slot(void)
{
	timestamp_sos = get_timestamp_slot();
	return 0;
}

/*******************************************************************************
* 名称  : set_4501_interval
* 描述  : 计算4501模式消息间隔
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U16 set_4501_interval(U16 speed)
{
	if(speed >= SPEEDFLAG)
	{
		timeout_4501_interval = TIMEOUT_HIGHSPEED_4501;
	}
	else
	{
		timeout_4501_interval = TIMEOUT_LOWSPEED_4501;
	}
	return 0;
}

U16 time_4501_last1;
U16 time_4501_last2;

#if 0
/*******************************************************************************
* 名称  : AISmain_4503_SOTDMA
* 描述  : ITDMA接入方式,WWY消息发送方式
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U16 AISmain_4503_ITDMA(void)
{
	U16 i;
	S32 time_4501_now;
    S32 slSlotCurr;
	U8 ucSendIntervalChangeFlg;
    U16 usTempSlot;
    //U16 usRandSlotOffset;
	
    /*判断MMSI是否有效*/   
	if(aiAISCell.UserID == 0)
	{   
		return 0;
	}

    slSlotCurr = get_timestamp_slot();                                          //lq 获取当前时间戳

	switch(state_4501)
	{
		case INIT_4501:
            
            /*lq 开机超过1min或开机1min内定位且同步完成则开始发送*/
            if(slSlotCurr >= 2250 || GetGpsPowerupFixState() == TRUE)
			{
                //lq...
#if 0           //lq 控制发射时隙为整数秒
                slSlotCurr = slSlotCurr + SLOT_INTERVAL_SEND_TO_TX + 1;
                slSlotCurr = slSlotCurr / 75 + 1;
                slSlotCurr *= 75;
                SetBurstTxTim(slSlotCurr);                    
#else
                SetBurstTxTim(slSlotCurr + SLOT_INTERVAL_SEND_TO_TX + 1);                    
#endif
                SetBurstSendTim(SLOT_INTERVAL_SEND_TO_TX);
			    state_4501 = NORMAL_WORK_BURST;
            }
			else
			{
                //lq...
			}

			break;
		case NORMAL_WORK_BURST:
			if(IsTimeout_slot(GetBurstSendTim(), slSlotCurr, 0))                //lq Burst发送定时到
			{   					
                SetBurstStarFlg();
                //lq ResetBurstFinsFlg();//lq... 启用standby模式需删除该语句
#if 0
                SetCommStaSlotNum(GetBurstTxTimSlot());				

                SetCommStaUTCHourMin();

                SetCommStaSlotOffset(GetRandSlotOffset());                      //lq 下一集的第一组的时隙偏移值

                struSOCommState.ucSlotTimeout = --struSOCommState.ucSlotTimeout < 0 
                                                ? 7 : struSOCommState.ucSlotTimeout;

                SetCommState();
#endif
                SetBurstFlg(TRUE);
 
                struSOTDMAPara.MsgCount = 1;
                SetTxChannel();

                /*lq 获取加入随机偏移的预占时隙的增量*/
                struSOTDMAPara.SlotOffset = GetRandSlotOffset((S16)eqEquipPara.SendInterval * 2250); //lq 后面会再次用到，需保存
                SetCommStateITDMA(GetGpsNormalFixState(), struSOTDMAPara.SlotOffset);

                /*lq 先后发送18和19号消息*/                 
                SendMsg18_SO(TRUE, FALSE);
			    usart_printf("[%6d:%4d]$Msg18 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);

                SendMsg19_SO(TRUE, FALSE);          
			    usart_printf("[%6d:%4d]$Msg19 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);
                                                                                       	 
                /*lq 该组内其他消息发射及发送定时*/
                SetMsgTxTim(SLOT_INTERVAL_MSG_IN_BURST);
                SetMsgSendTim(SLOT_INTERVAL_SEND_TO_TX);
                
                /*lq 下一组消息的发射及发送定时*/                
                SetBurstTxTim(GetSetCommStateSlotIncr());                                
                SetBurstSendTim(SLOT_INTERVAL_SEND_TO_TX);

                //lq SetGpsOnTim(SLOT_INTERVAL_GPS_ON_TO_TX);
                
                state_4501 = NORMAL_WORK_MSG;
#if 0 //lqdb
                gsiPhaseCnt = 0;                                                                //lq  gsiPhaseCnt、gsiSlotCnt、gslFrameCnt                                                                               
                gsiSlotCnt = 0;                                                                 //lq  三者仅用于消息发送的时间控制，
                gslFrameCnt = 0;                
#endif
			}
            
            break;
        case NORMAL_WORK_MSG:
			if(IsTimeout_slot(GetMsgSendTim(), slSlotCurr, 0))  //lq Burst内Msg发送定时到
			{   					
                struSOTDMAPara.MsgCount++;
                SetTxChannel();

                SetBurstFlg(FALSE);

                SetCommStateITDMA(GetGpsNormalFixState(), struSOTDMAPara.SlotOffset);

                /*lq 先后发送18和19号消息*/
                SendMsg18_SO(FALSE, FALSE);
			    usart_printf("[%6d:%4d]$Msg18 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);
                
                SendMsg19_SO(FALSE, FALSE);    
			    usart_printf("[%6d:%4d]$Msg19 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);	 
                                                                          
                if(struSOTDMAPara.MsgCount < eqEquipPara.NormalSendTime)
                {
                    SetMsgTxTim(SLOT_INTERVAL_MSG_IN_BURST);
                    SetMsgSendTim(SLOT_INTERVAL_SEND_TO_TX);
                }
                else
                {
                    state_4501 = NORMAL_WORK_BURST;                
                }
			}

            break;
		default:
			break;
	}

	return 0;
}
#endif

#if 0
/*******************************************************************************
* 名称  : AISmain_AtoN_FATDMA
* 描述  : FATDMA接入方式，AtoN消息发送方式
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U16 AISmain_AtoN_FATDMA(void)
{
	U16 i;
	S32 time_4501_now;
    S32 slSlotCurr;
	U8 ucSendIntervalChangeFlg;
    U16 usTempSlot;
    //U16 usRandSlotOffset;
	U32 slFrameCurr;
    U32 slFrameDay;
    U32 slSlotDay;
    U32 slFirstTxTime;
    U32 slTemp;

    /*判断MMSI是否有效*/   
	if(aiAISCell.UserID == 0)
	{   
		return 0;
	}

    slSlotCurr = get_timestamp_slot();                                          //lq 获取当前时间戳
    slFrameCurr = GetFrameNum();
	
    switch (state_4501)
	{
		case INIT_4501:
            
            /*lq 开机定位后才发送第一条消息*/
            if (GetGpsPowerupFixState() == TRUE)
			{
                /*lq 计算第一次发送时间*/
                slFirstTxTime = (siStaticPara.TxHourChannel1 * 60 + siStaticPara.TxMinChannel1) * 2250 
                                   + siStaticPara.TxStartSlotChannel1;
                
                slFrameDay = slFrameCurr % 1440;                                //lq 一天中的帧计数（以0点为基准）
                slSlotDay = slFrameDay * 2250 + GetSlotNum();

			    /*lq 第一次发送时间到 */
                if ((slSlotDay < slFirstTxTime)
                    && IsTimeout_slot(slFirstTxTime, slSlotDay + SLOT_INTERVAL_SEND_TO_TX, 0))
                {
                    slFirstTxTime += (slFrameCurr - slFrameDay) * 2250;         //lq 因发射时间定时使用本地帧计数，故发射时间为指定的发射时隙相对于0时的相对时隙数加上至上一个0点时本地帧计数值
                    SetBurstTxTim(slFirstTxTime);                                                        
                    
                    SetBurstSendTim(SLOT_INTERVAL_SEND_TO_TX);
    			    
                    state_4501 = NORMAL_WORK_BURST;
                                
                }                
#if 0           //lq 控制发射时隙为整数秒
                slSlotCurr = slSlotCurr + SLOT_INTERVAL_SEND_TO_TX + 1;
                slSlotCurr = slSlotCurr / 75 + 1;
                slSlotCurr *= 75;
                SetBurstTxTim(slSlotCurr);                    
#endif

            }
			else
			{
                //lq...
			}

			break;
		case NORMAL_WORK_BURST:
			if (IsTimeout_slot(GetBurstSendTim(), slSlotCurr, 0))                //lq Burst发送定时到
			{   					
                SetBurstStarFlg();
                //lq ResetBurstFinsFlg();//lq... 启用standby模式需删除该语句
#if 0
                SetCommStaSlotNum(GetBurstTxTimSlot());				

                SetCommStaUTCHourMin();

                SetCommStaSlotOffset(GetRandSlotOffset());                      //lq 下一集的第一组的时隙偏移值

                struSOCommState.ucSlotTimeout = --struSOCommState.ucSlotTimeout < 0 
                                                ? 7 : struSOCommState.ucSlotTimeout;

                SetCommState();
#endif
                SetBurstFlg(TRUE);
 
                struSOTDMAPara.MsgCount = 1;
                SetTxChannel();
#if 0
                /*lq 获取加入随机偏移的预占时隙的增量*/
                struSOTDMAPara.SlotOffset = GetRandSlotOffset((S16)eqEquipPara.SendInterval * 2250); //lq 后面会再次用到，需保存
                SetCommStateITDMA(GetGpsNormalFixState(), struSOTDMAPara.SlotOffset);
#endif
                /*lq 发送21号消息*/                 
                SendMsg21();
			    usart_printf("[%6d:%4d]$Msg21 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);

                SendMsg6();          
			    usart_printf("[%6d:%4d]$Msg06 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);

                /*lq 14消息 */
                if (GetAtoNAlarmFlag() == TRUE)
                {
                    SendMsg14();          
    			    usart_printf("[%6d:%4d]$Msg14 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                                 struSOTDMAPara.MsgCount);                                    
                }

                if (struSOTDMAPara.MsgCount < eqEquipPara.NormalSendTime)
                {                                                                                       	 
                    /*lq 该组内其他消息发射及发送定时*/
                    SetMsgTxTim(SLOT_INTERVAL_MSG_IN_BURST);
                    SetMsgSendTim(SLOT_INTERVAL_SEND_TO_TX);

                    state_4501 = NORMAL_WORK_MSG;
                }

                /*lq 下一组消息的发射及发送定时*/                
                if ((siStaticPara.OffPositionBehav == 1)
                    && (OP1_GetGPSOffPositionValid() == TRUE)
                    && (OP1_GetGPSOffPositionStat() == TRUE))
                {
                    /*lq 位置偏离时，使用新的发送间隔*/
                    SetBurstTxTim(siStaticPara.TxSlotIntervalChannel1Off);                                                
                }
                else
                {
                    SetBurstTxTim(siStaticPara.TxSlotIntervalChannel1);                                                
                }
                SetBurstSendTim(SLOT_INTERVAL_SEND_TO_TX);
			}
            
            break;

        case NORMAL_WORK_MSG:
			if(IsTimeout_slot(GetMsgSendTim(), slSlotCurr, 0))  //lq Burst内Msg发送定时到
			{   					
                struSOTDMAPara.MsgCount++;
                SetTxChannel();

                SetBurstFlg(FALSE);

                SetCommStateITDMA(GetGpsNormalFixState(), struSOTDMAPara.SlotOffset);

                /*lq 发送21号消息*/
                SendMsg21();
			    usart_printf("[%6d:%4d]$Msg21 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);
                
                SendMsg6();    
			    usart_printf("[%6d:%4d]$Msg06 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);	 
                                                                          
                /*lq 14消息 */
                if (GetAtoNAlarmFlag() == TRUE)
                {
                    SendMsg14();          
    			    usart_printf("[%6d:%4d]$Msg14 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                                 struSOTDMAPara.MsgCount);                                    
                }

                if(struSOTDMAPara.MsgCount < eqEquipPara.NormalSendTime)
                {
                    SetMsgTxTim(SLOT_INTERVAL_MSG_IN_BURST);
                    SetMsgSendTim(SLOT_INTERVAL_SEND_TO_TX);
                }
                else
                {
                    state_4501 = NORMAL_WORK_BURST;                
                }
			}

            break;

		default:
			break;
	}

	return 0;
}
#endif

#if 0
/*******************************************************************************
* 名称  : AISMain_4501
* 描述  : 4501消息发送协议（暂时采用WWY的发送方式，18和19消息连续发送，每组发送两
          条，6/8/12号消息暂未发送）
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U16 AISMain_4501(void)
{
	U16 i;
	S32 time_4501_now;
    S32 slSlotCurr;
	U8 ucSendIntervalChangeFlg;
    U16 usTempSlot;
    //U16 usRandSlotOffset;
	
    /*判断MMSI是否有效*/   
	if(aiAISCell.UserID == 0)
	{   
		return 0;
	}

    slSlotCurr = get_timestamp_slot();                                          //lq 获取当前时间戳

	switch(state_4501)
	{
		case INIT_4501:
            
            /*lq 开机超过1min或开机1min内定位且同步完成则开始发送*/
            if(slSlotCurr >= 2250 || GetGpsPowerupFixState() == TRUE)
			{
                //lq...
#if 0           //lq 控制发射时隙为整数秒
                slSlotCurr = slSlotCurr + SLOT_INTERVAL_SEND_TO_TX + 1;
                slSlotCurr = slSlotCurr / 75 + 1;
                slSlotCurr *= 75;
                SetBurstTxTim(slSlotCurr);                    
#else
                SetBurstTxTim(slSlotCurr + SLOT_INTERVAL_SEND_TO_TX + 1);                    
#endif
                SetBurstSendTim(SLOT_INTERVAL_SEND_TO_TX);
			    state_4501 = NORMAL_WORK_BURST;
            }
			else
			{
                //lq...
			}

			break;
		case NORMAL_WORK_BURST:
			if(IsTimeout_slot(GetBurstSendTim(), slSlotCurr, 0))                //lq Burst发送定时到
			{   					
                SetBurstStarFlg();

                SetBurstFlg(TRUE);
 
                struSOTDMAPara.MsgCount = 1;
                SetTxChannel();

                /*lq 获取加入随机偏移的预占时隙的增量*/
                struSOTDMAPara.SlotOffset = GetRandSlotOffset((S16)eqEquipPara.SendInterval * 2250); //lq 后面会再次用到，需保存
                SetCommStateITDMA(GetGpsNormalFixState(), struSOTDMAPara.SlotOffset);

                /*lq 先后发送18和19号消息*/                 
                SendMsg18(TRUE);
			    usart_printf("[%6d:%4d]$Msg18 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);

                SendMsg19(TRUE);          
			    usart_printf("[%6d:%4d]$Msg19 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);
                                                                                       	 
                /*lq 该组内其他消息发射及发送定时*/
                SetMsgTxTim(SLOT_INTERVAL_MSG_IN_BURST);
                SetMsgSendTim(SLOT_INTERVAL_SEND_TO_TX);
                
                /*lq 下一组消息的发射及发送定时*/                
                SetBurstTxTim(GetSetCommStateSlotIncr());                                
                SetBurstSendTim(SLOT_INTERVAL_SEND_TO_TX);

                //lq SetGpsOnTim(SLOT_INTERVAL_GPS_ON_TO_TX);
                
                state_4501 = NORMAL_WORK_MSG;
			}
            
            break;
        case NORMAL_WORK_MSG:
			if(IsTimeout_slot(GetMsgSendTim(), slSlotCurr, 0))  //lq Burst内Msg发送定时到
			{   					
                struSOTDMAPara.MsgCount++;
                SetTxChannel();

                SetBurstFlg(FALSE);

                SetCommStateITDMA(GetGpsNormalFixState(), struSOTDMAPara.SlotOffset);

                /*lq 先后发送18和19号消息*/
                SendMsg18(FALSE);
			    usart_printf("[%6d:%4d]$Msg18 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);
                
                SendMsg19(FALSE);    
			    usart_printf("[%6d:%4d]$Msg19 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);	 
                                                                          
                if(struSOTDMAPara.MsgCount < eqEquipPara.NormalSendTime)
                {
                    SetMsgTxTim(SLOT_INTERVAL_MSG_IN_BURST);
                    SetMsgSendTim(SLOT_INTERVAL_SEND_TO_TX);
                }
                else
                {
                    state_4501 = NORMAL_WORK_BURST;                
                }
			}

            break;
		default:
			break;
	}

	return 0;
}
#endif

#if 0
/*******************************************************************************
* 名称  : AISmain_ITDMA_MsgTest
* 描述  : ITDMA接入方式，消息测试
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U16 AISmain_ITDMA_MsgTest(void)
{
	U16 i;
	S32 time_4501_now;
    S32 slSlotCurr;
	U8 ucSendIntervalChangeFlg;
    U16 usTempSlot;
	
    /*判断MMSI是否有效*/   
	if(aiAISCell.UserID == 0)
	{   
		return 0;
	}

    slSlotCurr = get_timestamp_slot();                                          //lq 获取当前时间戳

	switch(state_4501)
	{
		case INIT_4501:
            
            /*lq 开机超过定时或开机定时内定位成功则开始发送*/           
            if(GetMsgTestSendFlg() == TRUE)
			{
                //lq...
                SetBurstTxTim(slSlotCurr + SLOT_INTERVAL_SEND_TO_TX + 1);                    
                SetBurstSendTim(SLOT_INTERVAL_SEND_TO_TX);
			    state_4501 = NORMAL_WORK_BURST;
            }
			else
			{
                //lq...
			}

			break;
		case NORMAL_WORK_BURST:
			if(IsTimeout_slot(GetBurstSendTim(), slSlotCurr, 0))                //lq Burst发送定时到
			{   					
                SetBurstStarFlg();
                
                /*lq 该Burst全部发射完再关闭GPS*/
                ResetBurstFinsFlg();

                /*lq 设置通信状态*/
                SetCommStateITDMA_Test(GetGpsNormalFixState());
                
                SetBurstFlg(TRUE);
                
                struSOTDMAPara.MsgCount = 1;
                SetTxChannel();
                                                                          
                SendMsg18(TRUE);
			    usart_printf("[%6d:%4d]$Msg18 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);

                SendMsg19(TRUE);
			    usart_printf("[%6d:%4d]$Msg19 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                             struSOTDMAPara.MsgCount);

                SetMsgTxTim(SLOT_INTERVAL_MSG_IN_BURST);
                SetMsgSendTim(SLOT_INTERVAL_SEND_TO_TX);
                                                                
                state_4501 = NORMAL_WORK_MSG;
			}
            
            break;
        case NORMAL_WORK_MSG:
			if(IsTimeout_slot(GetMsgSendTim(), slSlotCurr, 0))  //lq Burst内Msg发送定时到
			{   					
                struSOTDMAPara.MsgCount++;
                SetTxChannel();

                SetBurstFlg(FALSE);

                if (GetSOParaMsgCnt() == eqEquipPara.NormalSendTime)
                {
                    /*lq 设置通信状态*/
                    SetCommStateITDMA_Test(GetGpsNormalFixState());
                    	                     
                    SendMsg18(FALSE);    
			        usart_printf("[%6d:%4d]$Msg18 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                                 struSOTDMAPara.MsgCount);
                    SendMsg19(FALSE);    
			        usart_printf("[%6d:%4d]$Msg19 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                                 struSOTDMAPara.MsgCount);
                                                                 	                     
                    /*lq 可以关机*/
                    struMsgTestPara.ucPowerDownEn = TRUE;
                    
                    state_4501 = NORMAL_WORK_IDLE;
                }
                else
                {
                    /*lq 设置通信状态*/
                    SetCommStateITDMA_Test(GetGpsNormalFixState());
                                          
                    SendMsg18(FALSE);    
			        usart_printf("[%6d:%4d]$Msg18 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                                 struSOTDMAPara.MsgCount);                

                    SendMsg19(FALSE);    
			        usart_printf("[%6d:%4d]$Msg19 Num : Msg_%d\r\n", GetFrameNum(), GetSlotNum(),
                                 struSOTDMAPara.MsgCount);
                }

                if(struSOTDMAPara.MsgCount < eqEquipPara.NormalSendTime)
                {
                    SetMsgTxTim(SLOT_INTERVAL_MSG_IN_BURST);
                    SetMsgSendTim(SLOT_INTERVAL_SEND_TO_TX);
                }
			}

            break;
        case NORMAL_WORK_IDLE:

            break;
        default:
			break;
	}

	return 0;
}
#endif

/******************************************
函数：Ptl_Main
功能: 协议栈主程序，主函数调用该函数实现消息
管理相关处理
输入:
输出:
返回值：
备注：
*******************************************/
void Ptl_Main(void)
{
#if 0
	Update_GpsData_To_Msg();                                                    //lq 提取解析后的GPS消息，构建要发送的x号消息
	
    AISMain_4501();
#endif

}

/******************************************
函数：Ptl_Main_Test
功能: 消息测试模式，协议栈主程序，主函数调用该函数实现消息
管理相关处理
输入:
输出:
返回值：
备注：
*******************************************/
void Ptl_Main_Test(void)
{
#if 0
	Update_GpsData_To_Msg();                                                    //lq 提取解析后的GPS消息，构建要发送的x号消息
    AISmain_ITDMA_MsgTest();
#endif	
}





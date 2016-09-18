/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: mmi.c
  Author: vozhj      Version : V1.00        Date: 2012.03.21
  Description:    MMI模块
  Version:         V1.00
  Function List:   // 主要函数及其功能
  History:         // 历史修改记录
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#include "def.h"
#include "initial.h"
#include "debug.h"
#include "systemtick.h"
#include "keyscan.h"
#include "mmi.h"
#include "sleepctrl.h"
#include "PTL_Main.h"
#include "Tim.h"
#include "gpsctrl.h"
#include "stdio.h"
#include "parastorage.h"
#include "sotdma.h"

MsgCtrlStruct MsgCtrlPara;
#if CONFIG_KEY
MMIStruct MMIPara;
/*******************************************************************************
* 名称  : IniMMIPara
* 描述  : 初始化MMI必要的参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void IniMMIPara(void)
{
	MMIPara.MMICtrlSta = MMI_CTRLSTA_IDLE;
	MMIPara.MMISta = MMI_STA_NONE;
	MMIPara.Time100msCnt = 0;

	MMIPara.TimeCnt = 0;
	MMIPara.TimePeriod = 0;

	MMIPara.SOSConfirm=FALSE;//sos告警假

//	MMIPara.MMIKeyValue = eqEquipPara.SosKeyTime * 10;
    MMIPara.MMIKeyValue = 10;
}

/*******************************************************************************
* 名称  : RstMMIPara
* 描述  : 复位MMI必要的参数,保留SOS
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void RstMMIPara(void)//和ini差别就是不更改sosconfirm
{
	MMIPara.MMICtrlSta = MMI_CTRLSTA_IDLE;
	MMIPara.MMISta = MMI_STA_NONE; //这个无用
	MMIPara.Time100msCnt = 0;

	MMIPara.TimeCnt = 0;
	MMIPara.TimePeriod = 0;
}

/*******************************************************************************
* 名称  : MMIProcess
* 描述  : MMI控制主函数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void MMIProcess(void)
{
	U8 ucKeySta;
	//U16 MMIKeyValue;

	//MMIKeyValue = eqEquipPara.SosKeyTime * 10;

	//IDLE状态，检测按键是否有效
	if (MMIPara.MMICtrlSta == MMI_CTRLSTA_IDLE)
	{
#if !WIN32
		ucKeySta = GetIndivIO(KEY_SOS_ADDR);
		
		if (ucKeySta == KEY_LOW)
		{
			//开启按键检测定时器
			MMIPara.TimePeriod = MMI_100SECPEROID;
			MMIPara.TimeCnt = SetTimePeriodProcess(MMIPara.TimePeriod);

			MMIPara.MMICtrlSta = MMI_CTRLSTA_STEP1;			
		}
#endif
		
	}
	//按键持续按下检测
	else if (MMIPara.MMICtrlSta == MMI_CTRLSTA_STEP1)
	{
		//100ms定时到了
		if (MMIPara.TimeCnt == GetLocalClockTickProcess())
		{
#if !WIN32
			ucKeySta = GetIndivIO(KEY_SOS_ADDR);

			//表示持续按键按下
			if (ucKeySta == KEY_LOW)
			{				
				MMIPara.TimeCnt = SetTimePeriodProcess(MMIPara.TimePeriod);
				
				MMIPara.Time100msCnt++;

				if (MMIPara.Time100msCnt >= MMIPara.MMIKeyValue)
				{
					MMIPara.Time100msCnt = 0;
					//此处加入向协议栈发送消息
					//?????????????????????????
					MMIPara.SOSConfirm=TRUE;//确认sos

					UartResponseMsg("$P712,001,,,SOS Key Down!");
					MMIPara.MMICtrlSta = MMI_CTRLSTA_STEP2;
				}
			}
			else
			{
				RstMMIPara();
			}
#endif
		}
	}
	else
	{
#if !WIN32
		ucKeySta = GetIndivIO(KEY_SOS_ADDR);
		
		if (ucKeySta == KEY_HIGH)
		{
			//表示按键抬起，回到初始状态
			RstMMIPara();
		}
#endif
	}
}

/*******************************************************************************
* 名称  : GetMMISOSState
* 描述  : 获取MMI模块SOS告警状态
* 输入  : 无		  
* 输出  : 无
* 返回  : TRUE表示告警，读一次后复位
*******************************************************************************/
U8 GetMMISOSState(void)
{
	if (MMIPara.SOSConfirm == TRUE)
	{
		MMIPara.SOSConfirm = FALSE;
		return (TRUE);		
	}
	else
	{
	 	return (FALSE);
	}
}

/*******************************************************************************
* 名称  : GetMMISleepSta
* 描述  : 获取MMI模块省电状态
* 输入  : 无		  
* 输出  : 无
* 返回  : TRUE表示可以进入省电，FALSE禁止进入省电
*******************************************************************************/
U8 GetMMISleepSta(void)
{
	if (MMIPara.MMICtrlSta == MMI_CTRLSTA_IDLE)
	{
		return (TRUE);
	}		
	else
	{			   
		return (FALSE);
	}
}
#endif

/*******************************************************************************
* 名称  : InitMsgCtrlPara
* 描述  : 初始化消息控制参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitMsgCtrlPara(void)
{
	MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_POWERUP;
	MsgCtrlPara.GpsStartFrame = 0;
	MsgCtrlPara.GpsStartSlot = 0;
	MsgCtrlPara.GpsPowerup = FALSE;
	MsgCtrlPara.EnterHighFreq = FALSE;
	MsgCtrlPara.WaitCount = 0;
    MsgCtrlPara.slGpsOnSlot = 0;
}		

#if 0
/*******************************************************************************
* 名称  : MsgSendErrorCheck
* 描述  : 检测是否发送超时
* 输入  : slot时隙 frame 帧		  
* 输出  : 无
* 返回  : TRUE 超时 FALSE 不超
*******************************************************************************/
U8 MsgSendErrorCheck(S16 Slot,S16 Frame)
{
	if (Frame > eqEquipPara.SendErrorPeriod + MsgCtrlPara.GpsStartFrame) 
	{
		return (TRUE);
	}
	else if (Frame == eqEquipPara.SendErrorPeriod + MsgCtrlPara.GpsStartFrame)
	{
		if (Slot >= MsgCtrlPara.GpsStartSlot)
		{
			return (TRUE);
		}
	}
	else
	{
		return (FALSE);
	}
	
	return (FALSE);

}	
#endif

#if 0
/*******************************************************************************
* 名称  : MsgCtrlProcess
* 描述  : 消息控制主模块-SART
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void MsgCtrlProcess(void)                                                       //lq 该模块控制是否可以发送消息了，具体消息的发送由其他模块执行
{
	S16 siTempSlot;
	S32 siTempFrame;
    S32 slSlotCurr;

	siTempSlot = GetSlotNum();//获取现在的时间
	siTempFrame = GetFrameNum();
	slSlotCurr = GetSlotSum();

	if (MsgCtrlPara.MsgCtrlState == MSG_CTRL_STA_POWERUP)//刚开机
    {
        if (GetBurstStarFlg() == TRUE)
        {
            MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_SEND;                    
		    usart_printf("[%6d:%4d]$MsgCtrlState : MSG_CTRL_STA_SEND!\r\n", GetFrameNum(), GetSlotNum());	                            
        }                 
    }
    else if (MsgCtrlPara.MsgCtrlState == MSG_CTRL_STA_WAIT_SEND)
    {
        if (GetBurstStarFlg() == TRUE)
        {
            MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_SEND;                    
		    usart_printf("[%6d:%4d]$MsgCtrlState : MSG_CTRL_STA_SEND!\r\n", GetFrameNum(), GetSlotNum());	                            
        }            
    }
    else if (MsgCtrlPara.MsgCtrlState == MSG_CTRL_STA_SEND)
    {
        if (GetBurstFinsFlg() == TRUE)
        {
            /*lq 设定MCU唤醒定时*/				
            SetMcuWakeupTim(SLOT_INTERVAL_MCU_WAKEUP_TO_TX);

            MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_IDLE;                    
		    usart_printf("[%6d:%4d]$MsgCtrlState : MSG_CTRL_STA_IDLE!\r\n", GetFrameNum(), GetSlotNum());            	                            
        }     
    }
    else if (MsgCtrlPara.MsgCtrlState == MSG_CTRL_STA_IDLE)
    {
        /*lq GPS开启定时有效*/
        if (GetGpsOnTimEn() == TRUE)
        {
            /*lq GPS开启定时到*/
            if (IsGpsOnTimeout() == TRUE)
            {
                MsgStartNew();

                /*lq 复位组消息发送开始/结束标志*/  //lq [2013/08/06] GPS长开5min关闭后的开启定时早于关闭的时间，立即开启，未进入休眠
                ResetBurstFinsFlg();
                ResetBurstStarFlg();
            }
        }

        /*lq MCU唤醒定时到*/                
        if (IsMcuWakeupTimeout() == TRUE)
        {
            /*lq 唤醒MCU*/
            SleepCtrlPara.ucMcuWakeupFlg = TRUE;            
            
            /*lq 复位组消息发送开始/结束标志*/
            ResetBurstFinsFlg();
            ResetBurstStarFlg();

            /*lq 本次待机唤醒不开启GPS，直接进入GPS关闭状态设置下个周期GPS开启定时*/
            if (((GetGpsSaveFlg() == TRUE) || (GetGpsUnfix1hLngOpenFlg() == TRUE)
                || (GetGpsUnfixBey1hLngOpenFlg() == TRUE)) && (GetGpsOnEn() == FALSE))
            {
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;            
            }

            MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_WAIT_SEND;                    
		    usart_printf("[%6d:%4d]$MsgCtrlState : MSG_CTRL_STA_WAIT_SEND!\r\n", GetFrameNum(), GetSlotNum());	                            
        }
          
    }
    else
    {
        MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_POWERUP;
		usart_printf("[%6d:%4d]$MsgCtrlState : MSG_CTRL_STA_POWERUP!\r\n", GetFrameNum(), GetSlotNum());	                            
    }

}
#endif

/*******************************************************************************
* 名称  : MsgCtrlProcess
* 描述  : 消息控制主模块-AtoN
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void MsgCtrlProcess(void)                                                       //lq 该模块控制是否可以发送消息了，具体消息的发送由其他模块执行
{
	S16 siTempSlot;
	S32 siTempFrame;
    S32 slSlotCurr;

	siTempSlot = GetSlotNum();//获取现在的时间
	siTempFrame = GetFrameNum();
	slSlotCurr = GetSlotSum();

	if (MsgCtrlPara.MsgCtrlState == MSG_CTRL_STA_POWERUP)//刚开机
    {
        if (GetBurstStarFlg() == TRUE)
        {
            MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_SEND;                    
		    usart_printf("[%6d:%4d]$MsgCtrlState : MSG_CTRL_STA_SEND!\r\n", GetFrameNum(), GetSlotNum());	                            
        }                 
    }
    else if (MsgCtrlPara.MsgCtrlState == MSG_CTRL_STA_WAIT_SEND)
    {
        if (GetBurstStarFlg() == TRUE)
        {
            MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_SEND;                    
		    usart_printf("[%6d:%4d]$MsgCtrlState : MSG_CTRL_STA_SEND!\r\n", GetFrameNum(), GetSlotNum());	                            
        }            
    }
    else if (MsgCtrlPara.MsgCtrlState == MSG_CTRL_STA_SEND)
    {
        if (GetBurstFinsFlg() == TRUE)
        {
            /*lq 设定MCU唤醒定时*/				
            SetMcuWakeupTim(SLOT_INTERVAL_MCU_WAKEUP_TO_TX);

            /*lq 复位GPS唤醒标志*/
            ResetMsgGpsPowerup();       //lq 防止在GPS长开状态下，GPS唤醒标志一直有效，在长开定时到GPS休眠后会立即被唤醒

            MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_IDLE;                    
		    usart_printf("[%6d:%4d]$MsgCtrlState : MSG_CTRL_STA_IDLE!\r\n", GetFrameNum(), GetSlotNum());            	                            
        }     
    }
    else if (MsgCtrlPara.MsgCtrlState == MSG_CTRL_STA_IDLE)
    {
        /*lq MCU唤醒定时到*/
        if (IsMcuWakeupTimeout() == TRUE)
        {
            SleepCtrlPara.ucMcuWakeupFlg = TRUE;            

            MsgStartNew();

            /*lq 复位组消息发送开始/结束标志*/
            ResetBurstFinsFlg();
            ResetBurstStarFlg();            

            MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_WAIT_SEND;                    
		    usart_printf("[%6d:%4d]$MsgCtrlState : MSG_CTRL_STA_WAIT_SEND!\r\n", GetFrameNum(), GetSlotNum());	                            
        }
    }
    else
    {
        MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_POWERUP;
		usart_printf("[%6d:%4d]$MsgCtrlState : MSG_CTRL_STA_POWERUP!\r\n", GetFrameNum(), GetSlotNum());	                            
    }

}

/*******************************************************************************
* 名称  : GetMsgCtrlSleepState
* 描述  : 消息控制模块是否可进入休眠
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GetMsgCtrlSleepState(void)
{
	if (MsgCtrlPara.MsgCtrlState == MSG_CTRL_STA_IDLE)
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}

/*******************************************************************************
* 名称  : MsgStartNew
* 描述  : 开GPS，唤醒系统进入高频模式
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void MsgStartNew(void)
{
	MsgCtrlPara.GpsPowerup = TRUE;
	if (eqEquipPara.ucLowFreqSta == EQ_FREQSTA_LOW)//如果工作在低频模式
	{
		MsgCtrlPara.EnterHighFreq = TRUE;
	}

	//lq MsgCtrlPara.WaitCount = 0;

	ResetGpsNormalFixState();//防止上次对本次的影响，因为模块在外部，所以运行优先级高，不能等GPS模块复位
}

/*******************************************************************************
* 名称  : ResetMsgGpsPowerup
* 描述  : 复位让GPS上电标志位,给GPS当前流程快结束时用
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ResetMsgGpsPowerup(void)
{
	MsgCtrlPara.GpsPowerup = FALSE;
}

/*******************************************************************************
* 名称  : SetMsgGpsPowerup
* 描述  : 置位让GPS上电标志位
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetMsgGpsPowerup(void)
{
	MsgCtrlPara.GpsPowerup = TRUE;
}

/*******************************************************************************
* 名称  : GetMsgGpsPowerup
* 描述  : 获取GPS上电标志位
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GetMsgGpsPowerup(void)
{
	if (MsgCtrlPara.GpsPowerup == TRUE)
	{
		MsgCtrlPara.GpsPowerup = FALSE;
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}

/*******************************************************************************
* 名称  : GetMsgWakeupState
* 描述  : 查询是否可唤醒 
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GetMsgWakeupState(void)
{
	if (MsgCtrlPara.EnterHighFreq == TRUE)
	{
		MsgCtrlPara.EnterHighFreq = FALSE;
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}

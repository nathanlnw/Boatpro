/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: sleepctrl.c
  Author: vozhj      Version : V1.00        Date: 2012.03.21
  Description:    省电控制模块
  Version:         V1.00
  Function List:   // 主要函数及其功能
  History:         // 历史修改记录
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#include "config.h"
#include "def.h"
#include "sleepctrl.h"
#include "gpsctrl.h"
#include "systemtick.h"
#include "initial.h"
#include "powerdown.h"
#include "mmi.h"
#include "digunit.h"
#include "Tim.h"
#include "ledctrl.h"
#include "rtc.h"
#include "power.h"
#include "keyscan.h"
#include "LinkPtlPort.h"
#include "PTL_Main.h"
#include "debug.h"
#include "adjust.h"
#include "led.h"
#include "sotdma.h"
#include "MMI.h"
#include "bkpregister.h"
#include "gmskmod.h"
#include "parastorage.h"

SleepStruct		SleepCtrlPara;
U8 set_sleep_time_flag;//可以设置休眠时间
/*******************************************************************************
* 名称  : IniSleepCtrlPara
* 描述  : 初始化休眠管理必要的参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void IniSleepCtrlPara(void)
{
	SleepCtrlPara.PowerSta = SLEEP_POWERSTA_POWERUP;

	SleepCtrlPara.EnterSleepSlot = 0;
	SleepCtrlPara.EnterSleepFrame = 0;		//进入省电帧

	SleepCtrlPara.WakeUpSlot = 0;		   	//退出省电时隙
	SleepCtrlPara.WakeUpFrame = 0;	   		//退出省电帧

    SleepCtrlPara.TimeCnt = 0;
	SleepCtrlPara.TimePeriod = 0;

	SleepCtrlPara.TimeCnt1 = 0;
    SleepCtrlPara.TimePeriod1 = SLEEP_USART_FLAG_CLEAR_PERIOD;

	SleepCtrlPara.EnterSleepPrtTime = 0;	//进入省电保护时间
	
	SleepCtrlPara.SleepCtrlSta = SLEEP_STA_IDLE;
    
    SleepCtrlPara.ucMcuStopFlg = FALSE;
    SleepCtrlPara.ucMcuStandbyFlg = FALSE;
    
    SleepCtrlPara.ucGpsPulseFlg = FALSE; 
    SleepCtrlPara.ucMcuWakeupFlg = FALSE; 
    SleepCtrlPara.ucUsartFlg = FALSE; 
    SleepCtrlPara.ucStandbyAlarmSetFlg = FALSE;
    SleepCtrlPara.ucStandbyAlarmSetOk = FALSE; 	
    SleepCtrlPara.ucTimToRtcSwitch = FALSE;
    SleepCtrlPara.ucRtcToTimSwitch = FALSE;
}
#if 0
/*******************************************************************************
* 名称  : SleepCtrlHigFreq
* 描述  : 正常工作状态下省电控制，4501模式下不省电，4503模式下省电，要根据
*		  开机状态及省电周期进行控制
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SleepCtrlHigFreq(void)
{
	U8 ucGpsSta;
	U8 ucDigUnitSta;
	U8 ucMMISta;
	U8 ucPowerDownSta;
	U8 ucPtlFifoSta;
	//U8 ucPtlSta; //这个是协议栈状态判断函数，用于返回协议栈的状态
	U8 ucMsgCtrlSta;
	U8 ucAdjustSta;
	U8 ucChk;
//	U8 ucEnterSleep;

	//只有在4503模式下进行省电相关操作
	if (eqEquipPara.ucPowerSaveMode == EQ_MOD_POWERSAVE)	
	{
		ucGpsSta = GetGpsSleepSta();
		ucDigUnitSta = GetDigUnitSleepSta();
		ucMMISta = GetMMISleepSta();
		ucPowerDownSta = GetPowerDownSleepSta();//如果当时没有关机操作
		ucPtlFifoSta = GetPtlFifoSleepSta();//如果信息都发送完了
		
		ucMsgCtrlSta = GetMsgCtrlSleepState();		
		ucAdjustSta = GetAdjustSleepState();//时间调整模块是否可休眠
        //ucPtlSta=Is_4503_Idle();		

		ucChk = (ucGpsSta == TRUE)
				 & (ucDigUnitSta == TRUE)
				 & (ucMMISta == TRUE)
				 & (ucPowerDownSta == TRUE)
				 & (ucPtlFifoSta == TRUE)
				 & (ucMsgCtrlSta == TRUE)
				 & (ucAdjustSta == TRUE);
				 //& (ucPtlSta == TRUE)//协议栈 
		
//		ucChk = TRUE;

		if (SleepCtrlPara.SleepCtrlSta == SLEEP_STA_IDLE)
		{
			//开启省电定时器
			if (ucChk == TRUE)
			{
				UartResponseMsg("$P712,sleep state true");
				SleepCtrlPara.TimePeriod = SLEEP_PRT_PERIOD;
				SleepCtrlPara.TimeCnt = SetTimePeriodProcess(SleepCtrlPara.TimePeriod);
				SleepCtrlPara.EnterSleepPrtTime = 0;
				SleepCtrlPara.SleepCtrlSta = SLEEP_STA_STEP1;					
			}
		}
		else if (SleepCtrlPara.SleepCtrlSta == SLEEP_STA_STEP1)
		{
			if (ucChk == FALSE)
			{
				SleepCtrlPara.SleepCtrlSta = SLEEP_STA_IDLE; 	
			}
			else
			{
				//这个地方应该增加判断 SleepCtrlPara.TimeCnt==GetLocalClockTickProcess()
				if (SleepCtrlPara.TimeCnt==GetLocalClockTickProcess())
				{
					SleepCtrlPara.TimeCnt = SetTimePeriodProcess(SleepCtrlPara.TimePeriod);//这个定时10ms的作用
					SleepCtrlPara.EnterSleepPrtTime++;
				}
				//连续1s满足条件
				if (SleepCtrlPara.EnterSleepPrtTime >= SLEEP_1S_PRTTIME)
				{
					//进入到省电状态
					eqEquipPara.ucLowFreqSta = EQ_FREQSTA_LOW;
					
					UartResponseMsg("SLEEP");
					//关闭LED灯
					LEDInit(LED_TYPE_NONE);
					//sleep_4503();
//lq                    TEST_OUT_LOW();   //lq 开关电源置为高噪声模式
					SleepCtrlPara.SleepCtrlSta = SLEEP_STA_IDLE;
					ResetRtcCount();
							
				}		
			}	
		}				
	}
}
#endif

S32 gslTimToRtcCnt_2 = 0;                                                                
S32 gslRtcToTimCnt_2 = 0; 
S32 gslTimToRtcIdleCnt_2 = 0;                                                                
 
#if 0
/*******************************************************************************
* 名称  : SleepCtrlHigFreq
* 描述  : 正常工作状态下省电控制，4501模式下不省电，4503模式下省电，要根据
*		  开机状态及省电周期进行控制
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SleepCtrlHigFreq(void)
{
	U8 ucGpsSta;
	U8 ucDigUnitSta;
	U8 ucMMISta;
	U8 ucPowerDownSta;
	U8 ucPtlFifoSta;
	U8 ucMsgCtrlSta;
	U8 ucAdjustSta;
	U8 ucChk;
    U8 ucFSKDataXmtSta;
    U8 ucGpsPowerSta;
    U8 ucBurstStartFlg;
    U8 ucBurstFinisFlg;
    U8 ucUbxGpsCommSta;
    U8 ucLB1CommSta;
    U8 ucUsartRecSta;

    /*lq 定时清理USART唤醒标志 */
    if (GetUsartFlg() == TRUE)
    {
	    if (SleepCtrlPara.TimeCnt1 == GetLocalClockTickProcess())
        {
            ResetUsartFlg();
        }                   
    }

	/*lq 休眠条件*/
    ucGpsSta = GetGpsSleepSta();
	ucDigUnitSta = GetDigUnitSleepSta();
#if CONFIG_KEY
	ucMMISta = GetMMISleepSta();
#endif
#if CONFIG_POWERDOWN
	ucPowerDownSta = GetPowerDownSleepSta();                                    //如果当时没有关机操作
#endif
	ucPtlFifoSta = GetPtlFifoSleepSta();                                        //如果信息都发送完了
	ucMsgCtrlSta = GetMsgCtrlSleepState();		
	ucAdjustSta = GetAdjustSleepState();                                        //时间调整模块是否可休眠
    ucFSKDataXmtSta = GetFSKDataXmtSleepState();                                //调制数据发射模块				
    ucUbxGpsCommSta = UBX1_GetGpsCommSleepSta();                                //是否正在与GPS通信
#if 0
    ucLB1CommSta = LB1_GetLBCommSleepSta();                                     //是否正在于Light Beacon通信
#else
    ucLB1CommSta = TRUE;
#endif
    ucUsartRecSta = GetUsartFlg(); 
#if 0
    ucGpsPowerSta = GetGpsPowerSta();                                           //lq 从standby模式下唤醒需将GPS的供电状态置为供电
#endif
	ucChk = (ucGpsSta == TRUE)
			 & (ucDigUnitSta == TRUE)
#if CONFIG_KEY
			 & (ucMMISta == TRUE)
#endif
#if CONFIG_POWERDOWN
			 & (ucPowerDownSta == TRUE)
#endif
			 & (ucPtlFifoSta == TRUE)
			 & (ucMsgCtrlSta == TRUE)
			 & (ucAdjustSta == TRUE)
             & (ucFSKDataXmtSta == TRUE
             & (ucUbxGpsCommSta == TRUE)
             & (ucLB1CommSta == TRUE)
             & (ucUsartRecSta == FALSE));

	if (SleepCtrlPara.SleepCtrlSta == SLEEP_STA_IDLE)
	{
		//开启省电定时器
		if (ucChk == TRUE)
		{
			UartResponseMsg("$Sleep Conditions True!");
			SleepCtrlPara.TimePeriod = SLEEP_PRT_PERIOD;
			SleepCtrlPara.TimeCnt = SetTimePeriodProcess(SleepCtrlPara.TimePeriod);
			SleepCtrlPara.EnterSleepPrtTime = 0;
			SleepCtrlPara.SleepCtrlSta = SLEEP_STA_STEP1;					
		}
	}
	else if (SleepCtrlPara.SleepCtrlSta == SLEEP_STA_STEP1)
	{
		if (ucChk == FALSE)
		{
			SleepCtrlPara.SleepCtrlSta = SLEEP_STA_IDLE; 
			SleepCtrlPara.EnterSleepPrtTime = 0;            	
		}
		else
		{
			if (SleepCtrlPara.TimeCnt == GetLocalClockTickProcess())
			{
				SleepCtrlPara.TimeCnt = SetTimePeriodProcess(SleepCtrlPara.TimePeriod);//这个定时10ms的作用
				SleepCtrlPara.EnterSleepPrtTime++;
			}

			if (SleepCtrlPara.EnterSleepPrtTime >= SLEEP_1S_PRTTIME)			//连续1s满足条件
			{
                #if 0
                    /*lq 保存GPS成功定位次数、GPS超过3次不定位的次数、GPS5min长开次数至EEPROM */
                	sprintf((char *)ucTemp, "%5d", GpsCtrlPara.GpsFixCount);
                   	Para_Save(PARA_GPS_FIX_COUNT_TYPE, ucTemp, 5);
                
                	sprintf((char *)ucTemp, "%3d", GpsCtrlPara.GpsWaitCount);
                   	Para_Save(PARA_GPS_WAIT_COUNT_TYPE, ucTemp, 3);
                
                	sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
                   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);
                
                    /*lq 每8min保存一次关机时间*/
                    if (struSOCommState.ucSlotTimeout == 7)
                    {
                    	if (GetGpsNormalFixState() == TRUE)                                //lq 只有本次定位成功后，才会记录关机时间
                    	{
                    		/*lq 记录GPS日期和时间*/
                            sprintf((char *)ucTemp, "%2d", GpsPara.GPS_TimeMon);
                    		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeD);
                    		strcat((char *)ucTemp, (char *)ucTemp1);
                            sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeH);
                    		strcat((char *)ucTemp, (char *)ucTemp1);
                    		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeM);
                    		strcat((char *)ucTemp, (char *)ucTemp1);
                        	Para_Save(PARA_TIME_CLOSE_TYPE, ucTemp, PARA_TIME_CLOSE_MAXLEN);                           //lq [2013/04/16]
                    		
#if CONFIG_PARASTORAGE_BTT                
                            /*lq 记录本地帧计数的工作时长*/
                			sprintf((char *)ucTemp,"%5d",GetFrameNum());
                			Para_Save(PARA_BATTERY_TEST_TIME_TYPE,ucTemp,5);
#endif                    	
                        } 
                    }
                #endif   
#if 0                          
                /*相位计数由Tim定时转为RTC定时*/
                SleepCtrlPara.ucTimToRtcSwitch = TRUE; //lq SleepCtrlPara.ucTimToRtcSwitch需在eqEquipPara.ucLowFreqSta之前设置，防止定时中断在二者之间触发
                				
                /*进入到省电状态*/
				eqEquipPara.ucLowFreqSta = EQ_FREQSTA_LOW;
#endif
                /*lq 复位组消息发送开始/结束标志*/
                ResetBurstFinsFlg();
                ResetBurstStarFlg();
				
				LEDInit(LED_TYPE_NONE); //关闭LED灯

				SleepCtrlPara.SleepCtrlSta = SLEEP_STA_IDLE;
                SleepCtrlPara.ucMcuWakeupFlg = FALSE;

                /*lq 若GPS已关闭，则进入Standby模式（GPS开启状态下，进入Standby模式GPS会掉电）*/
#if 0
                if (GetGpsPowerSta() == FALSE)  //lq SART
#else
                if (0)                      //lq AtoN工作模式下，MCU不进入standby
#endif
                {
                    /*lq 计算待机休眠时长*/
                    // struRtcPara.ssRtcAlarmValue = CalcuSleepSpan();
                    // usart_printf("ssAlarmValue : %ld\r\n",struRtcPara.ssRtcAlarmValue);				                    
                    // ResetRtcCount(struRtcPara.ssRtcAlarmValue);//lq 休眠定时
#if 0                
                    gslTimToRtcCnt_2 = 0;
                    gslTimToRtcIdleCnt_2 = 0;
                    TEST_OUT_LOW_2();                               

#endif 
		            SleepCtrlPara.ucStandbyAlarmSetFlg = TRUE; 	
#if 1                          
                    /*相位计数由Tim定时转为RTC定时*/
                    SleepCtrlPara.ucTimToRtcSwitch = TRUE; //lq SleepCtrlPara.ucTimToRtcSwitch需在eqEquipPara.ucLowFreqSta之前设置，防止定时中断在二者之间触发
                    				
                    /*进入到省电状态*/
    				eqEquipPara.ucLowFreqSta = EQ_FREQSTA_LOW;
#endif
  
                    // usart_printf("[%6d:%4d]$MCU WorkMode : Standby!\r\n", GetFrameNum(), GetSlotNum());	                
                    
                    while(SleepCtrlPara.ucStandbyAlarmSetFlg == TRUE)
                    {
                       // Feed_Dog();
                    }

                    /*lq 进入待机模式*/
                    EnterStandbyMod();
                }
                else
                {
                    // usart_printf("[%6d:%4d]$MCU WorkMode : Stop!\r\n", GetFrameNum(), GetSlotNum());	
#if 0                
                    gslTimToRtcCnt_2 = 0;
                    gslTimToRtcIdleCnt_2 = 0;
                    TEST_OUT_LOW_2();                               

#endif 
#if 1                          
                    /*相位计数由Tim定时转为RTC定时*/
                    SleepCtrlPara.ucTimToRtcSwitch = TRUE; //lq SleepCtrlPara.ucTimToRtcSwitch需在eqEquipPara.ucLowFreqSta之前设置，防止定时中断在二者之间触发
                    				
                    /*进入到省电状态*/
    				eqEquipPara.ucLowFreqSta = EQ_FREQSTA_LOW;
#endif                                                    
 
                }	
    		}		
		}	
	}				
}
#endif

#if 0
/*******************************************************************************
* 名称  : SleepCtrlLowFreq
* 描述  : 省电状态下省电控制
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
extern void EnterStopMod(void);
extern void Tim2NVIC_Close(void);
extern void Tim2NVIC_Open(void);
void SleepCtrlLowFreq(void)
{	
	U8  ucSosKeyVal;
	U8  ucPowerKeyVal;
    U8  ucGpsPowerSta;
    U8  ucMsgWakeupFlg;
    U8  ucMcuWakeupFlg;
    U8  ucWakeupChkFlg;
    U8  ucUsartWakeupFlg;

    while(SleepCtrlPara.ucTimToRtcSwitch == TRUE);

	EnterStopMod();                                     //这里就是进入clock stop mode

	/*lq 查询低功耗模式转入工作模式的条件是否成立*/
#if CONFIG_KEY
    ucPowerKeyVal = ReadSingleKeyValProcess(&PowerKey); //lq 电源按键
	ucSosKeyVal = ReadSingleKeyValProcess(&SOSKey);     //lq Sos按键
#endif
#if 0
    ucMsgWakeupFlg = GetMsgWakeupState();               //lq GPS唤醒定时到
#endif    
    ucMcuWakeupFlg = SleepCtrlPara.ucMcuWakeupFlg;      //lq MCU唤醒定时到
    ucUsartWakeupFlg = GetUsartFlg();                   //lq USART唤醒（唤醒后需要间隔500ms左右才能正确接收第一条消息）

    ucWakeupChkFlg = (ucMcuWakeupFlg == TRUE)
#if CONFIG_KEY     
                     || (ucSosKeyVal == KEY_LOW)
                     || (ucPowerKeyVal == KEY_LOW)
#endif
#if 0                     
                     || (ucMsgWakeupFlg == TRUE) 
#endif     
                     
                     || (ucUsartWakeupFlg == TRUE);

	if (ucWakeupChkFlg)
	{
#if 0	
    	if (SleepCtrlPara.ucGpsPulseFlg == TRUE)
        {
            SleepCtrlPara.ucGpsPulseFlg = FALSE; 
		    usart_printf("[%6d:%4d]$MCU Wakeup by GpsPulse!\r\n", GetFrameNum(), GetSlotNum());	                
        }
#endif
#if 0
        if (ucMcuWakeupFlg == TRUE)
        { 
		    usart_printf("[%6d:%4d]$MCU Wakeup by McuWakeupTim!\r\n", GetFrameNum(), GetSlotNum());	                
        }
#if CONFIG_KEY
        if (ucSosKeyVal == KEY_LOW)
        {
		    usart_printf("[%6d:%4d]$MCU Wakeup by SosKey!\r\n", GetFrameNum(), GetSlotNum());	                
        }

        if (ucPowerKeyVal == KEY_LOW)
        {
		    usart_printf("[%6d:%4d]$MCU Wakeup by PowerKey!\r\n", GetFrameNum(), GetSlotNum());	                
        }
#endif
        if (ucMsgWakeupFlg == TRUE)
        {
		    usart_printf("[%6d:%4d]$MCU Wakeup by MsgWakeupForGpsOn!\r\n", GetFrameNum(), GetSlotNum());	                
        }
#endif                
#if 1
        gslRtcToTimCnt_2 = 0;
#endif

        /*相位计数由RTC定时转为Tim定时*/
        SleepCtrlPara.ucRtcToTimSwitch = TRUE;         //lq SleepCtrlPara.ucRtcToTimSwitch需在eqEquipPara.ucLowFreqSta之前设置，防止定时中断在二者之间触发

        eqEquipPara.ucLowFreqSta = EQ_FREQSTA_HIG;

		// usart_printf("[%6d:%4d]$MCU WorkMode : Running!\r\n", GetFrameNum(), GetSlotNum());	                
	}		
}
#endif

/*******************************************************************************
* 名称  : SaveBKP_DRPara
* 描述  : 保存备份寄存器参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SaveBKP_DRPara(void)
{
#if 0
    /*lq 标志设置*/
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_GPS_LNG_OPEN_FIRST_HOUR_INDEX,
                    GpsCtrlPara.ucGpsUnfix1hLngOpenFlg);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_GPS_LNG_OPEN_BEY_FIRST_HOUR_INDEX,
                    GpsCtrlPara.ucGpsUnfixBey1hLngOpenFlg);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_GPS_5MIN_LONG_OPEN_EN_INDEX,
                    GpsCtrlPara.ucGps5minLngOpenEn);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_GPS_POWER_UP_FIX_INDEX,
                    GpsCtrlPara.GpsPowerupFix);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_GPS_ON_TIM_EN_INDEX,
                    GpsCtrlPara.ucGpsOnTimEn);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_GPS_OFF_TIM_EN_INDEX,
                    GpsCtrlPara.ucGpsOffTimEn);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_GPS_SAVE_INDEX,
                    GpsCtrlPara.ucGpsSaveFlg);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_GPS_ON_INDEX,
                    GpsCtrlPara.ucGpsOnEn);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_GPS_LEAP_SECOND_INDEX,
                    GpsPara.GPS_LeapSecondFlg);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_RTC_LSE_OR_LSI_INDEX,
                    struRtcPara.ucRtcLSEorLSI);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_RTC_TIM_PHASE_INDEX,
                    struRtcTimPhase1.RtcTimPhaseFlg);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_GPS_SYNCH_ENABLE_INDEX,
                    GpsSynCell.GPSSyn_Enable);
    SetBKPDRDataBit(BKPR_FLAG_INDEX, BKPR_FLAG_BIT_EQ_WORKMOD_INDEX,
                    eqEquipPara.ucWorkMode);                    

    arrBKPDRData[BKPR_GPS_LATITUDE_H_INDEX] = (GpsPara.GPS_Latitude >> 16) & 0xFFFF;
    arrBKPDRData[BKPR_GPS_LATITUDE_L_INDEX] = GpsPara.GPS_Latitude & 0xFFFF;
    arrBKPDRData[BKPR_GPS_LONGITUDE_H_INDEX] = (GpsPara.GPS_Longitud >> 16) & 0xFFFF;;                   
    arrBKPDRData[BKPR_GPS_LONGITUDE_L_INDEX] = GpsPara.GPS_Longitud & 0xFFFF;
    arrBKPDRData[BKPR_GPS_SOG_INDEX] = GpsPara.GPS_Sog;
    arrBKPDRData[BKPR_GPS_COG_INDEX] = GpsPara.GPS_Cog;
    //arrBKPDRData[BKPR_GPS_TIME_HOUR_INDEX] = GpsPara.GPS_TimeH;
    //arrBKPDRData[BKPR_GPS_TIME_MIN_INDEX] = GpsPara.GPS_TimeM;

    arrBKPDRData[BKPR_GPS_FALL_COUNT_INDEX] = GpsCtrlPara.GpsFallCount;
    arrBKPDRData[BKPR_GPS_CTRL_STA_INDEX] = GpsCtrlPara.GpsCtrlSta; 
    arrBKPDRData[BKPR_GPS_UNFIX_1H_PERIOD_TIM_FRAME_H_INDEX] = (GpsCtrlPara.GpsUnfix1hPeriodTimFrame >> 16) & 0xFFFF;
    arrBKPDRData[BKPR_GPS_UNFIX_1H_PERIOD_TIM_FRAME_L_INDEX] = GpsCtrlPara.GpsUnfix1hPeriodTimFrame & 0xFFFF;
    arrBKPDRData[BKPR_GPS_UNFIX_1H_PERIOD_TIM_SLOT_INDEX] = GpsCtrlPara.GpsUnfix1hPeriodTimSlot;
    arrBKPDRData[BKPR_GPS_START_FRAME_H_INDEX] = (GpsCtrlPara.GpsOnTimFrame >> 16) & 0xFFFF;
    arrBKPDRData[BKPR_GPS_START_FRAME_L_INDEX] = GpsCtrlPara.GpsOnTimFrame & 0xFFFF;
    arrBKPDRData[BKPR_GPS_START_SLOT_INDEX] = GpsCtrlPara.GpsOnTimSlot;
    arrBKPDRData[BKPR_GPS_OFF_FRAME_H_INDEX] = (GpsCtrlPara.GpsOffTimFrame >> 16) & 0xFFFF;
    arrBKPDRData[BKPR_GPS_OFF_FRAME_L_INDEX] = GpsCtrlPara.GpsOffTimFrame & 0xFFFF;
    arrBKPDRData[BKPR_GPS_OFF_SLOT_INDEX] = GpsCtrlPara.GpsOffTimSlot;                                  
#if 0
    arrBKPDRData[BKPR_BURST_TX_TIM_FRAME_H_INDEX] = (struSOTDMAPara.BurstTxTimFrame >> 16) & 0xFFFF;
    arrBKPDRData[BKPR_BURST_TX_TIM_FRAME_L_INDEX] = struSOTDMAPara.BurstTxTimFrame & 0xFFFF;
    arrBKPDRData[BKPR_BURST_TX_TIM_SLOT_INDEX] = struSOTDMAPara.BurstTxTimSlot; 
#endif
#if 0
    arrBKPDRData[BKPR_SLOT_TIMEOUT_INDEX] = struSOCommState.ucSlotTimeout;
    arrBKPDRData[BKPR_SLOT_OFFSET_INDEX] = struSOCommState.usSlotOffset;
#endif
    arrBKPDRData[BKPR_RTC_TIM_PHASE_H_INDEX] = (struRtcTimPhase1.RtcTimPhaseNum >> 16) & 0xFFFF;
    arrBKPDRData[BKPR_RTC_TIM_PHASE_L_INDEX] = struRtcTimPhase1.RtcTimPhaseNum & 0xFFFF;

    arrBKPDRData[BKPR_RTC_FREQUENCE_INDEX] = struRtcPara.usRtcFreq;
    arrBKPDRData[BKPR_RTC_ALARM_MCU_OR_GPS_INDEX] = struRtcPara.ucRtcAlarmMcuOrGps;
    // arrBKPDRData[BKPR_RTC_ALARM_VALUE_INDEX] = struRtcPara.ssRtcAlarmValue;
    arrBKPDRData[BKPR_RTC_ALARM_VALUE_H_INDEX] = (struRtcPara.ssRtcAlarmValue >> 16) & 0xFFFF;
    arrBKPDRData[BKPR_RTC_ALARM_VALUE_L_INDEX] = struRtcPara.ssRtcAlarmValue & 0xFFFF;

    arrBKPDRData[BKPR_FRAME_CNT_H_INDEX] = (gslFrameCnt >> 16) & 0xFFFF;
    arrBKPDRData[BKPR_FRAME_CNT_L_INDEX] = gslFrameCnt & 0xFFFF;
	arrBKPDRData[BKPR_SLOT_CNT_INDEX] = gsiSlotCnt;
	arrBKPDRData[BKPR_PHASE_CNT_INDEX] = gsiPhaseCnt;

    arrBKPDRData[BKPR_TIM_ADJUST_1_INDEX] = TimAdjust1;
    arrBKPDRData[BKPR_TIM_ADJUST_10_INDEX] = TimAdjust10; 
    arrBKPDRData[BKPR_TIM_ADJUST_100_INDEX] = TimAdjust100;

    arrBKPDRData[BKPR_TIM_ADJUST_PERIOD_CNT_LAST_INDEX] = TimAdjustPara.PeriodCountLast; 

    // arrBKPDRData[BKPR_RTC_ADJUST_1_INDEX] = RtcPhaseAdjust1;
    // arrBKPDRData[BKPR_RTC_ADJUST_10_INDEX] = RtcPhaseAdjust10;
    // arrBKPDRData[BKPR_RTC_ADJUST_100_INDEX] = RtcPhaseAdjust100;
    arrBKPDRData[BKPR_GPS_INTERVAL_CNT_INDEX] = GpsCtrlPara.GpsIntervalCnt;

    arrBKPDRData[BKPR_RAND_SEED_H_INDEX] = (send_seed.seed >> 16) & 0xFFFF;
    arrBKPDRData[BKPR_RAND_SEED_L_INDEX] = send_seed.seed & 0xFFFF;
#endif
}

#if 0
/*******************************************************************************
* 名称  : GetSleepSpan
* 描述  : 计算休眠时长（根据时隙差）
* 输入  : 无		  
* 输出  : 无
* 返回  : RTC定时计数值
*******************************************************************************/
U32 CalcuSleepSpan(void)
{
    S32 slCurrSlotSum;
    S32 slGpsOnslot;
    S32 slSlotDiff;
    S32 slAlarmValue;

	if (IsRtcClockSourceLSE() == TRUE)
	{
		//LSE
		struRtcPara.usRtcFreq = RTC_LSE_FREQ_VALUE;
//lq        struRtcPara.usRtcFreq = 64;
	}
	else
	{
		//LSI
		struRtcPara.usRtcFreq = RTC_LSI_FREQ_VALUE;
//lq        struRtcPara.usRtcFreq = 100;
	}
    
    slSlotDiff = CalcuSleepSlots_Sleep();
    if (slSlotDiff < 0)
    {   
       slSlotDiff = 0; 
    }
    usart_printf("slSlotDiff : %ld\r\n",slSlotDiff);
	usart_printf("slRTCFreq : %d\r\n",struRtcPara.usRtcFreq);

    slAlarmValue = slSlotDiff * struRtcPara.usRtcFreq / 37.5 + 0.5;

    return slAlarmValue;
}

#else
/*******************************************************************************
* 名称  : GetSleepSpan
* 描述  : 计算休眠时长（根据相位差）
* 输入  : 无		  
* 输出  : 无
* 返回  : RTC定时计数值
*******************************************************************************/
U32 CalcuSleepSpan(void)
{
    S32 slCurrSlotSum;
    S32 slGpsOnslot;
    S32 slPhaseDiff;
    S32 slAlarmValue;


	if (IsRtcClockSourceLSE() == TRUE)
	{
		//LSE
		struRtcPara.usRtcFreq = RTC_LSE_FREQ_VALUE;
//lq        struRtcPara.usRtcFreq = 64;
	}
	else
	{
		//LSI
		struRtcPara.usRtcFreq = RTC_LSI_FREQ_VALUE;
//lq        struRtcPara.usRtcFreq = 100;
	}
   
    slPhaseDiff = CalcuSleepPhases_Sleep();
    if (slPhaseDiff < 0)
    {   
       slPhaseDiff = 0; 
    }
    // usart_printf("slPhaseDiff : %ld\r\n", slPhaseDiff);
	// usart_printf("slRTCFreq : %d\r\n", struRtcPara.usRtcFreq);
#if 0
    slAlarmValue = slPhaseDiff * struRtcPara.usRtcFreq / 48000 + 0.5;
#else
    slAlarmValue = (double)slPhaseDiff * ((double)struRtcPara.usRtcFreq / 48000.0) + 1;    //lq +1确保定时唤醒的时刻大于定时的时刻防止唤醒后未到真正的唤醒时刻再次进入休眠

    slAlarmValue = ((slAlarmValue >> 2) + 1) << 2;  //lq 将RTC的alarm定时值转换为不小于其原始值的4的倍数，以减小待机唤醒后转换为相位时的误差
#endif
    return slAlarmValue;
}

#endif

/*******************************************************************************
* 名称  : CalcuSleepSlots_Sleep
* 描述  : MCU唤醒定时是否超时
* 输入  : 无		  
* 输出  : 无
* 返回  : TRUE-超时；FALSE-未超时
*******************************************************************************/
S32 CalcuSleepSlots_Sleep(void)
{
    S32 slGpsWakeupSlot;
    S32 slMcuWakeupSlot;
    S32 slSleepSlot;
    S32 slCurrSlot;

    /*lq 获取当前时隙*/
    slCurrSlot = GetSlotSum();

    /*lq 获取MCU唤醒定时*/
    slMcuWakeupSlot = GetMcuWakeupTim();

    /*lq 获取GPS开启定时*/
    if (GetGpsOnTimEn() == TRUE)
    {
        slGpsWakeupSlot = GetGpsOnTim();    
    }
    else
    {
        slGpsWakeupSlot = 0;
    }

    /*lq 使用MCU唤醒定时将MCU从待机中唤醒*/
    if (slMcuWakeupSlot <= slGpsWakeupSlot)
    {
       slSleepSlot = slMcuWakeupSlot - slCurrSlot;

       struRtcPara.ucRtcAlarmMcuOrGps = RTC_ALARM_MCU;
    }
    else
    {
       slSleepSlot = slGpsWakeupSlot - slCurrSlot;

       struRtcPara.ucRtcAlarmMcuOrGps = RTC_ALARM_GPS;    
    }

    return slSleepSlot;
} 

/*******************************************************************************
* 名称  : CalcuSleepPhases_Sleep
* 描述  : 计算休眠的时隙数
* 输入  : 无		  
* 输出  : 无
* 返回  : TRUE-超时；FALSE-未超时
*******************************************************************************/
S32 CalcuSleepPhases_Sleep(void)
{
    S32 slGpsWakeupSlot;
    S32 slMcuWakeupSlot;
    S32 slSleepSlot;
    S32 slCurrSlot;

    /*lq 获取当前时隙*/
    slCurrSlot = GetSlotSum();

    /*lq 获取MCU唤醒定时*/
    slMcuWakeupSlot = GetMcuWakeupTim();

    /*lq 获取GPS开启定时*/
    if (GetGpsOnTimEn() == TRUE)
    {
        slGpsWakeupSlot = GetGpsOnTim();    
    }
    else
    {
        //lq slGpsWakeupSlot = 0;
        slGpsWakeupSlot = 0x7FFFFFFF;
    }

    /*lq 使用MCU唤醒定时将MCU从待机中唤醒*/
    if (slMcuWakeupSlot <= slGpsWakeupSlot)
    {
       slSleepSlot = slMcuWakeupSlot - slCurrSlot;

       struRtcPara.ucRtcAlarmMcuOrGps = RTC_ALARM_MCU;
    }
    else
    {
       slSleepSlot = slGpsWakeupSlot - slCurrSlot;

       struRtcPara.ucRtcAlarmMcuOrGps = RTC_ALARM_GPS;    
    }

    return slSleepSlot * 1280 - GetPhaseNum();              //lq 返回待机相位数，在整时隙点待机唤醒
} 

/*******************************************************************************
* 名称  : IsMcuWakeupTimeout
* 描述  : MCU唤醒定时是否超时
* 输入  : 无		  
* 输出  : 无
* 返回  : TRUE-超时；FALSE-未超时
*******************************************************************************/
U8 IsMcuWakeupTimeout(void)
{
    if (GetSlotSum() >= GetMcuWakeupTim())
    {
        return TRUE;
    }    
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : GetMcuWakeupTim
* 描述  : 获取MCU定时
* 输入  : 无		  
* 输出  : 无
* 返回  : MCU定时
*******************************************************************************/
S32 GetMcuWakeupTim(void)
{
    return SleepCtrlPara.WakeUpSlot + 2250 * SleepCtrlPara.WakeUpFrame;
}

/*******************************************************************************
* 名称  : SetMcuWakeupSlot
* 描述  : 设置MCU唤醒时隙
* 输入  : value - MCU唤醒时隙		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetMcuWakeupSlot(S16 value)
{
   if (value < 0)
   {
        value = GetSlotNum();
   }
   
   SleepCtrlPara.WakeUpSlot = value;
}

/*******************************************************************************
* 名称  : SetMcuWakeupFrame
* 描述  : 设置MCU唤醒帧
* 输入  : value - MCU唤醒帧		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetMcuWakeupFrame(S32 value)
{
   if (value < 0)
   {
        value = GetFrameNum() + 1;
   }
   
   SleepCtrlPara.WakeUpFrame = value;
}
#if 0
/*******************************************************************************
* Function Name  : SetMcuWakeupTim
* Description    : 设置MCU唤醒定时
* Input          : usValue - MCU唤醒定时至组消息发射的时隙偏差
* Output         : None
* Return         : None
*******************************************************************************/
void SetMcuWakeupTim(S16 usValue)
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

    SetMcuWakeupFrame(slSlotNums / 2250);
    SetMcuWakeupSlot(slSlotNums % 2250);
}
#endif
/*******************************************************************************
* Function Name  : SetUsartFlg
* Description    : 设置Usart唤醒标志
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetUsartFlg(void)
{
    SleepCtrlPara.ucUsartFlg = TRUE;

    SleepCtrlPara.TimeCnt1 = SetTimePeriodProcess(SleepCtrlPara.TimePeriod1);
}

/*******************************************************************************
* Function Name  : ResetUsartFlg
* Description    : 复位Usart唤醒标志
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ResetUsartFlg(void)
{
    SleepCtrlPara.ucUsartFlg = FALSE;
}

/*******************************************************************************
* Function Name  : GetUsartFlg
* Description    : 查询Usart唤醒标志
* Input          : None
* Output         : None
* Return         : Usart唤醒标志
*******************************************************************************/
U8 GetUsartFlg(void)
{
    return SleepCtrlPara.ucUsartFlg;
}

/*******************************************************************************
* Function Name  : GetUsartFlgRst
* Description    : 查询Usart唤醒标志
* Input          : None
* Output         : None
* Return         : Usart唤醒标志
*******************************************************************************/
U8 GetUsartFlgRst(void)
{
    if (SleepCtrlPara.ucUsartFlg == TRUE)
    {
        SleepCtrlPara.ucUsartFlg = FALSE;    
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: gpsmsg.c
  Author: vozhj      Version : V1.00        Date: 2012.03.21
  Description:    GPS报文处理模块
  Version:         V1.00
  Function List:   // 主要函数及其功能
  History:         // 历史修改记录
      <author>  <time>   	<version >   <desc>

*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "def.h"
#include "initial.h"
#if !WIN32
#include "debug.h"
#include "common.h"
#include "ioctrl.h"
#endif
#include "gpsctrl.h"
#include "uartmsg.h"
#include "systemtick.h"
#include "gps.h"
#include "ledctrl.h"
#include "mmi.h"
#include "parastorage.h"
#include "adjust.h"
#include "AIS_MsgModu.h"
#include "LED.h"
#include "Tim.h"
#include "SOTDMA.h"
#include "powerup.h"
#include "tempersensor.h"
#include "config.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
GPSStruct 		GpsPara;
GPSCtrlStruct 	GpsCtrlPara;
GPSSynStruct 	GpsSynCell;	   		//GPS同步操作结构体
GPSCtrlMsgTestStruct GPSCtrlMsgTestPara;
GpsSelfCheckStruct GpsSelfCheckInst; 
U16 usGPSSynContinueTime = 61;		   //两次GPS定位的时间间隔
double LastGPS_Latitude;				   //上一次GPS定纬度
double LastGPS_Longitud;				   //上一次GPS定经度
double NowGPS_Latitude;				   //本次GPS定纬度
double NowGPS_Longitud;				   //本次GPS定经度

/* Private function prototypes -----------------------------------------------*/
//U8 gps_msg_enable;
void SlotSynClac(void);

/*******************************************************************************
* 名称  : IniGpsCtrlMsgTestStru
* 描述  : 初始化消息测试模式下GPS控制结构体参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void IniGpsCtrlMsgTestStru(GPSCtrlMsgTestStruct *pGPSCtrlMsgTestCell)
{
    pGPSCtrlMsgTestCell->TimeCnt = 0;
    pGPSCtrlMsgTestCell->TimePeriod = GPS_CTRL_1MINPEROID;
    pGPSCtrlMsgTestCell->Time1mCnt = 0;
    pGPSCtrlMsgTestCell->GpsPowerupFix = FALSE;
    pGPSCtrlMsgTestCell->GpsCtrlSta = GPS_CTRL_STA_MSG_TEST_START;
}

/*******************************************************************************
* 名称  : IniGpsMsgStru
* 描述  : 初始化GPS串口通信必要的参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void IniGpsMsgStru(GPSStruct *pGpsCell)
{
	U8 i;

	for(i = 0; i < 6; i++)
	{
		pGpsCell->GPSTimeBuf[i] = '0';
	}

	//lq pGpsCell->GPS_Cog = 0;
    pGpsCell->GPS_Cog = 0xE10;                                                  //lq 360°        
	pGpsCell->GPS_Lati100 = 0;
	//lq pGpsCell->GPS_Latitude = 0;
	pGpsCell->GPS_Latitude = 0x3412140;                                         //lq 91°
	pGpsCell->GPS_Long100 = 0;
	//lq pGpsCell->GPS_Longitud = 0;
	pGpsCell->GPS_Longitud = 0x6791AC0;                                         //lq 181°
	pGpsCell->GPS_LatiD = 0;
	//lq pGpsCell->GPS_Sog = 0;
    pGpsCell->GPS_Sog = 0x3FF;                                                  //lq 102.3knot            
	pGpsCell->GPS_Stat = 0;
	pGpsCell->GPS_TimeM = 0;
	pGpsCell->GPS_TimeS = 0;
	pGpsCell->GPS_errlat = GPS_OUTRANGE;
	pGpsCell->GPS_errlon = GPS_OUTRANGE;
	pGpsCell->GPS_PosPrec = GPS_OUTRANGE;
	pGpsCell->GPS_QualityIndicator = 0;
    pGpsCell->GPS_FixMode = 1;
    pGpsCell->GPS_PDOP = GPS_DATA_PDOP_DEFAULT;
    pGpsCell->GPS_RAIMFlag = FALSE;
    pGpsCell->GPS_Sog_Index = 0;

	for(i=0; i<18; i++)
	{
		pGpsCell->GPS_Sog_Buf[i] = 0;
	}
	for (i=0; i<4; i++)
	{
		pGpsCell->GPS_Longitud_Buf[i] = pGpsCell->GPS_Longitud >> 2;
		pGpsCell->GPS_Latitude_Buf[i] = pGpsCell->GPS_Latitude >> 2;
	}
	pGpsCell->GPS_Longitud_Buf_Index = 0;
	pGpsCell->GPS_Latitude_Buf_Index = 0;

    pGpsCell->GPS_LonLatFilterFlg = FALSE;                                      
	pGpsCell->GPS_LatLonCnt = 0;

	pGpsCell->GPS_Latitude_Last = 0;
	pGpsCell->GPS_Longitud_Last = 0;
	pGpsCell->GPS_TimeD_Last = 0;
	pGpsCell->GPS_TimeH_Last = 0;
	pGpsCell->GPS_TimeM_Last = 60;
    pGpsCell->GPS_TimeS_Last = 0;
    pGpsCell->GPS_Sog_Last = 0;   
    pGpsCell->GPS_FirstLLFlg = FALSE; 
    
	pGpsCell->GPS_LeapSecondFlg = FALSE;
    pGpsCell->GPS_TimeHLeapLast = 60;
	pGpsCell->GPS_TimeMLeapLast = 60;
	pGpsCell->GPS_TimeSLeapLast = 60;    		    	
    pGpsCell->GPS_TimeSLeap = 60;

    pGpsCell->GPS_PPSGenFlag = FALSE;
    pGpsCell->GPS_MsgGenFlag = FALSE;
}

/*******************************************************************************
* 名称  : ResetGpsMsgStru
* 描述  : 初始化GPS串口通信必要的参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ResetGpsMsgStru(GPSStruct *pGpsCell)
{
	U8 i;
	
	pGpsCell->GPS_Stat = 0;
	pGpsCell->GPS_errlat = GPS_OUTRANGE;
	pGpsCell->GPS_errlon = GPS_OUTRANGE;
	pGpsCell->GPS_PosPrec = GPS_OUTRANGE;
	pGpsCell->GPS_QualityIndicator = 0;
    pGpsCell->GPS_FixMode = 1; 
    pGpsCell->GPS_PDOP = GPS_DATA_PDOP_DEFAULT;       
    pGpsCell->GPS_RAIMFlag = FALSE;
    
	pGpsCell->GPS_Sog_Index = 0;

	for(i=0; i<18; i++)
	{
		pGpsCell->GPS_Sog_Buf[i] = 0;
	}

    pGpsCell->GPS_LonLatFilterFlg = FALSE;                                         
	pGpsCell->GPS_LatLonCnt = 0;

    pGpsCell->GPS_PPSGenFlag = FALSE;
    pGpsCell->GPS_MsgGenFlag = FALSE;
}

/*******************************************************************************
* 名称  : InitGpsSynStru
* 描述  : 初始化GPS同步参数
* 输入  : GPSSynStruct类型指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitGpsSynStru(GPSSynStruct *pGpsSynStru)
{
	pGpsSynStru->GPSSyn_PulsePhase = 0;	//接收到秒脉冲时的时隙相位
	pGpsSynStru->GPSSyn_PhaseOffset = 0;	//时隙同步时相位偏移
    pGpsSynStru->GPSSyn_PulseSlot = 0;
	pGpsSynStru->GPSSyn_SlotOffset = 0;	//时隙同步时隙偏移量
	pGpsSynStru->GPSSyn_SynLevel = 0;		//同步优先级
	pGpsSynStru->GPSSyn_PhaseSynFlag = FALSE;	//相位同步标志
	pGpsSynStru->GPSSyn_SlotSynFlag = FALSE;	//相位同步标志
	pGpsSynStru->GPSSyn_ProtectCnt = 0;	//相位同步保护时间
	pGpsSynStru->GPSSyn_PulseEn = FALSE;		//秒脉冲有效标志
	pGpsSynStru->GPSSyn_Flag = FALSE;			//GPS同步有效标志
	pGpsSynStru->GPSSyn_SlotCnt = 0;
	pGpsSynStru->GPSSyn_SlotEn = FALSE;
	pGpsSynStru->GPSSyn_PulseMin = 0;
	pGpsSynStru->GPSSyn_MinOffset = 0;

	pGpsSynStru->ChkTimeS = 0;
	pGpsSynStru->ChkTimeM = 0;
	pGpsSynStru->ChkTimeH = 0;
    pGpsSynStru->ChkTimeFrame = 0;
    pGpsSynStru->ucGPSSyn_SlotPhaseSynFlag = FALSE;
    //lq pGpsSynStru->GPSSyn_FirstSynFlag = FALSE; 
    pGpsSynStru->GPSSyn_Enable = TRUE;
#if CONFIG_GPS_SYNCH_HOUR_MIN   
    pGpsSynStru->GPSSyn_HourFrameSynEnable = TRUE;
    pGpsSynStru->ucGPSSyn_HourFrameSynFlag = FALSE;                                              //lq 小时、分钟同步
	pGpsSynStru->GPSSyn_PulseHour = 0;    
    pGpsSynStru->GPSSyn_HourOffset = 0;	//lq 小时偏移
#endif
}	

/*******************************************************************************
* 名称  : IniGpsCtrlStru
* 描述  : 初始化控制参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void IniGpsCtrlStru(GPSCtrlStruct *pGpsCtrlCell)
{
	pGpsCtrlCell->TimePeriod = GPS_CTRL_TIME_PERIOD_1S;
	pGpsCtrlCell->TimeCnt = SetTimePeriodProcess(pGpsCtrlCell->TimePeriod);
	pGpsCtrlCell->Timeout = 0;	
	pGpsCtrlCell->TimeoutCnt = 0;	
	pGpsCtrlCell->Time1mCnt = 0;

    pGpsCtrlCell->GpsUnfix1hPeriodTimSlot = 0;
    pGpsCtrlCell->GpsUnfix1hPeriodTimFrame = 0;

    pGpsCtrlCell->GpsOnTimSlot = 0;
    pGpsCtrlCell->GpsOnTimFrame = 0; 
    pGpsCtrlCell->GpsOffTimSlot = 0;
    pGpsCtrlCell->GpsOffTimFrame = 0;    
    pGpsCtrlCell->ucGpsOnTimEn = FALSE;
    pGpsCtrlCell->ucGpsOffTimEn = FALSE;

	pGpsCtrlCell->GpsPowerSta = GPS_CTRL_GPSPOWERDOWN;
	pGpsCtrlCell->GpsCtrlSta = GPS_CTRL_STA_START;
	pGpsCtrlCell->GPSSyn_Flag = FALSE;

	pGpsCtrlCell->GpsPowerupFix = FALSE;
	pGpsCtrlCell->GpsNormalFix = FALSE;
	pGpsCtrlCell->SaveOpenTime = FALSE;

	pGpsCtrlCell->GpsSynCount = 0;

	pGpsCtrlCell->GpsFixCount = 0;
	pGpsCtrlCell->GpsFallCount = 0;
	pGpsCtrlCell->GpsWaitCount = 0;
	pGpsCtrlCell->GpsWaitFix = FALSE;

    pGpsCtrlCell->ucGpsUnfix1hLngOpenFlg = FALSE;
    pGpsCtrlCell->ucGpsUnfixBey1hLngOpenFlg = FALSE;
    pGpsCtrlCell->ucGps5minLngOpenEn = TRUE;
    pGpsCtrlCell->ssGps5minLngOpenCnt = 0;
    pGpsCtrlCell->GpsIntervalCnt = 0;    

    pGpsCtrlCell->ucGpsSaveFlg = FALSE;
    pGpsCtrlCell->ucGpsOnEn = TRUE;
    pGpsCtrlCell->GpsSaveJudgeTimePeriod = GPS_SAVE_JUDGE_PERIOD;
    pGpsCtrlCell->GpsSaveJudgeTimeCnt = SetTimePeriodProcess(pGpsCtrlCell->GpsSaveJudgeTimePeriod);
	pGpsCtrlCell->GpsSaveJudgeCntWorkTime = 0;
	pGpsCtrlCell->GpsSaveJudgeCntBatVolt = 0;	
	pGpsCtrlCell->GpsSaveJudgeCntTemper = 0;
	pGpsCtrlCell->GpsSaveJudgeFlgWorkTime = FALSE;
	pGpsCtrlCell->GpsSaveJudgeFlgBatVolt = FALSE;	
	pGpsCtrlCell->GpsSaveJudgeCFlgTemper = FALSE;

    pGpsCtrlCell->GpsOffPositionSampleEn = TRUE;
    pGpsCtrlCell->GpsPositionSampleNewFlg = FALSE;
}

/*******************************************************************************
* 名称  : IniGpsCtrlStru
* 描述  : 初始化控制参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ResetGpsCtrlStru(GPSCtrlStruct *pGpsCtrlCell)
{
	pGpsCtrlCell->TimeCnt = 0;
	pGpsCtrlCell->TimePeriod = GPS_CTRL_TIME_PERIOD_1S;
	pGpsCtrlCell->Timeout = 0;	
	pGpsCtrlCell->TimeoutCnt = 0;	
	pGpsCtrlCell->Time1mCnt = 0;

	pGpsCtrlCell->GpsPowerSta = GPS_CTRL_GPSPOWERUP;

	pGpsCtrlCell->GPSSyn_Flag = FALSE;
	pGpsCtrlCell->GpsNormalFix = FALSE;
    pGpsCtrlCell->ucGpsOnTimEn = FALSE;

	pGpsCtrlCell->GpsSynCount = 0;

    //lq SetGpsOffPositionSampleEn();
    pGpsCtrlCell->GpsOffPositionSampleEn = TRUE;
}

/*******************************************************************************
* 名称  : IniGpsPara
* 描述  : 初始化GPS通信必要的参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void IniGpsPara(void)
{
	IniGpsMsgStru(&GpsPara);
	IniGpsCtrlStru(&GpsCtrlPara);
    InitGpsSynStru(&GpsSynCell);

#if !WIN32
    /*lq 仅上电开机时执行*/
    if (ucStandbyWakeupFlg == FALSE)
    {    
	    IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_ON);                                     //lq 开机后立即给GPS供电
        GpsCtrlPara.GpsPowerSta = GPS_CTRL_GPSPOWERUP;
    }
#endif
}

//lqdb U8 ucGpsSecFlg = FALSE;
/*******************************************************************************
* 名称  : DealGpsMsg
* 描述  : GPS报文解析
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 DealGpsMsg(GPSStruct *pGpsCell, U8 *pDat, U16 uiLen)
{
	U8 ucCheck;
	char chTemp[30];
	U16 uiTempH, uiTempL;
	U32 ulTempH;
	U32 ulMax, ulMin;
	U16 i;
	float flTmp;
	S32 sTemp;
	S32 sTempLon, sTempLat, sTempLon_EW, sTempLat_NS;
    U8  ucLLValid;
    S8  scNS, scEW;
    U16 usLatBuffIndex, usLonBuffIndex; 
    double latitude_D, longitud_D, latitudeLast_D, longitudLast_D, distance;
    U32  ulTemp;
	U8 	ucLen;
    char *pEnd;

    ucLLValid = TRUE;

	//解析RMC语句
	if (strncmp((char *)pDat, "GPRMC", 5) == FALSE)
	{
        GpsSelfCheckInst.DataCommFlg = TRUE;

        /*lq GPS消息生成*/
        GpsPara.GPS_MsgGenFlag = TRUE;
		
        ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);

		if (ucCheck == FALSE)
		{
			return (FALSE);
		}

		pGpsCell->GPS_Stat = chTemp[0];

		//GPS数据有效
		if (chTemp[0] == 'A')                                     //lq 只有GPS定位后，才根据GPS的GPRMC报文更新GPS结构体
		{
			//提取时间
			ucCheck = GetSegment((char *)pDat, chTemp, 1, uiLen, 30);

			if (ucCheck == FALSE)
			{
				//return (FALSE);
			}
			else
			{
				//提取小时
				pGpsCell->GPS_TimeH = (chTemp[0] - '0') * 10 + (chTemp[1] - '0');
				
				//提取分钟
				pGpsCell->GPS_TimeM = (chTemp[2] - '0') * 10 + (chTemp[3] - '0');
				
				//提取秒
				pGpsCell->GPS_TimeS = (chTemp[4] - '0') * 10 + (chTemp[5] - '0');

				/*lq 保存时间用于判断是否出现闰秒*/
                if (pGpsCell->GPS_LeapSecondFlg == FALSE)
                {
    				/*lq 闰秒出现标志*/
                    if (pGpsCell->GPS_TimeHLeapLast == pGpsCell->GPS_TimeH
                        && pGpsCell->GPS_TimeMLeapLast == pGpsCell->GPS_TimeM
                        && pGpsCell->GPS_TimeSLeapLast == pGpsCell->GPS_TimeS)
                    {
                        pGpsCell->GPS_LeapSecondFlg = TRUE;
                        
                        /*lq 闰秒出现后使用GPS提供的时间进行同步*/
                        pGpsCell->GPS_TimeSLeap = pGpsCell->GPS_TimeS;
                    }
                    else
                    {
                        pGpsCell->GPS_TimeHLeapLast = pGpsCell->GPS_TimeH;
        				pGpsCell->GPS_TimeMLeapLast = pGpsCell->GPS_TimeM;
        				pGpsCell->GPS_TimeSLeapLast = pGpsCell->GPS_TimeS;

                        /*lq 在闰秒出现之前，人为减1s，并使用修正后的值用于同步*/
                        pGpsCell->GPS_TimeSLeap = pGpsCell->GPS_TimeS == 0 ? 59 : pGpsCell->GPS_TimeS - 1;
                    }
                }
                else
                {
                    /*lq 闰秒出现后使用GPS提供的时间进行同步*/
                    pGpsCell->GPS_TimeSLeap = pGpsCell->GPS_TimeS;
                }

#if 0 //lqdb
                if (ucGpsSecFlg == TRUE)
                {
                    TEST_OUT_LOW_2();
                    ucGpsSecFlg = FALSE;
                }
                else
                {
                    TEST_OUT_HIGH_2();
                    ucGpsSecFlg = TRUE;
                }
#endif
//lq                SlotSynClac();
			}

			//提取纬度
			ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

			if (ucCheck == FALSE)
			{
				//return (FALSE);
                ucLLValid = FALSE;
			}
			else
			{
				uiTempH = (chTemp[0] - '0') * 10 + (chTemp[1] - '0');
				uiTempL = (chTemp[2] - '0') * 10 + (chTemp[3] - '0');			
	
				pGpsCell->GPS_LatiD = uiTempH;
	
				ulTempH = (U32)uiTempH * 60 + uiTempL;
	
				uiTempL = (chTemp[5] - '0') * 1000;
				uiTempL += (chTemp[6] - '0') * 100;
				uiTempL += (chTemp[7] - '0') * 10;
				uiTempL += (chTemp[8] - '0');
	
	            sTempLat = ulTempH * 10000 + uiTempL;
             }

			ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);

			if (ucCheck == FALSE)
			{
				//return (FALSE);
                ucLLValid = FALSE;
			}
			else
			{
				if(chTemp[0] == 'S')
				{
					scNS = -1;
                    sTempLat_NS = -sTempLat;
				}
                else
                {
					scNS = 1;
                    sTempLat_NS = sTempLat;
                }
			}

			ucCheck = GetSegment((char *)pDat, chTemp, 5, uiLen, 30);

			if (ucCheck == FALSE)
			{
				//return (FALSE);
                ucLLValid = FALSE;
			}
			else
			{
				uiTempH = ((U16)(chTemp[0] - '0')) * 100 + 
								(chTemp[1] - '0') * 10 +
								(chTemp[2] - '0');
	
				uiTempL = (chTemp[3] - '0') * 10 + (chTemp[4] - '0');
				ulTempH = (U32)uiTempH * 60 + uiTempL;

				uiTempL = (chTemp[6] - '0') * 1000;
				uiTempL += (chTemp[7] - '0') * 100;
				uiTempL += (chTemp[8] - '0') * 10;
				uiTempL += (chTemp[9] - '0');
	
				sTempLon = ulTempH * 10000 + uiTempL;
            }

			ucCheck = GetSegment((char *)pDat, chTemp, 6, uiLen, 30);

			if (ucCheck == FALSE)
			{
				//return (FALSE);
                ucLLValid = FALSE;
			}
			else
			{
				if(chTemp[0] == 'W')
				{
					scEW = -1;
                    sTempLon_EW = -sTempLon;
				}
                else
                {
					scEW = 1;
                    sTempLon_EW = sTempLon;
                }
			}

            /*lq 经纬度数据存在*/
            if (ucLLValid == TRUE)
            {
    			if (pGpsCell->GPS_LonLatFilterFlg == TRUE)
                {
            		latitude_D = (double)sTempLat_NS / 600000.0;
            		longitud_D = (double)sTempLon_EW / 600000.0;
            	 	latitudeLast_D = (double)pGpsCell->GPS_Latitude / 600000.0;
            		longitudLast_D = (double)pGpsCell->GPS_Longitud / 600000.0;
            		
            		distance = GetDistance(latitude_D, longitud_D, 
                                           latitudeLast_D, longitudLast_D); 
                                                           
                    /*lq 位置突变且连续突变次数小于阈值20米，则认为数据无效；连续突变次数超过阈值认为数据有效*/
                    if (distance > 20 && pGpsCell->GPS_LatLonCnt < GPS_POSITION_BUFF_LENGTH)                                              //lq 超出范围，数据按无效处理
                    {
                        pGpsCell->GPS_LatLonCnt++;
    
                        /*lq 使用上次的定位数据*/
                        usLatBuffIndex = pGpsCell->GPS_Latitude_Buf_Index - 1 < 0
                                         ? GPS_POSITION_BUFF_LENGTH - 1 : pGpsCell->GPS_Latitude_Buf_Index - 1;
                        sTempLat = pGpsCell->GPS_Latitude_Buf[usLatBuffIndex];
    
                        usLonBuffIndex = pGpsCell->GPS_Longitud_Buf_Index - 1 < 0
                                         ? GPS_POSITION_BUFF_LENGTH - 1 : pGpsCell->GPS_Longitud_Buf_Index - 1;
                        sTempLon = pGpsCell->GPS_Longitud_Buf[usLonBuffIndex];
                    }
                    else
                    {
                        pGpsCell->GPS_LatLonCnt = 0;
    
                        /*lq 使用本次的定位数据*/
                        sTempLat = sTempLat >> 2;
                        sTempLon = sTempLon >> 2;                
                    }
                }
                else
                {
                    pGpsCell->GPS_LatLonCnt++;
                    if (pGpsCell->GPS_LatLonCnt >= GPS_POSITION_BUFF_LENGTH)
                    {
                        pGpsCell->GPS_LatLonCnt = 0;
                        pGpsCell->GPS_LonLatFilterFlg = TRUE;
                    }

                    /*lq 使用本次的定位数据*/
                    sTempLat = sTempLat >> 2;
                    sTempLon = sTempLon >> 2;   
                }
    			
                /*lq 纬度处理*/	
                //减去要挤出的数据
    			pGpsCell->GPS_Latitude = pGpsCell->GPS_Latitude - 
                                         pGpsCell->GPS_Latitude_Buf[pGpsCell->GPS_Latitude_Buf_Index];
    			
                //加上要存入的数据
    			pGpsCell->GPS_Latitude_Buf[pGpsCell->GPS_Latitude_Buf_Index++] = sTempLat;
    
    			pGpsCell->GPS_Latitude += sTempLat;
    
    			if (pGpsCell->GPS_Latitude_Buf_Index > (GPS_POSITION_BUFF_LENGTH - 1))
    			{
    				pGpsCell->GPS_Latitude_Buf_Index = 0;
    			}
    //lq			uiTempL = chTemp[5] - '0';
    //lq			pGpsCell->GPS_Lati100 = ulTempH * 10 + uiTempL;
                pGpsCell->GPS_Latitude *= scNS;
                 
    			/*lq 经度处理*/
                //减去要挤出的数据
    			pGpsCell->GPS_Longitud = pGpsCell->GPS_Longitud - 
                                         pGpsCell->GPS_Longitud_Buf[pGpsCell->GPS_Longitud_Buf_Index];
    			
                //加上要存入的数据
    			pGpsCell->GPS_Longitud_Buf[pGpsCell->GPS_Longitud_Buf_Index++] = sTempLon;
    
    			pGpsCell->GPS_Longitud += sTempLon;
    
    			if (pGpsCell->GPS_Longitud_Buf_Index > (GPS_POSITION_BUFF_LENGTH - 1))
    			{
    				pGpsCell->GPS_Longitud_Buf_Index = 0;
    			}
    //lq			uiTempL = chTemp[6] - '0';
    //lq			pGpsCell->GPS_Long100 = ulTempH * 10 + uiTempL;
    			pGpsCell->GPS_Longitud *= scEW;

                /*lq 位置偏离采样有效 */
                SetGpsPositionSampleNewFlg();
            }

			//对地航速转换
			ucCheck = GetSegment((char *)pDat, chTemp, 7, uiLen, 30);

			if (ucCheck == FALSE)
			{
				//return (FALSE);
			}
			else
			{
				flTmp = (float)atof(chTemp);
	
				pGpsCell->GPS_Sog = (U32)(flTmp * 10);                          //lq 实际航速的10倍（浮点型变为整型）
	
				//GPS sog的平均算法，消除速度突变
				pGpsCell->GPS_Sog_Buf[pGpsCell->GPS_Sog_Index++] = pGpsCell->GPS_Sog;
	
				if(pGpsCell->GPS_Sog_Index >= 18)
				{
					pGpsCell->GPS_Sog_Index = 0;
				}
	
				ulMax = 0;
				ulMin = 0x0FFFFFFF;
				ulTempH = 0;
				for(i = 0; i < 18; i++)
				{
					ulTempH += pGpsCell->GPS_Sog_Buf[i];                        //lq 求和
	
					if(pGpsCell->GPS_Sog_Buf[i] > ulMax)
					{				
						ulMax = pGpsCell->GPS_Sog_Buf[i];                       //lq 记录最大值
					}
	
					if(pGpsCell->GPS_Sog_Buf[i] < ulMin)
					{
						ulMin = pGpsCell->GPS_Sog_Buf[i];                       //lq 记录最小值
					}
				}
				ulTempH -= ulMax;
				ulTempH -= ulMin;
	
				//航速<0.3节，记为0                                             //lq 航速小于0.4节knot
				if(ulTempH <= 63)
				{
					pGpsCell->GPS_Sog = 0;	
				}
				else
				{
					pGpsCell->GPS_Sog = ulTempH >> 4;                           //lq 求平均值（/16）
				}
			}

			//对地航向转换
			ucCheck = GetSegment((char *)pDat, chTemp, 8, uiLen, 30);

			if (ucCheck == FALSE)
			{
				//return (FALSE);
			}
			else
			{
				flTmp = (float)atof(chTemp);
				pGpsCell->GPS_Cog = (U32)((flTmp + 0.05) * 10);
			}

			//年月日提取
			ucCheck = GetSegment((char *)pDat, chTemp, 9, uiLen, 30);

			if (ucCheck == FALSE)
			{
				//return (FALSE);
			}
			else
			{
				pGpsCell->GPS_TimeD = (chTemp[0] - '0') * 10 + (chTemp[1] - '0');
				pGpsCell->GPS_TimeMon = (chTemp[2] - '0') * 10 + (chTemp[3] - '0');
				pGpsCell->GPS_TimeY = (chTemp[4] - '0') * 10 + (chTemp[5] - '0');
			}
		}
        else
        {
            if (pGpsCell->GPS_LeapSecondFlg == FALSE)
            {
                pGpsCell->GPS_TimeHLeapLast = 60;
            	pGpsCell->GPS_TimeMLeapLast = 60;
            	pGpsCell->GPS_TimeSLeapLast = 60;               
            }
                    
        }
	}

    /*lq GGA语句解析*/
    else if (strncmp((char *)pDat, "GPGGA", 5) == FALSE)
    {
        /*lq GPS消息生成*/
        GpsPara.GPS_MsgGenFlag = TRUE;

		/*lq GPS quality indicator，该字段不会为空（标准规定）*/
        ucCheck = GetSegment((char *)pDat, chTemp, 6, uiLen, 30);
		if (ucCheck != FALSE)
		{
            pGpsCell->GPS_QualityIndicator = chTemp[0] - '0';    
		}           
    }

    /*lq GSA语句解析*/
    else if (strncmp((char *)pDat, "GPGSA", 5) == FALSE)
    {
        /*lq GPS消息生成*/
        GpsPara.GPS_MsgGenFlag = TRUE;

		/*lq Mode，Mode=1 (fix not available, 卫星数<3)，Mode=2 (2D, 卫星数=3)，Mode=3 (3D, 卫星数>3) */
        ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
		if (ucCheck != FALSE)
		{
            pGpsCell->GPS_FixMode = chTemp[0] - '0';    
		}
        else
        {
            pGpsCell->GPS_FixMode = 1;            
        }

		/*lq PDOP */
        ucCheck = GetSegment((char *)pDat, chTemp, 15, uiLen, 30);
		if (ucCheck != FALSE)
		{
            pGpsCell->GPS_PDOP = (U16)(atof(chTemp) * 10);
		}
        else
        {
            pGpsCell->GPS_PDOP = GPS_DATA_PDOP_DEFAULT;
        }                           
    }

    /*lq GBS语句解析*/
    else if (strncmp((char *)pDat, "GPGBS", 5) == FALSE)
    {
        /*lq GPS消息生成*/
        GpsPara.GPS_MsgGenFlag = TRUE;

	    /*lq Expected error in latitude*/
        ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
		if (ucCheck != FALSE)
		{
            pGpsCell->GPS_errlat = (U16)(atof(chTemp) + 0.5);
            pGpsCell->GPS_RAIMFlag = TRUE;    
		}
        else
        {
            pGpsCell->GPS_RAIMFlag = FALSE;            
        }

    	/*lq Expected error in longitude*/
        ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);
		if (ucCheck != FALSE)
		{
            pGpsCell->GPS_errlon = (U16)(atof(chTemp) + 0.5);
            //lq pGpsCell->GPS_RAIMFlag = TRUE; //lq 只有2和3字段均不为空时才置为TRUE                    
		}
        else
        {
            pGpsCell->GPS_RAIMFlag = FALSE;                    
        }        
    }

    /*lq TXT语句解析*/
	else if (strncmp((char *)pDat, "GPTXT", 5) == FALSE)
    {
        GpsSelfCheckInst.DataCommFlg = TRUE;

		ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);
		if (ucCheck == FALSE)
		{
			return (FALSE);
		} 

        /*lq 长度有效性判断*/
        ucLen = strlen(chTemp);
		if (ucLen != 2)
		{
			return (FALSE);
		}

        ulTemp = strtoul(chTemp, &pEnd, 10);

		/*lq 字符有效性判断*/
		if ((*pEnd) != 0)
		{
			return (FALSE);
		}
                
        if ((ulTemp == 0) || (ulTemp == 1))     //lq 00 - error, 01 - warning         
        {
            GpsSelfCheckInst.ModuleFlg = FALSE; //lq 出现一次即认为有问题
        }
        else if (ulTemp == 2)
        {   
            ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);
            if (ucCheck == FALSE)
            {
                return (FALSE);
            } 
                        
			if (strncmp(chTemp, "ANTSTATUS=OK", 12) == FALSE)
            {
                GpsSelfCheckInst.AntennaFlg = TRUE;               
            }
        } 
    }

	return (TRUE);
}

/*******************************************************************************
* 名称  : GpsOutputProcess
* 描述  : GPS报文输出处理
* 输入  : pDat - GPS语句缓存
          uiLen - GPS语句字节长度
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void GpsOutputProcess(U8 *pDat, U16 uiLen)
{
	char chTemp[100];
	U16 i;
	U16 uiIndex;
	U8 ucCheck;
    U8 outputEn = FALSE;

	if (DevDynamicPara.ucGpsOutput == TRUE)
	{
		if (strncmp((char *)pDat, "GPRMC", 5) == FALSE)
        {
            if (DevDynamicPara.GpsOutputInterval[
                DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_RMC] != 0)
            {
                DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_RMC]++;            
                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_RMC]
                    >= DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_RMC])
                {
                    DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_RMC] = 0;
                }

                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_RMC] == 0)       
                {
                    outputEn = TRUE;   
                }
            }
        }
        else if (strncmp((char *)pDat, "GPVTG", 5) == FALSE)
        {
            if (DevDynamicPara.GpsOutputInterval[
                DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_VTG] != 0)
            {            
                DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_VTG]++;            
                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_VTG]
                    >= DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_VTG])
                {
                    DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_VTG] = 0;
                }

                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_VTG] == 0)       
                {
                    outputEn = TRUE;   
                }
            }
        }
        else if (strncmp((char *)pDat, "GPGGA", 5) == FALSE)
        {
            if (DevDynamicPara.GpsOutputInterval[
                DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GGA] != 0)
            {            
                DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GGA]++;            
                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GGA]
                    >= DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GGA])
                {
                    DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GGA] = 0;
                }

                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GGA] == 0)       
                {
                    outputEn = TRUE;   
                }
            }            
        }
        else if (strncmp((char *)pDat, "GPGSA", 5) == FALSE)
        {
            if (DevDynamicPara.GpsOutputInterval[
                DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSA] != 0)
            {            
                DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSA]++;            
                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSA]
                    >= DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSA])
                {
                    DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSA] = 0;
                }

                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSA] == 0)       
                {
                    outputEn = TRUE;   
                }
            }
        }
        else if (strncmp((char *)pDat, "GPGSV", 5) == FALSE)
        {
            if (DevDynamicPara.GpsOutputInterval[
                DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSV] != 0)
            {            
                /*lq 获取当前语句的编号 */
                ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);   
        		if ((ucCheck == TRUE) && (chTemp[0] == '1'))    //lq 多语句，只对第一条语句计数
                {
                    DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSV]++;            
                    if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSV]
                        >= DevDynamicPara.GpsOutputInterval[
                        DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSV])
                    {
                        DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSV] = 0;
                    }            
                }

                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSV] == 0)       
                {
                    outputEn = TRUE;   
                }
            }
        }
        else if (strncmp((char *)pDat, "GPGLL", 5) == FALSE)
        {
            if (DevDynamicPara.GpsOutputInterval[
                DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GLL] != 0)
            {            
                DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GLL]++;            
                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GLL]
                    >= DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GLL])
                {
                    DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GLL] = 0;
                }

                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GLL] == 0)       
                {
                    outputEn = TRUE;   
                }
            }
        }
        else if (strncmp((char *)pDat, "GPGBS", 5) == FALSE)
        {
            if (DevDynamicPara.GpsOutputInterval[
                DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GBS] != 0)
            {            
                DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GBS]++;            
                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GBS]
                    >= DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GBS])
                {
                    DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GBS] = 0;
                }

                if (DevDynamicPara.GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GBS] == 0)       
                {
                    outputEn = TRUE;   
                }
            }
        }
                
		if (outputEn == TRUE)
        {
            uiIndex = 0;
    		chTemp[uiIndex++] = '$';
    		for (i = 0; i < uiLen; i++)
    		{
    			chTemp[uiIndex++] = pDat[i];	
    		}
    		chTemp[uiIndex++] = '\0';
    
    		UartResponseMsg(chTemp);	
        }
	}
}

/*******************************************************************************
* 名称  : GpsMsgProcess
* 描述  : GPS报文解析
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GpsMsgProcess(U8 *pDat, U16 uiLen)
{
	U8  ucCheck;

	/*lq 解析GPS语句 */
    ucCheck = DealGpsMsg(&GpsPara, pDat, uiLen);

    /*lq GPS语句串口输出处理 */
    GpsOutputProcess(pDat, uiLen);		

	return (ucCheck);
}

/*******************************************************************************
* 名称  : GetGpsFixSta
* 描述  : 返回GPS定位状态
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GetGpsFixSta(GPSStruct *pGpsCell)
{
	U16 x;

    /*lq GBS报文内容更新 */
    pGpsCell->GPS_RAIMFlag = GpsPara.GPS_RAIMFlag;
	pGpsCell->GPS_errlat = GpsPara.GPS_errlat;
	pGpsCell->GPS_errlon = GpsPara.GPS_errlon; 

    /*lq GGA报文内容更新 */        
    pGpsCell->GPS_QualityIndicator = GpsPara.GPS_QualityIndicator;
       	
    /*lq RMC报文内容更新 */        
    if (IsGpsFix() == TRUE)
	{		
  		for (x = 0; x < 6; x++)
  		{
  			pGpsCell->GPSTimeBuf[x] = GpsPara.GPSTimeBuf[x];
  		}
	  	
  		for (x = 0; x < 18; x++)
  		{
  			pGpsCell->GPS_Sog_Buf[x] = GpsPara.GPS_Sog_Buf[x];
  		}
	  	
  		pGpsCell->GPS_TimeY = GpsPara.GPS_TimeY;
  		pGpsCell->GPS_TimeMon = GpsPara.GPS_TimeMon;
  		pGpsCell->GPS_TimeD = GpsPara.GPS_TimeD;
  		pGpsCell->GPS_TimeH = GpsPara.GPS_TimeH;
  		pGpsCell->GPS_TimeM = GpsPara.GPS_TimeM;
  		pGpsCell->GPS_TimeS = GpsPara.GPS_TimeS;
  		pGpsCell->GPS_LatiD = GpsPara.GPS_LatiD;
  		pGpsCell->GPS_Latitude = GpsPara.GPS_Latitude;
  		pGpsCell->GPS_Lati100 = GpsPara.GPS_Lati100;
  		pGpsCell->GPS_Longitud = GpsPara.GPS_Longitud;
  		pGpsCell->GPS_Long100 = GpsPara.GPS_Long100;
  		pGpsCell->GPS_Sog = GpsPara.GPS_Sog;
  		pGpsCell->GPS_Sog_Index = GpsPara.GPS_Sog_Index;
  		pGpsCell->GPS_Cog = GpsPara.GPS_Cog;
  		pGpsCell->GPS_PosPrec = GpsPara.GPS_PosPrec;

		return (TRUE);
	}
	else
	{
		return (FALSE);
	}	
}

#if 0
/*******************************************************************************
* 名称  : GpsCtrlProcess
* 描述  : GPS状态机控制,适用于SART省电模式的GPS控制
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void GpsCtrlProcess(void)
{
	U8  ucTemp[10];
	U8  ucTemp1[10];
    S16 ssCurrSlot;
    S32 slCurrFrame;	

    ssCurrSlot = GetSlotNum();
    slCurrFrame = GetFrameNum();

	/*lq GPS时隙相位同步*/
    SlotSynClac();

#if CONFIG_GPS_SAVE
    
    /*lq GPS省电条件判断*/
    GpsSaveCondJudge();
#endif

    /*lq 定位状态的LED指示，仅在GPS开启后才进行LED指示*/
    if (GetGpsPowerSta() == TRUE)
    {
        if (IsGpsFix())
        {
            if (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_GPS_FIX)
            {
    			LEDInit(LED_TYPE_GPS_FIX);
            }
        }
        else
        {
            if (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_GPS_SEARCH)
            {
    			LEDInit(LED_TYPE_GPS_SEARCH);
            }                                                                      
        }
    }
    
    /*lq GPS关闭后LED的GPS指示停止*/
    else
    {
        if ((LEDCtrlPara.LedGreenPara.ucType == LED_TYPE_GPS_SEARCH)
            || LEDCtrlPara.LedGreenPara.ucType == LED_TYPE_GPS_FIX) 
        {
            LEDInit(LED_TYPE_NONE);    
        }   
    }


    /*lq GPS状态控制*/
	if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_START)                	        //lq 设备开机状态
	{
        /*lq GPS定位且同步*/
        if (GpsCtrlPara.GPSSyn_Flag == TRUE)
        {
            GpsCtrlPara.GPSSyn_Flag = FALSE;

            /*lq GPS定位长开关闭定时*/
            SetGpsOffTimSlot(ssCurrSlot);
            SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_FIX_LONG_OPEN);       //lq GPS定位长开5min

            /*lq GPS5min长开计数加1*/
            GpsCtrlPara.ssGps5minLngOpenCnt++;
            if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
            {
                ResetGps5minLngOpenEn();

#ifdef GPS_CTRL_DEBUG_MSG
			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
            }
#if 0   //lq 暂不写入EEPROM，防止每次开机后上次记录的值被立即覆盖
            /*lq 保存GPS5min长开计数*/
			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);
#endif
            GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_FIX_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
			usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_FIX_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif
        }
        else
        {
            /*lq GPS未定位首次长开关闭定时*/
            SetGpsOffTimSlot(ssCurrSlot);
            SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_FIRST_LONG_OPEN);   //lq GPS未定位首次长开5min

            /*lq GPS未定位1h定时*/
            SetGpsUnfix1hPeriodTimSlot(ssCurrSlot);
            SetGpsUnfix1hPeriodTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_1_HOUR);

            /*lq GPS5min长开计数加1*/
            GpsCtrlPara.ssGps5minLngOpenCnt++;
            if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
            {
                ResetGps5minLngOpenEn();

#ifdef GPS_CTRL_DEBUG_MSG
			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
            }
#if 0   //lq 暂不写入EEPROM，防止每次开机后上次记录的值被立即覆盖
            /*lq 保存GPS5min长开计数*/
			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);
#endif
            GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
			usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif
        }
                                                                            	
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN)
    {
        /*lq GPS定位且同步*/
        if (GpsCtrlPara.GPSSyn_Flag == TRUE)
        {
            GpsCtrlPara.GPSSyn_Flag = FALSE;

            /*复位GPS未定位长开标志*/
            ResetGpsUnfixLonOpenFlg();
            ResetGpsUnfix1hLngOpenFlg();
            ResetGpsUnfixBey1hLngOpenFlg();
            
            /*lq GPS5min长开使能*/
            if (GetGps5minLngOpenEn() == TRUE)        
            {
                /*lq GPS定位长开关闭定时*/
                SetGpsOffTimSlot(ssCurrSlot);
                SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_FIX_LONG_OPEN);   //lq GPS定位长开5min

                /*lq GPS5min长开计数加1*/
                GpsCtrlPara.ssGps5minLngOpenCnt++;
                if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
                {
                    ResetGps5minLngOpenEn();
    
#ifdef GPS_CTRL_DEBUG_MSG
    			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
                }

                /*lq 保存GPS5min长开计数*/
    			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
    		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);
                
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_FIX_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_FIX_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                         
            }
            else
            {
                /*lq 转入GPS关闭状态*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif  
            }
        } 
        else
        {
            /*lq GPS未定位首次长开定时到*/
            if (IsGpsOffTimeout() == TRUE)
            {
                /*lq 置位GPS未定时1h长开标志*/
                SetGpsUnfixLonOpenFlg();
                SetGpsUnfix1hLngOpenFlg();

                /*lq 转入GPS关闭状态*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif 
            }
        }  
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_UNFIX_FIRST_HOUR_LNG_OPEN)
    {
        /*lq GPS定位且同步*/
        if (GpsCtrlPara.GPSSyn_Flag == TRUE)
        {
            GpsCtrlPara.GPSSyn_Flag = FALSE;
            
            /*lq 复位GPS未定位1h长开标志*/
            ResetGpsUnfixLonOpenFlg();
            ResetGpsUnfix1hLngOpenFlg();

            /*lq GPS5min长开使能*/
            if (GetGps5minLngOpenEn() == TRUE)        
            {
                /*lq GPS定位长开关闭定时*/
                SetGpsOffTimSlot(ssCurrSlot);
                SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_FIX_LONG_OPEN);   //lq GPS定位长开5min

                /*lq GPS5min长开计数加1*/
                GpsCtrlPara.ssGps5minLngOpenCnt++;
                if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
                {
                    ResetGps5minLngOpenEn();
    
#ifdef GPS_CTRL_DEBUG_MSG
    			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
                }

                /*lq 保存GPS5min长开计数*/
    			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
    		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);

                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_FIX_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_FIX_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                    
            }
            else
            {
                /*lq 转入GPS关闭状态*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                  
            }
        }
        else
        {
            /*lq GPS未定位1h长开定时到*/
            if (IsGpsOffTimeout() == TRUE)
            {
                /*lq 1h定时到*/
                if (IsGpsUnfix1hPeriodTimeout() == TRUE)
                {
                    /*lq 复位GPS未定1h长开标志*/
                    ResetGpsUnfix1hLngOpenFlg(); 

                    /*lq 置位GPS未定位超过1h长开标志*/
                    SetGpsUnfixBey1hLngOpenFlg();
                }

                /*lq 转入GPS关闭状态*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                  
            }                                        
        }            
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_UNFIX_BEY_FIRST_HOUR_LNG_OPEN)
    {
        /*lq GPS定位且同步*/
        if (GpsCtrlPara.GPSSyn_Flag == TRUE)
        {
            GpsCtrlPara.GPSSyn_Flag = FALSE;

            /*lq 复位GPS未定位超过1h长开标志*/
            ResetGpsUnfixLonOpenFlg();
            ResetGpsUnfixBey1hLngOpenFlg();

            /*lq GPS5min长开使能*/
            if (GetGps5minLngOpenEn() == TRUE)        
            {
                /*lq GPS定位长开关闭定时*/
                SetGpsOffTimSlot(ssCurrSlot);
                SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_FIX_LONG_OPEN);   //lq GPS定位长开5min

                /*lq GPS5min长开计数加1*/
                GpsCtrlPara.ssGps5minLngOpenCnt++;
                if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
                {
                    ResetGps5minLngOpenEn();
    
#ifdef GPS_CTRL_DEBUG_MSG
    			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
                }

                /*lq 保存GPS5min长开计数*/
    			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
    		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);

                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_FIX_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_FIX_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                    
            }
            else
            {
                /*lq 转入GPS关闭状态*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                  
            }            
        }
        else
        {
            /*lq GPS未定位超过1h长开定时到*/
            if (IsGpsOffTimeout() == TRUE)
            {
                /*lq 转入GPS关闭状态*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                      
            }
        }                        
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_FIX_LNG_OPEN)
    {
        if (GetGpsPowerupFixState() == FALSE)
        {
            /*lq 置位GPS开机定位标志*/
            SetGpsPowerupFixState();

			/*lq 记录开机时间*/
            sprintf((char *)ucTemp, "%2d", GpsPara.GPS_TimeMon);
    		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeD);
    		strcat((char *)ucTemp, (char *)ucTemp1);
            sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeH);
    		strcat((char *)ucTemp, (char *)ucTemp1);
    		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeM);
    		strcat((char *)ucTemp, (char *)ucTemp1);
    		Para_Save(PARA_TIME_OPEN_TYPE, ucTemp, PARA_TIME_OPEN_MAXLEN);
        	Para_Save(PARA_TIME_CLOSE_TYPE, ucTemp, PARA_TIME_CLOSE_MAXLEN);                           //lq [2013/04/16]

			/*lq 清除关机原因*/
			sprintf((char *)ucTemp, "%d", 0);     		                        //lq 意外关机只能设为默认关机方式，只有按键关机或超低电量关机时，才会再更改关机原因
			Para_Save(PARA_CLOSE_RESION_TYPE, ucTemp, 1);
			
			/*lq 清除GPS3次未定位计数*/
			sprintf((char *)ucTemp, "%3d", 0);
		   	Para_Save(PARA_GPS_WAIT_COUNT_TYPE, ucTemp, 3);						

			/*lq 清除GPS正常定位计数*/
			sprintf((char *)ucTemp, "%5d", 0);
		   	Para_Save(PARA_GPS_FIX_COUNT_TYPE, ucTemp, 5);
            
            /*lq 清除GPS5min长开计数*/
			sprintf((char *)ucTemp, "%3d", 0);
		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);
        } 
        
        /*lq GPS定位长开定时到*/ 
        if (IsGpsOffTimeout() == TRUE)
        {
            /*lq 计算时隙、相位偏差调整值（在GPS开机定位保护完成后进行）*/
			SetAdjustCalcNow();

            /*lq 转入GPS关闭状态*/            
            GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		    usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                
        }
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_NORMAL_WORKING)
    {
        /*lq 组消息未开始发送*/
        if (GetBurstStarFlg() == FALSE)
        {
            /*lq GPS定位且同步*/
            if (GpsCtrlPara.GPSSyn_Flag == TRUE)
            {
                GpsCtrlPara.GPSSyn_Flag = FALSE;
                
                /*lq 置位正常定位标志*/
                SetGpsNormalFixState();

                /*lq 定位失败次数清零，正常定位次数加1*/
                GpsCtrlPara.GpsFallCount = 0;
                GpsCtrlPara.GpsFixCount++;
    			sprintf((char *)ucTemp, "%5d", GpsCtrlPara.GpsFixCount);
    		   	Para_Save(PARA_GPS_FIX_COUNT_TYPE, ucTemp, 5);//最多为65536次

                /*lq 转入GPS关闭状态*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
			    usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                
            }
                    
        }
        else
        {
            /*lq 组消息开始发送，GPS定位失败*/
            GpsCtrlPara.GpsFallCount++;
            usart_printf("$Gps Fall Count : %d\r\n", GpsCtrlPara.GpsFallCount);	                
            
            if (GpsCtrlPara.GpsFallCount >= eqEquipPara.GpsFixFallThres)
            {
            	/*lq GPS定位失败次数清零*/
                GpsCtrlPara.GpsFallCount = 0;
                
                /*lq GPS未定位长开次数加1*/
                GpsCtrlPara.GpsWaitCount++;

                /*lq 保存GPS3次未定位次数*/
    			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.GpsWaitCount);
    		   	Para_Save(PARA_GPS_WAIT_COUNT_TYPE, ucTemp, 3);

                /*lq GPS5min长开使能*/
                if (GetGps5minLngOpenEn() == TRUE)        
                {
                    /*lq GPS未定位首次长开关闭定时*/
                    SetGpsOffTimSlot(ssCurrSlot);
                    SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_FIRST_LONG_OPEN);   //lq GPS未定位首次长开5min
            
                    /*lq GPS未定位1h定时*/
                    SetGpsUnfix1hPeriodTimSlot(ssCurrSlot);
                    SetGpsUnfix1hPeriodTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_1_HOUR);

                    /*lq GPS5min长开计数加1*/
                    GpsCtrlPara.ssGps5minLngOpenCnt++;
                    if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
                    {
                        ResetGps5minLngOpenEn();
        
#ifdef GPS_CTRL_DEBUG_MSG
        			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
                    }

                    /*lq 保存GPS5min长开计数*/
        			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
        		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);

                    GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
			        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                       
                }
                else
                {
                    /*lq 置位未定位1h长开标志*/
                    SetGpsUnfixLonOpenFlg();
                    SetGpsUnfix1hLngOpenFlg();
                    
                    /*lq GPS未定位1h定时*/    
                    SetGpsUnfix1hPeriodTimSlot(ssCurrSlot);
                    SetGpsUnfix1hPeriodTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_1_HOUR);

                    /*lq 转入GPS关闭状态*/
                    GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;           	 

#ifdef GPS_CTRL_DEBUG_MSG
			        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                       
                }
            } 
            else
            {
                /*lq 转入GPS关闭状态*/
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;           	 

#ifdef GPS_CTRL_DEBUG_MSG
			    usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                       
            }        
        }            
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_POWER_DOWN)
    {
        if (GetGpsPowerSta() == TRUE)
        {
            GpsPowerDown();

#ifdef GPS_CTRL_DEBUG_MSG
            UartResponseMsg("$GPS_POWER_OFF!");
#endif
        }
        
        /*lq 组消息开始发送*/
        if (GetBurstStarFlg() == TRUE)
        {
            /*lq... GPS开启定时*/
            SetGpsOnTim(SLOT_INTERVAL_GPS_ON_TO_TX);

            /*lq 转入GPS空闲状态*/            
            GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_IDLE;

#ifdef GPS_CTRL_DEBUG_MSG
			usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_IDLE!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif      
        }     
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_IDLE)
    {
        /*lq GPS开启状态有效*/
        if (GetMsgGpsPowerup() == TRUE)
        {
            /*lq 开启GPS电源，重置GPS控制相关参数*/                   
            GpsPowerUp();
#ifdef GPS_CTRL_DEBUG_MSG
            UartResponseMsg("$GPS_POWER_ON!");
#endif
            /*lq 根据GPS定位情况选择相应的状态*/
            if (GetGpsUnfix1hLngOpenFlg() == TRUE)                          //lq 转入GPS未定位1h长开状态
            {
                /*lq GPS未定位1h长开定时*/
                SetGpsOffTimSlot(ssCurrSlot);
                SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_1HOUR_LONG_OPEN);   //lq GPS未定位1h内长开1min                    

                /*lq 转入GPS未定位1h长开状态*/                
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_UNFIX_FIRST_HOUR_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_UNFIX_FIRST_HOUR_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif   
            }
            else if (GetGpsUnfixBey1hLngOpenFlg() == TRUE)                  //lq 转入GPS未定位超过1h长开状态        
            {
                /*lq GPS未定位超过1h长开定时*/
                SetGpsOffTimSlot(ssCurrSlot);
                SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_BEY_1HOUR_LONG_OPEN);   //lq GPS未定位超过1h长开2min                    

                /*lq 转入GPS未定位超过1h长开状态*/                
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_UNFIX_BEY_FIRST_HOUR_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_UNFIX_BEY_FIRST_HOUR_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif   
            }
            else                                                            //lq 转入GPS正常工作状态 
            {
                /*lq 转入GPS正常工作状态*/                
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_NORMAL_WORKING;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_NORMAL_WORKING!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif   
            }
        }
    }
    else
    {
        /*lq 转入GPS开始状态*/                
        GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_START;

#ifdef GPS_CTRL_DEBUG_MSG
	    usart_printf("[%6d:%4d]$GPS_CTRL_STATE : GPS_CTRL_STA_START!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif   
    }    
}
#endif

#if 0
/*******************************************************************************
* 名称  : GpsCtrlProcess
* 描述  : GPS状态机控制，适用于AtoN工作模式的GPS控制。GPS在设备开机后一直处于省
          电工作模式，不会关闭。
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void GpsCtrlProcess(void)
{
	U8  ucTemp[10];
	U8  ucTemp1[10];
    S16 ssCurrSlot;
    S32 slCurrFrame;	

    /*lq 获取当前帧和时隙 */
    ssCurrSlot = GetSlotNum();
    slCurrFrame = GetFrameNum();

    /*lq GPS定位状态的LED指示，仅在GPS开启后才进行LED指示，GPS关闭后LED的GPS指示停止*/
    if (GpsCtrlPara.GpsCtrlSta != ATON_GPS_CTRL_STA_IDLE)
    {
        if (IsGpsFix())
        {
            if (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_GPS_FIX)
            {
    			LEDInit(LED_TYPE_GPS_FIX);
            }
        }
        else
        {
            if (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_GPS_SEARCH)
            {
    			LEDInit(LED_TYPE_GPS_SEARCH);
            }                                                                      
        }
    }
    else
    {
        if ((LEDCtrlPara.LedGreenPara.ucType == LED_TYPE_GPS_SEARCH)
            || LEDCtrlPara.LedGreenPara.ucType == LED_TYPE_GPS_FIX) 
        {
            LEDInit(LED_TYPE_NONE);    
        }   
    }

	/*lq GPS时隙相位同步*/
    SlotSynClac();  //lq... AtoN需改为帧、时隙和相位同步

    /*lq GPS偏离采样*/
    if (GetGpsOffPositionSampleEn() == TRUE)
    {
        if (GetGpsPositionSampleNewFlgRst() == TRUE)
        {
            OP1_GPSPositionAnal(GpsPara.GPS_Longitud, GpsPara.GPS_Latitude);
        }
    }
    
    /*lq AtoN GPS状态控制*/
    if (GpsCtrlPara.GpsCtrlSta == ATON_GPS_CTRL_STA_START)
    {
        /*lq GPS接收机启动配置已完成*/
        if (UBX1_GetSendRequireFlag() == FALSE)
        {
            /*lq 在GPS正常配置完成后，将GPS秒脉冲置为有效*/
            SetGpsPulseEn();

            /*lq GPS定位且同步*/
            if (GpsCtrlPara.GPSSyn_Flag == TRUE)
            {
                GpsCtrlPara.GPSSyn_Flag = FALSE;

                /*lq 置位正常定位标志*/
                SetGpsNormalFixState();
                        
                /*lq GPS定位长开关闭定时*/
                GpsCtrlPara.TimePeriod = GPS_CTRL_1MINPEROID;
            	GpsCtrlPara.TimeCnt = SetTimePeriodProcess(GpsCtrlPara.TimePeriod);
            	GpsCtrlPara.Time1mCnt = 0;  

                GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_FIX_LNG_OPEN;
    
#if CONFIG_DEBUG
    			usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_FIX_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif
            }
            else
            {
                GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_NORMAL_WORKING;
    
#if CONFIG_DEBUG
    			usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_NORMAL_WORKING!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif
            }            
        }                    
    }
    else if (GpsCtrlPara.GpsCtrlSta == ATON_GPS_CTRL_STA_FIX_LNG_OPEN)
    {
        if (GetGpsPowerupFixState() == FALSE)
        {
            /*lq 置位GPS开机定位标志*/
            SetGpsPowerupFixState();

			/*lq 记录开机时间*/
            sprintf((char *)ucTemp, "%2d", GpsPara.GPS_TimeMon);
    		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeD);
    		strcat((char *)ucTemp, (char *)ucTemp1);
            sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeH);
    		strcat((char *)ucTemp, (char *)ucTemp1);
    		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeM);
    		strcat((char *)ucTemp, (char *)ucTemp1);
    		Para_Save(PARA_TIME_OPEN_TYPE, ucTemp, PARA_TIME_OPEN_MAXLEN);
        	Para_Save(PARA_TIME_CLOSE_TYPE, ucTemp, PARA_TIME_CLOSE_MAXLEN);                           //lq [2013/04/16]

			/*lq 清除关机原因*/
			sprintf((char *)ucTemp, "%d", 0);     		                        //lq 意外关机只能设为默认关机方式，只有按键关机或超低电量关机时，才会再更改关机原因
			Para_Save(PARA_CLOSE_RESION_TYPE, ucTemp, 1);
			
			/*lq 清除GPS3次未定位计数*/
			sprintf((char *)ucTemp, "%3d", 0);
		   	Para_Save(PARA_GPS_WAIT_COUNT_TYPE, ucTemp, 3);						

			/*lq 清除GPS正常定位计数*/
			sprintf((char *)ucTemp, "%5d", 0);
		   	Para_Save(PARA_GPS_FIX_COUNT_TYPE, ucTemp, 5);
            
            /*lq 清除GPS5min长开计数*/
			sprintf((char *)ucTemp, "%3d", 0);
		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);
        } 

        /*lq 连续三次未定位且同步后重新定位且同步*/                    
        if (GetGpsUnfixLonOpenFlg() == TRUE)
        {
            ResetGpsUnfixLonOpenFlg();
        }

    	if (GpsCtrlPara.TimeCnt == GetLocalClockTickProcess())
    	{
    		GpsCtrlPara.TimeCnt = SetTimePeriodProcess(GpsCtrlPara.TimePeriod);
            
            GpsCtrlPara.Time1mCnt++;
            if (GpsCtrlPara.Time1mCnt >= GPS_CTRL_TIME_FIX_LONG_OPEN)
            {
                /*lq 计算时隙、相位偏差调整值（在GPS开机定位保护完成后进行）*/
    			SetAdjustCalcNow();
    
                /*lq 使用UBX protocol 消息配置GPS进入自定义省电工作模式*/
                UBX1_GpsIdleConfig();                
                
                /*lq 转入GPS空闲状态*/            
                GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_IDLE;
    
#if CONFIG_DEBUG
    		    usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_IDLE!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                
            }
        
        }
    }
    else if (GpsCtrlPara.GpsCtrlSta == ATON_GPS_CTRL_STA_NORMAL_WORKING)
    {
        /*lq GPS接收机进入自定义正常工作模式配置已完成*/
        if (UBX1_GetSendRequireFlag() == FALSE)
        {
            /*lq 在GPS正常配置完成后，将GPS秒脉冲置为有效*/
            SetGpsPulseEn();

            /*lq 正常定位且同步*/
            if (GetGpsNormalFixState() == FALSE)                
            {
                /*lq GPS定位且同步*/
                if (GpsCtrlPara.GPSSyn_Flag == TRUE)
                {
                    GpsCtrlPara.GPSSyn_Flag = FALSE;
    
                    /*lq 置位正常定位标志*/
                    SetGpsNormalFixState();
        
                    /*lq 定位失败次数清零，正常定位次数加1*/
                    GpsCtrlPara.GpsFallCount = 0;
                    GpsCtrlPara.GpsFixCount++;
        			sprintf((char *)ucTemp, "%5d", GpsCtrlPara.GpsFixCount);
        		   	Para_Save(PARA_GPS_FIX_COUNT_TYPE, ucTemp, 5);//最多为65536次 
 
                    /*lq 连续三次未定位且同步后重新定位且同步 OR 开机首次定位*/
                    if (GetGpsUnfixLonOpenFlg() == TRUE
                        || GetGpsPowerupFixState() == FALSE)
                    {
                        /*lq GPS定位长开关闭定时*/
                        GpsCtrlPara.TimePeriod = GPS_CTRL_1MINPEROID;
                    	GpsCtrlPara.TimeCnt = SetTimePeriodProcess(GpsCtrlPara.TimePeriod);
                    	GpsCtrlPara.Time1mCnt = 0;  
                        
                        /*lq 转入GPS定位长开状态*/            
                        GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_FIX_LNG_OPEN;
        
#if CONFIG_DEBUG
        		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_FIX_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif 
                    }
                }                                   
            }

            /*lq 组消息已开始发送*/
            if (GetBurstStarFlg() == TRUE)
            {
                /*lq 正常定位且同步*/
                if (GetGpsNormalFixState() == TRUE)                
                {
                    /*lq 可能偏离条件成立 */
                    if (OP1_GetGPSOffPositionPossStat() == TRUE)
                    {
                        /*lq 复位偏离判断结构体参数*/
                        OP1_GPSOffPositionInstanceReset();

                        /*lq 转入GPS偏离判断状态*/            
                        GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_OFF_POSITION_JUDGE;                        

#if CONFIG_DEBUG
		                usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_OFF_POSITION_JUDGE!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif
                    }
                    else
                    {
                        /*lq 使用UBX protocol 消息配置GPS进入自定义省电工作模式*/
                        UBX1_GpsIdleConfig();
                        
                        /*lq 转入GPS空闲状态*/            
                        GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_IDLE;                        

#if CONFIG_DEBUG
		                usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_IDLE!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                        
                    }
                }
                else
                {
                    /*lq 组消息开始发送，GPS定位失败*/
                    GpsCtrlPara.GpsFallCount++;
    
#if CONFIG_DEBUG
                    usart_printf("$Gps Fall Count : %d\r\n", GpsCtrlPara.GpsFallCount);	                
#endif                
                    if (GpsCtrlPara.GpsFallCount >= eqEquipPara.GpsFixFallThres)
                    {
                    	/*lq GPS定位失败次数清零*/
                        GpsCtrlPara.GpsFallCount = 0;
                        
                        /*lq GPS未定位长开次数加1*/
                        GpsCtrlPara.GpsWaitCount++;
        
                        /*lq 保存GPS3次未定位次数*/
            			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.GpsWaitCount);
            		   	Para_Save(PARA_GPS_WAIT_COUNT_TYPE, ucTemp, 3);
    
                        /*lq 置位未定位1h长开标志*/
                        SetGpsUnfixLonOpenFlg();                                                         
                    } 
    
                    /*lq 使用UBX protocol 消息配置GPS进入自定义省电工作模式*/
                    UBX1_GpsIdleConfig();
                    
                    /*lq 转入GPS空闲状态*/            
                    GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_IDLE;                        
    
#if CONFIG_DEBUG
                    usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_IDLE!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                 
                }
            }                          
        }        
    }
    else if (GpsCtrlPara.GpsCtrlSta == ATON_GPS_CTRL_STA_OFF_POSITION_JUDGE)
    {
        /*lq 正常定位且同步*/
        if (GetGpsNormalFixState() == FALSE)                
        {
            /*lq GPS定位且同步*/
            if (GpsCtrlPara.GPSSyn_Flag == TRUE)
            {
                GpsCtrlPara.GPSSyn_Flag = FALSE;

                /*lq 置位正常定位标志*/
                SetGpsNormalFixState();
            }                                   
        }

        /*lq 等待偏离条件有效*/
        if (OP1_GetGPSOffPositionValid() == TRUE)
        {
            /*lq 偏离条件成立 */    
            if (OP1_GetGPSOffPositionStat() == TRUE)
            {
                /*lq 转入GPS偏离长开状态 */            
                GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_OFF_POSITION;                        

#if CONFIG_DEBUG
                usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_OFF_POSITION!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                        
            }
            else
            {
                /*lq 使用UBX protocol 消息配置GPS进入自定义省电工作模式*/
                UBX1_GpsIdleConfig();
                
                /*lq 转入GPS空闲状态*/            
                GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_IDLE;                        

#if CONFIG_DEBUG
                usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_IDLE!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif            
            }                    
        }
    }
    else if (GpsCtrlPara.GpsCtrlSta == ATON_GPS_CTRL_STA_OFF_POSITION)
    {
        /*lq 正常定位且同步*/
        if (GetGpsNormalFixState() == FALSE)                
        {
            /*lq GPS定位且同步*/
            if (GpsCtrlPara.GPSSyn_Flag == TRUE)
            {
                GpsCtrlPara.GPSSyn_Flag = FALSE;

                /*lq 置位正常定位标志*/
                SetGpsNormalFixState();
            }                                   
        }

        /*lq 等待偏离条件有效*/
        if (OP1_GetGPSOffPositionValid() == TRUE)
        {
            /*lq 偏离条件不成立 */    
            if (OP1_GetGPSOffPositionStat() == FALSE)
            {
                /*lq 使用UBX protocol 消息配置GPS进入自定义省电工作模式*/
                UBX1_GpsIdleConfig();
                
                /*lq 转入GPS空闲状态*/            
                GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_IDLE;                        

#if CONFIG_DEBUG
                usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_IDLE!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif            
            }                    
        }    
    }
    else if (GpsCtrlPara.GpsCtrlSta == ATON_GPS_CTRL_STA_IDLE)
    {
        /*lq GPS偏离采样失能 */
        if (GetGpsOffPositionSampleEn() == TRUE)
        {
            ResetGpsOffPositionSampleEn();
        }

        /*lq GPS接收机进入自定义省电工作模式配置已完成*/
        if (UBX1_GetSendRequireFlag() == FALSE)
        {
            /*lq GPS开启状态有效，GPS并未掉电*/
            if (GetMsgGpsPowerup() == TRUE)
            {
                /*lq 重置GPS控制相关参数*/                   
                GpsWakeUp();
#if CONFIG_DEBUG
                UartResponseMsg("$GPS_WAKE_ON!");
#endif
                /*lq 使用UBX protocol 消息配置GPS进入自定义正常工作模式*/ 
                UBX1_GpsNormalConfig();
                
                /*lq 转入GPS正常工作状态*/                
                GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_NORMAL_WORKING;
    
#if CONFIG_DEBUG
                usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_NORMAL_WORKING!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif   
            }                    
        }    
    }
    else
    {
        /*lq GPS启动配置*/        
        UBX1_GpsStartupConfig();
        
        /*lq 转入GPS正常工作状态*/                
        GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_START;

#if CONFIG_DEBUG
        usart_printf("[%6d:%4d]$GPS_CTRL_STATE : ATON_GPS_CTRL_STA_START!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif     
    }
}

#endif

/*******************************************************************************
* 名称  : GpsCtrlProcess
* 描述  : GPS状态控制，适用于M4508D。GPS长开且一直处于正常工作模式。
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void GpsCtrlProcess(void)
{
    DateTimeStruct dateTime;
	U8  ucTemp[10];
	U8  ucTemp1[15];
    S16 currSlot;
    S32 currFrame;
	U8  ucTempBuf[40];
	S32	distance;

		

    /*lq 获取当前帧和时隙 */
    currSlot = GetSlotNum();
    currFrame = GetFrameNum();

    /*lq GPS定位状态LED指示 */
    if (IsGpsFix())
    {
		LEDInit(LED_TYPE_GPS_FIX);
		Para_Read(PARA_GPS_FIRST_TIME_TYPE,ucTempBuf);
		if(strncmp(ucTempBuf, PARA_GPS_FIRST_TIME_DEFAULT, 12) == FALSE)
		{ 
			/* 记录GPS第一次定位的时间 */
			dateTime.Year = GpsPara.GPS_TimeY;
			dateTime.Month = GpsPara.GPS_TimeMon;
			dateTime.Day = GpsPara.GPS_TimeD;
			dateTime.Hour = GpsPara.GPS_TimeH;
			dateTime.Minute = GpsPara.GPS_TimeM;
			dateTime.Second = GpsPara.GPS_TimeS;
			
			sprintf((char *)ucTempBuf, "%02d", (dateTime.Year) % 100);
			sprintf((char *)ucTemp1, "%02d", dateTime.Month);
			strcat((char *)ucTempBuf, (char *)ucTemp1);
			sprintf((char *)ucTemp1, "%02d", dateTime.Day);
			strcat((char *)ucTempBuf, (char *)ucTemp1);
			sprintf((char *)ucTemp1, "%02d", dateTime.Hour);
			strcat((char *)ucTempBuf, (char *)ucTemp1);
			sprintf((char *)ucTemp1, "%02d", dateTime.Minute);
			strcat((char *)ucTempBuf, (char *)ucTemp1);
			sprintf((char *)ucTemp1, "%02d", dateTime.Second);
			strcat((char *)ucTempBuf, (char *)ucTemp1);
			
			Para_Save(PARA_GPS_FIRST_TIME_TYPE,ucTempBuf,PARA_GPS_FIRST_TIME_MAXLEN);		
		}
		if(usGPSSynContinueTime == 61)// || (((GpsPara.GPS_TimeM + 60) - usGPSSynContinueTime) % 60) >= 5)
		{
			usGPSSynContinueTime = GpsPara.GPS_TimeM;
			LastGPS_Latitude = (double)GpsPara.GPS_Latitude;				  
			LastGPS_Longitud = (double)GpsPara.GPS_Longitud;
		}
		else if ((((GpsPara.GPS_TimeM + 60) - usGPSSynContinueTime) % 60) >= 60)
		{
			/* 读取本次之前的航行距离 */
			Para_Read(PARA_SAILING_DISTINCE_TYPE, ucTempBuf);               // 本次计算之前的总航距离
            distance = atol((char *)ucTempBuf);
			
			/* 0000.1分转换为度*/
        	LastGPS_Latitude = LastGPS_Latitude / 600000.0;
        	LastGPS_Longitud = LastGPS_Latitude / 600000.0;
        	NowGPS_Latitude = (double)GpsPara.GPS_Latitude / 600000.0;
        	NowGPS_Longitud = (double)GpsPara.GPS_Longitud / 600000.0;
			
			/* 计算本次航行距离 */        	
        	distance += GetDistance(NowGPS_Latitude, NowGPS_Longitud, LastGPS_Latitude, LastGPS_Longitud);

			sprintf(ucTempBuf, "%5d", distance);
            Para_Save(PARA_SAILING_DISTINCE_TYPE, (U8 *)ucTempBuf, 5);      // 计算之后的总航距离

			/* 更新时间、经纬度 */
			usGPSSynContinueTime = GpsPara.GPS_TimeM;
			LastGPS_Latitude = (double)GpsPara.GPS_Latitude;				  
			LastGPS_Longitud = (double)GpsPara.GPS_Longitud;
		}
    }
    else
    {
		LEDInit(LED_TYPE_GPS_SEARCH);
    }

	/*lq GPS时隙相位同步 */
    SlotSynClac();

    /*lq M4508D GPS状态控制 */
    switch(GpsCtrlPara.GpsCtrlSta)
    {
		case GPS_CTRL_STA_START:
		{
			/*lq GPS未定位告警定时 */
			GpsCtrlPara.TimePeriod = GPS_CTRL_TIME_PERIOD_1S;
			GpsCtrlPara.TimeCnt = SetTimePeriodProcess(GpsCtrlPara.TimePeriod);
			GpsCtrlPara.Timeout = GPS_CTRL_TIME_TIMEOUT_UNFIX_ERR;
			GpsCtrlPara.TimeoutCnt = 0;

			GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_UNFIXED;

			debug_printf("$[%6d:%4d] %s : Gps ctrl state to UNFIXED!\r\n",
				currFrame, currSlot, __func__);
			break;
		}
    
		case GPS_CTRL_STA_UNFIXED:
		{
			/*lq GPS定位且同步*/
			if (GpsCtrlPara.GPSSyn_Flag == TRUE)
			{
				GpsCtrlPara.GPSSyn_Flag = FALSE;

				/*lq 置位正常定位标志*/
				SetGpsNormalFixState();
                    
				/*lq GPS未定位判定定时 */
				GpsCtrlPara.TimePeriod = GPS_CTRL_TIME_PERIOD_1S;
				GpsCtrlPara.TimeCnt = SetTimePeriodProcess(GpsCtrlPara.TimePeriod);
				GpsCtrlPara.Timeout = GPS_CTRL_TIME_TIMEOUT_UNFIX_ASSERT;
				GpsCtrlPara.TimeoutCnt = 0;

				GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_FIXED;
	
				debug_printf("$[%6d:%4d] %s : Gps ctrl state to FIXED!\r\n",
					currFrame, currSlot, __func__);
			}
			else
			{
				/*lq GPS未定位告警判断 */
				if (GpsCtrlPara.TimeCnt == GetLocalClockTickProcess())
				{
					GpsCtrlPara.TimeCnt = SetTimePeriodProcess(GpsCtrlPara.TimePeriod);
				
					GpsCtrlPara.TimeoutCnt++;
					if (GpsCtrlPara.TimeoutCnt >= GpsCtrlPara.Timeout)
					{
						GpsCtrlPara.TimeoutCnt = 0;
       
						LEDInit(LED_TYPE_ERR);

						GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_UNFIXED_ERR;
      
						debug_printf("$[%6d:%4d] %s : Gps ctrl state to UNFIXED_ERR!\r\n",
							currFrame, currSlot, __func__);
					}
				}        
			}
			break;
		}
		case GPS_CTRL_STA_UNFIXED_ERR:
		{
			/*lq GPS定位且同步*/
			if (GpsCtrlPara.GPSSyn_Flag == TRUE)
			{
				GpsCtrlPara.GPSSyn_Flag = FALSE;

				/*lq 置位正常定位标志*/
				SetGpsNormalFixState();
                    
				/*lq GPS未定位判定定时 */
				GpsCtrlPara.TimePeriod = GPS_CTRL_TIME_PERIOD_1S;
				GpsCtrlPara.TimeCnt = SetTimePeriodProcess(GpsCtrlPara.TimePeriod);
				GpsCtrlPara.Timeout = GPS_CTRL_TIME_TIMEOUT_UNFIX_ASSERT;
				GpsCtrlPara.TimeoutCnt = 0;

				LEDInit(LED_TYPE_RED_NONE);
	
					GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_FIXED;

				debug_printf("$[%6d:%4d] %s : Gps ctrl state to FIXED!\r\n",
					currFrame, currSlot, __func__);
			}  
			break;
		}
		case GPS_CTRL_STA_FIXED:
		{
			if (GetGpsPowerupFixState() == FALSE)
			{
				/*lq 置位GPS开机定位标志 */
				SetGpsPowerupFixState();

				/*lq 记录开机时间 */
				dateTime.Year = GpsPara.GPS_TimeY;
				dateTime.Month = GpsPara.GPS_TimeMon;
				dateTime.Day = GpsPara.GPS_TimeD;
				dateTime.Hour = GpsPara.GPS_TimeH;
				dateTime.Minute = GpsPara.GPS_TimeM;
				dateTime.Second = GpsPara.GPS_TimeS;
            
				TIME_DateTimeOffset28Days(&dateTime, -GetFrameNum());     //lq 定位时的时间减去开机至现在的时间间隔

				sprintf((char *)ucTemp, "%02d", (dateTime.Year) % 100);
				sprintf((char *)ucTemp1, "%02d", dateTime.Month);
				strcat((char *)ucTemp, (char *)ucTemp1);
				sprintf((char *)ucTemp1, "%02d", dateTime.Day);
				strcat((char *)ucTemp, (char *)ucTemp1);
				sprintf((char *)ucTemp1, "%02d", dateTime.Hour);
				strcat((char *)ucTemp, (char *)ucTemp1);
				sprintf((char *)ucTemp1, "%02d", dateTime.Minute);
				strcat((char *)ucTemp, (char *)ucTemp1);
				sprintf((char *)ucTemp1, "%02d", dateTime.Second);
				strcat((char *)ucTemp, (char *)ucTemp1);

				Para_OpenTimeSave(ucTemp, DevDynamicPara.OpenCloseTimeIndex);
				Para_CloseTimeSave(ucTemp, DevDynamicPara.OpenCloseTimeIndex);
				TASK_TaskTimEnable(&RecordCloseTimeTask,
                TASK_TIME_TIMEOUT_RECORD_CLOSE_TIME);
			} 

			/*lq GPS定位且同步*/
			if (GpsCtrlPara.GPSSyn_Flag == TRUE)
			{
				GpsCtrlPara.GPSSyn_Flag = FALSE;
	
				/*lq GPS未定位判定重新定时 */
				GpsCtrlPara.TimePeriod = GPS_CTRL_TIME_PERIOD_1S;
				GpsCtrlPara.TimeCnt = SetTimePeriodProcess(GpsCtrlPara.TimePeriod);
				GpsCtrlPara.Timeout = GPS_CTRL_TIME_TIMEOUT_UNFIX_ASSERT;
				GpsCtrlPara.TimeoutCnt = 0;
			}
			else
			{
				/*lq GPS未定位判定判断 */
				if (GpsCtrlPara.TimeCnt == GetLocalClockTickProcess())
				{
					GpsCtrlPara.TimeCnt = SetTimePeriodProcess(GpsCtrlPara.TimePeriod);
				
					GpsCtrlPara.TimeoutCnt++;
					if (GpsCtrlPara.TimeoutCnt >= GpsCtrlPara.Timeout)
					{
						GpsCtrlPara.TimeoutCnt = 0;

						/*lq 判定为未定位 */
						ResetGpsNormalFixState();
                    
						/*lq GPS未定位告警定时 */
						GpsCtrlPara.TimePeriod = GPS_CTRL_TIME_PERIOD_1S;
						GpsCtrlPara.TimeCnt = SetTimePeriodProcess(GpsCtrlPara.TimePeriod);
						GpsCtrlPara.Timeout = GPS_CTRL_TIME_TIMEOUT_UNFIX_ERR;
						GpsCtrlPara.TimeoutCnt = 0;

						GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_UNFIXED;
        
						debug_printf("$[%6d:%4d] %s : Gps ctrl state to UNFIXED!\r\n",
							currFrame, currSlot, __func__);
					}
				}
			}  
			break;
		}
		default:
		{
			GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_START;

			debug_printf("$[%6d:%4d] %s : Gps ctrl state to START!\r\n",
				currFrame, currSlot, __func__);    
			break;
		}		
	}
}

/*******************************************************************************
* 名称  : GpsCtrlProcess
* 描述  : GPS状态机控制,适用于SART省电模式的GPS控制
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void GpsCtrlProcess_MsgTest(void)
{
    /*lq 定位状态的LED指示*/
    if (IsGpsFix())
    {
        if (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_GPS_FIX)
        {
			LEDInit(LED_TYPE_GPS_FIX);
        }
    }
    else
    {
        if (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_GPS_SEARCH)
        {
			LEDInit(LED_TYPE_GPS_SEARCH);
        }                                                                      
    }

    /*lq 用户测试模式下，GPS控制状态机*/
    if (GPSCtrlMsgTestPara.GpsCtrlSta == GPS_CTRL_STA_MSG_TEST_START)
    {
        if (IsGpsFix())
        {
            GPSCtrlMsgTestPara.GpsPowerupFix = TRUE;

            GPSCtrlMsgTestPara.GpsCtrlSta = GPS_CTRL_STA_MSG_TEST_POWER_DOWN;                        
		    usart_printf("[%6d:%4d]$MsgCtrlStateMsgTest : GPS_CTRL_STA_MSG_TEST_POWER_DOWN!\r\n",
                         GetFrameNum(), GetSlotNum());	                            
        }
        else
        {
            /*lq GPS1min定时*/
            GPSCtrlMsgTestPara.TimeCnt = SetTimePeriodProcess(GPSCtrlMsgTestPara.TimePeriod);

            GPSCtrlMsgTestPara.GpsCtrlSta = GPS_CTRL_STA_MSG_TEST_WAIT_FIX; 
		    usart_printf("[%6d:%4d]$MsgCtrlStateMsgTest : GPS_CTRL_STA_MSG_TEST_WAIT_FIX!\r\n",
                         GetFrameNum(), GetSlotNum());	                            
        }
    }
    else if (GPSCtrlMsgTestPara.GpsCtrlSta == GPS_CTRL_STA_MSG_TEST_WAIT_FIX)
    {
        if (IsGpsFix())
        {
            /*lq 定位成功*/
            GPSCtrlMsgTestPara.GpsPowerupFix = TRUE;

            GPSCtrlMsgTestPara.GpsCtrlSta = GPS_CTRL_STA_MSG_TEST_POWER_DOWN;                        
		    usart_printf("[%6d:%4d]$MsgCtrlStateMsgTest : GPS_CTRL_STA_MSG_TEST_POWER_DOWN!\r\n",
                         GetFrameNum(), GetSlotNum());	                            
        }
        else
        {
	         //lq 等待定位                   
        }

        if (GPSCtrlMsgTestPara.TimeCnt == GetLocalClockTickProcess())
        {
            GPSCtrlMsgTestPara.TimeCnt = SetTimePeriodProcess(GPSCtrlMsgTestPara.TimePeriod);
            
            GPSCtrlMsgTestPara.Time1mCnt++;
            if (GPSCtrlMsgTestPara.Time1mCnt >= GPS_CTRL_TIME_THRES_MSG_TEST)
            {
                /*lq 用户测试模式定时到，关闭GPS*/
                GPSCtrlMsgTestPara.GpsCtrlSta = GPS_CTRL_STA_MSG_TEST_POWER_DOWN;                        
    		    usart_printf("[%6d:%4d]$MsgCtrlStateMsgTest : GPS_CTRL_STA_MSG_TEST_POWER_DOWN!\r\n",
                             GetFrameNum(), GetSlotNum());	                            
            }
        }                
    }
    else if (GPSCtrlMsgTestPara.GpsCtrlSta == GPS_CTRL_STA_MSG_TEST_POWER_DOWN)
    {
        /*lq GPS关闭*/
        GpsPowerDown();
        
        GPSCtrlMsgTestPara.GpsCtrlSta = GPS_CTRL_STA_MSG_TEST_IDLE;                        
	    usart_printf("[%6d:%4d]$MsgCtrlStateMsgTest : GPS_CTRL_STA_MSG_TEST_IDLE!\r\n",
                     GetFrameNum(), GetSlotNum());	                            
                                
    }
    else if (GPSCtrlMsgTestPara.GpsCtrlSta == GPS_CTRL_STA_MSG_TEST_IDLE)
    {
        //lq 等待关机
    }
    else
    {
        GPSCtrlMsgTestPara.GpsCtrlSta = GPS_CTRL_STA_MSG_TEST_START;
        usart_printf("[%6d:%4d]$MsgCtrlStateMsgTest : GPS_CTRL_STA_MSG_TEST_START!\r\n",
                     GetFrameNum(), GetSlotNum());	                            

    }

}

/*******************************************************************************
* 名称  : GetMsgTestSendFlg
* 描述  : 获取用户测试模式消息发送标志
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GetMsgTestSendFlg(void)
{
    if (GPSCtrlMsgTestPara.GpsCtrlSta == GPS_CTRL_STA_MSG_TEST_IDLE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
/*******************************************************************************
* 名称  : GpsPowerUp
* 描述  : 由省电到正常工作下GPS控制，包括电源与参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void GpsPowerUp(void)
{
	ResetGpsCtrlStru(&GpsCtrlPara);
	ResetGpsMsgStru(&GpsPara);
    ResetGpsSynSta();
    //lq GpsSynCell.GPSSyn_FirstSynFlag = FALSE;    
	LEDInit(LED_TYPE_GPS_SEARCH);

#if !WIN32
	IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_ON);
#endif
}

/*******************************************************************************
* 名称  : GpsWakeUp
* 描述  : 由省电到正常工作下GPS控制，包括参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void GpsWakeUp(void)
{
	ResetGpsCtrlStru(&GpsCtrlPara);
	ResetGpsMsgStru(&GpsPara);
    ResetGpsSynSta();
#if 0
    OP1_GPSOffPositionInstanceReset();
#endif
    //lq GpsSynCell.GPSSyn_FirstSynFlag = FALSE;    
	LEDInit(LED_TYPE_GPS_SEARCH);
}

/*******************************************************************************
* 名称  : GetGpsSleepSta
* 描述  : 获取GPS供电状态,只要GPS不是在定位长开状态和开机不定位首次长开状态，
          便可让MCU休眠，因在GPS定位长开时需要进行本地定时器同GPS的校准
          AtoN:GPS长开，通过报文控制GPS Receiver的工作状态，仅当GPS控制模块处于
          ATON_GPS_CTRL_STA_IDLE时MCU可进入stop休眠模式
* 输入  : 无		  
* 输出  : 无
* 返回  : TRUE表示可以进入省电，FALSE禁止进入省电
*******************************************************************************/
U8 GetGpsSleepSta(void)
{
#if 0
	if ((GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_FIX_LNG_OPEN)
        || (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN && !GetGpsPowerupFixState()))
	{
		return (FALSE);	
	}
	else
	{
		return (TRUE);
	}
#else
	if (GpsCtrlPara.GpsCtrlSta == ATON_GPS_CTRL_STA_IDLE)
	{
		return (TRUE);	
	}
	else
	{
		return (FALSE);
	}
#endif	
}

#if 0 //lq
/*******************************************************************************
* 名称  : SlotSynClac
* 描述  : 时隙同步，时隙相位同步
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SlotSynClac(void)
{
	U16 uiTemp;
	U16 ChkTimeS;

//lqdb    GpsSynCell.GPSSyn_PulseEn = TRUE; 

	if (GpsSynCell.GPSSyn_PulseEn == TRUE)                                      //lq GPS定位后才进行同步
	{                                                                           
		GpsSynCell.GPSSyn_PulseEn = FALSE;                                      //lq 每个秒脉冲（每1s）仅同步1次

		ChkTimeS = GpsPara.GPS_TimeS + 1;                                         //lq 因为该同步是在GPS秒脉冲之后进行的，所以加1（即用于同步的标准时间晚于秒脉冲）。
		if (ChkTimeS >= 60)                                                     //lq ChkTimeS为标准秒（GPS秒）
		{
			ChkTimeS = 0;
		}
		//时隙同步                                                              //lq 时隙同步仅在奇数秒时进行同步
		if ((ChkTimeS & 0x01) == 1)	 //时隙同步每奇数秒进行  times是从gps报文中提取的秒数
		{									//这个gps_timeS这个值如果在GPS的保护状态下，即定位之后保护10秒，则这个值为连续的
		    uiTemp = (ChkTimeS >> 1) * 75 + 37;//这个是当前的slot的值   1秒的slot个数为75/2  这个是奇数秒
			GpsSynCell.GPSSyn_SlotOffset = uiTemp - GpsSynCell.GPSSyn_PulseSlot;//当前的值减去保存的值  //lq ChkTimeS为60s一循环，GPSSyn_PulseSlot为2250slot一循环（也即60s一循环），
			//MinCount = gpsStoreCell.GPS_TimeM;                                                        //lq 所以若ARM时钟与GPS完全同步的话，ChkTimeS与GPSSyn_PulseSlot所表示的时间应完全同步
			GpsSynCell.GPSSyn_SlotSynFlag = TRUE;
		}
        //lq? 偶数秒时的时隙同步？

		//时隙相位同步，每1s进行
		if ((ChkTimeS & 0x01) == 0)//这个是偶数秒                               //lq 偶数秒时进行的相位同步
		{				
			uiTemp = (ChkTimeS >> 1) * 75;//偶数秒所以不需要加那个37
			if (uiTemp > GpsSynCell.GPSSyn_PulseSlot)//若计算的当前slot值大于同步时保存的值 //lq 由于是在偶数秒时进行同步，所以标准相位数一定为0（or 1280）？
			{
				GpsSynCell.GPSSyn_PhaseOffset = 1280 - GpsSynCell.GPSSyn_PulsePhase;    //lq 当前时间比GPS标准时间慢，将相位数置为1280，
			}                                                                           //lq 在其他地方会将相位由1280置为0，同时将时隙加1
			else
			{
				GpsSynCell.GPSSyn_PhaseOffset = -GpsSynCell.GPSSyn_PulsePhase;  //lq 当前时间比GPS标准时间快，将相位数置为0，
			}						
			GpsSynCell.GPSSyn_PhaseSynFlag = TRUE;
		}
		else                                                                    //lq 奇数秒时进行的相位同步
		{
			GpsSynCell.GPSSyn_PhaseOffset = 640 - GpsSynCell.GPSSyn_PulsePhase;	//lq? 整秒时不应将相位置为0 or 1280?					
			GpsSynCell.GPSSyn_PhaseSynFlag = TRUE;
		}
/*lq
		usart_printf("GpsSynCell.GPSSyn_PhaseOffset %d\r\n",GpsSynCell.GPSSyn_PhaseOffset);
		usart_printf("GpsSynCell.GPSSyn_SlotOffset %d\r\n",GpsSynCell.GPSSyn_SlotOffset);
*/
		if ((GpsSynCell.GPSSyn_PhaseOffset > -15)
		    && (GpsSynCell.GPSSyn_PhaseOffset < 15)
		    && (GpsSynCell.GPSSyn_SlotOffset == 0))
		 {
		    GpsCtrlPara.GpsSynCount++;
			usart_printf("Gps Syn Count %d\r\n",GpsCtrlPara.GpsSynCount);

            if (GpsCtrlPara.GpsSynCount >= GPS_SYN_COUNT_THRESHOLD)             //lq 连续GPS_SYN_COUNT_THRESHOLD次满足条件，
			{                                                                   //lq 才认为同步完成。
				GpsCtrlPara.GPSSyn_Flag = TRUE;	
				GpsSynCell.GPSSyn_Flag = TRUE;
			}
		 }
         else 
         {
            GpsCtrlPara.GpsSynCount = 0;
         }
	}
}
#else
/*******************************************************************************
* 名称  : SlotSynClac
* 描述  : 时隙同步，时隙相位同步，时隙和相位同时同步
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SlotSynClac(void)
{
	U16 uiTemp;
	U16 ChkTimeS;
    S32 slPhaseDiffSum;

	if (GpsSynCell.GPSSyn_PulseEn == TRUE)                                      //lq GPS定位后才进行同步
	{                                                                           
		GpsSynCell.GPSSyn_PulseEn = FALSE;                                      //lq 每个秒脉冲（每1s）仅同步1次

        ChkTimeS = GpsSynCell.ChkTimeS;
    	if (ChkTimeS >= 60)                                                     //lq ChkTimeS为标准秒（GPS秒）
		{
			ChkTimeS = 0;

#if CONFIG_GPS_SYNCH_HOUR_MIN
            GpsSynCell.ChkTimeM++;
            if (GpsSynCell.ChkTimeM >= 60)
            {
    			GpsSynCell.ChkTimeM = 0;
                GpsSynCell.ChkTimeH++;
                if (GpsSynCell.ChkTimeH >= 24)
                {
                    GpsSynCell.ChkTimeH = 0;
                }            
            }
#endif
		}

		/*lq 时隙相位同步*/
		if ((ChkTimeS & 0x01) == 1)	                                            //lq 奇数秒同步
		{									
		    uiTemp = (ChkTimeS >> 1) * 75 + 37;
			
            /*lq 时隙同步*/
            GpsSynCell.GPSSyn_SlotOffset = uiTemp - GpsSynCell.GPSSyn_PulseSlot;//lq ChkTimeS为60s一循环，GPSSyn_PulseSlot为2250slot一循环（也即60s一循环），
                                                                                //lq 所以若ARM时钟与GPS完全同步的话，ChkTimeS与GPSSyn_PulseSlot所表示的时间应完全同步
#if 1
    		/*lq 边界处理，本地时隙比标准时隙慢，当标准时隙为2250（即秒为0）时，本地时隙小于2250*/
    		if (GpsSynCell.GPSSyn_SlotOffset <= -2000)//lq 2212
    		{
    			GpsSynCell.GPSSyn_SlotOffset += 2250;	
    		}
            else if (GpsSynCell.GPSSyn_SlotOffset >= 2000)
            {
    			GpsSynCell.GPSSyn_SlotOffset -= 2250;	            
            }
#endif

            /*lq 相位同步*/
			GpsSynCell.GPSSyn_PhaseOffset = 640 - GpsSynCell.GPSSyn_PulsePhase; //lq 1s等于37.5slot，故奇数秒时对应的相位值为640（1slot=640phase）
            
            if (GpsSynCell.GPSSyn_Enable == TRUE)
            {
                GpsSynCell.ucGPSSyn_SlotPhaseSynFlag = TRUE;
#if CONFIG_GPS_SYNCH_HOUR_MIN                 
                /*lq 小时分钟同步*/                
                if (GpsSynCell.GPSSyn_HourFrameSynEnable == TRUE)
                {
                    GpsSynCell.ChkTimeFrame = (GpsSynCell.ChkTimeH * 60 + GpsSynCell.ChkTimeM) % 1440;  //lq 1440=24*60
                    GpsSynCell.ucGPSSyn_HourFrameSynFlag = TRUE;                

			        debug_printf("$[%6d:%4d]GPSSyn_HourFrameSynFlag : OK!\r\n", GetFrameNum(), GetSlotNum());
                }
#endif
            }	        
    	}
        else                                                                    //lq 偶数秒同步
        {
			uiTemp = (ChkTimeS >> 1) * 75;
            
            /*lq 时隙同步*/
            GpsSynCell.GPSSyn_SlotOffset = uiTemp - GpsSynCell.GPSSyn_PulseSlot;
#if 1
    		/*lq 边界处理，本地时隙比标准时隙慢，当标准时隙为2250（即秒为0）时，本地时隙小于2250*/
    		if (GpsSynCell.GPSSyn_SlotOffset <= -2000)
    		{
    			GpsSynCell.GPSSyn_SlotOffset += 2250;	
    		}
            else if (GpsSynCell.GPSSyn_SlotOffset >= 2000)
            {
    			GpsSynCell.GPSSyn_SlotOffset -= 2250;	            
            }
#endif
            /*lq 相位同步*/
			GpsSynCell.GPSSyn_PhaseOffset = 0 - GpsSynCell.GPSSyn_PulsePhase;   //lq 1s等于37.5slot，故偶数秒时对应的相位值为0

            if (GpsSynCell.GPSSyn_Enable == TRUE)
            {
                GpsSynCell.ucGPSSyn_SlotPhaseSynFlag = TRUE;
#if CONFIG_GPS_SYNCH_HOUR_MIN                
                /*lq 小时分钟同步*/
                if (GpsSynCell.GPSSyn_HourFrameSynEnable == TRUE)
                {
                    GpsSynCell.ChkTimeFrame = (GpsSynCell.ChkTimeH * 60 + GpsSynCell.ChkTimeM) % 1440;  //lq 1440=24*60
                    GpsSynCell.ucGPSSyn_HourFrameSynFlag = TRUE;                

			        debug_printf("$[%6d:%4d]GPSSyn_HourFrameSynFlag : OK!\r\n", GetFrameNum(), GetSlotNum());
                }
#endif
            }
        }

		debug_printf("GPSTimeS     %d\r\n", GpsPara.GPS_TimeS);
		debug_printf("GPSTimeSLeap %d\r\n", GpsPara.GPS_TimeSLeap);
        debug_printf("ChkTimeS     %d\r\n", ChkTimeS);
    	debug_printf("GpsSynCell.GPSSyn_PulseSlot %d\r\n", GpsSynCell.GPSSyn_PulseSlot);
		debug_printf("GpsSynCell.GPSSyn_PhaseOffset %d\r\n", GpsSynCell.GPSSyn_PhaseOffset);
		debug_printf("GpsSynCell.GPSSyn_SlotOffset  %d\r\n", GpsSynCell.GPSSyn_SlotOffset);

        /*lq 判断同步偏差值是否在设定范围内*/
		slPhaseDiffSum = GpsSynCell.GPSSyn_PhaseOffset + GpsSynCell.GPSSyn_SlotOffset * 1280;
        if (slPhaseDiffSum > -15 && slPhaseDiffSum < 15)
		{
		    if (GpsSynCell.ucGPSSyn_SlotPhaseSynFlag == FALSE)                  //lq 此轮（秒）同步校准已完成，同步偏差值清0    
            {
                GpsSynCell.GPSSyn_SlotOffset = 0;
                GpsSynCell.GPSSyn_PhaseOffset = 0;		            
            }

            GpsCtrlPara.GpsSynCount++;
			if (GpsCtrlPara.GpsSynCount >= GPS_SYN_COUNT_THRESHOLD)             //lq 连续GPS_SYN_COUNT_THRESHOLD次满足条件，
			{                                                                   //lq 才认为同步完成。
                GpsCtrlPara.GpsSynCount = GPS_SYN_COUNT_THRESHOLD;              //lq GPS一直工作时，GPS同步会一直进行，防止计数溢出
				GpsCtrlPara.GPSSyn_Flag = TRUE;	
				GpsSynCell.GPSSyn_Flag = TRUE;
			}

			debug_printf("Gps Syn Count %d\r\n", GpsCtrlPara.GpsSynCount);
		 }
         else 
         {
            GpsCtrlPara.GpsSynCount = 0;
         }
	}
}
#endif

/*******************************************************************************
* 名称  : GetGpsSynSta
* 描述  : 时隙同步，时隙相位同步
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GetGpsSynSta(void)
{
	if (GpsSynCell.GPSSyn_Flag == TRUE)
	{
		GpsSynCell.GPSSyn_Flag = FALSE;

		return (TRUE);
	}	

	return (FALSE);
}

/*******************************************************************************
* 名称  : ResetGpsSynSta
* 描述  : 时隙同步，时隙相位同步
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ResetGpsSynSta(void)
{
    GpsSynCell.GPSSyn_Flag = FALSE;
}

#if 0
/*******************************************************************************
* 名称  : GetGpsMsgRequest
* 描述  : GPS消息发送请求查询
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/	
U8 GetGpsMsgRequest(void)
{
	if (GpsCtrlPara.GpsMsgEnable == TRUE)
	{
		GpsCtrlPara.GpsMsgEnable = FALSE;
		return(TRUE);
	}
	else
	{
		return(FALSE);
	}			
}

/*******************************************************************************
* 名称  : SetGpsMsgRequest
* 描述  : GPS消息发送请求发起
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetGpsMsgRequest(void)
{
	GpsCtrlPara.GpsMsgEnable = TRUE;                                            //lq 请求往协议栈发消息
}
#endif

/*******************************************************************************
* 名称  : GetGpsPowerUpFixState
* 描述  : 查询第一次GPS定位且同步是否完成
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GetGpsPowerupFixState(void)
{
	return (GpsCtrlPara.GpsPowerupFix);
}

/*******************************************************************************
* 名称  : SetGpsPowerupFixState
* 描述  : 置位第一次GPS定位且同步
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetGpsPowerupFixState(void)
{
	GpsCtrlPara.GpsPowerupFix = TRUE;
}

/*******************************************************************************
* 名称  : GetGpsNormalFixState
* 描述  : 查询正常GPS定位且同步是否完成
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GetGpsNormalFixStateRst(void)
{
	if (GpsCtrlPara.GpsNormalFix == TRUE)
	{
		GpsCtrlPara.GpsNormalFix = FALSE;
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}

/*******************************************************************************
* 名称  : GetGpsNormalFixState
* 描述  : 查询正常GPS定位且同步是否完成
* 输入  : 无		  
* 输出  : 无
* 返回  : 正常GPS定位且同步是否完成
*******************************************************************************/
U8 GetGpsNormalFixState(void)
{
    return GpsCtrlPara.GpsNormalFix;
}

/*******************************************************************************
* 名称  : ResetGpsNormalFixState
* 描述  : 复位GPS定位标志
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ResetGpsNormalFixState(void)
{
	GpsCtrlPara.GpsNormalFix = FALSE;
}

/*******************************************************************************
* 名称  : SetGpsNormalFixState
* 描述  : 置位GPS定位标志
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetGpsNormalFixState(void)
{
	GpsCtrlPara.GpsNormalFix = TRUE;
}

#if 0
/*******************************************************************************
* 名称  : GetGpsIdelState
* 描述  : 查询GPS是否因为自身超时休眠了
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GetGpsIdelState(void)
{
	if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_IDLE)
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
* 名称  : IsGpsFix
* 描述  : 判断GPS是否定位
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 IsGpsFix(void)
{
	if (GpsPara.GPS_Stat == 'A')
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}

/*******************************************************************************
* 名称  : IsGpsPowerupProtect
* 描述  : GPS是否在开机定位保护状态（M4508D即为定位状态）,本地时钟偏差统计计
          算函数TimAdjustProcess()中会调用该函数。
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 IsGpsPowerupProtect(void)
{
	if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_FIXED)
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}

/*******************************************************************************
* 名称  : SetGpsOnTimSlot
* 描述  : 设置GPS开启定时时隙
* 输入  : value - GPS开启定时时隙		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetGpsOnTimSlot(S16 value)	
{
    if (value < 0)
    {
        value = 0;    
    }

    GpsCtrlPara.GpsOnTimSlot = value;
}	
	
/*******************************************************************************
* 名称  : SetGpsOnTimFrame
* 描述  : 设置GPS开启定时帧
* 输入  : value - GPS开启定时帧		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetGpsOnTimFrame(S32 value)	
{
    if (value < 0)
    {
        value = 0;    
    }

    GpsCtrlPara.GpsOnTimFrame = value;
}

/*******************************************************************************
* 名称  : GetGpsOnTim
* 描述  : 获取GPS定时
* 输入  : 无		  
* 输出  : 无
* 返回  : GPS定时
*******************************************************************************/
S32 GetGpsOnTim(void)	
{
    return GpsCtrlPara.GpsOnTimFrame * 2250 + GpsCtrlPara.GpsOnTimSlot;
}	

/*******************************************************************************
* 名称  : SetGpsOnTimEn
* 描述  : GPS开启定时有效标志置位
* 输入  : 无		  
* 输出  : 无
* 返回  : GPS定时
*******************************************************************************/
void SetGpsOnTimEn(void)
{
    GpsCtrlPara.ucGpsOnTimEn = TRUE;
}

/*******************************************************************************
* 名称  : ResetGpsOnTimEn
* 描述  : GPS开启定时有效标志复位
* 输入  : 无		  
* 输出  : 无
* 返回  : GPS定时
*******************************************************************************/
void ResetGpsOnTimEn(void)
{
    GpsCtrlPara.ucGpsOnTimEn = FALSE;
}

/*******************************************************************************
* 名称  : GetGpsOnTimEn
* 描述  : 获取GPS开启定时有效标志
* 输入  : 无		  
* 输出  : 无
* 返回  : GPS开启定时有效标志
*******************************************************************************/
U8 GetGpsOnTimEn(void)
{
    return GpsCtrlPara.ucGpsOnTimEn;
}

/*******************************************************************************
* 名称  : GetGpsOnTimEnRst
* 描述  : 获取GPS开启定时有效标志，若有效则复位
* 输入  : 无		  
* 输出  : 无
* 返回  : GPS开启定时有效标志
*******************************************************************************/
U8 GetGpsOnTimEnRst(void)
{
    if (GetGpsOnTimEn() == TRUE)
    {
        ResetGpsOnTimEn();
        
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SetGpsOffTimSlot
* 描述  : 设置GPS关闭定时时隙
* 输入  : value - GPS关闭定时时隙		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetGpsOffTimSlot(S16 value)	
{
    if (value < 0)
    {
        value = 0;    
    }

    GpsCtrlPara.GpsOffTimSlot = value;
}

/*******************************************************************************
* 名称  : SetGpsOffTimFrame
* 描述  : 设置GPS关闭定时帧
* 输入  : value - GPS关闭定时帧		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetGpsOffTimFrame(S32 value)	
{
    if (value < 0)
    {
        value = 0;    
    }

    GpsCtrlPara.GpsOffTimFrame = value;
}

/*******************************************************************************
* 名称  : GetGpsOffTim
* 描述  : 获取GPS定时
* 输入  : 无		  
* 输出  : 无
* 返回  : GPS定时
*******************************************************************************/
S32 GetGpsOffTim(void)	
{
    return GpsCtrlPara.GpsOffTimFrame * 2250 + GpsCtrlPara.GpsOffTimSlot;
}
#if 0	
/*******************************************************************************
* 名称  : SetUnfixPeriodTim
* 描述  : 设置开机未定位时1h周期定时
* 输入  : value - 周期值	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetGpsUnfixPeriodTim(S32 value)	
{
    if (value < 0)
    {
        value = 0;    
    }

    GpsCtrlPara.GpsUnfixPeriodTim += value;
}

/*******************************************************************************
* 名称  : ResetUnfixPeriodTim
* 描述  : 复位开机未定位时1h周期定时
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ResetGpsUnfixPeriodTim(void)	
{
    GpsCtrlPara.GpsUnfixPeriodTim = 0;
}

/*******************************************************************************
* 名称  : GetUnfixPeriodTim
* 描述  : 获取开机未定位时值
* 输入  : 无	  
* 输出  : 无
* 返回  : 开机未定位时值
*******************************************************************************/
S32 GetGpsUnfixPeriodTim(void)	
{
    return GpsCtrlPara.GpsUnfixPeriodTim;
}
#endif
/*******************************************************************************
* 名称  : GetGpsWakeupSta
* 描述  : GPS唤醒定时是否到
* 输入  : 无	  
* 输出  : 无
* 返回  : GPS唤醒状态
*******************************************************************************/
U8 GetGpsWakeupSta(void)
{
    if (GetSlotSum() >= GetGpsOnTim())
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : GetGpsPowerSta
* 描述  : GPS供电状态
* 输入  : 无	  
* 输出  : 无
* 返回  : GPS供电状态
*******************************************************************************/
U8 GetGpsPowerSta(void)
{
    if (GpsCtrlPara.GpsPowerSta == GPS_CTRL_GPSPOWERUP)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : GpsPowerDown
* 描述  : GPS关闭处理
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void GpsPowerDown(void)
{
    IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_OFF);
    
    GpsCtrlPara.GpsPowerSta = GPS_CTRL_GPSPOWERDOWN;
}

/*******************************************************************************
* 名称  : ResetGps5minLngOpenEn
* 描述  : GPS5min长开使能标志复位
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ResetGps5minLngOpenEn(void)
{
    GpsCtrlPara.ucGps5minLngOpenEn = FALSE;
}

/*******************************************************************************
* 名称  : GetGps5minLngOpenEn
* 描述  : 获取GPS5min长开使能标志
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GetGps5minLngOpenEn(void)
{
    return GpsCtrlPara.ucGps5minLngOpenEn;
}

/*******************************************************************************
* 名称  : SetGpsUnfix1hPeriodTimFrame
* 描述  : GPS未定位1h定时
* 输入  : value - 定时值（单位：帧）	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetGpsUnfix1hPeriodTimFrame(S32 value)
{
    if (value < 0)
    {
        value = GetFrameNum() + 60;
    }
    
    GpsCtrlPara.GpsUnfix1hPeriodTimFrame = value;
}

/*******************************************************************************
* 名称  : SetGpsUnfix1hPeriodTimSlot
* 描述  : GPS未定位1h定时
* 输入  : value - 定时值（单位：时隙）	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetGpsUnfix1hPeriodTimSlot(S16 value)
{
    if (value < 0)
    {
        value = GetSlotNum();
    }
    
    GpsCtrlPara.GpsUnfix1hPeriodTimSlot = value;
}

/*******************************************************************************
* 名称  : GetGpsUnfix1hPeriodTim
* 描述  : 获取GPS未定位1h定时
* 输入  : value - 定时值（单位：帧）	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
S32 GetGpsUnfix1hPeriodTim(void)
{
    return GpsCtrlPara.GpsUnfix1hPeriodTimFrame * 2250 + GpsCtrlPara.GpsUnfix1hPeriodTimSlot;    
}

/*******************************************************************************
* 名称  : SetGpsUnfixLonOpenFlg
* 描述  : GPS未定位长开标志置位
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetGpsUnfixLonOpenFlg(void)
{
    GpsCtrlPara.GpsWaitFix = TRUE;
}

/*******************************************************************************
* 名称  : ResetGpsUnfixLonOpenFlg
* 描述  : GPS未定位长开标志复位
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ResetGpsUnfixLonOpenFlg(void)
{
    GpsCtrlPara.GpsWaitFix = FALSE;
}

/*******************************************************************************
* 名称  : GetGpsUnfixLonOpenFlg
* 描述  : 获取GPS未定位长开标志
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GetGpsUnfixLonOpenFlg(void)
{
    return GpsCtrlPara.GpsWaitFix;
}

/*******************************************************************************
* 名称  : SetGpsUnfix1hLngOpenFlg
* 描述  : GPS未定位1h长开标志置位
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetGpsUnfix1hLngOpenFlg(void)
{
    GpsCtrlPara.ucGpsUnfix1hLngOpenFlg = TRUE;
}

/*******************************************************************************
* 名称  : ResetGpsUnfix1hLngOpenFlg
* 描述  : GPS未定位1h长开标志复位
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ResetGpsUnfix1hLngOpenFlg(void)
{
    GpsCtrlPara.ucGpsUnfix1hLngOpenFlg = FALSE;
}

/*******************************************************************************
* 名称  : GetGpsUnfix1hLngOpenFlg
* 描述  : 获取GPS未定位1h长开标志
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GetGpsUnfix1hLngOpenFlg(void)
{
    return GpsCtrlPara.ucGpsUnfix1hLngOpenFlg;
}

/*******************************************************************************
* 名称  : SetGpsUnfixBey1hLngOpenFlg
* 描述  : GPS未定位超过1h长开标志置位
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetGpsUnfixBey1hLngOpenFlg(void)
{
    GpsCtrlPara.ucGpsUnfixBey1hLngOpenFlg = TRUE;
}

/*******************************************************************************
* 名称  : ResetGpsUnfixBey1hLngOpenFlg
* 描述  : GPS未定位超过1h长开标志复位
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ResetGpsUnfixBey1hLngOpenFlg(void)
{
    GpsCtrlPara.ucGpsUnfixBey1hLngOpenFlg = FALSE;
}

/*******************************************************************************
* 名称  : GetGpsUnfixBey1hLngOpenFlg
* 描述  : 获取GPS未定位超过1h长开标志
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GetGpsUnfixBey1hLngOpenFlg(void)
{
    return GpsCtrlPara.ucGpsUnfixBey1hLngOpenFlg;
}

/*******************************************************************************
* 名称  : IsGpsOffTimeout
* 描述  : 是否Gps关闭定时到
* 输入  : 无	  
* 输出  : 无
* 返回  : 是否Gps关闭定时到
*******************************************************************************/
U8 IsGpsOffTimeout(void)
{
    if (GetSlotSum() >= GetGpsOffTim())
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : IsGpsOnTimeout
* 描述  : 是否Gps开启定时到
* 输入  : 无	  
* 输出  : 无
* 返回  : 是否Gps开启定时到
*******************************************************************************/
U8 IsGpsOnTimeout(void)
{
    if (GetSlotSum() >= GetGpsOnTim())
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : IsGpsUnfix1hPeriodTimeout
* 描述  : 是否Gps未定位1h定时时到
* 输入  : 无	  
* 输出  : 无
* 返回  : 是否Gps未定位1h定时时到
*******************************************************************************/
U8 IsGpsUnfix1hPeriodTimeout(void)
{
    if (GetSlotSum() >= GetGpsUnfix1hPeriodTim())
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }
}
#if 0
/*******************************************************************************
* Function Name  : SetGpsOnTim
* Description    : 设置GPS开启定时，不是每次组消息发送时都进行GPS开启定时
* Input          : usValue - GPS启动定时和发射定时的时隙差值
* Output         : None
* Return         : None
*******************************************************************************/
void SetGpsOnTim(S16 usValue)
{
    S32 slSlotNums;

    if (usValue < 0)
    {
        usValue = SLOT_INTERVAL_GPS_ON_TO_TX;
    }
     
    /*lq GPS开启定时无效*/
    if (GetGpsOnTimEn() == FALSE)
    {
        /*lq GPS关闭后才可确定GPS的定位状态，进而确定GPS的唤醒周期*/
        if (GetGpsPowerSta() == FALSE)
        {
            /*lq GPS未定位1h唤醒定时*/
            if (GetGpsUnfix1hLngOpenFlg() == TRUE)      
            {
                /*lq 消息发送周期计数*/
                if (++GpsCtrlPara.GpsIntervalCnt > GPS_CTRL_PERIOD_UNFIX_1HOUR_LONG_OPEN)
                {
                    SetGpsOnEn();    
                    GpsCtrlPara.GpsIntervalCnt = 0;
                }
                else
                {
                    ResetGpsOnEn();    
                }
                
                if (GetGpsOnEn() == FALSE)
                {
                    /*lq 若GPS省电条件满足，且下个周期不开启GPS*/
                    ResetGpsOnTimEn();                 
                }
                else
                {
                    slSlotNums = struSOTDMAPara.BurstTxTimFrame * 2250 + struSOTDMAPara.BurstTxTimSlot;
                    slSlotNums -= usValue;
                    if (slSlotNums < 0)
                    {
                        slSlotNums = 0;
                    }
                
                    SetGpsOnTimFrame(slSlotNums / 2250);
                    SetGpsOnTimSlot(slSlotNums % 2250);
    
                    /*lq GPS开启定时使能*/
                    SetGpsOnTimEn();        
                    SetGpsOnEn(); 

                    usart_printf("$GPS On Tim :[%6d:%4d]\r\n", GpsCtrlPara.GpsOnTimFrame, GpsCtrlPara.GpsOnTimSlot);                
                }
            }

            /*lq GPS未定位超过1h唤醒定时*/
            else if (GetGpsUnfixBey1hLngOpenFlg() == TRUE)
            {
                /*lq 消息发送周期计数*/
                if (++GpsCtrlPara.GpsIntervalCnt > GPS_CTRL_PERIOD_UNFIX_BEY_1HOUR_LONG_OPEN)
                {
                    SetGpsOnEn();    
                    GpsCtrlPara.GpsIntervalCnt = 0;                    
                }
                else
                {
                    ResetGpsOnEn();    
                }
                
                if (GetGpsOnEn() == FALSE)
                {
                    /*lq 若GPS省电条件满足，且下个周期不开启GPS*/
                    ResetGpsOnTimEn();                 
                }
                else
                {
                    slSlotNums = struSOTDMAPara.BurstTxTimFrame * 2250 + struSOTDMAPara.BurstTxTimSlot;
                    slSlotNums -= usValue;
                    if (slSlotNums < 0)
                    {
                        slSlotNums = 0;
                    }
                
                    SetGpsOnTimFrame(slSlotNums / 2250);
                    SetGpsOnTimSlot(slSlotNums % 2250);
    
                    /*lq GPS开启定时使能*/
                    SetGpsOnTimEn();        
                    SetGpsOnEn(); 

                    usart_printf("$GPS On Tim :[%6d:%4d]\r\n", GpsCtrlPara.GpsOnTimFrame, GpsCtrlPara.GpsOnTimSlot);                
                }
            }

            /*lq GPS正常唤醒定时*/
            else
            {
                /*lq GPS省电模式是否开启*/
                if (GetGpsSaveFlg() == TRUE)
                {
                    /*lq GPS下个周期是否需开启*/
                    if (GetGpsOnEn() == TRUE)
                    {
                        ResetGpsOnEn();    
                    }
                    else
                    {
                        SetGpsOnEn();    
                    }        
                }
                else
                {
                    SetGpsOnEn();    
                }

                if ((GetGpsSaveFlg() == TRUE) && (GetGpsOnEn() == FALSE))
                {
                    /*lq 若GPS省电条件满足，且下个周期不开启GPS*/
                    ResetGpsOnTimEn();                 
                }
                else
                {
                    slSlotNums = struSOTDMAPara.BurstTxTimFrame * 2250 + struSOTDMAPara.BurstTxTimSlot;
                    slSlotNums -= usValue;
                    if (slSlotNums < 0)
                    {
                        slSlotNums = 0;
                    }
                
                    SetGpsOnTimFrame(slSlotNums / 2250);
                    SetGpsOnTimSlot(slSlotNums % 2250);
    
                    /*lq GPS开启定时使能*/
                    SetGpsOnTimEn();        
                    SetGpsOnEn(); 

                    usart_printf("$GPS On Tim :[%6d:%4d]\r\n", GpsCtrlPara.GpsOnTimFrame, GpsCtrlPara.GpsOnTimSlot);                
                }

            }
        }    
    }
}
#endif
/*******************************************************************************
* Function Name  : GetGpsOnEnRst
* Description    : 获取GPS开启状态,若为真则复位
* Input          : None
* Output         : None
* Return         : GPS开启状态
*******************************************************************************/
U8 GetGpsOnEnRst(void)
{
    if (GpsCtrlPara.ucGpsOnEn == TRUE)
    {
        GpsCtrlPara.ucGpsOnEn = FALSE;
        
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* Function Name  : GetGpsOnEn
* Description    : 获取GPS开启状态
* Input          : None
* Output         : None
* Return         : GPS开启标志
*******************************************************************************/
U8 GetGpsOnEn(void)
{
    if (GpsCtrlPara.ucGpsOnEn == TRUE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* Function Name  : SetGpsOnEn
* Description    : 置位GPS开启标志
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetGpsOnEn(void)
{
    GpsCtrlPara.ucGpsOnEn = TRUE;
}

/*******************************************************************************
* Function Name  : ResetGpsOnEn
* Description    : 复位GPS开启标志
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ResetGpsOnEn(void)
{
    GpsCtrlPara.ucGpsOnEn = FALSE;
}

/*******************************************************************************
* Function Name  : SetGpsOffPositionSampleEn
* Description    : 置位GPS偏移采样功能
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetGpsOffPositionSampleEn(void)
{
    GpsCtrlPara.GpsOffPositionSampleEn = TRUE;
}

/*******************************************************************************
* Function Name  : ResetGpsOffPositionSampleEn
* Description    : 复位GPS偏移采样功能
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ResetGpsOffPositionSampleEn(void)
{
    GpsCtrlPara.GpsOffPositionSampleEn = FALSE;
}

/*******************************************************************************
* Function Name  : GetGpsOffPositionSampleEn
* Description    : 查询GPS偏移采样功能
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
U8 GetGpsOffPositionSampleEn(void)
{
    return GpsCtrlPara.GpsOffPositionSampleEn;
}

/*******************************************************************************
* Function Name  : SetGpsOffPositionSampleEn
* Description    : 置位GPS采样新点标志
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetGpsPositionSampleNewFlg(void)
{
    GpsCtrlPara.GpsPositionSampleNewFlg = TRUE;
}

/*******************************************************************************
* Function Name  : ResetGpsPositionSampleNewFlg
* Description    : 复位GPS采样新点标志
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ResetGpsPositionSampleNewFlg(void)
{
    GpsCtrlPara.GpsPositionSampleNewFlg = FALSE;
}

/*******************************************************************************
* Function Name  : GetGpsPositionSampleNewFlgRst
* Description    : 查询GPS采样新点标志，并复位
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
U8 GetGpsPositionSampleNewFlgRst(void)
{
    if (GpsCtrlPara.GpsPositionSampleNewFlg == TRUE)
    {
        GpsCtrlPara.GpsPositionSampleNewFlg = FALSE;
        
        return TRUE;
    }
    else
    {
        return FALSE;    
    }
}

/*******************************************************************************
* Function Name  : GetGpsSaveFlgRst
* Description    : 获取GPS省电标志,若为真则复位
* Input          : None
* Output         : None
* Return         : GPS省电标志
*******************************************************************************/
U8 GetGpsSaveFlgRst(void)
{
    if (GpsCtrlPara.ucGpsSaveFlg == TRUE)
    {
        GpsCtrlPara.ucGpsSaveFlg = FALSE;
        
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* Function Name  : GetGpsSaveFlg
* Description    : 获取GPS省电标志
* Input          : None
* Output         : None
* Return         : GPS省电标志
*******************************************************************************/
U8 GetGpsSaveFlg(void)
{
    if (GpsCtrlPara.ucGpsSaveFlg == TRUE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* Function Name  : SetGpsSaveFlg
* Description    : 置位GPS省电标志
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetGpsSaveFlg(void)
{
    GpsCtrlPara.ucGpsSaveFlg = TRUE;
}

/*******************************************************************************
* Function Name  : ResetGpsSaveFlg
* Description    : 复位GPS省电标志
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ResetGpsSaveFlg(void)
{
    GpsCtrlPara.ucGpsSaveFlg = FALSE;
}

#if CONFIG_GPS_SAVE
/*******************************************************************************
* Function Name  : GpsSaveCondJudge
* Description    : GPS省电条件判断
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GpsSaveCondJudge(void)
{
    if (GpsCtrlPara.GpsSaveJudgeTimeCnt == GetLocalClockTickProcess())
    {
        GpsCtrlPara.GpsSaveJudgeTimeCnt = SetTimePeriodProcess(GpsCtrlPara.GpsSaveJudgeTimePeriod);

        /*lq 条件未满足*/
        if (GetGpsSaveFlg() == FALSE)
        {
            /*lq 工作总时长条件判断*/
            if (eqEquipPara.WorkTimeSum + gslFrameCnt > GPS_SAVE_COND_WORKTIME)
            {
     	        GpsCtrlPara.GpsSaveJudgeCntWorkTime++;	    
                if (GpsCtrlPara.GpsSaveJudgeCntWorkTime > GPS_SAVE_COND_OK_TIME)
                {
      	            GpsCtrlPara.GpsSaveJudgeCntWorkTime = 0;
    
                    SetGpsSaveFlg();    //lq 进入GPS省电模式
			        usart_printf("[%6d:%4d]$Enter GPSSaveMode for WorkTime!\r\n", GetFrameNum(), GetSlotNum());	                                    
                }
            }
            else
            {
     	        GpsCtrlPara.GpsSaveJudgeCntWorkTime = 0;
            }  

            /*lq 电池电压条件判断*/
            if (BS1_GetCurVoltValue() < GPS_SAVE_COND_BATTERYVOLT)
            {
     	        GpsCtrlPara.GpsSaveJudgeCntBatVolt++;	   
                if (GpsCtrlPara.GpsSaveJudgeCntBatVolt > GPS_SAVE_COND_OK_TIME)
                {
      	            GpsCtrlPara.GpsSaveJudgeCntBatVolt = 0;
    
                    SetGpsSaveFlg();    //lq 进入GPS省电模式
			        usart_printf("[%6d:%4d]$Enter GPSSaveMode for BatVolt!\r\n", GetFrameNum() ,GetSlotNum());
                }
            }
            else
            {
     	        GpsCtrlPara.GpsSaveJudgeCntBatVolt = 0;
            } 

            /*lq 温度条件判断*/
            if (struTemperSensorPara.fsTemper < GPS_SAVE_COND_TEMPER)   //lq? GetTemper()的返回值为何不对？
            {
     	        GpsCtrlPara.GpsSaveJudgeCntTemper++;	    
                if (GpsCtrlPara.GpsSaveJudgeCntTemper > GPS_SAVE_COND_OK_TIME)
                {
      	            GpsCtrlPara.GpsSaveJudgeCntTemper = 0;
    
                    SetGpsSaveFlg();    //lq 进入GPS省电模式
			        usart_printf("[%6d:%4d]$Enter GPSSaveMode for Temperature!\r\n", GetFrameNum() ,GetSlotNum());
                }
            }
            else
            {
     	        GpsCtrlPara.GpsSaveJudgeCntTemper = 0;
            }

            /*lq GPS省电条件满足*/
            if (GetGpsSaveFlg() == TRUE)
            {
  	            GpsCtrlPara.GpsSaveJudgeCntWorkTime = 0;
  	            GpsCtrlPara.GpsSaveJudgeCntBatVolt = 0;
  	            GpsCtrlPara.GpsSaveJudgeCntTemper = 0;            
            }                              
        }
        else
        {
            /*lq 工作总时长条件判断*/
            if (eqEquipPara.WorkTimeSum + gslFrameCnt <= GPS_SAVE_COND_WORKTIME)
            {
     	        GpsCtrlPara.GpsSaveJudgeCntWorkTime++;	    
                if (GpsCtrlPara.GpsSaveJudgeCntWorkTime > GPS_SAVE_COND_OK_TIME)
                {
      	            GpsCtrlPara.GpsSaveJudgeCntWorkTime = 0;
    
                    GpsCtrlPara.GpsSaveJudgeFlgWorkTime = TRUE;
			        usart_printf("[%6d:%4d]$GPSSaveMode WorkTime condition not meet!\r\n", GetFrameNum() ,GetSlotNum());
                }
            }
            else
            {
     	        GpsCtrlPara.GpsSaveJudgeCntWorkTime = 0;

                GpsCtrlPara.GpsSaveJudgeFlgWorkTime = FALSE;
            }  
            
            /*lq 电池电压条件判断*/
            if (BS1_GetCurVoltValue() >= GPS_SAVE_COND_BATTERYVOLT)
            {
     	        GpsCtrlPara.GpsSaveJudgeCntBatVolt++;	   
                if (GpsCtrlPara.GpsSaveJudgeCntBatVolt > GPS_SAVE_COND_OK_TIME)
                {
      	            GpsCtrlPara.GpsSaveJudgeCntBatVolt = 0;
    
                    GpsCtrlPara.GpsSaveJudgeFlgBatVolt = TRUE;
			        usart_printf("[%6d:%4d]$GPSSaveMode BatVolt condition not meet!\r\n", GetFrameNum() ,GetSlotNum());
                }
            }
            else
            {
     	        GpsCtrlPara.GpsSaveJudgeCntBatVolt = 0;

                GpsCtrlPara.GpsSaveJudgeFlgBatVolt = FALSE;
            } 

            /*lq 温度条件判断*/
            if (struTemperSensorPara.fsTemper >= GPS_SAVE_COND_TEMPER)      //lq? GetTemper()的返回值为何不对？
            {
     	        GpsCtrlPara.GpsSaveJudgeCntTemper++;	    
                if (GpsCtrlPara.GpsSaveJudgeCntTemper > GPS_SAVE_COND_OK_TIME)
                {
      	            GpsCtrlPara.GpsSaveJudgeCntTemper = 0;
    
                    GpsCtrlPara.GpsSaveJudgeCFlgTemper = TRUE;
			        usart_printf("[%6d:%4d]$GPSSaveMode Temperature condition not meet!\r\n", GetFrameNum() ,GetSlotNum());
                }
            }
            else
            {
     	        GpsCtrlPara.GpsSaveJudgeCntTemper = 0;

                GpsCtrlPara.GpsSaveJudgeCFlgTemper = FALSE;
            }
            
            /*lq GPS省电条件不满足*/                     
            if (GpsCtrlPara.GpsSaveJudgeFlgWorkTime == TRUE
                && GpsCtrlPara.GpsSaveJudgeFlgBatVolt == TRUE
                && GpsCtrlPara.GpsSaveJudgeCFlgTemper == TRUE)
            {
                ResetGpsSaveFlg();  //lq 退出GPS省电模式

  	            GpsCtrlPara.GpsSaveJudgeCntWorkTime = 0;
  	            GpsCtrlPara.GpsSaveJudgeCntBatVolt = 0;
  	            GpsCtrlPara.GpsSaveJudgeCntTemper = 0; 
                
			    usart_printf("[%6d:%4d]$Exit GPSSaveMode!\r\n", GetFrameNum() ,GetSlotNum());	                                    
            }
        } 
    }
}
#endif

#if 0
/*******************************************************************************
* 名称  : GpsCtrlProcess_BatTest
* 描述  : GPS状态机控制,适用于SART电池测试模式的GPS控制。相对于GpsCtrlProcess()
          的区别是GPS进入GPS_CTRL_STA_NORMAL_WORKING工作状态后，不再对连续三次不
          定位次数进行计数，因而会一直按正常定时的状态工作。
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void GpsCtrlProcess_BatTest(void)
{
	U8  ucTemp[10];
	U8  ucTemp1[10];
    S16 ssCurrSlot;
    S32 slCurrFrame;	

    ssCurrSlot = GetSlotNum();
    slCurrFrame = GetFrameNum();

	/*lq GPS时隙相位同步*/
    SlotSynClac();
    
    /*lq GPS省电条件判断*/
    GpsSaveCondJudge();

    /*lq 定位状态的LED指示，仅在GPS开启后才进行LED指示*/
    if (GetGpsPowerSta() == TRUE)
    {
        if (IsGpsFix())
        {
            if (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_GPS_FIX)
            {
    			LEDInit(LED_TYPE_GPS_FIX);
            }
        }
        else
        {
            if (LEDCtrlPara.LedGreenPara.ucType != LED_TYPE_GPS_SEARCH)
            {
    			LEDInit(LED_TYPE_GPS_SEARCH);
            }                                                                      
        }
    }
    
    /*lq GPS关闭后LED的GPS指示停止*/
    else
    {
        if ((LEDCtrlPara.LedGreenPara.ucType == LED_TYPE_GPS_SEARCH)
            || LEDCtrlPara.LedGreenPara.ucType == LED_TYPE_GPS_FIX) 
        {
            LEDInit(LED_TYPE_NONE);    
        }   
    }


    /*lq GPS状态控制*/
	if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_START)                	        //lq 设备开机状态
	{
        /*lq GPS定位且同步*/
        if (GpsCtrlPara.GPSSyn_Flag == TRUE)
        {
            GpsCtrlPara.GPSSyn_Flag = FALSE;

            /*lq GPS定位长开关闭定时*/
            SetGpsOffTimSlot(ssCurrSlot);
            SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_FIX_LONG_OPEN);       //lq GPS定位长开5min

            /*lq GPS5min长开计数加1*/
            GpsCtrlPara.ssGps5minLngOpenCnt++;
            if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
            {
                ResetGps5minLngOpenEn();

#ifdef GPS_CTRL_DEBUG_MSG
			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
            }
#if 0   //lq 暂不写入EEPROM，防止每次开机后上次记录的值被立即覆盖
            /*lq 保存GPS5min长开计数*/
			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);
#endif
            GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_FIX_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
			usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_FIX_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif
        }
        else
        {
            /*lq GPS未定位首次长开关闭定时*/
            SetGpsOffTimSlot(ssCurrSlot);
            SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_FIRST_LONG_OPEN);   //lq GPS未定位首次长开5min

            /*lq GPS未定位1h定时*/
            SetGpsUnfix1hPeriodTimSlot(ssCurrSlot);
            SetGpsUnfix1hPeriodTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_1_HOUR);

            /*lq GPS5min长开计数加1*/
            GpsCtrlPara.ssGps5minLngOpenCnt++;
            if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
            {
                ResetGps5minLngOpenEn();

#ifdef GPS_CTRL_DEBUG_MSG
			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
            }
#if 0   //lq 暂不写入EEPROM，防止每次开机后上次记录的值被立即覆盖
            /*lq 保存GPS5min长开计数*/
			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);
#endif
            GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
			usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif
        }
                                                                            	
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN)
    {
        /*lq GPS定位且同步*/
        if (GpsCtrlPara.GPSSyn_Flag == TRUE)
        {
            GpsCtrlPara.GPSSyn_Flag = FALSE;

            /*复位GPS未定位长开标志*/
            ResetGpsUnfixLonOpenFlg();
            ResetGpsUnfix1hLngOpenFlg();
            ResetGpsUnfixBey1hLngOpenFlg();
            
            /*lq GPS5min长开使能*/
            if (GetGps5minLngOpenEn() == TRUE)        
            {
                /*lq GPS定位长开关闭定时*/
                SetGpsOffTimSlot(ssCurrSlot);
                SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_FIX_LONG_OPEN);   //lq GPS定位长开5min

                /*lq GPS5min长开计数加1*/
                GpsCtrlPara.ssGps5minLngOpenCnt++;
                if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
                {
                    ResetGps5minLngOpenEn();
    
#ifdef GPS_CTRL_DEBUG_MSG
    			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
                }

                /*lq 保存GPS5min长开计数*/
    			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
    		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);
                
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_FIX_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_FIX_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                         
            }
            else
            {
                /*lq 转入GPS关闭状态*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif  
            }
        } 
        else
        {
            /*lq GPS未定位首次长开定时到*/
            if (IsGpsOffTimeout() == TRUE)
            {
                /*lq 置位GPS未定时1h长开标志*/
                SetGpsUnfixLonOpenFlg();
                SetGpsUnfix1hLngOpenFlg();

                /*lq 转入GPS关闭状态*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif 
            }
        }  
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_UNFIX_FIRST_HOUR_LNG_OPEN)
    {
        /*lq GPS定位且同步*/
        if (GpsCtrlPara.GPSSyn_Flag == TRUE)
        {
            GpsCtrlPara.GPSSyn_Flag = FALSE;
            
            /*lq 复位GPS未定位1h长开标志*/
            ResetGpsUnfixLonOpenFlg();
            ResetGpsUnfix1hLngOpenFlg();

            /*lq GPS5min长开使能*/
            if (GetGps5minLngOpenEn() == TRUE)        
            {
                /*lq GPS定位长开关闭定时*/
                SetGpsOffTimSlot(ssCurrSlot);
                SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_FIX_LONG_OPEN);   //lq GPS定位长开5min

                /*lq GPS5min长开计数加1*/
                GpsCtrlPara.ssGps5minLngOpenCnt++;
                if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
                {
                    ResetGps5minLngOpenEn();
    
#ifdef GPS_CTRL_DEBUG_MSG
    			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
                }

                /*lq 保存GPS5min长开计数*/
    			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
    		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);

                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_FIX_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_FIX_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                    
            }
            else
            {
                /*lq 转入GPS关闭状态*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                  
            }
        }
        else
        {
            /*lq GPS未定位1h长开定时到*/
            if (IsGpsOffTimeout() == TRUE)
            {
                /*lq 1h定时到*/
                if (IsGpsUnfix1hPeriodTimeout() == TRUE)
                {
                    /*lq 复位GPS未定1h长开标志*/
                    ResetGpsUnfix1hLngOpenFlg(); 

                    /*lq 置位GPS未定位超过1h长开标志*/
                    SetGpsUnfixBey1hLngOpenFlg();
                }

                /*lq 转入GPS关闭状态*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                  
            }                                        
        }            
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_UNFIX_BEY_FIRST_HOUR_LNG_OPEN)
    {
        /*lq GPS定位且同步*/
        if (GpsCtrlPara.GPSSyn_Flag == TRUE)
        {
            GpsCtrlPara.GPSSyn_Flag = FALSE;

            /*lq 复位GPS未定位超过1h长开标志*/
            ResetGpsUnfixLonOpenFlg();
            ResetGpsUnfixBey1hLngOpenFlg();

            /*lq GPS5min长开使能*/
            if (GetGps5minLngOpenEn() == TRUE)        
            {
                /*lq GPS定位长开关闭定时*/
                SetGpsOffTimSlot(ssCurrSlot);
                SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_FIX_LONG_OPEN);   //lq GPS定位长开5min

                /*lq GPS5min长开计数加1*/
                GpsCtrlPara.ssGps5minLngOpenCnt++;
                if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
                {
                    ResetGps5minLngOpenEn();
    
#ifdef GPS_CTRL_DEBUG_MSG
    			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
                }

                /*lq 保存GPS5min长开计数*/
    			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
    		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);

                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_FIX_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_FIX_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                    
            }
            else
            {
                /*lq 转入GPS关闭状态*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                  
            }            
        }
        else
        {
            /*lq GPS未定位超过1h长开定时到*/
            if (IsGpsOffTimeout() == TRUE)
            {
                /*lq 转入GPS关闭状态*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                      
            }
        }                        
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_FIX_LNG_OPEN)
    {
        if (GetGpsPowerupFixState() == FALSE)
        {
            /*lq 置位GPS开机定位标志*/
            SetGpsPowerupFixState();

			/*lq 记录开机时间*/
            sprintf((char *)ucTemp, "%2d", GpsPara.GPS_TimeMon);
    		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeD);
    		strcat((char *)ucTemp, (char *)ucTemp1);
            sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeH);
    		strcat((char *)ucTemp, (char *)ucTemp1);
    		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeM);
    		strcat((char *)ucTemp, (char *)ucTemp1);
    		Para_Save(PARA_TIME_OPEN_TYPE, ucTemp, PARA_TIME_OPEN_MAXLEN);
        	Para_Save(PARA_TIME_CLOSE_TYPE, ucTemp, PARA_TIME_CLOSE_MAXLEN);                           //lq [2013/04/16]

			/*lq 清除关机原因*/
			sprintf((char *)ucTemp, "%d", 0);     		                        //lq 意外关机只能设为默认关机方式，只有按键关机或超低电量关机时，才会再更改关机原因
			Para_Save(PARA_CLOSE_RESION_TYPE, ucTemp, 1);
			
			/*lq 清除GPS3次未定位计数*/
			sprintf((char *)ucTemp, "%3d", 0);
		   	Para_Save(PARA_GPS_WAIT_COUNT_TYPE, ucTemp, 3);						

			/*lq 清除GPS正常定位计数*/
			sprintf((char *)ucTemp, "%5d", 0);
		   	Para_Save(PARA_GPS_FIX_COUNT_TYPE, ucTemp, 5);
            
            /*lq 清除GPS5min长开计数*/
			sprintf((char *)ucTemp, "%3d", 0);
		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);
        } 
        
        /*lq GPS定位长开定时到*/ 
        if (IsGpsOffTimeout() == TRUE)
        {
            /*lq 计算时隙、相位偏差调整值（在GPS开机定位保护完成后进行）*/
			SetAdjustCalcNow();

            /*lq 转入GPS关闭状态*/            
            GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
		    usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                
        }
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_NORMAL_WORKING)
    {
        /*lq 组消息未开始发送*/
        if (GetBurstStarFlg() == FALSE)
        {
            /*lq GPS定位且同步*/
            if (GpsCtrlPara.GPSSyn_Flag == TRUE)
            {
                GpsCtrlPara.GPSSyn_Flag = FALSE;
                
                /*lq 置位正常定位标志*/
                SetGpsNormalFixState();

                /*lq 定位失败次数清零，正常定位次数加1*/
                GpsCtrlPara.GpsFallCount = 0;
                GpsCtrlPara.GpsFixCount++;
    			sprintf((char *)ucTemp, "%5d", GpsCtrlPara.GpsFixCount);
    		   	Para_Save(PARA_GPS_FIX_COUNT_TYPE, ucTemp, 5);//最多为65536次

                /*lq 转入GPS关闭状态*/            
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;

#ifdef GPS_CTRL_DEBUG_MSG
			    usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                
            }
                    
        }
        else
        {
            /*lq 组消息开始发送，GPS定位失败*/
            GpsCtrlPara.GpsFallCount = 0;   //lq 不对定位失败进行计数，因而一直处于正常定位工作状态
            usart_printf("$Gps Fall Count : %d\r\n", GpsCtrlPara.GpsFallCount);	                
            
            // if (GpsCtrlPara.GpsFallCount >= eqEquipPara.GpsFixFallThres)
            if (0)  //lq 认为每次定位均成功
            {
            	/*lq GPS定位失败次数清零*/
                GpsCtrlPara.GpsFallCount = 0;
                
                /*lq GPS未定位长开次数加1*/
                GpsCtrlPara.GpsWaitCount++;

                /*lq 保存GPS3次未定位次数*/
    			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.GpsWaitCount);
    		   	Para_Save(PARA_GPS_WAIT_COUNT_TYPE, ucTemp, 3);

                /*lq GPS5min长开使能*/
                if (GetGps5minLngOpenEn() == TRUE)        
                {
                    /*lq GPS未定位首次长开关闭定时*/
                    SetGpsOffTimSlot(ssCurrSlot);
                    SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_FIRST_LONG_OPEN);   //lq GPS未定位首次长开5min
            
                    /*lq GPS未定位1h定时*/
                    SetGpsUnfix1hPeriodTimSlot(ssCurrSlot);
                    SetGpsUnfix1hPeriodTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_1_HOUR);

                    /*lq GPS5min长开计数加1*/
                    GpsCtrlPara.ssGps5minLngOpenCnt++;
                    if (GpsCtrlPara.ssGps5minLngOpenCnt >= GPS_CTRL_CNT_THRES_5MIN_LON_OPEN)
                    {
                        ResetGps5minLngOpenEn();
        
#ifdef GPS_CTRL_DEBUG_MSG
        			    UartResponseMsg("$Gps 5min Long Open Disable!");	                                    
#endif
                    }

                    /*lq 保存GPS5min长开计数*/
        			sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
        		   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);

                    GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
			        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                       
                }
                else
                {
                    /*lq 置位未定位1h长开标志*/
                    SetGpsUnfixLonOpenFlg();
                    SetGpsUnfix1hLngOpenFlg();
                    
                    /*lq GPS未定位1h定时*/    
                    SetGpsUnfix1hPeriodTimSlot(ssCurrSlot);
                    SetGpsUnfix1hPeriodTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_1_HOUR);

                    /*lq 转入GPS关闭状态*/
                    GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;           	 

#ifdef GPS_CTRL_DEBUG_MSG
			        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                       
                }
            } 
            else
            {
                /*lq 转入GPS关闭状态*/
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_POWER_DOWN;           	 

#ifdef GPS_CTRL_DEBUG_MSG
			    usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_POWER_DOWN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif                                       
            }        
        }            
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_POWER_DOWN)
    {
        if (GetGpsPowerSta() == TRUE)
        {
            GpsPowerDown();

#ifdef GPS_CTRL_DEBUG_MSG
            UartResponseMsg("$GPS_POWER_OFF!");
#endif
        }
        
        /*lq 组消息开始发送*/
        if (GetBurstStarFlg() == TRUE)
        {
            /*lq... GPS开启定时*/
            SetGpsOnTim(SLOT_INTERVAL_GPS_ON_TO_TX);

            /*lq 转入GPS空闲状态*/            
            GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_IDLE;

#ifdef GPS_CTRL_DEBUG_MSG
			usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_IDLE!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif      
        }     
    }
    else if (GpsCtrlPara.GpsCtrlSta == GPS_CTRL_STA_IDLE)
    {
        /*lq GPS开启状态有效*/
        if (GetMsgGpsPowerup() == TRUE)
        {
            /*lq 开启GPS电源，重置GPS控制相关参数*/                   
            GpsPowerUp();
#ifdef GPS_CTRL_DEBUG_MSG
            UartResponseMsg("$GPS_POWER_ON!");
#endif
            /*lq 根据GPS定位情况选择相应的状态*/
            if (GetGpsUnfix1hLngOpenFlg() == TRUE)                          //lq 转入GPS未定位1h长开状态
            {
                /*lq GPS未定位1h长开定时*/
                SetGpsOffTimSlot(ssCurrSlot);
                SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_1HOUR_LONG_OPEN);   //lq GPS未定位1h内长开1min                    

                /*lq 转入GPS未定位1h长开状态*/                
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_UNFIX_FIRST_HOUR_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_UNFIX_FIRST_HOUR_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif   
            }
            else if (GetGpsUnfixBey1hLngOpenFlg() == TRUE)                  //lq 转入GPS未定位超过1h长开状态        
            {
                /*lq GPS未定位超过1h长开定时*/
                SetGpsOffTimSlot(ssCurrSlot);
                SetGpsOffTimFrame(slCurrFrame + GPS_CTRL_TIME_UNFIX_BEY_1HOUR_LONG_OPEN);   //lq GPS未定位超过1h长开2min                    

                /*lq 转入GPS未定位超过1h长开状态*/                
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_UNFIX_BEY_FIRST_HOUR_LNG_OPEN;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_UNFIX_BEY_FIRST_HOUR_LNG_OPEN!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif   
            }
            else                                                            //lq 转入GPS正常工作状态 
            {
                /*lq 转入GPS正常工作状态*/                
                GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_NORMAL_WORKING;

#ifdef GPS_CTRL_DEBUG_MSG
		        usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_NORMAL_WORKING!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif   
            }
        }
    }
    else
    {
        /*lq 转入GPS开始状态*/                
        GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_START;

#ifdef GPS_CTRL_DEBUG_MSG
	    usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_START!\r\n", slCurrFrame ,ssCurrSlot);	                
#endif   
    }    
}
#endif

/*******************************************************************************
* 名称  : GetPPSGenFlag
* 描述  : PPS生成标志
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GetGpsPPSGenFlag(void)
{
    return GpsPara.GPS_PPSGenFlag;
}

/*******************************************************************************
* 名称  : GetGpsMsgGenFlag
* 描述  : GPS消息生成标志
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GetGpsMsgGenFlag(void)
{
    return GpsPara.GPS_MsgGenFlag;
}

/*******************************************************************************
* 名称  : GetGpsCtrlState
* 描述  : GPS控制状态
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GetGpsCtrlState(void)
{
    return GpsCtrlPara.GpsCtrlSta;
}

/*******************************************************************************
* 名称  : IsCoordinateOK
* 描述  : 定位模式
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 IsCoordinateOK(void)
{
    if (GpsPara.GPS_FixMode == 3)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : IsPdopOK
* 描述  : PDOP
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 IsPdopOK(void)
{
    if (GpsPara.GPS_PDOP < (GPS_DATA_PDOP_CEIL >> 1))
    {
        return TRUE;    //lq 小于最大值的一半则认为OK,否则 too high
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : GPS_GpsSelfCheckStructInit
* 描述  : 初始化结构体
* 输入  : pGpsSelfCheckStruct - GpsSelfCheckStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void GPS_GpsSelfCheckStructInit(GpsSelfCheckStruct *pGpsSelfCheckStruct)
{
	pGpsSelfCheckStruct->TimeCnt = 0;
	pGpsSelfCheckStruct->TimePeriod = GPS_SLFCHK_TIME_PERIOD;	
    pGpsSelfCheckStruct->TimeoutCnt = 0;

    pGpsSelfCheckStruct->CtrlState = GPS_SLFCHK_CTRL_STATE_IDLE;
    pGpsSelfCheckStruct->StartFlg = TRUE;
    pGpsSelfCheckStruct->CompleteFlg = FALSE;
    pGpsSelfCheckStruct->TimepulseFlg = FALSE; 
    pGpsSelfCheckStruct->DataCommFlg = FALSE; 
    pGpsSelfCheckStruct->OnDataCommFlg = FALSE; 
    pGpsSelfCheckStruct->OffDataCommFlg = FALSE; 
    pGpsSelfCheckStruct->AntennaFlg = FALSE; 
    pGpsSelfCheckStruct->ModuleFlg = TRUE; 

    pGpsSelfCheckStruct->VccCtrlOkFlg = FALSE; 
    pGpsSelfCheckStruct->TimepulseOkFlg = FALSE; 
    pGpsSelfCheckStruct->DataCommOkFlg = FALSE; 
    pGpsSelfCheckStruct->AntennaOkFlg = FALSE; 
    pGpsSelfCheckStruct->ModuleOkFlg = FALSE;           
}

/*******************************************************************************
* 名称  : GPS_GpsSelfCheckReset
* 描述  : 结构体复位
* 输入  : pGpsSelfCheckStruct - GpsSelfCheckStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void GPS_GpsSelfCheckReset(GpsSelfCheckStruct *pGpsSelfCheckStruct)
{
    pGpsSelfCheckStruct->StartFlg = FALSE;
    pGpsSelfCheckStruct->CompleteFlg = FALSE;
    pGpsSelfCheckStruct->TimepulseFlg = FALSE; 
    pGpsSelfCheckStruct->DataCommFlg = FALSE; 
    pGpsSelfCheckStruct->OnDataCommFlg = FALSE; 
    pGpsSelfCheckStruct->OffDataCommFlg = FALSE; 
    pGpsSelfCheckStruct->AntennaFlg = FALSE; 
    pGpsSelfCheckStruct->ModuleFlg = TRUE; 

    pGpsSelfCheckStruct->VccCtrlOkFlg = FALSE; 
    pGpsSelfCheckStruct->TimepulseOkFlg = FALSE; 
    pGpsSelfCheckStruct->DataCommOkFlg = FALSE; 
    pGpsSelfCheckStruct->AntennaOkFlg = FALSE; 
    pGpsSelfCheckStruct->ModuleOkFlg = FALSE;         
}

/*******************************************************************************
* 名称  : GPS_SelfCheck
* 描述  : 初始化结构体
* 输入  : pGpsSelfCheckStruct - GpsSelfCheckStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void GPS_SelfCheck(GpsSelfCheckStruct *pGpsSelfCheckStruct)
{
	U32 ulTemp;

    switch (pGpsSelfCheckStruct->CtrlState)
    {
        case GPS_SLFCHK_CTRL_STATE_IDLE:
            
            if (pGpsSelfCheckStruct->StartFlg == TRUE)
            {
                GPS_GpsSelfCheckReset(pGpsSelfCheckStruct);

                /*lq GPS断电 */
            	IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_OFF);      //lq 上电前先断电，确保在GPS电源控制引脚正常的情况下，能够从GPS上电后的开机报文中获取GPS天线状态等GPS模块信息，而不会遗漏GPS上电后的报文
                Delay_ms(50);
                                
                /*lq GPS上电 */
            	IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_ON);
        		pGpsSelfCheckStruct->TimeCnt = SetTimePeriodProcess(pGpsSelfCheckStruct->TimePeriod);               
                pGpsSelfCheckStruct->CtrlState = GPS_SLFCHK_CTRL_STATE_POWERON;            
            }

            break; 
        case GPS_SLFCHK_CTRL_STATE_POWERON:

        	if (pGpsSelfCheckStruct->TimeCnt == GetLocalClockTickProcess())
        	{
        		pGpsSelfCheckStruct->TimeCnt = SetTimePeriodProcess(pGpsSelfCheckStruct->TimePeriod);
                pGpsSelfCheckStruct->TimeoutCnt++;

				/* 消除GPS模块上电后 */
				if(pGpsSelfCheckStruct->TimeoutCnt == 2)
				{
					pGpsSelfCheckStruct->TimepulseFlg = FALSE;				
				}                
                if (pGpsSelfCheckStruct->TimeoutCnt >= GPS_SLFCHK_TIME_TIMEOUT)
                {
                    pGpsSelfCheckStruct->TimeoutCnt = 0;

                    /*lq 数据通信 */
                    if (pGpsSelfCheckStruct->DataCommFlg == TRUE)
                    {
                        pGpsSelfCheckStruct->DataCommOkFlg = TRUE;
                        pGpsSelfCheckStruct->OnDataCommFlg = TRUE;
                    }

                    /*lq 天线 */
                    if (pGpsSelfCheckStruct->AntennaFlg == TRUE)
                    {
                        pGpsSelfCheckStruct->AntennaOkFlg = TRUE;
                    }

                    /*lq GPS模块 */
                    if (pGpsSelfCheckStruct->ModuleFlg == TRUE)
                    {
                        pGpsSelfCheckStruct->ModuleOkFlg = TRUE;
                    }

					ulTemp = SLFCHK_GetCheckSelectValue(); 
					if((ulTemp & SLFCHK_ITEM_MASK_GPS_TIMEPULSE) != SLFCHK_ITEM_MASK_GPS_TIMEPULSE)
					{ 
						/*lq GPS断电 */
	                	IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_OFF);
	                    pGpsSelfCheckStruct->DataCommFlg = FALSE;                                                  
	            		pGpsSelfCheckStruct->TimeCnt = SetTimePeriodProcess(pGpsSelfCheckStruct->TimePeriod);

                    	pGpsSelfCheckStruct->CtrlState = GPS_SLFCHK_CTRL_STATE_POWEROFF; 
					}
					else
					{
						pGpsSelfCheckStruct->CtrlState = GPS_SLFCHK_CTRL_STATE_TIMEPULSE;
					}
                }                        
            }    

            break;
		case GPS_SLFCHK_CTRL_STATE_TIMEPULSE:
			if (pGpsSelfCheckStruct->TimeCnt == GetLocalClockTickProcess())
        	{
        		pGpsSelfCheckStruct->TimeCnt = SetTimePeriodProcess(pGpsSelfCheckStruct->TimePeriod);
                pGpsSelfCheckStruct->TimeoutCnt++;
				if (pGpsSelfCheckStruct->TimeoutCnt >= GPS_SLFCHK_TIME_TIMEOUT_TIMEPULSE)
                {
					/*lq 秒脉冲 */
                    if (pGpsSelfCheckStruct->TimepulseFlg == TRUE)
                    {
                        pGpsSelfCheckStruct->TimepulseOkFlg = TRUE;
                    }

					/*lq GPS断电 */
                	IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_OFF);
                    pGpsSelfCheckStruct->DataCommFlg = FALSE;                                                  
            		pGpsSelfCheckStruct->TimeCnt = SetTimePeriodProcess(pGpsSelfCheckStruct->TimePeriod);

                	pGpsSelfCheckStruct->CtrlState = GPS_SLFCHK_CTRL_STATE_POWEROFF;
				}
			}

			break;
        case GPS_SLFCHK_CTRL_STATE_POWEROFF:

        	if (pGpsSelfCheckStruct->TimeCnt == GetLocalClockTickProcess())
        	{
        		pGpsSelfCheckStruct->TimeCnt = SetTimePeriodProcess(pGpsSelfCheckStruct->TimePeriod);
                pGpsSelfCheckStruct->TimeoutCnt++;                
                if (pGpsSelfCheckStruct->TimeoutCnt >= GPS_SLFCHK_TIME_TIMEOUT_POWERON)
                {
                    pGpsSelfCheckStruct->TimeoutCnt = 0;

                    /*lq 数据通信 */
                    if (pGpsSelfCheckStruct->DataCommFlg == TRUE)
                    {
                        pGpsSelfCheckStruct->OffDataCommFlg = TRUE;
                    }

                    /*lq GPS电源控制 */
                    if ((pGpsSelfCheckStruct->OnDataCommFlg == TRUE)
                        && (pGpsSelfCheckStruct->OffDataCommFlg == FALSE))
                    {
                        pGpsSelfCheckStruct->VccCtrlOkFlg = TRUE;
                    }

                    pGpsSelfCheckStruct->CompleteFlg = TRUE;
                    
                    pGpsSelfCheckStruct->CtrlState = GPS_SLFCHK_CTRL_STATE_IDLE;                                                   
                }                        
            }    
            
            break;
        default :
            
            GPS_GpsSelfCheckStructInit(pGpsSelfCheckStruct);            
            break;
    }   
}

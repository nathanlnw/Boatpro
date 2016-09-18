/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: ITDMA.h
  Author: lq       Version : V1.00        Date: 2013.01.15
  Description:     ARM 进入 Standby 模式时，保存在 Backup Register 中的变量相关的
                   宏、结构体定义，变量、函数声明
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _BKPREGISTER_H
#define _BKPREGISTER_H

/* Includes ------------------------------------------------------------------*/
#include "def.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct BKPRStruct
{
	S16 ssGPS_LongitudH;
	S16 ssGPS_LongitudL;
	S16 ssGPS_LatitudeH;    //lq 精确到0.0001分
	S16 ssGPS_LatitudeL;    //lq 精确到0.0001分
	U16 usGPS_Sog;
	U16 usGPS_Cog;
    U16 usGPS_TimeH;        //lq GPS小时
    U16 usGPS_TimeM;        //lq GPS分钟
    U16 usGpsFallCount;     //lq GPS定位失败计数
    U8  ucGpsWaitFix;       //lq GPS长开定位标志
    U8  ucGpsLngOpenFirstHour;//lq GPS第一小时内长开标志
    U8  ucGpsLngOpenBeyFirstHour;//lq GPS超过一小时长开标志
    U8  ucGps5minLngOpenEn; //lq GPS5min长开使能标志
    U8  ucGpsPowerupFix;	//lq GPS开机定位标志
    U8  ucGpsCtrlSta;		//lq GPS状态机状态

    S16 GpsUnfix1hPeriodTimSlot;        //lq 未定位1h定时
    S16 GpsUnfix1hPeriodTimFrameH;
    S16 GpsUnfix1hPeriodTimFrameL;
	S16 ssGpsStartFrameH;
	S16 ssGpsStartFrameL;
	S16 ssGpsStartSlot;
    S16 ssGpsOffTimFrameH;    
    S16 ssGpsOffTimFrameL;    
    S16 ssGpsOffTimSlot;    
    S16 ucGpsOnTimEn;       //lq GPS开启定时有效
    S16 ucGpsOffTimEn;      //lq GPS关闭定时有效

    S16 ssBurstTxTimFrameH; //lq 组消息发射定时帧高位
    S16 ssBurstTxTimFrameL;
    S16 ssBurstTxTimSlot;   //lq 组消息发射定时时隙

    S8  ucSlotTimeout;      //lq 通信状态—时隙超时
    U16 usSlotOffset;       //lq 通信状态—时隙偏移

    U8  ucRtcLSEorLSI;      //lq 使用内部or外部晶振
    U8  ucRtcAlarmMcuOrGps;	//lq Standby唤醒是因Arm定时orGPS定时
    S16 ssRtcAlarmValue;    //lq Standby休眠时长

    S16 gsiPhaseCnt;        //lq 本地相位计数  
    S16 gsiSlotCnt;         //lq 本地时隙计数  
    S16 gslFrameCntH;       //lq 本地帧计数高位  
    S16 gslFrameCntL;       //lq 本地帧计数低位 
#if 0
	U16 usGpsFallCount;     //GPS定位失败计数
	U8	ucGpsWaitFix;       //GPS长开等定位标志
//    U8  ucShutdownFlg;    //lq 关机or休眠标志位
    U8  ucStandbyMod;
#endif                                                              
    
    S16 TimAdjust1;         //定时器每2秒调整phase数
    S16 TimAdjust10;        //定时器每10秒调整phase数
    S16 TimAdjust100;       //定时器每50秒调整phase数
    S16 TimAdjustPeriodCntLast;//lq 上次计数周期（秒）数
#if 0
    S16 RtcPhaseAdjust1;    //rtc每1秒调整的phase计数
    S16 RtcPhaseAdjust10;   //rtc每10秒调整的phase计数
    S16 RtcPhaseAdjust100;  //rtc每100秒调整的phase计数
#endif
}BKPRStruct;

/* Private macro ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function  ---------------------------------------------------------*/

/* Public  macro ------------------------------------------------------------*/
/*lq 备份域数据寄存器参数索引宏定义*/
#define BKPR_MAXNUM                     42                                      //lq BKP Registers 最大数目
#define BKPR_PARA_MAXNUM                42                                      //lq 存入BKP Registers中的参数的数目

#define BKPR_FLAG_BIT_GPS_LNG_OPEN_FIRST_HOUR_INDEX         0
#define BKPR_FLAG_BIT_GPS_LNG_OPEN_BEY_FIRST_HOUR_INDEX     1
#define BKPR_FLAG_BIT_GPS_5MIN_LONG_OPEN_EN_INDEX           2
#define BKPR_FLAG_BIT_GPS_POWER_UP_FIX_INDEX                3
#define BKPR_FLAG_BIT_GPS_ON_TIM_EN_INDEX                   4
#define BKPR_FLAG_BIT_GPS_OFF_TIM_EN_INDEX                  5
#define BKPR_FLAG_BIT_GPS_LEAP_SECOND_INDEX                 6
#define BKPR_FLAG_BIT_RTC_LSE_OR_LSI_INDEX                  7
#define BKPR_FLAG_BIT_RTC_TIM_PHASE_INDEX                   8
#define BKPR_FLAG_BIT_GPS_SYNCH_ENABLE_INDEX                9   //lq 调试用，可删除
#define BKPR_FLAG_BIT_GPS_SAVE_INDEX                        10
#define BKPR_FLAG_BIT_GPS_ON_INDEX                          11
#define BKPR_FLAG_BIT_EQ_WORKMOD_INDEX                      12  //lq 使用布尔值，只能有两种工作模式

#define BKPR_FLAG_INDEX                         0
#define BKPR_GPS_LONGITUDE_H_INDEX              1
#define BKPR_GPS_LONGITUDE_L_INDEX              2
#define BKPR_GPS_LATITUDE_H_INDEX               3
#define BKPR_GPS_LATITUDE_L_INDEX               4
#define BKPR_GPS_SOG_INDEX                      5
#define BKPR_GPS_COG_INDEX                      6
#define BKPR_RTC_ALARM_VALUE_L_INDEX            7
//#define BKPR_GPS_TIME_HOUR_INDEX                7
//#define BKPR_GPS_TIME_MIN_INDEX                 8
#define BKPR_RTC_TIM_PHASE_L_INDEX              8
#define BKPR_GPS_FALL_COUNT_INDEX               9
#define BKPR_GPS_CTRL_STA_INDEX                 10
#define BKPR_GPS_UNFIX_1H_PERIOD_TIM_FRAME_H_INDEX  11 
#define BKPR_GPS_UNFIX_1H_PERIOD_TIM_FRAME_L_INDEX  12
#define BKPR_GPS_UNFIX_1H_PERIOD_TIM_SLOT_INDEX     13
#define BKPR_GPS_START_FRAME_H_INDEX            14
#define BKPR_GPS_START_FRAME_L_INDEX            15
#define BKPR_GPS_START_SLOT_INDEX               16
#define BKPR_GPS_OFF_FRAME_H_INDEX              17 
#define BKPR_GPS_OFF_FRAME_L_INDEX              18
#define BKPR_GPS_OFF_SLOT_INDEX                 19
#define BKPR_BURST_TX_TIM_FRAME_H_INDEX         20
#define BKPR_BURST_TX_TIM_FRAME_L_INDEX         21
#define BKPR_BURST_TX_TIM_SLOT_INDEX            22
#define BKPR_SLOT_TIMEOUT_INDEX                 23
#define BKPR_SLOT_OFFSET_INDEX                  24
#define BKPR_RTC_TIM_PHASE_H_INDEX              25
#define BKPR_RTC_FREQUENCE_INDEX                26
#define BKPR_RTC_ALARM_MCU_OR_GPS_INDEX         27
#define BKPR_RTC_ALARM_VALUE_H_INDEX            28
#define BKPR_PHASE_CNT_INDEX                    29
#define BKPR_SLOT_CNT_INDEX                     30
#define BKPR_FRAME_CNT_H_INDEX                  31
#define BKPR_FRAME_CNT_L_INDEX                  32
#define BKPR_TIM_ADJUST_1_INDEX                 33
#define BKPR_TIM_ADJUST_10_INDEX                34
#define BKPR_TIM_ADJUST_100_INDEX               35
#define BKPR_TIM_ADJUST_PERIOD_CNT_LAST_INDEX   36
//#define BKPR_RTC_ADJUST_1_INDEX                 37
//#define BKPR_RTC_ADJUST_10_INDEX                38
//#define BKPR_RTC_ADJUST_100_INDEX               39
#define BKPR_GPS_INTERVAL_CNT_INDEX             37
#define BKPR_RAND_SEED_H_INDEX                  40
#define BKPR_RAND_SEED_L_INDEX                  41

#define STANDBY_MOD_SHUTDOWN            0
#define STANDBY_MOD_SLEEPING            1
/* Public  variables ---------------------------------------------------------*/
extern U16 arrBKPDRData[BKPR_MAXNUM];
extern const U8 arrBKP_DR_PARA_NAME[BKPR_PARA_MAXNUM][44];
/* Public  function  ---------------------------------------------------------*/
extern void InitBKP_DRData(void);
extern void SaveBKP_DRData(void);
extern void GetBKP_DRData(void);
extern void BKP_DRDataInit(void);
extern void InitBKP_DR(void);
extern U8 IsRtcAlarm(void);
#endif

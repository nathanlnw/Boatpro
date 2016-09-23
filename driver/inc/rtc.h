/****************************************************************************
*  Copyright (C), 2010, 712.Co.,Ltd.		                                *
*  FileName: rtc.h	 	                                  	            	*
*  Description:  实时时钟													*
*****************************************************************************
*  History:               				                                    *
*      <author>  <time>    <version >   <desc>                              *
*      vozhj   	 10/12/02     1.0       build this moudle		            *
****************************************************************************/
#ifndef __RTC_H
#define __RTC_H

/*lq RTC分频设置宏定义*/
#define RTC_LSE_PRE_VALUE   (64)                        //lq 对应频率512
#define RTC_LSI_PRE_VALUE   (80)                        //lq 对应频率500
#define RTC_LSE_FREQ_VALUE  (32768 / RTC_LSE_PRE_VALUE)
#define RTC_LSI_FREQ_VALUE  (40000 / RTC_LSI_PRE_VALUE)

#define RTC_ALARM_INTERVAL_VALUE (1)                    //lq 对应LSE分频后的一个周期
#define RTC_LSE_ALARM_FREQ_VALUE (RTC_LSE_FREQ_VALUE / RTC_ALARM_INTERVAL_VALUE) //lq 不能整除时不可以使用
#define RTC_LSI_ALARM_FREQ_VALUE (RTC_LSI_FREQ_VALUE / RTC_ALARM_INTERVAL_VALUE) //lq 不能整除时不可以使用

#define RTC_ADJUST_1S       (RTC_LSE_FREQ_VALUE)        //rtc频率下1秒
#define RTC_ADJUST_10S      (RTC_LSE_FREQ_VALUE * 10)   //rtc频率下10秒
#define RTC_ADJUST_100S     (RTC_LSE_FREQ_VALUE * 100)  //rtc频率下100秒

#define RTC_ADJUST_1S_LSI   (RTC_LSI_FREQ_VALUE)        //rtc频率下1秒
#define RTC_ADJUST_10S_LSI  (RTC_LSI_FREQ_VALUE * 10)   //rtc频率下10秒
#define RTC_ADJUST_100S_LSI (RTC_LSI_FREQ_VALUE * 100)  //rtc频率下100秒

/*lq RTC时钟源宏定义*/
#define RTC_CLK_SOURCE_LSI  0
#define RTC_CLK_SOURCE_LSE  1

/*lq MCU从待机定时唤醒是因GPS定时orMCU定时*/
#define RTC_ALARM_GPS  0
#define RTC_ALARM_MCU  1

/*lq RTC和Tim间频率变换精度*/
#define RTC_TIM_PHASE_PRECISE       (1000000.0)
#define RTC_TIM_PHASE_NUM_DEFAULT   (93750000)
typedef struct RtcPara
{
    U16 usRtcFreq;          //lq 休眠待机期间RTC Alarm 频率
    U8  ucRtcLSEorLSI;      //lq 使用内部or外部晶振
    U8  ucRtcAlarmMcuOrGps;	//lq Standby唤醒是因Arm定时orGPS定时
    // U16 ssRtcAlarmValue;    //lq Standby休眠时长
    U32 ssRtcAlarmValue;    //lq Standby休眠时长

}RtcPara;

typedef struct RtcTimPhaseStruct
{
    U8  RtcTimPhaseFlg;
    U32 RtcTimPhasePeriodNum;
    U32 RtcTimPhaseCntLast;
    U32 RtcTimPhaseCntNow;
    double RtcTimPhaseNumTemp;
    U32 RtcTimPhaseNum;
    double lfRtcTimPhaseNum;
}RtcTimPhaseStruct;

typedef struct RtcSelfCheckStruct
{
    U8  StartFlg;    
    U8  CompleteFlg;
    U8  RtcLseOkFlg;
}RtcSelfCheckStruct;

extern RtcTimPhaseStruct struRtcTimPhase1;
extern U32 GpsRtcCount;
extern U32 RtcCnt;
extern U32 RtcCnt1s;                                                                   //lq 本地Rtc在UTC1s间隔内的计数值
extern U32 RtcCnt10s;                                                                  //lq 本地Rtc在UTC10s间隔内的计数值
extern U32 RtcCnt100s;                                                                 //lq 本地Rtc在UTC100s间隔内的计数值

extern S16 RtcPhaseAdjust1;//rtc每1秒调整的phase计数
extern S16 RtcPhaseAdjust10;//rtc每10秒调整的phase计数
extern S16 RtcPhaseAdjust100;//rtc每100秒调整的phase计数

extern S16 RtcCntAdjust1;//rtc每1秒调整计数
extern S16 RtcCntAdjust10;//rtc每10秒调整计数
extern S16 RtcCntAdjust100;//rtc每100秒调整计数
extern RtcPara struRtcPara;

extern U16 RtcTimPhaseCnt;

extern U8 gucTestRtc;
//extern void RTC_Configuration(void);
extern void RTCAlarmInt(void);

extern void InitRtcPara(void);
extern void CalcRtcPhaseAdjust(S32 DiffNum ,U16 PeriodCount);
extern void ResetRtcCount(U16 usAlarmValue);

extern void RTCConfigurationPart1(void);
extern void RTCConfigurationPart2(void);

//lnw add
extern void InitRtcParaStruct(RtcPara *pRtcPara);
extern 	void InitRtcTimPhaseStruct(void);
extern void RTC_RtcSelfCheckStructInit(RtcSelfCheckStruct *pRtcSelfCheckStruct);
extern void RTC_SelfCheck(RtcSelfCheckStruct *pRtcSelfCheckStruct);


#endif

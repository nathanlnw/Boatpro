/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	gpsmsg.h
  Author: vozhj      Version: V1.00       Date: 2012.03.21 
  Description:  
  				主要功能： 
				1. 定义相关宏定义
				2. 变量声明
				3. 函数声明
				4. 定义结构体类型
				
  Others:         // 其它内容的说明
  Function List:  // 主要函数列表，每条记录应包括函数名及功能简要说明
  History:        // 修改历史记录列表，每条修改记录应包括修改日期、修改
                  // 者及修改内容简述  
    1. Date:
       Author:
       Modification:
*******************************************************************************/
#ifndef _GPSCTRL_H
#define _GPSCTRL_H
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/*lq 调试宏定义*/
#define GPS_CTRL_DEBUG_MSG

#define GPS_OUTRANGE		(0xFFFF)
#define GPS_RANGE			(10)
#define GPS_POSITION_BUFF_LENGTH (4)//位置平均缓冲器长度
#define GPS_SYN_COUNT_THRESHOLD (2)//(4)//同步最少保持时间4秒

/*lq GPS报文数据宏定义*/
#define GPS_DATA_PDOP_CEIL       (999)      //lq 实际值的10倍
#define GPS_DATA_PDOP_FLOOR      (5)        //lq 实际值的10倍
#define GPS_DATA_PDOP_DEFAULT    (999)      //lq 实际值的10倍


/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/

/**
	@ingroup GPS
	@brief GPS消息结构体
*/
typedef struct {
	U8	GPSTimeBuf[6];
	U16 GPS_TimeY;
	U16 GPS_TimeMon;
	U16 GPS_TimeD;
	U16 GPS_TimeH;
	U16 GPS_TimeM;
	U16 GPS_TimeS;
	S16 GPS_LatiD;
	S32 GPS_Latitude;
	S32 GPS_Lati100;
	S32 GPS_Longitud;
	S32 GPS_Long100;
	U32 GPS_Sog;                                                                //lq 记录值为实际值的10倍（浮点型转换为整型）
	U32 GPS_Sog_Buf[18];
	U16 GPS_Sog_Index;
	U32 GPS_Cog;
	U16 GPS_Stat;
	U16 GPS_errlat;
	U16 GPS_errlon;
	U16 GPS_PosPrec;
    U16 GPS_QualityIndicator;
    U16 GPS_FixMode;
    U16 GPS_PDOP;
    U8  GPS_RAIMFlag;
	S32 GPS_Longitud_Buf[GPS_POSITION_BUFF_LENGTH];
	S32 GPS_Latitude_Buf[GPS_POSITION_BUFF_LENGTH];
	U16 GPS_Longitud_Buf_Index;
	U16 GPS_Latitude_Buf_Index;
    U8  GPS_LonLatFilterFlg;                                                    //lq GPS经纬度是否过滤
    U8  GPS_LatLonCnt;                                                          //lq 起始的经纬度不过滤

	/*lq 用于根据经纬度计算航速*/
	S32 GPS_Latitude_Last;
	S32 GPS_Longitud_Last;
	U16 GPS_TimeD_Last;
	U16 GPS_TimeH_Last;
	U16 GPS_TimeM_Last;
    U16 GPS_TimeS_Last;	
	U32 GPS_Sog_Last; 
    U8  GPS_FirstLLFlg;                                                         //lq 开机第一次获取经纬度

    /*lq 用于消除闰秒的影响*/
	U8  GPS_LeapSecondFlg;
    U16 GPS_TimeHLeapLast;
	U16 GPS_TimeMLeapLast;
	U16 GPS_TimeSLeapLast;
    U16 GPS_TimeSLeap;                                                          //lq 闰秒出现之前人为进行闰秒校正后的秒

    /*lq GPS PPS生成标志 */
    U8  GPS_PPSGenFlag;
    U8  GPS_MsgGenFlag;
}GPSStruct;

#define GPS_CTRL_PROTECT_PERIOD (1)//GPS第一次定位时间设置为1分钟

#define GPS_CTRL_1MINPEROID		12000    //单位5ms
#define GPS_CTRL_3MINCNT		3
#define	GPS_CTRL_2MINCNT		2
#define GPS_CTRL_1MINCNT		1
#define GPS_CTRL_5MINCNT		5//第一次定位后5分钟保护
#define GPS_CTRL_15MINCNT		15
#define GPS_CTRL_30MINCNT       30
#define GPS_CTRL_40SECPEROID	8000
#define GPS_CTRL_10SECPEROID	2000

#define GPS_CTRL_SHUT_UP_KEEP_MIN   15
#define GPS_CTRL_FIRST_HOUR_KEEP_MIN    30
#define GPS_CTRL_UNFIX_PERIOD       60
#define GPS_CTRL_NORMAL_KEEP_MIN    5

#define GPS_CTRL_GPSPOWERUP		0
#define GPS_CTRL_GPSPOWERDOWN	1

/*lq 用户测试模式下，GPS状态机状态*/
#define GPS_CTRL_STA_MSG_TEST_START             0
#define GPS_CTRL_STA_MSG_TEST_WAIT_FIX          1
#define GPS_CTRL_STA_MSG_TEST_POWER_DOWN        2
#define GPS_CTRL_STA_MSG_TEST_IDLE              3

#if 0
/*lq SART的GPS状态机状态*/
#define GPS_CTRL_STA_START                          0
#define GPS_CTRL_STA_UNFIX_FIRST_LNG_OPEN           1
#define GPS_CTRL_STA_UNFIX_FIRST_HOUR_LNG_OPEN      2
#define GPS_CTRL_STA_UNFIX_BEY_FIRST_HOUR_LNG_OPEN  3
#define GPS_CTRL_STA_FIX_LNG_OPEN                   4
#define GPS_CTRL_STA_NORMAL_WORKING                 5
#define GPS_CTRL_STA_POWER_DOWN                     6
#define GPS_CTRL_STA_IDLE                           7
#endif

/*lq AtoN的GPS状态机状态 */
#define ATON_GPS_CTRL_STA_START                     0
#define ATON_GPS_CTRL_STA_FIX_LNG_OPEN              1
#define ATON_GPS_CTRL_STA_NORMAL_WORKING            2
#define ATON_GPS_CTRL_STA_OFF_POSITION_JUDGE        3
#define ATON_GPS_CTRL_STA_OFF_POSITION              4
#define ATON_GPS_CTRL_STA_IDLE                      5

/*lq M4508D的GPS状态机状态*/
#define GPS_CTRL_STA_START                          0
#define GPS_CTRL_STA_UNFIXED                        1
#define GPS_CTRL_STA_FIXED                          2
#define GPS_CTRL_STA_UNFIXED_ERR                    3

/*lq GPS时间参数宏定义*/
#define GPS_CTRL_TIME_UNFIX_1_HOUR                  5   //min
#define GPS_CTRL_TIME_UNFIX_FIRST_LONG_OPEN         5   //min
#define GPS_CTRL_TIME_UNFIX_1HOUR_LONG_OPEN         1   //min
#define GPS_CTRL_TIME_UNFIX_BEY_1HOUR_LONG_OPEN     2   //min
#define GPS_CTRL_TIME_FIX_LONG_OPEN                 5   //min
#define GPS_CTRL_TIME_PERIOD_1S		                200     //lq unit: 5ms
#define GPS_CTRL_TIME_TIMEOUT_UNFIX_ASSERT		    5       //lq unit: 1s
#define GPS_CTRL_TIME_TIMEOUT_UNFIX_ERR		        1800    //lq unit: 1s

#define GPS_CTRL_PERIOD_UNFIX_1HOUR_LONG_OPEN       1   //消息发送周期
#define GPS_CTRL_PERIOD_UNFIX_BEY_1HOUR_LONG_OPEN   2   //消息发送周期

/*lq GPS阈值宏定义*/
#define GPS_CTRL_CNT_THRES_5MIN_LON_OPEN            10  //次
#define GPS_CTRL_TIME_THRES_MSG_TEST                15  //min

/*lq GPS省电控制条件宏定义*/
#define GPS_SAVE_JUDGE_PERIOD           20      //unit 5ms
#define GPS_SAVE_COND_OK_TIME           100     //unit 100ms    //条件满足持续时间
#define GPS_SAVE_COND_WORKTIME          4320    //unit min     工作总时长
#define GPS_SAVE_COND_BATTERYVOLT       450     //unit 0.01V   电池电压
#define GPS_SAVE_COND_TEMPER            0       //unit c       温度

/*lq GPS自测宏定义 */
#define GPS_SLFCHK_TIME_PERIOD          200     //lq unit:5ms
//#define GPS_SLFCHK_TIME_TIMEOUT         10      //lq unit:1s
#define GPS_SLFCHK_TIME_TIMEOUT         4       //lq unit:1s
#define GPS_SLFCHK_TIME_TIMEOUT_TIMEPULSE 120	// unit:1s
#define GPS_SLFCHK_TIME_TIMEOUT_POWERON 2       //lq unit:1s

#define GPS_SLFCHK_CTRL_STATE_IDLE      0
#define GPS_SLFCHK_CTRL_STATE_POWERON   1
#define GPS_SLFCHK_CTRL_STATE_TIMEPULSE	2
#define GPS_SLFCHK_CTRL_STATE_POWEROFF  3

/**
	@ingroup GPS
	@brief GPS状态控制结构体
*/
typedef struct {
	U32 TimeCnt;
	U16 TimePeriod;	
	U16 Timeout;	
	U16 TimeoutCnt;	
	U16 Time1mCnt;		                //1分钟计时器

    S16 GpsUnfix1hPeriodTimSlot;        //lq 未定位1h定时
    S32 GpsUnfix1hPeriodTimFrame;            
    
    S16 GpsOnTimSlot;                   //lq GPS开启定时
    S32 GpsOnTimFrame;    
    S16 GpsOffTimSlot;                  //lq GPS关闭定时
    S32 GpsOffTimFrame;    
    U8  ucGpsOnTimEn;                   //lq GPS开启定时有效
    U8  ucGpsOffTimEn;                  //lq GPS关闭定时有效

	U8  GpsPowerSta;	                //GPS供电状态
	U8  GpsCtrlSta;		                //GPS控制状态机
	U8	GPSSyn_Flag;
	
	U8  GpsPowerupFix;
	U8  GpsNormalFix;
	U8	SaveOpenTime;                   //保存开机时间标志位，用于解决GPS热启动保护长开后的重复保存开机时间问题

	U16 GpsSynCount;                    //同步时间计数  至少保证4秒

	U16 GpsFixCount;                    //gps正常定位次数
	U16 GpsFallCount;                   //GPS定位失败计数
	U16	GpsWaitCount;		            //gps长开次数计数
	U8	GpsWaitFix;                     //GPS长开等定位标志
    U8  ucGpsUnfix1hLngOpenFlg;         //GPS未定位1h内长开标志（1min）；
    U8  ucGpsUnfixBey1hLngOpenFlg;      //GPS未定位超过1h长开标志（2min）；
    U8  ucGps5minLngOpenEn;             //GPS5min长开使能标志；
    S16 ssGps5minLngOpenCnt;            //GPS5min长开计数；
    U8  GpsIntervalCnt;                 //GPS间隔开计数    

    U8  ucGpsSaveFlg;                   //GPS省电标志，正常定位时每隔一个发送周期开启一次；
    U8  ucGpsOnEn;                      //GPS省电模式下，GPS开启
	U32 GpsSaveJudgeTimeCnt;
	U16 GpsSaveJudgeTimePeriod;	
	U16 GpsSaveJudgeCntWorkTime;	    //GPS省电的工作时长判断计时
	U16 GpsSaveJudgeCntBatVolt;	
	U16 GpsSaveJudgeCntTemper;	
	U8  GpsSaveJudgeFlgWorkTime;	    //GPS省电的工作时长判断计时
	U8  GpsSaveJudgeFlgBatVolt;	
	U8  GpsSaveJudgeCFlgTemper;
//lq	U8  GpsMsgEnable;//请求协议栈发送消息
//lq    U8  ucGpsUnfixLngOpenFlg;//GPS未定位长开标志；
//lq    S16 ssGpsUnfixCnt;//GPS未定位次数计数；
//lq    S16 ssGpsUnfixBey3Cnt;//GPS未定位次数超过3次的计数；
//lq    S16 ssGpsNormalFixCnt;//GPS正常定位次数计数；

    U8  GpsOffPositionSampleEn;                 //lq GPS位置偏移采样使能
    U8  GpsPositionSampleNewFlg;                //lq GPS新采样点标志
}GPSCtrlStruct;

typedef struct {
	U32 TimeCnt;
	U16 TimePeriod;	
	U16 Time1mCnt;		                //1分钟计时器

	U8  GpsPowerupFix;
	U8  GpsCtrlSta;		                //GPS控制状态机
}GPSCtrlMsgTestStruct;

/**
	@ingroup GPS
	@brief GPS时间同步结构体
*/
typedef struct {
	S16  GPSSyn_PulsePhase;	//接收到秒脉冲时的时隙相位
	S16  GPSSyn_PhaseOffset;	//时隙同步时相位偏移
//lq-	U16 GPSSyn_PulseSlot;		//接收到秒脉冲时的时隙值 
    S16  GPSSyn_PulseSlot;
	S16  GPSSyn_SlotOffset;	//时隙同步时隙偏移量
	U16 GPSSyn_SynLevel;		//同步优先级
	U16 GPSSyn_PhaseSynFlag;	//相位同步标志
	U16 GPSSyn_SlotSynFlag;	//相位同步标志
	U16 GPSSyn_ProtectCnt;	//相位同步保护时间
	U16 GPSSyn_PulseEn;		//秒脉冲有效标志
	U16 GPSSyn_Flag;			//GPS同步有效标志
	U16 GPSSyn_SlotCnt;
	U16 GPSSyn_SlotEn;
	S32 GPSSyn_PulseMin;
	S16 GPSSyn_MinOffset;
	S32 GPSSyn_PulseHour;    
    S16 GPSSyn_HourOffset;	//lq 小时偏移

	U16 ChkTimeS;
	U16 ChkTimeM;
	U16 ChkTimeH;
    U32 ChkTimeFrame;       //lq 一天中的第几帧
    U8  ucGPSSyn_SlotPhaseSynFlag;                                              //lq [20130129]
    //lq U8  GPSSyn_FirstSynFlag;
	U8  GPSSyn_Enable;      //lq GPS同步使能
    
	U8  GPSSyn_HourFrameSynEnable;
    U8  ucGPSSyn_HourFrameSynFlag;                                              //lq 小时、分钟同步

}GPSSynStruct;

typedef struct GpsSelfCheckStruct
{
	U32 TimeCnt;
	U16 TimePeriod;	
    U16 TimeoutCnt;
    
    U8  CtrlState;
    U8  StartFlg;
    U8  CompleteFlg;
    U8  TimepulseFlg;
    U8  DataCommFlg;
    U8  OnDataCommFlg;
    U8  OffDataCommFlg;
    U8  AntennaFlg;
    U8  ModuleFlg;

    U8  VccCtrlOkFlg;
    U8  TimepulseOkFlg;
    U8  DataCommOkFlg;
    U8  AntennaOkFlg;
    U8  ModuleOkFlg;
}GpsSelfCheckStruct;

extern GPSSynStruct GpsSynCell;	   		//GPS同步操作结构体
extern GPSStruct GpsPara;
extern GPSCtrlStruct GpsCtrlPara;
extern GPSCtrlMsgTestStruct GPSCtrlMsgTestPara;
extern GpsSelfCheckStruct GpsSelfCheckInst;

/* Public  function  ---------------------------------------------------------*/
extern void IniGpsPara(void);

/**
	@ingroup GPS
	@brief GPS报文解析主函数
	@param pDat 报文指针
	@param uiLen 报文长度
	@retval TRUE 解析成功
	@retval FALSE 解析失败

	完成GPS的报文解析，主要解析GPRMC报文
*/
extern U8 GpsMsgProcess(U8 *pDat, U16 uiLen);

/**
	@ingroup GPS
	@brief GPS控制主函数
	@param 无
	@retval 无

	用于根据GPS是否定位控制GPS的开关等
*/
extern void GpsCtrlProcess(void);
extern void GpsPowerUp(void);

/**
	@ingroup 休眠
	@brief 查看GPS模块是否定位/同步完成
	@param 无
	@retval TRUE GPS定位/同步完成，且GPS断电
	@retval FALSE GPS忙
*/
extern U8 GetGpsSleepSta(void);
extern U8 GetGpsSynSta(void);

/**
	@ingroup GPS
	@brief 协议栈和GPS模块接口函数
	@param pGpsCell 协议栈用于存GPS数据的结构体
	@retval TRUE GPS定位
	@retval FALSE GPS未定位
*/
extern U8 GetGpsFixSta(GPSStruct *pGpsCell);

/**
	@ingroup GPS
	@brief 时间同步函数
	@param 无
	@retval 无

	根据GPS报文解析时间和GPS秒脉冲时保存下的本地时间来计算时间误差，从而完成时间同步
*/
extern void SlotSynClac(void);

//lq extern U8 GetGpsMsgRequest(void);
extern void SetGpsMsgRequest(void);
extern U8 GetGpsPowerupFixState(void);
extern U8 GetGpsNormalFixState(void);
extern U8 GetGpsNormalFixStateRst(void);
extern void SetGpsNormalFixState(void);
extern void ResetGpsNormalFixState(void);
extern U8 GetGpsIdelState(void);
extern U8 IsGpsFix(void);
extern void InitGpsSynStru(GPSSynStruct *pGpsSynStru);
extern void SetGpsPowerupFixState(void);
extern void SetGpsUnfixPeriodTim(S32 value);
extern void ResetGpsUnfixPeriodTim(void);
extern S32 GetGpsUnfixPeriodTim(void);
extern U8 GetGpsWakeupSta(void);
extern void GpsPowerDown(void);
extern void ResetGpsSynSta(void);
extern U8 IsGpsPowerupProtect(void);
extern U8 GetGpsPowerSta(void);
extern void SetGpsOffTimSlot(S16 value);
extern void SetGpsOffTimFrame(S32 value);
extern S32 GetGpsOffTim(void);
extern void ResetGps5minLngOpenEn(void);
extern U8 GetGps5minLngOpenEn(void);
extern void SetGpsUnfix1hPeriodTimFrame(S32 value);
extern void SetGpsUnfix1hPeriodTimSlot(S16 value);
extern S32 GetGpsUnfix1hPeriodTim(void);
extern void SetGpsUnfixLonOpenFlg(void);
extern void ResetGpsUnfixLonOpenFlg(void);
extern U8 GetGpsUnfixLonOpenFlg(void);
extern void SetGpsUnfix1hLngOpenFlg(void);
extern void ResetGpsUnfix1hLngOpenFlg(void);
extern U8 GetGpsUnfix1hLngOpenFlg(void);
extern void SetGpsUnfixBey1hLngOpenFlg(void);
extern void ResetGpsUnfixBey1hLngOpenFlg(void);
extern U8 GetGpsUnfixBey1hLngOpenFlg(void);
extern U8 IsGpsOffTimeout(void);
extern void SetGpsOnTimEn(void);
extern void ResetGpsOnTimEn(void);
extern U8 GetGpsOnTimEn(void);
extern U8 IsGpsOnTimeout(void);
extern U8 IsGpsUnfix1hPeriodTimeout(void);
extern void SetGpsOnTim(S16 usValue);
extern U8 GetMsgTestSendFlg(void);
extern void GpsCtrlProcess_MsgTest(void);
extern U8 GetGpsOnEn(void);
extern U8 GetGpsSaveFlg(void);
extern void GpsSaveCondJudge(void);
extern void SetGpsOnEn(void);
extern void ResetGpsOnEn(void);
extern U8 GetGpsOffPositionSampleEn(void);
extern U8 GetGpsPositionSampleNewFlgRst(void);
extern void SetGpsPositionSampleNewFlg(void);
extern void GpsWakeUp(void);
extern void ResetGpsOffPositionSampleEn(void);


extern void IniGpsMsgStru(GPSStruct *pGpsCell);//lnw add

#endif

/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: DeviceInfo.h
  Author: lq       Version : V1.0.0        Date: 2015.05.07
  Description:     设备信息定义
  Version:         V1.0.0
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _DEVICEINFO_H_
#define _DEVICEINFO_H_

/* Include -------------------------------------------------------------------*/
#include "Include.h"

/* Public define -------------------------------------------------------------*/
/*lq 上下限值 */
#define DEV_INFO_SERIAL_MIN			    0			                            //整机序列号存储最大长度
#define DEV_INFO_SERIAL_MAX			    99999999                                //整机序列号存储最大长度
#define DEV_INFO_GPS_OUTPUT_INTERVAL_MIN        0                               //lq unit: 1s
#define DEV_INFO_GPS_OUTPUT_INTERVAL_MAX        255

/*lq 默认值 */
#define DEV_INFO_FREQADJ_DEFAULT        32
#define DEV_INFO_MODAMP_DEFAULT			32
#define DEV_INFO_RF_VOLT_ADJUST_DEFAULT 10           
#define DEV_INFO_WORKTIME_DEFAULT       0
#define DEV_INFO_AIS_CLASS_DEFAULT      (AIS_CLASS_DEFAULT)                     //lq 默认为Class B
#define DEV_INFO_GPS_OUTPUT_INTERVAL_DEFAULT    1                               //lq unit: 1s

/*lq GPS输出缓存定义 */
#define DEV_INFO_GPS_OUTPUT_INTERVAL_BUFF_LEN   7
#define DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_RMC  0
#define DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_VTG  1
#define DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GGA  2
#define DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSA  3
#define DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSV  4
#define DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GLL  5
#define DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GBS  6

/* Public typedef ------------------------------------------------------------*/
/*lq 设备静态信息（永久保存） */
typedef struct
{
	U16 uiFreqAdj;						//lq 射频发射频率准确度
	U16 uiFreqOffset;					//lq 频率偏移
	U16 uiModAmpAdj;					//lq 调制幅度参数，16 ~ 48
	U16 uiModAmp;						//lq 调制幅度
	U32 uiXmtFreq;
    S32 slXmtFreqComp;                  //lq 发射频率补偿值	   slXmtFreqComp= siXmtFreqAdj*1850
	U16 GridVoltAdj;                    //lq 栅压DA控制调整
    S16 GridVoltOffset;
    U8  AisClass;

    /*lq P712*/
	U32 SerialNumber;
}DevStaticInfoStruct;

/*lq 设备动态信息（工作时有效） */
typedef struct
{
	U8  ucLowFreqSta;					//lq 休眠标志
	U8  ucWorkMode;						//lq 测试态或正常工作状态
    U8  TestSta;
	U8  ucGpsOutput;					//lq GPS语句输出状态
    U8  GpsOutputInterval[DEV_INFO_GPS_OUTPUT_INTERVAL_BUFF_LEN];
    S16 GpsOutputCnt[DEV_INFO_GPS_OUTPUT_INTERVAL_BUFF_LEN];
    U8  OpenCloseTimeIndex;				//lq 开关机时间索引

}DevDynamicInfoStruct;

/* Public variables ----------------------------------------------------------*/
extern DevStaticInfoStruct DevStaticPara;
extern DevDynamicInfoStruct DevDynamicPara;

/* Public functions ----------------------------------------------------------*/

#endif
/*==============================================================================
------------------------------------End of File---------------------------------
==============================================================================*/

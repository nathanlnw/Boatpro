/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: AisInfo.h
  Author: lq       Version : V1.0.0        Date: 2015.04.29
  Description:     AIS信息定义
  Version:         V1.0.0
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _AISINFO_H_
#define _AISINFO_H_

/* Include -------------------------------------------------------------------*/
#include "def.h"
#include "AISDigitalInterface.h"

/* Public define -------------------------------------------------------------*/
/*lq AIS INFO 字符串类型信息的长度 */
#define AIS_INFO_VEND_ID_LEN            7
#define AIS_INFO_CALL_SIGN_LEN          7
#define AIS_INFO_SHIP_NAME_LEN          20
#define AIS_INFO_DESTINATION_LEN        20

/*lq AIS INFO 数值类型信息的取值范围 */
#define AIS_INFO_MMSI_MIN			    0
#define AIS_INFO_MMSI_MAX			    999999999
#define AIS_INFO_SHIPA_MIN			    0
#define AIS_INFO_SHIPA_MAX			    511
#define AIS_INFO_SHIPB_MIN			    0
#define AIS_INFO_SHIPB_MAX			    511
#define AIS_INFO_SHIPC_MIN			    0
#define AIS_INFO_SHIPC_MAX			    63
#define AIS_INFO_SHIPD_MIN			    0
#define AIS_INFO_SHIPD_MAX			    63
#define AIS_INFO_DTE_MIN			    0
#define AIS_INFO_DTE_MAX			    1
#define AIS_INFO_SHIP_TYPE_MIN	        0
#define AIS_INFO_SHIP_TYPE_MAX		    255
#define AIS_INFO_MAX_DRAUGHT_MIN	    0
#define AIS_INFO_MAX_DRAUGHT_MAX		255
#define AIS_INFO_PERSONS_MIN	        0
#define AIS_INFO_PERSONS_MAX		    8191
#define AIS_INFO_ETA_MONTH_MIN	        0
#define AIS_INFO_ETA_MONTH_MAX		    12
#define AIS_INFO_ETA_DAY_MIN	        0
#define AIS_INFO_ETA_DAY_MAX		    31
#define AIS_INFO_ETA_HOUR_MIN	        0
#define AIS_INFO_ETA_HOUR_MAX		    24
#define AIS_INFO_ETA_MINUTE_MIN	        0
#define AIS_INFO_ETA_MINUTE_MAX		    60
#define AIS_INFO_NAVIGATION_STATUS_MIN	0
#define AIS_INFO_NAVIGATION_STATUS_MAX	15
#define AIS_INFO_REGION_APP_FLAG_MIN	0
#define AIS_INFO_REGION_APP_FLAG_MAX	15
#define AIS_INFO_EPFS_TYPE_MIN	        0
#define AIS_INFO_EPFS_TYPE_MAX		    15
#define AIS_INFO_AI_DAC_MAX		        0
#define AIS_INFO_AI_DAC_MIN		        1023
#define AIS_INFO_AI_FI_MAX		        0
#define AIS_INFO_AI_FI_MIN		        63

/*lq AIS INFO 默认值 */
#define AIS_INFO_MMSI_DEFAULT		    999999999
#define AIS_INFO_SHIPA_DEFAULT			0
#define AIS_INFO_SHIPB_DEFAULT			0
#define AIS_INFO_SHIPC_DEFAULT			0
#define AIS_INFO_SHIPD_DEFAULT			0
#define AIS_INFO_DTE_DEFAULT            0
#define AIS_INFO_SHIP_TYPE_DEFAULT      0
#define AIS_INFO_MAX_DRAUGHT_DEFAULT	0
#define AIS_INFO_PERSONS_DEFAULT	    0
#define AIS_INFO_ETA_MONTH_DEFAULT	    0
#define AIS_INFO_ETA_DAY_DEFAULT	    0
#define AIS_INFO_ETA_HOUR_DEFAULT		24
#define AIS_INFO_ETA_MINUTE_DEFAULT		60
#define AIS_INFO_NAVIGATION_STATUS_DEFAULT          15
#define AIS_INFO_REGION_APP_FLAG_DEFAULT            0
#define AIS_INFO_EPFS_TYPE_DEFAULT      1   //lq GPS

#define AIS_INFO_VEND_ID_DEFAULT        "0000000"
#define AIS_INFO_CALL_SIGN_DEFAULT      "@@@@@@@"
#define AIS_INFO_SHIP_NAME_DEFAULT      "@@@@@@@@@@@@@@@@@@@@"
#define AIS_INFO_DESTINATION_DEFAULT    "@@@@@@@@@@@@@@@@@@@@"
#define AIS_INFO_SOG_DEFAULT            1023
#define AIS_INFO_POSITION_ACCURACY_DEFAULT          0
#define AIS_INFO_LONGITUDE_DEFAULT      0x6791AC0
#define AIS_INFO_LATITUDE_DEFAULT       0x3412140
#define AIS_INFO_COG_DEFAULT            3600
#define AIS_INFO_TRUE_HEADING_DEFAULT   511
#define AIS_INFO_TIME_STAMP_DEFAULT     60
#define AIS_INFO_CLASS_B_UNIT_FLAG_DEFAULT          0   //lq SOTDMA unit
#define AIS_INFO_CLASS_B_DISPLAY_FLAG_DEFAULT       0   //lq No display availible
#define AIS_INFO_CLASS_B_DSC_FLAG_DEFAULT           0   //lq No DSC function
#define AIS_INFO_CLASS_B_BAND_FLAG_DEFAULT          0   //lq Capable of operating over the upper 525 kHz band of the marine band
#define AIS_INFO_CLASS_B_MSG22_FLAG_DEFAULT         0   //lq 0 = No frequency management via Message 22, operating on AIS 1, AIS 2 only
#define AIS_INFO_CLASS_B_MODE_FLAG_DEFAULT          0   //lq 0 = Station operating in autonomous and continuous mode
#define AIS_INFO_CLASS_B_RAIM_FLAG_DEFAULT          0   //lq 0 = RAIM not in use = default
#define AIS_INFO_CLASS_B_COMM_STATE_FLAG_DEFAULT    0   //lq 0 = SOTDMA
#define AIS_INFO_ROT_AIS_DEFAULT                    -128
#define AIS_INFO_SPEC_MAN_INDICATOR_DEFAULT         0
#define AIS_INFO_AIS_VERSION_INDICATOR_DEFAULT      2   //lq 2 = station compliant with Recommendation ITU-R M.1371-5 (or later)
#define AIS_INFO_IMO_NUMBER_DEFAULT                 0   //lq 0 = not available = default – Not applicable to SAR aircraft
#define AIS_INFO_AI_DAC_DEFAULT                     413 //lq 一体机的DAC=413
#define AIS_INFO_AI_FI_DEFAULT                      1   //lq 一体机的FI=1

/*lq 接收台站 */
#define AIS_INFO_REC_STATION_BUFF_LEN   12

/* Public typedef ------------------------------------------------------------*/
/*lq AIS静态信息 */
typedef struct AisStaticInfoStruct
{
	U32 MMSI;               // (xxxxxxxxx)

    /*lq SSD */
    U8  CallSign[AIS_INFO_CALL_SIGN_LEN + 1];   //lq c-c，含‘\0’（变长数据）
    U8  ShipName[AIS_INFO_SHIP_NAME_LEN + 1];   //lq c-c，含‘\0’（变长数据）
    U16	ShipA;			    // (xxx)
    U16	ShipB;			    // (xxx)
    U8	ShipC;			    // (xx)
    U8	ShipD;			    // (xx)
    U8  DTE;                // (c)     
    U8  TalkerID[AIS_DI_TALKER_ID_LEN + 1];     // (aa)，含‘\0’

    /*lq VSD */
    U8  TypeOfShip;         // (x.x)
    U8  MaxDraught;         // (x.x)
    U8  Persons;            // (x.x)
    U8  Destination[AIS_INFO_DESTINATION_LEN + 1];      //lq c-c，含‘\0’（变长数据）
    U8  ETAMonth;           // (xx)
    U8  ETADay;             // (xx)
    U8  ETAHour;            // (xx)
    U8  ETAMinute;          // (xx)
    U8  NavigationStatus;   // (x.x)
    U8  RegionAppFlag;      // (x.x)
    
    /*lq */
    U8	TypeOfEPFS;		    // (x)

    /*lq P712 */
	U8  VendID[AIS_INFO_VEND_ID_LEN + 1];       //lq 含‘\0’，仅每个非‘\0’字节元素的低6位为有效信息，参见结构体VendorIDStruct
    U8  AISVersionIndicator;
    U32 IMONumber;
    U16 AI_DAC;             //lq 10 bits
    U8  AI_FI;              //lq 6 bits

    U8  Refresh;
}AisStaticInfoStruct;

/*lq AIS动态信息 */
typedef struct AisDynamicInfoStruct
{
    U16 SOG;
    U8  PositionAccuracy;
    U32 Longitude;
    U32 Latitude;
    U16 COG;
    U16 TrueHeading;
    U8  TimeStamp;
    U8  ClassBUnitFlag;
    U8  ClassBDisplayFlag;
    U8  ClassBDscFlag;
    U8  ClassBBandFlag;
    U8  ClassBMsg22Flag;
    U8  ModeFlag;
    U8  RaimFlag;
    U8  CommStateSelectorFlag;
    S8  RotAIS;
    U8  SpecialManoeuvreIndicator;
    U16 ReceivedStations;
}AisDynamicInfoStruct;

/*lq 
 *  当前接收到的台站数计算方法：
 *  使用长度为12的数组记录最近6min内接收到的5号（ClassA）和24A号（ClassA）消息数，
 *  每30s将最久的30s中收到的台站数置为0；
 */
typedef struct
{
    U16 RecStations[AIS_INFO_REC_STATION_BUFF_LEN];
    S16 BuffIndex;
    U32 RecStationTotal;
}RecStationStruct;

/* Public variables ----------------------------------------------------------*/
extern AisStaticInfoStruct AisStaticPara;
extern AisDynamicInfoStruct AisDynamicPara;

/* Public functions ----------------------------------------------------------*/

#endif
/*==============================================================================
------------------------------------End of File---------------------------------
==============================================================================*/

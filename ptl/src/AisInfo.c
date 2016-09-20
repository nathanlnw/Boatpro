/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: TDMA.c
  Author: lq       Version : V1.0.0        Date: 2015.01.16
  Description:     TDMA 管理模块
  Version:         V1.0.0
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "Include.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/
AisStaticInfoStruct AisStaticPara;
AisDynamicInfoStruct AisDynamicPara;
static RecStationStruct RecStationPara;

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* 名称  : AISINFO_AisStaticInfoStructInit
* 描述  : AisStaticInfoStruct结构体初始化
* 输入  : pAisStaticInfoStruct - 指向AisStaticInfoStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISINFO_AisStaticInfoStructInit(AisStaticInfoStruct *pAisStaticInfoStruct)
{
 //    U16 i;

	pAisStaticInfoStruct->MMSI = AIS_INFO_MMSI_DEFAULT;

    /*lq SSD */
    strcpy((char*)pAisStaticInfoStruct->CallSign, AIS_INFO_CALL_SIGN_DEFAULT);
    strcpy((char*)pAisStaticInfoStruct->ShipName, AIS_INFO_SHIP_NAME_DEFAULT);
    pAisStaticInfoStruct->ShipA = AIS_INFO_SHIPA_DEFAULT;
    pAisStaticInfoStruct->ShipB = AIS_INFO_SHIPB_DEFAULT;
    pAisStaticInfoStruct->ShipC = AIS_INFO_SHIPC_DEFAULT;
    pAisStaticInfoStruct->ShipD = AIS_INFO_SHIPD_DEFAULT;   
    pAisStaticInfoStruct->DTE = AIS_INFO_DTE_DEFAULT;    
    strcpy((char*)pAisStaticInfoStruct->TalkerID, AIS_DI_TALKER_ID_DEFAULT);

    /*lq VSD */
    pAisStaticInfoStruct->TypeOfShip = AIS_INFO_SHIP_TYPE_DEFAULT;
    pAisStaticInfoStruct->MaxDraught = AIS_INFO_MAX_DRAUGHT_DEFAULT;
    pAisStaticInfoStruct->Persons = AIS_INFO_PERSONS_DEFAULT;
    strcpy((char*)pAisStaticInfoStruct->Destination, AIS_INFO_DESTINATION_DEFAULT);
    pAisStaticInfoStruct->ETAMonth = AIS_INFO_ETA_MONTH_DEFAULT;
    pAisStaticInfoStruct->ETADay = AIS_INFO_ETA_DAY_DEFAULT;
    pAisStaticInfoStruct->ETAHour = AIS_INFO_ETA_HOUR_DEFAULT;
    pAisStaticInfoStruct->ETAMinute = AIS_INFO_ETA_MINUTE_DEFAULT;
    pAisStaticInfoStruct->NavigationStatus = AIS_INFO_NAVIGATION_STATUS_DEFAULT;
    pAisStaticInfoStruct->RegionAppFlag = AIS_INFO_REGION_APP_FLAG_DEFAULT;

    /*lq  */
    pAisStaticInfoStruct->TypeOfEPFS = AIS_INFO_EPFS_TYPE_DEFAULT;

    /*lq P712*/
    strcpy((char*)pAisStaticInfoStruct->VendID, AIS_INFO_VEND_ID_DEFAULT);
    pAisStaticInfoStruct->AISVersionIndicator = AIS_INFO_AIS_VERSION_INDICATOR_DEFAULT;
    pAisStaticInfoStruct->IMONumber = AIS_INFO_IMO_NUMBER_DEFAULT;
    pAisStaticInfoStruct->AI_DAC = AIS_INFO_AI_DAC_DEFAULT;
    pAisStaticInfoStruct->AI_FI = AIS_INFO_AI_FI_DEFAULT;

	pAisStaticInfoStruct->Refresh = FALSE;
}

/*******************************************************************************
* 名称  : AISINFO_AisDynamicInfoStructInit
* 描述  : AisDynamicInfoStruct结构体初始化
* 输入  : pAisDynamicInfoStruct - 指向AisDynamicInfoStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISINFO_AisDynamicInfoStructInit(AisDynamicInfoStruct *pAisDynamicInfoStruct)
{
    pAisDynamicInfoStruct->SOG = AIS_INFO_SOG_DEFAULT;
    pAisDynamicInfoStruct->PositionAccuracy = AIS_INFO_POSITION_ACCURACY_DEFAULT;
    pAisDynamicInfoStruct->Longitude = AIS_INFO_LONGITUDE_DEFAULT;
    pAisDynamicInfoStruct->Latitude = AIS_INFO_LATITUDE_DEFAULT;
    pAisDynamicInfoStruct->COG = AIS_INFO_COG_DEFAULT;
    pAisDynamicInfoStruct->TrueHeading = AIS_INFO_TRUE_HEADING_DEFAULT;
    pAisDynamicInfoStruct->TimeStamp = AIS_INFO_TIME_STAMP_DEFAULT;
    pAisDynamicInfoStruct->ClassBUnitFlag = AIS_INFO_CLASS_B_UNIT_FLAG_DEFAULT;
    pAisDynamicInfoStruct->ClassBDisplayFlag = AIS_INFO_CLASS_B_DISPLAY_FLAG_DEFAULT;
    pAisDynamicInfoStruct->ClassBDscFlag = AIS_INFO_CLASS_B_DSC_FLAG_DEFAULT;
    pAisDynamicInfoStruct->ClassBBandFlag = AIS_INFO_CLASS_B_BAND_FLAG_DEFAULT;
    pAisDynamicInfoStruct->ClassBMsg22Flag = AIS_INFO_CLASS_B_MSG22_FLAG_DEFAULT;
    pAisDynamicInfoStruct->ModeFlag = AIS_INFO_CLASS_B_MODE_FLAG_DEFAULT;
    pAisDynamicInfoStruct->RaimFlag = AIS_INFO_CLASS_B_RAIM_FLAG_DEFAULT;
    pAisDynamicInfoStruct->CommStateSelectorFlag = AIS_INFO_CLASS_B_COMM_STATE_FLAG_DEFAULT;
    pAisDynamicInfoStruct->RotAIS = AIS_INFO_ROT_AIS_DEFAULT;
    pAisDynamicInfoStruct->SpecialManoeuvreIndicator = AIS_INFO_SPEC_MAN_INDICATOR_DEFAULT;
    pAisDynamicInfoStruct->ReceivedStations = 0;
}

/*******************************************************************************
* 名称  : AISINFO_RecStationStructInit
* 描述  : RecStationStruct结构体初始化
* 输入  : pRecStationStruct - 指向RecStationStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISINFO_RecStationStructInit(RecStationStruct *pRecStationStruct)
{
    S16 i;

    for (i = 0; i < AIS_INFO_REC_STATION_BUFF_LEN; i++)
    {
        pRecStationStruct->RecStations[i] = 0;
    }

    pRecStationStruct->BuffIndex = 0;
    pRecStationStruct->RecStationTotal = 0;
}

/*******************************************************************************
* 名称  : AISINFO_RecStationQuantityIncrease
* 描述  : 接收台站数计数
* 输入  : pRecStationStruct - 指向RecStationStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISINFO_RecStationQuantityIncrease(RecStationStruct *pRecStationStruct)
{
    pRecStationStruct->RecStations[pRecStationStruct->BuffIndex]++;
    pRecStationStruct->RecStationTotal++;
}

/*******************************************************************************
* 名称  : AISINFO_InstRecStationQuantityIncrease
* 描述  : 接收台站数实例计数
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISINFO_InstRecStationQuantityIncrease(void)
{
    AISINFO_RecStationQuantityIncrease(&RecStationPara);
}

/*******************************************************************************
* 名称  : AISINFO_RecStationQuantityUpdate
* 描述  : 接收台站数定时更新
* 输入  : pRecStationStruct - 指向RecStationStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISINFO_RecStationQuantityUpdate(RecStationStruct *pRecStationStruct)
{
    pRecStationStruct->BuffIndex = (pRecStationStruct->BuffIndex + 1)
        % AIS_INFO_REC_STATION_BUFF_LEN;

    pRecStationStruct->RecStationTotal -= 
        pRecStationStruct->RecStations[pRecStationStruct->BuffIndex];
    pRecStationStruct->RecStations[pRecStationStruct->BuffIndex] = 0;
}

/*******************************************************************************
* 名称  : AISINFO_InstRecStationQuantityUpdate
* 描述  : 接收台站数实例定时更新
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISINFO_InstRecStationQuantityUpdate(void)
{
    AISINFO_RecStationQuantityUpdate(&RecStationPara);
}

/*******************************************************************************
* 名称  : AISINFO_RecStationGetQuantity
* 描述  : 接收台站数定时更新
* 输入  : pRecStationStruct - 指向RecStationStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static U32 AISINFO_RecStationGetQuantity(RecStationStruct *pRecStationStruct)
{
    return pRecStationStruct->RecStationTotal;
}

/*******************************************************************************
* 名称  : AISINFO_InstRecStationGetQuantity
* 描述  : 接收台站数实例定时更新
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U32 AISINFO_InstRecStationGetQuantity(void)
{
    return AISINFO_RecStationGetQuantity(&RecStationPara);
}

/*******************************************************************************
* 名称  : AISINFO_ModuleInit
* 描述  : 模块初始化
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISINFO_ModuleInit(void) 
{
    AISINFO_AisStaticInfoStructInit(&AisStaticPara);
    AISINFO_AisDynamicInfoStructInit(&AisDynamicPara);
    AISINFO_RecStationStructInit(&RecStationPara);
}

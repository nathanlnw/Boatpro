/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: initial.c
  Author: vozhj      Version : V1.00        Date: 2012.03.21
  Description:    初始化参数，将全部模块的参数在此处进行初始化
  Version:         V1.00
  Function List:   // 主要函数及其功能
  History:         // 历史修改记录
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#include "def.h"
#include "initial.h"
#include "parastorage.h"
#include "gmskmod.h"
#include "LinkPtlPort.h"
#include "LinkPtlReport.h"
#include "parastorage.h"
#include "powerup.h"
#include "powerdown.h"
#include "uartmsg.h"
#include "gpsctrl.h"
#include "mmi.h"
#include "sleepctrl.h"
#include "testing.h"
#include "keyscan.h"
#include "digunit.h"
#include "ledctrl.h"
#include "batteryctrl.h"
#include "ledctrl.h"
#include "adjust.h"
#include "rtc.h"
#include "DAC.h"
#include "sotdma.h"
#include "offposition.h"
#include "config.h"
#include "Include.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private  function  --------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
#if 0
EqipStruc			eqEquipPara;		//整机参数
StaticInforStruc	siStaticPara;		//静态参数	
DriverStru			dsDriverPara;
#endif
/* Public  function  ---------------------------------------------------------*/
#if 0
/*******************************************************************************
* 名称  : InitEquipCell
* 描述  : 初始化整机的参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitEquipCell(void)
{
	if (ucStandbyWakeupFlg == TRUE)
    {
        eqEquipPara.ucLowFreqSta = EQ_FREQSTA_LOW;	//进入休眠      
    }
    else
    {
        eqEquipPara.ucLowFreqSta = EQ_FREQSTA_HIG;	//不进入休眠
    }
	eqEquipPara.UserTestCntThres = 98;
	eqEquipPara.ucWorkMode = EQ_WORKMOD_NORMAL;
	eqEquipPara.ucGpsOutput = FALSE;

	eqEquipPara.uiFreqAdj = PARA_FREQADJ_DEFAULT;		//该参数需要后期调试！！！
	eqEquipPara.uiFreqOffset = (U16)(((U32)eqEquipPara.uiFreqAdj * EQ_MOD_OFFSET ) >> 5);
	eqEquipPara.uiModAmpAdj = PARA_MODAMP_DEFAULT;
	eqEquipPara.uiModAmp = (U16)(((U32)eqEquipPara.uiModAmpAdj * EQ_MOD_AMP + 16) >> 5);

	eqEquipPara.uiXmtFreq = EQ_XMTFREQ_CHA;
    eqEquipPara.slXmtFreqComp = 0;                  //lq

    eqEquipPara.GridVoltAdj = PARA_RF_VOLT_ADJUST_DEFAULT;
    eqEquipPara.GridVoltOffset = (eqEquipPara.GridVoltAdj - 10) * DA_VALUE_01V;
#if 0
	eqEquipPara.SendInterval = 6;                   //发送间隔默认6分钟
	eqEquipPara.SendErrorPeriod = eqEquipPara.SendInterval >> 1;    //发送超时时间发送一半

	if (eqEquipPara.SendErrorPeriod < 1)
	{
		eqEquipPara.SendErrorPeriod = 1;
	}	
#endif	
	eqEquipPara.PowerOpenTime = 3;                  //默认开机时间3秒
	eqEquipPara.SosOpenTime = 3;                    //默认SOS开机时间3秒
	eqEquipPara.GpsHotTime = 4;                     //默认热启动超时时间40秒
	eqEquipPara.NormPoweronCntThres = 1;            //告警SOS按键时间
	eqEquipPara.BatteryLowLevel = PARA_BATTERY_LOW_DEFAULT;             //默认低门限6.8v
	eqEquipPara.BatteryUltraLowLevel = PARA_BATTERY_ULTRALOW_DEFAULT;   //超低门限6.2v
	eqEquipPara.CloseTime = 3;                      //默认关机按键时间3秒
	eqEquipPara.GpsPoweronProtectTime = 5;          //默认开机第一次定位时间5分钟
	eqEquipPara.GpsFixFallThres = 3;                //默认热启动失败三次引发保护

    eqEquipPara.WorkTimeSum = PARA_WORKTIME_DEFAULT;
	eqEquipPara.SendInterval = ITDMA_MSG_SEND_INTERVAL_DEF_VAL;         //消息发送间隔，单位分钟
	eqEquipPara.NormalSendTime = ITDMA_MSG_SEND_TIME_DEF_VAL;           //正常消息发送次数

    eqEquipPara.TestSta = EQ_MOD_TEST_STA_MAIN;
}
#endif

#if 0
/*******************************************************************************
* 名称  : InitStaticInforCell
* 描述  : 初始化静态信息参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitStaticInforCell(void)
{
    U16 i;

    siStaticPara.MMSI = PARA_MMSI_DEFAULT;
    siStaticPara.DestMMSI = PARA_MMSI_DEST_DEFAULT;
    sprintf(siStaticPara.VendID, "%s", PARA_VENDID_DEFAULT);    

    siStaticPara.SerialNumber = PARA_SERIAL_DEFAULT;
    siStaticPara.Refresh = FALSE;
    siStaticPara.AtoNStatus = PARA_ATON_STATUS_DEFAULT;
    siStaticPara.OffPositionThresh = PARA_OFF_POSITION_THRES_DEFAULT;
    siStaticPara.AckProcedure = 0;                                      //lq... Type1 AtoN不需要
    siStaticPara.OffPositionBehav = PARA_OFF_POSITION_BEHAV_DEFAULT;
    siStaticPara.SynchLostBehav = PARA_SYNCH_LOST_BEHAV_DEFAULT;
    sprintf(siStaticPara.NameofAtoN, "%s", PARA_ATON_NAME_DEFAULT);    
    
    siStaticPara.ShipA = PARA_SHIPA_DEFAULT;
    siStaticPara.ShipB = PARA_SHIPB_DEFAULT;
    siStaticPara.ShipC = PARA_SHIPC_DEFAULT;
    siStaticPara.ShipD = PARA_SHIPD_DEFAULT;
    siStaticPara.TypeofEPFS = PARA_EPFS_TYPE_DEFAULT;
    siStaticPara.NomLongitude = PARA_NOM_LONGITUDE_DEFAULT;
    siStaticPara.NomLatitude = PARA_NOM_LATITUDE_DEFAULT;
    siStaticPara.PositionAccuracy = PARA_POSITION_ACCURACY_DEFAULT;
    siStaticPara.TxChannel1 = PARA_CHANNEL_1_DEFAULT;
    siStaticPara.TxChannel2 = PARA_CHANNEL_2_DEFAULT;
    siStaticPara.PowerLevel = PARA_POWER_LEVEL_DEFAULT;
    siStaticPara.TypeofAtoN = PARA_ATON_TYPE_DEFAULT;
    siStaticPara.VirtualAtoNFlag = PARA_VIRTUAL_ATON_FLAG_DEFAULT;
    siStaticPara.TxHourChannel1 = PARA_TX_HOUR_CHANNEL_1_DEFAULT;
    siStaticPara.TxMinChannel1 = PARA_TX_MIN_CHANNEL_1_DEFAULT;
    siStaticPara.TxStartSlotChannel1 = PARA_TX_START_SLOT_CHANNEL_1_DEFAULT;
    siStaticPara.TxSlotIntervalChannel1 = PARA_TX_INTERVAL_CHANNEL_1_DEFAULT;
    siStaticPara.TxSlotIntervalChannel1Off = PARA_TX_INTERVAL_CHANNEL_1_OFF_DEFAULT;
    
    /*lq 信道2的发射时隙在FATDMA结构体参数中重新指定，此处仅提供初始值*/
    siStaticPara.TxHourChannel2 = PARA_TX_HOUR_CHANNEL_1_DEFAULT;
    siStaticPara.TxMinChannel2 = PARA_TX_MIN_CHANNEL_1_DEFAULT;
    siStaticPara.TxStartSlotChannel2 = PARA_TX_START_SLOT_CHANNEL_1_DEFAULT;
    siStaticPara.TxSlotIntervalChannel2 =PARA_TX_INTERVAL_CHANNEL_1_DEFAULT;
}
#endif

#if 0
/*******************************************************************************
* 名称  : InitDsDriverPara
* 描述  : 初始化驱动控制的参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitDsDriverPara(void)
{
	dsDriverPara.uc1846XmtSta = DRIVER_1846_POWERDOWN;
}
#endif

/*******************************************************************************
* 名称  : Parameter_Init
* 描述  : 初始化必要的参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void Parameter_Init(void)
{	
    DEVINFO_ModuleInit();
    AISINFO_ModuleInit();	
    IniEEPROM_Para();                                                           //从EEPROM中读取参数
	IniGpsPara();                                                               //lq 开机后立即给GPS供电
	InitGpsCell();
    InitaLEDCtrlPara();
	LEDInit(LED_TYPE_NONE);

#if CONFIG_POWERDOWN
	IniPowerDownPara();
#endif

	IniSleepCtrlPara();

#if CONFIG_BAT_MONITOR
    BS1_InstanceInit();                                                         //lq 电量检测模块
	InitBatteryCtrlPara();
#endif

#if CONFIG_TEMPERATURE_MONITOR    
    InitTemperSensorPara();                                                     //lq 温度检测	
#endif    

#if CONFIG_KEY
	IniMMIPara();	
	IniSingleKey(KEY_POWER_ADDR, KEY_HIGH);
	IniSingleKey(KEY_SOS_ADDR, KEY_HIGH);
#endif

	InitTimAdjust();                                                            //定时器调整初始化
	InitRtcPara();
    InitRtcParaStruct(&struRtcPara);
    InitPowerStru();                                                            //lq 按键状态初始化

#if CONFIG_GPS_UBX_EN
    UBX_ModuleInit();
#endif

    InitRtcTimPhaseStruct();

    RF1_RFInstanceInit();
    PHY_ModuleInit();
    DLS_ModuleInit();
    LME_ModuleInit();    
    TRCTRL_ModuleInit();    	
    AISDI_ModuleInit();
    MSGMAN_ModuleInit();
    TDMA_ModuleInit();
    InitRxTestStruct(&RxTestInstance);
    TASK_ModuleInit();

#if 0
#if 0
	InitaDigDeal();
	InitFSKDataTranPara();
	InitaLinkPtlPortPara();                                                     //lq 初始化消息队列（未调制之前）
    InitFSKDataXmtFIFO(&gstruFSKDataXmtFIFO);                                   //lq 初始化消息发射队列（调制之后）
	InitaReportPara();
	InitPowerUppara();
	IniUartMsgPara();		
	Ptl_Init();
	InitMsgCtrlPara();
    InitSOTDMAPara();
	IniTestingPara();
    IniGpsCtrlMsgTestStru(&GPSCtrlMsgTestPara);
#endif
    OP1_GPSOffPositionInstanceInit();
    LB1_LBPrtCommInstanceInit();
#endif

#if CONFIG_DEBUG
    /*lq 研发测试*/
#if 0
    InitPinPulse();
    BS_TestInstanceInit();                                                      //lq 电压AD采样点统计
    ADCTestInit();
#endif
#endif

#if CONFIG_SD_CARD_MODULE_EN
	/***************创建报文文件***********************************************/
	CreateFile();
#endif
}
#if 0
/*******************************************************************************
* 名称  : IsFixedAtoN
* 描述  : 是否是固定AtoN
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 IsFixedAtoN(void)
{
    if (siStaticPara.TypeofAtoN >= 5
        && siStaticPara.TypeofAtoN <= 19)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }
}
#endif
#if 0
/*******************************************************************************
* 名称  : GetAtoNAlarmFlag
* 描述  : 查询AtoN警告标志
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GetAtoNAlarmFlag(void)
{
    if ((siStaticPara.AtoNStatus & 0x1) == 1)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }
}
#endif

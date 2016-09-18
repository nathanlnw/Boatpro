/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	interfunc.h
  Author: 李斌      Version: V1.00       Date: 2012.03.21 
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
#ifndef _INITIAL_H
#define _INITIAL_H
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
#define EQ_FREQSTA_LOW			0		//休眠状态
#define EQ_FREQSTA_HIG			1		//正常工作状态

//#define EQ_MOD_POWERSAVE		0		//省电模式
#define EQ_MOD_NORMAL			0		//省电模式
#define EQ_MOD_POWERSAVE		1		//常规AIS模式

#define EQ_WORKMOD_TEST			0		//测试态
#define EQ_WORKMOD_NORMAL		1		//正常工作状态
#define EQ_WORKMOD_READFILE		2		//读取SD卡文件状态

#define EQ_XMTFREQ_CHA			161975000
#define EQ_XMTFREQ_CHB			162025000

#define EQ_MOD_OFFSET			1850		//默认状态下的频率准确度值
#define EQ_MOD_AMP				30			//默认状态下调制幅度的值

#define EQ_MOD_TEST_STA_MAIN        0
#define EQ_MOD_TEST_STA_BATTERY     1
#define EQ_MOD_TEST_STA_MSG         2
#define EQ_MOD_TEST_STA_RX          3
#define EQ_MOD_TEST_STA_SELF_CHECK  4

/*lq 字符长度 */
#define EQ_VEND_ID_LEN              7
#define EQ_ATON_NAME_LEN            34

/*lq */
#define VIRTUAL_ATON_FLAG_REAL      0
#define VIRTUAL_ATON_FLAG_VIRTUAL   1

#if 0
/**
	@ingroup 参数存取
	@brief 整机参数结构体
*/
typedef struct {						
	U8  ucLowFreqSta;					///<休眠标志
	U8  ucWorkMode;						///<测试态或正常工作状态
	U8  ucGpsOutput;					///<GPS语句输出状态
	
	U16 uiFreqAdj;						///<射频发射频率准确度
	U16 uiFreqOffset;					///<频率偏移
	U16 uiModAmpAdj;					///<调制幅度参数，16 ~ 48
	U16 uiModAmp;						///<调制幅度
	U32 uiXmtFreq;
    S32 slXmtFreqComp;                  //lq RDA1846发射频率补偿值	   slXmtFreqComp= siXmtFreqAdj*1850
	U16 GridVoltAdj;                    //lq 栅压DA控制调整
    S16 GridVoltOffset;

	U16 PowerOpenTime;                  //power按键开机时间
	U16 SosOpenTime;                    //sos按键开机时间
	U16 GpsHotTime;                     //GPS热启动超时时间
	U16 NormPoweronCntThres;            //正常开机次数阈值
	U16 BatteryLowLevel;                //电池电压低门限
	U16 BatteryUltraLowLevel;           //电池电压超低门限
	U16 CloseTime;                      //关机按键响应时间
	U16 UserTestCntThres;               //用户测试次数阈值
	U16 GpsPoweronProtectTime;          //gps开机定位保护时间，单位分钟
	U16 GpsFixFallThres;                //GPS热启动不定位保护阈值 
    U16 WorkTimeSum;                    //工作总时长
	U16 SendInterval;                   //消息发送间隔，单位分钟
	U16 NormalSendTime;                 //正常消息发送次数

	U8	BatteryTestSta;                 //测试态检测电池模式标志位
    U8  TestSta;
} EqipStruc;
#endif
#if 0
//静态消息结构体
/**
	@ingroup 参数存取
	@brief 静态参数结构体
*/
typedef struct {
	U32 MMSI;              // (xxxxxxxxx)
    U32 DestMMSI;          // (xxxxxxxxx)

    /*lq P712*/
	char VendID[EQ_VEND_ID_LEN + 1];          //lq 含‘\0’
	U32 SerialNumber;
	U8  Refresh;

    /*lq ACE(X Q)*/
    U8  AtoNStatus;       	// (hh)
    U16 OffPositionThresh;	// (xxxx)
    U8  AckProcedure;     	// (x)Type1不需要
    U8  OffPositionBehav; 	// (x)
    U8  SynchLostBehav;		// (x)
    char NameofAtoN[EQ_ATON_NAME_LEN + 1];    //含‘\0’, (c--c)maximum 34 characters
    U16	ShipA;			    // (xxx)
    U16	ShipB;			    // (xxx)
    U8	ShipC;			    // (xx)
    U8	ShipD;			    // (xx)
    
    /*lq ACF(X Q)*/
    U8	TypeofEPFS;		    // (x)
    S32	NomLongitude;		// (yyyyyyyyy)
    S32	NomLatitude;		// (llllllll)
    U8	PositionAccuracy;	// (x)
    U16 TxChannel1;		    // (xxxx)
    U16 TxChannel2;		    // (xxxx)
    U8	PowerLevel;		    // (x)
    U8  TypeofAtoN;		    // (xx)
    U8	VirtualAtoNFlag;	// (x)

    /*lq AAR(X Q)*/
    U8	TxHourChannel1;		// (xx)
    U8	TxMinChannel1;		// (xx)
    U16	TxStartSlotChannel1;// (xxxx)
    S32	TxSlotIntervalChannel1;	// (xxxxxxx)
    S32	TxSlotIntervalChannel1Off;	// (xxxxxxx)
    U8	TxHourChannel2;		// (xx)
    U8	TxMinChannel2;		// (xx)
    U16	TxStartSlotChannel2;// (xx)
    S32	TxSlotIntervalChannel2;	// (xxxxxxx)0-24*60*2250

} StaticInforStruc;
#endif

#if 0
extern StaticInforStruc	siStaticPara;		//静态参数
extern EqipStruc eqEquipPara;               //整机参数
#endif

#define DRIVER_1846_POWERDOWN		0
#define DRIVER_1846_XMT				1
#define DRIVER_1846_NONE			2
#define DRIVER_1846_REC				3

typedef struct {
	U8  uc1846XmtSta;					//1846发送接收状态控制
}DriverStru;

extern DriverStru dsDriverPara;
/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/
extern void Parameter_Init(void);
#endif

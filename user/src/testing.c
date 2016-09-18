/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: testing.c
  Author: vozhj      Version : V1.00        Date: 2012.03.21
  Description:    测试态模块
  Version:         V1.00
  Function List:   // 主要函数及其功能
  History:         // 历史修改记录
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#include "stm32f10x.h"
#include "def.h"
#include "initial.h"
#include "debug.h"
#include "common.h"
#include "parastorage.h"
#include "testing.h"
#include "systemtick.h"
#include "keyscan.h"
#include "ledctrl.h"
#include "gmskmod.h"
#include "digunit.h"
#include "ioctrl.h"
#include "LED.h"
#include "Tim.h"
#include "stdio.h"
#include "sleepctrl.h"
#include "adjust.h"
#include "gpsctrl.h"
#include "config.h"
#include "RxTx.h"
#include "Include.h"

extern void EnterStopMod(void);

#if 0
#define SINGLE_LENTH		(29)
const U8 SingleTwo[SINGLE_LENTH] =
{
	0x33,0x33,0x33,0x01,
	0x55,0x55,0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,0x55,0x55,
	0x81,0x1E,
	0x01,0x00,
};
#else
#define SINGLE_LENTH		(21)
#define SINGLE_T2_LENTH     (20)
const U8 SingleTwo[SINGLE_LENTH] =
{
	0x48,0x99,0x7,0xad,0x40,0x0,0x4c,0xb,0xa6,0x74,0x52,
	0xe4,0xdb,0x3d,0x86,0x32,0xac,0x0,0x1e,0x0,0x6,
};
#endif

U8 TestTempFlag = 0;
const U8 SingleT2[SINGLE_T2_LENTH] = 
{
	0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,
};
const U8 SingleT3[SINGLE_LENTH] = 
{
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,
};

/*lq SART标准测试信号1；01010101*/
const U8 STSignal_1[SINGLE_LENTH] = 
{
	0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,
	0x55,
};

/*lq SART标准测试信号2；00001111*/
const U8 STSignal_2[SINGLE_LENTH] = 
{
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,
};

/*lq SART标准测试信号3；伪随机序列*/
const U8 STSignal_3[SINGLE_LENTH] =
{
	0x48,0x99,0x7,0xad,0x40,0x0,0x4c,0xb,0xa6,0x74,0x52,
	0xe4,0xdb,0x3d,0x86,0x32,0xac,0x0,0x1e,0x0,0x6,
};

TestCtrlStruct TestCtrlPara;
BatteryTestCtrlStruct BatteryTestCtrlPara;
MsgTestStruct struMsgTestPara;

RxTestStruct RxTestInstance;

/*******************************************************************************
* 名称  : TEST_TxTestLedInit
* 描述  : 发射测试初始化
* 输入  : type - 测试信号类型
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TEST_TxTestLedInit(U8 type)
{
	if (type == TRCTRL_TEST_TX_SIGNAL_CW)
	{
		LEDInit(LED_TYPE_TEST_TEST1);
	}
	else if (type == TRCTRL_TEST_TX_SIGNAL_NUM1)
	{
		LEDInit(LED_TYPE_TEST_TEST2);
	}
	else if (type == TRCTRL_TEST_TX_SIGNAL_NUM2)
	{
		LEDInit(LED_TYPE_TEST_TEST3);
	}
	else if (type == TRCTRL_TEST_TX_SIGNAL_NUM3)
	{
		LEDInit(LED_TYPE_TEST_TEST4);
	}
}

/*******************************************************************************
* 名称  : TEST_TxTestInit
* 描述  : 发射测试初始化
* 输入  : channel - 发射信道
          type - 测试信号类型
          transceiverType - 收发机类型
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TEST_TxTestInit(AisChannelEnum channel, U8 type,
                     TransceiverTypeEnum transceiverType)
{
    RxTxCtrl(RXTX_CTRL_TYPE_TX, 0, 0);
    
    TRCTRL_InstTransceiverTestStructInit(transceiverType);
    
    TRCTRL_InstTxTestSetTxSignalType(transceiverType,
        type);
    
    TRCTRL_InstTxTestSetTxChannel(transceiverType,
        channel);
    
    TRCTRL_InstSetTxTestFlg(transceiverType); 
    
    TEST_TxTestLedInit(type);   
}

/*******************************************************************************
* 名称  : TEST_TxTestStop
* 描述  : 发射测试结束
* 输入  : transceiverType - 收发机类型
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TEST_TxTestStop(TransceiverTypeEnum transceiverType)
{  
    SI446XChipEnum cs;

    /*lq 选择实例 */
    if (transceiverType == TransceiverTypeA)
    {
        cs = SI446X_CHIP_A;
    }
    else if (transceiverType == TransceiverTypeB)
    {
        cs = SI446X_CHIP_B;
    }
    else
    {
        return ;
    }

    /*lq 退出收发状态 */
    RF_PowerDown();	    
    SI446X_StateTo(SI446X_MODE_IDLE, cs);
    TRCTRL_InstTransceiverTestStructInit(transceiverType);
    
    /*lq LED初始化 */
    LEDInit(LED_TYPE_RED_NONE);
}

/*******************************************************************************
* 名称  : InitRxTestStruct
* 描述  : 初始化接收测试结构体
* 输入  : pRxTestStruct - RxTestStruct结构体指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitRxTestStruct(RxTestStruct *pRxTestStruct)
{
	pRxTestStruct->RxTestEn = FALSE;
}


/*******************************************************************************
* 名称  : IniTestingPara
* 描述  : 初始化必要的参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void IniTestingPara(void)
{
	TestCtrlPara.TimeCnt = 0;
	TestCtrlPara.TimePeriod = TEST_200MS_PERIOD;

	TestCtrlPara.TimeCnt = SetTimePeriodProcess(TestCtrlPara.TimePeriod);

	TestCtrlPara.ucTestNum = TEST_TESTSTA_IDLE;
	TestCtrlPara.KeyScanSta = TEST_KEYSTA_IDLE;
	TestCtrlPara.LEDMsgFlag = FALSE;
	TestCtrlPara.ucRFXmtSta = TEST_RFXMT_IDLE;
	TestCtrlPara.ucXmtEn = FALSE;
	TestCtrlPara.ucXmtData = 0;
	TestCtrlPara.FirstGenFlag = TRUE;//开始信号没有产生

    TestCtrlPara.FirstEnterTestFlag = FALSE;
}

/*******************************************************************************
* 名称  : IniTestingPara
* 描述  : 初始化必要的参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void IniTesting(void)
{
	IniTestingPara();
	//InitFSKDataTranPara();
	DevStaticPara.uiXmtFreq = EQ_XMTFREQ_CHA;
#if 0
	RDA1846Ctrl(DRIVER_1846_POWERDOWN, DevStaticPara.uiXmtFreq, RDA1846_CS_2);
	//LEDInit(LED_TYPE_NONE);
#endif
}

/*******************************************************************************
* 名称  : ResetTesting
* 描述  : 复位测试相关参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ResetTesting(void)
{
	TestCtrlPara.ucTestNum = TEST_TESTSTA_IDLE;
	TestCtrlPara.KeyScanSta = TEST_KEYSTA_IDLE;
	TestCtrlPara.LEDMsgFlag = FALSE;
	TestCtrlPara.ucXmtEn = FALSE;
	TestCtrlPara.ucXmtData = 0;
	DevStaticPara.uiXmtFreq = EQ_XMTFREQ_CHA;
}
#if CONFIG_KEY     
/*******************************************************************************
* 名称  : TestingKeyScan
* 描述  : 测试态下按键扫描
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void Testing_KeyScan(void)
{
	U8 ucKeySta;
#if !WIN32
	ucKeySta = GetIndivIO(KEY_SOS_ADDR);

	if (TestCtrlPara.KeyScanSta == TEST_KEYSTA_IDLE)
	{
		//首先需要检测按键是否抬起，防止开机始终按着SOS按键
		if (ucKeySta == KEY_HIGH)
		{
			TestCtrlPara.KeyScanSta = TEST_KEYSTA_STEP1;
		}
	}
	else if (TestCtrlPara.KeyScanSta == TEST_KEYSTA_STEP1)
	{
		if (ucKeySta == KEY_LOW)
		{
#if 0
			//表示有按键按下，测试项加1
			TestCtrlPara.ucTestNum++;

			TestCtrlPara.FirstGenFlag = TRUE;
			if (TestCtrlPara.ucTestNum > TEST_TESTSTA_MOD)
			{
				TestCtrlPara.ucTestNum = TEST_TESTSTA_CARRIER;
			}
#endif
			TestCtrlPara.FirstGenFlag = TRUE;//应重新产生信号			  		//lq 测试项改变，需重新产生待发射的调制信号，仅当该测试项的首次发射时调制信号，该测试项的后续发射无需再进行调制
																				
			if (TestCtrlPara.ucTestNum == TEST_TESTSTA_CARRIER)
			{
				TestCtrlPara.ucTestNum = TEST_TESTSTA_T2;
			}
			else if (TestCtrlPara.ucTestNum == TEST_TESTSTA_T2)
			{
				TestCtrlPara.ucTestNum = TEST_TESTSTA_SIG2;
			}
			else if (TestCtrlPara.ucTestNum == TEST_TESTSTA_SIG2)
			{
				TestCtrlPara.ucTestNum = TEST_TESTSTA_STSIGNAL2;
			}
			else 		
			{
				TestCtrlPara.ucTestNum = TEST_TESTSTA_CARRIER;
			}
			TestCtrlPara.LEDMsgFlag = TRUE;                                     //lq 更改LED指示使能

			TestCtrlPara.KeyScanSta = TEST_KEYSTA_IDLE;
		}
	}
	else
	{
		TestCtrlPara.KeyScanSta = TEST_KEYSTA_IDLE;
	}
#endif
}
#endif
/*******************************************************************************
* 名称  : Testing_LEDCtrl
* 描述  : 测试态下LED控制
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void Testing_LEDCtrl(void)
{
    if (TestCtrlPara.FirstEnterTestFlag == TRUE)
    {
        TestCtrlPara.FirstEnterTestFlag = FALSE;

        /*lq 测试模式LED初始化 */    
		LEDInit(LED_TYPE_NONE);
		LEDInit(LED_TYPE_POWER_TEST);
    }

	if (TestCtrlPara.LEDMsgFlag == TRUE)
	{
		TestCtrlPara.LEDMsgFlag = FALSE;

		if (TestCtrlPara.ucTestNum == TEST_TESTSTA_CARRIER)
		{
			LEDInit(LED_TYPE_TEST_TEST1);
		}
		else if (TestCtrlPara.ucTestNum == TEST_TESTSTA_T2)
		{
			LEDInit(LED_TYPE_TEST_TEST2);
		}
		else if (TestCtrlPara.ucTestNum == TEST_TESTSTA_SIG2)
		{
			LEDInit(LED_TYPE_TEST_TEST3);
		}
		else if (TestCtrlPara.ucTestNum == TEST_TESTSTA_STSIGNAL2)
		{
			LEDInit(LED_TYPE_TEST_TEST4);
		}
	}
}

/*******************************************************************************
* 名称  : Testing_SetTestType
* 描述  : 串口设置当前工作模式
* 输入  : ucType：工作模式		  
* 输出  : 无
* 返回  : 成功/失败
*******************************************************************************/
U8 Testing_SetTestType(U8 ucType, U32 ulFreq)
{
	if (ucType == TEST_TESTSTA_FREQADJ)
	{
		if (TestCtrlPara.ucTestNum != TEST_TESTSTA_FREQADJEN)
		{
			return (FALSE);
		}
	}
	else if (ucType == TEST_TESTSTA_MODADJ)
	{
		if (TestCtrlPara.ucTestNum != TEST_TESTSTA_MODADJEN)
		{
			return (FALSE);
		}
	}
    else if (ucType == TEST_TESTSTA_GRIDADJ)
    {
		if (TestCtrlPara.ucTestNum != TEST_TESTSTA_GRIDADJEN)
		{
			return (FALSE);
		}    
    }

	else
	{
		TestCtrlPara.FirstGenFlag = TRUE;//应重新产生信号
		TestCtrlPara.ucTestNum = ucType;
		DevStaticPara.uiXmtFreq = ulFreq;
	}

    TestCtrlPara.LEDMsgFlag = TRUE;

	return (TRUE);
}

extern S16 gusSXCount;	       		//2250时隙计数
/*******************************************************************************
* 名称  : Testing_CarrierTest
* 描述  : 测试态下载波测试项
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
#define TEST_TESTBUF_LEN	60
void GMSKTest(void)                                                             //lq 测试态状态控制
{
#if 0
	if (TestCtrlPara.ucRFXmtSta == TEST_RFXMT_IDLE)
	{
		if (TestCtrlPara.ucXmtEn == TRUE)					 					//lq 启动新的发射	
		{
			TestCtrlPara.ucXmtEn = FALSE;
			TestCtrlPara.ucRFXmtSta = TEST_RFXMT_STEP1;
		}
	}
	else if (TestCtrlPara.ucRFXmtSta == TEST_RFXMT_STEP1)
	{
        RxTxSwitch(RXTX_SWITCH_TYPE_RX_TO_TX);                           
#if 0
        RDA1846Ctrl(DRIVER_1846_XMT, DevStaticPara.uiXmtFreq + DevStaticPara.slXmtFreqComp, RDA1846_CS_2);//lq
#endif 
         
		if ((TestCtrlPara.ucXmtType == 0)
			|| (TestCtrlPara.ucXmtType == 1))
		{
			//GMSK调制
			GMSKModTest(&TestCtrlPara.ucXmtBuf[0], 
						FSKDATA_NOWSLOT, 
						TestCtrlPara.uiXmtBufLen, 
						DevStaticPara.uiModAmp, 
						DevStaticPara.uiFreqOffset,
						TestCtrlPara.ucXmtType);//注意这种调制不带帧头帧尾
		}
		else
		{
#if 0
			GMSKModTest2(&TestCtrlPara.ucXmtBuf[0], //这个是正常的调制函数
							FSKDATA_NOWSLOT, 
							TestCtrlPara.uiXmtBufLen, 
							DevStaticPara.uiModAmp, 
							DevStaticPara.uiFreqOffset);			
#else
			GMSKModTest3(&TestCtrlPara.ucXmtBuf[0], //lq 不使用NRZI编码数据及校验码
							FSKDATA_NOWSLOT, 
							TestCtrlPara.uiXmtBufLen, 
							DevStaticPara.uiModAmp, 
							DevStaticPara.uiFreqOffset);
#endif		
        }

		pFSKDataXmtStru2 = &FSKDataXmtStru;//lq+
		SetFSKDataXmtEn();

		TestCtrlPara.ucRFXmtSta = TEST_RFXMT_STEP2;		
//lq-	   	TestCtrlPara.ucRFXmtSta = TEST_RFXMT_IDLE;	
	}
	else if (TestCtrlPara.ucRFXmtSta == TEST_RFXMT_STEP2)
	{
		//等待调制数据发送完成
//lq-		if (FSKDataXmtStru.uiTran_Sta == FSKDATA_TRAN_IDLE)
	    if (!GetFSKDataXmtEn())	
    	{
			//DigUnitPara.TranEnd = FALSE;

			//关闭1846
//lq-			RDA1846Ctrl(DRIVER_1846_POWERDOWN, DevStaticPara.uiXmtFreq);
			TestCtrlPara.ucRFXmtSta = TEST_RFXMT_IDLE;
		}
	}
	else
	{
		 TestCtrlPara.ucRFXmtSta = TEST_RFXMT_IDLE;
	}
#endif    	
}



/*******************************************************************************
* 名称  : Testing_ModTest
* 描述  : 测试态下调制测试项
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void Testing_ModTest(U8 ucType)                                                 //lq 发射使能，并指定需调制的数据
{
	U16 i;
	TestCtrlPara.ucXmtEn = TRUE;                                                //lq 发射使能

	if (TestCtrlPara.FirstGenFlag == TRUE)                                      //lq 处于某个测试项时只执行一次，仅在该测试项的第一次发射时执行
	{	

		if (ucType == TEST_XMTTYPE_CARRIER)
		{
			for (i = 0; i < TEST_TESTBUF_LEN; i++)
			{
				TestCtrlPara.ucXmtBuf[i] = 0;
			}
			TestCtrlPara.uiXmtBufLen = TEST_TESTBUF_LEN;
			TestCtrlPara.ucXmtType = 1;
		}
		else if (ucType == TEST_XMTTYPE_MOD)
		{
			for (i = 0; i < TEST_TESTBUF_LEN; i++)
			{
				TestCtrlPara.ucXmtBuf[i] = 0x55;
			}
			TestCtrlPara.uiXmtBufLen = TEST_TESTBUF_LEN;
			TestCtrlPara.ucXmtType = 0;
		}
		else if (ucType == TEST_XMTTYPE_SIG2)
		{
			for (i = 0; i < SINGLE_LENTH; i++)
			{
				TestCtrlPara.ucXmtBuf[i] = SingleTwo[i];
			}
			TestCtrlPara.uiXmtBufLen = SINGLE_LENTH;
			TestCtrlPara.ucXmtType = 2;
		}
		else if (ucType == TEST_XMTTYPE_T2)
		{
			for (i = 0; i < SINGLE_T2_LENTH; i++)
			{
				TestCtrlPara.ucXmtBuf[i] = SingleT2[i];
			}
			TestCtrlPara.uiXmtBufLen = SINGLE_T2_LENTH;
			TestCtrlPara.ucXmtType = 2;
		}
		else if (ucType == TEST_XMTTYPE_T3)
		{
			for (i = 0; i < SINGLE_LENTH; i++)
			{
				TestCtrlPara.ucXmtBuf[i] = SingleT3[i];
			}
			TestCtrlPara.uiXmtBufLen = SINGLE_LENTH;
			TestCtrlPara.ucXmtType = 2;
		}
		else if (ucType == TEST_XMTTYPE_STSIGNAL2)
		{
			for (i = 0; i < SINGLE_LENTH; i++)
			{
				TestCtrlPara.ucXmtBuf[i] = STSignal_2[i];
			}
			TestCtrlPara.uiXmtBufLen = SINGLE_LENTH;
			TestCtrlPara.ucXmtType = 2;
		}	
	}	
}

/*******************************************************************************
* 名称  : Testing_TestMain
* 描述  : 测试态下调制测试项
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void Testing_TestMain(void)
{
	//200ms周期发射计时
	if (TestCtrlPara.TimeCnt == GetLocalClockTickProcess())						//lq 每200ms发射一次
	{
		TestCtrlPara.TimeCnt = SetTimePeriodProcess(TestCtrlPara.TimePeriod);

		if ((TestCtrlPara.ucTestNum == TEST_TESTSTA_CARRIER)
			|| (TestCtrlPara.ucTestNum == TEST_TESTSTA_FREQADJEN))
		{
			Testing_ModTest(TEST_XMTTYPE_CARRIER);							   	//lq 允许新的一次发射，并指定发射类型和待调制的数据
		}
		else if ((TestCtrlPara.ucTestNum == TEST_TESTSTA_MOD)
				|| (TestCtrlPara.ucTestNum == TEST_TESTSTA_MODADJEN))
		{
			Testing_ModTest(TEST_XMTTYPE_MOD);
		}
		else if (TestCtrlPara.ucTestNum == TEST_TESTSTA_SIG2)
		{
			Testing_ModTest(TEST_XMTTYPE_SIG2);	
		}
		else if (TestCtrlPara.ucTestNum == TEST_TESTSTA_T2)
		{
			Testing_ModTest(TEST_XMTTYPE_T2);	
		}
		else if (TestCtrlPara.ucTestNum == TEST_TESTSTA_T3)
		{
			Testing_ModTest(TEST_XMTTYPE_T3);	
		}
		else if (TestCtrlPara.ucTestNum == TEST_TESTSTA_STSIGNAL2)
		{
			Testing_ModTest(TEST_XMTTYPE_STSIGNAL2);	
		}
	}

	GMSKTest();
}
/*******************************************************************************
* 名称  : Testing
* 描述  : 测试态模块
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void Testing(void)
{
#if CONFIG_KEY     
	//此处加入按键扫描
	Testing_KeyScan();
#endif	
	//测试态发射测试
	Testing_TestMain();

	//此处加入LED控制
	Testing_LEDCtrl();

	RtcClockChoice();//加入RTC时钟源的调整
}

/*******************************************************************************
* 名称  : BatteryTestCtrlLowFreq
* 描述  : 电池电量检测模式低频函数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void BatteryTestCtrlLowFreq(void)
{
	U8  ucPowerKeyVal;

	EnterStopMod();

	if (GetMsgTestCtrlWakeupFlag() == TRUE)
	{
		DevDynamicPara.ucLowFreqSta = EQ_FREQSTA_HIG;
//lq        TEST_OUT_HIGH();   //lq 开关电源置为低噪声模式
	}

#if CONFIG_KEY     
	ucPowerKeyVal = ReadSingleKeyValProcess(&PowerKey);//当前按键状态
	if (ucPowerKeyVal == KEY_LOW)
	{
		DevStaticPara.ucLowFreqSta = EQ_FREQSTA_HIG;
//lq        TEST_OUT_HIGH();   //lq 开关电源置为低噪声模式	
	}
#endif
}

/*******************************************************************************
* 名称  : InitBatteryTest
* 描述  : 初始化电池电量检测参数,此函数在串口配置的时候调用
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitBatteryTest(void)
{
#if 0
	ResetTimeCount();
	
	BatteryTestCtrlPara.CtrlState = BATTERY_TEST_STATE_POWERUP;
	
	BatteryTestCtrlPara.TimeCntGps = 0;
	BatteryTestCtrlPara.TimeCntMessage = 0;
	BatteryTestCtrlPara.TimePeriod = 0;

	BatteryTestCtrlPara.GpsPowerOn = FALSE;

	DevStaticPara.ucLowFreqSta = EQ_FREQSTA_HIG;
	
	BatteryTestCtrlPara.ClockCount = 0;//时钟清零
	BatteryTestCtrlPara.ClockSlotPast = 0;
		
	BatteryTestCtrlPara.FrameCntPast = 0;

	InitMsgTestCtrl();
	
	InitSleepTestCtrl();

	//BatteryTestCtrlPara.WaitForSleep = FALSE;
#else
#if 0    
    GpsCtrlPara.GpsCtrlSta = GPS_CTRL_STA_NORMAL_WORKING;    //lq GPS进入开机状态

    usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : GPS_CTRL_STA_NORMAL_WORKING!\r\n", GetFrameNum() ,GetSlotNum());	                
#else
    GpsCtrlPara.GpsCtrlSta = ATON_GPS_CTRL_STA_NORMAL_WORKING;    //lq GPS进入开机状态

    usart_printf("[%6d:%4d]$GPS_CTRL_STATE_BatTest : ATON_GPS_CTRL_STA_NORMAL_WORKING!\r\n", GetFrameNum() ,GetSlotNum());
#endif
#endif
}

///*******************************************************************************
//* 名称  : BatteryTestCtrlLowFreq
//* 描述  : 电池电量检测模式控制函数
//* 输入  : 无		  
//* 输出  : 无
//* 返回  : 无
//*******************************************************************************/
//void BatteryTestCtrl(void)
//{
//	S32 TempFrameNum;
//	U8 ucTemp[10];
//	
//	if (BatteryTestCtrlPara.CtrlState == BATTERY_TEST_STATE_POWERUP)
//	{
//		if (BatteryTestCtrlPara.GpsPowerOn == FALSE)
//		{
//			IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_HIGH);
//			UartResponseMsg("gps power on");
//
//			LEDInit(LED_TYPE_GPS_FIX);//模拟定位
//			
//			BatteryTestSendMsg();//发送消息
//			
//			//发送定时配置
//			BatteryTestCtrlPara.TimePeriod = DevStaticPara.SendInterval * BATTERY_TEST_MIN_COUNT;//发送间隔
//			BatteryTestCtrlPara.TimeCntMessage = SetBatteryTestTimer(BatteryTestCtrlPara.TimePeriod);//设置消息发送间隔
//			
//			//gps开机保护配置
//			BatteryTestCtrlPara.TimePeriod = DevStaticPara.GpsPoweronProtectTime * BATTERY_TEST_MIN_COUNT;
//			BatteryTestCtrlPara.TimeCntGps = SetBatteryTestTimer(BatteryTestCtrlPara.TimePeriod); 
//				
//			BatteryTestCtrlPara.GpsPowerOn = TRUE;
//
//			UartResponseMsg("begin battery test");
//		}
//
//		//到达消息发送定时间隔
//		if (BatteryTestCtrlPara.TimeCntMessage == GetBattertTestTick())
//		{
//			BatteryTestSendMsg();//发送消息  标准信号2
//
//			//设置下次发送
//			BatteryTestCtrlPara.TimePeriod = DevStaticPara.SendInterval * BATTERY_TEST_MIN_COUNT;//发送间隔
//			BatteryTestCtrlPara.TimeCntMessage = SetBatteryTestTimer(BatteryTestCtrlPara.TimePeriod);//设置消息发送间隔
//		}
//		
//		//到达gps开机定位保护结束
//		if (BatteryTestCtrlPara.TimeCntGps == GetBattertTestTick())
//		{
//			IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_LOW);//关闭GPS
//			UartResponseMsg("gps power off");
//			LEDInit(LED_TYPE_NONE);//关闭LED
//
//			if (TestCtrlPara.ucRFXmtSta == TEST_RFXMT_IDLE)//如果发送完毕
//			{
//				UartResponseMsg("battery test gps poweron protect finish");	
//						
//				LEDInit(LED_TYPE_NONE);//关闭LED
//				DevStaticPara.ucLowFreqSta = EQ_FREQSTA_LOW;//进入低频模式
//				BatteryTestCtrlPara.CtrlState = BATTERY_TEST_STATE_LOWFREQ;//进入低频模式
//				UartResponseMsg("BatteryTestCtrlPara.CtrlState -> BATTERY_TEST_STATE_LOWFREQ");
//			}
//		}
//		
//		//高频保存时间
//		TempFrameNum = GetFrameNum();
//		if (BatteryTestCtrlPara.FrameCntPast != TempFrameNum)
//		{
//			BatteryTestCtrlPara.FrameCntPast = TempFrameNum;
//			//存储已经工作了多久
//			sprintf((char *)ucTemp,"%5d",TempFrameNum);
//			Para_Save(PARA_BATTERY_TEST_TIME_TYPE,ucTemp,5);
//		}		
//	}
//	else if (BatteryTestCtrlPara.CtrlState == BATTERY_TEST_STATE_LOWFREQ)
//	{
//		//如果到达了要发消息时间
//		if (BatteryTestCtrlPara.TimeCntMessage == GetBattertTestTick())
//		{
//			IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_HIGH);//开GPS
//			UartResponseMsg("gps power on");
//
//			LEDInit(LED_TYPE_GPS_FIX);//模拟定位
//
//			UartResponseMsg("battery test wakeup");
//
//			//GPS开启定时10秒
//			BatteryTestCtrlPara.TimePeriod = BATTERY_TEST_GPS_HOT_POWERUP;
//			BatteryTestCtrlPara.TimeCntGps = SetBatteryTestTimer(BatteryTestCtrlPara.TimePeriod);
//
//			//发送定时配置
//			BatteryTestCtrlPara.TimePeriod = DevStaticPara.SendInterval * BATTERY_TEST_MIN_COUNT;//发送间隔
//			BatteryTestCtrlPara.TimeCntMessage = SetBatteryTestTimer(BatteryTestCtrlPara.TimePeriod);//设置消息发送间隔
//		
//			DevStaticPara.ucLowFreqSta = EQ_FREQSTA_HIG;
//			BatteryTestCtrlPara.CtrlState = BATTERY_TEST_STATE_HIGHFREQ;//进入高频模式
//			UartResponseMsg("BatteryTestCtrlPara.CtrlState -> BATTERY_TEST_STATE_HIGHFREQ");
//		}
//	}
//
//	else if (BatteryTestCtrlPara.CtrlState == BATTERY_TEST_STATE_HIGHFREQ)
//	{
//		//高频保存时间
//		TempFrameNum = GetFrameNum();
//		if (BatteryTestCtrlPara.FrameCntPast != TempFrameNum)
//		{
//			BatteryTestCtrlPara.FrameCntPast = TempFrameNum;
//			//存储已经工作了多久
//			sprintf((char *)ucTemp,"%5d",TempFrameNum);
//			Para_Save(PARA_BATTERY_TEST_TIME_TYPE,ucTemp,5);
//		} 
//		
//		if (BatteryTestCtrlPara.TimeCntGps == GetBattertTestTick())//10秒的gps启动完成
//		{
//			IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_LOW);//关GPS
//			UartResponseMsg("gps power off");
//			
//			BatteryTestSendMsg();//发送消息
//
//			BatteryTestCtrlPara.TimePeriod = 2;//4秒间隔
//			BatteryTestCtrlPara.TimeCntWait = SetBatteryTestTimer(BatteryTestCtrlPara.TimePeriod);
//
//			BatteryTestCtrlPara.CtrlState = BATTERY_TEST_STATE_WAIT;//进入等待模式
//			UartResponseMsg("BatteryTestCtrlPara.CtrlState -> BATTERY_TEST_STATE_WAIT");
//		}
//	}
//	else if (BatteryTestCtrlPara.CtrlState == BATTERY_TEST_STATE_WAIT)
//	{
//		if (BatteryTestCtrlPara.TimeCntWait == GetBattertTestTick())//直接等待不判断
//		{
//			DevStaticPara.ucLowFreqSta = EQ_FREQSTA_LOW;//进入低频模式
//			BatteryTestCtrlPara.CtrlState = BATTERY_TEST_STATE_LOWFREQ;//进入低频模式
//			UartResponseMsg("BatteryTestCtrlPara.CtrlState -> BATTERY_TEST_STATE_LOWFREQ");
//		}
//	}
//}		



/*******************************************************************************
* 名称  : BatteryTestSendMsg
* 描述  : 发送标准信号2
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void BatteryTestSendMsg(void)
{
	LEDInit(LED_TYPE_RF_XMT);
	Testing_ModTest(TEST_XMTTYPE_SIG2);	
	UartResponseMsg("send battery test sig2");
}

/*******************************************************************************
* 名称  : BattertTestClock
* 描述  : 全局定时器，不受高/低频影响	2秒精度
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void BattertTestClock(void)
{
	S16 TempSlotCnt;
	//S32 TempFrameCnt;

	//TempFrameCnt = GetFrameNum();
	TempSlotCnt = GetSlotNum();

	if (TempSlotCnt > BatteryTestCtrlPara.ClockSlotPast)
	{
		if ((TempSlotCnt - BatteryTestCtrlPara.ClockSlotPast) > BATTERY_TEST_CLOCK_INTERVAL)
		{
			BatteryTestCtrlPara.ClockCount++;
			usart_printf("clock tick %d\r\n",2 * BatteryTestCtrlPara.ClockCount);
			BatteryTestCtrlPara.ClockSlotPast = TempSlotCnt;
		}
	}
	else if (BatteryTestCtrlPara.ClockSlotPast > TempSlotCnt)
	{
		if ((TempSlotCnt + 2250 - BatteryTestCtrlPara.ClockSlotPast) > BATTERY_TEST_CLOCK_INTERVAL)
		{
			BatteryTestCtrlPara.ClockCount++;
			usart_printf("clock tick %d\r\n",2 * BatteryTestCtrlPara.ClockCount);
			BatteryTestCtrlPara.ClockSlotPast = TempSlotCnt;
		}
	}
}		

/*******************************************************************************
* 名称  : SetBatteryTestTimer
* 描述  : 设置定时器要等待的时间
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U32 SetBatteryTestTimer(U32 period)
{
	return (BatteryTestCtrlPara.ClockCount + period);
}

/*******************************************************************************
* 名称  : GetBattertTestTick
* 描述  : 查询时钟值
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U32 GetBattertTestTick(void)
{
	return (BatteryTestCtrlPara.ClockCount);
}

//////////////////////////////////////////////////
//消息发送控制
MsgTestCtrlStruct MsgTestCtrlPara; 

/*******************************************************************************
* 名称  : InitMsgTestCtrl
* 描述  : 初始化消息控制结构体
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitMsgTestCtrl(void)
{
	MsgTestCtrlPara.CtrlState = MSG_TEST_STATE_POWERUP;
	MsgTestCtrlPara.TimePeriod = 0;
	MsgTestCtrlPara.SendIntervalTimeCnt = 0;
	MsgTestCtrlPara.GpsPoweronTimeCnt = 0;
	MsgTestCtrlPara.GpsProtectTimeCnt = 0;
	MsgTestCtrlPara.WakeupFlag = FALSE;	
	MsgTestCtrlPara.GpsPowerOn = FALSE;
	MsgTestCtrlPara.GpsProtect = FALSE;
	MsgTestCtrlPara.FrameCntPast = 0;
}

#if 0
/*******************************************************************************
* 名称  : MsgTestCtrl
* 描述  : 电量检测模式下控制消息发模块
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void MsgTestCtrl(void)
{
	S32 TempFrameNum;
	U8 ucTemp[10];
	
	if (MsgTestCtrlPara.CtrlState == MSG_TEST_STATE_POWERUP)
	{
		BatteryTestSendMsg();//发送消息
		
		IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_HIGH);//开GPS
		LEDInit(LED_TYPE_GPS_FIX);

		MsgTestCtrlPara.GpsPowerOn = TRUE;

		UartResponseMsg("first gps power on");

		//发送定时配置
		MsgTestCtrlPara.TimePeriod = DevStaticPara.SendInterval * BATTERY_TEST_MIN_COUNT;//发送间隔
		MsgTestCtrlPara.SendIntervalTimeCnt = SetBatteryTestTimer(MsgTestCtrlPara.TimePeriod);//设置消息发送间隔
		
		//gps开机保护配置
		MsgTestCtrlPara.TimePeriod = DevStaticPara.GpsPoweronProtectTime * BATTERY_TEST_MIN_COUNT;
		MsgTestCtrlPara.GpsProtectTimeCnt = SetBatteryTestTimer(MsgTestCtrlPara.TimePeriod); 
			
		MsgTestCtrlPara.GpsPowerOn = TRUE;
		MsgTestCtrlPara.GpsProtect = TRUE;//gps开机保护	

		MsgTestCtrlPara.CtrlState = MSG_TEST_STATE_WAIT;

		UartResponseMsg("MsgTestCtrlPara.CtrlState -> MSG_TEST_STATE_WAIT");
	}
	else if (MsgTestCtrlPara.CtrlState == MSG_TEST_STATE_WAIT)
	{
		if (MsgTestCtrlPara.SendIntervalTimeCnt == GetBattertTestTick())
		{
			if (MsgTestCtrlPara.GpsPowerOn == FALSE)
			{
				IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_HIGH);//开GPS
				LEDInit(LED_TYPE_GPS_FIX);
				MsgTestCtrlPara.GpsPowerOn = TRUE;

				UartResponseMsg("normal gps power on");
			}
			
			//发送定时配置
			MsgTestCtrlPara.TimePeriod = DevStaticPara.SendInterval * BATTERY_TEST_MIN_COUNT;//发送间隔
			MsgTestCtrlPara.SendIntervalTimeCnt = SetBatteryTestTimer(MsgTestCtrlPara.TimePeriod);//设置消息发送间隔	

			//GPS开启定时10秒
			MsgTestCtrlPara.TimePeriod = BATTERY_TEST_GPS_HOT_POWERUP;
			MsgTestCtrlPara.GpsPoweronTimeCnt = SetBatteryTestTimer(MsgTestCtrlPara.TimePeriod);

			if (DevStaticPara.ucLowFreqSta == EQ_FREQSTA_LOW)
			{
				MsgTestCtrlPara.WakeupFlag = TRUE;
			}
			
			MsgTestCtrlPara.CtrlState = MSG_TEST_STATE_GPS;
			UartResponseMsg("MsgTestCtrlPara.CtrlState -> MSG_TEST_STATE_GPS");
		}
	}
	else if (MsgTestCtrlPara.CtrlState == MSG_TEST_STATE_GPS)
	{
		//高频保存时间
		TempFrameNum = GetFrameNum();
		if (MsgTestCtrlPara.FrameCntPast != TempFrameNum)
		{
			MsgTestCtrlPara.FrameCntPast = TempFrameNum;
			//存储已经工作了多久
#if CONFIG_PARASTORAGE_BTT
			sprintf((char *)ucTemp,"%5d",TempFrameNum);
			Para_Save(PARA_BATTERY_TEST_TIME_TYPE,ucTemp,5);
#endif
		} 
		
		if (MsgTestCtrlPara.GpsPoweronTimeCnt == GetBattertTestTick())//10秒到达
		{
			if (MsgTestCtrlPara.GpsProtect == FALSE)//如果不是GPS长开保护
			{	
				IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_LOW);//关GPS
				MsgTestCtrlPara.GpsPowerOn = FALSE;	

				UartResponseMsg("normal gps power off");
			}

			BatteryTestSendMsg();//发送消息
			
			MsgTestCtrlPara.CtrlState = MSG_TEST_STATE_WAIT;
		}
	}

	if (MsgTestCtrlPara.GpsProtect == TRUE)
	{
		if (MsgTestCtrlPara.GpsProtectTimeCnt == GetBattertTestTick())//保护时间到
		{
			MsgTestCtrlPara.GpsProtect = FALSE;

			IO_Ctrl(IO_GPSPOWER_TYPE, IO_VAL_LOW);//关GPS
			MsgTestCtrlPara.GpsPowerOn = FALSE;
			
			//LEDInit(LED_TYPE_NONE);//关闭LED
			UartResponseMsg("protect gps power off");
		}
	}
}	
#endif

/*******************************************************************************
* 名称  : GetGpsTestCtrlWakeupFlag
* 描述  : 查看是否GPSTEST要唤醒
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GetMsgTestCtrlWakeupFlag(void)
{
	if (MsgTestCtrlPara.WakeupFlag == TRUE)
	{
		MsgTestCtrlPara.WakeupFlag = FALSE;
		return TRUE;	
	}
	else
	{
		return FALSE;
	}	
}


////////////////////////////////////////////
//休眠控制部分
SleepTestCtrlStruct SleepTestCtrlPara;

void InitSleepTestCtrl(void)
{
	SleepTestCtrlPara.CtrlState = SLEEP_TEST_STATE_IDEL;
	SleepTestCtrlPara.TimePeriod = 0;
	SleepTestCtrlPara.TimeCnt = 0;
	SleepTestCtrlPara.WaitCount = 0;
}

/*******************************************************************************
* 名称  : SleepTestCtrl
* 描述  : 电量检测模式下休眠控制
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SleepTestCtrl(void)
{
	U8 ucPowerKeyVal;
		
	if (SleepTestCtrlPara.CtrlState == SLEEP_TEST_STATE_IDEL)
	{
#if CONFIG_KEY     
		ucPowerKeyVal = ReadSingleKeyValProcess(&PowerKey);//当前按键状态
#endif		
		if (MsgTestCtrlPara.GpsPowerOn == FALSE 
			&& TestCtrlPara.ucRFXmtSta == TEST_RFXMT_IDLE
#if CONFIG_KEY     
			&& ucPowerKeyVal == KEY_HIGH
#endif             
			)//发送完毕
		{
			UartResponseMsg("sleep condition confirm");
			SleepTestCtrlPara.TimePeriod = SLEEP_PRT_PERIOD;
			SleepTestCtrlPara.TimeCnt = SetTimePeriodProcess(SleepTestCtrlPara.TimePeriod);
			SleepTestCtrlPara.WaitCount = 0;
			SleepTestCtrlPara.CtrlState = SLEEP_TEST_STATE_STEP1;
		}
	}
	else if (SleepTestCtrlPara.CtrlState == SLEEP_TEST_STATE_STEP1)
	{
		if (MsgTestCtrlPara.GpsPowerOn != FALSE || TestCtrlPara.ucRFXmtSta != TEST_RFXMT_IDLE)
		{
			SleepTestCtrlPara.CtrlState = SLEEP_TEST_STATE_IDEL;
			UartResponseMsg("back to awake");
		}
		else
		{
			if (SleepTestCtrlPara.TimeCnt == GetLocalClockTickProcess())
			{
				SleepTestCtrlPara.TimeCnt = SetTimePeriodProcess(SleepTestCtrlPara.TimePeriod);//这个定时10ms的作用
				SleepTestCtrlPara.WaitCount++;	
			}

			if (SleepTestCtrlPara.WaitCount >= SLEEP_1S_PRTTIME)
			{
				UartResponseMsg("sleep");
				LEDInit(LED_TYPE_NONE);//关闭LED
//lq                TEST_OUT_LOW();   //lq 开关电源置为高噪声模式
				DevDynamicPara.ucLowFreqSta = EQ_FREQSTA_LOW;
				SleepTestCtrlPara.CtrlState = SLEEP_TEST_STATE_IDEL;
			}
		}
	}
	
	RtcClockChoice();//加入RTC时钟源的选择					
}

/*******************************************************************************
* 名称  : InitMsgTestPara
* 描述  : 初始化消息测试模块参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitMsgTestPara(void)
{
    struMsgTestPara.ucPowerDownEn = FALSE;
}

/*******************************************************************************
* 名称  : InitMsgTest
* 描述  : 初始化消息测试模块参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitMsgTest(void)
{
#if 0
	ResetTimeCount();

    InitMsgTestPara();

    IniGpsPara();
	InitGpsCell();

	InitaDigDeal();
	InitFSKDataTranPara();
	InitaLinkPtlPortPara();                                                     //lq 初始化消息队列（未调制之前）
    InitFSKDataXmtFIFO(&gstruFSKDataXmtFIFO); 

	LEDInit(LED_TYPE_NONE);

	Ptl_Init();

    InitSOTDMAPara();

    ResetFSKDataXmtEn();
    ResetFetchFirstFskDataEn();

	DevStaticPara.ucLowFreqSta = EQ_FREQSTA_HIG;
#endif
}

/*******************************************************************************
* 名称  : DigDealMain_MsgTest
* 描述  : 消息发射测试下，消息的组建、发送和调制
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void DigDealMain_MsgTest(void)
{
#if 0
	/*lq 进行消息的组建和发送*/
    Ptl_Main_Test();                                                                 //lq 从解析后的GPS消息中提取信息构建x号消息，并将构建的消息放入协议栈

    /*lq 调制发送队列中的消息*/
	DigDealUnit();
#endif
}

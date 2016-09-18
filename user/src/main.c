/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: main.c
  Author: 张键       Version : V1.00        Date: 2011.09.23
  Description:    建立程序
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			1．函数名：Main
    	   函数功能：控制ARM整个程序的架构
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
       lq       20150806              实现M4508D各功能模块控制
*******************************************************************************/
#include <string.h>
#include <stdio.h>
#include "stm32f10x.h"
#include "def.h"
#include "common.h"
#include "initial.h"
#include "systemtick.h"
#include "debug.h"
#include "powerup.h"
#include "powerdown.h"
#include "ledctrl.h"
#include "keyscan.h"
#include "testing.h"
#include "digunit.h"
#include "mmi.h"
#include "gpsctrl.h"
#include "batteryctrl.h"
#include "usart.h"
#include "sleepctrl.h"
#include "LED.h"
#include "24c02.h"
#include "PTL_Main.h"
#include "wdg.h"
#include "adjust.h"
#include "rtc.h"
#include "batteryscan.h"
#include "config.h"
#include "Include.h"
#include "ff.h"
#include "flashctrl.h"
#include "parastorage.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern TransceiverTxCtrlStruct TransceiverTxCtrlInstance;
/* Private functions ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private  function  --------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
/*******************************************************************************
* 名称  : main
* 描述  : 入口程序
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
int main(void)
{
	TransceiverTxCtrlStruct *pTransceiverTxCtrlStruct;
	U8 	ucTempBuf[40];
	U8 	ucCount;
	char chRetBuf[100];
	
    PowerUpProcess();

	while(1)
	{
		Feed_Dog();

		if (DevDynamicPara.ucWorkMode == EQ_WORKMOD_NORMAL)                     //lq 工作模式
		{
			LocalClockCtrlMainProcess();
#if CONFIG_KEY
			KeyScanProcess();
			MMIProcess();
#endif                	
			LEDCtrlProcess();
#if CONFIG_BAT_MONITOR 
            BS1_BatteryScanProcess();               
            BatteryCtrlProcess();
#endif
#if CONFIG_TEMPERATURE_MONITOR
			CalcuTemper();
#endif
#if CONFIG_POWERDOWN
            PowerDownProcess();	
#endif

			GetCOMMMess(COMM2);

#if CONFIG_GPS_UBX_EN
            UBX_GetCOMMMess(COMM2);
            UBX1_UBXPrtCommCtrlProcess();
#endif	
            GpsCtrlProcess();
			TimAdjustProcess();            
            AisMain();
#if 0
            TRCTRL_InstTxTestCtrl();               
#endif     


           //========================= 
           #if  CONFIG_PTT_EN	// lnw  create
              PTT_Work_Process();
           #endif
           //=========================



            TASK_TaskProcess();
			GetCOMMMess(COMM1);
        }
		else                                                                    //lq 测试模式
		{
	        if (DevDynamicPara.TestSta == EQ_MOD_TEST_STA_MAIN)                 //lq 发射测试
    		{
				LocalClockCtrlMainProcess();
			    RtcClockChoice();
#if CONFIG_KEY
				KeyScanProcess();
#endif
#if CONFIG_BAT_MONITOR 
				BS1_BatteryScanProcess();
#endif
				LEDCtrlProcess();
				GetCOMMMess(COMM1);
                TRCTRL_InstTxTestCtrl();               
			}
            else if (DevDynamicPara.TestSta == EQ_MOD_TEST_STA_RX)              //lq 接收测试
            {                                                                
				LocalClockCtrlMainProcess();
			    RtcClockChoice();
				LEDCtrlProcess();
				GetCOMMMess(COMM1);
                DLS_InstPacketProbeProcess();
                LME_InstBinMsgRxFifoPortIn();
            }
            else if (DevDynamicPara.TestSta == EQ_MOD_TEST_STA_SELF_CHECK)      //lq 自检测试
            {
				pTransceiverTxCtrlStruct = &TransceiverTxCtrlInstance;
				LocalClockCtrlMainProcess();
			    RtcClockChoice();
				LEDCtrlProcess();
#if CONFIG_BAT_MONITOR 
                BS1_BatteryScanProcess();
#endif
				GetCOMMMess(COMM1);
				GetCOMMMess(COMM2);
                TRCTRL_TransmitProcess();
                DLS_InstPacketProbeProcess();
                LME_BinMsgPort();
                SLFCHK_InstSelfCheckProcess();
				if(pTransceiverTxCtrlStruct->TxDataCtrlState == TRCTRL_TX_DATA_CTRL_STATE_DATA)
//					&& (pTransceiverTxCtrlStruct->TxBufferIndex == ((pTransceiverTxCtrlStruct->TxBuffer[0] + 2)/2-5))) 
//					&& ((pTransceiverTxCtrlStruct->TxBufferIndex == ((pTransceiverTxCtrlStruct->TxBuffer[0] + 2)/2 - 5))
//							|| (pTransceiverTxCtrlStruct->TxBufferIndex == (pTransceiverTxCtrlStruct->TxBuffer[0] + 2)/2)
//							|| (pTransceiverTxCtrlStruct->TxBufferIndex == ((pTransceiverTxCtrlStruct->TxBuffer[0] + 2)/2 + 5))))
				{ 
					ucCount = 4;
					while(ucCount--)
					{
						Delay_ms(2); 
						sprintf(chRetBuf, "$P712,TR,RSSI,2");  
	                    sprintf(ucTempBuf, "%d", SI446X_GetRssi(SI446X_CHIP_B));
	    				strcat(chRetBuf, ",");               
	                    strcat(chRetBuf, (char *)ucTempBuf);                                                                                                                                                
	    				UartResponseMsg(chRetBuf); 
					}					
				}
            }
		} 	        
    }	
}

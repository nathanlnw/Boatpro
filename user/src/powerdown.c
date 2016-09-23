/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: powerdown.c
  Author: vozhj      Version : V1.00        Date: 2012.03.21
  Description:    关机模块
  Version:         V1.00
  Function List:   // 主要函数及其功能
  History:         // 历史修改记录
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#include "config.h"

#if CONFIG_POWERDOWN
#include "def.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "initial.h"
#include "debug.h"
#include "keyscan.h"
#include "systemtick.h"
#include "powerdown.h"
#include "ledctrl.h"
#include "common.h"
#include "ioctrl.h"
#include "gpsctrl.h"
#include "parastorage.h"
#include "wdg.h"
#include "testing.h"
#include "tim.h"

PowerDownStruct PowerDownPara;
	
/*******************************************************************************
* 名称  : IniPowerDownPara
* 描述  : 初始化关机模块参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void IniPowerDownPara(void)
{
//lq	PowerDownPara.TimeCnt = 0;	
	PowerDownPara.TimePeriod = POWERDOWN_POWERUP_PEROID;	//50ms检测一下按键状态

    PowerDownPara.TimePeriod1min = POWERDOWN_TIME_1min_PERIOD;
    PowerDownPara.TimeCnt1min = SetTimePeriodProcess(PowerDownPara.TimePeriod1min);
    PowerDownPara.TimeCntMin = 0;

	PowerDownPara.XDouCnt = 0;
	PowerDownPara.Sta = POWERDOWN_STA_IDLE;

	PowerDownPara.KeyVal = KEY_LOW;
	PowerDownPara.KeyValPast = KEY_LOW;//按键过去值，只有当按键从高变为低才认为开始powerdown进程，防止开机长按关机

	PowerDownPara.TimeCnt = SetTimePeriodProcess(PowerDownPara.TimePeriod);

	PowerDownPara.ShutDownTime = eqEquipPara.CloseTime * 200;
}

/*******************************************************************************
* 名称  : PowerDownProcess
* 描述  : 关机模块，完成按键扫描，LED控制，整机状态控制，关机操作
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void PowerDownProcess(void)
{
	U8 ucTemp[10];
	U8 ucTemp1[10];
//lq-	U8 ucTemp_key1;
	U16 PowerDownCount;
	//U16 ShutDownTime;
	
	PowerDownCount = 0;
#if 0		
	/*lq 每5min记录一次GPS日期和时间，作为非正常关机时间*/
    if (GetLocalClockTickProcess() == PowerDownPara.TimeCnt1min)
    {
        PowerDownPara.TimeCnt1min = SetTimePeriodProcess(PowerDownPara.TimePeriod1min);
        
        PowerDownPara.TimeCntMin++;
        if (PowerDownPara.TimeCntMin >= POWERDOWN_TIME_Min_PERIOD)
        {
            PowerDownPara.TimeCntMin = 0;
        	if (GetGpsPowerupFixState() == TRUE)                                //lq 只有开机定位成功后，才会记录关机时间
        	{
        		/*lq 记录GPS日期和时间*/
                sprintf((char *)ucTemp, "%2d", GpsPara.GPS_TimeMon);
        		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeD);
        		strcat((char *)ucTemp, (char *)ucTemp1);
                sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeH);
        		strcat((char *)ucTemp, (char *)ucTemp1);
        		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeM);
        		strcat((char *)ucTemp, (char *)ucTemp1);
            	Para_Save(PARA_TIME_CLOSE_TYPE, ucTemp, PARA_TIME_CLOSE_MAXLEN);                           //lq [2013/04/16]


                /*lq 记录本地帧计数的工作时长*/
    			sprintf((char *)ucTemp,"%5d",GetFrameNum());
    			Para_Save(PARA_BATTERY_TEST_TIME_TYPE,ucTemp,5);
        	}              
        }    
    }
#endif    
    //开机初始状态，需要检测到电源按键抬起方能进入到真正的关键状态
	if (PowerDownPara.Sta == POWERDOWN_STA_IDLE)
	{
		if (GetLocalClockTickProcess() == PowerDownPara.TimeCnt)
		{
			PowerDownPara.TimeCnt = SetTimePeriodProcess(PowerDownPara.TimePeriod);
			PowerDownPara.KeyValPast = PowerDownPara.KeyVal;
			PowerDownPara.KeyVal = GetIndivIO(KEY_POWER_ADDR);
			
			//按键抬起，进入下一个工作状态
			//if (PowerDownPara.KeyVal == KEY_HIGH)
			if (PowerDownPara.KeyVal == KEY_LOW && PowerDownPara.KeyValPast == KEY_HIGH)
			{
				PowerDownPara.TimePeriod = POWERDOWN_NORMAL_PEROID;	//检测关机按键以5ms速度进行检测
				PowerDownPara.TimeCnt = SetTimePeriodProcess(PowerDownPara.TimePeriod);
				PowerDownPara.Sta = POWERDOWN_STA_STEP1; 	//进入关机按键检测状态
			}	
		}	
	}
	//正常工作状态，检测3s关机按键按下	
	else if (PowerDownPara.Sta == POWERDOWN_STA_STEP1)
	{
		if (GetLocalClockTickProcess() == PowerDownPara.TimeCnt)
		{
			PowerDownPara.TimeCnt = SetTimePeriodProcess(PowerDownPara.TimePeriod);
			PowerDownPara.KeyVal = GetIndivIO(KEY_POWER_ADDR);

			//按键按下，检测按键按下时间
			if (PowerDownPara.KeyVal == KEY_LOW)
			{
				PowerDownPara.XDouCnt++;

				//按键连续按下超过3s
				if (PowerDownPara.XDouCnt >= PowerDownPara.ShutDownTime)
				{
					UartResponseMsg("$P712,001,,,PowerDown Key Down!");
					PowerDownPara.Sta = POWERDOWN_STA_STEP2; 	//进入关机按键检测状态	
				}  				
			}
			else
			{
				PowerDownPara.XDouCnt = 0;
				PowerDownPara.Sta = POWERDOWN_STA_IDLE;//短按则回复
			}
		}	
	}
	//关机状态，执行关机操作
	else if (PowerDownPara.Sta == POWERDOWN_STA_STEP2)							//lq 2013-01-27
	{
        /*lq 记录本次开机后工作时长*/
        sprintf(ucTemp, "%5d", (U16)gslFrameCnt);
        Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)ucTemp, 5);  
    
    	if (GetGpsPowerupFixState() == TRUE)                                    //lq 只有开机定位成功后，才会记录关机时间
    	{
    		//关机时间
            sprintf((char *)ucTemp, "%2d", GpsPara.GPS_TimeMon);
    		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeD);
    		strcat((char *)ucTemp, (char *)ucTemp1);
            sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeH);
    		strcat((char *)ucTemp, (char *)ucTemp1);
    		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeM);
    		strcat((char *)ucTemp, (char *)ucTemp1);
        	Para_Save(PARA_TIME_CLOSE_TYPE, ucTemp, PARA_TIME_CLOSE_MAXLEN);                           //lq [2013/04/16]
    		UartResponseMsg("POWER DOWN");
    	}
    
    	/*lq 关机原因*/
    	sprintf((char *)ucTemp, "%d", 1);
    	Para_Save(PARA_CLOSE_RESION_TYPE, ucTemp, 1);//锟斤拷锟斤拷锟截伙拷
    
    	LEDAllLight();
    	
        IO_Ctrl(IO_POWER_TYPE, IO_VAL_OFF);	
    
    	while (PowerDownCount < 20)//LED亮2秒后关闭
    	{
    		Is_Feed_Dog = TRUE;
    		Feed_Dog();
    		Delay_ms(100);
    		PowerDownCount++;
    	}	
    	
    //lq-		IO_Ctrl(IO_POWER_TYPE, IO_VAL_LOW);	
    	while (1)
    	{
    /*lq-
    		ucTemp_key1 = ReadSingleKeyValProcess(&PowerKey);
    		//ucTemp_key2 = ReadSingleKeyValProcess(&SOSKey);
    
    		if (ucTemp_key1 == KEY_LOW)//若power按键一直按下
    		{
    			Is_Feed_Dog = TRUE;
    			Feed_Dog();//喂狗
    			Delay_ms(250);//一直喂狗，来保证关机后长按POWER不会导致关机后再次复位开机
    		}
    */
    //lq+		
    		Is_Feed_Dog = TRUE;
    		Feed_Dog();//喂狗
    		Delay_ms(250);//一直喂狗，来保证关机后长按POWER不会导致关机后再次复位开机
    //lq~       
    	} 
	}
}

/*******************************************************************************
* 名称  : GetPowerDownSleepSta
* 描述  : 获取关机模块状态
* 输入  : 无		  
* 输出  : 无
* 返回  : TRUE表示可以进入省电，FALSE禁止进入省电
*******************************************************************************/
U8 GetPowerDownSleepSta(void)
{
	if (PowerDownPara.Sta == POWERDOWN_STA_IDLE)
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}		
}

/*******************************************************************************
* 名称  : PowerDown_MsgTest
* 描述  : 消息测试模式下，自动关机模块
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void PowerDown_MsgTest(void)
{
	U8 ucTemp[10];
	U16 PowerDownCount;
	
	PowerDownCount = 0;
   
    /*lq 最后一条消息发射完成*/  
    if (struMsgTestPara.ucPowerDownEn == TRUE && GetBurstFinsFlg() == TRUE)
    {
		UartResponseMsg("$Msg Testing Finish!");

        /*lq 记录本次开机后工作时长*/
        sprintf(ucTemp, "%5d", (U16)gslFrameCnt);
        Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)ucTemp, 5);

		LEDAllLight();
		
        IO_Ctrl(IO_POWER_TYPE, IO_VAL_OFF);	
        
        /*lq LED亮2秒后关闭*/
		while (PowerDownCount < 20)
		{
			Is_Feed_Dog = TRUE;
			Feed_Dog();
			Delay_ms(100);
			PowerDownCount++;
		}	
		
		/*lq 一直喂狗，来保证关机后长按POWER不会导致关机后再次复位开机*/
        while (1)
		{
			Is_Feed_Dog = TRUE;
			Feed_Dog();//喂狗
			Delay_ms(250);
		}    
    }
}

/*******************************************************************************
* 名称  : ExecShutdownAction
* 描述  : 关机处理
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ExecShutdownAction(void)
{
	U8 ucTemp[10];
	U8 ucTemp1[10];
	U16 PowerDownCount = 0;

	/*lq 关机原因*/
	sprintf((char *)ucTemp, "%d", 1);
	Para_Save(PARA_CLOSE_RESION_TYPE, ucTemp, 1);//锟斤拷锟斤拷锟截伙拷

	LEDAllLight();
	
    IO_Ctrl(IO_POWER_TYPE, IO_VAL_OFF);	

	while (PowerDownCount < 20)//LED亮2秒后关闭
	{
		Is_Feed_Dog = TRUE;
		Feed_Dog();
		Delay_ms(100);
		PowerDownCount++;
	}	
	
//lq-		IO_Ctrl(IO_POWER_TYPE, IO_VAL_LOW);	
	while (1)
	{
/*lq-
		ucTemp_key1 = ReadSingleKeyValProcess(&PowerKey);
		//ucTemp_key2 = ReadSingleKeyValProcess(&SOSKey);

		if (ucTemp_key1 == KEY_LOW)//若power按键一直按下
		{
			Is_Feed_Dog = TRUE;
			Feed_Dog();//喂狗
			Delay_ms(250);//一直喂狗，来保证关机后长按POWER不会导致关机后再次复位开机
		}
*/
//lq+		
		Is_Feed_Dog = TRUE;
		Feed_Dog();//喂狗
		Delay_ms(250);//一直喂狗，来保证关机后长按POWER不会导致关机后再次复位开机
//lq~       
	} 
}
#endif

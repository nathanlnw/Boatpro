/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: powerup.c
  Author: vozhj      Version : V1.00        Date: 2012.03.21
  Description:    开机模块
  Version:         V1.00
  Function List:   // 主要函数及其功能
  History:         // 历史修改记录
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#include "stm32f10x.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "def.h"
#include "initial.h"
#include "debug.h"
#include "keyscan.h"
#include "powerup.h"
#include "ledctrl.h"
#include "ioctrl.h"
#include "common.h"
#include "debug.h"
#include "parastorage.h"
#include "wdg.h"
#include "BKPRegister.h"
#include "gpsctrl.h"
#include "sotdma.h"
#include "rtc.h"
#include "tim.h"
#include "adjust.h"
#include "mmi.h"
#include "PTL_Main.h"
#include "gmskmod.h" 
#include "led.h"
#include "Include.h"
#include "flashctrl.h"

/* Private variables ---------------------------------------------------------*/
U8 ucStandbyWakeupFlg = FALSE;
U8 ucStandbyWakeupDelayFlg = FALSE;

/* Private function prototypes -----------------------------------------------*/  
void PowerUpLedCtrl(void);

/*******************************************************************************
* 名称  : InitPowerUppara
* 描述  : 初始化开机模块必要的参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitPowerUppara(void)
{	             

}

#if 0
/*******************************************************************************
* 名称  : PowerUpProcess
* 描述  : 开机模块，采用阻塞方式查询按键状态，并控制电源开关，进入相应的工作状态
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void PowerUpProcess(void)
{
	U8 ucPowerKey;
	U8 ucSOSKey;
	U8 i;
	char chTemp[100];
    U8 ucTempBuf[40];
	U16 PowerEnableCount;
	U16 SosEnableCount;
	U16 usTemp;

	U16 PowerKeyEnableValue;//power开机按键计数阈值
	U16 SosKeyEnableValue;//sos开机按键计数阈值

	PowerEnableCount = 0;
	SosEnableCount = 0;

	PowerKeyEnableValue = eqEquipPara.PowerOpenTime * 100;//每10ms检测一次
	SosKeyEnableValue = eqEquipPara.SosOpenTime * 100;

	/* Check if the system has resumed from IWDG reset */
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)//如果是狗复位，则直接开机
	{
		/* Clear reset flags */
    	RCC_ClearFlag();
	
		IO_Ctrl(IO_POWER_TYPE, IO_VAL_HIGH);
		eqEquipPara.ucWorkMode = EQ_WORKMOD_NORMAL;

		for (i = 0; i < 3; i++)
		{
#if LED_EN_HIGH
			IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_HIGH);
			IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_HIGH);
#else
			IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_LOW);
			IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_LOW);
#endif
			Delay_ms(200);
			Is_Feed_Dog = TRUE;
			Feed_Dog();//喂狗						
#if LED_EN_HIGH
			IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_LOW);
			IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_LOW);
#else
			IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_HIGH);
			IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_HIGH);
#endif
			Delay_ms(200);
			Is_Feed_Dog = TRUE;
			Feed_Dog();//喂狗			
		}

		UartResponseMsg("$P712,001,,,Powerup By IWDG!");
		//此处加入LED开机控制
		LEDInit(LED_TYPE_GPS_SEARCH);

        /*lq 本次正常开机至现在看门狗复位次数加1*/
        Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);
        usTemp = atol((char *)ucTempBuf);
        usTemp = ++usTemp > PARA_WDG_RESET_CNT_CEIL ? PARA_WDG_RESET_CNT_CEIL : usTemp;
        sprintf((char*)ucTempBuf, "%2d", usTemp);
		Para_Save(PARA_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//存
	}
	else
	{
	
		while (1)
		{
			ucPowerKey = ReadSingleKeyValProcess(&PowerKey);
			ucSOSKey = ReadSingleKeyValProcess(&SOSKey);
	
			if (ucPowerKey == KEY_LOW)
			{
				if (PowerEnableCount < PowerKeyEnableValue)//按键时间未到且不是复位后重启
				{
					PowerEnableCount++;
					SosEnableCount = 0;
					
					Is_Feed_Dog = TRUE;
					Feed_Dog();//喂狗
	
					Delay_ms(10);//10ms检测一次
				}
				else//达到阈值或者是复位重启
				{
					//控制开机电源，同时置当前为正常工作状态
					IO_Ctrl(IO_POWER_TYPE, IO_VAL_HIGH);
					eqEquipPara.ucWorkMode = EQ_WORKMOD_NORMAL;
		
					for (i = 0; i < 3; i++)
					{
#if LED_EN_HIGH
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_HIGH);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_HIGH);
#else
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_LOW);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_LOW);
#endif
						Delay_ms(200);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//喂狗						
#if LED_EN_HIGH	
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_LOW);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_LOW);
#else
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_HIGH);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_HIGH);
#endif
						Delay_ms(200);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//喂狗							
					}
		
					UartResponseMsg("$P712,001,,,Powerup By PowerKey!");
					//此处加入LED开机控制
					LEDInit(LED_TYPE_GPS_SEARCH);
                	
                    /*lq 保存上次正常开机至本次正常开机看门狗复位次数*/
                    Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);
					Para_Save(PARA_LAST_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//存
                    
                    /*lq 本次开机至现在看门狗复位次数清0*/
                    sprintf((char*)ucTempBuf, "%2d", 0);
					Para_Save(PARA_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//存

                    /*lq 正常开机次数加1*/
                    Para_Read(PARA_NORMAL_POWER_ON_CNT_TYPE, ucTempBuf);
                    usTemp = atol((char *)ucTempBuf);
                    usTemp = ++usTemp > PARA_NORMAL_POWER_ON_CNT_CEIL ? PARA_NORMAL_POWER_ON_CNT_CEIL : usTemp;
                    sprintf((char*)ucTempBuf, "%2d", usTemp);
            		Para_Save(PARA_NORMAL_POWER_ON_CNT_TYPE, (U8 *)ucTempBuf, 2);//存
					break;
				}
			}
			else if (ucSOSKey == KEY_LOW)
			{
				if (SosEnableCount < SosKeyEnableValue)
				{
					SosEnableCount++;
					PowerEnableCount = 0;
	
					Is_Feed_Dog = TRUE;
					Feed_Dog();//喂狗
	
					Delay_ms(10);//10ms检测一次
				}
				else
				{
					//控制开机电源，同时置当前为正常工作状态
					IO_Ctrl(IO_POWER_TYPE, IO_VAL_HIGH);
					eqEquipPara.ucWorkMode = EQ_WORKMOD_TEST;
                    eqEquipPara.TestSta = EQ_MOD_TEST_STA_MSG;

					for (i = 0; i < 3; i++)
					{
#if LED_EN_HIGH
						IO_Ctrl(IO_RED2_TYPE, IO_VAL_HIGH);
#else
						IO_Ctrl(IO_RED2_TYPE, IO_VAL_LOW);
#endif
						Delay_ms(250);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//喂狗						
	
#if LED_EN_HIGH						
                        IO_Ctrl(IO_RED2_TYPE, IO_VAL_LOW);
#else
						IO_Ctrl(IO_RED2_TYPE, IO_VAL_HIGH);
#endif
						Delay_ms(750);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//喂狗							
					}
				
					UartResponseMsg("$P712,001,,,Powerup By TestKey!");
					//此处加入LED开机控制
					
					//LEDInit(LED_TYPE_TEST_STARTUP);
                    
                    /*lq 保存上次正常开机至本次正常开机看门狗复位次数*/
                    Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);
					Para_Save(PARA_LAST_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//存
                    
                    /*lq 本次开机至现在看门狗复位次数清0*/
                    sprintf((char*)ucTempBuf, "%2d", 0);
					Para_Save(PARA_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//存	
                    
                    /*lq 用户测试次数加1*/
                    Para_Read(PARA_USER_TEST_CNT_TYPE, ucTempBuf);
                    usTemp = atol((char *)ucTempBuf);
                    usTemp = ++usTemp > PARA_USER_TEST_CNT_CEIL ? PARA_USER_TEST_CNT_CEIL : usTemp;
                    sprintf((char*)ucTempBuf, "%2d", usTemp);
            		Para_Save(PARA_USER_TEST_CNT_TYPE, (U8 *)ucTempBuf, 2);//存                    				
					break;
				}
			}
		}
	}
	
	UartResponseMsg("$P712,001,,,--------------------------------------");
	UartResponseMsg("$P712,001,,,-- AIS Transponder");
	sprintf(chTemp, "$P712,001,,,-- SW Ver %s", VERSION);
	UartResponseMsg(chTemp);
	sprintf(chTemp, "$P712,001,,,-- HW Ver %s", VERSION_HW);
	UartResponseMsg(chTemp);
	sprintf(chTemp, "$P712,001,,,-- Last compiled: %s %s", __DATE__, __TIME__);
	UartResponseMsg(chTemp);
	UartResponseMsg("$P712,001,,,--------------------------------------");
}
#endif

#if 0
/*******************************************************************************
* 名称  : PowerUpProcess
* 描述  : 开机模块，新版网位仪方式开机，复位开机包括：上电复位、工作中看门狗复位、
          待机中看门狗复位、待机唤醒复位by Wakeup pin 、待机唤醒复位by RTC Alarm、
          Reset Pin复位（调试用）
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void PowerUpProcess(void)
{
	U8 ucPowerKey;
	U8 ucSOSKey;
	U8 i;
	char chTemp[100];
    U8 ucTempBuf[40];
	U16 PowerEnableCount;
	U16 SosEnableCount;
	U16 usTemp;
    U16 usTemp2;
	U16 PowerKeyEnableValue;//power开机按键计数阈值
	U16 SosKeyEnableValue;//sos开机按键计数阈值

	PowerEnableCount = 0;
	SosEnableCount = 0;
#if 0
    /*lq 电源时钟打开*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);                             //lq 读取备份数据寄存器时需要
    
    if ((RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
        && (PWR_GetFlagStatus(PWR_FLAG_SB) != RESET))
    {
        /* Clear reset flags */
    	RCC_ClearFlag();
                    
        PWR_WakeUpPinCmd(ENABLE);                                                   //lq 使能wakeup pin
        PWR_EnterSTANDBYMode();                                             //lq 复位后看门狗自动关闭
                
    }
#endif	
    /*lq 设备系统时钟初始化*/
    Target_SysClock_Init();

    /*lq 电源时钟打开*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);                             //lq 读取备份数据寄存器时需要
    
    /*lq 上电复位*/
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)                                //lq 上电复位    
    {
        /*lq 清除标志位*/
        RCC_ClearFlag();

    	/* Reset Backup Domain */
    	BKP_DeInit();  
              
        /*lq 备份数据寄存器初始化*/
        InitBKP_DR();

        /*lq 备份数据寄存器数据初始化*/
        InitBKP_DRData();
        
        /*lq 外设初始化*/
    	InitPowerCtrl();
        IO_Ctrl(IO_POWER_TYPE, IO_VAL_LOW);                                     //lq 电源开关关闭，按键成功后再开启
        Target_Periph_Init();
        
        /*lq 参数初始化*/
        Parameter_Init();//初始化相关参数                                           //lq 初始化各个结构体的成员变量，全局变量

    	PowerKeyEnableValue = eqEquipPara.PowerOpenTime * 100;//每10ms检测一次
    	SosKeyEnableValue = eqEquipPara.SosOpenTime * 100;

        UartResponseMsg("$Reset By PowerOn!");		

        /*lq 上电开机按键检测*/
		while (1)
		{
			ucPowerKey = ReadSingleKeyValProcess(&PowerKey);
			ucSOSKey = ReadSingleKeyValProcess(&SOSKey);
	
			if (ucPowerKey == KEY_LOW)
			{
				if (PowerEnableCount < PowerKeyEnableValue)//按键时间未到且不是复位后重启
				{
					PowerEnableCount++;
					SosEnableCount = 0;
					
					Is_Feed_Dog = TRUE;
					Feed_Dog();//喂狗
	
					Delay_ms(10);//10ms检测一次
				}
				else//达到阈值或者是复位重启
				{
					/*lq 控制开机电源，同时置当前为正常工作状态*/
					IO_Ctrl(IO_POWER_TYPE, IO_VAL_HIGH);
					eqEquipPara.ucWorkMode = EQ_WORKMOD_NORMAL;
		
                    /*lq 正常开机LED指示*/
					for (i = 0; i < 3; i++)
					{
#if LED_EN_HIGH
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_HIGH);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_HIGH);
#else
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_LOW);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_LOW);
#endif
						Delay_ms(200);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//喂狗						
#if LED_EN_HIGH	
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_LOW);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_LOW);
#else
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_HIGH);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_HIGH);
#endif			
            			Delay_ms(200);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//喂狗							
					}
		
					UartResponseMsg("$P712,001,,,Powerup By PowerKey!");
                	
                    /*lq 记录累计总工作时长*/
                    Para_Read(PARA_WORKTIME_TYPE, ucTempBuf);               //lq 上次开机之前工作总时长
                    usTemp = atol((char *)ucTempBuf);
                    Para_Read(PARA_WORKTIME_LAST_TYPE, ucTempBuf);          //lq 上次开机之后工作时长
                    usTemp2 = atol((char *)ucTempBuf);
                    usTemp += usTemp2;                                      //lq 计算本次开机之前工作总时长
                    if (usTemp < PARA_WORKTIME_FLOOR)
                    {
                        usTemp = PARA_WORKTIME_FLOOR;
                    }
                
                    if (usTemp > PARA_WORKTIME_CEIL)
                    {
                        usTemp = PARA_WORKTIME_CEIL;
                    } 
                    sprintf((char*)ucTempBuf, "%5d", usTemp);
                	Para_Save(PARA_WORKTIME_TYPE, (U8 *)ucTempBuf, 5);      //lq 存本次开机之前工作总时长
                    eqEquipPara.WorkTimeSum = usTemp;
                    sprintf((char*)ucTempBuf, "%5d", 0);
					Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)ucTempBuf, 5); //lq 上次开机之后工作时长清0，防止下次开机后重复累加 

                    /*lq 保存上次正常开机至本次正常开机看门狗复位次数*/
                    Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);
					Para_Save(PARA_LAST_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//存
                    
                    /*lq 本次开机至现在看门狗复位次数清0*/
                    sprintf((char*)ucTempBuf, "%2d", 0);
					Para_Save(PARA_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//存
#if 0
                    /*lq 正常开机次数加1*/
                    Para_Read(PARA_NORMAL_POWER_ON_CNT_TYPE, ucTempBuf);
                    usTemp = atol((char *)ucTempBuf);
                    usTemp = ++usTemp > PARA_NORMAL_POWER_ON_CNT_CEIL ? PARA_NORMAL_POWER_ON_CNT_CEIL : usTemp;
                    sprintf((char*)ucTempBuf, "%2d", usTemp);
            		Para_Save(PARA_NORMAL_POWER_ON_CNT_TYPE, (U8 *)ucTempBuf, 2);//存
#endif	
    				break;
				}
			}
			else if (ucSOSKey == KEY_LOW)
			{
				if (SosEnableCount < SosKeyEnableValue)
				{
					SosEnableCount++;
					PowerEnableCount = 0;
	
					Is_Feed_Dog = TRUE;
					Feed_Dog();//喂狗
	
					Delay_ms(10);//10ms检测一次
				}
				else
				{
					/*lq 控制开机电源，同时置当前为正常工作状态*/
					IO_Ctrl(IO_POWER_TYPE, IO_VAL_HIGH);
					eqEquipPara.ucWorkMode = EQ_WORKMOD_TEST;
                    eqEquipPara.TestSta = EQ_MOD_TEST_STA_MAIN;

                    /*lq 用户测试开机LED指示*/
					for (i = 0; i < 3; i++)
					{
#if LED_EN_HIGH
						IO_Ctrl(IO_RED2_TYPE, IO_VAL_HIGH);
#else
						IO_Ctrl(IO_RED2_TYPE, IO_VAL_LOW);
#endif			
            			Delay_ms(250);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//喂狗						
#if LED_EN_HIGH
						IO_Ctrl(IO_RED2_TYPE, IO_VAL_LOW);
#else
						IO_Ctrl(IO_RED2_TYPE, IO_VAL_HIGH);
#endif			
            			Delay_ms(750);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//喂狗							
					}
				
					UartResponseMsg("$P712,001,,,Powerup By TestKey!");					

                    /*lq 记录累计总工作时长*/
                    Para_Read(PARA_WORKTIME_TYPE, ucTempBuf);               //lq 上次开机之前工作总时长
                    usTemp = atol((char *)ucTempBuf);
                    Para_Read(PARA_WORKTIME_LAST_TYPE, ucTempBuf);          //lq 上次开机之后工作时长
                    usTemp2 = atol((char *)ucTempBuf);
                    usTemp += usTemp2;                                      //lq 计算本次开机之前工作总时长
                    if (usTemp < PARA_WORKTIME_FLOOR)
                    {
                        usTemp = PARA_WORKTIME_FLOOR;
                    }
                
                    if (usTemp > PARA_WORKTIME_CEIL)
                    {
                        usTemp = PARA_WORKTIME_CEIL;
                    } 
                    sprintf((char*)ucTempBuf, "%5d", usTemp);
                	Para_Save(PARA_WORKTIME_TYPE, (U8 *)ucTempBuf, 5);      //lq 存本次开机之前工作总时长
                    eqEquipPara.WorkTimeSum = usTemp;                    
                    sprintf((char*)ucTempBuf, "%5d", 0);
					Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)ucTempBuf, 5); //lq 上次开机之后工作时长清0，防止下次开机后重复累加 
                                        
                    /*lq 保存上次正常开机至本次正常开机看门狗复位次数*/
                    Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);
					Para_Save(PARA_LAST_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//存
                    
                    /*lq 本次开机至现在看门狗复位次数清0*/
                    sprintf((char*)ucTempBuf, "%2d", 0);
					Para_Save(PARA_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//存	
#if 0                    
                    /*lq 用户测试次数加1*/
                    Para_Read(PARA_USER_TEST_CNT_TYPE, ucTempBuf);
                    usTemp = atol((char *)ucTempBuf);
                    usTemp = ++usTemp > PARA_USER_TEST_CNT_CEIL ? PARA_USER_TEST_CNT_CEIL : usTemp;
                    sprintf((char*)ucTempBuf, "%2d", usTemp);
            		Para_Save(PARA_USER_TEST_CNT_TYPE, (U8 *)ucTempBuf, 2);//存                    				
#endif	
    				break;
				}
			}
		}
    }
    
    /*lq 软件复位，关闭看门狗*/
    else if (RCC_GetFlagStatus(RCC_FLAG_SFTRST) != RESET)
    {
		/* Clear reset flags */
    	RCC_ClearFlag();   
#if 1
        /*lq 若RTC时钟源为LSI，则需重新开启LSI，因LSI在待机唤醒时会复位*/
        /* Allow access to BKP Domain */
        PWR_BackupAccessCmd(ENABLE);

        /*lq 若是LSI选作RTC时钟源*/
        if ((RCC->BDCR | 0xFFFFFCFF) == 0xFFFFFEFF)
        {
    	 	/* Enable the LSI OSC */
    		RCC_LSICmd(ENABLE);                                             //lq 开启内部RC振荡器,在待机唤醒时，LSI会被复位
    		
            /* Wait till LSI is ready */
    		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)//如果这个时候复位了就说明LSI振荡电路在低温也不行
    		{}             
        }
#endif
        PWR_WakeUpPinCmd(ENABLE);                                                   //lq 使能wakeup pin
        PWR_EnterSTANDBYMode();                                                 //lq 复位后看门狗自动关闭         
    }

    /*lq 看门狗复位*/
    else if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)                      //lq 看门狗复位
    {
		/* Clear reset flags */
    	RCC_ClearFlag();
	
        if (PWR_GetFlagStatus(PWR_FLAG_SB) != RESET)                            //lq 待机中看门狗复位
        {
#if 1
            /*lq 若RTC时钟源为LSI，则需重新开启LSI，因LSI在待机唤醒时会复位*/
            /* Allow access to BKP Domain */
	        PWR_BackupAccessCmd(ENABLE);

            /*lq 若是LSI选作RTC时钟源*/
            if ((RCC->BDCR | 0xFFFFFCFF) == 0xFFFFFEFF)
            {
        	 	/* Enable the LSI OSC */
        		RCC_LSICmd(ENABLE);                                             //lq 开启内部RC振荡器,在待机唤醒时，LSI会被复位
        		
                /* Wait till LSI is ready */
        		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)//如果这个时候复位了就说明LSI振荡电路在低温也不行
        		{}             
            }
#endif
            PWR_WakeUpPinCmd(ENABLE);                                                   //lq 使能wakeup pin
            PWR_EnterSTANDBYMode();                                             //lq 复位后看门狗自动关闭
        }
        else                                                                    //lq 工作中看门狗复位    
        {
        	/* Reset Backup Domain */
        	BKP_DeInit();   

            /*lq 备份数据寄存器初始化*/
            InitBKP_DR();
    
            /*lq 备份数据寄存器数据初始化*/
            InitBKP_DRData();
  
            /*lq 外设初始化*/
        	InitPowerCtrl();
            IO_Ctrl(IO_POWER_TYPE, IO_VAL_HIGH);                                //lq 电源开关开启
            Target_Periph_Init();
         	
            /*lq 参数初始化*/
            Parameter_Init();                                                   //lq 初始化各个结构体的成员变量，全局变量

			/*lq 正常开机LED指示*/
            for (i = 0; i < 3; i++)
			{
#if LED_EN_HIGH
				IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_HIGH);
				IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_HIGH);
#else
				IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_LOW);
				IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_LOW);
#endif
				Delay_ms(200);
				Is_Feed_Dog = TRUE;
				Feed_Dog();//喂狗						
#if LED_EN_HIGH
				IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_LOW);
				IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_LOW);
#else
				IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_HIGH);
				IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_HIGH);
#endif			
            	Delay_ms(200);
				Is_Feed_Dog = TRUE;
				Feed_Dog();//喂狗							
			}

            UartResponseMsg("$Reset By IWDG!");		
       		UartResponseMsg("$P712,001,,,Powerup By IWDG!");

            /*lq 记录累计总工作时长*/
            Para_Read(PARA_WORKTIME_TYPE, ucTempBuf);               //lq 上次开机之前工作总时长
            usTemp = atol((char *)ucTempBuf);
            Para_Read(PARA_WORKTIME_LAST_TYPE, ucTempBuf);          //lq 上次开机之后工作时长
            usTemp2 = atol((char *)ucTempBuf);
            usTemp += usTemp2;                                      //lq 计算本次开机之前工作总时长
            if (usTemp < PARA_WORKTIME_FLOOR)
            {
                usTemp = PARA_WORKTIME_FLOOR;
            }
        
            if (usTemp > PARA_WORKTIME_CEIL)
            {
                usTemp = PARA_WORKTIME_CEIL;
            } 
            sprintf((char*)ucTempBuf, "%5d", usTemp);
        	Para_Save(PARA_WORKTIME_TYPE, (U8 *)ucTempBuf, 5);      //lq 存本次开机之前工作总时长
            sprintf((char*)ucTempBuf, "%5d", 0);
			Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)ucTempBuf, 5); //lq 上次开机之后工作时长清0，防止下次开机后重复累加 

            /*lq 本次正常开机至现在看门狗复位次数加1*/
            Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);
            usTemp = atol((char *)ucTempBuf);
            usTemp = ++usTemp > PARA_WDG_RESET_CNT_CEIL ? PARA_WDG_RESET_CNT_CEIL : usTemp;
            sprintf((char*)ucTempBuf, "%2d", usTemp);
    		Para_Save(PARA_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//存
        }                                                                       
    } 
    
    /*lq 待机唤醒复位*/       
    else if (PWR_GetFlagStatus(PWR_FLAG_WU) != RESET)                           //lq 待机唤醒
    {
        /*lq 清除 WUF标志位*/
        PWR_ClearFlag(PWR_FLAG_WU);
        
        /* Clear the StandBy pending flag */
        PWR_ClearFlag(PWR_FLAG_SB);

        /*lq 若RTC时钟源为LSI，则需重新开启LSI，因LSI在待机唤醒时会复位*/
        /* Allow access to BKP Domain */
        PWR_BackupAccessCmd(ENABLE);

        /*lq 若是LSI选作RTC时钟源*/
        if ((RCC->BDCR | 0xFFFFFCFF) == 0xFFFFFEFF)
        {
    	 	/* Enable the LSI OSC */
    		RCC_LSICmd(ENABLE);                                             //lq 开启内部RC振荡器,在待机唤醒时，LSI会被复位
    		
            /* Wait till LSI is ready */
    		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)//如果这个时候复位了就说明LSI振荡电路在低温也不行
    		{}             
        }  
              
        /*lq 若是待机唤醒则不再重新配置RTC*/
        ucStandbyWakeupFlg = TRUE;
        
        /*lq 待机唤醒初始化延时补偿*/
        ucStandbyWakeupDelayFlg = TRUE;

        /*lq 获取备份数据寄存器数据*/
        GetBKP_DRData();
        
        //lq if (RTC_GetFlagStatus(RTC_FLAG_ALR) != RESET)
        if (IsRtcAlarm())
        {
//            RTC_ClearFlag(RTC_FLAG_RSF);

            /*lq 失能wakeup pin*/
            PWR_WakeUpPinCmd(DISABLE);                                                   //lq 使能wakeup pin

            /*lq 给系统供电*/
            InitPowerCtrl();      
            IO_Ctrl(IO_POWER_TYPE, IO_VAL_HIGH);                                //lq 电源开关开启
            Delay_ms(10);//lqdb                                                 //lq 延时（至少10ms）等系统上电完成，再对1846等外设进行设置

            /*lq 设备初始化*/
            Target_Periph_Init();//初始化 arm                                                  //lq 配置ARM
        	
            /*lq 参数初始化*/
            Parameter_Init();//初始化相关参数                                           //lq 初始化各个结构体的成员变量，全局变量
   
            /*lq 休眠待机唤醒初始化 */
            InitBKP_DRPara();
            SleepWakeUpInit();

            // UartResponseMsg("$Reset By RTCAlarm!");		
            //lq 休眠唤醒
            // UartResponseMsg("$P712,001,,,-- Sleep Out");
    		//lq LEDInit(LED_TYPE_GPS_SEARCH);                                       //lq LED GPS定位搜索指示

            return;
        }
        else                                                                    //lq wakeup按键唤醒
        {
#if 1 
        	/*lq 设备初始化*/
            InitPowerCtrl();
            IO_Ctrl(IO_POWER_TYPE, IO_VAL_HIGH);                                //lq 电源开关开启
            Target_Periph_Init();//初始化 arm                                                  //lq 配置ARM
        	
            /*lq 参数初始化*/
            Parameter_Init();//初始化相关参数                                           //lq 初始化各个结构体的成员变量，全局变量
        
        	PowerKeyEnableValue = eqEquipPara.CloseTime * 100;//每10ms检测一次
        	//lq SosKeyEnableValue = eqEquipPara.CloseTime * 100;

            UartResponseMsg("$Reset By Wakeup Pin!");		
			
            /*lq 工作指示*/
#if LED_EN_HIGH
            IO_Ctrl(IO_RED1_TYPE, IO_VAL_HIGH);
#else
			IO_Ctrl(IO_RED1_TYPE, IO_VAL_LOW);
#endif
			Delay_ms(100);
#if LED_EN_HIGH			
            IO_Ctrl(IO_RED1_TYPE, IO_VAL_LOW);
#else
            IO_Ctrl(IO_RED1_TYPE, IO_VAL_HIGH);
#endif

            while (1)
    		{ 
			    ucPowerKey = ReadSingleKeyValProcess(&PowerKey);                
                //lq ucSOSKey = ReadSingleKeyValProcess(&SOSKey);  //lq 关机按键改为Test键

                if (ucPowerKey == KEY_LOW)
    			{
                    UartResponseMsg("KEY_LOW!");
                    if (PowerEnableCount < PowerKeyEnableValue)//按键时间未到且不是复位后重启
    				{
                        PowerEnableCount++;
    					//lq SosEnableCount++;
    					
    					Is_Feed_Dog = TRUE;
    					Feed_Dog();//喂狗
    	
    					Delay_ms(10);//10ms检测一次
    				}
    				else
    				{
				        UartResponseMsg("$P712,001,,,PowerDown Key Down During Standby!");
		
    					/*lq 休眠中关机*/
                        ExecShutdownAction(); //lq 保存时间有问题
    				}
    			}
                else
                {
                    UartResponseMsg("KEY_HIGH!");
#if 1
                    /* Generate a system reset */
                    NVIC_GenerateSystemReset();                                                 //lq 关掉IWDG
#else
                    PWR_WakeUpPinCmd(ENABLE);                                                   //lq 使能wakeup pin
                    PWR_EnterSTANDBYMode();
#endif
                }
    		}    

#endif
        }
    }
    
    /*lq JTAG调试引脚复位*/
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)//lqdb nRST Pin 复位      JTAG调试时复位
    {
        /*lq 清除标志位*/
        RCC_ClearFlag();

    	/* Reset Backup Domain */
    	BKP_DeInit();  
              
        /*lq 备份数据寄存器初始化*/
        InitBKP_DR();

        /*lq 备份数据寄存器数据初始化*/
        InitBKP_DRData();
        
        /*lq 外设初始化*/
    	InitPowerCtrl();
        IO_Ctrl(IO_POWER_TYPE, IO_VAL_LOW);                                     //lq 电源开关关闭，按键成功后再开启
        Target_Periph_Init();
        
        /*lq 参数初始化*/
        Parameter_Init();//初始化相关参数                                           //lq 初始化各个结构体的成员变量，全局变量

    	PowerKeyEnableValue = eqEquipPara.PowerOpenTime * 100;//每10ms检测一次
    	SosKeyEnableValue = eqEquipPara.SosOpenTime * 100;

        UartResponseMsg("$Reset By nRst Pin!");		

        /*lq 上电开机按键检测*/
		while (1)
		{
			ucPowerKey = ReadSingleKeyValProcess(&PowerKey);
			ucSOSKey = ReadSingleKeyValProcess(&SOSKey);
	
			if (ucPowerKey == KEY_LOW)
			{
				if (PowerEnableCount < PowerKeyEnableValue)//按键时间未到且不是复位后重启
				{
					PowerEnableCount++;
					SosEnableCount = 0;
					
					Is_Feed_Dog = TRUE;
					Feed_Dog();//喂狗
	
					Delay_ms(10);//10ms检测一次
				}
				else//达到阈值或者是复位重启
				{
					//控制开机电源，同时置当前为正常工作状态
					IO_Ctrl(IO_POWER_TYPE, IO_VAL_HIGH);
					eqEquipPara.ucWorkMode = EQ_WORKMOD_NORMAL;
		
					for (i = 0; i < 3; i++)
					{
#if LED_EN_HIGH
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_HIGH);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_HIGH);
#else
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_LOW);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_LOW);
#endif
						Delay_ms(200);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//喂狗						
#if LED_EN_HIGH 	
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_LOW);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_LOW);
#else
						IO_Ctrl(IO_GREEN1_TYPE, IO_VAL_HIGH);
						IO_Ctrl(IO_GREEN2_TYPE, IO_VAL_HIGH);
#endif			
            			Delay_ms(200);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//喂狗							
					}
		
					UartResponseMsg("$P712,001,,,Powerup By PowerKey!");

                    /*lq 记录累计总工作时长*/
                    Para_Read(PARA_WORKTIME_TYPE, ucTempBuf);               //lq 上次开机之前工作总时长
                    usTemp = atol((char *)ucTempBuf);
                    Para_Read(PARA_WORKTIME_LAST_TYPE, ucTempBuf);          //lq 上次开机之后工作时长
                    usTemp2 = atol((char *)ucTempBuf);
                    usTemp += usTemp2;                                      //lq 计算本次开机之前工作总时长
                    if (usTemp < PARA_WORKTIME_FLOOR)
                    {
                        usTemp = PARA_WORKTIME_FLOOR;
                    }
                
                    if (usTemp > PARA_WORKTIME_CEIL)
                    {
                        usTemp = PARA_WORKTIME_CEIL;
                    } 
                    sprintf((char*)ucTempBuf, "%5d", usTemp);
                	Para_Save(PARA_WORKTIME_TYPE, (U8 *)ucTempBuf, 5);      //lq 存本次开机之前工作总时长
                    eqEquipPara.WorkTimeSum = usTemp;               
                    sprintf((char*)ucTempBuf, "%5d", 0);
					Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)ucTempBuf, 5); //lq 上次开机之后工作时长清0，防止下次开机后重复累加 
                                    	
                    /*lq 保存上次正常开机至本次正常开机看门狗复位次数*/
                    Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);
					Para_Save(PARA_LAST_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//存
                    
                    /*lq 本次开机至现在看门狗复位次数清0*/
                    sprintf((char*)ucTempBuf, "%2d", 0);
					Para_Save(PARA_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//存
#if 0
                    /*lq 正常开机次数加1*/
                    Para_Read(PARA_NORMAL_POWER_ON_CNT_TYPE, ucTempBuf);
                    usTemp = atol((char *)ucTempBuf);
                    usTemp = ++usTemp > PARA_NORMAL_POWER_ON_CNT_CEIL ? PARA_NORMAL_POWER_ON_CNT_CEIL : usTemp;
                    sprintf((char*)ucTempBuf, "%2d", usTemp);
            		Para_Save(PARA_NORMAL_POWER_ON_CNT_TYPE, (U8 *)ucTempBuf, 2);//存
#endif	
    				break;
				}
			}
			else if (ucSOSKey == KEY_LOW)
			{
				if (SosEnableCount < SosKeyEnableValue)
				{
					SosEnableCount++;
					PowerEnableCount = 0;
	
					Is_Feed_Dog = TRUE;
					Feed_Dog();//喂狗
	
					Delay_ms(10);//10ms检测一次
				}
				else
				{
					//控制开机电源，同时置当前为正常工作状态
					IO_Ctrl(IO_POWER_TYPE, IO_VAL_HIGH);
					eqEquipPara.ucWorkMode = EQ_WORKMOD_TEST;
                    eqEquipPara.TestSta = EQ_MOD_TEST_STA_MAIN;

					for (i = 0; i < 3; i++)
					{
#if LED_EN_HIGH
						IO_Ctrl(IO_RED2_TYPE, IO_VAL_HIGH);
#else
						IO_Ctrl(IO_RED2_TYPE, IO_VAL_LOW);
#endif			
            			Delay_ms(250);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//喂狗						
#if LED_EN_HIGH
						IO_Ctrl(IO_RED2_TYPE, IO_VAL_LOW);
#else
						IO_Ctrl(IO_RED2_TYPE, IO_VAL_HIGH);
#endif			
            			Delay_ms(750);
						Is_Feed_Dog = TRUE;
						Feed_Dog();//喂狗							
					}
				
					UartResponseMsg("$P712,001,,,Powerup By TestKey!");

                    /*lq 记录累计总工作时长*/
                    Para_Read(PARA_WORKTIME_TYPE, ucTempBuf);               //lq 上次开机之前工作总时长
                    usTemp = atol((char *)ucTempBuf);
                    Para_Read(PARA_WORKTIME_LAST_TYPE, ucTempBuf);          //lq 上次开机之后工作时长
                    usTemp2 = atol((char *)ucTempBuf);
                    usTemp += usTemp2;                                      //lq 计算本次开机之前工作总时长
                    if (usTemp < PARA_WORKTIME_FLOOR)
                    {
                        usTemp = PARA_WORKTIME_FLOOR;
                    }
                
                    if (usTemp > PARA_WORKTIME_CEIL)
                    {
                        usTemp = PARA_WORKTIME_CEIL;
                    } 
                    sprintf((char*)ucTempBuf, "%5d", usTemp);
                	Para_Save(PARA_WORKTIME_TYPE, (U8 *)ucTempBuf, 5);      //lq 存本次开机之前工作总时长
                    eqEquipPara.WorkTimeSum = usTemp;                    
                    sprintf((char*)ucTempBuf, "%5d", 0);
					Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)ucTempBuf, 5); //lq 上次开机之后工作时长清0，防止下次开机后重复累加 
                    					
                    /*lq 保存上次正常开机至本次正常开机看门狗复位次数*/
                    Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);
					Para_Save(PARA_LAST_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//存
                    
                    /*lq 本次开机至现在看门狗复位次数清0*/
                    sprintf((char*)ucTempBuf, "%2d", 0);
					Para_Save(PARA_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//存	
#if 0                    
                    /*lq 用户测试次数加1*/
                    Para_Read(PARA_USER_TEST_CNT_TYPE, ucTempBuf);
                    usTemp = atol((char *)ucTempBuf);
                    usTemp = ++usTemp > PARA_USER_TEST_CNT_CEIL ? PARA_USER_TEST_CNT_CEIL : usTemp;
                    sprintf((char*)ucTempBuf, "%2d", usTemp);
            		Para_Save(PARA_USER_TEST_CNT_TYPE, (U8 *)ucTempBuf, 2);//存                    				
#endif	
    				break;
				}
			}
		}
    }
	
    //lq LEDInit(LED_TYPE_GPS_SEARCH);                               //lq LED GPS定位搜索指示

	UartResponseMsg("$P712,001,,,--------------------------------------");
	UartResponseMsg("$P712,001,,,-- AIS Tranceiver");
	sprintf(chTemp, "$P712,001,,,-- SW Ver %s", VERSION);
	UartResponseMsg(chTemp);
	sprintf(chTemp, "$P712,001,,,-- HW Ver %s", VERSION_HW);
	UartResponseMsg(chTemp);	
    sprintf(chTemp, "$P712,001,,,-- Last compiled: %s %s", __DATE__, __TIME__);
	UartResponseMsg(chTemp);
	UartResponseMsg("$P712,001,,,--------------------------------------");
}
#endif

/*******************************************************************************
* 名称  : PowerUpProcess
* 描述  : 开机模块，上电直接开机，取消按键检测。复位开机包括：上电复位、工作中看门狗复位、
          待机中看门狗复位、待机唤醒复位by Wakeup pin 、待机唤醒复位by RTC Alarm、
          Reset Pin复位（调试用）
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void PowerUpProcess(void)
{
//	U8 ucPowerKey;
//	U8 ucSOSKey;
//	U8 i;
	char chTemp[100];
    U8 ucTempBuf[40];
//	U16 PowerEnableCount;
//	U16 SosEnableCount;
	U32 usTemp1;  // lnw  modify    old U16
    U32 usTemp2; // lnw modify  old U16
//	U16 PowerKeyEnableValue;//power开机按键计数阈值
//	U16 SosKeyEnableValue;//sos开机按键计数阈值

	//PowerEnableCount = 0;
//	SosEnableCount = 0;
	
    /*lq 设备系统时钟初始化*/
    Target_SysClock_Init();

    /*lq 电源时钟打开*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);                             //lq 读取备份数据寄存器时需要
    
    /*lq 上电复位*/
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)                                //lq 上电复位    
    {
        /*lq 清除标志位*/
        RCC_ClearFlag();

    	/* Reset Backup Domain */
    	BKP_DeInit();  
              
        /*lq 备份数据寄存器初始化*/
        InitBKP_DR();

        /*lq 备份数据寄存器数据初始化*/
        InitBKP_DRData();
        
        /*lq 外设初始化*/
    	InitPowerCtrl();        
    	
        //lq IO_Ctrl(IO_POWER_TYPE, IO_VAL_OFF);                                     //lq 电源开关关闭，按键成功后再开启
        Target_Periph_Init();
        
        /*lq 参数初始化*/
        Parameter_Init();//初始化相关参数                                           //lq 初始化各个结构体的成员变量，全局变量

        UartResponseMsg("$Reset By PowerOn!");		

		/*lq 控制开机电源，同时置当前为正常工作状态*/
		//lq IO_Ctrl(IO_POWER_TYPE, IO_VAL_ON);
		DevDynamicPara.ucWorkMode = EQ_WORKMOD_NORMAL;

        /*lq 正常开机LED指示*/
		PowerUpLedCtrl();

		UartResponseMsg("$P712,001,,,Powerup By PowerOn!");
    	
        /*lq 开关机时间记录索引 */
        Para_Read(PARA_OPEN_CLOSE_TIME_INDEX_TYPE, ucTempBuf);
        usTemp1 = atol((char *)ucTempBuf);
        DevDynamicPara.OpenCloseTimeIndex = usTemp1;
        usTemp1 = (usTemp1 + 1) % PARA_TIME_OPEN_CLOSE_TOTAL;     //lq 下次开机时要获取的索引值
        sprintf((char*)ucTempBuf, "%d", usTemp1);
    	Para_Save(PARA_OPEN_CLOSE_TIME_INDEX_TYPE, (U8 *)ucTempBuf, PARA_OPEN_CLOSE_TIME_INDEX_MAXLEN);

        /*lq 本次开关机时间复位 */
        Para_OpenTimeSave(PARA_TIME_OPEN_DEFAULT, DevDynamicPara.OpenCloseTimeIndex);
        Para_CloseTimeSave(PARA_TIME_CLOSE_DEFAULT, DevDynamicPara.OpenCloseTimeIndex);

        /*lq 记录累计总工作时长*/
        Para_Read(PARA_WORKTIME_TYPE, ucTempBuf);               //lq 上次开机之前工作总时长
        usTemp1 = atol((char *)ucTempBuf);
        Para_Read(PARA_WORKTIME_LAST_TYPE, ucTempBuf);          //lq 上次开机之后工作时长
        usTemp2 = atol((char *)ucTempBuf);
        usTemp1 += usTemp2;                                      //lq 计算本次开机之前工作总时长
        if (usTemp1 < PARA_WORKTIME_FLOOR)
        {
            usTemp1 = PARA_WORKTIME_FLOOR;
        }
    
        if (usTemp1 > PARA_WORKTIME_CEIL)
        {
            usTemp1 = PARA_WORKTIME_CEIL;
        } 
        sprintf((char*)ucTempBuf, "%5d", usTemp1);
    	Para_Save(PARA_WORKTIME_TYPE, (U8 *)ucTempBuf, 5);      //lq 存本次开机之前工作总时长

        sprintf((char*)ucTempBuf, "%5d", 0);
		Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)ucTempBuf, 5); //lq 上次开机之后工作时长清0，防止下次开机后重复累加 

        /*lq 保存上次正常开机至本次正常开机看门狗复位次数*/
        Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);
		Para_Save(PARA_LAST_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//存
        
        /*lq 本次开机至现在看门狗复位次数清0*/
        sprintf((char*)ucTempBuf, "%2d", 0);
		Para_Save(PARA_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//存

        /*lq 正常开机次数加1*/
        Para_Read(PARA_NORMAL_POWER_ON_CNT_TYPE, ucTempBuf);
        usTemp1 = atol((char *)ucTempBuf);
        usTemp1 = ++usTemp1 > PARA_NORMAL_POWER_ON_CNT_CEIL ? PARA_NORMAL_POWER_ON_CNT_CEIL : usTemp1;
        sprintf((char*)ucTempBuf, "%2d", usTemp1);
		Para_Save(PARA_NORMAL_POWER_ON_CNT_TYPE, (U8 *)ucTempBuf, 2);//存

    }
    
    /*lq 软件复位，关闭看门狗*/
    else if (RCC_GetFlagStatus(RCC_FLAG_SFTRST) != RESET)
    {
		/* Clear reset flags */
    	RCC_ClearFlag();   
#if 1
        /*lq 若RTC时钟源为LSI，则需重新开启LSI，因LSI在待机唤醒时会复位*/
        /* Allow access to BKP Domain */
        PWR_BackupAccessCmd(ENABLE);

        /*lq 若是LSI选作RTC时钟源*/
        if ((RCC->BDCR | 0xFFFFFCFF) == 0xFFFFFEFF)
        {
    	 	/* Enable the LSI OSC */
    		RCC_LSICmd(ENABLE);                                             //lq 开启内部RC振荡器,在待机唤醒时，LSI会被复位
    		
            /* Wait till LSI is ready */
    		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)//如果这个时候复位了就说明LSI振荡电路在低温也不行
    		{}             
        }
#endif
        PWR_WakeUpPinCmd(ENABLE);                                               //lq 使能wakeup pin
        PWR_EnterSTANDBYMode();                                                 //lq 复位后看门狗自动关闭         
    }

    /*lq 看门狗复位*/
    else if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)                      //lq 看门狗复位
    {
		/* Clear reset flags */
    	RCC_ClearFlag();
	
        if (PWR_GetFlagStatus(PWR_FLAG_SB) != RESET)                            //lq 待机中看门狗复位
        {
#if 1
            /*lq 若RTC时钟源为LSI，则需重新开启LSI，因LSI在待机唤醒时会复位*/
            /* Allow access to BKP Domain */
	        PWR_BackupAccessCmd(ENABLE);

            /*lq 若是LSI选作RTC时钟源*/
            if ((RCC->BDCR | 0xFFFFFCFF) == 0xFFFFFEFF)
            {
        	 	/* Enable the LSI OSC */
        		RCC_LSICmd(ENABLE);                                             //lq 开启内部RC振荡器,在待机唤醒时，LSI会被复位
        		
                /* Wait till LSI is ready */
        		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)//如果这个时候复位了就说明LSI振荡电路在低温也不行
        		{}             
            }
#endif
            PWR_WakeUpPinCmd(ENABLE);                                                   //lq 使能wakeup pin
            PWR_EnterSTANDBYMode();                                             //lq 复位后看门狗自动关闭
        }
        else                                                                    //lq 工作中看门狗复位    
        {
        	/* Reset Backup Domain */
        	BKP_DeInit();   

            /*lq 备份数据寄存器初始化*/
            InitBKP_DR();
    
            /*lq 备份数据寄存器数据初始化*/
            InitBKP_DRData();
  
            /*lq 外设初始化*/
        	InitPowerCtrl();
            //lq IO_Ctrl(IO_POWER_TYPE, IO_VAL_ON);                                //lq 电源开关开启
            Target_Periph_Init();
         	
            /*lq 参数初始化*/
            Parameter_Init();                                                   //lq 初始化各个结构体的成员变量，全局变量

			/*lq 正常开机LED指示*/
    		PowerUpLedCtrl();

            UartResponseMsg("$Reset By IWDG!");		
       		UartResponseMsg("$P712,001,,,Powerup By IWDG!");

            /*lq 开关机时间记录索引 */
            Para_Read(PARA_OPEN_CLOSE_TIME_INDEX_TYPE, ucTempBuf);
            usTemp1 = atol((char *)ucTempBuf);
            DevDynamicPara.OpenCloseTimeIndex = usTemp1;
            usTemp1 = (usTemp1 + 1) % PARA_TIME_OPEN_CLOSE_TOTAL;     //lq 下次开机时要获取的索引值
            sprintf((char*)ucTempBuf, "%d", usTemp1);
        	Para_Save(PARA_OPEN_CLOSE_TIME_INDEX_TYPE, (U8 *)ucTempBuf, PARA_OPEN_CLOSE_TIME_INDEX_MAXLEN);

            /*lq 本次开关机时间复位 */
            Para_OpenTimeSave(PARA_TIME_OPEN_DEFAULT, DevDynamicPara.OpenCloseTimeIndex);
            Para_CloseTimeSave(PARA_TIME_CLOSE_DEFAULT, DevDynamicPara.OpenCloseTimeIndex);

            /*lq 记录累计总工作时长*/
            Para_Read(PARA_WORKTIME_TYPE, ucTempBuf);               //lq 上次开机之前工作总时长
            usTemp1 = atol((char *)ucTempBuf);
            Para_Read(PARA_WORKTIME_LAST_TYPE, ucTempBuf);          //lq 上次开机之后工作时长
            usTemp2 = atol((char *)ucTempBuf);
            usTemp1 += usTemp2;                                      //lq 计算本次开机之前工作总时长
            if (usTemp1 < PARA_WORKTIME_FLOOR)
            {
                usTemp1 = PARA_WORKTIME_FLOOR;
            }
        
            if (usTemp1 > PARA_WORKTIME_CEIL)
            {
                usTemp1 = PARA_WORKTIME_CEIL;
            } 
            sprintf((char*)ucTempBuf, "%5d", usTemp1);
        	Para_Save(PARA_WORKTIME_TYPE, (U8 *)ucTempBuf, 5);      //lq 存本次开机之前工作总时长
            sprintf((char*)ucTempBuf, "%5d", 0);
			Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)ucTempBuf, 5); //lq 上次开机之后工作时长清0，防止下次开机后重复累加 

            /*lq 本次正常开机至现在看门狗复位次数加1*/
            Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);
            usTemp1 = atol((char *)ucTempBuf);
            usTemp1 = ++usTemp1 > PARA_WDG_RESET_CNT_CEIL ? PARA_WDG_RESET_CNT_CEIL : usTemp1;
            sprintf((char*)ucTempBuf, "%2d", usTemp1);
    		Para_Save(PARA_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//存
        }                                                                       
    } 
    
    /*lq 待机唤醒复位*/       
    else if (PWR_GetFlagStatus(PWR_FLAG_WU) != RESET)                           //lq 待机唤醒
    {
        /*lq 清除 WUF标志位*/
        PWR_ClearFlag(PWR_FLAG_WU);
        
        /* Clear the StandBy pending flag */
        PWR_ClearFlag(PWR_FLAG_SB);

        /*lq 若RTC时钟源为LSI，则需重新开启LSI，因LSI在待机唤醒时会复位*/
        /* Allow access to BKP Domain */
        PWR_BackupAccessCmd(ENABLE);

        /*lq 若是LSI选作RTC时钟源*/
        if ((RCC->BDCR | 0xFFFFFCFF) == 0xFFFFFEFF)
        {
    	 	/* Enable the LSI OSC */
    		RCC_LSICmd(ENABLE);                                             //lq 开启内部RC振荡器,在待机唤醒时，LSI会被复位
    		
            /* Wait till LSI is ready */
    		while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)//如果这个时候复位了就说明LSI振荡电路在低温也不行
    		{}             
        }  
              
        /*lq 若是待机唤醒则不再重新配置RTC*/
        ucStandbyWakeupFlg = TRUE;
        
        /*lq 待机唤醒初始化延时补偿*/
        ucStandbyWakeupDelayFlg = TRUE;

        /*lq 获取备份数据寄存器数据*/
        GetBKP_DRData();
        
        //lq if (RTC_GetFlagStatus(RTC_FLAG_ALR) != RESET)
        if (IsRtcAlarm())
        {
//            RTC_ClearFlag(RTC_FLAG_RSF);

            /*lq 失能wakeup pin*/
            PWR_WakeUpPinCmd(DISABLE);                                                   //lq 使能wakeup pin

            /*lq 给系统供电*/
            InitPowerCtrl();      
            //lq IO_Ctrl(IO_POWER_TYPE, IO_VAL_ON);                                //lq 电源开关开启
            Delay_ms(10);//lqdb                                                 //lq 延时（至少10ms）等系统上电完成，再对1846等外设进行设置

            /*lq 设备初始化*/
            Target_Periph_Init();//初始化 arm                                                  //lq 配置ARM
        	
            /*lq 参数初始化*/
            Parameter_Init();//初始化相关参数                                           //lq 初始化各个结构体的成员变量，全局变量
   
            /*lq 休眠待机唤醒初始化 */
            InitBKP_DRPara();
#if 0
            SleepWakeUpInit();
#endif
            return;
        }
        else                                                                    //lq wakeup按键唤醒
        {
#if CONFIG_KEY
#if 1 
        	/*lq 设备初始化*/
            InitPowerCtrl();
            //lq IO_Ctrl(IO_POWER_TYPE, IO_VAL_ON);                                //lq 电源开关开启
            Target_Periph_Init();//初始化 arm                                                  //lq 配置ARM
        	
            /*lq 参数初始化*/
            Parameter_Init();//初始化相关参数                                           //lq 初始化各个结构体的成员变量，全局变量
        
        	PowerKeyEnableValue = eqEquipPara.CloseTime * 100;//每10ms检测一次
        	//lq SosKeyEnableValue = eqEquipPara.CloseTime * 100;

            UartResponseMsg("$Reset By Wakeup Pin!");		
			
            /*lq 工作指示*/
            IO_Ctrl(IO_LED_GREEN_TYPE, IO_VAL_ON);
			Delay_ms(100);
            IO_Ctrl(IO_LED_GREEN_TYPE, IO_VAL_OFF);

            while (1)
    		{ 
			    ucPowerKey = ReadSingleKeyValProcess(&PowerKey);                
                //lq ucSOSKey = ReadSingleKeyValProcess(&SOSKey);  //lq 关机按键改为Test键

                if (ucPowerKey == KEY_LOW)
    			{
                    UartResponseMsg("KEY_LOW!");
                    if (PowerEnableCount < PowerKeyEnableValue)//按键时间未到且不是复位后重启
    				{
                        PowerEnableCount++;
    					//lq SosEnableCount++;
    					
    					Is_Feed_Dog = TRUE;
    					Feed_Dog();//喂狗
    	
    					Delay_ms(10);//10ms检测一次
    				}
    				else
    				{
				        UartResponseMsg("$P712,001,,,PowerDown Key Down During Standby!");
		
    					/*lq 休眠中关机*/
                        ExecShutdownAction(); //lq 保存时间有问题
    				}
    			}
                else
                {
                    UartResponseMsg("KEY_HIGH!");
#if 1
                    /* Generate a system reset */
                    NVIC_GenerateSystemReset();                                                 //lq 关掉IWDG
#else
                    PWR_WakeUpPinCmd(ENABLE);                                                   //lq 使能wakeup pin
                    PWR_EnterSTANDBYMode();
#endif
                }
    		}    

#endif
#endif
        }
    }
    
    /*lq JTAG调试引脚复位*/
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)//lqdb nRST Pin 复位      JTAG调试时复位
    {
        /*lq 清除标志位*/
        RCC_ClearFlag();

    	/* Reset Backup Domain */
    	BKP_DeInit();  
              
        /*lq 备份数据寄存器初始化*/
        InitBKP_DR();

        /*lq 备份数据寄存器数据初始化*/
        InitBKP_DRData();
        
        /*lq 外设初始化*/
    	InitPowerCtrl();
        //lq IO_Ctrl(IO_POWER_TYPE, IO_VAL_OFF);                                     //lq 电源开关关闭，按键成功后再开启
        Target_Periph_Init();
        
        /*lq 参数初始化*/
        Parameter_Init();//初始化相关参数                                           //lq 初始化各个结构体的成员变量，全局变量

        UartResponseMsg("$Reset By nRst Pin!");		

		//控制开机电源，同时置当前为正常工作状态
		//lq IO_Ctrl(IO_POWER_TYPE, IO_VAL_ON);
		DevDynamicPara.ucWorkMode = EQ_WORKMOD_NORMAL;

		/*lq 正常开机LED指示*/
		PowerUpLedCtrl();

		UartResponseMsg("$P712,001,,,Powerup By PowerOn!");

        /*lq 开关机时间记录索引 */
        Para_Read(PARA_OPEN_CLOSE_TIME_INDEX_TYPE, ucTempBuf);
        usTemp1 = atol((char *)ucTempBuf);
        DevDynamicPara.OpenCloseTimeIndex = usTemp1;
        usTemp1 = (usTemp1 + 1) % PARA_TIME_OPEN_CLOSE_TOTAL;     //lq 下次开机时要获取的索引值
        sprintf((char*)ucTempBuf, "%d", usTemp1);
    	Para_Save(PARA_OPEN_CLOSE_TIME_INDEX_TYPE, (U8 *)ucTempBuf, PARA_OPEN_CLOSE_TIME_INDEX_MAXLEN);

        /*lq 本次开关机时间复位 */
        Para_OpenTimeSave(PARA_TIME_OPEN_DEFAULT, DevDynamicPara.OpenCloseTimeIndex);
        Para_CloseTimeSave(PARA_TIME_CLOSE_DEFAULT, DevDynamicPara.OpenCloseTimeIndex);

        /*lq 记录累计总工作时长*/
        Para_Read(PARA_WORKTIME_TYPE, ucTempBuf);               //lq 上次开机之前工作总时长
        usTemp1 = atol((char *)ucTempBuf);
        Para_Read(PARA_WORKTIME_LAST_TYPE, ucTempBuf);          //lq 上次开机之后工作时长
        usTemp2 = atol((char *)ucTempBuf);
        usTemp1 += usTemp2;                                      //lq 计算本次开机之前工作总时长
        if (usTemp1 < PARA_WORKTIME_FLOOR)
        {
            usTemp1 = PARA_WORKTIME_FLOOR;
        }
    
        if (usTemp1 > PARA_WORKTIME_CEIL)
        {
            usTemp1 = PARA_WORKTIME_CEIL;
        } 
        sprintf((char*)ucTempBuf, "%5d", usTemp1);
    	Para_Save(PARA_WORKTIME_TYPE, (U8 *)ucTempBuf, 5);      //lq 存本次开机之前工作总时长

        sprintf((char*)ucTempBuf, "%5d", 0);
		Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)ucTempBuf, 5); //lq 上次开机之后工作时长清0，防止下次开机后重复累加 
                        	
        /*lq 保存上次正常开机至本次正常开机看门狗复位次数*/
        Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);
		Para_Save(PARA_LAST_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//存
        
        /*lq 本次开机至现在看门狗复位次数清0*/
        sprintf((char*)ucTempBuf, "%2d", 0);
		Para_Save(PARA_WDG_RESET_CNT_TYPE, (U8 *)ucTempBuf, 2);//存
#if 0
        /*lq 正常开机次数加1*/
        Para_Read(PARA_NORMAL_POWER_ON_CNT_TYPE, ucTempBuf);
        usTemp1 = atol((char *)ucTempBuf);
        usTemp1 = ++usTemp > PARA_NORMAL_POWER_ON_CNT_CEIL ? PARA_NORMAL_POWER_ON_CNT_CEIL : usTemp1;
        sprintf((char*)ucTempBuf, "%2d", usTemp1);
		Para_Save(PARA_NORMAL_POWER_ON_CNT_TYPE, (U8 *)ucTempBuf, 2);//存
#endif	
    }
	
    LEDInit(LED_TYPE_POWER_NORMAL);                               //lq LED 电源指示

	UartResponseMsg("$P712,001,,,--------------------------------------");
	UartResponseMsg("$P712,001,,,-- AIS Tranceiver2");
	sprintf(chTemp, "$P712,001,,,-- SW Ver %s", VERSION);
	UartResponseMsg(chTemp);
	//Para_Read(PARA_HWVERSION_TYPE, ucTempBuf);	
	FlashRead(STM32_FLASH_HWVERSION_TYPE, ucTempBuf);
    sprintf(chTemp, "$P712,001,,,-- HW Ver %s", ucTempBuf);
	UartResponseMsg(chTemp);	
    sprintf(chTemp, "$P712,001,,,-- Last compiled: %s %s", __DATE__, __TIME__);
	UartResponseMsg(chTemp);
	UartResponseMsg("$P712,001,,,--------------------------------------");
	//File_Create(fp);
}

/*******************************************************************************
* 名称  : InitBKP_DRPara
* 描述  : 使用备份数据寄存器的数据初始化相关参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitBKP_DRPara(void)
{
#if 0
    /*lq 标志数据*/
    GpsCtrlPara.ucGpsUnfix1hLngOpenFlg = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                         BKPR_FLAG_BIT_GPS_LNG_OPEN_FIRST_HOUR_INDEX);
    GpsCtrlPara.ucGpsUnfixBey1hLngOpenFlg = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                            BKPR_FLAG_BIT_GPS_LNG_OPEN_BEY_FIRST_HOUR_INDEX);
    GpsCtrlPara.ucGps5minLngOpenEn = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                     BKPR_FLAG_BIT_GPS_5MIN_LONG_OPEN_EN_INDEX);
    GpsCtrlPara.GpsPowerupFix = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                BKPR_FLAG_BIT_GPS_POWER_UP_FIX_INDEX);
    GpsCtrlPara.ucGpsOnTimEn = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                               BKPR_FLAG_BIT_GPS_ON_TIM_EN_INDEX);
    GpsCtrlPara.ucGpsOffTimEn = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                BKPR_FLAG_BIT_GPS_OFF_TIM_EN_INDEX);
    GpsCtrlPara.ucGpsSaveFlg = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                BKPR_FLAG_BIT_GPS_SAVE_INDEX);
    GpsCtrlPara.ucGpsOnEn = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                BKPR_FLAG_BIT_GPS_ON_INDEX);
    GpsPara.GPS_LeapSecondFlg = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                BKPR_FLAG_BIT_GPS_LEAP_SECOND_INDEX);
    struRtcPara.ucRtcLSEorLSI = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                BKPR_FLAG_BIT_RTC_LSE_OR_LSI_INDEX);
    struRtcTimPhase1.RtcTimPhaseFlg = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                BKPR_FLAG_BIT_RTC_TIM_PHASE_INDEX);
    GpsSynCell.GPSSyn_Enable = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                BKPR_FLAG_BIT_GPS_SYNCH_ENABLE_INDEX);
    eqEquipPara.ucWorkMode = GetBKPDRDataBit(BKPR_FLAG_INDEX, 
                                BKPR_FLAG_BIT_EQ_WORKMOD_INDEX);                                                                
                                  
    GpsPara.GPS_Latitude = arrBKPDRData[BKPR_GPS_LATITUDE_H_INDEX];
    GpsPara.GPS_Latitude = ((GpsPara.GPS_Latitude << 16) & 0xFFFF0000) 
                            | (arrBKPDRData[BKPR_GPS_LATITUDE_L_INDEX] & 0xFFFF); 
    GpsPara.GPS_Longitud = arrBKPDRData[BKPR_GPS_LONGITUDE_H_INDEX];                   
    GpsPara.GPS_Longitud = ((GpsPara.GPS_Longitud << 16) & 0xFFFF0000) 
                            | (arrBKPDRData[BKPR_GPS_LONGITUDE_L_INDEX] & 0xFFFF); 
    GpsPara.GPS_Sog = arrBKPDRData[BKPR_GPS_SOG_INDEX];
    GpsPara.GPS_Cog = arrBKPDRData[BKPR_GPS_COG_INDEX];
    // GpsPara.GPS_TimeH = arrBKPDRData[BKPR_GPS_TIME_HOUR_INDEX];
    // GpsPara.GPS_TimeM = arrBKPDRData[BKPR_GPS_TIME_MIN_INDEX];
   
    GpsCtrlPara.GpsFallCount = arrBKPDRData[BKPR_GPS_FALL_COUNT_INDEX];
    GpsCtrlPara.GpsCtrlSta = arrBKPDRData[BKPR_GPS_CTRL_STA_INDEX]; 
    GpsCtrlPara.GpsUnfix1hPeriodTimFrame = arrBKPDRData[BKPR_GPS_UNFIX_1H_PERIOD_TIM_FRAME_H_INDEX];
    GpsCtrlPara.GpsUnfix1hPeriodTimFrame = ((GpsCtrlPara.GpsUnfix1hPeriodTimFrame << 16) & 0xFFFF0000) 
                                            | (arrBKPDRData[BKPR_GPS_UNFIX_1H_PERIOD_TIM_FRAME_L_INDEX] & 0xFFFF); 
    GpsCtrlPara.GpsUnfix1hPeriodTimSlot = arrBKPDRData[BKPR_GPS_UNFIX_1H_PERIOD_TIM_SLOT_INDEX];
    GpsCtrlPara.GpsOnTimFrame = arrBKPDRData[BKPR_GPS_START_FRAME_H_INDEX];
    GpsCtrlPara.GpsOnTimFrame = ((GpsCtrlPara.GpsOnTimFrame << 16) & 0xFFFF0000) 
                                 | (arrBKPDRData[BKPR_GPS_START_FRAME_L_INDEX] & 0xFFFF); 
    GpsCtrlPara.GpsOnTimSlot = arrBKPDRData[BKPR_GPS_START_SLOT_INDEX];
    GpsCtrlPara.GpsOffTimFrame = arrBKPDRData[BKPR_GPS_OFF_FRAME_H_INDEX]; 
    GpsCtrlPara.GpsOffTimFrame = ((GpsCtrlPara.GpsOffTimFrame << 16) & 0xFFFF0000) 
                                  | (arrBKPDRData[BKPR_GPS_OFF_FRAME_L_INDEX] & 0xFFFF); 
    GpsCtrlPara.GpsOffTimSlot = arrBKPDRData[BKPR_GPS_OFF_SLOT_INDEX];                                  

    struSOTDMAPara.BurstTxTimFrame = arrBKPDRData[BKPR_BURST_TX_TIM_FRAME_H_INDEX];
    struSOTDMAPara.BurstTxTimFrame = ((struSOTDMAPara.BurstTxTimFrame << 16) & 0xFFFF0000) 
                                      | (arrBKPDRData[BKPR_BURST_TX_TIM_FRAME_L_INDEX] & 0xFFFF); 
    struSOTDMAPara.BurstTxTimSlot = arrBKPDRData[BKPR_BURST_TX_TIM_SLOT_INDEX]; 
#if 0
    struSOCommState.ucSlotTimeout = arrBKPDRData[BKPR_SLOT_TIMEOUT_INDEX];
    struSOCommState.usSlotOffset = arrBKPDRData[BKPR_SLOT_OFFSET_INDEX];
#endif
    struRtcTimPhase1.RtcTimPhaseNum = arrBKPDRData[BKPR_RTC_TIM_PHASE_H_INDEX];
    struRtcTimPhase1.RtcTimPhaseNum = ((struRtcTimPhase1.RtcTimPhaseNum << 16) & 0xFFFF0000) 
                            | (arrBKPDRData[BKPR_RTC_TIM_PHASE_L_INDEX] & 0xFFFF); 
    struRtcTimPhase1.lfRtcTimPhaseNum = (double)struRtcTimPhase1.RtcTimPhaseNum / RTC_TIM_PHASE_PRECISE; 

    struRtcPara.usRtcFreq = arrBKPDRData[BKPR_RTC_FREQUENCE_INDEX];
    struRtcPara.ucRtcAlarmMcuOrGps = arrBKPDRData[BKPR_RTC_ALARM_MCU_OR_GPS_INDEX];
    // struRtcPara.ssRtcAlarmValue = arrBKPDRData[BKPR_RTC_ALARM_VALUE_INDEX];
    struRtcPara.ssRtcAlarmValue = arrBKPDRData[BKPR_RTC_ALARM_VALUE_H_INDEX];
    struRtcPara.ssRtcAlarmValue = ((struRtcPara.ssRtcAlarmValue << 16) & 0xFFFF0000) 
                            | (arrBKPDRData[BKPR_RTC_ALARM_VALUE_L_INDEX] & 0xFFFF); 

    gslFrameCnt = arrBKPDRData[BKPR_FRAME_CNT_H_INDEX];
    gslFrameCnt = ((gslFrameCnt << 16) & 0xFFFF0000) | (arrBKPDRData[BKPR_FRAME_CNT_L_INDEX] & 0xFFFF); 
	gsiSlotCnt = arrBKPDRData[BKPR_SLOT_CNT_INDEX];
	gsiPhaseCnt = arrBKPDRData[BKPR_PHASE_CNT_INDEX];

    TimAdjust1 = arrBKPDRData[BKPR_TIM_ADJUST_1_INDEX];
    TimAdjust10 = arrBKPDRData[BKPR_TIM_ADJUST_10_INDEX]; 
    TimAdjust100 = arrBKPDRData[BKPR_TIM_ADJUST_100_INDEX];

    TimAdjustPara.PeriodCountLast = arrBKPDRData[BKPR_TIM_ADJUST_PERIOD_CNT_LAST_INDEX];  

    // RtcPhaseAdjust1 = arrBKPDRData[BKPR_RTC_ADJUST_1_INDEX];
    // RtcPhaseAdjust10 = arrBKPDRData[BKPR_RTC_ADJUST_10_INDEX];
    // RtcPhaseAdjust100 = arrBKPDRData[BKPR_RTC_ADJUST_100_INDEX];
    GpsCtrlPara.GpsIntervalCnt = arrBKPDRData[BKPR_GPS_INTERVAL_CNT_INDEX];
  
    send_seed.seed = arrBKPDRData[BKPR_RAND_SEED_H_INDEX];
    send_seed.seed = ((send_seed.seed << 16) & 0xFFFF0000) | (arrBKPDRData[BKPR_RAND_SEED_L_INDEX] & 0xFFFF);  
#endif
}

/*******************************************************************************
* 名称  : CalcuSleepSlots_Wakeup
* 描述  : 计算休眠的时隙数
* 输入  : RTC设定值		  
* 输出  : 无
* 返回  : RTC设定值对应的时隙数
*******************************************************************************/
U32 CalcuSleepSlots_Wakeup(const U32 ulAlarmValue)
{
    U32 ulSlotDiff;
 
    ulSlotDiff = ulAlarmValue * 37.5 / arrBKPDRData[BKPR_RTC_FREQUENCE_INDEX] + 0.5;

    return ulSlotDiff;
}

/*******************************************************************************
* 名称  : CalcuSleepPhases_Wakeup
* 描述  : 计算休眠的相位数
* 输入  : RTC设定值		  
* 输出  : 无
* 返回  : RTC设定值对应的相位数
*******************************************************************************/
U32 CalcuSleepPhases_Wakeup(const U32 ulAlarmValue)
{
    U32 ulPhaseDiff;
 
    ulPhaseDiff = ulAlarmValue * 48000 / arrBKPDRData[BKPR_RTC_FREQUENCE_INDEX];

    return ulPhaseDiff;
}
#if 0
/*******************************************************************************
* 名称  : SleepWakeUpInit
* 描述  : 休眠唤醒初始化
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SleepWakeUpInit(void)
{
    int i;
    S32 slGPS_Longitud_Buf_Elem;
    S32 slGPS_Latitude_Buf_Elem;
    U32 slGPS_SOG_Buf_Elem;
	U8 	ucTempBuf[40];
    S32 PhaseDiff;
    S32 Temp1sCnt = 0;
    S32 Temp10sCnt = 0;
    S32 Temp100sCnt = 0;
    S32 slTemp1;

    /*lq 待机唤醒后初始化*/
#if 0
    gsiSlotCnt += CalcuSleepSlots_Wakeup(struRtcPara.ssRtcAlarmValue);
    if (gsiSlotCnt >= 2250)
    {
        gslFrameCnt +=  gsiSlotCnt / 2250;
        gsiSlotCnt = gsiSlotCnt % 2250;
    }
#endif

#if 1
    /*lq 待机时长对应的相位数*/
    // PhaseDiff = CalcuSleepPhases_Wakeup(struRtcPara.ssRtcAlarmValue);
    
    /*lq 将RTC的相位数转换为Tim计数*/
    PhaseDiff = (struRtcPara.ssRtcAlarmValue + 1) * ((double)struRtcTimPhase1.RtcTimPhaseNum / RTC_TIM_PHASE_PRECISE) + 0.5;

    Temp1sCnt = PhaseDiff / 48000;
    Temp10sCnt = PhaseDiff / 480000;
    Temp100sCnt = PhaseDiff / 4800000;

    /*lq 对间隔相位数进行校准*/
    PhaseDiff -= TimAdjust1 * Temp1sCnt;
    PhaseDiff -= TimAdjust10 * Temp10sCnt;
    PhaseDiff -= TimAdjust100 * Temp100sCnt;
    
    PhaseDiff += gsiPhaseCnt;
    if (PhaseDiff < 0)
    {
        slTemp1 = PhaseDiff / 1280;
        slTemp1--;                                                              
        gsiPhaseCnt = (-slTemp1 * 1280 + PhaseDiff);                                //lq 借位
        gsiSlotCnt += slTemp1;
        if (gsiSlotCnt < 0)
        {
            slTemp1 = gsiSlotCnt / 2250;
            slTemp1--;                                                              
            gsiSlotCnt = (-slTemp1 * 2250 + gsiSlotCnt);                            //lq 借位
            gslFrameCnt += slTemp1;
            if (gslFrameCnt < 0)
            {
                gslFrameCnt = 0;
                gsiSlotCnt = 0;
                gsiPhaseCnt = 0;
            }
        }
    }
    else
    {
        gsiPhaseCnt = PhaseDiff % 1280;
        gsiSlotCnt += PhaseDiff / 1280;
        if (gsiSlotCnt >= 2250)
        {
            gslFrameCnt +=  gsiSlotCnt / 2250;
            gsiSlotCnt = gsiSlotCnt % 2250;
        }    
    }

#endif
#if 1    
    /*lq 每次进入standby后RTC中断使能位因掉电无效，待机唤醒后需重新使能*/    
	EXTI_ClearITPendingBit(EXTI_Line17);	
    
    /* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();
	RTC_WaitForLastTask();   

	/* Clear RTC Alarm interrupt pending bit */
	RTC_ClearITPendingBit(RTC_IT_ALR);	
	
    /* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
	/* Enable the RTC Alarm interrupt */
	RTC_ITConfig(RTC_IT_ALR, ENABLE);

	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

    /*lq 设置RTC Alarm值*/
    //ResetRtcCount(30);
	RTC_EnterConfigMode();
     
	RTC_WaitForLastTask();
	//RTC_WaitForSynchro();
#if 1
    RTC_SetAlarm(RTC_GetCounter() + 1);
#else
    TempAlarm = RTC_GetCounter() + 3;
    RTC_SetAlarm(TempAlarm);
#endif
	RTC_WaitForLastTask();
	RTC_ExitConfigMode();

	RTC_WaitForLastTask();
#endif
    slGPS_Longitud_Buf_Elem = GpsPara.GPS_Longitud / GPS_POSITION_BUFF_LENGTH;
    slGPS_Latitude_Buf_Elem = GpsPara.GPS_Latitude / GPS_POSITION_BUFF_LENGTH;
    for (i = 0; i < GPS_POSITION_BUFF_LENGTH; i++)
    {
        GpsPara.GPS_Longitud_Buf[i] = slGPS_Longitud_Buf_Elem;
        GpsPara.GPS_Latitude_Buf[i] = slGPS_Latitude_Buf_Elem;    
    }

//lq    slGPS_SOG_Buf_Elem = GpsPara.GPS_Sog / 18;
    slGPS_SOG_Buf_Elem = GpsPara.GPS_Sog;
    for (i = 0; i < 18; i++)
    {
        GpsPara.GPS_Sog_Buf[i] = slGPS_SOG_Buf_Elem;
    }

    /*lq 消息字段初始化（需要在GpsPara.GPS_Sog GpsPara.GPS_Cog等使用Bkp_DR中的值重新赋值之后）*/
    ResetMsgPara();
      
    /*lq 工作状态*/
    /*lq 若工作模式为测试模式，则一定是电池测试模式，其他测试模式不会进入待机模式*/
    if (eqEquipPara.ucWorkMode == EQ_WORKMOD_TEST)
    {
        eqEquipPara.TestSta = EQ_MOD_TEST_STA_BATTERY;    
    }
    else
    {
	    eqEquipPara.ucWorkMode = EQ_WORKMOD_NORMAL;    
    }
    eqEquipPara.ucLowFreqSta = EQ_FREQSTA_LOW;

    MsgCtrlPara.MsgCtrlState = MSG_CTRL_STA_IDLE;
    state_4501 = NORMAL_WORK_BURST;                
	if (struRtcPara.ucRtcLSEorLSI == RTC_CLK_SOURCE_LSI)
    {
        TimAdjustPara.RtcClockSource = ADJUST_RTC_SOURCE_LSI;
	    RtcCnt1s = RTC_ADJUST_1S_LSI;
        RtcCnt10s = RTC_ADJUST_10S_LSI;
        RtcCnt100s = RTC_ADJUST_100S_LSI;              
    }
    else
    {
        TimAdjustPara.RtcClockSource = ADJUST_RTC_SOURCE_LSE;
        RtcCnt1s = RTC_ADJUST_1S;
        RtcCnt10s = RTC_ADJUST_10S;
        RtcCnt100s = RTC_ADJUST_100S;
    } 
	TimAdjustPara.RtcClockChoice = TRUE;

//    RTCConfigurationPart2();

    /*lq 设定MCU唤醒定时*/				
    SetMcuWakeupTim(SLOT_INTERVAL_MCU_WAKEUP_TO_TX);
 
    /*lq 设定组消息发送定时*/
    SetBurstSendTim(SLOT_INTERVAL_SEND_TO_TX);

    /*lq 从EEPROM中读取GPS成功定位次数、GPS超过3次不定位的次数、GPS5min长开次数赋值给对应的变量*/ 
	Para_Read(PARA_GPS_FIX_COUNT_TYPE, ucTempBuf);
    GpsCtrlPara.GpsFixCount = atol((char *)ucTempBuf);

	Para_Read(PARA_GPS_WAIT_COUNT_TYPE, ucTempBuf);
    GpsCtrlPara.GpsWaitCount = atol((char *)ucTempBuf);

    Para_Read(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTempBuf);
    GpsCtrlPara.ssGps5minLngOpenCnt = atol((char *)ucTempBuf);

    // UartResponseMsg("$MSG_CTRL_STATE : MSG_CTRL_STA_IDEL after sleeping!");
    
    TEST_OUT_HIGH_2();                               
            
}
#endif
/*******************************************************************************
* 名称  : PowerUpLedCtrl
* 描述  : 正常开机LED指示
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void PowerUpLedCtrl(void)
{
	int i;

	for (i = 0; i < 2; i++)
	{
        LEDAllLight();
		Delay_ms(500);
		Is_Feed_Dog = TRUE;
		Feed_Dog();
           
        LEDAllDown();
		Delay_ms(500);
		Is_Feed_Dog = TRUE;
		Feed_Dog();
	}
}

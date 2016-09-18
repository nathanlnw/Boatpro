/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: stm32f10x_target.c
  Author: 王伟       Version : V1.00        Date: 2011.09.23
  Description:    STM32芯片相关配置
  Version:         V1.00
  Function List:   // 主要函数及其功能
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
//#include "stm32f10x_starup.s"
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "migrate.h"
#include "def.h"
#include "power.h"
#include "Adc.h"
#include "DAC.h"
#include "usart.h"
#include "Tim.h"
#include "LED.h"
#include "24c02.h"
#include "gps.h"
#include "wdg.h"
#include "gps.h"
#include "rtc.h"
#include "powerup.h"
#include "gpsctrl.h"
#include "parastorage.h"
#include "sotdma.h"
#include "RxTx.h"
#include "config.h"
#include "Include.h"
#include  "misc.h"
#include "sleepctrl.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus HSEStartUpStatus;
void RCC_GetHSE_Value(vu32 User_HSE_Value);
/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/

/*******************************************************************************
* 名称  : RCC_Configuration
* 描述  : stm32f10x时钟系统配置
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void RCC_Configuration(void)	
{
	/* RCC system reset(for debug purpose) */
	RCC_DeInit();
	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);
	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	if(HSEStartUpStatus == SUCCESS)
	{
		/* Enable Prefetch Buffer */
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);		
		/* Flash 2 wait state */
		FLASH_SetLatency(FLASH_Latency_2);

		/* HCLK = SYSCLK */
		RCC_HCLKConfig(RCC_SYSCLK_Div1); 
		
		/* PCLK2 = HCLK */
		RCC_PCLK2Config(RCC_HCLK_Div1); 
		
		/* PCLK1 = HCLK/2 */
		RCC_PCLK1Config(RCC_HCLK_Div2);
		
		/* ADCCLK = PCLK2/4 */
		RCC_ADCCLKConfig(RCC_PCLK2_Div4);
		 
		/* PLLCLK = 12MHz * 6 = 72 MHz */
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6);
		
		/* Enable PLL */ 
		RCC_PLLCmd(ENABLE);
		
		/* Wait till PLL is ready */
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
		
		/* Select PLL as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		
		/* Wait till PLL is used as system clock source */
		while(RCC_GetSYSCLKSource() != 0x08);
	}
	
	/* Enable GPIOx and AFIO clocks */                                          
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |        //lq 与Clock_Configuration()中的配置重复
	                       RCC_APB2Periph_GPIOC , ENABLE);
	
}

/***************************************************************************************
** 函数名称: NVIC_Configuration
** 功能描述: stm32f10x配置基本向量中断
** 参    数: None
** 返 回 值: None       
** 作　  者: 
** 日  　期: 
**--------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
static void NVIC_Configuration(void)
{ 
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Set the Vector Table base location at 0x08000000 */ 
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	
	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/* Enable the USART1 Interrupt */                                           //lq USB串口中断
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable the USART2 Interrupt */                                           //lq GPS报文中断
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the TIM2 gloabal Interrupt */                                     //lq TIM2定时器中断
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   //lq 3;   //lq 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //lq 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);
#if 0
	/* Enable the ADC1_2 gloabal Interrupt */                                   //lq 电池电量检测ADC中断
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);
#endif
	/* Enable the DMA1 gloabal Interrupt */                                     //lq [2013/10/30]电池电量检测DMA中断
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);

	/* Configure and enable EXTI1 interrupt --------------------------------*/  //lq GPS秒脉冲中断
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQChannel;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

	/* Configure and enable RTC interrupt -----------------------------------*/ //lq RTC定时器中断
	NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);
#if CONFIG_KEY
	/* Configure and enable EXTI15_10 interrupt -----------------------------*/ //lq power键中断
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQChannel;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

	/* Configure and enable EXTI15_10 interrupt -----------------------------*/ //lq sos键中断
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQChannel;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
#endif
#if 0
    /* Configure and enable SPI2 interrupt -------------------------------------*/
    NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);    	
#endif
	/* Enable the SPIRIT1 IRQ Interrupt */                                       //lq SPIRIT1中断
#if 0
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

	/* Configure and enable EXTI15_10 interrupt -----------------------------*/ //lq Si446x 收发数据时钟中断
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQChannel;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

#if CONFIG_SD_CARD_MODULE_EN
	/* Enable the SDIO Interrupt */                                           //lq SDIO中断,即SD卡中断
	NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

}

/*******************************************************************************
* 名称  : EnableRtcInterrupt
* 描述  : 使能RTC中断。防止RTC中断频繁造成开机延迟
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void EnableRtcInterrupt(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
	

/*******************************************************************************
* 名称  : Clock_Configuration
* 描述  : 根据实际需要开启和关闭外设时钟，以降低功耗。
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void Clock_Configuration(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SRAM
							| RCC_AHBPeriph_FLITF 
							| RCC_AHBPeriph_CRC
							| RCC_AHBPeriph_SDIO, DISABLE);
	/* Enable DMA1 clock */
  	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2, DISABLE);
  	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* Enable the FSMC Clock */	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, DISABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ALL, DISABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO							
							| RCC_APB2Periph_GPIOA
							| RCC_APB2Periph_GPIOB
							| RCC_APB2Periph_GPIOC
							| RCC_APB2Periph_GPIOD
							| RCC_APB2Periph_GPIOE
							| RCC_APB2Periph_GPIOF
							| RCC_APB2Periph_GPIOG
							| RCC_APB2Periph_ADC1
							| RCC_APB2Periph_USART1
							, ENABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_ALL, DISABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2
							| RCC_APB1Periph_BKP
							| RCC_APB1Periph_DAC 
							| RCC_APB1Periph_USART2
                            | RCC_APB1Periph_SPI2
							| RCC_APB1Periph_I2C1
                            , ENABLE);


}

/*******************************************************************************
* Function Name  : SYSCLKConfig_STOP
* Description    : Configures system clock after wake-up from STOP: enable HSE, PLL
*                  and select PLL as system clock source.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SYSCLKConfig_STOP(void)
{
	ErrorStatus HSEStartUpStatus;
	
	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);
	
	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if(HSEStartUpStatus == SUCCESS)
	{
		/* Enable PLL */ 
		RCC_PLLCmd(ENABLE);
		
		/* Wait till PLL is ready */
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}
		
		/* Select PLL as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		
		/* Wait till PLL is used as system clock source */
		while(RCC_GetSYSCLKSource() != 0x08)
		{
		}
	}

    /*lq 时钟校准计时清0*/
//    InitTim2Para_2();
#if 0
    /*lq 清除RTC寄存器同步标志位*/
    RTC_WaitForLastTask();
    RTC_ClearFlag(RTC_FLAG_RSF);
    RTC_WaitForLastTask();   //20131017
#endif

}

/*******************************************************************************
* 名称  : EnterStopMod
* 描述  : 进入省电状态
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void EnterStopMod(void)
{
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
	//PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);//打开电压转换器
	/* Configures system clock after wake-up from STOP: enable HSE, PLL and select 
	   PLL as system clock source (HSE and PLL are disabled in STOP mode) */
	SYSCLKConfig_STOP();
}

/*******************************************************************************
* 名称  : EnterStandbyMod
* 描述  : 进入待机状态
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void EnterStandbyMod(void)
{
	#if 0
	U8 ucTemp[10];
	U8 ucTemp1[10];
  #endif
	
    /*lq 保存数据至备份数据寄存器*/        
    SaveBKP_DRPara();
    SaveBKP_DRData(); 

    // usart_printf("ssAlarmValue : %ld\r\n",struRtcPara.ssRtcAlarmValue);				                    
#if 0
    /*lq 保存GPS成功定位次数、GPS超过3次不定位的次数、GPS5min长开次数至EEPROM */
	sprintf((char *)ucTemp, "%5d", GpsCtrlPara.GpsFixCount);
   	Para_Save(PARA_GPS_FIX_COUNT_TYPE, ucTemp, 5);

	sprintf((char *)ucTemp, "%3d", GpsCtrlPara.GpsWaitCount);
   	Para_Save(PARA_GPS_WAIT_COUNT_TYPE, ucTemp, 3);

	sprintf((char *)ucTemp, "%3d", GpsCtrlPara.ssGps5minLngOpenCnt);
   	Para_Save(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTemp, 3);

    /*lq 记录本次开机后工作时长*/
    sprintf(ucTemp, "%5d", (U16)gslFrameCnt);
    Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)ucTemp, 5); 

    /*lq 每次进入待机前保存一次关机时间*/
	if (GetGpsNormalFixState() == TRUE)                                //lq 只有本次定位成功后，才会记录关机时间
	{
		/*lq 记录GPS日期和时间*/
        sprintf((char *)ucTemp, "%2d", GpsPara.GPS_TimeMon);
		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeD);
		strcat((char *)ucTemp, (char *)ucTemp1);
        sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeH);
		strcat((char *)ucTemp, (char *)ucTemp1);
		sprintf((char *)ucTemp1, "%2d", GpsPara.GPS_TimeM);
		strcat((char *)ucTemp, (char *)ucTemp1);
		
		Para_Save(PARA_TIME_CLOSE_TYPE, ucTemp, PARA_TIME_CLOSE_MAXLEN);

#if CONFIG_PARASTORAGE_BTT
        /*lq 记录本地帧计数的工作时长*/
		sprintf((char *)ucTemp,"%5d",GetFrameNum());
		Para_Save(PARA_BATTERY_TEST_TIME_TYPE,ucTemp,5);
#endif
	} 
#endif

#if 1
    /* Generate a system reset */
    NVIC_GenerateSystemReset();                                                 //lq 关掉IWDG
#else  

    PWR_WakeUpPinCmd(ENABLE);                                                   //lq 使能wakeup pin
   
	PWR_EnterSTANDBYMode();
#endif
}


/*******************************************************************************
* 名称  : Target_Periph_Init
* 描述  : 外设配置
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void MCU_DebugConfig(void)
{
    DBGMCU_Config(DBGMCU_STANDBY | DBGMCU_STOP, ENABLE);    
}

/*******************************************************************************
* 名称  : Target_Clock_Init
* 描述  : 系统时钟配置
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void Target_SysClock_Init (void)
{
	//设置外部晶体为12.288MHZ
	//RCC_GetHSE_Value(12000000);                                                 //lq?
	
	/* System Clocks Configuration */                                           //lq 配置HSE PLL SYSCLK HCLK PCLK1 PCLK2 ADCCLK; 
	RCC_Configuration();	                                                    //lq 同时Enable GPIOx and AFIO clocks

	/*lq DebugMCU Configuration*/
	MCU_DebugConfig();
}

/*******************************************************************************
* 名称  : Target_Periph_Init
* 描述  : 外设配置
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void Target_Periph_Init (void)
{

	//根据实际需要开启和关闭外设时钟，以降低功耗。
	Clock_Configuration();                                                      //lq 开启和关闭外设时钟

	//配置实时时钟RTC
	//RTC_Configuration();
    
	RTCConfigurationPart1();                                                    //lq 配置RTCAlarm中断，Enable PWR and BKP clocks，开启LSE时钟    

	NVIC_DeInit();




  
#if 1
	Initia_wdg();//看门狗
#endif	
	InitiaLED();//初始化led                                                     //lq 配置LED端口引脚
	
	InitCommStruc(COMM1, 38400);                                                //lq 配置USB串口
	InitCommStruc(COMM2, 9600);                                                 //lq 配置GPS串口
#if CONFIG_KEY	
    Initia_POWER_Crl();//初始化电源                                             //lq 配置电源开关端口引脚
#endif
	InitTim2();//初始化定时器                                                   //lq 配置定时器Tim2

#if CONFIG_ADC1
    ADC1_PerInit();    //lq AIS Tranceiver不需
#endif
	I2C_EE_Init();//初始化i2c                                                   //lq 配置EEPROM(CAT24C02)
	
#if 0
	DAC_RegInit();//初始化DAC                                                   //lq 配置DAC通道2
#endif
#if 0
	Init_RDA1846();//初始化1846                                                 //lq 配置RDA1846
#endif
	Initia_RFPower();                                                           //lq 配置功放电源开关引脚（初始化功放关闭）

    InitRxTx();
#if 0
    SPIRIT1_Init();
#endif





	//========================================
#if  CONFIG_PTT_EN		 //  lnw   create
	  PTT_Init_IO();
	  PTT_OBJ_INIT();
#endif
	//========================================







    SI446X_ModuleInit();
	
    /* NVIC configuration */
	NVIC_Configuration();                                                       //lq 配置各种中断（中断源，中断优先级，中断使能）
#if 0
    /*lq 配置SPIRIT1 */
    SPIRIT1_InstConfiguration();
#endif
}
/***************************************************************************************
** 函数名称: Target_Init
** 功能描述: 板级初始化函数
** 参    数: None
** 返 回 值: None       
** 作　  者: 
** 日  　期: 
**--------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
void Target_Init (void)
{
   /*lq 系统时钟配置*/
    Target_SysClock_Init();
    
    /*lq 外设配置*/
    InitPowerCtrl();
    Target_Periph_Init();
}




/*******************************************************************************
* 名称  : BSP_CPU_ClkFreq
* 描述  : 获取RCC频率
* 输入  : 无
* 输出  : 无
* 返回  : RCC频率
*******************************************************************************/
u32 BSP_CPU_ClkFreq(void)
{
    static RCC_ClocksTypeDef rcc_clocks;

    RCC_GetClocksFreq(&rcc_clocks);

    return ((u32)rcc_clocks.HCLK_Frequency);
}

#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


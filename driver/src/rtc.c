/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: rtc.c
  Author: vozhj      Version : V1.00        Date: 2011.10.18
  Description:    rtc定时器
  Version:         V1.00
  Function List:   // 主要函数及其功能
  History:         // 历史修改记录
      <author>  <time>   	<version >   <desc>

*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "def.h"
#include "rtc.h"
#include "Tim.h"
#include "initial.h"
#include "power.h"
#include "LED.h"
#include "gpsctrl.h"
#include "adjust.h"
#include "wdg.h"
#include "debug.h"
#include "powerup.h"
#include "BKPRegister.h"
#include "sleepctrl.h"


extern U32 GpsTimCount;
extern U8 TestIO1Flag;
extern U8 TestIOFlag;
U32 RTCIntCount;//不能等除的时候计数来保证一个周期加的phase数正确 
U32 GpsRtcCount;
U32 RtcSubCnt1;
U32 RtcSubCnt2;
U32 RtcSubCnt3;
U32 RtcCnt;
U32 RtcCnt1s;                                                                   //lq 本地Rtc在UTC1s间隔内的计数值
U32 RtcCnt10s;                                                                  //lq 本地Rtc在UTC10s间隔内的计数值
U32 RtcCnt100s;                                                                 //lq 本地Rtc在UTC100s间隔内的计数值

double RtcTimPhaseCnt1 = 0;
double RtcTimPhaseCnt10 = 0;
double RtcTimPhaseCnt100 = 0;

S16 RtcPhaseAdjust1;//rtc每1秒调整的phase计数
S16 RtcPhaseAdjust10;//rtc每10秒调整的phase计数
S16 RtcPhaseAdjust100;//rtc每100秒调整的phase计数
S16 RtcCntAdjust1;//rtc每1秒调整计数
S16 RtcCntAdjust10;//rtc每10秒调整计数
S16 RtcCntAdjust100;//rtc每100秒调整计数

U8 RtcHighCountFlag;//RTC分频FLAG
RtcPara struRtcPara;
RtcTimPhaseStruct struRtcTimPhase1;

#if 1
/*******************************************************************************
* 名称  : CalcRtcPhaseAdjust
* 描述  : 计算RTC低频时要进行的时间调整
* 输入  : DiffNum 4096误差周期数  PeriodCount 统计周期
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CalcRtcPhaseAdjust(S32 DiffNum ,U16 PeriodCount)
{
	S32 slTemp1;
    S16 RtcPhaseAdjust1_Temp;//rtc每1秒调整的phase计数
    S16 RtcPhaseAdjust10_Temp;//rtc每10秒调整的phase计数
    S16 RtcPhaseAdjust100_Temp;//rtc每100秒调整的phase计数

	if (IsRtcClockSourceLSE() == TRUE)
	{
		//LSE
		slTemp1 = DiffNum * 48000 / RTC_LSE_FREQ_VALUE;//换算为时隙数                          //lq 将RTC的中断次数转换为相位数
	}
	else
	{
		//LSI
		slTemp1 = DiffNum * 48000 / RTC_LSI_FREQ_VALUE;//换算为时隙数	
	}

	RtcPhaseAdjust1_Temp = slTemp1 / PeriodCount;
	RtcPhaseAdjust10_Temp = (slTemp1 * 10 - RtcPhaseAdjust1_Temp * PeriodCount * 10) / PeriodCount;
	RtcPhaseAdjust100_Temp = (slTemp1 * 100 - RtcPhaseAdjust1_Temp * PeriodCount * 100 
                         - (RtcPhaseAdjust10_Temp * PeriodCount) * 10) / PeriodCount;
	
    usart_printf("RtcPhaseAdjust1_Temp  : %d\r\n", RtcPhaseAdjust1_Temp);				
	usart_printf("RtcPhaseAdjust10_Temp : %d\r\n", RtcPhaseAdjust10_Temp);				
	usart_printf("RtcPhaseAdjust100_Temp: %d\r\n", RtcPhaseAdjust100_Temp);

    /*lq 将Tim的时间调整值转换为最短时间调整值*/
    TransPhaseAdjust(&RtcPhaseAdjust1_Temp, &RtcPhaseAdjust10_Temp, &RtcPhaseAdjust100_Temp);

	if (RtcPhaseAdjust1_Temp < ADJUST_PHASE_MAX_ADJUST 
        && RtcPhaseAdjust1_Temp > -ADJUST_PHASE_MAX_ADJUST)                      //lq RtcPhaseAdjust10_Temp,RtcPhaseAdjust100_Temp取值范围[0,9]
	{
		RtcPhaseAdjust1 = RtcPhaseAdjust1_Temp;
		RtcPhaseAdjust10 = RtcPhaseAdjust10_Temp;
		RtcPhaseAdjust100 = RtcPhaseAdjust100_Temp;
	}
}
#else
/*******************************************************************************
* 名称  : CalcRtcPhaseAdjust
* 描述  : 计算RTC低频时要进行的时间调整(RtcPhaseAdjust1每次在原值上累加)
* 输入  : DiffNum 4096误差周期数  PeriodCount 统计周期
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CalcRtcPhaseAdjust(S32 DiffNum ,U16 PeriodCount)
{
	S32 slTemp1;
//    double slTemp1;
    S16 RtcPhaseAdjust1_Temp;//rtc每1秒调整的phase计数
    S16 RtcPhaseAdjust10_Temp;//rtc每10秒调整的phase计数
    S16 RtcPhaseAdjust100_Temp;//rtc每100秒调整的phase计数
    S16 RtcCntAdjust1_Temp;
    S16 RtcCntAdjust10_Temp;
    S16 RtcCntAdjust100_Temp;

    usart_printf("Rtc DiffNum  : %d\r\n", DiffNum);				

    /*lq Rtc计数偏差计算*/
    RtcCntAdjust1_Temp = DiffNum / PeriodCount;
    RtcCntAdjust10_Temp = (DiffNum * 10 - RtcCntAdjust1_Temp * PeriodCount * 10) / PeriodCount;
    RtcCntAdjust100_Temp = (DiffNum * 100 - RtcCntAdjust1_Temp * PeriodCount * 100 
                              - ((RtcCntAdjust10_Temp * PeriodCount) / 10) * 100) / PeriodCount;

	if (RtcCntAdjust1_Temp > ADJUST_PHASE_MAX_ADJUST 
        || RtcCntAdjust1_Temp < -ADJUST_PHASE_MAX_ADJUST)//超出范围，出现错误
	{
		RtcCntAdjust1_Temp = 0;
	}

	if (RtcCntAdjust10_Temp > ADJUST_PHASE_MAX_ADJUST 
        || RtcCntAdjust10_Temp < -ADJUST_PHASE_MAX_ADJUST)//超出范围，出现错误
	{
		RtcCntAdjust10_Temp = 0;
	}

	if (RtcCntAdjust100_Temp > ADJUST_PHASE_MAX_ADJUST 
        || RtcCntAdjust100_Temp < -ADJUST_PHASE_MAX_ADJUST)//超出范围，出现错误
	{
		RtcCntAdjust100_Temp = 0;
	}

    RtcCntAdjust1 += RtcCntAdjust1_Temp;                                        //rtc每1秒调整计数
    RtcCntAdjust10 += RtcCntAdjust10_Temp;                                      //rtc每10秒调整计数
    RtcCntAdjust100 += RtcCntAdjust100_Temp;                                    //rtc每100秒调整计数

    /*lq Rtc相位偏差计算*/
	if (IsRtcClockSourceLSE() == TRUE)
	{
		//LSE
		slTemp1 = DiffNum * 48000 / RTC_LSE_FREQ_VALUE;//换算为时隙数                      //lq 将RTC的中断次数转换为相位数
	}
	else
	{
		//LSI
		slTemp1 = DiffNum * 48000 / RTC_LSI_FREQ_VALUE;//换算为时隙数	
	}

#if 0
	RtcPhaseAdjust1_Temp = (S16)(slTemp1 / PeriodCount);
	RtcPhaseAdjust10_Temp = (S16)((slTemp1 * 10 - RtcPhaseAdjust1_Temp * PeriodCount * 10) / PeriodCount);
	RtcPhaseAdjust100_Temp = (S16)((slTemp1 * 100 - RtcPhaseAdjust1_Temp * PeriodCount * 100 
                              - ((RtcPhaseAdjust10_Temp * PeriodCount) / 10) * 100) / PeriodCount);
#else
	RtcPhaseAdjust1_Temp = slTemp1 / PeriodCount;
	RtcPhaseAdjust10_Temp = (slTemp1 * 10 - RtcPhaseAdjust1_Temp * PeriodCount * 10) / PeriodCount;
	RtcPhaseAdjust100_Temp = (slTemp1 * 100 - RtcPhaseAdjust1_Temp * PeriodCount * 100 
                              - ((RtcPhaseAdjust10_Temp * PeriodCount) / 10) * 100) / PeriodCount;
#endif

	if (RtcPhaseAdjust1_Temp > ADJUST_PHASE_MAX_ADJUST 
        || RtcPhaseAdjust1_Temp < -ADJUST_PHASE_MAX_ADJUST)//超出范围，出现错误
	{
		RtcPhaseAdjust1_Temp = 0;
	}

	if (RtcPhaseAdjust10_Temp > ADJUST_PHASE_MAX_ADJUST 
        || RtcPhaseAdjust10_Temp < -ADJUST_PHASE_MAX_ADJUST)//超出范围，出现错误
	{
		RtcPhaseAdjust10_Temp = 0;
	}

	if (RtcPhaseAdjust100_Temp > ADJUST_PHASE_MAX_ADJUST 
        || RtcPhaseAdjust100_Temp < -ADJUST_PHASE_MAX_ADJUST)//超出范围，出现错误
	{
		RtcPhaseAdjust100_Temp = 0;
	}

	RtcPhaseAdjust1 += RtcPhaseAdjust1_Temp;
	RtcPhaseAdjust10 += RtcPhaseAdjust10_Temp;
	RtcPhaseAdjust100 += RtcPhaseAdjust100_Temp;
}
#endif

/*******************************************************************************
* 名称  : InitRtcPara
* 描述  : 初始化RTC控制参数
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitRtcPara(void)
{
	RTCIntCount = 0;
	GpsRtcCount = 0;
    RtcCnt = 0;

	RtcPhaseAdjust1 = 0;//调整的时隙数
	RtcPhaseAdjust10 = 0;//调整的时隙数
	RtcPhaseAdjust100 = 0;//调整的时隙数
    
    RtcCntAdjust1 = 0;//rtc每1秒调整计数
    RtcCntAdjust10 = 0;//rtc每10秒调整计数
    RtcCntAdjust100 = 0;//rtc每100秒调整计数

	RtcHighCountFlag = TRUE;
}

/*******************************************************************************
* 名称  : ResetRtcCount
* 描述  : 复位RTC计数
* 输入  : RTC定时值
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ResetRtcCount(U16 usAlarmValue)
{
	RTC_WaitForLastTask();
	RTC_EnterConfigMode(); 
		
	RTC_WaitForLastTask();
	RTC_SetCounter(0);//重新开始RTC计数	
    RTC_WaitForSynchro();                                                   //lq 解决工作状态和休眠状态下RTC频率不一致的问题

	RTC_WaitForLastTask();
	RTC_SetAlarm(RTC_GetCounter() + usAlarmValue);

	RTC_WaitForLastTask();
	RTC_ExitConfigMode();

	RTC_WaitForLastTask();
}	

/*******************************************************************************
* 名称  : RTCConfigurationPart1
* 描述  : 配置实时时钟RTC的第一部分
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void RTCConfigurationPart1(void)                                                //lq 完成RTC中断配置
{
	//设置RTC中断
	EXTI_InitTypeDef EXTI_InitStructure;

	/* Configure EXTI Line17(RTC Alarm) to generate an interrupt on rising edge */
	EXTI_ClearITPendingBit(EXTI_Line17);
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;                                 //lq Line17为RTCAlarm中断
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);

    /*lq 仅上电开机时执行*/
    if (ucStandbyWakeupFlg == FALSE)
    {
	    /* Reset Backup Domain */
	    BKP_DeInit();

    	/* Enable the LSI OSC */
    	//RCC_LSICmd(ENABLE);
        RCC_LSEConfig(RCC_LSE_ON); //RCC打开了LSE时钟                               //lq 先打开外部晶振，待机唤醒后LSE会保持休眠之前的状态，不会复位
    }	
}

/*******************************************************************************
* 名称  : RTCConfigurationPart2
* 描述  : 配置实时时钟RTC的第二部分
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void RTCConfigurationPart2(void)                                                //lq 根据选择的RTC时钟源，配置时钟频率
{
	//此部分的等待放置在timeadjust部分判断
//	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)//等待LSE有效
//	{}
//
//	UartResponseMsg("32.768MHz CLOCK READY!");
//
//	/* Select the RTC Clock Source */
//	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//配置时钟为LSE
//	
	/* Enable RTC Clock */
	RCC_RTCCLKCmd(ENABLE);
	
	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();             //lq 用时约220us
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();            //lq 用时约600ns

	/* Enable the RTC Alarm interrupt */
	//RTC_ITConfig(RTC_IT_ALR, ENABLE);
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	
	/* Set RTC prescaler: set RTC period to 1sec */
	
	if (IsRtcClockSourceLSE() == TRUE)
	{
		//LSE
		//lq RTC_SetPrescaler(31);//高频1024 低频512                                 //lq 32分频（分频计数0-31）
	    RTC_SetPrescaler(RTC_LSE_PRE_VALUE - 1);                                                   //lq 512分频，频率为64
        RtcCnt1s = RTC_ADJUST_1S;
        RtcCnt10s = RTC_ADJUST_10S;
        RtcCnt100s = RTC_ADJUST_100S;
	}
	else
	{
		//LSI
		//lq RTC_SetPrescaler(39);//高频1000 低频500                                 //lq 40分频
	    RTC_SetPrescaler(RTC_LSI_PRE_VALUE - 1);                                                   //lq 800分频，频率为50

	    RtcCnt1s = RTC_ADJUST_1S_LSI;
        RtcCnt10s = RTC_ADJUST_10S_LSI;
        RtcCnt100s = RTC_ADJUST_100S_LSI;
    }
		
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Alarm in 5 second */
	RTC_SetAlarm(RTC_GetCounter() + 30);                                        //lq?
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();

	/* Enable the RTC Alarm interrupt */
	RTC_ITConfig(RTC_IT_ALR, ENABLE);
	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}		

#if 0 //lq
/*******************************************************************************
* 名称  : RTCAlarmInt
* 描述  : RTC中断服务程序，0.375K中断，相当于48K的128分频
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
//U8 gucTestRtc = 0;
void RTCAlarmInt(void)                                                          //lq 该中断每触发一次，相当于128个相位
{
	if(RTC_GetITStatus(RTC_IT_ALR) != RESET)
	{

		if (eqEquipPara.ucLowFreqSta == EQ_FREQSTA_LOW)                         //lq RTC时钟用于休眠状态下相位计数
		{

			Is_Feed_Dog = TRUE;//喂狗标志位
#if 1			
			
			RtcSubCnt1++;                                                       //lq RTC的"1s"计数
			RtcSubCnt2++;                                                       //lq RTC的"10s"计数
			RtcSubCnt3++;                                                       //lq RTC的"100s"计数

			if (IsRtcClockSourceLSE() == TRUE)
			{
				//LSE下的时隙计算                                               //lq 48000/512=93.75 0.75*4=3 （每4次计数3次）
				if (RTCIntCount < 3)//这个是补偿0.75
				{
					gsiPhaseCnt += 94;
					RTCIntCount++;
				}
				else if (RTCIntCount == 3)
				{
					gsiPhaseCnt += 93;
					RTCIntCount = 0;
				}
				else
				{
					RTCIntCount = 0;
				}
			}
			else
			{
				//LSI下的时隙计算
				gsiPhaseCnt += 96;//500HZ中断 每次加96                          //lq 1s含有48000个相位，48000/500=96
			}

#if 1                                                                           //lq 低频时RTC为512Hz
			if (RtcSubCnt1 >= RTC_ADJUST_1S)//1秒                                //lq 每RTC的1s对相位计数进行修正
			{
				RtcSubCnt1 = 0;
				gsiPhaseCnt -= RtcPhaseAdjust1; //低频模式中断频率一半，所以乘2
				//GpsTimCount -= 8;
			}

			if (RtcSubCnt2 >= RTC_ADJUST_10S)//10秒                             //lq 每Tim2的10s对相位计数进行修正
			{
				RtcSubCnt2 = 0;
				gsiPhaseCnt -= RtcPhaseAdjust10;
				//GpsTimCount -= 1;
			}

			if (RtcSubCnt3 >= RTC_ADJUST_100S)//100秒                           //lq 每Tim2的100s对相位计数进行修正
			{
				RtcSubCnt3 = 0;
				gsiPhaseCnt -= RtcPhaseAdjust100;
				//GpsTimCount -= 1;
			}
#endif
				
			//1280个相位为一个时隙
			if (gsiPhaseCnt >= 1280)
			{
				gsiPhaseCnt -= 1280;
	
				gsiSlotCnt++;
				//2250个时隙为一个帧，一帧为60s，为1min，此处分钟计数到永远
				if (gsiSlotCnt >= 2250)
				//if (gsiSlotCnt >= 225)
				{
					gsiSlotCnt = 0;
	
					gslFrameCnt++;
				}
			}
#endif
		}
		//高频部分
		else if (eqEquipPara.ucLowFreqSta == EQ_FREQSTA_HIG)                    //lq 用于RTC的调整
		{
			if (IsGpsPowerupProtect() == TRUE)
			{
				if (RtcHighCountFlag == TRUE)                                   //lq 工作状态，RTC为1024（or 1000）Hz（低频状态下的2倍），
				{                                                               //lq? 所以每2次中断计数1次 ？
					GpsRtcCount++;                                              //lq GPS定位状态下，RTC计数

					RtcHighCountFlag = FALSE;
				}
				else
				{
					RtcHighCountFlag = TRUE;
				}
			}
		}

		/* Clear EXTI line17 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line17);
		
		/* Check if the Wake-Up flag is set */
		if(PWR_GetFlagStatus(PWR_FLAG_WU) != RESET)
		{
			/* Clear Wake Up flag */
			PWR_ClearFlag(PWR_FLAG_WU);
		}		

		RTC_WaitForLastTask();
		RTC_EnterConfigMode(); 
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();   
		/* Clear RTC Alarm interrupt pending bit */
		RTC_ClearITPendingBit(RTC_IT_ALR);
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
		/* Alarm in 5 second */
		RTC_SetAlarm(RTC_GetCounter()+ 1);
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();		
		RTC_WaitForLastTask();
		RTC_ExitConfigMode();
		RTC_WaitForLastTask();	
	}
}
#endif

/*******************************************************************************
* 名称  : RTCAlarmInt
* 描述  : RTC中断服务程序，0.375K中断，相当于48K的128分频
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 gucTestRtc = TRUE;
void RTCAlarmInt(void)                                                          //lq 该中断每触发一次，相当于128个相位
{
	U32 TempRtcInteral = 0;
    S32 PhaseDiff = 0;
    S32 Temp1sCnt = 0;
    S32 Temp10sCnt = 0;
    S32 Temp100sCnt = 0;
    U32 tempDivider = 0;
    S32 slTemp1 = 0;

    if(RTC_GetITStatus(RTC_IT_ALR) != RESET)
	{
#if 0   //lqdb
        if (gucTestRtc == TRUE)
        {
            TEST_OUT_HIGH_1();
            gucTestRtc = FALSE;
        }
        else
        {
            TEST_OUT_LOW_1();//lq 测试
            gucTestRtc = TRUE;
        }
#endif
#if 0		
        RtcSubCnt1++;                                                           //lq RTC的"1s"计数
		RtcSubCnt2++;                                                           //lq RTC的"10s"计数
		RtcSubCnt3++;                                                           //lq RTC的"100s"计数
#endif
		if (DevDynamicPara.ucLowFreqSta == EQ_FREQSTA_LOW)                      //lq 休眠状态下，RTC时钟用于相位计数和校准
		{  
			Is_Feed_Dog = TRUE;//喂狗标志位                                     //lq  EQ_FREQSTA_HIG状态下，由1KHz的Tim喂狗；EQ_FREQSTA_LOW状态下，由RTC喂狗
#if 0
            /*lq 待机唤醒后的第一次定时中断*/
            if (ucStandbyWakeupDelayFlg == TRUE)
            {
                ucStandbyWakeupDelayFlg = FALSE;

                /*lq 读RTC_CNT前需等APB1与RTC时钟同步*/
            	RTC_WaitForSynchro();

                /*lq 计算进入待机前设定alarm定时的时刻至待机唤醒后RTC时钟中断开启的时间间隔*/
                TempRtcInteral = RTC_GetCounter() - arrBKPDRData[BKPR_RTC_ALARM_VALUE_INDEX];//lq

                /*lq 计算延时对应的相位数*/
                // PhaseDiff = (double)TempRtcInteral * 48000.0 / (double)arrBKPDRData[BKPR_RTC_FREQUENCE_INDEX] + 0.5;
                
                /*lq 计算延时对应的Tim计数的相位数*/
                PhaseDiff = TempRtcInteral * ((double)struRtcTimPhase1.RtcTimPhaseNum / RTC_TIM_PHASE_PRECISE) + 0.5;

                Temp1sCnt = PhaseDiff / 48000;
                Temp10sCnt = PhaseDiff / 480000;
                Temp100sCnt = PhaseDiff / 4800000;

                /*lq 对间隔相位数进行校准*/
                PhaseDiff -= TimAdjust1 * Temp1sCnt;
                PhaseDiff -= TimAdjust10 * Temp10sCnt;
                PhaseDiff -= TimAdjust100 * Temp100sCnt;
                
                PhaseDiff += gsiPhaseCnt;

                gsiPhaseCnt = PhaseDiff % 1280;
                gsiSlotCnt += PhaseDiff / 1280;
                if (gsiSlotCnt >= 2250)
                {
                    gslFrameCnt +=  gsiSlotCnt / 2250;
                    gsiSlotCnt = gsiSlotCnt % 2250;
                }
            }
#endif
#if 0
                if (SleepCtrlPara.ucStandbyAlarmSetFlg == TRUE)
                {
                    SleepCtrlPara.ucStandbyAlarmSetFlg = FALSE;
                                  
                    /*lq 计算待机休眠时长*/
                    struRtcPara.ssRtcAlarmValue = CalcuSleepSpan();
                    SleepCtrlPara.ucStandbyAlarmSetOk = TRUE;
                    
                }   
#endif
            if (SleepCtrlPara.ucTimToRtcSwitch == TRUE)
            {            
                /*lq 停止使用Tim进行相位计数*/
                SleepCtrlPara.ucTimToRtcSwitch = FALSE;
 
                //lq TEST_OUT_HIGH_2();                               

                /*lq 设定Standby唤醒定时*/
                if (SleepCtrlPara.ucStandbyAlarmSetFlg == TRUE)
                {
                    SleepCtrlPara.ucStandbyAlarmSetFlg = FALSE;
                                  
                    /*lq 计算待机休眠时长*/
                    struRtcPara.ssRtcAlarmValue = CalcuSleepSpan();
                    SleepCtrlPara.ucStandbyAlarmSetOk = TRUE;
                    
                }                
                //lq 由Tim转换为Rtc的第一次RTC中断RTC相位计数不应加94（93）                                   
            }
            else
            {
    #if 1
                /*lq 待机唤醒后的第一次定时中断*/
                if (ucStandbyWakeupDelayFlg == TRUE)
                {
                    ucStandbyWakeupDelayFlg = FALSE;
    
                    /*lq 读RTC_CNT前需等APB1与RTC时钟同步*/
                	RTC_WaitForSynchro();
    
                    /*lq 计算进入待机前设定alarm定时的时刻至待机唤醒后RTC时钟中断开启的时间间隔*/
#if 1                    
                    // TempRtcInteral = RTC_GetCounter() -1 - arrBKPDRData[BKPR_RTC_ALARM_VALUE_INDEX];//lq
                    TempRtcInteral = RTC_GetCounter() -1 - (((arrBKPDRData[BKPR_RTC_ALARM_VALUE_H_INDEX] << 16) & 0xFFFF0000) 
                                     | (arrBKPDRData[BKPR_RTC_ALARM_VALUE_L_INDEX] & 0xFFFF));
#else
                    tempCnt = RTC_GetCounter();
                    TempRtcInteral = tempCnt - arrBKPDRData[BKPR_RTC_ALARM_VALUE_INDEX];//lq
#endif    
                    /*lq 计算延时对应的相位数*/
                    // PhaseDiff = (double)TempRtcInteral * 48000.0 / (double)arrBKPDRData[BKPR_RTC_FREQUENCE_INDEX] + 0.5;
                    
                    /*lq 计算延时对应的Tim计数的相位数*/
                    PhaseDiff = TempRtcInteral * ((double)struRtcTimPhase1.RtcTimPhaseNum / RTC_TIM_PHASE_PRECISE) + 0.5;
    
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
                }
                else
                {
            #if 1		
                    RtcSubCnt1++;                                                           //lq RTC的"1s"计数
            		RtcSubCnt2++;                                                           //lq RTC的"10s"计数
            		RtcSubCnt3++;                                                           //lq RTC的"100s"计数
            #endif
        			if (IsRtcClockSourceLSE() == TRUE)
        			{
#if 1
        				/*lq LSE下的时隙计算，对应RTC频率512，alarm频率512*/                                          
        				if (RTCIntCount < 3)                                            //lq 48000/512=93.75 0.75*4=3 （每4次计数3次）
        				{
        					gsiPhaseCnt += 94;
        					RTCIntCount++;
        				}
        				else if (RTCIntCount == 3)
        				{
        					gsiPhaseCnt += 93;
        					RTCIntCount = 0;
        				}
        				else
        				{
        					RTCIntCount = 0;
        				}
#else
                        if (RTCIntCount < 15)
                        {
                            if (RTCIntCount % 2 == 0)
                            {
        					    gsiPhaseCnt += 23;
                            }
                            else
                            {
        					    gsiPhaseCnt += 24;                            
                            } 
                            RTCIntCount++;                       
                        }
                        else if (RTCIntCount == 15)
                        {
        					gsiPhaseCnt += 23;
        					RTCIntCount = 0;                        
                        }
                        else
                        {
        					RTCIntCount = 0;
                        }
#endif                                                               
        			}
        			else
        			{
        				/*lq LSI下的时隙计算，对应RTC频率500，alarm频率500*/                                          
        				gsiPhaseCnt += 96;                                              //lq 1s含有48000个相位，48000/500=96
                	}
    #if 0
        			if (RtcSubCnt1 >= RtcCnt1s)                                         //lq 每UTC的1s对相位计数进行修正
        			{
        				RtcSubCnt1 = 0;
        				gsiPhaseCnt -= RtcPhaseAdjust1; 
        			}
        
        			if (RtcSubCnt2 >= RtcCnt10s)                                        //lq 每UTC的10s对相位计数进行修正
        			{
        				RtcSubCnt2 = 0;
        				gsiPhaseCnt -= RtcPhaseAdjust10;
        			}
        
        			if (RtcSubCnt3 >= RtcCnt100s)                                       //lq 每UTC的100s对相位计数进行修正
        			{
        				RtcSubCnt3 = 0;
        				gsiPhaseCnt -= RtcPhaseAdjust100;
        			}
    #else
                    RtcTimPhaseCnt1 += struRtcTimPhase1.lfRtcTimPhaseNum;
                    if (RtcTimPhaseCnt1 >= TimCnt1s)
                    {
                        RtcTimPhaseCnt1 = 0;
                        gsiPhaseCnt -= TimAdjust1;                                      
                    }
                    
                    RtcTimPhaseCnt10 += struRtcTimPhase1.lfRtcTimPhaseNum;
                    if (RtcTimPhaseCnt10 >= TimCnt10s)
                    {
                        RtcTimPhaseCnt10 = 0;
                        gsiPhaseCnt -= TimAdjust10;                                      
                    }
                                    
                    RtcTimPhaseCnt100 += struRtcTimPhase1.lfRtcTimPhaseNum;
                    if (RtcTimPhaseCnt100 >= TimCnt100s)
                    {
                        RtcTimPhaseCnt100 = 0;
                        gsiPhaseCnt -= TimAdjust100;                                      
                    }
    
    #endif     
        			/*lq 相位、时隙和帧的换算，1280个相位为1个时隙，2250个时隙为1帧（1分钟）*/
        			if (gsiPhaseCnt >= 1280)
        			{
        				gsiSlotCnt += gsiPhaseCnt / 1280;
                        gsiPhaseCnt %= 1280;
        				if (gsiSlotCnt >= 2250)                                         //2250个时隙为一个帧，一帧为60s，为1min，此处分钟计数到永远
        				{
        					gslFrameCnt += gsiSlotCnt / 2250; 
                            gsiSlotCnt %= 2250;	
        				}
        			}
                    else if (gsiPhaseCnt < 0)
                    {
                        slTemp1 = gsiPhaseCnt / 1280;
                        slTemp1--;
                        gsiPhaseCnt += -slTemp1 * 1280;                         //lq 借位
                        gsiSlotCnt += slTemp1;
                        if (gsiSlotCnt < 0)
                        {
                            slTemp1 = gsiSlotCnt / 2250;
                            slTemp1--;                                                              
                            gsiSlotCnt += -slTemp1 * 2250;                      //lq 借位
                            gslFrameCnt += slTemp1;
                            if (gslFrameCnt < 0)
                            {
                                gslFrameCnt = 0;
                                gsiSlotCnt = 0;
                                gsiPhaseCnt = 0;
                            }
                        }
                    }                
                }
    #endif

            }
       
		}
		else if (DevDynamicPara.ucLowFreqSta == EQ_FREQSTA_HIG)                    //lq 工作状态下，为RTC时钟校准做准备（工作状态下的相位计数和校准由Tim完成）
		{
#if 1
            /*lq 进入HighFreq状态后的RTC第一次中断触发前继续使用RTC计数，不使用Tim相位计数*/
            if (SleepCtrlPara.ucRtcToTimSwitch == TRUE)
            {    			
                SleepCtrlPara.ucRtcToTimSwitch = FALSE;     
#if 1
                gslRtcToTimCnt_2++;

#endif
        #if 1		
                RtcSubCnt1++;                                                           //lq RTC的"1s"计数
        		RtcSubCnt2++;                                                           //lq RTC的"10s"计数
        		RtcSubCnt3++;                                                           //lq RTC的"100s"计数
        #endif
                if (IsRtcClockSourceLSE() == TRUE)
    			{
#if 1
    				/*lq LSE下的时隙计算，对应RTC频率512，alarm频率512*/                                          
    				if (RTCIntCount < 3)                                            //lq 48000/512=93.75 0.75*4=3 （每4次计数3次）
    				{
    					gsiPhaseCnt += 94;
    					RTCIntCount++;
    				}
    				else if (RTCIntCount == 3)
    				{
    					gsiPhaseCnt += 93;
    					RTCIntCount = 0;
    				}
    				else
    				{
    					RTCIntCount = 0;
    				}
#else
                    if (RTCIntCount < 15)
                    {
                        if (RTCIntCount % 2 == 0)
                        {
    					    gsiPhaseCnt += 23;
                        }
                        else
                        {
    					    gsiPhaseCnt += 24;                            
                        } 
                        RTCIntCount++;                       
                    }
                    else if (RTCIntCount == 15)
                    {
    					gsiPhaseCnt += 23;
    					RTCIntCount = 0;                        
                    }
                    else
                    {
    					RTCIntCount = 0;
                    }
#endif                                                                
    			}
    			else
    			{
    				/*lq LSI下的时隙计算，对应RTC频率500，alarm频率500*/                                          
    				gsiPhaseCnt += 96;                                              //lq 1s含有48000个相位，48000/500=96
            	}
#if 1
#if 0
    			if (RtcSubCnt1 >= RtcCnt1s)                                         //lq 每UTC的1s对相位计数进行修正
    			{
    				RtcSubCnt1 = 0;
    				gsiPhaseCnt -= RtcPhaseAdjust1; 
    			}
    
    			if (RtcSubCnt2 >= RtcCnt10s)                                        //lq 每UTC的10s对相位计数进行修正
    			{
    				RtcSubCnt2 = 0;
    				gsiPhaseCnt -= RtcPhaseAdjust10;
    			}
    
    			if (RtcSubCnt3 >= RtcCnt100s)                                       //lq 每UTC的100s对相位计数进行修正
    			{
    				RtcSubCnt3 = 0;
    				gsiPhaseCnt -= RtcPhaseAdjust100;
    			}
#else
                RtcTimPhaseCnt1 += struRtcTimPhase1.lfRtcTimPhaseNum;
                if (RtcTimPhaseCnt1 >= TimCnt1s)
                {
                    RtcTimPhaseCnt1 = 0;
                    gsiPhaseCnt -= TimAdjust1;                                      
                }
                
                RtcTimPhaseCnt10 += struRtcTimPhase1.lfRtcTimPhaseNum;
                if (RtcTimPhaseCnt10 >= TimCnt10s)
                {
                    RtcTimPhaseCnt10 = 0;
                    gsiPhaseCnt -= TimAdjust10;                                      
                }
                                
                RtcTimPhaseCnt100 += struRtcTimPhase1.lfRtcTimPhaseNum;
                if (RtcTimPhaseCnt100 >= TimCnt100s)
                {
                    RtcTimPhaseCnt100 = 0;
                    gsiPhaseCnt -= TimAdjust100;                                      
                }
#endif    
    			/*lq 相位、时隙和帧的换算，1280个相位为1个时隙，2250个时隙为1帧（1分钟）*/
    			if (gsiPhaseCnt >= 1280)
    			{
    				gsiSlotCnt += gsiPhaseCnt / 1280;
                    gsiPhaseCnt %= 1280;
    				if (gsiSlotCnt >= 2250)                                         //2250个时隙为一个帧，一帧为60s，为1min，此处分钟计数到永远
    				{
    					gslFrameCnt += gsiSlotCnt / 2250; 
                        gsiSlotCnt %= 2250;	
    				}
    			}
                else if (gsiPhaseCnt < 0)
                {
                    slTemp1 = gsiPhaseCnt / 1280;
                    slTemp1--;
                    gsiPhaseCnt += -slTemp1 * 1280;                         //lq 借位
                    gsiSlotCnt += slTemp1;
                    if (gsiSlotCnt < 0)
                    {
                        slTemp1 = gsiSlotCnt / 2250;
                        slTemp1--;                                                              
                        gsiSlotCnt += -slTemp1 * 2250;                      //lq 借位
                        gslFrameCnt += slTemp1;
                        if (gslFrameCnt < 0)
                        {
                            gslFrameCnt = 0;
                            gsiSlotCnt = 0;
                            gsiPhaseCnt = 0;
                        }
                    }
                }
#endif       
            }
#endif
        	/*lq GPS5min定位保护时，进行RTC时钟偏差的统计计算*/
            if (IsGpsPowerupProtect() == TRUE)
			{
				RtcCnt++;                                                       //lq 本地Rtc计数（未校准）
                GpsRtcCount++;                                                  //lq GPS定位状态下，RTC计数（校准）
#if 0
                /*lq 每个RTC周期保存Tim当前的相位计数*/
                RtcTimCountSave = GpsTimCount;
                RtcTimSaveFlag = TRUE;
#endif                
                if (struRtcTimPhase1.RtcTimPhaseFlg == FALSE)
                {
                    if (struRtcTimPhase1.RtcTimPhasePeriodNum == 0)
                    {
                        /*lq 第一次计数*/
                        struRtcTimPhase1.RtcTimPhaseCntLast = GpsTimCount;
                        struRtcTimPhase1.RtcTimPhasePeriodNum++;
                    }
                    else
                    {
                        struRtcTimPhase1.RtcTimPhaseCntNow = GpsTimCount;
                        struRtcTimPhase1.RtcTimPhaseNumTemp += (struRtcTimPhase1.RtcTimPhaseCntNow  - struRtcTimPhase1.RtcTimPhaseCntLast
                                                                 - struRtcTimPhase1.RtcTimPhaseNumTemp) / (double)struRtcTimPhase1.RtcTimPhasePeriodNum;
                        
                        struRtcTimPhase1.RtcTimPhaseCntLast = struRtcTimPhase1.RtcTimPhaseCntNow;

                        if (struRtcTimPhase1.RtcTimPhasePeriodNum >= GPS_CTRL_TIME_FIX_LONG_OPEN * 60 * 500)
                        {
                            struRtcTimPhase1.RtcTimPhaseNum = (U32)(struRtcTimPhase1.RtcTimPhaseNumTemp * RTC_TIM_PHASE_PRECISE);
                            struRtcTimPhase1.lfRtcTimPhaseNum = (double)struRtcTimPhase1.RtcTimPhaseNum / RTC_TIM_PHASE_PRECISE; 
                            
                            struRtcTimPhase1.RtcTimPhaseFlg = TRUE;
                        }
                        struRtcTimPhase1.RtcTimPhasePeriodNum++;
                    }                   
                }                   
            
            }
		}
		
        /* Clear EXTI line17 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line17);
		
		/* Check if the Wake-Up flag is set */
		if(PWR_GetFlagStatus(PWR_FLAG_WU) != RESET)
		{
			/* Clear Wake Up flag */
			PWR_ClearFlag(PWR_FLAG_WU);
		}		

		RTC_WaitForLastTask();
		RTC_EnterConfigMode(); 
		
        /* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();   
		
        /* Clear RTC Alarm interrupt pending bit */
		RTC_ClearITPendingBit(RTC_IT_ALR);
		
        /* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();                                                      //lq 用时约600ns
		
        if (SleepCtrlPara.ucStandbyAlarmSetOk == TRUE) 
        {
        	SleepCtrlPara.ucStandbyAlarmSetOk = FALSE;
            RTC_SetCounter(0);                                                      //重新开始RTC计数	
#if 0        	
            RTC_WaitForLastTask();
            RTC_WaitForSynchro();                                                   //lq 解决工作状态和休眠状态下RTC频率不一致的问题
            tempDivider = RTC_GetDivider();
#endif
        	RTC_WaitForLastTask();
        	RTC_SetAlarm(struRtcPara.ssRtcAlarmValue);           //lq 待机定时 0至（struRtcPara.ssRtcAlarmValue - 1）共struRtcPara.ssRtcAlarmValue个512频率周期
        }
        else
        {
        	/* Alarm in 5 second */
            RTC_WaitForSynchro();                                                   //lq 解决工作状态和休眠状态下RTC频率不一致的问题
                                                                                    //lq 用时30us           
            
            RTC_SetAlarm(RTC_GetCounter() + RTC_ALARM_INTERVAL_VALUE - 1);          //lq RTC_ALARM_INTERVAL_VALUE个周期定时
        }      
		
        /* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();		
		RTC_WaitForLastTask();
		RTC_ExitConfigMode();
		RTC_WaitForLastTask();	
    }
}



/*******************************************************************************
* 名称  : InitRtcParaStruct
* 描述  : 初始化RtcPara结构体参数
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitRtcParaStruct(RtcPara *pRtcPara)
{
    pRtcPara->usRtcFreq = RTC_LSE_FREQ_VALUE;          
    pRtcPara->ucRtcLSEorLSI = RTC_CLK_SOURCE_LSE;
    pRtcPara->ucRtcAlarmMcuOrGps = RTC_ALARM_MCU;
    pRtcPara->ssRtcAlarmValue = 0;
}

/*******************************************************************************
* 名称  : InitRtcTimPhaseStruct
* 描述  : 结构体初始化
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitRtcTimPhaseStruct(void)
{
    struRtcTimPhase1.RtcTimPhaseFlg = FALSE;
    struRtcTimPhase1.RtcTimPhasePeriodNum = 0;
    struRtcTimPhase1.RtcTimPhaseCntLast = 0;
    struRtcTimPhase1.RtcTimPhaseCntNow = 0;
    struRtcTimPhase1.RtcTimPhaseNumTemp = 0;
    struRtcTimPhase1.RtcTimPhaseNum = RTC_TIM_PHASE_NUM_DEFAULT;
    struRtcTimPhase1.lfRtcTimPhaseNum = (double)struRtcTimPhase1.RtcTimPhaseNum / RTC_TIM_PHASE_PRECISE; 
}

/*******************************************************************************
* Function Name  : RTC_SelfCheck
* Description    : RTC自检
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_SelfCheck(RtcSelfCheckStruct *pRtcSelfCheckStruct)
{
    if (pRtcSelfCheckStruct->StartFlg == TRUE)
    {
        if (TimAdjustPara.RtcClockChoice == TRUE)
        {
            if (TimAdjustPara.RtcClockSource == ADJUST_RTC_SOURCE_LSE)
            {
                pRtcSelfCheckStruct->RtcLseOkFlg = TRUE;        
            }
            else
            {
                pRtcSelfCheckStruct->RtcLseOkFlg = FALSE;                
            }
            
            pRtcSelfCheckStruct->CompleteFlg = TRUE;
            pRtcSelfCheckStruct->StartFlg = FALSE;
        }            
    }
}

/*******************************************************************************
* 名称  : EEPROM_EepromSelfCheckStructInit
* 描述  : 初始化结构体
* 输入  : pRtcSelfCheckStruct - RtcSelfCheckStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void RTC_RtcSelfCheckStructInit(RtcSelfCheckStruct *pRtcSelfCheckStruct)
{
    pRtcSelfCheckStruct->StartFlg = TRUE;
    pRtcSelfCheckStruct->CompleteFlg = FALSE;
    pRtcSelfCheckStruct->RtcLseOkFlg = FALSE;      
} 
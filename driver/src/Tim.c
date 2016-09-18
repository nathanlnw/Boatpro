/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: Tim.c
  Author: vozhj      Version : V1.00        Date: 2011.10.18
  Description:    硬件定时器
  Version:         V1.00
  Function List:   // 主要函数及其功能
  History:         // 历史修改记录
      <author>  <time>   	<version >   <desc>

*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "def.h"
#include "Tim.h"
#include "Adc.h"
#include "systemtick.h"
#include "wdg.h"
#include "gmskmod.h"
#include "LinkPtlReport.h"
#include "initial.h"
#include "digunit.h"
#include "Dac.h"
#include "power.h"
#include "gpsctrl.h"
#include "LED.h"
#include "rtc.h"
#include "testing.h"
#include "SOTDMA.h"
#include "ledctrl.h"
#include "adjust.h"
#include "sleepctrl.h"
#include "config.h"
#include "SPI.h"
#include "gpiospi.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
U16 CCR1_Val;
U16 CCR2_Val;
U16 CCR3_Val;
S16 gsiPhaseCnt;                                                                //lq  gsiPhaseCnt、gsiSlotCnt、gslFrameCnt                                                                               
S16 gsiSlotCnt;                                                                 //lq  三者仅用于消息发送的时间控制，
S32 gslFrameCnt;                                                                //lq  其他的定时使用本地5ms定时器。

U32 TimSubCnt1;//定时器误差调整计数
U32 TimSubCnt2;//定时器误差调整计数
U32 TimSubCnt3;//定时器误差调整计数

S16 TimAdjust1;//定时器每2秒调整phase数
S16 TimAdjust10;//定时器每10秒调整phase数
S16 TimAdjust100;//定时器每50秒调整phase数

U32 GpsTimCount;//用于TIM2时间调整的计数器                                      //lq 本地Tim计数（根据与UTC的偏差值校准）
U32 TimCnt;                                                                     //lq 本地Tim计数（未校准）                                                                
U32 TimCnt1s;                                                                   //lq 本地Tim在UTC1s间隔内的计数值
U32 TimCnt10s;                                                                  //lq 本地Tim在UTC10s间隔内的计数值
U32 TimCnt100s;                                                                 //lq 本地Tim在UTC100s间隔内的计数值

U8  gucFSKDataXmtEn = FALSE;
U8  gucFetchFirstFskDataEn = FALSE;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

U16 capture = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/ 
/*******************************************************************************
* 名称  : SetTimAdjust1
* 描述  : 设置定时器每1秒调整phase数
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetTimAdjust1(S16 data)
{
//lq	TimAdjust1 = TimAdjust1 + data;//在原来的基础上调整                         //lq 调整值累加？
	TimAdjust1 = data;                                                          //lq 不应累加，因为每次统计阶段用于统计时钟数的GpsTimCount均是按定时器Tim的设定频率计数的，
                                                                                //lq 后来用于计数的GpsTimCount并没有使用之前计算得到的校准值进行校准
}
/*******************************************************************************
* 名称  : SetTimAdjust10
* 描述  : 设置定时器每10秒调整phase数
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetTimAdjust10(S16 data)
{
//lq	TimAdjust10 = TimAdjust10 + data;//在原来的基础上调整
    TimAdjust10 = data;
}
/*******************************************************************************
* 名称  : SetTimAdjust100
* 描述  : 设置定时器每100秒调整phase数
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetTimAdjust100(S16 data)
{
//lq	TimAdjust100 = TimAdjust100 + data;//在原来的基础上调整
	TimAdjust100 = data;//在原来的基础上调整
}

/*******************************************************************************
* 名称  : CalcTimPhaseAdjust
* 描述  : 计算Tim的时间调整
* 输入  : DiffNum 4096误差周期数  PeriodCount 统计周期
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CalcTimPhaseAdjust(S32 DiffNum ,U16 PeriodCount)
{
    S16 TimAdjust1_temp;
    S16 TimAdjust10_temp;
    S16 TimAdjust100_temp;

	TimAdjust1_temp = DiffNum / PeriodCount;
	TimAdjust10_temp = (DiffNum * 10 - TimAdjust1_temp * PeriodCount * 10) / PeriodCount;
	TimAdjust100_temp = (DiffNum * 100 - TimAdjust1_temp * PeriodCount * 100 
                         - (TimAdjust10_temp * PeriodCount) * 10) / PeriodCount;

	usart_printf("TimAdjust1_temp  : %d\r\n", TimAdjust1_temp);				
	usart_printf("TimAdjust10_temp : %d\r\n", TimAdjust10_temp);				
	usart_printf("TimAdjust100_temp: %d\r\n", TimAdjust100_temp);	

    /*lq 将Tim的时间调整值转换为最短时间调整值*/
    TransPhaseAdjust(&TimAdjust1_temp, &TimAdjust10_temp, &TimAdjust100_temp);

	if (TimAdjust1_temp < ADJUST_PHASE_MAX_ADJUST 
        && TimAdjust1_temp > -ADJUST_PHASE_MAX_ADJUST)                          //lq RtcPhaseAdjust10_Temp,RtcPhaseAdjust100_Temp取值范围[0,9]
	{
		TimAdjust1 = TimAdjust1_temp;
		TimAdjust10 = TimAdjust10_temp;
		TimAdjust100 = TimAdjust100_temp;
	}
}

/*******************************************************************************
* 名称  : TransPhaseAdjust
* 描述  : 将Tim的时间调整值转换为最短时间调整值
* 输入  : pAdjust1:1s校准值
          pAdjust10:10s校准值
          pAdjust100:100s校准值
* 输出  : 转换后的校准值
* 返回  : 无
*******************************************************************************/
void TransPhaseAdjust(S16 * const pAdjust1, S16 * const pAdjust10, S16 * const pAdjust100)
{
    /*lq 100s校准值小于-4s*/
    if (*pAdjust100 < -4)
    {
        *pAdjust100 = 10 + *pAdjust100;
        (*pAdjust10)--;
    }
    else if (*pAdjust100 > 4)
    {
        *pAdjust100 = -10 + *pAdjust100;
        (*pAdjust10)++;
    } 

    /*lq 10s校准值小于-4s*/
    if (*pAdjust10 < -4)
    {
        *pAdjust10 = 10 + *pAdjust10;
        (*pAdjust1)--;
    }
    else if (*pAdjust10 > 4)
    {
        *pAdjust10 = -10 + *pAdjust10;
        (*pAdjust1)++;        
    }            
}
 /*******************************************************************************
* 名称  : InitTim2Para
* 描述  : 初始化定时器2相关参数
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitTim2Para(void)
{
	CCR1_Val = 125; // 48 k
	CCR2_Val = 6000; // 1k
    CCR3_Val = 60; //100k
//	CCR4_Val = 4000;
	
	gsiPhaseCnt = 0;
	gsiSlotCnt = 0;
	gslFrameCnt = 0;
	TimSubCnt1 = 0;//TIM2定时调整计数
	TimSubCnt2 = 0;
	TimSubCnt3 = 0;
	TimAdjust1 = 0;
	TimAdjust10 = 0;
	TimAdjust100 = 0;
	GpsTimCount = 0;

    TimCnt = 0;
    TimCnt1s = TIM_ADJUST_1S;
    TimCnt10s = TIM_ADJUST_10S;
    TimCnt100s = TIM_ADJUST_100S;
}

/*******************************************************************************
* 名称  : InitTim2Para_2
* 描述  : 初始化定时器2相关参数
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitTim2Para_2(void)
{
	TimSubCnt1 = 0;//TIM2定时调整计数
	TimSubCnt2 = 0;
	TimSubCnt3 = 0;
}

/*******************************************************************************
* 名称  : InitTim2
* 描述  : 初始化定时器2
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitTim2(void)
{	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	InitTim2Para();

	/* ---------------------------------------------------------------
	TIM2 Configuration: Output Compare Timing Mode:
	TIM2CLK = 2*PCLK1 = 2*PLLCLK/2 = 72MHz, 
	Prescaler = (11)+1, TIM2 counter clock = 72/12 = 6 MHz                      //lq TIM2时钟配置见Clock_Configuration()
	--------------------------------------------------------------- */	
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 65535;                                   //lq TIM_Period为寄存器ARR的值？
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);	
	/* Prescaler configuration */
	TIM_PrescalerConfig(TIM2, 11, TIM_PSCReloadMode_Immediate);                 //lq 12分频后72MHz/12=6MHz


	/* ---------------------------------------------------------------
	CC1 update rate = TIM2 counter clock / CCR1_Val = 48KHz                 
	--------------------------------------------------------------- */	
	/* Output Compare Timing Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;                                   //lq CCR1_Val=125 6MHz/125=48k
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);	
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);

	/* ---------------------------------------------------------------
	CC1 update rate = TIM2 counter clock / CCR2_Val = 1KHz
	--------------------------------------------------------------- */
	/* Output Compare Timing Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR2_Val;	
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);	
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);

	/* ---------------------------------------------------------------
	CC1 update rate = TIM2 counter clock / CCR3_Val = 1KHz
	--------------------------------------------------------------- */	
	/* Output Compare Timing Mode configuration: Channel3 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR3_Val;	
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);	
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Disable);

#if 0	
	/* ---------------------------------------------------------------
	CC1 update rate = TIM2 counter clock / CCR4_Val = 
	--------------------------------------------------------------- */
	/* Output Compare Timing Mode configuration: Channel4 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR4_Val;	
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);	
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Disable);
#endif	
	/* TIM IT enable */
	TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_CC3, ENABLE);
	
	/* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);	
}

/*******************************************************************************
* 名称  : Tim2Interrupt
* 描述  : 定时器2中断服务程序
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void Tim2Interrupt(void)
{
    S32 slTemp1 = 0;

	/*lq 48KHZ定时器 */
    if(TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)                              //lq 1/48k 秒即为1个相位 
	{                                                                           //lq 按Tim2计时的一个相位（不是标准的一个相位）
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
		
		if (DevDynamicPara.ucLowFreqSta == EQ_FREQSTA_HIG)                      //zjw 正常工作状态   //lq Tim2时钟用于工作状态下相位计数
		{                                                                       //lq Tim2在EQ_FREQSTA_HIG下一直开启，
                                                                                //lq 在EQ_FREQSTA_LOW下间歇性的开启关闭
            /*lq 进入HighFreq状态后的RTC第一次中断触发前继续使用RTC计数，不使用Tim相位计数*/
            if (SleepCtrlPara.ucRtcToTimSwitch != TRUE)			
            {
                //48K中断控制
    			TimSubCnt1++;                                                    //lq 用于"1s"计时
    			TimSubCnt2++;                                                   //lq 用于"10s"计时
    			TimSubCnt3++;                                                   //lq 用于"100s"计时
    			
    			if (IsGpsPowerupProtect() == TRUE)                              //lq 开机定位保护
    			{
    				GpsTimCount++;                                              //lq 开机保护状态下Tim相位计数
                    TimCnt++;			
                }
    
    			gsiPhaseCnt++;	                                                //lq 相位计数（未修正前）
    						
#if 1           /*lq 时钟校准，主要用于GPS未定位时*/
    			if (TimSubCnt1 >= TimCnt1s)                                     //zjw Tim的1秒48k //lq 误差调整（只要Tim在工作，该调整就进行）
    			{                                                               //lq 每Tim2的1s对相位计数进行修正
    				TimSubCnt1 = 0;
                    if (GpsSynCell.ucGPSSyn_SlotPhaseSynFlag == FALSE)
                    {
                        gsiPhaseCnt -= TimAdjust1;                              //zjw Tim定时器每2秒调整phase数         
                    }
    			}
    
    			if (TimSubCnt2 >= TimCnt10s)                                    //zjw Tim的10秒480k //lq 误差调整（只要Tim在工作，该调整就进行）
    			{                                                               //lq 每Tim2的10s对相位计数进行修正
    				TimSubCnt2 = 0;
                    if (GpsSynCell.ucGPSSyn_SlotPhaseSynFlag == FALSE)
                    {
                        gsiPhaseCnt -= TimAdjust10;                            //zjw Tim2定时器每10秒调整phase数          
                    }			
                }
    						
    			if (TimSubCnt3 >= TimCnt100s)                                   //zjw Tim的100秒4800k //lq 误差调整（只要Tim在工作，该调整就进行）
    			{                                                               //lq 每Tim2的100s对相位计数进行修正
    				TimSubCnt3 = 0;
                    if (GpsSynCell.ucGPSSyn_SlotPhaseSynFlag == FALSE)			//zjw Tim2定时器每50秒调整phase数
                    {
                        gsiPhaseCnt -= TimAdjust100;                                      
                    }			
    			}
#endif    
    			/*lq 相位、时隙和帧的换算，1280个相位为1个时隙，2250个时隙为1帧（1分钟）*/
    			if (gsiPhaseCnt >= 1280)
    			{
    				gsiSlotCnt += gsiPhaseCnt / 1280;
                    gsiPhaseCnt %= 1280;
    				if (gsiSlotCnt >= 2250)                                     //2250个时隙为一个帧，一帧为60s，为1min，此处分钟计数到永远
    				{
    					gslFrameCnt += gsiSlotCnt / 2250; 
                        gsiSlotCnt %= 2250;	
    				}
    			}
                else if (gsiPhaseCnt < 0)
                {
                    slTemp1 = gsiPhaseCnt / 1280;
                    slTemp1--;
                    gsiPhaseCnt += -slTemp1 * 1280;                             //lq 借位
                    gsiSlotCnt += slTemp1;
                    if (gsiSlotCnt < 0)
                    {
                        slTemp1 = gsiSlotCnt / 2250;
                        slTemp1--;                                                              
                        gsiSlotCnt += -slTemp1 * 2250;                          //lq 借位
                        gslFrameCnt += slTemp1;
                        if (gslFrameCnt < 0)
                        {
                            gslFrameCnt = 0;
                            gsiSlotCnt = 0;
                            gsiPhaseCnt = 0;
                        }
                    }
                }
    
                /*lq 时钟校准完成后，再进行GPS同步，用于GPS已定位时 */
#if 1            
                /*lq GPS时隙相位同步调整*/
                if (GpsSynCell.ucGPSSyn_SlotPhaseSynFlag == TRUE)
                {
        			GpsSynCell.ucGPSSyn_SlotPhaseSynFlag = FALSE;               //lq 每个秒脉冲调整一次（无秒脉冲时不调整）
#if CONFIG_GPS_SYNCH_HOUR_MIN													//zjw 没有进行，此处已经屏蔽了
                    /*lq 小时分钟同步*/                        				
                    if (GpsSynCell.GPSSyn_HourFrameSynEnable == TRUE)
                    {
                        if (GpsSynCell.ucGPSSyn_HourFrameSynFlag == TRUE)
                        {
                            GpsSynCell.ucGPSSyn_HourFrameSynFlag = FALSE;
                            GpsSynCell.GPSSyn_HourFrameSynEnable = FALSE;       //lq 每次开机后，小时分钟只同步一次
                            gslFrameCnt = GpsSynCell.ChkTimeFrame;   
                        }
                    }
#endif
                    /*lq 时隙同步调整*/
                    gsiSlotCnt += GpsSynCell.GPSSyn_SlotOffset;
    				if (gsiSlotCnt >= 2250)
    				{
    					gslFrameCnt += gsiSlotCnt / 2250;
                        gsiSlotCnt %= 2250;
    				}
    				else if (gsiSlotCnt < 0)
    				{
                        slTemp1 = gsiSlotCnt / 2250;
                        slTemp1--;                                                              
                        gsiSlotCnt += -slTemp1 * 2250;                          //lq 借位
                        gslFrameCnt += slTemp1;
                        if (gslFrameCnt < 0)
                        {
                            gslFrameCnt = 0;
                            gsiSlotCnt = 0;
                            gsiPhaseCnt = 0;
                        }
                    }	
                   
    				/*lq 相位同步调整*/
                    gsiPhaseCnt += GpsSynCell.GPSSyn_PhaseOffset;
        			if (gsiPhaseCnt >= 1280)
        			{
        				gsiSlotCnt += gsiPhaseCnt / 1280;
                        gsiPhaseCnt %= 1280;
        				if (gsiSlotCnt >= 2250)                                 //2250个时隙为一个帧，一帧为60s，为1min，此处分钟计数到永远
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
                
                	TimSubCnt1 = 0;                                              //TIM2定时调整计数
                	TimSubCnt2 = 0;
                	TimSubCnt3 = 0;
                }
#endif
            }        
		}
        else																	//zjw 休眠状态
        {
            /*lq 进入LowFreq状态后还未待机时，在RTC中断触发前继续使用Tim相位计数*/
            if (SleepCtrlPara.ucTimToRtcSwitch == TRUE)
            {
#if 1                
                gslTimToRtcCnt_2++;
#endif    			
                //48K中断控制
    			TimSubCnt1++;                                                    //lq 用于"1s"计时
    			TimSubCnt2++;                                                   //lq 用于"10s"计时
    			TimSubCnt3++;                                                   //lq 用于"100s"计时
    			
    			gsiPhaseCnt++;	                                                //lq 相位计数（未修正前）
    						
#if 1           /*lq 时钟校准，主要用于GPS未定位时*/
    			if (TimSubCnt1 >= TimCnt1s)                                     //zjw Tim的1秒48k //lq 误差调整（只要Tim在工作，该调整就进行）
    			{                                                               //lq 每Tim2的1s对相位计数进行修正
    				TimSubCnt1 = 0;
                    if (GpsSynCell.ucGPSSyn_SlotPhaseSynFlag == FALSE)
                    {
                        gsiPhaseCnt -= TimAdjust1;                              //zjw Tim定时器每2秒调整phase数        
                    }					
    			}
    
    			if (TimSubCnt2 >= TimCnt10s)                                    //zjw Tim的10秒480k //lq 误差调整（只要Tim在工作，该调整就进行）
    			{                                                               //lq 每Tim2的10s对相位计数进行修正
    				TimSubCnt2 = 0;
                    if (GpsSynCell.ucGPSSyn_SlotPhaseSynFlag == FALSE)
                    {
                        gsiPhaseCnt -= TimAdjust10;                             //zjw Tim定时器每102秒调整phase数         
                    }			
                }
    						
    			if (TimSubCnt3 >= TimCnt100s)                                   //zjw Tim的100秒4800k //lq 误差调整（只要Tim在工作，该调整就进行）
    			{                                                               //lq 每Tim2的100s对相位计数进行修正
    				TimSubCnt3 = 0;
                    if (GpsSynCell.ucGPSSyn_SlotPhaseSynFlag == FALSE)
                    {
                        gsiPhaseCnt -= TimAdjust100;                            //zjw Tim定时器每50秒调整phase数          
                    }		
    			}
#endif
    
    			/*lq 相位、时隙和帧的换算，1280个相位为1个时隙，2250个时隙为1帧（1分钟）*/
    			if (gsiPhaseCnt >= 1280)
    			{
    				gsiSlotCnt += gsiPhaseCnt / 1280;
                    gsiPhaseCnt %= 1280;
    				if (gsiSlotCnt >= 2250)                                     //2250个时隙为一个帧，一帧为60s，为1min，此处分钟计数到永远
    				{
    					gslFrameCnt += gsiSlotCnt / 2250; 
                        gsiSlotCnt %= 2250;	
    				}
    			}
                else if (gsiPhaseCnt < 0)
                {
                    slTemp1 = gsiPhaseCnt / 1280;
                    slTemp1--;
                    gsiPhaseCnt += -slTemp1 * 1280;                             //lq 借位
                    gsiSlotCnt += slTemp1;
                    if (gsiSlotCnt < 0)
                    {
                        slTemp1 = gsiSlotCnt / 2250;
                        slTemp1--;                                                              
                        gsiSlotCnt += -slTemp1 * 2250;                          //lq 借位
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
            else
            {
                gslTimToRtcIdleCnt_2++;
            }                    
        }
	
		capture = TIM_GetCapture1(TIM2);
		TIM_SetCompare1(TIM2, capture + CCR1_Val);		  		
	}

	/*lq 1KHZ定时器 */
    else if(TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)	                        //lq 1ms定时，每1ms触发一次
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);

		if (DevDynamicPara.ucLowFreqSta == EQ_FREQSTA_HIG)						//zjw 正常工作状态
		{
			Is_Feed_Dog = TRUE;                                                 //lq  EQ_FREQSTA_HIG状态下，由1KHz的Tim喂狗；EQ_FREQSTA_LOW状态下，由RTC喂狗

			ADC1Start();
	
			LocalClockTick.Time5msCnt++;
	
			if (LocalClockTick.Time5msCnt >= 5)
			{
				LocalClockTick.Time5msCnt = 0;
				LocalClockTick.TimeIntFlg = TRUE;                               //lq 每5ms使能本地时钟计数一次
			}
		}


           //========================= 
           #if  CONFIG_PTT_EN	// lnw  create
              PTT_Timer_Service();
           #endif
           //=========================


		
	   	
		capture = TIM_GetCapture2(TIM2);
		TIM_SetCompare2(TIM2, capture + CCR2_Val);
	}
	
	if(TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);		

        /*lq GPIOSPI 时钟分频计数 */
        GPIOSPI_InstClockCount(GPIOSPI1);

		capture = TIM_GetCapture3(TIM2);
		TIM_SetCompare3(TIM2, capture + CCR3_Val);	
	}
#if 0
	else
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);

		capture = TIM_GetCapture4(TIM2);
		TIM_SetCompare4(TIM2, capture + CCR4_Val);
	}
#endif
}

/*******************************************************************************
* 名称  : GetPhaseNum
* 描述  : 获取相位号
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
S16 GetPhaseNum(void)
{
	return (gsiPhaseCnt);	
}

/*******************************************************************************
* 名称  : GetSlotNum
* 描述  : 获取时隙号
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
S16 GetSlotNum(void)
{
	return (gsiSlotCnt);	
}

/*******************************************************************************
* 名称  : GetFrameNum
* 描述  : 获取帧号
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
S32 GetFrameNum(void)
{
	return (gslFrameCnt);	
}

/*******************************************************************************
* 名称  : GetSlotSum
* 描述  : 获取当前的时隙计数总数
* 输入  : 无
* 输出  : 无
* 返回  : 当前的时隙计数总数
*******************************************************************************/
S32 GetSlotSum(void)
{
    U32 ulFrameNumCurr;
    U16 usSlotNumCurr;
    U32 ulSlotSumCurr;                                                              

    ulFrameNumCurr = GetFrameNum();
    usSlotNumCurr = GetSlotNum();
    ulSlotSumCurr = ulFrameNumCurr * 2250 + usSlotNumCurr; 
    
    return ulSlotSumCurr;   
}

/*******************************************************************************
* 名称  : SetSlotTimer
* 描述  : 使用时隙数定时
* 输入  : 无
* 输出  : 无
* 返回  : 时隙定时值
*******************************************************************************/
S32 SetSlotTimer(U16 inTimeInterval)
{
    U32 ulSlotSumCurr;                                                              

    ulSlotSumCurr = GetSlotSum(); 
    
    return (ulSlotSumCurr + inTimeInterval);   
}

/*******************************************************************************
* 名称  : ResetTimeCount
* 描述  : 复位时间寄存器
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ResetTimeCount(void)
{
	gslFrameCnt = 0;
	gsiSlotCnt = 0;
	gsiPhaseCnt = 0;
}	

/*******************************************************************************
* 名称  : SetFSKDataXmtEn
* 描述  : 使能调制数据发送使能
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetFSKDataXmtEn(void)
{
    gucFSKDataXmtEn = TRUE;
}

/*******************************************************************************
* 名称  : ResetFSKDataXmtFlg
* 描述  : 复位调制数据发送使能
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ResetFSKDataXmtEn(void)
{
    gucFSKDataXmtEn = FALSE;
}	

/*******************************************************************************
* 名称  : GetFSKDataXmtEn
* 描述  : 获取调制数据发送使能
* 输入  : 无
* 输出  : 无
* 返回  : 调制数据发送使能状态
*******************************************************************************/
U8 GetFSKDataXmtEn(void)
{
    return gucFSKDataXmtEn;
}

/*******************************************************************************
* 名称  : GetFSKDataXmtSleepState
* 描述  : 获取调制数据发送模块是否允许MCU进入休眠
* 输入  : 无
* 输出  : 无
* 返回  : 是否允许
*******************************************************************************/
U8 GetFSKDataXmtSleepState(void)
{
    if (!GetFSKDataXmtEn())
    {
        return TRUE;    
    }
    else
    {
        return FALSE; 
    }
}

/*******************************************************************************
* 名称  : SetFetchFirstFskDataEn
* 描述  : 使能从调制队列中取第一条调制数据
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetFetchFirstFskDataEn(void)
{
    gucFetchFirstFskDataEn = TRUE;
}

/*******************************************************************************
* 名称  : ResetFetchFirstFskDataEn
* 描述  : 失能从调制队列中取第一条调制数据
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ResetFetchFirstFskDataEn(void)
{
    gucFetchFirstFskDataEn = FALSE;
}

/*******************************************************************************
* 名称  : GetFetchFirstFskDataEn
* 描述  : 查询状态
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GetFetchFirstFskDataEn(void)
{
    return gucFetchFirstFskDataEn;
}

/*******************************************************************************
  Copyright (C), 2012, 712.Co.,Ltd.
  FileName: adjust.c
  Author: 郭长勇      Version : V1.00        Date: 2012.07.28
  Description:    时间自动校对模块
  Version:         V1.00
  Function List:   // 主要函数及其功能
  History:         // 历史修改记录
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#include "stm32f10x.h"
#include "Include.h"
#include "def.h"
#include "adjust.h"
#include "gpsctrl.h"
#include "gps.h"
#include "Tim.h"
#include "initial.h"
#include "rtc.h"
#include "debug.h"
#include "systemtick.h"

TimeAdjustStruct TimAdjustPara;

/*******************************************************************************
* 名称  : InitTimAdjust
* 描述  : 定时器调整初始化
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitTimAdjust(void)
{
	TimAdjustPara.AdjustCtrlState = ADJUST_CTRL_STA_IDEL;
	TimAdjustPara.TimPastCount = 0;
	TimAdjustPara.RtcPastCount = 0;
	//TimAdjustPara.NowCount = 0;
	//TimAdjustPara.DiffCount = 0;
	TimAdjustPara.TimDiffCount = 0;
	TimAdjustPara.RtcDiffCount = 0;
	TimAdjustPara.TimDiffSum = 0;
	TimAdjustPara.RtcDiffSum = 0;
	TimAdjustPara.PeriodCount = 0;
	TimAdjustPara.CalcNowFlag = FALSE;

    TimAdjustPara.TimCntLast = 0;
    TimAdjustPara.RtcCntLast = 0;
    TimAdjustPara.TimCntSec1 = 0;
    TimAdjustPara.RtcCntSec1 = 0;
    TimAdjustPara.TimCntSec1Sum = 0;
    TimAdjustPara.RtcCntSec1Sum = 0;

    TimAdjustPara.PeriodCountLast = 10;                                         //lq 初始阈值设为10s

	//TimAdjustPara.TimeCnt = 0;
	TimAdjustPara.TimePeriod = ADJUST_WAIT_LSI_TIME;
	TimAdjustPara.TimeCnt = SetTimePeriodProcess(TimAdjustPara.TimePeriod);//设置等待时间
	TimAdjustPara.RtcClockSource = ADJUST_RTC_SOURCE_LSE;//默认使用LSE时钟源
	TimAdjustPara.RtcClockChoice = FALSE;//开机未选择RTC时钟
}

/*******************************************************************************
* 名称  : ResetTimAdjust
* 描述  : 定时器调整复位
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/	
void ResetTimAdjust(void)
{
	TimAdjustPara.TimPastCount = 0;
	TimAdjustPara.RtcPastCount = 0;
	//TimAdjustPara.NowCount = 0;
	//TimAdjustPara.DiffCount = 0;
	TimAdjustPara.TimDiffCount = 0;
	TimAdjustPara.RtcDiffCount = 0;
	TimAdjustPara.TimDiffSum = 0;
	TimAdjustPara.RtcDiffSum = 0;
	TimAdjustPara.PeriodCount = 0;
	TimAdjustPara.CalcNowFlag = FALSE;
    
    TimAdjustPara.TimCntLast = 0;
    TimAdjustPara.RtcCntLast = 0;
    TimAdjustPara.TimCntSec1 = 0;
    TimAdjustPara.RtcCntSec1 = 0;
    TimAdjustPara.TimCntSec1Sum = 0;
    TimAdjustPara.RtcCntSec1Sum = 0;
        	
}	

/*******************************************************************************
* 名称  : ResetTimAdjustCtrlState
* 描述  : 复位定时调整模块
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ResetTimAdjustCtrlState(void)
{
	TimAdjustPara.AdjustCtrlState = ADJUST_CTRL_STA_IDEL;
}	

/*******************************************************************************
* 名称  : TimAdjustProcess
* 描述  : 定时器调整主函数，当GPS同步期间出现GPS秒脉冲断掉的
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TimAdjustProcess(void)//此模块只工作在省电模式                             //lq 正常模式下，GPS长开， 
{                                                                               //lq Tim可以一直与GPS同步， 
	S16 Phase1Adjust;//时隙每1秒调整值                                          //lq 实际是为省电模式的休眠状态下偏差的调整做准备
	S16 Phase10Adjust;//时隙每10秒调整值
	S16 Phase100Adjust;//时隙每100秒调整值

	U32 TempCount;
	U32 TempCount2;
	
	//如果是空闲
	if (TimAdjustPara.AdjustCtrlState == ADJUST_CTRL_STA_IDEL)
	{
		if (IsGpsPowerupProtect() == TRUE)//GPS是否在开机定位保护状态       //lq 只有处在GPS开机定位保护状态时，才进行调整
		{
			ResetTimSaveState();//复位GPS脉冲标志位
			ResetTimAdjust();//复位寄存器

            TimCnt = 0;
            RtcCnt = 0;

			TimAdjustPara.AdjustCtrlState = ADJUST_CTRL_STA_FRISTFLAG;
			debug_printf("$ADJUST_CTRL_STA : ADJUST_CTRL_STA_FRISTFLAG!");
		}
	}
	//等第一个GPS脉冲
	else if (TimAdjustPara.AdjustCtrlState == ADJUST_CTRL_STA_FRISTFLAG)
	{
		if (GetTimSaveState() == TRUE)
		{
			TimAdjustPara.TimPastCount = GetTimCount();//保存第一个
			TimAdjustPara.RtcPastCount = GetRtcCount();//保存第一个

            TimAdjustPara.TimCntLast = GetTimCnt();
            TimAdjustPara.RtcCntLast = GetRtcCnt();

			TimAdjustPara.AdjustCtrlState = ADJUST_CTRL_STA_COUNT;
			debug_printf("$ADJUST_CTRL_STA : ADJUST_CTRL_STA_COUNT!");
		}
		else
		{
			if (TimAdjustPara.CalcNowFlag == TRUE)  //lq 进入开机定位保护后一直未再次定上位
			{
				TimAdjustPara.AdjustCtrlState = ADJUST_CTRL_STA_CALC;//开始计算调整值
			    debug_printf("$ADJUST_CTRL_STA : ADJUST_CTRL_STA_CALC!");
			}
		}
	}
	//统计阶段
	else if (TimAdjustPara.AdjustCtrlState == ADJUST_CTRL_STA_COUNT)
	{
		if (GetTimSaveState() == TRUE)
		{				
			//Tim偏差
			TempCount = GetTimCount();
			TimAdjustPara.TimDiffCount = TempCount - TimAdjustPara.TimPastCount - 48000;//lq 1s内Tim的计数（校准后）与标准值之差  
			TimAdjustPara.TimPastCount = TempCount;//保存过去的计数状态			

			TempCount = GetTimCnt();           
            TimAdjustPara.TimCntSec1 = TempCount - TimAdjustPara.TimCntLast;
            TimAdjustPara.TimCntLast = TempCount;

			//Rtc偏差
		   	TempCount = GetRtcCount();												   //rtc的中断频率为8192 //lq?
			if (IsRtcClockSourceLSE() == TRUE)
			{
				//LSE偏差
				TimAdjustPara.RtcDiffCount = TempCount - TimAdjustPara.RtcPastCount
                                             - RTC_LSE_FREQ_VALUE;//计数差和512的误差	
			}                                                                             //lq 1s内RTC的实际中断数与标准值（ 
			else                                                                          //lq 低频时为512，高频时为1024，但高频时每2次中断计数一次）之差     
			{
				//LSI偏差
				TimAdjustPara.RtcDiffCount = TempCount - TimAdjustPara.RtcPastCount
                                             - RTC_LSI_FREQ_VALUE;//计数差和500的误差
			}	
			TimAdjustPara.RtcPastCount = TempCount;//保存过去的计数状

			TempCount = GetRtcCnt();           
            TimAdjustPara.RtcCntSec1 = TempCount - TimAdjustPara.RtcCntLast;
            TimAdjustPara.RtcCntLast = TempCount;

			if (TimAdjustPara.TimDiffCount > ADJUST_MAX_DIFF_VALUE)//两次GPS脉冲是断掉的
			{
				//此处加入直接进入计算统计结果状态
				debug_printf("TIM RTC GPS PLUS FAIL !!!");
				TimAdjustPara.AdjustCtrlState = ADJUST_CTRL_STA_CALC;//开始计算调整值
			    debug_printf("$ADJUST_CTRL_STA : ADJUST_CTRL_STA_CALC!");
			}
			else                                                            //lq 计算累计偏差和
			{
				TimAdjustPara.TimDiffSum += TimAdjustPara.TimDiffCount;
				TimAdjustPara.RtcDiffSum += TimAdjustPara.RtcDiffCount;
				
                TimAdjustPara.TimCntSec1Sum += TimAdjustPara.TimCntSec1;
                TimAdjustPara.RtcCntSec1Sum += TimAdjustPara.RtcCntSec1;

                TimAdjustPara.PeriodCount++;//从1开始算                     //lq 累计次数（秒脉冲数）
            }

//lqdb				debug_printf("TIM ADJUST COUNT : %d\r\n ",TimAdjustPara.PeriodCount);
		}
		//如果没有GPS脉冲
		else
		{											//如果是长开16分钟就调整一次时钟
			if (TimAdjustPara.CalcNowFlag == TRUE || TimAdjustPara.PeriodCount > 1000)//GPS模块开机保护时间过去了，则计算修正值
			{
				TimAdjustPara.AdjustCtrlState = ADJUST_CTRL_STA_CALC;//开始计算调整值   //lq 开机定位保护时间到or长开16min(1000s)，开始调整
			}
			else//查看如果是大于2秒依然没有GPS脉冲，则重新计算
			{
				TempCount2 = GpsTimCount - TimAdjustPara.TimPastCount;//计算最后一次秒脉冲到现在的时间
				
				if (TempCount2 > ADJUST_MAX_WAIT_PLUS_VALUE)//等待超时      //lq 超过2s没有收到GPS秒脉冲
				{
					debug_printf("TIM RTC GPS PLUS FAIL !!!");
					TimAdjustPara.AdjustCtrlState = ADJUST_CTRL_STA_CALC;//开始计算调整值   //lq 超过2s没有收到GPS秒脉冲,开始调整
			        debug_printf("$ADJUST_CTRL_STA : ADJUST_CTRL_STA_CALC!");				
                }				
			}
		}
	}	

	//计算阶段
	else if (TimAdjustPara.AdjustCtrlState == ADJUST_CTRL_STA_CALC)
	{
        if (TimAdjustPara.PeriodCount < TimAdjustPara.PeriodCountLast)          //lq 小于上次的计数周期的话，就不计算了
		{
			debug_printf("TIM RTC Adjust Fail,Much less Sample Point !!!");
			debug_printf("Sample Point Num : %d, Last Sample Point Num : %d\r\n",
                         TimAdjustPara.PeriodCount, TimAdjustPara.PeriodCountLast);				

			TimAdjustPara.AdjustCtrlState = ADJUST_CTRL_STA_IDEL;//状态机空闲
			debug_printf("$ADJUST_CTRL_STA : ADJUST_CTRL_STA_IDEL!");

		}
		else//可以计算
		{
    		/*lq 记录本次计数周期值*/
            TimAdjustPara.PeriodCountLast = TimAdjustPara.PeriodCount;

            /*lq 计算本地Tim在UTC1s内的计数值*/
    		TempCount = (U32)((double)TimAdjustPara.TimCntSec1Sum / (double)TimAdjustPara.PeriodCount + 0.5);
            if (TempCount > ADJUST_TIM_CNT_1S_FLOOR && TempCount < ADJUST_TIM_CNT_1S_CEIL)
            {
                TimCnt1s = TempCount;
                TimCnt10s = (U32)(((double)TimAdjustPara.TimCntSec1Sum / (double)TimAdjustPara.PeriodCount) * 10 + 0.5);
                TimCnt100s = (U32)(((double)TimAdjustPara.TimCntSec1Sum / (double)TimAdjustPara.PeriodCount) * 100 + 0.5);   
            }
    
    		/*lq 计算本地Rtc在UTC1s内的计数值*/
            TempCount = (U32)((double)TimAdjustPara.RtcCntSec1Sum / (double)TimAdjustPara.PeriodCount + 0.5);
            if (IsRtcClockSourceLSE() == TRUE)
            {
                if (TempCount > ADJUST_RTC_LSE_CNT_1S_FLOOR && TempCount < ADJUST_RTC_LSE_CNT_1S_CEIL)
                {
                    RtcCnt1s = TempCount;
                    RtcCnt10s = (U32)(((double)TimAdjustPara.RtcCntSec1Sum / (double)TimAdjustPara.PeriodCount) * 10 + 0.5);
                    RtcCnt100s = (U32)(((double)TimAdjustPara.RtcCntSec1Sum / (double)TimAdjustPara.PeriodCount) * 100 + 0.5);
                }
            }
            else
            {
                if (TempCount > ADJUST_RTC_LSI_CNT_1S_FLOOR && TempCount < ADJUST_RTC_LSI_CNT_1S_CEIL)
                {
                    RtcCnt1s = TempCount;
                    RtcCnt10s = (U32)(((double)TimAdjustPara.RtcCntSec1Sum / (double)TimAdjustPara.PeriodCount) * 10 + 0.5);
                    RtcCnt100s = (U32)(((double)TimAdjustPara.RtcCntSec1Sum / (double)TimAdjustPara.PeriodCount) * 100 + 0.5);
                }        
            }
    
    		debug_printf("Tim 1s Count  : %d\r\n", TimCnt1s);				
    		debug_printf("Tim 10s Count : %d\r\n", TimCnt10s);				
    		debug_printf("Tim 100s Count: %d\r\n", TimCnt100s);				
    
    		debug_printf("Rtc 1s Count  : %d\r\n", RtcCnt1s);				
    		debug_printf("Rtc 10s Count : %d\r\n", RtcCnt10s);				
    		debug_printf("Rtc 100s Count: %d\r\n", RtcCnt100s);				

			debug_printf("TIM RTC Adjust Calc Begin!!!");
			debug_printf("Sample Point Num : %d\r\n", TimAdjustPara.PeriodCount);
        
        #if 0
			                                                                //lq 计算Tim2时钟的偏差补偿
			//Phase2Adjust = (U16)TimError;//1秒要调整的phase数量           //lq Tim的中断次数与相位数相等（48kHzTim的中断间隔等于相位时长）    
			Phase1Adjust = TimAdjustPara.TimDiffSum / TimAdjustPara.PeriodCount;//整除，多余的舍去  //lq 偏差的整数部分（每一秒需调整的相位数）
			Phase10Adjust = (TimAdjustPara.TimDiffSum * 10 - Phase1Adjust * TimAdjustPara.PeriodCount * 10) / TimAdjustPara.PeriodCount;//每10秒补充的部分  //lq 偏差的十分位小数部分（十分位小数部分每10s调整一次）
			Phase100Adjust = (TimAdjustPara.TimDiffSum * 100 - Phase1Adjust * TimAdjustPara.PeriodCount * 100 
                              - ((Phase10Adjust * TimAdjustPara.PeriodCount) / 10) *100) / TimAdjustPara.PeriodCount;//每100秒补充的部分    //lq 偏差的百分位小数部分（百分位小数每100s调整一次） 				

			if (Phase1Adjust > ADJUST_PHASE_MAX_ADJUST || Phase1Adjust< -ADJUST_PHASE_MAX_ADJUST)//超出范围，出现错误
			{
				Phase1Adjust = 0;
			}

			if (Phase10Adjust > ADJUST_PHASE_MAX_ADJUST || Phase10Adjust< -ADJUST_PHASE_MAX_ADJUST)//超出范围，出现错误
			{
				Phase10Adjust = 0;
			}

			if (Phase100Adjust > ADJUST_PHASE_MAX_ADJUST || Phase100Adjust< -ADJUST_PHASE_MAX_ADJUST)//超出范围，出现错误
			{
				Phase100Adjust = 0;
			}

			SetTimAdjust1(Phase1Adjust);                                    //lq 调整值累加    
			SetTimAdjust10(Phase10Adjust);
			SetTimAdjust100(Phase100Adjust);
        #else
			CalcTimPhaseAdjust(TimAdjustPara.TimDiffSum, TimAdjustPara.PeriodCount); //lq 计算RTC时钟的偏差补偿
        #endif

			CalcRtcPhaseAdjust(TimAdjustPara.RtcDiffSum, TimAdjustPara.PeriodCount); //lq 计算RTC时钟的偏差补偿

			debug_printf("TIM RTC Adjust Finish !!!");

    		debug_printf("TimAdjust1  : %d\r\n", TimAdjust1);				
    		debug_printf("TimAdjust10 : %d\r\n", TimAdjust10);				
    		debug_printf("TimAdjust100: %d\r\n", TimAdjust100);				
    
    		debug_printf("RtcPhaseAdjust1  : %d\r\n", RtcPhaseAdjust1);				
    		debug_printf("RtcPhaseAdjust10 : %d\r\n", RtcPhaseAdjust10);				
    		debug_printf("RtcPhaseAdjust100: %d\r\n", RtcPhaseAdjust100);
    		debug_printf("RtcTimPhaseCnt   : %d\r\n", struRtcTimPhase1.RtcTimPhaseNum);
            	
			TimAdjustPara.AdjustCtrlState = ADJUST_CTRL_STA_IDEL;//状态机空闲
			debug_printf("$ADJUST_CTRL_STA : ADJUST_CTRL_STA_IDEL!");
		}
	}
	else
	{
		TimAdjustPara.AdjustCtrlState = ADJUST_CTRL_STA_IDEL;//状态机空闲
		debug_printf("$ADJUST_CTRL_STA : ADJUST_CTRL_STA_IDEL!");        	
	}
	                                                                            
	RtcClockChoice();                                                           //lq? 为何在此时选择RTC时钟源？
}

/*******************************************************************************
* 名称  : RtcClockChoice
* 描述  : 选择RTC时钟源
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void RtcClockChoice(void)
{
	//RTC时钟选择模块
	if (TimAdjustPara.RtcClockChoice == FALSE)//未选择时钟源
	{
		if (TimAdjustPara.TimeCnt == GetLocalClockTickProcess())
		{
			TimAdjustPara.RtcClockChoice = TRUE;

			if (!(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET))//如果判断LSE起振成功
			{
				UartResponseMsg("$32.768KHz CLOCK READY!");	
				
				TimAdjustPara.RtcClockSource = ADJUST_RTC_SOURCE_LSE;//选择时钟源为LSE
				struRtcPara.ucRtcLSEorLSI = RTC_CLK_SOURCE_LSE;

				/* Select the RTC Clock Source */
				RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//配置时钟为LSE	        //lq 外部晶振起振成功，LSE晶振选为RTC时钟源
			}
			else//若没有起振则换LSI
			{
			 	/* Enable the LSI OSC */
				RCC_LSICmd(ENABLE);                                             //lq 开启内部RC振荡器
				/* Wait till LSI is ready */
				while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)//如果这个时候复位了就说明LSI振荡电路在低温也不行
				{}

				UartResponseMsg("$40KHZ CLOCK READY!");
				TimAdjustPara.RtcClockSource = ADJUST_RTC_SOURCE_LSI;//选择时钟源为LSI
				struRtcPara.ucRtcLSEorLSI = RTC_CLK_SOURCE_LSI;

				/* Select the RTC Clock Source */
				RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);                         //lq 外部晶振启动失败，LSI晶振选为RTC时钟源
			}

			RTCConfigurationPart2();//完成剩余配置
		}
	}
}

/*******************************************************************************
* 名称  : IsRtcClockSourceLSE
* 描述  : 判断RTC时钟是否为LSE
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 IsRtcClockSourceLSE(void)
{
	if (TimAdjustPara.RtcClockSource == ADJUST_RTC_SOURCE_LSE)	
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*******************************************************************************
* 名称  : SetAdjustCalcNow
* 描述  : 令时间调整模块立刻计算修正值
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetAdjustCalcNow(void)
{
	TimAdjustPara.CalcNowFlag = TRUE;//立刻计算
}	

/*******************************************************************************
* 名称  : GetAdjustSleepState
* 描述  : 获取可休眠状态
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GetAdjustSleepState(void)
{
	//获取可休眠状态
	if (TimAdjustPara.AdjustCtrlState == ADJUST_CTRL_STA_IDEL)
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}


	

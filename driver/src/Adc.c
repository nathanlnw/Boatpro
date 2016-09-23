/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: Adc.c
  Author: vozhj      Version : V1.00        Date: 2011.10.18
  Description:    模数转换器控制
  Version:         V1.00
  Function List:   // 主要函数及其功能
  History:         // 历史修改记录
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#include <locale.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <stdarg.h>

#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "def.h"
#include "Adc.h"
#include "LED.h"
#include "tempersensor.h"
#include "Include.h"

#ifdef ADC_TEST_STA
#include "debug.h"
#include "common.h"
#endif

U16 guiADC1_14_Val;
U16 arrADC1_Val[ADC1_CHANNEL_NUM];
U8  ucADC1_ConvFinishFlag = FALSE;
S16 ssADC1_CalValue;
/*******************************************************************************
* 名称  : ADC1_ValInit
* 描述  : ADC1的DMA值初始化
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ADC1_ValInit(void)
{
    int i;
    for (i = 0; i < ADC1_CHANNEL_NUM; i++)
    {
        arrADC1_Val[i] = 0;
    }

    ssADC1_CalValue = 0;
}

/*******************************************************************************
* 名称  : ADC1_Init
* 描述  : ADC1初始化
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ADC1_RegInit(void)
{
//	ADC_InitTypeDef  ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = PUBLIC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(PUBLIC_PORT, &GPIO_InitStructure);

	//////////////////////////////////////////////////////

	ADC1->CR1 =	((u32)1 << 5) | ((u32)1 << 11);                                     //lq 允许产生EOC中断，开启规则通道组上的间断模式

	ADC1->CR2 =((u32)1 << 20) | ((u32)7 << 17);                                     //lq 使用规则通道的外部触发，且外部触发事件为SWSTART

	//选择通道（14）的采样时间
	ADC1->SMPR1 = ((u32)3 << 12);                                                 //lq 通道14的采样时间为28.5个周期

	//规则通道转换序列中转换总数：1
	ADC1->SQR1 = ((u32)0 << 20);                                                  //lq 规则通道序列长度为1

	//设置转换通道
	//第一个转换通道编号：14
	ADC1->SQR3 = ((u32)14 << 0);                                                //lq 第一个转换通道的编号为14

	//当该位为0时，写入1将把ADC从断电模式下唤醒
	ADC1->CR2 |= ((u32)1 << 0);                                                 //lq 开启AD转换器


}

/*******************************************************************************
* 名称  : ADC1_PerInit
* 描述  : ADC1初始化
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ADC1_PerInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure; 

	GPIO_InitStructure.GPIO_Pin = PUBLIC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(PUBLIC_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PUBLIC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(PUBLIC_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PUBLIC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(PUBLIC_PORT, &GPIO_InitStructure);  

    /* Initialize the ADC1 according to the ADC_InitStructure members */ 
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 
    ADC_InitStructure.ADC_ScanConvMode = ENABLE; 
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; 
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; 
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 
    ADC_InitStructure.ADC_NbrOfChannel = 4; 
    ADC_Init(ADC1, &ADC_InitStructure); 

    /* Configures ADC1 Channel8 as: second converted channel with an 28.5 cycles sample time */ 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 2, ADC_SampleTime_28Cycles5);       

    /* Configures ADC1 Channel14 as: first converted channel with an 28.5 cycles sample time */ 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_28Cycles5);       

    /* Configures ADC1 Channel15 as: third converted channel with an 28.5 cycles sample time */ 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 3, ADC_SampleTime_28Cycles5);       

    /* Configures ADC1 Channel16 as: fourth converted channel with an 239.5 cycles sample time */ 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 4, ADC_SampleTime_239Cycles5);//lq 温度传感器       
    
    /* Enable the temperature sensor and vref internal channel */ 
    ADC_TempSensorVrefintCmd(ENABLE);
    
    /* Initialize the DMA Channel1*/
    DMA_PerInit();

    ADC1_ValInit();
        
    /* Enable ADC1 DMA transfer */ 
    ADC_DMACmd(ADC1, ENABLE);  
    
    /* Enable ADC1 */ 
    ADC_Cmd(ADC1, ENABLE);             

    /*lq ADC1校准*/
    ADC1_Calibration();   
}

/*******************************************************************************
* 名称  : ADC1_Calibration
* 描述  : ADC1校准
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ADC1_Calibration(void)
{
//    FlagStatus Status; 

    /* Reset the ADC1 Calibration registers */ 
    ADC_ResetCalibration(ADC1);
       
    /* Get the ADC1 reset calibration registers status */                       
    while(ADC_GetResetCalibrationStatus(ADC1) != RESET )                        //lq 等待重置完成
    {
    }

    /* Start the ADC1 Calibration */ 
    ADC_StartCalibration(ADC1);

    /* Get the ADC1 calibration status */ 
    while(ADC_GetCalibrationStatus(ADC1) != RESET)                              //lq 等待校准完成
    {
    }

    SetADC1_CalValue(ADC_GetConversionValue(ADC1));
}
/*******************************************************************************
* 名称  : DMA_PerInit
* 描述  : DMA初始化,DMA channel1用作ADC1
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void DMA_PerInit(void)
{
    /* Initialize the DMA Channel1 according to the DMA_InitStructure members */ 
    DMA_InitTypeDef DMA_InitStructure; 

    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR; 
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)arrADC1_Val;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 
    DMA_InitStructure.DMA_BufferSize = 4; 
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; 
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; 
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; 
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 
    DMA_Init(DMA1_Channel1, &DMA_InitStructure); 
    
    /* Enable DMA Channel1 complete transfer interrupt */ 
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
    
    /* Enable DMA Channel1 */ 
    DMA_Cmd(DMA1_Channel1, ENABLE);       
}

/*******************************************************************************
* 名称  : DMA_ADC1_ISR
* 描述  : DMA中断处理
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void DMA_ADC1_ISR(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC1) != RESET)
    {
        SetADC1_ConvFinishFlag(); 
#if CONFIG_TEMPERATURE_MONITOR
        SetADC1_TemperConvFinishFlag();   
#endif        
        /* Clear the DMA Channel5 global interrupt pending bit */ 
        DMA_ClearITPendingBit(DMA1_IT_TC1);               
    }
}

 /*******************************************************************************
* 名称  : ADC1Start
* 描述  : ADC1开始采样
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ADC1Start(void)
{
//lq	ADC1->CR2 |= ((u32)1 << 22);

    /* Start by software the ADC1 Conversion */ 
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/*******************************************************************************
* 名称  : ADC1IntProcess
* 描述  : ADC1采样中断处理
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ADC1IntProcess(void)
{
	guiADC1_14_Val = ADC1->DR;
}

/*******************************************************************************
* 名称  : GetADC1_14_Val
* 描述  : 获取电池电压采样值
* 输入  : 无
* 输出  : 无
* 返回  : 采样值
*******************************************************************************/
U16 GetADC1_14_Val(void)
{
#if 0
	return arrADC1_Val[0];
#else
    return arrADC1_Val[0] - ssADC1_CalValue;    //lq 电池电压采样数组中的采样值是未校准的值，实际采样值是校准的值（因为温度采样）
#endif
}

/*******************************************************************************
* 名称  : SetADC1_ConvFinishFlag
* 描述  : ADC1转换完成标志置位
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetADC1_ConvFinishFlag(void)
{
    ucADC1_ConvFinishFlag = TRUE;    
}

/*******************************************************************************
* 名称  : ResetADC1_ConvFinishFlag
* 描述  : ADC1转换完成标志复位
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ResetADC1_ConvFinishFlag(void)
{
    ucADC1_ConvFinishFlag = FALSE;    
}

/*******************************************************************************
* 名称  : GetADC1_ConvFinishFlag
* 描述  : 查询ADC1转换状态
* 输入  : 无
* 输出  : 无
* 返回  : ADC1转换状态
*******************************************************************************/
U8 GetADC1_ConvFinishFlag(void)
{
    return ucADC1_ConvFinishFlag;
}

/*******************************************************************************
* 名称  : GetADC1_CalValue
* 描述  : 获取校准值
* 输入  : 无
* 输出  : 无
* 返回  : 校准值
*******************************************************************************/
S16 GetADC1_CalValue(void)
{
    return ssADC1_CalValue;
}

/*******************************************************************************
* 名称  : SetADC1_CalValue
* 描述  : 设置校准值
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetADC1_CalValue(const S16 ssCalValue)
{
    ssADC1_CalValue = ssCalValue;
}

#ifdef ADC_TEST_STA
ADCTestStruct ADCTest;
/*******************************************************************************
* 名称  : ADCTestInit
* 描述  : ADC测试参数初始化
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ADCTestInit(void)
{
    ADCTest.TimePeriod = 200;   //lq 1s    
    ADCTest.TimeCnt = SetTimePeriodProcess(ADCTest.TimePeriod);
}

/*******************************************************************************
* 名称  : GetLDPOWER_IN
* 描述  : ADC1采样内部功率
* 输入  : 无
* 输出  : 无
* 返回  : 采样值
*******************************************************************************/
void ADCTestMain(void)
{
	U16 uiTempVal;
	char chTemp[50];
    U16 i;
    float temp;

    if (ADCTest.TimeCnt == GetLocalClockTickProcess())
	{
		ADCTest.TimeCnt = SetTimePeriodProcess(ADCTest.TimePeriod);
    
        usart_printf("$T712,TXT,CALVALUE = %.4d\r\n",GetADC1_CalValue());
    
        if (GetADC1_ConvFinishFlag())
        {
            ResetADC1_ConvFinishFlag();
            for (i = 0; i < 4; i++)
            {
    //        	sprintf(chTemp, "$T712,TXT,ADVAL%d = %.4d", i,arrADC1_Val[i]);
    //          UartResponseMsg(chTemp);        
                usart_printf("$T712,TXT,ADVAL%d = %.4d\r\n", i, arrADC1_Val[i]);
            }
        }
    //    temp = GetTemper();
    //    usart_printf("$T712,TXT,TEMPER = %.4f\r\n",struTemperSensorPara.fsTemper);
    //    usart_printf("$T712,TXT,TEMPER = %.4f\r\n",temp);
        usart_printf("$T712,TXT,TEMPER = %.4f\r\n",GetTemper());
    }    
}
#endif


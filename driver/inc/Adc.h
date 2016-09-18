/****************************************************************************
*  Copyright (C), 2011, Institute Mod. Opt. NKU                             *
*  FileName: Adc.h	 	                        	          	            *
*  Description:  ADC驱动控制初始化函数声明									*
*****************************************************************************
*  History:               				                                    *
*      <author>  <time>    <version >   <desc>                              *
*      vozhj   	 10/12/02     1.0       build this moudle		            *
****************************************************************************/

#ifndef __ADC_H
#define __ADC_H

#undef ADC_TEST_STA

#define ADC1_CHANNEL_NUM    4

typedef struct ADCTest
{
    U32 TimeCnt;
    U16 TimePeriod;
}ADCTestStruct;

extern U16 arrADC1_Val[ADC1_CHANNEL_NUM];

extern void ADC1_RegInit(void);
extern void ADC1Start(void);
extern void ADC1IntProcess(void);
extern U16 GetADC1_14_Val(void);
extern void ADC1_PerInit(void);
extern void ADC1_ValInit(void);
extern void DMA_PerInit(void);
extern void DMA_ADC1_ISR(void);
extern void SetADC1_ConvFinishFlag(void);
extern void ResetADC1_ConvFinishFlag(void);
extern U8 GetADC1_ConvFinishFlag(void);
extern void ADC1_Calibration(void);
extern void SetADC1_CalValue(const S16 ssCalValue);

#ifdef ADC_TEST_STA
void ADCTestMain(void);
#endif

#endif

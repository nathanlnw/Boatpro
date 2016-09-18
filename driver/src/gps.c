/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: Usart.c
  Author: 李斌       Version : V1.00        Date: 2012.03.12
  Description:    串口底层驱动和应用处理程序
  Version:         V1.00
  Function List:   // 主要函数及其功能    	 
			1．函数名：InitUsart2
    	   	   函数功能：初始化串口2
 
			2．函数名：Uart2Interrupt
    	   	   函数功能：串口2
    	   	   
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "def.h"
#include "usart.h"
#include "gps.h"
#include "Tim.h"
#include "common.h"
#include "LED.h"
#include "gpsctrl.h"
#include "rtc.h"
#include "initial.h"
#include "config.h"
#include "Include.h"

/* Private variables ---------------------------------------------------------*/
struct UartTxFIFOStruct Uart2TxFIFO; //发送缓冲
struct UartRxFIFOStruct Uart2RxFIFO;

//lq U16 gps_pps_count = 0;//GPS秒脉冲计数，用于清除不定位情况下上电冲击的干扰
U32 GpsTimCountSave;
U32	GpsRtcCountSave;
U32 TimCntSave = 0;
U32	RtcCntSave = 0;

U8	GpsTimSaveFlag = FALSE;
/* Private  function  ---------------------------------------------------------*/
void InitUsart2(U32 baud);

/*******************************************************************************
* 名称  : GetTimCount
* 描述  : 返回TIM的时间调整计数
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U32 GetTimCount(void)
{
	return (GpsTimCountSave);
}

/*******************************************************************************
* 名称  : GetRtcCount
* 描述  : 返回RTC的时间调整计数
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U32 GetRtcCount(void)
{
	return (GpsRtcCountSave);
}

/*******************************************************************************
* 名称  : GetTimCnt
* 描述  : 返回Tim的时间调整计数
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U32 GetTimCnt(void)
{
	return (TimCntSave);
}

/*******************************************************************************
* 名称  : GetRtcCnt
* 描述  : 返回RTC的时间调整计数
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U32 GetRtcCnt(void)
{
	return (RtcCntSave);
}

/*******************************************************************************
* 名称  : GetTimSaveState
* 描述  : 查看是否GPS秒脉冲到来并保存时间调整计数
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GetTimSaveState(void)
{
	if (GpsTimSaveFlag == TRUE)
	{
		GpsTimSaveFlag = FALSE;
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}

/*******************************************************************************
* 名称  : ResetTimSaveState
* 描述  : 复位GPS脉冲标志位
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ResetTimSaveState(void)
{
	GpsTimSaveFlag = FALSE;
}

/*******************************************************************************
* 名称  : Usart2Interrupt
* 描述  : 串口2中断服务程序
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void Usart2Interrupt(void)
{
	U8 Data;
	
	//溢出——如果发生溢出需要先读SR，再读DR寄存器，则可消除不断入中断的问题
	if(USART_GetFlagStatus(USART2,USART_FLAG_ORE) == SET)
	{
	 	USART_ClearFlag(USART2,USART_FLAG_ORE);			// 读SR其实就是消除标志
		USART_ReceiveData(USART2);						// 读DR
	}

	//接收中断
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		/* Read one byte from the receive data register */
		Data = USART_ReceiveData(USART2);
		/* 支持3252串口数据透传 */
		//HPI_WriteByte(Data);

        /*lq 若是正常工作模式，且处于低功耗模式则忽略GPS报文*/
        if (!((DevDynamicPara.ucWorkMode == EQ_WORKMOD_NORMAL)
            && (DevDynamicPara.ucLowFreqSta == EQ_FREQSTA_LOW)))
        {
            /*lq NMEA protocol 消息接收 */
		    UartRxProcess(&Uart2RxFIFO, Data);
#if CONFIG_GPS_UBX_EN            
            /*lq UBX protocol 消息接收 */
            UBX_UartRxProcess(COMM2, Data); 
#endif                   
        }
		//USART_SendData(USART1, Data);
	}
		
	//发送中断
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{   
		UartTxIntProcess(&Uart2TxFIFO);
	}
}

/*******************************************************************************
* 名称  : Exti1_Int
* 描述  : 外部中断服务程序 (GPS中断)
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GpsPulseEn = FALSE;      //lq 消除GPS上电和掉电时秒脉冲干扰
void Exti1_Int(void)
{
	if(EXTI_GetITStatus(PULSE_EN_LINE) != RESET)
	{
        GpsSelfCheckInst.TimepulseFlg = TRUE;   //lq 有中断触发即认为秒脉冲正常
        
		if (IsGpsFix() == TRUE && GpsPulseEn == TRUE)                           //lq 秒脉冲早于GPS报文，所以要忽略第一次秒脉冲
        {                                                                       //lq 正常情况下，只有定位成功后GPS模块才会触发秒脉冲
			if (IsGpsPowerupProtect() == TRUE)                                  //lq Tim和RTC时钟偏差计算仅在GPS开机保护期间进行
			{                                                                   //lq Tim和RTC时钟偏差补偿在其各自对应的每次时钟中断中进行
				/*lq 用于统计GPS标准1s内本地Tim和RTC的理论值与实际计数值的偏差*/
                GpsTimCountSave = GpsTimCount;                                  //lq 用于Tim时钟偏差的校准
				GpsRtcCountSave = GpsRtcCount;//保存RTC计数值                   //lq 用于RTC时钟偏差的校准
				GpsTimSaveFlag = TRUE;

                /*lq 用于统计GPS标准1s内本地Tim和RTC的实际计数值*/
                TimCntSave = TimCnt;
                RtcCntSave = RtcCnt;
			}
			
            /*lq GPS同步偏差计算使能*/
            GpsSynCell.GPSSyn_PulseEn = TRUE;			//脉冲使能
			GpsSynCell.GPSSyn_ProtectCnt = 0;			//清保护时间	

            /*lq 记录秒脉冲触发时刻本地时隙相位计数值*/
			GpsSynCell.GPSSyn_PulsePhase = gsiPhaseCnt;	//保存秒脉冲触发时相位  //lq 保存相位计数,用于与GPS同步
			GpsSynCell.GPSSyn_PulseSlot = gsiSlotCnt;
			GpsSynCell.GPSSyn_PulseMin = gslFrameCnt;

            /*lq 秒脉冲对应的GPS标准秒*/
#if CONFIG_GPS_SYNCH_LEAP
            GpsSynCell.ChkTimeS = GpsPara.GPS_TimeSLeap + 1;                    //lq 先有秒脉冲，后有GPS报文
#else
            GpsSynCell.ChkTimeS = GpsPara.GPS_TimeS + 1;                        //lq 先有秒脉冲，后有GPS报文
#endif

#if CONFIG_GPS_SYNCH_HOUR_MIN
            GpsSynCell.ChkTimeM = GpsPara.GPS_TimeM;
            GpsSynCell.ChkTimeH = GpsPara.GPS_TimeH;
#endif		
            /*lq GPS PPS生成标志 */
            GpsPara.GPS_PPSGenFlag = TRUE;
        }

		/* Clear the Key Button EXTI line pending bit */  
		EXTI_ClearITPendingBit(PULSE_EN_LINE);	                                //lq 上升沿触发中断						
	}
}

/************************************************************
*  Function:       InitGPSPower								*
*  Description:    参数初始化								*
*  Input:          无										*
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
void InitGPSPower(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPS_PORT, &GPIO_InitStructure);
	
	//置高
	//lq GPSPOWER_HIGH(); //lq 防止每次待机唤醒后开启GPS
}

/*******************************************************************************
* 名称  : EXTI_Configuration
* 描述  : 配置外部中断和RTC Alarm中断
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void EXTI_Configuration(void)
{	
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = PULSE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PULSE_PORT, &GPIO_InitStructure);

	/* Connect EXTI Line to GPIO Pin */
	GPIO_EXTILineConfig(PULSE_EN_SOURCE_PORT, PULSE_EN_SOURCE_PIN);
	
	/* Configure EXTI Line to generate an interrupt on falling edge */  
	EXTI_InitStructure.EXTI_Line = PULSE_EN_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//下降沿触发，但是要写上升沿才有效，库有问题//lq 是上升沿触发，库没问题
	
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

/*******************************************************************************
* 名称  : InitUsart2
* 描述  : 初始化串口2
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitUsart2(U32 baud) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* Configure USART1 Rx (PA.3) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART1 Tx (PA.2) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USART1 configuration ------------------------------------------------------*/
	/* USART1 configured as follow:
		- BaudRate = 38400 baud  
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure USART2 */
  	USART_Init(USART2, &USART_InitStructure);

	/* Enable USART1 Receive and Transmit interrupts */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);	

	/* Enable the USART1 */
  	USART_Cmd(USART2, ENABLE);  

	InitGPSPower(); 

	EXTI_Configuration();                                                       //lq 配置外部中断-GPS秒脉冲
}

/*******************************************************************************
* 名称  : SetGpsPulseEn
* 描述  : 使能GPS秒脉冲，消除GPS上电和掉电时对GPS秒脉冲的干扰
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SetGpsPulseEn(void)
{
    GpsPulseEn = TRUE;
}

/*******************************************************************************
* 名称  : ResetGpsPulseEn
* 描述  : 失能GPS秒脉冲，消除GPS上电和掉电时对GPS秒脉冲的干扰
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ResetGpsPulseEn(void)
{
    GpsPulseEn = FALSE;
}
#if 0
/*******************************************************************************
* 名称  : ResetPPSCount
* 描述  : 复位秒脉冲计数
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ResetPPSCount(void)
{
	gps_pps_count = 0;
}	
#endif

#if 0
/*******************************************************************************
* 名称 ：GPS_UartTxString
* 描述 ：GPS串口发送字符串
* 输入 ：COMM：串口地址标志
			COMM2：
		 pStr：字符串指针
		 Length：字符串长度
* 输出 ：无
* 返回 ：无
*******************************************************************************/
U8 GPS_UartTxString(char *pStr)
{				
	UartTxDataBuf(COMM2, (U8*)pStr, strlen(pStr));

	return TRUE;		
}
#endif

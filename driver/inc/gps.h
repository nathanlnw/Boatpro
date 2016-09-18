/*******************************************************************************
  Copyright (C), 2010, 712.Co.,Ltd.
  File name:	gps.h
  Author: 李斌      Version: V1.00       Date: 2012.03.12 
  Description:  
  				主要功能：
				1. 定义相关宏定义
				2. 变量声明
				3. 函数声明
				4. 定义结构体类型
				
  Others:         // 其它内容的说明
  Function List:  // 主要函数列表，每条记录应包括函数名及功能简要说明
  History:        // 修改历史记录列表，每条修改记录应包括修改日期、修改
                  // 者及修改内容简述  
    1. Date:
       Author:
       Modification:
*******************************************************************************/
#ifndef __GPS_H__
#define __GPS_H__

#include "def.h"
#define GPS_PORT							GPIOA  					//GPS电源接口
#define GPS_RCC_APB2Periph_GPIO  	    	RCC_APB2Periph_GPIOC 	//开始GPS时钟
#define GPS_PIN						    	GPIO_Pin_0 				//GPS电源引脚

#define GPSPOWER_LOW()						GPIO_ResetBits(GPS_PORT, GPS_PIN)
#define GPSPOWER_HIGH()						GPIO_SetBits(GPS_PORT, GPS_PIN)

#define GPS_POWER_OFF()						GPSPOWER_LOW()
#define GPS_POWER_ON()						GPSPOWER_HIGH()

#define PULSE_PORT							GPIOA  					//GPS电源接口
#define PULSE_RCC_APB2Periph_GPIO  	    	RCC_APB2Periph_GPIOA 	//开始GPS时钟
#define PULSE_PIN						    GPIO_Pin_1 				//GPS电源引脚
#define PULSE_EN_SOURCE_PORT	        	GPIO_PortSourceGPIOA
#define PULSE_EN_SOURCE_PIN	           		GPIO_PinSource1
#define PULSE_EN_LINE        				EXTI_Line1

#define PPS_COUNT_THRESHOLD 1//忽略前几个秒脉冲
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
extern struct UartTxFIFOStruct Uart2TxFIFO; //发送缓冲
extern struct UartRxFIFOStruct Uart2RxFIFO;
/* Private  function  ---------------------------------------------------------*/


/* Public  function  ---------------------------------------------------------*/

extern void InitUsart2(U32 baud);
extern void Usart2Interrupt(void);
extern void Exti1_Int(void);
extern void InitGPSPower(void);
extern void ResetPPSCount(void);

extern U32 GetTimCount(void);
extern U32 GetRtcCount(void);
extern U8 GetTimSaveState(void);
extern void ResetTimSaveState(void);
extern void SetGpsPulseEn(void);
extern void ResetGpsPulseEn(void);
#endif	//__USART_H__





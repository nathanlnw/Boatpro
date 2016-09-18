/****************************************************************************
*  Copyright (C), 2010, 712.Co.,Ltd.		                             *
*  FileName: LED.h	 	                                  	            	*
*  Description:  LED控制头文件												*
*****************************************************************************
*  History:               				                                    *
*      <author>  <time>    <version >   <desc>                              *
*      vozhj   	 10/12/02     1.0       build this moudle		            *
****************************************************************************/
#include "stm32f10x.h"
#include "def.h"

#ifndef __LED_H
#define __LED_H

/*lq 引脚脉冲宽度定时*/
typedef struct PinPulseStruct{
	U32 TimeCnt;
	U16 TimePeriod;
    U8  PinPulseFlg;	   
}PinPulseStruct;

extern PinPulseStruct struPinPulsePara;

/*lq LED1, Green */
#define LED_GREEN_PORT						GPIOC
#define LED_GREEN_RCC_APB2Periph_GPIO  	    RCC_APB2Periph_GPIOC
#define LED_GREEN_PIN						GPIO_Pin_0

#define LED_GREEN_LOW()       				GPIO_ResetBits(LED_GREEN_PORT, LED_GREEN_PIN)
#define LED_GREEN_HIGH()      				GPIO_SetBits(LED_GREEN_PORT, LED_GREEN_PIN)

#define LED_GREEN_ON()                      LED_GREEN_LOW()       				
#define LED_GREEN_OFF()      				LED_GREEN_HIGH()

/*lq LED2, Yellow */
#define LED_YELLOW_PORT						GPIOC
#define LED_YELLOW_RCC_APB2Periph_GPIO  	RCC_APB2Periph_GPIOC
#define LED_YELLOW_PIN						GPIO_Pin_1

#define LED_YELLOW_LOW()       				GPIO_ResetBits(LED_YELLOW_PORT, LED_YELLOW_PIN)
#define LED_YELLOW_HIGH()      				GPIO_SetBits(LED_YELLOW_PORT, LED_YELLOW_PIN)

#define LED_YELLOW_ON()                     LED_YELLOW_LOW()       				
#define LED_YELLOW_OFF()      				LED_YELLOW_HIGH()

/*lq LED3, Red */
#define LED_RED_PORT						GPIOC
#define LED_RED_RCC_APB2Periph_GPIO  	    RCC_APB2Periph_GPIOC
#define LED_RED_PIN						    GPIO_Pin_2

#define LED_RED_LOW()       				GPIO_ResetBits(LED_RED_PORT, LED_RED_PIN)
#define LED_RED_HIGH()      				GPIO_SetBits(LED_RED_PORT, LED_RED_PIN)

#define LED_RED_ON()                        LED_RED_LOW()       				
#define LED_RED_OFF()      				    LED_RED_HIGH()

/*lq LED4, Blue */
#define LED_BLUE_PORT						GPIOC
#define LED_BLUE_RCC_APB2Periph_GPIO  	    RCC_APB2Periph_GPIOC
#define LED_BLUE_PIN						GPIO_Pin_3

#define LED_BLUE_LOW()       				GPIO_ResetBits(LED_BLUE_PORT, LED_BLUE_PIN)
#define LED_BLUE_HIGH()      				GPIO_SetBits(LED_BLUE_PORT, LED_BLUE_PIN)

#define LED_BLUE_ON()                       LED_BLUE_LOW()       				
#define LED_BLUE_OFF()      				LED_BLUE_HIGH()

#if 1
/*测试--------------*/
#define TEST_OUT_PORT			GPIOC
#define TEST_OUT_PIN_1			GPIO_Pin_8
#define TEST_OUT_PIN_2			GPIO_Pin_9

#define TEST_OUT_HIGH_1()       			GPIO_SetBits(TEST_OUT_PORT, TEST_OUT_PIN_1)
#define TEST_OUT_LOW_1()      				GPIO_ResetBits(TEST_OUT_PORT, TEST_OUT_PIN_1)
#define TEST_OUT_READ_1()                   GPIO_ReadOutputDataBit(TEST_OUT_PORT, TEST_OUT_PIN_1)

#define TEST_OUT_HIGH_2()       			GPIO_SetBits(TEST_OUT_PORT, TEST_OUT_PIN_2)
#define TEST_OUT_LOW_2()      				GPIO_ResetBits(TEST_OUT_PORT, TEST_OUT_PIN_2)
#define TEST_OUT_READ_2()                   GPIO_ReadOutputDataBit(TEST_OUT_PORT, TEST_OUT_PIN_2)

extern void InitTestIO(void);
extern void InitPinPulse(void);
extern void PinPulseDown(void);

#endif
extern void InitiaLED(void);



#endif

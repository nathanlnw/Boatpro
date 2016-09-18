/*******************************************************************************
  Copyright (C), 2010, 712.Co.,Ltd.
  File name:	power.h
  Author: 郭长勇      Version: V1.00       Date: 2012.08.16 
  Description:  
  				使用内狗IWDG
				
  Others:         // 其它内容的说明
  Function List:  // 主要函数列表，每条记录应包括函数名及功能简要说明
  History:        // 修改历史记录列表，每条修改记录应包括修改日期、修改
                  // 者及修改内容简述  
    1. Date:
       Author:
       Modification:
*******************************************************************************/

#include "stm32f10x.h"
#include "stm32f10x_iwdg.h"
#include "def.h"
#include "wdg.h"

U8 Is_Feed_Dog;
/********************************************* ***************
*  Function:       Initia_wdg								*
*  Description:    电源控制参数初始化								*
*  Input:          无										*
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
void Initia_wdg(void)
{

//    GPIO_InitTypeDef GPIO_InitStructure;
//
//	GPIO_InitStructure.GPIO_Pin = WDG_PIN;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(WDG_PORT, &GPIO_InitStructure);
	
	*(U32 *)DBGMCU_CR |= (u32)1 << 8;//配置内狗在DEBUG下停止 
	
	/* Enable write access to IWDG_PR and IWDG_RLR registers */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//使能对IWDG的配置
	
	/* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz */
	IWDG_SetPrescaler(IWDG_Prescaler_32);                                       //lq? IWDG使用专用的40kHzLSI?    

	/* Set counter reload value */
  	IWDG_SetReload(2500);//2500-2秒不喂狗就复位 ;最大值4095，对应3.2768S

	/* Reload IWDG counter */
  	IWDG_ReloadCounter();                                                          
	
	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  	IWDG_Enable();
	
	Is_Feed_Dog = FALSE;
}


/************************************************************
*  Function:       Feed_Dog							    	*
*  Description:    喂狗     								*
*  Input:          无										*
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
void Feed_Dog(void)
{
	if(Is_Feed_Dog == TRUE)
	{
		Is_Feed_Dog = FALSE;
		
		/* Reload IWDG counter */
  		IWDG_ReloadCounter();
	}
}

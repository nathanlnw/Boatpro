/****************************************************************************
*  Copyright (C), 2010, 712.Co.,Ltd.		                                *
*  FileName: wdg.h	 	                                  	            	*
*  Description:  看门狗控制头文件											*
*****************************************************************************
*  History:               				                                    *
*      <author>  <time>    <version >   <desc>                              *
*      vozhj   	 10/12/02     1.0       build this moudle		            *
****************************************************************************/
#ifndef __WDG_H
#define __WDG_H

#define WDG_PORT						PUBLIC_PORT
#define WDG_RCC_APB2Periph_GPIO  	    RCC_APB2Periph_GPIOC
#define WDG_PIN						    PUBLIC_PIN
#define WDG_LOW()       				GPIO_SetBits(WDG_PORT, WDG_PIN)//看门狗端口置低
#define WDG_HIGH()      				GPIO_ResetBits(WDG_PORT, WDG_PIN)
#define WDG_TOGGLE()					GPIO_WriteBit(WDG_PORT, WDG_PIN, (BitAction)(1 - GPIO_ReadOutputDataBit(WDG_PORT, WDG_PIN)))

#define DBGMCU_CR (0xE0042004)//DEBUG MCU 寄存器，用于配置DEBUG下内狗停止

void Initia_wdg(void);
extern void Feed_Dog(void);

extern U8 Is_Feed_Dog;
#endif

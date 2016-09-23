/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: Include.h
  Author: lq       Version : V1.0.0        Date: 2015.01.15
  Description:     项目头文件
  Version:         V1.0.0
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _INCLUDE_H_
#define _INCLUDE_H_



/* Include -------------------------------------------------------------------*/
#include <stdlib.h> 
#include <stdio.h> 
#include <string.h>
#include <math.h>
#include "config.h"
#include "def.h"
#include "common.h"
#include "AisDef.h"
#include "stm32f10x.h"
#include "stm32f10x_dac.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_sdio.h"
#include  "stm32f10x_dma.h"
#include  "stm32f10x_gpio.h"
#include  "stm32f10x_rcc.h"
#include  "stm32f10x_exti.h"
#include  "stm32f10x_usart.h"
#include  "stm32f10x_tim.h"
#include  "stm32f10x_rtc.h"
#include  "stm32f10x_spi.h"
#include  "stm32f10x_dbgmcu.h"
#include  "stm32f10x_pwr.h"
#include  "stm32f10x_bkp.h"
#include   "misc.h"

#include "PhysicalLayer.h"
#include "DataLinkService.h"
#include "LinkManagementEntity.h"
#include "si446x_patch.h"
#include "Si446x.h"
#include "Si446xDefs.h"
#include "radio_config_Si4463.h"
#include "TransceiverCtrl.h"
#include "initial.h"
#include "LED.h"
#include "gps.h"
#include "gpsctrl.h"
#include "Slot.h"
#include "TDMA.h"
#include "RxTx.h"
#include "RxTxCtrl.h"
#include "AISMsgModule.h"
#include "AisInfo.h"
#include "AISDigitalInterface.h"
#include "AIS_MsgModu.h"
#include "DeviceInfo.h"
#include "MsgManager.h"
#include "testing.h"
#include "RfLinkSelfCheck.h"
#include "SelfCheck.h"
#include "ledctrl.h"
#include "powerup.h"
#include "DAC.h"
#include "TaskManager.h"

 //  below add by lnw
#include "BKPRegister.h"
#include "migrate.h"
#include "Si446x.h"
#include "power.h"
#include "debug.h"
#include "Tim.h"
#include "uartmsg.h"
#include "wdg.h"
#include "SPI.h"
#include "systemtick.h"
#include "def.h"
#include "UbloxProtocol.h"
#include "AISDigitalInterface.h"
#include "stm32f10x_target.h"
#include "systemtick.h"
#include "Gps.h"
#include "parastorage.h"
#include "sleepctrl.h"





#if  CONFIG_PTT_EN
  #include "PTT_lnw.h"
#endif



#if CONFIG_SD_CARD_MODULE_EN
#include "sdcard.h"
#include "ff.h"
#include "diskio.h"	
#endif

#include "flashctrl.h"
#include "encrypt.h"

/* Public define -------------------------------------------------------------*/

/* Public typedef ------------------------------------------------------------*/
/*lq 公共端口引脚定义（该引脚为无用引脚） */
#define PUBLIC_PORT				        GPIOA
#define PUBLIC_RCC_APB2Periph_GPIO  	RCC_APB2Periph_GPIOA
#define PUBLIC_PIN						GPIO_Pin_11
#define PUBLIC_SOURCE_PORT	        	GPIO_PortSourceGPIOA
#define PUBLIC_SOURCE_PIN	           	GPIO_PinSource11
#define PUBLIC_LINE        				EXTI_Line11

/* Public variables ----------------------------------------------------------*/



/* Public functions ----------------------------------------------------------*/

#endif

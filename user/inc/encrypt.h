/*******************************************************************************
  Copyright (C), 2010, 712.Co.,Ltd.
  File name:	encrypt.h
  Author: 张嘉旺      Version: V1.00       Date: 2016.03.10 
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
*******************************************************************************/

#ifndef __ENCRYPT_H
#define __ENCRYPT_H

/* Includes ------------------------------------------------------------------*/
#include "Include.h"
#include "def.h"
#include "config.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum CryType
{
	CRY_MAIN,
	CRY_BACKUP1,
	CRY_BACKUP2
}CryTypeEnum;

/* Private define ------------------------------------------------------------*/
/*-----------------密文地址定义--------------------*/
#define STM32_FLASH_CRY_BASE_ADDR  				(STM32_FLASH_U8_BASE + 20)					//密文起始存储地址
#define STM32_FLASH_CRY_0_7_ADDR				(STM32_FLASH_CRY_BASE_ADDR + 0)				//密文的0_7位
#define STM32_FLASH_CRY_0_7_BACKUP1_ADDR		(STM32_FLASH_CRY_BASE_ADDR + 2)				//密文备份1的0_7位
#define STM32_FLASH_CRY_0_7_BACKUP2_ADDR		(STM32_FLASH_CRY_BASE_ADDR + 4)				//密文备份2的0_7位
#define STM32_FLASH_CRY_8_15_ADDR				(STM32_FLASH_CRY_BASE_ADDR + 6)				//密文的8_15位
#define STM32_FLASH_CRY_8_15_BACKUP1_ADDR		(STM32_FLASH_CRY_BASE_ADDR + 8)				//密文备份1的8_15位
#define STM32_FLASH_CRY_8_15_BACKUP2_ADDR		(STM32_FLASH_CRY_BASE_ADDR + 10)			//密文备份2的8_15位
#define STM32_FLASH_CRY_16_23_ADDR				(STM32_FLASH_CRY_BASE_ADDR + 12)			//密文的16_23位
#define STM32_FLASH_CRY_16_23_BACKUP1_ADDR		(STM32_FLASH_CRY_BASE_ADDR + 14)			//密文备份1的16_23位
#define STM32_FLASH_CRY_16_23_BACKUP2_ADDR		(STM32_FLASH_CRY_BASE_ADDR + 16)			//密文备份2的16_23位
#define STM32_FLASH_CRY_23_31_ADDR				(STM32_FLASH_CRY_BASE_ADDR + 18)			//密文的24_31位
#define STM32_FLASH_CRY_23_31_BACKUP1_ADDR		(STM32_FLASH_CRY_BASE_ADDR + 20)			//密文备份1的24_31位
#define STM32_FLASH_CRY_23_31_BACKUP2_ADDR		(STM32_FLASH_CRY_BASE_ADDR + 22)			//密文备份2的24_31位
#define STM32_FLASH_CRY_CRC_0_7_ADDR			(STM32_FLASH_CRY_BASE_ADDR + 24)			//密文CRC校验的0_7位
#define STM32_FLASH_CRY_CRC_0_7_BACKUP1_ADDR	(STM32_FLASH_CRY_BASE_ADDR + 26)			//密文CRC校验备份1的0_7位
#define STM32_FLASH_CRY_CRC_0_7_BACKUP2_ADDR	(STM32_FLASH_CRY_BASE_ADDR + 28)			//密文CRC校验备份2的0_7位
#define STM32_FLASH_CRY_CRC_8_15_ADDR			(STM32_FLASH_CRY_BASE_ADDR + 30)			//密文CRC校验的8_15位
#define STM32_FLASH_CRY_CRC_8_15_BACKUP1_ADDR	(STM32_FLASH_CRY_BASE_ADDR + 32)			//密文CRC校验备份1的8_15位
#define STM32_FLASH_CRY_CRC_8_15_BACKUP2_ADDR	(STM32_FLASH_CRY_BASE_ADDR + 34)			//密文CRC校验备份2的8_15位
/* Private variables ---------------------------------------------------------*/

/* Private  function  ---------------------------------------------------------*/

/* Public define ------------------------------------------------------------*/

/* Public  variables ---------------------------------------------------------*/
U32 Decode(U32 ulSrc, U32 *ulKeyBuff, U8 KeySize);
void ReadDeviceID(U32 *ucIDBuff);
U32 ReadCry(U8 ucCryType);
void WriteCry(U8 ucCryType, U8 *ucCryBuff);    // lnw  modify     before    U8 return but  not return in  function
void FlashReadCry(U8 *ucCryBuff, U32 ulAddr);
void FlashWriteCry(U8 *ucCryBuff, U32 ulAddr);

#endif

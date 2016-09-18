/*******************************************************************************
  Copyright (C), 2010, 712.Co.,Ltd.
  File name:	filectrl.h
  Author: 张嘉旺      Version: V1.00       Date: 2015.11.10 
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


#ifndef __FILECTRL_H__
#define __FILECTRL_H__

/* Includes ------------------------------------------------------------------*/
#include "Include.h"
#include "def.h"
#include "sdcard.h"
#include "ff.h"
#include "diskio.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
typedef struct 
{
	U32 MMSI;	 											//设备MMSI
	U8  ShipName[AIS_INFO_SHIP_NAME_LEN + 1];				//船舶名称
	U8  SerialNumber[9];									//设备序列号
	U8  SoftwareVersion[9];									//软件版本号
	U8  HardwareVersion[6];									//硬件版本号
	U8  CallSign[AIS_INFO_CALL_SIGN_LEN + 1];				//呼号
	U8  ShipType;											//船舶类型
	U16 ShipA;												//尺寸A
	U16 ShipB;												//尺寸B
	U8  ShipC;												//尺寸C
	U8  ShipD;												//尺寸D
	U32 IMONumber;											//IMO编号
}FileHeadStruct;

/* Private macro -------------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void CreateFile(void);
void WriteFile(U8 * buff,UINT bufflen);
void ReadFile(char * FileNameBuff);
void InitFileHead(void);
void AdjustUSART1Band(U32 Band);
void ReadFileName(UINT iNumber);
void ReadMulFile(UINT iNumber);
void CtrlAISAndGPS(UINT iFlag);
FRESULT set_timestamp (
    char *obj,     /* Pointer to the file name */
    int year,
    int month,
    int mday,
    int hour,
    int min,
    int sec
);

#endif

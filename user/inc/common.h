/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	Common.h
  Author: 王伟      Version: V1.00       Date: 2011.09.23 
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
#ifndef __COMMON_H__
#define __COMMON_H__

/* Include -------------------------------------------------------------------*/
#include "Include.h"

/* Public define -------------------------------------------------------------*/
#define KEY_NONE		0
#define KEY_UP			1
#define KEY_DOWN		2
#define KEY_PRESS		3

/*lq 日期时间宏定义 */
#define DATE_TIME_IS_HOUR_VALID(hour)   (((hour >= 0) && (hour < 24)) ? TRUE : FALSE)
#define DATE_TIME_IS_MINUTE_VALID(min)  (((min >= 0) && (min < 60)) ? TRUE : FALSE)
#define DATE_TIME_IS_SECOND_VALID(sec)  (((sec >= 0) && (sec < 60)) ? TRUE : FALSE)
#define DATE_TIME_IS_MONTH_31DAYS(mon)  (((mon == 1) || (mon == 3) || (mon == 5) || \
                                         (mon == 7) || (mon == 8) || (mon == 10) || \
                                         (mon == 12)) ? TRUE : FALSE)
#define DATE_TIME_IS_MONTH_30DAYS(mon)  (((mon == 4) || (mon == 6) || (mon == 9) || \
                                         (mon == 11)) ? TRUE : FALSE)
#define DATE_TIME_IS_YEAR_LEAP(year)    (((year % 100 == 0) && (year % 400 != 0)) ? TRUE : FALSE)

#define DATE_TIME_SECOND_28DAYS         2419200

#define DATE_TIME_INVALID_VALUE_YEAR    0
#define DATE_TIME_INVALID_VALUE_MONTH   0
#define DATE_TIME_INVALID_VALUE_DAY     0
#define DATE_TIME_INVALID_VALUE_HOUR    24
#define DATE_TIME_INVALID_VALUE_MINUTE  60
#define DATE_TIME_INVALID_VALUE_SECOND  60
   
/* Public typedef ------------------------------------------------------------*/
typedef struct 
{
    S16 Year;
    S16 Month;
    S16 Day;
    S16 Hour;
    S16 Minute;
    S16 Second;
}DateTimeStruct;

/* Public variables ----------------------------------------------------------*/
extern S16 mach_dete_handle;
extern S16 comm1_handle;
extern S16 GPS_handle; 
extern S16 IO_set_handle;
extern S16 LCD_handle;
extern S16 bell_handle;
extern S16 SCT3252_handle;
extern S16 Flash_handle;
extern S16 Record_handle;
extern S16 Moto_handle;
extern const U16 ARM_STATE;/*A.B.C = 0xAB.2C*/

/* Public functions ----------------------------------------------------------*/
U8 HexToAscII(U8 Data);
U8 UartMsgCheckSum(U8 *pData, U16 Len);
void DataToAscII(U16 *pData, U8 Len, char *pString);
U16 Cal_Crc16(U8 *ucStarAddr, U32 ucDataLen);		//ucStarAddr为需作crc校验的数组首地址，ucDataLen为crc校验的数据个数
U16 ChkSum(U16 *pBuf, U16 size);
void ByteToIntDataCopy(U16 *Dest, U8 *Sour, U8 Len);
void Delay_us(U32 nTime);
void Delay_ms(U32 nTime);
U8 EEpromChkSum(U8 *pData,  U16 Len);
U16 AIS_Cal_CRC16(U8 *data, U16 count);

//  lnw  add
extern S32 Random(S32 bound1, S32 bound2);

#endif
/*==============================================================================
------------------------------------End of File---------------------------------
==============================================================================*/


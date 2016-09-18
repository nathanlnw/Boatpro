/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	Debug.h
  Author: 邓志刚      Version: V1.00       Date: 2010.09.10 
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
#ifndef __DEBUG_H__
#define __DEBUG_H__

#define MMI_DEBUG_MSG	0
#define PTL_DEBUG_MSG	1
#define ARM_DEBUG_MSG	2
#define DSP_DEBUG_MSG	3
#define PD_DEBUG_MSG	4
#define FLASH_DEBUG_MSG 5
#define NO_DEBUG_MSG    10

extern U8 DebugOutputFlag;

U8 UartTxString(char *pStr);
void usart_printf(char * format, ...);

U8 UartDebugMsg(U8 Type,  U8 Addr,  char * format,...);

U8 UartResponseMsg(char *pData);
U8 UartTxString(char *pStr);

#endif





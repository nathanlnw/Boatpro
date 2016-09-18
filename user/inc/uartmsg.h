/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	uartmsg.h
  Author: vozhj     Version: V1.00       Date: 2012.03.21 
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
#ifndef _UARTMSG_H
#define _UARTMSG_H
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LON_LAT_PRECISION   4   //lq 经纬度精度 4 - 0.0001分

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/
extern void IniUartMsgPara(void);

/**
	@ingroup DEBUG 
	@brief 串口调试报文解析
	@param pData 报文指针
	@param uiLen 报文长度
	@retval TRUE 解析成功
	@retval FALSE 解析失败
*/
extern U8 UartMsgProcess(U8 *pData, U16 uiLen);
extern U8 GetSegment(char *pInput, char *Output, U16 SegmentIndex, U16 SumLength, U16 Max_Bytes);
#endif

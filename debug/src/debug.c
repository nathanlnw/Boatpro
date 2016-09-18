/*******************************************************************************
  Copyright (C),  2011,  712.Co., Ltd.
  FileName: debug.c
  Author: 王伟       Version : V1.00        Date: 2011.09.23
  Description:    通过串口输出debug信息	
  Version:         V1.00
  Function List:   // 主要函数及其功能    	 
			1．函数名：UartDebugMsg
    	   	   函数功能：串口调试报文输出	   	    	   	     	   	
    	   	   
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include <locale.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <stdarg.h>

#include "def.h"
#include "debug.h"
#include "usart.h"
#include "common.h"
#include "usart.h"
#include "config.h"

U8 DebugOutputFlag = FALSE;             //lq 默认值为FALSE，即默认不输出
//U8 DebugOutputFlag = TRUE;              //lq 默认值为TRUE，即默认输出

/*******************************************************************************
* 名称 ：UartTxString
* 描述 ：串口发送字符串
* 输入 ：COMM：串口地址标志
			COMM1：
		 pStr：字符串指针
		 Length：字符串长度
* 输出 ：无
* 返回 ：无
*******************************************************************************/
U8 UartTxString(char *pStr)
{				
	//U8 uchRet;

//	DCB_write(comm1_handle, 0, 0, 0, (U8*)pStr, strlen(pStr));
	UartTxDataBuf(COMM1, (U8*)pStr, strlen(pStr));
#if CONFIG_SD_CARD_MODULE_EN
	WriteFile((U8*)pStr,strlen(pStr));
#endif
	return TRUE;		
}


/*******************************************************************************
* 名称 ：usart_printf
* 描述 ：串口发送字符串
* 输入 ：COMM：串口地址标志
			COMM0：
			COMM1：
			COMM2：
		 format：print格式
* 输出 ：无
* 返回 ：无
*******************************************************************************/
void usart_printf(char * format,...)
{
	va_list ap;
	static char string[256];

	va_start(ap, format);
	//sprintf(string, format, ap);
	vsprintf(string, format, ap);
	UartTxString(string);
	va_end(ap);
}

/*******************************************************************************
* 名称 ：debug_printf
* 描述 ：串口发送调试字符串
* 输入 ：format：print格式
* 输出 ：无
* 返回 ：无
*******************************************************************************/
void debug_printf(char * format,...)
{
#if CONFIG_DEBUG_PRINT_EN
	va_list ap;
	static char string[256];

    /*lq 此处可以改用变量控制输出 */
    //lq...
    if (DebugOutputFlag == TRUE)
    {
    	va_start(ap, format);
    	//sprintf(string, format, ap);
    	vsprintf(string, format, ap);
    	UartTxString(string);
    	va_end(ap);
    }
#endif
}

/*******************************************************************************
* 名称  : UartDebugMsg
* 描述  : 串口调试报文输出
* 输入  : Type：类型
			MMI_DEBUG_MSG
			PTL_DEBUG_MSG
			ARM_DEBUG_MSG
			DSP_DEBUG_MSG
			PD_DEBUG_MSG			
		  Addr：0x00~0x30
		  pData：字符串指针
* 输出  : 无
* 返回  : FALSE：失败；TRUE：成功
*******************************************************************************/
U8 UartDebugMsg(U8 Type,  U8 Addr,  char * format,...)
{
	va_list ap;
	static char string[256];
	U16 Index;
	U8 Check;

//	OS_ENTER_CRITICAL();
//	if(wsWorkState.DebugInfoEnable == FALSE)
//	{
//		OS_EXIT_CRITICAL();
//		return FALSE;
//	}
//	OS_EXIT_CRITICAL();

	switch(Type)
	{
		case DSP_DEBUG_MSG:
			Addr += 0x00;
			break;
		case PTL_DEBUG_MSG:
			Addr += 0x30;
			break;
		case MMI_DEBUG_MSG:
			Addr += 0x60;
			break;
		case ARM_DEBUG_MSG:
			Addr += 0x90;
			break;
		case PD_DEBUG_MSG:
			Addr += 0xC0;
			break;
		default:
			break;
		
	}	
	if(Type < 10)
	{
		sprintf(string, "$T712,TXT,%.2X,,", Addr);
		Index = strlen(string);
		va_start(ap,format);
		//sprintf(string,format,ap);
		vsprintf(&string[Index],format,ap);
		
		Check = UartMsgCheckSum((U8 *)&string[1],  strlen(string) - 1);	//对$和*号之间的数据做校验
		sprintf(&string[strlen(string)], "*%.2X\r\n", Check);
	}
	else
	{
		va_start(ap,format);
		vsprintf(&string[0],format,ap);
		sprintf(&string[strlen(string)], "\r\n");
	}
	
	UartTxString(string);
	va_end(ap);
	
	return TRUE;

}

/*******************************************************************************
* 名称  : UartResponseMsg
* 描述  : 串口应答报文输出
* 输入  : pData：字符串指针
* 输出  : 无
* 返回  : FALSE：失败；TRUE：成功
*******************************************************************************/
U8 UartResponseMsg(char *pData)	  //输出字符串专用
{
	U16 i, Index = 0;
	U16 Len;
	U8 Check = 0;
	char UartDebugBuff[100];
		
	Len = strlen(pData);

	if(Len > 90)
	{
		return FALSE;
	}
	
	for (i=0; i<Len; i++)
	{
		UartDebugBuff[Index++] = pData[i];
	}
	
	if(Index <= 1)
	{
		Check = UartMsgCheckSum((U8 *)&UartDebugBuff[1], Index);//对$和*号之间的数据做校验
	}
	else
	{
		Check = UartMsgCheckSum((U8 *)&UartDebugBuff[1], Index - 1);//对$和*号之间的数据做校验
	}

	UartDebugBuff[Index++] = '*';
	UartDebugBuff[Index++] = HexToAscII(Check >> 4);
	UartDebugBuff[Index++] = HexToAscII(Check & 0xf);
	UartDebugBuff[Index++] = '\r';
	UartDebugBuff[Index++] = '\n';
	UartDebugBuff[Index++] = '\0';

	UartTxString(UartDebugBuff);
	
	return TRUE;
}

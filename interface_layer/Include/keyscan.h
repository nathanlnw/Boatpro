/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	interfunc.h
  Author: 李斌      Version: V1.00       Date: 2012.03.21 
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
#ifndef _KEYSCAN_H
#define _KEYSCAN_H
/* Private typedef -----------------------------------------------------------*/

/**
	@ingroup MMI 
	@brief 按键结构体，包含用于按键计时和防抖动等
*/
typedef struct {
	U16 TimeCnt;
	U16 TimePeriod;	
	
	U8 XDouCnt;
	U8 Val;
	U8 ChangeFlg;					
} SingleKeyStruct;

extern SingleKeyStruct PowerKey;
extern SingleKeyStruct SOSKey;
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
#define KEY_POWER_ADDR		0
#define KEY_SOS_ADDR		1

#define KEY_LOW				0
#define KEY_HIGH			1
/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/
extern void IniSingleKey(U8 IO_Addr, U8 InitVal);
extern U8 ReadSingleKeyValProcess(SingleKeyStruct *pSingleKeyStruct);

/**
	@ingroup MMI
	@brief 读取按键状态接口
	@param IO_Addr 按键为SOS还是POWER
	@retval KEY_HIGH 按键接口高电平
	@retval KEY_LOW 按键接口低电平
*/
extern U8 GetIndivIO(U8 IO_Addr);

/**
	@ingroup MMI
	@brief 按键扫描主函数
	@param 无
	@retval 无
*/
extern void KeyScanProcess(void);

/**
	@ingroup MMI
	@brief 按键扫描
	@param pSingleKeyStruct 按键对应的结构体
	@retval 无
*/
extern void SingleKeyCtrl(SingleKeyStruct *pSingleKeyStruct);
#endif

/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: main.c
  Author: 王伟       Version : V1.00        Date: 2011.09.23
  Description:    项目中挂载UCOS操作系统
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			1．函数名：Main
    	   函数功能：控制ARM整个程序的架构
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include "config.h"

#if CONFIG_KEY
#include "stm32f10x.h"
#include "def.h"
#include "systemtick.h"
#include "keyscan.h"
#include "power.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private  function  --------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
SingleKeyStruct PowerKey;
SingleKeyStruct SOSKey;
/* Public  function  ---------------------------------------------------------*/


/*******************************************************************************
* 名称  : SingleKeyStateCtrlProcess
* 描述  : 独立按键状态控制处理(应用接口函数)
* 输入  : IO_Addr：按键地址
*		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void IniSingleKey(U8 IO_Addr, U8 InitVal)
{	
	SingleKeyStruct *pSingleKeyStruct;
	
	if(IO_Addr == KEY_POWER_ADDR)
	{
		pSingleKeyStruct = &PowerKey;
	}
	else if(IO_Addr == KEY_SOS_ADDR)
	{
		pSingleKeyStruct = &SOSKey;
	}
		
	pSingleKeyStruct->TimePeriod = 1;
	pSingleKeyStruct->XDouCnt = 0;
	pSingleKeyStruct->Val = InitVal;
	//pSingleKeyStruct->Val = ReadSingleKeyValProcess(pSingleKeyStruct);
	pSingleKeyStruct->ChangeFlg = FALSE;



	pSingleKeyStruct->TimeCnt = SetTimePeriodProcess(pSingleKeyStruct->TimePeriod);
}

/*******************************************************************************
* 名称  : ReadSingleKeyValProcess
* 描述  : 读独立按键值处理(底层驱动函数)
* 输入  : pSingleKeyStruct：
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 ReadSingleKeyValProcess(SingleKeyStruct *pSingleKeyStruct)
{	
	U8 KeyVal = 0;
	
	if (pSingleKeyStruct == &PowerKey)
	{
		if (READ_POWER_ON_PIN() == FALSE)				//端口为低电平
		{
			KeyVal = KEY_LOW; 
		}
		else										//端口为高电平
		{
			KeyVal = KEY_HIGH; 
		}
	}
	else if (pSingleKeyStruct == &SOSKey)
	{		
		if (READ_POWER_EMERGENCY_PIN() == FALSE)				//端口为低电平
		{
			KeyVal = KEY_LOW; 
		}
		else										//端口为高电平
		{
			KeyVal = KEY_HIGH; 
		}
	}
	
	return KeyVal;
}
/*******************************************************************************
* 名称  : SingleKeyCtrlMainProcess
* 描述  : 独立按键控制主程序处理
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SingleKeyCtrl(SingleKeyStruct *pSingleKeyStruct)
{	
	U8 KeyVal;
			
	if (GetLocalClockTickProcess() == pSingleKeyStruct->TimeCnt)
	{				
		pSingleKeyStruct->TimeCnt = SetTimePeriodProcess(pSingleKeyStruct->TimePeriod);								
	
		KeyVal = ReadSingleKeyValProcess(pSingleKeyStruct);
		
		if (KeyVal != pSingleKeyStruct->Val)
		{
			pSingleKeyStruct->XDouCnt++;
			if (pSingleKeyStruct->XDouCnt >= 2)
			{
				pSingleKeyStruct->XDouCnt = 0;
				
				pSingleKeyStruct->Val = KeyVal;
				pSingleKeyStruct->ChangeFlg = TRUE;
			}
		}
		else
		{
			pSingleKeyStruct->XDouCnt = 0;
		}	
	}	
}

/*******************************************************************************
* 名称  : SingleKeyCtrlMainProcess
* 描述  : 独立按键控制主程序处理
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void KeyScanProcess(void)
{
	SingleKeyCtrl(&PowerKey);
	SingleKeyCtrl(&SOSKey);	
}
/*******************************************************************************
* 名称  : GetIndivIO
* 描述  : 获取独立IO值
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GetIndivIO(U8 IO_Addr)
{
	if(IO_Addr == KEY_POWER_ADDR)
	{
		return PowerKey.Val;
	}
	else if(IO_Addr == KEY_SOS_ADDR)
	{
		return SOSKey.Val;
	}
	else
	{
		return 0xFF;
	}
}
#endif

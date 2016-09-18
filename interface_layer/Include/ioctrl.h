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
#ifndef _IOCTRL_H
#define _IOCTRL_H
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
#define IO_LED_GREEN_TYPE		0		//Green LED设置
#define IO_LED_YELLOW_TYPE		1		//Yellow LED设置
#define IO_LED_RED_TYPE		    2		//Red LED设置
#define IO_LED_BLUE_TYPE		3		//Blue LED设置
#define IO_POWER_TYPE		    4	 	//整机电源控制
#define IO_GPSPOWER_TYPE	    5	   	//GPS电源控制
#define IO_RDA1846_1_TYPE	    6		//RDA1846控制
#define IO_RDA1846_2_TYPE	    7		//RDA1846控制
#define IO_RX_EN_TYPE           8       //接收控制
#define IO_ANT_SW_RX_TYPE       9       //天线转换控制
#define IO_RDA1846_1_TX_TYPE    10      
#define IO_RDA1846_1_RX_TYPE    11
#define IO_RDA1846_2_TX_TYPE    12
#define IO_RDA1846_2_RX_TYPE    13
#define IO_RF_VCC_TYPE          14

#define IO_VAL_LOW			0 
#define IO_VAL_HIGH			1 

#define IO_VAL_OFF			0 
#define IO_VAL_ON			1 

/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/
extern void IO_Ctrl(U8 ucType, U8 ucVal);
extern void IO_Write(U8 ucType, U32 ulVal);
#endif

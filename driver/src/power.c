/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: Usart.c
  Author: 李斌       Version : V1.00        Date: 2012.03.12
  Description:    串口底层驱动和应用处理程序
  Version:         V1.00
  Function List:   // 主要函数及其功能    	 
			1．函数名：Initia_POWER_Crl
    	   	   函数功能：初始化按键
 
			2．函数名：
    	   	   函数功能：串口2
    	   	   
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_dac.h"
#include "def.h"
#include "common.h"
#include "power.h"
#include "dac.h"
#include "initial.h"
#include "config.h"

PowerStru	PowerPara;
RFStruct RFPara;

U16 arrRFCtrlVoltVal[RF_DA_CTRL_VOLT_LEVEL_NUM] = {3250, 3380, 3500, 3630, 3750};
                                                 //2.6v  2.7v  2.8v  2.9v  3.0v                                                 
                                                 //{3510, 3620, 3740, 4095, 4095};
                                                 //2.8v  2.9v  3.0v  3.18v 3.18v
                                                 //{3620, 3740, 3860, 4095, 4095};
                                                 //2.9v  3.0v  3.1v  3.18v 3.18v
/************************************************************
*  Function:       InitPowerStru							*
*  Description:    按键状态初始化						    *
*  Input:          无										*
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
void InitPowerStru(void)
{
	PowerPara.SosKeyFlag = FALSE;
	PowerPara.PowerKeyFlag = FALSE;	
}

/*******************************************************************************
* 名称  : RF_RFStructInit
* 描述  : RFStruct结构体初始化
* 输入  : pRFStruct-RFStruct类型指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void RF_RFStructInit(RFStruct *pRFStruct)
{
    pRFStruct->RFAntState = TRUE;   
}

/*******************************************************************************
* 名称  : RF1_RFInstanceInit
* 描述  : RFStruct结构体初始化
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void RF1_RFInstanceInit(void)
{
    RF_RFStructInit(&RFPara);
}

/*******************************************************************************
* 名称 ：RF_GetRFAntState
* 描述 ：查询RF天线状态
* 输入 ：pRFStruct：RFStruct类型指针
* 输出 ：无
* 返回 ：无
*******************************************************************************/
static U8 RF_GetRFAntState(RFStruct *pRFStruct)
{
    if (pRFStruct->RFAntState == TRUE)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称 ：RF1_GetRFAntState
* 描述 ：查询RF天线状态
* 输入 ：无
* 输出 ：无
* 返回 ：无
*******************************************************************************/
U8 RF1_GetRFAntState(void)
{
    return RF_GetRFAntState(&RFPara);    
}

#if CONFIG_KEY
/************************************************************
*  Function:       Initia_POWER_Crl						    *
*  Description:    电源控制参数初始化，按键初始化			*
*  Input:          无										*
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
void Initia_POWER_Crl(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
			
	GPIO_InitStructure.GPIO_Pin = POWER_ON_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY_PORT, &GPIO_InitStructure);                                      //lq 配置ARM检测电源键开关电平的引脚

	GPIO_InitStructure.GPIO_Pin = POWER_EMERGENCY_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY_PORT, &GPIO_InitStructure);                                      //lq 配置ARM检测测试键开关电平的引脚

	/*lq 按键中断设置*/
	/* Connect EXTI Line to GPIO Pin*/ 
	GPIO_EXTILineConfig(POWER_SOURCE_PORT, POWER_ON_SOURCE_PIN);
	
	/* Configure EXTI Line to generate an interrupt on falling edge */  
	EXTI_InitStructure.EXTI_Line = POWER_ON_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Connect EXTI Line to GPIO Pin */
	GPIO_EXTILineConfig(POWER_SOURCE_PORT, POWER_EMERGENCY_SOURCE_PIN);
	
	/* Configure EXTI Line to generate an interrupt on falling edge */  
	EXTI_InitStructure.EXTI_Line = POWER_EMERGENCY_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}
#endif

#if CONFIG_PA_GRID_DA_CTRL
/************************************************************
*  Function:       Initia_RFPower							*
*  Description:    射频功放控制初始化(通过DA控制)			*
*  Input:          无										*
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
void Initia_RFPower(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef  DAC_InitStructure;

    /*lq RF电路LDO */
	GPIO_InitStructure.GPIO_Pin = RF_VCC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RF_VCC_PORT, &GPIO_InitStructure);
    RF_VCC_ON();                                  //lq 使能RF LDO
    	
    /*lq 第一级功放栅压通过I/O控制*/
	GPIO_InitStructure.GPIO_Pin = RF_POWER_1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RF_POWER_1_PORT, &GPIO_InitStructure);

    /*lq 后两级功放通过DA控制*/
	/* Configure DAC channe1 outputs pins */
	GPIO_InitStructure.GPIO_Pin =  RF_POWER_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(RF_POWER_PORT, &GPIO_InitStructure); 

	/* DAC channel1 Configuration */
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	// DAC_Init(DAC_Channel_2, &DAC_InitStructure);

	/* Enable DAC Channel1 */
	DAC_Cmd(DAC_Channel_1, ENABLE);
	/* Enable DAC Channel2 */
	// DAC_Cmd(DAC_Channel_2, ENABLE);

	RF_PowerDown();
}
#else
/************************************************************
*  Function:       Initia_RFPower							*
*  Description:    射频功放控制初始化						*
*  Input:          无										*
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
void Initia_RFPower(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
	//按照新原理图配置
	GPIO_InitStructure.GPIO_Pin = RF_POWER_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RF_POWER_PORT, &GPIO_InitStructure);
	
	RF_POWER_HIGH();                                                            //lq 初始化功放关闭
//	RF_POWER_LOW();
}
#endif

/************************************************************
*  Function:       RF_PowerDown							*
*  Description:    射频功放关闭(通过DA控制)			*
*  Input:          无										*
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
void RF_PowerDown(void)
{
    /*lq 2、3级功放控制*/
    SetDAC1Val(0);

    /*lq 1级功放控制*/
    RF_POWER_1_LOW(); 

//    usart_printf("RFCtrlVoltVal : %4d", 0);
}

/************************************************************
*  Function:       RF_PowerOn							*
*  Description:    射频功放开启(通过DA控制)			*
*  Input:          无										*
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
void RF_PowerOn(void)
{
    /*lq 1级功放控制*/
    RF_POWER_1_HIGH(); 
 
    /*lq 2、3级功放控制*/
#if CONFIG_BAT_MONITOR 
    if (BS1_GetCurVoltValue() >= RF_DA_CTRL_VOLT_LEVEL_3)
    {
        SetDAC1Val(arrRFCtrlVoltVal[0] + eqEquipPara.GridVoltOffset);

//        usart_printf("RFCtrlVoltVal : %4d", arrRFCtrlVoltVal[0]);
    }
    else if (BS1_GetCurVoltValue() >= RF_DA_CTRL_VOLT_LEVEL_2)
    {
        SetDAC1Val(arrRFCtrlVoltVal[1] + eqEquipPara.GridVoltOffset);

//        usart_printf("RFCtrlVoltVal : %4d", arrRFCtrlVoltVal[1]);
    }
    else if (BS1_GetCurVoltValue() >= RF_DA_CTRL_VOLT_LEVEL_1)
    {
        SetDAC1Val(arrRFCtrlVoltVal[2] + eqEquipPara.GridVoltOffset);

//        usart_printf("RFCtrlVoltVal : %4d", arrRFCtrlVoltVal[2]);
    }
    else if (BS1_GetCurVoltValue() >= RF_DA_CTRL_VOLT_LEVEL_0)
    {
        SetDAC1Val(arrRFCtrlVoltVal[3] + eqEquipPara.GridVoltOffset);

//        usart_printf("RFCtrlVoltVal : %4d", arrRFCtrlVoltVal[3]);
    }
    else
    {
        SetDAC1Val(arrRFCtrlVoltVal[3] + eqEquipPara.GridVoltOffset);

//        usart_printf("RFCtrlVoltVal : %4d", arrRFCtrlVoltVal[3]);
    }
#else
    SetDAC1Val(4095);
#endif
}

/*******************************************************************************
* 名称  : Exti3_Int
* 描述  : 外部中断服务程序(Power按键)
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void Exti15_10_Int_PowerKey(void)
{
	if(EXTI_GetITStatus(POWER_ON_LINE) != RESET)
	{		 
		/* Clear the Key Button EXTI line pending bit */  
		EXTI_ClearITPendingBit(POWER_ON_LINE);//下降沿触发中断
		
		PowerPara.PowerKeyFlag = TRUE; 							
	}
}

/*******************************************************************************
* 名称  : Exti4_Int
* 描述  : 外部中断服务程序(SOS按键)
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void Exti15_10_Int_SosKey(void)
{
	if(EXTI_GetITStatus(POWER_EMERGENCY_LINE) != RESET)
	{ 
		/* Clear the Key Button EXTI line pending bit */  
		EXTI_ClearITPendingBit(POWER_EMERGENCY_LINE);//下降沿触发中断

		PowerPara.SosKeyFlag = TRUE;
	}
}

/*******************************************************************************
* 名称  : GetSosLowPowerFlag
* 描述  : 获得SOS按键在休眠下状态，一次有效
* 输入  : 无
* 输出  : 无
* 返回  : SOS状态
*******************************************************************************/
U8 GetSosLowPowerSta(void)
{
	if (PowerPara.SosKeyFlag == TRUE)
	{
		PowerPara.SosKeyFlag = FALSE;

		return (TRUE);
	}		

	return (FALSE);
}

/*******************************************************************************
* 名称  : GetPowerLowPowerSta
* 描述  : 获得Power按键在休眠下状态，一次有效
* 输入  : 无
* 输出  : 无
* 返回  : Power状态
*******************************************************************************/
U8 GetPowerLowPowerSta(void)
{
	if (PowerPara.PowerKeyFlag == TRUE)
	{
		PowerPara.PowerKeyFlag = FALSE;

	    return (TRUE);
	}		

	return (FALSE);
}

/************************************************************
*  Function:       InitPowerCtrl							*
*  Description:    电源控制参数初始化						*
*  Input:          无										*
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
void InitPowerCtrl(void)
{
  #if  0    // mask by Nathan  , because  it is no use actuallly
	GPIO_InitTypeDef GPIO_InitStructure;
    	
    /*lq 电源开关控制引脚配置*/
	GPIO_InitStructure.GPIO_Pin = POWER_CRL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(POWER_PORT, &GPIO_InitStructure);                                      //lq 配置ARM控制电源D触发器的D

	GPIO_InitStructure.GPIO_Pin = POWER_CLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(POWER_PORT, &GPIO_InitStructure);                                      //lq 配置ARM控制电源D触发器的CLK
  #endif
  
#if 0
    /*lq 初始化电源控制引脚无效，使用D触发器时不应进行如下设置
    **  （因每次从standby唤醒后下面配置会使系统掉电）
    */
    POWER_CRL_LOW();
    POWER_CLK_LOW();
    Delay_ms(10);
    POWER_CLK_HIGH();
#endif
}

/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: PhysicalLayer.c
  Author: lq       Version : V1.00        Date: 2014.07.11
  Description:     AIS物理层
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "def.h"
#include "PhysicalLayer.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static NRZIStruct NRZIDecode1;
static NRZIStruct NRZIDecode2;
 
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* 名称  : PHY_NRZIStructInit
* 描述  : 初始化结构体
* 输入  : pNRZIStruct - NRZIStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void PHY_NRZIStructInit(NRZIStruct *pNRZIStruct)
{
    pNRZIStruct->RunningFlg = FALSE;
    pNRZIStruct->InitialValue = 1;
    pNRZIStruct->PriorValue = 1;    
}

/*******************************************************************************
* 名称  : PHY_InstNRZIStructInit
* 描述  : 初始化结构体
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void PHY_InstNRZIStructInit(void)
{
    PHY_NRZIStructInit(&NRZIDecode1);    
    PHY_NRZIStructInit(&NRZIDecode2);        
}

/*******************************************************************************
* 名称  : PHY_NRZIDecodeStart
* 描述  : NRZI解码开始
* 输入  : pNRZIStruct - NRZIStruct结构体指针
          initValue - 初始值	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void PHY_NRZIDecodeStart(NRZIStruct *pNRZIStruct, const U8 initValue)
{
    pNRZIStruct->RunningFlg = TRUE;
    pNRZIStruct->InitialValue = initValue & 0x01;
    pNRZIStruct->PriorValue = pNRZIStruct->InitialValue;    
}

/*******************************************************************************
* 名称  : PHY_InstNRZIDecodeStart
* 描述  : NRZI解码开始
* 输入  : NRZIType - NRZI类型
          initValue - 初始值	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void PHY_InstNRZIDecodeStart(const NRZITypeEnum NRZIType, const U8 initValue)
{
    if (NRZIType == NRZIType1)
    {
        PHY_NRZIDecodeStart(&NRZIDecode1, initValue);    
    }
    else if (NRZIType == NRZIType2) 
    {
        PHY_NRZIDecodeStart(&NRZIDecode2, initValue);
    }    
}

/*******************************************************************************
* 名称  : PHY_NRZIDecodeStop
* 描述  : NRZI解码停止
* 输入  : pNRZIStruct - NRZIStruct结构体指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void PHY_NRZIDecodeStop(NRZIStruct *pNRZIStruct)
{
    pNRZIStruct->RunningFlg = FALSE;
}

/*******************************************************************************
* 名称  : PHY_InstNRZIDecodeStop
* 描述  : NRZI解码停止
* 输入  : NRZIType - NRZI类型
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void PHY_InstNRZIDecodeStop(const NRZITypeEnum NRZIType)
{
    if (NRZIType == NRZIType1)
    {
        PHY_NRZIDecodeStop(&NRZIDecode1);    
    }
    else if (NRZIType == NRZIType2) 
    {
        PHY_NRZIDecodeStop(&NRZIDecode2);
    }
}

/*******************************************************************************
* 名称  : PHY_NRZIDecode
* 描述  : NRZI解码
* 输入  : pNRZIStruct - NRZIStruct结构体指针
          curValue - 当前值
          pDecodeValue - 指向存放解码值的指针	  
* 输出  : 无
* 返回  : TRUE - 解码值有效，FALSE - 解码值无效 
*******************************************************************************/
static U8 PHY_NRZIDecode(NRZIStruct *pNRZIStruct, const U8 curValue, U8 *pDecodeValue)
{
    if (pNRZIStruct->RunningFlg == TRUE)
    {
        if ((curValue & 0x01) == pNRZIStruct->PriorValue)
        {
            *pDecodeValue = 1;
        }
        else
        {
            *pDecodeValue = 0;
        }
    
        pNRZIStruct->PriorValue = curValue & 0x01;

        return TRUE;
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* 名称  : PHY_InstNRZIDecode
* 描述  : NRZI解码
* 输入  : NRZIType - NRZI类型
          curValue - 当前值
          pDecodeValue - 指向存放解码值的指针	  
* 输出  : 无
* 返回  : TRUE - 解码值有效，FALSE - 解码值无效 
*******************************************************************************/
U8 PHY_InstNRZIDecode(const NRZITypeEnum NRZIType, const U8 curValue, U8 *pDecodeValue)
{
    if (NRZIType == NRZIType1)
    {
        return PHY_NRZIDecode(&NRZIDecode1, curValue, pDecodeValue);    
    }
    else if (NRZIType == NRZIType2) 
    {
        return PHY_NRZIDecode(&NRZIDecode2, curValue, pDecodeValue);        
    }
    else
    {
        return FALSE;    
    }
}

/*******************************************************************************
* 名称  : PHY_ModuleInit
* 描述  : 物理层模块初始化
* 输入  : 无	  
* 输出  : 无
* 返回  : 无 
*******************************************************************************/
void PHY_ModuleInit(void)
{
    PHY_InstNRZIStructInit();
    PHY_InstNRZIDecodeStart(NRZIType1, 1);
    PHY_InstNRZIDecodeStart(NRZIType2, 1);
}

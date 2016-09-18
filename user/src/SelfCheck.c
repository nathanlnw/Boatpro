/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: SelfCheck.c
  Author: lq       Version : V1.00        Date: 2014.07.17
  Description:     设备自检模块
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
#include "SelfCheck.h"
#include "systemtick.h"
#include "24c02.h"
#include "gpsctrl.h"
#include "batteryctrl.h"
#include "rtc.h"
#include "ledctrl.h"
#include "Si446x.h"
#include "RfLinkSelfCheck.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
static SelfCheckStruct SelfCheckInst;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
 
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* 名称  : SLFCHK_SelfCheckStructInit
* 描述  : 初始化结构体
* 输入  : pSelfCheckStruct - SelfCheckStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SLFCHK_SelfCheckStructInit(SelfCheckStruct *pSelfCheckStruct)
{
	pSelfCheckStruct->TimeCnt = 0;
	pSelfCheckStruct->TimePeriod = SLFCHK_TIME_PERIOD;	
    pSelfCheckStruct->TimeoutCnt = 0;
        
    pSelfCheckStruct->CtrlState = SLFCHK_CTRL_STATE_IDLE;
    pSelfCheckStruct->StartFlg = FALSE;
    pSelfCheckStruct->CompleteFlg = FALSE;
    pSelfCheckStruct->CheckResult = 0;
    pSelfCheckStruct->CompleteBitFlg = 0;
    pSelfCheckStruct->CheckSelect = SLFCHK_ITEM_MASK_ALL;
/*lq
    pSelfCheckStruct->PowerSupplyVoltOkFlg = FALSE;
    pSelfCheckStruct->RtcLseOkFlg = FALSE;
    pSelfCheckStruct->GpsVccCtrlOkFlg = FALSE;
    pSelfCheckStruct->GpsTimePulseOkFlg = FALSE;
    pSelfCheckStruct->GpsCommOkFlg = FALSE;
    pSelfCheckStruct->GpsAntennaOkFlg = FALSE;
    pSelfCheckStruct->GpsSelfCheckOkFlg = FALSE;
    pSelfCheckStruct->Rda1846VccCtrlOkFlg = FALSE;
    pSelfCheckStruct->Rda1846CommOkFlg = FALSE;
    pSelfCheckStruct->EepromWriteProtectOkFlg = FALSE;
    pSelfCheckStruct->EepromCommOkFlg = FALSE;
*/
}

/*******************************************************************************
* 名称  : SLFCHK_InstSelfCheckStructInit
* 描述  : 初始化结构体
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SLFCHK_InstSelfCheckStructInit(void)
{
    SLFCHK_SelfCheckStructInit(&SelfCheckInst);
}

/*******************************************************************************
* 名称  : SLFCHK_GetBitNum32
* 描述  : 根据掩码值返回位序号
* 输入  : bitMask - 位掩码(位掩码中的1必须连续)	  
* 输出  : 无
* 返回  : 掩码来对应的位序号（即位掩码中最低位的1的位序号）
*******************************************************************************/
U8 SLFCHK_GetBitNum32(const U32 bitMask)
{
    U8 i;
       
    for (i = 0; i < 32; i++)
    {
        if (((bitMask >> i) & 0x00000001) != 0)
        {
            break;
        }
    } 
    
    return i;   
}

/*******************************************************************************
* 名称  : SLFCHK_GetCheckResultValue
* 描述  : 获取自检结果中某一项的状态
* 输入  : pSelfCheckStruct - SelfCheckStruct结构体指针
          bitMask - 字段对应的位掩码	  
* 输出  : 无
* 返回  : 位掩码对应的值
*******************************************************************************/
static U8 SLFCHK_GetCheckResultValue(const SelfCheckStruct *pSelfCheckStruct, const U32 bitMask)
{
    U32 temp32;

    temp32 = pSelfCheckStruct->CheckResult & bitMask;
    temp32 >>= SLFCHK_GetBitNum32(bitMask);
    
    return temp32;  
}

/*******************************************************************************
* 名称  : SLFCHK_GetCheckSelect
* 描述  : 获取自检选项中某一项的状态
* 输入  : pSelfCheckStruct - SelfCheckStruct结构体指针
          bitMask - 字段对应的位掩码	  
* 输出  : 无
* 返回  : 位掩码对应的值
*******************************************************************************/
static U8 SLFCHK_GetCheckSelect(const SelfCheckStruct *pSelfCheckStruct, const U32 bitMask)
{
    U32 temp32;

    temp32 = pSelfCheckStruct->CheckSelect & bitMask;
    temp32 >>= SLFCHK_GetBitNum32(bitMask);
    
    return temp32;  
}

/*******************************************************************************
* 名称  : SLFCHK_InstGetCheckResultValue
* 描述  : 获取自检结果中某一项的状态
* 输入  : bitMask - 字段对应的位掩码	  
* 输出  : 无
* 返回  : 位掩码对应的值
*******************************************************************************/
U8 SLFCHK_InstGetCheckResultValue(const U32 bitMask)
{ 
    return SLFCHK_GetCheckResultValue(&SelfCheckInst, bitMask);  
}

/*******************************************************************************
* 名称  : SLFCHK_SetCheckResultValue
* 描述  : 置位自检结果中某一项的状态
* 输入  : pSelfCheckStruct - SelfCheckStruct结构体指针
          bitMask - 字段对应的位掩码	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SLFCHK_SetCheckResultValue(SelfCheckStruct *pSelfCheckStruct, const U32 bitMask)
{
    pSelfCheckStruct->CheckResult |= bitMask;    
}

/*******************************************************************************
* 名称  : SLFCHK_CfgCheckSelect
* 描述  : 置位自检选项中指定项的状态，未指定项复位
* 输入  : pSelfCheckStruct - SelfCheckStruct结构体指针
          bitMask - 字段对应的位掩码	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SLFCHK_CfgCheckSelect(SelfCheckStruct *pSelfCheckStruct, const U32 bitMask)
{
    pSelfCheckStruct->CheckSelect = bitMask;    
}

/*******************************************************************************
* 名称  : SLFCHK_SetCheckSelect
* 描述  : 置位自检选项中某一项的状态
* 输入  : pSelfCheckStruct - SelfCheckStruct结构体指针
          bitMask - 字段对应的位掩码	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SLFCHK_SetCheckSelect(SelfCheckStruct *pSelfCheckStruct, const U32 bitMask)
{
    pSelfCheckStruct->CheckSelect |= bitMask;    
}

/*******************************************************************************
* 名称  : SLFCHK_ResetCheckSelect
* 描述  : 复位自检选项中某一项的状态
* 输入  : pSelfCheckStruct - SelfCheckStruct结构体指针
          bitMask - 字段对应的位掩码	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SLFCHK_ResetCheckSelect(SelfCheckStruct *pSelfCheckStruct, const U32 bitMask)
{
    pSelfCheckStruct->CheckSelect &= ~bitMask;    
}

/*******************************************************************************
* 名称  : SLFCHK_CfgCheckSelect
* 描述  : 置位自检选项中指定项的状态，未指定项复位
* 输入  : bitMask - 字段对应的位掩码	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SLFCHK_InstCfgCheckSelect(const U32 bitMask)
{
    SLFCHK_CfgCheckSelect(&SelfCheckInst, bitMask);    
}

/*******************************************************************************
* 名称  : SLFCHK_SetCheckSelect
* 描述  : 置位自检选项中某一项的状态
* 输入  : bitMask - 字段对应的位掩码	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SLFCHK_InstSetCheckSelect(const U32 bitMask)
{
    SLFCHK_SetCheckSelect(&SelfCheckInst, bitMask);    
}

/*******************************************************************************
* 名称  : SLFCHK_InstResetCheckSelect
* 描述  : 复位自检选项中某一项的状态
* 输入  : bitMask - 字段对应的位掩码	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SLFCHK_InstResetCheckSelect(const U32 bitMask)
{
    SLFCHK_ResetCheckSelect(&SelfCheckInst, bitMask);    
}

/*******************************************************************************
* 名称  : SLFCHK_SetCheckResultValue
* 描述  : 置位自检结果中某一项的状态
* 输入  : pSelfCheckStruct - SelfCheckStruct结构体指针
          bitMask - 字段对应的位掩码	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SLFCHK_InstSetCheckResultValue(const U32 bitMask)
{
    SLFCHK_SetCheckResultValue(&SelfCheckInst, bitMask);    
}

/*******************************************************************************
* 名称  : SLFCHK_ResetCheckResultValue
* 描述  : 复位自检结果中某一项的状态
* 输入  : pSelfCheckStruct - SelfCheckStruct结构体指针
          bitMask - 字段对应的位掩码	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SLFCHK_ResetCheckResultValue(SelfCheckStruct *pSelfCheckStruct, const U32 bitMask)
{
    pSelfCheckStruct->CheckResult &= ~bitMask;    
}

/*******************************************************************************
* 名称  : SLFCHK_InstResetCheckResultValue
* 描述  : 复位自检结果中某一项的状态
* 输入  : pSelfCheckStruct - SelfCheckStruct结构体指针
          bitMask - 字段对应的位掩码	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SLFCHK_InstResetCheckResultValue(const U32 bitMask)
{
    SLFCHK_ResetCheckResultValue(&SelfCheckInst, bitMask);    
}

/*******************************************************************************
* 名称  : SLFCHK_SelfCheckStart
* 描述  : 自检启动
* 输入  : pSelfCheckStruct - SelfCheckStruct结构体指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SLFCHK_SelfCheckStart(SelfCheckStruct *pSelfCheckStruct)
{
    pSelfCheckStruct->StartFlg = TRUE;    
}

/*******************************************************************************
* 名称  : SLFCHK_InstSelfCheckStart
* 描述  : 自检启动
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SLFCHK_InstSelfCheckStart(void)
{
    SLFCHK_SelfCheckStart(&SelfCheckInst);    
}

/*******************************************************************************
* 名称  : SLFCHK_SetCompleteBitValue
* 描述  : 置位完成标志某一项的状态
* 输入  : pSelfCheckStruct - SelfCheckStruct结构体指针
          bitMask - 字段对应的位掩码	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SLFCHK_SetCompleteBitValue(SelfCheckStruct *pSelfCheckStruct, const U32 bitMask)
{
    pSelfCheckStruct->CompleteBitFlg |= bitMask;    
}

/*******************************************************************************
* 名称  : SLFCHK_IsSelfCheckComplete
* 描述  : 自检是否完成
* 输入  : pSelfCheckStruct - SelfCheckStruct结构体指针
* 输出  : 无
* 返回  : TRUE - 已完成，FALSE - 未完成
*******************************************************************************/
static U8 SLFCHK_IsSelfCheckComplete(SelfCheckStruct *pSelfCheckStruct)
{
    if ((pSelfCheckStruct->CompleteBitFlg & pSelfCheckStruct->CheckSelect)
        == pSelfCheckStruct->CheckSelect)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SLFCHK_IsSelfCheckOk
* 描述  : 自检是否正常
* 输入  : pSelfCheckStruct - SelfCheckStruct结构体指针
* 输出  : 无
* 返回  : TRUE - 正常，FALSE - 不正常
*******************************************************************************/
static U8 SLFCHK_IsSelfCheckOk(SelfCheckStruct *pSelfCheckStruct)
{
    if ((pSelfCheckStruct->CheckResult & pSelfCheckStruct->CheckSelect)
        == pSelfCheckStruct->CheckSelect)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SLFCHK_SelfCheckProcess
* 描述  : 自检处理
* 输入  : pSelfCheckStruct - SelfCheckStruct结构体指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SLFCHK_SelfCheckProcess(SelfCheckStruct *pSelfCheckStruct)
{
    static EepromSelfCheckStruct EepromSelfCheckInst;
    static Si446xSelfCheckStruct Si446xSelfCheckInstA;
    static Si446xSelfCheckStruct Si446xSelfCheckInstB;
    static BatterySelfCheckStruct BatterySelfCheckInst;
    static RtcSelfCheckStruct RtcSelfCheckInst;
    static RfLinkSelfCheckStruct RfLinkSelfCheckInst;
	char chRetBuf[100];
	char chTemp[30];
     
    switch (pSelfCheckStruct->CtrlState)
    {
        case SLFCHK_CTRL_STATE_IDLE:
           
            if (pSelfCheckStruct->StartFlg == TRUE)
            {
    			LEDInit(LED_TYPE_SELF_CHECK_RUNNING);                
                pSelfCheckStruct->CtrlState = SLFCHK_CTRL_STATE_START;                                            
                pSelfCheckStruct->StartFlg = FALSE;
            }
            break;
        case SLFCHK_CTRL_STATE_START:
        
            EEPROM_EepromSelfCheckStructInit(&EepromSelfCheckInst);
            SI446X_Si446xSelfCheckStructInit(&Si446xSelfCheckInstA);
            SI446X_Si446xSelfCheckStructInit(&Si446xSelfCheckInstB);
#if CONFIG_SLFCHK_POWER_SUPPLY_VOLT_EN
            BAT_BatterySelfCheckStructInit(&BatterySelfCheckInst);
#endif
            RTC_RtcSelfCheckStructInit(&RtcSelfCheckInst);
            GPS_GpsSelfCheckStructInit(&GpsSelfCheckInst);
            RFL_RfLinkSelfCheckStructInit(&RfLinkSelfCheckInst);

            //lq pSelfCheckStruct->TimeCnt = SetTimePeriodProcess(pSelfCheckStruct->TimePeriod);               
            pSelfCheckStruct->CtrlState = SLFCHK_CTRL_STATE_RUNNING;                                            
            
            break;
        case SLFCHK_CTRL_STATE_RUNNING:

            EEPROM_SelfCheck(&EepromSelfCheckInst);
            SI446X_SelfCheck(&Si446xSelfCheckInstA, SI446X_CHIP_A);			   //射频芯片自检，利用PORT_INFO命令自检
            SI446X_SelfCheck(&Si446xSelfCheckInstB, SI446X_CHIP_B);
#if CONFIG_SLFCHK_POWER_SUPPLY_VOLT_EN
            BAT_SelfCheck(&BatterySelfCheckInst);							   //电压检测，获取电压的AD采样值
#endif
            RTC_SelfCheck(&RtcSelfCheckInst);
            GPS_SelfCheck(&GpsSelfCheckInst);								   //GPS自检模式,GPS掉电，50ms延时后，再对GPS上点，检测GPS是否有消息
            RFL_SelfCheck(&RfLinkSelfCheckInst);							   //发射检测，利用回环检测，先用B信道发射一条6号测试，然后用芯片B进行接收

/*********************************************** 自检结果输出 **************************************************************************/
            /*lq EEPROM自检结果 */
            if (EepromSelfCheckInst.CompleteFlg == TRUE)
            {
                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_EEPROM_WP) == TRUE)
                {
                    if (EepromSelfCheckInst.WPOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_EEPROM_WP);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_EEPROM_WP);                                   
                    }

                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_EEPROM_WP);
                    
                    /*lq 以下报文是配合上位机的测试进度条控件 */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_EEPROM_WP);                                    //lq 指定当前测试项
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_EEPROM_WP);  //lq 当前测试项的测试结果
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                    
                }

                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_EEPROM_DATA_COMM) == TRUE)
                {
                    if (EepromSelfCheckInst.DataCommOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_EEPROM_DATA_COMM);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_EEPROM_DATA_COMM);                                   
                    }

                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_EEPROM_DATA_COMM);
                    
                    /*lq 以下报文是配合上位机的测试进度条控件 */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_EEPROM_DATA_COMM);                                    //lq 指定当前测试项
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_EEPROM_DATA_COMM);  //lq 当前测试项的测试结果
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                     
                }

                EepromSelfCheckInst.CompleteFlg = FALSE;
            }
            
            /*lq SI446X_A自检结果 */        
            if (Si446xSelfCheckInstA.CompleteFlg == TRUE)
            {
                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_PDN) == TRUE)
                {
                    if (Si446xSelfCheckInstA.PdnOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_PDN);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_PDN);                                   
                    }  
                    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_PDN);

                    /*lq 以下报文是配合上位机的测试进度条控件 */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_SI446X_A_PDN);                                    //lq 指定当前测试项
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_SI446X_A_PDN);  //lq 当前测试项的测试结果
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                    
                }

                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_SEN) == TRUE)
                {
                    if (Si446xSelfCheckInstA.SenOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_SEN);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_SEN);                                   
                    }
                    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_SEN);

                    /*lq 以下报文是配合上位机的测试进度条控件 */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_SI446X_A_SEN);                                    //lq 指定当前测试项
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_SI446X_A_SEN);  //lq 当前测试项的测试结果
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                     
                }

                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_DATA_COMM) == TRUE)
                {
                    if (Si446xSelfCheckInstA.DataCommOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_DATA_COMM);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_DATA_COMM);                                   
                    }
    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_DATA_COMM);

                    /*lq 以下报文是配合上位机的测试进度条控件 */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_SI446X_A_DATA_COMM);                                    //lq 指定当前测试项
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_SI446X_A_DATA_COMM);  //lq 当前测试项的测试结果
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                
                }

                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_TYPE) == TRUE)
                {
                    if (Si446xSelfCheckInstA.TypeOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_TYPE);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_TYPE);                                   
                    }
                    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_A_TYPE);

                    /*lq 以下报文是配合上位机的测试进度条控件 */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_SI446X_A_TYPE);                                     //lq 指定当前测试项
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_SI446X_A_TYPE);  //lq 当前测试项的测试结果
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                     
                }

                Si446xSelfCheckInstA.CompleteFlg = FALSE;
            }

            /*lq SI446X_B自检结果 */        
            if (Si446xSelfCheckInstB.CompleteFlg == TRUE)
            {
                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_PDN) == TRUE)
                {
                    if (Si446xSelfCheckInstB.PdnOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_PDN);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_PDN);                                   
                    }  
                    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_PDN);

                    /*lq 以下报文是配合上位机的测试进度条控件 */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_SI446X_B_PDN);                                    //lq 指定当前测试项
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_SI446X_B_PDN);  //lq 当前测试项的测试结果
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                    
                }

                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_SEN) == TRUE)
                {
                    if (Si446xSelfCheckInstB.SenOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_SEN);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_SEN);                                   
                    }
                    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_SEN);

                    /*lq 以下报文是配合上位机的测试进度条控件 */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_SI446X_B_SEN);                                    //lq 指定当前测试项
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_SI446X_B_SEN);  //lq 当前测试项的测试结果
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                     
                }

                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_DATA_COMM) == TRUE)
                {
                    if (Si446xSelfCheckInstB.DataCommOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_DATA_COMM);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_DATA_COMM);                                   
                    }
    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_DATA_COMM);

                    /*lq 以下报文是配合上位机的测试进度条控件 */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_SI446X_B_DATA_COMM);                                    //lq 指定当前测试项
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_SI446X_B_DATA_COMM);  //lq 当前测试项的测试结果
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                
                }

                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_TYPE) == TRUE)
                {
                    if (Si446xSelfCheckInstB.TypeOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_TYPE);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_TYPE);                                   
                    }
                    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_SI446X_B_TYPE);

                    /*lq 以下报文是配合上位机的测试进度条控件 */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_SI446X_B_TYPE);                                     //lq 指定当前测试项
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_SI446X_B_TYPE);  //lq 当前测试项的测试结果
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                     
                }

                Si446xSelfCheckInstB.CompleteFlg = FALSE;
            }

            /*lq Supply voltage电压自检结果 */        
            if (BatterySelfCheckInst.CompleteFlg == TRUE)
            {
                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_POWER_SUPPLY_VOLT) == TRUE)
                {
                    if (BatterySelfCheckInst.SupplyVoltOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_POWER_SUPPLY_VOLT);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_POWER_SUPPLY_VOLT);                                   
                    }
    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_POWER_SUPPLY_VOLT);

                    /*lq 以下报文是配合上位机的测试进度条控件 */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_POWER_SUPPLY_VOLT);                                    //lq 指定当前测试项
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_POWER_SUPPLY_VOLT);  //lq 当前测试项的测试结果
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                  
                }                

                BatterySelfCheckInst.CompleteFlg = FALSE;
            }

            /*lq RTC自检结果 */        
            if (RtcSelfCheckInst.CompleteFlg == TRUE)
            {
                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_RTC_LSE) == TRUE)
                {
                    if (RtcSelfCheckInst.RtcLseOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_RTC_LSE);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_RTC_LSE);                                   
                    }
    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_RTC_LSE);

                    /*lq 以下报文是配合上位机的测试进度条控件 */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_RTC_LSE);                                    //lq 指定当前测试项
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_RTC_LSE);  //lq 当前测试项的测试结果
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                
                }

                RtcSelfCheckInst.CompleteFlg = FALSE;
            }            

            /*lq GPS自检结果 */        
            if (GpsSelfCheckInst.CompleteFlg == TRUE)
            {
                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_VCC_CTRL) == TRUE)
                {
                    if (GpsSelfCheckInst.VccCtrlOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_VCC_CTRL);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_VCC_CTRL);                                   
                    }

                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_VCC_CTRL);

                    /*lq 以下报文是配合上位机的测试进度条控件 */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_GPS_VCC_CTRL);                                    //lq 指定当前测试项
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_GPS_VCC_CTRL);  //lq 当前测试项的测试结果
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                    
                }

                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_TIMEPULSE) == TRUE)
                {
                    if (GpsSelfCheckInst.TimepulseOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_TIMEPULSE);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_TIMEPULSE);                                   
                    } 

                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_TIMEPULSE);

                    /*lq 以下报文是配合上位机的测试进度条控件 */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_GPS_TIMEPULSE);                                    //lq 指定当前测试项
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_GPS_TIMEPULSE);  //lq 当前测试项的测试结果
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                    
                }

                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_DATA_COMM) == TRUE)
                {
                    if (GpsSelfCheckInst.DataCommOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_DATA_COMM);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_DATA_COMM);                                   
                    }
                    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_DATA_COMM);

                    /*lq 以下报文是配合上位机的测试进度条控件 */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_GPS_DATA_COMM);                                    //lq 指定当前测试项
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_GPS_DATA_COMM);  //lq 当前测试项的测试结果
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                    
                }

                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_ANTENNA) == TRUE)
                {
                    if (GpsSelfCheckInst.AntennaOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_ANTENNA);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_ANTENNA);                                   
                    }

                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_ANTENNA);
                    
                    /*lq 以下报文是配合上位机的测试进度条控件 */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_GPS_ANTENNA);                                    //lq 指定当前测试项
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_GPS_ANTENNA);  //lq 当前测试项的测试结果
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                    
                }

                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_MODULE) == TRUE)
                {
                    if (GpsSelfCheckInst.ModuleOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_MODULE);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_MODULE);                                   
                    }
                    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_GPS_MODULE);

                    /*lq 以下报文是配合上位机的测试进度条控件 */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_GPS_MODULE);                                    //lq 指定当前测试项
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_GPS_MODULE);  //lq 当前测试项的测试结果
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                                      
                }

                GpsSelfCheckInst.CompleteFlg = FALSE;
            }

            /*lq RF link自检结果 */        
            if (RfLinkSelfCheckInst.CompleteFlg == TRUE)
            {
                if (SLFCHK_GetCheckSelect(pSelfCheckStruct, SLFCHK_ITEM_MASK_RF_LINK) == TRUE)
                {
                    if (RfLinkSelfCheckInst.RfLinkOkFlg == TRUE)
                    {
                        SLFCHK_SetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_RF_LINK);                   
                    }
                    else
                    {
                        SLFCHK_ResetCheckResultValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_RF_LINK);                                   
                    }
    
                    SLFCHK_SetCompleteBitValue(pSelfCheckStruct, SLFCHK_ITEM_MASK_RF_LINK);

                    /*lq 以下报文是配合上位机的测试进度条控件 */
                    sprintf(chRetBuf, "$P712,SLFCHK,");
                    sprintf(chTemp, "%08X", SLFCHK_ITEM_MASK_RF_LINK);                                    //lq 指定当前测试项
                    strcat(chRetBuf, chTemp);
                    sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & SLFCHK_ITEM_MASK_RF_LINK);  //lq 当前测试项的测试结果
                    strcat(chRetBuf, chTemp);
                    UartResponseMsg(chRetBuf);                
                }

                RfLinkSelfCheckInst.CompleteFlg = FALSE;
            }            

            if (SLFCHK_IsSelfCheckComplete(pSelfCheckStruct) == TRUE)
            {
    			if (SLFCHK_IsSelfCheckOk(pSelfCheckStruct) == TRUE)
                {
                    LEDInit(LED_TYPE_SELF_CHECK_OK);                                
                }
                else
                {
                    LEDInit(LED_TYPE_SELF_CHECK_ERR);
                }

                sprintf(chRetBuf, "$P712,SLFCHK,");
                sprintf(chTemp, "%08X", pSelfCheckStruct->CheckSelect);                                    //lq 指定当前测试项
                strcat(chRetBuf, chTemp);
                sprintf(chTemp, ",%08X", (pSelfCheckStruct->CheckResult) & pSelfCheckStruct->CheckSelect);  //lq 当前测试项的测试结果
                strcat(chRetBuf, chTemp);
                UartResponseMsg(chRetBuf); 

                pSelfCheckStruct->CompleteFlg = TRUE;
                
                pSelfCheckStruct->CtrlState = SLFCHK_CTRL_STATE_IDLE;                                
            }

            break;                            
        default :
            
            SLFCHK_SelfCheckStructInit(pSelfCheckStruct);
            LEDInit(LED_TYPE_SELF_CHECK_IDLE);                                
            break;       
    }    
}

/*******************************************************************************
* 名称  : SLFCHK_InstSelfCheckProcess
* 描述  : 自检处理
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SLFCHK_InstSelfCheckProcess(void)
{
    SLFCHK_SelfCheckProcess(&SelfCheckInst);
}

/*******************************************************************************
* 名称  : SLFCHK_GetCheckSelect
* 描述  : 获取自检选项
* 输入  : 无
* 输出  : 无
* 返回  : 自检选项
*******************************************************************************/
U32 SLFCHK_GetCheckSelectValue(void)
{
	SelfCheckStruct *pSelfCheckStruct;

	pSelfCheckStruct = &SelfCheckInst;

	return	pSelfCheckStruct->CheckSelect;
}

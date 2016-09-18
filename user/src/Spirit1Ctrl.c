/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: Spirit1Ctrl.c
  Author: lq       Version : V1.0.0        Date: 2014.08.06
  Description:     SPIRIT1控制
  Version:         V1.0.0
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "stm32f10x_lib.h"
#include "def.h"
#include "SPIRIT1.h"
#include "Spirit1Ctrl.h"
#include "sotdma.h"
#include "RxTxCtrl.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define ST_SIGNAL_LEN   21

/* Private macro -------------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
const U8 StSignal1[ST_SIGNAL_LEN] = 
{
    0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,
	0x55,0x55,0x55,0x55,0x55,
	0x55,
};

const U8 StSignal2[ST_SIGNAL_LEN] = 
{
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,0x0F,0x0F,0x0F,0x0F,
	0x0F,
};

const U8 StSignal3[ST_SIGNAL_LEN] = 
{
	0x48,0x99,0x07,0xad,0x40,
    0x00,0x4c,0x0b,0xa6,0x74,
    0x52,0xe4,0xdb,0x3d,0x86,
    0x32,0xac,0x00,0x1e,0x00,
    0x06,
};

SPIRIT1TestStruct SPIRIT1Test1;
SPIRIT1TestStruct SPIRIT1Test2;
SPIRIT1TxCtrlStruct SPIRIT1TxCtrlInstance;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* 名称  : SPRTCTRL_SPIRIT1TxCtrlStructInit
* 描述  : 结构体初始化
* 输入  : SPIRIT1TxCtrlStruct - pSPIRIT1TxCtrlStruct结构体	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SPRTCTRL_SPIRIT1TxCtrlStructInit(SPIRIT1TxCtrlStruct *pSPIRIT1TxCtrlStruct)
{
	pSPIRIT1TxCtrlStruct->TimeCnt = 0;
	pSPIRIT1TxCtrlStruct->TimePeriod = SPRTCTRL_TIMEOUT_TX_DONE_WAITING;

    pSPIRIT1TxCtrlStruct->TxCtrlState = SPRTCTRL_TX_CTRL_STATE_IDLE;	
    pSPIRIT1TxCtrlStruct->TxFlg = FALSE;
    pSPIRIT1TxCtrlStruct->TxSlot = 0;
    pSPIRIT1TxCtrlStruct->TxChannel = 'A';	
        
    pSPIRIT1TxCtrlStruct->TxBufferIndex = 0;
    pSPIRIT1TxCtrlStruct->TxBufferOffset = 0;            
}

/*******************************************************************************
* 名称  : SPRTCTRL_InstSPIRIT1TxCtrlStructInit
* 描述  : 实例初始化
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPRTCTRL_InstSPIRIT1TxCtrlStructInit(void)
{
    SPRTCTRL_SPIRIT1TxCtrlStructInit(&SPIRIT1TxCtrlInstance);       
}

/*******************************************************************************
* 名称  : SPRTCTRL_SPIRIT1TestStructInit
* 描述  : 结构体初始化
* 输入  : pSPIRIT1TestStruct - SPIRIT1TestStruct结构体	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SPRTCTRL_SPIRIT1TestStructInit(SPIRIT1TestStruct *pSPIRIT1TestStruct)
{
    pSPIRIT1TestStruct->TimeCnt = 0;
    pSPIRIT1TestStruct->TimePeriod = SPRTCTRL_TIME_PERIOD;
    pSPIRIT1TestStruct->TimeoutTx = 0;
    pSPIRIT1TestStruct->TimeoutHalt = 0;
    pSPIRIT1TestStruct->TimeoutCntTx = 0;
    pSPIRIT1TestStruct->TimeoutCntHalt = 0;
    pSPIRIT1TestStruct->TxCtrlSta = SPRTCTRL_TEST_TX_CTRL_STA_IDLE;
    pSPIRIT1TestStruct->TxTestFlg = FALSE;
    pSPIRIT1TestStruct->TxSignalType = SPRTCTRL_TEST_TX_SIGNAL_NUM1; 
    pSPIRIT1TestStruct->TxBufferIndex = 0;
    pSPIRIT1TestStruct->TxBufferOffset = 0;             
}

/*******************************************************************************
* 名称  : SPRTCTRL_InstSPIRIT1TestStructInit
* 描述  : 结构体初始化
* 输入  : cs - SPIRIT1芯片类型  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPRTCTRL_InstSPIRIT1TestStructInit(SPIRIT1TypeEnum cs)
{
    if (cs == SPIRIT1TypeA)
    {
        SPRTCTRL_SPIRIT1TestStructInit(&SPIRIT1Test1);
    }
    else if (cs == SPIRIT1TypeB)
    {
        SPRTCTRL_SPIRIT1TestStructInit(&SPIRIT1Test2);
    }            
}

/*******************************************************************************
* 名称  : SPRTCTRL_TxTestInitSignal
* 描述  : 初始化SPIRIT发射测试信号
* 输入  : pSPIRIT1TestStruct - SPIRIT1TestStruct结构体	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SPRTCTRL_TxTestInitSignal(SPIRIT1TestStruct *pSPIRIT1TestStruct)
{
    U8 *pBuff;
   
    if (pSPIRIT1TestStruct->TxSignalType == SPRTCTRL_TEST_TX_SIGNAL_NUM1)
    {
        pBuff = (U8 *)malloc((ST_SIGNAL_LEN + 2) * sizeof(U8));
        memset((U8 *)pBuff, 0, (ST_SIGNAL_LEN + 2) * sizeof(U8));
        pBuff[0] = ST_SIGNAL_LEN;
        pBuff[1] = 0;
        memcpy(&pBuff[2], &StSignal1[0], ST_SIGNAL_LEN);                
        DLS_FrameBuildTest(&pBuff[0], pSPIRIT1TestStruct->TxBuffer);
        free((U8 *)pBuff);

        pSPIRIT1TestStruct->TimeoutTx = SPRTCTRL_TEST_TX_TIMEOUT_TX_1;            //lq 不同的信号的发射周期和发射时长可以分别控制
        pSPIRIT1TestStruct->TimeoutHalt = SPRTCTRL_TEST_TX_TIMEOUT_HALT_1;                            
    }
    else if (pSPIRIT1TestStruct->TxSignalType == SPRTCTRL_TEST_TX_SIGNAL_NUM2)
    {
        pBuff = (U8 *)malloc((ST_SIGNAL_LEN + 2) * sizeof(U8));
        memset((U8 *)pBuff, 0, (ST_SIGNAL_LEN + 2) * sizeof(U8));
        pBuff[0] = ST_SIGNAL_LEN;
        pBuff[1] = 0;
        memcpy(&pBuff[2], &StSignal2[0], ST_SIGNAL_LEN);                
        DLS_FrameBuildTest(&pBuff[0], pSPIRIT1TestStruct->TxBuffer);
        free((U8 *)pBuff);

        pSPIRIT1TestStruct->TimeoutTx = SPRTCTRL_TEST_TX_TIMEOUT_TX_2;
        pSPIRIT1TestStruct->TimeoutHalt = SPRTCTRL_TEST_TX_TIMEOUT_HALT_2;                             
    }
    else if (pSPIRIT1TestStruct->TxSignalType == SPRTCTRL_TEST_TX_SIGNAL_NUM3)
    {
        pBuff = (U8 *)malloc((ST_SIGNAL_LEN + 2) * sizeof(U8));
        memset((U8 *)pBuff, 0, (ST_SIGNAL_LEN + 2) * sizeof(U8));
        pBuff[0] = ST_SIGNAL_LEN;
        pBuff[1] = 0;
        memcpy(&pBuff[2], &StSignal3[0], ST_SIGNAL_LEN);                
#if 1
        DLS_FrameBuildTest(&pBuff[0], pSPIRIT1TestStruct->TxBuffer);
#else
        DLS_FrameBuildTest2(&pBuff[0], pSPIRIT1TestStruct->TxBuffer);
#endif     
        free((U8 *)pBuff);

        pSPIRIT1TestStruct->TimeoutTx = SPRTCTRL_TEST_TX_TIMEOUT_TX_3;
        pSPIRIT1TestStruct->TimeoutHalt = SPRTCTRL_TEST_TX_TIMEOUT_HALT_3;                             
    }
    else if (pSPIRIT1TestStruct->TxSignalType == SPRTCTRL_TEST_TX_SIGNAL_CW)
    {
        memset((U8 *)(pSPIRIT1TestStruct->TxBuffer), 0, (SPRTCTRL_TEST_TX_BUFFER_LEN) * sizeof(U8));        
        pSPIRIT1TestStruct->TimeoutTx = SPRTCTRL_TEST_TX_TIMEOUT_TX_4;
        pSPIRIT1TestStruct->TimeoutHalt = SPRTCTRL_TEST_TX_TIMEOUT_HALT_4;                             
    }
    else if (pSPIRIT1TestStruct->TxSignalType == SPRTCTRL_TEST_TX_SIGNAL_PN9)
    {
        memset((U8 *)(pSPIRIT1TestStruct->TxBuffer), 0, (SPRTCTRL_TEST_TX_BUFFER_LEN) * sizeof(U8));        
        pSPIRIT1TestStruct->TimeoutTx = SPRTCTRL_TEST_TX_TIMEOUT_TX_5;
        pSPIRIT1TestStruct->TimeoutHalt = SPRTCTRL_TEST_TX_TIMEOUT_HALT_5;                             
    }
    else if (pSPIRIT1TestStruct->TxSignalType == SPRTCTRL_TEST_TX_SIGNAL_ALL0)
    {
        memset((U8 *)(pSPIRIT1TestStruct->TxBuffer), 0, (SPRTCTRL_TEST_TX_BUFFER_LEN) * sizeof(U8));    //lq all '0'        
        pSPIRIT1TestStruct->TxBuffer[0] = SPRTCTRL_TEST_TX_BUFFER_LEN - 2;
        pSPIRIT1TestStruct->TxBuffer[1] = 0;
        pSPIRIT1TestStruct->TimeoutTx = SPRTCTRL_TEST_TX_TIMEOUT_TX_6;            //lq 不同的信号的发射周期和发射时长可以分别控制
        pSPIRIT1TestStruct->TimeoutHalt = SPRTCTRL_TEST_TX_TIMEOUT_HALT_6;                            
    }
    else if (pSPIRIT1TestStruct->TxSignalType == SPRTCTRL_TEST_TX_SIGNAL_ALL1)
    {
        memset((U8 *)(pSPIRIT1TestStruct->TxBuffer), 0xFF, (SPRTCTRL_TEST_TX_BUFFER_LEN) * sizeof(U8)); //lq all '1'        
        pSPIRIT1TestStruct->TxBuffer[0] = SPRTCTRL_TEST_TX_BUFFER_LEN - 2;
        pSPIRIT1TestStruct->TxBuffer[1] = 0;
        pSPIRIT1TestStruct->TimeoutTx = SPRTCTRL_TEST_TX_TIMEOUT_TX_7;
        pSPIRIT1TestStruct->TimeoutHalt = SPRTCTRL_TEST_TX_TIMEOUT_HALT_7;                             
    }
    pSPIRIT1TestStruct->TxBufferIndex = 2;
    pSPIRIT1TestStruct->TxBufferOffset = 0;
}

/*******************************************************************************
* 名称  : SPRTCTRL_TxTestInit
* 描述  : SPIRIT发射测试初始化
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SPRTCTRL_TxTestInit(SPIRIT1TypeEnum cs)
{
    SPIRIT1TestStruct *pSPIRIT1TestStruct;

    /*lq 选择实例 */
    if (cs == SPIRIT1TypeA)
    {
        pSPIRIT1TestStruct = &SPIRIT1Test1;
    }
    else if (cs == SPIRIT1TypeB)
    {
        pSPIRIT1TestStruct = &SPIRIT1Test2;    
    }
    else
    {
        return ;
    }

    if (pSPIRIT1TestStruct->TxSignalType == SPRTCTRL_TEST_TX_SIGNAL_NUM1)
    {
        SPIRIT1_CfgGpioTx(cs);        
        SPRTCTRL_TxTestInitSignal(pSPIRIT1TestStruct);
    }
    else if (pSPIRIT1TestStruct->TxSignalType == SPRTCTRL_TEST_TX_SIGNAL_NUM2)
    {
        SPIRIT1_CfgGpioTx(cs);        
        SPRTCTRL_TxTestInitSignal(pSPIRIT1TestStruct);
    }
    else if (pSPIRIT1TestStruct->TxSignalType == SPRTCTRL_TEST_TX_SIGNAL_NUM3)
    {
        SPIRIT1_CfgGpioTx(cs);        
        SPRTCTRL_TxTestInitSignal(pSPIRIT1TestStruct);
    }
    else if (pSPIRIT1TestStruct->TxSignalType == SPRTCTRL_TEST_TX_SIGNAL_CW)
    {
        SPIRIT1_CfgCwTx(cs);                            
        SPRTCTRL_TxTestInitSignal(pSPIRIT1TestStruct);
    }
    else if (pSPIRIT1TestStruct->TxSignalType == SPRTCTRL_TEST_TX_SIGNAL_PN9)
    {
        SPIRIT1_CfgPN9Tx(cs);                            
        SPRTCTRL_TxTestInitSignal(pSPIRIT1TestStruct);
    }
    else if (pSPIRIT1TestStruct->TxSignalType == SPRTCTRL_TEST_TX_SIGNAL_ALL0)
    {
        SPIRIT1_CfgGpioTx(cs);        
        SPRTCTRL_TxTestInitSignal(pSPIRIT1TestStruct);
    }
    else if (pSPIRIT1TestStruct->TxSignalType == SPRTCTRL_TEST_TX_SIGNAL_ALL1)
    {
        SPIRIT1_CfgGpioTx(cs);        
        SPRTCTRL_TxTestInitSignal(pSPIRIT1TestStruct);
    }
}

/*******************************************************************************
* 名称  : SPRTCTRL_TxTestResetSignal
* 描述  : 复位SPIRIT发射测试信号
* 输入  : pSPIRIT1TestStruct - SPIRIT1TestStruct结构体	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SPRTCTRL_TxTestResetSignal(SPIRIT1TestStruct *pSPIRIT1TestStruct)
{
    pSPIRIT1TestStruct->TxBufferIndex = 2;
    pSPIRIT1TestStruct->TxBufferOffset = 0;     
}

/*******************************************************************************
* 名称  : SPRTCTRL_TxTestGetSignalBit
* 描述  : 获取发射信号位
* 输入  : pSPIRIT1TestStruct - SPIRIT1TestStruct结构体	  
* 输出  : 无
* 返回  : 发射信号位
*******************************************************************************/
static U8 SPRTCTRL_TxTestGetSignalBit(SPIRIT1TestStruct *pSPIRIT1TestStruct)
{
    U8 retValue;
    
    if ((pSPIRIT1TestStruct->TxBuffer[pSPIRIT1TestStruct->TxBufferIndex]
         & (1u << pSPIRIT1TestStruct->TxBufferOffset)) == 0)
    {
        retValue = 0;
    }    
    else
    {
        retValue = 1;
    }

    pSPIRIT1TestStruct->TxBufferOffset++;
    if (pSPIRIT1TestStruct->TxBufferOffset >= 8)
    {
        pSPIRIT1TestStruct->TxBufferOffset = 0;
        pSPIRIT1TestStruct->TxBufferIndex++;
        if (pSPIRIT1TestStruct->TxBufferIndex >= SPRTCTRL_TEST_TX_BUFFER_LEN)   //lq 溢出判断
        {
            pSPIRIT1TestStruct->TxBufferIndex = 2;
        } 
    }

    /*lq 数据结束判断 */
    if ((pSPIRIT1TestStruct->TxBufferIndex >= (pSPIRIT1TestStruct->TxBuffer[0] + 2))
        && (pSPIRIT1TestStruct->TxBufferOffset >= pSPIRIT1TestStruct->TxBuffer[1]))
    {
        pSPIRIT1TestStruct->TxBufferIndex = 2;
        pSPIRIT1TestStruct->TxBufferOffset = 0;
    }

    return retValue;
}

/*******************************************************************************
* 名称  : SPRTCTRL_TxGetSignalBit
* 描述  : 获取发射信号位
* 输入  : pSPIRIT1TxCtrlStruct - SPIRIT1TxCtrlStruct结构体	  
* 输出  : 无
* 返回  : 发射信号位
*******************************************************************************/
static U8 SPRTCTRL_TxGetSignalBit(SPIRIT1TxCtrlStruct *pSPIRIT1TxCtrlStruct)
{
    U8 retValue;

    if (pSPIRIT1TxCtrlStruct->TxFlg == TRUE)
    {
        if ((pSPIRIT1TxCtrlStruct->TxBuffer[pSPIRIT1TxCtrlStruct->TxBufferIndex]
             & (1u << pSPIRIT1TxCtrlStruct->TxBufferOffset)) == 0)
        {
            retValue = 0;
        }    
        else
        {
            retValue = 1;
        }
    
        pSPIRIT1TxCtrlStruct->TxBufferOffset++;
        if (pSPIRIT1TxCtrlStruct->TxBufferOffset >= 8)
        {
            pSPIRIT1TxCtrlStruct->TxBufferOffset = 0;
            pSPIRIT1TxCtrlStruct->TxBufferIndex++;
            if (pSPIRIT1TxCtrlStruct->TxBufferIndex >= SPRTCTRL_TEST_TX_BUFFER_LEN)   //lq 溢出判断
            {
                pSPIRIT1TxCtrlStruct->TxBufferIndex = 2;
                
                pSPIRIT1TxCtrlStruct->TxFlg = FALSE;
            } 
        }
    
        /*lq 数据结束判断 */
        if ((pSPIRIT1TxCtrlStruct->TxBufferIndex >= (pSPIRIT1TxCtrlStruct->TxBuffer[0] + 2))
            && (pSPIRIT1TxCtrlStruct->TxBufferOffset >= pSPIRIT1TxCtrlStruct->TxBuffer[1]))
        {
            pSPIRIT1TxCtrlStruct->TxBufferIndex = 2;
            pSPIRIT1TxCtrlStruct->TxBufferOffset = 0;
            
            pSPIRIT1TxCtrlStruct->TxFlg = FALSE;
        }    
    }
    else
    {
        retValue = 0;
    } 
       
    return retValue;    
}

/*******************************************************************************
* 名称  : SPRTCTRL_InstTxTestGetSignalBit
* 描述  : 获取发射信号位
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : 发射信号位
*******************************************************************************/
U8 SPRTCTRL_InstTxTestGetSignalBit(SPIRIT1TypeEnum cs)
{
    if (cs == SPIRIT1TypeA)
    {
        return SPRTCTRL_TxTestGetSignalBit(&SPIRIT1Test1);
    }
    else if (cs == SPIRIT1TypeB)
    {
        return SPRTCTRL_TxTestGetSignalBit(&SPIRIT1Test2);
    }
    else
    {
        return 0;
    }
}

/*******************************************************************************
* 名称  : SPRTCTRL_InstTxTestGetSignalBit
* 描述  : 获取发射信号位
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : 发射信号位
*******************************************************************************/
U8 SPRTCTRL_InstTxGetSignalBit(void)
{
    return SPRTCTRL_TxGetSignalBit(&SPIRIT1TxCtrlInstance);
}

/*******************************************************************************
* 名称  : SPRTCTRL_GetTxTestFlg
* 描述  : 发射测试状态
* 输入  : pSPIRIT1TestStruct - SPIRIT1TestStruct结构体	  
* 输出  : 无
* 返回  : 发射测试状态
*******************************************************************************/
static U8 SPRTCTRL_GetTxTestFlg(SPIRIT1TestStruct *pSPIRIT1TestStruct)
{
    if (pSPIRIT1TestStruct->TxTestFlg == TRUE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SPRTCTRL_InstGetTxTestFlg
* 描述  : 发射测试状态
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : 发射测试状态
*******************************************************************************/
U8 SPRTCTRL_InstGetTxTestFlg(SPIRIT1TypeEnum cs)
{
    if (cs == SPIRIT1TypeA)
    {
        return SPRTCTRL_GetTxTestFlg(&SPIRIT1Test1);
    }
    else if (cs == SPIRIT1TypeB)
    {
        return SPRTCTRL_GetTxTestFlg(&SPIRIT1Test2);
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SPRTCTRL_SetTxTestFlg
* 描述  : 置位发射测试状态
* 输入  : pSPIRIT1TestStruct - SPIRIT1TestStruct结构体	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SPRTCTRL_SetTxTestFlg(SPIRIT1TestStruct *pSPIRIT1TestStruct)
{
    pSPIRIT1TestStruct->TxTestFlg = TRUE;
}

/*******************************************************************************
* 名称  : SPRTCTRL_InstSetTxTestFlg
* 描述  : 置位发射测试状态
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPRTCTRL_InstSetTxTestFlg(SPIRIT1TypeEnum cs)
{
    if (cs == SPIRIT1TypeA)
    {
        SPRTCTRL_SetTxTestFlg(&SPIRIT1Test1);
    }
    else if (cs == SPIRIT1TypeB)
    {
        SPRTCTRL_SetTxTestFlg(&SPIRIT1Test2);
    }
}

/*******************************************************************************
* 名称  : SPRTCTRL_SetTxTestFlg
* 描述  : 设置发射信号类型
* 输入  : pSPIRIT1TestStruct - SPIRIT1TestStruct结构体	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SPRTCTRL_TxTestSetTxSignalType(SPIRIT1TestStruct *pSPIRIT1TestStruct, U8 type)
{
    pSPIRIT1TestStruct->TxSignalType = type;
}

/*******************************************************************************
* 名称  : SPRTCTRL_InstTxTestSetTxSignalType
* 描述  : 设置发射信号类型
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPRTCTRL_InstTxTestSetTxSignalType(SPIRIT1TypeEnum cs, U8 type)
{
    if (cs == SPIRIT1TypeA)
    {
        SPRTCTRL_TxTestSetTxSignalType(&SPIRIT1Test1, type);
    }
    else if (cs == SPIRIT1TypeB)
    {
        SPRTCTRL_TxTestSetTxSignalType(&SPIRIT1Test2, type);
    }
}

/*******************************************************************************
* 名称  : SPRTCTRL_TxTestCtrl
* 描述  : 发射测试控制
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPRTCTRL_TxTestCtrl(SPIRIT1TypeEnum cs)
{
    SPIRIT1TestStruct *pSPIRIT1TestStruct;

    /*lq 选择实例 */
    if (cs == SPIRIT1TypeA)
    {
        pSPIRIT1TestStruct = &SPIRIT1Test1;
    }
    else if (cs == SPIRIT1TypeB)
    {
        pSPIRIT1TestStruct = &SPIRIT1Test2;    
    }
    else
    {
        return ;
    }

	switch (pSPIRIT1TestStruct->TxCtrlSta)
    {
        case SPRTCTRL_TEST_TX_CTRL_STA_IDLE:
            
            if (pSPIRIT1TestStruct->TxTestFlg == TRUE)
            {
                /*lq GPIO数据发射配置 */
                SPRTCTRL_TxTestInit(cs);
                pSPIRIT1TestStruct->TimeCnt = SetTimePeriodProcess(pSPIRIT1TestStruct->TimePeriod);
                pSPIRIT1TestStruct->TxCtrlSta = SPRTCTRL_TEST_TX_CTRL_STA_TX;            
            }

            break;
        case SPRTCTRL_TEST_TX_CTRL_STA_TX:
            
            if (pSPIRIT1TestStruct->TimeCnt == GetLocalClockTickProcess())
            {
                pSPIRIT1TestStruct->TimeCnt = SetTimePeriodProcess(pSPIRIT1TestStruct->TimePeriod);                
                
                if (pSPIRIT1TestStruct->TimeoutTx != 0)     //lq 超时为0表示不间断连续发射
                {
                    pSPIRIT1TestStruct->TimeoutCntTx++;
                    if (pSPIRIT1TestStruct->TimeoutCntTx >= pSPIRIT1TestStruct->TimeoutTx)
                    {
                        pSPIRIT1TestStruct->TimeoutCntTx = 0;
    
                        /*lq 退出收发状态 */
                        SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_SABORT);
                        pSPIRIT1TestStruct->TxCtrlSta = SPRTCTRL_TEST_TX_CTRL_STA_HALT;                 
                    }                 
                }
            }
            
            break;
        case SPRTCTRL_TEST_TX_CTRL_STA_HALT:

            if (pSPIRIT1TestStruct->TimeCnt == GetLocalClockTickProcess())
            {
                pSPIRIT1TestStruct->TimeCnt = SetTimePeriodProcess(pSPIRIT1TestStruct->TimePeriod);                
                pSPIRIT1TestStruct->TimeoutCntHalt++;
                if (pSPIRIT1TestStruct->TimeoutCntHalt >= pSPIRIT1TestStruct->TimeoutHalt)
                {
                    pSPIRIT1TestStruct->TimeoutCntHalt = 0;

                    /*lq 进入TX态 */
                    SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_TX);
                    SPRTCTRL_TxTestResetSignal(pSPIRIT1TestStruct);                                        
                    pSPIRIT1TestStruct->TxCtrlSta = SPRTCTRL_TEST_TX_CTRL_STA_TX;                     
                }
            }
                        
            break;
        default :

            pSPIRIT1TestStruct->TxCtrlSta = SPRTCTRL_TEST_TX_CTRL_STA_IDLE;
            break;
    }
     
}

/*******************************************************************************
* 名称  : SPRTCTRL_InstTxTestCtrl
* 描述  : 发射测试控制
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPRTCTRL_InstTxTestCtrl(void)
{
    SPRTCTRL_TxTestCtrl(SPIRIT1TypeA);    
    SPRTCTRL_TxTestCtrl(SPIRIT1TypeB); 
}

/*******************************************************************************
* 名称  : SPRTCTRL_TxTestIsTx
* 描述  : 判断是否允许发射
* 输入  : pSPIRIT1TestStruct - SPIRIT1TestStruct结构体	  
* 输出  : 无
* 返回  : TRUE - 发射， FALSE - 不发射
*******************************************************************************/
static U8 SPRTCTRL_TxTestIsTx(SPIRIT1TestStruct *pSPIRIT1TestStruct)
{
    if (pSPIRIT1TestStruct->TxCtrlSta == SPRTCTRL_TEST_TX_CTRL_STA_TX)
    {
        return TRUE;
    } 
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SPRTCTRL_TxTestIsTx
* 描述  : 判断是否允许发射
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : TRUE - 发射， FALSE - 不发射
*******************************************************************************/
U8 SPRTCTRL_InstTxTestIsTx(SPIRIT1TypeEnum cs)
{
    if (cs == SPIRIT1TypeA)
    {
        return SPRTCTRL_TxTestIsTx(&SPIRIT1Test1);
    }
    else if (cs == SPIRIT1TypeB)
    {
        return SPRTCTRL_TxTestIsTx(&SPIRIT1Test2);
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : SPRTCTRL_TransmitProcess
* 描述  : 发射控制
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPRTCTRL_TransmitProcess(void)
{
    FrameTxCellStruct *pFrameTxCell;    
    SPIRIT1TxCtrlStruct *pSPIRIT1TxCtrlStruct;
    S32 currSlotSum;
    S16 SlotDiff1;
    S16 SlotDiff2;

    pSPIRIT1TxCtrlStruct = &SPIRIT1TxCtrlInstance;

    currSlotSum = GetSlotNum();

    switch (pSPIRIT1TxCtrlStruct->TxCtrlState)
    {
        case SPRTCTRL_TX_CTRL_STATE_IDLE:

            pFrameTxCell = (FrameTxCellStruct *)malloc(sizeof(FrameTxCellStruct));
            memset((FrameTxCellStruct *)pFrameTxCell, 0, sizeof(FrameTxCellStruct));

            /*lq 帧发送队列非空 */
            if (DLS_InstFrameTxFifoDeque(pFrameTxCell) == TRUE)
            {
                pSPIRIT1TxCtrlStruct->TxSlot = pFrameTxCell->TxSlot;
                pSPIRIT1TxCtrlStruct->TxChannel = pFrameTxCell->TxChannel;
                
                memcpy(&pSPIRIT1TxCtrlStruct->TxBuffer[0], &pFrameTxCell->Data[0], pFrameTxCell->Data[0] + 3);  //lq 前2字节+整字节数+非整字节                                
                pSPIRIT1TxCtrlStruct->TxBufferIndex = 2;
                pSPIRIT1TxCtrlStruct->TxBufferOffset = 0;
                pSPIRIT1TxCtrlStruct->TxFlg = FALSE;
                                                   
                pSPIRIT1TxCtrlStruct->TxCtrlState = SPRTCTRL_TX_CTRL_STATE_TX_WAITING; 
                usart_printf("$SPIRIT1 TX CtrlState: TX_WAITING!\r\n");                                                                   
            } 

            free((FrameTxCellStruct *)pFrameTxCell);
                    
            break;    

        case SPRTCTRL_TX_CTRL_STATE_TX_WAITING:

            SlotDiff1 = pSPIRIT1TxCtrlStruct->TxSlot - currSlotSum;
            SlotDiff2 = pSPIRIT1TxCtrlStruct->TxSlot + 2250 - currSlotSum;

            /*lq 预定时隙有效 */
            if (!((SlotDiff1 > TIME_OFFSET_SEND_TO_TX)
                || (SlotDiff1 <= 0 && SlotDiff2 > TIME_OFFSET_SEND_TO_TX)))
            {
                /*lq 发射时隙到 */
                if ((currSlotSum + 1) % 2250 == pSPIRIT1TxCtrlStruct->TxSlot)    //lq CMX7042在收到发射任务的时隙的下一时隙开始发射
                {
                    pSPIRIT1TxCtrlStruct->TxCtrlState = SPRTCTRL_TX_CTRL_STATE_TX_EXEC;
                    usart_printf("$SPIRIT1 TX CtrlState: TX_EXEC!\r\n");                                                                   
                }                
            }
            else
            {
                pSPIRIT1TxCtrlStruct->TxCtrlState = SPRTCTRL_TX_CTRL_STATE_IDLE;
                usart_printf("$SPIRIT1 TX CtrlState: TX_IDLE!\r\n");                                                                   
            } 
                       
            break;

        case SPRTCTRL_TX_CTRL_STATE_TX_EXEC:

            /*lq 收发转换 */
            RxTxCtrl(RXTX_CTRL_TYPE_TX, 0, 0);            

            /*lq SPIRIT1发射配置 */
            SPIRIT1_TxCfg(SPIRIT1TypeA, pSPIRIT1TxCtrlStruct->TxChannel);                     
            pSPIRIT1TxCtrlStruct->TxFlg = TRUE;

    		/*lq 设置发射超时定时 */
            pSPIRIT1TxCtrlStruct->TimeCnt = SetTimePeriodProcess(pSPIRIT1TxCtrlStruct->TimePeriod);
            pSPIRIT1TxCtrlStruct->TxCtrlState = SPRTCTRL_TX_CTRL_STATE_DONE_WAITING;
            usart_printf("$SPIRIT1 TX CtrlState: DONE_WAITING!\r\n");                                                                   

            break;

        case SPRTCTRL_TX_CTRL_STATE_DONE_WAITING:

            if (SPIRIT1_IsTxDone() == TRUE) //lq... GPIO模式下发射完成标志的判断方式暂未指定
            {            
                /*lq SPIRIT1接收配置 */
                SPIRIT1_RxCfg(SPIRIT1TypeA, AIS_CHANNEL_A);

                /*lq 收发转换 */
                RxTxCtrl(RXTX_CTRL_TYPE_RX, 0, 0);                                        

                pSPIRIT1TxCtrlStruct->TxCtrlState = SPRTCTRL_TX_CTRL_STATE_IDLE;
                usart_printf("$SPIRIT1 TX CtrlState: IDLE!\r\n");                                                                   
            }
            else
            {
                /*lq 发射超时 */
                if (pSPIRIT1TxCtrlStruct->TimeCnt == GetLocalClockTickProcess())
            	{
                    /*lq SPIRIT1接收配置 */
                    SPIRIT1_RxCfg(SPIRIT1TypeA, AIS_CHANNEL_A); 

                    /*lq 收发转换 */
                    RxTxCtrl(RXTX_CTRL_TYPE_RX, 0, 0);                                        
    
                    pSPIRIT1TxCtrlStruct->TxCtrlState = SPRTCTRL_TX_CTRL_STATE_IDLE;
                    usart_printf("$SPIRIT1 TX timeout!\r\n");                                            
                    usart_printf("$SPIRIT1 TX CtrlState: IDLE!\r\n");                                                                   
            	}
            }

            break;

        default :
            
            /*lq 默认接收 */
            /*lq SPIRIT1接收配置 */
            SPIRIT1_RxCfg(SPIRIT1TypeA, AIS_CHANNEL_A);
            RxTxCtrl(RXTX_CTRL_TYPE_RX, 0, 0);
            pSPIRIT1TxCtrlStruct->TxCtrlState = SPRTCTRL_TX_CTRL_STATE_IDLE;                                                
            usart_printf("$SPIRIT1 TX CtrlState: IDLE from default case!\r\n");                                                                                                                                                                                               
            
            break;
    }     
}

/*******************************************************************************
* 名称  : SPRTCTRL_ModuleInit
* 描述  : 模块初始化
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPRTCTRL_ModuleInit(void)
{
    SPRTCTRL_SPIRIT1TestStructInit(&SPIRIT1Test1);
    SPRTCTRL_SPIRIT1TestStructInit(&SPIRIT1Test2);
    SPRTCTRL_SPIRIT1TxCtrlStructInit(&SPIRIT1TxCtrlInstance);
}


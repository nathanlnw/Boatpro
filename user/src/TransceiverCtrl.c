/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: TransceiverCtrl.c
  Author: lq       Version : V1.0.0        Date: 2014.08.06
  Description:     Transceiver控制
  Version:         V1.0.0
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "stm32f10x.h"
#include "def.h"
#include "sotdma.h"
#include "RxTxCtrl.h"
#include "Include.h"

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

TransceiverTestStruct TransceiverTest1;
TransceiverTestStruct TransceiverTest2;
TransceiverTxCtrlStruct TransceiverTxCtrlInstance;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* 名称  : TRCTRL_TransceiverTxCtrlStructInit
* 描述  : 结构体初始化
* 输入  : TransceiverTxCtrlStruct - pTransceiverTxCtrlStruct结构体	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void TRCTRL_TransceiverTxCtrlStructInit(TransceiverTxCtrlStruct *pTransceiverTxCtrlStruct)
{
	pTransceiverTxCtrlStruct->TimeCnt = 0;
	pTransceiverTxCtrlStruct->TimePeriod = TRCTRL_TIMEOUT_TX_DONE_WAITING;

    pTransceiverTxCtrlStruct->TxCtrlState = TRCTRL_TX_CTRL_STATE_IDLE;	
    pTransceiverTxCtrlStruct->TxFlg = FALSE;
    pTransceiverTxCtrlStruct->TxDoneFlg = FALSE;
    pTransceiverTxCtrlStruct->TxSlot = 0;
    pTransceiverTxCtrlStruct->TxChannel = ENUM_AIS_CHANNEL_A;
    pTransceiverTxCtrlStruct->TxDataCtrlState = TRCTRL_TX_DATA_CTRL_STATE_IDLE;	    
    pTransceiverTxCtrlStruct->TxDataStartBitCnt = 0;
    pTransceiverTxCtrlStruct->TxDataEndBitCnt = 0;
        
    pTransceiverTxCtrlStruct->TxBufferIndex = 0;
    pTransceiverTxCtrlStruct->TxBufferOffset = 0;            
}

/*******************************************************************************
* 名称  : TRCTRL_InstTransceiverTxCtrlStructInit
* 描述  : 实例初始化
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TRCTRL_InstTransceiverTxCtrlStructInit(void)
{
    TRCTRL_TransceiverTxCtrlStructInit(&TransceiverTxCtrlInstance);       
}

/*******************************************************************************
* 名称  : TRCTRL_TransceiverTestStructInit
* 描述  : 结构体初始化
* 输入  : pTransceiverTestStruct - TransceiverTestStruct结构体	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void TRCTRL_TransceiverTestStructInit(TransceiverTestStruct *pTransceiverTestStruct)
{
    pTransceiverTestStruct->TimeCnt = 0;
    pTransceiverTestStruct->TimePeriod = TRCTRL_TIME_PERIOD;
    pTransceiverTestStruct->TimeoutTx = 0;
    pTransceiverTestStruct->TimeoutHalt = 0;
    pTransceiverTestStruct->TimeoutCntTx = 0;
    pTransceiverTestStruct->TimeoutCntHalt = 0;
    pTransceiverTestStruct->TxCtrlSta = TRCTRL_TEST_TX_CTRL_STA_IDLE;
    pTransceiverTestStruct->TxTestFlg = FALSE;
    pTransceiverTestStruct->TxChannel = ENUM_AIS_CHANNEL_A;
    pTransceiverTestStruct->TxSignalType = TRCTRL_TEST_TX_SIGNAL_NUM1; 
    pTransceiverTestStruct->TxBufferIndex = 0;
    pTransceiverTestStruct->TxBufferOffset = 0;             
}

/*******************************************************************************
* 名称  : TRCTRL_InstTransceiverTestStructInit
* 描述  : 结构体初始化
* 输入  : cs - Transceiver芯片类型  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TRCTRL_InstTransceiverTestStructInit(TransceiverTypeEnum cs)
{
    if (cs == TransceiverTypeA)
    {
        TRCTRL_TransceiverTestStructInit(&TransceiverTest1);
    }
    else if (cs == TransceiverTypeB)
    {
        TRCTRL_TransceiverTestStructInit(&TransceiverTest2);
    }            
}

/*******************************************************************************
* 名称  : TRCTRL_TxTestSignalInit
* 描述  : 初始化SPIRIT发射测试信号
* 输入  : pTransceiverTestStruct - TransceiverTestStruct结构体	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void TRCTRL_TxTestSignalInit(TransceiverTestStruct *pTransceiverTestStruct)
{
    U8 *pBuff;
   
    if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_NUM1)
    {
        pBuff = (U8 *)malloc((ST_SIGNAL_LEN + 2) * sizeof(U8));
        memset((U8 *)pBuff, 0, (ST_SIGNAL_LEN + 2) * sizeof(U8));
        pBuff[0] = ST_SIGNAL_LEN;
        pBuff[1] = 0;
        memcpy(&pBuff[2], &StSignal1[0], ST_SIGNAL_LEN);                
        DLS_FrameBuildTest(&pBuff[0], pTransceiverTestStruct->TxBuffer);
        free((U8 *)pBuff);

        pTransceiverTestStruct->TimeoutTx = TRCTRL_TEST_TX_TIMEOUT_TX_1;            //lq 不同的信号的发射周期和发射时长可以分别控制
        pTransceiverTestStruct->TimeoutHalt = TRCTRL_TEST_TX_TIMEOUT_HALT_1;                            
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_NUM2)
    {
        pBuff = (U8 *)malloc((ST_SIGNAL_LEN + 2) * sizeof(U8));
        memset((U8 *)pBuff, 0, (ST_SIGNAL_LEN + 2) * sizeof(U8));
        pBuff[0] = ST_SIGNAL_LEN;
        pBuff[1] = 0;
        memcpy(&pBuff[2], &StSignal2[0], ST_SIGNAL_LEN);                
        DLS_FrameBuildTest(&pBuff[0], pTransceiverTestStruct->TxBuffer);
        free((U8 *)pBuff);

        pTransceiverTestStruct->TimeoutTx = TRCTRL_TEST_TX_TIMEOUT_TX_2;
        pTransceiverTestStruct->TimeoutHalt = TRCTRL_TEST_TX_TIMEOUT_HALT_2;                             
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_NUM3)
    {
        pBuff = (U8 *)malloc((ST_SIGNAL_LEN + 2) * sizeof(U8));
        memset((U8 *)pBuff, 0, (ST_SIGNAL_LEN + 2) * sizeof(U8));
        pBuff[0] = ST_SIGNAL_LEN;
        pBuff[1] = 0;
        memcpy(&pBuff[2], &StSignal3[0], ST_SIGNAL_LEN);                
        DLS_FrameBuildTest(&pBuff[0], pTransceiverTestStruct->TxBuffer);
        free((U8 *)pBuff);

        pTransceiverTestStruct->TimeoutTx = TRCTRL_TEST_TX_TIMEOUT_TX_3;
        pTransceiverTestStruct->TimeoutHalt = TRCTRL_TEST_TX_TIMEOUT_HALT_3;                             
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_CW)
    {
        memset((U8 *)(pTransceiverTestStruct->TxBuffer), 0, (TRCTRL_TEST_TX_BUFFER_LEN) * sizeof(U8));        
        pTransceiverTestStruct->TimeoutTx = TRCTRL_TEST_TX_TIMEOUT_TX_4;
        pTransceiverTestStruct->TimeoutHalt = TRCTRL_TEST_TX_TIMEOUT_HALT_4;                             
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_PN9)
    {
        memset((U8 *)(pTransceiverTestStruct->TxBuffer), 0, (TRCTRL_TEST_TX_BUFFER_LEN) * sizeof(U8));        
        pTransceiverTestStruct->TimeoutTx = TRCTRL_TEST_TX_TIMEOUT_TX_5;
        pTransceiverTestStruct->TimeoutHalt = TRCTRL_TEST_TX_TIMEOUT_HALT_5;                             
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_ALL0)
    {
        memset((U8 *)(pTransceiverTestStruct->TxBuffer), 0, (TRCTRL_TEST_TX_BUFFER_LEN) * sizeof(U8));    //lq all '0'        
        pTransceiverTestStruct->TxBuffer[0] = TRCTRL_TEST_TX_BUFFER_LEN - 2;
        pTransceiverTestStruct->TxBuffer[1] = 0;
        pTransceiverTestStruct->TimeoutTx = TRCTRL_TEST_TX_TIMEOUT_TX_6;            //lq 不同的信号的发射周期和发射时长可以分别控制
        pTransceiverTestStruct->TimeoutHalt = TRCTRL_TEST_TX_TIMEOUT_HALT_6;                            
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_ALL1)
    {
        memset((U8 *)(pTransceiverTestStruct->TxBuffer), 0xFF, (TRCTRL_TEST_TX_BUFFER_LEN) * sizeof(U8)); //lq all '1'        
        pTransceiverTestStruct->TxBuffer[0] = TRCTRL_TEST_TX_BUFFER_LEN - 2;
        pTransceiverTestStruct->TxBuffer[1] = 0;
        pTransceiverTestStruct->TimeoutTx = TRCTRL_TEST_TX_TIMEOUT_TX_7;
        pTransceiverTestStruct->TimeoutHalt = TRCTRL_TEST_TX_TIMEOUT_HALT_7;                             
    }
    pTransceiverTestStruct->TxBufferIndex = 2;
    pTransceiverTestStruct->TxBufferOffset = 0;
}

/*******************************************************************************
* 名称  : TRCTRL_TxTestInitSignal
* 描述  : 初始化SPIRIT发射测试信号
* 输入  : pTransceiverTestStruct - TransceiverTestStruct结构体	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void TRCTRL_TxTestCfgInit(TransceiverTestStruct *pTransceiverTestStruct,
                                 SI446XChipEnum cs)
{
    if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_NUM1)
    {
        SI446X_TxCfg(cs, pTransceiverTestStruct->TxChannel);
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_NUM2)
    {
        SI446X_TxCfg(cs, pTransceiverTestStruct->TxChannel);
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_NUM3)
    {
        SI446X_TxCfg(cs, pTransceiverTestStruct->TxChannel);
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_CW)
    {
        SI446X_CfgCwTx(cs, pTransceiverTestStruct->TxChannel);
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_PN9)
    {
        SI446X_CfgPN9Tx(cs, pTransceiverTestStruct->TxChannel);                            
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_ALL0)
    {
        SI446X_TxCfg(cs, pTransceiverTestStruct->TxChannel);
    }
    else if (pTransceiverTestStruct->TxSignalType == TRCTRL_TEST_TX_SIGNAL_ALL1)
    {
        SI446X_TxCfg(cs, pTransceiverTestStruct->TxChannel);
    }
}

/*******************************************************************************
* 名称  : TRCTRL_TxTestInit
* 描述  : SPIRIT发射测试初始化
* 输入  : cs - Transceiver芯片类型	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void TRCTRL_TxTestInit(TransceiverTypeEnum cs)
{
    TransceiverTestStruct *pTransceiverTestStruct;
    SI446XChipEnum SI446X_cs;

    /*lq 选择实例 */
    if (cs == TransceiverTypeA)
    {
        pTransceiverTestStruct = &TransceiverTest1;
        SI446X_cs = SI446X_CHIP_A;
    }
    else if (cs == TransceiverTypeB)
    {
        pTransceiverTestStruct = &TransceiverTest2;    
        SI446X_cs = SI446X_CHIP_B;
    }
    else
    {
        return ;
    }

    /*lq 芯片配置初始化 */
    TRCTRL_TxTestCfgInit(pTransceiverTestStruct, SI446X_cs);

    /*lq 发射信号初始化 */
    TRCTRL_TxTestSignalInit(pTransceiverTestStruct);
}

/*******************************************************************************
* 名称  : TRCTRL_TxTestResetSignal
* 描述  : 复位SPIRIT发射测试信号
* 输入  : pTransceiverTestStruct - TransceiverTestStruct结构体	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void TRCTRL_TxTestResetSignal(TransceiverTestStruct *pTransceiverTestStruct)
{
    pTransceiverTestStruct->TxBufferIndex = 2;
    pTransceiverTestStruct->TxBufferOffset = 0;     
}

/*******************************************************************************
* 名称  : TRCTRL_TxTestGetSignalBit
* 描述  : 获取发射信号位
* 输入  : pTransceiverTestStruct - TransceiverTestStruct结构体	  
* 输出  : 无
* 返回  : 发射信号位
*******************************************************************************/
static U8 TRCTRL_TxTestGetSignalBit(TransceiverTestStruct *pTransceiverTestStruct)
{
    U8 retValue;
    
    if ((pTransceiverTestStruct->TxBuffer[pTransceiverTestStruct->TxBufferIndex]
         & (1u << pTransceiverTestStruct->TxBufferOffset)) == 0)
    {
        retValue = 0;
    }    
    else
    {
        retValue = 1;
    }

    pTransceiverTestStruct->TxBufferOffset++;
    if (pTransceiverTestStruct->TxBufferOffset >= 8)
    {
        pTransceiverTestStruct->TxBufferOffset = 0;
        pTransceiverTestStruct->TxBufferIndex++;
        if (pTransceiverTestStruct->TxBufferIndex >= TRCTRL_TEST_TX_BUFFER_LEN)   //lq 溢出判断
        {
            pTransceiverTestStruct->TxBufferIndex = 2;
        } 
    }

    /*lq 数据结束判断 */
    if ((pTransceiverTestStruct->TxBufferIndex >= (pTransceiverTestStruct->TxBuffer[0] + 2))
        && (pTransceiverTestStruct->TxBufferOffset >= pTransceiverTestStruct->TxBuffer[1]))
    {
        pTransceiverTestStruct->TxBufferIndex = 2;
        pTransceiverTestStruct->TxBufferOffset = 0;
    }

    return retValue;
}

/*******************************************************************************
* 名称  : TRCTRL_TxGetSignalBit
* 描述  : 获取发射信号位
* 输入  : pTransceiverTxCtrlStruct - TransceiverTxCtrlStruct结构体	  
* 输出  : 无
* 返回  : 发射信号位
*******************************************************************************/
static U8 TRCTRL_TxGetSignalBit(TransceiverTxCtrlStruct *pTransceiverTxCtrlStruct)
{
    U8 retValue;

    if (pTransceiverTxCtrlStruct->TxFlg == TRUE)
    {
        if ((pTransceiverTxCtrlStruct->TxBuffer[pTransceiverTxCtrlStruct->TxBufferIndex]
             & (1u << pTransceiverTxCtrlStruct->TxBufferOffset)) == 0)
        {
            retValue = 0;
        }    
        else
        {
            retValue = 1;
        }
    
        pTransceiverTxCtrlStruct->TxBufferOffset++;
        if (pTransceiverTxCtrlStruct->TxBufferOffset >= 8)
        {
            pTransceiverTxCtrlStruct->TxBufferOffset = 0;
            pTransceiverTxCtrlStruct->TxBufferIndex++;
            if (pTransceiverTxCtrlStruct->TxBufferIndex >= TRCTRL_TEST_TX_BUFFER_LEN)   //lq 溢出判断
            {
                pTransceiverTxCtrlStruct->TxBufferIndex = 2;
                
                pTransceiverTxCtrlStruct->TxFlg = FALSE;
            } 
        }
    
        /*lq 数据结束判断 */
        if ((pTransceiverTxCtrlStruct->TxBufferIndex >= (pTransceiverTxCtrlStruct->TxBuffer[0] + 2))
            && (pTransceiverTxCtrlStruct->TxBufferOffset >= pTransceiverTxCtrlStruct->TxBuffer[1]))
        {
            pTransceiverTxCtrlStruct->TxBufferIndex = 2;
            pTransceiverTxCtrlStruct->TxBufferOffset = 0;
            
            pTransceiverTxCtrlStruct->TxFlg = FALSE;
        }    
    }
    else
    {
        retValue = 0;
    } 
       
    return retValue;    
}

/*******************************************************************************
* 名称  : TRCTRL_TxSetTxDoneFlg
* 描述  : 设置发射状态下发射完成标志
* 输入  : pTransceiverTxCtrlStruct - TransceiverTxCtrlStruct结构体
          flag - 标志状态	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void TRCTRL_TxSetTxDoneFlg(TransceiverTxCtrlStruct *pTransceiverTxCtrlStruct,
                                  U8 flag)
{
    pTransceiverTxCtrlStruct->TxDoneFlg = flag;
}

/*******************************************************************************
* 名称  : TRCTRL_InstTxSetTxDoneFlg
* 描述  : 设置发射状态下发射完成标志
* 输入  : flag - 标志状态	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TRCTRL_InstTxSetTxDoneFlg(U8 flag)
{
    TRCTRL_TxSetTxDoneFlg(&TransceiverTxCtrlInstance, flag);
}

/*******************************************************************************
* 名称  : TRCTRL_TxGetTxFlg
* 描述  : 发射状态下获取发射标志
* 输入  : pTransceiverTxCtrlStruct - TransceiverTxCtrlStruct结构体	  
* 输出  : 无
* 返回  : 发射标志
*******************************************************************************/
static U8 TRCTRL_TxGetTxFlg(TransceiverTxCtrlStruct *pTransceiverTxCtrlStruct)
{
    return pTransceiverTxCtrlStruct->TxFlg;
}

/*******************************************************************************
* 名称  : TRCTRL_InstTxGetTxFlg
* 描述  : 发射状态下获取发射标志
* 输入  : pTransceiverTxCtrlStruct - TransceiverTxCtrlStruct结构体	  
* 输出  : 无
* 返回  : 发射标志
*******************************************************************************/
U8 TRCTRL_InstTxGetTxFlg(void)
{
    return TRCTRL_TxGetTxFlg(&TransceiverTxCtrlInstance);
}

/*******************************************************************************
* 名称  : TRCTRL_InstTxTestGetSignalBit
* 描述  : 获取发射信号位
* 输入  : cs - Transceiver芯片类型	  
* 输出  : 无
* 返回  : 发射信号位
*******************************************************************************/
U8 TRCTRL_InstTxTestGetSignalBit(TransceiverTypeEnum cs)
{
    if (cs == TransceiverTypeA)
    {
        return TRCTRL_TxTestGetSignalBit(&TransceiverTest1);
    }
    else if (cs == TransceiverTypeB)
    {
        return TRCTRL_TxTestGetSignalBit(&TransceiverTest2);
    }
    else
    {
        return 0;
    }
}

/*******************************************************************************
* 名称  : TRCTRL_InstTxTestGetSignalBit
* 描述  : 获取发射信号位
* 输入  : cs - Transceiver芯片类型	  
* 输出  : 无
* 返回  : 发射信号位
*******************************************************************************/
U8 TRCTRL_InstTxGetSignalBit(void)
{
    return TRCTRL_TxGetSignalBit(&TransceiverTxCtrlInstance);
}

/*******************************************************************************
* 名称  : TRCTRL_GetTxTestFlg
* 描述  : 发射测试状态
* 输入  : pTransceiverTestStruct - TransceiverTestStruct结构体	  
* 输出  : 无
* 返回  : 发射测试状态
*******************************************************************************/
static U8 TRCTRL_GetTxTestFlg(TransceiverTestStruct *pTransceiverTestStruct)
{
    if (pTransceiverTestStruct->TxTestFlg == TRUE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : TRCTRL_InstGetTxTestFlg
* 描述  : 发射测试状态
* 输入  : cs - Transceiver芯片类型	  
* 输出  : 无
* 返回  : 发射测试状态
*******************************************************************************/
U8 TRCTRL_InstGetTxTestFlg(TransceiverTypeEnum cs)
{
    if (cs == TransceiverTypeA)
    {
        return TRCTRL_GetTxTestFlg(&TransceiverTest1);
    }
    else if (cs == TransceiverTypeB)
    {
        return TRCTRL_GetTxTestFlg(&TransceiverTest2);
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : TRCTRL_SetTxTestFlg
* 描述  : 置位发射测试状态
* 输入  : pTransceiverTestStruct - TransceiverTestStruct结构体	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void TRCTRL_SetTxTestFlg(TransceiverTestStruct *pTransceiverTestStruct)
{
    pTransceiverTestStruct->TxTestFlg = TRUE;
}

/*******************************************************************************
* 名称  : TRCTRL_InstSetTxTestFlg
* 描述  : 置位发射测试状态
* 输入  : cs - Transceiver芯片类型	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TRCTRL_InstSetTxTestFlg(TransceiverTypeEnum cs)
{
    if (cs == TransceiverTypeA)
    {
        TRCTRL_SetTxTestFlg(&TransceiverTest1);
    }
    else if (cs == TransceiverTypeB)
    {
        TRCTRL_SetTxTestFlg(&TransceiverTest2);
    }
}

/*******************************************************************************
* 名称  : TRCTRL_TxTestSetTxSignalType
* 描述  : 设置发射信号类型
* 输入  : pTransceiverTestStruct - TransceiverTestStruct结构体	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void TRCTRL_TxTestSetTxSignalType(TransceiverTestStruct *pTransceiverTestStruct, U8 type)
{
    pTransceiverTestStruct->TxSignalType = type;
}

/*******************************************************************************
* 名称  : TRCTRL_InstTxTestSetTxSignalType
* 描述  : 设置发射信号类型
* 输入  : cs - Transceiver芯片类型	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TRCTRL_InstTxTestSetTxSignalType(TransceiverTypeEnum cs, U8 type)
{
    if (cs == TransceiverTypeA)
    {
        TRCTRL_TxTestSetTxSignalType(&TransceiverTest1, type);
    }
    else if (cs == TransceiverTypeB)
    {
        TRCTRL_TxTestSetTxSignalType(&TransceiverTest2, type);
    }
}

/*******************************************************************************
* 名称  : TRCTRL_TxTestSetTxChannel
* 描述  : 设置发射信道
* 输入  : pTransceiverTestStruct - TransceiverTestStruct结构体
          channel - 信道	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void TRCTRL_TxTestSetTxChannel(TransceiverTestStruct *pTransceiverTestStruct,
                                      AisChannelEnum channel)
{
    pTransceiverTestStruct->TxChannel = channel;
}

/*******************************************************************************
* 名称  : TRCTRL_InstTxTestSetTxChannel
* 描述  : 设置发射信道
* 输入  : cs - Transceiver芯片类型
          channel - 信道	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TRCTRL_InstTxTestSetTxChannel(TransceiverTypeEnum cs, AisChannelEnum channel)
{
    if (cs == TransceiverTypeA)
    {
        TRCTRL_TxTestSetTxChannel(&TransceiverTest1, channel);
    }
    else if (cs == TransceiverTypeB)
    {
        TRCTRL_TxTestSetTxChannel(&TransceiverTest2, channel);
    }
}

/*******************************************************************************
* 名称  : TRCTRL_TxTestCtrl
* 描述  : 发射测试控制
* 输入  : cs - Transceiver芯片类型	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TRCTRL_TxTestCtrl(TransceiverTypeEnum cs)
{
    TransceiverTestStruct *pTransceiverTestStruct;

    /*lq 选择实例 */
    if (cs == TransceiverTypeA)
    {
        pTransceiverTestStruct = &TransceiverTest1;
    }
    else if (cs == TransceiverTypeB)
    {
        pTransceiverTestStruct = &TransceiverTest2;    
    }
    else
    {
        return ;
    }

	switch (pTransceiverTestStruct->TxCtrlSta)
    {
        case TRCTRL_TEST_TX_CTRL_STA_IDLE:
            
            if (pTransceiverTestStruct->TxTestFlg == TRUE)
            {
                /*lq GPIO数据发射配置 */
                TRCTRL_TxTestInit(cs);
                RF_PowerOn();	    
                pTransceiverTestStruct->TimeCnt = SetTimePeriodProcess(pTransceiverTestStruct->TimePeriod);
                pTransceiverTestStruct->TxCtrlSta = TRCTRL_TEST_TX_CTRL_STA_TX;            
            }

            break;
        case TRCTRL_TEST_TX_CTRL_STA_TX:
            
            if (pTransceiverTestStruct->TimeCnt == GetLocalClockTickProcess())
            {
                pTransceiverTestStruct->TimeCnt = SetTimePeriodProcess(pTransceiverTestStruct->TimePeriod);                
                
                if (pTransceiverTestStruct->TimeoutTx != 0)     //lq 超时为0表示不间断连续发射
                {
                    pTransceiverTestStruct->TimeoutCntTx++;
                    if (pTransceiverTestStruct->TimeoutCntTx >= pTransceiverTestStruct->TimeoutTx)
                    {
                        pTransceiverTestStruct->TimeoutCntTx = 0;
    
                        /*lq 退出收发状态 */
    	                RF_PowerDown();	    
                        SI446X_StateTo(SI446X_MODE_IDLE, SI446X_CHIP_A);
                        pTransceiverTestStruct->TxCtrlSta = TRCTRL_TEST_TX_CTRL_STA_HALT;                 
                    }                 
                }
            }
            
            break;
        case TRCTRL_TEST_TX_CTRL_STA_HALT:

            if (pTransceiverTestStruct->TimeCnt == GetLocalClockTickProcess())
            {
                pTransceiverTestStruct->TimeCnt = SetTimePeriodProcess(pTransceiverTestStruct->TimePeriod);                
                pTransceiverTestStruct->TimeoutCntHalt++;
                if (pTransceiverTestStruct->TimeoutCntHalt >= pTransceiverTestStruct->TimeoutHalt)
                {
                    pTransceiverTestStruct->TimeoutCntHalt = 0;

                    /*lq 进入TX态 */
                    SI446X_StateTo(SI446X_MODE_TX, SI446X_CHIP_A);
    	            RF_PowerOn();	    
                    TRCTRL_TxTestResetSignal(pTransceiverTestStruct);                                        
                    pTransceiverTestStruct->TxCtrlSta = TRCTRL_TEST_TX_CTRL_STA_TX;                     
                }
            }
                        
            break;
        default :

            pTransceiverTestStruct->TxCtrlSta = TRCTRL_TEST_TX_CTRL_STA_IDLE;
            break;
    }
     
}

/*******************************************************************************
* 名称  : TRCTRL_InstTxTestCtrl
* 描述  : 发射测试控制
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TRCTRL_InstTxTestCtrl(void)
{
    TRCTRL_TxTestCtrl(TransceiverTypeA);    
    TRCTRL_TxTestCtrl(TransceiverTypeB); 
}

/*******************************************************************************
* 名称  : TRCTRL_TxTestIsTx
* 描述  : 判断是否允许发射
* 输入  : pTransceiverTestStruct - TransceiverTestStruct结构体	  
* 输出  : 无
* 返回  : TRUE - 发射， FALSE - 不发射
*******************************************************************************/
static U8 TRCTRL_TxTestIsTx(TransceiverTestStruct *pTransceiverTestStruct)
{
    if (pTransceiverTestStruct->TxCtrlSta == TRCTRL_TEST_TX_CTRL_STA_TX)
    {
        return TRUE;
    } 
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : TRCTRL_TxTestIsTx
* 描述  : 判断是否允许发射
* 输入  : cs - Transceiver芯片类型	  
* 输出  : 无
* 返回  : TRUE - 发射， FALSE - 不发射
*******************************************************************************/
U8 TRCTRL_InstTxTestIsTx(TransceiverTypeEnum cs)
{
    if (cs == TransceiverTypeA)
    {
        return TRCTRL_TxTestIsTx(&TransceiverTest1);
    }
    else if (cs == TransceiverTypeB)
    {
        return TRCTRL_TxTestIsTx(&TransceiverTest2);
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : TRCTRL_TransmitProcess
* 描述  : 发射控制
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TRCTRL_TransmitProcess(void)
{
    FrameTxCellStruct *pFrameTxCell;    
    TransceiverTxCtrlStruct *pTransceiverTxCtrlStruct;
    S32 currFrame;
    S16 currSlot;
//    S16 currPhase;
    S16 slotDiff;
    U8  txSlotValidFlg = FALSE;
    U8  txDoneFlg = FALSE;

    pTransceiverTxCtrlStruct = &TransceiverTxCtrlInstance;

    currFrame = GetFrameNum();
    currSlot = GetSlotNum();
//    currPhase = GetPhaseNum();

    switch (pTransceiverTxCtrlStruct->TxCtrlState)
    {
        case TRCTRL_TX_CTRL_STATE_IDLE:

            pFrameTxCell = (FrameTxCellStruct *)malloc(sizeof(FrameTxCellStruct));
            memset((FrameTxCellStruct *)pFrameTxCell, 0,
                sizeof(FrameTxCellStruct));

            /*lq 帧发送队列非空 */
            if (DLS_InstFrameTxFifoDeque(pFrameTxCell) == TRUE)
            {
                pTransceiverTxCtrlStruct->TxSlot = pFrameTxCell->TxSlot;
                pTransceiverTxCtrlStruct->TxChannel = pFrameTxCell->TxChannel;
                
                memcpy(&pTransceiverTxCtrlStruct->TxBuffer[0],
                    &pFrameTxCell->Data[0], pFrameTxCell->Data[0] + 3);  //lq 前2字节+整字节数+非整字节                                
                pTransceiverTxCtrlStruct->TxBufferIndex = 2;
                pTransceiverTxCtrlStruct->TxBufferOffset = 0;
                pTransceiverTxCtrlStruct->TxFlg = FALSE;
                pTransceiverTxCtrlStruct->TxDoneFlg = FALSE;
                                                   
                pTransceiverTxCtrlStruct->TxCtrlState =
                    TRCTRL_TX_CTRL_STATE_TX_INIT;
                
                debug_printf("$[%6d:%4d] %s : Ctrl state to TX_INIT!\r\n",
                    currFrame, currSlot, __func__);
            } 

            free((FrameTxCellStruct *)pFrameTxCell);
                    
            break;    

        case TRCTRL_TX_CTRL_STATE_TX_INIT:

            txSlotValidFlg = TRUE;
            if (pTransceiverTxCtrlStruct->TxSlot != SLOT_FORCE_TX_VALUE)
            {
                slotDiff = (pTransceiverTxCtrlStruct->TxSlot - currSlot
                    + FRAME_SLOT_SIZE) % FRAME_SLOT_SIZE;
    
                /*lq 发射时隙是否有效判断 */
                if (slotDiff > AIS_SLOT_INTERVAL_SEND_TO_TX)
                {
                    /*lq 时隙已过 */
                    txSlotValidFlg = FALSE;
                }                        
            }

            if (txSlotValidFlg == TRUE)
            {
                /*lq 收发转换 */
                RxTxCtrl(RXTX_CTRL_TYPE_TX, 0, 0);            
                
                /*lq Transceiver发射配置 */
                pTransceiverTxCtrlStruct->TxDataCtrlState = 
                    TRCTRL_TX_DATA_CTRL_STATE_IDLE;     //lq 进行收发机发射配置前先设置控制状态
                pTransceiverTxCtrlStruct->TxFlg = TRUE;
                SI446X_TxCfg(SI446X_CHIP_A, pTransceiverTxCtrlStruct->TxChannel);                     
    
        		/*lq 设置发射超时定时 */
                pTransceiverTxCtrlStruct->TimeCnt = SetTimePeriodProcess(
                    pTransceiverTxCtrlStruct->TimePeriod);

			    /*lq 消息发射LED指示 */
                LEDInit(LED_TYPE_RF_XMT);
    
                pTransceiverTxCtrlStruct->TxCtrlState = TRCTRL_TX_CTRL_STATE_TX_RUNNING;
                debug_printf("$[%6d:%4d] %s : Ctrl state to TX_RUNNING!\r\n",
                    currFrame, currSlot, __func__);            
            }
            else
            {
                pTransceiverTxCtrlStruct->TxCtrlState = TRCTRL_TX_CTRL_STATE_IDLE;
				if(DevDynamicPara.TestSta == EQ_MOD_TEST_STA_SELF_CHECK)
				{
					UartResponseMsg("A is not OK!");
				}
                debug_printf("$[%6d:%4d] %s : Ctrl state to TX_IDLE, Tx slot [%4d] has expired!\r\n",
                    currFrame, currSlot, __func__, pTransceiverTxCtrlStruct->TxSlot);            
            
            }

            break;

        case TRCTRL_TX_CTRL_STATE_TX_RUNNING:

            if (pTransceiverTxCtrlStruct->TxDoneFlg == TRUE)
            {            
                txDoneFlg = TRUE;
				if(DevDynamicPara.TestSta == EQ_MOD_TEST_STA_SELF_CHECK)
				{
					UartResponseMsg("A is OK!");
				}
            }
            else
            {
                /*lq 发射超时 */
                if (pTransceiverTxCtrlStruct->TimeCnt == GetLocalClockTickProcess())
            	{
                    txDoneFlg = TRUE;

    			    /*lq 超时LED指示 */
                    LEDInit(LED_TYPE_TIMEOUT);
    
					if(DevDynamicPara.TestSta == EQ_MOD_TEST_STA_SELF_CHECK)
					{
						UartResponseMsg("A is not OK!");
					}
                    debug_printf("$[%6d:%4d] %s : Transceiver TX timeout!\r\n",
                        currFrame, currSlot, __func__);
            	}
            }

            /*lq 发送完成处理 */
            if (txDoneFlg == TRUE)
            {
                /*lq 发送完成 */
                pTransceiverTxCtrlStruct->TxDoneFlg = FALSE;

                /*lq Transceiver接收配置 */
                SI446X_RxCfg(SI446X_CHIP_A, ENUM_AIS_CHANNEL_A);

                /*lq 收发转换 */
                RxTxCtrl(RXTX_CTRL_TYPE_RX, 0, 0);                                                    

			    /*lq 超时LED指示清除 */
                LEDInit(LED_TYPE_YELLOW_NONE);

                pTransceiverTxCtrlStruct->TxCtrlState = TRCTRL_TX_CTRL_STATE_IDLE;

                debug_printf("$[%6d:%4d] %s : Ctrl state to IDLE!\r\n",
                    currFrame, currSlot, __func__);
            }

            break;

        default :
            
            /*lq 默认接收 */
            /*lq Transceiver接收配置 */
            SI446X_RxCfg(SI446X_CHIP_A, ENUM_AIS_CHANNEL_A);
            RxTxCtrl(RXTX_CTRL_TYPE_RX, 0, 0);
            pTransceiverTxCtrlStruct->TxCtrlState = TRCTRL_TX_CTRL_STATE_IDLE;                                                
            debug_printf("$[%6d:%4d] %s : Ctrl state to IDLE from default case!\r\n",
                currFrame, currSlot, __func__);
            
            break;
    }     
}

/*******************************************************************************
* 名称  : TRCTRL_TxDataClockISR
* 描述  : 正常模式发射数据时钟中断服务例程
* 输入  : transceiverType - 收发机类型	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TRCTRL_TxDataClockISR(TransceiverTypeEnum transceiverType)
{
    TransceiverTxCtrlStruct *pTransceiverTxCtrlStruct;
    S16 currSlot;
    S16 currPhase;
    S16 slotDiff;
    U8  txDataEn = FALSE;

    pTransceiverTxCtrlStruct = &TransceiverTxCtrlInstance;

    currSlot = GetSlotNum();
    currPhase = GetPhaseNum();
    
    switch (pTransceiverTxCtrlStruct->TxDataCtrlState)
    {
        case TRCTRL_TX_DATA_CTRL_STATE_IDLE :

            /*lq 发射使能判断 */
            if (TRCTRL_TxGetTxFlg(pTransceiverTxCtrlStruct) == TRUE)
            {
                pTransceiverTxCtrlStruct->TxDataCtrlState = 
                    TRCTRL_TX_DATA_CTRL_STATE_START;
            }

            break;

        case TRCTRL_TX_DATA_CTRL_STATE_START :

            pTransceiverTxCtrlStruct->TxDataCtrlState = 
                TRCTRL_TX_DATA_CTRL_STATE_READY;

            if (pTransceiverTxCtrlStruct->TxSlot != SLOT_FORCE_TX_VALUE)
            {
                slotDiff = (pTransceiverTxCtrlStruct->TxSlot - currSlot
                    + FRAME_SLOT_SIZE) % FRAME_SLOT_SIZE;
    
                /*lq 发射时隙是否有效判断 */
                if (slotDiff > AIS_SLOT_INTERVAL_SEND_TO_TX)
                {
                    /*lq 时隙已过 */
                    TRCTRL_TxSetTxDoneFlg(pTransceiverTxCtrlStruct, TRUE);

                    pTransceiverTxCtrlStruct->TxDataCtrlState = 
                        TRCTRL_TX_DATA_CTRL_STATE_IDLE;
                }                        
            }

            break;
    
        case TRCTRL_TX_DATA_CTRL_STATE_READY :

            /*lq 发射时隙判断 */
            if (pTransceiverTxCtrlStruct->TxSlot == SLOT_FORCE_TX_VALUE)
            {
                /*lq 发射相位判断 */
                if ((currPhase >= AIS_PHASE_INTERVAL_DATA_START)
                    && (currPhase <= AIS_PHASE_INTERVAL_DATA_START
                    + AIS_BIT_PHASE_SIZE))      //lq 1bit = 5phase，数据时钟与时隙起始相位可能不同步，需用区间比较
                {
                    txDataEn = TRUE;
                }                        
            }
            else if (currSlot == pTransceiverTxCtrlStruct->TxSlot)
            {
                /*lq 发射相位判断 */
                if (currPhase >= AIS_PHASE_INTERVAL_DATA_START)
                {
                    txDataEn = TRUE;
                }
            }

            if (txDataEn == TRUE)
            {
                /*lq 功放开启 */    
                RF_PowerOn();	    

                pTransceiverTxCtrlStruct->TxDataCtrlState = 
                    TRCTRL_TX_DATA_CTRL_STATE_DATA;                        
            }

            break;

        case TRCTRL_TX_DATA_CTRL_STATE_DATA :

            if (TRCTRL_TxGetTxFlg(pTransceiverTxCtrlStruct) == TRUE)
            {
                if (TRCTRL_TxGetSignalBit(pTransceiverTxCtrlStruct) == 0)
                {
                    if (transceiverType == TransceiverTypeA)
                    {
                        SI446X_A_DATA_LOW(); 
                    }
                    else
                    {
                        SI446X_B_DATA_LOW(); 
                    }
                }
                else
                {
                    if (transceiverType == TransceiverTypeA)
                    {
                        SI446X_A_DATA_HIGH();
                    }
                    else
                    {
                        SI446X_B_DATA_HIGH();
                    }
                }

                /*lq 数据发送完成判断 */
                if (TRCTRL_TxGetTxFlg(pTransceiverTxCtrlStruct) == FALSE)
                {
                    pTransceiverTxCtrlStruct->TxDataEndBitCnt = 0;

                    pTransceiverTxCtrlStruct->TxDataCtrlState = 
                        TRCTRL_TX_DATA_CTRL_STATE_END;
                }
            }

            break;

        case TRCTRL_TX_DATA_CTRL_STATE_END :

            /*lq 数据发送完成计数 */
            pTransceiverTxCtrlStruct->TxDataEndBitCnt++;
            if (pTransceiverTxCtrlStruct->TxDataEndBitCnt
                >= AIS_BIT_TIME_DATA_END)
            {
                pTransceiverTxCtrlStruct->TxDataEndBitCnt = 0;

                /*lq 功放关闭 */
    	        RF_PowerDown();	                                
                
                TRCTRL_TxSetTxDoneFlg(pTransceiverTxCtrlStruct, TRUE);

                pTransceiverTxCtrlStruct->TxDataCtrlState = 
                    TRCTRL_TX_DATA_CTRL_STATE_IDLE;
            }

            break;
    }

}

/*******************************************************************************
* 名称  : TRCTRL_ModuleInit
* 描述  : 模块初始化
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TRCTRL_ModuleInit(void)
{
    TRCTRL_TransceiverTestStructInit(&TransceiverTest1);
    TRCTRL_TransceiverTestStructInit(&TransceiverTest2);
    TRCTRL_TransceiverTxCtrlStructInit(&TransceiverTxCtrlInstance);
}


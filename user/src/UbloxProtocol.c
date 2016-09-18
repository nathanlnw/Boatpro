/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: UbloxProtocol.c
  Author: lq       Version : V1.00        Date: 2014.01.13
  Description:     ublox protocol
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "def.h"
#include "UbloxProtocol.h"
#include "usart.h"
#include "config.h"

/*lq 模块实例定义*/
static UBXPrtCommStruct UBXPrtCommPara;

/*lq 消息定义*/
/*lq continuous mode */
const U8 UBX_PRT_MSG_CFG_RXM_CONTINUOUS[8] = {0xB5, 0x62, 0x06, 0x11, 0x02, 0x00, 0x08, 0x00};

/*lq Power Save Mode */
const U8 UBX_PRT_MSG_CFG_RXM_PSM[8] = {0xB5, 0x62, 0x06, 0x11, 0x02, 0x00, 0x08, 0x01};

/*lq PSM-cyclic operation-normal,update period=1s,search period=10s acquisition timeout=0s,on time=0s */
const U8 UBX_PRT_MSG_CFG_PM2_NORM[50] = {0xb5, 0x62, 0x06, 0x3b, 0x2c, 0x00, 0x01, 0x06,
                                         0x00, 0x00, 0x00, 0x90, 0x02, 0x00, 0xe8, 0x03,
                                         0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00,
                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x01,
                                         0x00, 0x00, 0x4f, 0xc1, 0x03, 0x00, 0x86, 0x02,
                                         0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x64, 0x40,
                                         0x01, 0x00};
/*lq PSM-cyclic operation-normal,update period=10s,search period=10s acquisition timeout=0s,on time=0s */
const U8 UBX_PRT_MSG_CFG_PM2_SAVE[50] = {0xb5, 0x62, 0x06, 0x3b, 0x2c, 0x00, 0x01, 0x06,
                                         0x00, 0x00, 0x00, 0x90, 0x02, 0x00, 0x10, 0x27,//lq 0x10, 0x27 (little endian)表示updateperiod=10000ms
                                         0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00,
                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x01,
                                         0x00, 0x00, 0x4f, 0xc1, 0x03, 0x00, 0x86, 0x02,
                                         0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x64, 0x40,
                                         0x01, 0x00};
/*lq 恢复默认配置并载入*/
const U8 UBX_PRT_MSG_CFG_CFG_DEFAULT[19] = {0xb5, 0x62, 0x06, 0x09, 0x0d, 0x00, 0xff, 0xff,
                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
                                            0x00, 0x00, 0x03};
/*lq 恢复默认配置*/
const U8 UBX_PRT_MSG_CFG_CFG_CLEAR[19] = {0xb5, 0x62, 0x06, 0x09, 0x0d, 0x00, 0xff, 0xff,
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                          0x00, 0x00, 0x03};
/*lq 保存当前配置*/
const U8 UBX_PRT_MSG_CFG_CFG_SAVE[19] = {0xb5, 0x62, 0x06, 0x09, 0x0d, 0x00, 0x00, 0x00,
                                         0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
                                         0x00, 0x00, 0x03};
/*lq 载入永久配置*/
const U8 UBX_PRT_MSG_CFG_CFG_LOAD[19] = {0xb5, 0x62, 0x06, 0x09, 0x0d, 0x00, 0x00, 0x00,
                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
                                         0x00, 0x00, 0x03};

/*lq 秒脉冲使能*/
const U8 UBX_PRT_MSG_CFG_TP5_ACTIVE[38] = {0xb5, 0x62, 0x06, 0x31, 0x20, 0x00, 0x00, 0x00,
                                           0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x40, 0x42,
                                           0x0f, 0x00, 0x40, 0x42, 0x0f, 0x00, 0x00, 0x00,
                                           0x00, 0x00, 0xa0, 0x86, 0x01, 0x00, 0x00, 0x00,
                                           0x00, 0x00, 0xf7, 0x00, 0x00, 0x00};
/*lq 秒脉冲失能*/
const U8 UBX_PRT_MSG_CFG_TP5_INACTIVE[38] = {0xb5, 0x62, 0x06, 0x31, 0x20, 0x00, 0x00, 0x00,
                                             0x00, 0x00, 0x32, 0x00, 0x00, 0x00, 0x40, 0x42,
                                             0x0f, 0x00, 0x40, 0x42, 0x0f, 0x00, 0x00, 0x00,
                                             0x00, 0x00, 0xa0, 0x86, 0x01, 0x00, 0x00, 0x00,
                                             0x00, 0x00, 0xf6, 0x00, 0x00, 0x00};
 
/*lq NEMA报文输出失能，rate=0 */
const U8 UBX_PRT_MSG_CFG_MSG_R0_GXVTG[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x05,
                                             0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R0_GXGGA[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x00,
                                             0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R0_GXGSA[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x02,
                                             0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R0_GXGSV[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x03,
                                             0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R0_GXGLL[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x01,
                                             0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R0_GXRMC[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x04,
                                             0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R0_GXGBS[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x09,
                                             0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
/*lq NEMA报文输出使能，rate=1 */
const U8 UBX_PRT_MSG_CFG_MSG_R1_GXVTG[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x05,
                                             0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R1_GXGGA[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x00,
                                             0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R1_GXGSA[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x02,
                                             0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R1_GXGSV[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x03,
                                             0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R1_GXGLL[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x01,
                                             0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R1_GXRMC[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x04,
                                             0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
const U8 UBX_PRT_MSG_CFG_MSG_R1_GXGBS[14] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x09,
                                             0x01, 0x01, 0x01, 0x01, 0x01, 0x01};

/*******************************************************************************
* 名称  : UBX_UBXPrtCommStructInit
* 描述  : UBXPrtCommStruct结构体初始化
* 输入  : pUBXPrtCommStruct-UBXPrtCommStruct类型指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void UBX_UBXPrtCommStructInit(UBXPrtCommStruct *pUBXPrtCommStruct)
{
    U16 i, j;

    pUBXPrtCommStruct->TimePeriod = UBX_PRT_TIME_WAKEUP_WAIT;
    pUBXPrtCommStruct->TimeCnt = 0;

    for (i = 0; i < UBX_PRT_MSG_BUFFER_LEN; i++)
    {
        for (j = 0; j < UBX_PRT_MSG_BUFFER_DEPTH; j++)
        {
            pUBXPrtCommStruct->MsgTxBuffer[j][i] = 0;
        }
        pUBXPrtCommStruct->MsgTx[i] = 0;             
    }

    pUBXPrtCommStruct->CtrlState = UBX_PRT_COMM_CTRL_STAT_IDLE;
    pUBXPrtCommStruct->SendRequireFlag = FALSE;
    pUBXPrtCommStruct->SendFlag = FALSE;
    pUBXPrtCommStruct->MsgClass = UBX_PRT_MSG_CLASS_CFG;
    pUBXPrtCommStruct->MsgID = UBX_PRT_MSG_ID_CFG_CFG;
    pUBXPrtCommStruct->RespFlag = FALSE;
    pUBXPrtCommStruct->AckFlag = FALSE;
    pUBXPrtCommStruct->SendCnt = 0;

    UartRxFIFOStructInit(&(pUBXPrtCommStruct->Uart2RxFIFO2));
}

/*******************************************************************************
* 名称  : UBX_UBXPrtCommInstanceInit
* 描述  : UBXPrtCommStruct结构体实例初始化
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void UBX1_UBXPrtCommInstanceInit(void)
{
    UBX_UBXPrtCommStructInit(&UBXPrtCommPara);
}

/*******************************************************************************
* 名称  : UBX_UBXPrtCommStructReset
* 描述  : UBXPrtCommStruct结构体复位
* 输入  : pUBXPrtCommStruct-UBXPrtCommStruct类型指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void UBX_UBXPrtCommStructReset(UBXPrtCommStruct *pUBXPrtCommStruct)
{
    U16 i, j;

    pUBXPrtCommStruct->TimePeriod = UBX_PRT_TIME_WAKEUP_WAIT;
    pUBXPrtCommStruct->TimeCnt = 0;

    for (i = 0; i < UBX_PRT_MSG_BUFFER_LEN; i++)
    {
        for (j = 0; j < UBX_PRT_MSG_BUFFER_DEPTH; j++)
        {
            pUBXPrtCommStruct->MsgTxBuffer[j][i] = 0;
        }
        pUBXPrtCommStruct->MsgTx[i] = 0;             
    }

    pUBXPrtCommStruct->CtrlState = UBX_PRT_COMM_CTRL_STAT_IDLE;
    pUBXPrtCommStruct->SendRequireFlag = FALSE;
    pUBXPrtCommStruct->SendFlag = FALSE;
    pUBXPrtCommStruct->MsgClass = UBX_PRT_MSG_CLASS_CFG;
    pUBXPrtCommStruct->MsgID = UBX_PRT_MSG_ID_CFG_CFG;
    pUBXPrtCommStruct->RespFlag = FALSE;
    pUBXPrtCommStruct->AckFlag = FALSE;
    pUBXPrtCommStruct->SendCnt = 0;

    UartRxFIFOStructInit(&(pUBXPrtCommStruct->Uart2RxFIFO2));
}

/*******************************************************************************
* 名称  : UBX1_UBXPrtCommInstanceReset
* 描述  : UBXPrtCommStruct结构体实例1复位
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void UBX1_UBXPrtCommInstanceReset(void)
{
    UBX_UBXPrtCommStructReset(&UBXPrtCommPara);
}

/*******************************************************************************
* 名称  : UBX_UBXPrtMsgSendBuff
* 描述  : 保存发送报文消息
* 输入  : pUBXPrtCommStruct-UBXPrtCommStruct类型指针
          pMsg-要缓存的消息		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static U8 UBX_UBXPrtMsgSendBuff(UBXPrtCommStruct *pUBXPrtCommStruct, const U8 *pMsg, U16 Len)
{
    U16 i;

    /*lq 缓存队列已满 */
    if ((pUBXPrtCommStruct->TxFIFOIn + 1) % UBX_PRT_MSG_BUFFER_DEPTH
         == pUBXPrtCommStruct->TxFIFOOut)
    {
        return FALSE;  
    }
    else
    {
        pUBXPrtCommStruct->MsgTxBuffer[pUBXPrtCommStruct->TxFIFOIn][0] = (U8)Len;    
        for (i = 0; i < Len; i++)
        {
           pUBXPrtCommStruct->MsgTxBuffer[pUBXPrtCommStruct->TxFIFOIn][i + 1] = pMsg[i];
        }
        
        pUBXPrtCommStruct->TxFIFOIn++;
        pUBXPrtCommStruct->TxFIFOIn %= UBX_PRT_MSG_BUFFER_DEPTH;
        
        return TRUE;            
    }
}

/*******************************************************************************
* 名称  : UBX_UBXPrtMsgSendFetch
* 描述  : 提取一条发送报文消息
* 输入  : pUBXPrtCommStruct-UBXPrtCommStruct类型指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static U8 UBX_UBXPrtMsgSendFetch(UBXPrtCommStruct *pUBXPrtCommStruct)
{
    U16 i;
    U16 Len;

    /*lq 消息缓存为空 */
    if (pUBXPrtCommStruct->TxFIFOIn == pUBXPrtCommStruct->TxFIFOOut)
    {
        return FALSE;    
    } 
    else
    {
        Len = pUBXPrtCommStruct->MsgTxBuffer[pUBXPrtCommStruct->TxFIFOOut][0];
        for (i = 0; i <= Len; i++)
        {
            pUBXPrtCommStruct->MsgTx[i] = pUBXPrtCommStruct->MsgTxBuffer[pUBXPrtCommStruct->TxFIFOOut][i];
        }
        pUBXPrtCommStruct->MsgClass = pUBXPrtCommStruct->MsgTx[3];
        pUBXPrtCommStruct->MsgID = pUBXPrtCommStruct->MsgTx[4];

        pUBXPrtCommStruct->TxFIFOOut++;
        pUBXPrtCommStruct->TxFIFOOut %= UBX_PRT_MSG_BUFFER_DEPTH;

        return TRUE;    
    }
}

/*******************************************************************************
* 名称  : UBX_UBXPrtMsgSendBuff
* 描述  : 保存发送报文消息
* 输入  : pMsg-要缓存的消息		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void UBX1_UBXPrtMsgSendBuff(U8 *pMsg, U16 Len)
{
    UBX_UBXPrtMsgSendBuff(&UBXPrtCommPara, pMsg, Len);
}

/*******************************************************************************
* 名称  : UBX_SetSendFlag
* 描述  : 设置发送标志
* 输入  : pUBXPrtCommStruct-UBXPrtCommStruct类型指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void UBX_SetSendFlag(UBXPrtCommStruct *pUBXPrtCommStruct)
{
    pUBXPrtCommStruct->SendFlag = TRUE;
}

/*******************************************************************************
* 名称  : UBX1_SetSendFlag
* 描述  : 设置发送标志
* 输入  : pUBXPrtCommStruct-UBXPrtCommStruct类型指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void UBX1_SetSendFlag(void)
{
    UBX_SetSendFlag(&UBXPrtCommPara);
}

/*******************************************************************************
* 名称  : UBX_ResetSendFlag
* 描述  : 复位发送标志
* 输入  : pUBXPrtCommStruct-UBXPrtCommStruct类型指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void UBX_ResetSendFlag(UBXPrtCommStruct *pUBXPrtCommStruct)
{
    pUBXPrtCommStruct->SendFlag = FALSE;
}

/*******************************************************************************
* 名称  : UBX1_ResetSendFlag
* 描述  : 复位发送标志
* 输入  : pUBXPrtCommStruct-UBXPrtCommStruct类型指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void UBX1_ResetSendFlag(void)
{
    UBX_ResetSendFlag(&UBXPrtCommPara);
}

/*******************************************************************************
* 名称 ：UBX_UartTxUbxMsg
* 描述 ：GPS串口发送消息
* 输入 ：pStr：消息数组指针
		 Len：消息字节长度
* 输出 ：无
* 返回 ：无
*******************************************************************************/
U8 UBX_UartTxUbxMsg(U8 *pStr, U16 Len)
{				
	UartTxDataBuf(COMM2, pStr, Len);

	return TRUE;		
}

/*******************************************************************************
* 名称 ：UBX_UartMsgCheckSum
* 描述 ：UBLOX protocol数据校验码
* 输入 ：pStr：字符串指针
		 Length：字符串长度
* 输出 ：无
* 返回 ：Check_Sum - 校验码
*******************************************************************************/
U16 UBX_UartMsgCheckSum(char *pStr, U16 Length)
{
    U8 CK_A, CK_B;
    U16 i;
    U16 Check_Sum;

    CK_A = 0;
    CK_B = 0;
    Check_Sum = 0;

    for (i = 0; i < Length; i++)
    {
        CK_A = CK_A + pStr[i];
        CK_B = CK_B + CK_A;
    }

    Check_Sum = CK_A;
    Check_Sum = (Check_Sum << 8) | CK_B;

    return Check_Sum;
}

/*******************************************************************************
* 名称  : UBX_UartResponseUbxMsg
* 描述  : UBX Protocol消息输出
* 输入  : pData：字符串指针
* 输出  : 无
* 返回  : FALSE：失败；TRUE：成功
*******************************************************************************/
U8 UBX_UartResponseUbxMsg(U8 *pData, U16 Len)
{
	U16 i, Index = 0;
	U16 Check = 0;
	char UartDebugBuff[100];
	
	if(Len > 90)
	{
		return FALSE;
	}
	
	Index = 0;
	for (i = 0; i < Len; i++)
	{
		UartDebugBuff[Index++] = pData[i];
	}
	
	if(Index > 2)
	{
		Check = UBX_UartMsgCheckSum((U8 *)&UartDebugBuff[2], Index - 2);
	}

	UartDebugBuff[Index++] = Check >> 8;    //lq Check_A
	UartDebugBuff[Index++] = Check & 0xFF;  //lq Check_B

	UBX_UartTxUbxMsg(UartDebugBuff, Index);
	
	return TRUE;
}

/*******************************************************************************
* 名称  : UBX_UBXPrtCommCtrlProcess
* 描述  : 使用UBX Protocol 与GPS通信控制
* 输入  : pUBXPrtCommStruct-UBXPrtCommStruct类型指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void UBX_UBXPrtCommCtrlProcess(UBXPrtCommStruct *pUBXPrtCommStruct)
{
    U8 TempBuff[30];

    switch(pUBXPrtCommStruct->CtrlState)
    {
		case UBX_PRT_COMM_CTRL_STAT_IDLE :
			{
			/*lq 有UBLOX消息要发送*/
				if (pUBXPrtCommStruct->SendRequireFlag == TRUE)
				{
					/*lq 提取一条消息 */            
					if (UBX_UBXPrtMsgSendFetch(pUBXPrtCommStruct) == TRUE)
					{
						/*lq 转入GPS唤醒状态 */
						pUBXPrtCommStruct->CtrlState = UBX_PRT_COMM_CTRL_STAT_WAKEUP;
					}
					else
					{
						/*lq 所有请求的消息已发送完成 */
						pUBXPrtCommStruct->SendRequireFlag = FALSE;            
					}
				}
				break;
			}
		case UBX_PRT_COMM_CTRL_STAT_WAKEUP:
			{
				/*lq 发送GPS唤醒序列 */
				TempBuff[0] = UBX_PRT_DUMMY_SEQUENCE;
				UBX_UartTxUbxMsg(TempBuff, 1);
        
				/*lq 唤醒等待定时 */        
				pUBXPrtCommStruct->TimePeriod = UBX_PRT_TIME_WAKEUP_WAIT;
				pUBXPrtCommStruct->TimeCnt = SetTimePeriodProcess(pUBXPrtCommStruct->TimePeriod);
    
				/*lq 转入消息发送状态 */
				pUBXPrtCommStruct->CtrlState = UBX_PRT_COMM_CTRL_STAT_SEND;
				break;    
			}
		case UBX_PRT_COMM_CTRL_STAT_SEND:
		{
			/*lq GPS接收机唤醒定时延时到*/
			if (pUBXPrtCommStruct->TimeCnt == GetLocalClockTickProcess())
			{
				/*lq 发送消息 */
				UBX_UartResponseUbxMsg(&(pUBXPrtCommStruct->MsgTx[1]), pUBXPrtCommStruct->MsgTx[0]);
            
				/*lq 单条消息发送次数加1 */
				pUBXPrtCommStruct->SendCnt++;
            
				/*lq 置位消息发送标志 */
				pUBXPrtCommStruct->RespFlag = FALSE;
				pUBXPrtCommStruct->AckFlag = FALSE;
				pUBXPrtCommStruct->SendFlag = TRUE;

				/*lq 响应等待定时 */
				pUBXPrtCommStruct->TimePeriod = UBX_PRT_TIME_RESPONSE_WAIT;
				pUBXPrtCommStruct->TimeCnt = SetTimePeriodProcess(pUBXPrtCommStruct->TimePeriod);

				/*lq 转入响应等待状态 */
				pUBXPrtCommStruct->CtrlState = UBX_PRT_COMM_CTRL_STAT_RESPONSE_WAIT;
			}   
			break;		
		}
		case UBX_PRT_COMM_CTRL_STAT_RESPONSE_WAIT:
		{
			/*lq GPS接收机消息响应超时或消息响应成功 */
			if (pUBXPrtCommStruct->TimeCnt == GetLocalClockTickProcess()
				|| pUBXPrtCommStruct->RespFlag == TRUE)
			{
				/*lq 消息确认成功 */
				if (pUBXPrtCommStruct->AckFlag == TRUE)
				{
					/*lq 复位消息确认标志 */
					pUBXPrtCommStruct->RespFlag = FALSE;
					pUBXPrtCommStruct->AckFlag = FALSE;                
					pUBXPrtCommStruct->SendFlag = FALSE;
                
					/*lq 单条消息发送次数清0 */
					pUBXPrtCommStruct->SendCnt = 0;                                  
                
					/*lq 转入空闲状态 */
					pUBXPrtCommStruct->CtrlState = UBX_PRT_COMM_CTRL_STAT_IDLE;    
				}
				else
				{
					/*lq 复位消息发送标志 */
					pUBXPrtCommStruct->RespFlag = FALSE;
					pUBXPrtCommStruct->AckFlag = FALSE;                
					pUBXPrtCommStruct->SendFlag = FALSE;            //lq 若因响应超时，则需在此复位发送标志

					debug_printf("UBX Msg %02X %02X repeat : %d\r\n", 
								pUBXPrtCommStruct->MsgClass, pUBXPrtCommStruct->MsgID, pUBXPrtCommStruct->SendCnt);

					/*lq 未达到发送次数阈值 */
					if (pUBXPrtCommStruct->SendCnt < UBX_PRT_SINGLE_MSG_SEND_CNT_THRES)
					{
						/*lq 转入唤醒状态 */
						pUBXPrtCommStruct->CtrlState = UBX_PRT_COMM_CTRL_STAT_WAKEUP;            
					}
					else
					{
						/*lq 单条消息发送次数清0 */
						pUBXPrtCommStruct->SendCnt = 0;                                  
	
						/*lq 转入空闲状态 */
						pUBXPrtCommStruct->CtrlState = UBX_PRT_COMM_CTRL_STAT_IDLE; //lq 超过尝试次数后便不再尝试    
					}            
				}
			} 
			break;			
		}
		default:
		{
			/*lq 转入空闲状态 */        
			pUBXPrtCommStruct->CtrlState = UBX_PRT_COMM_CTRL_STAT_IDLE;    
		}
	}		
}

/*******************************************************************************
* 名称  : UBX1_UBXPrtCommCtrlProcess
* 描述  : 使用UBX Protocol 与GPS通信控制
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void UBX1_UBXPrtCommCtrlProcess(void)
{
    UBX_UBXPrtCommCtrlProcess(&UBXPrtCommPara);
}

/*******************************************************************************
* 名称 ：UBX_UartRxProcess
* 描述 ：串口接收单字节（UBX protocol）
* 输入 ：pUartRxFIFOStruct：串口接收结构体指针
		 data：数据
* 输出 ：无
* 返回 ：无
*******************************************************************************/
static void UBX_MsgRxProcess(struct UartRxFIFOStruct *pUartRxFIFOStruct, U8 data)
{	
    U16 Len;

	switch(data)
	{
        case '\xB5':                                                            //lq uBX protocol报文起始字符
			pUartRxFIFOStruct->Rx0xb5Flg = TRUE;
            break;

        case '\x62':                                                            //lq 串口命令报文起始字符
            if (pUartRxFIFOStruct->Rx0xb5Flg == TRUE)
            {
                pUartRxFIFOStruct->Rx0xb5Flg = FALSE;                

                pUartRxFIFOStruct->RxBuffIndex = 1;	                            // 接受结构体列标,0号索引存总长度（除同步字符\xB5\x62）
    			if (((pUartRxFIFOStruct->RxFIFOInput+1) & (UART_RX_FIFO_DEPTH-1)) // 判断接受结构体满还是空
    					!= pUartRxFIFOStruct->RxFIFOOutput)                     //lq 接收缓冲区结构体还有空间
    			{
    				pUartRxFIFOStruct->RxEnFlg = TRUE;
    			}
    			else                                                            //lq 接收缓冲区结构体满
    			{
    				pUartRxFIFOStruct->RxEnFlg = FALSE;				
    			}			
            }
            else
            {
				pUartRxFIFOStruct->RxEnFlg = FALSE;            
            }
			break;
#if 0
		case '\r':		//0x0d
			pUartRxFIFOStruct->Rx0x0dFlg = TRUE;
			break;

		case '\n':		//0x0a					  //接受完毕
			if(pUartRxFIFOStruct->Rx0x0dFlg == TRUE)
			{
				pUartRxFIFOStruct->Rx0x0dFlg = FALSE;

				if (pUartRxFIFOStruct->RxEnFlg == TRUE)
				{
					pUartRxFIFOStruct->RxEnFlg = FALSE;
					
					pUartRxFIFOStruct->RxFIFO[pUartRxFIFOStruct->RxFIFOInput++][0]  //lq RxFIFO[ ][0]存放报文的长度 
								= pUartRxFIFOStruct->RxBuffIndex - 1;	//保存当前接受数据长度
					if (pUartRxFIFOStruct->RxFIFOInput >= UART_RX_FIFO_DEPTH)
					{
						pUartRxFIFOStruct->RxFIFOInput = 0;
					}
				}
			}
			else																//lq [2013-01-27]'\n'之前未出现'\r',报文格式无效
			{
				pUartRxFIFOStruct->RxEnFlg = FALSE;
			}
			break;
#endif
		default:
			if(pUartRxFIFOStruct->Rx0xb5Flg == TRUE)                            //lq 报文格式无效（'\xB5'后未紧跟出现'\x62'）    
			{
				pUartRxFIFOStruct->Rx0xb5Flg = FALSE;
				pUartRxFIFOStruct->RxEnFlg = FALSE;
			}
			
			if (pUartRxFIFOStruct->RxEnFlg == TRUE)
			{
				pUartRxFIFOStruct->RxFIFO[pUartRxFIFOStruct->RxFIFOInput][pUartRxFIFOStruct->RxBuffIndex++] = data;
   				if (pUartRxFIFOStruct->RxBuffIndex >= UART_RX_BUFF_SIZE)        //lq 串口报文过长，该报文无效（会被后面的报文覆盖）
				{
					pUartRxFIFOStruct->RxBuffIndex = 0;
					pUartRxFIFOStruct->RxEnFlg = FALSE;
				}
                else if (pUartRxFIFOStruct->RxBuffIndex >= 5)
                {
                    /*lq 获取payload长度 */
                    Len = (pUartRxFIFOStruct->RxFIFO[pUartRxFIFOStruct->RxFIFOInput][4] << 8) 
                          | (pUartRxFIFOStruct->RxFIFO[pUartRxFIFOStruct->RxFIFOInput][3]); //lq 协议中小端模式存长度
                    
                    /*lq 数据接收完毕 */
                    if (pUartRxFIFOStruct->RxBuffIndex >= (Len + 7))    //lq (7=1总长度+1Msgclass+1MsgID+2payloadLen+2checksum)                        
                    {
    					pUartRxFIFOStruct->RxEnFlg = FALSE;
    					
    					pUartRxFIFOStruct->RxFIFO[pUartRxFIFOStruct->RxFIFOInput++][0]  //lq RxFIFO[ ][0]存放报文的长度 
                            = pUartRxFIFOStruct->RxBuffIndex - 1;	    //保存当前接受数据长度（不包括同步字符\xB5\x62）
    					if (pUartRxFIFOStruct->RxFIFOInput >= UART_RX_FIFO_DEPTH)
    					{
    						pUartRxFIFOStruct->RxFIFOInput = 0;
    					}                    
                    }
                }
			}
			break;	
	}
}

/*******************************************************************************
* 名称 ：UBX_UartRxProcess
* 描述 ：串口接收单字节（UBX protocol）
* 输入 ：COMM：端口号
		 data：数据
* 输出 ：无
* 返回 ：无
*******************************************************************************/
void UBX_UartRxProcess(U8 COMM, U8 data)
{
    if (COMM == COMM2)
	{
        UBX_MsgRxProcess(&UBXPrtCommPara.Uart2RxFIFO2, data);
	}    
}

/*******************************************************************************
* 名称 ：UBX_UbxMsgProcess
* 描述 ：处理UbxMsg
* 输入 ：COMM：串口接收结构体指针
* 输出 ：无
* 返回 ：无
*******************************************************************************/
static S16 UBX_MsgDealProcess(UBXPrtCommStruct *pUBXPrtCommStruct, U8 *pDat, U16 uiLen)
{
    /*lq 该响应消息为ACK类消息*/
    if (pDat[0] == UBX_PRT_MSG_CLASS_ACK)
    {
        /*lq 发送过配置消息发送 */
        if (pUBXPrtCommStruct->SendFlag == TRUE)
        {
            /*lq 响应消息与发送消息对应 */
            if (pDat[4] == pUBXPrtCommStruct->MsgClass
                && pDat[5] == pUBXPrtCommStruct->MsgID)
            {
                /*lq 配置成功 */
                if (pDat[1] == UBX_PRT_MSG_ID_ACK_ACK)
                {
                    pUBXPrtCommStruct->AckFlag = TRUE;

                    debug_printf("UBX Msg %02X %02X : ACK\r\n", pDat[4], pDat[5]);
                }
                else
                {
                    pUBXPrtCommStruct->AckFlag = FALSE;

                    debug_printf("UBX Msg %02X %02X : NAK\r\n", pDat[4], pDat[5]);
                }
                
                pUBXPrtCommStruct->RespFlag = TRUE;
                pUBXPrtCommStruct->SendFlag = FALSE;                                  
            }    
        } 
    }              
}

/*******************************************************************************
* 名称 ：UBX_GetUbxMess
* 描述 ：获取UBX Protocol信息
* 输入 ：pUBXPrtCommStruct：UBXPrtCommStruct类型指针
* 输出 ：无
* 返回 ：无
*******************************************************************************/
static U16 UBX_GetReceivedUbxMess(UBXPrtCommStruct *pUBXPrtCommStruct)
{
	U16 CheckSum;
	U16 uiLength;
	U16 Len;
	struct UartRxFIFOStruct *pUartRxFIFO;
	
	uiLength = 0;
    pUartRxFIFO = &(pUBXPrtCommStruct->Uart2RxFIFO2);
	
    if (pUartRxFIFO->RxFIFOOutput != pUartRxFIFO->RxFIFOInput)
	{
		Len = pUartRxFIFO->RxFIFO[pUartRxFIFO->RxFIFOOutput][0];
		if (Len > 2)    //lq (2=2checksum)
		{
			/*lq 计算实际接收数据部分的校验码 */
            CheckSum = UBX_UartMsgCheckSum(&pUartRxFIFO->RxFIFO[pUartRxFIFO->RxFIFOOutput][1], Len - 2);
						
			if (((CheckSum >> 8) == pUartRxFIFO->RxFIFO[pUartRxFIFO->RxFIFOOutput][Len - 1])
				&& ((CheckSum & 0xFF) == pUartRxFIFO->RxFIFO[pUartRxFIFO->RxFIFOOutput][Len]))
			{
				uiLength = Len - 2; //lq (2=2checksum)
				
				/*lq UBX Protocol报文解析函数 */
                UBX_MsgDealProcess(pUBXPrtCommStruct, &pUartRxFIFO->RxFIFO[pUartRxFIFO->RxFIFOOutput][1], uiLength);	
			}
		}
        
        /*lq 出队 */				
		pUartRxFIFO->RxFIFOOutput++;
		if (pUartRxFIFO->RxFIFOOutput >= UART_RX_FIFO_DEPTH)
		{
			pUartRxFIFO->RxFIFOOutput = 0;
		}
	}
	
	return uiLength;
}

/*******************************************************************************
* 名称 ：UBX_GetCOMMMess
* 描述 ：获取串口UBX Protocol信息
* 输入 ：pUBXPrtCommStruct：UBXPrtCommStruct类型指针
* 输出 ：无
* 返回 ：无
*******************************************************************************/
U16 UBX_GetCOMMMess(U8 COMM)
{
	U16 uiLength;

    uiLength = 0;

    if (COMM == COMM2)
	{
        uiLength = UBX_GetReceivedUbxMess(&UBXPrtCommPara);
	}

    return uiLength;	

}

/*******************************************************************************
* 名称 ：UBX_GpsStartupConfig
* 描述 ：使用UBX protocol 消息配置GPS进入自定义正常工作模式
* 输入 ：pUBXPrtCommStruct：UBXPrtCommStruct类型指针
* 输出 ：无
* 返回 ：无
*******************************************************************************/
static void UBX_GpsStartupConfig(UBXPrtCommStruct *pUBXPrtCommStruct)
{
    /*lq 恢复默认配置并载入*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_CFG_DEFAULT, UBX_PRT_MSG_LEN_CFG_CFG);

    /*lq 配置GPS工作模式为continuous mode，防止配置过程中信息丢失*/
    //lq UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_RXM_CONTINUOUS, UBX_PRT_MSG_LEN_CFG_RXM);

    /*lq 配置GPS接收机只输出GxRMC、GxGGA、GxGSA、GxGBS报文 */
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R0_GXVTG, UBX_PRT_MSG_LEN_CFG_MSG);
    //lq UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R0_GXGGA, UBX_PRT_MSG_LEN_CFG_MSG);
    //lq UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R0_GXGSA, UBX_PRT_MSG_LEN_CFG_MSG);
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R0_GXGSV, UBX_PRT_MSG_LEN_CFG_MSG);
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R0_GXGLL, UBX_PRT_MSG_LEN_CFG_MSG);
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R1_GXGBS, UBX_PRT_MSG_LEN_CFG_MSG);

    /*lq 配置GPS cyclic operation的参数为正常工作参数*/
    //lq UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_PM2_NORM, UBX_PRT_MSG_LEN_CFG_PM2);

    /*lq 保存当前配置，防止配置GPS为PSM工作模式后，GPS一旦进入inactive状态，上述配置因来不及保存而丢失*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_CFG_SAVE, UBX_PRT_MSG_LEN_CFG_CFG);

    /*lq 配置GPS为PSM工作模式 */
    //lq UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_RXM_PSM, UBX_PRT_MSG_LEN_CFG_RXM);

    /*lq 保存当前配置*/
    //lq UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_CFG_SAVE, UBX_PRT_MSG_LEN_CFG_CFG);

    /*lq 设置UBX消息的发送请求 */
    pUBXPrtCommStruct->SendRequireFlag = TRUE;
}

/*******************************************************************************
* 名称 ：UBX1_GpsStartupConfig
* 描述 ：使用UBX protocol 消息配置GPS进入自定义正常工作模式
* 输入 ：pUBXPrtCommStruct：UBXPrtCommStruct类型指针
* 输出 ：无
* 返回 ：无
*******************************************************************************/
void UBX1_GpsStartupConfig(void)
{
    UBX_GpsStartupConfig(&UBXPrtCommPara);
}

/*******************************************************************************
* 名称 ：UBX_GpsIdleConfig
* 描述 ：使用UBX protocol 消息配置GPS进入自定义省电工作模式
* 输入 ：pUBXPrtCommStruct：UBXPrtCommStruct类型指针
* 输出 ：无
* 返回 ：无
*******************************************************************************/
static void UBX_GpsIdleConfig(UBXPrtCommStruct *pUBXPrtCommStruct)
{
    /*lq 在进行GPS省电配置之前，将秒脉冲置为无效*/
    ResetGpsPulseEn();

    /*lq 配置GPS工作模式为continuous mode，防止配置过程中信息丢失*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_RXM_CONTINUOUS, UBX_PRT_MSG_LEN_CFG_RXM);

    /*lq 配置PPS失能，先关PPS，后关GxRMC */
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_TP5_INACTIVE, UBX_PRT_MSG_LEN_CFG_TP5);

    /*lq 配置NMEA-GxRMC输出失能*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R0_GXRMC, UBX_PRT_MSG_LEN_CFG_MSG);
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R0_GXGGA, UBX_PRT_MSG_LEN_CFG_MSG);
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R0_GXGSA, UBX_PRT_MSG_LEN_CFG_MSG);
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R0_GXGBS, UBX_PRT_MSG_LEN_CFG_MSG);

    /*lq 配置GPS cyclic operation的参数为省电模式参数*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_PM2_SAVE, UBX_PRT_MSG_LEN_CFG_PM2);

    /*lq 保存当前配置，防止配置GPS为PSM工作模式后，GPS一旦进入inactive状态，上述配置因来不及保存而丢失*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_CFG_SAVE, UBX_PRT_MSG_LEN_CFG_CFG);

    /*lq 配置GPS为PSM工作模式 */
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_RXM_PSM, UBX_PRT_MSG_LEN_CFG_RXM);

    /*lq 保存当前配置*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_CFG_SAVE, UBX_PRT_MSG_LEN_CFG_CFG);

    /*lq 设置UBX消息的发送请求 */
    pUBXPrtCommStruct->SendRequireFlag = TRUE;

}

/*******************************************************************************
* 名称 ：UBX1_GpsIdleConfig
* 描述 ：使用UBX protocol 消息配置GPS进入自定义省电工作模式
* 输入 ：pUBXPrtCommStruct：UBXPrtCommStruct类型指针
* 输出 ：无
* 返回 ：无
*******************************************************************************/
void UBX1_GpsIdleConfig(void)
{
    UBX_GpsIdleConfig(&UBXPrtCommPara);
}

/*******************************************************************************
* 名称 ：UBX_GpsNormalConfig
* 描述 ：使用UBX protocol 消息配置GPS进入自定义正常工作模式
* 输入 ：pUBXPrtCommStruct：UBXPrtCommStruct类型指针
* 输出 ：无
* 返回 ：无
*******************************************************************************/
static void UBX_GpsNormalConfig(UBXPrtCommStruct *pUBXPrtCommStruct)
{
    /*lq 配置GPS工作模式为continuous mode，防止配置过程中信息丢失*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_RXM_CONTINUOUS, UBX_PRT_MSG_LEN_CFG_RXM);

    /*lq 配置GPS cyclic operation的参数为正常工作参数*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_PM2_NORM, UBX_PRT_MSG_LEN_CFG_PM2);

    /*lq 配置NMEA-GxRMC输出使能，先开GxRMC，后开PPS*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R1_GXRMC, UBX_PRT_MSG_LEN_CFG_MSG);
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R1_GXGGA, UBX_PRT_MSG_LEN_CFG_MSG);
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R1_GXGSA, UBX_PRT_MSG_LEN_CFG_MSG);    
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R1_GXGBS, UBX_PRT_MSG_LEN_CFG_MSG);
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R1_GXVTG, UBX_PRT_MSG_LEN_CFG_MSG);
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R1_GXGSV, UBX_PRT_MSG_LEN_CFG_MSG);
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_MSG_R1_GXGLL, UBX_PRT_MSG_LEN_CFG_MSG);

    /*lq 配置PPS使能 */
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_TP5_ACTIVE, UBX_PRT_MSG_LEN_CFG_TP5);
#if 0
    /*lq 保存当前配置，防止配置GPS为PSM工作模式后，GPS一旦进入inactive状态，上述配置因来不及保存而丢失*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_CFG_SAVE, UBX_PRT_MSG_LEN_CFG_CFG);

    /*lq 配置GPS为PSM工作模式 */
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_RXM_PSM, UBX_PRT_MSG_LEN_CFG_RXM);
#endif
    /*lq 保存当前配置*/
    UBX_UBXPrtMsgSendBuff(pUBXPrtCommStruct, UBX_PRT_MSG_CFG_CFG_SAVE, UBX_PRT_MSG_LEN_CFG_CFG);

    /*lq 设置UBX消息的发送请求 */
    pUBXPrtCommStruct->SendRequireFlag = TRUE;

}

/*******************************************************************************
* 名称 ：UBX1_GpsNormalConfig
* 描述 ：使用UBX protocol 消息配置GPS进入自定义正常工作模式
* 输入 ：pUBXPrtCommStruct：UBXPrtCommStruct类型指针
* 输出 ：无
* 返回 ：无
*******************************************************************************/
void UBX1_GpsNormalConfig(void)
{
    UBX_GpsNormalConfig(&UBXPrtCommPara);
}

/*******************************************************************************
* 名称 ：UBX_GetSendRequireFlag
* 描述 ：查询发送请求标志
* 输入 ：pUBXPrtCommStruct：UBXPrtCommStruct类型指针
* 输出 ：无
* 返回 ：无
*******************************************************************************/
static U8 UBX_GetSendRequireFlag(UBXPrtCommStruct *pUBXPrtCommStruct)
{
    return pUBXPrtCommStruct->SendRequireFlag;    
}

/*******************************************************************************
* 名称 ：UBX1_GetSendRequireFlag
* 描述 ：查询发送请求标志
* 输入 ：无
* 输出 ：无
* 返回 ：无
*******************************************************************************/
U8 UBX1_GetSendRequireFlag(void)
{
    return UBX_GetSendRequireFlag(&UBXPrtCommPara);    
}

/*******************************************************************************
* 名称 ：UBX_GetGpsCommSleepSta
* 描述 ：查询GPS通信模块休眠条件
* 输入 ：pUBXPrtCommStruct：UBXPrtCommStruct类型指针
* 输出 ：无
* 返回 ：无
*******************************************************************************/
static U8 UBX_GetGpsCommSleepSta(UBXPrtCommStruct *pUBXPrtCommStruct)
{
    if (pUBXPrtCommStruct->SendRequireFlag == FALSE)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称 ：UBX1_GetGpsCommSleepSta
* 描述 ：查询GPS通信模块休眠条件
* 输入 ：无
* 输出 ：无
* 返回 ：无
*******************************************************************************/
U8 UBX1_GetGpsCommSleepSta(void)
{
    return UBX_GetGpsCommSleepSta(&UBXPrtCommPara);    
}

/*******************************************************************************
* 名称  : UBX_ModuleInit
* 描述  : 模块初始化
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void UBX_ModuleInit(void) 
{
    UBX_UBXPrtCommStructInit(&UBXPrtCommPara);
    UBX_GpsNormalConfig(&UBXPrtCommPara);
}


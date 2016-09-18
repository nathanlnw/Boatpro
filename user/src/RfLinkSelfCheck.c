/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: RfLinkSelfCheck.c
  Author: lq       Version : V1.0.0        Date: 2015.07.22
  Description:     射频链路自检模块
  Version:         V1.0.0
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "Include.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* 名称  : RFL_RfLinkSelfCheckStructInit
* 描述  : 初始化结构体
* 输入  : pRfLinkSelfCheckStruct - RfLinkSelfCheckStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void RFL_RfLinkSelfCheckStructInit(RfLinkSelfCheckStruct *pRfLinkSelfCheckStruct)
{
	pRfLinkSelfCheckStruct->TimeCnt = 0;
	pRfLinkSelfCheckStruct->TimePeriod = RFL_SLFCHK_TIME_PERIOD;	
    pRfLinkSelfCheckStruct->Timeout = RFL_SLFCHK_TIME_TIMEOUT_TX;
    pRfLinkSelfCheckStruct->TimeoutCnt = 0;

    pRfLinkSelfCheckStruct->CtrlState = RFL_SLFCHK_CTRL_STATE_IDLE;
    pRfLinkSelfCheckStruct->StartFlg = TRUE;
    pRfLinkSelfCheckStruct->CompleteFlg = FALSE;

    pRfLinkSelfCheckStruct->RfLinkOkFlg = FALSE;           
}

/*******************************************************************************
* 名称  : RFL_RfLinkSelfCheckReset
* 描述  : 结构体复位
* 输入  : pRfLinkSelfCheckStruct - RfLinkSelfCheckStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void RFL_RfLinkSelfCheckReset(RfLinkSelfCheckStruct *pRfLinkSelfCheckStruct)
{
    pRfLinkSelfCheckStruct->StartFlg = FALSE;
    pRfLinkSelfCheckStruct->CompleteFlg = FALSE;
    pRfLinkSelfCheckStruct->RfLinkOkFlg = FALSE;           
}

/*******************************************************************************
* 名称  : RFL_SelfCheck
* 描述  : 自检控制
* 输入  : pRfLinkSelfCheckStruct - RfLinkSelfCheckStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void RFL_SelfCheck(RfLinkSelfCheckStruct *pRfLinkSelfCheckStruct)
{
    MsgCellStruct msgCell;

    switch (pRfLinkSelfCheckStruct->CtrlState)
    {
        case RFL_SLFCHK_CTRL_STATE_IDLE:
            
            if (pRfLinkSelfCheckStruct->StartFlg == TRUE)
            {
                RFL_RfLinkSelfCheckReset(pRfLinkSelfCheckStruct);

                /*lq SI446X配置 */
                SI446X_ModuleInit();
        		
                pRfLinkSelfCheckStruct->Timeout = RFL_SLFCHK_TIME_TIMEOUT_TX;
                pRfLinkSelfCheckStruct->TimeoutCnt = 0;
                pRfLinkSelfCheckStruct->TimeCnt = SetTimePeriodProcess(pRfLinkSelfCheckStruct->TimePeriod);

                pRfLinkSelfCheckStruct->CtrlState = RFL_SLFCHK_CTRL_STATE_TX;            
            }

            break; 
        case RFL_SLFCHK_CTRL_STATE_TX:

            /*lq 发送超时判断 */
            if (pRfLinkSelfCheckStruct->TimeCnt == GetLocalClockTickProcess())
        	{
        		pRfLinkSelfCheckStruct->TimeCnt = SetTimePeriodProcess(pRfLinkSelfCheckStruct->TimePeriod);
                pRfLinkSelfCheckStruct->TimeoutCnt++;                
                if (pRfLinkSelfCheckStruct->TimeoutCnt >= pRfLinkSelfCheckStruct->Timeout)
                {
                    pRfLinkSelfCheckStruct->TimeoutCnt = 0;

                	/*lq 使用信道B发射一条6号测试消息 */
//                    SendMsg6Test(ENUM_AIS_CHANNEL_B, SLOT_FORCE_TX_VALUE);
					SendMsg18Test(ENUM_AIS_CHANNEL_B, SLOT_FORCE_TX_VALUE);
            		
                    pRfLinkSelfCheckStruct->Timeout = RFL_SLFCHK_TIME_TIMEOUT_RX;
                    pRfLinkSelfCheckStruct->TimeoutCnt = 0;
                    pRfLinkSelfCheckStruct->TimeCnt = SetTimePeriodProcess(pRfLinkSelfCheckStruct->TimePeriod);
                    pRfLinkSelfCheckStruct->CtrlState = RFL_SLFCHK_CTRL_STATE_RX;
                }                        
            }    

            break;
        case RFL_SLFCHK_CTRL_STATE_RX:

            /*lq 消息接收判断 */
            if (MSGMAN_RxMsgListGetElem(&msgCell) == TRUE)
            {
//                if (AISMSG_ParseMsg(&msgCell.Data[0]) == AISMsgType6)
//                {
//                    if (AISMsg_6.DestinationID == AisStaticPara.MMSI)
//                    {
//                        if ((AISMsg_6.AI_DAC == RFL_SLFCHK_MSG6_AI_DAC)
//                            && (AISMsg_6.AI_FI == RFL_SLFCHK_MSG6_AI_FI)
//                            && (AISMsg_6.ApplicationData[0] == RFL_SLFCHK_MSG6_AD)
//                            && (AISMsg_6.AppDataBitLen == RFL_SLFCHK_MSG6_AD_BIT_LEN))
//                        {
				if (AISMSG_ParseMsg(&msgCell.Data[0]) == AISMsgType18)
	                {
	                    if (AISMsg_18.UserID == AIS_INFO_MMSI_DEFAULT)
	                    {
	                        /*lq RL Link OK */
	                        pRfLinkSelfCheckStruct->RfLinkOkFlg = TRUE;
	                        pRfLinkSelfCheckStruct->CompleteFlg = TRUE;
	                        
							UartResponseMsg("B is OK!");
	                        pRfLinkSelfCheckStruct->CtrlState = RFL_SLFCHK_CTRL_STATE_IDLE;                                                   
//                        }
	                    }
	                }
            }
            
        	/*lq 接收超时判断 */
            if (pRfLinkSelfCheckStruct->TimeCnt == GetLocalClockTickProcess())
        	{
        		pRfLinkSelfCheckStruct->TimeCnt = SetTimePeriodProcess(pRfLinkSelfCheckStruct->TimePeriod);
                pRfLinkSelfCheckStruct->TimeoutCnt++;                
                if (pRfLinkSelfCheckStruct->TimeoutCnt >= pRfLinkSelfCheckStruct->Timeout)
                {
                    pRfLinkSelfCheckStruct->TimeoutCnt = 0;

                    pRfLinkSelfCheckStruct->CompleteFlg = TRUE;
                    
					UartResponseMsg("B is not OK!");
                    pRfLinkSelfCheckStruct->CtrlState = RFL_SLFCHK_CTRL_STATE_IDLE;                                                   
                }                        
            }    
            
            break;
        default :
            
            RFL_RfLinkSelfCheckStructInit(pRfLinkSelfCheckStruct);            
            break;
    }   
}

/*==============================================================================
------------------------------------End of File---------------------------------
==============================================================================*/

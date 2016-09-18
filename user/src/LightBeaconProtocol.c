/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: LightBeaconProtocol.c
  Author: lq       Version : V1.00        Date: 2014.01.21
  Description:     Protocol between AIS and light beacon
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "def.h"
#include "LightBeaconProtocol.h"
#include "gpsctrl.h"
#include "initial.h"
#include "parastorage.h"
#include "config.h"

/*lq 模块实例定义*/
static LBPrtCommStruct LBPrtCommPara;

/*******************************************************************************
* 名称  : LB_LBPrtCommStructInit
* 描述  : LBPrtCommStruct结构体初始化
* 输入  : pLBPrtCommStruct-LBPrtCommStruct类型指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void LB_LBPrtCommStructInit(LBPrtCommStruct *pLBPrtCommStruct)
{
    U16 i;

    pLBPrtCommStruct->TimePeriod = LB_PRT_TIME_REPETITION_INTERVAL;
    pLBPrtCommStruct->TimeCnt = 0;

    pLBPrtCommStruct->InterrogationFlag = FALSE;
#if 0
    for (i = 0; i < LB_PRT_SEND_FIFO_DEPTH; i++)
    {
        pLBPrtCommStruct->SendFIFO[i][0] = 0;
        pLBPrtCommStruct->SendFIFO[i][1] = 0;
    }
    pLBPrtCommStruct->SendFIFOIn = 0;
    pLBPrtCommStruct->SendFIFOOut = 0;
#endif
    for (i = 0; i < LB_PRT_REPET_BUFF_LEN; i++)
    {
        pLBPrtCommStruct->RepetBuff[i] = 0;
    }

    pLBPrtCommStruct->LB_Stat = 'V';
    pLBPrtCommStruct->LB_TimeY = 0;
	pLBPrtCommStruct->LB_TimeMon = 0;
	pLBPrtCommStruct->LB_TimeD = 0;
	pLBPrtCommStruct->LB_TimeH = 24;
	pLBPrtCommStruct->LB_TimeM = 63;
	pLBPrtCommStruct->LB_TimeS = 63;
	pLBPrtCommStruct->LB_Latitude = 0x3412140;                  //lq 91°N
	pLBPrtCommStruct->LB_Longitud = 0x6791AC0;                  //lq 181°E

    //lq UartRxFIFOStructInit(&(pUBXPrtCommStruct->Uart2RxFIFO2));
}

/*******************************************************************************
* 名称  : LB1_LBPrtCommInstanceInit
* 描述  : LBPrtCommStruct结构体初始化
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void LB1_LBPrtCommInstanceInit(void)
{
    LB_LBPrtCommStructInit(&LBPrtCommPara);
}

/*******************************************************************************
* 名称  : LB_LBPrtCommStructReset
* 描述  : LBPrtCommStruct结构体复位
* 输入  : pLBPrtCommStruct-LBPrtCommStruct类型指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void LB_LBPrtCommStructReset(LBPrtCommStruct *pLBPrtCommStruct)
{
    U16 i;

    pLBPrtCommStruct->TimePeriod = LB_PRT_TIME_REPETITION_INTERVAL;
    pLBPrtCommStruct->TimeCnt = 0;

    pLBPrtCommStruct->InterrogationFlag = FALSE;

    for (i = 0; i < LB_PRT_REPET_BUFF_LEN; i++)
    {
        pLBPrtCommStruct->RepetBuff[i] = 0;
    }

    pLBPrtCommStruct->LB_Stat = 'V';        
    pLBPrtCommStruct->LB_TimeY = 0;
	pLBPrtCommStruct->LB_TimeMon = 0;
	pLBPrtCommStruct->LB_TimeD = 0;
	pLBPrtCommStruct->LB_TimeH = 24;
	pLBPrtCommStruct->LB_TimeM = 63;
	pLBPrtCommStruct->LB_TimeS = 63;
	pLBPrtCommStruct->LB_Latitude = 0x3412140;                  //lq 91°N
	pLBPrtCommStruct->LB_Longitud = 0x6791AC0;                  //lq 181°E
}

/*******************************************************************************
* 名称  : LB_SetInterrogationFlag
* 描述  : 置位InterrogationFlag
* 输入  : pLBPrtCommStruct-LBPrtCommStruct类型指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void LB_SetInterrogationFlag(LBPrtCommStruct *pLBPrtCommStruct)
{
    pLBPrtCommStruct->InterrogationFlag = TRUE;
}

/*******************************************************************************
* 名称  : LB_SetInterrogationFlag
* 描述  : 置位InterrogationFlag
* 输入  : pLBPrtCommStruct-LBPrtCommStruct类型指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void LB1_SetInterrogationFlag(void)
{
    LB_SetInterrogationFlag(&LBPrtCommPara);
}

/*******************************************************************************
* 名称  : LB_ResetInterrogationFlag
* 描述  : 复位InterrogationFlag
* 输入  : pLBPrtCommStruct-LBPrtCommStruct类型指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void LB_ResetInterrogationFlag(LBPrtCommStruct *pLBPrtCommStruct)
{
    pLBPrtCommStruct->InterrogationFlag = FALSE;
}

/*******************************************************************************
* 名称  : LB1_ResetInterrogationFlag
* 描述  : 置位InterrogationFlag
* 输入  : pLBPrtCommStruct-LBPrtCommStruct类型指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void LB1_ResetInterrogationFlag(void)
{
    LB_ResetInterrogationFlag(&LBPrtCommPara);
}

/*******************************************************************************
* 名称  : LB1_LBPrtCommStructReset
* 描述  : LBPrtCommStruct结构体复位
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void LB1_LBPrtCommStructReset(void)
{
    LB_LBPrtCommStructReset(&LBPrtCommPara);
}

#if 0
/*******************************************************************************
* 名称  : LB_SendFIFOSearch
* 描述  : 在发送队列查找指定的命令类型
* 输入  : pLBPrtCommStruct-LBPrtCommStruct类型指针
          CmdType-查找的命令类型号		  
* 输出  : 无
* 返回  : 查找的命令在队列中的索引号，-1:未找到
*******************************************************************************/
static S16 LB_SendFIFOSearch(LBPrtCommStruct *pLBPrtCommStruct, U8 CmdType)
{
    U16 Index;

    /*lq 队列为空*/
    if (pLBPrtCommStruct->SendFIFOIn == pLBPrtCommStruct->SendFIFOOut)
    {
        return -1;    
    }
    else
    {
        Index = pLBPrtCommStruct->SendFIFOOut;
        while(Index != pLBPrtCommStruct->SendFIFOIn)
        {
            if (pLBPrtCommStruct->SendFIFO[Index][0] == CmdType)
            {
                return Index;   //lq 返回索引号
            }
            else
            {
                Index++;
                Index %= LB_PRT_SEND_FIFO_DEPTH;
            }
        }

        return -1;
    }           
}

/*******************************************************************************
* 名称  : LB_SendFIFOAdd
* 描述  : 向发送队列中添加消息
* 输入  : pLBPrtCommStruct-LBPrtCommStruct类型指针
          CmdType-入队命令类型号		  
          RepetCnt-该命令对应的重传次数
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static U8 LB_SendFIFOAdd(LBPrtCommStruct *pLBPrtCommStruct, U8 CmdType, U8 RepetCnt)
{
    S16 Index;

    /*lq 先查找要插入的命令是否在队列中*/
    Index = LB_SendFIFOSearch(pLBPrtCommStruct, CmdType);
        
    if (Index < 0)   //lq 不在队列中
    {
        /*lq 队列已满 */
        if ((pLBPrtCommStruct->SendFIFOIn + 1) % LB_PRT_SEND_FIFO_DEPTH
             == pLBPrtCommStruct->SendFIFOOut)
        {
            return FALSE;    
        }
        else
        {
            /*lq 入队 */
            pLBPrtCommStruct->SendFIFO[pLBPrtCommStruct->SendFIFOIn][0] = CmdType;
            pLBPrtCommStruct->SendFIFO[pLBPrtCommStruct->SendFIFOIn][1] = RepetCnt;

            pLBPrtCommStruct->SendFIFOIn++;
            pLBPrtCommStruct->SendFIFOIn %= LB_PRT_SEND_FIFO_DEPTH; 
        }            
    }
    else
    {
        /*lq 用新的重传次数替换原来的剩余次数 */
        pLBPrtCommStruct->SendFIFO[Index][1] = RepetCnt;           
    }
}
#endif

/*******************************************************************************
* 名称  : LB_LBPrtCommCtrlProcess
* 描述  : 使用Light beacon Protocol 与light beacon通信控制
* 输入  : pLBPrtCommStruct-LBPrtCommStruct类型指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void LB_LBPrtCommCtrlProcess(LBPrtCommStruct *pLBPrtCommStruct)
{
    U8  Index;
    U8  Len;
	U8 	ucTempBuf[40];
    S32 slTemp;
    U8  ucTemp;
	char chRetBuf[100];
    U16 i;

    /*lq 处于通信模式 */
    if (pLBPrtCommStruct->InterrogationFlag == TRUE)
    {
        /*lq 发送周期定时到 */
        if (pLBPrtCommStruct->TimeCnt == GetLocalClockTickProcess())
        {
            pLBPrtCommStruct->TimeCnt = SetTimePeriodProcess(pLBPrtCommStruct->TimePeriod);

            /*lq 需要重传*/
            if (pLBPrtCommStruct->RepetBuff[0] > 0)
            {
                pLBPrtCommStruct->RepetBuff[0]--;                           //lq 重传次数减1
                
                /*lq 重传命令数 */
                Len = pLBPrtCommStruct->RepetBuff[1];

                if (Len > 0)
                {
                    sprintf(chRetBuf, "$AIRDM");
                    for (Index = 2; Index < Len + 2; Index++)
                    {
                        switch (pLBPrtCommStruct->RepetBuff[Index])
                        {
                            case LB_PRT_RDM_DATE:
                            {                				
                                strcat(chRetBuf, ",");                
                                sprintf(ucTempBuf, "%02d", GpsPara.GPS_TimeD);
                                strcat(chRetBuf, (char *)ucTempBuf);
                                sprintf(ucTempBuf, "%02d", GpsPara.GPS_TimeMon);
                                strcat(chRetBuf, (char *)ucTempBuf);                                
                                sprintf(ucTempBuf, "%02d", GpsPara.GPS_TimeY);
                                strcat(chRetBuf, (char *)ucTempBuf);                                                    
                                break;
                            }
                            
                            case LB_PRT_RDM_UTC_TIME:
                			{	
                                strcat(chRetBuf, ",");                
                                sprintf(ucTempBuf, "%02d", GpsPara.GPS_TimeH);
                                strcat(chRetBuf, (char *)ucTempBuf);
                                sprintf(ucTempBuf, "%02d", GpsPara.GPS_TimeM);
                                strcat(chRetBuf, (char *)ucTempBuf);                                
                                sprintf(ucTempBuf, "%02d", GpsPara.GPS_TimeS);
                                strcat(chRetBuf, (char *)ucTempBuf);
                                strcat(chRetBuf, ".00");                    //lq 秒的小数部分为0               
                                //lq break;                                 //lq 若命令类型为UBX_PRT_RDM_UTC_TIME，则定位经纬度一起返回
                            }
                            
                            case LB_PRT_RDM_ANCHOR:
                			{	
                                strcat(chRetBuf, ",");
                                if (GpsPara.GPS_Stat == 'A')
                                {
                                    strcat(chRetBuf, "A");                                
                                }
                                else
                                {
                                    strcat(chRetBuf, "V");                                                                
                                } 
                                
                                slTemp = GpsPara.GPS_Latitude / 100.0;      //lq 精度变为0.01分
                                if (slTemp < 0)
                                {
                                    if (ToLatFormat(-slTemp, ucTempBuf, 2))
                                    {
                                        strcat(chRetBuf, ",");                
                        				strcat(chRetBuf, (char *)ucTempBuf);
                                        strcat(chRetBuf, ",S");                
                                    }
                                    else
                                    {
                                        strcat(chRetBuf, ",,");             //lq 数据无效，返回空字段               
                                    }
                                }
                                else
                                {
                                    if (ToLatFormat(slTemp, ucTempBuf, 2))
                                    {
                                        strcat(chRetBuf, ",");                
                        				strcat(chRetBuf, (char *)ucTempBuf);
                                        strcat(chRetBuf, ",N");                
                                    } 
                                    else
                                    {
                                        strcat(chRetBuf, ",,");             //lq 数据无效，返回空字段               
                                    }                                   
                                }

                                slTemp = GpsPara.GPS_Longitud / 100.0;      //lq 精度变为0.01分
                                if (slTemp < 0)
                                {
                                    if (ToLonFormat(-slTemp, ucTempBuf, 2))
                                    {
                                        strcat(chRetBuf, ",");                
                        				strcat(chRetBuf, (char *)ucTempBuf);
                                        strcat(chRetBuf, ",W");                
                                    }
                                    else
                                    {
                                        strcat(chRetBuf, ",,");             //lq 数据无效，返回空字段               
                                    }
                                }
                                else
                                {
                                    if (ToLonFormat(slTemp, ucTempBuf, 2))
                                    {
                                        strcat(chRetBuf, ",");                
                        				strcat(chRetBuf, (char *)ucTempBuf);
                                        strcat(chRetBuf, ",E");                
                                    } 
                                    else
                                    {
                                        strcat(chRetBuf, ",,");             //lq 数据无效，返回空字段               
                                    }                                   
                                }                                                                  
                                break;
                            }
                            
                            case LB_PRT_RDM_MMSI:
                			{	
                                strcat(chRetBuf, ",");                
                                sprintf(ucTempBuf, "%09d", siStaticPara.MMSI);
                                strcat(chRetBuf, (char *)ucTempBuf);                                                    
                                break;
                            }
                            
                            case LB_PRT_RDM_LIGHT_BEACON_TYPE:
                			{	
                                strcat(chRetBuf, ",");                
                                sprintf(ucTempBuf, "%0*d", PARA_ATON_TYPE_MAXLEN, siStaticPara.MMSI);
                                strcat(chRetBuf, (char *)ucTempBuf);                                                    
                                break;
                            }
                            
                            case LB_PRT_RDM_NAME:
                			{	
                                strcat(chRetBuf, ",");                
                                strcat(chRetBuf, siStaticPara.NameofAtoN);                                                    
                                break;
                            }
                            
                            case LB_PRT_RDM_AIS_STATUE:
                			{	
#if 0
                                strcat(chRetBuf, ",");
                                for (i = 0; i < 8; i++)
                                {
                                    ucTemp = siStaticPara.AtoNStatus >> (7 - i);    //lq 21号消息中的AtoN状态
                                    ucTemp &= 0x1;                          //lq 移位后的最低位 
                                    if (ucTemp == 0)
                                    {
                                        strcat(chRetBuf, "0");                                    
                                    }
                                    else
                                    {
                                        strcat(chRetBuf, "1");                                                                        
                                    }                                                                         
                                }
#else
                                strcat(chRetBuf, ",");
                                
                                /* lq RF antenna state */
                                if (RF1_GetRFAntState() == TRUE)                    
                                {
                                    strcat(chRetBuf, "0");                      //lq indicate OK                                    
                                }
                                else
                                {
                                    strcat(chRetBuf, "1");                      //lq indicate problem                                                  
                                }

                                /*lq GPS attenna state，无源天线认为始终OK*/
                                strcat(chRetBuf, "0");                          //lq indicate OK                                    

                                /*lq GPS receiver state */
                                if ((GetGpsCtrlState() != ATON_GPS_CTRL_STA_IDLE)
                                    && (GetGpsMsgGenFlag() == FALSE))
                                {
                                    strcat(chRetBuf, "1");                      //lq indicate problem
                                }
                                else
                                {
                                    strcat(chRetBuf, "0");                      //lq indicate OK
                                }
                                
                                /*lq GPS fix state*/                                    
                                if (IsGpsFix() == TRUE)
                                {
                                    strcat(chRetBuf, "0");                      //lq indicate OK                                    
                                }
                                else
                                {
                                    strcat(chRetBuf, "1");                      //lq indicate problem                                                  
                                }

                                /*lq coordinate state*/
                                if (IsCoordinateOK() == TRUE)
                                {
                                    strcat(chRetBuf, "0");                      //lq indicate OK                                    
                                }
                                else
                                {
                                    strcat(chRetBuf, "1");                      //lq indicate problem                                                  
                                }

                                /*lq PDOP */
                                if (IsPdopOK() == TRUE)
                                {
                                    strcat(chRetBuf, "0");                      //lq indicate OK                                    
                                }
                                else
                                {
                                    strcat(chRetBuf, "1");                      //lq indicate too high                                                  
                                }

                                /*lq PPS */
                                if (GetGpsPPSGenFlag() == TRUE)
                                {
                                    strcat(chRetBuf, "0");                      //lq indicate OK                                    
                                }
                                else
                                {
                                    strcat(chRetBuf, "1");                      //lq indicate too high                                                  
                                }
                                
                                /*lq Spare */
                                strcat(chRetBuf, "0");                      //lq indicate OK                                                                                                    
#endif                                               
                                break;
                            }
                            
                            case LB_PRT_RDM_USART_SETTING:
                			{	
                                strcat(chRetBuf, ",");                
                                sprintf(ucTempBuf, "%d", Uart1Config.Baud);
                                strcat(chRetBuf, (char *)ucTempBuf);
                                strcat(chRetBuf, ",");                                                                                                    
                                sprintf(ucTempBuf, "%d", Uart1Config.Data);
                                strcat(chRetBuf, (char *)ucTempBuf);
                                strcat(chRetBuf, ",");                                                                                                    
                                sprintf(ucTempBuf, "%d", Uart1Config.Parity);
                                strcat(chRetBuf, (char *)ucTempBuf);
                                strcat(chRetBuf, ",");                                                                                                    
                                sprintf(ucTempBuf, "%d", Uart1Config.Stop);
                                strcat(chRetBuf, (char *)ucTempBuf);                                                    
                                break;
                            }
                            
                            default :
                                break;
                        }   
                    } 
                    
				    /*lq 发送报文 */
                    UartResponseMsg(chRetBuf);		                                   
                }      
            }
        }
    }    
}

/*******************************************************************************
* 名称  : LB1_LBPrtCommCtrlProcess
* 描述  : 使用Light beacon Protocol 与light beacon通信控制
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void LB1_LBPrtCommCtrlProcess(void)
{
    LB_LBPrtCommCtrlProcess(&LBPrtCommPara);
}

/*******************************************************************************
* 名称 ：LB_MsgDealProcess
* 描述 ：处理Light Beacon Msg
* 输入 ：无
* 输出 ：无
* 返回 ：TRUE-消息报文是Light Beacon Msg，FALSE-消息报文不是Light Beacon Msg
*******************************************************************************/
static U8 LB_MsgDealProcess(LBPrtCommStruct *pLBPrtCommStruct, U8 *pDat, U16 uiLen)
{
	char chTemp[40];
	U16 i;
    U32 ulTemp;
    S32 slTemp;
    U8  ucTemp1, ucTemp2, ucTemp3;
	U8  ucTemp;
	U8 	ucCheck;
	U8 	ucLen;

    /*lq AIRDM报文*/
    if (strncmp((char *)pDat, "AIRDM", 5) == FALSE)
    {
		/*lq 获取第1字段内容，repetition time*/
		ucCheck = GetSegment((char *)pDat, chTemp, 1, uiLen, 30);        
        if (ucCheck == FALSE)
        {
            /*lq 为空，表示终止重传 */            
            pLBPrtCommStruct->RepetBuff[0] = 0;                
        }
        else
        {
            ulTemp = atol(chTemp);
            if (ulTemp > 65534)
            {
                ulTemp = 65534;   
            }
            
            /*lq 保存重传次数 */
            pLBPrtCommStruct->RepetBuff[0] = ulTemp + 1;                
            
            /*lq 提取命令类型号 */
            for (i = 2; i < LB_PRT_REPET_BUFF_LEN; i++)
            {
		        /*lq 依次获取各个命令类型号 */
                ucCheck = GetSegment((char *)pDat, chTemp, i, uiLen, 30);        
                if (ucCheck != FALSE)
                {
                    /*lq 按在报文中出现的先后顺序记录命令类型号 */
                    pLBPrtCommStruct->RepetBuff[i] = atol(chTemp);                
                }
                else
                {
                    break;
                }
            }

            /*lq 保存命令数 */            
            if (i >= LB_PRT_REPET_BUFF_LEN)
            {
                pLBPrtCommStruct->RepetBuff[1] = LB_PRT_REPET_BUFF_LEN - 2;            
            }
            else
            {
                pLBPrtCommStruct->RepetBuff[1] = i - 2;            
            }
            
            /*lq 报文响应定时*/ 
            pLBPrtCommStruct->TimeCnt = SetTimePeriodProcess(LB_PRT_TIME_RESPONSE_INTERVAL);
        }                
    }

    /*lq AIWRM报文*/
    else if (strncmp((char *)pDat, "AIWRM", 5) == FALSE)
    {
		/*lq 获取第1字段内容，cmd type*/
		ucCheck = GetSegment((char *)pDat, chTemp, 1, uiLen, 30);        
        if (ucCheck != FALSE)
        {
            ulTemp = atol(chTemp);
            switch (ulTemp)
            {
                case LB_PRT_WRM_DATE:
                {
            		/*lq 获取第2字段内容，年月日 */
            		ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);    				
                    if (ucCheck != FALSE)
                    {
                        pLBPrtCommStruct->LB_TimeD = (chTemp[0] - '0') * 10 + (chTemp[1] - '0');
        				pLBPrtCommStruct->LB_TimeMon = (chTemp[2] - '0') * 10 + (chTemp[3] - '0');
        				pLBPrtCommStruct->LB_TimeY = (chTemp[4] - '0') * 10 + (chTemp[5] - '0');                      
                    
                		UartResponseMsg("$AIWRM,OK");                        
                    }
                    else
                    {
                		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？
                        break;                                           
                    }
                    
                    break;                
                }    

                case LB_PRT_WRM_UTC_TIME:
                {
            		/*lq 获取第2字段内容，时分秒 */
            		ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);    				
                    if (ucCheck != FALSE)
                    {
        				pLBPrtCommStruct->LB_TimeH = (chTemp[0] - '0') * 10 + (chTemp[1] - '0');
        				pLBPrtCommStruct->LB_TimeM = (chTemp[2] - '0') * 10 + (chTemp[3] - '0');
        				pLBPrtCommStruct->LB_TimeS = (chTemp[4] - '0') * 10 + (chTemp[5] - '0');                     
                    }
                    else
                    {
                		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                        break;
                    }

            		/*lq 获取第3字段内容，定位状态 */
            		ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);    				
                    if (ucCheck != FALSE)
                    {
        				pLBPrtCommStruct->LB_Stat = chTemp[0];                     
                    }
                    else
                    {
                		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                        break;
                    }

        			/**************************Latitude******************/
        			ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);
        
                    /*lq 字段非空*/
        			if (ucCheck != FALSE)
        			{
                        if (IsLatFormat(chTemp, &ulTemp, 4)== FALSE)
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                            break;                          
                        }

        			    /*lq -------N/S字段--------- */
        			    ucCheck = GetSegment((char *)pDat, chTemp, 5, uiLen, 30);
                        
                        /*lq 字段非空*/
            			if (ucCheck != FALSE)
            			{
                			/*lq 长度有效性判断*/
                			ucLen = strlen(chTemp);
                			if (ucLen != 1)
                			{
                        		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                                break;
                			}

                            /*lq 字符有效性判断*/
                            if (chTemp[0] == 'N')
                            {
 			                    slTemp = ulTemp;                            
                            }
                            else if (chTemp[0] == 'S')
                            {
 			                    slTemp = -ulTemp;                            
                            }
                            else
                            {
                        		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                                break;                             
                            }

	                        pLBPrtCommStruct->LB_Latitude = slTemp;
                        }
                        else
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                            break;                          
                        }
        			}
                    else
                    {
        			    /*lq -------N/S字段--------- */
        			    ucCheck = GetSegment((char *)pDat, chTemp, 5, uiLen, 30);
                    
                        /*lq 纬度数值字段为空，但N/S字段非空*/
        			    if (ucCheck != FALSE)
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                            break;                           
                        }
                    }

        			/**************************Longitude******************/
        			ucCheck = GetSegment((char *)pDat, chTemp, 6, uiLen, 30);
        
                    /*lq 字段非空*/
        			if (ucCheck != FALSE)
        			{
                        if (IsLonFormat(chTemp, &ulTemp, 4)== FALSE)
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                            break;                            
                        }

        			    /*lq -------E/W字段--------- */
                        ucCheck = GetSegment((char *)pDat, chTemp, 7, uiLen, 30);
                        
                        /*lq 字段非空*/
            			if (ucCheck != FALSE)
            			{
                			/*lq 长度有效性判断*/
                			ucLen = strlen(chTemp);
                			if (ucLen != 1)
                			{
                        		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                                break; 
                			}

                            /*lq 字符有效性判断*/
                            if (chTemp[0] == 'E')
                            {
 			                    slTemp = ulTemp;                            
                            }
                            else if (chTemp[0] == 'W')
                            {
 			                    slTemp = -ulTemp;                            
                            }
                            else
                            {
                        		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                                break;                              
                            }

	                        pLBPrtCommStruct->LB_Longitud = slTemp;
                        }
                        else
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                            break;                       
                        }
        			}
                    else
                    {
        			    /*lq -------E/W字段--------- */
        			    ucCheck = GetSegment((char *)pDat, chTemp, 7, uiLen, 30);
                    
                        /*lq 经度数值字段为空，但E/W字段非空*/
        			    if (ucCheck != FALSE)
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                            break;                       
                        }
                    }
                	
                    UartResponseMsg("$AIWRM,OK");                        
                    break;                
                } 

                case LB_PRT_WRM_ANCHOR:
                {
            		/*lq 获取第2字段内容，定位状态 */
            		ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);    				
                    if (ucCheck != FALSE)
                    {
        				pLBPrtCommStruct->LB_Stat = chTemp[0];                     
                    }
                    else
                    {
                		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                        break;
                    }

        			/**************************Latitude******************/
        			ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);
        
                    /*lq 字段非空*/
        			if (ucCheck != FALSE)
        			{
                        if (IsLatFormat(chTemp, &ulTemp, 4)== FALSE)
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                            break;                          
                        }

        			    /*lq -------N/S字段--------- */
        			    ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);
                        
                        /*lq 字段非空*/
            			if (ucCheck != FALSE)
            			{
                			/*lq 长度有效性判断*/
                			ucLen = strlen(chTemp);
                			if (ucLen != 1)
                			{
                        		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                                break;
                			}

                            /*lq 字符有效性判断*/
                            if (chTemp[0] == 'N')
                            {
 			                    slTemp = ulTemp;                            
                            }
                            else if (chTemp[0] == 'S')
                            {
 			                    slTemp = -ulTemp;                            
                            }
                            else
                            {
                        		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                                break;                             
                            }

	                        pLBPrtCommStruct->LB_Latitude = slTemp;
                        }
                        else
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                            break;                          
                        }
        			}
                    else
                    {
        			    /*lq -------N/S字段--------- */
        			    ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);
                    
                        /*lq 纬度数值字段为空，但N/S字段非空*/
        			    if (ucCheck != FALSE)
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                            break;                           
                        }
                    }

        			/**************************Longitude******************/
        			ucCheck = GetSegment((char *)pDat, chTemp, 5, uiLen, 30);
        
                    /*lq 字段非空*/
        			if (ucCheck != FALSE)
        			{
                        if (IsLonFormat(chTemp, &ulTemp, 4)== FALSE)
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                            break;                            
                        }

        			    /*lq -------E/W字段--------- */
                        ucCheck = GetSegment((char *)pDat, chTemp, 6, uiLen, 30);
                        
                        /*lq 字段非空*/
            			if (ucCheck != FALSE)
            			{
                			/*lq 长度有效性判断*/
                			ucLen = strlen(chTemp);
                			if (ucLen != 1)
                			{
                        		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                                break; 
                			}

                            /*lq 字符有效性判断*/
                            if (chTemp[0] == 'E')
                            {
 			                    slTemp = ulTemp;                            
                            }
                            else if (chTemp[0] == 'W')
                            {
 			                    slTemp = -ulTemp;                            
                            }
                            else
                            {
                        		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                                break;                              
                            }

	                        pLBPrtCommStruct->LB_Longitud = slTemp;
                        }
                        else
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                            break;                       
                        }
        			}
                    else
                    {
        			    /*lq -------E/W字段--------- */
        			    ucCheck = GetSegment((char *)pDat, chTemp, 6, uiLen, 30);
                    
                        /*lq 经度数值字段为空，但E/W字段非空*/
        			    if (ucCheck != FALSE)
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？                    
                            break;                       
                        }
                    }
                	
                    UartResponseMsg("$AIWRM,OK");                        
                    break;                
                } 

                case LB_PRT_WRM_MMSI:
                {
		            /*lq 获取第2字段内容，提取MMSI*/
					ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
					if (ucCheck == FALSE)
					{
                		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？
                        break; 
					}

                    /*lq 长度有效性判断*/
					ucLen = strlen(chTemp);
					if (ucLen != 9)
					{
                		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？
                        break; 
					}

                    /*lq 数值有效性判断*/
                    if (IsAscStringOk(chTemp) == FALSE)
                    {
                		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？
                        break;                             
                    }

					/*lq 写入MMSI号 */
					Para_Save(PARA_MMSI_TYPE, (U8 *)chTemp, ucLen);
					Para_Save(PARA_MMSI_BACKUP_TYPE, (U8 *)chTemp, ucLen);//同时写备份

					/*lq 此处增加更新协议栈接口 */
					siStaticPara.MMSI = atol(chTemp);
					siStaticPara.Refresh = TRUE;

            		UartResponseMsg("$AIWRM,OK");
                    
                    break;                
                }
                
                case LB_PRT_WRM_LIGHT_BEACON_TYPE:
                {
       			    /***********************Type of AtoN***********************/
                    ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
        
        			/*lq 字段非空*/
                    if (ucCheck != FALSE)
        			{
            			/*lq 长度有效性判断*/
                        ucLen = strlen(chTemp);
            			if (ucLen != 2)
            			{
                    		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？
                            break; 
            			}
                        			
                        /*lq 字符有效性判断*/
                        ucCheck = IsAscStringOk(chTemp);
            			if (ucCheck == FALSE)
            			{
                    		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？
                            break; 
            			}
                        else
                        {
                            /*lq 取值范围有效性判断*/
                            ulTemp = atol(chTemp);
                            if (ulTemp < PARA_ATON_TYPE_FLOOR
                                || ulTemp > PARA_ATON_TYPE_CEIL)
                            {
                        		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？
                                break;                
                            }            
                        }
            
            			Para_Save(PARA_ATON_TYPE_TYPE, (U8 *)chTemp, ucLen);
            			siStaticPara.TypeofAtoN = atoi(chTemp);
                        siStaticPara.Refresh = TRUE;
        			}

            		UartResponseMsg("$AIWRM,OK");
                    
                    break;                
                }

                case LB_PRT_WRM_NAME:
                {
        			/******************************Name of AtoN************************/
                    ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 40);
        
                    /*lq 字段非空*/
        			if (ucCheck != FALSE)
        			{
            			/*lq 长度有效性判断*/
                        ucLen = strlen(chTemp);
            			if (ucLen > PARA_ATON_NAME_MAXLEN)
            			{
                    		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？
                            break;   
            			}
                        			
            			Para_Save(PARA_ATON_NAME_TYPE, (U8 *)chTemp, ucLen);
            			strcpy(siStaticPara.NameofAtoN, chTemp);
					    siStaticPara.Refresh = TRUE;
        			}

            		UartResponseMsg("$AIWRM,OK");
                    
                    break;                
                }

                case LB_PRT_WRM_ATON_STATUE:
                {
                    /***************************AtoN status bits***********************/
        			ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
        			
                    /*lq 字段非空*/
                    if (ucCheck != FALSE)
        			{
            			/*lq 长度有效性判断*/
            			ucLen = strlen(chTemp);
            			if (ucLen != 8)
            			{
                    		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？
                            break;  
            			}
            
                        /*lq 字符有效性判断*/
                        if (IsHexStringOk(chTemp) == FALSE)
                        {
                    		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？
                            break;     			                                   
                        }
                        else
                        {
                            ucTemp = 0;
                            for (i = 0; i < 8; i++)
                            {
                                ucTemp <<= 1;
                                ucTemp |= ((chTemp[i] - '0') & 0x1);
                            }
                            siStaticPara.AtoNStatus = ucTemp; 
					        siStaticPara.Refresh = TRUE;                                    
                        }
        			}

            		UartResponseMsg("$AIWRM,OK");
                    
                    break;                
                }

                case LB_PRT_WRM_USART_SETTING:
                {
        		    /*lq baud rate*/
                	ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
                    if (ucCheck != FALSE)
        			{
                        ulTemp = atol(chTemp);
        			}
                    else
                    {
                		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？
                        break;                     
                    }

        		    /*lq data length*/
                	ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);
                    if (ucCheck != FALSE)
        			{
                        ucTemp1 = atoi(chTemp);
        			}
                    else
                    {
                		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？
                        break;                     
                    }

        		    /*lq parity*/
                	ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);
                    if (ucCheck != FALSE)
        			{
                        ucTemp2 = atoi(chTemp);
        			}
                    else
                    {
                		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？
                        break;                     
                    }

        		    /*lq stop bits*/
                	ucCheck = GetSegment((char *)pDat, chTemp, 5, uiLen, 30);
                    if (ucCheck != FALSE)
        			{
                        ucTemp3 = atoi(chTemp);
        			}
                    else
                    {
                		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？
                        break;                     
                    }

                    /*lq 修改USART1配置*/
                    if (Usart1_UsartConfig(ulTemp, ucTemp1, ucTemp2, ucTemp3) == FALSE)
                    {
                 		UartResponseMsg("$AIWRM,301");      //lq?错误代码含义？
                        break;                   
                    }
                    else
                    {
            		    /*lq 修改USART1配置结构体数据*/
                        USART1_UsartConfigStructInit(ulTemp, ucTemp1, ucTemp2, ucTemp3);

                        UartResponseMsg("$AIWRM,OK");                    
                    }
                    
                    break;                
                }
                                                                                                                
                default :
                    break;
            }       
        }   
    }

    /*lq AICDM报文*/
    else if (strncmp((char *)pDat, "AICDM", 5) == FALSE)
    {
		/*lq 获取第1字段内容，cmd type*/
		ucCheck = GetSegment((char *)pDat, chTemp, 1, uiLen, 30);        
        if (ucCheck != FALSE)
        {
            ulTemp = atol(chTemp);
            switch (ulTemp)
            {
                case LB_PRT_CDM_START_INTERROGATION:
                {               
                    /*lq 置位轮询标志*/
                    pLBPrtCommStruct->InterrogationFlag = TRUE;                    
                    UartResponseMsg("$AICDM,OK");                                            
                    break;
                }
                case LB_PRT_CDM_AIS_SLEEP:
                {
                    //lq... 与stop interrogation命令的效果一样
                    /*lq 复位结构体数据 */
                    LB_LBPrtCommStructReset(pLBPrtCommStruct);
                    UartResponseMsg("$AICDM,OK");                                            
                    break;
                }
                case LB_PRT_CDM_STOP_INTERROGATION:
                {
                    /*lq 复位结构体数据 */
                    LB_LBPrtCommStructReset(pLBPrtCommStruct);                                        
                    UartResponseMsg("$AICDM,OK");                                                                
                    break;
                }                                                
                default :
                    break;
            }
        }
    }

    else
    {
        return FALSE;
    }

    return TRUE;
}

/*******************************************************************************
* 名称 ：LB_MsgDealProcess
* 描述 ：处理Light Beacon Msg
* 输入 ：无
* 输出 ：无
* 返回 ：TRUE-消息报文是Light Beacon Msg，FALSE-消息报文不是Light Beacon Msg
*******************************************************************************/
U8 LB1_MsgDealProcess(U8 *pDat, U16 uiLen)
{
    return LB_MsgDealProcess(&LBPrtCommPara, pDat, uiLen);    
}

/*******************************************************************************
* 名称 ：LB_GetLBCommSleepSta
* 描述 ：查询light beacon通信模块休眠条件
* 输入 ：pLBPrtCommStruct：LBPrtCommStruct类型指针
* 输出 ：无
* 返回 ：无
*******************************************************************************/
static U8 LB_GetLBCommSleepSta(LBPrtCommStruct *pLBPrtCommStruct)
{
    if (pLBPrtCommStruct->InterrogationFlag == FALSE)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称 ：LB1_GetLBCommSleepSta
* 描述 ：查询light beacon通信模块休眠条件
* 输入 ：无
* 输出 ：无
* 返回 ：无
*******************************************************************************/
U8 LB1_GetLBCommSleepSta(void)
{
    return LB_GetLBCommSleepSta(&LBPrtCommPara);    
}

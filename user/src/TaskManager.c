/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: TaskManager.c
  Author: lq       Version : V1.0.0        Date: 2015.07.31
  Description:     杂项任务管理
  Version:         V1.0.0
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
    <author>    <time>      <version >  <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "Include.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/
TaskTimStruct RecordCloseTimeTask;    // GPS 开关机
TaskTimStruct UpdateRecStationTask;

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* 名称  : TASK_TaskTimStructInit
* 描述  : TaskTimStruct初始化结构体
* 输入  : pTaskTimStruct - TaskTimStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TASK_TaskTimStructInit(TaskTimStruct *pTaskTimStruct)
{
	pTaskTimStruct->TimeCnt = 0;
	pTaskTimStruct->TimePeriod = TASK_TIME_PERIOD;	
    pTaskTimStruct->Timeout = TASK_TIME_TIMEOUT;
    pTaskTimStruct->TimeoutCnt = 0;

    pTaskTimStruct->Enable = FALSE;
}

/*******************************************************************************
* 名称  : TASK_TaskTimEnable
* 描述  : 任务定时使能
* 输入  : pTaskTimStruct - TaskTimStruct结构体指针
          timeout - 任务周期	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TASK_TaskTimEnable(TaskTimStruct *pTaskTimStruct, U16 timeout)
{
	pTaskTimStruct->TimeCnt = SetTimePeriodProcess(pTaskTimStruct->TimePeriod);
    pTaskTimStruct->Timeout = timeout;
    pTaskTimStruct->TimeoutCnt = 0;

    pTaskTimStruct->Enable = TRUE;
}

/*******************************************************************************
* 名称  : TASK_TaskTimDisable
* 描述  : 任务定时失能
* 输入  : pTaskTimStruct - TaskTimStruct结构体指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TASK_TaskTimDisable(TaskTimStruct *pTaskTimStruct)
{
    pTaskTimStruct->Enable = FALSE;
}

/*******************************************************************************
* 名称  : TASK_PeriodTaskIsTimeout
* 描述  : 周期任务超时判断
* 输入  : pTaskTimStruct - TaskTimStruct结构体指针
* 输出  : 无
* 返回  : 是否任务定时到
*******************************************************************************/
U8 TASK_PeriodTaskIsTimeout(TaskTimStruct *pTaskTimStruct)
{
    if (pTaskTimStruct->Enable == TRUE)
    {
        if (pTaskTimStruct->TimeCnt == GetLocalClockTickProcess())
    	{
    		pTaskTimStruct->TimeCnt = SetTimePeriodProcess(pTaskTimStruct->TimePeriod);
            pTaskTimStruct->TimeoutCnt++;                
            if (pTaskTimStruct->TimeoutCnt >= pTaskTimStruct->Timeout)
            {
                pTaskTimStruct->TimeoutCnt = 0;
    
            	return TRUE;
            }                        
        }    
    }

    return FALSE;
}

/*******************************************************************************
* 名称  : TASK_SingleTaskIsTimeout
* 描述  : 单次任务超时判断
* 输入  : pTaskTimStruct - TaskTimStruct结构体指针
* 输出  : 无
* 返回  : 是否任务定时到
*******************************************************************************/
U8 TASK_SingleTaskIsTimeout(TaskTimStruct *pTaskTimStruct)
{
    if (pTaskTimStruct->Enable == TRUE)
    {
        if (pTaskTimStruct->TimeCnt == GetLocalClockTickProcess())
    	{
    		pTaskTimStruct->TimeCnt = SetTimePeriodProcess(pTaskTimStruct->TimePeriod);
            pTaskTimStruct->TimeoutCnt++;                
            if (pTaskTimStruct->TimeoutCnt >= pTaskTimStruct->Timeout)
            {
                pTaskTimStruct->TimeoutCnt = 0;
    
            	pTaskTimStruct->Enable = FALSE;
                
                return TRUE;
            }                        
        }    
    }

    return FALSE;
}

/*******************************************************************************
* 名称  : TASK_TaskProcess
* 描述  : 杂项任务处理
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TASK_TaskProcess(void)
{
    DateTimeStruct dateTime;
	U8  ucTemp[10];
	U8  ucTemp1[15];

    /*lq 记录 GPS 关机时间 */
    if (TASK_PeriodTaskIsTimeout(&RecordCloseTimeTask) == TRUE)
    {
		/*lq 记录关机时间 */
        dateTime.Year = GpsPara.GPS_TimeY;
        dateTime.Month = GpsPara.GPS_TimeMon;
        dateTime.Day = GpsPara.GPS_TimeD;
        dateTime.Hour = GpsPara.GPS_TimeH;
        dateTime.Minute = GpsPara.GPS_TimeM;
        dateTime.Second = GpsPara.GPS_TimeS;
        
        sprintf((char *)ucTemp, "%02d", (dateTime.Year) % 100);
    	sprintf((char *)ucTemp1, "%02d", dateTime.Month);
    	strcat((char *)ucTemp, (char *)ucTemp1);
        sprintf((char *)ucTemp1, "%02d", dateTime.Day);
    	strcat((char *)ucTemp, (char *)ucTemp1);
    	sprintf((char *)ucTemp1, "%02d", dateTime.Hour);
    	strcat((char *)ucTemp, (char *)ucTemp1);
        sprintf((char *)ucTemp1, "%02d", dateTime.Minute);
    	strcat((char *)ucTemp, (char *)ucTemp1);
    	sprintf((char *)ucTemp1, "%02d", dateTime.Second);
    	strcat((char *)ucTemp, (char *)ucTemp1);

        Para_CloseTimeSave(ucTemp, DevDynamicPara.OpenCloseTimeIndex);
        debug_printf("$[%6d:%4d] %s : Record close time!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);

    }

    /*lq 更新接收船台数 */
    if (TASK_PeriodTaskIsTimeout(&UpdateRecStationTask) == TRUE)
    {
        AISINFO_InstRecStationQuantityUpdate();
        debug_printf("$[%6d:%4d] %s : Update received stations!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);
    }
}

/*******************************************************************************
* 名称  : TASK_ModuleInit
* 描述  : 模块初始化
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TASK_ModuleInit(void) 
{
    TASK_TaskTimStructInit(&RecordCloseTimeTask);
    TASK_TaskTimStructInit(&UpdateRecStationTask);
    TASK_TaskTimEnable(&UpdateRecStationTask,
        TASK_TIME_TIMEOUT_UPDATE_REC_STATION);
}

/*==============================================================================
------------------------------------End of File---------------------------------
==============================================================================*/

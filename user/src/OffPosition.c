/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: batteryscan.c
  Author: lq       Version : V1.00        Date: 2014.01.02
  Description:     电量扫描
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "def.h"
#include "initial.h"
#include "OffPosition.h"
#include "config.h"

/*lq 模块实例定义*/
GPSOffPositionStruct GPSOffPositionPara;

/*******************************************************************************
* 名称  : GPSOffPositionStructInit
* 描述  : 初始化GPS位置偏离结构体参数
* 输入  : GPSOffPositionStruct类型指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void OP_GPSOffPositionStructInit(GPSOffPositionStruct *pGPSOffPositionStruct)
{
    U16 i;

    for (i = 0; i < GPS_ON_POSITION_SAMPLE_NUM + 1; i++)
    {
        pGPSOffPositionStruct->PositionNormSample[i][0] = 0;
        pGPSOffPositionStruct->PositionNormSample[i][1] = 0;
    }

    for (i = 0; i < GPS_OFF_POSITION_SAMPLE_NUM + 1; i++)
    {
        pGPSOffPositionStruct->PositionOffSample[i][0] = 0;
        pGPSOffPositionStruct->PositionOffSample[i][1] = 0;
    }

    for (i = 0; i < GPS_ON_POSITION_SAMPLE_NUM + 1; i++)
    {
        pGPSOffPositionStruct->PositionFlag[i] = TRUE;
    }
    
    pGPSOffPositionStruct->PositionOffIndexIn = 0;
    pGPSOffPositionStruct->PositionOffIndexOut = 0;
    pGPSOffPositionStruct->PositionFlagIndexIn = 0;
    pGPSOffPositionStruct->PositionFlagIndexOut = 0;

    pGPSOffPositionStruct->InsideSum = 0;

    pGPSOffPositionStruct->AvarDist = 0;
    pGPSOffPositionStruct->AvarLon = 0;
    pGPSOffPositionStruct->AvarLat = 0;
    //lq pGPSOffPositionStruct->OnPositionFlag = FALSE;
    pGPSOffPositionStruct->OffPositionFlag = FALSE;
    pGPSOffPositionStruct->OffPositionPossFlag = FALSE;
    pGPSOffPositionStruct->OffPositionValid = FALSE;
    pGPSOffPositionStruct->OffPositionPossValid = FALSE;
}

/*******************************************************************************
* 名称  : GPSOffPositionStructReset
* 描述  : 复位GPS位置偏离结构体参数
* 输入  : GPSOffPositionStruct类型指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void OP_GPSOffPositionStructReset(GPSOffPositionStruct *pGPSOffPositionStruct)
{
    U16 i;

    for (i = 0; i < GPS_ON_POSITION_SAMPLE_NUM + 1; i++)
    {
        pGPSOffPositionStruct->PositionNormSample[i][0] = 0;
        pGPSOffPositionStruct->PositionNormSample[i][1] = 0;
    }

    for (i = 0; i < GPS_OFF_POSITION_SAMPLE_NUM + 1; i++)
    {
        pGPSOffPositionStruct->PositionOffSample[i][0] = 0;
        pGPSOffPositionStruct->PositionOffSample[i][1] = 0;
    }

    for (i = 0; i < GPS_ON_POSITION_SAMPLE_NUM + 1; i++)
    {
        pGPSOffPositionStruct->PositionFlag[i] = TRUE;
    }
    
    pGPSOffPositionStruct->PositionOffIndexIn = 0;
    pGPSOffPositionStruct->PositionOffIndexOut = 0;
    pGPSOffPositionStruct->PositionFlagIndexIn = 0;
    pGPSOffPositionStruct->PositionFlagIndexOut = 0;

    pGPSOffPositionStruct->InsideSum = 0;

    pGPSOffPositionStruct->AvarDist = 0;
    pGPSOffPositionStruct->AvarLon = 0;
    pGPSOffPositionStruct->AvarLat = 0;
    //lq pGPSOffPositionStruct->OnPositionFlag = FALSE;    
    pGPSOffPositionStruct->OffPositionFlag = FALSE;
    pGPSOffPositionStruct->OffPositionPossFlag = FALSE;
    pGPSOffPositionStruct->OffPositionValid = FALSE;
    pGPSOffPositionStruct->OffPositionPossValid = FALSE;
}

/*******************************************************************************
* 名称  : GPSOffPositionInstanceInit
* 描述  : 初始化GPS位置偏离结构体实例
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void OP1_GPSOffPositionInstanceInit(void)
{
    OP_GPSOffPositionStructInit(&GPSOffPositionPara);
}

/*******************************************************************************
* 名称  : GPSOffPositionInstanceInit
* 描述  : 复位GPS位置偏离结构体实例
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void OP1_GPSOffPositionInstanceReset(void)
{
    OP_GPSOffPositionStructReset(&GPSOffPositionPara);
}

#if (CONFIG_OFF_POSITON_ALGORITHM == 0)
/*******************************************************************************
* 名称  : GPSOffPositionAddSamplePer
* 描述  : 记录新采样点的位置偏离状态，百分比方法，
* 输入  : State-位置偏离状态，TRUE：未偏离，FALSE-偏离		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void OP_GPSOffPositionAddSamplePerc(GPSOffPositionStruct *pGPSOffPositionStruct, const U8 State)
{
    /*lq 缓存已满 */      
    if (((pGPSOffPositionStruct->PositionFlagIndexIn + 1) % (GPS_OFF_POSITION_SAMPLE_NUM + 1))
        == pGPSOffPositionStruct->PositionFlagIndexOut)
    {
        /*lq 保存数据 */      
        pGPSOffPositionStruct->PositionFlag[pGPSOffPositionStruct->PositionFlagIndexIn] = State;

        /*lq 重新统计目前位置未偏离的样点数*/
        if (pGPSOffPositionStruct->PositionFlag[pGPSOffPositionStruct->PositionFlagIndexIn]
            != pGPSOffPositionStruct->PositionFlag[pGPSOffPositionStruct->PositionFlagIndexOut])
        {
            if (pGPSOffPositionStruct->PositionFlag[pGPSOffPositionStruct->PositionFlagIndexIn] == TRUE)
            {
                pGPSOffPositionStruct->InsideSum++;      
            }
            else
            {
                pGPSOffPositionStruct->InsideSum--;      
            }    
        }
        
        /*lq IndexIn索引加1*/
        pGPSOffPositionStruct->PositionFlagIndexIn++;
        pGPSOffPositionStruct->PositionFlagIndexIn %= (GPS_OFF_POSITION_SAMPLE_NUM + 1);

        /*lq IndexOut索引加1*/
        pGPSOffPositionStruct->PositionFlagIndexOut++;
        pGPSOffPositionStruct->PositionFlagIndexOut %= (GPS_OFF_POSITION_SAMPLE_NUM + 1);
    }
    
    /*lq 缓存未满 */
    else
    {        
        /*lq 保存数据 */      
        pGPSOffPositionStruct->PositionFlag[pGPSOffPositionStruct->PositionFlagIndexIn] = State;

        /*lq 重新统计目前位置未偏离的样点数*/
        if (pGPSOffPositionStruct->PositionFlag[pGPSOffPositionStruct->PositionFlagIndexIn] == TRUE)
        {
            pGPSOffPositionStruct->InsideSum++;      
        }

        /*lq IndexIn索引加1*/
        pGPSOffPositionStruct->PositionFlagIndexIn++;
        pGPSOffPositionStruct->PositionFlagIndexIn %= (GPS_OFF_POSITION_SAMPLE_NUM + 1);
    }
}

/*******************************************************************************
* 名称  : GPSPositionAnalPerc
* 描述  : GPS定位位置分析，百分比方法，
* 输入  : Lon-经度（单位0.0001分），Lat-纬度（单位0.0001分）
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void OP_GPSPositionAnalPerc(GPSOffPositionStruct *pGPSOffPositionStruct, const S32 Lon, const S32 Lat)
{
    float Lon1, Lat1, Lon2, Lat2;
    float Dist;

    /*lq 当前经纬度换算为以度为单位*/
    Lon1 = Lon / 600000.0;
    Lat1 = Lat / 600000.0;

    /*lq 目的经纬度换算为以度为单位*/
    Lon2 = siStaticPara.NomLongitude / 600000.0;
    Lat2 = siStaticPara.NomLatitude / 600000.0;
        
    /*lq 计算当前位置坐标与目标位置坐标的距离*/        
    Dist = GetDistance(Lat1, Lon1, Lat2, Lon2);

    /*lq 保存当前位置坐标的偏离状态*/
    if (Dist > siStaticPara.OffPositionThresh)
    {
        OP_GPSOffPositionAddSamplePerc(pGPSOffPositionStruct, FALSE);
    }
    else
    {
        OP_GPSOffPositionAddSamplePerc(pGPSOffPositionStruct, TRUE);
    }     

    /*lq 偏离状态下，偏离状态设置*/
    if (((pGPSOffPositionStruct->PositionFlagIndexIn + 1) % (GPS_OFF_POSITION_SAMPLE_NUM + 1))
        == pGPSOffPositionStruct->PositionFlagIndexOut)
    {
        pGPSOffPositionStruct->OffPositionValid = TRUE;

        /*lq 在有效范围内的点的数目大于阈值*/
        if (pGPSOffPositionStruct->InsideSum > GPS_OFF_POSITION_PERC_THRES)
        {
            pGPSOffPositionStruct->OffPositionFlag = FALSE;            
        }
        
        /*lq 在有效范围外的点的数目大于阈值*/
        else if ((GPS_OFF_POSITION_SAMPLE_NUM - pGPSOffPositionStruct->InsideSum)
                  > GPS_OFF_POSITION_PERC_THRES)
        {
            pGPSOffPositionStruct->OffPositionFlag = TRUE;            
        }       
    }
    else
    {
        /*lq 正常状态下，偏离状态设置*/
        if (pGPSOffPositionStruct->PositionFlagIndexIn >= GPS_ON_POSITION_SAMPLE_NUM)
        {
            pGPSOffPositionStruct->OffPositionPossValid = TRUE;
            
            if (pGPSOffPositionStruct->PositionFlagIndexIn != pGPSOffPositionStruct->InsideSum)
            {
                pGPSOffPositionStruct->OffPositionPossFlag = TRUE;            
            }
        }
    }

#if CONFIG_DEBUG
    usart_printf("[%6d:%4d]$GPSPositionAnalPerc Distance : %f!\r\n", 
                 GetFrameNum(), GetSlotNum(), Dist);
    
    usart_printf("[%6d:%4d]$GPSPositionAnalPerc InsideSum: %d!\r\n", 
                 GetFrameNum(), GetSlotNum() ,pGPSOffPositionStruct->InsideSum);
#endif
}

#elif (CONFIG_OFF_POSITON_ALGORITHM == 1)
/*******************************************************************************
* 名称  : GPSOffPositionAddSamplePos
* 描述  : 记录新采样点位置坐标，平均位置方法，
* 输入  : Lon-经度，Lat-纬度		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void OP_GPSOffPositionAddSamplePos(GPSOffPositionStruct *pGPSOffPositionStruct, const S32 Lon, const S32 Lat)
{
    float Temp1;
    float Temp2;
    
    Temp1 = (float)Lon;    
    Temp2 = (float)Lat;

    /*lq 缓存已满 */  
    if (((pGPSOffPositionStruct->PositionOffIndexIn + 1) % (GPS_OFF_POSITION_SAMPLE_NUM + 1))
        == pGPSOffPositionStruct->PositionOffIndexOut)
    {
        /*lq 缓存中存放原始值的GPS_OFF_POSITION_SAMPLE_NUM分之1值*/
        Temp1 = (float)Lon / GPS_OFF_POSITION_SAMPLE_NUM;    
        Temp2 = (float)Lat / GPS_OFF_POSITION_SAMPLE_NUM;

        /*lq 计算最近GPS_OFF_POSITION_SAMPLE_NUM个经度采样点的平均值*/
        pGPSOffPositionStruct->AvarLon -= pGPSOffPositionStruct->PositionOffSample[pGPSOffPositionStruct->PositionOffIndexOut][0];
        pGPSOffPositionStruct->AvarLon += Temp1;

        /*lq 计算最近GPS_OFF_POSITION_SAMPLE_NUM个纬度采样点的平均值*/
        pGPSOffPositionStruct->AvarLat -= pGPSOffPositionStruct->PositionOffSample[pGPSOffPositionStruct->PositionOffIndexOut][1];
        pGPSOffPositionStruct->AvarLat += Temp2;

        /*lq 保存数据 */      
        pGPSOffPositionStruct->PositionOffSample[pGPSOffPositionStruct->PositionOffIndexIn][0] = Temp1;
        pGPSOffPositionStruct->PositionOffSample[pGPSOffPositionStruct->PositionOffIndexIn][1] = Temp2;

        /*lq IndexIn索引加1*/
        pGPSOffPositionStruct->PositionOffIndexIn++;
        pGPSOffPositionStruct->PositionOffIndexIn %= (GPS_OFF_POSITION_SAMPLE_NUM + 1);

        /*lq IndexOut索引加1*/
        pGPSOffPositionStruct->PositionOffIndexOut++;
        pGPSOffPositionStruct->PositionOffIndexOut %= (GPS_OFF_POSITION_SAMPLE_NUM + 1);
    }
    
    /*lq 缓存未满 */
    else
    {        
        /*lq 计算平均坐标值*/
    	pGPSOffPositionStruct->AvarLon += (Temp1 - pGPSOffPositionStruct->AvarLon)
                                           / (pGPSOffPositionStruct->PositionOffIndexIn + 1);   
    	pGPSOffPositionStruct->AvarLat += (Temp2 - pGPSOffPositionStruct->AvarLat)
                                           / (pGPSOffPositionStruct->PositionOffIndexIn + 1);

        /*lq 缓存中存放原始值的GPS_OFF_POSITION_SAMPLE_NUM分之1值*/
        Temp1 = (float)Lon / GPS_OFF_POSITION_SAMPLE_NUM;    
        Temp2 = (float)Lat / GPS_OFF_POSITION_SAMPLE_NUM;

        /*lq 保存数据 */
        pGPSOffPositionStruct->PositionOffSample[pGPSOffPositionStruct->PositionOffIndexIn][0] = Temp1;
        pGPSOffPositionStruct->PositionOffSample[pGPSOffPositionStruct->PositionOffIndexIn][1] = Temp2;

        /*lq IndexIn索引加1*/
        pGPSOffPositionStruct->PositionOffIndexIn++;
        pGPSOffPositionStruct->PositionOffIndexIn %= (GPS_OFF_POSITION_SAMPLE_NUM + 1);
    }
}

/*******************************************************************************
* 名称  : GPSPositionAnalAvarPos
* 描述  : GPS定位位置分析，平均位置方法，
* 输入  : Lon-经度，Lat-纬度		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void OP_GPSPositionAnalAvarPos(GPSOffPositionStruct *pGPSOffPositionStruct, const S32 Lon, const S32 Lat)
{
    float Lon1, Lat1, Lon2, Lat2;
    float Dist;

    /*lq 添加坐标点，并计算位置坐标平均值*/
    OP_GPSOffPositionAddSamplePos(pGPSOffPositionStruct, Lon, Lat);

    /*lq 平均经纬度换算为以度为单位*/
    Lon1 = pGPSOffPositionStruct->AvarLon / 600000.0;
    Lat1 = pGPSOffPositionStruct->AvarLat / 600000.0;

    /*lq 目的经纬度换算为以度为单位*/
    Lon2 = siStaticPara.NomLongitude / 600000.0;
    Lat2 = siStaticPara.NomLatitude / 600000.0;

    /*lq 计算当前平均位置坐标与目标位置坐标的距离*/
    Dist = GetDistance(Lat1, Lon1, Lat2, Lon2);
    
    /*lq 偏离状态下，当前位置偏离判断*/
    if (((pGPSOffPositionStruct->PositionOffIndexIn + 1) % (GPS_OFF_POSITION_SAMPLE_NUM + 1))
        == pGPSOffPositionStruct->PositionOffIndexOut)
    {
        pGPSOffPositionStruct->OffPositionValid = TRUE;
        
        if (Dist > siStaticPara.OffPositionThresh)
        {
            pGPSOffPositionStruct->OffPositionFlag = TRUE;    
        }
        else
        {
            pGPSOffPositionStruct->OffPositionFlag = FALSE;        
        } 
    }
    else 
    {
        /*lq 正常状态下，偏离状态设置*/
        if (pGPSOffPositionStruct->PositionOffIndexIn >= GPS_ON_POSITION_SAMPLE_NUM)
        {
            pGPSOffPositionStruct->OffPositionPossValid = TRUE;

            if (Dist > siStaticPara.OffPositionThresh)
            {
                pGPSOffPositionStruct->OffPositionPossFlag = TRUE;
            }        
        }
    }
}

#elif (CONFIG_OFF_POSITON_ALGORITHM == 2)
/*******************************************************************************
* 名称  : GPSOffPositionAddSampleDist
* 描述  : 记录新采样点位置距离，平均距离方法，
* 输入  : Lon-经度，Lat-纬度		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void OP_GPSOffPositionAddSampleDist(GPSOffPositionStruct *pGPSOffPositionStruct, const float Dist)
{
    float Temp1;

    /*lq 缓存已满 */     
    if (((pGPSOffPositionStruct->PositionOffIndexIn + 1) % (GPS_OFF_POSITION_SAMPLE_NUM + 1))
        == pGPSOffPositionStruct->PositionOffIndexOut)
    {
        /*lq 缓存中存放原始值的GPS_OFF_POSITION_SAMPLE_NUM分之1值*/
        Temp1 = Dist / GPS_OFF_POSITION_SAMPLE_NUM;    

        /*lq 计算最近GPS_OFF_POSITION_SAMPLE_NUM个经度采样点距离的平均值*/
        pGPSOffPositionStruct->AvarDist -= pGPSOffPositionStruct->PositionOffSample[pGPSOffPositionStruct->PositionOffIndexOut][0];
        pGPSOffPositionStruct->AvarDist += Temp1;

        /*lq 保存数据 */      
        pGPSOffPositionStruct->PositionOffSample[pGPSOffPositionStruct->PositionOffIndexIn][0] = Temp1;

        /*lq IndexIn索引加1*/
        pGPSOffPositionStruct->PositionOffIndexIn++;
        pGPSOffPositionStruct->PositionOffIndexIn %= (GPS_OFF_POSITION_SAMPLE_NUM + 1);

        /*lq IndexOut索引加1*/
        pGPSOffPositionStruct->PositionOffIndexOut++;
        pGPSOffPositionStruct->PositionOffIndexOut %= (GPS_OFF_POSITION_SAMPLE_NUM + 1);
    }
    
    /*lq 缓存未满 */
    else
    {        
        /*lq 计算平均距离值*/
    	pGPSOffPositionStruct->AvarDist += (Dist - pGPSOffPositionStruct->AvarDist)
                                           / (pGPSOffPositionStruct->PositionOffIndexIn + 1);   

        /*lq 缓存中存放原始值的GPS_OFF_POSITION_SAMPLE_NUM分之1值*/
        Temp1 = Dist / GPS_OFF_POSITION_SAMPLE_NUM;    

        /*lq 保存数据 */
        pGPSOffPositionStruct->PositionOffSample[pGPSOffPositionStruct->PositionOffIndexIn][0] = Temp1;

        /*lq IndexIn索引加1*/
        pGPSOffPositionStruct->PositionOffIndexIn++;
        pGPSOffPositionStruct->PositionOffIndexIn %= (GPS_OFF_POSITION_SAMPLE_NUM + 1);
    }
}

/*******************************************************************************
* 名称  : GPSPositionAnalAvarDist
* 描述  : GPS定位位置分析，平均距离方法，
* 输入  : Lon-经度，Lat-纬度		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void OP_GPSPositionAnalAvarDist(GPSOffPositionStruct *pGPSOffPositionStruct, const S32 Lon, const S32 Lat)
{
    float Lon1, Lat1, Lon2, Lat2;
    float Dist;

    /*lq 当前经纬度换算为以度为单位*/
    Lon1 = Lon / 600000.0;
    Lat1 = Lat / 600000.0;

    /*lq 目的经纬度换算为以度为单位*/
    Lon2 = siStaticPara.NomLongitude / 600000.0;
    Lat2 = siStaticPara.NomLatitude / 600000.0;
        
    /*lq 计算当前位置坐标与目标位置坐标的距离*/        
    Dist = GetDistance(Lat1, Lon1, Lat2, Lon2);

    /*lq 保存当前位置坐标的距离 */
    OP_GPSOffPositionAddSampleDist(pGPSOffPositionStruct, Dist);

    /*lq 偏离状态下，当前位置偏离判断*/
    if (((pGPSOffPositionStruct->PositionOffIndexIn + 1) % (GPS_OFF_POSITION_SAMPLE_NUM + 1))
        == pGPSOffPositionStruct->PositionOffIndexOut)
    {
        pGPSOffPositionStruct->OffPositionValid = TRUE;
        
        if (pGPSOffPositionStruct->AvarDist > siStaticPara.OffPositionThresh)
        {
            pGPSOffPositionStruct->OffPositionFlag = TRUE;    
        }
        else
        {
            pGPSOffPositionStruct->OffPositionFlag = FALSE;        
        } 
    }
    else 
    {
        /*lq 正常状态下，偏离状态设置*/
        if (pGPSOffPositionStruct->PositionOffIndexIn >= GPS_ON_POSITION_SAMPLE_NUM)
        {
            pGPSOffPositionStruct->OffPositionPossValid = TRUE;

            if (pGPSOffPositionStruct->AvarDist > siStaticPara.OffPositionThresh)
            {
                pGPSOffPositionStruct->OffPositionPossFlag = TRUE;
            }        
        }
    }    
}
#endif

/*******************************************************************************
* 名称  : OP1_GPSPositionAnalPerc
* 描述  : GPS定位位置分析，外部接口
* 输入  : Lon-经度（单位0.0001分），Lat-纬度（单位0.0001分）
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void OP1_GPSPositionAnal(const S32 Lon, const S32 Lat)
{
#if (CONFIG_OFF_POSITON_ALGORITHM == 0)
    OP_GPSPositionAnalPerc(&GPSOffPositionPara, Lon, Lat);

#elif (CONFIG_OFF_POSITON_ALGORITHM == 1)
    OP_GPSPositionAnalAvarPos(&GPSOffPositionPara, Lon, Lat);

#elif (CONFIG_OFF_POSITON_ALGORITHM == 2)
    OP_GPSPositionAnalAvarDist(&GPSOffPositionPara, Lon, Lat);

#endif
}

/*******************************************************************************
* 名称  : GetGPSOffPositionStat
* 描述  : 查询位置偏离状态
* 输入  : TRUE-偏离，FALSE-未偏离		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static U8 OP_GetGPSOffPositionStat(GPSOffPositionStruct *pGPSOffPositionStruct)
{
    return pGPSOffPositionStruct->OffPositionFlag;
}

/*******************************************************************************
* 名称  : GetGPSOffPositionPossStat
* 描述  : 查询可能位置偏离状态
* 输入  : TRUE-可能偏离，FALSE-未偏离		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static U8 OP_GetGPSOffPositionPossStat(GPSOffPositionStruct *pGPSOffPositionStruct)
{
    return pGPSOffPositionStruct->OffPositionPossFlag;
}

/*******************************************************************************
* 名称  : OP_GetGPSOffPositionPossValid
* 描述  : 查询可能位置偏离状态是否有效
* 输入  : TRUE-有效，FALSE-无效		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static U8 OP_GetGPSOffPositionPossValid(GPSOffPositionStruct *pGPSOffPositionStruct)
{
    return pGPSOffPositionStruct->OffPositionPossValid;
}

/*******************************************************************************
* 名称  : OP1_GetGPSOffPositionPossValid
* 描述  : 查询可能位置偏离状态是否有效
* 输入  : TRUE-有效，FALSE-无效		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 OP1_GetGPSOffPositionPossValid(void)
{
    return OP_GetGPSOffPositionPossValid(&GPSOffPositionPara);
}

/*******************************************************************************
* 名称  : OP_GetGPSOffPositionValid
* 描述  : 查询位置偏离状态是否有效
* 输入  : TRUE-有效，FALSE-无效		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static U8 OP_GetGPSOffPositionValid(GPSOffPositionStruct *pGPSOffPositionStruct)
{
    return pGPSOffPositionStruct->OffPositionValid;
}

/*******************************************************************************
* 名称  : OP1_GetGPSOffPositionValid
* 描述  : 查询位置偏离状态是否有效
* 输入  : TRUE-有效，FALSE-无效		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 OP1_GetGPSOffPositionValid(void)
{
    return OP_GetGPSOffPositionValid(&GPSOffPositionPara);
}

/*******************************************************************************
* 名称  : GetGPSOffPositionStat
* 描述  : 查询位置偏离状态
* 输入  : TRUE-偏离，FALSE-未偏离		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 OP1_GetGPSOffPositionStat(void)
{
    return OP_GetGPSOffPositionStat(&GPSOffPositionPara);
}

/*******************************************************************************
* 名称  : GetGPSOffPositionPossStat
* 描述  : 查询可能位置偏离状态
* 输入  : TRUE-可能偏离，FALSE-未偏离		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 OP1_GetGPSOffPositionPossStat(void)
{
    return OP_GetGPSOffPositionPossStat(&GPSOffPositionPara);
}



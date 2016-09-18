/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: OffPosition.h
  Author: lq       Version : V1.00        Date: 2014.01.02
  Description:     位置偏离判断
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _OFFPOSITION_H
#define _OFFPOSITION_H

/* Private define ------------------------------------------------------------*/
/*lq 位置偏离判断相关宏定义 */
#define GPS_ON_POSITION_SAMPLE_NUM            5     //lq 在正常状态下，即未偏离状态下，判断偏离状态的采样点数
#define GPS_OFF_POSITION_SAMPLE_NUM           100   //lq 在偏离状态下，判断偏离状态的采样点数
#define GPS_OFF_POSITION_PERC_THRES           80    //lq 是否偏离的百分比阈值

/* Private typedef -----------------------------------------------------------*/

/*lq GPS位置偏离判断结构体*/
typedef struct {
    
    /*lq 用于GPS偏离判断 */
    float PositionNormSample[GPS_ON_POSITION_SAMPLE_NUM + 1][2];    //lq 正常工作时的采样点缓存
    float PositionOffSample[GPS_OFF_POSITION_SAMPLE_NUM + 1][2];    //lq 经纬度采样点的缓存，用于计算经纬度
    U8  PositionFlag[GPS_OFF_POSITION_SAMPLE_NUM + 1];              //lq 各个采样点对应的偏离状态；1-未偏离，0-偏离
    U16 PositionOffIndexIn;
    U16 PositionOffIndexOut;
    U16 PositionFlagIndexIn;
    U16 PositionFlagIndexOut;
    U16 InsideSum;
    float AvarDist;
    float AvarLon;
    float AvarLat;

    //U8  OnPositionFlag;                                             //lq 位置未偏离标志
    U8  OffPositionFlag;                                            //lq 位置偏离标志
    U8  OffPositionPossFlag;                                        //lq 位置可能偏离标志
    U8  OffPositionValid;                                           //lq 位置偏离标志是否有效
    U8  OffPositionPossValid;                                       //lq 位置可能偏离标志是否有效
                
}GPSOffPositionStruct;

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
extern GPSOffPositionStruct GPSOffPositionPara;

/* Public  function  ---------------------------------------------------------*/
/**
	@ingroup 电池
	@brief 电池电量检测
*/

#endif

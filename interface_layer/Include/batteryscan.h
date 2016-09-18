/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: batteryscan.h
  Author: lq       Version : V1.00        Date: 2013.09.02
  Description:     电量扫描
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _BATTERYSCAN_H
#define _BATTERYSCAN_H

/* Private define ------------------------------------------------------------*/
#define BATTERY_SCAN_AVGBUF_LEN		    15
#define BATTERY_SCAN_20MS_PERIOD	    4

#define BATTERY_SCAN_VOLT_CEIL          840         //lq 电压上限值（单位0.01v）
#define BATTERY_SCAN_VOLT_FLOOR         400         //lq 电压下限值（单位0.01v）
#define BATTERY_SCAN_VOLT_PREC          10          //lq 电压精度值（单位0.01v）

#define BATTERY_SCAN_VOLT_INTERVAL_NUM  ((BATTERY_SCAN_VOLT_CEIL - BATTERY_SCAN_VOLT_FLOOR) \
                                          / BATTERY_SCAN_VOLT_PREC) //lq 电压区间数

#define BATTERY_SCAN_VOLT_STEADY_THRES  10          //lq 电压持续稳定次数（单位20ms）

/* Private typedef -----------------------------------------------------------*/

/**
	@ingroup 电池
	@brief 电池电量检测结构体，包含电池电压平均等
*/
typedef struct {
	U32 TimeCnt;
	U16 TimePeriod;	
	
	U16 AvgBuf[BATTERY_SCAN_AVGBUF_LEN + 1];	        //   平滑滤波器
	U8  AvgBufIndex;
	U32 AvgSum;                                         //lq 采样值总和
	U16 AvgValue;                                       //lq 平均采样值

    S16 CurVoltIndex;                                   //lq 当前电压区间索引
    U16 CurVoltValue;                                   //lq 当前电压值（单位0.01v）
    S16 NewVoltIndex;                                   //lq 待确定电压区间索引
    U16 NewVoltCnt;                                     //lq 待确定电压区间计数
    
    //U8  VoltInterval[BATTERY_SCAN_VOLT_INTERVAL_NUM + 2];   //lq 电压区间
} BatteryScanStruct;

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
extern BatteryScanStruct BatteryScanPara;

/* Public  function  ---------------------------------------------------------*/
/**
	@ingroup 电池
	@brief 电池电量检测
*/

#endif

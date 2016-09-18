/*******************************************************************************
  Copyright (C),  2010,  712.Co., Ltd.
  File name:	interfunc.h
  Author: 李斌      Version: V1.00       Date: 2012.03.21 
  Description:  
  				主要功能： 
				1. 定义相关宏定义
				2. 变量声明
				3. 函数声明
				4. 定义结构体类型
				
  Others:         // 其它内容的说明
  Function List:  // 主要函数列表，每条记录应包括函数名及功能简要说明
  History:        // 修改历史记录列表，每条修改记录应包括修改日期、修改
                  // 者及修改内容简述  
    1. Date:
       Author:
       Modification:
*******************************************************************************/
#ifndef _DIGUNIT_H
#define _DIGUNIT_H
/* Private typedef -----------------------------------------------------------*/
#define DIGUNIT_MOD_IDLE	0
#define DIGUNIT_MOD_STEP1	1
#define DIGUNIT_MOD_STEP2	2
//#define DIGUNIT_MOD_STEP3	3
/* Private define ------------------------------------------------------------*/

/**
	@ingroup 数据处理
	@brief 数据发送总体控制结构体
*/
typedef struct {
	U8  ModSta;		//发射状态机状态
	U8  TranEnd;	//数据发送完成标志
} DigUnitStru;
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
extern DigUnitStru	DigUnitPara;
/* Public  function  ---------------------------------------------------------*/
extern void InitaDigDeal(void);
/**
	@ingroup 数据处理
	@brief 数据处理主函数
	@param 无
	@retval 无
*/
extern void DigDealMain(void);
/**
	@ingroup 数据处理
	@brief 数据调制发送状态机控制
	@param 无
	@retval 无
*/
extern void DigDealUnit(void);
/**
	@ingroup 数据处理
	@brief GMSK调制
	@param 无
	@retval 无 

	GMSK调制 信道选择以及1846开关控制
*/
extern U8 GMSKTran(void);
extern U8 GetDigUnitSta(void);
extern U8 GMSKTran_ITDMA(void);

/**
	@ingroup 休眠
	@brief 查看信号处理模块是否忙
	@param 无
	@retval TRUE 信号处理模块空闲
	@retval FALSE 信号处理模块忙
*/
extern U8 GetDigUnitSleepSta(void);
extern void RDA1846Ctrl(U8 Sta, U32 ulFreq, U8 cs);
extern void Ptl_Main(void);
#endif

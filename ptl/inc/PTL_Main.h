/*******************************************************************************
  Copyright (C), 2012, 712.Co.,Ltd.
  FileName: PTL_main.h
  Author: 王畅      Version : V1.00        Date: 2012.07.10
  Description:    实现协议栈消息组织与发送
  Version:         V1.00
  Function List:   // 主要函数及其功能
  History:         // 历史修改记录
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#ifndef PTL_MAIN_H
#define PTL_MAIN_H

#include "gentype.h"

/**
	@brief 4503状态机
*/
enum ctrlstate_4503
{
	INIT_4503,
	FIRST_GPS_4503,
	NORMAL_WORK_4503
};

/**
	@brief 4501状态机
*/
enum ctrlstate_4501
{
	INIT_4501,
	NORMAL_WORK_BURST,
	NORMAL_WORK_MSG,
    NORMAL_WORK_IDLE,
    //lq+
    CHANGE_SEND_INTERVAL_4501
    //lq~
};

/*lq 变量声明*/
extern U8 state_4501;

/**
	@brief 协议栈初始化
	@retval 0 完成	
*/
U16 Ptl_Init(void);

/**
	@ingroup 协议栈
	@brief 4503部分主函数
	@param 无
	@retval 1 成功
	@retval 0 失败
	
	4503模式消息发送控制函数	
*/
U16 AISmain_4503(void);
/**
	@ingroup 协议栈
	@brief 4501部分主函数
	@param 无
	@retval 1 成功
	@retval 0 失败
	
	4501模式消息发送控制函数	
*/
U16 AISmain_4501(void);
/**
	@param time1 输入时间1
	@param time2 输入时间2
	@param timeout 时间差
	@retval TRUE 超时
	@retval FALSE 未超时
*/
U16 IsTimeout(U16 time1, U16 time2, U16 timeout);

/**
	@ingroup 协议栈
	@brief 4503部分主函数
	@param 无
	@retval 无
	
	协议栈主函数	
*/
void Ptl_Main(void);

extern U16 Is_4503_Idle(void);
extern U16 sleep_4503(void);
extern U16 wakeup_4503(void);

extern U16 set_4501_interval(U16 speed);

extern U8 state_4503;

#endif

/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: main.c
  Author: 王伟       Version : V1.00        Date: 2011.09.23
  Description:    项目中挂载UCOS操作系统
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			1．函数名：Main
    	   函数功能：控制ARM整个程序的架构
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include "config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define OS_INIT_TASK_STACK_SIZE		128			/* 初始化任务堆栈大小 	*/ 
/* Private variables ---------------------------------------------------------*/
OS_STK	InitTaskStk[OS_INIT_TASK_STACK_SIZE];	/* 初始化任务堆栈 		*/
/* Private  function  --------------------------------------------------------*/
extern void  application_init (void);
extern void  init_task_core(void *pdata);
/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/

/*******************************************************************************
* 名称  : application_init
* 描述  : 基本应用初始化
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void application_init(void)
{
	DCB_init();	/* 注册表初始化 */
	
	//********中间层模块和任务初始化********
	//串口数据处理模块初始化 
	comm1_init();
	comm1_handle = DCB_open(COMM1_ID, 0);

	//独立IO输出外设模块初始化
	IO_set_init();
	IO_set_handle = DCB_open(IO_SET_ID,0);

	//液晶显示模块初始化
	lcd_interface_init();
	LCD_handle = DCB_open(LCD_ID,0);

	//外设检测与自检模块初始化
	mach_dete_init();
	mach_dete_handle = DCB_open(MACH_DETE_ID, 0);

	//铃声管理模块初始化
	bell_init();
	bell_handle = DCB_open(BELL_ID, 0);

	//SCT3252控制与通信模块初始化
	SCT3252_init();
	SCT3252_handle = DCB_open(SCT3252_ID, 0);

	//Flash控制模块
	SpiFlash_Init();
	Flash_handle = DCB_open(FLASH_ID, 0);

	//录音控制模块
	record_interface_init();
	Record_handle = DCB_open(RECORD_ID, 0);

	//马达控制
	moto_interface_init();
	Moto_handle = DCB_open(MOTO_ID, 0);
}


/*******************************************************************************
* 名称  : main
* 描述  : 入口程序
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
int main(void)
{
	#if (OS_TASK_NAME_SIZE >= 16) 
		INT8U err;
	#endif

	//初始化C库
	setlocale(LC_ALL,"C");

  	Target_Init();			/* 目标基本初始化 */

  	OSInit();				/* 初始化OS 	  */

  	Tmr_TickInit();			/* 初始化OS Tick  */
  
  	OSTaskCreateExt(init_task_core, (void *)0, (OS_STK *)&InitTaskStk[OS_INIT_TASK_STACK_SIZE - 1],
					OS_TASK_INIT_PRIO,
					OS_TASK_INIT_PRIO,
					(OS_STK *)&InitTaskStk[0],
					OS_INIT_TASK_STACK_SIZE,
					(void *)0,
					OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);	
					
	#if (OS_TASK_NAME_SIZE >= 16)
		OSTaskNameSet(OS_TASK_IDLE_PRIO, (INT8U *)"Idle task", &err);
		OSTaskNameSet(OS_TASK_INIT_PRIO, (INT8U *)"Init task", &err);
	#endif

	OSStart();				/* 启动多任务环境 */
	 
  	return(0);
}

/***************************************************************************************
** 函数名称: init_task_core
** 功能描述: 初始化任务核心函数：(1) 初始化应用  (2) 初始化操作系统组件
** 参    数: *pdata
** 返 回 值: None       
** 作　  者:  
** 日  　期:  
**--------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
static void init_task_core(void *pdata)
{
	pdata = pdata;				/* 防止编译器警告 				*/
	 
	//create_os_task(); 			/* 创建应用中的大多数任务 		*/

	create_power_on_task();		/* 创建应用中的开机任务 		*/

	create_task_status();		/* 设置初始化状态需要挂起的任务 */
	
	create_os_semphore();		/* 创建应用中的大多数信号量 	*/

	create_os_mutex();			/* 创建应用中的大多数互斥量 	*/

	create_os_mailbox();		/* 创建应用中的大多数邮箱消息 	*/

	create_os_queue();			/* 创建应用中的大多数队列消息 	*/

	create_os_timer();			/* 创建应用中的大多定时器 		*/
	
	create_os_group();			/* 创建应用中的大多通用事件组 	*/

	mem_init();					/* 创建应用中的大多数内存分区 	*/	
	 		
	application_init();			/* 初始化应用中参数				*/
//	Wdg_Init();					/* 窗口看门狗初始化 			*/		
		
	while(1)
	{
		UartDebugMsg(ARM_DEBUG_MSG, 0, "/----------------------------------------");	
		UartDebugMsg(ARM_DEBUG_MSG, 0, "TM208 version : V%d.%d.%.2d", 
					(ARM_STATE&0xF000)>>24,
					(ARM_STATE&0x0F00)>>16,
					(ARM_STATE&0x00FF));

		OSTaskSuspend(OS_PRIO_SELF);	/* 挂起初始化任务 		*/
	}	
}


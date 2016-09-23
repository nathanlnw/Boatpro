/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: parastorage.h
  Author: lq       Version : V1.00        Date: 2013.12.05
  Description:     EEPROM参数宏定义
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
    <author>    <time>      <version >  <desc>
      lq        20131206                参数存储格式中省去了1B的间隔
      lq        20131230                修改EEPROM中参数的存储方式
*******************************************************************************/

#ifndef _PARASTORAGE_H
#define _PARASTORAGE_H
/* Includes ------------------------------------------------------------------*/
#include "config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/*lq
EEPROM中单个参数存储方式：
++++++++++REGION_U32:++++++++++++++++++++
  4Byte   1Byte   
参数数值+ 校验码

参数数值字段存放类型：小端模式的数值类型
校验码字段存放类型：数值类型
无参数字符实际长度字段
++++++++++REGION_U16:++++++++++++++++++++
  2Byte   1Byte   
参数数值+ 校验码

参数数值字段存放类型：小端模式的数值类型
校验码字段存放类型：数值类型
无参数字符实际长度字段
++++++++++REGION_U8:++++++++++++++++++++
  1Byte   1Byte   
参数数值+ 校验码

参数数值字段存放类型：小端模式的数值类型
校验码字段存放类型：数值类型
无参数字符实际长度字段
++++++++++REGION_CHAR:++++++++++++++++++++
       1Byte       maxlenByte    1Byte   
参数字符实际长度 + 实际参数值 +  校验码

参数字符实际长度字段存放类型：数值类型
实际参数值字段存放类型：ASCII
校验码字段存放类型：数值类型

当参数的实际长度小于最大长度时，“实际长度”字段、“参数”字段及“校验码”字段
是连续存放的，中间无空余字节
*/

/*lq ======================EEPROM参数存储地址宏定义========================== */
/*-----------------REGION_U32--------------------*/
#define PARA_REGION_U32_BASE_ADDR       2                                       //lq EEPROM的起始2个字节不用，不稳定？
#define PARA_WORKTIME_ADDR              (PARA_REGION_U32_BASE_ADDR + 0)        //工作总时长
#define PARA_WORKTIME_LAST_ADDR         (PARA_REGION_U32_BASE_ADDR + 5)        //上次工作总时长
#define PARA_SAILING_DISTINCE_ADDR      (PARA_REGION_U32_BASE_ADDR + 10)       //航行总距离（单位：米）
/*-----------------REGION_U16--------------------*/
#define PARA_REGION_U16_BASE_ADDR       (PARA_REGION_U32_BASE_ADDR + 15)        // address = 17
#define PARA_PERSONS_ADDR				(PARA_REGION_U16_BASE_ADDR + 0)			//船载人数
#define PARA_WDG_RESET_CNT_ADDR	        (PARA_REGION_U8_BASE_ADDR + 4)         //从本次正常开机到现在，看门狗复位次数
#define PARA_LAST_WDG_RESET_CNT_ADDR    (PARA_REGION_U8_BASE_ADDR + 7)         //从上次正常开机到本次正常开机，看门狗复位次数
#define PARA_NORMAL_POWER_ON_CNT_ADDR		(PARA_REGION_U8_BASE_ADDR + 10)		//正常开机次数
/*-----------------REGION_U8---------------------*/
#define PARA_REGION_U8_BASE_ADDR        (PARA_REGION_U16_BASE_ADDR + 13)         // address = 30
#define PARA_ETA_MONTH_ADDR             (PARA_REGION_U8_BASE_ADDR + 0)          //估计到达时间，月
#define PARA_NAVIGATION_STATUS_ADDR     (PARA_REGION_U8_BASE_ADDR + 2)          //导航状态
#define PARA_REGION_APP_FLAG_ADDR       (PARA_REGION_U8_BASE_ADDR + 4)         //区域应用标志
#define PARA_POWER_LEVEL_ADDR           (PARA_REGION_U8_BASE_ADDR + 6)         //功率水平
#define PARA_ETA_DAY_ADDR               (PARA_REGION_U8_BASE_ADDR + 8)         //估计到达时间，日
#define PARA_ETA_HOUR_ADDR              (PARA_REGION_U8_BASE_ADDR + 10)         //估计到达时间，时
#define PARA_ETA_MINUTE_ADDR            (PARA_REGION_U8_BASE_ADDR + 12)         //估计到达时间，分
#define PARA_OPEN_CLOSE_TIME_INDEX_ADDR (PARA_REGION_U8_BASE_ADDR + 14)         //开关机时间索引		    
/*-----------------REGION_CHAR-------------------*/
#define PARA_REGION_CHAR_BASE_ADDR      (PARA_REGION_U8_BASE_ADDR + 16)         // address = 46
#define PARA_TIME_OPEN_0_ADDR 		    (PARA_REGION_CHAR_BASE_ADDR + 0)       //开机时间
#define PARA_TIME_OPEN_1_ADDR 		    (PARA_REGION_CHAR_BASE_ADDR + 14)      //开机时间
#define PARA_TIME_OPEN_2_ADDR 		    (PARA_REGION_CHAR_BASE_ADDR + 28)      //开机时间
#define PARA_TIME_CLOSE_0_ADDR 		    (PARA_REGION_CHAR_BASE_ADDR + 42)      //关机时间
#define PARA_TIME_CLOSE_1_ADDR 		    (PARA_REGION_CHAR_BASE_ADDR + 46)      //关机时间
#define PARA_TIME_CLOSE_2_ADDR 		    (PARA_REGION_CHAR_BASE_ADDR + 70)      //关机时间
#define PARA_GPS_FIRST_TIME_ADDR        (PARA_REGION_U32_BASE_ADDR +  84)      //GPS第一次定位时间
/*剩余可用字节数112B*/

/*lq ====================EEPROM参数存储保护长度宏定义======================== */
/*
 *  REGION_CHAR分区存储参数时会用到MAXLEN，
 *  REGION_U32、REGION_U16及REGION_U8分区存储参数时不使用（参数存储的长度固定），
 *  在参数的配置报文中会用到
 */
/*-----------------REGION_U32--------------------*/
#define PARA_WORKTIME_MAXLEN            5                                       //上次开机之前工作总时长
#define PARA_WORKTIME_LAST_MAXLEN       5                                       //上次开机之后工作时长
#define PARA_SAILING_DISTINCE_MAXLEN	5										//航行总距离（单位：米）
/*-----------------REGION_U16--------------------*/
#define PARA_PERSONS_MAXLEN			    4			                            //船载人数
#define PARA_WDG_RESET_CNT_MAXLEN	    3                                       //看门狗复位次数 
#define PARA_LAST_WDG_RESET_CNT_MAXLEN  3                                       //从上次正常开机到本次正常开机，看门狗复位次数
#define PARA_NORMAL_POWER_ON_CNT_MAXLEN	3 										//正常开机次数
/*-----------------REGION_U8---------------------*/
#define PARA_ETA_MONTH_MAXLEN           2                                       //估计到达时间，月
#define PARA_NAVIGATION_STATUS_MAXLEN   2                                       //导航状态
#define PARA_REGION_APP_FLAG_MAXLEN     2                                       //区域应用标志
#define PARA_POWER_LEVEL_MAXLEN         1                                       //功率水平
#define PARA_ETA_DAY_MAXLEN             2                                       //估计到达时间，日
#define PARA_ETA_HOUR_MAXLEN            2                                       //估计到达时间，时
#define PARA_ETA_MINUTE_MAXLEN          2                                       //估计到达时间，分
#define PARA_OPEN_CLOSE_TIME_INDEX_MAXLEN   2                                   //开关机时间索引
/*-----------------REGION_CHAR-------------------*/
#define PARA_TIME_OPEN_MAXLEN 		    12                                      //开机时间
#define PARA_TIME_CLOSE_MAXLEN 		    12                                      //关机时间
#define PARA_GPS_FIRST_TIME_MAXLEN      12      								//GPS第一次定位时间

/*lq ===================EEPROM参数存储类型宏定义============================= */
/*-----------------REGION_U32--------------------*/
#define PARA_WORKTIME_TYPE              0                                       //上次开机之前工作总时长
#define PARA_WORKTIME_LAST_TYPE         1                                       //上次开机之后工作时长
#define PARA_SAILING_DISTINCE_TYPE   	2										//航行总距离（单位：米）
/*-----------------REGION_U16--------------------*/
#define PARA_PERSONS_TYPE			    3			                            //船载人数
#define PARA_WDG_RESET_CNT_TYPE	        4                                       //看门狗复位次数 
#define PARA_LAST_WDG_RESET_CNT_TYPE    5                                       //从上次正常开机到本次正常开机，看门狗复位次数
#define PARA_NORMAL_POWER_ON_CNT_TYPE	6								   		//正常开机次数
/*-----------------REGION_U8---------------------*/
#define PARA_ETA_MONTH_TYPE             7                                      //估计到达时间，月
#define PARA_NAVIGATION_STATUS_TYPE     8                                      //导航状态
#define PARA_REGION_APP_FLAG_TYPE       9                                      //区域应用标志
#define PARA_POWER_LEVEL_TYPE           10                                     //功率水平
#define PARA_ETA_DAY_TYPE               11                                     //估计到达时间，日
#define PARA_ETA_HOUR_TYPE              12                                     //估计到达时间，时
#define PARA_ETA_MINUTE_TYPE            13                                     //估计到达时间，分
#define PARA_OPEN_CLOSE_TIME_INDEX_TYPE 14                                     //开关机时间索引
/*-----------------REGION_CHAR-------------------*/
#define PARA_TIME_OPEN_0_TYPE 		    15                                      //开机时间
#define PARA_TIME_OPEN_1_TYPE 		    16                                      //开机时间
#define PARA_TIME_OPEN_2_TYPE 		    17                                      //开机时间
#define PARA_TIME_CLOSE_0_TYPE 		    18                                      //关机时间
#define PARA_TIME_CLOSE_1_TYPE 		    19                                      //关机时间
#define PARA_TIME_CLOSE_2_TYPE 		    20                                      //关机时间
#define PARA_GPS_FIRST_TIME_TYPE        21      								//GPS第一次定位时间

/*lq EEPROM参数总个数*/
#define PARA_INDEXBUF_LEN			    22
#define PARA_TIME_OPEN_CLOSE_TOTAL 		3                                       //开机/关机时间记录总数

/*lq ==============EEPROM参数上（ceil）/下（floor）限宏定义================== */
/*-----------------REGION_U32--------------------*/
#define PARA_WORKTIME_FLOOR             0//0
#define PARA_WORKTIME_CEIL              0xFFFFFFFF								// 4294967295
#define PARA_WORKTIME_LAST_FLOOR        0
#define PARA_WORKTIME_LAST_CEIL         0xFFFFFFFF								// 4294967295
#define PARA_SAILING_DISTINCE_FLOOR	    0										//航行总距离（单位：米）
#define PARA_SAILING_DISTINCE_CELL	    0xFFFFFFFF								//航行总距离（单位：米）
/*-----------------REGION_U16--------------------*/
#define PARA_PERSONS_FLOOR			    (AIS_INFO_PERSONS_MIN)	                //船载人数
#define PARA_PERSONS_CEIL			    (AIS_INFO_PERSONS_MAX)	                //船载人数
#define PARA_WDG_RESET_CNT_FLOOR	    0                                       //看门狗复位次数 
#define PARA_WDG_RESET_CNT_CEIL         0xFFFF                                   //看门狗复位次数
 																				//0xFFFF
#define PARA_LAST_WDG_RESET_CNT_FLOOR   0                                       //从上次正常开机到本次正常开机，看门狗复位次数
#define PARA_LAST_WDG_RESET_CNT_CEIL    0xFFFF                                   //从上次正常开机到本次正常开机，看门狗复位次数
#define PARA_NORMAL_POWER_ON_CNT_FLOOR	0									 	//正常开机次数
#define PARA_NORMAL_POWER_ON_CNT_CEIL	0xFFFF								 	//正常开机次数
/*-----------------REGION_U8---------------------*/
#define PARA_ETA_MONTH_FLOOR            (AIS_INFO_ETA_MONTH_MIN)                //估计到达时间，月
#define PARA_ETA_MONTH_CEIL             (AIS_INFO_ETA_MONTH_MAX)                //估计到达时间，月
#define PARA_NAVIGATION_STATUS_FLOOR    (AIS_INFO_NAVIGATION_STATUS_MIN)        //导航状态
#define PARA_NAVIGATION_STATUS_CEIL     (AIS_INFO_NAVIGATION_STATUS_MAX)        //导航状态
#define PARA_REGION_APP_FLAG_FLOOR      (AIS_INFO_REGION_APP_FLAG_MIN)          //区域应用标志
#define PARA_REGION_APP_FLAG_CEIL       (AIS_INFO_REGION_APP_FLAG_MAX)          //区域应用标志
#define PARA_POWER_LEVEL_FLOOR          0                                       //功率水平
#define PARA_POWER_LEVEL_CEIL           9                                       //功率水平
#define PARA_ETA_DAY_FLOOR              (AIS_INFO_ETA_DAY_MIN)                  //估计到达时间，日
#define PARA_ETA_DAY_CEIL               (AIS_INFO_ETA_DAY_MAX)                  //估计到达时间，日
#define PARA_ETA_HOUR_FLOOR             (AIS_INFO_ETA_HOUR_MIN)                 //估计到达时间，时
#define PARA_ETA_HOUR_CEIL              (AIS_INFO_ETA_HOUR_MAX)                 //估计到达时间，时
#define PARA_ETA_MINUTE_FLOOR           (AIS_INFO_ETA_MINUTE_MIN)               //估计到达时间，分
#define PARA_ETA_MINUTE_CEIL            (AIS_INFO_ETA_MINUTE_MAX)               //估计到达时间，分  
#define PARA_OPEN_CLOSE_TIME_INDEX_FLOOR    0
#define PARA_OPEN_CLOSE_TIME_INDEX_CEIL     (PARA_TIME_OPEN_TOTAL)

/*-----------------REGION_CHAR-------------------*/
//lq 非数值类型参数无上/下限值

/*lq =======================EEPROM参数默认值宏定义=============================== */
/*-----------------REGION_U32--------------------*/
#define PARA_WORKTIME_DEFAULT           (DEV_INFO_WORKTIME_DEFAULT)             //工作总时长
#define PARA_WORKTIME_LAST_DEFAULT      0
#define PARA_SAILING_DISTINCE_DEFAULT   0										//航行总距离（单位：米）
/*-----------------REGION_U16--------------------*/
#define PARA_PERSONS_DEFAULT			(AIS_INFO_PERSONS_DEFAULT)	            //船载人数
#define PARA_WDG_RESET_CNT_DEFAULT	    0                                       //看门狗复位次数 
#define PARA_LAST_WDG_RESET_CNT_DEFAULT 0                                       //从上次正常开机到本次正常开机，看门狗复位次数
#define PARA_NORMAL_POWER_ON_CNT_DEFAULT	0									//正常开机次数
/*-----------------REGION_U8--------------------*/
#define PARA_ETA_MONTH_DEFAULT          (AIS_INFO_ETA_MONTH_DEFAULT)            //估计到达时间，月
#define PARA_NAVIGATION_STATUS_DEFAULT  (AIS_INFO_NAVIGATION_STATUS_DEFAULT)    //导航状态
#define PARA_REGION_APP_FLAG_DEFAULT    (AIS_INFO_REGION_APP_FLAG_DEFAULT)      //区域应用标志
#define PARA_POWER_LEVEL_DEFAULT        0                                       //功率水平,默认值0-12.5we
#define PARA_ETA_DAY_DEFAULT            (AIS_INFO_ETA_DAY_DEFAULT)              //估计到达时间，日
#define PARA_ETA_HOUR_DEFAULT           (AIS_INFO_ETA_HOUR_DEFAULT)             //估计到达时间，时
#define PARA_ETA_MINUTE_DEFAULT         (AIS_INFO_ETA_MINUTE_DEFAULT)           //估计到达时间，分
#define PARA_OPEN_CLOSE_TIME_INDEX_DEFAULT  0
/*-----------------REGION_CHAR-------------------*/
#define PARA_TIME_OPEN_DEFAULT 		    "000000246060"                          //开机时间,具有特定格式
#define PARA_TIME_CLOSE_DEFAULT 	    "000000246060"	                        //关机时间,具有特定格式
#define PARA_GPS_FIRST_TIME_DEFAULT     "000000246060"      					//GPS第一次定位时间,具有特定格式
                    
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/

/**
	@ingroup 参数存取
	@brief 参数存储
	@param ucPara_Type 存的参数类型
	@param pPara 参数指针
	@param ucLen 参数长度
	@retval TRUE 存储成功
	@retval FALSE 存储失败

	参数存储至EEPROM
*/
U8 Para_Save(U8 ucPara_Type, U8 *pPara, U8 ucLen);

/**
	@ingroup 参数存取
	@brief 参数读取
	@param ucPara_Type 读的参数类型
	@param pPara 参数指针
	@retval TRUE 存储成功
	@retval FALSE 存储失败

	参数存储至EEPROM
*/
U8 Para_Read(U8 ucPara_Type, U8 *pPara);
void IniEEPROM_Para(void);



//lnw  add
extern U8 Para_OpenTimeSave(U8 *pPara, U16 index);
extern U8 Para_CloseTimeSave(U8 *pPara, U16 index);


#endif

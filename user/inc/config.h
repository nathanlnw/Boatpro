/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: config.h
  Author: lq       Version : V1.00        Date: 2013.10.31
  Description:     功能配置头文件，编译控制
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _CONFIG_H
#define _CONFIG_H

/* Includes ------------------------------------------------------------------*/

/* Public  macro -------------------------------------------------------------*/
/*lq 需在使用下列宏的文件中加入该头文件*/
/*lq 0-工程中所有与调试相关的功能全部关闭，1-工程中与调试相关的功能开启，各个具体的调试项需单独设置*/
#define CONFIG_DEBUG                1

/*lq 0-工程中所有的状态报文输出功能全部关闭，1-工程中与状态报文输出相关的功能开启，各个具体的项可能需单独设置*/
#define CONFIG_STAT_MSG_PRINT       1

/*lq 0-功能关闭，1-功能开启，若开启该功能需关闭冲突项*/
#define CONFIG_PARASTORAGE_AISSD    1   //静态数据参数，对应报文“$AISSD,A--A,B--B,C,D,E,F,G,H*hh”

/*lq 1-使用闰秒修正后的GPS时间进行同步，0-使用GPS原始时间进行同步*/
#define CONFIG_GPS_SYNCH_LEAP       0   //

/*lq 1-功放栅压使用DA控制，0-功放栅压不使用DA控制*/ 
#define CONFIG_PA_GRID_DA_CTRL      1

/*lq 1-记录电池检测模拟工作时长，0-不记录电池检测模拟工作时长*/
#define CONFIG_PARASTORAGE_BTT      0   //lq 若开启，需在EEPROM中分配地址

/*lq 1-启用GPS省电控制，0-禁用GPS省电控制*/
#define CONFIG_GPS_SAVE             0   //

/*lq 1-18号消息的SOG指示电池电量百分比，0-18号消息的SOG指示航速*/
#define CONFIG_SOG_FOR_BAT_VOLUME   0

/*lq 1-18号消息的COG指示环境温度，0-18号消息的COG指示航向*/
#define CONFIG_COG_FOR_TEMPERATURE  0

/*lq 1-SART MMSI格式校验使能，0-SART MMSI格式校验失能*/
#define CONFIG_MMSI_SARTFORM_CHECK  0     

/*lq 1-超低电量自动关机，0-超低电量不自动关机*/ 
#define CONFIG_BATTERY_CTRL_AUTOPOWERDOWN   0 

/*lq 0-百分比法，1-平均位置坐标法，2-平均距离法 */
#define CONFIG_OFF_POSITON_ALGORITHM        0  

/*lq 0-使用RFM（一体机可以解析），1-使用IFM（一体机无法解析） */
#define CONFIG_MSG_6_DAC_IFM        0  

/*lq 0-不进行小时分钟同步，1-进行小时分钟同步 */
#define CONFIG_GPS_SYNCH_HOUR_MIN   0  

/*lq 0-ADC1功能关闭，1-ADC1功能开启 */
#define CONFIG_ADC1                 0

/*lq 0-不进行电池电量检测及控制，1-进行电池电量检测及控制 */
#define CONFIG_BAT_MONITOR          0

/*lq 0-不进行温度检测，1-进行温度检测 */
#define CONFIG_TEMPERATURE_MONITOR  0

/*lq 0-按键功能关闭，1-按键功能开启 */
#define CONFIG_KEY                  0

/*lq 0-关机功能关闭，1-关机功能开启 */
#define CONFIG_POWERDOWN            0

/*lq 0-调试输出功能关闭，1-调试输出功能开启 */
#define CONFIG_DEBUG_PRINT_EN       1

/*lq 0-GPS模块的uBlox协议通信功能关闭，1-GPS模块的uBlox协议通信功能开启 */
#define CONFIG_GPS_UBX_EN           1

/*lq 0-AtoN的GPS位置偏移判断功能关闭，1-AtoN的GPS位置偏移判断功能开启 */
#define CONFIG_GPS_OFF_POSITION_EN  0

/*lq 0-电源供电电压自检功能关闭，1-电源供电电压自检功能开启 */
#define CONFIG_SLFCHK_POWER_SUPPLY_VOLT_EN      0

/* 0-SD卡模块功能关闭，1-SD卡模块功能开启 */
#define CONFIG_SD_CARD_MODULE_EN      0


/*  lnw    1.  2次开发 增加语音对讲机提示功能 */
#define CONFIG_PTT_EN          1

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function  ---------------------------------------------------------*/

/* Public  variables ---------------------------------------------------------*/

/* Public  function  ---------------------------------------------------------*/

#endif

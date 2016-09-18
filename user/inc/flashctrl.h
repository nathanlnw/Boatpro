/*******************************************************************************
  Copyright (C), 2010, 712.Co.,Ltd.
  File name:	flashctrl.h
  Author: 张嘉旺      Version: V1.00       Date: 2015.12.21 
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
#ifndef __FLASHCTRL_H__
#define __FLASHCTRL_H__

/* Includes ------------------------------------------------------------------*/
#include "Include.h"
#include "def.h"
#include "config.h"
#include "AisInfo.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
	REGION_U32=0,
	REGION_U16,
	REGION_U8,
	REGION_CHAR
}Para_Type;

/* Private define ------------------------------------------------------------*/

/* ==============STM32的FLASH相关定义================== */
#define STM32_FLASH_PAGES      						256
#define STM32_FLASH_U32_BASE						(STM32_FLASH_BASE+200*0x00000800)
#define STM32_FLASH_U16_BASE						(STM32_FLASH_BASE+201*0x00000800)
#define STM32_FLASH_U8_BASE							(STM32_FLASH_BASE+202*0x00000800)
#define STM32_FLASH_CHAR_BASE						(STM32_FLASH_BASE+203*0x00000800)
#define STM32_FLASH_PAGE_DATA_MAXLEN				100

/*
FLASH中单个参数存储方式：
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

/* ======================FLASH参数存储地址宏定义========================== */
/*-----------------REGION_U32--------------------*/                                      
#define STM32_FLASH_MMSI_ADDR				    (STM32_FLASH_U32_BASE + 0)         	//MMSI，
#define STM32_FLASH_MMSI_DEST_ADDR             	(STM32_FLASH_U32_BASE + 6)         	//目的MMSI
#define STM32_FLASH_MMSI_BACKUP_ADDR		    (STM32_FLASH_U32_BASE + 12)        	//MMSI备份存储
#define STM32_FLASH_MMSI_DEST_BACKUP_ADDR      	(STM32_FLASH_U32_BASE + 18)        	//目的MMSI备份存储
#define STM32_FLASH_SERIAL_ADDR			    	(STM32_FLASH_U32_BASE + 24)			//整机序列号存储地址

/*-----------------REGION_U16--------------------*/
#define STM32_FLASH_SHIPA_ADDR				    (STM32_FLASH_U16_BASE + 0)			//船舶大小A点位置存储地址
#define STM32_FLASH_SHIPB_ADDR				    (STM32_FLASH_U16_BASE + 4)			//船舶大小B点位置存储地址

/*-----------------REGION_U8---------------------*/
#define STM32_FLASH_MAX_DRAUGHT_ADDR           	(STM32_FLASH_U8_BASE + 0)          	//最大吃水深度
#define STM32_FLASH_SHIPC_ADDR				    (STM32_FLASH_U8_BASE + 2)			//船舶大小C点位置存储地址
#define STM32_FLASH_SHIPD_ADDR				    (STM32_FLASH_U8_BASE + 4)			//船舶大小D点位置存储地址
#define STM32_FLASH_EPFS_TYPE_ADDR             	(STM32_FLASH_U8_BASE + 6)         	//电子定位系统类型
#define STM32_FLASH_SHIP_TYPE_ADDR             	(STM32_FLASH_U8_BASE + 8)         	//船舶类型
#define STM32_FLASH_MODAMP_ADDR			    	(STM32_FLASH_U8_BASE + 10)			//调制幅度存储地址
#define STM32_FLASH_FREQADJ_ADDR			    (STM32_FLASH_U8_BASE + 12)			//发射频率准确度存储地址
#define STM32_FLASH_RF_VOLT_ADJUST_ADDR        	(STM32_FLASH_U8_BASE + 14)         	//栅压DA控制调整
#define STM32_FLASH_DTE_ADDR                   	(STM32_FLASH_U8_BASE + 16)         	//DTE
#define STM32_FLASH_AIS_CLASS_ADDR             	(STM32_FLASH_U8_BASE + 18)         	//AIS类别

/*-----------------REGION_CHAR-------------------*/
#define STM32_FLASH_DESTINATION_ADDR           	(STM32_FLASH_CHAR_BASE + 0)        	//目的地	   20+1+1
#define STM32_FLASH_VENDID_ADDR			    	(STM32_FLASH_CHAR_BASE + 22)		//厂商信息	   7+1+1
#define STM32_FLASH_SHIP_NAME_ADDR             	(STM32_FLASH_CHAR_BASE + 32)       	//船舶名称	   20+1+1
#define STM32_FLASH_HWVERSION_ADDR             	(STM32_FLASH_CHAR_BASE + 54)       	//硬件版本号   5+1+1

/* ====================FLASH参数存储保护长度宏定义======================== */
/*
 *  REGION_CHAR分区存储参数时会用到MAXLEN，
 *  REGION_U32、REGION_U16及REGION_U8分区存储参数时不使用（参数存储的长度固定），
 *  在参数的配置报文中会用到
 */
/*-----------------REGION_U32--------------------*/
#define STM32_FLASH_MMSI_MAXLEN			    		9 			                            //MMSI存储最大长度
#define STM32_FLASH_MMSI_DEST_MAXLEN           		9                                       //目的MMSI
#define STM32_FLASH_MMSI_BACKUP_MAXLEN		    	9                                       //mmsi备份存储长度
#define STM32_FLASH_MMSI_DEST_BACKUP_MAXLEN    		9                                       //目的mmsi备份存储长度
#define STM32_FLASH_SERIAL_MAXLEN			    	8			                            //整机序列号存储最大长度
/*-----------------REGION_U16--------------------*/
#define STM32_FLASH_SHIPA_MAXLEN			    	3			                            //船舶大小A点位置存储最大长度
#define STM32_FLASH_SHIPB_MAXLEN			    	3			                            //船舶大小B点位置存储最大长度
/*-----------------REGION_U8---------------------*/
#define STM32_FLASH_MAX_DRAUGHT_MAXLEN         		3                                       //最大吃水深度
#define STM32_FLASH_SHIPC_MAXLEN			    	2			                            //船舶大小C点位置存储最大长度
#define STM32_FLASH_SHIPD_MAXLEN			    	2			                            //船舶大小D点位置存储最大长度
#define STM32_FLASH_EPFS_TYPE_MAXLEN           		2                                       //电子定位系统类型
#define STM32_FLASH_SHIP_TYPE_MAXLEN           		3                                       //船舶类型
#define STM32_FLASH_MODAMP_MAXLEN			    	2			                            //调制幅度存储最大长度
#define STM32_FLASH_FREQADJ_MAXLEN			    	2			                            //发射功率存储最大长度
#define STM32_FLASH_RF_VOLT_ADJUST_MAXLEN      		2                                       //栅压DA控制调整
#define STM32_FLASH_DTE_MAXLEN                 		1                                       //DTE
#define STM32_FLASH_AIS_CLASS_MAXLEN           		1                                       //AIS类别
/*-----------------REGION_CHAR-------------------*/
#define STM32_FLASH_DESTINATION_MAXLEN         		20                                      //目的地
#define STM32_FLASH_VENDID_MAXLEN			    	7			                            //厂商信息
#define STM32_FLASH_SHIP_NAME_MAXLEN 		    	20                                      //船舶名称
#define STM32_FLASH_HWVERSION_MAXLEN           		5                                       //硬件版本号

/* ===================FLASH参数存储类型宏定义============================= */
/*-----------------REGION_U32--------------------*/
#define STM32_FLASH_MMSI_TYPE			        	0 			                            //MMSI存储最大长度
#define STM32_FLASH_MMSI_DEST_TYPE             		1                                       //目的MMSI
#define STM32_FLASH_MMSI_BACKUP_TYPE		    	2                                       //mmsi备份存储长度
#define STM32_FLASH_MMSI_DEST_BACKUP_TYPE      		3                                       //目的mmsi备份存储长度
#define STM32_FLASH_SERIAL_TYPE			    		4			                            //整机序列号存储最大长度
/*-----------------REGION_U16--------------------*/
#define STM32_FLASH_SHIPA_TYPE			        	5			                            //船舶大小A点位置存储最大长度
#define STM32_FLASH_SHIPB_TYPE			        	6			                            //船舶大小B点位置存储最大长度
/*-----------------REGION_U8---------------------*/
#define STM32_FLASH_MAX_DRAUGHT_TYPE           		7                                      //最大吃水深度
#define STM32_FLASH_SHIPC_TYPE			        	8			                            //船舶大小C点位置存储最大长度
#define STM32_FLASH_SHIPD_TYPE			        	9			                            //船舶大小D点位置存储最大长度
#define STM32_FLASH_EPFS_TYPE_TYPE             		10                                      //电子定位系统类型
#define STM32_FLASH_SHIP_TYPE_TYPE             		11                                      //船舶类型
#define STM32_FLASH_MODAMP_TYPE			    		12			                            //调制幅度存储最大长度
#define STM32_FLASH_FREQADJ_TYPE			    	13			                            //发射功率存储最大长度
#define STM32_FLASH_RF_VOLT_ADJUST_TYPE        		14                                      //栅压DA控制调整
#define STM32_FLASH_DTE_TYPE                   		15                                      //DTE
#define STM32_FLASH_AIS_CLASS_TYPE             		16                                      //AIS类别
/*-----------------REGION_CHAR-------------------*/
#define STM32_FLASH_DESTINATION_TYPE           		17                                      //目的地
#define STM32_FLASH_VENDID_TYPE			    		18			                            //厂商信息
#define STM32_FLASH_SHIP_NAME_TYPE 		    		19                                      //船舶名称
#define STM32_FLASH_HWVERSION_TYPE             		20                                      //硬件版本号

/* FLASH参数总个数*/
#define STM32_FLASH_INDEXBUF_LEN			    	21

/* ==============FLASH参数上（ceil）/下（floor）限宏定义================== */
/*-----------------REGION_U32--------------------*/
#define STM32_FLASH_MMSI_FLOOR			        (AIS_INFO_MMSI_MIN) 	                //MMSI存储最大长度
#define STM32_FLASH_MMSI_CEIL			        (AIS_INFO_MMSI_MAX)                     //MMSI存储最大长度
#define STM32_FLASH_MMSI_DEST_FLOOR            	(AIS_INFO_MMSI_MIN)                     //目的MMSI
#define STM32_FLASH_MMSI_DEST_CEIL             	(AIS_INFO_MMSI_MAX)                     //目的MMSI
#define STM32_FLASH_MMSI_BACKUP_FLOOR	    	(AIS_INFO_MMSI_MIN)                     //MMSI备份存储长度
#define STM32_FLASH_MMSI_BACKUP_CEIL		    (AIS_INFO_MMSI_MAX)                     //MMSI备份存储长度
#define STM32_FLASH_MMSI_DEST_BACKUP_FLOOR     	(AIS_INFO_MMSI_MIN)                     //目的MMSI备份存储长度
#define STM32_FLASH_MMSI_DEST_BACKUP_CEIL      	(AIS_INFO_MMSI_MAX)                     //目的MMSI备份存储长度
#define STM32_FLASH_SERIAL_FLOOR			    (DEV_INFO_SERIAL_MIN)			        //整机序列号存储最大长度
#define STM32_FLASH_SERIAL_CEIL			    	(DEV_INFO_SERIAL_MIN)                   //整机序列号存储最大长度
/*-----------------REGION_U16--------------------*/
#define STM32_FLASH_SHIPA_FLOOR			    	(AIS_INFO_SHIPA_MIN)                    //船舶大小A点位置存储最大长度
#define STM32_FLASH_SHIPA_CEIL			        (AIS_INFO_SHIPA_MAX)	                //船舶大小A点位置存储最大长度
#define STM32_FLASH_SHIPB_FLOOR			    	(AIS_INFO_SHIPB_MIN)	                //船舶大小B点位置存储最大长度
#define STM32_FLASH_SHIPB_CEIL			        (AIS_INFO_SHIPB_MAX)	                //船舶大小B点位置存储最大长度
/*-----------------REGION_U8---------------------*/
#define STM32_FLASH_MAX_DRAUGHT_FLOOR          	(AIS_INFO_MAX_DRAUGHT_MIN)              //最大吃水深度
#define STM32_FLASH_MAX_DRAUGHT_CEIL           	(AIS_INFO_MAX_DRAUGHT_MAX)              //最大吃水深度
#define STM32_FLASH_SHIPC_FLOOR			    	(AIS_INFO_SHIPC_MIN)	                //船舶大小C点位置存储最大长度
#define STM32_FLASH_SHIPC_CEIL			        (AIS_INFO_SHIPC_MAX)	                //船舶大小C点位置存储最大长度
#define STM32_FLASH_SHIPD_FLOOR			    	(AIS_INFO_SHIPD_MIN)	                //船舶大小D点位置存储最大长度
#define STM32_FLASH_SHIPD_CEIL			        (AIS_INFO_SHIPD_MAX)	                //船舶大小D点位置存储最大长度
#define STM32_FLASH_EPFS_TYPE_FLOOR            	(AIS_INFO_EPFS_TYPE_MIN)                //电子定位系统类型
#define STM32_FLASH_EPFS_TYPE_CEIL             	(AIS_INFO_EPFS_TYPE_MAX)                //电子定位系统类型
#define STM32_FLASH_SHIP_TYPE_FLOOR            	(AIS_INFO_SHIP_TYPE_MIN)                //船舶类型
#define STM32_FLASH_SHIP_TYPE_CEIL             	(AIS_INFO_SHIP_TYPE_MAX)                //船舶类型
#define STM32_FLASH_MODAMP_FLOOR			    16			                            //调制幅度存储最大长度
#define STM32_FLASH_MODAMP_CEIL			    	48			                            //调制幅度存储最大长度
#define STM32_FLASH_FREQADJ_FLOOR			    16			                            //发射功率存储最大长度
#define STM32_FLASH_FREQADJ_CEIL			    48			                            //发射功率存储最大长度
#define STM32_FLASH_RF_VOLT_ADJUST_FLOOR       	0           
#define STM32_FLASH_RF_VOLT_ADJUST_CEIL        	30  
#define STM32_FLASH_DTE_FLOOR                  	(AIS_INFO_DTE_MIN)                      //DTE
#define STM32_FLASH_DTE_CEIL                   	(AIS_INFO_DTE_MAX)                      //DTE
#define STM32_FLASH_AIS_CLASS_FLOOR            	(AIS_CLASS_A)                           //AIS类别
#define STM32_FLASH_AIS_CLASS_CEIL             	(AIS_CLASS_B)                           //AIS类别
/*-----------------REGION_CHAR-------------------*/
// 非数值类型参数无上/下限值

/* =======================EEPROM参数默认值宏定义=============================== */
/*-----------------REGION_U32--------------------*/
#define STM32_FLASH_MMSI_DEFAULT		        (AIS_INFO_MMSI_DEFAULT)                 //MMSI存储最大长度
#define STM32_FLASH_MMSI_DEST_DEFAULT          	(AIS_INFO_MMSI_DEFAULT)                 //目的MMSI
#define STM32_FLASH_MMSI_BACKUP_DEFAULT	    	(AIS_INFO_MMSI_DEFAULT)                 //mmsi备份存储长度
#define STM32_FLASH_MMSI_DEST_BACKUP_DEFAULT   	(AIS_INFO_MMSI_DEFAULT)                 //目的mmsi备份存储长度
#define STM32_FLASH_SERIAL_DEFAULT			    0			                            //整机序列号存储最大长度
///*-----------------REGION_U16--------------------*/
#define STM32_FLASH_SHIPA_DEFAULT			    (AIS_INFO_SHIPA_DEFAULT)                //船舶大小A点位置存储最大长度
#define STM32_FLASH_SHIPB_DEFAULT			    (AIS_INFO_SHIPB_DEFAULT)	            //船舶大小B点位置存储最大长度
///*-----------------REGION_U8--------------------*/
#define STM32_FLASH_MAX_DRAUGHT_DEFAULT        	(AIS_INFO_MAX_DRAUGHT_DEFAULT)          //最大吃水深度
#define STM32_FLASH_SHIPC_DEFAULT			    (AIS_INFO_SHIPC_DEFAULT)	            //船舶大小C点位置存储最大长度
#define STM32_FLASH_SHIPD_DEFAULT			    (AIS_INFO_SHIPD_DEFAULT)	            //船舶大小D点位置存储最大长度
#define STM32_FLASH_EPFS_TYPE_DEFAULT          	(AIS_INFO_EPFS_TYPE_DEFAULT)            //电子定位系统类型，默认值1-GPS
#define STM32_FLASH_SHIP_TYPE_DEFAULT          	(AIS_INFO_SHIP_TYPE_DEFAULT)            //船舶类型，默认值0-not available
#define STM32_FLASH_MODAMP_DEFAULT			    (DEV_INFO_MODAMP_DEFAULT)	            //调制幅度存储最大长度
#define STM32_FLASH_FREQADJ_DEFAULT		    	(DEV_INFO_FREQADJ_DEFAULT)	            //发射功率存储最大长度
#define STM32_FLASH_RF_VOLT_ADJUST_DEFAULT     	(DEV_INFO_RF_VOLT_ADJUST_DEFAULT)       
#define STM32_FLASH_DTE_DEFAULT                	(AIS_INFO_DTE_DEFAULT)                  //lq 单位1次
#define STM32_FLASH_AIS_CLASS_DEFAULT          	(AIS_CLASS_DEFAULT)                     
/*-----------------REGION_CHAR-------------------*/
#define STM32_FLASH_DESTINATION_DEFAULT        	(AIS_INFO_DESTINATION_DEFAULT)          //目的地
#define STM32_FLASH_VENDID_DEFAULT			    (AIS_INFO_VEND_ID_DEFAULT)
#define STM32_FLASH_SHIP_NAME_DEFAULT          	(AIS_INFO_SHIP_NAME_DEFAULT)
#define STM32_FLASH_HWVERSION_DEFAULT          	"0.0.0"                                 //硬件版本号

/* Private macro -------------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
U8 FlashWrite(U8 ucPara_Type, U8 *pPara, U8 ucLen);
U8 FlashRead(U8 ucPara_Type, U8 *pPara);
U8 U8ToU16(U8 *ucTemp,U16 *usTemp,U8 ucLen);
U8 U16ToU8(U16 *usTemp,U8 *ucTemp,U8 usLen);
void ReadCHAR(U8 *u8Temp,U16 *u16Temp,U8 u8Len);
u16 FLASH_ReadHalfWord(u32 address);
U8 STM32_FLASH_DefaultRead(U8 ucPara_Type, U8 *pPara);
#endif

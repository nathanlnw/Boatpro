/***********************************************
*
*		编程	：	WT
*		文件名	：	AISconfig.h
*		作用	：
*			本文件保存程序各个可配置参数
*
************************************************/
#ifndef AIS_CONFIG
#define AIS_CONFIG

/***********************************************
*
*		定义数据类型
*
***********************************************	 
#define UCHAR	unsigned char
#define SCHAR	char
#define USINT   unsigned short int
#define SINT    short int
#define ULINT   unsigned long int
#define SLINT   long int
 */

/************************************************
*
*		其他定义
*
************************************************/
//	信道定义
#define  CHANNEL_A 'A'
#define  CHANNEL_B 'B'

//时隙
#define FRAMETOSLOT 2250//时隙的个数

//	消息实质内容大小定义 8-bit
#define MSG18DATASIZE 21
#define MSG19DATASIZE 39
#define MSG14DATASIZE 21

//	消息实质内容字大小定义 16bit
#define MSG18MEMORYSIZE (((MSG18DATASIZE + 1) >> 1) + 1)
#define MSG19MEMORYSIZE (((MSG19DATASIZE + 1) >> 1) + 1)

//	判决门限
#define THRESHOLDH 40
#define THRESHOLDL 20

//GPS初始化的定位时限、GPS正常使用时的定位时限
//#define INITGPSFIXTIME  0x3000
//#define COMGPSFIXTIME   0x1000

//定时唤醒间隔：7分
#define WAKEUPINTERVAL  0x7000

//#define TIMEOUT_INIT_GPS_4503  (FRAMETOSLOT * 3)
#define TIMEOUT_INIT_GPS_4503  (FRAMETOSLOT * 2)//第一次GPS等待时间，改为2分钟
#define TIMESTAMP_MASK	0xFF
#define TIMEOUT_SLEEP_4503	(FRAMETOSLOT * 5)//睡五分钟
#define TIMEOUT_NORMAL_GPS_4503	(FRAMETOSLOT * 1)//正常1分钟
#define TIMEOUT_SOS_4503	(FRAMETOSLOT * 1)//sos一分钟

#define TIMEOUT_ROUTINE_4501	(FRAMETOSLOT * 6)
#define TIMEOUT_HIGHSPEED_4501	(FRAMETOSLOT / 2)
#define TIMEOUT_LOWSPEED_4501	(FRAMETOSLOT * 3)
#define TIMEOUT_SOS_4501	(FRAMETOSLOT * 1)//sos一分钟

#define SPEEDFLAG 20		//高低航速的门限值


#endif


/***********************************************
*
*		编程	：	WT
*		文件名	：	AIS_MsgModu.h
*		作用	：
*			本文件保存消息相关声明
*
************************************************/
#ifndef AIS_MSGMODU
#define AIS_MSGMODU

#include "Include.h"

/*lq 电池电量最低电压、最高电压*/
#define BATTERY_VOLUME_MOST_VOLT    800
#define BATTERY_VOLUME_LEAST_VOLT   680

/*lq 6号消息内容最大长度*/
#define MSG6_IFM0_TEXT_LEN  102                                                 //lq 可能过长，class-B 要求Msg6的binary data至多36bytes(6bits?)，IFM0除去指定位后至多31bytes

#define RAIM_ERR_THRES              10  //lq 单位m
typedef struct bits_map_struct
{
	U8 index;                   //lq array的字节索引
	U8 offset;                  //lq index所指字节中的位偏移，offset==0表示指向MSB
	U8* array;
	U8 array_size;
	
}BITMAP;

extern U8 bitmap_set_value_for_U8(BITMAP* map, U8 data, U8 data_bit_len);
extern U8 bitmap_set_value(BITMAP* map, U32 data, U8 data_bit_len);
extern void InitMsg1(AISMsgCommStateUnion commState);
extern void InitMsg14(void);
extern void InitMsg18(AISMsgCommStateEnum commStateType, AISMsgCommStateUnion commState);
extern void InitMsg19(void);
extern U16 InitMsgPara(void);
extern void InitGpsCell(void);
extern U16 GetTemperForCOG(float Temper);
extern float GetBatteryVolume(void);
extern U16 GetSOG(void);
static double rad(double d);
#if 0
double GetDistance(double lat1, double lng1, double lat2, double lng2);
#else
S32 GetDistance(double lat1, double lng1, double lat2, double lng2);
#endif
/**
	@ingroup 协议栈
	@brief 协议栈和底层接口,更新GPS数据至协议栈，并产生消息
	@param 无
	@retval 0 结束

	更新GPS数据至协议栈，产生消息18，19，判断是否定位来决定是否发送消息
*/
extern U16 Update_GpsData_To_Msg(void);

extern U8 GPS_valid;
//extern U16 set_4501_interval(U16 speed);
extern void ResetMsgPara(void);

#endif

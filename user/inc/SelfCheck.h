/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: SelfCheck.h
  Author: lq       Version : V1.00        Date: 2014.07.17
  Description:     设备自检模块
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _SELF_CHECK_H
#define _SELF_CHECK_H

/* Include ------------------------------------------------------------------*/

/* Public define ------------------------------------------------------------*/
#define SLFCHK_TIME_PERIOD              200 //lq unit: 5ms
#define SLFCHK_TIME_TIMEOUT             10  //lq unit: 1s

/*lq 自检控制状态 */
#define SLFCHK_CTRL_STATE_IDLE          0
#define SLFCHK_CTRL_STATE_START         1
#define SLFCHK_CTRL_STATE_RUNNING       2

/*lq 自检项掩码 */
#define SLFCHK_ITEM_MASK_POWER_SUPPLY_VOLT          0x00000001
#define SLFCHK_ITEM_MASK_RTC_LSE                    0x00000002
#define SLFCHK_ITEM_MASK_GPS_VCC_CTRL               0x00000004
#define SLFCHK_ITEM_MASK_GPS_TIMEPULSE              0x00000008
#define SLFCHK_ITEM_MASK_GPS_DATA_COMM              0x00000010
#define SLFCHK_ITEM_MASK_GPS_ANTENNA                0x00000020
#define SLFCHK_ITEM_MASK_GPS_MODULE                 0x00000040
#define SLFCHK_ITEM_MASK_SI446X_A_PDN               0x00000080
#define SLFCHK_ITEM_MASK_SI446X_A_SEN               0x00000100
#define SLFCHK_ITEM_MASK_SI446X_A_DATA_COMM         0x00000200
#define SLFCHK_ITEM_MASK_EEPROM_WP                  0x00000400
#define SLFCHK_ITEM_MASK_EEPROM_DATA_COMM           0x00000800
#define SLFCHK_ITEM_MASK_SI446X_A_TYPE              0x00001000
#define SLFCHK_ITEM_MASK_SI446X_B_TYPE              0x00002000
#define SLFCHK_ITEM_MASK_SI446X_B_PDN               0x00004000
#define SLFCHK_ITEM_MASK_SI446X_B_SEN               0x00008000
#define SLFCHK_ITEM_MASK_SI446X_B_DATA_COMM         0x00010000
#define SLFCHK_ITEM_MASK_RF_LINK                    0x00020000
#define SLFCHK_ITEM_MASK_ALL                        0x0003FFFF
          
/* Public typedef -----------------------------------------------------------*/
typedef struct SelfCheckStruct
{
	U32 TimeCnt;
	U16 TimePeriod;	
    U16 TimeoutCnt;
        
    U8  CtrlState;
    U8  StartFlg;
    U8  CompleteFlg;
    U32 CheckResult;        //lq 每一位对应一个自检项，1-OK，0-ERROR    
    U32 CompleteBitFlg;     //lq 每一位对应一个自检项，1-完成，0-未完成    
    U32 CheckSelect;        //lq 每一位对应一个自检项，1-进行自检，0-不进行自检    

/*lq
    U8  PowerSupplyVoltOkFlg;
    U8  RtcLseOkFlg;
    U8  GpsVccCtrlOkFlg;
    U8  GpsTimePulseOkFlg;
    U8  GpsCommOkFlg;
    U8  GpsAntennaOkFlg;
    U8  GpsSelfCheckOkFlg;
    U8  Rda1846VccCtrlOkFlg;
    U8  Rda1846CommOkFlg;
    U8  EepromWriteProtectOkFlg;
    U8  EepromCommOkFlg;
*/    
}SelfCheckStruct;

/* Public variables ---------------------------------------------------------*/

/* Public function ----------------------------------------------------------*/

//lnw add
extern 	U32 SLFCHK_GetCheckSelectValue(void);
extern  void SLFCHK_InstSelfCheckProcess(void);
extern void SLFCHK_InstSelfCheckStructInit(void);
extern void SLFCHK_InstCfgCheckSelect(const U32 bitMask);
extern void SLFCHK_InstSelfCheckStart(void);


#endif

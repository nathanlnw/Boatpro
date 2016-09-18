/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: ITDMA.h
  Author: lq       Version : V1.00        Date: 2013.01.15
  Description:     ARM 进入 Standby 模式时，保存在 Backup Register 中的变量相关的
                   宏、结构体定义，变量、函数声明
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include "BKPRegister.h"
#include "def.h"
//#include "stm32f10x_bkp.h"
#include "stm32f10x.h"
#include "Include.h"

/*lq 备份域数据寄存器索引*/
u16 arrBKP_DR_INDEX[BKPR_MAXNUM] = {

BKP_DR1, BKP_DR2, BKP_DR3, BKP_DR4, BKP_DR5, BKP_DR6, BKP_DR7, 
BKP_DR8, BKP_DR9, BKP_DR10, BKP_DR11, BKP_DR12, BKP_DR13, BKP_DR14, 
BKP_DR15, BKP_DR16, BKP_DR17, BKP_DR18, BKP_DR19, BKP_DR20, BKP_DR21, 
BKP_DR22, BKP_DR23, BKP_DR24, BKP_DR25, BKP_DR26, BKP_DR27, BKP_DR28, 
BKP_DR29, BKP_DR30, BKP_DR31, BKP_DR32, BKP_DR33, BKP_DR34, BKP_DR35, 
BKP_DR36, BKP_DR37, BKP_DR38, BKP_DR39, BKP_DR40, BKP_DR41, BKP_DR42 };

#if 0
/*lq 备份域数据寄存器参数名，调试用*/
const U8 arrBKP_DR_PARA_NAME[BKPR_PARA_MAXNUM][44] = {
"BKPR_FLAG_INDEX                            ",  
"BKPR_GPS_LONGITUDE_H_INDEX                 ",  
"BKPR_GPS_LONGITUDE_L_INDEX                 ",  
"BKPR_GPS_LATITUDE_H_INDEX                  ",  
"BKPR_GPS_LATITUDE_L_INDEX                  ",  
"BKPR_GPS_SOG_INDEX                         ",  
"BKPR_GPS_COG_INDEX                         ",  
"BKPR_GPS_TIME_HOUR_INDEX                   ",  
"BKPR_GPS_TIME_MIN_INDEX                    ",  
"BKPR_GPS_FALL_COUNT_INDEX                  ",   
"BKPR_GPS_CTRL_STA_INDEX                    ", 
"BKPR_GPS_UNFIX_1H_PERIOD_TIM_FRAME_H_INDEX ", 
"BKPR_GPS_UNFIX_1H_PERIOD_TIM_FRAME_L_INDEX ",
"BKPR_GPS_UNFIX_1H_PERIOD_TIM_SLOT_INDEX    ",
"BKPR_GPS_START_FRAME_H_INDEX               ",  
"BKPR_GPS_START_FRAME_L_INDEX               ",  
"BKPR_GPS_START_SLOT_INDEX                  ",
"BKPR_GPS_OFF_FRAME_H_INDEX                 ",   
"BKPR_GPS_OFF_FRAME_L_INDEX                 ",  
"BKPR_GPS_OFF_SLOT_INDEX                    ",     
"BKPR_BURST_TX_TIM_FRAME_H_INDEX            ",  
"BKPR_BURST_TX_TIM_FRAME_L_INDEX            ",  
"BKPR_BURST_TX_TIM_SLOT_INDEX               ",  
"BKPR_SLOT_TIMEOUT_INDEX                    ",  
"BKPR_SLOT_OFFSET_INDEX                     ",  
"BKPR_RTC_LSE_OR_LSI_INDEX                  ",  
"BKPR_RTC_FREQUENCE_INDEX                   ",  
"BKPR_RTC_ALARM_MCU_OR_GPS_INDEX            ",  
"BKPR_RTC_ALARM_VALUE_INDEX                 ",  
"BKPR_PHASE_CNT_INDEX                       ",  
"BKPR_SLOT_CNT_INDEX                        ",  
"BKPR_FRAME_CNT_H_INDEX                     ",  
"BKPR_FRAME_CNT_L_INDEX                     ",  
"BKPR_TIM_ADJUST_1_INDEX                    ",  
"BKPR_TIM_ADJUST_10_INDEX                   ",  
"BKPR_TIM_ADJUST_100_INDEX                  ",  
"BKPR_TIM_ADJUST_PERIOD_CNT_LAST_INDEX      ",  
"BKPR_RAND_SEED_H_INDEX                     ",
"BKPR_RAND_SEED_L_INDEX                     ",
};
#endif

/*lq 备份域数据寄存器数组*/
U16 arrBKPDRData[BKPR_MAXNUM];

/*******************************************************************************
* Function Name  : InitBKP_DRData
* Description    : 初始化需保存至BKP Register中的数据
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitBKP_DRData(void)
{
    int i;

    for (i = 0; i < BKPR_MAXNUM; i++)
    {
       arrBKPDRData[i] =  0;
    }
}

/*******************************************************************************
* Function Name  : SaveBKP_DRData
* Description    : 保存临时数据至 BKP Register
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SaveBKP_DRData(void)
{
    int i;

    PWR_BackupAccessCmd(ENABLE);

    for (i = 0; i < BKPR_PARA_MAXNUM; i++)
    {
        BKP_WriteBackupRegister(arrBKP_DR_INDEX[i], arrBKPDRData[i]); //lq... 类型装换（函数的第二个参数的数据类型为 u16）       
    }
}

/*******************************************************************************
* Function Name  : GetBKP_DRData
* Description    : 从BKP Register中读取数据
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GetBKP_DRData(void)
{
    int i;

    PWR_BackupAccessCmd(ENABLE);

    for (i = 0; i < BKPR_PARA_MAXNUM; i++)
    {
        arrBKPDRData[i] = BKP_ReadBackupRegister(arrBKP_DR_INDEX[i]);//lq... 类型装换（函数的返回值的数据类型为 u16）        
    }
}

#if 0
/*******************************************************************************
* Function Name  : BKP_DRDataInit
* Description    : 备份数据寄存器数据初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BKP_DRDataInit(void)
{
    int i;

    for (i = 0; i < BKPR_MAXNUM; i++)
    {
        arrBKPDRData[i] = 0;
    }
}
#endif

/*******************************************************************************
* Function Name  : InitBKP_DR
* Description    : 备份数据寄存器初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitBKP_DR(void)
{
    int i;

    PWR_BackupAccessCmd(ENABLE);

    for (i = 0; i < BKPR_MAXNUM; i++)
    {
        BKP_WriteBackupRegister(arrBKP_DR_INDEX[i], 0);        
    }
}

/*******************************************************************************
* Function Name  : IsRtcAlarm
* Description    : 是否是RTC闹钟唤醒
* Input          : None
* Output         : None
* Return         : TRUE 是，FALSE 否
*******************************************************************************/
U8 IsRtcAlarm(void)
{
    /*lq 读RTC_CNT前需等APB1与RTC时钟同步*/
	RTC_WaitForSynchro();

    // if (RTC_GetCounter() >= arrBKPDRData[BKPR_RTC_ALARM_VALUE_INDEX])
    if (RTC_GetCounter() >= (((arrBKPDRData[BKPR_RTC_ALARM_VALUE_H_INDEX] << 16) & 0xFFFF0000) 
        | (arrBKPDRData[BKPR_RTC_ALARM_VALUE_L_INDEX] & 0xFFFF)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* Function Name  : SetBKPDRDataBit
* Description    : 将指定寄存器数据的指定位置1或置0
* Input          : argBKPDRDataNum - 寄存器数据号，
                   argBitNum - 数据位，
                   argBitValue - 位值
* Output         : None
* Return         : None
*******************************************************************************/
void SetBKPDRDataBit(U8 argBKPDRDataNum, U8 argBitNum, U8 argBitValue)
{    
    if (argBitValue == TRUE)
    {
        arrBKPDRData[argBKPDRDataNum] |= 1 << argBitNum; 
    }
    else
    {
        arrBKPDRData[argBKPDRDataNum] &= ~(1 << argBitNum);  
    }
}

/*******************************************************************************
* Function Name  : SetBKPDRDataBit
* Description    : 将指定寄存器数据的指定位置1或置0
* Input          : argBKPDRDataNum - 寄存器数据号，
                   argBitNum - 数据位，
                   argBitValue - 位值
* Output         : None
* Return         : None
*******************************************************************************/
U8 GetBKPDRDataBit(U8 argBKPDRDataNum, U8 argBitNum)
{    
    if (arrBKPDRData[argBKPDRDataNum] & (U16)(1 << argBitNum))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


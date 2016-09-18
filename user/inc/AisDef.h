/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: AisDef.h
  Author: lq       Version : V1.0.0        Date: 2015.04.29
  Description:     AIS通用定义
  Version:         V1.0.0
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _AISDEF_H_
#define _AISDEF_H_

/* Include -------------------------------------------------------------------*/
#include "def.h"

/* Public define -------------------------------------------------------------*/
/*lq 静态链表无效索引 */
#define STATIC_LIST_INDEX_NULL          -1

/*lq AIS 信道相关定义 */
#define AIS_CHANNEL_FREQ_A              161975000u  //lq 单位：1Hz
#define AIS_CHANNEL_FREQ_B              162025000u
#define AIS_CHANNEL_NAME_A              "A"
#define AIS_CHANNEL_NAME_B              "B"
#define AIS_CHANNEL_CHAR_A              'A'
#define AIS_CHANNEL_CHAR_B              'B'
#define AIS_CHANNEL_AIS_TOTAL           2

/*lq 信道宏定义（以下宏定义用于兼容，不建议使用） */
#define CHANNEL_AIS_TOTAL               2   //lq AIS专用信道数（Channel A and Channel B）
#define CHANNEL_NAME_A                  (AIS_CHANNEL_NAME_A)
#define CHANNEL_NAME_B                  (AIS_CHANNEL_NAME_B)
#define CHANNEL_FREQ_A                  (AIS_CHANNEL_FREQ_A)
#define CHANNEL_FREQ_B                  (AIS_CHANNEL_FREQ_B)

/*lq AIS频率 */
#define AIS_FREQ_A                      (AIS_CHANNEL_FREQ_A)
#define AIS_FREQ_B                      (AIS_CHANNEL_FREQ_B)

/*lq AIS类别 */
#define AIS_CLASS_A                     0
#define AIS_CLASS_B                     1
#define AIS_CLASS_DEFAULT               (AIS_CLASS_B)

/*lq AIS bit时长定义 */
#define AIS_BIT_PHASE_SIZE              5       //lq unit: 1phase
#define AIS_BIT_TIME_DATA_START         6       //lq unit: 1bit
#define AIS_BIT_TIME_DATA_END           8       //lq unit: 1bit

/*lq AIS时隙间隔定义 */ 
#define AIS_SLOT_INTERVAL_3M                            ((FRAME_SLOT_SIZE) * 3)     //lq unit: 1slot，3min
#define AIS_SLOT_INTERVAL_6M                            ((FRAME_SLOT_SIZE) * 6)     //lq unit: 1slot，6min
#define AIS_SLOT_INTERVAL_MSG24A_TO_MSG19               375                         //lq unit: 1slot，msg24A与msg19之间的发送间隔
#define AIS_SLOT_INTERVAL_MSG24B_TO_MSG24A              375                         //lq unit: 1slot，msg24B与msg24A之间的发送间隔
#define AIS_SLOT_INTERVAL_SEND_TO_TX                    2                           //lq unit: 1slot，消息开始发送（组建消息并送入发送队列）到开始发射的时间间隔
#define AIS_SLOT_INTERVAL_FIXED_PERIOD_TX_PERIOD        (AIS_SLOT_INTERVAL_6M)      //lq unit: 1slot，固定周期发射的周期
#define AIS_SLOT_INTERVAL_SLOT_SELECT_OFFSET_MAX        ((S32)((AIS_SLOT_INTERVAL_FIXED_PERIOD_TX_PERIOD) * 0.01))     //lq unit: 1slot，固定周期发射下，发射时隙与标称时隙的偏移量最大值（此处(S32)类型转换是为解决Random()调用bug）

#define AIS_PHASE_INTERVAL_DATA_START                   ((AIS_BIT_PHASE_SIZE) * (AIS_BIT_TIME_DATA_START))

/*lq AIS确认 */
#define AIS_ACK_ONCE_QUANTITY_MAX       4       //lq AIS一次确认中包含的目的的数量的最大值

/* Public typedef ------------------------------------------------------------*/
/*lq 信道枚举类型定义 */
typedef enum
{
    ENUM_AIS_CHANNEL_A = 0,             //lq 161.975MHz
    ENUM_AIS_CHANNEL_B,                 //lq 162.025MHz
    ENUM_AIS_CHANNEL_BOTH,              //lq A and B    
    ENUM_AIS_CHANNEL_EITHER             //lq A or B    
}AisChannelEnum;

/* Public variables ----------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/

#endif
/*==============================================================================
------------------------------------End of File---------------------------------
==============================================================================*/

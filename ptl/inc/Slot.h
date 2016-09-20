/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: Slot.h
  Author: lq       Version : V1.0.0        Date: 2015.01.15
  Description:     AIS 时隙管理
  Version:         V1.0.0
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _SLOT_H_
#define _SLOT_H_

/* Include -------------------------------------------------------------------*/
#include "Include.h"

/* Public define -------------------------------------------------------------*/
/*lq 时隙相位定义 */
#define FRAME_SLOT_SIZE                 2250    //lq slot number of one frame
#define SLOT_PHASE_SIZE                 1280    //lq phase number of one slot

/*lq 时隙有效性判断 */
#define SLOT_VALIDITY_CHECK(slot)       (((slot >= 0) && (slot < FRAME_SLOT_SIZE)) ? TRUE : FALSE)

/*lq 专用时隙值定义 */
#define SLOT_INVALID_VALUE              FRAME_SLOT_SIZE     //lq 无效时隙值
#define SLOT_FORCE_TX_VALUE             -1      //lq 强制发送标志时隙值（使用当前的时隙立即发送）

/*lq 无效帧 */
#define FRAME_INVALID_VALUE             -1

/*lq 相位 */
#define PHASE_VALUE_TX_START            0       //lq 发射起始相位

/*lq 时隙映射帧数 */
#define SLOT_MAP_FRAME_SIZE             7       //lq 时隙映射表的帧数

/*lq 时隙状态 */
#define SLOT_STATE_INVALID              0   //lq 无效状态
#define SLOT_STATE_FREE                 1
#define SLOT_STATE_INTER_ALLOC          2
#define SLOT_STATE_EXTER_ALLOC          3
#define SLOT_STATE_AVAILABLE            4
#define SLOT_STATE_UNAVAILABLE          5

/*lq 候选时隙选择间隔 */
#define SLOT_CANDIDATE_SELECT_INTERVAL_OFFSET_MIN   7   //lq unit: 1slot, i.e. 75 / 10
#define SLOT_CANDIDATE_SELECT_INTERVAL_OFFSET_MAX   112 //lq unit: 1slot, i.e. 1125 / 10
#define SLOT_CANDIDATE_BUFF_LEN                     ((SLOT_CANDIDATE_SELECT_INTERVAL_OFFSET_MAX << 1) + 1)    

/* Public typedef ------------------------------------------------------------*/
/*lq 候选时隙选择方式类型定义 */
typedef enum
{
    SLOT_CAND_SELECT_UNI = 0,   //lq 单边选择
    SLOT_CAND_SELECT_BI,        //lq 双边选择
}CandSlotSelectEnum;

/*lq 时隙状态映射表元素定义 */
typedef U8 SlotMapArray [FRAME_SLOT_SIZE];  //lq 一帧的时隙映射表

/*lq 候选时隙集合数组 */
typedef U16 CandidateSlotArray[SLOT_CANDIDATE_BUFF_LEN];

/*lq 时隙状态映射表 */
typedef struct
{
    SlotMapArray MapTable[SLOT_MAP_FRAME_SIZE];
    U8 TableIndex;                              //lq 当前帧索引
    S32 IndexFrame;                             //lq TableIndex对应的帧号（绝对值）

    CandidateSlotArray CandidateSlotBuff;
    U16 CandidateSlotCnt;
}SlotMapStruct;

/*lq 时隙计时结构体 */
typedef struct
{
    S32 Frame;
    S16 slot;
    S16 phase;
}SlotTimeStruct;
/* Public variables ----------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/
extern U8 SetSlotState(AisChannelEnum channel, U16 slotNum, U8 slotSate, S32 frame);
extern U8 GetSlotState(AisChannelEnum channel, U16 slotNum, S32 frame);
extern S16 SelectCandidateSlot(AisChannelEnum channel, S16 slot, U16 selectInterval, S32 frame, CandSlotSelectEnum selectType);
extern void SLOT_ModuleInit(void);

//  lnw  add  below
extern S16 CalcSlotNum(S32 frame, S16 slot, S32 slotOffset);//lnw add 
extern S32 CalcSlotOffset(S32 frame1, S16 slot1, S32 frame2, S16 slot2); 
extern U8 SetSlotState(AisChannelEnum channel, U16 slot, U8 slotSate, S32 frame);
extern U8 GetSlotState(AisChannelEnum channel, U16 slot, S32 frame);
extern S16 SelectCandidateSlot(AisChannelEnum channel, S16 slot, U16 selectInterval,S32 frame, CandSlotSelectEnum selectType);
extern S32 SelectCandidateSlotFrame(S16 candSlot, S16 nominalSlot, S32 nominalFrame); 
extern S32 CalcFrameNum(S32 frame, S16 slot, S32 slotOffset); 
extern S32 CalcSlotSum(S32 frame, S16 slot, S32 slotOffset); 
extern U8  IsSlotValid(S16 slot) ;


#endif

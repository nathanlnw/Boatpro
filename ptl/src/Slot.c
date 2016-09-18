/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: Slot.c
  Author: lq       Version : V1.0.0        Date: 2015.01.15
  Description:     AIS 时隙管理
  Version:         V1.0.0
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "Include.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static SlotMapStruct SlotMap;      //lq 时隙映射

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* 名称  : SlotMapStructInit
* 描述  : 结构体初始化
* 输入  : pSlotMapStruct - SlotMapStruct结构体指针  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SlotMapStructInit(SlotMapStruct *pSlotMapStruct)
{
    U16 i;
    U16 j;

    for (i = 0; i < SLOT_MAP_FRAME_SIZE; i++)
    {
        for (j = 0; j < FRAME_SLOT_SIZE; j++)
        {
            pSlotMapStruct->MapTable[i][j] = (SLOT_STATE_FREE << 4) | SLOT_STATE_FREE;
        }
    }
    pSlotMapStruct->TableIndex = 0;
    pSlotMapStruct->IndexFrame = FRAME_INVALID_VALUE;

    for (i = 0; i < SLOT_CANDIDATE_BUFF_LEN; i++)
    {
        pSlotMapStruct->CandidateSlotBuff[i] = 0;
    }    
    pSlotMapStruct->CandidateSlotCnt = 0;
}

/*******************************************************************************
* 名称  : SLOT_UpdateSlotMapIndexFrame
* 描述  : 更新时隙映射表当前索引及相应的帧号
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SLOT_UpdateSlotMapIndexFrame(void) 
{
    S32 currFrame;
    S32 frameOffset;
    S16 tempS16;
    S16 i;
    S16 k;

    currFrame = GetFrameNum();

    if (SlotMap.IndexFrame == FRAME_INVALID_VALUE)
    {
        SlotMap.IndexFrame = currFrame;
    }
    else
    {
        frameOffset = currFrame - SlotMap.IndexFrame;
        if (frameOffset > 0)
        {
            if (frameOffset > SLOT_MAP_FRAME_SIZE)
            {
                frameOffset = SLOT_MAP_FRAME_SIZE;
            }

            /*lq 时隙复用 */
            for (i = 0; i < frameOffset; i++)
            {
                tempS16 = (SlotMap.TableIndex + i) % SLOT_MAP_FRAME_SIZE;
                for (k = 0; k < FRAME_SLOT_SIZE; k++)
                {
                    SlotMap.MapTable[tempS16][k] =
                        (SLOT_STATE_FREE << 4) | SLOT_STATE_FREE;
                }
            }

            SlotMap.TableIndex = (SlotMap.TableIndex + frameOffset) % SLOT_MAP_FRAME_SIZE;
            SlotMap.IndexFrame = currFrame;
        }
    }
}

/*******************************************************************************
* 名称  : SetSlotState
* 描述  : 设置时隙状态（可扩展至更多信道）
* 输入  : channel - 信道号
          slot - 时隙号
          slotState - 时隙状态
          frame - 帧号（绝对值）	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 SetSlotState(AisChannelEnum channel, U16 slot, U8 slotSate, S32 frame) 
{
    U8 channelMask;
    U8 channelOffset;
    U8 tempU8;
    U16 tempU16;
    S32 tempS32;

    /*lq 更新时隙映射表当前帧 */
    SLOT_UpdateSlotMapIndexFrame();

    /*lq 计算指定帧号与当前帧号之差 */
    tempS32 = frame - SlotMap.IndexFrame;
    if ((tempS32 >= SLOT_MAP_FRAME_SIZE) || (tempS32 < 0))
    {
        return FALSE;
    }
    else
    {
        tempS32 = (SlotMap.TableIndex + tempS32) % SLOT_MAP_FRAME_SIZE; //lq 第frame帧在MapTable中的索引
    }

    /*lq 信道号判定 */
    if (channel == ENUM_AIS_CHANNEL_A)
    {
        channelMask = 0x0F;
        channelOffset = 0;
    }
    else if (channel == ENUM_AIS_CHANNEL_B)
    {
        channelMask = 0xF0;
        channelOffset = 4;
    }
    else
    {
        return FALSE;
    }

    /*lq 时隙号有效性处理 */
    tempU16 = slot % FRAME_SLOT_SIZE;
    
    /*lq 时隙状态转为映射表元素 */
    tempU8 = slotSate << channelOffset;
    tempU8 &= channelMask;

    /*lq 修改指定时隙的状态 */
    SlotMap.MapTable[tempS32][tempU16] &= (~channelMask);
    SlotMap.MapTable[tempS32][tempU16] |= tempU8;

    return TRUE;        
}

/*******************************************************************************
* 名称  : GetSlotState
* 描述  : 查询时隙状态（可扩展至更多信道）
* 输入  : channel - 信道号
          slot - 时隙号
          frame - 帧号（绝对值）	  
* 输出  : 无
* 返回  : 时隙状态
*******************************************************************************/
U8 GetSlotState(AisChannelEnum channel, U16 slot, S32 frame) 
{
    U8 channelMask;
    U8 channelOffset;
    U8 temp8;
    U8 slotSate;
    U16 tempU16;
    S32 tempS32;
    
    /*lq 更新时隙映射表当前帧 */
    SLOT_UpdateSlotMapIndexFrame();

    /*lq 计算指定帧号与当前帧号之差 */
    tempS32 = frame - SlotMap.IndexFrame;
    if ((tempS32 >= SLOT_MAP_FRAME_SIZE) || (tempS32 < 0))
    {
        return SLOT_STATE_INVALID;
    }
    else
    {
        tempS32 = (SlotMap.TableIndex + tempS32) % SLOT_MAP_FRAME_SIZE; //lq 第frame帧在MapTable中的索引
    }

    /*lq 信道号判定 */
    if (channel == ENUM_AIS_CHANNEL_A)
    {
        channelMask = 0x0F;
        channelOffset = 0;
    }
    else if (channel == ENUM_AIS_CHANNEL_B)
    {
        channelMask = 0xF0;
        channelOffset = 4;
    }
    else
    {
        return SLOT_STATE_INVALID;
    }

    /*lq 时隙号有效性处理 */
    tempU16 = slot % FRAME_SLOT_SIZE;

    /*lq 获取指定时隙的状态 */
    temp8 = SlotMap.MapTable[tempS32][tempU16] & channelMask;
    slotSate = temp8 >> channelOffset;
     
    return slotSate;        
}

/*******************************************************************************
* 名称  : SelectCandidateSlot
* 描述  : 选择候选时隙
* 输入  : channel - 信道号
          slot - 时隙号
          selectInterval - 候选时隙选择间隔单边偏离值
          frame - 帧号（绝对值）
          selectType - 单边选择or双边选择	  
* 输出  : 无
* 返回  : 选定的某个候选时隙号
*******************************************************************************/
S16 SelectCandidateSlot(AisChannelEnum channel, S16 slot, U16 selectInterval,
                        S32 frame, CandSlotSelectEnum selectType) 
{
    S16 tempS16;
    S32 tempS32;
    U16 i;
    U16 candidateSlotIndex;

    if (SlotMap.IndexFrame == FRAME_INVALID_VALUE)
    {
        return SLOT_INVALID_VALUE;
    }

    /*lq 计算指定帧号与当前帧号之差 */
    tempS32 = frame - SlotMap.IndexFrame;
    if ((tempS32 >= SLOT_MAP_FRAME_SIZE) || (tempS32 < 0))
    {
        return SLOT_INVALID_VALUE;
    }

    /*lq 信道号判定 */
    if (!((channel == ENUM_AIS_CHANNEL_A) || (channel == ENUM_AIS_CHANNEL_B)))
    {
        return SLOT_INVALID_VALUE;
    }

    /*lq 时隙号有效性处理 */
    slot = slot % FRAME_SLOT_SIZE;

    /*lq 选择间隔处理 */
    if (selectInterval < SLOT_CANDIDATE_SELECT_INTERVAL_OFFSET_MIN)
    {
        selectInterval = SLOT_CANDIDATE_SELECT_INTERVAL_OFFSET_MIN;
    }
    else if (selectInterval > SLOT_CANDIDATE_SELECT_INTERVAL_OFFSET_MAX)
    {
        selectInterval = SLOT_CANDIDATE_SELECT_INTERVAL_OFFSET_MAX;    
    }

    /*lq 确定候选时隙集合 */
    SlotMap.CandidateSlotCnt = 0;
    if (GetSlotState(channel, slot, frame) == SLOT_STATE_FREE)
    {
        SlotMap.CandidateSlotBuff[SlotMap.CandidateSlotCnt++] = slot;    
    }
    for (i = 1; i <= selectInterval; i++)
    {
        /*lq 双边选择 */
        if (selectType == SLOT_CAND_SELECT_BI)
        {
            if ((slot - i) < 0)
            {
                tempS32 = frame - 1;
            }
            else
            {
                tempS32 = frame;
            }
            tempS16 = (slot - i + FRAME_SLOT_SIZE) % FRAME_SLOT_SIZE;
            if (GetSlotState(channel, tempS16, tempS32) == SLOT_STATE_FREE)
            {
                SlotMap.CandidateSlotBuff[SlotMap.CandidateSlotCnt++] = tempS16;

                //lq... A/B信道相邻or相对时隙限制
            }
        }

        if ((slot + i) >= FRAME_SLOT_SIZE)
        {
            tempS32 = frame + 1;
        }
        else
        {
            tempS32 = frame;
        }
        tempS16 = (slot + i) % FRAME_SLOT_SIZE;
        if (GetSlotState(channel, tempS16, tempS32) == SLOT_STATE_FREE)
        {
            SlotMap.CandidateSlotBuff[SlotMap.CandidateSlotCnt++] = tempS16;            
        }
    }

    if (SlotMap.CandidateSlotCnt == 0)
    {
        return SLOT_INVALID_VALUE;
    }

    /*lq 从候选时隙集合中随机选择一个时隙 */
    candidateSlotIndex = (rand() % SlotMap.CandidateSlotCnt);

    return SlotMap.CandidateSlotBuff[candidateSlotIndex];
}

/*******************************************************************************
* 名称  : SelectCandidateSlotFrame
* 描述  : 选择候选时隙所在的帧
* 输入  : candSlot - 候选时隙
          nominalSlot - 标称时隙
          nominalFrame - 标称帧
* 输出  : 无
* 返回  : 候选时隙所在的帧
*******************************************************************************/
S32 SelectCandidateSlotFrame(S16 candSlot, S16 nominalSlot, S32 nominalFrame) 
{
    S32 tempS32;

    if (candSlot > nominalSlot)
    {
        if (candSlot - nominalSlot > SLOT_CANDIDATE_SELECT_INTERVAL_OFFSET_MAX)
        {
            tempS32 = nominalFrame - 1;
            if (tempS32 < 0)
            {
                tempS32 = 0;
            }
        }
        else
        {
            tempS32 = nominalFrame;
        }
    }
    else
    {
        if (nominalSlot - candSlot > SLOT_CANDIDATE_SELECT_INTERVAL_OFFSET_MAX)
        {
            tempS32 = nominalFrame + 1;
        }
        else
        {
            tempS32 = nominalFrame;
        }    
    }

    return tempS32;
}

/*******************************************************************************
* 名称  : CalcSlotOffset
* 描述  : 计算时隙偏移量
* 输入  : frame1 - 源帧号
          slot1 - 源时隙号
          frame2 - 目的帧号
          slot2 - 目的时隙号
* 输出  : 无
* 返回  : 时隙偏移量
*******************************************************************************/
S32 CalcSlotOffset(S32 frame1, S16 slot1, S32 frame2, S16 slot2) 
{
    S32 tempS32;

    tempS32 = (frame2 - frame1) * FRAME_SLOT_SIZE + (slot2 - slot1);    

    return tempS32;
}

/*******************************************************************************
* 名称  : CalcFrameNum
* 描述  : 计算帧号
* 输入  : frame - 帧号
          slot - 时隙号
          slotOffset - 时隙偏移量
* 输出  : 无
* 返回  : 加上偏移量后的帧号
*******************************************************************************/
S32 CalcFrameNum(S32 frame, S16 slot, S32 slotOffset) 
{
    S32 tempS32;

    tempS32 = (frame * FRAME_SLOT_SIZE + slot + slotOffset) / FRAME_SLOT_SIZE;

    return tempS32;
    
}

/*******************************************************************************
* 名称  : CalcSlotNum
* 描述  : 计算时隙号
* 输入  : frame - 帧号
          slot - 时隙号
          slotOffset - 时隙偏移量
* 输出  : 无
* 返回  : 加上偏移量后的时隙号
*******************************************************************************/
S16 CalcSlotNum(S32 frame, S16 slot, S32 slotOffset) 
{
    S16 tempS16;

    tempS16 = (frame * FRAME_SLOT_SIZE + slot + slotOffset) % FRAME_SLOT_SIZE;

    return tempS16;
    
}

/*******************************************************************************
* 名称  : CalcSlotOffset
* 描述  : 计算时隙偏移量
* 输入  : frame - 帧号
          slot - 时隙号
          slotOffset - 时隙偏移量
* 输出  : 无
* 返回  : 加上偏移量后的时隙总数
*******************************************************************************/
S32 CalcSlotSum(S32 frame, S16 slot, S32 slotOffset) 
{
    S32 tempS32;

    tempS32 = frame * FRAME_SLOT_SIZE + slot + slotOffset;

    return tempS32;    
}

/*******************************************************************************
* 名称  : IsSlotValid
* 描述  : 时隙有效性判断
* 输入  : slot - 时隙
* 输出  : 无
* 返回  : TRUE- 有效， FALSE - 无效
*******************************************************************************/
U8 IsSlotValid(S16 slot) 
{
    if (slot >= 0 && slot < FRAME_SLOT_SIZE)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* 名称  : SLOT_ModuleInit
* 描述  : 模块初始化
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SLOT_ModuleInit(void) 
{
    SlotMapStructInit(&SlotMap);
}


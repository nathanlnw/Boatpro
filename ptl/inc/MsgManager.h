/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: MsgManager.h
  Author: lq       Version : V1.0.0        Date: 2015.06.23
  Description:     消息管理模块
  Version:         V1.0.0
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _MSGMANAGER_H_
#define _MSGMANAGER_H_

/* Include -------------------------------------------------------------------*/
#include "Include.h"

/* Public define -------------------------------------------------------------*/
#define MSG_MAN_MSG_STATIC_LIST_LEN     7
#define MSG_MAN_ACK_FIFO_LEN            7

/* Public typedef ------------------------------------------------------------*/
/*lq 
 *  周期消息发送控制枚举
 */
typedef enum
{
    ENUM_PERIOD_SCHED_CTRL_STATE_IDLE = 0,
    ENUM_PERIOD_SCHED_CTRL_STATE_RUNNIG
}PeriodSchedCtrlStateEnum;

/*lq 消息优先级定义 */
typedef enum
{
    ENUM_AIS_MSG_PRIORITY_H = 0,
    ENUM_AIS_MSG_PRIORITY_1 = 1,        //lq AIS消息中的最高优先级
    ENUM_AIS_MSG_PRIORITY_2,
    ENUM_AIS_MSG_PRIORITY_3,
    ENUM_AIS_MSG_PRIORITY_4,
    ENUM_AIS_MSG_PRIORITY_L,
    ENUM_AIS_MSG_PRIORITY_DEFAULT = ENUM_AIS_MSG_PRIORITY_L
}AisMsgPriorityEnum;

/*lq AIS消息单元 */
typedef struct MsgCellStruct
{
    AISMsgTypeEnum MsgType;
    AisMsgPriorityEnum MsgPriority;
    AisChannelEnum Channel;
    S16 Slot;
    S32 Frame;
    U8 MsgState;
	AISMsgBinBuffArray Data;            //lq Data[0] 存放有效数据的字节长度（不包含Data[0]），AIS消息均为整字节长 			    
	U16 DataLength;		 			    //lq unit: 1Byte，数据长度（包含Data[0]，DataLength = Data[0] + 1），AIS消息均为整字节长
}MsgCellStruct;

/*lq AIS消息静态链表元素 */
typedef struct MsgStaticListElemStruct
{
    MsgCellStruct MsgCell;
    S16 NextIndex;
}MsgStaticListElemStruct;

/*lq 
 *  AIS消息静态链表
 *  链表中的元素按消息优先级有序排列      
 */
typedef struct MsgStaticListStruct
{
    MsgStaticListElemStruct StaticList[MSG_MAN_MSG_STATIC_LIST_LEN];
    S16 Head;                           //lq 已分配静态链表头索引
    S16 FreeHead;                       //lq 空闲静态链表头索引
}MsgStaticListStruct;

/*lq 
 *  AIS固定周期类消息发送任务结构体
 */
typedef struct
{
    AisChannelEnum Channel;
    S32 NominalFrame;
    S16 NominalSlot;                    //lq 标称时隙
    S32 TxFrame;                        
    S16 TxSlot;                         //lq 发射时隙为标称时隙加上一个随机值
    S32 TxSlotInterval;                 //lq unit: 1slot
    U8  ValidFlag;                      //lq 定时有效标志
}MsgTxTaskStruct;

/*lq 
 *  AIS固定周期类消息调度结构体
 */
typedef struct
{
    MsgTxTaskStruct MsgTxTaskMsg19;
    MsgTxTaskStruct MsgTxTaskMsg24A;
    MsgTxTaskStruct MsgTxTaskMsg24B;
	MsgTxTaskStruct MsgTxTaskMsg5;

    PeriodSchedCtrlStateEnum CtrlState;
}PeriodSchedStruct;

/*lq 
 *  AIS确认单元结构体
 */
typedef struct
{
    U32 DestinationID;
    U8  SequenceNumber;    
}AisAckCellStruct;

/*lq 
 *  AIS确认单元结构体
 */
typedef struct
{
    AisAckCellStruct FIFO[MSG_MAN_ACK_FIFO_LEN];    
    U16 Input;
    U16 Output;
}AisAckFifoStruct;

/*lq 
 *  AIS突发类消息调度结构体
 */
typedef struct
{
    AisAckFifoStruct Msg6AckA;          //lq 对信道A上6号消息的确认队列
    AisAckFifoStruct Msg6AckB;
    AisAckFifoStruct Msg12AckA;
    AisAckFifoStruct Msg12AckB;

    /*lq 可将通过ABM和BBM语句设置的消息放入该结构体中 */
    //lq... 通过ABM和BBM语句设置的消息暂时独立处理

}BurstSchedStruct;

/* Public variables ----------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/
extern U8 MSGMAN_TxMsgListIsEmpty(void);
extern U8 MSGMAN_TxMsgListIsFull(void);
extern U8 MSGMAN_TxMsgListInsert(const MsgCellStruct *pMsgCellStruct);
extern U8 MSGMAN_TxMsgListGetElem(MsgCellStruct *pMsgCellStruct);
extern void MSGMAN_ModuleInit(void);
extern U8 SendMsg1(AisChannelEnum channel, S16 txSlot, AISMsgCommStateUnion commState);
extern U8 SendMsg5(AisChannelEnum channel, S16 txSlot);
extern U8 SendMsg6(AisChannelEnum channel, S16 txSlot);
extern U8 SendMsg7(AisChannelEnum channel, S16 txSlot, U8 ackType, U32 *pDestinationID,
            U8 *pSequenceNumber);
extern U8 SendMsg8(AisChannelEnum channel, S16 txSlot);
extern U8 SendMsg12(AisChannelEnum channel, S16 txSlot);
extern U8 SendMsg13(AisChannelEnum channel, S16 txSlot, U8 ackType, U32 *pDestinationID,
            U8 *pSequenceNumber);
extern U8 SendMsg14(AisChannelEnum channel, S16 txSlot);
extern U8 SendMsg18(AisChannelEnum channel, S16 txSlot,
             AISMsgCommStateEnum commStateType, AISMsgCommStateUnion commState);
extern U8 SendMsg19(AisChannelEnum channel, S16 txSlot);
extern U8 SendMsg24A(AisChannelEnum channel, S16 txSlot);
extern U8 SendMsg24B(AisChannelEnum channel, S16 txSlot);



extern U8 MSGMAN_RxMsgListInsert(const MsgCellStruct *pMsgCellStruct);// lnw add
extern U8 SendMsg3(AisChannelEnum channel, S16 txSlot, AISMsgCommStateUnion commState);//lnw add
#endif
/*==============================================================================
------------------------------------End of File---------------------------------
==============================================================================*/

/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: AISMsgModule.c
  Author: lq       Version : V1.00        Date: 2014.04.22
  Description:     AIS消息类型定义
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "def.h"
#include "AISMsgModule.h"
#include "AIS_MsgModu.h"

/*lq 二进制消息缓存（用于存放构造出的二进制消息） */
AISMsgBinBuffArray_1_2_3  AISMsgBinBuff_1;
AISMsgBinBuffArray_1_2_3  AISMsgBinBuff_2;
AISMsgBinBuffArray_1_2_3  AISMsgBinBuff_3;
AISMsgBinBuffArray_4_11 AISMsgBinBuff_4;
AISMsgBinBuffArray_5    AISMsgBinBuff_5;
AISMsgBinBuffArray_6    AISMsgBinBuff_6;
AISMsgBinBuffArray_7_13 AISMsgBinBuff_7;
AISMsgBinBuffArray_8    AISMsgBinBuff_8;
AISMsgBinBuffArray_9    AISMsgBinBuff_9;
AISMsgBinBuffArray_10   AISMsgBinBuff_10;
AISMsgBinBuffArray_4_11 AISMsgBinBuff_11;
AISMsgBinBuffArray_12   AISMsgBinBuff_12;
AISMsgBinBuffArray_7_13 AISMsgBinBuff_13;
AISMsgBinBuffArray_14   AISMsgBinBuff_14;
AISMsgBinBuffArray_15   AISMsgBinBuff_15;
AISMsgBinBuffArray_16   AISMsgBinBuff_16;
AISMsgBinBuffArray_17   AISMsgBinBuff_17;
AISMsgBinBuffArray_18   AISMsgBinBuff_18;
AISMsgBinBuffArray_19   AISMsgBinBuff_19;
AISMsgBinBuffArray_20   AISMsgBinBuff_20;
AISMsgBinBuffArray_21   AISMsgBinBuff_21;
AISMsgBinBuffArray_22   AISMsgBinBuff_22;
AISMsgBinBuffArray_23   AISMsgBinBuff_23;
AISMsgBinBuffArray_24A  AISMsgBinBuff_24A;
AISMsgBinBuffArray_24B  AISMsgBinBuff_24B;
AISMsgBinBuffArray_25   AISMsgBinBuff_25;
AISMsgBinBuffArray_26   AISMsgBinBuff_26;
AISMsgBinBuffArray_27   AISMsgBinBuff_27;

/*lq AIS消息（用于存放解析出的AIS消息）*/
AISMsgTypeStruct_1_2_3  AISMsg_1;
AISMsgTypeStruct_1_2_3  AISMsg_2;
AISMsgTypeStruct_1_2_3  AISMsg_3;
AISMsgTypeStruct_4_11   AISMsg_4;
AISMsgTypeStruct_5      AISMsg_5;
AISMsgTypeStruct_6      AISMsg_6;
AISMsgTypeStruct_7_13   AISMsg_7;
AISMsgTypeStruct_8      AISMsg_8;
AISMsgTypeStruct_9      AISMsg_9;
AISMsgTypeStruct_10     AISMsg_10;
AISMsgTypeStruct_4_11   AISMsg_11;
AISMsgTypeStruct_12     AISMsg_12;
AISMsgTypeStruct_7_13   AISMsg_13;
AISMsgTypeStruct_14     AISMsg_14;
AISMsgTypeStruct_15     AISMsg_15;
AISMsgTypeStruct_16     AISMsg_16;
AISMsgTypeStruct_17     AISMsg_17;
AISMsgTypeStruct_18     AISMsg_18;
AISMsgTypeStruct_19     AISMsg_19;
AISMsgTypeStruct_20     AISMsg_20;
AISMsgTypeStruct_21     AISMsg_21;
AISMsgTypeStruct_22     AISMsg_22;
AISMsgTypeStruct_23     AISMsg_23;
AISMsgTypeStruct_24A    AISMsg_24A;
AISMsgTypeStruct_24B    AISMsg_24B;
AISMsgTypeStruct_25     AISMsg_25;
AISMsgTypeStruct_26     AISMsg_26;
AISMsgTypeStruct_27     AISMsg_27;

/*******************************************************************************
* 名称  : AISMSG_BuildMsg1
* 描述  : 组建AIS消息1
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg1(const AISMsgTypeStruct_1_2_3* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_1;
	msg_bitmap.array_size = sizeof(AISMsgBinBuff_1);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

	bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->UserID, 30);
	bitmap_set_value(&msg_bitmap, pAISMsg->NavigationalStatus, 4);
	bitmap_set_value(&msg_bitmap, pAISMsg->RotAIS, 8);
	bitmap_set_value(&msg_bitmap, pAISMsg->SOG, 10);
	bitmap_set_value(&msg_bitmap, pAISMsg->PositionAccuracy, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->Longitude, 28);
	bitmap_set_value(&msg_bitmap, pAISMsg->Latitude, 27);
	bitmap_set_value(&msg_bitmap, pAISMsg->COG, 12);
	bitmap_set_value(&msg_bitmap, pAISMsg->TrueHeading, 9);
	bitmap_set_value(&msg_bitmap, pAISMsg->TimeStamp, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->SpecialManoeuvreIndicator, 2);
	bitmap_set_value(&msg_bitmap, 0, 3);
	bitmap_set_value(&msg_bitmap, pAISMsg->RAIMFlag, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SyncState, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout, 3);
    if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 0)
    {
	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SubMessage.SlotOffset, 14);    
    }
    else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 1)
    {
	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SubMessage.UTCHourAndMinute, 14);        
    }
    else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 2
             || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 4
             || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 6)
    {
	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SubMessage.SlotNumber, 14);        
    }
    else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 3
             || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 5
             || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 7)
    {
	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SubMessage.ReceivedStations, 14);        
    }

    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg2
* 描述  : 组建AIS消息2
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg2(const AISMsgTypeStruct_1_2_3* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_2;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuff_2);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

	bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->UserID, 30);
	bitmap_set_value(&msg_bitmap, pAISMsg->NavigationalStatus, 4);
	bitmap_set_value(&msg_bitmap, pAISMsg->RotAIS, 8);
	bitmap_set_value(&msg_bitmap, pAISMsg->SOG, 10);
	bitmap_set_value(&msg_bitmap, pAISMsg->PositionAccuracy, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->Longitude, 28);
	bitmap_set_value(&msg_bitmap, pAISMsg->Latitude, 27);
	bitmap_set_value(&msg_bitmap, pAISMsg->COG, 12);
	bitmap_set_value(&msg_bitmap, pAISMsg->TrueHeading, 9);
	bitmap_set_value(&msg_bitmap, pAISMsg->TimeStamp, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->SpecialManoeuvreIndicator, 2);
	bitmap_set_value(&msg_bitmap, 0, 3);
	bitmap_set_value(&msg_bitmap, pAISMsg->RAIMFlag, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SyncState, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout, 3);
    if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 0)
    {
	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SubMessage.SlotOffset, 14);    
    }
    else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 1)
    {
	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SubMessage.UTCHourAndMinute, 14);        
    }
    else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 2
             || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 4
             || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 6)
    {
	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SubMessage.SlotNumber, 14);        
    }
    else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 3
             || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 5
             || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 7)
    {
	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SubMessage.ReceivedStations, 14);        
    }

    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg3
* 描述  : 组建AIS消息3
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg3(const AISMsgTypeStruct_1_2_3* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_3;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuff_3);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

	bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->UserID, 30);
	bitmap_set_value(&msg_bitmap, pAISMsg->NavigationalStatus, 4);
	bitmap_set_value(&msg_bitmap, pAISMsg->RotAIS, 8);
	bitmap_set_value(&msg_bitmap, pAISMsg->SOG, 10);
	bitmap_set_value(&msg_bitmap, pAISMsg->PositionAccuracy, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->Longitude, 28);
	bitmap_set_value(&msg_bitmap, pAISMsg->Latitude, 27);
	bitmap_set_value(&msg_bitmap, pAISMsg->COG, 12);
	bitmap_set_value(&msg_bitmap, pAISMsg->TrueHeading, 9);
	bitmap_set_value(&msg_bitmap, pAISMsg->TimeStamp, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->SpecialManoeuvreIndicator, 2);
	bitmap_set_value(&msg_bitmap, 0, 3);
	bitmap_set_value(&msg_bitmap, pAISMsg->RAIMFlag, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.ITDMACommState.SyncState, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.ITDMACommState.SlotIncrement, 13);
	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.ITDMACommState.NumberOfSlots, 3);
	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.ITDMACommState.KeepFlag, 1);

    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg4
* 描述  : 组建AIS消息4
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg4(const AISMsgTypeStruct_4_11* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_4;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuff_4);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
    
    bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->UserID, 30);
	bitmap_set_value(&msg_bitmap, pAISMsg->UTCYear, 14);
	bitmap_set_value(&msg_bitmap, pAISMsg->UTCMonth, 4);
	bitmap_set_value(&msg_bitmap, pAISMsg->UTCDay, 5);
	bitmap_set_value(&msg_bitmap, pAISMsg->UTCHour, 5);
	bitmap_set_value(&msg_bitmap, pAISMsg->UTCMinute, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->UTCSecond, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->PositionAccuracy, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->Longitude, 28);
	bitmap_set_value(&msg_bitmap, pAISMsg->Latitude, 27);
	bitmap_set_value(&msg_bitmap, pAISMsg->TypeOfEPFD, 4);
	bitmap_set_value(&msg_bitmap, pAISMsg->TxCtrlForLrbm, 1);
	bitmap_set_value(&msg_bitmap, 0, 9);
	bitmap_set_value(&msg_bitmap, pAISMsg->RAIMFlag, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SyncState, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SlotTimeout, 3);
    if (pAISMsg->CommunicationState.SlotTimeout == 0)
    {
	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SubMessage.SlotOffset, 14);    
    }
    else if (pAISMsg->CommunicationState.SlotTimeout == 1)
    {
	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SubMessage.UTCHourAndMinute, 14);        
    }
    else if (pAISMsg->CommunicationState.SlotTimeout == 2
             || pAISMsg->CommunicationState.SlotTimeout == 4
             || pAISMsg->CommunicationState.SlotTimeout == 6)
    {
	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SubMessage.SlotNumber, 14);        
    }
    else if (pAISMsg->CommunicationState.SlotTimeout == 3
             || pAISMsg->CommunicationState.SlotTimeout == 5
             || pAISMsg->CommunicationState.SlotTimeout == 7)
    {
	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SubMessage.ReceivedStations, 14);        
    }

    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg5
* 描述  : 组建AIS消息5
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg5(const AISMsgTypeStruct_5* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_5;
   	msg_bitmap.array_size = sizeof(AISMsgBinBuff_5);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
      
    bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
    bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
    bitmap_set_value(&msg_bitmap, pAISMsg->UserID, 30);
    bitmap_set_value(&msg_bitmap, pAISMsg->AISVersionIndicator, 2);
    bitmap_set_value(&msg_bitmap, pAISMsg->IMONumber, 30);
    for (i = 0; i < 7; i++)
    {
        bitmap_set_value(&msg_bitmap, pAISMsg->CallSign[i], 6);    
    }
    for (i = 0; i < 20; i++)
    {
        bitmap_set_value(&msg_bitmap, pAISMsg->Name[i], 6);    
    }
    bitmap_set_value(&msg_bitmap, pAISMsg->TypeOfShipAndCargo, 8);    
    bitmap_set_value(&msg_bitmap, pAISMsg->DimensionA, 9);    
    bitmap_set_value(&msg_bitmap, pAISMsg->DimensionB, 9);    
    bitmap_set_value(&msg_bitmap, pAISMsg->DimensionC, 6);    
    bitmap_set_value(&msg_bitmap, pAISMsg->DimensionD, 6);    
    bitmap_set_value(&msg_bitmap, pAISMsg->TypeOfEPFD, 4);    
    bitmap_set_value(&msg_bitmap, pAISMsg->ETAMonth, 4);    
    bitmap_set_value(&msg_bitmap, pAISMsg->ETADay, 5);    
    bitmap_set_value(&msg_bitmap, pAISMsg->ETAHour, 5);    
    bitmap_set_value(&msg_bitmap, pAISMsg->ETAMinute, 6);    
    bitmap_set_value(&msg_bitmap, pAISMsg->MaxPresentStaticDraught, 8);    
    for (i = 0; i < 20; i++)
    {
        bitmap_set_value(&msg_bitmap, pAISMsg->Destination[i], 6);    
    }
    bitmap_set_value(&msg_bitmap, pAISMsg->DTE, 1);    
    bitmap_set_value(&msg_bitmap, 0, 1);    


    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg6
* 描述  : 组建AIS消息6
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg6(AISMsgTypeStruct_6* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_6;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuff_6);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
         
    bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
    bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
    bitmap_set_value(&msg_bitmap, pAISMsg->SourceID, 30);
    bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber, 2);
    bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID, 30);
    bitmap_set_value(&msg_bitmap, pAISMsg->RetransmitFlag, 1);
    bitmap_set_value(&msg_bitmap, 0, 1);
    bitmap_set_value(&msg_bitmap, pAISMsg->AI_DAC, 10);
    bitmap_set_value(&msg_bitmap, pAISMsg->AI_FI, 6);
    if (pAISMsg->AppDataBitLen > AIS_MSG6_APPDATA_BIT_MAXLEN)
    {
        /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/
        pAISMsg->AppDataBitLen = AIS_MSG6_APPDATA_BIT_MAXLEN;      
    }
    Temp = pAISMsg->AppDataBitLen / 8;
    for (i = 0; i < Temp; i++)
    {
        bitmap_set_value(&msg_bitmap, pAISMsg->ApplicationData[i], 8);    
    }
    Temp = pAISMsg->AppDataBitLen % 8;
    if (Temp != 0)
    {
        bitmap_set_value(&msg_bitmap, pAISMsg->ApplicationData[i], Temp);
        bitmap_set_value(&msg_bitmap, 0, 8 - Temp);     //lq 字节边界                    
    }    

    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg7
* 描述  : 组建AIS消息7
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg7(const AISMsgTypeStruct_7_13* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_7;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuff_7);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

    bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
    bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
    bitmap_set_value(&msg_bitmap, pAISMsg->SourceID, 30);
    bitmap_set_value(&msg_bitmap, 0, 2);
    switch (pAISMsg->AckType)
    {
        case AIS_MSG7_13_ACK_TYPE_1:
        {
            bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID1, 30);
            bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber1, 2);
            break;
        }
        case AIS_MSG7_13_ACK_TYPE_2:
        {
            bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID1, 30);
            bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber1, 2);
            bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID2, 30);
            bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber2, 2);
            break;
        }
        case AIS_MSG7_13_ACK_TYPE_3:
        {
            bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID1, 30);
            bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber1, 2);
            bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID2, 30);
            bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber2, 2); 
            bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID3, 30);
            bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber3, 2);                       
            break;
        }
        case AIS_MSG7_13_ACK_TYPE_4:
        {
            bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID1, 30);
            bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber1, 2);            
            bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID2, 30);
            bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber2, 2);            
            bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID3, 30);
            bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber3, 2);
            bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID4, 30);
            bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber4, 2);                          
            break;
        }
        default :
        {
            bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID1, 30);
            bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber1, 2);
        }
    }
  
    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg8
* 描述  : 组建AIS消息8
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg8(AISMsgTypeStruct_8* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_8;
   	msg_bitmap.array_size = sizeof(AISMsgBinBuff_8);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
            
    bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
    bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
    bitmap_set_value(&msg_bitmap, pAISMsg->SourceID, 30);
    bitmap_set_value(&msg_bitmap, 0, 2);
    bitmap_set_value(&msg_bitmap, pAISMsg->AI_DAC, 10);
    bitmap_set_value(&msg_bitmap, pAISMsg->AI_FI, 6);
    if (pAISMsg->AppDataBitLen > AIS_MSG8_APPDATA_BIT_MAXLEN)
    {
        /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/
        pAISMsg->AppDataBitLen = AIS_MSG8_APPDATA_BIT_MAXLEN;      
    }
    Temp = pAISMsg->AppDataBitLen / 8;
    for (i = 0; i < Temp; i++)
    {
        bitmap_set_value(&msg_bitmap, pAISMsg->ApplicationData[i], 8);    
    }
    Temp = pAISMsg->AppDataBitLen % 8;
    if (Temp != 0)
    {
        bitmap_set_value(&msg_bitmap, pAISMsg->ApplicationData[i], Temp);
        bitmap_set_value(&msg_bitmap, 0, 8 - Temp);     //lq 字节边界                    
    }    

    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg9
* 描述  : 组建AIS消息9
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg9(const AISMsgTypeStruct_9* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_9;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuff_9);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

	bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->UserID, 30);
	bitmap_set_value(&msg_bitmap, pAISMsg->Altitude, 12);
	bitmap_set_value(&msg_bitmap, pAISMsg->SOG, 10);
	bitmap_set_value(&msg_bitmap, pAISMsg->PositionAccuracy, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->Longitude, 28);
	bitmap_set_value(&msg_bitmap, pAISMsg->Latitude, 27);
	bitmap_set_value(&msg_bitmap, pAISMsg->COG, 12);
	bitmap_set_value(&msg_bitmap, pAISMsg->TimeStamp, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->AltitudeSensor, 1);
	bitmap_set_value(&msg_bitmap, 0, 7);
	bitmap_set_value(&msg_bitmap, pAISMsg->DTE, 1);
	bitmap_set_value(&msg_bitmap, 0, 3);
	bitmap_set_value(&msg_bitmap, pAISMsg->AssignedModeFlag, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->RAIMFlag, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->CommStateSelectFlag, 1);
    if (pAISMsg->CommStateSelectFlag == 0)
    {
    	/*lq SOTDMA */
        bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SyncState, 2);
    	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout, 3);
        if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 0)
        {
    	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SubMessage.SlotOffset, 14);    
        }
        else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 1)
        {
    	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SubMessage.UTCHourAndMinute, 14);        
        }
        else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 2
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 4
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 6)
        {
    	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SubMessage.SlotNumber, 14);        
        }
        else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 3
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 5
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 7)
        {
    	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SubMessage.ReceivedStations, 14);        
        }    
    }
    else
    {
    	/*lq ITDMA */
        bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.ITDMACommState.SyncState, 2);
    	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.ITDMACommState.SlotIncrement, 13);
    	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.ITDMACommState.NumberOfSlots, 3);
    	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.ITDMACommState.KeepFlag, 1);    
    }

    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg10
* 描述  : 组建AIS消息10
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg10(const AISMsgTypeStruct_10* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_10;
 	msg_bitmap.array_size = sizeof(AISMsgBinBuff_10);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

	bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->SourceID, 30);
	bitmap_set_value(&msg_bitmap, 0, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID, 30);
	bitmap_set_value(&msg_bitmap, 0, 2);

    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg11
* 描述  : 组建AIS消息11
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg11(const AISMsgTypeStruct_4_11* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_11;
 	msg_bitmap.array_size = sizeof(AISMsgBinBuff_11);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
    
    bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->UserID, 30);
	bitmap_set_value(&msg_bitmap, pAISMsg->UTCYear, 14);
	bitmap_set_value(&msg_bitmap, pAISMsg->UTCMonth, 4);
	bitmap_set_value(&msg_bitmap, pAISMsg->UTCDay, 5);
	bitmap_set_value(&msg_bitmap, pAISMsg->UTCHour, 5);
	bitmap_set_value(&msg_bitmap, pAISMsg->UTCMinute, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->UTCSecond, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->PositionAccuracy, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->Longitude, 28);
	bitmap_set_value(&msg_bitmap, pAISMsg->Latitude, 27);
	bitmap_set_value(&msg_bitmap, pAISMsg->TypeOfEPFD, 4);
	bitmap_set_value(&msg_bitmap, pAISMsg->TxCtrlForLrbm, 1);
	bitmap_set_value(&msg_bitmap, 0, 9);
	bitmap_set_value(&msg_bitmap, pAISMsg->RAIMFlag, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SyncState, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SlotTimeout, 3);
    if (pAISMsg->CommunicationState.SlotTimeout == 0)
    {
	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SubMessage.SlotOffset, 14);    
    }
    else if (pAISMsg->CommunicationState.SlotTimeout == 1)
    {
	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SubMessage.UTCHourAndMinute, 14);        
    }
    else if (pAISMsg->CommunicationState.SlotTimeout == 2
             || pAISMsg->CommunicationState.SlotTimeout == 4
             || pAISMsg->CommunicationState.SlotTimeout == 6)
    {
	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SubMessage.SlotNumber, 14);        
    }
    else if (pAISMsg->CommunicationState.SlotTimeout == 3
             || pAISMsg->CommunicationState.SlotTimeout == 5
             || pAISMsg->CommunicationState.SlotTimeout == 7)
    {
	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SubMessage.ReceivedStations, 14);        
    }

    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg12
* 描述  : 组建AIS消息12
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg12(AISMsgTypeStruct_12* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_12;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuff_12);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

	bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->SourceID, 30);
	bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID, 30);
	bitmap_set_value(&msg_bitmap, pAISMsg->RetransmitFlag, 1);
	bitmap_set_value(&msg_bitmap, 0, 1);
    if (pAISMsg->TextLen > AIS_MSG12_TEXT_MAXLEN)
    {
        /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/        
        pAISMsg->TextLen = AIS_MSG12_TEXT_MAXLEN;     
    }

    for (i = 0; i < pAISMsg->TextLen; i++)
    {
	    bitmap_set_value(&msg_bitmap, pAISMsg->SafetyRelatedText[i], 6);
    } 
    
    /*lq 处理字节边界 */
    Temp = (pAISMsg->TextLen * 6) % 8;
	if (Temp != 0)
    {
        bitmap_set_value(&msg_bitmap, 0, 8 - Temp);        
    }

    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg13
* 描述  : 组建AIS消息13
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg13(const AISMsgTypeStruct_7_13* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_13;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuff_13);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

    bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
    bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
    bitmap_set_value(&msg_bitmap, pAISMsg->SourceID, 30);
    bitmap_set_value(&msg_bitmap, 0, 2);
    switch (pAISMsg->AckType)
    {
        case AIS_MSG7_13_ACK_TYPE_1:
        {
            bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID1, 30);
            bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber1, 2);
            break;
        }
        case AIS_MSG7_13_ACK_TYPE_2:
        {
            bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID1, 30);
            bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber1, 2);
            bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID2, 30);
            bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber2, 2);
            break;
        }
        case AIS_MSG7_13_ACK_TYPE_3:
        {
            bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID1, 30);
            bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber1, 2);
            bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID2, 30);
            bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber2, 2); 
            bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID3, 30);
            bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber3, 2);                       
            break;
        }
        case AIS_MSG7_13_ACK_TYPE_4:
        {
            bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID1, 30);
            bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber1, 2);            
            bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID2, 30);
            bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber2, 2);            
            bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID3, 30);
            bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber3, 2);
            bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID4, 30);
            bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber4, 2);                          
            break;
        }
        default :
        {
            bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID1, 30);
            bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber1, 2);
        }
    }
  
    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg14
* 描述  : 组建AIS消息14
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg14(AISMsgTypeStruct_14* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_14;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuff_14);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

	bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->SourceID, 30);
	bitmap_set_value(&msg_bitmap, 0, 2);
    if (pAISMsg->TextLen > AIS_MSG14_TEXT_MAXLEN)
    {
        /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/        
        pAISMsg->TextLen = AIS_MSG14_TEXT_MAXLEN;   
    }

    for (i = 0; i < pAISMsg->TextLen; i++)
    {
	    bitmap_set_value(&msg_bitmap, pAISMsg->SafetyRelatedText[i], 6);    
    }

    /*lq 处理字节边界 */
    Temp = (pAISMsg->TextLen * 6) % 8;
	if (Temp != 0)
    {
        bitmap_set_value(&msg_bitmap, 0, 8 - Temp);        
    }            

    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg15
* 描述  : 组建AIS消息15
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg15(const AISMsgTypeStruct_15* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_15;
 	msg_bitmap.array_size = sizeof(AISMsgBinBuff_15);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

	bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->SourceID, 30);
	bitmap_set_value(&msg_bitmap, 0, 2);

    switch (pAISMsg->InterrogationType)
    {
        case AIS_MSG15_INTEEROGATION_TYPE_1:
        {
	        bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID1, 30);
	        bitmap_set_value(&msg_bitmap, pAISMsg->MessageID1_1, 6);
	        bitmap_set_value(&msg_bitmap, pAISMsg->SlotOffset1_1, 12);
                        
            break;
        }
        case AIS_MSG15_INTEEROGATION_TYPE_2:
        {
	        bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID1, 30);
	        bitmap_set_value(&msg_bitmap, pAISMsg->MessageID1_1, 6);
	        bitmap_set_value(&msg_bitmap, pAISMsg->SlotOffset1_1, 12);
	        bitmap_set_value(&msg_bitmap, 0, 2);
	        bitmap_set_value(&msg_bitmap, pAISMsg->MessageID1_2, 6);
	        bitmap_set_value(&msg_bitmap, pAISMsg->SlotOffset1_2, 12);
	        bitmap_set_value(&msg_bitmap, 0, 4);    //lq 保持字节边界
                                              
            break;
        }
        case AIS_MSG15_INTEEROGATION_TYPE_3:
        {
	        bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID1, 30);
	        bitmap_set_value(&msg_bitmap, pAISMsg->MessageID1_1, 6);
	        bitmap_set_value(&msg_bitmap, pAISMsg->SlotOffset1_1, 12);
	        bitmap_set_value(&msg_bitmap, 0, 2);
	        bitmap_set_value(&msg_bitmap, 0, 6);
	        bitmap_set_value(&msg_bitmap, 0, 12);
	        bitmap_set_value(&msg_bitmap, 0, 2);
	        bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID2, 30);
	        bitmap_set_value(&msg_bitmap, pAISMsg->MessageID2_1, 6);
	        bitmap_set_value(&msg_bitmap, pAISMsg->SlotOffset2_1, 12);
	        bitmap_set_value(&msg_bitmap, 0, 2); 

            break;
        }
        case AIS_MSG15_INTEEROGATION_TYPE_4:
        {
	        bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID1, 30);
	        bitmap_set_value(&msg_bitmap, pAISMsg->MessageID1_1, 6);
	        bitmap_set_value(&msg_bitmap, pAISMsg->SlotOffset1_1, 12);
	        bitmap_set_value(&msg_bitmap, 0, 2);
	        bitmap_set_value(&msg_bitmap, pAISMsg->MessageID1_2, 6);
	        bitmap_set_value(&msg_bitmap, pAISMsg->SlotOffset1_2, 12);
	        bitmap_set_value(&msg_bitmap, 0, 2);
	        bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID2, 30);
	        bitmap_set_value(&msg_bitmap, pAISMsg->MessageID2_1, 6);
	        bitmap_set_value(&msg_bitmap, pAISMsg->SlotOffset2_1, 12);
	        bitmap_set_value(&msg_bitmap, 0, 2);
              
            break;
        }
        default :
        {
	        bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID1, 30);
	        bitmap_set_value(&msg_bitmap, pAISMsg->MessageID1_1, 6);
	        bitmap_set_value(&msg_bitmap, pAISMsg->SlotOffset1_1, 12);        
        }
    }

    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg16
* 描述  : 组建AIS消息16
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg16(const AISMsgTypeStruct_16* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_16;
 	msg_bitmap.array_size = sizeof(AISMsgBinBuff_16);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

	bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->SourceID, 30);
	bitmap_set_value(&msg_bitmap, 0, 2);

    switch (pAISMsg->CommandType)
    {
        case AIS_MSG16_COMMAND_TYPE_1:
        {
	        bitmap_set_value(&msg_bitmap, pAISMsg->DestinationA, 30);
	        bitmap_set_value(&msg_bitmap, pAISMsg->OffsetA, 12);
	        bitmap_set_value(&msg_bitmap, pAISMsg->IncrementA, 10);
	        bitmap_set_value(&msg_bitmap, 0, 4);
                        
            break;
        }
        case AIS_MSG16_COMMAND_TYPE_2:
        {
	        bitmap_set_value(&msg_bitmap, pAISMsg->DestinationA, 30);
	        bitmap_set_value(&msg_bitmap, pAISMsg->OffsetA, 12);
	        bitmap_set_value(&msg_bitmap, pAISMsg->IncrementA, 10);
	        bitmap_set_value(&msg_bitmap, pAISMsg->DestinationB, 30);
	        bitmap_set_value(&msg_bitmap, pAISMsg->OffsetB, 12);
	        bitmap_set_value(&msg_bitmap, pAISMsg->IncrementB, 10);
                                              
            break;
        }
        default :
        {
	        bitmap_set_value(&msg_bitmap, pAISMsg->DestinationA, 30);
	        bitmap_set_value(&msg_bitmap, pAISMsg->OffsetA, 12);
	        bitmap_set_value(&msg_bitmap, pAISMsg->IncrementA, 10);
	        bitmap_set_value(&msg_bitmap, 0, 4);       
        }
    }

    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg17
* 描述  : 组建AIS消息17
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg17(const AISMsgTypeStruct_17* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_17;
 	msg_bitmap.array_size = sizeof(AISMsgBinBuff_17);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

	bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->SourceID, 30);
	bitmap_set_value(&msg_bitmap, 0, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->Longitude, 18);
	bitmap_set_value(&msg_bitmap, pAISMsg->Latitude, 17);
	bitmap_set_value(&msg_bitmap, 0, 5);
    if (pAISMsg->N != 0)
    {
        bitmap_set_value(&msg_bitmap, pAISMsg->MessageType, 6);
    	bitmap_set_value(&msg_bitmap, pAISMsg->StationID, 10);
    	bitmap_set_value(&msg_bitmap, pAISMsg->ZCount, 13);
    	bitmap_set_value(&msg_bitmap, pAISMsg->SequenceNumber, 3);
        bitmap_set_value(&msg_bitmap, pAISMsg->N, 5);
        bitmap_set_value(&msg_bitmap, pAISMsg->Health, 3);
        
        /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/        
        if (pAISMsg->N > 29)
        {
            for (i = 0; i < 29; i++)
            {
                bitmap_set_value(&msg_bitmap, pAISMsg->DGNSSDataWord[i], 24);        
            }        
        }
        else
        {
            for (i = 0; i < pAISMsg->N; i++)
            {
                bitmap_set_value(&msg_bitmap, pAISMsg->DGNSSDataWord[i], 24);        
            }         
        }                      
    }

    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg18
* 描述  : 组建AIS消息18
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg18(const AISMsgTypeStruct_18* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_18;
 	msg_bitmap.array_size = sizeof(AISMsgBinBuff_18);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

	bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->UserID, 30);
	bitmap_set_value(&msg_bitmap, 0, 8);
	bitmap_set_value(&msg_bitmap, pAISMsg->SOG, 10);
	bitmap_set_value(&msg_bitmap, pAISMsg->PositionAccuracy, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->Longitude, 28);
	bitmap_set_value(&msg_bitmap, pAISMsg->Latitude, 27);
	bitmap_set_value(&msg_bitmap, pAISMsg->COG, 12);
	bitmap_set_value(&msg_bitmap, pAISMsg->TrueHeading, 9);
	bitmap_set_value(&msg_bitmap, pAISMsg->TimeStamp, 6);
	bitmap_set_value(&msg_bitmap, 0, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->ClassBUnitFlag, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->ClassBDisplayFlag, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->ClassBDSCFlag, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->ClassBBandFlag, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->ClassBMsg22Flag, 1);
 	bitmap_set_value(&msg_bitmap, pAISMsg->ModeFlag, 1);
 	bitmap_set_value(&msg_bitmap, pAISMsg->RAIMFlag, 1);
 	bitmap_set_value(&msg_bitmap, pAISMsg->CommStateSelectFlag, 1);
    if (pAISMsg->CommStateSelectFlag == 0)
    {
    	/*lq SOTDMA */
        bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SyncState, 2);
    	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout, 3);
        if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 0)
        {
    	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SubMessage.SlotOffset, 14);    
        }
        else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 1)
        {
    	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SubMessage.UTCHourAndMinute, 14);        
        }
        else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 2
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 4
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 6)
        {
    	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SubMessage.SlotNumber, 14);        
        }
        else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 3
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 5
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 7)
        {
    	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SubMessage.ReceivedStations, 14);        
        }    
    }
    else
    {
    	/*lq ITDMA */
        bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.ITDMACommState.SyncState, 2);
    	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.ITDMACommState.SlotIncrement, 13);
    	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.ITDMACommState.NumberOfSlots, 3);
    	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.ITDMACommState.KeepFlag, 1);    
    }

    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg19
* 描述  : 组建AIS消息19
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg19(const AISMsgTypeStruct_19* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_19;
 	msg_bitmap.array_size = sizeof(AISMsgBinBuff_19);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

	bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->UserID, 30);
	bitmap_set_value(&msg_bitmap, 0, 8);
	bitmap_set_value(&msg_bitmap, pAISMsg->SOG, 10);
	bitmap_set_value(&msg_bitmap, pAISMsg->PositionAccuracy, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->Longitude, 28);
	bitmap_set_value(&msg_bitmap, pAISMsg->Latitude, 27);
	bitmap_set_value(&msg_bitmap, pAISMsg->COG, 12);
	bitmap_set_value(&msg_bitmap, pAISMsg->TrueHeading, 9);
	bitmap_set_value(&msg_bitmap, pAISMsg->TimeStamp, 6);
	bitmap_set_value(&msg_bitmap, 0, 4);
    for (i = 0; i < 20; i++)
    {
        bitmap_set_value(&msg_bitmap, pAISMsg->Name[i], 6);    
    }
	bitmap_set_value(&msg_bitmap, pAISMsg->TypeOfShipAndCargo, 8);
	bitmap_set_value(&msg_bitmap, pAISMsg->DimensionA, 9);
	bitmap_set_value(&msg_bitmap, pAISMsg->DimensionB, 9);
	bitmap_set_value(&msg_bitmap, pAISMsg->DimensionC, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->DimensionD, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->TypeOfEPFD, 4);
 	bitmap_set_value(&msg_bitmap, pAISMsg->RAIMFlag, 1);
 	bitmap_set_value(&msg_bitmap, pAISMsg->DTE, 1);
 	bitmap_set_value(&msg_bitmap, pAISMsg->AssignedModeFlag, 1);
	bitmap_set_value(&msg_bitmap, 0, 4);

    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg20
* 描述  : 组建AIS消息20
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg20(const AISMsgTypeStruct_20* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_20;
 	msg_bitmap.array_size = sizeof(AISMsgBinBuff_20);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

	bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->SourceStationID, 30);
	bitmap_set_value(&msg_bitmap, 0, 2);

    switch (pAISMsg->ManagementType)
    {
        case AIS_MSG20_MANAGEMENT_TYPE_1:
        {
	        bitmap_set_value(&msg_bitmap, pAISMsg->OffsetNumber1, 12);
	        bitmap_set_value(&msg_bitmap, pAISMsg->NumberOfSlot1, 4);
	        bitmap_set_value(&msg_bitmap, pAISMsg->Timeout1, 3);
	        bitmap_set_value(&msg_bitmap, pAISMsg->Increment1, 11);
	        bitmap_set_value(&msg_bitmap, 0, 2);
                        
            break;
        }
        case AIS_MSG20_MANAGEMENT_TYPE_2:
        {
	        bitmap_set_value(&msg_bitmap, pAISMsg->OffsetNumber1, 12);
	        bitmap_set_value(&msg_bitmap, pAISMsg->NumberOfSlot1, 4);
	        bitmap_set_value(&msg_bitmap, pAISMsg->Timeout1, 3);
	        bitmap_set_value(&msg_bitmap, pAISMsg->Increment1, 11);
	        bitmap_set_value(&msg_bitmap, pAISMsg->OffsetNumber2, 12);
	        bitmap_set_value(&msg_bitmap, pAISMsg->NumberOfSlot2, 4);
	        bitmap_set_value(&msg_bitmap, pAISMsg->Timeout2, 3);
	        bitmap_set_value(&msg_bitmap, pAISMsg->Increment2, 11);
	        bitmap_set_value(&msg_bitmap, 0, 4);
                                              
            break;
        }
        case AIS_MSG20_MANAGEMENT_TYPE_3:
        {
	        bitmap_set_value(&msg_bitmap, pAISMsg->OffsetNumber1, 12);
	        bitmap_set_value(&msg_bitmap, pAISMsg->NumberOfSlot1, 4);
	        bitmap_set_value(&msg_bitmap, pAISMsg->Timeout1, 3);
	        bitmap_set_value(&msg_bitmap, pAISMsg->Increment1, 11);
	        bitmap_set_value(&msg_bitmap, pAISMsg->OffsetNumber2, 12);
	        bitmap_set_value(&msg_bitmap, pAISMsg->NumberOfSlot2, 4);
	        bitmap_set_value(&msg_bitmap, pAISMsg->Timeout2, 3);
	        bitmap_set_value(&msg_bitmap, pAISMsg->Increment2, 11);
	        bitmap_set_value(&msg_bitmap, pAISMsg->OffsetNumber3, 12);
	        bitmap_set_value(&msg_bitmap, pAISMsg->NumberOfSlot3, 4);
	        bitmap_set_value(&msg_bitmap, pAISMsg->Timeout3, 3);
	        bitmap_set_value(&msg_bitmap, pAISMsg->Increment3, 11);
	        bitmap_set_value(&msg_bitmap, 0, 6);

            break;
        }
        case AIS_MSG20_MANAGEMENT_TYPE_4:
        {
	        bitmap_set_value(&msg_bitmap, pAISMsg->OffsetNumber1, 12);
	        bitmap_set_value(&msg_bitmap, pAISMsg->NumberOfSlot1, 4);
	        bitmap_set_value(&msg_bitmap, pAISMsg->Timeout1, 3);
	        bitmap_set_value(&msg_bitmap, pAISMsg->Increment1, 11);
	        bitmap_set_value(&msg_bitmap, pAISMsg->OffsetNumber2, 12);
	        bitmap_set_value(&msg_bitmap, pAISMsg->NumberOfSlot2, 4);
	        bitmap_set_value(&msg_bitmap, pAISMsg->Timeout2, 3);
	        bitmap_set_value(&msg_bitmap, pAISMsg->Increment2, 11);
	        bitmap_set_value(&msg_bitmap, pAISMsg->OffsetNumber3, 12);
	        bitmap_set_value(&msg_bitmap, pAISMsg->NumberOfSlot3, 4);
	        bitmap_set_value(&msg_bitmap, pAISMsg->Timeout3, 3);
	        bitmap_set_value(&msg_bitmap, pAISMsg->Increment3, 11);
	        bitmap_set_value(&msg_bitmap, pAISMsg->OffsetNumber4, 12);
	        bitmap_set_value(&msg_bitmap, pAISMsg->NumberOfSlot4, 4);
	        bitmap_set_value(&msg_bitmap, pAISMsg->Timeout4, 3);
	        bitmap_set_value(&msg_bitmap, pAISMsg->Increment4, 11);
              
            break;
        }
        default :
        {
	        bitmap_set_value(&msg_bitmap, 0, 32);   //lq 32 = 12+4+3+11+2        
        }
    }

    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg21
* 描述  : 组建AIS消息21
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg21(const AISMsgTypeStruct_21* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_21;
	msg_bitmap.array_size = sizeof(AISMsgBinBuff_21);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

	bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->ID, 30);
	bitmap_set_value(&msg_bitmap, pAISMsg->TypeOfAtoN, 5);
    for (i = 0; i < 20; i++)
    {
	    bitmap_set_value(&msg_bitmap, pAISMsg->NameOfAtoN[i], 6);    
    }
	bitmap_set_value(&msg_bitmap, pAISMsg->PositionAccuracy, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->Longitude, 28);
	bitmap_set_value(&msg_bitmap, pAISMsg->Latitude, 27);
	bitmap_set_value(&msg_bitmap, pAISMsg->DimensionA, 9);
	bitmap_set_value(&msg_bitmap, pAISMsg->DimensionB, 9);
	bitmap_set_value(&msg_bitmap, pAISMsg->DimensionC, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->DimensionD, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->TypeOfEPFD, 4);
	bitmap_set_value(&msg_bitmap, pAISMsg->TimeStamp, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->OffPositionIndicator, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->AtoNStatus, 8);
 	bitmap_set_value(&msg_bitmap, pAISMsg->RAIMFlag, 1);
 	bitmap_set_value(&msg_bitmap, pAISMsg->VirtualAtoNFlag, 1);
 	bitmap_set_value(&msg_bitmap, pAISMsg->AssignedModeFlag, 1);
	bitmap_set_value(&msg_bitmap, 0, 1);
    if (pAISMsg->NameOfAtoNExtensionLen > 14)
    {
	    for (i = 0; i < 14; i++)
        {
            bitmap_set_value(&msg_bitmap, pAISMsg->NameOfAtoNExtension[i], 6);                
        }
    }
    else
    {
	    for (i = 0; i < pAISMsg->NameOfAtoNExtensionLen; i++)
        {
            bitmap_set_value(&msg_bitmap, pAISMsg->NameOfAtoNExtension[i], 6);                
        }    
    }
    
    /*lq 边界对齐*/
    i = 6 * i;      //lq 超出比特数
    i = i % 8;
    if ( i != 0)
    {
        i = 8 - i;  //lq 填充比特数 
        bitmap_set_value(&msg_bitmap, 0, i);        
    } 

    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg22
* 描述  : 组建AIS消息22
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg22(const AISMsgTypeStruct_22* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_22;
	msg_bitmap.array_size = sizeof(AISMsgBinBuff_22);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

    bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
    bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
    bitmap_set_value(&msg_bitmap, pAISMsg->StationID, 30);
    bitmap_set_value(&msg_bitmap, 0, 2);
    bitmap_set_value(&msg_bitmap, pAISMsg->ChannelA, 12);
    bitmap_set_value(&msg_bitmap, pAISMsg->ChannelB, 12);
    bitmap_set_value(&msg_bitmap, pAISMsg->TxRxMode, 4);
    bitmap_set_value(&msg_bitmap, pAISMsg->Power, 1);
    bitmap_set_value(&msg_bitmap, pAISMsg->LonOrMSBOfAddID1, 18);
    bitmap_set_value(&msg_bitmap, pAISMsg->LatOrLSBOfAddID1, 17);
    bitmap_set_value(&msg_bitmap, pAISMsg->LonOrMSBOfAddID2, 18);
    bitmap_set_value(&msg_bitmap, pAISMsg->LatOrLSBOfAddID2, 17);
    bitmap_set_value(&msg_bitmap, pAISMsg->AddOrBroadMsgIndicator, 1);
    bitmap_set_value(&msg_bitmap, pAISMsg->ChannelABandwidth, 1);
    bitmap_set_value(&msg_bitmap, pAISMsg->ChannelBBandwidth, 1);
    bitmap_set_value(&msg_bitmap, pAISMsg->TransitionalZoneSize, 3);
    bitmap_set_value(&msg_bitmap, 0, 23);
  
    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg23
* 描述  : 组建AIS消息23
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg23(const AISMsgTypeStruct_23* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_23;
 	msg_bitmap.array_size = sizeof(AISMsgBinBuff_23);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

    bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
    bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
    bitmap_set_value(&msg_bitmap, pAISMsg->SourceID, 30);
    bitmap_set_value(&msg_bitmap, 0, 2);
    bitmap_set_value(&msg_bitmap, pAISMsg->Longitude1, 18);
    bitmap_set_value(&msg_bitmap, pAISMsg->Latitude1, 17);
    bitmap_set_value(&msg_bitmap, pAISMsg->Longitude2, 18);
    bitmap_set_value(&msg_bitmap, pAISMsg->Latitude2, 17);
    bitmap_set_value(&msg_bitmap, pAISMsg->StationType, 4);
    bitmap_set_value(&msg_bitmap, pAISMsg->TypeOfShipAndCargo, 8);
    bitmap_set_value(&msg_bitmap, 0, 22);
    bitmap_set_value(&msg_bitmap, pAISMsg->TxRxMode, 4);
    bitmap_set_value(&msg_bitmap, pAISMsg->ReportingInterval, 4);
    bitmap_set_value(&msg_bitmap, pAISMsg->QuietTime, 4);
    bitmap_set_value(&msg_bitmap, 0, 6);
  
    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg24A
* 描述  : 组建AIS消息24A
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg24A(const AISMsgTypeStruct_24A* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_24A;
 	msg_bitmap.array_size = sizeof(AISMsgBinBuff_24A);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

    bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
    bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
    bitmap_set_value(&msg_bitmap, pAISMsg->UserID, 30);
    bitmap_set_value(&msg_bitmap, pAISMsg->PartNumber, 2);
    for (i = 0; i < 20; i++)
    {
        bitmap_set_value(&msg_bitmap, pAISMsg->Name[i], 6);    
    }
  
    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg24B
* 描述  : 组建AIS消息24B
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg24B(const AISMsgTypeStruct_24B* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_24B;
	msg_bitmap.array_size = sizeof(AISMsgBinBuff_24B);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

	bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->UserID, 30);
	bitmap_set_value(&msg_bitmap, pAISMsg->PartNumber, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->TypeOfShipAndCargo, 8);
    for (i = 0; i < 3; i++)
    {
        bitmap_set_value(&msg_bitmap, pAISMsg->VendorID.ManufacturerID[i], 6);    
    }
    bitmap_set_value(&msg_bitmap, pAISMsg->VendorID.UnitModelCode, 4);    
    bitmap_set_value(&msg_bitmap, pAISMsg->VendorID.UnitSerialNumber, 20);    
    for (i = 0; i < 7; i++)
    {
        bitmap_set_value(&msg_bitmap, pAISMsg->CallSign[i], 6);    
    }
	bitmap_set_value(&msg_bitmap, pAISMsg->DimensionA, 9);
	bitmap_set_value(&msg_bitmap, pAISMsg->DimensionB, 9);
	bitmap_set_value(&msg_bitmap, pAISMsg->DimensionC, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->DimensionD, 6);
	bitmap_set_value(&msg_bitmap, 0, 6);

    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg25
* 描述  : 组建AIS消息25
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg25(AISMsgTypeStruct_25* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_25;
 	msg_bitmap.array_size = sizeof(AISMsgBinBuff_25);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

	bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->SourceID, 30);
	bitmap_set_value(&msg_bitmap, pAISMsg->DestinationIndicator, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->BinaryDataFlag, 1);
    if (pAISMsg->DestinationIndicator == 0)
    {
        /*lq broadcast */        
        if (pAISMsg->BinaryDataFlag == 0)
        {
            /*lq unstructed binary data */
            if (pAISMsg->BinaryDataBitLen > AIS_MSG25_BINDATA_BIT_MAXLEN_00)
            {
                /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/        
                pAISMsg->BinaryDataBitLen = AIS_MSG25_BINDATA_BIT_MAXLEN_00;
            }

            Temp = pAISMsg->BinaryDataBitLen / 8;
            for (i = 0; i < Temp; i++)
            {
                bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.UnstructedBinData[i], 8);                                   
            }
            Temp = pAISMsg->BinaryDataBitLen % 8;
            if (Temp != 0)
            {
                bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.UnstructedBinData[i], Temp);                                                   	                
                //lq bitmap_set_value(&msg_bitmap, 0, 8 - Temp); //lq 字节边界处理（可以不用，因为msg_bitmap.array未设置的bit默认为0）
            }
        }
        else
        {
            /*lq structed binary data AI */
	        bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.StructedBinData.AI_DAC, 10);                                   
	        bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.StructedBinData.AI_FI, 6);                                   

            /*lq structed binary data App Data */
            if (pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen > AIS_MSG25_BINDATA_BIT_MAXLEN_01)
            {
                /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/        
                pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen = AIS_MSG25_BINDATA_BIT_MAXLEN_01;
            }
            pAISMsg->BinaryDataBitLen = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen + 16;            

            Temp = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen / 8;
            for (i = 0; i < Temp; i++)
            {
                bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.StructedBinData.ApplicationData[i], 8);                                   
            }
            Temp = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen % 8;
            if (Temp != 0)
            {
                bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.StructedBinData.ApplicationData[i], Temp);                                                   	                
                //lq bitmap_set_value(&msg_bitmap, 0, 8 - Temp); //lq 字节边界处理（可以不用，因为msg_bitmap.array未设置的bit默认为0）                                                   
            }
        }    
    }
    else
    {
        /*lq addressed */
	    /*lq destination id */
        bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID, 30);                
                
        /*lq binary data */
        if (pAISMsg->BinaryDataFlag == 0)
        {
            /*lq unstructed binary data */
            if (pAISMsg->BinaryDataBitLen > AIS_MSG25_BINDATA_BIT_MAXLEN_10)
            {
                /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/        
                pAISMsg->BinaryDataBitLen = AIS_MSG25_BINDATA_BIT_MAXLEN_10;
            }

            Temp = pAISMsg->BinaryDataBitLen / 8;
            for (i = 0; i < Temp; i++)
            {
                bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.UnstructedBinData[i], 8);                                   
            }
            Temp = pAISMsg->BinaryDataBitLen % 8;
            if (Temp != 0)
            {
                bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.UnstructedBinData[i], Temp);                                                   	                
                //lq bitmap_set_value(&msg_bitmap, 0, 8 - Temp); //lq 字节边界处理（可以不用，因为msg_bitmap.array未设置的bit默认为0）                                                  
            }
        }
        else
        {
            /*lq structed binary data AI */
	        bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.StructedBinData.AI_DAC, 10);                                   
	        bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.StructedBinData.AI_FI, 6);                                   

            /*lq structed binary data App Data */
            if (pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen > AIS_MSG25_BINDATA_BIT_MAXLEN_11)
            {
                /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/        
                pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen = AIS_MSG25_BINDATA_BIT_MAXLEN_11;
            }
            pAISMsg->BinaryDataBitLen = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen + 16;            

            Temp = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen / 8;
            for (i = 0; i < Temp; i++)
            {
                bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.StructedBinData.ApplicationData[i], 8);                                   
            }
            Temp = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen % 8;
            if (Temp != 0)
            {
                bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.StructedBinData.ApplicationData[i], Temp);                                                   	                
            }
        }     
    }

    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg26
* 描述  : 组建AIS消息26
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg26(AISMsgTypeStruct_26* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_26;
 	msg_bitmap.array_size = sizeof(AISMsgBinBuff_26);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

	bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->SourceID, 30);
	bitmap_set_value(&msg_bitmap, pAISMsg->DestinationIndicator, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->BinaryDataFlag, 1);
    if (pAISMsg->DestinationIndicator == 0)
    {
        /*lq broadcast */        
        if (pAISMsg->BinaryDataFlag == 0)
        {
            /*lq unstructed binary data */
            /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/                    
            if (pAISMsg->BinaryDataBitLen > AIS_MSG26_BINDATA_BIT_MAXLEN_00)
            {
                pAISMsg->BinaryDataBitLen = AIS_MSG26_BINDATA_BIT_MAXLEN_00;            
            }

            Temp = pAISMsg->BinaryDataBitLen / 8;
            for (i = 0; i < Temp; i++)
            {
                bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.UnstructedBinData[i], 8);                                   
            }
            Temp = pAISMsg->BinaryDataBitLen % 8;
            if (Temp != 0)
            {
                bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.UnstructedBinData[i], Temp);                                                   	                
            }               
        }
        else
        {
            /*lq structed binary data AI */
	        bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.StructedBinData.AI_DAC, 10);                                   
	        bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.StructedBinData.AI_FI, 6);                                   

            /*lq structed binary data App Data */
            /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/                    
            if (pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen > AIS_MSG26_BINDATA_BIT_MAXLEN_01)
            {
                pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen = AIS_MSG26_BINDATA_BIT_MAXLEN_01;            
            }
            pAISMsg->BinaryDataBitLen = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen + 16;            

            Temp = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen / 8;
            for (i = 0; i < Temp; i++)
            {
                bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.StructedBinData.ApplicationData[i], 8);                                   
            }
            Temp = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen % 8;
            if (Temp != 0)
            {
                bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.StructedBinData.ApplicationData[i], Temp);                                                   	                
            }        
        }
        
        /*lq 这些字段的位数应当保护整条消息的字节边界，
         *且保证Communication state selector flag和Communication state 为最后两个字段。
        */
        Temp = (40 + pAISMsg->BinaryDataBitLen + 20) % 8;
        if (Temp != 0)
        {
            bitmap_set_value(&msg_bitmap, 0, 8 - Temp); //lq 提前为保护字节边界做准备                    
        }            
    }
    else
    {
        /*lq addressed */
	    /*lq destination id */
        bitmap_set_value(&msg_bitmap, pAISMsg->DestinationID, 30);                
                
        /*lq binary data */
        if (pAISMsg->BinaryDataFlag == 0)
        {
            /*lq unstructed binary data */
            /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/                    
            if (pAISMsg->BinaryDataBitLen > AIS_MSG26_BINDATA_BIT_MAXLEN_10)
            {
                pAISMsg->BinaryDataBitLen = AIS_MSG26_BINDATA_BIT_MAXLEN_10;            
            }

            Temp = pAISMsg->BinaryDataBitLen / 8;
            for (i = 0; i < Temp; i++)
            {
                bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.UnstructedBinData[i], 8);                                   
            }
            Temp = pAISMsg->BinaryDataBitLen % 8;
            if (Temp != 0)
            {
                bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.UnstructedBinData[i], Temp);                                                   	                
            }
        }
        else
        {
            /*lq structed binary data AI */
	        bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.StructedBinData.AI_DAC, 10);                                   
	        bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.StructedBinData.AI_FI, 6);                                   

            /*lq structed binary data App Data */
            /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/                    
            if (pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen > AIS_MSG26_BINDATA_BIT_MAXLEN_11)
            {
                pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen = AIS_MSG26_BINDATA_BIT_MAXLEN_11;            
            }
            pAISMsg->BinaryDataBitLen = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen + 16;

            Temp = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen / 8;
            for (i = 0; i < Temp; i++)
            {
                bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.StructedBinData.ApplicationData[i], 8);                                   
            }
            Temp = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen % 8;
            if (Temp != 0)
            {
                bitmap_set_value(&msg_bitmap, pAISMsg->BinaryData.StructedBinData.ApplicationData[i], Temp);                                                   	                
            }        
        } 
        
        /*lq 这些字段的位数应当保护整条消息的字节边界，
         *且保证Communication state selector flag和Communication state 为最后两个字段。
        */
        Temp = (40 + 30 + pAISMsg->BinaryDataBitLen + 20) % 8;
        if (Temp != 0)
        {
            bitmap_set_value(&msg_bitmap, 0, 8 - Temp); //lq 提前为保护字节边界做准备                    
        }            
    }
    
 	bitmap_set_value(&msg_bitmap, pAISMsg->CommStateSelectFlag, 1);
    if (pAISMsg->CommStateSelectFlag == 0)
    {
    	/*lq SOTDMA */
        bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SyncState, 2);
    	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout, 3);
        if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 0)
        {
    	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SubMessage.SlotOffset, 14);    
        }
        else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 1)
        {
    	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SubMessage.UTCHourAndMinute, 14);        
        }
        else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 2
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 4
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 6)
        {
    	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SubMessage.SlotNumber, 14);        
        }
        else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 3
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 5
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 7)
        {
    	    bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.SOTDMACommState.SubMessage.ReceivedStations, 14);        
        }    
    }
    else
    {
    	/*lq ITDMA */
        bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.ITDMACommState.SyncState, 2);
    	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.ITDMACommState.SlotIncrement, 13);
    	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.ITDMACommState.NumberOfSlots, 3);
    	bitmap_set_value(&msg_bitmap, pAISMsg->CommunicationState.ITDMACommState.KeepFlag, 1);    
    }

    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_BuildMsg27
* 描述  : 组建AIS消息27
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISMSG_BuildMsg27(const AISMsgTypeStruct_27* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff_27;
 	msg_bitmap.array_size = sizeof(AISMsgBinBuff_27);

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

	bitmap_set_value(&msg_bitmap, pAISMsg->MessageID, 6);
	bitmap_set_value(&msg_bitmap, pAISMsg->RepeatIndicator, 2);
	bitmap_set_value(&msg_bitmap, pAISMsg->UserID, 30);
	bitmap_set_value(&msg_bitmap, pAISMsg->PositionAccuracy, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->RAIMFlag, 1);
	bitmap_set_value(&msg_bitmap, pAISMsg->NavigationalStatus, 4);
	bitmap_set_value(&msg_bitmap, pAISMsg->Longitude, 18);
	bitmap_set_value(&msg_bitmap, pAISMsg->Latitude, 17);
	bitmap_set_value(&msg_bitmap, pAISMsg->SOG, 6);	
    bitmap_set_value(&msg_bitmap, pAISMsg->COG, 9);
	bitmap_set_value(&msg_bitmap, pAISMsg->StatusOfCurrentGNSSPos, 1);
	bitmap_set_value(&msg_bitmap, 0, 1);

    len_1 = msg_bitmap.index;
	len_2 = msg_bitmap.offset == 0 ? len_1 - 1 : len_1;
    	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;	
	bitmap_set_value(&msg_bitmap, len_2, 8);
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg1_2
* 描述  : 解析AIS消息1和2
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg1_2(AISMsgBinBuffArray_1_2_3 AISMsgBinBuff, AISMsgTypeStruct_1_2_3* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_1_2_3);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->UserID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 4);
    pAISMsg->NavigationalStatus = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 8);
    pAISMsg->RotAIS = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 10);
    pAISMsg->SOG = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->PositionAccuracy = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 28);
    pAISMsg->Longitude = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 27);
    pAISMsg->Latitude = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 12);
    pAISMsg->COG = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 9);
    pAISMsg->TrueHeading = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->TimeStamp = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->SpecialManoeuvreIndicator = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 3);//lq spare
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->RAIMFlag = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->CommunicationState.SOTDMACommState.SyncState = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 3);
    pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout = Temp32;
    if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 0)
    {
	    bitmap_get_value(&msg_bitmap, &Temp32, 14);    
        pAISMsg->CommunicationState.SOTDMACommState.SubMessage.SlotOffset = Temp32;
    }
    else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 1)
    {
	    bitmap_get_value(&msg_bitmap, &Temp32, 14);        
        pAISMsg->CommunicationState.SOTDMACommState.SubMessage.UTCHourAndMinute = Temp32;
    }
    else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 2
             || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 4
             || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 6)
    {
	    bitmap_get_value(&msg_bitmap, &Temp32, 14);        
        pAISMsg->CommunicationState.SOTDMACommState.SubMessage.SlotNumber = Temp32;
    }
    else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 3
             || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 5
             || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 7)
    {
	    bitmap_get_value(&msg_bitmap, &Temp32, 14);        
        pAISMsg->CommunicationState.SOTDMACommState.SubMessage.ReceivedStations = Temp32;
    }
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg3
* 描述  : 解析AIS消息3
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg3(AISMsgBinBuffArray_1_2_3 AISMsgBinBuff, AISMsgTypeStruct_1_2_3* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_1_2_3);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
	
    bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
	
    bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->UserID = Temp32;
	
    bitmap_get_value(&msg_bitmap, &Temp32, 4);
    pAISMsg->NavigationalStatus = Temp32;
	
    bitmap_get_value(&msg_bitmap, &Temp32, 8);
    pAISMsg->RotAIS = Temp32;
	
    bitmap_get_value(&msg_bitmap, &Temp32, 10);
    pAISMsg->SOG = Temp32;
	
    bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->PositionAccuracy = Temp32;
	
    bitmap_get_value(&msg_bitmap, &Temp32, 28);
    pAISMsg->Longitude = Temp32;
	
    bitmap_get_value(&msg_bitmap, &Temp32, 27);
    pAISMsg->Latitude = Temp32;
	
    bitmap_get_value(&msg_bitmap, &Temp32, 12);
    pAISMsg->COG = Temp32;
	
    bitmap_get_value(&msg_bitmap, &Temp32, 9);
    pAISMsg->TrueHeading = Temp32;
	
    bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->TimeStamp = Temp32;
	
    bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->SpecialManoeuvreIndicator = Temp32;
	
    bitmap_get_value(&msg_bitmap, &Temp32, 3);//lq spare
	
    bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->RAIMFlag = Temp32;
	
    bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->CommunicationState.ITDMACommState.SyncState = Temp32;
	
    bitmap_get_value(&msg_bitmap, &Temp32, 13);
    pAISMsg->CommunicationState.ITDMACommState.SlotIncrement = Temp32;
	
    bitmap_get_value(&msg_bitmap, &Temp32, 3);
    pAISMsg->CommunicationState.ITDMACommState.NumberOfSlots = Temp32;
	
    bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->CommunicationState.ITDMACommState.KeepFlag = Temp32;
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg4_11
* 描述  : 解析AIS消息4和11
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg4_11(AISMsgBinBuffArray_4_11 AISMsgBinBuff, AISMsgTypeStruct_4_11* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_4_11);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
    
    bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->UserID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 14);
    pAISMsg->UTCYear = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 4);
    pAISMsg->UTCMonth = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 5);
    pAISMsg->UTCDay = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 5);
    pAISMsg->UTCHour = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->UTCMinute = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->UTCSecond = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->PositionAccuracy = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 28);
    pAISMsg->Longitude = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 27);
    pAISMsg->Latitude = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 4);
    pAISMsg->TypeOfEPFD = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->TxCtrlForLrbm = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 9);    //lq spare
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->RAIMFlag = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->CommunicationState.SyncState = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 3);
    pAISMsg->CommunicationState.SlotTimeout = Temp32;
    if (pAISMsg->CommunicationState.SlotTimeout == 0)
    {
	    bitmap_get_value(&msg_bitmap, &Temp32, 14);    
        pAISMsg->CommunicationState.SubMessage.SlotOffset = Temp32;
    }
    else if (pAISMsg->CommunicationState.SlotTimeout == 1)
    {
	    bitmap_get_value(&msg_bitmap, &Temp32, 14);        
        pAISMsg->CommunicationState.SubMessage.UTCHourAndMinute = Temp32;
    }
    else if (pAISMsg->CommunicationState.SlotTimeout == 2
             || pAISMsg->CommunicationState.SlotTimeout == 4
             || pAISMsg->CommunicationState.SlotTimeout == 6)
    {
	    bitmap_get_value(&msg_bitmap, &Temp32, 14);        
        pAISMsg->CommunicationState.SubMessage.SlotNumber = Temp32;
    }
    else if (pAISMsg->CommunicationState.SlotTimeout == 3
             || pAISMsg->CommunicationState.SlotTimeout == 5
             || pAISMsg->CommunicationState.SlotTimeout == 7)
    {
	    bitmap_get_value(&msg_bitmap, &Temp32, 14); 
        pAISMsg->CommunicationState.SubMessage.ReceivedStations = Temp32;       
    }
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg5
* 描述  : 解析AIS消息5
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg5(AISMsgBinBuffArray_5 AISMsgBinBuff, AISMsgTypeStruct_5* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_5);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
    
    bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->UserID = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->AISVersionIndicator = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->IMONumber = Temp32;
    for (i = 0; i < 7; i++)
    {
        bitmap_get_value(&msg_bitmap, &Temp32, 6);
        if ((Temp32 & 0x20) == 0)
        {
            Temp32 |= 0x40;     //lq 6-bit ASCII转 8-bit ASCII        
        }            
        pAISMsg->CallSign[i] = Temp32;
    }
    for (i = 0; i < 20; i++)
    {
        bitmap_get_value(&msg_bitmap, &Temp32, 6);
        if ((Temp32 & 0x20) == 0)
        {
            Temp32 |= 0x40;     //lq 6-bit ASCII转 8-bit ASCII        
        }            
        pAISMsg->Name[i] = Temp32;
    }
    bitmap_get_value(&msg_bitmap, &Temp32, 8);    
    pAISMsg->TypeOfShipAndCargo = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 9);    
    pAISMsg->DimensionA = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 9);    
    pAISMsg->DimensionB = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 6);    
    pAISMsg->DimensionC = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 6);    
    pAISMsg->DimensionD = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 4);    
    pAISMsg->TypeOfEPFD = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 4);    
    pAISMsg->ETAMonth = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 5);    
    pAISMsg->ETADay = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 5);    
    pAISMsg->ETAHour = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 6);    
    pAISMsg->ETAMinute = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 8);    
    pAISMsg->MaxPresentStaticDraught = Temp32;
    for (i = 0; i < 20; i++)
    {
        bitmap_get_value(&msg_bitmap, &Temp32, 6);
        if ((Temp32 & 0x20) == 0)
        {
            Temp32 |= 0x40;     //lq 6-bit ASCII转 8-bit ASCII        
        }            
        pAISMsg->Destination[i] = Temp32;
    }
    bitmap_get_value(&msg_bitmap, &Temp32, 1);    
    pAISMsg->DTE = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 1);    //lq spare    
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg6
* 描述  : 解析AIS消息6
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg6(AISMsgBinBuffArray_6 AISMsgBinBuff, AISMsgTypeStruct_6* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;
    U8 Temp8;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_6);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
    Temp8 = Temp32; //lq 该消息所占字节总数
    bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->SourceID = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->SequenceNumber = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->DestinationID = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->RetransmitFlag = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 1);    //lq spare
    bitmap_get_value(&msg_bitmap, &Temp32, 10);
    pAISMsg->AI_DAC = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->AI_FI = Temp32;
    pAISMsg->AppDataBitLen = (Temp8 * 8) - 88;   
    if (pAISMsg->AppDataBitLen > AIS_MSG6_APPDATA_BIT_MAXLEN)
    {
        /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/
        pAISMsg->AppDataBitLen = AIS_MSG6_APPDATA_BIT_MAXLEN;      
    }
    Temp = pAISMsg->AppDataBitLen / 8;
    for (i = 0; i < Temp; i++)
    {
        bitmap_get_value(&msg_bitmap, &Temp32, 8);    
        pAISMsg->ApplicationData[i] = Temp32;
    }
    Temp = pAISMsg->AppDataBitLen % 8;
    if (Temp != 0)
    {
        bitmap_get_value(&msg_bitmap, &Temp32, Temp);
        pAISMsg->ApplicationData[i] = Temp32;
        bitmap_get_value(&msg_bitmap, &Temp32, 8 - Temp);     //lq 字节边界                    
    }        
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg7_13
* 描述  : 解析AIS消息7和13
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg7_13(AISMsgBinBuffArray_7_13 AISMsgBinBuff, AISMsgTypeStruct_7_13* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;
    U8 Temp8;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_7_13);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
    Temp8 = Temp32; //lq 该消息所占字节总数
    bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->SourceID = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 2); //lq spare

    bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->DestinationID1 = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 2);    
    pAISMsg->SequenceNumber1 = Temp32;
    pAISMsg->AckType = AIS_MSG7_13_ACK_TYPE_1;

    if (Temp8 > 9)
    {
        bitmap_get_value(&msg_bitmap, &Temp32, 30);
        pAISMsg->DestinationID2 = Temp32;
        bitmap_get_value(&msg_bitmap, &Temp32, 2);
        pAISMsg->SequenceNumber2 = Temp32;
        pAISMsg->AckType = AIS_MSG7_13_ACK_TYPE_2;              
    }

    if (Temp8 > 13)
    {
        bitmap_get_value(&msg_bitmap, &Temp32, 30);
        pAISMsg->DestinationID3 = Temp32;
        bitmap_get_value(&msg_bitmap, &Temp32, 2);
        pAISMsg->SequenceNumber3 = Temp32;
        pAISMsg->AckType = AIS_MSG7_13_ACK_TYPE_3;              
    }

    if (Temp8 > 17)
    {
        bitmap_get_value(&msg_bitmap, &Temp32, 30);
        pAISMsg->DestinationID4 = Temp32;
        bitmap_get_value(&msg_bitmap, &Temp32, 2);
        pAISMsg->SequenceNumber4 = Temp32;
        pAISMsg->AckType = AIS_MSG7_13_ACK_TYPE_4;              
    }      
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg8
* 描述  : 解析AIS消息8
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg8(AISMsgBinBuffArray_8 AISMsgBinBuff, AISMsgTypeStruct_8* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;
    U8 Temp8;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_8);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
    Temp8 = Temp32; //lq 该消息所占字节总数
    bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->SourceID = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 2); //lq spare
    bitmap_get_value(&msg_bitmap, &Temp32, 10);
    pAISMsg->AI_DAC = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->AI_FI = Temp32;
    pAISMsg->AppDataBitLen = (Temp8 * 8) - 56;   
    if (pAISMsg->AppDataBitLen > AIS_MSG8_APPDATA_BIT_MAXLEN)
    {
        /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/
        pAISMsg->AppDataBitLen = AIS_MSG8_APPDATA_BIT_MAXLEN;      
    }
    Temp = pAISMsg->AppDataBitLen / 8;
    for (i = 0; i < Temp; i++)
    {
        bitmap_get_value(&msg_bitmap, &Temp32, 8);    
        pAISMsg->ApplicationData[i] = Temp32;
    }
    Temp = pAISMsg->AppDataBitLen % 8;
    if (Temp != 0)
    {
        bitmap_get_value(&msg_bitmap, &Temp32, Temp);
        pAISMsg->ApplicationData[i] = Temp32;
        bitmap_get_value(&msg_bitmap, &Temp32, 8 - Temp);     //lq 字节边界                    
    }        
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg9
* 描述  : 解析AIS消息9
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg9(AISMsgBinBuffArray_9 AISMsgBinBuff, AISMsgTypeStruct_9* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;
    U8 Temp8;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_9);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
	
    bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->UserID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 12);
    pAISMsg->Altitude = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 10);
    pAISMsg->SOG = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->PositionAccuracy = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 28);
    pAISMsg->Longitude = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 27);
    pAISMsg->Latitude = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 12);
    pAISMsg->COG = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->TimeStamp = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->AltitudeSensor = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 7); //lq spare
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->DTE = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 3); //lq spare
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->AssignedModeFlag = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->RAIMFlag = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->CommStateSelectFlag = Temp32;
    if (pAISMsg->CommStateSelectFlag == 0)
    {
    	/*lq SOTDMA */
        bitmap_get_value(&msg_bitmap, &Temp32, 2);
        pAISMsg->CommunicationState.SOTDMACommState.SyncState = Temp32;
    	bitmap_get_value(&msg_bitmap, &Temp32, 3);
        pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout = Temp32;
        if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 0)
        {
    	    bitmap_get_value(&msg_bitmap, &Temp32, 14);    
            pAISMsg->CommunicationState.SOTDMACommState.SubMessage.SlotOffset = Temp32;
        }
        else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 1)
        {
    	    bitmap_get_value(&msg_bitmap, &Temp32, 14);        
            pAISMsg->CommunicationState.SOTDMACommState.SubMessage.UTCHourAndMinute = Temp32;
        }
        else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 2
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 4
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 6)
        {
    	    bitmap_get_value(&msg_bitmap, &Temp32, 14);        
            pAISMsg->CommunicationState.SOTDMACommState.SubMessage.SlotNumber = Temp32;
        }
        else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 3
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 5
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 7)
        {
    	    bitmap_get_value(&msg_bitmap, &Temp32, 14);        
            pAISMsg->CommunicationState.SOTDMACommState.SubMessage.ReceivedStations = Temp32;
        }    
    }
    else
    {
    	/*lq ITDMA */
        bitmap_get_value(&msg_bitmap, &Temp32, 2);
        pAISMsg->CommunicationState.ITDMACommState.SyncState = Temp32;
    	bitmap_get_value(&msg_bitmap, &Temp32, 13);
        pAISMsg->CommunicationState.ITDMACommState.SlotIncrement = Temp32;
    	bitmap_get_value(&msg_bitmap, &Temp32, 3);
        pAISMsg->CommunicationState.ITDMACommState.NumberOfSlots = Temp32;
    	bitmap_get_value(&msg_bitmap, &Temp32, 1);    
        pAISMsg->CommunicationState.ITDMACommState.KeepFlag = Temp32;
    }
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg10
* 描述  : 解析AIS消息10
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg10(AISMsgBinBuffArray_10 AISMsgBinBuff, AISMsgTypeStruct_10* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;
    U8 Temp8;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_10);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
	
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->SourceID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2); //lq spare
	bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->DestinationID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2); //lq spare
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg12
* 描述  : 解析AIS消息12
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg12(AISMsgBinBuffArray_12 AISMsgBinBuff, AISMsgTypeStruct_12* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;
    U8 Temp8;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_12);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
    Temp8 = Temp32; //lq 该消息所占字节总数
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->SourceID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->SequenceNumber = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->DestinationID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->RetransmitFlag = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1); //lq spare
    pAISMsg->TextLen = ((Temp8 * 8) - 72) / 6;
    if ((((Temp8 * 8) - 72) % 6) != 0)
    {
        pAISMsg->TextLen++;           
    }
           
    if (pAISMsg->TextLen > AIS_MSG12_TEXT_MAXLEN)
    {
        /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/        
        pAISMsg->TextLen = AIS_MSG12_TEXT_MAXLEN;     
    }

    for (i = 0; i < pAISMsg->TextLen; i++)
    {
	    bitmap_get_value(&msg_bitmap, &Temp32, 6);
        if ((Temp32 & 0x20) == 0)
        {
            Temp32 |= 0x40;     //lq 6-bit ASCII转 8-bit ASCII        
        }
        pAISMsg->SafetyRelatedText[i] = Temp32;
    }       
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg14
* 描述  : 解析AIS消息14
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg14(AISMsgBinBuffArray_14 AISMsgBinBuff, AISMsgTypeStruct_14* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;
    U8 Temp8;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_14);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
    Temp8 = Temp32; //lq 该消息所占字节总数
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->SourceID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2); //lq spare
    pAISMsg->TextLen = ((Temp8 * 8) - 40) / 6;
    if ((((Temp8 * 8) - 40) % 6) != 0)
    {
        pAISMsg->TextLen++;    
    }    
    if (pAISMsg->TextLen > AIS_MSG14_TEXT_MAXLEN)
    {
        /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/        
        pAISMsg->TextLen = AIS_MSG14_TEXT_MAXLEN;   
    }

    for (i = 0; i < pAISMsg->TextLen; i++)
    {
	    bitmap_get_value(&msg_bitmap, &Temp32, 6);
        if ((Temp32 & 0x20) == 0)
        {
            Temp32 |= 0x40;     //lq 6-bit ASCII转 8-bit ASCII        
        }            
        pAISMsg->SafetyRelatedText[i] = Temp32;
    }                
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg15
* 描述  : 解析AIS消息15
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg15(AISMsgBinBuffArray_15 AISMsgBinBuff, AISMsgTypeStruct_15* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;
    U8 Temp8;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_15);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
    Temp8 = Temp32; //lq 该消息所占字节总数
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->SourceID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2); //lq spare
    bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->DestinationID1 = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID1_1 = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 12);
    pAISMsg->SlotOffset1_1 = Temp32;
    pAISMsg->InterrogationType = AIS_MSG15_INTEEROGATION_TYPE_1;

    if (Temp8 > 11)
    {
        bitmap_get_value(&msg_bitmap, &Temp32, 2); //lq spare
        bitmap_get_value(&msg_bitmap, &Temp32, 6);
        pAISMsg->MessageID1_2 = Temp32;
        bitmap_get_value(&msg_bitmap, &Temp32, 12);    
        pAISMsg->SlotOffset1_2 = Temp32;
        pAISMsg->InterrogationType = AIS_MSG15_INTEEROGATION_TYPE_2;
    }

    if (Temp8 > 14)
    {
        bitmap_get_value(&msg_bitmap, &Temp32, 2); //lq spare
        bitmap_get_value(&msg_bitmap, &Temp32, 30);
        pAISMsg->DestinationID2 = Temp32;
        bitmap_get_value(&msg_bitmap, &Temp32, 6);
        pAISMsg->MessageID2_1 = Temp32;
        bitmap_get_value(&msg_bitmap, &Temp32, 12);
        pAISMsg->SlotOffset2_1 = Temp32;
        bitmap_get_value(&msg_bitmap, &Temp32, 2); //lq spare

        if ((pAISMsg->MessageID1_2 == 0) && (pAISMsg->SlotOffset1_2 == 0))
        {
            pAISMsg->InterrogationType = AIS_MSG15_INTEEROGATION_TYPE_3;        
        }
        else
        {
            pAISMsg->InterrogationType = AIS_MSG15_INTEEROGATION_TYPE_4;                
        }
    }             
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg16
* 描述  : 解析AIS消息16
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg16(AISMsgBinBuffArray_16 AISMsgBinBuff, AISMsgTypeStruct_16* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;
    U8 Temp8;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_16);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
    Temp8 = Temp32; //lq 该消息所占字节总数
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->SourceID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2); //lq spare
    bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->DestinationA = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 12);
    pAISMsg->OffsetA = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 10);
    pAISMsg->IncrementA = Temp32;
    pAISMsg->CommandType = AIS_MSG16_COMMAND_TYPE_1;

    if (Temp8 > 12)
    {
        bitmap_get_value(&msg_bitmap, &Temp32, 30);
        pAISMsg->DestinationB = Temp32;
        bitmap_get_value(&msg_bitmap, &Temp32, 12);
        pAISMsg->OffsetB = Temp32;
        bitmap_get_value(&msg_bitmap, &Temp32, 10);
        pAISMsg->IncrementB = Temp32;
        pAISMsg->CommandType = AIS_MSG16_COMMAND_TYPE_2;            
    }          
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg17
* 描述  : 解析AIS消息17
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg17(AISMsgBinBuffArray_17 AISMsgBinBuff, AISMsgTypeStruct_17* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;
    U8 Temp8;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_17);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
    Temp8 = Temp32; //lq 该消息所占字节总数
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->SourceID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2); //lq spare
	bitmap_get_value(&msg_bitmap, &Temp32, 18);
    pAISMsg->Longitude = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 17);
    pAISMsg->Latitude = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 5); //lq spare

    if (Temp8 > 10)
    {
        bitmap_get_value(&msg_bitmap, &Temp32, 6);
        pAISMsg->MessageType = Temp32;
    	bitmap_get_value(&msg_bitmap, &Temp32, 10);
        pAISMsg->StationID = Temp32;
    	bitmap_get_value(&msg_bitmap, &Temp32, 13);
        pAISMsg->ZCount = Temp32;
    	bitmap_get_value(&msg_bitmap, &Temp32, 3);
        pAISMsg->SequenceNumber = Temp32;
        bitmap_get_value(&msg_bitmap, &Temp32, 5);
        pAISMsg->N = Temp32;
        bitmap_get_value(&msg_bitmap, &Temp32, 3);    
        pAISMsg->Health = Temp32;
        
        /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/        
        if (pAISMsg->N > 29)
        {
            pAISMsg->N = 29;
        }
        for (i = 0; i < pAISMsg->N; i++)
        {
            bitmap_get_value(&msg_bitmap, &Temp32, 24);
            pAISMsg->DGNSSDataWord[i] = Temp32;        
        } 
    }        
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg18
* 描述  : 解析AIS消息18
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg18(AISMsgBinBuffArray_18 AISMsgBinBuff, AISMsgTypeStruct_18* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;
    U8 Temp8;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_18);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->UserID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 8); //lq spare
	bitmap_get_value(&msg_bitmap, &Temp32, 10);
    pAISMsg->SOG = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->PositionAccuracy = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 28);
    pAISMsg->Longitude = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 27);
    pAISMsg->Latitude = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 12);
    pAISMsg->COG = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 9);
    pAISMsg->TrueHeading = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->TimeStamp = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2); //lq spare
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->ClassBUnitFlag = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->ClassBDisplayFlag = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->ClassBDSCFlag = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->ClassBBandFlag = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->ClassBMsg22Flag = Temp32;
 	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->ModeFlag = Temp32;
 	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->RAIMFlag = Temp32;
 	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->CommStateSelectFlag = Temp32;
    if (pAISMsg->CommStateSelectFlag == 0)
    {
    	/*lq SOTDMA */
        bitmap_get_value(&msg_bitmap, &Temp32, 2);
        pAISMsg->CommunicationState.SOTDMACommState.SyncState = Temp32;
    	bitmap_get_value(&msg_bitmap, &Temp32, 3);
        pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout = Temp32;
        if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 0)
        {
    	    bitmap_get_value(&msg_bitmap, &Temp32, 14);    
            pAISMsg->CommunicationState.SOTDMACommState.SubMessage.SlotOffset = Temp32;
        }
        else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 1)
        {
    	    bitmap_get_value(&msg_bitmap, &Temp32, 14);        
            pAISMsg->CommunicationState.SOTDMACommState.SubMessage.UTCHourAndMinute = Temp32;
        }
        else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 2
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 4
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 6)
        {
    	    bitmap_get_value(&msg_bitmap, &Temp32, 14);        
            pAISMsg->CommunicationState.SOTDMACommState.SubMessage.SlotNumber = Temp32;
        }
        else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 3
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 5
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 7)
        {
    	    bitmap_get_value(&msg_bitmap, &Temp32, 14);        
            pAISMsg->CommunicationState.SOTDMACommState.SubMessage.ReceivedStations = Temp32;
        }    
    }
    else
    {
    	/*lq ITDMA */
        bitmap_get_value(&msg_bitmap, &Temp32, 2);
        pAISMsg->CommunicationState.ITDMACommState.SyncState = Temp32;
    	bitmap_get_value(&msg_bitmap, &Temp32, 13);
        pAISMsg->CommunicationState.ITDMACommState.SlotIncrement = Temp32;
    	bitmap_get_value(&msg_bitmap, &Temp32, 3);
        pAISMsg->CommunicationState.ITDMACommState.NumberOfSlots = Temp32;
    	bitmap_get_value(&msg_bitmap, &Temp32, 1);    
        pAISMsg->CommunicationState.ITDMACommState.KeepFlag = Temp32;
    }
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg19
* 描述  : 解析AIS消息19
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg19(AISMsgBinBuffArray_19 AISMsgBinBuff, AISMsgTypeStruct_19* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;
    U8 Temp8;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_19);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->UserID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 8); //lq spare
	bitmap_get_value(&msg_bitmap, &Temp32, 10);
    pAISMsg->SOG = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->PositionAccuracy = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 28);
    pAISMsg->Longitude = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 27);
    pAISMsg->Latitude = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 12);
    pAISMsg->COG = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 9);
    pAISMsg->TrueHeading = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->TimeStamp = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 4); //lq spare
    for (i = 0; i < 20; i++)
    {
        bitmap_get_value(&msg_bitmap, &Temp32, 6);
        if ((Temp32 & 0x20) == 0)
        {
            Temp32 |= 0x40;     //lq 6-bit ASCII转 8-bit ASCII        
        }
        pAISMsg->Name[i] = Temp32;     
    }
	bitmap_get_value(&msg_bitmap, &Temp32, 8);
    pAISMsg->TypeOfShipAndCargo = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 9);
    pAISMsg->DimensionA = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 9);
    pAISMsg->DimensionB = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->DimensionC = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->DimensionD = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 4);
    pAISMsg->TypeOfEPFD = Temp32;
 	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->RAIMFlag = Temp32;
 	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->DTE = Temp32;
 	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->AssignedModeFlag = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 4); //lq spare
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg20
* 描述  : 解析AIS消息20
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg20(AISMsgBinBuffArray_20 AISMsgBinBuff, AISMsgTypeStruct_20* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;
    U8 Temp8;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_20);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
    Temp8 = Temp32; //lq 该消息所占字节总数
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->SourceStationID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2); //lq spare
    bitmap_get_value(&msg_bitmap, &Temp32, 12);
    pAISMsg->OffsetNumber1 = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 4);
    pAISMsg->NumberOfSlot1 = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 3);
    pAISMsg->Timeout1 = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 11);
    pAISMsg->Increment1 = Temp32;
    pAISMsg->ManagementType = AIS_MSG20_MANAGEMENT_TYPE_1;
    
    if (Temp8 > 9)
    {
        bitmap_get_value(&msg_bitmap, &Temp32, 12);
        pAISMsg->OffsetNumber2 = Temp32;
        bitmap_get_value(&msg_bitmap, &Temp32, 4);
        pAISMsg->NumberOfSlot2 = Temp32;
        bitmap_get_value(&msg_bitmap, &Temp32, 3);
        pAISMsg->Timeout2 = Temp32;
        bitmap_get_value(&msg_bitmap, &Temp32, 11);    
        pAISMsg->Increment2 = Temp32;
        pAISMsg->ManagementType = AIS_MSG20_MANAGEMENT_TYPE_2;
    }

    if (Temp8 > 13)
    {
        bitmap_get_value(&msg_bitmap, &Temp32, 12);
        pAISMsg->OffsetNumber3 = Temp32;
        bitmap_get_value(&msg_bitmap, &Temp32, 4);
        pAISMsg->NumberOfSlot3 = Temp32;
        bitmap_get_value(&msg_bitmap, &Temp32, 3);
        pAISMsg->Timeout3 = Temp32;
        bitmap_get_value(&msg_bitmap, &Temp32, 11);    
        pAISMsg->Increment3 = Temp32;
        pAISMsg->ManagementType = AIS_MSG20_MANAGEMENT_TYPE_3;
    }

    if (Temp8 > 17)
    {
        bitmap_get_value(&msg_bitmap, &Temp32, 12);
        pAISMsg->OffsetNumber4 = Temp32;
        bitmap_get_value(&msg_bitmap, &Temp32, 4);
        pAISMsg->NumberOfSlot4 = Temp32;
        bitmap_get_value(&msg_bitmap, &Temp32, 3);
        pAISMsg->Timeout4 = Temp32;
        bitmap_get_value(&msg_bitmap, &Temp32, 11);    
        pAISMsg->Increment4 = Temp32;
        pAISMsg->ManagementType = AIS_MSG20_MANAGEMENT_TYPE_4;
    }        
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg21
* 描述  : 解析AIS消息21
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg21(AISMsgBinBuffArray_21 AISMsgBinBuff, AISMsgTypeStruct_21* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;
    U8 Temp8;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_21);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
    Temp8 = Temp32; //lq 该消息所占字节总数
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->ID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 5);
    pAISMsg->TypeOfAtoN = Temp32;
    for (i = 0; i < 20; i++)
    {
	    bitmap_get_value(&msg_bitmap, &Temp32, 6);    
        if ((Temp32 & 0x20) == 0)
        {
            Temp32 |= 0x40;     //lq 6-bit ASCII转 8-bit ASCII        
        }        
        pAISMsg->NameOfAtoN[i] = Temp32;
    }
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->PositionAccuracy = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 28);
    pAISMsg->Longitude = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 27);
    pAISMsg->Latitude = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 9);
    pAISMsg->DimensionA = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 9);
    pAISMsg->DimensionB = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->DimensionC = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->DimensionD = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 4);
    pAISMsg->TypeOfEPFD = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->TimeStamp = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->OffPositionIndicator = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 8);
    pAISMsg->AtoNStatus = Temp32;
 	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->RAIMFlag = Temp32;
 	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->VirtualAtoNFlag = Temp32;
 	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->AssignedModeFlag = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1); //lq spare

    /*lq 判断有无NameOfAtoNExtension */
    if (((Temp8 * 8) - 272) > 0)
    {
        pAISMsg->NameOfAtoNExtensionLen = ((Temp8 * 8) - 272) / 6;
        if (pAISMsg->NameOfAtoNExtensionLen > 14)
        {
            pAISMsg->NameOfAtoNExtensionLen = 14;
        }
    
        for (i = 0; i < pAISMsg->NameOfAtoNExtensionLen; i++)
        {
            bitmap_get_value(&msg_bitmap, &Temp32, 6);                
            if ((Temp32 & 0x20) == 0)
            {
                Temp32 |= 0x40;     //lq 6-bit ASCII转 8-bit ASCII        
            }            
            pAISMsg->NameOfAtoNExtension[i] = Temp32;
        }
        
        /*lq 若最后一个字符为‘@’，则该‘@’是为保持字节边界而添加的6bit0，应删除 */ 
        if (pAISMsg->NameOfAtoNExtension[i - 1] == '@')
        {
            pAISMsg->NameOfAtoNExtensionLen--;        
        }   
    }       
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg22
* 描述  : 解析AIS消息22
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg22(AISMsgBinBuffArray_22 AISMsgBinBuff, AISMsgTypeStruct_22* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;
    U8 Temp8;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_22);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
    bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->StationID = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 2); //lq spare
    bitmap_get_value(&msg_bitmap, &Temp32, 12);
    pAISMsg->ChannelA = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 12);
    pAISMsg->ChannelB = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 4);
    pAISMsg->TxRxMode = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->Power = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 18);
    pAISMsg->LonOrMSBOfAddID1 = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 17);
    pAISMsg->LatOrLSBOfAddID1 = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 18);
    pAISMsg->LonOrMSBOfAddID2 = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 17);
    pAISMsg->LatOrLSBOfAddID2 = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->AddOrBroadMsgIndicator = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->ChannelABandwidth = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->ChannelBBandwidth = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 3);
    pAISMsg->TransitionalZoneSize = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 23);    //lq spare
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg23
* 描述  : 解析AIS消息23
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg23(AISMsgBinBuffArray_23 AISMsgBinBuff, AISMsgTypeStruct_23* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;
    U8 Temp8;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_23);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
    bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->SourceID = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 2); //lq spare
    bitmap_get_value(&msg_bitmap, &Temp32, 18);
    pAISMsg->Longitude1 = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 17);
    pAISMsg->Latitude1 = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 18);
    pAISMsg->Longitude2 = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 17);
    pAISMsg->Latitude2 = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 4);
    pAISMsg->StationType = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 8);
    pAISMsg->TypeOfShipAndCargo = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 22);//lq spare
    bitmap_get_value(&msg_bitmap, &Temp32, 4);
    pAISMsg->TxRxMode = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 4);
    pAISMsg->ReportingInterval = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 4);
    pAISMsg->QuietTime = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 6); //lq spare
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg24A
* 描述  : 解析AIS消息24A
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg24A(AISMsgBinBuffArray_24A AISMsgBinBuff, AISMsgTypeStruct_24A* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;
    U8 Temp8;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_24A);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
    bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->UserID = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->PartNumber = Temp32;
    for (i = 0; i < 20; i++)
    {
        bitmap_get_value(&msg_bitmap, &Temp32, 6);    
        if ((Temp32 & 0x20) == 0)
        {
            Temp32 |= 0x40;     //lq 6-bit ASCII转 8-bit ASCII        
        }         
        pAISMsg->Name[i] = Temp32;
    }
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg24B
* 描述  : 解析AIS消息24B
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg24B(AISMsgBinBuffArray_24B AISMsgBinBuff, AISMsgTypeStruct_24B* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;
    U8 Temp8;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_24B);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->UserID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->PartNumber = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 8);
    pAISMsg->TypeOfShipAndCargo = Temp32;
    for (i = 0; i < 3; i++)
    {
        bitmap_get_value(&msg_bitmap, &Temp32, 6);    
        if ((Temp32 & 0x20) == 0)
        {
            Temp32 |= 0x40;     //lq 6-bit ASCII转 8-bit ASCII        
        }         
        pAISMsg->VendorID.ManufacturerID[i] = Temp32;
    }
    bitmap_get_value(&msg_bitmap, &Temp32, 4);    
    pAISMsg->VendorID.UnitModelCode = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 20);    
    pAISMsg->VendorID.UnitSerialNumber = Temp32;

    for (i = 0; i < 7; i++)
    {
        bitmap_get_value(&msg_bitmap, &Temp32, 6);
        if ((Temp32 & 0x20) == 0)
        {
            Temp32 |= 0x40;     //lq 6-bit ASCII转 8-bit ASCII        
        }            
        pAISMsg->CallSign[i] = Temp32;
    }
	bitmap_get_value(&msg_bitmap, &Temp32, 9);
    pAISMsg->DimensionA = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 9);
    pAISMsg->DimensionB = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->DimensionC = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->DimensionD = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 6); //lq spare
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg25
* 描述  : 解析AIS消息25
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg25(AISMsgBinBuffArray_25 AISMsgBinBuff, AISMsgTypeStruct_25* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;
    U8 Temp8;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_25);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
    Temp8 = Temp32; //lq 该消息所占字节总数	
    bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->SourceID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->DestinationIndicator = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->BinaryDataFlag = Temp32;
    if (pAISMsg->DestinationIndicator == 0)
    {
        /*lq broadcast */        
        if (pAISMsg->BinaryDataFlag == 0)
        {
            /*lq unstructed binary data */
            pAISMsg->BinaryDataBitLen = (Temp8 * 8) - 40;
            if (pAISMsg->BinaryDataBitLen > AIS_MSG25_BINDATA_BIT_MAXLEN_00)
            {
                /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/        
                pAISMsg->BinaryDataBitLen = AIS_MSG25_BINDATA_BIT_MAXLEN_00;
            }

            Temp = pAISMsg->BinaryDataBitLen / 8;
            for (i = 0; i < Temp; i++)
            {
                bitmap_get_value(&msg_bitmap, &Temp32, 8);                                   
                pAISMsg->BinaryData.UnstructedBinData[i] = Temp32;
            }
            Temp = pAISMsg->BinaryDataBitLen % 8;
            if (Temp != 0)
            {
                bitmap_get_value(&msg_bitmap, &Temp32, Temp);                                                   	                
                pAISMsg->BinaryData.UnstructedBinData[i] = Temp32;
                //lq bitmap_get_value(&msg_bitmap, 0, 8 - Temp); //lq 字节边界处理（可以不用，因为msg_bitmap.array未设置的bit默认为0）
            }
        }
        else
        {
            /*lq structed binary data AI */
	        bitmap_get_value(&msg_bitmap, &Temp32, 10);                                   
            pAISMsg->BinaryData.StructedBinData.AI_DAC = Temp32;
	        bitmap_get_value(&msg_bitmap, &Temp32, 6);                                   
            pAISMsg->BinaryData.StructedBinData.AI_FI = Temp32;

            /*lq structed binary data App Data */
            pAISMsg->BinaryDataBitLen = (Temp8 * 8) - 40;
            pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen = pAISMsg->BinaryDataBitLen - 16;
            if (pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen > AIS_MSG25_BINDATA_BIT_MAXLEN_01)
            {
                /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/        
                pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen = AIS_MSG25_BINDATA_BIT_MAXLEN_01;
                pAISMsg->BinaryDataBitLen = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen + 16;            
            }

            Temp = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen / 8;
            for (i = 0; i < Temp; i++)
            {
                bitmap_get_value(&msg_bitmap, &Temp32, 8);                                   
                pAISMsg->BinaryData.StructedBinData.ApplicationData[i] = Temp32;
            }
            Temp = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen % 8;
            if (Temp != 0)
            {
                bitmap_get_value(&msg_bitmap, &Temp32, Temp);                                                   	                
                pAISMsg->BinaryData.StructedBinData.ApplicationData[i] = Temp32;
                //lq bitmap_get_value(&msg_bitmap, 0, 8 - Temp); //lq 字节边界处理（可以不用，因为msg_bitmap.array未设置的bit默认为0）                                                   
            }
        }    
    }
    else
    {
        /*lq addressed */
	    /*lq destination id */
        bitmap_get_value(&msg_bitmap, &Temp32, 30);                
        pAISMsg->DestinationID = Temp32;
                
        /*lq binary data */
        if (pAISMsg->BinaryDataFlag == 0)
        {
            /*lq unstructed binary data */
            pAISMsg->BinaryDataBitLen = (Temp8 * 8) - 70;
            if (pAISMsg->BinaryDataBitLen > AIS_MSG25_BINDATA_BIT_MAXLEN_10)
            {
                /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/        
                pAISMsg->BinaryDataBitLen = AIS_MSG25_BINDATA_BIT_MAXLEN_10;
            }

            Temp = pAISMsg->BinaryDataBitLen / 8;
            for (i = 0; i < Temp; i++)
            {
                bitmap_get_value(&msg_bitmap, &Temp32, 8);                                   
                pAISMsg->BinaryData.UnstructedBinData[i] = Temp32;
            }
            Temp = pAISMsg->BinaryDataBitLen % 8;
            if (Temp != 0)
            {
                bitmap_get_value(&msg_bitmap, &Temp32, Temp);                                                   	                
                pAISMsg->BinaryData.UnstructedBinData[i] = Temp32;
                //lq bitmap_get_value(&msg_bitmap, 0, 8 - Temp); //lq 字节边界处理（可以不用，因为msg_bitmap.array未设置的bit默认为0）                                                  
            }
        }
        else
        {
            /*lq structed binary data AI */
	        bitmap_get_value(&msg_bitmap, &Temp32, 10);                                   
            pAISMsg->BinaryData.StructedBinData.AI_DAC = Temp32;
	        bitmap_get_value(&msg_bitmap, &Temp32, 6);                                   
            pAISMsg->BinaryData.StructedBinData.AI_FI = Temp32;

            /*lq structed binary data App Data */
            pAISMsg->BinaryDataBitLen = (Temp8 * 8) - 70;
            pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen = pAISMsg->BinaryDataBitLen - 16;
            if (pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen > AIS_MSG25_BINDATA_BIT_MAXLEN_11)
            {
                /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/        
                pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen = AIS_MSG25_BINDATA_BIT_MAXLEN_11;
                pAISMsg->BinaryDataBitLen = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen + 16;            
            }

            Temp = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen / 8;
            for (i = 0; i < Temp; i++)
            {
                bitmap_get_value(&msg_bitmap, &Temp32, 8);                                   
                pAISMsg->BinaryData.StructedBinData.ApplicationData[i] = Temp32;
            }
            Temp = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen % 8;
            if (Temp != 0)
            {
                bitmap_get_value(&msg_bitmap, &Temp32, Temp);                                                   	                
                pAISMsg->BinaryData.StructedBinData.ApplicationData[i] = Temp32;
            }
        }     
    }
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg26
* 描述  : 解析AIS消息26
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg26(AISMsgBinBuffArray_26 AISMsgBinBuff, AISMsgTypeStruct_26* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;
    U8 Temp8;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_26);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
    Temp8 = Temp32; //lq 该消息所占字节总数	
    
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->SourceID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->DestinationIndicator = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->BinaryDataFlag = Temp32;
    if (pAISMsg->DestinationIndicator == 0)
    {
        /*lq broadcast */        
        if (pAISMsg->BinaryDataFlag == 0)
        {
            /*lq unstructed binary data */
            pAISMsg->BinaryDataBitLen = (Temp8 * 8) - 60;   //lq 含有为保护整条消息的字节边界而填充的0，但无法识别那些事填充0，需由自定义协议指定                    
            if (pAISMsg->BinaryDataBitLen > AIS_MSG26_BINDATA_BIT_MAXLEN_00)
            {
                /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/                
                pAISMsg->BinaryDataBitLen = AIS_MSG26_BINDATA_BIT_MAXLEN_00;            
            }

            Temp = pAISMsg->BinaryDataBitLen / 8;
            for (i = 0; i < Temp; i++)
            {
                bitmap_get_value(&msg_bitmap, &Temp32, 8);                                   
                pAISMsg->BinaryData.UnstructedBinData[i] = Temp32;
            }
            Temp = pAISMsg->BinaryDataBitLen % 8;
            if (Temp != 0)
            {
                bitmap_get_value(&msg_bitmap, &Temp32, Temp);                                                   	                
                pAISMsg->BinaryData.UnstructedBinData[i] = Temp32;
            }               
        }
        else
        {
            /*lq structed binary data AI */
	        bitmap_get_value(&msg_bitmap, &Temp32, 10);                                   
            pAISMsg->BinaryData.StructedBinData.AI_DAC = Temp32;
	        bitmap_get_value(&msg_bitmap, &Temp32, 6);                                   
            pAISMsg->BinaryData.StructedBinData.AI_FI = Temp32;

            /*lq structed binary data App Data */
            pAISMsg->BinaryDataBitLen = (Temp8 * 8) - 60;   //lq 含有为保护整条消息的字节边界而填充的0，但无法识别那些事填充0，需由自定义协议指定                                                    
            pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen = pAISMsg->BinaryDataBitLen - 16;
            if (pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen > AIS_MSG26_BINDATA_BIT_MAXLEN_01)
            {
                /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/                
                pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen = AIS_MSG26_BINDATA_BIT_MAXLEN_01;
                pAISMsg->BinaryDataBitLen = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen + 16;            
            }

            Temp = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen / 8;
            for (i = 0; i < Temp; i++)
            {
                bitmap_get_value(&msg_bitmap, &Temp32, 8);                                   
                pAISMsg->BinaryData.StructedBinData.ApplicationData[i] = Temp32;
            }
            Temp = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen % 8;
            if (Temp != 0)
            {
                bitmap_get_value(&msg_bitmap, &Temp32, Temp);                                                   	                
                pAISMsg->BinaryData.StructedBinData.ApplicationData[i] = Temp32;
            }        
        }    
    }
    else
    {
        /*lq addressed */
	    /*lq destination id */
        bitmap_get_value(&msg_bitmap, &Temp32, 30);                
        pAISMsg->DestinationID = Temp32;
                
        /*lq binary data */
        if (pAISMsg->BinaryDataFlag == 0)
        {
            /*lq unstructed binary data */
            pAISMsg->BinaryDataBitLen = (Temp8 * 8) - 90;   //lq 含有为保护整条消息的字节边界而填充的0，但无法识别那些事填充0，需由自定义协议指定                                
            if (pAISMsg->BinaryDataBitLen > AIS_MSG26_BINDATA_BIT_MAXLEN_10)
            {
                /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/                                    
                pAISMsg->BinaryDataBitLen = AIS_MSG26_BINDATA_BIT_MAXLEN_10;            
            }

            Temp = pAISMsg->BinaryDataBitLen / 8;
            for (i = 0; i < Temp; i++)
            {
                bitmap_get_value(&msg_bitmap, &Temp32, 8);                                   
                pAISMsg->BinaryData.UnstructedBinData[i] = Temp32;
            }
            Temp = pAISMsg->BinaryDataBitLen % 8;
            if (Temp != 0)
            {
                bitmap_get_value(&msg_bitmap, &Temp32, Temp);                                                   	                
                pAISMsg->BinaryData.UnstructedBinData[i] = Temp32;
            }
        }
        else
        {
            /*lq structed binary data AI */
	        bitmap_get_value(&msg_bitmap, &Temp32, 10);                                   
            pAISMsg->BinaryData.StructedBinData.AI_DAC = Temp32;
	        bitmap_get_value(&msg_bitmap, &Temp32, 6);                                   
            pAISMsg->BinaryData.StructedBinData.AI_FI = Temp32;

            /*lq structed binary data App Data */
            pAISMsg->BinaryDataBitLen = (Temp8 * 8) - 90;   //lq 含有为保护整条消息的字节边界而填充的0，但无法识别那些事填充0，需由自定义协议指定                                            
            pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen = pAISMsg->BinaryDataBitLen - 16;            
            if (pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen > AIS_MSG26_BINDATA_BIT_MAXLEN_11)
            {
                /*lq 溢出保护，当超过最大长度时，只保留未超出的部分*/                                    
                pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen = AIS_MSG26_BINDATA_BIT_MAXLEN_11;
                pAISMsg->BinaryDataBitLen = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen + 16;            
            }

            Temp = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen / 8;
            for (i = 0; i < Temp; i++)
            {
                bitmap_get_value(&msg_bitmap, &Temp32, 8);                                   
                pAISMsg->BinaryData.StructedBinData.ApplicationData[i] = Temp32;
            }
            Temp = pAISMsg->BinaryData.StructedBinData.ApplicationDataBitLen % 8;
            if (Temp != 0)
            {
                bitmap_get_value(&msg_bitmap, &Temp32, Temp);                                                   	                
                pAISMsg->BinaryData.StructedBinData.ApplicationData[i] = Temp32;
            }        
        }     
    }    

 	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->CommStateSelectFlag = Temp32;
    if (pAISMsg->CommStateSelectFlag == 0)
    {
    	/*lq SOTDMA */
        bitmap_get_value(&msg_bitmap, &Temp32, 2);
        pAISMsg->CommunicationState.SOTDMACommState.SyncState = Temp32;
    	bitmap_get_value(&msg_bitmap, &Temp32, 3);
        pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout = Temp32;
        if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 0)
        {
    	    bitmap_get_value(&msg_bitmap, &Temp32, 14);    
            pAISMsg->CommunicationState.SOTDMACommState.SubMessage.SlotOffset = Temp32;
        }
        else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 1)
        {
    	    bitmap_get_value(&msg_bitmap, &Temp32, 14);        
            pAISMsg->CommunicationState.SOTDMACommState.SubMessage.UTCHourAndMinute = Temp32;
        }
        else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 2
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 4
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 6)
        {
    	    bitmap_get_value(&msg_bitmap, &Temp32, 14);        
            pAISMsg->CommunicationState.SOTDMACommState.SubMessage.SlotNumber = Temp32;
        }
        else if (pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 3
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 5
                 || pAISMsg->CommunicationState.SOTDMACommState.SlotTimeout == 7)
        {
    	    bitmap_get_value(&msg_bitmap, &Temp32, 14);        
            pAISMsg->CommunicationState.SOTDMACommState.SubMessage.ReceivedStations = Temp32;
        }    
    }
    else
    {
    	/*lq ITDMA */
        bitmap_get_value(&msg_bitmap, &Temp32, 2);
        pAISMsg->CommunicationState.ITDMACommState.SyncState = Temp32;
    	bitmap_get_value(&msg_bitmap, &Temp32, 13);
        pAISMsg->CommunicationState.ITDMACommState.SlotIncrement = Temp32;
    	bitmap_get_value(&msg_bitmap, &Temp32, 3);
        pAISMsg->CommunicationState.ITDMACommState.NumberOfSlots = Temp32;
    	bitmap_get_value(&msg_bitmap, &Temp32, 1);    
        pAISMsg->CommunicationState.ITDMACommState.KeepFlag = Temp32;
    }
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg27
* 描述  : 解析AIS消息27
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISMSG_ParseMsg27(AISMsgBinBuffArray_27 AISMsgBinBuff, AISMsgTypeStruct_27* pAISMsg)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 Temp;
    U8 Temp8;
    U32 Temp32;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = AISMsgBinBuff;
  	msg_bitmap.array_size = sizeof(AISMsgBinBuffArray_27);

	bitmap_get_value(&msg_bitmap, &Temp32, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）
	bitmap_get_value(&msg_bitmap, &Temp32, 6);
    pAISMsg->MessageID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 2);
    pAISMsg->RepeatIndicator = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 30);
    pAISMsg->UserID = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->PositionAccuracy = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->RAIMFlag = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 4);
    pAISMsg->NavigationalStatus = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 18);
    pAISMsg->Longitude = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 17);
    pAISMsg->Latitude = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 6);	
    pAISMsg->SOG = Temp32;
    bitmap_get_value(&msg_bitmap, &Temp32, 9);
    pAISMsg->COG = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1);
    pAISMsg->StatusOfCurrentGNSSPos = Temp32;
	bitmap_get_value(&msg_bitmap, &Temp32, 1); //lq spare
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsg
* 描述  : 解析AIS消息
* 输入  : AISMsgBinBuff - 二进制形式的AIS消息		  
* 输出  : 无
* 返回  : msgType - 消息类型
*******************************************************************************/
AISMsgTypeEnum AISMSG_ParseMsg(AISMsgBinBuffArray AISMsgBinBuff)
{
    AISMsgTypeEnum msgType;
    
    msgType = AISMSG_ParseMsgType(&AISMsgBinBuff[1]);

    switch (msgType)
    {
        case AISMsgType1:
            
            AISMSG_ParseMsg1_2(AISMsgBinBuff, &AISMsg_1);
            
            break;
        case AISMsgType2:
            
            AISMSG_ParseMsg1_2(AISMsgBinBuff, &AISMsg_2);
            
            break;
        case AISMsgType3:
            
            AISMSG_ParseMsg3(AISMsgBinBuff, &AISMsg_3);
            
            break;
        case AISMsgType4:
            
            AISMSG_ParseMsg4_11(AISMsgBinBuff, &AISMsg_4);
            
            break;
        case AISMsgType5:
            
            AISMSG_ParseMsg5(AISMsgBinBuff, &AISMsg_5);
            
            break;
        case AISMsgType6:
            
            AISMSG_ParseMsg6(AISMsgBinBuff, &AISMsg_6);
            
            break;
        case AISMsgType7:
            
            AISMSG_ParseMsg7_13(AISMsgBinBuff, &AISMsg_7);
            
            break;
        case AISMsgType8:
            
            AISMSG_ParseMsg8(AISMsgBinBuff, &AISMsg_8);
            
            break;
        case AISMsgType9:
            
            AISMSG_ParseMsg9(AISMsgBinBuff, &AISMsg_9);
            
            break;
        case AISMsgType10:
            
            AISMSG_ParseMsg10(AISMsgBinBuff, &AISMsg_10);
            
            break;
        case AISMsgType11:
            
            AISMSG_ParseMsg4_11(AISMsgBinBuff, &AISMsg_11);
            
            break;
        case AISMsgType12:
            
            AISMSG_ParseMsg12(AISMsgBinBuff, &AISMsg_12);
            
            break;
        case AISMsgType13:
            
            AISMSG_ParseMsg7_13(AISMsgBinBuff, &AISMsg_13);
            
            break;
        case AISMsgType14:
            
            AISMSG_ParseMsg14(AISMsgBinBuff, &AISMsg_14);
            
            break;
        case AISMsgType15:
            
            AISMSG_ParseMsg15(AISMsgBinBuff, &AISMsg_15);
            
            break;
        case AISMsgType16:
            
            AISMSG_ParseMsg16(AISMsgBinBuff, &AISMsg_16);
            
            break;
        case AISMsgType17:
            
            AISMSG_ParseMsg17(AISMsgBinBuff, &AISMsg_17);
            
            break;
        case AISMsgType18:
            
            AISMSG_ParseMsg18(AISMsgBinBuff, &AISMsg_18);
            
            break;
        case AISMsgType19:
            
            AISMSG_ParseMsg19(AISMsgBinBuff, &AISMsg_19);
            
            break;
        case AISMsgType20:
            
            AISMSG_ParseMsg20(AISMsgBinBuff, &AISMsg_20);
            
            break;
        case AISMsgType21:
            
            AISMSG_ParseMsg21(AISMsgBinBuff, &AISMsg_21);
            
            break;
        case AISMsgType22:
            
            AISMSG_ParseMsg22(AISMsgBinBuff, &AISMsg_22);
            
            break;
        case AISMsgType23:
            
            AISMSG_ParseMsg23(AISMsgBinBuff, &AISMsg_23);
            
            break;
        case AISMsgType24A:
            
            AISMSG_ParseMsg24A(AISMsgBinBuff, &AISMsg_24A);
            
            break;
        case AISMsgType24B:
            
            AISMSG_ParseMsg24B(AISMsgBinBuff, &AISMsg_24B);
            
            break;
        case AISMsgType25:
            
            AISMSG_ParseMsg25(AISMsgBinBuff, &AISMsg_25);
            
            break;
        case AISMsgType26:
            
            AISMSG_ParseMsg26(AISMsgBinBuff, &AISMsg_26);
            
            break;
        case AISMsgType27:
            
            AISMSG_ParseMsg27(AISMsgBinBuff, &AISMsg_27);
            
            break;
        default :
            break;
    }   

    return msgType;
}

/*******************************************************************************
* 名称  : AISMSG_ParseMsgType
* 描述  : 解析AIS消息类型
* 输入  : pData - 二进制形式的AIS消息		  
* 输出  : 无
* 返回  : msgType - 消息类型
*******************************************************************************/
AISMsgTypeEnum AISMSG_ParseMsgType(U8 *pData)
{
    AISMsgTypeEnum msgType;
    U8  msgId;

    msgId = (pData[0] >> 2) & 0x3F; //lq 消息ID

    if (msgId == 1)
    {
        msgType = AISMsgType1;   
    }
    else if (msgId == 2)
    {
        msgType = AISMsgType2;   
    } 
    else if (msgId == 3)
    {
        msgType = AISMsgType3;   
    }  
    else if (msgId == 4)
    {
        msgType = AISMsgType4;   
    }  
    else if (msgId == 5)
    {
        msgType = AISMsgType5;   
    }  
    else if (msgId == 6)
    {
        msgType = AISMsgType6;   
    }  
    else if (msgId == 7)
    {
        msgType = AISMsgType7;   
    }  
    else if (msgId == 8)
    {
        msgType = AISMsgType8;   
    }  
    else if (msgId == 9)
    {
        msgType = AISMsgType9;   
    }  
    else if (msgId == 10)
    {
        msgType = AISMsgType10;   
    }  
    else if (msgId == 11)
    {
        msgType = AISMsgType11;   
    }  
    else if (msgId == 12)
    {
        msgType = AISMsgType12;   
    }  
    else if (msgId == 13)
    {
        msgType = AISMsgType13;   
    }  
    else if (msgId == 14)
    {
        msgType = AISMsgType14;   
    }  
    else if (msgId == 15)
    {
        msgType = AISMsgType15;   
    }  
    else if (msgId == 16)
    {
        msgType = AISMsgType16;   
    }  
    else if (msgId == 17)
    {
        msgType = AISMsgType17;   
    }  
    else if (msgId == 18)
    {
        msgType = AISMsgType18;   
    }  
    else if (msgId == 19)
    {
        msgType = AISMsgType19;   
    }  
    else if (msgId == 20)
    {
        msgType = AISMsgType20;   
    }  
    else if (msgId == 21)
    {
        msgType = AISMsgType21;   
    }  
    else if (msgId == 22)
    {
        msgType = AISMsgType22;   
    }  
    else if (msgId == 23)
    {
        msgType = AISMsgType23;   
    }  
    else if (msgId == 24)
    {
        if ((pData[4] & 0x03) == 0)
        {
            msgType = AISMsgType24A;                           
        }
        else if ((pData[4] & 0x03) == 1)
        {
            msgType = AISMsgType24B;                           
        }
        else
        {
            msgType = AISMsgTypeError;                                       
        }
    }  
    else if (msgId == 25)
    {
        msgType = AISMsgType25;   
    }  
    else if (msgId == 26)
    {
        msgType = AISMsgType26;   
    }  
    else if (msgId == 27)
    {
        msgType = AISMsgType27;   
    }  
    else
    {
        msgType = AISMsgTypeError;   
    }  

    return msgType;                   
}

/*******************************************************************************
* 名称  : AISMSG_ConvertToMsgType
* 描述  : 消息号转为消息类型
* 输入  : msgId - 消息号
          partNumber - 消息部分编号（仅对24号消息（24A和24B）有用）
* 输出  : 无
* 返回  : msgType - 消息类型
*******************************************************************************/
AISMsgTypeEnum AISMSG_ConvertToMsgType(U8 msgId, U8 partNumber)
{
    AISMsgTypeEnum msgType;

    if (msgId == 1)
    {
        msgType = AISMsgType1;   
    }
    else if (msgId == 2)
    {
        msgType = AISMsgType2;   
    } 
    else if (msgId == 3)
    {
        msgType = AISMsgType3;   
    }  
    else if (msgId == 4)
    {
        msgType = AISMsgType4;   
    }  
    else if (msgId == 5)
    {
        msgType = AISMsgType5;   
    }  
    else if (msgId == 6)
    {
        msgType = AISMsgType6;   
    }  
    else if (msgId == 7)
    {
        msgType = AISMsgType7;   
    }  
    else if (msgId == 8)
    {
        msgType = AISMsgType8;   
    }  
    else if (msgId == 9)
    {
        msgType = AISMsgType9;   
    }  
    else if (msgId == 10)
    {
        msgType = AISMsgType10;   
    }  
    else if (msgId == 11)
    {
        msgType = AISMsgType11;   
    }  
    else if (msgId == 12)
    {
        msgType = AISMsgType12;   
    }  
    else if (msgId == 13)
    {
        msgType = AISMsgType13;   
    }  
    else if (msgId == 14)
    {
        msgType = AISMsgType14;   
    }  
    else if (msgId == 15)
    {
        msgType = AISMsgType15;   
    }  
    else if (msgId == 16)
    {
        msgType = AISMsgType16;   
    }  
    else if (msgId == 17)
    {
        msgType = AISMsgType17;   
    }  
    else if (msgId == 18)
    {
        msgType = AISMsgType18;   
    }  
    else if (msgId == 19)
    {
        msgType = AISMsgType19;   
    }  
    else if (msgId == 20)
    {
        msgType = AISMsgType20;   
    }  
    else if (msgId == 21)
    {
        msgType = AISMsgType21;   
    }  
    else if (msgId == 22)
    {
        msgType = AISMsgType22;   
    }  
    else if (msgId == 23)
    {
        msgType = AISMsgType23;   
    }  
    else if (msgId == 24)
    {
        if (partNumber == 0)
        {
            msgType = AISMsgType24A;                           
        }
        else if (partNumber == 1)
        {
            msgType = AISMsgType24B;                           
        }
        else
        {
            msgType = AISMsgTypeError;                                       
        }
    }  
    else if (msgId == 25)
    {
        msgType = AISMsgType25;   
    }  
    else if (msgId == 26)
    {
        msgType = AISMsgType26;   
    }  
    else if (msgId == 27)
    {
        msgType = AISMsgType27;   
    }  
    else
    {
        msgType = AISMsgTypeError;   
    }  

    return msgType;                   
}

/*******************************************************************************
* 名称  : AISMSG_ConvertToMsgID
* 描述  : 消息类型转为消息号
* 输入  : msgType - 消息类型
* 输出  : 无
* 返回  : 消息号
*******************************************************************************/
U8 AISMSG_ConvertToMsgID(AISMsgTypeEnum msgType)
{
    U8 msgID;

	switch (msgType)
	{
		case AISMsgType1:

			msgID = 1;
			
			break;

		case AISMsgType2:

			msgID = 2;

			break;

		case AISMsgType3:

			msgID = 3;

			break;

		case AISMsgType4:

			msgID = 4;

			break;

		case AISMsgType5:

			msgID = 5;

			break;

		case AISMsgType6:

			msgID = 6;

			break;

		case AISMsgType7:

			msgID = 7;

			break;

		case AISMsgType8:

			msgID = 8;

			break;

		case AISMsgType9:

			msgID = 9;

			break;

		case AISMsgType10:

			msgID = 10;

			break;

		case AISMsgType11:

			msgID = 11;

			break;

		case AISMsgType12:

			msgID = 12;

			break;

		case AISMsgType13:

			msgID = 13;

			break;

		case AISMsgType14:

			msgID = 14;

			break;

		case AISMsgType15:

			msgID = 15;

			break;

		case AISMsgType16:

			msgID = 16;

			break;

		case AISMsgType17:

			msgID = 17;

			break;

		case AISMsgType18:

			msgID = 18;

			break;

		case AISMsgType19:

			msgID = 19;

			break;

		case AISMsgType20:

			msgID = 20;

			break;

		case AISMsgType21:

			msgID = 21;

			break;

		case AISMsgType22:

			msgID = 22;

			break;

		case AISMsgType23:

			msgID = 23;

			break;

		case AISMsgType24A:

			msgID = 24;

			break;

		case AISMsgType24B:

			msgID = 24;

			break;

		case AISMsgType25:

			msgID = 25;

			break;

		case AISMsgType26:

			msgID = 26;

			break;

		case AISMsgType27:

			msgID = 27;

			break;
			
		default :
			
			msgID = 1;

			break;
	}

	return msgID;
}


/******************************************************************
Copyright(c) 2010, TJU-TCB
All right reserved
File name:		AIS_MsgModu.c
Description:	Msg related parameter initialization and Msg process
Others:			void
Author:			WT
Version:		1.0
Date:			2010.7.21
Function List:  
*****************************************************************************
*  History:               				                                    *
*      <author>   <time>    <version >     <desc>                           *
*         WT     10/07/21     1.0       build this moudle                   *
****************************************************************************/
#include "Include.h"

GPSStruct pGpsCell;

U8 GPS_valid = 0;

/*******************************************************************************
* 名称  : InitGpsCell
* 描述  : 初始化pGpsCell
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitGpsCell(void)
{
	IniGpsMsgStru(&pGpsCell);
}

/*******************************************************************************
* 名称  : bitmap_set_value_for_U8
* 描述  : 按位设置值（用data的低data_bit_len位对map->array[map->index]的无效位的
          高data_bit_len位赋值，对字节map->array[map->index]的赋值顺序是先对MSB
          赋值，后对LSB赋值）
* 输入  : map - 结构体变量，
          data - 提供赋值位的字节数据，
          data_bit_len - data用于赋值的位数（LSB）		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 bitmap_set_value_for_U8(BITMAP* map, U8 data, U8 data_bit_len)
{
	U8 data_mask;
	U8 temp;
    U8 byteMask;

	if(data_bit_len == 0)
	{
		return 1;
	}
	else if (data_bit_len > 8)
    {
        data_bit_len = 8;
    }

	data_mask = 0xFF >> (8 - data_bit_len);
	
	if(map->offset + data_bit_len < 8)
	{
		temp = 8 - map->offset - data_bit_len;
		map->array[map->index] = (map->array[map->index] >> (8 - map->offset)) << (8 - map->offset);    //lq 将当前字节map->array[map->index]的当前无效位清0(为下面的‘|’位运算做准备)
		map->array[map->index] |= ((data & data_mask) << temp);                 //lq 将当前字节map->array[map->index]的无效位的前（MSB）data_bit_len位置为data的低data_bit_len位    
		
		map->offset += data_bit_len;
		map->offset %= 8;
	}
	else if(map->offset + data_bit_len == 8)
	{
		temp = 8 - map->offset - data_bit_len;
		map->array[map->index] = (map->array[map->index] >> (8 - map->offset)) << (8 - map->offset);
		map->array[map->index] |= ((data & data_mask) << temp);
		
		map->offset = 0;
		map->index++;
	}                                                                           //lq 指向下一字节
	else
	{
		if(map->index == map->array_size - 1)                                   //lq map->array已满
		{
			return 1;
		}
		
		temp = 8 - map->offset;                                                 //lq 当前字节map->array[map->index]可提供的用于赋值的无效位位数
		map->array[map->index] = (map->array[map->index] >> (8 - map->offset)) << (8 - map->offset);
		map->array[map->index] |= ((data & data_mask) >> (data_bit_len - temp));//lq 使用字节数据data的低data_bit_len位的高temp位对当前字节map->array[map->index]的低temp位赋值
		map->array[map->index + 1] = map->array[map->index + 1] << 8;           //lq 将字节map->array[map->index + 1]清0(为下面的‘|’位运算做准备)
        map->array[map->index + 1] |= ((data & data_mask) << (8 - (data_bit_len - temp)));
		
		map->index++;
		map->offset += data_bit_len;
		map->offset %= 8;
	}

	return 0;
}

/*******************************************************************************
* 名称  : bitmap_set_value
* 描述  : 按位设置值
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 bitmap_set_value(BITMAP* map, U32 data, U8 data_bit_len)
{
	U8 split_data[4];
	U8 split_bit_len[4];
	U8 temp;
	S8 i;

	temp = data_bit_len;
	for(i = 3; i >= 0; i--)
	{
		if(temp >= 8)
		{
			split_bit_len[i] = 8;
			temp -=8;
		}
		else
		{
			split_bit_len[i] = temp;
			temp = 0;
		}
	}

	//修改split_data排列顺序
	split_data[3] = data & 0xFF;
	split_data[2] = (data & 0xFF00) >> 8;
	split_data[1] = (data & 0xFF0000) >> 16;
	split_data[0] = (data & 0xFF000000) >> 24;

	bitmap_set_value_for_U8(map, split_data[0], split_bit_len[0]);
	bitmap_set_value_for_U8(map, split_data[1], split_bit_len[1]);
	bitmap_set_value_for_U8(map, split_data[2], split_bit_len[2]);
	bitmap_set_value_for_U8(map, split_data[3], split_bit_len[3]);

	return 0;
}

/*******************************************************************************
* 名称  : bitmap_get_value_for_U8
* 描述  : 按位获取值（用map->array[map->index]的有效位（还未读取的位）的高
          data_bit_len位对pData所指数据单元赋值，data_bit_len位数据的MSB和LSB的
          顺序在map->array[map->index]和*pData中一致，
          对字节map->array[map->index]的读取顺序是先读取MSB，后读取LSB）
* 输入  : map - 结构体变量，
          pData - 存储获取位的字节数据（存放在pData所指字节单元的LSBs），
          data_bit_len - 读取的位数		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 bitmap_get_value_for_U8(BITMAP* map, U8* pData, U8 data_bit_len)
{
	*pData = 0;

    if(data_bit_len == 0)
	{
		return 1;
	}
	else if (data_bit_len > 8)
    {
        data_bit_len = 8;
    }
	
	if(map->offset + data_bit_len <= 8)
	{
		*pData = map->array[map->index] << map->offset; //lq 将data_bit_len长度的比特移至LSBs，并使高MSBs为0
	    *pData >>= 8 - data_bit_len;
    }
	else
	{
		if(map->index == map->array_size - 1)                                   //lq map->array已满
		{
			return 1;
		}
		
		*pData = map->array[map->index] << map->offset; //lq 将data_bit_len长度的比特移至LSBs，并使高MSBs为0
	    *pData >>= map->offset;
        *pData = (*pData << (map->offset + data_bit_len - 8)) 
                 | (map->array[map->index + 1] >> (16 - map->offset - data_bit_len));
	}

	map->offset += data_bit_len;
	map->index += map->offset / 8;
	map->offset %= 8;

	return 0;
}
/*******************************************************************************
* 名称  : bitmap_get_value
* 描述  : 按位获取值
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 bitmap_get_value(BITMAP* map, U32* pData, U8 data_bit_len)
{
	U8 split_data[4];
	U8 split_bit_len[4];
	U8 temp;
	S8 i;

	temp = data_bit_len;
	for(i = 3; i >= 0; i--)
	{
		if(temp >= 8)
		{
			split_bit_len[i] = 8;
			temp -=8;
		}
		else
		{
			split_bit_len[i] = temp;
			temp = 0;
		}
	}

    for (i = 0; i < 3; i++)
    {
        split_data[i] = 0;
    }

	bitmap_get_value_for_U8(map, split_data, split_bit_len[0]);
	bitmap_get_value_for_U8(map, split_data + 1, split_bit_len[1]);
	bitmap_get_value_for_U8(map, split_data + 2, split_bit_len[2]);
	bitmap_get_value_for_U8(map, split_data + 3, split_bit_len[3]);

    *pData = (split_data[0] << 24)
           | (split_data[1] << 16)
           | (split_data[2] << 8)
           | split_data[3];

	return 0;
}

/*******************************************************************************
* 名称  : InitMsg1
* 描述  : 初始化消息1
* 输入  : 无		  
* 输出  : commState - 通信状态 
* 返回  : 无
*******************************************************************************/
void InitMsg1(AISMsgCommStateUnion commState)
{
    AISMsgTypeStruct_1_2_3 msg1;

    /*lq 初始化消息各个字段的值 */
    msg1.MessageID = 1;                                                         //lq 6 bits
    msg1.RepeatIndicator = 0;                                                   //lq 2 bits
    msg1.UserID = AisStaticPara.MMSI;                                           //lq 30 bits
    msg1.NavigationalStatus = AisStaticPara.NavigationStatus;                   //lq 4 bits
    msg1.RotAIS = AisDynamicPara.RotAIS;                                        //lq 8 bits
    msg1.SOG = AisDynamicPara.SOG;                                              //lq 10 bits
    msg1.PositionAccuracy = AisDynamicPara.PositionAccuracy;                    //lq 1 bit
    msg1.Longitude = AisDynamicPara.Longitude;                                  //lq 28 bits
    msg1.Latitude = AisDynamicPara.Latitude;                                    //lq 27 bits
    msg1.COG = AisDynamicPara.COG;                                              //lq 12 bits
    msg1.TrueHeading = AisDynamicPara.TrueHeading;                              //lq 9 bits
    msg1.TimeStamp = AisDynamicPara.TimeStamp;                                  //lq 6 bits
    msg1.SpecialManoeuvreIndicator = AisDynamicPara.SpecialManoeuvreIndicator;  //lq 2 bits
    //  Spare;                                                                  //lq 3 bits, all 0
    msg1.RAIMFlag = AisDynamicPara.RaimFlag;                                    //lq 1 bit
    msg1.CommunicationState.SOTDMACommState.SyncState
        = commState.SOTDMACommState.SyncState;                                  //lq 2 bits
    msg1.CommunicationState.SOTDMACommState.SlotTimeout
        = commState.SOTDMACommState.SlotTimeout;                                //lq 3 bits
    msg1.CommunicationState.SOTDMACommState.SubMessage.SubMsg
        = commState.SOTDMACommState.SubMessage.SubMsg;                          //lq 14 bits
                                                                                //lq total 168 bits，1 slot

    /*lq 构造消息的二进制表示 */
    AISMSG_BuildMsg1(&msg1);
}

/*******************************************************************************
* 名称  : InitMsg3
* 描述  : 初始化消息3
* 输入  : 无		  
* 输出  : commState - 通信状态 
* 返回  : 无
*******************************************************************************/
void InitMsg3(AISMsgCommStateUnion commState)
{
    AISMsgTypeStruct_1_2_3 msg3;

    /*lq 初始化消息各个字段的值 */
    msg3.MessageID = 3;                                                         //lq 6 bits
    msg3.RepeatIndicator = 0;                                                   //lq 2 bits
    msg3.UserID = AisStaticPara.MMSI;                                           //lq 30 bits
    msg3.NavigationalStatus = AisStaticPara.NavigationStatus;                   //lq 4 bits
    msg3.RotAIS = AisDynamicPara.RotAIS;                                        //lq 8 bits
    msg3.SOG = AisDynamicPara.SOG;                                              //lq 10 bits
    msg3.PositionAccuracy = AisDynamicPara.PositionAccuracy;                    //lq 1 bit
    msg3.Longitude = AisDynamicPara.Longitude;                                  //lq 28 bits
    msg3.Latitude = AisDynamicPara.Latitude;                                    //lq 27 bits
    msg3.COG = AisDynamicPara.COG;                                              //lq 12 bits
    msg3.TrueHeading = AisDynamicPara.TrueHeading;                              //lq 9 bits
    msg3.TimeStamp = AisDynamicPara.TimeStamp;                                  //lq 6 bits
    msg3.SpecialManoeuvreIndicator = AisDynamicPara.SpecialManoeuvreIndicator;  //lq 2 bits
    //  Spare;                                                                  //lq 3 bits, all 0
    msg3.RAIMFlag = AisDynamicPara.RaimFlag;                                    //lq 1 bit
    msg3.CommunicationState.ITDMACommState.SyncState
        = commState.ITDMACommState.SyncState;                                   //lq 2 bits
    msg3.CommunicationState.ITDMACommState.SlotIncrement
        = commState.ITDMACommState.SlotIncrement;                               //lq 13 bits
    msg3.CommunicationState.ITDMACommState.NumberOfSlots
        = commState.ITDMACommState.NumberOfSlots;                               //lq 3 bits
    msg3.CommunicationState.ITDMACommState.KeepFlag
        = commState.ITDMACommState.KeepFlag;                                    //lq 1 bits
                                                                                //lq total 168 bits，1 slot

    /*lq 构造消息的二进制表示 */
    AISMSG_BuildMsg3(&msg3);
}

/*******************************************************************************
* 名称  : InitMsg5
* 描述  : 初始化消息5
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitMsg5(void)
{
    AISMsgTypeStruct_5 msg5;

    msg5.MessageID = 5;                                                     //lq 6 bits
    msg5.RepeatIndicator = 0;                                               //lq 2 bits
    msg5.UserID = AisStaticPara.MMSI;                                       //lq 30 bits
    msg5.AISVersionIndicator = AisStaticPara.AISVersionIndicator;           //lq 2 bits
    msg5.IMONumber = AisStaticPara.IMONumber;                               //lq 30 bits
    strncpy(msg5.CallSign, AisStaticPara.CallSign, AIS_INFO_CALL_SIGN_LEN); //lq 42 bits (6-bit ASCII)
    strncpy(msg5.Name, AisStaticPara.ShipName, AIS_INFO_SHIP_NAME_LEN);     //lq 120 bits (6-bit ASCII)
    msg5.TypeOfShipAndCargo = AisStaticPara.TypeOfShip;                     //lq 8 bits
    msg5.DimensionA = AisStaticPara.ShipA;                                  //lq 9 bits
    msg5.DimensionB = AisStaticPara.ShipB;                                  //lq 9 bits
    msg5.DimensionC = AisStaticPara.ShipC;                                  //lq 6 bits
    msg5.DimensionD = AisStaticPara.ShipD;                                  //lq 6 bits
    msg5.TypeOfEPFD = AisStaticPara.TypeOfEPFS;                             //lq 4 bits
    msg5.ETAMonth = AisStaticPara.ETAMonth;                                 //lq 4 bits
    msg5.ETADay = AisStaticPara.ETADay;                                     //lq 5 bits
    msg5.ETAHour = AisStaticPara.ETAHour;                                   //lq 5 bits
    msg5.ETAMinute = AisStaticPara.ETAMinute;                               //lq 6 bits
    msg5.MaxPresentStaticDraught = AisStaticPara.MaxDraught;                //lq 8 bits
    strncpy(msg5.Destination, AisStaticPara.Destination, AIS_INFO_DESTINATION_LEN); //lq 120 bits (6-bit ASCII)
    msg5.DTE = AisStaticPara.DTE;                                           //lq 1 bits
    //  Spare;                                                              //lq 1 bit, all 0
                                                                            //lq total 424 bits，2 slots

    /*lq 构造消息的二进制表示 */
    AISMSG_BuildMsg5(&msg5);
}

/*******************************************************************************
* 名称  : InitMsg6
* 描述  : 初始化消息6
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitMsg6(void)
{
    static U8 seqNum = 0;           //lq 0 - 3
    S16 i;
    AISMsgTypeStruct_6 msg6;
	BITMAP msg_bitmap;

    msg6.MessageID = 6;                                                     //lq 6 bits
    msg6.RepeatIndicator = 0;                                               //lq 2 bits
    msg6.SourceID = AisStaticPara.MMSI;                                     //lq 30 bits
    msg6.SequenceNumber = seqNum;                                           //lq 2 bits
    msg6.DestinationID = ABMPara.MmsiDest;                                  //lq 30 bits
    msg6.RetransmitFlag = 0;                                                //lq 1 bit
    //  Spare;                                                              //lq 1 bit, all 0

    /*lq Binary data中的Application identifier，ASCII转6-bit binary */
    msg6.AI_DAC = (AsciiToSixBinCode(ABMPara.EncapData[0]) << 4)
        || (AsciiToSixBinCode(ABMPara.EncapData[1]) >> 2);                  //lq 10 bits
    msg6.AI_FI = ((AsciiToSixBinCode(ABMPara.EncapData[1]) & 0x03) << 4)
        || (AsciiToSixBinCode(ABMPara.EncapData[2]) >> 2);                  //lq 6 bits

    /*lq Binary data中的Application data，ASCII转6-bit binary */
	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));
	msg_bitmap.array = msg6.ApplicationData;
  	msg_bitmap.array_size = 115;
    bitmap_set_value(&msg_bitmap, AsciiToSixBinCode(ABMPara.EncapData[2]), 2);
    for (i = 3; i < ABMPara.EncapDataLen - 1; i++)
    {
        bitmap_set_value(&msg_bitmap, AsciiToSixBinCode(ABMPara.EncapData[i]),
            6);
    }    
    bitmap_set_value(&msg_bitmap, AsciiToSixBinCode(ABMPara.EncapData[i]) >> ABMPara.FillBitNumber,
        6 - ABMPara.FillBitNumber);                                         //lq Maximum 920 bits (8-bit byte)
    msg6.AppDataBitLen = 6 * ABMPara.EncapDataLen - 16 - ABMPara.FillBitNumber;
                                                                            //lq Maximum 1008 bits，1-5 slots(B-Class mobile AIS station not exceed 2 slots)

    seqNum = (seqNum + 1) % 4;

    /*lq 构造消息的二进制表示 */
    AISMSG_BuildMsg6(&msg6);
}

/*******************************************************************************
* 名称  : InitMsg6Test
* 描述  : 初始化测试消息6
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitMsg6Test(void)
{
    static U8 seqNum = 0;           //lq 0 - 3
    S16 i;
    AISMsgTypeStruct_6 msg6;
	BITMAP msg_bitmap;

    msg6.MessageID = 6;                                                     //lq 6 bits
    msg6.RepeatIndicator = 0;                                               //lq 2 bits
    msg6.SourceID = AisStaticPara.MMSI;                                     //lq 30 bits
    msg6.SequenceNumber = seqNum;                                           //lq 2 bits
    msg6.DestinationID = AisStaticPara.MMSI;                                //lq 30 bits
    msg6.RetransmitFlag = 0;                                                //lq 1 bit
    //  Spare;                                                              //lq 1 bit, all 0

    /*lq Binary data中的Application identifier */
    msg6.AI_DAC = RFL_SLFCHK_MSG6_AI_DAC;                                   //lq 10 bits
    msg6.AI_FI = RFL_SLFCHK_MSG6_AI_FI;                                     //lq 6 bits

    /*lq Binary data中的Application data */
	msg6.ApplicationData[0] = RFL_SLFCHK_MSG6_AD;                           //lq Maximum 920 bits (8-bit byte)
    msg6.AppDataBitLen = RFL_SLFCHK_MSG6_AD_BIT_LEN;
                                                                            //lq Maximum 1008 bits，1-5 slots(B-Class mobile AIS station not exceed 2 slots)
    seqNum = (seqNum + 1) % 4;

    /*lq 构造消息的二进制表示 */
    AISMSG_BuildMsg6(&msg6);
}

#if 0
/*******************************************************************************
* 名称  : InitMsg6
* 描述  : 初始化使用消息6的IFM0
* 输入  : info - text内容		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitMsg6(void)
{
	BITMAP msg_bitmap;
	U8 len_1;
//	U8 len_2;
//	U8 len_3;
	U8 len_4;
	U8 len_5;
	U8 i;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = G_Msg6IFM0;

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

	bitmap_set_value(&msg_bitmap, 6, 6);                                        //lq Message ID
	bitmap_set_value(&msg_bitmap, 0, 2);                                        //lq Repeat indicator
	bitmap_set_value(&msg_bitmap, aiAISCell.UserID, 30);                        //lq Source ID
	bitmap_set_value(&msg_bitmap, 0, 2);                                        //lq Sequence number
	bitmap_set_value(&msg_bitmap, aiAISCell.DestMMSI, 30);                      //lq Destination ID
	bitmap_set_value(&msg_bitmap, 0, 1);                                        //lq Retransmit flag
	bitmap_set_value(&msg_bitmap, 0, 1);                                        //lq Spare
	bitmap_set_value(&msg_bitmap, Msg06_DAC, 10);                               //lq 一体机的DAC=413
	bitmap_set_value(&msg_bitmap, Msg06_FI, 6);                                 //lq 一体机的FI=1
	bitmap_set_value(&msg_bitmap, Msg06_AnalogueInternal, 10); 
	bitmap_set_value(&msg_bitmap, Msg06_AnalogueExternalNo1, 10);                  
	bitmap_set_value(&msg_bitmap, Msg06_AnalogueExternalNo2, 10);                  
	bitmap_set_value(&msg_bitmap, Msg06_StatusInternal, 5);                  
	bitmap_set_value(&msg_bitmap, Msg06_StatusExternal, 8);                  
	bitmap_set_value(&msg_bitmap, Msg06_OffPositionStatus, 1);                  
	bitmap_set_value(&msg_bitmap, 0, 4);                                        //lq For future use

	len_4 = msg_bitmap.index;
	len_5 = msg_bitmap.offset == 0 ? len_4 - 1 : len_4;
	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;
	
	bitmap_set_value(&msg_bitmap, len_5, 8);
}
#endif

/*******************************************************************************
* 名称  : InitMsg7
* 描述  : 初始化消息7
* 输入  : ackType - 确认类型
          pDestinationID - 确认消息的目的ID数组指针
          pSequenceNumber - 确认消息的序号数组指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitMsg7(U8 ackType, U32 *pDestinationID, U8 *pSequenceNumber)
{
    AISMsgTypeStruct_7_13 msg7;

    msg7.MessageID = 7;                                 //lq 6 bits
    msg7.RepeatIndicator = 0;                           //lq 2 bits
    msg7.SourceID = AisStaticPara.MMSI;                 //lq 30 bits
    //  Spare;                                          //lq 2 bits, all 0

    if (ackType == AIS_MSG7_13_ACK_TYPE_1)
    {
        msg7.DestinationID1 = pDestinationID[0];        //lq 30 bits
        msg7.SequenceNumber1 = pSequenceNumber[0];      //lq 2 bits
        msg7.AckType = AIS_MSG7_13_ACK_TYPE_1;
    }
    else if (ackType == AIS_MSG7_13_ACK_TYPE_2)
    {
        msg7.DestinationID1 = pDestinationID[0];        //lq 30 bits
        msg7.SequenceNumber1 = pSequenceNumber[0];      //lq 2 bits
        msg7.DestinationID2 = pDestinationID[1];        //lq 30 bits
        msg7.SequenceNumber2 = pSequenceNumber[1];      //lq 2 bits
        msg7.AckType = AIS_MSG7_13_ACK_TYPE_2;
    }
    else if (ackType == AIS_MSG7_13_ACK_TYPE_3)
    {
        msg7.DestinationID1 = pDestinationID[0];        //lq 30 bits
        msg7.SequenceNumber1 = pSequenceNumber[0];      //lq 2 bits
        msg7.DestinationID2 = pDestinationID[1];        //lq 30 bits
        msg7.SequenceNumber2 = pSequenceNumber[1];      //lq 2 bits
        msg7.DestinationID3 = pDestinationID[2];        //lq 30 bits
        msg7.SequenceNumber3 = pSequenceNumber[2];      //lq 2 bits
        msg7.AckType = AIS_MSG7_13_ACK_TYPE_3;        
    }
    else if (ackType == AIS_MSG7_13_ACK_TYPE_4)
    {
        msg7.DestinationID1 = pDestinationID[0];        //lq 30 bits
        msg7.SequenceNumber1 = pSequenceNumber[0];      //lq 2 bits
        msg7.DestinationID2 = pDestinationID[1];        //lq 30 bits
        msg7.SequenceNumber2 = pSequenceNumber[1];      //lq 2 bits
        msg7.DestinationID3 = pDestinationID[2];        //lq 30 bits
        msg7.SequenceNumber3 = pSequenceNumber[2];      //lq 2 bits
        msg7.DestinationID4 = pDestinationID[3];        //lq 30 bits
        msg7.SequenceNumber4 = pSequenceNumber[3];      //lq 2 bits
        msg7.AckType = AIS_MSG7_13_ACK_TYPE_4;    
    }
    else
    {
        msg7.DestinationID1 = pDestinationID[0];        //lq 30 bits
        msg7.SequenceNumber1 = pSequenceNumber[0];      //lq 2 bits
        msg7.AckType = AIS_MSG7_13_ACK_TYPE_1;    
    }
                                                        //lq total 72-168 bits，1 slot

    /*lq 构造消息的二进制表示 */
    AISMSG_BuildMsg7(&msg7);
}

/*******************************************************************************
* 名称  : InitMsg8
* 描述  : 初始化消息8
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitMsg8(void)
{
    S16 i;
    AISMsgTypeStruct_8 msg8;
	BITMAP msg_bitmap;

    msg8.MessageID = 8;                                                     //lq 6 bits
    msg8.RepeatIndicator = 0;                                               //lq 2 bits
    msg8.SourceID = AisStaticPara.MMSI;                                     //lq 30 bits
    //  Spare;                                                              //lq 2 bits, all 0

    /*lq Binary data中的Application identifier，ASCII转6-bit binary */
    msg8.AI_DAC = (AsciiToSixBinCode(BBMPara.EncapData[0]) << 4)
        || (AsciiToSixBinCode(BBMPara.EncapData[1]) >> 2);                  //lq 10 bits
    msg8.AI_FI = ((AsciiToSixBinCode(BBMPara.EncapData[1]) & 0x03) << 4)
        || (AsciiToSixBinCode(BBMPara.EncapData[2]) >> 2);                  //lq 6 bits

    /*lq Binary data中的Application data，ASCII转6-bit binary */
	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));
	msg_bitmap.array = msg8.ApplicationData;
  	msg_bitmap.array_size = 119;
    bitmap_set_value(&msg_bitmap, AsciiToSixBinCode(BBMPara.EncapData[2]), 2);
    for (i = 3; i < BBMPara.EncapDataLen - 1; i++)
    {
        bitmap_set_value(&msg_bitmap, AsciiToSixBinCode(BBMPara.EncapData[i]),
            6);
    }    
    bitmap_set_value(&msg_bitmap, AsciiToSixBinCode(BBMPara.EncapData[i]) >> BBMPara.FillBitNumber,
        6 - BBMPara.FillBitNumber);                                         //lq Maximum 920 bits (8-bit byte)
    msg8.AppDataBitLen = 6 * BBMPara.EncapDataLen - 16 - BBMPara.FillBitNumber;
                                                                            //lq Maximum 1008 bits，1-5 slots(B-Class mobile AIS station not exceed 2 slots)

    /*lq 构造消息的二进制表示 */
    AISMSG_BuildMsg8(&msg8);
}

/*******************************************************************************
* 名称  : InitMsg12
* 描述  : 初始化消息12
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitMsg12(void)
{
    static U8 seqNum = 0;                                                    //lq 0 - 3
    S16 i;
    AISMsgTypeStruct_12 msg12;
	BITMAP msg_bitmap;

    msg12.MessageID = 12;                                                    //lq 6 bits
    msg12.RepeatIndicator = 0;                                               //lq 2 bits
    msg12.SourceID = AisStaticPara.MMSI;                                     //lq 30 bits
    msg12.SequenceNumber = seqNum;                                           //lq 2 bits
    msg12.DestinationID = ABMPara.MmsiDest;                                  //lq 30 bits
    msg12.RetransmitFlag = 0;                                                //lq 1 bit
    //  Spare;                                                               //lq 1 bit, all 0

    /*lq Safety related text */
    for (i = 0; i < ABMPara.EncapDataLen; i++)
    {
        msg12.SafetyRelatedText[i] = AsciiToSixBinCode(ABMPara.EncapData[i]);//lq Maximum 936 bits (6-bit ASCII)    
    }
    msg12.TextLen = ABMPara.EncapDataLen;

    seqNum = (seqNum + 1) % 4;

    /*lq 构造消息的二进制表示 */
    AISMSG_BuildMsg12(&msg12);
}

/*******************************************************************************
* 名称  : InitMsg13
* 描述  : 初始化消息13
* 输入  : ackType - 确认类型
          pDestinationID - 确认消息的目的ID数组指针
          pSequenceNumber - 确认消息的序号数组指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitMsg13(U8 ackType, U32 *pDestinationID, U8 *pSequenceNumber)
{
    AISMsgTypeStruct_7_13 msg13;

    msg13.MessageID = 13;                               //lq 6 bits
    msg13.RepeatIndicator = 0;                          //lq 2 bits
    msg13.SourceID = AisStaticPara.MMSI;                //lq 30 bits
    //  Spare;                                          //lq 2 bits, all 0

    if (ackType == AIS_MSG7_13_ACK_TYPE_1)
    {
        msg13.DestinationID1 = pDestinationID[0];       //lq 30 bits
        msg13.SequenceNumber1 = pSequenceNumber[0];     //lq 2 bits
        msg13.AckType = AIS_MSG7_13_ACK_TYPE_1;
    }
    else if (ackType == AIS_MSG7_13_ACK_TYPE_2)
    {
        msg13.DestinationID1 = pDestinationID[0];       //lq 30 bits
        msg13.SequenceNumber1 = pSequenceNumber[0];     //lq 2 bits
        msg13.DestinationID2 = pDestinationID[1];       //lq 30 bits
        msg13.SequenceNumber2 = pSequenceNumber[1];     //lq 2 bits
        msg13.AckType = AIS_MSG7_13_ACK_TYPE_2;
    }
    else if (ackType == AIS_MSG7_13_ACK_TYPE_3)
    {
        msg13.DestinationID1 = pDestinationID[0];       //lq 30 bits
        msg13.SequenceNumber1 = pSequenceNumber[0];     //lq 2 bits
        msg13.DestinationID2 = pDestinationID[1];       //lq 30 bits
        msg13.SequenceNumber2 = pSequenceNumber[1];     //lq 2 bits
        msg13.DestinationID3 = pDestinationID[2];       //lq 30 bits
        msg13.SequenceNumber3 = pSequenceNumber[2];     //lq 2 bits
        msg13.AckType = AIS_MSG7_13_ACK_TYPE_3;        
    }
    else if (ackType == AIS_MSG7_13_ACK_TYPE_4)
    {
        msg13.DestinationID1 = pDestinationID[0];       //lq 30 bits
        msg13.SequenceNumber1 = pSequenceNumber[0];     //lq 2 bits
        msg13.DestinationID2 = pDestinationID[1];       //lq 30 bits
        msg13.SequenceNumber2 = pSequenceNumber[1];     //lq 2 bits
        msg13.DestinationID3 = pDestinationID[2];       //lq 30 bits
        msg13.SequenceNumber3 = pSequenceNumber[2];     //lq 2 bits
        msg13.DestinationID4 = pDestinationID[3];       //lq 30 bits
        msg13.SequenceNumber4 = pSequenceNumber[3];     //lq 2 bits
        msg13.AckType = AIS_MSG7_13_ACK_TYPE_4;    
    }
    else
    {
        msg13.DestinationID1 = pDestinationID[0];       //lq 30 bits
        msg13.SequenceNumber1 = pSequenceNumber[0];     //lq 2 bits
        msg13.AckType = AIS_MSG7_13_ACK_TYPE_1;    
    }
                                                        //lq total 72-168 bits，1 slot

    /*lq 构造消息的二进制表示 */
    AISMSG_BuildMsg13(&msg13);
}

/*******************************************************************************
* 名称  : InitMsg14
* 描述  : 初始化消息14
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitMsg14(void)
{
    S16 i;
    AISMsgTypeStruct_14 msg14;
	BITMAP msg_bitmap;

    msg14.MessageID = 14;                                                    //lq 6 bits
    msg14.RepeatIndicator = 0;                                               //lq 2 bits
    msg14.SourceID = AisStaticPara.MMSI;                                     //lq 30 bits
    //  Spare;                                                               //lq 2 bits, all 0

    /*lq Safety related text */
    for (i = 0; i < BBMPara.EncapDataLen; i++)
    {
        msg14.SafetyRelatedText[i] = AsciiToSixBinCode(BBMPara.EncapData[i]);//lq Maximum 936 bits (6-bit ASCII)    
    }
    msg14.TextLen = BBMPara.EncapDataLen;
                                                                             //lq Maximum 1008 bits，1-5 slots(B-Class mobile AIS station not exceed 2 slots)

    /*lq 构造消息的二进制表示 */
    AISMSG_BuildMsg14(&msg14);
}

/*******************************************************************************
* 名称  : InitMsg18
* 描述  : 初始化消息18
* 输入  : 无		  
* 输出  : commStateType - 通信状态类型
          commState - 通信状态 
* 返回  : 无
*******************************************************************************/
void InitMsg18(AISMsgCommStateEnum commStateType, AISMsgCommStateUnion commState)
{
    AISMsgTypeStruct_18 msg18;

    /*lq 初始化消息各个字段的值 */
    msg18.MessageID = 18;                                               //lq 6 bits
    msg18.RepeatIndicator = 0;                                          //lq 2 bits
    msg18.UserID = AisStaticPara.MMSI;                                  //lq 30 bits
    //  Spare;                                                          //lq 8 bits, all 0
    msg18.SOG = AisDynamicPara.SOG;                                     //lq 10 bits
    msg18.PositionAccuracy = AisDynamicPara.PositionAccuracy;           //lq 1 bit
    msg18.Longitude = AisDynamicPara.Longitude;                         //lq 28 bits
    msg18.Latitude = AisDynamicPara.Latitude;                           //lq 27 bits
    msg18.COG = AisDynamicPara.COG;                                     //lq 12 bits
    msg18.TrueHeading = AisDynamicPara.TrueHeading;                     //lq 9 bits
    msg18.TimeStamp = AisDynamicPara.TimeStamp;                         //lq 6 bits
    //  Spare;                                                          //lq 2 bits, all 0
    msg18.ClassBUnitFlag = AisDynamicPara.ClassBUnitFlag;               //lq 1 bit
    msg18.ClassBDisplayFlag = AisDynamicPara.ClassBDisplayFlag;         //lq 1 bit
    msg18.ClassBDSCFlag = AisDynamicPara.ClassBDscFlag;                 //lq 1 bit
    msg18.ClassBBandFlag = AisDynamicPara.ClassBBandFlag;               //lq 1 bit
    msg18.ClassBMsg22Flag = AisDynamicPara.ClassBMsg22Flag;             //lq 1 bit
    msg18.ModeFlag = AisDynamicPara.ModeFlag;                           //lq 1 bit
    msg18.RAIMFlag = AisDynamicPara.RaimFlag;                           //lq 1 bit
    msg18.CommStateSelectFlag = AisDynamicPara.CommStateSelectorFlag;   //lq 1 bit
    if (commStateType == AIS_MSG_COMMSTATE_SOTDMA)
    {
        msg18.CommunicationState.SOTDMACommState.SyncState
            = commState.SOTDMACommState.SyncState;                      //lq 2 bits
        msg18.CommunicationState.SOTDMACommState.SlotTimeout
            = commState.SOTDMACommState.SlotTimeout;                    //lq 3 bits
        msg18.CommunicationState.SOTDMACommState.SubMessage.SubMsg
            = commState.SOTDMACommState.SubMessage.SubMsg;              //lq 14 bits
    }
    else
    {
        msg18.CommunicationState.ITDMACommState.SyncState
            = commState.ITDMACommState.SyncState;                       //lq 2 bits
        msg18.CommunicationState.ITDMACommState.SlotIncrement
            = commState.ITDMACommState.SlotIncrement;                   //lq 13 bits
        msg18.CommunicationState.ITDMACommState.NumberOfSlots
            = commState.ITDMACommState.NumberOfSlots;                   //lq 3 bits
        msg18.CommunicationState.ITDMACommState.KeepFlag
            = commState.ITDMACommState.KeepFlag;                        //lq 1 bit        
    }
                                                                        //lq total 168 bits，1 slot

    /*lq 构造消息的二进制表示 */
    AISMSG_BuildMsg18(&msg18);
}

/*******************************************************************************
* 名称  : InitMsg18Test
* 描述  : 初始化测试消息18
* 输入  : 无		  
* 输出  : commStateType - 通信状态类型
          commState - 通信状态 
* 返回  : 无
*******************************************************************************/
void InitMsg18Test(void)
{
    AISMsgTypeStruct_18 msg18;

    /*lq 初始化消息各个字段的值 */
    msg18.MessageID = 18;                                               //lq 6 bits
    msg18.RepeatIndicator = 0;                                          //lq 2 bits
    msg18.UserID = AIS_INFO_MMSI_DEFAULT;                                  //lq 30 bits
    //  Spare;                                                          //lq 8 bits, all 0
    msg18.SOG = AIS_INFO_SOG_DEFAULT;                                     //lq 10 bits
    msg18.PositionAccuracy = AIS_INFO_POSITION_ACCURACY_DEFAULT;           //lq 1 bit
    msg18.Longitude = AIS_INFO_LONGITUDE_DEFAULT;                         //lq 28 bits
    msg18.Latitude = AIS_INFO_LATITUDE_DEFAULT;                           //lq 27 bits
    msg18.COG = AIS_INFO_COG_DEFAULT;                                     //lq 12 bits
    msg18.TrueHeading = AIS_INFO_TRUE_HEADING_DEFAULT;                     //lq 9 bits
    msg18.TimeStamp = AIS_INFO_TIME_STAMP_DEFAULT;                         //lq 6 bits
    //  Spare;                                                          //lq 2 bits, all 0
    msg18.ClassBUnitFlag = AIS_INFO_CLASS_B_UNIT_FLAG_DEFAULT;               //lq 1 bit
    msg18.ClassBDisplayFlag = AIS_INFO_CLASS_B_DISPLAY_FLAG_DEFAULT;         //lq 1 bit
    msg18.ClassBDSCFlag = AIS_INFO_CLASS_B_DSC_FLAG_DEFAULT;                 //lq 1 bit
    msg18.ClassBBandFlag = AIS_INFO_CLASS_B_BAND_FLAG_DEFAULT;               //lq 1 bit
    msg18.ClassBMsg22Flag = AIS_INFO_CLASS_B_MSG22_FLAG_DEFAULT;             //lq 1 bit
    msg18.ModeFlag = AIS_INFO_CLASS_B_MODE_FLAG_DEFAULT;                           //lq 1 bit
    msg18.RAIMFlag = AIS_INFO_CLASS_B_RAIM_FLAG_DEFAULT;                           //lq 1 bit
    msg18.CommStateSelectFlag = AIS_INFO_CLASS_B_COMM_STATE_FLAG_DEFAULT;   //lq 1 bit
    msg18.CommunicationState.SOTDMACommState.SyncState
        = 0;                      											//lq 2 bits
    msg18.CommunicationState.SOTDMACommState.SlotTimeout
        = 0;                   												//lq 3 bits
    msg18.CommunicationState.SOTDMACommState.SubMessage.SubMsg
            = 0;              													//lq 14 bits

    /*lq 构造消息的二进制表示 */
    AISMSG_BuildMsg18(&msg18);
}

/*******************************************************************************
* 名称  : InitMsg19
* 描述  : 初始化消息19
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitMsg19(void)
{
    AISMsgTypeStruct_19 msg19;

    msg19.MessageID = 19;                                                   //lq 6 bits
    msg19.RepeatIndicator = 0;                                              //lq 2 bits
    msg19.UserID = AisStaticPara.MMSI;                                      //lq 30 bits
    //  Spare;                                                              //lq 8 bits, all 0
    msg19.SOG = AisDynamicPara.SOG;                                         //lq 10 bits
    msg19.PositionAccuracy = AisDynamicPara.PositionAccuracy;               //lq 1 bit
    msg19.Longitude = AisDynamicPara.Longitude;                             //lq 28 bits
    msg19.Latitude = AisDynamicPara.Latitude;                               //lq 27 bits
    msg19.COG = AisDynamicPara.COG;                                         //lq 12 bits
    msg19.TrueHeading = AisDynamicPara.TrueHeading;                         //lq 9 bits
    msg19.TimeStamp = AisDynamicPara.TimeStamp;                             //lq 6 bits
    //  Spare;                                                              //lq 4 bits, all 0
    strncpy(msg19.Name, AisStaticPara.ShipName, AIS_INFO_SHIP_NAME_LEN);    //lq 120 bits (6-bit ASCII)
    msg19.TypeOfShipAndCargo = AisStaticPara.TypeOfShip;                    //lq 8 bits
    msg19.DimensionA = AisStaticPara.ShipA;                                 //lq 9 bits
    msg19.DimensionB = AisStaticPara.ShipB;                                 //lq 9 bits
    msg19.DimensionC = AisStaticPara.ShipC;                                 //lq 6 bits
    msg19.DimensionD = AisStaticPara.ShipD;                                 //lq 6 bits
    msg19.TypeOfEPFD = AisStaticPara.TypeOfEPFS;                            //lq 4 bits
    msg19.RAIMFlag = AisDynamicPara.RaimFlag;                               //lq 1 bit
    msg19.DTE = AisStaticPara.DTE;                                          //lq 1 bits
    msg19.AssignedModeFlag = AisDynamicPara.ModeFlag;                       //lq 1 bit
    //  Spare;                                                              //lq 4 bits, all 0
                                                                            //lq total 312 bits，2 slots
    /*lq 构造消息的二进制表示 */
    AISMSG_BuildMsg19(&msg19);
}

#if 0
/*******************************************************************************
* 名称  : InitMsg21
* 描述  : 初始化消息21
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitMsg21(void)
{
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_4;
	U8 len_5;
	U8 i;
    U8 Temp;

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));

	msg_bitmap.array = G_Msg21;

	bitmap_set_value(&msg_bitmap, 0, 8);//占位                                  //lq msg_bitmap.array[0]存放数据的长度（单位byte）

	bitmap_set_value(&msg_bitmap, 21, 6);                                       // Message ID
	bitmap_set_value(&msg_bitmap, 0, 2);                                        // Repeat indicator
	bitmap_set_value(&msg_bitmap, aiAISCell.UserID, 30);                        // User ID
	bitmap_set_value(&msg_bitmap, aiAISCell.TypeofAtoN, 5);                     // Type of aids-tonavigation
	
    /*lq AtoN name*/
    Temp = strlen(aiAISCell.Name);
    if (Temp >= 20)
    {
        for(i = 0; i < 20; i++)
    	{
    		bitmap_set_value(&msg_bitmap, aiAISCell.Name[i], 6);                // Name of Aidsto-Navigation
    	}    
    }
    else
    {
        for(i = 0; i < Temp; i++)
    	{
    		bitmap_set_value(&msg_bitmap, aiAISCell.Name[i], 6);                // Name of Aidsto-Navigation
    	}
        
        /*lq 不足20个字符用‘@’填充*/
        for(i = Temp; i < 20; i++)
        {
    		bitmap_set_value(&msg_bitmap, '@', 6);                                  // Name of Aidsto-Navigation        
        }                      
    }
       	
	bitmap_set_value(&msg_bitmap, Msg18_Positionaccuracy, 1);                   // Position accuracy
	bitmap_set_value(&msg_bitmap, Msg18_Longitude, 28);                         // Longitude
	bitmap_set_value(&msg_bitmap, Msg18_Latitude, 27);                          // Latitude
	bitmap_set_value(&msg_bitmap, aiAISCell.Dimension_Position[0], 9);          // Dimension A of ship/reference for position
	bitmap_set_value(&msg_bitmap, aiAISCell.Dimension_Position[1], 9);          // Dimension B of ship/reference for position 
	bitmap_set_value(&msg_bitmap, aiAISCell.Dimension_Position[2], 6);          // Dimension C of ship/reference for position
	bitmap_set_value(&msg_bitmap, aiAISCell.Dimension_Position[3], 6);          // Dimension D of ship/reference for position
    
    /*lq 根据AtoN类型和AtoN虚拟标志设置消息中EPFS类型*/
    if ((IsFixedAtoN() == TRUE) 
        || (siStaticPara.VirtualAtoNFlag == VIRTUAL_ATON_FLAG_VIRTUAL))
    {
	    bitmap_set_value(&msg_bitmap, 7, 4);                                    // Type of electronic position fixing device        
    }
    else
    {
	    bitmap_set_value(&msg_bitmap, aiAISCell.TypeofEPFS, 4);                 // Type of electronic position fixing device    
    }

	bitmap_set_value(&msg_bitmap, Msg18_Timestamp, 6);                          // Time stamp	
    bitmap_set_value(&msg_bitmap, Msg21_OffPositionIndicator, 1);               // Off-position indicator
    bitmap_set_value(&msg_bitmap, aiAISCell.AtoNStatus, 8);                     // AtoN status
	bitmap_set_value(&msg_bitmap, Msg18_RAIMflag, 1);                           // RAIM-flag
	bitmap_set_value(&msg_bitmap, aiAISCell.VirtualAtoNFlag, 1);                // Virtual AtoN flag
	bitmap_set_value(&msg_bitmap, Msg19_AssignedModeFlag, 1);                   // Assigned mode flag
	bitmap_set_value(&msg_bitmap, 0, 1);                // Spare
    
    /*lq AtoN name 扩展*/
    if (strlen(aiAISCell.Name) > 20)
    {
    	Temp = strlen(aiAISCell.Name);
        for(i = 20; i < Temp; i++)
    	{
    		bitmap_set_value(&msg_bitmap, aiAISCell.Name[i], 6);                // Name of Aidsto-Navigation
    	}

        /*lq 边界对齐*/
        i = i - 20;     //lq 超出字符数
        i = 6 * i;      //lq 超出比特数
        i = i % 8;
        if ( i != 0)
        {
            i = 8 - i;  //lq 填充比特数 
            bitmap_set_value(&msg_bitmap, 0, i);                                // Name of Aidsto-Navigation        
        }               
    }

    len_4 = msg_bitmap.index;
	len_5 = msg_bitmap.offset == 0 ? len_4 - 1 : len_4;
	
	msg_bitmap.index = 0;
	msg_bitmap.offset= 0;
	
	bitmap_set_value(&msg_bitmap, len_5, 8);
}
#endif

/*******************************************************************************
* 名称  : InitMsg24A
* 描述  : 初始化消息24A
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitMsg24A(void)
{
    AISMsgTypeStruct_24A msg24A;

    msg24A.MessageID = 24;                                                  //lq 6 bits
    msg24A.RepeatIndicator = 0;                                             //lq 2 bits
    msg24A.UserID = AisStaticPara.MMSI;                                     //lq 30 bits
    msg24A.PartNumber = 0;                                                  //lq 2 bits
    strncpy(msg24A.Name, AisStaticPara.ShipName, AIS_INFO_SHIP_NAME_LEN);   //lq 120 bits (6-bit ASCII)
                                                                            //lq total 160 bits，1 slot

    /*lq 构造消息的二进制表示 */
    AISMSG_BuildMsg24A(&msg24A);
}

/*******************************************************************************
* 名称  : InitMsg24B
* 描述  : 初始化消息24B
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitMsg24B(void)
{
    S16 i;
    AISMsgTypeStruct_24B msg24B;

    msg24B.MessageID = 24;                                                      //lq 6 bits
    msg24B.RepeatIndicator = 0;                                                 //lq 2 bits
    msg24B.UserID = AisStaticPara.MMSI;                                         //lq 30 bits
    msg24B.PartNumber = 1;                                                      //lq 2 bits
    msg24B.TypeOfShipAndCargo = AisStaticPara.TypeOfShip;                       //lq 8 bits
    for (i = 0; i < 3; i++)
    {   
        msg24B.VendorID.ManufacturerID[i] = (AisStaticPara.VendID[i] & 0x3F);   //lq 18bits,3 6-bit ASCII characters
    }
    msg24B.VendorID.UnitModelCode = (AisStaticPara.VendID[3] >> 2) & 0x0F;      //lq 4bits,0-15
    msg24B.VendorID.UnitSerialNumber = (AisStaticPara.VendID[3] & 0x03) << 18
        || ((AisStaticPara.VendID[4] & 0x3F) << 12)
        || ((AisStaticPara.VendID[5] & 0x3F) << 6)
        || ((AisStaticPara.VendID[7] & 0x3F));                                  //lq 20bits
    strncpy(msg24B.CallSign, AisStaticPara.CallSign, AIS_INFO_CALL_SIGN_LEN);   //lq 42 bits (6-bit ASCII)
    msg24B.DimensionA = AisStaticPara.ShipA;                                    //lq 9 bits
    msg24B.DimensionB = AisStaticPara.ShipB;                                    //lq 9 bits
    msg24B.DimensionC = AisStaticPara.ShipC;                                    //lq 6 bits
    msg24B.DimensionD = AisStaticPara.ShipD;                                    //lq 6 bits
    //  Spare;                                                                  //lq 6 bits, all 0
                                                                                //lq total 168 bits，1 slot

    /*lq 构造消息的二进制表示 */
    AISMSG_BuildMsg24B(&msg24B);
}

#if 0
/******************************************
函数：InitMsgPara
功能: 初始化消息参数
输入:
输出:
返回值：
备注：
*******************************************/
USINT InitMsgPara(void)
{
	Msg18_Reservedforregional = 0;
	Msg18_SOG = 1023;//0;//1023;
	Msg18_Positionaccuracy = 0;
	Msg18_Longitude = 0x6791AC0;//0x4310D2C;//0x6791AC0;
	Msg18_Latitude = 0x3412140;//0x1667B7C;//0x3412140;
	Msg18_COG = 3600;//0;//3600;
	Msg18_Trueheading = 511;
//lq	Msg18_Timestamp = 60;
	Msg18_Timestamp = 63;
	Msg18_Reservedforregionalapplication = 1;
	Msg18_B_unitflag = 0;
	Msg18_B_displayflag = 0;
	Msg18_B_dscflag = 0;
	Msg18_B_bandflag = 1;
	Msg18_B_22flag = 0;
	Msg18_B_modeflag = 0;
	Msg18_RAIMflag = 0;
	Msg18_Communicationstateselectorflag =1;
	Msg18_CommunicationState = 0xD86E;//ITDMA方式的0x1D86E代表：同步方式为UTC直接同步，offset为7分钟，占连续3个时隙，不占用下一帧中的该时隙

	Msg19_Typeofelectronicposition = siStaticPara.TypeofEPFS;
	Msg19_RAIMflag = 0;
	Msg19_DTE = 1;
    Msg19_AssignedModeFlag = 0;
    Msg21_OffPositionIndicator = 0;
#if CONFIG_MSG_6_DAC_IFM
    Msg06_DAC = 235;                    
    Msg06_FI = 10;
#else
    Msg06_DAC = 413;                    
    Msg06_FI = 1;
#endif
    Msg06_AnalogueInternal = 720;       //lq 对应36v
    Msg06_AnalogueExternalNo1 = 720;    //lq 对应36v;
    Msg06_AnalogueExternalNo2 = 720;    //lq 对应36v;
    Msg06_StatusInternal = 0x0;         //lq... 需后期确定
    Msg06_StatusExternal = 0x0;         //lq... 需后期确定
    Msg06_OffPositionStatus = 0;        //lq on position
    
    InitMsg21();
    InitMsg6();
	return 0;
}
#endif

#if 0 
/******************************************
函数：ResetMsgPara
功能: 待机唤醒后初始化消息参数
输入:
输出:
返回值：
备注：
*******************************************/
void ResetMsgPara(void)
{
    AisDynamicPara.SOG = GpsPara.GPS_Sog;
	AisDynamicPara.COG = GpsPara.GPS_Cog;
    AisDynamicPara.Longitude = GpsPara.GPS_Longitud;
	AisDynamicPara.Latitude = GpsPara.GPS_Latitude;
}
#endif

/*******************************************************************************
* 名称  : UpdateDynamicReportInterval
* 描述  : 更新动态消息报告间隔，（目前仅根据航速改变报告间隔，ClassB"SO" 由
          "Nominal reporting interval"变为"Increased reporting interval"的条件
          “当前时隙的占用情况”的判断也可在该函数中进行）
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void UpdateDynamicReportInterval(void)
{
    if (AisDynamicPara.SOG != AIS_MSG_SOG_INVALID)
    {
        if (AisDynamicPara.SOG <= TDMA_REPORT_INTERVAL_SOG_MAX_B_3M)
        {
            TDMA_InstReportIntervalSample(TDMA_REPORT_INTERVAL_SLOT_3M);
        }
        else if (AisDynamicPara.SOG <= TDMA_REPORT_INTERVAL_SOG_MAX_B_30S)
        {
            TDMA_InstReportIntervalSample(TDMA_REPORT_INTERVAL_SLOT_30S);
        }
        else if (AisDynamicPara.SOG <= TDMA_REPORT_INTERVAL_SOG_MAX_B_15S)
        {
            TDMA_InstReportIntervalSample(TDMA_REPORT_INTERVAL_SLOT_15S);
        }
        else
        {
            TDMA_InstReportIntervalSample(TDMA_REPORT_INTERVAL_SLOT_5S);
        }
    }
}

/*******************************************************************************
* 名称  : UpdateAisDynamicInfo
* 描述  : 更新AIS动态信息
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U16 UpdateAisDynamicInfo(void)
{
	U8 rc;
    float sfTemp;	

	rc = GetGpsFixSta(&pGpsCell);

	if (rc == TRUE)
	{
		AisDynamicPara.SOG = pGpsCell.GPS_Sog;
        AisDynamicPara.SOG = AisDynamicPara.SOG > 1022 ? 1022 : AisDynamicPara.SOG;

        AisDynamicPara.Longitude = pGpsCell.GPS_Longitud;
		AisDynamicPara.Latitude = pGpsCell.GPS_Latitude;

		AisDynamicPara.COG = pGpsCell.GPS_Cog;
        
        /*lq 时间戳更新*/
        AisDynamicPara.TimeStamp = pGpsCell.GPS_TimeS;
    }
    else
    {               
        /*lq 时间戳更新*/
        AisDynamicPara.TimeStamp = AIS_INFO_TIME_STAMP_DEFAULT;
    }

    /*lq RAIM和PA更新*/
    if (pGpsCell.GPS_RAIMFlag == TRUE)
    {
        /*lq 更新RAIM*/
        AisDynamicPara.RaimFlag = 1;
        
        /*lq 计算EXPECTED RAIM error，更新PA*/
        sfTemp =  sqrt(pGpsCell.GPS_errlat * pGpsCell.GPS_errlat + pGpsCell.GPS_errlon * pGpsCell.GPS_errlon);
        if (sfTemp <= RAIM_ERR_THRES)
        {
            AisDynamicPara.PositionAccuracy = 1;
        }
        else
        {
            AisDynamicPara.PositionAccuracy = 0;        
        }   
    }
    else
    {
        /*lq 更新RAIM*/ 
        AisDynamicPara.RaimFlag = 0;

        /*lq 根据quality indicator，更新PA*/            
        if (pGpsCell.GPS_QualityIndicator == 0)
        {
            AisDynamicPara.PositionAccuracy = 0;       
        }
        else
        {
            AisDynamicPara.PositionAccuracy = 1;        
        }    
    }

#if 0
    /*lq 更新AIS消息中的静态字段 */
    if (siStaticPara.Refresh == TRUE)
    {
        siStaticPara.Refresh = FALSE;
        AISStcStructInit();
    }
#endif	
    return 0;
}


#if 0
/*******************************************************************************
* 名称  : Update_GpsData_To_Msg
* 描述  : 从底层获取GPS参数，并判断GPS是否定位有效，是否可产生消息
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U16 Update_GpsData_To_Msg(void)
{
	U8 rc;
    float sfTemp;	

	rc = GetGpsFixSta(&pGpsCell);                                               //lq? 为何要另外声明一个GPSStruct变量,而不直接用GpsPara?

	if(rc == TRUE)                                                              //lq 要求往协议栈发送消息
	{
#if !CONFIG_SOG_FOR_BAT_VOLUME
#if 1
		Msg18_SOG = pGpsCell.GPS_Sog;
#else
		Msg18_SOG = GetSOG();
#endif	        
        Msg18_SOG = Msg18_SOG > 1023 ? 1022 : Msg18_SOG;
#endif		
        Msg18_Longitude = pGpsCell.GPS_Longitud;
		Msg18_Latitude = pGpsCell.GPS_Latitude;

#if !CONFIG_COG_FOR_TEMPERATURE
		Msg18_COG = pGpsCell.GPS_Cog;
#endif
        
        /*lq 时间戳更新*/
        //lq Msg18_Timestamp = pGpsCell.GPS_TimeS;                              //lq 每组消息只发一次时，可以使用
        Msg18_Timestamp = ((float)GetSlotNum()) / 37.5;                         //lq 消息发送时GPS有可能已经关闭

        /*lq 若无效则使用之前的值 */
        if (OP1_GetGPSOffPositionValid() == TRUE)
        {
            if (OP1_GetGPSOffPositionStat() == TRUE)        
            {
                Msg21_OffPositionIndicator = 1;            
            }
            else
            {
                Msg21_OffPositionIndicator = 0;
            }
        }
    }
    else
    {
        if (GetGpsNormalFixState() == FALSE)                                    //lq 与功放是否开启的控制条件一致
        {
            /*lq 经纬度更新*/
            Msg18_Longitude = 0x6791AC0;
    		Msg18_Latitude = 0x3412140;        
        }
                
        /*lq 时间戳更新*/
        Msg18_Timestamp = 63;
    }

    /*lq RAIM和PA更新*/
    if (pGpsCell.GPS_RAIMFlag == TRUE)
    {
        /*lq 更新RAIM*/
        Msg18_RAIMflag = 1;
        Msg19_RAIMflag = 1;
        
        /*lq 计算EXPECTED RAIM error，更新PA*/
        sfTemp =  sqrt(pGpsCell.GPS_errlat * pGpsCell.GPS_errlat + pGpsCell.GPS_errlon * pGpsCell.GPS_errlon);
        if (sfTemp <= RAIM_ERR_THRES)
        {
            Msg18_Positionaccuracy = 1;
        }
        else
        {
            Msg18_Positionaccuracy = 0;        
        }   
    }
    else
    {
        /*lq 更新RAIM*/ 
        Msg18_RAIMflag = 0;
        Msg19_RAIMflag = 0;

        /*lq 根据quality indicator，更新PA*/            
        if (pGpsCell.GPS_QualityIndicator == 0)
        {
            Msg18_Positionaccuracy = 0;       
        }
        else
        {
            Msg18_Positionaccuracy = 1;        
        }    
    }

    /*lq 更新AIS消息中的静态字段 */
    if (siStaticPara.Refresh == TRUE)
    {
        siStaticPara.Refresh = FALSE;
        AISStcStructInit();
    }

#if CONFIG_SOG_FOR_BAT_VOLUME
    
    /*lq 航速改用作电池电量百分比*/
    if (!((GetBurstStarFlg() == TRUE) && (GetBurstFinsFlg() == FALSE))) //lq 消息发送期间不更新SOG，此时电压值不准确
    {
        Msg18_SOG = (GetBatteryVolume() + 0.05) * 10;      //lq 对应0-1 
        Msg18_SOG = Msg18_SOG > 1023 ? 1022 : Msg18_SOG;
    }
#endif

#if CONFIG_COG_FOR_TEMPERATURE
    
    /*lq COG该用作环境温度*/
    if (!((GetBurstStarFlg() == TRUE) && (GetBurstFinsFlg() == FALSE))) //lq 消息发送期间不更新SOG，此时电压值不准确
    {
		Msg18_COG = GetTemperForCOG(struTemperSensorPara.fsTemper);     //lq? GetTemper()的返回值为何不对？
    }
#endif
	
    return 0;
}
#endif

/******************************************
函数：GetTemperForCOG
功能: 将环境温度转换为COG，环境温度的默认范围
      为-99.9~+99.9，超出此范围的温度用COG表示
      无效
输入:
输出:
返回值：转换为COG后的环境温度
备注：
*******************************************/
U16 GetTemperForCOG(float Temper)
{
    U16 TemperForCOG;

    if (Temper >= 0.0)
    {
        /*lq 温度为正*/
        Temper += 0.05;                         //lq 近似
        TemperForCOG = (U16)(Temper * 10);      //lq 转换为COG
    }
    else
    {
        /*lq 温度为负*/
        Temper = 100.05 - Temper;               //lq 负变正，近似，同时将百位置1标志温度为负
        TemperForCOG = (U16)(Temper * 10);      //lq 转换为COG
    }

    /*lq COG有效值判断*/
    if (TemperForCOG > 3600)
    {
        TemperForCOG = 3600;
    }

    return TemperForCOG;
}

#if CONFIG_BAT_MONITOR 
/******************************************
函数：GetBatteryVolume
功能: 计算电池电量
输入:
输出:
返回值：BatVolumePerct:电池电量百分比
备注：
*******************************************/
float GetBatteryVolume(void)
{
    float BatVolumePerct;
    
    BatVolumePerct = (float)(BS1_GetCurVoltValue() - BATTERY_VOLUME_LEAST_VOLT) 
            / (float)(BATTERY_VOLUME_MOST_VOLT - BATTERY_VOLUME_LEAST_VOLT);

    /*lq 最低指示0.1*/
    if (BatVolumePerct < 0.1)
    {
        BatVolumePerct = 0.1;
    }
    else if (BatVolumePerct > 1.0)
    {
        BatVolumePerct = 1.0;
    }

    return BatVolumePerct;
}
#endif
/*******************************************************************************
* 名称  : GetSOG
* 描述  : 根据经纬度计算航速
* 输入  : 无		  
* 输出  : 无
* 返回  : 当前航速
*******************************************************************************/
U16 GetSOG(void)
{
    S16 ssSDiff;
	S16 ssMDiff;
	S16 ssHDiff;
	S16 ssDDiff;
    S16 ssSecDiffSum;
	U8 ucValidFlg;
    U8 ucSpanFlg;
	U16 usSOG;
	double latitudeLast_D;
	double longitudLast_D;
	double latitude_D;
	double longitud_D;
	double distance;
	double speed;

    if (pGpsCell.GPS_FirstLLFlg == TRUE)                                           //lq 不是开机第一次
    {
        if (GpsPara.GPS_LonLatFilterFlg == TRUE)
        {
        	/*lq 计算当前时间和上一次之差*/
            ssSDiff = pGpsCell.GPS_TimeS - pGpsCell.GPS_TimeS_Last;
            ssMDiff = pGpsCell.GPS_TimeM - pGpsCell.GPS_TimeM_Last;
        	ssHDiff = pGpsCell.GPS_TimeH - pGpsCell.GPS_TimeH_Last;
        	ssDDiff = pGpsCell.GPS_TimeD - pGpsCell.GPS_TimeD_Last;
            
            if (ssSDiff < 0)
            {
                ssSDiff += 60;
                ssMDiff--;
            }
        
            if (ssMDiff < 0)
            {
                ssMDiff += 60;
                ssHDiff--;
            }
        
            if (ssHDiff < 0)
            {
                ssHDiff += 24;
                ssDDiff--;
            }
        
            if (ssDDiff < 0)                                                        //lq 跨月，不考虑    
            {
                ucValidFlg = FALSE;
            }
            else
            {            
                ucValidFlg = TRUE;
    
                ssSecDiffSum = ssSDiff + ssMDiff * 60 + ssHDiff * 3600;                
    		}
            
            if (ucValidFlg && ssSecDiffSum >= 46 && ssSecDiffSum <= 1800)           //lq 46s~30min    
            {
                ucSpanFlg = TRUE;
            }
            else
            {
                ucSpanFlg = FALSE;
            }
        
            /*lq 根据经纬度计算航速*/
            if (ucSpanFlg)
        	{
        		/*lq 0.0001分转换为度*/
        		latitudeLast_D = (double)pGpsCell.GPS_Latitude_Last / 600000.0;
        		longitudLast_D = (double)pGpsCell.GPS_Longitud_Last / 600000.0;
        	 	latitude_D = (double)pGpsCell.GPS_Latitude / 600000.0;
        		longitud_D = (double)pGpsCell.GPS_Longitud / 600000.0;
        		
        		distance = GetDistance(latitude_D, longitud_D, latitudeLast_D, longitudLast_D);
        		distance /= 1852.0;													//lq 换算成海里
    
                speed = distance / (double)ssSecDiffSum * 3600.0;					//lq 航速knot                		
                usSOG = (U16)(speed * 10 + 0.5);									//lq Msg18_SOG的单位为0.1knot
                pGpsCell.GPS_Sog_Last = usSOG;
    
            #if 1            
                usart_printf("distance: %lf\r\n", distance);//lqdb
                usart_printf("ssSecDiffSum: %d\r\n", ssSecDiffSum);//lqdb
    
                usart_printf("SOG_GPS: %ld\r\n", pGpsCell.GPS_Sog);//lqdb
                usart_printf("SOG_CAL: %ld\r\n", usSOG);//lqdb    
            #endif 		        
            }
        	else                                                                    //lq 使用GPS报文的速度    
        	{
        	    //lq usSOG = pGpsCell.GPS_Sog;
                usSOG = pGpsCell.GPS_Sog_Last;                                  //lq 使用上次计算的速度
        	}
        
            /*lq 记录当前经纬度和时间*/
            if (!(ucValidFlg && ssSecDiffSum < 46))
            {
        		pGpsCell.GPS_Latitude_Last = pGpsCell.GPS_Latitude;
        		pGpsCell.GPS_Longitud_Last = pGpsCell.GPS_Longitud;
        		pGpsCell.GPS_TimeD_Last = pGpsCell.GPS_TimeD;
        		pGpsCell.GPS_TimeH_Last = pGpsCell.GPS_TimeH;
        		pGpsCell.GPS_TimeM_Last = pGpsCell.GPS_TimeM;		    
        		pGpsCell.GPS_TimeS_Last = pGpsCell.GPS_TimeS;		      
            }         
        }
        else
        {
            usSOG = pGpsCell.GPS_Sog;//lqdb                		            
        }
    }
    else
    {
        if (GpsPara.GPS_LonLatFilterFlg == TRUE)
        {
    		pGpsCell.GPS_Latitude_Last = pGpsCell.GPS_Latitude;
    		pGpsCell.GPS_Longitud_Last = pGpsCell.GPS_Longitud;
    		pGpsCell.GPS_TimeD_Last = pGpsCell.GPS_TimeD;
    		pGpsCell.GPS_TimeH_Last = pGpsCell.GPS_TimeH;
    		pGpsCell.GPS_TimeM_Last = pGpsCell.GPS_TimeM;		    
    		pGpsCell.GPS_TimeS_Last = pGpsCell.GPS_TimeS;    	
            pGpsCell.GPS_Sog_Last = pGpsCell.GPS_Sog;
            
            pGpsCell.GPS_FirstLLFlg = TRUE; 		    	
        }
      
        usSOG = pGpsCell.GPS_Sog;//lqdb                		    
    }

	return usSOG;
}

#if 1
const double EARTH_RADIUS = 6378.137;
const double PI = 3.1415926;
/*******************************************************************************
* 名称  : rad
* 描述  : 度转换为弧度
* 输入  : 度		  
* 输出  : 无
* 返回  : 弧度
*******************************************************************************/
static double rad(double d)
{
   return d * PI / 180.0;
}

#if 0
/*******************************************************************************
* 名称  : GetDistance
* 描述  : 根据两点的经纬度计算两点间的距离
* 输入  : 两点的经纬度		  
* 输出  : 无
* 返回  : 两点间的距离
*******************************************************************************/ 
double GetDistance(double lat1, double lng1, double lat2, double lng2)
{
   double radLat1 = rad(lat1);
   
   double radLat2 = rad(lat2);
   
   double a = radLat1 - radLat2;
   
   double b = rad(lng1) - rad(lng2);
   
   double s = 2 * asin(sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2)));
   
   s = s * EARTH_RADIUS;
   
//   s = round(s * 10000) / 10000;		  										//lq 精确到1分米
   
   return s;
}
#else
/*******************************************************************************
* 名称  : GetDistance
* 描述  : 根据两点的经纬度计算两点间的距离
* 输入  : 两点的经纬度		  
* 输出  : 无
* 返回  : 两点间的距离（单位：米）
*******************************************************************************/ 
S32 GetDistance(double lat1, double lng1, double lat2, double lng2)
{
   double radLat1 = rad(lat1);
   
   double radLat2 = rad(lat2);
   
   double a = radLat1 - radLat2;
   
   double b = rad(lng1) - rad(lng2);
   
   double s = 2 * asin(sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2)));
   
   s = s * EARTH_RADIUS;
   
   s = s * 1000 + 0.5;		  										//lq 精确到1米
   
   return (S32)s;
}
#endif
#endif

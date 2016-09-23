/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: AISMsgModule.h
  Author: lq       Version : V1.00        Date: 2014.04.18
  Description:     AIS 消息类型定义模块
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _AISMSGMODULE_H
#define _AISMSGMODULE_H

/* Include ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/*lq AIS消息二进制位长度 */
#define AIS_MSG_BIT_LENGTH_1_2_3    (21 * 8)
#define AIS_MSG_BIT_LENGTH_4_11     (21 * 8)
#define AIS_MSG_BIT_LENGTH_5        (53 * 8)
#define AIS_MSG_BIT_LENGTH_6        (126 * 8)
#define AIS_MSG_BIT_LENGTH_7_13     (21 * 8)
#define AIS_MSG_BIT_LENGTH_8        (126 * 8)
#define AIS_MSG_BIT_LENGTH_9        (21 * 8)
#define AIS_MSG_BIT_LENGTH_10       (9 * 8)
#define AIS_MSG_BIT_LENGTH_12       (126 * 8)
#define AIS_MSG_BIT_LENGTH_14       (126 * 8)
#define AIS_MSG_BIT_LENGTH_15       (20 * 8)
#define AIS_MSG_BIT_LENGTH_16       (18 * 8)
#define AIS_MSG_BIT_LENGTH_17       (102 * 8)
#define AIS_MSG_BIT_LENGTH_18       (21 * 8)
#define AIS_MSG_BIT_LENGTH_19       (39 * 8)
#define AIS_MSG_BIT_LENGTH_20       (20 * 8)
#define AIS_MSG_BIT_LENGTH_21       (45 * 8)
#define AIS_MSG_BIT_LENGTH_22       (21 * 8)
#define AIS_MSG_BIT_LENGTH_23       (20 * 8)
#define AIS_MSG_BIT_LENGTH_24A      (20 * 8)
#define AIS_MSG_BIT_LENGTH_24B      (21 * 8)
#define AIS_MSG_BIT_LENGTH_25       (21 * 8)
#define AIS_MSG_BIT_LENGTH_26       (133 * 8)     //lq 最大长度1064bits(Msg26)
#define AIS_MSG_BIT_LENGTH_27       (12 * 8)
#define AIS_MSG_BIT_LENGTH          AIS_MSG_BIT_LENGTH_26   //lq 最大长度

/*lq 消息6应用数据位最大bit数 */
#define AIS_MSG6_APPDATA_BIT_MAXLEN         920            

/*lq 消息8应用数据位最大bit数 */
#define AIS_MSG8_APPDATA_BIT_MAXLEN         952 

/*lq 消息12文本最大字符数 */
#define AIS_MSG12_TEXT_MAXLEN               156

/*lq 消息7_13确认类型 */
#define AIS_MSG7_13_ACK_TYPE_1              1
#define AIS_MSG7_13_ACK_TYPE_2              2
#define AIS_MSG7_13_ACK_TYPE_3              3
#define AIS_MSG7_13_ACK_TYPE_4              4

/*lq 消息14文本最大字符数 */
#define AIS_MSG14_TEXT_MAXLEN               161

/*lq 消息15询问类型 */
#define AIS_MSG15_INTEEROGATION_TYPE_1      1   //lq 只询问一（1）个台站的一（1）个消息
#define AIS_MSG15_INTEEROGATION_TYPE_2      2   //lq 询问一（1）个台站的两（2）个消息
#define AIS_MSG15_INTEEROGATION_TYPE_3      3   //lq 询问第一和第二个台站的各一（1）个消息
#define AIS_MSG15_INTEEROGATION_TYPE_4      4   //lq 询问第一个台站的两（2）个消息和第二个台站的一（1）个消息

/*lq 消息16支配类型 */
#define AIS_MSG16_COMMAND_TYPE_1            1   //lq 为一个台站指配模式
#define AIS_MSG16_COMMAND_TYPE_2            2   //lq 为两个台站指配模式

/*lq 消息20类型 */
#define AIS_MSG20_MANAGEMENT_TYPE_1         1   //lq 指定一组时隙预留方案
#define AIS_MSG20_MANAGEMENT_TYPE_2         2   //lq 指定二组时隙预留方案
#define AIS_MSG20_MANAGEMENT_TYPE_3         3   //lq 指定三组时隙预留方案
#define AIS_MSG20_MANAGEMENT_TYPE_4         4   //lq 指定四组时隙预留方案

/*lq 消息25二进制数据最大bit数 */
#define AIS_MSG25_BINDATA_BIT_MAXLEN_00     128//lq MAXLEN_XY:X-DestinationIndicator,Y-BinaryDataFlag    
#define AIS_MSG25_BINDATA_BIT_MAXLEN_01     112 
#define AIS_MSG25_BINDATA_BIT_MAXLEN_10     98
#define AIS_MSG25_BINDATA_BIT_MAXLEN_11     82

/*lq 消息26二进制数据最大bit数 */
#define AIS_MSG26_BINDATA_BIT_MAXLEN_00     1004//lq MAXLEN_XY:X-DestinationIndicator,Y-BinaryDataFlag    
#define AIS_MSG26_BINDATA_BIT_MAXLEN_01     988 
#define AIS_MSG26_BINDATA_BIT_MAXLEN_10     974
#define AIS_MSG26_BINDATA_BIT_MAXLEN_11     958

/*lq 通信状态默认值 */
#define AIS_MSG_COMMSTATE_SOTDMA_RECEIVED_STATIONS_DEFAULT      0

/*lq AIS消息字段值 */
#define AIS_MSG_SOG_INVALID             1023

/* Private typedef -----------------------------------------------------------*/
/*lq AIS消息通信状态枚举 */
typedef enum
{
    AIS_MSG_COMMSTATE_SOTDMA = 0,
    AIS_MSG_COMMSTATE_ITDMA
}AISMsgCommStateEnum;

/*lq AIS消息类型 */
typedef enum AISMsgType
{
    AISMsgType1,
    AISMsgType2,
    AISMsgType3,
    AISMsgType4,
    AISMsgType5,
    AISMsgType6,
    AISMsgType7,
    AISMsgType8,
    AISMsgType9,
    AISMsgType10,
    AISMsgType11,
    AISMsgType12,
    AISMsgType13,
    AISMsgType14,
    AISMsgType15,
    AISMsgType16,
    AISMsgType17,
    AISMsgType18,
    AISMsgType19,
    AISMsgType20,
    AISMsgType21,
    AISMsgType22,
    AISMsgType23,
    AISMsgType24A,
    AISMsgType24B,
    AISMsgType25,
    AISMsgType26,
    AISMsgType27,
    AISMsgTypeError,
}AISMsgTypeEnum;

/*lq 消息二进制缓存数据组 */
typedef U8  AISMsgBinBuffArray_1_2_3[AIS_MSG_BIT_LENGTH_1_2_3 / 8 + 1];
typedef U8  AISMsgBinBuffArray_4_11[AIS_MSG_BIT_LENGTH_4_11 / 8  + 1];
typedef U8  AISMsgBinBuffArray_5[AIS_MSG_BIT_LENGTH_5 / 8  + 1];
typedef U8  AISMsgBinBuffArray_6[AIS_MSG_BIT_LENGTH_6 / 8  + 1];
typedef U8  AISMsgBinBuffArray_7_13[AIS_MSG_BIT_LENGTH_7_13 / 8  + 1];
typedef U8  AISMsgBinBuffArray_8[AIS_MSG_BIT_LENGTH_8 / 8  + 1];
typedef U8  AISMsgBinBuffArray_9[AIS_MSG_BIT_LENGTH_9 / 8  + 1];
typedef U8  AISMsgBinBuffArray_10[AIS_MSG_BIT_LENGTH_10 / 8  + 1];
typedef U8  AISMsgBinBuffArray_12[AIS_MSG_BIT_LENGTH_12 / 8  + 1];
typedef U8  AISMsgBinBuffArray_14[AIS_MSG_BIT_LENGTH_14 / 8  + 1];
typedef U8  AISMsgBinBuffArray_15[AIS_MSG_BIT_LENGTH_15 / 8  + 1];
typedef U8  AISMsgBinBuffArray_16[AIS_MSG_BIT_LENGTH_16 / 8  + 1];
typedef U8  AISMsgBinBuffArray_17[AIS_MSG_BIT_LENGTH_17 / 8  + 1];
typedef U8  AISMsgBinBuffArray_18[AIS_MSG_BIT_LENGTH_18 / 8  + 1];
typedef U8  AISMsgBinBuffArray_19[AIS_MSG_BIT_LENGTH_19 / 8  + 1];
typedef U8  AISMsgBinBuffArray_20[AIS_MSG_BIT_LENGTH_20 / 8  + 1];
typedef U8  AISMsgBinBuffArray_21[AIS_MSG_BIT_LENGTH_21 / 8  + 1];
typedef U8  AISMsgBinBuffArray_22[AIS_MSG_BIT_LENGTH_22 / 8  + 1];
typedef U8  AISMsgBinBuffArray_23[AIS_MSG_BIT_LENGTH_23 / 8  + 1];
typedef U8  AISMsgBinBuffArray_24A[AIS_MSG_BIT_LENGTH_24A / 8  + 1];
typedef U8  AISMsgBinBuffArray_24B[AIS_MSG_BIT_LENGTH_24B / 8  + 1];
typedef U8  AISMsgBinBuffArray_25[AIS_MSG_BIT_LENGTH_25 / 8  + 1];
typedef U8  AISMsgBinBuffArray_26[AIS_MSG_BIT_LENGTH_26 / 8  + 1];     //lq 最大长度1064bits(Msg26)
typedef U8  AISMsgBinBuffArray_27[AIS_MSG_BIT_LENGTH_27 / 8  + 1];
typedef U8  AISMsgBinBuffArray[AIS_MSG_BIT_LENGTH / 8 + 1];

/*lq SOTDMA通信状态子消息联合体 */
typedef union SOTDMASubMsg{
    U16 ReceivedStations;           //lq 14 bits
    U16 SlotNumber;                 //lq 14 bits
    U16 UTCHourAndMinute;           //lq 14 bits
    U16 SlotOffset;                 //lq 14 bits
    U16 SubMsg;                     //lq 通用名称
}SOTDMASubMsgUnion;

/*lq SOTDMA通信状态结构体 */
typedef struct SOTDMACommState{
    U8  SyncState;                  //lq 2 bits
    U8  SlotTimeout;                //lq 3 bits
    SOTDMASubMsgUnion SubMessage;   //lq 14 bits
}SOTDMACommStateStruct;

/*lq ITDMA通信状态结构体 */
typedef struct ITDMACommState{
    U8  SyncState;                  //lq 2 bits
    U16 SlotIncrement;              //lq 13 bits
    U8  NumberOfSlots;              //lq 3 bits
    U8  KeepFlag;                   //lq 1 bit
}ITDMACommStateStruct;

/*lq AIS消息通信状态联合体 */
typedef union AISMsgCommState{
    SOTDMACommStateStruct SOTDMACommState;
    ITDMACommStateStruct  ITDMACommState;  
}AISMsgCommStateUnion;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Messages 1, 2, 3: Position reports
*/
typedef struct AISMsgType_1_2_3{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 UserID;                     //lq 30 bits
    U8  NavigationalStatus;         //lq 4 bits
    S8  RotAIS;                     //lq 8 bits
    U16 SOG;                        //lq 10 bits
    U8  PositionAccuracy;           //lq 1 bit
    S32 Longitude;                  //lq 28 bits
    S32 Latitude;                   //lq 27 bits
    U16 COG;                        //lq 12 bits
    U16 TrueHeading;                //lq 9 bits
    U8  TimeStamp;                  //lq 6 bits
    U8  SpecialManoeuvreIndicator;  //lq 2 bits
    //  Spare;                      //lq 3 bits, all 0
    U8  RAIMFlag;                   //lq 1 bit
    AISMsgCommStateUnion  CommunicationState;        //lq 19 bits
                                    //lq total 168 bits，1 slot
}AISMsgTypeStruct_1_2_3;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 4: Base station report
 *Message 11: UTC and date response
*/
typedef struct AISMsgType_4_11{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 UserID;                     //lq 30 bits
    U16 UTCYear;                    //lq 14 bits
    U8  UTCMonth;                   //lq 4 bits
    U8  UTCDay;                     //lq 5 bits
    U8  UTCHour;                    //lq 5 bitS
    U8  UTCMinute;                  //lq 6 bits
    U8  UTCSecond;                  //lq 6 bits
    U8  PositionAccuracy;           //lq 1 bit
    S32 Longitude;                  //lq 28 bits
    S32 Latitude;                   //lq 27 bits
    U8  TypeOfEPFD;                 //lq 4 bits
    U8  TxCtrlForLrbm;              //lq 1 bit
    //  Spare;                      //lq 9 bits, all 0
    U8  RAIMFlag;                   //lq 1 bit
    AISMsgCommStateUnion  CommunicationState;        //lq 19 bits      
                                    // lnw  modify   OLD TYPE : SOTDMACommStateStruct   ITmust be :   AISMsgCommStateUnion
                                    //lq total 168 bits，1 slot
}AISMsgTypeStruct_4_11;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 5: Ship static and voyage related data
*/
typedef struct AISMsgType_5{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 UserID;                     //lq 30 bits
    U8  AISVersionIndicator;        //lq 2 bits
    U32 IMONumber;                  //lq 30 bits
    U8  CallSign[7];                //lq 42 bits (6-bit ASCII)
    U8  Name[20];                   //lq 120 bits (6-bit ASCII)
    U8  TypeOfShipAndCargo;         //lq 8 bits
    U16 DimensionA;                 //lq 9 bits
    U16 DimensionB;                 //lq 9 bits
    U8  DimensionC;                 //lq 6 bits
    U8  DimensionD;                 //lq 6 bits
    U8  TypeOfEPFD;                 //lq 4 bits
    U8  ETAMonth;                   //lq 4 bits
    U8  ETADay;                     //lq 5 bits
    U8  ETAHour;                    //lq 5 bits
    U8  ETAMinute;                  //lq 6 bits
    U8  MaxPresentStaticDraught;    //lq 8 bits
    U8  Destination[20];            //lq 120 bits (6-bit ASCII)
    U8  DTE;                        //lq 1 bits
    //  Spare;                      //lq 1 bit, all 0
                                    //lq total 424 bits，2 slots
}AISMsgTypeStruct_5;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 6: Addressed binary message
*/
typedef struct AISMsgType_6{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 SourceID;                   //lq 30 bits
    U8  SequenceNumber;             //lq 2 bits
    U32 DestinationID;              //lq 30 bits
    U8  RetransmitFlag;             //lq 1 bit
    //  Spare;                      //lq 1 bit, all 0
    U16 AI_DAC;                     //lq 10 bits
    U8  AI_FI;                      //lq 6 bits
    U8  ApplicationData[115];       //lq Maximum 920 bits (8-bit byte)
    U16 AppDataBitLen;
                                    //lq Maximum 1008 bits，1-5 slots(B-Class mobile AIS station not exceed 2 slots)
}AISMsgTypeStruct_6;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 7: Binary acknowledge
 *Message 13: Safety related acknowledge
*/
typedef struct AISMsgType_7_13{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 SourceID;                   //lq 30 bits
    //  Spare;                      //lq 2 bits, all 0
    U32 DestinationID1;             //lq 30 bits
    U8  SequenceNumber1;            //lq 2 bits
    U32 DestinationID2;             //lq 30 bits
    U8  SequenceNumber2;            //lq 2 bits
    U32 DestinationID3;             //lq 30 bits
    U8  SequenceNumber3;            //lq 2 bits
    U32 DestinationID4;             //lq 30 bits
    U8  SequenceNumber4;            //lq 2 bits
    U8  AckType;
                                    //lq total 72-168 bits，1 slot
}AISMsgTypeStruct_7_13;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 8: Binary broadcast message
*/
typedef struct AISMsgType_8{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 SourceID;                   //lq 30 bits
    //  Spare;                      //lq 2 bits, all 0
    U16 AI_DAC;                     //lq 10 bits
    U8  AI_FI;                      //lq 6 bits
    U8  ApplicationData[119];       //lq Maximum 952 bits (8-bit byte)
    U16 AppDataBitLen;
                                    //lq Maximum 1008 bits，1-5 slots(B-Class mobile AIS station not exceed 2 slots)
}AISMsgTypeStruct_8;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 9: Standard SAR aircraft position report
*/
typedef struct AISMsgType_9{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 UserID;                     //lq 30 bits
    U16 Altitude;                   //lq 12 bits
    U16 SOG;                        //lq 10 bits
    U8  PositionAccuracy;           //lq 1 bit
    S32 Longitude;                  //lq 28 bits
    S32 Latitude;                   //lq 27 bits
    U16 COG;                        //lq 12 bits
    U8  TimeStamp;                  //lq 6 bits
    U8  AltitudeSensor;             //lq 1 bit
    //  Spare;                      //lq 7 bits, all 0
    U8  DTE;                        //lq 1 bits
    //  Spare;                      //lq 3 bits, all 0
    U8  AssignedModeFlag;           //lq 1 bit
    U8  RAIMFlag;                   //lq 1 bit
    U8  CommStateSelectFlag;        //lq 1 bit
    AISMsgCommStateUnion CommunicationState;         //lq 19 bits
                                    //lq total 168 bits，1 slot
}AISMsgTypeStruct_9;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 10: UTC and date inquiry
*/
typedef struct AISMsgType_10{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 SourceID;                   //lq 30 bits
    //  Spare;                      //lq 2 bits, all 0
    U32 DestinationID;              //lq 30 bits
    //  Spare;                      //lq 2 bits, all 0
                                    //lq total 72 bits，1 slot
}AISMsgTypeStruct_10;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 12: Addressed safety related message
*/
typedef struct AISMsgType_12{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 SourceID;                   //lq 30 bits
    U8  SequenceNumber;             //lq 2 bits
    U32 DestinationID;              //lq 30 bits
    U8  RetransmitFlag;             //lq 1 bit
    //  Spare;                      //lq 1 bit, all 0
    U8  SafetyRelatedText[156];     //lq Maximum 936 bits (6-bit ASCII)
    U8  TextLen;
                                    //lq Maximum 1008 bits，1-5 slots(B-Class mobile AIS station not exceed 2 slots)
}AISMsgTypeStruct_12;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 14: Safety related broadcast message
*/
typedef struct AISMsgType_14{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 SourceID;                   //lq 30 bits
    //  Spare;                      //lq 2 bits, all 0
    U8  SafetyRelatedText[161];     //lq Maximum 968 bits (6-bit ASCII)
    U8  TextLen;
                                    //lq Maximum 1008 bits，1-5 slots(B-Class mobile AIS station not exceed 2 slots)
}AISMsgTypeStruct_14;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 15: Interrogation
*/
typedef struct AISMsgType_15{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 SourceID;                   //lq 30 bits
    //  Spare;                      //lq 2 bits, all 0
    U32 DestinationID1;             //lq 30 bits
    U8  MessageID1_1;               //lq 6 bits
    U16 SlotOffset1_1;              //lq 12 bits
    //  Spare;                      //lq 2 bits, all 0
    U8  MessageID1_2;               //lq 6 bits
    U16 SlotOffset1_2;              //lq 12 bits
    //  Spare;                      //lq 2 bits, all 0
    U32 DestinationID2;             //lq 30 bits
    U8  MessageID2_1;               //lq 6 bits
    U16 SlotOffset2_1;              //lq 12 bits
    //  Spare;                      //lq 2 bits, all 0
    U8  InterrogationType;
                                    //lq total 88-160 bits，1 slot
}AISMsgTypeStruct_15;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 16: Assigned mode command
*/
typedef struct AISMsgType_16{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 SourceID;                   //lq 30 bits
    //  Spare;                      //lq 2 bits, all 0
    U32 DestinationA;               //lq 30 bits
    U16 OffsetA;                    //lq 12 bits
    U16 IncrementA;                 //lq 10 bits
    U32 DestinationB;               //lq 30 bits
    U16 OffsetB;                    //lq 12 bits
    U16 IncrementB;                 //lq 10 bits
    //  Spare;                      //lq maximum 4 bits, all 0 (observe byte boundaries)
    U8  CommandType;                                    
                                    //lq total 96-144 bits，1 slot
}AISMsgTypeStruct_16;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 17: GNSS broadcast binary message
*/
typedef struct AISMsgType_17{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 SourceID;                   //lq 30 bits
    //  Spare;                      //lq 2 bits, all 0
    S32 Longitude;                  //lq 18 bits
    S32 Latitude;                   //lq 17 bits
    //  Spare;                      //lq 5 bits, all 0
    U8  MessageType;                //lq 6 bits
    U16 StationID;                  //lq 10 bits
    U16 ZCount;                     //lq 13 bits
    U8  SequenceNumber;             //lq 3 bits
    U8  N;                          //lq 5 bits
    U8  Health;                     //lq 3 bits
    U32 DGNSSDataWord[29];          //lq 29 * 24bits
                                    //lq total 80-816 bits，1-5 slot
}AISMsgTypeStruct_17;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 18: Standard Class B equipment position report
*/
typedef struct AISMsgType_18{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 UserID;                     //lq 30 bits
    //  Spare;                      //lq 8 bits, all 0
    U16 SOG;                        //lq 10 bits
    U8  PositionAccuracy;           //lq 1 bit
    S32 Longitude;                  //lq 28 bits
    S32 Latitude;                   //lq 27 bits
    U16 COG;                        //lq 12 bits
    U16 TrueHeading;                //lq 9 bits
    U8  TimeStamp;                  //lq 6 bits
    //  Spare;                      //lq 2 bits, all 0
    U8  ClassBUnitFlag;             //lq 1 bit
    U8  ClassBDisplayFlag;          //lq 1 bit
    U8  ClassBDSCFlag;              //lq 1 bit
    U8  ClassBBandFlag;             //lq 1 bit
    U8  ClassBMsg22Flag;            //lq 1 bit
    U8  ModeFlag;                   //lq 1 bit
    U8  RAIMFlag;                   //lq 1 bit
    U8  CommStateSelectFlag;        //lq 1 bit
    AISMsgCommStateUnion CommunicationState;         //lq 19 bits
                                    //lq total 168 bits，1 slot
}AISMsgTypeStruct_18;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 19: Extended Class B equipment position report
*/
typedef struct AISMsgType_19{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 UserID;                     //lq 30 bits
    //  Spare;                      //lq 8 bits, all 0
    U16 SOG;                        //lq 10 bits
    U8  PositionAccuracy;           //lq 1 bit
    S32 Longitude;                  //lq 28 bits
    S32 Latitude;                   //lq 27 bits
    U16 COG;                        //lq 12 bits
    U16 TrueHeading;                //lq 9 bits
    U8  TimeStamp;                  //lq 6 bits
    //  Spare;                      //lq 4 bits, all 0
    U8  Name[20];                   //lq 120 bits (6-bit ASCII)
    U8  TypeOfShipAndCargo;         //lq 8 bits
    U16 DimensionA;                 //lq 9 bits
    U16 DimensionB;                 //lq 9 bits
    U8  DimensionC;                 //lq 6 bits
    U8  DimensionD;                 //lq 6 bits
    U8  TypeOfEPFD;                 //lq 4 bits
    U8  RAIMFlag;                   //lq 1 bit
    U8  DTE;                        //lq 1 bits
    U8  AssignedModeFlag;           //lq 1 bit
    //  Spare;                      //lq 4 bits, all 0
                                    //lq total 312 bits，2 slots
}AISMsgTypeStruct_19;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 20: Data link management message
*/
typedef struct AISMsgType_20{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 SourceStationID;            //lq 30 bits
    //  Spare;                      //lq 2 bits, all 0
    U16 OffsetNumber1;              //lq 12 bits
    U8  NumberOfSlot1;              //lq 4 bits
    U8  Timeout1;                   //lq 3 bits
    U16 Increment1;                 //lq 11 bits
    U16 OffsetNumber2;              //lq 12 bits
    U8  NumberOfSlot2;              //lq 4 bits
    U8  Timeout2;                   //lq 3 bits
    U16 Increment2;                 //lq 11 bits
    U16 OffsetNumber3;              //lq 12 bits
    U8  NumberOfSlot3;              //lq 4 bits
    U8  Timeout3;                   //lq 3 bits
    U16 Increment3;                 //lq 11 bits
    U16 OffsetNumber4;              //lq 12 bits
    U8  NumberOfSlot4;              //lq 4 bits
    U8  Timeout4;                   //lq 3 bits
    U16 Increment4;                 //lq 11 bits
    //  Spare;                      //lq maximum 6 bits, all 0 (observe byte boundaries)
    U8  ManagementType;
                                    //lq total 72-160 bits，1 slot
}AISMsgTypeStruct_20;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 21: Aids-to-navigation report (AtoN)
*/
typedef struct AISMsgType_21{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 ID;                         //lq 30 bits
    U8  TypeOfAtoN;                 //lq 5 bits
    U8  NameOfAtoN[20];             //lq 120 bits (6-bit ASCII)
    U8  PositionAccuracy;           //lq 1 bit
    S32 Longitude;                  //lq 28 bits
    S32 Latitude;                   //lq 27 bits
    U16 DimensionA;                 //lq 9 bits
    U16 DimensionB;                 //lq 9 bits
    U8  DimensionC;                 //lq 6 bits
    U8  DimensionD;                 //lq 6 bits
    U8  TypeOfEPFD;                 //lq 4 bits
    U8  TimeStamp;                  //lq 6 bits
    U8  OffPositionIndicator;       //lq 1 bit
    U8  AtoNStatus;                 //lq 8 bits
    U8  RAIMFlag;                   //lq 1 bit
    U8  VirtualAtoNFlag;            //lq 1 bit
    U8  AssignedModeFlag;           //lq 1 bit
    //  Spare;                      //lq 1 bit, all 0
    U8  NameOfAtoNExtension[14];    //lq maximum 84 bits (6-bit ASCII)
    //  Spare;                      //lq 0\2\4\6 bits, all 0 (observe byte boundaries)
    U8  NameOfAtoNExtensionLen;
                                    //lq total 272-360 bits，2 slots
}AISMsgTypeStruct_21;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 22: Channel management
*/
typedef struct AISMsgType_22{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 StationID;                  //lq 30 bits
    //  Spare;                      //lq 2 bits, all 0
    U16 ChannelA;                   //lq 12 bits
    U16 ChannelB;                   //lq 12 bits
    U8  TxRxMode;                   //lq 4 bits
    U8  Power;                      //lq 1 bit
    S32 LonOrMSBOfAddID1;           //lq 18 bits
    S32 LatOrLSBOfAddID1;           //lq 17 bits
    S32 LonOrMSBOfAddID2;           //lq 18 bits
    S32 LatOrLSBOfAddID2;           //lq 17 bits
    U8  AddOrBroadMsgIndicator;     //lq 1 bit
    U8  ChannelABandwidth;          //lq 1 bit
    U8  ChannelBBandwidth;          //lq 1 bit
    U8  TransitionalZoneSize;       //lq 3 bits
    //  Spare;                      //lq 23 bits, all 0
                                    //lq total 168 bits，1 slot
}AISMsgTypeStruct_22;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 23: Group Assignment Command
*/
typedef struct AISMsgType_23{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 SourceID;                   //lq 30 bits
    //  Spare;                      //lq 2 bits, all 0
    S32 Longitude1;                 //lq 18 bits
    S32 Latitude1;                  //lq 17 bits
    S32 Longitude2;                 //lq 18 bits
    S32 Latitude2;                  //lq 17 bits
    U8  StationType;                //lq 4 bits
    U8  TypeOfShipAndCargo;         //lq 8 bits
    //  Spare;                      //lq 22 bits, all 0
    U8  TxRxMode;                   //lq 4 bits
    U8  ReportingInterval;          //lq 4 bits
    U8  QuietTime;                  //lq 4 bits
    //  Spare;                      //lq 6 bits, all 0
                                    //lq total 160 bits，1 slot
}AISMsgTypeStruct_23;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 24A: Static data report Part A
*/
typedef struct AISMsgType_24A{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 UserID;                     //lq 30 bits
    U8  PartNumber;                 //lq 2 bits
    U8  Name[20];                   //lq 120 bits (6-bit ASCII)
                                    //lq total 160 bits，1 slot
}AISMsgTypeStruct_24A;

/*lq VendorID结构体定义*/
typedef struct VendorID{
    U8  ManufacturerID[3];          //lq 18bits,3 6-bit ASCII characters
    U8  UnitModelCode;              //lq 4bits,0-15
    U32 UnitSerialNumber;           //lq 20bits
}VendorIDStruct;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 24B: Static data report Part B
*/
typedef struct AISMsgType_24B{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 UserID;                     //lq 30 bits
    U8  PartNumber;                 //lq 2 bits
    U8  TypeOfShipAndCargo;         //lq 8 bits
    VendorIDStruct  VendorID;       //lq 42 bits (6-bit ASCII)
    U8  CallSign[7];                //lq 42 bits (6-bit ASCII)
    U16 DimensionA;                 //lq 9 bits
    U16 DimensionB;                 //lq 9 bits
    U8  DimensionC;                 //lq 6 bits
    U8  DimensionD;                 //lq 6 bits
    //  Spare;                      //lq 6 bits, all 0
                                    //lq total 168 bits，1 slot
}AISMsgTypeStruct_24B;

/*lq 25号消息二进制数据编码结构化定义 */
typedef struct AISMsg25AIBinData{
    U16 AI_DAC;                     //lq 10 bits
    U8  AI_FI;                      //lq 6 bits
    U8  ApplicationData[14];        //lq Maximum 112 bits (8-bit byte)
    U8  ApplicationDataBitLen;
}AISMsg25AIBinDataStruct;

/*lq 25号消息二进制数据定义 */
typedef union AISMsg25BinData{
    U8  UnstructedBinData[16];      //lq Maximum 128 bits (8-bit byte)
    AISMsg25AIBinDataStruct StructedBinData;    //lq由AI定义的结构化编码数据 
}AISMsg25BinDataUnion;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 25: Single slot binary message
*/
typedef struct AISMsgType_25{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 SourceID;                   //lq 30 bits
    U8  DestinationIndicator;       //lq 1 bit
    U8  BinaryDataFlag;             //lq 1 bit
    U32 DestinationID;              //lq 30 bits
    AISMsg25BinDataUnion  BinaryData;             //lq Maximum 128 bits (8-bit byte)
    U8  BinaryDataBitLen;           //lq 对于结构化二进制数据，包含DAC和FI
                                    //lq Maximum 168 bits，1 slot
}AISMsgTypeStruct_25;

/*lq 26号消息二进制数据编码结构化定义 */
typedef struct AISMsg26AIBinData{
    U16 AI_DAC;                     //lq 10 bits
    U8  AI_FI;                      //lq 6 bits
    U8  ApplicationData[124];       //lq Maximum 988 bits (8-bit byte)
    U16  ApplicationDataBitLen;     // lnw  modify  Old  U8    , it should be U16 
}AISMsg26AIBinDataStruct;

/*lq 26号消息二进制数据定义 */
typedef union AISMsg26BinData{
    U8  UnstructedBinData[126];      //lq Maximum 1004 bits (8-bit byte)
    AISMsg26AIBinDataStruct StructedBinData;    //lq由AI定义的结构化编码数据 
}AISMsg26BinDataUnion;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 26: Multiple slot binary message with communications state
*/
typedef struct AISMsgType_26{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 SourceID;                   //lq 30 bits
    U8  DestinationIndicator;       //lq 1 bit
    U8  BinaryDataFlag;             //lq 1 bit
    U32 DestinationID;              //lq 30 bits
    AISMsg26BinDataUnion  BinaryData;             //lq Maximum 1004 bits (8-bit byte)
    U8  CommStateSelectFlag;        //lq 1 bit
    AISMsgCommStateUnion CommunicationState;         //lq 19 bits
    U16  BinaryDataBitLen;           //lq 对于结构化二进制数据，包含DAC和FI    
                                     // lnw  change  old :U8   ,Range 0-255  ,  actual value max 1064 ,so  it must be change to U16
                                    //lq Maximum 1064 bits，1-5 slots(B-Class mobile AIS station not exceed 2 slots)
}AISMsgTypeStruct_26;

/*lq 
 *AIS消息类型定义，各个字段的数值格式与标准1371对该消息相应参数的要求一致 
 *Message 27: Long-range AIS broadcast message
*/
typedef struct AISMsgType_27{
    U8  MessageID;                  //lq 6 bits
    U8  RepeatIndicator;            //lq 2 bits
    U32 UserID;                     //lq 30 bits
    U8  PositionAccuracy;           //lq 1 bit
    U8  RAIMFlag;                   //lq 1 bit
    U8  NavigationalStatus;         //lq 4 bits
    S32 Longitude;                  //lq 18 bits
    S32 Latitude;                   //lq 17 bits
    U8  SOG;                        //lq 6 bits
    U16 COG;                        //lq 9 bits
    U8  StatusOfCurrentGNSSPos;     //lq 1 bit
    //  Spare;                      //lq 1 bit, all 0
                                    //lq total 96 bits，1 slot
}AISMsgTypeStruct_27;

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
/*lq 二进制消息缓存（用于存放构造出的二进制消息） */
extern AISMsgBinBuffArray_1_2_3  AISMsgBinBuff_1;
extern AISMsgBinBuffArray_1_2_3  AISMsgBinBuff_2;
extern AISMsgBinBuffArray_1_2_3  AISMsgBinBuff_3;
extern AISMsgBinBuffArray_4_11 AISMsgBinBuff_4;
extern AISMsgBinBuffArray_5    AISMsgBinBuff_5;
extern AISMsgBinBuffArray_6    AISMsgBinBuff_6;
extern AISMsgBinBuffArray_7_13 AISMsgBinBuff_7;
extern AISMsgBinBuffArray_8    AISMsgBinBuff_8;
extern AISMsgBinBuffArray_9    AISMsgBinBuff_9;
extern AISMsgBinBuffArray_10   AISMsgBinBuff_10;
extern AISMsgBinBuffArray_4_11 AISMsgBinBuff_11;
extern AISMsgBinBuffArray_12   AISMsgBinBuff_12;
extern AISMsgBinBuffArray_7_13 AISMsgBinBuff_13;
extern AISMsgBinBuffArray_14   AISMsgBinBuff_14;
extern AISMsgBinBuffArray_15   AISMsgBinBuff_15;
extern AISMsgBinBuffArray_16   AISMsgBinBuff_16;
extern AISMsgBinBuffArray_17   AISMsgBinBuff_17;
extern AISMsgBinBuffArray_18   AISMsgBinBuff_18;
extern AISMsgBinBuffArray_19   AISMsgBinBuff_19;
extern AISMsgBinBuffArray_20   AISMsgBinBuff_20;
extern AISMsgBinBuffArray_21   AISMsgBinBuff_21;
extern AISMsgBinBuffArray_22   AISMsgBinBuff_22;
extern AISMsgBinBuffArray_23   AISMsgBinBuff_23;
extern AISMsgBinBuffArray_24A  AISMsgBinBuff_24A;
extern AISMsgBinBuffArray_24B  AISMsgBinBuff_24B;
extern AISMsgBinBuffArray_25   AISMsgBinBuff_25;
extern AISMsgBinBuffArray_26   AISMsgBinBuff_26;
extern AISMsgBinBuffArray_27   AISMsgBinBuff_27;

/*lq AIS消息（用于存放解析出的AIS消息）*/
extern AISMsgTypeStruct_1_2_3  AISMsg_1;
extern AISMsgTypeStruct_1_2_3  AISMsg_2;
extern AISMsgTypeStruct_1_2_3  AISMsg_3;
extern AISMsgTypeStruct_4_11   AISMsg_4;
extern AISMsgTypeStruct_5      AISMsg_5;
extern AISMsgTypeStruct_6      AISMsg_6;
extern AISMsgTypeStruct_7_13   AISMsg_7;
extern AISMsgTypeStruct_8      AISMsg_8;
extern AISMsgTypeStruct_9      AISMsg_9;
extern AISMsgTypeStruct_10     AISMsg_10;
extern AISMsgTypeStruct_4_11   AISMsg_11;
extern AISMsgTypeStruct_12     AISMsg_12;
extern AISMsgTypeStruct_7_13   AISMsg_13;
extern AISMsgTypeStruct_14     AISMsg_14;
extern AISMsgTypeStruct_15     AISMsg_15;
extern AISMsgTypeStruct_16     AISMsg_16;
extern AISMsgTypeStruct_17     AISMsg_17;
extern AISMsgTypeStruct_18     AISMsg_18;
extern AISMsgTypeStruct_19     AISMsg_19;
extern AISMsgTypeStruct_20     AISMsg_20;
extern AISMsgTypeStruct_21     AISMsg_21;
extern AISMsgTypeStruct_22     AISMsg_22;
extern AISMsgTypeStruct_23     AISMsg_23;
extern AISMsgTypeStruct_24A    AISMsg_24A;
extern AISMsgTypeStruct_24B    AISMsg_24B;
extern AISMsgTypeStruct_25     AISMsg_25;
extern AISMsgTypeStruct_26     AISMsg_26;
extern AISMsgTypeStruct_27     AISMsg_27;

/* Public  function  ---------------------------------------------------------*/
extern AISMsgTypeEnum AISMSG_ParseMsg(AISMsgBinBuffArray AISMsgBinBuff);
extern AISMsgTypeEnum AISMSG_ParseMsgType(U8 *pData);

// lnw  add  below  
extern void AISMSG_BuildMsg1(const AISMsgTypeStruct_1_2_3* pAISMsg);// lnw add
extern void AISMSG_BuildMsg2(const AISMsgTypeStruct_1_2_3* pAISMsg);
extern void AISMSG_BuildMsg3(const AISMsgTypeStruct_1_2_3* pAISMsg);//lnw add
extern void AISMSG_BuildMsg5(const AISMsgTypeStruct_5* pAISMsg);//lnw add
extern void AISMSG_BuildMsg6(AISMsgTypeStruct_6* pAISMsg);
extern void AISMSG_BuildMsg7(const AISMsgTypeStruct_7_13* pAISMsg);
extern void AISMSG_BuildMsg8(AISMsgTypeStruct_8* pAISMsg);
extern void AISMSG_BuildMsg9(const AISMsgTypeStruct_9* pAISMsg);
extern void AISMSG_BuildMsg10(const AISMsgTypeStruct_10* pAISMsg);
extern void AISMSG_BuildMsg11(const AISMsgTypeStruct_4_11* pAISMsg);
extern void AISMSG_BuildMsg12(AISMsgTypeStruct_12* pAISMsg);
extern void AISMSG_BuildMsg13(const AISMsgTypeStruct_7_13* pAISMsg);
extern void AISMSG_BuildMsg14(AISMsgTypeStruct_14* pAISMsg);
extern void AISMSG_BuildMsg15(const AISMsgTypeStruct_15* pAISMsg);
extern void AISMSG_BuildMsg16(const AISMsgTypeStruct_16* pAISMsg);
extern void AISMSG_BuildMsg17(const AISMsgTypeStruct_17* pAISMsg);
extern void AISMSG_BuildMsg18(const AISMsgTypeStruct_18* pAISMsg);
extern void AISMSG_BuildMsg19(const AISMsgTypeStruct_19* pAISMsg);
extern void AISMSG_BuildMsg20(const AISMsgTypeStruct_20* pAISMsg);
extern void AISMSG_BuildMsg21(const AISMsgTypeStruct_21* pAISMsg);
extern void AISMSG_BuildMsg22(const AISMsgTypeStruct_22* pAISMsg);
extern void AISMSG_BuildMsg23(const AISMsgTypeStruct_23* pAISMsg);
extern void AISMSG_BuildMsg24A(const AISMsgTypeStruct_24A* pAISMsg);
extern void AISMSG_BuildMsg24B(const AISMsgTypeStruct_24B* pAISMsg);
extern void AISMSG_BuildMsg25(AISMsgTypeStruct_25* pAISMsg);
extern void AISMSG_BuildMsg26(AISMsgTypeStruct_26* pAISMsg);
extern void AISMSG_BuildMsg27(const AISMsgTypeStruct_27* pAISMsg);
extern AISMsgTypeEnum AISMSG_ConvertToMsgType(U8 msgId, U8 partNumber);
extern void AisMain(void);






#endif

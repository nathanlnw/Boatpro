/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: DataLinkService.h
  Author: lq       Version : V1.00        Date: 2014.07.09
  Description:     AIS数据链路服务层
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _DATA_LINK_SERVICE_H
#define _DATA_LINK_SERVICE_H

/* Include ------------------------------------------------------------*/
#include "Include.h"

/* Public define ------------------------------------------------------------*/
#define DLS_BITSTREAM_BUFF_LEN                      1281    //lq unit:1byte

#define DLS_PACKET_PROBE_STATE_HEAD_PROBING         0
#define DLS_PACKET_PROBE_STATE_TAIL_PROBING         1

#define DLS_FRAME_HEADER_LEN                        8       //lq unit:1bit
#define DLS_FRAME_HEADER                            0x7E
#define DLS_FRAME_TAILER_LEN                        DLS_FRAME_HEADER_LEN
#define DLS_FRAME_TAILER                            DLS_FRAME_HEADER
#define DLS_FRAME_MAX_LEN_BYTE                      160     //lq unit:1byte

#define DLS_FRAME_FIFO_DEPTH                        7       //lq unit:1byte

#define DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN            8       //lq unit: 1bit，比特流缓存元素的比特长度

/* Public typedef -----------------------------------------------------------*/
typedef enum PacketProbeTypeEnum
{
    PacketProbeType1,
    PacketProbeType2
}PacketProbeTypeEnum;

typedef struct BitStreamStruct
{
    U8  DataBuff[DLS_BITSTREAM_BUFF_LEN];               //lq 每个字节存储1bit，5个时隙总共256*5=1280bit
    U16 Input;
    U16 Output;
    U16 HeadIndex;
    U16 TailIndex;
    U8  InputOffset;
    U8  OutputOffset;
    U8  HeadOffset;
    U8  TailOffset;
}BitStreamStruct;

typedef struct PacketProbeStruct
{
    BitStreamStruct BitStream;
    U8  State;
}PacketProbeStruct;

typedef struct FrameRxCellStruct
{
	U8  Data[DLS_FRAME_MAX_LEN_BYTE + 1];
	U16 DataLen;                                        //lq 单位：1byte
	//lq U16 DCOffset;
	S32 ArrivalFrame;
    S16 ArrivalSlot;
    AisChannelEnum  RxChannel;	
}FrameRxCellStruct;

typedef struct FrameTxCellStruct
{                                                       
	U8  Data[DLS_FRAME_MAX_LEN_BYTE + 2];               //lq Data[0]-整字节数，Data[1]-非整字节比特数
	//lq U16 DataLenBit;                                //lq 有效数据总比特数
    S16 TxSlot;
    AisChannelEnum TxChannel;	
}FrameTxCellStruct;

typedef struct FrameRxFifoStruct
{
    FrameRxCellStruct FIFO[DLS_FRAME_FIFO_DEPTH];
    U16 Input;
    U16 Output;
}FrameRxFifoStruct;

typedef struct FrameTxFifoStruct
{
    FrameTxCellStruct FIFO[DLS_FRAME_FIFO_DEPTH];
    U16 Input;
    U16 Output;
}FrameTxFifoStruct;

#if 0
typedef struct FrameFifoStruct
{
    U8  FIFO[DLS_FRAME_FIFO_DEPTH][DLS_FRAME_MAX_LEN_BYTE + 1];  //lq DataBuff[0][]存放实际数据长度，5个时隙总共256*5=1280bit=160byte, 
    U16 Input;
    U16 Output;
}FrameFifoStruct;
#endif

/* Public variables ---------------------------------------------------------*/

/* Public function  ---------------------------------------------------------*/

#endif

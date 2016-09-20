/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: AISDigitalInterface.h
  Author: lq       Version : V1.00        Date: 2014.04.25
  Description:     AIS数字通信接口
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _AISDIGITALINTERFACE_H
#define _AISDIGITALINTERFACE_H

/* Include ------------------------------------------------------------*/
#include "def.h"
#include "AISMsgModule.h"

/* Private define ------------------------------------------------------------*/
#define AIS_DI_SENTENCE_MAXLEN          82		//lq 单位：1个字符
#define AIS_DI_SEN_FIFO_DEPTH			16
#define AIS_DI_SEN_TOTAL_MIN            1
#define AIS_DI_SEN_TOTAL_MAX            9
#define AIS_DI_SEN_NUM_MIN              1
#define AIS_DI_SEN_NUM_MAX              9
#define AIS_DI_MSG_SEQ_MIN              0//0
#define AIS_DI_MSG_SEQ_MAX              9
#define AIS_DI_BBM_CHN_MIN              0//0
#define AIS_DI_BBM_CHN_MAX              3
#define AIS_DI_BBM_FILL_BIT_MIN         1//0
#define AIS_DI_BBM_FILL_BIT_MAX         5
#define AIS_DI_ABM_CHN_MIN              0//0
#define AIS_DI_ABM_CHN_MAX              3
#define AIS_DI_ABM_FILL_BIT_MIN         0//0
#define AIS_DI_ABM_FILL_BIT_MAX         5
#define AIS_DI_ABK_ACK_TYPE_MSG6_12_ACK_SUCCESS         0//0
#define AIS_DI_ABK_ACK_TYPE_MSG6_12_BROADCAST_SUCCESS   1
#define AIS_DI_ABK_ACK_TYPE_MSG_BROADCAST_FAILURE       2
#define AIS_DI_ABK_ACK_TYPE_MSG8_14_5_BROADCAST_SUCCESS 3
#define AIS_DI_ABK_ACK_TYPE_MSG7_13_LATER_RECEPTION     4

#define AIS_DI_TALKER_ID_LEN            2
#define AIS_DI_TALKER_ID_DEFAULT        "AI"

/* Private typedef -----------------------------------------------------------*/
typedef U8  AISDISenBuffArray[AIS_DI_SENTENCE_MAXLEN + 1];

typedef struct AisDiSenFifo
{
	AISDISenBuffArray Fifo[AIS_DI_SEN_FIFO_DEPTH];
	U16 FifoInput;
	U16 FifoOutput;
}AisDiSenFifoStruct;

#if 0
/*lq 多语句消息结构体 */
typedef struct AisDIMultiSenStruct
{
    U16 SentenceTotal;
    U16 SentenceNumber;
    U16 MessageNumber;
    void *pSenStruct;      
}AisDiMultiSenStruct;
#endif

/*lq ABM语句结构体 */
typedef struct ABMStruct
{
    U16 SentenceTotal;
    U16 SentenceNumber;
    U16 MessageNumber;
    U32 MmsiDest;
    AisChannelEnum Channel;
    U8  MsgID;
    U8  EncapData[AIS_MSG12_TEXT_MAXLEN + 1];   //lq 有效数据末位存放'\0'
    U16 EncapDataLen;
    U8  FillBitNumber; 
    U8  ValidFlg;   
}ABMStruct;

/*lq BBM语句结构体 */
typedef struct BBMStruct
{
    U16 SentenceTotal;
    U16 SentenceNumber;
    U16 MessageNumber;
    AisChannelEnum Channel;
    U8  MsgID;
    U8  EncapData[AIS_MSG14_TEXT_MAXLEN + 1];  //lq 有效数据末位存放'\0'
    U16 EncapDataLen;
    U8  FillBitNumber;    
    U8  ValidFlg;
}BBMStruct;
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
extern ABMStruct ABMPara;
extern BBMStruct BBMPara;
       
/* Public  function  ---------------------------------------------------------*/
extern void AISDI_BuildVDM(U8 listener[], U8 data[], U16 len, U8 channel);// lnw  add
extern void AISDI_BuildVDO(U8 listener[], U8 data[], U16 len, U8 channel);// lnw add
extern U8 AsciiToSixBinCode(U8 code);// lnw add

#endif

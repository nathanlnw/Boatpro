/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: DataLinkService.c
  Author: lq       Version : V1.00        Date: 2014.07.09
  Description:     AIS数据链路服务层
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "Include.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static PacketProbeStruct PacketProbeInst1;
static PacketProbeStruct PacketProbeInst2;

static FrameRxFifoStruct FrameFifoRx1;
static FrameRxFifoStruct FrameFifoRx2;
static FrameTxFifoStruct FrameFifoTx;
 
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* 名称  : DLS_BitStreamStructInit
* 描述  : 初始化结构体
* 输入  : pBitStreamStruct - BitStreamStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void DLS_BitStreamStructInit(BitStreamStruct *pBitStreamStruct)
{
    pBitStreamStruct->Input = 0;
    pBitStreamStruct->Output = 0;
    pBitStreamStruct->HeadIndex = 0;
    pBitStreamStruct->TailIndex = 0;
    pBitStreamStruct->InputOffset = 0;
    pBitStreamStruct->OutputOffset = 0;
    pBitStreamStruct->HeadOffset = 0;
    pBitStreamStruct->TailOffset = 0;           
}

/*******************************************************************************
* 名称  : DLS_PacketProbeStructInit
* 描述  : 初始化结构体
* 输入  : pPacketProbeStruct - PacketProbeStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void DLS_PacketProbeStructInit(PacketProbeStruct *pPacketProbeStruct)
{
    DLS_BitStreamStructInit(&pPacketProbeStruct->BitStream);
    pPacketProbeStruct->State = DLS_PACKET_PROBE_STATE_HEAD_PROBING;   
}

/*******************************************************************************
* 名称  : DLS_FrameRxFifoStructInit
* 描述  : 初始化结构体
* 输入  : pFrameRxFifoStruct - FrameRxFifoStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void DLS_FrameRxFifoStructInit(FrameRxFifoStruct *pFrameRxFifoStruct)
{
    pFrameRxFifoStruct->Input = 0;
    pFrameRxFifoStruct->Output = 0; 
}

/*******************************************************************************
* 名称  : DLS_FrameTxFifoStructInit
* 描述  : 初始化结构体
* 输入  : pFrameTxFifoStruct - FrameTxFifoStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void DLS_FrameTxFifoStructInit(FrameTxFifoStruct *pFrameTxFifoStruct)
{
    pFrameTxFifoStruct->Input = 0;
    pFrameTxFifoStruct->Output = 0; 
}

/*******************************************************************************
* 名称  : DLS_InstFrameFifoStructInit
* 描述  : 初始化结构体
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void DLS_InstFrameFifoStructInit(void)
{
    DLS_FrameRxFifoStructInit(&FrameFifoRx1);
    DLS_FrameRxFifoStructInit(&FrameFifoRx2);
    DLS_FrameTxFifoStructInit(&FrameFifoTx);
}

/*******************************************************************************
* 名称  : DLS_InstPacketProbeStructInit
* 描述  : 初始化结构体实例
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void DLS_InstPacketProbeStructInit(void)
{
    DLS_PacketProbeStructInit(&PacketProbeInst1);
    DLS_PacketProbeStructInit(&PacketProbeInst2);
}

/*******************************************************************************
* 名称  : DLS_BitStreamIsBuffFull
* 描述  : 比特流缓存是否已满
* 输入  : pBitStreamStruct - BitStreamStruct结构体指针		  
* 输出  : 无
* 返回  : TRUE - 已满，FALSE - 未满
*******************************************************************************/
static U8 DLS_BitStreamIsBuffFull(BitStreamStruct *pBitStreamStruct)
{
    if ((pBitStreamStruct->Input + 1) % DLS_BITSTREAM_BUFF_LEN
        == pBitStreamStruct->Output)
    {
        return TRUE; 

        debug_printf("$[%6d:%4d] %s : Bitstream buffer [%s]is full!\r\n",
            GetFrameNum(), GetSlotNum(), __func__,
            pBitStreamStruct == &PacketProbeInst1.BitStream ? "A" : "B");
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* 名称  : DLS_BitStreamIsBuffEmpty
* 描述  : 比特流缓存是否为空
* 输入  : pBitStreamStruct - BitStreamStruct结构体指针		  
* 输出  : 无
* 返回  : TRUE - 为空，FALSE - 非空
*******************************************************************************/
static U8 DLS_BitStreamIsBuffEmpty(BitStreamStruct *pBitStreamStruct)
{
    if ((pBitStreamStruct->Input == pBitStreamStruct->Output)
        && (pBitStreamStruct->InputOffset == pBitStreamStruct->OutputOffset))
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* 名称  : DLS_BitStreamPutBit
* 描述  : 向缓存存入数据
* 输入  : pBitStreamStruct - BitStreamStruct结构体指针
          data - 数据	  
* 输出  : 无
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
static U8 DLS_BitStreamPutBit(BitStreamStruct *pBitStreamStruct, const U8 data)
{
    U8 tempU8;
    U8 tempU8_2;

    if (DLS_BitStreamIsBuffFull(pBitStreamStruct) == FALSE)
    {
        /*lq 计算位掩码 */
        tempU8_2 = ~(1 << pBitStreamStruct->InputOffset);
        tempU8 = (data & 0x01) << pBitStreamStruct->InputOffset;

        /*lq 指定位赋值 */
        pBitStreamStruct->DataBuff[pBitStreamStruct->Input] = 
            (pBitStreamStruct->DataBuff[pBitStreamStruct->Input] & tempU8_2)
            | tempU8;

        pBitStreamStruct->InputOffset++;
        if (pBitStreamStruct->InputOffset >= DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN)
        {
            pBitStreamStruct->InputOffset = 0;
            pBitStreamStruct->Input = (pBitStreamStruct->Input + 1)
                                       % DLS_BITSTREAM_BUFF_LEN;            
        }
    
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : DLS_InstBitStreamPutBit
* 描述  : 向缓存存入数据
* 输入  : type - 实例类型
          data - 数据	  
* 输出  : 无
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
U8 DLS_InstBitStreamPutBit(const PacketProbeTypeEnum type, const U8 data)
{
    if (type == PacketProbeType1)
    {
        return DLS_BitStreamPutBit(&PacketProbeInst1.BitStream, data);    
    }
    else if (type == PacketProbeType2)
    {
        return DLS_BitStreamPutBit(&PacketProbeInst2.BitStream, data);        
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : DLS_BitStreamGetBit
* 描述  : 从缓存读取数据
* 输入  : pBitStreamStruct - BitStreamStruct结构体指针
* 输出  : pData - 读取的数据
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
static U8 DLS_BitStreamGetBit(BitStreamStruct *pBitStreamStruct, U8 *pData)
{
    U8 tempU8;

    if (DLS_BitStreamIsBuffEmpty(pBitStreamStruct) == FALSE)
    {
        tempU8 = pBitStreamStruct->DataBuff[pBitStreamStruct->Output];
        tempU8 &= (1 << pBitStreamStruct->OutputOffset);

        pBitStreamStruct->OutputOffset++;
        if (pBitStreamStruct->OutputOffset >= DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN)
        {
            pBitStreamStruct->OutputOffset = 0;
            pBitStreamStruct->Output = (pBitStreamStruct->Output + 1)
                                        % DLS_BITSTREAM_BUFF_LEN;            
        }
    
        if (tempU8 == 0)
        {
            *pData = 0;
        }
        else
        {
            *pData = 1;
        }

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : DLS_InstBitStreamGetBit
* 描述  : 从缓存读取数据
* 输入  : type - 实例类型
* 输出  : pData - 读取的数据
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
U8 DLS_InstBitStreamGetBit(const PacketProbeTypeEnum type, U8 *pData)
{
    if (type == PacketProbeType1)
    {
        return DLS_BitStreamGetBit(&PacketProbeInst1.BitStream, pData);    
    }
    else if (type == PacketProbeType2)
    {
        return DLS_BitStreamGetBit(&PacketProbeInst2.BitStream, pData);        
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : DLS_BitStreamFlushBuff
* 描述  :达到最大长度，保留最后7比特数据
* 输入  : pBitStreamStruct - BitStreamStruct结构体指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void DLS_BitStreamFlushBuff(BitStreamStruct *pBitStreamStruct)
{
	/*pBitStreamStruct->Output = pBitStreamStruct->Input;
    pBitStreamStruct->HeadIndex = pBitStreamStruct->Input;
    pBitStreamStruct->TailIndex = pBitStreamStruct->Input;
    pBitStreamStruct->OutputOffset = pBitStreamStruct->InputOffset;
    pBitStreamStruct->HeadOffset = pBitStreamStruct->InputOffset;
    pBitStreamStruct->TailOffset = pBitStreamStruct->InputOffset;*/
	if((pBitStreamStruct->InputOffset-6)<0)
	{
		pBitStreamStruct->OutputOffset = pBitStreamStruct->InputOffset-6+DLS_FRAME_HEADER_LEN;
		if((pBitStreamStruct->Input-1)<0)
		{
			pBitStreamStruct->Output = pBitStreamStruct->Input-1+DLS_BITSTREAM_BUFF_LEN;
		}
		else
		{
			pBitStreamStruct->Output = pBitStreamStruct->Input-1;
		}
	}
	else
	{
		pBitStreamStruct->OutputOffset = pBitStreamStruct->InputOffset-6;
		pBitStreamStruct->Output = pBitStreamStruct->Input;
	}
    pBitStreamStruct->HeadIndex = pBitStreamStruct->Output;
    pBitStreamStruct->TailIndex = pBitStreamStruct->Output;
    pBitStreamStruct->HeadOffset = pBitStreamStruct->OutputOffset;
    pBitStreamStruct->TailOffset = pBitStreamStruct->OutputOffset;
}

/*******************************************************************************
* 名称  : DLS_BitStreamNextFrameSetup
* 描述  : 下一帧检测配置（因为帧头和帧尾相同，所以将当前“帧”的尾作为下一帧的头，
           防止当前“帧”不是真正的一帧时，漏掉下一帧，当前“帧”是否为真正的一帧，
           可由后续的校验码检测）
* 输入  : pBitStreamStruct - BitStreamStruct结构体指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void DLS_BitStreamNextFrameSetup(BitStreamStruct *pBitStreamStruct)
{
    pBitStreamStruct->Output = pBitStreamStruct->TailIndex;
    pBitStreamStruct->OutputOffset = pBitStreamStruct->TailOffset;
    pBitStreamStruct->HeadIndex = pBitStreamStruct->TailIndex;
    pBitStreamStruct->HeadOffset = pBitStreamStruct->TailOffset;
}

/*******************************************************************************
* 名称  : DLS_BitStreamGetBitQuantity
* 描述  : 获取两个位置坐标之间的比特数
* 输入  : pBitStreamStruct - BitStreamStruct结构体指针
* 输出  : index1 - 起始坐标数组元素索引值
          offset1 - 起始坐标位偏移量
          index2 - 终止坐标数组元素索引值
          offset2 - 终止坐标位偏移量
* 返回  : 两个位置坐标之间的比特数
*******************************************************************************/
static U16 DLS_BitStreamGetBitQuantity(U16 index1, U16 offset1, U16 index2,
                                       U16 offset2)
{   
    S16 tempS16;

    tempS16 = ((index2 + DLS_BITSTREAM_BUFF_LEN - index1) % DLS_BITSTREAM_BUFF_LEN) //lq 索引值均为无符号数，先加后减
        * DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN + offset2 - offset1;

    if (tempS16 < 0)
    {
        tempS16 = 0;
    }
    
    return tempS16;
}

/*******************************************************************************
* 名称  : DLS_BitStreamGetBitNum
* 描述  : 获取缓存比特数
* 输入  : pBitStreamStruct - BitStreamStruct结构体指针
* 输出  : 无
* 返回  : 缓存比特数
*******************************************************************************/
static U16 DLS_BitStreamGetBitNum(BitStreamStruct *pBitStreamStruct)
{
    return DLS_BitStreamGetBitQuantity(pBitStreamStruct->Output,
        pBitStreamStruct->OutputOffset, pBitStreamStruct->Input,
        pBitStreamStruct->InputOffset);
}

/*******************************************************************************
* 名称  : DLS_BitStreamGetFrameBitNum
* 描述  : 获取缓存中当前帧的比特数
* 输入  : pBitStreamStruct - BitStreamStruct结构体指针
* 输出  : 无
* 返回  : 缓存中当前帧的比特数
*******************************************************************************/
static U16 DLS_BitStreamGetFrameBitNum(BitStreamStruct *pBitStreamStruct)
{
    U16 tempU16;

    tempU16 = DLS_BitStreamGetBitQuantity(pBitStreamStruct->HeadIndex,
        pBitStreamStruct->HeadOffset, pBitStreamStruct->TailIndex,
        pBitStreamStruct->TailOffset);

    if (tempU16 >= DLS_FRAME_HEADER_LEN)
    {
        tempU16 -= DLS_FRAME_HEADER_LEN;
    }
    else
    {
        tempU16 = 0;
    }

    return tempU16;
}

/*******************************************************************************
* 名称  : DLS_BitStreamGetFrame
* 描述  : 获取缓存中的当前帧（缓存中比特的缓存顺序是帧的发射顺序，即帧的每个字节
          的LSB先收到，见“1371标准的3.3.7”）
* 输入  : pBitStreamStruct - BitStreamStruct结构体指针
* 输出  : pFrameBuff - 数据帧（已剔除填充位，不含帧头和帧尾，只包含数据和校验码，
          pFrameBuff中每个字节的LSB比MSB先收到）
* 返回  : TRUE - 帧有效，FALSE - 帧无效
*******************************************************************************/
static U8 DLS_BitStreamGetFrame(BitStreamStruct *pBitStreamStruct, U8 *pFrameBuff)
{
    U16 tempU16;
    U16 outputIndex;
    U16 outputOffset;
    U16 frameBitNum;
    U16 i;
    U16 k;
    U8  highCnt;
    U8  byteData;
    U8  validBitCnt;
#if 0 //lqdb
    U8 temp[2 * DLS_FRAME_MAX_LEN_BYTE + 30];
    U8 temp2[3];
#endif
    k = 0;
    highCnt = 0;
    byteData = 0;
    validBitCnt = 0;
    frameBitNum = DLS_BitStreamGetFrameBitNum(pBitStreamStruct);
    
    tempU16 = pBitStreamStruct->HeadOffset + DLS_FRAME_HEADER_LEN;
    outputOffset = tempU16 % DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN;
    outputIndex = (pBitStreamStruct->HeadIndex + (tempU16 /
        DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN)) % DLS_BITSTREAM_BUFF_LEN;

    for (i = 0; i < frameBitNum; i++, outputOffset++)
    {
        if (outputOffset >= DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN)
        {
            outputOffset = 0;
            outputIndex = (outputIndex + 1) % DLS_BITSTREAM_BUFF_LEN;
        }
        if ((pBitStreamStruct->DataBuff[outputIndex] & (1 << outputOffset)) != 0)
        {
            /*lq 比特1 */
            if(highCnt >= 5)
            {
                DLS_BitStreamNextFrameSetup(pBitStreamStruct);                
#if 0
                usart_printf("$PacketProbe: Frame error for continous 1!\r\n");                                                                      
#endif     
                return FALSE;                   //lq 正常情况下，帧中不会出现连续6个"1"
            }
            else
            {
                byteData |= (1 << validBitCnt); //lq 此处完成帧中bit的实际发射顺序和逻辑顺序的变换（先收到的bit作为LSB）
                validBitCnt++;
                highCnt++;                      //lq 连续"1"计数
            }       
        }
        else
        {
            /*lq 比特0 */
            if(highCnt >= 5)
            {
                /*lq 剔除填充位，当比特0之前出现连续5个"1"，删除该比特"0" */ 
            }
            else
            {
                validBitCnt++;               
            }

            highCnt = 0;
        }
        
        if (validBitCnt >= 8)
        {
            validBitCnt = 0;
            
            /*lq 最大帧长判断 */
            if (k >= DLS_FRAME_MAX_LEN_BYTE)
            {
                DLS_BitStreamNextFrameSetup(pBitStreamStruct);
#if 0
                usart_printf("$PacketProbe: Frame error for too long!\r\n");                                                                      
#endif
                return FALSE;                   //lq 超过最大帧长            
            }
            else
            {
                pFrameBuff[++k] = byteData;     //lq pFrameBuff[0] 存放有效字节总数
                byteData = 0;                                     
            }
        }    
    }
#if 0 //lqdb
    sprintf(temp, "$PacketProbe FrameData: ");            
    for (i = 0; i < k; i++)
    {
        sprintf(temp2, "%02X", pFrameBuff[1 + i]);
        strcat(temp, temp2);
    }
    strcat(temp, "\r\n");
    usart_printf(temp);
#endif  
    /*lq 整数字节判断 */
    if (validBitCnt == 0)
    {
        if (k < 3)
        {
            DLS_BitStreamNextFrameSetup(pBitStreamStruct);
#if 0
            usart_printf("$PacketProbe: Frame error for too short!\r\n");                                                                  
#endif
            return FALSE;                       //lq 帧长无效（至少为3，1byte of data and 2bytes of CRC）        
        }
        else
        {
            pFrameBuff[0] = k;                  //lq 记录帧长
            DLS_BitStreamNextFrameSetup(pBitStreamStruct);
            return TRUE;        
        }
    }
    else
    {
        DLS_BitStreamNextFrameSetup(pBitStreamStruct);
#if 0
        usart_printf("$PacketProbe: Frame error for boundary!\r\n");                                                      
#endif     
        return FALSE;                           //lq 不是整数字节
    }     
}

/*******************************************************************************
* 名称  : DLS_BitStreamGetFrame2
* 描述  : 获取缓存中的当前帧（缓存中比特的缓存顺序是帧的发射顺序，即帧的每个字节
          的LSB先收到，见“1371标准的3.3.7”）
* 输入  : pBitStreamStruct - BitStreamStruct结构体指针
* 输出  : pFrameBuff - 数据帧（已剔除填充位，不含帧头和帧尾，只包含数据和校验码，
          pFrameBuff中每个字节的MSB比LSB先收到）
* 返回  : TRUE - 帧有效，FALSE - 帧无效
*******************************************************************************/
static U8 DLS_BitStreamGetFrame2(BitStreamStruct *pBitStreamStruct, U8 *pFrameBuff)
{
    U16 tempU16;
    U16 outputIndex;
    U16 outputOffset;
    U16 frameBitNum;
    U16 i;
    U16 k;
    U8  highCnt;
    U8  byteData;
    U8  validBitCnt;
#if 0 //lqdb
    U8 temp[2 * DLS_FRAME_MAX_LEN_BYTE + 30];
    U8 temp2[3];
#endif
    k = 0;
    highCnt = 0;
    byteData = 0;
    validBitCnt = 0;
    frameBitNum = DLS_BitStreamGetFrameBitNum(pBitStreamStruct);
        
    tempU16 = pBitStreamStruct->HeadOffset + DLS_FRAME_HEADER_LEN;
    outputOffset = tempU16 % DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN;
    outputIndex = (pBitStreamStruct->HeadIndex + (tempU16 /
        DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN)) % DLS_BITSTREAM_BUFF_LEN;

    for (i = 0; i < frameBitNum; i++, outputOffset++)
    {
        if (outputOffset >= DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN)
        {
            outputOffset = 0;
            outputIndex = (outputIndex + 1) % DLS_BITSTREAM_BUFF_LEN;
        }
        if ((pBitStreamStruct->DataBuff[outputIndex] & (1 << outputOffset)) != 0)
        {
            /*lq 比特1 */
            if(highCnt >= 5)
            {
                DLS_BitStreamNextFrameSetup(pBitStreamStruct);                
#if 0
                usart_printf("$PacketProbe: Frame error for continous 1!\r\n");                                                                      
#endif     
                return FALSE;                   //lq 正常情况下，帧中不会出现连续6个"1"
            }
            else
            {
                byteData <<= 1;
                byteData |= 0x01;               //lq 记录'1'
                validBitCnt++;
                highCnt++;                      //lq 连续"1"计数
            }       
        }
        else
        {
            /*lq 比特0 */
            if(highCnt >= 5)
            {
                /*lq 剔除填充位，当比特0之前出现连续5个"1"，删除该比特"0" */ 
            }
            else
            {
                byteData <<= 1;                 //lq 记录'0'
                validBitCnt++;               
            }

            highCnt = 0;
        }
        
        if (validBitCnt >= 8)
        {
            validBitCnt = 0;
            
            /*lq 最大帧长判断 */
            if (k >= DLS_FRAME_MAX_LEN_BYTE)
            {
                DLS_BitStreamNextFrameSetup(pBitStreamStruct);
#if 0
                usart_printf("$PacketProbe: Frame error for too long!\r\n");                                                                      
#endif     
                return FALSE;                   //lq 超过最大帧长            
            }
            else
            {
                pFrameBuff[++k] = byteData;     //lq pFrameBuff[0] 存放有效字节总数
                byteData = 0;                                     
            }
        }    
    }
#if 0 //lqdb
    sprintf(temp, "$PacketProbe FrameData: ");            
    for (i = 0; i < k; i++)
    {
        sprintf(temp2, "%02X", pFrameBuff[1 + i]);
        strcat(temp, temp2);
    }
    strcat(temp, "\r\n");
    usart_printf(temp);
#endif  
    /*lq 整数字节判断 */
    if (validBitCnt == 0)
    {
        if (k < 3)
        {
            DLS_BitStreamNextFrameSetup(pBitStreamStruct);
#if 0
            usart_printf("$PacketProbe: Frame error for too short!\r\n");                                                                  
#endif     
            return FALSE;                       //lq 帧长无效（至少为3，1byte of data and 2bytes of CRC）        
        }
        else
        {
            pFrameBuff[0] = k;                  //lq 记录帧长
            DLS_BitStreamNextFrameSetup(pBitStreamStruct);
            return TRUE;        
        }
    }
    else
    {
        DLS_BitStreamNextFrameSetup(pBitStreamStruct);
#if 0
        usart_printf("$PacketProbe: Frame error for boundary!\r\n");                                                      
#endif     
        return FALSE;                           //lq 不是整数字节
    }     
}

/*******************************************************************************
* 名称  : DLS_BitStreamGetFrame3
* 描述  : 获取缓存中的当前帧（缓存中比特的缓存顺序是帧的发射顺序，即帧的每个字节
          的LSB先收到，见“1371标准的3.3.7”）
* 输入  : pBitStreamStruct - BitStreamStruct结构体指针
* 输出  : pFrameBuff - 数据帧（未剔除填充位，不含帧头和帧尾，只包含数据和校验码，
          pFrameBuff中每个字节的MSB比LSB先收到）
* 返回  : TRUE - 帧有效，FALSE - 帧无效
*******************************************************************************/
static U8 DLS_BitStreamGetFrame3(BitStreamStruct *pBitStreamStruct, U8 *pFrameBuff)
{
    U16 tempU16;
    U16 outputIndex;
    U16 outputOffset;
    U16 frameBitNum;
    U16 i;
    U16 k;
    U8  byteData;
    U8  validBitCnt;
#if 0 //lqdb
    U8 temp[2 * DLS_FRAME_MAX_LEN_BYTE + 30];
    U8 temp2[3];
#endif
    k = 0;
    byteData = 0;
    validBitCnt = 0;
    frameBitNum = DLS_BitStreamGetFrameBitNum(pBitStreamStruct);
    
    tempU16 = pBitStreamStruct->HeadOffset + DLS_FRAME_HEADER_LEN;
    outputOffset = tempU16 % DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN;
    outputIndex = (pBitStreamStruct->HeadIndex + (tempU16 /
        DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN)) % DLS_BITSTREAM_BUFF_LEN;

    for (i = 0; i < frameBitNum; i++, outputOffset++)
    {
        if (outputOffset >= DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN)
        {
            outputOffset = 0;
            outputIndex = (outputIndex + 1) % DLS_BITSTREAM_BUFF_LEN;
        }
        if ((pBitStreamStruct->DataBuff[outputIndex] & (1 << outputOffset)) != 0)
        {
            /*lq 比特1 */
                byteData <<= 1;
                byteData |= 0x01;               //lq 记录'1'
                validBitCnt++;
        }
        else
        {
            /*lq 比特0 */
            byteData <<= 1;                 //lq 记录'0'
            validBitCnt++; 
        }
        
        if (validBitCnt >= 8)
        {
            validBitCnt = 0;
            
            /*lq 最大帧长判断 */
            if (k >= DLS_FRAME_MAX_LEN_BYTE)
            {
                DLS_BitStreamNextFrameSetup(pBitStreamStruct);
#if 0
                usart_printf("$PacketProbe: Frame error for too long!\r\n");                                                                      
#endif     
                return FALSE;                   //lq 超过最大帧长            
            }
            else
            {
                pFrameBuff[++k] = byteData;     //lq pFrameBuff[0] 存放有效字节总数
                byteData = 0;                                     
            }
        }    
    }
#if 0 //lqdb
    sprintf(temp, "$PacketProbe FrameData: ");            
    for (i = 0; i < k; i++)
    {
        sprintf(temp2, "%02X", pFrameBuff[1 + i]);
        strcat(temp, temp2);
    }
    strcat(temp, "\r\n");
    usart_printf(temp);
#endif  
    /*lq 整数字节判断 */
    if (validBitCnt == 0)
    {
        if (k < 3)
        {
            DLS_BitStreamNextFrameSetup(pBitStreamStruct);
#if 0
            usart_printf("$PacketProbe: Frame error for too short!\r\n");                                                                  
#endif     
            return FALSE;                       //lq 帧长无效（至少为3，1byte of data and 2bytes of CRC）        
        }
        else
        {
            pFrameBuff[0] = k;                  //lq 记录帧长
            DLS_BitStreamNextFrameSetup(pBitStreamStruct);
            return TRUE;        
        }
    }
    else
    {
        DLS_BitStreamNextFrameSetup(pBitStreamStruct);
#if 0
        usart_printf("$PacketProbe: Frame error for boundary!\r\n");                                                      
#endif     
        return FALSE;                           //lq 不是整数字节
    }     
}

/*******************************************************************************
* 名称  : DLS_BitStreamFindFrameHeader
* 描述  : 从缓存查找帧头
* 输入  : pBitStreamStruct - BitStreamStruct结构体指针
* 输出  : pData - 读取的数据
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
static U8 DLS_BitStreamFindFrameHeader(BitStreamStruct *pBitStreamStruct)
{
    U32 i;
    U16 outputIndex;
    U16 outputOffset;
    U16 tempU16;
    U8  temp8;
    U8  header;

    /*lq 缓存bit数达到帧头长度 */
    while (DLS_BitStreamGetBitNum(pBitStreamStruct) >= DLS_FRAME_HEADER_LEN)
    {
        header = 0;
        
        /*lq 提取帧头长度的比特数 */
        outputIndex = pBitStreamStruct->Output;
        outputOffset = pBitStreamStruct->OutputOffset;

        for (i = 0; i < DLS_FRAME_HEADER_LEN; i++, outputOffset++)
        {
            if (outputOffset >= DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN)
            {
                outputOffset = 0;
                outputIndex = (outputIndex + 1) % DLS_BITSTREAM_BUFF_LEN; 
            }
            header <<= 1;
            if ((pBitStreamStruct->DataBuff[outputIndex] & ( 1 << outputOffset))
                != 0)
            {
                header |=  1;
            }
        }

        if (header == DLS_FRAME_HEADER)
        {
            pBitStreamStruct->HeadIndex = pBitStreamStruct->Output;
            pBitStreamStruct->HeadOffset = pBitStreamStruct->OutputOffset;
            
            tempU16 = pBitStreamStruct->HeadOffset + DLS_FRAME_HEADER_LEN;
            pBitStreamStruct->TailOffset = tempU16 % DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN;
            pBitStreamStruct->TailIndex = (pBitStreamStruct->Output
                + (tempU16 / DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN))
                % DLS_BITSTREAM_BUFF_LEN;       //lq 帧尾检测的起始位置

            return TRUE;
        }
        else
        {
            /*lq 删除帧头出现位置之前的比特 */
            DLS_BitStreamGetBit(pBitStreamStruct, &temp8);
        }
    }

    return FALSE;
}

/*******************************************************************************
* 名称  : DLS_BitStreamFindFrameTailer
* 描述  : 从缓存查找帧尾（帧头和帧尾相同的情况）
* 输入  : pBitStreamStruct - BitStreamStruct结构体指针
* 输出  : pData - 读取的数据
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
static U8 DLS_BitStreamFindFrameTailer(BitStreamStruct *pBitStreamStruct)
{
    U32 i;
    U16 tailIndex;
    U16 tailOffset;
    U16 tempU16;
    U8  tailer;
    
    /*lq 缓存bit数未超过2倍帧头长度 */
    if (DLS_BitStreamGetBitNum(pBitStreamStruct) < (DLS_FRAME_HEADER_LEN << 1))
    {
        return FALSE;
    }

    /*lq 查找帧尾（查找范围为TailIndex ~ Input） */
    while (DLS_BitStreamGetBitQuantity(pBitStreamStruct->TailIndex,
           pBitStreamStruct->TailOffset, pBitStreamStruct->Input,
           pBitStreamStruct->InputOffset) >= DLS_FRAME_HEADER_LEN) //lq TailIndex ~ Input之间的比特数大于帧尾
    {
        tailer = 0;
        
        /*lq 提取帧头长度的比特数 */
        tailIndex = pBitStreamStruct->TailIndex;
        tailOffset = pBitStreamStruct->TailOffset;

        for (i = 0; i < DLS_FRAME_HEADER_LEN; i++, tailOffset++)
        {
            if (tailOffset >= DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN)
            {
                tailOffset = 0;
                tailIndex = (tailIndex + 1) % DLS_BITSTREAM_BUFF_LEN; 
            }

            tailer <<= 1;
            if ((pBitStreamStruct->DataBuff[tailIndex] & (1 << tailOffset))
                != 0)
            {
                tailer |= 1; 
            }
        }

        if (tailer == DLS_FRAME_HEADER)
        {            
            return TRUE;
        }
        else
        {
            /*lq 帧尾索引后移 */
            pBitStreamStruct->TailOffset++;
            if (pBitStreamStruct->TailOffset >= DLS_BIT_STREAM_BUFF_ELEM_BIT_LEN)
            {
                pBitStreamStruct->TailOffset = 0;
                pBitStreamStruct->TailIndex = (pBitStreamStruct->TailIndex + 1)
                                               % DLS_BITSTREAM_BUFF_LEN;
            }
        }
    }

    return FALSE;
}

/*******************************************************************************
* 名称  : DLS_FrameRxFifoIsFull
* 描述  : 帧队列是否已满
* 输入  : pFrameRxFifoStruct - FrameRxFifoStruct结构体指针		  
* 输出  : 无
* 返回  : TRUE - 已满，FALSE - 未满
*******************************************************************************/
static U8 DLS_FrameRxFifoIsFull(FrameRxFifoStruct *pFrameRxFifoStruct)
{
    if ((pFrameRxFifoStruct->Input + 1) % DLS_FRAME_FIFO_DEPTH
        == pFrameRxFifoStruct->Output)
    {
        return TRUE;

        debug_printf("$[%6d:%4d] %s : Frame rx fifo [%s]is full!\r\n",
            GetFrameNum(), GetSlotNum(), __func__,
            pFrameRxFifoStruct == &FrameFifoRx1 ? "A" : "B");
    }
    else
    {
        return FALSE;
    }     
}

/*******************************************************************************
* 名称  : DLS_FrameTxFifoIsFull
* 描述  : 帧队列是否已满
* 输入  : pFrameTxFifoStruct - FrameTxFifoStruct结构体指针		  
* 输出  : 无
* 返回  : TRUE - 已满，FALSE - 未满
*******************************************************************************/
static U8 DLS_FrameTxFifoIsFull(FrameTxFifoStruct *pFrameTxFifoStruct)
{
    if ((pFrameTxFifoStruct->Input + 1) % DLS_FRAME_FIFO_DEPTH
        == pFrameTxFifoStruct->Output)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }     
}

/*******************************************************************************
* 名称  : DLS_FrameRxFifoIsEmpty
* 描述  : 帧队列是否是否为空
* 输入  : pFrameRxFifoStruct - FrameRxFifoStruct结构体指针		  
* 输出  : 无
* 返回  : TRUE - 为空，FALSE - 非空
*******************************************************************************/
static U8 DLS_FrameRxFifoIsEmpty(FrameRxFifoStruct *pFrameRxFifoStruct)
{
    if (pFrameRxFifoStruct->Input == pFrameRxFifoStruct->Output)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* 名称  : DLS_FrameTxFifoIsEmpty
* 描述  : 帧队列是否是否为空
* 输入  : pFrameTxFifoStruct - FrameTxFifoStruct结构体指针		  
* 输出  : 无
* 返回  : TRUE - 为空，FALSE - 非空
*******************************************************************************/
static U8 DLS_FrameTxFifoIsEmpty(FrameTxFifoStruct *pFrameTxFifoStruct)
{
    if (pFrameTxFifoStruct->Input == pFrameTxFifoStruct->Output)
    {
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* 名称  : DLS_FrameRxFifoEnque
* 描述  : 帧队列入队
* 输入  : pFrameRxFifoStruct - FrameRxFifoStruct结构体指针
          pFrameRxCellStruct - 指向入队数据单元的指针
* 输出  : 无
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
static U8 DLS_FrameRxFifoEnque(FrameRxFifoStruct *pFrameRxFifoStruct, const FrameRxCellStruct *pFrameRxCellStruct)
{
    U16 i;
    FrameRxCellStruct *pCell;
       
    if (DLS_FrameRxFifoIsFull(pFrameRxFifoStruct) == FALSE)
    {      
        pCell = &(pFrameRxFifoStruct->FIFO[pFrameRxFifoStruct->Input]);
        
        pCell->DataLen = pFrameRxCellStruct->DataLen;
        for (i = 0; i < pFrameRxCellStruct->DataLen; i++)        
        {
            pCell->Data[i] = pFrameRxCellStruct->Data[i];         
        }
        pCell->RxChannel = pFrameRxCellStruct->RxChannel;
        pCell->ArrivalFrame = pFrameRxCellStruct->ArrivalFrame;
        pCell->ArrivalSlot = pFrameRxCellStruct->ArrivalSlot;
                
        pFrameRxFifoStruct->Input = (pFrameRxFifoStruct->Input + 1) % DLS_FRAME_FIFO_DEPTH;
        
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* 名称  : DLS_FrameTxFifoEnque
* 描述  : 发射帧队列入队
* 输入  : pFrameTxFifoStruct - FrameTxFifoStruct结构体指针
          pFrameTxCellStruct - 指向入队数据单元的指针
* 输出  : 无
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
static U8 DLS_FrameTxFifoEnque(FrameTxFifoStruct *pFrameTxFifoStruct, const FrameTxCellStruct *pFrameTxCellStruct)
{
    U16 i;
    FrameTxCellStruct *pCell;
       
    if (DLS_FrameTxFifoIsFull(pFrameTxFifoStruct) == FALSE)
    {      
        pCell = &(pFrameTxFifoStruct->FIFO[pFrameTxFifoStruct->Input]);
        
        //lq pCell->DataLenBit = pFrameTxCellStruct->DataLenBit;
        for (i = 0; i < pFrameTxCellStruct->Data[0] + 3; i++)   //lq 前2字节+整字节数+非整字节        
        {
            pCell->Data[i] = pFrameTxCellStruct->Data[i];         
        }
        pCell->TxChannel = pFrameTxCellStruct->TxChannel;
        pCell->TxSlot = pFrameTxCellStruct->TxSlot;
                
        pFrameTxFifoStruct->Input = (pFrameTxFifoStruct->Input + 1) % DLS_FRAME_FIFO_DEPTH;
        
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* 名称  : DLS_Inst1FrameRxFifoEnque
* 描述  : 接收帧队列入队
* 输入  : pFrameTxCellStruct - 指向入队数据单元的指针      		  
* 输出  : 无
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
U8 DLS_Inst1FrameRxFifoEnque(const FrameRxCellStruct *pFrameRxCellStruct)
{
    return DLS_FrameRxFifoEnque(&FrameFifoRx1, pFrameRxCellStruct);
}

/*******************************************************************************
* 名称  : DLS_Inst2FrameRxFifoEnque
* 描述  : 帧队列入队
* 输入  : pFrameTxCellStruct - 指向入队数据单元的指针      		  
* 输出  : 无
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
U8 DLS_Inst2FrameRxFifoEnque(const FrameRxCellStruct *pFrameRxCellStruct)
{
    return DLS_FrameRxFifoEnque(&FrameFifoRx2, pFrameRxCellStruct);
}

/*******************************************************************************
* 名称  : DLS_InstFrameTxFifoEnque
* 描述  : 帧队列入队
* 输入  : pFrameTxCellStruct - 指向入队数据单元的指针       		  
* 输出  : 无
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
U8 DLS_InstFrameTxFifoEnque(const FrameTxCellStruct *pFrameTxCellStruct)
{
    return DLS_FrameTxFifoEnque(&FrameFifoTx, pFrameTxCellStruct);
}

/*******************************************************************************
* 名称  : DLS_FrameRxFifoDeque
* 描述  : 帧队列出队
* 输入  : pFrameRxFifoStruct - FrameFifoStruct结构体指针          
* 输出  : pFrameRxCellStruct - 指向出队数据单元的指针
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
static U8 DLS_FrameRxFifoDeque(FrameRxFifoStruct *pFrameRxFifoStruct, FrameRxCellStruct *pFrameRxCellStruct)
{
    U16 i;
    FrameRxCellStruct *pCell;

    if (DLS_FrameRxFifoIsEmpty(pFrameRxFifoStruct) == FALSE)
    {        
        pCell = &(pFrameRxFifoStruct->FIFO[pFrameRxFifoStruct->Output]);
        
        pFrameRxCellStruct->DataLen = pCell->DataLen;
        for (i = 0; i < pCell->DataLen; i++)        
        {
            pFrameRxCellStruct->Data[i] = pCell->Data[i];         
        }
        pFrameRxCellStruct->RxChannel = pCell->RxChannel;
        pFrameRxCellStruct->ArrivalFrame = pCell->ArrivalFrame;
        pFrameRxCellStruct->ArrivalSlot = pCell->ArrivalSlot;

        pFrameRxFifoStruct->Output = (pFrameRxFifoStruct->Output + 1) % DLS_FRAME_FIFO_DEPTH;
        
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* 名称  : DLS_FrameTxFifoDeque
* 描述  : 帧队列出队
* 输入  : pFrameTxFifoStruct - FrameFifoStruct结构体指针          
* 输出  : pFrameTxCellStruct - 指向出队数据单元的指针
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
static U8 DLS_FrameTxFifoDeque(FrameTxFifoStruct *pFrameTxFifoStruct, FrameTxCellStruct *pFrameTxCellStruct)
{
    U16 i;
    FrameTxCellStruct *pCell;

    if (DLS_FrameTxFifoIsEmpty(pFrameTxFifoStruct) == FALSE)
    {        
        pCell = &(pFrameTxFifoStruct->FIFO[pFrameTxFifoStruct->Output]);
        
        //lq pFrameTxCellStruct->DataLenBit = pCell->DataLenBit;
        for (i = 0; i < pCell->Data[0] + 3; i++)       //lq 前2字节+整字节数+非整字节                
        {
            pFrameTxCellStruct->Data[i] = pCell->Data[i];         
        }
        pFrameTxCellStruct->TxChannel = pCell->TxChannel;
        pFrameTxCellStruct->TxSlot = pCell->TxSlot;

        pFrameTxFifoStruct->Output = (pFrameTxFifoStruct->Output + 1) % DLS_FRAME_FIFO_DEPTH;
        
        return TRUE;    
    }
    else
    {
        return FALSE;
    }    
}

/*******************************************************************************
* 名称  : DLS_Inst1FrameRxFifoDeque
* 描述  : 帧队列出队
* 输入  : 无
* 输出  : pFrameTxCellStruct - 指向出队数据单元的指针
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
U8 DLS_Inst1FrameRxFifoDeque(FrameRxCellStruct *pFrameRxCellStruct)
{
    return DLS_FrameRxFifoDeque(&FrameFifoRx1, pFrameRxCellStruct);
}

/*******************************************************************************
* 名称  : DLS_Inst2FrameRxFifoDeque
* 描述  : 帧队列出队
* 输入  : 无
* 输出  : pFrameTxCellStruct - 指向出队数据单元的指针
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
U8 DLS_Inst2FrameRxFifoDeque(FrameRxCellStruct *pFrameRxCellStruct)
{
    return DLS_FrameRxFifoDeque(&FrameFifoRx2, pFrameRxCellStruct);
}

/*******************************************************************************
* 名称  : DLS_InstFrameRxFifoDeque
* 描述  : 帧队列出队
* 输入  : 无
* 输出  : pFrameTxCellStruct - 指向出队数据单元的指针
* 返回  : TRUE - 成功，FALSE - 失败
*******************************************************************************/
U8 DLS_InstFrameTxFifoDeque(FrameTxCellStruct *pFrameTxCellStruct)
{
    return DLS_FrameTxFifoDeque(&FrameFifoTx, pFrameTxCellStruct);
}

/*******************************************************************************
* 名称  : DLS_PacketProbeProcess
* 描述  : 数据包探测
* 输入  : pPacketProbeStruct - PacketProbeStruct结构体指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void DLS_PacketProbeProcess(PacketProbeStruct *pPacketProbeStruct, FrameRxFifoStruct *pFrameRxFifoStruct)
{
    FrameRxCellStruct *pFrameRxCell;
    U8 *pFrame;
    U16 i;
    S32 tempS32;

    switch (pPacketProbeStruct->State)
    {
        case DLS_PACKET_PROBE_STATE_HEAD_PROBING:
            
            /*lq 判断是否发现帧头 */
            if (DLS_BitStreamFindFrameHeader(&pPacketProbeStruct->BitStream) == TRUE)
            {
                pPacketProbeStruct->State = DLS_PACKET_PROBE_STATE_TAIL_PROBING;
                //lq usart_printf("$PacketProbe: Frame header found!\r\n");                                            
            }

            break;
        case DLS_PACKET_PROBE_STATE_TAIL_PROBING:
            
            /*lq 判断比特流缓存是否已满 */
            if (DLS_BitStreamIsBuffFull(&pPacketProbeStruct->BitStream) == TRUE)
            {
                /*lq 清空缓存 */ 
				/*zjw 达到最大长度，保留最后7比特数据 */
                DLS_BitStreamFlushBuff(&pPacketProbeStruct->BitStream);
                
                /*lq 转入帧头检测 */
                pPacketProbeStruct->State = DLS_PACKET_PROBE_STATE_HEAD_PROBING;                    
            }
            else
            {
                /*lq 判断是否发现帧尾 */
                if (DLS_BitStreamFindFrameTailer(&pPacketProbeStruct->BitStream) == TRUE)
                {
                    //lq usart_printf("$PacketProbe: Frame tailer found!\r\n");                                              
#if 0
                    usart_printf("$PacketProbe: RSSI %d, %d,",
                    SPIRIT1_ReadByte(SPIRIT1TypeA, SPIRIT1_REGADDR_FU_RSSI_LEVEL),
                    SPIRIT1_GetRegFieldValue(SPIRIT1TypeA, SPIRIT1_REGADDR_FU_LINK_QUALIF(1), SPIRIT1_FU_LINK_QUALIF1_BITMASK_CS));                                              
#endif
                    /*lq 提取帧 */
                    pFrame = (U8 *)malloc((DLS_FRAME_MAX_LEN_BYTE + 1) * sizeof(U8));   //lq [0]存放帧长的字节数
                    memset((U8 *)pFrame, 0, (DLS_FRAME_MAX_LEN_BYTE + 1) * sizeof(U8));
#if 1
                    if (DLS_BitStreamGetFrame(&pPacketProbeStruct->BitStream, pFrame) == TRUE)
#else   //lqdb
                    if (DLS_BitStreamGetFrame3(&pPacketProbeStruct->BitStream, pFrame) == TRUE)
#endif
                    {
                        pFrameRxCell = (FrameRxCellStruct *)malloc(sizeof(FrameRxCellStruct));
                        memset((FrameRxCellStruct *)pFrameRxCell, 0, sizeof(FrameRxCellStruct));
                                                
                        tempS32 = GetSlotSum();
                        pFrameRxCell->ArrivalFrame = tempS32 / FRAME_SLOT_SIZE;
                        pFrameRxCell->ArrivalSlot = tempS32 % FRAME_SLOT_SIZE;

                        pFrameRxCell->DataLen = pFrame[0];
                        for (i = 0; i < pFrame[0]; i++)
                        {
                            pFrameRxCell->Data[i] = pFrame[i + 1];
                        }
                        
                        if (pPacketProbeStruct == &PacketProbeInst1)
                        {
                            pFrameRxCell->RxChannel = ENUM_AIS_CHANNEL_A;
                        }
                        else
                        {
                            pFrameRxCell->RxChannel = ENUM_AIS_CHANNEL_B;
                        }
                                                                        
                        /*lq 接收帧入队 */
                        DLS_FrameRxFifoEnque(pFrameRxFifoStruct, pFrameRxCell);
                        free((FrameRxCellStruct *)pFrameRxCell);
#if 0
                        usart_printf("$PacketProbe: Frame extraction completed!\r\n"); 
#endif                                                                     
                    }
                    
                    free((U8 *)pFrame);

                    /*lq 转入帧头检测 */
                    pPacketProbeStruct->State = DLS_PACKET_PROBE_STATE_HEAD_PROBING;
                }            
            }

            break;
        default :
            
            DLS_PacketProbeStructInit(pPacketProbeStruct);
            break;
    }    
}

/*******************************************************************************
* 名称  : DLS_InstPacketProbeProcess
* 描述  : 数据包探测
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void DLS_InstPacketProbeProcess(void)
{
    DLS_PacketProbeProcess(&PacketProbeInst1, &FrameFifoRx1);    
    DLS_PacketProbeProcess(&PacketProbeInst2, &FrameFifoRx2);    
}

#if 0
/*******************************************************************************
* 名称  : DLS_FrameCrcCheck
* 描述  : 构造包含CRC的数据
* 输入  : buff1 - 待校验数据（buff1[0]-待校验数据的整字节数，buff1[1]-待校验数据
          的非整字节的比特数，该值应始终为0）
* 输出  : buff1 - 含CRC的数据
* 返回  : 无
*******************************************************************************/
void DLS_FrameCrcCheck(U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2])
{
    U16 CRCVal;

    CRCVal = AIS_Cal_CRC16(&buff1[2], buff1[0]); 
	buff1[buff1[0] + 2] = ((CRCVal >> 8) & 0x00ff);     //lq CRC的高位
	buff1[buff1[0] + 3] = (CRCVal & 0x00ff);
    buff1[0] += 2;                                      //lq 数据字节长度加2       
}
#else
/*******************************************************************************
* 名称  : DLS_FrameCrcCheck
* 描述  : 构造包含CRC的数据
* 输入  : buff1 - 待校验数据（buff1[0]-待校验数据的整字节数，buff1[1]-待校验数据
          的非整字节的比特数，该值应始终为0）
* 输出  : buff2 - 含CRC的数据
* 返回  : 无
*******************************************************************************/
void DLS_FrameCrcCheck(const U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], U8 buff2[DLS_FRAME_MAX_LEN_BYTE + 2])
{
    U16 CRCVal;
    U16 index1;
    U8  byteNum;

	byteNum = buff1[0];

	memset((U8*)buff2, 0, (DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));

    /*lq 整字节 */
    for (index1 = 2; index1 < byteNum + 2; index1++)
    {
        buff2[index1] = buff1[index1];
    }

    CRCVal = AIS_Cal_CRC16(&buff2[2], byteNum); 
	buff2[index1++] = ((CRCVal >> 8) & 0x00ff);                 //lq CRC的高位
	buff2[index1++] = (CRCVal & 0x00ff);
    buff2[0] = byteNum + 2;                                     //lq 数据字节长度加2       
    buff2[1] = 0;
}
#endif

/*******************************************************************************
* 名称  : DLS_FrameBitStuff
* 描述  : 构造包含填充位的数据 （buff2与buff1中数据的MSB与LSB顺序一致，填充位时连
          续1的计数是从每个字节的LSB至MSB的方向计数的，即帧实际发送至物理链路的顺
          序）
* 输入  : buff1 - 待添加填充位的数据（buff1[0]-待添加填充位的数据的整字节数，
          buff1[1]-待添加填充位的数据的非整字节比特数。）
* 输出  : buff2 - 已添加填充位的数据（buff2[0]-已添加填充位的数据的整字节数，
          buff2[1]-已添加填充位的数据的非整字节比特数。）
* 返回  : 无
*******************************************************************************/
void DLS_FrameBitStuff(const U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], U8 buff2[DLS_FRAME_MAX_LEN_BYTE + 2])
{
    U16 index1;
    U16 index2;
    U8  offset1;
    U8  offset2;
    U8  byteNum;
    U8  bitNum;
    U8  consOnes;

    index2 = 2;
    offset2 = 0;
    consOnes = 0;
	byteNum = buff1[0];
    bitNum = buff1[1];

	memset((U8*)buff2, 0, (DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));

    /*lq 整字节 */
    for (index1 = 2; index1 < byteNum + 2; index1++)
    {
        for (offset1 = 0; offset1 < 8; offset1++)
        {
            if ((buff1[index1] & (1u << offset1)) != 0) //lq 当前位为‘1’
            {
                buff2[index2] |= (1u << offset2++);
                if (offset2 >= 8)
                {
                    offset2 = 0;
                    index2++;
                }

                consOnes++;
                if (consOnes >= 5)                      //lq 连续5个1，填充一个0
                {
                    consOnes = 0;
                    offset2++;                          //lq 缓存已初始化为0，只需移动索引
                    if (offset2 >= 8)
                    {
                        offset2 = 0;
                        index2++;
                    }
                }
            }
            else                                        //lq 当前位为‘0’
            {
                consOnes = 0;
                offset2++;                              //lq 缓存已初始化为0，只需移动索引
                if (offset2 >= 8)
                {
                    offset2 = 0;
                    index2++;
                }            
            }
        }
    } 
    
    /*lq 非整字节比特 */
    for (offset1 = 0; offset1 < bitNum; offset1++)
    {
        if ((buff1[index1] & (1u << offset1)) != 0)     //lq 当前位为‘1’
        {
            buff2[index2] |= (1u << offset2++);
            if (offset2 >= 8)
            {
                offset2 = 0;
                index2++;
            }

            consOnes++;
            if (consOnes >= 5)                          //lq 连续5个1，填充一个0
            {
                consOnes = 0;
                offset2++;                              //lq 缓存已初始化为0，只需移动索引
                if (offset2 >= 8)
                {
                    offset2 = 0;
                    index2++;
                }
            }
        }
        else                                            //lq 当前位为‘0’
        {
            consOnes = 0;
            offset2++;                                  //lq 缓存已初始化为0，只需移动索引
            if (offset2 >= 8)
            {
                offset2 = 0;
                index2++;
            }            
        }        
    }
    
    /*lq 记录整字节数和非整字节比特数 */
    buff2[0] = index2 - 2;
    buff2[1] = offset2; 
}

/*******************************************************************************
* 名称  : DLS_FrameBitDestuff
* 描述  : 构造剔除填充位的数据 （buff2与buff1中数据的MSB与LSB顺序一致，填充位时连续1
          的计数是从每个字节的LSB至MSB的方向计数的，即帧实际发送至物理链路的顺序）
* 输入  : buff1 - 待剔除填充位的数据（buff1[0]-待剔除填充位的数据的整字节数，
          buff1[1]-待剔除填充位的数据的非整字节比特数。）
* 输出  : buff2 - 已剔除填充位的数据（buff2[0]-已剔除填充位的数据的整字节数，
          buff2[1]-已剔除填充位的数据的非整字节比特数。）
* 返回  : 无
*******************************************************************************/
void DLS_FrameBitDestuff(const U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], U8 buff2[DLS_FRAME_MAX_LEN_BYTE + 2])
{
    U16 index1;
    U16 index2;
    U8  offset1;
    U8  offset2;
    U8  byteNum;
    U8  bitNum;
    U8  consOnes;

    index2 = 2;
    offset2 = 0;
    consOnes = 0;
	byteNum = buff1[0];
    bitNum = buff1[1];

	memset((U8*)buff2, 0, (DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));

    /*lq 整字节 */
    for (index1 = 2; index1 < byteNum + 2; index1++)
    {
        for (offset1 = 0; offset1 < 8; offset1++)
        {
            if ((buff1[index1] & (1u << offset1)) != 0) //lq 当前位为‘1’
            {
                buff2[index2] |= (1u << offset2++);
                if (offset2 >= 8)
                {
                    offset2 = 0;
                    index2++;
                }

                consOnes++;
            }
            else                                        //lq 当前位为‘0’
            {
                if (consOnes >= 5)                      //lq 连续5个1，删除后面紧跟的一个0
                {
                    /*lq 剔除填充位，当比特0之前出现连续5个"1"，删除该比特"0" */ 
                }
                else
                {
                    offset2++;
                    if (offset2 >= 8)
                    {
                        offset2 = 0;
                        index2++;
                    }                
                }
                
                consOnes = 0;                        
            }
        }
    } 
    
    /*lq 非整字节比特 */
    for (offset1 = 0; offset1 < bitNum; offset1++)
    {
        if ((buff1[index1] & (1u << offset1)) != 0) //lq 当前位为‘1’
        {
            buff2[index2] |= (1u << offset2++);
            if (offset2 >= 8)
            {
                offset2 = 0;
                index2++;
            }

            consOnes++;
        }
        else                                        //lq 当前位为‘0’
        {
            if (consOnes >= 5)                      //lq 连续5个1，删除后面紧跟的一个0
            {
                /*lq 剔除填充位，当比特0之前出现连续5个"1"，删除该比特"0" */ 
            }
            else
            {
                offset2++;
                if (offset2 >= 8)
                {
                    offset2 = 0;
                    index2++;
                }                
            }
            
            consOnes = 0;                        
        }        
    }
    
    /*lq 记录整字节数和非整字节比特数 */
    buff2[0] = index2 - 2;
    buff2[1] = offset2; 
}

/*******************************************************************************
* 名称  : DLS_FrameDataPack
* 描述  : 数据打包成帧
* 输入  : buff1 - 待打包数据（包括数据部分和CRC，且已进行位填充）
* 输出  : buff2 - 已打包数据（包括同步序列、帧头、数据、校验码和帧尾，但未进行
          NRZI编码）
* 返回  : 无
*******************************************************************************/
void DLS_FrameDataPack(const U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], U8 buff2[DLS_FRAME_MAX_LEN_BYTE + 2])
{
    U16 index1;
    U16 index2;
    U8  offset1;
    U8  offset2;
    U8  byteNum;
    U8  bitNum;

    index2 = 2;
    offset2 = 0;	
    byteNum = buff1[0];
    bitNum = buff1[1];
             
	memset((U8*)buff2, 0, (DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));

    /*lq 同步序列 */
    buff2[index2++] = 0xAA;     //lq LSB先发送，故实际发送的值为0x55
    buff2[index2++] = 0xAA;
    buff2[index2++] = 0xAA;

    /*lq 帧头 */
    buff2[index2++] = DLS_FRAME_HEADER;

    /*lq 数据+CRC （整字节部分）*/
    for (index1 = 2; index1 < byteNum + 2; index1++)
    {
        buff2[index2++] = buff1[index1];    
    }

    /*lq 非整字节比特 */
    for (offset1 = 0; offset1 < bitNum; offset1++)
    {
        if ((buff1[index1] & (1u << offset1)) != 0) //lq 当前位为‘1’
        {
            buff2[index2] |= (1u << offset2++);
            if (offset2 >= 8)
            {
                offset2 = 0;
                index2++;
            }
        }
        else                                        //lq 当前位为‘0’
        {
            offset2++;
            if (offset2 >= 8)
            {
                offset2 = 0;
                index2++;
            }                        
        }        
    }

    /*lq 帧尾 */     
    for (index1 = 0; index1 < 8; index1++)
    {
        if ((DLS_FRAME_TAILER & (1u << index1)) != 0)    //lq 当前位为‘1’
        {
            buff2[index2] |= (1u << offset2++);
            if (offset2 >= 8)
            {
                offset2 = 0;
                index2++;
            }
        }
        else                                        //lq 当前位为‘0’
        {
            offset2++;
            if (offset2 >= 8)
            {
                offset2 = 0;
                index2++;
            }                        
        }
     }

    /*lq 记录整字节数和非整字节比特数 */
    buff2[0] = index2 - 2;
    buff2[1] = offset2; 
}

/*******************************************************************************
* 名称  : DLS_FrameDataPack1
* 描述  : 数据打包成帧（同步序列、帧头和帧尾使用NRZI，数据和校验码不使用NRZI）
* 输入  : buff1 - 待打包数据（包括数据部分和CRC，且已进行位填充）
* 输出  : buff2 - 已打包数据（包括同步序列、帧头、数据、校验码和帧尾，同步序列、
          帧头和帧尾使用NRZI，数据和校验码不使用NRZI）
* 返回  : 无
*******************************************************************************/
void DLS_FrameDataPack1(const U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], U8 buff2[DLS_FRAME_MAX_LEN_BYTE + 2])
{
    U16 index1;
    U16 index2;
    U8  offset1;
    U8  offset2;
    U8  byteNum;
    U8  bitNum;
    U8  priorValue;

    index2 = 2;
    offset2 = 0;	
    byteNum = buff1[0];
    bitNum = buff1[1];
             
	memset((U8*)buff2, 0, (DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));

    /*lq 同步序列（NZRI） */
    buff2[index2++] = 0xCC;     //lq LSB先发送，实际发送的是01010101的NRZI（初始值为1）编码数据
    buff2[index2++] = 0xCC;
    buff2[index2++] = 0xCC;

    /*lq 帧头（NZRI） */
    buff2[index2++] = 0x81;     //lq NRZI（初始值为NRZI后的同步序列的最后一位）

    /*lq 数据+CRC （整字节部分）*/
    for (index1 = 2; index1 < byteNum + 2; index1++)
    {
        buff2[index2++] = buff1[index1];    
    }

    /*lq 非整字节比特 */
    for (offset1 = 0; offset1 < bitNum; offset1++)
    {
        if ((buff1[index1] & (1u << offset1)) != 0) //lq 当前位为‘1’
        {
            buff2[index2] |= (1u << offset2++);
            if (offset2 >= 8)
            {
                offset2 = 0;
                index2++;
            }
        }
        else                                        //lq 当前位为‘0’
        {
            offset2++;
            if (offset2 >= 8)
            {
                offset2 = 0;
                index2++;
            }                        
        }        
    }

    /*lq 确定最后一位的值（用于帧尾NRZI的初始值） */
    if (offset2 != 0)
    {
        if ((buff2[index2] & (1u << (offset2 - 1))) == 0)
        {
            priorValue = 0;
        }
        else
        {
            priorValue = 1;        
        }
    }
    else
    {
        if ((buff2[index2 - 1] & 0x80) == 0)
        {
            priorValue = 0;
        }
        else
        {
            priorValue = 1;        
        }    
    }

    /*lq 帧尾（NRZI） */
    for (offset1 = 0; offset1 < 8; offset1++)
    {
        if ((DLS_FRAME_TAILER & (1u << offset1)) != 0)  //lq 当前位为‘1’, 需保持
        {                
            if (priorValue == 0)
            {
                buff2[index2] &= (~(1u << offset2));    //lq 保持为‘0’                
                priorValue = 0;                
            }
            else
            {
                buff2[index2] |= (1u << offset2);       //lq 保持为‘1’
                priorValue = 1;
            }
        }
        else                                            //lq 当前位为‘0’，需翻转
        {
            if (priorValue == 0)
            {
                buff2[index2] |= (1u << offset2);       //lq 翻转为‘1’
                priorValue = 1;
            }
            else
            {
                buff2[index2] &= (~(1u << offset2));    //lq 翻转为‘0’                
                priorValue = 0;
            }
        }

        offset2++;
        if (offset2 >= 8)
        {
            offset2 = 0;
            index2++;
        }
    }

    /*lq 记录整字节数和非整字节比特数 */
    buff2[0] = index2 - 2;
    buff2[1] = offset2; 
}

/*******************************************************************************
* 名称  : DLS_FrameDataDepack
* 描述  : 数据帧提取数据
* 输入  : buff1 - 待提取数据（包括同步序列、帧头、数据、校验码和帧尾，但未进行
          NRZI编码）
* 输出  : buff2 - 已提取数据（包括数据部分和CRC，且包含位填充）
* 返回  : 无
*******************************************************************************/
void DLS_FrameDataDepack(const U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], U8 buff2[DLS_FRAME_MAX_LEN_BYTE + 2])
{
    U16 index1;
    U16 index2;
    U8  offset1;
    U8  offset2;
    U8  byteNum;
    U8  bitNum;

    index2 = 2;
    offset2 = 0;	
    byteNum = buff1[0];
    bitNum = buff1[1];
             
	memset((U8*)buff2, 0, (DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));

    /*lq 数据+CRC （整字节部分）*/
    for (index1 = 6; index1 < byteNum + 1; index1++)
    {
        buff2[index2++] = buff1[index1];    
    }

    /*lq 非整字节比特 */
    for (offset1 = 0; offset1 < bitNum; offset1++)
    {
        if ((buff1[index1] & (1u << offset1)) != 0) //lq 当前位为‘1’
        {
            buff2[index2] |= (1u << offset2++);
            if (offset2 >= 8)
            {
                offset2 = 0;
                index2++;
            }
        }
        else                                        //lq 当前位为‘0’
        {
            offset2++;
            if (offset2 >= 8)
            {
                offset2 = 0;
                index2++;
            }                        
        }        
    }

    /*lq 记录整字节数和非整字节比特数 */
    buff2[0] = index2 - 2;
    buff2[1] = offset2; 
}

#if 0
/*******************************************************************************
* 名称  : DLS_FrameNrziCode
* 描述  : NRZI编码（每个字节的编码顺序是从LSB至MSB，与送入物理链路的比特流顺序一
          致）
* 输入  : buff1 - 待NRZI编码数据（包括同步序列、帧头、数据、校验码和帧尾及位填充）
          initValue - NRZI初始值
* 输出  : buff1 - 已NRZI编码数据（包括同步序列、帧头、数据、校验码和帧尾及位填充）
* 返回  : 无
*******************************************************************************/
void DLS_FrameNrziCode(U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], const U8 initValue)
{
    U16 index1;
    U8  offset1;
    U8  byteNum;
    U8  bitNum;
    U8  priorValue;
	
    byteNum = buff1[0];
    bitNum = buff1[1];
    priorValue = initValue;
     
    if (priorValue != 0) //lq 无效值保护
    {
        priorValue = 1;
    }

    /*lq 整字节 */
    for (index1 = 2; index1 < byteNum + 2; index1++)
    {
        for (offset1 = 0; offset1 < 8; offset1++)
        {                                  
            if ((buff1[index1] & (1u << offset1)) != 0) //lq 当前位为‘1’, 需保持
            {                
                if (priorValue == 0)
                {
                    buff1[index1] &= ~(1u << offset1);  //lq 保持为‘0’                
                    priorValue = 0;
                }
                else
                {
                    buff1[index1] |= 1u << offset1;     //lq 保持为‘1’
                    priorValue = 1;
                }
            }
            else                                        //lq 当前位为‘0’，需翻转
            {
                if (priorValue == 0)
                {
                    buff1[index1] |= 1u << offset1;     //lq 翻转为‘1’
                    priorValue = 1;
                }
                else
                {
                    buff1[index1] &= ~(1u << offset1);  //lq 翻转为‘0’                
                    priorValue = 0;
                }
            }
        }    
    }

    /*lq 非整字节比特 */
    for (offset1 = 0; offset1 < bitNum; offset1++)
    {
        if ((buff1[index1] & (1u << offset1)) != 0) //lq 当前位为‘1’, 需保持
        {                
            if (priorValue == 0)
            {
                buff1[index1] &= ~(1u << offset1);  //lq 保持为‘0’                
                priorValue = 0;
            }
            else
            {
                buff1[index1] |= 1u << offset1;     //lq 保持为‘1’
                priorValue = 1;
            }
        }
        else                                        //lq 当前位为‘0’，需翻转
        {
            if (priorValue == 0)
            {
                buff1[index1] |= 1u << offset1;     //lq 翻转为‘1’
                priorValue = 1;
            }
            else
            {
                buff1[index1] &= ~(1u << offset1);  //lq 翻转为‘0’                
                priorValue = 0;
            }
        }   
    }
    
    /*lq 整字节数和非整字节比特数均不变 */
}
#else
/*******************************************************************************
* 名称  : DLS_FrameNrziCode
* 描述  : NRZI编码（每个字节的编码顺序是从LSB至MSB，与送入物理链路的比特流顺序一
          致）
* 输入  : buff1 - 待NRZI编码数据（包括同步序列、帧头、数据、校验码和帧尾及位填充）
          initValue - NRZI初始值
* 输出  : buff2 - 已NRZI编码数据（包括同步序列、帧头、数据、校验码和帧尾及位填充）
* 返回  : 无
*******************************************************************************/
void DLS_FrameNrziCode(const U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], U8 buff2[DLS_FRAME_MAX_LEN_BYTE + 2], const U8 initValue)
{
    U16 index1;
    U8  offset1;
    U8  byteNum;
    U8  bitNum;
    U8  priorValue;
	
    byteNum = buff1[0];
    bitNum = buff1[1];
    priorValue = initValue;
     
    if (priorValue != 0) //lq 无效值保护
    {
        priorValue = 1;
    }

	memset((U8*)buff2, 0, (DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));

    /*lq 整字节 */
    for (index1 = 2; index1 < byteNum + 2; index1++)
    {
        for (offset1 = 0; offset1 < 8; offset1++)
        {                                  
            if ((buff1[index1] & (1u << offset1)) != 0)     //lq 当前位为‘1’, 需保持
            {                
                if (priorValue == 0)
                {
                    buff2[index1] &= (~(1u << offset1));    //lq 保持为‘0’                
                    priorValue = 0;                
                }
                else
                {
                    buff2[index1] |= (1u << offset1);       //lq 保持为‘1’
                    priorValue = 1;
                }
            }
            else                                            //lq 当前位为‘0’，需翻转
            {
                if (priorValue == 0)
                {
                    buff2[index1] |= (1u << offset1);       //lq 翻转为‘1’
                    priorValue = 1;
                }
                else
                {
                    buff2[index1] &= (~(1u << offset1));    //lq 翻转为‘0’                
                    priorValue = 0;
                }
            }
        }    
    }

    /*lq 非整字节比特 */
    for (offset1 = 0; offset1 < bitNum; offset1++)
    {
        if ((buff1[index1] & (1u << offset1)) != 0)     //lq 当前位为‘1’, 需保持
        {                
            if (priorValue == 0)
            {
                buff2[index1] &= (~(1u << offset1));    //lq 保持为‘0’                
                priorValue = 0;                
            }
            else
            {
                buff2[index1] |= (1u << offset1);       //lq 保持为‘1’
                priorValue = 1;
            }
        }
        else                                            //lq 当前位为‘0’，需翻转
        {
            if (priorValue == 0)
            {
                buff2[index1] |= (1u << offset1);       //lq 翻转为‘1’
                priorValue = 1;
            }
            else
            {
                buff2[index1] &= (~(1u << offset1));    //lq 翻转为‘0’                
                priorValue = 0;
            }
        }
    }
    
    /*lq 记录整字节数和非整字节比特数 */
    buff2[0] = buff1[0];
    buff2[1] = buff1[1]; 
}
#endif

#if 0
/*******************************************************************************
* 名称  : DLS_FrameNrziDecode
* 描述  : NRZI解码（每个字节的解码顺序是从LSB至MSB，与物理链路采样的比特流顺序一
          致）
* 输入  : buff1 - 待NRZI解码数据（包括同步序列、帧头、数据、校验码和帧尾及位填充）
          initValue - NRZI初始值
* 输出  : buff1 - 已NRZI解码数据（包括同步序列、帧头、数据、校验码和帧尾及位填充）
* 返回  : 无
*******************************************************************************/
void DLS_FrameNrziDecode(U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], const U8 initValue)
{
    U16 index1;
    U8  offset1;
    U8  byteNum;
    U8  bitNum;
    U8  priorValue;
	
    byteNum = buff1[0];
    bitNum = buff1[1];
    priorValue = initValue;
     
    if (priorValue != 0) //lq 无效值保护
    {
        priorValue = 1;
    }

    /*lq 整字节 */
    for (index1 = 2; index1 < byteNum + 2; index1++)
    {
        for (offset1 = 0; offset1 < 8; offset1++)
        {                                  
            if ((buff1[index1] & (1u << offset1)) == (priorValue << offset1))   //lq 保持, 当前位为‘1’
            {                
                priorValue = (buff1[index1] >> offset1) & 0x01;
                
                buff1[index1] |= 1u << offset1;                                 //lq 当前位为‘1’
            }
            else                                                                //lq 翻转, 当前位为‘0’
            {
                priorValue = (buff1[index1] >> offset1) & 0x01;
                
                buff1[index1] &= ~(1u << offset1);                              //lq 当前位为‘0’                     
            }
        }    
    }

    /*lq 非整字节比特 */
    for (offset1 = 0; offset1 < bitNum; offset1++)
    {                                  
        if ((buff1[index1] & (1u << offset1)) == (priorValue << offset1))   //lq 保持, 当前位为‘1’
        {                
            priorValue = (buff1[index1] >> offset1) & 0x01;
            
            buff1[index1] |= 1u << offset1;                                 //lq 当前位为‘1’
        }
        else                                                                //lq 翻转, 当前位为‘0’
        {
            priorValue = (buff1[index1] >> offset1) & 0x01;
            
            buff1[index1] &= ~(1u << offset1);                              //lq 当前位为‘0’                     
        }
    }
    
    /*lq 整字节数和非整字节比特数均不变 */
}
#else
/*******************************************************************************
* 名称  : DLS_FrameNrziDecode
* 描述  : NRZI解码（每个字节的解码顺序是从LSB至MSB，与物理链路采样的比特流顺序一
          致）
* 输入  : buff1 - 待NRZI解码数据（包括同步序列、帧头、数据、校验码和帧尾及位填充）
          initValue - NRZI初始值
* 输出  : buff2 - 已NRZI解码数据（包括同步序列、帧头、数据、校验码和帧尾及位填充）
* 返回  : 无
*******************************************************************************/
void DLS_FrameNrziDecode(const U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], U8 buff2[DLS_FRAME_MAX_LEN_BYTE + 2], const U8 initValue)
{
    U16 index1;
    U8  offset1;
    U8  byteNum;
    U8  bitNum;
    U8  priorValue;
	
    byteNum = buff1[0];
    bitNum = buff1[1];
    priorValue = initValue;
     
    if (priorValue != 0) //lq 无效值保护
    {
        priorValue = 1;
    }

	memset((U8*)buff2, 0, (DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));

    /*lq 整字节 */
    for (index1 = 2; index1 < byteNum + 2; index1++)
    {
        for (offset1 = 0; offset1 < 8; offset1++)
        {                                  
            if ((buff1[index1] & (1u << offset1)) == (priorValue << offset1))   //lq 保持, 当前位为‘1’
            {                
                priorValue = (buff1[index1] >> offset1) & 0x01;
                
                buff2[index1] |= (1u << offset1);                               //lq 当前位为‘1’
            }
            else                                                                //lq 翻转, 当前位为‘0’
            {
                priorValue = (buff1[index1] >> offset1) & 0x01;
                
                buff2[index1] &= (~(1u << offset1));                            //lq 当前位为‘0’                     
            }
        }    
    }

    /*lq 非整字节比特 */
    for (offset1 = 0; offset1 < bitNum; offset1++)
    {                                  
        if ((buff1[index1] & (1u << offset1)) == (priorValue << offset1))   //lq 保持, 当前位为‘1’
        {                
            priorValue = (buff1[index1] >> offset1) & 0x01;
            
            buff2[index1] |= (1u << offset1);                               //lq 当前位为‘1’
        }
        else                                                                //lq 翻转, 当前位为‘0’
        {
            priorValue = (buff1[index1] >> offset1) & 0x01;
            
            buff2[index1] &= (~(1u << offset1));                            //lq 当前位为‘0’                     
        }
    }
        
    /*lq 记录整字节数和非整字节比特数 */
    buff2[0] = buff1[0];
    buff2[1] = buff1[1]; 
}
#endif

/*******************************************************************************
* 名称  : DLS_FrameBuild
* 描述  : 组帧
* 输入  : buff1 - 原始数据
* 输出  : buff2 - 已组帧数据（包括同步序列、帧头、数据、校验码和帧尾及位填充，并
          且所有数据已进行NRZI）
* 返回  : 无
*******************************************************************************/
void DLS_FrameBuild(const U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], U8 buff2[DLS_FRAME_MAX_LEN_BYTE + 2])
{
    U8 *pBuff;

    pBuff = (U8 *)malloc((DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));
    memset((U8 *)pBuff, 0, (DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));

    DLS_FrameCrcCheck(buff1, pBuff);
    DLS_FrameBitStuff(pBuff, buff2);
    DLS_FrameDataPack(buff2, pBuff);
    DLS_FrameNrziCode(pBuff, buff2, 1);
    
    free((U8 *)pBuff);
}

/*******************************************************************************
* 名称  : DLS_FrameBuildTest
* 描述  : 组帧（同步序列、帧头和帧尾使用NRZI，数据和校验码不使用NRZI）
* 输入  : buff1 - 原始数据
* 输出  : buff2 - 已组帧数据（同步序列、帧头和帧尾使用NRZI，数据和校验码不使用NRZI）
* 返回  : 无
*******************************************************************************/
void DLS_FrameBuildTest(const U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], U8 buff2[DLS_FRAME_MAX_LEN_BYTE + 2])
{
    U8 *pBuff;

    pBuff = (U8 *)malloc((DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));
    memset((U8 *)pBuff, 0, (DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));

    DLS_FrameCrcCheck(buff1, buff2);
    DLS_FrameBitStuff(buff2, pBuff);
    DLS_FrameDataPack1(pBuff, buff2);   //lq 同步序列、帧头和帧尾使用NRZI，数据和校验码不使用NRZI
    
    free((U8 *)pBuff);
}

/*******************************************************************************
* 名称  : DLS_FrameBuildTest2
* 描述  : 组帧（同步序列、帧头和帧尾，数据和校验码都不使用NRZI）
* 输入  : buff1 - 原始数据
* 输出  : buff2 - 已组帧数据（同步序列、帧头和帧尾，数据和校验码都不使用NRZI）
* 返回  : 无
*******************************************************************************/
void DLS_FrameBuildTest2(const U8 buff1[DLS_FRAME_MAX_LEN_BYTE + 2], U8 buff2[DLS_FRAME_MAX_LEN_BYTE + 2])
{
    U8 *pBuff;

    pBuff = (U8 *)malloc((DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));
    memset((U8 *)pBuff, 0, (DLS_FRAME_MAX_LEN_BYTE + 2) * sizeof(U8));

    DLS_FrameCrcCheck(buff1, buff2);
    DLS_FrameBitStuff(buff2, pBuff);
    DLS_FrameDataPack(pBuff, buff2);
    
    free((U8 *)pBuff);
}

/*******************************************************************************
* 名称  : DLS_ModuleInit
* 描述  : 模块初始化
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void DLS_ModuleInit(void)
{
    DLS_InstFrameFifoStructInit();
    DLS_InstPacketProbeStructInit();
}

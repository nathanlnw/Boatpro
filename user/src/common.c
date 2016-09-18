/*******************************************************************************
  Copyright (C),  2011,  712.Co., Ltd.
  FileName: common.c
  Author: 王伟       Version : V1.00        Date: 2011.09.23
  Description:    公用程序
  Version:         V1.00
  Function List:   // 主要函数及其功能    	   	    	   	     	   	   	   	   
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include "def.h"
#include "common.h"
#include "stdlib.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//CRC16 Table
#if 0
const U16 guiCrctable_16[256] = {
0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0,
};
#endif

const U16 ais_crc16_table[256]={
		0x0000,0x8911,0x1223,0x9b32,0x2446,0xad57,0x3665,0xbf74,
		0x488c,0xc19d,0x5aaf,0xd3be,0x6cca,0xe5db,0x7ee9,0xf7f8,
		0x8110,0x0801,0x9333,0x1a22,0xa556,0x2c47,0xb775,0x3e64,
		0xc99c,0x408d,0xdbbf,0x52ae,0xedda,0x64cb,0xfff9,0x76e8,
		0x0221,0x8b30,0x1002,0x9913,0x2667,0xaf76,0x3444,0xbd55,
		0x4aad,0xc3bc,0x588e,0xd19f,0x6eeb,0xe7fa,0x7cc8,0xf5d9,
		0x8331,0x0a20,0x9112,0x1803,0xa777,0x2e66,0xb554,0x3c45,
		0xcbbd,0x42ac,0xd99e,0x508f,0xeffb,0x66ea,0xfdd8,0x74c9,
		0x0442,0x8d53,0x1661,0x9f70,0x2004,0xa915,0x3227,0xbb36,
		0x4cce,0xc5df,0x5eed,0xd7fc,0x6888,0xe199,0x7aab,0xf3ba,
		0x8552,0x0c43,0x9771,0x1e60,0xa114,0x2805,0xb337,0x3a26,
		0xcdde,0x44cf,0xdffd,0x56ec,0xe998,0x6089,0xfbbb,0x72aa,
		0x0663,0x8f72,0x1440,0x9d51,0x2225,0xab34,0x3006,0xb917,
		0x4eef,0xc7fe,0x5ccc,0xd5dd,0x6aa9,0xe3b8,0x788a,0xf19b,
		0x8773,0x0e62,0x9550,0x1c41,0xa335,0x2a24,0xb116,0x3807,
		0xcfff,0x46ee,0xdddc,0x54cd,0xebb9,0x62a8,0xf99a,0x708b,
		0x0884,0x8195,0x1aa7,0x93b6,0x2cc2,0xa5d3,0x3ee1,0xb7f0,
		0x4008,0xc919,0x522b,0xdb3a,0x644e,0xed5f,0x766d,0xff7c,
		0x8994,0x0085,0x9bb7,0x12a6,0xadd2,0x24c3,0xbff1,0x36e0,
		0xc118,0x4809,0xd33b,0x5a2a,0xe55e,0x6c4f,0xf77d,0x7e6c,
		0x0aa5,0x83b4,0x1886,0x9197,0x2ee3,0xa7f2,0x3cc0,0xb5d1,
		0x4229,0xcb38,0x500a,0xd91b,0x666f,0xef7e,0x744c,0xfd5d,
		0x8bb5,0x02a4,0x9996,0x1087,0xaff3,0x26e2,0xbdd0,0x34c1,
		0xc339,0x4a28,0xd11a,0x580b,0xe77f,0x6e6e,0xf55c,0x7c4d,
		0x0cc6,0x85d7,0x1ee5,0x97f4,0x2880,0xa191,0x3aa3,0xb3b2,
		0x444a,0xcd5b,0x5669,0xdf78,0x600c,0xe91d,0x722f,0xfb3e,
		0x8dd6,0x04c7,0x9ff5,0x16e4,0xa990,0x2081,0xbbb3,0x32a2,
		0xc55a,0x4c4b,0xd779,0x5e68,0xe11c,0x680d,0xf33f,0x7a2e,
		0x0ee7,0x87f6,0x1cc4,0x95d5,0x2aa1,0xa3b0,0x3882,0xb193,
		0x466b,0xcf7a,0x5448,0xdd59,0x622d,0xeb3c,0x700e,0xf91f,
		0x8ff7,0x06e6,0x9dd4,0x14c5,0xabb1,0x22a0,0xb992,0x3083,
		0xc77b,0x4e6a,0xd558,0x5c49,0xe33d,0x6a2c,0xf11e,0x780f,
};
/* Private  function  --------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/

/*******************************************************************************
* 名称  : HexToAscII
* 描述  : BCD码转ASCII码
* 输入  : Data：BCD码
* 输出  : 无
* 返回  : ASCII
*******************************************************************************/
U8 HexToAscII(U8 Data)
{
	U8 tmp;

	tmp = Data & 0x0f;

	if (tmp <= 9)		//0-9
	{
		tmp += '0';
	}
	else
	{
		tmp += 55;
	}
	
	return tmp;
}

/*******************************************************************************
* 名称  : DataToAscII
* 描述  : BCD码转ASCII码
* 输入  : pData：数据指针
		  Len：数据长度
		  pString：存储转换后的ASCII
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void DataToAscII(U16 *pData,  U8 Len,  char *pString)
{
	U8 i;

	for (i=0; i<Len; i++)
	{
		*pString++ = HexToAscII((U8)(pData[i] >> 12));
		*pString++ = HexToAscII((U8)(pData[i] >> 8));
		*pString++ = HexToAscII((U8)(pData[i] >> 4));
		*pString++ = HexToAscII((U8)(pData[i]));
	}
	*pString++ = '\0';
}

/*******************************************************************************
* 名称  : UartMsgCheckSum
* 描述  : 串口报文校验
* 输入  : pData：校验数据指针
		  Len：校验数据长度
* 输出  : 无
* 返回  : 校验和
*******************************************************************************/
U8 UartMsgCheckSum(U8 *pData,  U16 Len)
{
#if 1
	U8 i; 
	U8 tmpSum;
	
	tmpSum = 0;
	for (i = 0; i < Len; i++)
	{
		tmpSum ^= pData[i];
	}
	
	return tmpSum;
#else
	U16 i;
	U16 sum = 0;
	
	for (i = 0; i < Len; i++)
	{
		sum += pData[i];
	}
	
	sum = ~sum;
	sum += 1;
	
	return sum;
#endif
}

/*******************************************************************************
* 名称  : UartMsgCheckSum
* 描述  : 串口报文校验
* 输入  : pData：校验数据指针
		  Len：校验数据长度
* 输出  : 无
* 返回  : 校验和
*******************************************************************************/
U8 EEpromChkSum(U8 *pData,  U16 Len)
{
	U8 i; 
	U8 tmpSum = 0;
	
	for (i=0; i<Len; i++)
	{
		tmpSum += pData[i];
	}
	
	tmpSum = ~tmpSum;

	return tmpSum;
}

/*******************************************************************************
* 名称  : ChkSum
* 描述  : 校验和
* 输入  : pBuf：数据指针
		  size：数据长度
* 输出  : 无
* 返回  : 校验和
*******************************************************************************/
U16 ChkSum(U16 *pBuf,  U16 size)
{
	U16 i;
	U16 sum = 0;
	
	for (i=0; i<size; i++)
	{
		sum += pBuf[i];
	}
	
	sum = ~sum;
	sum += 1;
	
	return sum;
}

/*******************************************************************************
* 名称  : AIS_Cal_CRC16
* 描述  : CRC编码
* 输入  : data: 输入数据
		  count：数据长度
* 输出  : 无
* 返回  : 校验和
*******************************************************************************/ 
U16 AIS_Cal_CRC16(U8 *data, U16 count)
{
	U16 crc = 0x0;
	U16 i, tmp1, tmp2, tmp3;

	tmp2 = ~(data[0]);
	tmp3 = ~(data[1]);

	tmp1 = crc;
	crc = ais_crc16_table[(tmp1 >> 8) ^ (tmp2 & 0x00ff)];
	crc = crc ^ (tmp1 << 8);
	tmp1 = crc;
	crc = ais_crc16_table[(tmp1 >> 8) ^ (tmp3 & 0x00ff)];
	crc = crc ^ (tmp1 << 8);

	for (i = 2; i < count; i++)
	{
		tmp1 = crc;
		crc = ais_crc16_table[(tmp1 >> 8) ^ (data[i] & 0x00ff)];
		crc = crc ^ (tmp1 << 8);
	}
	crc = ~crc;

	return(crc);
}

#if 0
/************************************************************
*  Function:       Cal_Crc16								*
*  Description:    CRC16校验;字节型CRC计算；(查表法算CRC)	*
*				   生成多项式为0x1024						*
*				   即X^16+X^12+X^5+1						*
*  Input:          *ucStarAddr								*
*				   ucDataLen								*
*  Output:         无										*
*  Return:         uiCrcData								*
*  Others:         无										*
************************************************************/ 
U16 Cal_Crc16(U8 *ucStarAddr, U32 ucDataLen)		//ucStarAddr为需作crc校验的数组首地址，ucDataLen为crc校验的数据个数
{	
	U16 uicRc_16 = 0x0000;    // 初始化
    
    while(ucDataLen>0)
    {
        uicRc_16 = (uicRc_16 << 8) ^ guiCrctable_16[((uicRc_16>>8) ^ *ucStarAddr) & 0xff]; //guiCrctable_16表由函数mK_cRctable生成
        ucDataLen--;
        ucStarAddr++;
    }
    
    return uicRc_16; 
}
#endif
/*******************************************************************************
* 名称  : ByteToIntDataCopy
* 描述  : 字节类型转换为U16型
* 输入  : Dest：目的数据
		  Sour：源数据
		  Len：数据长度
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ByteToIntDataCopy(U16 *Dest, U8 *Sour, U8 Len)
{
	U8 i;
	
	for (i = 0; i < Len; i++)
	{
		Dest[i] = Sour[i];
	}
}

/*******************************************************************************
* 名称  : Delay_us
* 描述  : 延时函数
* 输入  : nTime：定义延时长度，单位为US
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void Delay_us(U32 nTime)
{
	U32 i,j;

	for (i = 0; i < nTime; i++)
	{
		for (j = 0; j < 10; j++)
		{
		   ;
		}				
	}
}

/*******************************************************************************
* Function Name  : delay_ms
* Description    : 延时函数
* Input          : nTime：定义延时长度，单位为ms
* Output         : None
* Return         : None
*******************************************************************************/
void Delay_ms(U32 nTime)
{
  	U32 i,j;

	for (i = 0; i < nTime; i++)
	{
		for (j = 0; j < 6466; j++)
		{
			;
		}
  	}
}

/*******************************************************************************
* 名称  : GetMaskBitNum8
* 描述  : 根据掩码值返回位序号（从0开始）
* 输入  : bitMask - 位掩码(位掩码中的1必须连续)	  
* 输出  : 无
* 返回  : 掩码值对应的位序号（即位掩码中最低位的1的位序号）
*******************************************************************************/
U8 GetMaskBitNum8(const U8 bitMask)
{
    U8 i;
       
    for (i = 0; i < 8; i++)
    {
        if (((bitMask >> i) & 0x01) != 0)
        {
            break;
        }
    } 
    
    return i;   
}

/*******************************************************************************
* 名称  : GetMaskBitNum16
* 描述  : 根据掩码值返回位序号（从0开始）
* 输入  : bitMask - 位掩码(位掩码中的1必须连续)	  
* 输出  : 无
* 返回  : 掩码值对应的位序号（即位掩码中最低位的1的位序号）
*******************************************************************************/
U8 GetMaskBitNum16(const U8 bitMask)
{
    U8 i;
       
    for (i = 0; i < 16; i++)
    {
        if (((bitMask >> i) & 0x0001) != 0)
        {
            break;
        }
    } 
    
    return i;   
}

/*******************************************************************************
* 名称  : GetMaskBitNum32
* 描述  : 根据掩码值返回位序号（从0开始）
* 输入  : bitMask - 位掩码(位掩码中的1必须连续)	  
* 输出  : 无
* 返回  : 掩码值对应的位序号（即位掩码中最低位的1的位序号）
*******************************************************************************/
U8 GetMaskBitNum32(const U32 bitMask)
{
    U8 i;
       
    for (i = 0; i < 32; i++)
    {
        if (((bitMask >> i) & 0x00000001) != 0)
        {
            break;
        }
    } 
    
    return i;   
}

/*******************************************************************************
* 名称  : GetWordFieldValue
* 描述  : 获取字的字段值
* 输入  : value - 寄存器值
          bitMask - 位掩码(位掩码中的1必须连续)	  
* 输出  : 无
* 返回  : 掩码值对应的字段值
*******************************************************************************/
U32 GetWordFieldValue(const U32 value, const U32 bitMask)
{ 
    return ((value & bitMask) >> (GetMaskBitNum32(bitMask)));   
}

/*******************************************************************************
* 名称  : SetWordFieldValue
* 描述  : 修改字的字段值
* 输入  : wordValue - 字的原始值
          bitMask - 位掩码(位掩码中的1必须连续)
          fieldValue - bitMask指定的字段的新值	  
* 输出  : 无
* 返回  : 修改字段后的字的新值
*******************************************************************************/
U32 SetWordFieldValue(U32 wordValue, U32 bitMask, U32 fieldValue)
{ 
    fieldValue <<= GetMaskBitNum32(bitMask);
    fieldValue &= bitMask;
    wordValue &= ~bitMask;

    return (wordValue | fieldValue);   
}

/*******************************************************************************
* 名称  : Random
* 描述  : 生成位于区间[min(bound1,bound2),max(bound1,bound2)]上的随机值，若bound1
          和bound2相等则返回rand()
* 输入  : bound1 - 随机数边界1
          bound2 - 随机数边界2
* 输出  : 无
* 返回  : 随机值
*******************************************************************************/
S32 Random(S32 bound1, S32 bound2) 
{
    S32 min;
    S32 max;
    S32 span;

    if (bound1 == bound2)
    {
        return rand();   
    }
    else if (bound1 < bound2)
    {
        min = bound1;
        max = bound2;
    }
    else
    {
        min = bound2;
        max = bound1;
    }
    
    span = max - min + 1;
    
    return (rand() % span) + min;       //lq 随机值转为区间[min, max]上的值
}


/*******************************************************************************
* 名称  : TIME_DateTimeStructInit
* 描述  : DateTimeStruct结构体初始化
* 输入  : pDateTimeStruct - 指向DateTimeStruct结构体的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TIME_DateTimeStructInit(DateTimeStruct *pDateTimeStruct) 
{
    pDateTimeStruct->Year = DATE_TIME_INVALID_VALUE_YEAR;
    pDateTimeStruct->Month = DATE_TIME_INVALID_VALUE_MONTH;
    pDateTimeStruct->Day = DATE_TIME_INVALID_VALUE_DAY;
    pDateTimeStruct->Hour = DATE_TIME_INVALID_VALUE_HOUR;
    pDateTimeStruct->Minute = DATE_TIME_INVALID_VALUE_MINUTE;
    pDateTimeStruct->Second = DATE_TIME_INVALID_VALUE_SECOND;
}

/*******************************************************************************
* 名称  : TIME_DateTimeStructCopy
* 描述  : DateTimeStruct结构体复制
* 输入  : pDst - 复制目的
          pSrc - 复制源
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void TIME_DateTimeStructCopy(DateTimeStruct * pDst, const DateTimeStruct *pSrc) 
{
    pDst->Year = pSrc->Year;
    pDst->Month = pSrc->Month;
    pDst->Day = pSrc->Day;
    pDst->Hour = pSrc->Hour;
    pDst->Minute = pSrc->Minute;
    pDst->Second = pSrc->Second;
}

/*******************************************************************************
* 名称  : TIME_IsDateValid
* 描述  : 日期是否有效
* 输入  : year - 年
          month - 月
          day - 日
* 输出  : 无
* 返回  : 是否有效
*******************************************************************************/
U8 TIME_IsDateValid(S16 year, S16 month, S16 day) 
{
    if (year < 0)
    {
        return FALSE;
    }

    if (DATE_TIME_IS_MONTH_31DAYS(month) == TRUE)
    {
        if ((day <= 0) || (day > 31))
        {
            return FALSE;
        }
    }
    else if (DATE_TIME_IS_MONTH_30DAYS(month) == TRUE)
    {
        if ((day <= 0) || (day > 30))
        {
            return FALSE;
        }
    }
    else if (month == 2)
    {
        if (DATE_TIME_IS_YEAR_LEAP(year) == TRUE)
        {
            if ((day <= 0) || (day > 29))
            {
                return FALSE;
            }            
        }
        else
        {
            if ((day <= 0) || (day > 28))
            {
                return FALSE;
            }                    
        }
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

/*******************************************************************************
* 名称  : TIME_IsDateTimeValid
* 描述  : 日期时间是否有效
* 输入  : pDateTimeStruct - 指向DateTimeStruct结构体的指针
* 输出  : 无
* 返回  : 是否有效
*******************************************************************************/
U8 TIME_IsDateTimeValid(const DateTimeStruct *pDateTimeStruct) 
{
    if (TIME_IsDateValid(pDateTimeStruct->Year,
        pDateTimeStruct->Month, pDateTimeStruct->Day) == FALSE)
    {
        return FALSE;
    }

    if (DATE_TIME_IS_HOUR_VALID(pDateTimeStruct->Hour) == FALSE)
    {
        return FALSE;
    }

    if (DATE_TIME_IS_MINUTE_VALID(pDateTimeStruct->Minute) == FALSE)
    {
        return FALSE;
    }

    if (DATE_TIME_IS_SECOND_VALID(pDateTimeStruct->Second) == FALSE)
    {
        return FALSE;
    }
    
    return TRUE;
}

/*******************************************************************************
* 名称  : TIME_DateTimeOffset28Days
* 描述  : 计算日期时间在加上偏移量后的新值
* 输入  : pDateTimeStruct - 指向DateTimeStruct结构体的指针
          second - 偏移量，单位秒，范围正负28天（含）以内
* 输出  : 无
* 返回  : 结果是否有效
*******************************************************************************/
U8 TIME_DateTimeOffset28Days(DateTimeStruct *pDateTimeStruct, S32 second) 
{
    S32 tempS32;
    S32 tempS32_1;
    S32 days;
    S32 seconds;

    if ((second > DATE_TIME_SECOND_28DAYS) || (second < -(DATE_TIME_SECOND_28DAYS)))
    {
        /*lq 28天 */
        return FALSE;
    }

    if (TIME_IsDateTimeValid(pDateTimeStruct) == FALSE)
    {
        return FALSE;
    }

    if (second > 0)
    {
        tempS32 = pDateTimeStruct->Second + second;
        pDateTimeStruct->Second = tempS32 % 60;
        if (tempS32 < 60)
        {
            return TRUE;    
        }

        tempS32 = pDateTimeStruct->Minute + (tempS32 / 60);
        pDateTimeStruct->Minute = tempS32 % 60;
        if (tempS32 < 60)
        {
            return TRUE;    
        }

        tempS32 = pDateTimeStruct->Hour + (tempS32 / 60);
        pDateTimeStruct->Hour = tempS32 % 24;
        if (tempS32 < 24)
        {
            return TRUE;    
        }

        tempS32 = pDateTimeStruct->Day + (tempS32 / 24);       
        if (DATE_TIME_IS_MONTH_31DAYS(pDateTimeStruct->Month) == TRUE)
        {
            days = 31;
        }
        else if (DATE_TIME_IS_MONTH_30DAYS(pDateTimeStruct->Month) == TRUE)
        {
            days = 30;
        }
        else
        {
            if (DATE_TIME_IS_YEAR_LEAP(pDateTimeStruct->Year) == TRUE)
            {
                days = 29;
            }
            else
            {
                days = 28;
            }
        }

        if (tempS32 <= days)
        {
            pDateTimeStruct->Day = tempS32;
        }
        else
        {
            pDateTimeStruct->Day = 1;
            tempS32 = pDateTimeStruct->Month + 1;       //lq 此处限制second的绝对值最大为28天
            if (tempS32 <= 12)
            {
                pDateTimeStruct->Month = tempS32;
            }
            else
            {
                pDateTimeStruct->Month = 1;
                pDateTimeStruct->Year = pDateTimeStruct->Year + 1;
            }
        }

        return TRUE;
    }
    else if (second < 0)
    {
        second = -second;
        
        days = second / 86400;      //lq 86400 = 24*60*60
        seconds = second % 86400;
        tempS32 = pDateTimeStruct->Second + pDateTimeStruct->Minute * 60
            + pDateTimeStruct->Hour * 3600;

        if (tempS32 >= seconds)
        {
            tempS32 = tempS32 - seconds;
            tempS32_1 = 0;
        }
        else
        {
            tempS32 = tempS32 - seconds + 86400;
            tempS32_1 = 1;
        }

        pDateTimeStruct->Hour = tempS32 / 3600;
        pDateTimeStruct->Minute = (tempS32 - (pDateTimeStruct->Hour * 3600)) / 60;
        pDateTimeStruct->Second = tempS32 - (pDateTimeStruct->Hour * 3600)
            - (pDateTimeStruct->Minute * 60);

        tempS32_1 = pDateTimeStruct->Day - days - tempS32_1;
        if (tempS32_1 <= 0)
        {
            pDateTimeStruct->Month--;
            if (pDateTimeStruct->Month <= 0)
            {
                pDateTimeStruct->Month = 12;
                pDateTimeStruct->Year--;
            }

            if (DATE_TIME_IS_MONTH_31DAYS(pDateTimeStruct->Month) == TRUE)
            {
                tempS32_1 += 31;
            }
            else if (DATE_TIME_IS_MONTH_30DAYS(pDateTimeStruct->Month) == TRUE)
            {
                tempS32_1 += 30;
            }
            else
            {
                if (DATE_TIME_IS_YEAR_LEAP(pDateTimeStruct->Year) == TRUE)
                {
                    tempS32_1 += 29;
                }
                else
                {
                    tempS32_1 += 28;
                }
            }
        } 

        pDateTimeStruct->Day = tempS32_1;
        
        return TRUE;           
    }

    return TRUE;
}

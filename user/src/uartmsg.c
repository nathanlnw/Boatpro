/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: uartmsg.c
  Author: vozhj      Version : V1.00        Date: 2012.03.21
  Description:    报文处理模块
  Version:         V1.00
  Function List:   // 主要函数及其功能
  History:         // 历史修改记录
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#include <locale.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <stdarg.h>
#include "config.h"
#include "def.h"
#include "initial.h"
#include "uartmsg.h"
#include "common.h"
#include "testing.h"
#if !WIN32
#include "debug.h"
#include "parastorage.h"
#include "powerup.h"
#endif
#include "gpsctrl.h"
#include "digunit.h"
#include "mmi.h"
#include "powerdown.h"
#include "LinkPtlPort.h"
#include "sleepctrl.h"
#include "Tim.h"
#include "24c02.h"
#include "ledctrl.h"
#include "DAC.h"
#include "tempersensor.h"
#include "PhysicalLayer.h"
#include "RxTxCtrl.h"
#include "Include.h"
#include "filectrl.h"
#include "flashctrl.h"

/*******************************************************************************
* 名称  : IniUartMsgPara
* 描述  : 初始化串口通信必要的参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void IniUartMsgPara(void)
{	

}

/*******************************************************************************
* 名称  : GetString
* 描述  : 获取报文中字段
* 输入  : pInput : 输入字符串
		  SegmentIndex: 提取字段n，第一个逗号之后的为第一字段
		  SumLength : 字符串总长度
* 输出  : Output : 输出字符串
* 返回  : 无
*******************************************************************************/
U16 GetString(char *pInput, char *Output, U16 SegmentIndex, U16 SumLength, U16 Max_Bytes)
{
	U16 i;
	U16 uiIndex;
	U16 uiTempSegment;
	U8 ucSta;
	
	uiIndex = 0;
	uiTempSegment = 0;
	ucSta = 0;

	for (i = 0; i < SumLength; i++)
	{
		//状态0，查找相关字段
		if (ucSta == 0)
		{
			if (pInput[i] == ',')
			{
				 uiTempSegment++;
	
				 //查找到所需提取的段
				 if (uiTempSegment == SegmentIndex)
				 {
				 	ucSta = 1;
				 }
				 else if (uiTempSegment > SegmentIndex)
				 {
					return (0);	
				 }
			}
			//到*位置仍不能满足条件，退出
			else if (pInput[i] == '*')
			{
				return (0);	
			}
		}
		//状态1，从相关字段中提取数据
		else if (ucSta == 1)
		{
			if (pInput[i] == ',' || pInput[i] == '*')
			{
				break;
			}
			else
			{
				Output[uiIndex++] = pInput[i];

				if (uiIndex >= Max_Bytes)
				{
					return (0);
				}
			}		
		}	
	}

	return (uiIndex);
}

/*******************************************************************************
* 名称  : IsAscStringOk
* 描述  : 判断ASC字符在0到9之间
* 输入  : 无
* 输出  : 无
* 返回  : TRUE：正确；FALSE：错误
*******************************************************************************/
U8 IsAscStringOk(char *pInput)
{
	U8 i, size;
	
	size = (U8)strlen(pInput);

	for (i = 0; i < size; i++)
	{
		if ((pInput[i] < '0') || (pInput[i] > '9'))
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

/*******************************************************************************
* 名称  : IsHexStringOk
* 描述  : 判断ASC字符在0到F之间
* 输入  : 无
* 输出  : 无
* 返回  : TRUE：正确；FALSE：错误
*******************************************************************************/
U8 IsHexStringOk(char *pInput)
{
	U8 i, size;
	
	size = (U8)strlen(pInput);

	for (i = 0; i < size; i++)
	{
		if (!(((pInput[i] >= '0') && (pInput[i] <= '9'))
            || ((pInput[i] >= 'A') && (pInput[i] <= 'F')) 
            || ((pInput[i] >= 'a') && (pInput[i] <= 'f'))))
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

/*******************************************************************************
* 名称  : IsVersionString
* 描述  : 判断ASC字符为数字和小数点
* 输入  : 无
* 输出  : 无
* 返回  : TRUE：正确；FALSE：错误
*******************************************************************************/
U8 IsVersionString(char *pInput)
{
	U8 i, size;
	
	size = (U8)strlen(pInput);

	for (i = 0; i < size; i++)
	{
		if (!(((pInput[i] >= '0') && (pInput[i] <= '9'))
            || ((pInput[i] == '.'))))
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

/*******************************************************************************
* 名称  : ConvertBcdToByte
* 描述  : BCD码转换为字节
* 输入  : pInput-BCD码（即HEX字符）字符串
* 输出  : pOutput-合并后的字节字符串
* 返回  : TRUE：正确；FALSE：错误
*******************************************************************************/
U8 ConvertBcdToByte(char *pInput, U8 *pOutput)
{
	U8 i, size, TempChar;
	
    if (!IsHexStringOk(pInput))
    {
        /*lq 含有非HEX字符*/
        return FALSE;
    }
    else
    {
	    size = (U8)strlen(pInput);
        for (i = 0; 2 * i < size; i++)
        {              
            /*lq 字节的低4位*/ 
            pOutput[i] &= 0;
            pOutput[i] = pInput[2 * i] & 0x0F;

            /*lq 字节的高4位*/            
            if (2 * i + 1 < size) 
            {
                TempChar = pInput[2 * i + 1] << 4;
                TempChar &= 0xF0;
                pOutput[i] |= TempChar;            
            }
        }                   
        pOutput[i] = '\0';

        return TRUE;    
    }
}

/*******************************************************************************
* 名称  : ConvertByteToBcd
* 描述  : 字节转换为BCD码
* 输入  : pInput-待转换字节字符串
* 输出  : pOutput-转换后的BCD码（即HEX字符）字符串
* 返回  : TRUE：正确；FALSE：错误
*******************************************************************************/
U8 ConvertByteToBcd(char *pInput, U8 *pOutput)
{
	U8 i, size, TempByte;
	
    size = (U8)strlen(pInput);
    for (i = 0; i < size; i++)
    {              
        /*lq 字节的低4位*/
        pOutput[2 * i] = pInput[i] & 0x0F;
        
        /*lq 字节的高4位*/
        TempByte = pInput[i] >> 4;
        TempByte &= 0x0F;
        pOutput[2 * i + 1] = TempByte;
    }                   
    pOutput[2 * i] = '\0';

    return TRUE;    
}

/*******************************************************************************
* 名称  : GetSegment
* 描述  : 获取报文中字段，并对报文中字段内容进行处理，转换为16进制数据
* 输入  : pInput : 输入字符串
		  SegmentIndex: 提取字段n，第一个逗号之后的为第一字段
		  SumLength : 字符串总长度
		  Max_Bytes：最大输出字节长度
* 输出  : Output：返回的二进制数据
* 返回  : 转换是否正确
*******************************************************************************/
U8 GetSegment(char *pInput, char *Output, U16 SegmentIndex, U16 SumLength, U16 Max_Bytes)
{
	U16 uiLen;
//	U8 ucCheck;

	uiLen = GetString(pInput, Output, SegmentIndex, SumLength, Max_Bytes);

	//成功获得字符串
	if (uiLen != 0)
	{
		Output[uiLen] = '\0';
		
		return (TRUE);
	}

	return (FALSE);
}

#if 0
/*******************************************************************************
* 名称  : GetHexData
* 描述  : 获取报文中字段，并对报文中字段内容进行处理，转换为16进制数据
* 输入  : pInput : 输入字符串
		  SegmentIndex: 提取字段n，第一个逗号之后的为第一字段
		  SumLength : 字符串总长度
* 输出  : Output：返回的二进制数据
* 返回  : 转换是否正确
*******************************************************************************/
U8 GetHexData(char *pInput, U32 *Output, U16 SegmentIndex, U16 SumLength)
{
	char chRetrunS1[33];
	U8 ucCheck;
	
	//字符校验正确
	ucCheck = GetSegment(pInput, chRetrunS1, SegmentIndex, SumLength, 32);

	if (ucCheck == TRUE)
	{
		ucCheck = IsAscStringOk(chRetrunS1);

		if (ucCheck == TRUE)
		{
			Output[0] = atol(chRetrunS1);
			return (TRUE);
		}	
	}

	return (FALSE);
}
#endif

/*******************************************************************************
* 名称  : SartMmsiFormCheck
* 描述  : 判断MMSI是否符合SART标准
* 输入  : 待检测字符串
* 输出  : 无
* 返回  : TRUE：正确；FALSE：错误
*******************************************************************************/
U8 SartMmsiFormCheck(char *pInput)
{
	U8 size;
	
	size = (U8)strlen(pInput);

    if (size != 9)
    {
        return FALSE;
    }

    if (pInput[0] == '9' && pInput[1] == '7' && pInput[2] == '0')
    {
        return TRUE;
    }
    else
    {
	    return FALSE;
    }
}

/*******************************************************************************
* 名称  : IsLatFormat
* 描述  : 判断是否是有效的纬度DDMM.mmmm
          整数部分长度固定为4，小数部分最多Precision位
* 输入  : pData-待检测字符串
          Precision-小数部分最大位数，即精度
* 输出  : pOut-单位为Precision的纬度
* 返回  : TRUE；有效；FALSE：无效
*******************************************************************************/
U8 IsLatFormat(char *pData, U32 *pOut, U8 Precision)
{
    char *pEnd;
    U8  ucLen;
    U8  i;
	U32 ulTemp1;
	U32 ulTemp2;
	U32 ulTemp3;
	U32 ulTemp4;

    ucLen = strlen(pData);
    
    /*lq 总长度有效性判断*/
    if (ucLen == 4 || (ucLen > 5 && ucLen <= 5 + Precision))
    {
        /*lq 整数部分字符有效性判断*/
        for (i = 0; i < 4; i++)    
        {
            if (!(pData[i] >= '0' && pData[i] <= '9'))
            {
                return FALSE;
            }
        }

        /*lq 纬度-度有效性判断 */
        ulTemp1 = (pData[0] - '0') * 10 + (pData[1] - '0');   //lq 度
        if (ulTemp1 > 90)
        {
    		return FALSE;                              
        }
    
        /*lq 分有效性判断 */
        ulTemp2 = (pData[2] - '0') * 10 + (pData[3] - '0');   //lq 分
        if (ulTemp2 >= 60)
        {
    		return FALSE;                              
        }
        
        ulTemp3 = ulTemp1 * 60 + ulTemp2;
    
        /*lq 按精度扩展*/
        for (i = 0; i < Precision; i++)
        {
            ulTemp3 *= 10;                                    //lq 精度
        }

        /*lq 小数部分处理*/
        ulTemp4 = 0;
        if (ucLen != 4)        
        {
            if (pData[4] != '.')                                
            {
        		return FALSE;                                   
            }
    
            /*lq 小数部分数值有效性判断*/
            for (i = 0; pData[5 + i] != '\0'; i++)
            {
                if ((pData[5 + i] >= '0' && pData[5 + i] <= '9'))
                {
        		    ulTemp4 = ulTemp4 * 10 + (pData[5 + i] - '0');
                }
                else
                {
                    return FALSE;                                   
                }
            } 
            
            for (; i < Precision; i++)
            {
                ulTemp4 *= 10;
            }       
        }

        /*lq 实际纬度值*/
        ulTemp3 = ulTemp3 + ulTemp4;
        
        /*lq 纬度转换为对应精度的最大值*/
        ulTemp4 = 90 * 60;
        for (i = 0; i < Precision; i++)
        {
    		ulTemp4 *= 10;                                   
        }

        /*lq 纬度有效性判断*/
        if (ulTemp3 > ulTemp4)
        {
            return FALSE;        
        }
        else
        {
            *pOut = ulTemp3;
            return TRUE;        
        }
    }
    else
    {
        return FALSE;
    }
}


/*******************************************************************************
* 名称  : IsLonFormat
* 描述  : 判断是否是有效的经度DDDMM.mmmm
          整数部分长度固定为5，小数部分最多Precision位
* 输入  : pData-待检测字符串
          Precision-小数部分最大位数，即精度
* 输出  : pOut-单位为Precision的经度
* 返回  : TRUE；有效；FALSE：无效
*******************************************************************************/
U8 IsLonFormat(char *pData, U32 *pOut, U8 Precision)
{
    char *pEnd;
    U8  ucLen;
    U8  i;
	U32 ulTemp1;
	U32 ulTemp2;
	U32 ulTemp3;
	U32 ulTemp4;

    ucLen = strlen(pData);
    
    /*lq 总长度有效性判断*/
    if (ucLen == 5 || (ucLen > 6 && ucLen <= 6 + Precision))
    {
        /*lq 整数部分字符有效性判断*/
        for (i = 0; i < 5; i++)    
        {
            if (!(pData[i] >= '0' && pData[i] <= '9'))
            {
                return FALSE;
            }
        }

        /*lq 经度-度有效性判断 */
        ulTemp1 = (pData[0] - '0') * 100 + (pData[1] - '0') * 10 + (pData[2] - '0');   //lq 度
        if (ulTemp1 > 180)
        {
    		return FALSE;                              
        }
    
        /*lq 分有效性判断 */
        ulTemp2 = (pData[3] - '0') * 10 + (pData[4] - '0');   //lq 分
        if (ulTemp2 >= 60)
        {
    		return FALSE;                              
        }
        
        ulTemp3 = ulTemp1 * 60 + ulTemp2;
    
        /*lq 按精度扩展*/
        for (i = 0; i < Precision; i++)
        {
            ulTemp3 *= 10;                                    //lq 精度
        }

        /*lq 小数部分处理*/
        ulTemp4 = 0;
        if (ucLen != 5)        
        {
            if (pData[5] != '.')                                
            {
        		return FALSE;                                   
            }
    
            /*lq 小数部分数值有效性判断*/
            for (i = 0; pData[6 + i] != '\0'; i++)
            {
                if ((pData[6 + i] >= '0' && pData[6 + i] <= '9'))
                {
        		    ulTemp4 = ulTemp4 * 10 + (pData[6 + i] - '0');
                }
                else
                {
                    return FALSE;                                   
                }
            } 
            
            for (; i < Precision; i++)
            {
                ulTemp4 *= 10;
            }       
        }

        /*lq 实际经度值*/
        ulTemp3 = ulTemp3 + ulTemp4;
        
        /*lq 经度转换为对应精度的最大值*/
        ulTemp4 = 180 * 60;
        for (i = 0; i < Precision; i++)
        {
    		ulTemp4 *= 10;                                   
        }

        /*lq 经度有效性判断*/
        if (ulTemp3 > ulTemp4)
        {
            return FALSE;        
        }
        else
        {
            *pOut = ulTemp3;
            return TRUE;        
        }
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : ToLatFormat
* 描述  : 将单位为1Precision的纬度转换为DDMM.mmmm格式
          整数部分长度固定为4，小数部分为Precision位
* 输入  : Data-单位为1Precision的纬度
          Precision-转换后小数部分位数，即精度
* 输出  : pOut-DDMM.mmmm格式的纬度
* 返回  : TRUE；有效；FALSE：无效
*******************************************************************************/
U8 ToLatFormat(U32 Data, U8 *pOut, U8 Precision)
{
    double MaxValue;
    double  lfTemp;
    U32 ulTemp;
    char RetBuf[30];
    char TempBuf[30];
    U8  i;

    MaxValue = 90 * 60;     //lq 精度为1分
    lfTemp = (double)Data;

    /*lq 转换为单位为1分*/
    for (i = 0; i < Precision; i++)
    {
        lfTemp /= 10;
    }

    if (lfTemp > MaxValue)
    {
        return FALSE;       //lq 按照指定的精度转换后为无效值
    }
    else
    {
        ulTemp = (U16)(lfTemp / 60);
        lfTemp -= (ulTemp * 60);

        sprintf(pOut, "%0*d", 2, ulTemp);
        if (Precision == 0)
        {
            sprintf(TempBuf, "%0*d", 2, (U16)lfTemp);        
        }
        else
        {
            sprintf(TempBuf, "%0*.*Lf", 3 + Precision, Precision, lfTemp);        
        }
        strcat(pOut, TempBuf);
        return TRUE;
    }
}

/*******************************************************************************
* 名称  : ToLonFormat
* 描述  : 将单位为1Precision的精度转换为DDDMM.mmmm格式
          整数部分长度固定为5，小数部分为Precision位
* 输入  : Data-单位为1Precision的经度
          Precision-转换后小数部分位数，即精度
* 输出  : pOut-DDDMM.mmmm格式的经度
* 返回  : TRUE；有效；FALSE：无效
*******************************************************************************/
U8 ToLonFormat(U32 Data, U8 *pOut, U8 Precision)
{
    double MaxValue;
    double  lfTemp;
    U32 ulTemp;
    char RetBuf[30];
    char TempBuf[30];
    U8  i;

    MaxValue = 180 * 60;     //lq 精度为1分
    lfTemp = (double)Data;

    /*lq 转换为单位为1分*/
    for (i = 0; i < Precision; i++)
    {
        lfTemp /= 10;
    }

    if (lfTemp > MaxValue)
    {
        return FALSE;       //lq 按照指定的精度转换后为无效值
    }
    else
    {
        ulTemp = (U16)(lfTemp / 60);
        lfTemp -= (ulTemp * 60);

        sprintf(pOut, "%0*d", 3, ulTemp);
        if (Precision == 0)
        {
            sprintf(TempBuf, "%0*d", 2, (U16)lfTemp);        
        }
        else
        {
            sprintf(TempBuf, "%0*.*Lf", 3 + Precision, Precision, lfTemp);        
        }
        strcat(pOut, TempBuf);
        return TRUE;
    }
}

#if 0
void IsLatValidTest(char *Lat)
{
    U8 i;
    U32 ulLat;
    U8 precision;
    // char Lat[10];

    precision = 4;
    // sprintf(Lat, "%s", "8959");
    // sprintf(Lat, "%s", "8959.");
    // sprintf(Lat, "%s", "900");

    for (i = 0; i < precision + 1; i++)
    {
        if (IsLatFormat(Lat, &ulLat, i) == TRUE)
        {
            usart_printf("Lat valid for precision of %d: %ld\r\n", i, ulLat);            
        }
        else
        {
            usart_printf("Lat invalid for precision of %d\r\n", i);
        }
    }
}

void IsLonValidTest(char *Lon)
{
    U8 i;
    U32 ulLon;
    U8 precision;
    // char Lat[10];

    precision = 4;
    // sprintf(Lat, "%s", "8959");
    // sprintf(Lat, "%s", "8959.");
    // sprintf(Lat, "%s", "900");

    for (i = 0; i < precision + 1; i++)
    {
        if (IsLonFormat(Lon, &ulLon, i) == TRUE)
        {
            usart_printf("Lon valid for precision of %d: %ld\r\n", i, ulLon);            
        }
        else
        {
            usart_printf("Lon invalid for precision of %d\r\n", i);
        }
    }
}

void ToLatFormatTest(U32 ulLon)
{
    U8 i;
    //U32 ulLon;
    U8 precision;
    char Lat[10];

    precision = 4;
    // sprintf(Lat, "%s", "8959");
    // sprintf(Lat, "%s", "8959.");
    // sprintf(Lat, "%s", "900");
    for (i = 0; i < precision + 1; i++)
    {
        if (ToLatFormat(ulLon, Lat, i) == TRUE)
        {
            usart_printf("Lon valid for precision of %d: %s\r\n", i, Lat);            
        }
        else
        {
            usart_printf("Lon invalid for precision of %d\r\n", i);
        }
    }
}

void ToLonFormatTest(U32 ulLon)
{
    U8 i;
    //U32 ulLon;
    U8 precision;
    char Lat[10];

    precision = 4;
    // sprintf(Lat, "%s", "8959");
    // sprintf(Lat, "%s", "8959.");
    // sprintf(Lat, "%s", "900");
    for (i = 0; i < precision + 1; i++)
    {
        if (ToLonFormat(ulLon, Lat, i) == TRUE)
        {
            usart_printf("Lon valid for precision of %d: %s\r\n", i, Lat);            
        }
        else
        {
            usart_printf("Lon invalid for precision of %d\r\n", i);
        }
    }
}
#endif
/*******************************************************************************
* 名称  : UartMsgProcess
* 描述  : 串口报文解析。
          当通过一个语句配置多个参数时，若某字段为空则忽略该字段，仅当某字段的取
          值无效时才认为配置失败（在其之前的配置成功）,若所有字段或者为空或者取值有
          效则配置成功。
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 UartMsgProcess(U8 *pDat, U16 uiLen)
{
#if !WIN32
	char chTemp[83];
    char chTemp2[83];
	U8 	ucTempBuf[40];
	U8 	ucCheck;
	U8 	ucLen;
//	U8	ucRetLen;
    S32 slTemp;
	U32 ulTemp;
	U32 ulTemp1;
	U32 ulTemp2;
	U32 ulTemp3;
	U32 ulTemp4;
    char *pEnd;
    char *pChar;
	U8 ucTemp;
	char chRetBuf[100];
    U8  ByteTemp[40];
    U8  ucSartFlg;
	U8	uiTempValue;
	U8	uiTempValue2;
    float sfTemp;
    double lfTemp;
    static U16 VerSeqMsgId = 0;
    U8  buffer[10];
    S16 i;
    AisChannelEnum channel;
	U32	ulDeviceIDBuff[11];

    /*lq AIS数字接口标准语句解析 */
    if (AISDI_SentenceParse(pDat, uiLen) == FALSE)
    {
        /*lq 已解析 */
        return (TRUE);
    }
    //===================================================================================
    /*lq P712开头，参数设置*/
	//  Process  type  1     :  note by Nathan
	else if (strncmp((char *)pDat, "P712", 4) == FALSE)
	{
		//获取第1字段内容，判断当前命令
		ucCheck = GetSegment((char *)pDat, chTemp, 1, uiLen, 30);

		if (ucCheck == FALSE)
		{
			return (FALSE);
		}
		if (DevDynamicPara.ucWorkMode == EQ_WORKMOD_NORMAL)
		{
			//MMSI号更新
			if (strncmp(chTemp, "010", 3) == FALSE)
			{
				//读取EEPROM中MMSI值，若为0，允许更新，否则不允许更新
				//Para_Read(PARA_MMSI_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_MMSI_TYPE, ucTempBuf);

				//MMSI号需要先复位再更新
				if (strncmp((char *)ucTempBuf, "000000000", 9) == FALSE)
				{
					//写入MMSI号，提取MMSI号
					ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);

					if (ucCheck == FALSE)
					{
						UartResponseMsg("$DUAIR,0,010");
						return (FALSE);
					}

					ucLen = strlen(chTemp);
					ucCheck = IsAscStringOk(chTemp);

#if CONFIG_MMSI_SARTFORM_CHECK
                    /*lq SART mmsi 格式检验*/
                    ucSartFlg = SartMmsiFormCheck(chTemp);
#else
                    ucSartFlg = TRUE;
#endif
					if ((ucLen != 9) || (ucCheck == FALSE) || (ucSartFlg == FALSE))
					{
						UartResponseMsg("$DUAIR,0,010");
						return (FALSE);
					}

					//写入MMSI号
					//Para_Save(PARA_MMSI_TYPE, (U8 *)chTemp, ucLen);
					FlashWrite(STM32_FLASH_MMSI_TYPE, (U8 *)chTemp,ucLen);
					//Para_Save(PARA_MMSI_BACKUP_TYPE, (U8 *)chTemp, ucLen);//同时写备份
					FlashWrite(STM32_FLASH_MMSI_BACKUP_TYPE, (U8 *)chTemp,ucLen);

					//此处增加更新协议栈接口
					AisStaticPara.MMSI = atol(chTemp);
					AisStaticPara.Refresh = TRUE;

					UartResponseMsg("$DUAIR,1,010");
				}
				else
				{
					UartResponseMsg("$DUAIR,0,010");
					return (FALSE);
				}
			}
			//复位MMSI号
			else if (strncmp(chTemp, "013", 3) == FALSE)
			{
				strncpy(&chTemp[0],"000000000",10);
				
                /*lq 复位MMSI*/
				//Para_Save(PARA_MMSI_TYPE, (U8 *)chTemp, 9);
				FlashWrite(STM32_FLASH_MMSI_TYPE, (U8 *)chTemp,9);
				AisStaticPara.MMSI = atol(chTemp);
#if 0
                /*lq 复位目的MMSI*/
				Para_Save(PARA_MMSI_DEST_TYPE, (U8 *)chTemp, 9);
				siStaticPara.DestMMSI = atol(chTemp);
#endif
				AisStaticPara.Refresh = TRUE;
				UartResponseMsg("$DUAIR,1,013");	
			}
			//序列号写入
			else if (strncmp(chTemp, "SNW", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
			
				ucLen = strlen(chTemp);

				if ((ucCheck == FALSE) || (ucLen != 8))
				{
					UartResponseMsg("$P712,0,SNW");
					return (FALSE);
				}

				//写入序列号
				//Para_Save(PARA_SERIAL_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_SERIAL_TYPE, (U8 *)chTemp,ucLen);

				UartResponseMsg("$P712,1,SNW");
				return (TRUE);
			}

			//厂商信息配置
			else if (strncmp(chTemp, "SRM", 3) == FALSE)
			{
				//***************************VendID*******************************//
				//写入VendID
				ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);
	
				if (ucCheck == FALSE)
				{
					UartResponseMsg("$DUAIR,0,SRM");
					return (FALSE);
				}
	
				ucLen = strlen(chTemp);
	
				if (ucLen != 7)
				{
					UartResponseMsg("$DUAIR,0,SRM");
					return (FALSE);
				}
	
				//写入VendID
				//Para_Save(PARA_VENDID_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_VENDID_TYPE, (U8 *)chTemp,ucLen);
	
				//此处增加更新协议栈接口
				//lq strncpy((char *)&siStaticPara.VendID[0], chTemp, ucLen);
				strcpy((char *)&AisStaticPara.VendID[0], chTemp);                    //lq 同时复制‘\0’
		
				AisStaticPara.Refresh = TRUE;
				UartResponseMsg("$DUAIR,1,SRM");		
			}
			//GPS串口语句输出状态
			else if (strncmp(chTemp, "GPSDATA", 7) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);				
				
				if (ucCheck == FALSE)
				{
					UartResponseMsg("$DUAIR,0,GPS");
					return (FALSE);	
				}

				ucCheck = IsAscStringOk(chTemp);

				if (ucCheck == FALSE)
				{
					UartResponseMsg("$DUAIR,0,GPS");
					return (FALSE);
				}

				if (chTemp[0] == '0')
				{
					DevDynamicPara.ucGpsOutput = FALSE;
                    DEVINFO_InstResetGpsOutputCnt();                
					UartResponseMsg("$DUAIR,1,GPS1");	
				}
				else if (chTemp[0] == '1')
				{
					DevDynamicPara.ucGpsOutput = TRUE;	
                    DEVINFO_InstResetGpsOutputCnt();                
					UartResponseMsg("$DUAIR,1,GPS0");
				}
				else
				{
					UartResponseMsg("$DUAIR,0,GPS");	
				}	
			}

			/*lq GPS串口输出频率配置 */
			else if (strncmp(chTemp, "GPSOP", 5) == FALSE)
			{
			    /*lq 读取第2字段的内容 */
                ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
				if (ucCheck == TRUE)                                            //lq 若字段空，则不进行该项的配置
				{
                    /*lq 长度有效性判断 */
                    ucLen = strlen(chTemp);
            		if (ucLen > 3)
            		{
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
            
            		/*lq 字符有效性判断 */
                    ulTemp = strtoul(chTemp, &pEnd, 10);
            		if ((*pEnd) != 0)
            		{
            		    /*lq 字符无效 */
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
                    else
                    {
                        /*lq 取值范围有效性判断 */
                        if (ulTemp < DEV_INFO_GPS_OUTPUT_INTERVAL_MIN
                            || ulTemp > DEV_INFO_GPS_OUTPUT_INTERVAL_MAX)
                        {
            		        /*lq 取值无效 */
            				UartResponseMsg("$DUAIR,0,GPSOP");
                			return (FALSE);
                        }
                    }

                    DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_RMC] = ulTemp;
				}
                
			    /*lq 读取第3字段的内容 */
                ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);
				if (ucCheck == TRUE)                                            //lq 若字段空，则不进行该项的配置
				{
                    /*lq 长度有效性判断 */
                    ucLen = strlen(chTemp);
            		if (ucLen > 3)
            		{
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
            
            		/*lq 字符有效性判断 */
                    ulTemp = strtoul(chTemp, &pEnd, 10);
            		if ((*pEnd) != 0)
            		{
            		    /*lq 字符无效 */
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
                    else
                    {
                        /*lq 取值范围有效性判断 */
                        if (ulTemp < DEV_INFO_GPS_OUTPUT_INTERVAL_MIN
                            || ulTemp > DEV_INFO_GPS_OUTPUT_INTERVAL_MAX)
                        {
            		        /*lq 取值无效 */
            				UartResponseMsg("$DUAIR,0,GPSOP");
                			return (FALSE);
                        }
                    }

                    DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_VTG] = ulTemp;
				}

			    /*lq 读取第4字段的内容 */
                ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);
				if (ucCheck == TRUE)                                            //lq 若字段空，则不进行该项的配置
				{
                    /*lq 长度有效性判断 */
                    ucLen = strlen(chTemp);
            		if (ucLen > 3)
            		{
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
            
            		/*lq 字符有效性判断 */
                    ulTemp = strtoul(chTemp, &pEnd, 10);
            		if ((*pEnd) != 0)
            		{
            		    /*lq 字符无效 */
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
                    else
                    {
                        /*lq 取值范围有效性判断 */
                        if (ulTemp < DEV_INFO_GPS_OUTPUT_INTERVAL_MIN
                            || ulTemp > DEV_INFO_GPS_OUTPUT_INTERVAL_MAX)
                        {
            		        /*lq 取值无效 */
            				UartResponseMsg("$DUAIR,0,GPSOP");
                			return (FALSE);
                        }
                    }

                    DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GGA] = ulTemp;
				}
                
			    /*lq 读取第5字段的内容 */
                ucCheck = GetSegment((char *)pDat, chTemp, 5, uiLen, 30);
				if (ucCheck == TRUE)                                            //lq 若字段空，则不进行该项的配置
				{
                    /*lq 长度有效性判断 */
                    ucLen = strlen(chTemp);
            		if (ucLen > 3)
            		{
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
            
            		/*lq 字符有效性判断 */
                    ulTemp = strtoul(chTemp, &pEnd, 10);
            		if ((*pEnd) != 0)
            		{
            		    /*lq 字符无效 */
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
                    else
                    {
                        /*lq 取值范围有效性判断 */
                        if (ulTemp < DEV_INFO_GPS_OUTPUT_INTERVAL_MIN
                            || ulTemp > DEV_INFO_GPS_OUTPUT_INTERVAL_MAX)
                        {
            		        /*lq 取值无效 */
            				UartResponseMsg("$DUAIR,0,GPSOP");
                			return (FALSE);
                        }
                    }

                    DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSA] = ulTemp;
				}
                
			    /*lq 读取第6字段的内容 */
                ucCheck = GetSegment((char *)pDat, chTemp, 6, uiLen, 30);
				if (ucCheck == TRUE)                                            //lq 若字段空，则不进行该项的配置
				{
                    /*lq 长度有效性判断 */
                    ucLen = strlen(chTemp);
            		if (ucLen > 3)
            		{
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
            
            		/*lq 字符有效性判断 */
                    ulTemp = strtoul(chTemp, &pEnd, 10);
            		if ((*pEnd) != 0)
            		{
            		    /*lq 字符无效 */
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
                    else
                    {
                        /*lq 取值范围有效性判断 */
                        if (ulTemp < DEV_INFO_GPS_OUTPUT_INTERVAL_MIN
                            || ulTemp > DEV_INFO_GPS_OUTPUT_INTERVAL_MAX)
                        {
            		        /*lq 取值无效 */
            				UartResponseMsg("$DUAIR,0,GPSOP");
                			return (FALSE);
                        }
                    }

                    DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSV] = ulTemp;
				}
                
			    /*lq 读取第7字段的内容 */
                ucCheck = GetSegment((char *)pDat, chTemp, 7, uiLen, 30);
				if (ucCheck == TRUE)                                            //lq 若字段空，则不进行该项的配置
				{
                    /*lq 长度有效性判断 */
                    ucLen = strlen(chTemp);
            		if (ucLen > 3)
            		{
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
            
            		/*lq 字符有效性判断 */
                    ulTemp = strtoul(chTemp, &pEnd, 10);
            		if ((*pEnd) != 0)
            		{
            		    /*lq 字符无效 */
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
                    else
                    {
                        /*lq 取值范围有效性判断 */
                        if (ulTemp < DEV_INFO_GPS_OUTPUT_INTERVAL_MIN
                            || ulTemp > DEV_INFO_GPS_OUTPUT_INTERVAL_MAX)
                        {
            		        /*lq 取值无效 */
            				UartResponseMsg("$DUAIR,0,GPSOP");
                			return (FALSE);
                        }
                    }

                    DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GLL] = ulTemp;
				}
                
			    /*lq 读取第8字段的内容 */
                ucCheck = GetSegment((char *)pDat, chTemp, 8, uiLen, 30);
				if (ucCheck == TRUE)                                            //lq 若字段空，则不进行该项的配置
				{
                    /*lq 长度有效性判断 */
                    ucLen = strlen(chTemp);
            		if (ucLen > 3)
            		{
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
            
            		/*lq 字符有效性判断 */
                    ulTemp = strtoul(chTemp, &pEnd, 10);
            		if ((*pEnd) != 0)
            		{
            		    /*lq 字符无效 */
        				UartResponseMsg("$DUAIR,0,GPSOP");
            			return (FALSE);
            		}
                    else
                    {
                        /*lq 取值范围有效性判断 */
                        if (ulTemp < DEV_INFO_GPS_OUTPUT_INTERVAL_MIN
                            || ulTemp > DEV_INFO_GPS_OUTPUT_INTERVAL_MAX)
                        {
            		        /*lq 取值无效 */
            				UartResponseMsg("$DUAIR,0,GPSOP");
                			return (FALSE);
                        }
                    }

                    DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GBS] = ulTemp;
				}
                
                DEVINFO_InstResetGpsOutputCnt();                
        	    UartResponseMsg("$DUAIR,1,GPSOP");
            }			

			//GPS同步
			else if (strncmp(chTemp, "SYNCH", 5) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);				
				
				if (ucCheck == FALSE)
				{
					UartResponseMsg("$DUAIR,0,SYNCH");
					return (FALSE);	
				}

				ucCheck = IsAscStringOk(chTemp);

				if (ucCheck == FALSE)
				{
					UartResponseMsg("$DUAIR,0,SYNCH");
					return (FALSE);
				}

				if (chTemp[0] == '0')
				{
					GpsSynCell.GPSSyn_Enable = FALSE;
					UartResponseMsg("$DUAIR,1,SYNCH0");	
				}
				else if (chTemp[0] == '1')
				{
					GpsSynCell.GPSSyn_Enable = TRUE;	
					UartResponseMsg("$DUAIR,1,SYNCH1");
				}
				else
				{
					UartResponseMsg("$DUAIR,0,SYNCH");	
				}	
			}
#if 0
			//复位正常开机次数
			else if (strncmp(chTemp, "RNPOC", 5) == FALSE)
			{
                sprintf(chTemp, "%2d", 0);
				Para_Save(PARA_NORMAL_POWER_ON_CNT_TYPE, (U8 *)chTemp, 2);

				UartResponseMsg("$DUAIR,1,RNPOC");  						
			}

			//复位用户测试次数
			else if (strncmp(chTemp, "RSTC", 4) == FALSE)
			{
                sprintf(chTemp, "%2d", 0);
				Para_Save(PARA_USER_TEST_CNT_TYPE, (U8 *)chTemp, 2);

				UartResponseMsg("$DUAIR,1,RSTC");  						
			}
#endif
			//复位工作总时长
			else if (strncmp(chTemp, "RWTS", 4) == FALSE)
			{
                sprintf(chTemp, "%5d", 0);
				Para_Save(PARA_WORKTIME_TYPE, (U8 *)chTemp, 5);

				UartResponseMsg("$DUAIR,1,RWTS");  						
			}

			//复位上次开机工作时长
			else if (strncmp(chTemp, "RLWT", 4) == FALSE)
			{
                sprintf(chTemp, "%5d", 0);
				Para_Save(PARA_WORKTIME_LAST_TYPE, (U8 *)chTemp, 5);

				UartResponseMsg("$DUAIR,1,RLWT");  						
			}

            //设置硬件版本号
			else if (strncmp(chTemp, "HWF", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
			
				ucLen = strlen(chTemp);

				if ((ucCheck == FALSE) || (ucLen > STM32_FLASH_HWVERSION_MAXLEN))
				{
					UartResponseMsg("$DUAIR,0,HWF");
					return (FALSE);
				}

                if (IsVersionString(chTemp) == FALSE)
                {
					UartResponseMsg("$DUAIR,0,HWF");
					return (FALSE);                       
                }

				//写入硬件版本号
				//Para_Save(PARA_HWVERSION_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_HWVERSION_TYPE, (U8 *)chTemp,ucLen);
				UartResponseMsg("$DUAIR,1,HWF");
				
                return (TRUE);  						
			}

			/*lq AIS类别配置 */
			else if (strncmp(chTemp, "AISCLASS", 8) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);//读取第二字段的内容

				if (ucCheck == FALSE)
				{
    				/*lq 字段为空 */
                    UartResponseMsg("$DUAIR,0,AISCLASS");
					return (FALSE);
				}
                
                /*lq 长度有效性判断 */
                ucLen = strlen(chTemp);
        		if (ucLen != 1)
        		{
    				UartResponseMsg("$DUAIR,0,AISCLASS");
        			return (FALSE);
        		}
        
        		/*lq 字符有效性判断 */
                ulTemp = strtoul(chTemp, &pEnd, 10);
        		if ((*pEnd) != 0)
        		{
        		    /*lq 字符无效 */
    				UartResponseMsg("$DUAIR,0,AISCLASS");
        			return (FALSE);
        		}
                else
                {
                    /*lq 取值范围有效性判断 */
                    if (ulTemp < STM32_FLASH_AIS_CLASS_FLOOR
                        || ulTemp > STM32_FLASH_AIS_CLASS_CEIL)
                    {
        		        /*lq 取值无效 */
        				UartResponseMsg("$DUAIR,0,AISCLASS");
            			return (FALSE);
                    }
                }
                
                /*lq 写入EEPROM */
                //Para_Save(PARA_AIS_CLASS_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_AIS_CLASS_TYPE, (U8 *)chTemp,ucLen);
        	    UartResponseMsg("$DUAIR,1,AISCLASS");
            }			

			/*lq Transceiver配置 */
			else if (strncmp(chTemp, "TR", 2) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);//读取第二字段的内容

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}	
				/*lq Transceiver （SI446X）状态配置 */
				else if (strncmp(chTemp, "006", 3) == FALSE)
				{
    				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);//读取第3字段的内容    
    				if (ucCheck == FALSE)
    				{
        				UartResponseMsg("$DUAIR,0,TR,006");    					
                        return (FALSE);
    				}

                    /*lq 长度有效性判断*/
                    ucLen = strlen(chTemp);
        			if (ucLen != 1)
        			{
        				UartResponseMsg("$DUAIR,0,TR,006");
        				return (FALSE);
        			}
        
        			/*lq 字符有效性判断*/
                    ucCheck = IsAscStringOk(chTemp);
        			if (ucCheck == FALSE)
        			{
        				UartResponseMsg("$DUAIR,0,TR,006");
        				return (FALSE);
        			}
                    else
                    {
                        /*lq 取值范围有效性判断*/
                        ulTemp = strtoul(chTemp, &pEnd, 10);
                        if (ulTemp == 1)
                        {
            				ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);//读取第4字段的内容    
            				if (ucCheck == FALSE)
            				{
                				UartResponseMsg("$DUAIR,0,TR,006");    					
                                return (FALSE);
            				}
        
                            /*lq 长度有效性判断*/
                            ucLen = strlen(chTemp);
                			if (ucLen != 1)
                			{
                				UartResponseMsg("$DUAIR,0,TR,006");
                				return (FALSE);
                			}
                
                			/*lq 字符有效性判断*/
                            ucCheck = IsAscStringOk(chTemp);
                			if (ucCheck == FALSE)
                			{
                				UartResponseMsg("$DUAIR,0,TR,006");
                				return (FALSE);
                			}
                            else
                            {
                                /*lq 取值范围有效性判断*/
                                ulTemp = strtoul(chTemp, &pEnd, 10);
                                if ((ulTemp >= 0) && (ulTemp <= 8))
                                {
                                    SI446X_CHANGE_STATE(ulTemp, SI446X_CHIP_A);
                                    UartResponseMsg("$DUAIR,1,TR,006");         
                                }
                                else
                                {
                 			        UartResponseMsg("$DUAIR,0,TR,006");                                        
                                }
                            }
                        }
                        else if (ulTemp == 2)
                        {
            				ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);//读取第4字段的内容    
            				if (ucCheck == FALSE)
            				{
                				UartResponseMsg("$DUAIR,0,TR,006");    					
                                return (FALSE);
            				}
        
                            /*lq 长度有效性判断*/
                            ucLen = strlen(chTemp);
                			if (ucLen != 1)
                			{
                				UartResponseMsg("$DUAIR,0,TR,006");
                				return (FALSE);
                			}
                
                			/*lq 字符有效性判断*/
                            ucCheck = IsAscStringOk(chTemp);
                			if (ucCheck == FALSE)
                			{
                				UartResponseMsg("$DUAIR,0,TR,006");
                				return (FALSE);
                			}
                            else
                            {
                                /*lq 取值范围有效性判断*/
                                ulTemp = strtoul(chTemp, &pEnd, 10);
                                if ((ulTemp >= 0) && (ulTemp <= 8))
                                {
                                    SI446X_CHANGE_STATE(ulTemp, SI446X_CHIP_B);
                                    UartResponseMsg("$DUAIR,1,TR,006");         
                                }
                                else
                                {
                 			        UartResponseMsg("$DUAIR,0,TR,006");                                        
                                }
                            }          
                        }
                        else
                        {
            				UartResponseMsg("$DUAIR,0,TR,006");
            				return (FALSE);                             
                        }
                    }                                                       
                }

				/*lq Transceiver （SI446X）输出功率配置（暂未写入EEPROM） */
				else if (strncmp(chTemp, "007", 3) == FALSE)
				{
    				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);//读取第3字段的内容    
    				if (ucCheck == FALSE)
    				{
        				UartResponseMsg("$DUAIR,0,TR,007");    					
                        return (FALSE);
    				}

                    /*lq 长度有效性判断 */
                    ucLen = strlen(chTemp);
            		if (ucLen != 1)
            		{
        				UartResponseMsg("$DUAIR,0,TR,007");
            			return (FALSE);
            		}
            
            		/*lq 字符有效性判断 */
                    ulTemp = strtoul(chTemp, &pEnd, 10);
            		if ((*pEnd) != 0)
            		{
            		    /*lq 字符无效 */
        				UartResponseMsg("$DUAIR,0,TR,007");
            			return (FALSE);
            		}

                    /*lq 取值范围有效性判断 */
                    if ((ulTemp < 1) || (ulTemp > 2))
                    {
            		    /*lq 取值无效 */
        				UartResponseMsg("$DUAIR,0,TR,007");
            			return (FALSE);
                    }

    				ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);//读取第4字段的内容    
    				if (ucCheck == FALSE)
    				{
        				UartResponseMsg("$DUAIR,0,TR,007");    					
                        return (FALSE);
    				}

                    /*lq 长度有效性判断*/
                    ucLen = strlen(chTemp);
        			if ((ucLen < 1) || (ucLen > 3))
        			{
        				UartResponseMsg("$DUAIR,0,TR,007");
        				return (FALSE);
        			}

            		/*lq 字符有效性判断 */
                    ulTemp1 = strtoul(chTemp, &pEnd, 10);
            		if ((*pEnd) != 0)
            		{
            		    /*lq 字符无效 */
        				UartResponseMsg("$DUAIR,0,TR,007");
            			return (FALSE);
            		}

                    /*lq 取值范围有效性判断 */
                    if ((ulTemp1 < 1) || (ulTemp1 > 127))
                    {
            		    /*lq 取值无效 */
        				UartResponseMsg("$DUAIR,0,TR,007");
            			return (FALSE);
                    }

                    if (ulTemp == 1)
                    {
                        SI446X_SetSi446xPaPwrLvl(ulTemp1, SI446X_CHIP_A);
                    }
                    else
                    {
                        SI446X_SetSi446xPaPwrLvl(ulTemp1, SI446X_CHIP_B);
                    }

                    UartResponseMsg("$DUAIR,1,TR,007");
                }

			}

			/*lq 调试输出配置 */
			else if (strncmp(chTemp, "DBO", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);//读取第二字段的内容

				if (ucCheck == FALSE)
				{
    				/*lq 字段为空 */
                    UartResponseMsg("$DUAIR,0,DBO");
					return (FALSE);
				}
                
                /*lq 长度有效性判断 */
                ucLen = strlen(chTemp);
        		if (ucLen != 1)
        		{
    				UartResponseMsg("$DUAIR,0,DBO");
        			return (FALSE);
        		}
        
        		/*lq 字符有效性判断 */
                ulTemp = strtoul(chTemp, &pEnd, 10);
        		if ((*pEnd) != 0)
        		{
        		    /*lq 字符无效 */
    				UartResponseMsg("$DUAIR,0,DBO");
        			return (FALSE);
        		}
                else
                {
                    /*lq 取值范围有效性判断 */
                    if (ulTemp < 0
                        || ulTemp > 1)
                    {
        		        /*lq 取值无效 */
        				UartResponseMsg("$DUAIR,0,DBO");
            			return (FALSE);
                    }
                }
                
                /*lq 执行配置 */
                if (ulTemp == 1)
                {
                    DebugOutputFlag = TRUE;
                }
                else
                {
                    DebugOutputFlag = FALSE;                    
                }
        	    UartResponseMsg("$DUAIR,1,DBO");
            }			

            //恢复出厂设置
			else if (strncmp(chTemp, "RST", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);	
				}

				if (strncmp(chTemp, "%%Reset", 7) == FALSE)//指令有效
				{
					ucCheck = EraseAllData();

					if (ucCheck == TRUE)
					{
						UartResponseMsg("$DUAIR,RST,1");
						UartResponseMsg("$712,ResetFinish........");
					}
					else
					{
						UartResponseMsg("$DUAIR,RST,0");
					}
				}
			}
					

			/**********************************************************************/
			/**********************************************************************/
			/**********************************************************************/
			/**********************************************************************/
			/**********************************************************************/

			else if (strncmp(chTemp, "050", 3) == FALSE)//可配置参数部分
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);//读取第二字段的内容

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}	
                //未写完
			
			}
			
			//进入测试态
			else if (strncmp(chTemp, "007", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}

				// 表示进入测试态
				if (strncmp(chTemp, "%%Test", 6) == FALSE)
				{
					//进入测试态
					DevDynamicPara.ucWorkMode = EQ_WORKMOD_TEST;
                    DevDynamicPara.TestSta = EQ_MOD_TEST_STA_MAIN;

                    /*lq 测试模式LED初始化 */    
            		LEDInit(LED_TYPE_NONE);
            		LEDInit(LED_TYPE_POWER_TEST);
    
					UartResponseMsg("$P712,Testing........");
				}
			}
#if CONFIG_SD_CARD_MODULE_EN
			//进入读取SD卡文件状态
			else if (strncmp(chTemp, "008", 3) == FALSE)
			{ 
				ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}

				// 表示进入读取SD卡文件状态
				if (strncmp(chTemp, "%%ReadFile", 10) == FALSE)
				{
					//进入读取SD卡文件状态
					DevDynamicPara.ucWorkMode = EQ_WORKMOD_READFILE;

                    /* 读取SD卡文件模式LED初始化 */ 
					InitiaLED();					

					/* 控制AIS和GPS是否使能 */
					CtrlAISAndGPS(0);												 // AIS和GPS无效
										
					UartResponseMsg("$P712,ReadFile........");
					Delay_ms(25);														//25ms的时延

					/* 将串口1的波特率改为115200 */  
    				InitCommStruc(1, 115200); 					
				}			
			}
#endif
			
			
			else if (strncmp(chTemp, "ReadID", 6) == FALSE)
			{ 
				
				ReadDeviceID(ulDeviceIDBuff);
				Feed_Dog();
				sprintf(chTemp2,"$P712,DeviceID1");
				for(ucTemp = 0;ucTemp < 5;ucTemp++)
				{ 
					sprintf(chTemp, ",%d",ulDeviceIDBuff[ucTemp]);		
					strcat(chTemp2, chTemp);					
				}
				UartResponseMsg(chTemp2);
				sprintf(chTemp2,"$P712,DeviceID2");
				for(ucTemp = 5;ucTemp < 10;ucTemp++)
				{ 
					sprintf(chTemp, ",%d",ulDeviceIDBuff[ucTemp]);		
					strcat(chTemp2, chTemp);					
				}
				UartResponseMsg(chTemp2);
							
			}

			else if (strncmp(chTemp, "WriteCry", 8) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
		
				if (ucCheck == FALSE)
				{
					return (FALSE);
				}
				
				for(ucTemp = 0;ucTemp < 3;ucTemp++)
				{
					WriteCry(ucTemp,chTemp);				 	
				}

				
				ucTemp = 0;
				ReadDeviceID(ulDeviceIDBuff);
				ulTemp2 = strtoul(chTemp, &pEnd, 10);
				do
				{
					ulTemp1 = ReadCry(ucTemp);
					Feed_Dog();
					ucTemp++;
				}while(ucTemp < 3 && ulTemp2 != ulTemp1);

			
				if(ulTemp2 == ulTemp1)
				{
					UartResponseMsg("$P712,WriteCry,1");
				}
				else
				{
					UartResponseMsg("$P712,WriteCry,0");
				}
			}
							
		}
		else if (DevDynamicPara.ucWorkMode == EQ_WORKMOD_TEST)
		{
			ucCheck = GetSegment((char *)pDat, chTemp, 1, uiLen, 30);
			if (ucCheck == FALSE)
			{
				return (FALSE);
			}

			//写入序列号
			if (strncmp(chTemp, "SNW", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
			
				ucLen = strlen(chTemp);

				if ((ucCheck == FALSE) || (ucLen != 8))
				{
					UartResponseMsg("$P712,0,SNW");
					return (FALSE);
				}

				//写入序列号
				//Para_Save(PARA_SERIAL_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_SERIAL_TYPE, (U8 *)chTemp,ucLen);

				UartResponseMsg("$P712,1,SNW");
				return (TRUE);
			}

			if (strncmp(chTemp, "007", 3) == TRUE)
			{
				return (FALSE);
			}

			ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);

			if (ucCheck == TRUE)
			{
				// 表示进入测试态
				if (strncmp(chTemp, "%%Test", 6) == FALSE)
				{
					//初始化测试态参数
					IniTesting();
					
					//进入测试态
					DevDynamicPara.ucWorkMode = EQ_WORKMOD_TEST;
                    DevDynamicPara.TestSta = EQ_MOD_TEST_STA_MAIN;

                    /*lq 测试模式LED初始化 */    
            		LEDInit(LED_TYPE_NONE);
            		LEDInit(LED_TYPE_POWER_TEST);

					UartResponseMsg("$P712,Testing........");

					return (TRUE);
				}
			}		

			ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
			
			if (ucCheck == FALSE)
			{
				return (FALSE);
			}
			
			//按时隙发送载波
			if (strncmp(chTemp, "001", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}

				if (strncmp(chTemp, "2087", 4) == FALSE)
				{
					channel = ENUM_AIS_CHANNEL_A;
				}
				else if (strncmp(chTemp, "2088", 4) == FALSE)
				{
					channel = ENUM_AIS_CHANNEL_B;
				}
				else
				{
					channel = ENUM_AIS_CHANNEL_A;
				}

                TEST_TxTestInit(channel, TRCTRL_TEST_TX_SIGNAL_CW,
                    TransceiverTypeA);

				UartResponseMsg("$P712,007,001,1");
			}
			
            //lq 发射标准信号4（伪随机信号）or接收测试
			else if (strncmp(chTemp, "002", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}

				ucCheck = GetSegment((char *)pDat, chTemp2, 4, uiLen, 30);   //lq 通过该字段的有无与“按时隙发送调制数据”进行区别

				if (ucCheck == FALSE)   //lq 按时隙发送调制数据
				{
    				if (strncmp(chTemp, "2087", 4) == FALSE)
    				{
					    channel = ENUM_AIS_CHANNEL_A;
    				}
    				else if (strncmp(chTemp, "2088", 4) == FALSE)
    				{
					    channel = ENUM_AIS_CHANNEL_B;
    				}
    				else
    				{
					    channel = ENUM_AIS_CHANNEL_A;
    				}
    
                    TEST_TxTestInit(channel, TRCTRL_TEST_TX_SIGNAL_NUM3,
                        TransceiverTypeA);

    				UartResponseMsg("$P712,007,002,1"); 
				}   
                else    //lq 接收测试    
                {
	                DevDynamicPara.TestSta = EQ_MOD_TEST_STA_RX;

                    SI446X_ModuleInit();                   

                    InitRxTestStruct(&RxTestInstance);
                    RxTestInstance.RxTestEn = TRUE;
    			    LEDInit(LED_TYPE_TEST_RX);                     
                    RxTxCtrl(RXTX_CTRL_TYPE_RX, 0, 0);                                        
    				UartResponseMsg("$P712,Receiving........");
                }
			}
			
            //发射频率准确度调整使能
			else if (strncmp(chTemp, "003", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}

				if (strncmp(chTemp, "2087", 4) == FALSE)
				{
					ulTemp = EQ_XMTFREQ_CHA;
				}
				else if (strncmp(chTemp, "2088", 4) == FALSE)
				{
					ulTemp = EQ_XMTFREQ_CHB;
				}
				else
				{
					ulTemp = EQ_XMTFREQ_CHA;
				}

				Testing_SetTestType(TEST_TESTSTA_FREQADJEN, ulTemp);

				UartResponseMsg("$P712,007,003,1");
			}
			
            //发射频率准确度调整
			else if (strncmp(chTemp, "004", 3) == FALSE)
			{
				ucCheck = Testing_SetTestType(TEST_TESTSTA_FREQADJ, ulTemp);

				if (ucCheck == TRUE)
				{
					ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

					if (ucCheck == FALSE)
					{
						UartResponseMsg("$P712,007,004,0");
						return (FALSE);
					}
					
					ucCheck = IsAscStringOk(chTemp);

					if (ucCheck == FALSE)
					{
						UartResponseMsg("$P712,007,004,0");
						return (FALSE);
					}

					ucLen = strlen(chTemp);

					DevStaticPara.uiFreqAdj = atoi(chTemp);		//该参数需要后期调试！！！

					if (DevStaticPara.uiFreqAdj < 16 || DevStaticPara.uiFreqAdj > 48)
					{
						UartResponseMsg("$P712,007,004,0");
						return (FALSE);
					}

					//保存频率准确度调整值
					//Para_Save(PARA_FREQADJ_TYPE, (U8 *)chTemp, ucLen);
					FlashWrite(STM32_FLASH_FREQADJ_TYPE, (U8 *)chTemp,ucLen);

//lq    				DevStaticPara.uiFreqOffset = (U16)(((U32)DevStaticPara.uiFreqAdj * EQ_MOD_OFFSET ) >> 5);
                    DevStaticPara.slXmtFreqComp = (S32)((DevStaticPara.uiFreqAdj - 32) * 125);   //lq 有问题 如何实现正负

					UartResponseMsg("$P712,007,004,1");
				}
				else
				{
					UartResponseMsg("$P712,007,004,0");
				}
				
			}
			
            //发射幅度调整使能
			else if (strncmp(chTemp, "005", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}

				if (strncmp(chTemp, "2087", 4) == FALSE)
				{
					ulTemp = EQ_XMTFREQ_CHA;
				}
				else if (strncmp(chTemp, "2088", 4) == FALSE)
				{
					ulTemp = EQ_XMTFREQ_CHB;
				}
				else
				{
					ulTemp = EQ_XMTFREQ_CHA;
				}

				Testing_SetTestType(TEST_TESTSTA_MODADJEN, ulTemp);

				UartResponseMsg("$P712,007,005,1");
			}
			
            //发射幅度调整
			else if (strncmp(chTemp, "006", 3) == FALSE)
			{
				ucCheck = Testing_SetTestType(TEST_TESTSTA_MODADJ, ulTemp);

				if (ucCheck == TRUE)
				{

					ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);
	
					if (ucCheck == FALSE)
					{
						UartResponseMsg("$P712,007,006,0");
						return (FALSE);
					}
	
					ucCheck = IsAscStringOk(chTemp);
	
					if (ucCheck == FALSE)
					{
						UartResponseMsg("$P712,007,006,0");
						return (FALSE);
					}

					ucLen = strlen(chTemp);	

					DevStaticPara.uiModAmpAdj = atoi(chTemp);		//该参数需要后期调试！！！
	
					if (DevStaticPara.uiModAmpAdj < 16 || DevStaticPara.uiModAmpAdj > 48)
					{
						UartResponseMsg("$P712,007,006,0");
						return (FALSE);
					}
	
					//保存调制幅度调整值
					//Para_Save(PARA_MODAMP_TYPE, (U8 *)chTemp, ucLen);
					FlashWrite(STM32_FLASH_MODAMP_TYPE, (U8 *)chTemp,ucLen);
	
					DevStaticPara.uiModAmp = (U16)(((U32)DevStaticPara.uiModAmpAdj * EQ_MOD_AMP + 16) >> 5);
	
					UartResponseMsg("$P712,007,006,1");
				}
				else
				{
					UartResponseMsg("$P712,007,006,0");
				}
			}
			
            //停止发射
			else if (strncmp(chTemp, "007", 3) == FALSE)
			{
				TEST_TxTestStop(TransceiverTypeA);
				UartResponseMsg("$P712,007,007,1");
			}
			
            //lq 发射标准信号3（00001111）
			else if (strncmp(chTemp, "008", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

				if (ucCheck == FALSE)
				{
				    UartResponseMsg("$P712,007,008,0");					
                    return (FALSE);
				}

				if (strncmp(chTemp, "2087", 4) == FALSE)
				{
					channel = ENUM_AIS_CHANNEL_A;
				}
				else if (strncmp(chTemp, "2088", 4) == FALSE)
				{
					channel = ENUM_AIS_CHANNEL_B;
				}
				else
				{
					channel = ENUM_AIS_CHANNEL_A;
				}

                TEST_TxTestInit(channel, TRCTRL_TEST_TX_SIGNAL_NUM2,
                    TransceiverTypeA);

				UartResponseMsg("$P712,007,008,1");
			}

#if CONFIG_PARASTORAGE_BTT
			//查询电池电量模拟模式工作时间
			else if (strncmp(chTemp, "011", 3) == FALSE)
			{
				Para_Read(PARA_BATTERY_TEST_TIME_TYPE, ucTempBuf);

				sprintf(chRetBuf, "$P712,007,011,");
				strcat(chRetBuf,(char *)ucTempBuf);

				UartResponseMsg(chRetBuf);
			}
#endif			
            //lq 发送标准信号2(01010101)
			else if (strncmp(chTemp, "012", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

				if (ucCheck == FALSE)
				{
				    UartResponseMsg("$P712,007,012,0");					
                    return (FALSE);
				}

				if (strncmp(chTemp, "2087", 4) == FALSE)
				{
					channel = ENUM_AIS_CHANNEL_A;
				}
				else if (strncmp(chTemp, "2088", 4) == FALSE)
				{
					channel = ENUM_AIS_CHANNEL_B;
				}
				else
				{
					channel = ENUM_AIS_CHANNEL_A;
				}

                TEST_TxTestInit(channel, TRCTRL_TEST_TX_SIGNAL_NUM1,
                    TransceiverTypeA);

				UartResponseMsg("$P712,007,012,1");
			}
            //栅压控制调整使能
			else if (strncmp(chTemp, "015", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}

				if (strncmp(chTemp, "2087", 4) == FALSE)
				{
					ulTemp = EQ_XMTFREQ_CHA;
				}
				else if (strncmp(chTemp, "2088", 4) == FALSE)
				{
					ulTemp = EQ_XMTFREQ_CHB;
				}
				else
				{
					ulTemp = EQ_XMTFREQ_CHA;
				}

				Testing_SetTestType(TEST_TESTSTA_GRIDADJEN, ulTemp);

				UartResponseMsg("$P712,007,015,1");
			} 

            //栅压控制调整
			else if (strncmp(chTemp, "016", 3) == FALSE)
			{
				ucCheck = Testing_SetTestType(TEST_TESTSTA_GRIDADJ, ulTemp);

				if (ucCheck == TRUE)
				{

					ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);
	
					if (ucCheck == FALSE)
					{
						UartResponseMsg("$P712,007,016,0");
						return (FALSE);
					}
	
					ucCheck = IsAscStringOk(chTemp);
	
					if (ucCheck == FALSE)
					{
						UartResponseMsg("$P712,007,016,0");
						return (FALSE);
					}

					ucLen = strlen(chTemp);	

					DevStaticPara.GridVoltAdj = atoi(chTemp);		//该参数需要后期调试！！！
	
					if (DevStaticPara.GridVoltAdj < STM32_FLASH_RF_VOLT_ADJUST_FLOOR 
                        || DevStaticPara.GridVoltAdj > STM32_FLASH_RF_VOLT_ADJUST_CEIL)
					{
						UartResponseMsg("$P712,007,016,0");
						return (FALSE);
					}
	
					//保存调制幅度调整值
					//Para_Save(PARA_RF_VOLT_ADJUST_TYPE, (U8 *)chTemp, ucLen);
					FlashWrite(STM32_FLASH_RF_VOLT_ADJUST_TYPE, (U8 *)chTemp,ucLen);
	
                    DevStaticPara.GridVoltOffset = (DevStaticPara.GridVoltAdj - 10) * DA_VALUE_01V;
	
					UartResponseMsg("$P712,007,016,1");
				}
				else
				{
					UartResponseMsg("$P712,007,016,0");
				}
			} 
#if 0            
            //发射SART标准信号2:00001111
			else if (strncmp(chTemp, "017", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

				if (ucCheck == FALSE)
				{
				    UartResponseMsg("$P712,007,017,0");					
                    return (FALSE);
				}

				if (strncmp(chTemp, "2087", 4) == FALSE)
				{
					ulTemp = EQ_XMTFREQ_CHA;
				}
				else if (strncmp(chTemp, "2088", 4) == FALSE)
				{
					ulTemp = EQ_XMTFREQ_CHB;
				}
				else
				{
					ulTemp = EQ_XMTFREQ_CHA;
				}

				Testing_SetTestType(TEST_TESTSTA_STSIGNAL2, ulTemp);
				UartResponseMsg("$P712,007,017,1");
			}
#endif            
            //进入设备自检模式
			else if (strncmp(chTemp, "018", 3) == FALSE)
			{
    			ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);
    			if (ucCheck == FALSE)
    			{
                    UartResponseMsg("$P712,007,018,0");
    				return (FALSE);
    			}
        
                /*lq 长度有效性判断*/
                ucLen = strlen(chTemp);
        		if (ucLen != 8)
        		{
                    UartResponseMsg("$P712,007,018,0");
        			return (FALSE);
        		}
        
                ulTemp = strtoul(chTemp, &pEnd, 16);
        
        		/*lq 字符有效性判断*/
        		if ((*pEnd) != 0)
        		{
                    UartResponseMsg("$P712,007,018,0");
        			return (FALSE);
        		}

	            DevDynamicPara.TestSta = EQ_MOD_TEST_STA_SELF_CHECK;
			
				SLFCHK_InstSelfCheckStructInit();
                SLFCHK_InstCfgCheckSelect(ulTemp);
                SLFCHK_InstSelfCheckStart();
                LEDInit(LED_TYPE_SELF_CHECK_IDLE);                                
				
                UartResponseMsg("$P712,007,018,1");
			}                                              					
		}
#if CONFIG_SD_CARD_MODULE_EN		
		/********************读取SD卡文件模式*******************************************/
		else if (DevDynamicPara.ucWorkMode == EQ_WORKMOD_READFILE)
		{  
			ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);

			if (ucCheck == FALSE)
			{
				return (FALSE);
			}

			if (strncmp(chTemp, "001", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}

				if(strncmp(chTemp, "ALL", 3) == FALSE)
				{
					LED_RED_ON(); 
					ReadFileName(0);
					LED_RED_OFF();
				}
				else
				{
					LED_RED_ON(); 
					ReadFileName(atoi(chTemp));
					LED_RED_OFF();
				}
			}
			else if (strncmp(chTemp, "002", 3) == FALSE) 
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}
				LED_RED_ON();
				ReadFile(chTemp);
				LED_RED_OFF();
			}
			else if (strncmp(chTemp, "003", 3) == FALSE)
			{
				LED_RED_ON();
				ReadMulFile(1);
				LED_RED_OFF();
			}
			else if (strncmp(chTemp, "004", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}
				LED_RED_ON();
				ReadMulFile(atoi(chTemp));
				LED_RED_OFF();
			}
		}
#endif	
	}

    //===================================================================================
    /*lq DUAIQ开头，参数查询*/
	//  Process  type  2      :  note by Nathan
    else if (strncmp((char *)pDat, "DUAIQ", 5) == FALSE)
	{
		/*lq 获取第1字段内容，判断当前命令 */
		ucCheck = GetSegment((char *)pDat, chTemp, 1, uiLen, 30);

		if (ucCheck == FALSE)
		{
			return (FALSE);
		}

		if (DevDynamicPara.ucWorkMode == EQ_WORKMOD_NORMAL)
		{
            if (FALSE)
            {}
            /*lq SSD 查询*/
			else if (strncmp(chTemp, "SSD", 3) == FALSE)
			{
                /*lq 语句地址字段 */
				sprintf(chRetBuf, "$AISSD");

				/*lq... Call sign （该字段暂为空，EEPROM中未存储） */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
#if 0
				Para_Read(PARA_CALL_SIGN_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);
#endif

				/*lq Ship Name */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				//Para_Read(PARA_SHIP_NAME_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_SHIP_NAME_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq ShipA */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				//Para_Read(PARA_SHIPA_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_SHIPA_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq ShipB */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				//Para_Read(PARA_SHIPB_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_SHIPB_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq ShipC */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				//Para_Read(PARA_SHIPC_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_SHIPC_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq ShipD */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				//Para_Read(PARA_SHIPD_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_SHIPD_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq DTE */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				//Para_Read(PARA_DTE_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_DTE_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq... Source identifier （EEPROM中未存储）*/
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
#if 0
				Para_Read(PARA_TALK_ID_TYPE, ucTempBuf);
#else
				sprintf((char *)ucTempBuf, AIS_DI_TALKER_ID_DEFAULT);
#endif	
    			strcat(chRetBuf, (char *)ucTempBuf);

				UartResponseMsg(chRetBuf);		
			}		

            /*lq VSD 查询*/
			else if (strncmp(chTemp, "VSD", 3) == FALSE)
			{
                /*lq 语句地址字段 */
				sprintf(chRetBuf, "$AIVSD");

                /*lq Type of ship and cargo category */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				//Para_Read(PARA_SHIP_TYPE_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_SHIP_TYPE_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq Maximum present static draught */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				//Para_Read(PARA_MAX_DRAUGHT_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_MAX_DRAUGHT_TYPE, ucTempBuf);
				sprintf(chTemp, "%.1f", strtod(ucTempBuf, &pEnd) / 10.0);
                strcat(chRetBuf, (char *)chTemp);

				/*lq Persons on-board */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				Para_Read(PARA_PERSONS_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq Destination */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				//Para_Read(PARA_DESTINATION_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_DESTINATION_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq Estimated UTC of arrival at destination */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				Para_Read(PARA_ETA_HOUR_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);
				Para_Read(PARA_ETA_MINUTE_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);
				sprintf(chTemp, "00");  //lq 秒，EEPROM中不存储，AIS消息中不使用
				strcat(chRetBuf, chTemp);

				/*lq Estimated day of arrival at destination */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				Para_Read(PARA_ETA_DAY_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq Estimated month of arrival at destination */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				Para_Read(PARA_ETA_MONTH_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq Navigational status */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				Para_Read(PARA_NAVIGATION_STATUS_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				/*lq Regional application flags */
				sprintf(chTemp, ",");
				strcat(chRetBuf, chTemp);
				Para_Read(PARA_REGION_APP_FLAG_TYPE, ucTempBuf);
				strcat(chRetBuf, (char *)ucTempBuf);

				UartResponseMsg(chRetBuf);		
			}
			
			//查询 MMSI ，712格式
			else if (strncmp(chTemp, "010", 3) == FALSE)
			{
				//从EEPROM中读取MMSI号
				//Para_Read(PARA_MMSI_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_MMSI_TYPE, ucTempBuf);
				ulTemp = atol((char *)ucTempBuf);

				if (ulTemp == 0)
				{
					//Para_Read(PARA_MMSI_BACKUP_TYPE, ucTempBuf);//读备份
					FlashRead(STM32_FLASH_MMSI_DEST_TYPE, ucTempBuf);
					ulTemp = atol((char *)ucTempBuf);
					
					if (ulTemp != 0)
					{
						ucLen = strlen((char *)ucTempBuf);
						//Para_Save(PARA_MMSI_TYPE, ucTempBuf,ucLen);//回写
						FlashWrite(STM32_FLASH_MMSI_TYPE, ucTempBuf,ucLen);
					}

				}//不管读的对不对，都返回了
				
				//组报文返回MMSI号
				sprintf(chRetBuf, "$P712,010,,,");
				strcat(chRetBuf, (char *)ucTempBuf);

				UartResponseMsg(chRetBuf);		
			}

            //查询序列号
			else if (strncmp(chTemp, "SNO", 3) == FALSE)
			{
				//从EEPROM中读取SERIAL
				//Para_Read(PARA_SERIAL_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_SERIAL_TYPE, ucTempBuf);
				
				//组报文返回SERIAL
				sprintf(chRetBuf, "$P712,SNO,");
				strcat(chRetBuf, (char *)ucTempBuf);

				UartResponseMsg(chRetBuf);		
			}
			
            //查询设备参数，软件版本号查询
			else if (strncmp(chTemp, "SWF", 3) == FALSE)
			{
				sprintf(chTemp, "$P712,SWF,%s", VERSION);
				UartResponseMsg(chTemp);		
			}
			
            //查船舶厂商
			else if (strncmp(chTemp, "SRM", 3) == FALSE)
			{
				//从EEPROM中读取POWERMOD
				//Para_Read(PARA_VENDID_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_VENDID_TYPE, ucTempBuf);
				
				//组报文返回POWERMOD
				sprintf(chRetBuf, "$P712,SRM,,,");
				strcat(chRetBuf, (char *)ucTempBuf);

				UartResponseMsg(chRetBuf);	
			}	
			
			//查询休眠条件
			else if (strncmp(chTemp, "CKS", 3) == FALSE)
			{
				ucTemp=GetGpsSleepSta();
				usart_printf("ucGpsSta %d\r\n",ucTemp);
#if 0
				ucTemp=GetDigUnitSleepSta();
				usart_printf("ucDigUnitSta %d\r\n",ucTemp);
#endif
#if CONFIG_KEY
				ucTemp=GetMMISleepSta();
				usart_printf("ucMMISta %d\r\n",ucTemp);
#endif
#if CONFIG_POWERDOWN
				ucTemp=GetPowerDownSleepSta();
#endif
				usart_printf("ucPowerDownSta %d\r\n",ucTemp);
#if 0
				ucTemp=GetPtlFifoSleepSta();
				usart_printf("ucPtlFifoSta %d\r\n",ucTemp);		
#endif
			}

			//查询唤醒条件
			else if (strncmp(chTemp, "CKW", 3) == FALSE)
			{
				 usart_printf("wakeup frame %d\r\n",SleepCtrlPara.WakeUpFrame);
				 usart_printf("now frame %d\r\n",gslFrameCnt);
				 usart_printf("wakeup slot %d\r\n",SleepCtrlPara.WakeUpSlot);
				 usart_printf("now slot %d\r\n",gsiSlotCnt);
			}

			//查询开机时间
			else if (strncmp(chTemp, "TOP", 3) == FALSE)//返回报文格式 $P712,TOP,i,YYYY-mm-DD HH:MM:SS
			{
				for (i = 0; i < PARA_TIME_OPEN_CLOSE_TOTAL; i++)
                {
    				/*lq 计算开机时间参数类型 */
                    ulTemp = PARA_TIME_OPEN_0_TYPE + ((DevDynamicPara.OpenCloseTimeIndex + 1 + i)
                        % PARA_TIME_OPEN_CLOSE_TOTAL);

                    /*lq 从EEPROM中读取开机时间 */
                    Para_Read(ulTemp, ucTempBuf);
                    
    				/*lq 组报文返回开机时间 */
    				sprintf(chRetBuf, "$P712,TOP,%d,20", PARA_TIME_OPEN_CLOSE_TOTAL - 1 - i);
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[0]), 2);  //lq year
    				strcat((char *)chRetBuf, "-");
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[2]), 2);  //lq month
    				strcat((char *)chRetBuf, "-");
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[4]), 2);  //lq day
    				strcat((char *)chRetBuf, " ");
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[6]), 2);  //lq hour
    				strcat((char *)chRetBuf, ":");
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[8]), 2);  //lq minute
    				strcat((char *)chRetBuf, ":");
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[10]), 2); //lq second
    
    				UartResponseMsg(chRetBuf);	
                }				
			} 

			//查询第一次GPS定位时间
			else if (strncmp(chTemp, "GPSFT", 5) == FALSE)//返回报文格式 $P712,GPSFT,YYYY-mm-DD HH:MM:SS
			{ 
                /* 从EEPROM中读取第一次GPS定位时间 */
                Para_Read(PARA_GPS_FIRST_TIME_TYPE, ucTempBuf);
                   
   				/* 组报文返回开机时间 */
   				sprintf(chRetBuf, "$P712,GPSFT,20");
   				strncat((char *)chRetBuf, (char *)(&ucTempBuf[0]), 2);  //lq year
   				strcat((char *)chRetBuf, "-");
   				strncat((char *)chRetBuf, (char *)(&ucTempBuf[2]), 2);  //lq month
   				strcat((char *)chRetBuf, "-");
   				strncat((char *)chRetBuf, (char *)(&ucTempBuf[4]), 2);  //lq day
   				strcat((char *)chRetBuf, " ");
   				strncat((char *)chRetBuf, (char *)(&ucTempBuf[6]), 2);  //lq hour
   				strcat((char *)chRetBuf, ":");
   				strncat((char *)chRetBuf, (char *)(&ucTempBuf[8]), 2);  //lq minute
   				strcat((char *)chRetBuf, ":");
   				strncat((char *)chRetBuf, (char *)(&ucTempBuf[10]), 2); //lq second
   
   				UartResponseMsg(chRetBuf);					
			}

			//查询关机时间
			else if (strncmp(chTemp, "TST", 3) == FALSE)//返回报文格式 $P712,TST,i,YYYY-mm-DD HH:MM:SS
			{
				for (i = 0; i < PARA_TIME_OPEN_CLOSE_TOTAL; i++)
                {
    				/*lq 计算关机时间参数类型 */
                    ulTemp = PARA_TIME_CLOSE_0_TYPE + ((DevDynamicPara.OpenCloseTimeIndex + 1 + i)
                        % PARA_TIME_OPEN_CLOSE_TOTAL);

                    /*lq 从EEPROM中读取关机时间 */
                    Para_Read(ulTemp, ucTempBuf);
                    
    				/*lq 组报文返回关机时间 */
    				sprintf(chRetBuf, "$P712,TST,%d,20", PARA_TIME_OPEN_CLOSE_TOTAL - 1 - i);
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[0]), 2);  //lq year
    				strcat((char *)chRetBuf, "-");
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[2]), 2);  //lq month
    				strcat((char *)chRetBuf, "-");
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[4]), 2);  //lq day
    				strcat((char *)chRetBuf, " ");
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[6]), 2);  //lq hour
    				strcat((char *)chRetBuf, ":");
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[8]), 2);  //lq minute
    				strcat((char *)chRetBuf, ":");
    				strncat((char *)chRetBuf, (char *)(&ucTempBuf[10]), 2); //lq second
    
    				UartResponseMsg(chRetBuf);	
                }				
			}
            //本次看门狗复位次数查询
			else if (strncmp(chTemp, "CWRC", 4) == FALSE)
			{
				Para_Read(PARA_WDG_RESET_CNT_TYPE, ucTempBuf);

				sprintf(chRetBuf, "$P712,CWRC,");
				strcat(chRetBuf,(char *)ucTempBuf);

				UartResponseMsg(chRetBuf);
			}
            
			//上次看门狗复位次数查询
			else if (strncmp(chTemp, "LWRC", 4) == FALSE)
			{
				Para_Read(PARA_LAST_WDG_RESET_CNT_TYPE, ucTempBuf);

				sprintf(chRetBuf, "$P712,LWRC,");
				strcat(chRetBuf,(char *)ucTempBuf);

				UartResponseMsg(chRetBuf);
			} 
           
			//正常开机次数查询
			else if (strncmp(chTemp, "NPOC", 4) == FALSE)
			{
				Para_Read(PARA_NORMAL_POWER_ON_CNT_TYPE, ucTempBuf);

				sprintf(chRetBuf, "$P712,NPOC,");
				strcat(chRetBuf,(char *)ucTempBuf);

				UartResponseMsg(chRetBuf);
			}  
#if 0 
			//用户测试次数查询
			else if (strncmp(chTemp, "STC", 3) == FALSE)
			{
				Para_Read(PARA_USER_TEST_CNT_TYPE, ucTempBuf);

				sprintf(chRetBuf, "$P712,STC,");
				strcat(chRetBuf,(char *)ucTempBuf);

				UartResponseMsg(chRetBuf);
			} 

            //GPS5min长开次数查询
			else if (strncmp(chTemp, "GLOC", 4) == FALSE)
			{
				Para_Read(PARA_GPS_5MIN_LON_OPEN_COUNT_TYPE, ucTempBuf);

				sprintf(chRetBuf, "$P712,GLOC,");
				strcat(chRetBuf,(char *)ucTempBuf);

				UartResponseMsg(chRetBuf);
			}
#endif
			//工作总时长查询
			else if (strncmp(chTemp, "WTS", 3) == FALSE)
			{
				Para_Read(PARA_WORKTIME_TYPE, ucTempBuf);

				sprintf(chRetBuf, "$P712,WTS,");
				strcat(chRetBuf,(char *)ucTempBuf);

				UartResponseMsg(chRetBuf);
			}           

			//上次开机工作时长查询
			else if (strncmp(chTemp, "LWT", 3) == FALSE)
			{
				Para_Read(PARA_WORKTIME_LAST_TYPE, ucTempBuf);

				sprintf(chRetBuf, "$P712,LWT,");
				strcat(chRetBuf,(char *)ucTempBuf);

				UartResponseMsg(chRetBuf);
			}

			//航行总距离查询
			else if (strncmp(chTemp, "SAILDISINCE", 11) == FALSE)
			{
				Para_Read(PARA_SAILING_DISTINCE_TYPE, ucTempBuf);

				sprintf(chRetBuf, "$P712,SAILDISINCE,");
				strcat(chRetBuf,(char *)ucTempBuf);

				UartResponseMsg(chRetBuf);
			}

#if CONFIG_TEMPERATURE_MONITOR
			//当前温度查询
			else if (strncmp(chTemp, "TEMP", 4) == FALSE)
			{
#if 0
                sprintf(chRetBuf, "$P712,TEMP,%.4f", GetTemper());  //lq? 为何GetTemper()函数返回值错误？
#else
                sprintf(chRetBuf, "$P712,TEMP,%.4f", struTemperSensorPara.fsTemper);
#endif
				UartResponseMsg(chRetBuf);
			}
#endif
            //查询设备参数，硬件版本号查询
			else if (strncmp(chTemp, "HWF", 3) == FALSE)
			{
				//Para_Read(PARA_HWVERSION_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_HWVERSION_TYPE, ucTempBuf);
				sprintf(chRetBuf, "$P712,HWF,");
				strcat(chRetBuf, (char *)ucTempBuf);

				UartResponseMsg(chRetBuf);		
			}                                                                                      			

            //查询设备参数，AIS类别查询
			else if (strncmp(chTemp, "AISCLASS", 8) == FALSE)
			{
				//Para_Read(PARA_AIS_CLASS_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_AIS_CLASS_TYPE, ucTempBuf);
				sprintf(chRetBuf, "$P712,AISCLASS,");
				strcat(chRetBuf, (char *)ucTempBuf);

				UartResponseMsg(chRetBuf);		
			}                                                                                      			

            //查询Transceiver状态
            else if (strncmp(chTemp, "TR", 2) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);//读取第二字段的内容
				if (ucCheck == FALSE)
				{
					return (FALSE);
				}	

				/*lq RSSI */
				if (strncmp(chTemp, "RSSI", 4) == FALSE)
				{
    				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);//读取第3字段的内容    
    				if (ucCheck == FALSE)
    				{
                        return (FALSE);
    				}

                    /*lq 长度有效性判断*/
                    ucLen = strlen(chTemp);
        			if (ucLen != 1)
        			{
        				return (FALSE);
        			}
        
        			/*lq 字符有效性判断*/
                    ucCheck = IsAscStringOk(chTemp);
        			if (ucCheck == FALSE)
        			{
        				return (FALSE);
        			}
                    else
                    {
                        /*lq 取值范围有效性判断*/
                        ulTemp = strtoul(chTemp, &pEnd, 10);
                        if (ulTemp == 1)
                        {
                            sprintf(chRetBuf, "$P712,TR,RSSI,1");
            
            				/*lq RSSI */
                            sprintf(ucTempBuf, "%d", SI446X_GetRssi(SI446X_CHIP_A));
            				strcat(chRetBuf, ",");               
                            strcat(chRetBuf, (char *)ucTempBuf);
                                                                                                                                                        
            				UartResponseMsg(chRetBuf);                        
                        }
                        else if (ulTemp == 2)
                        {
                            sprintf(chRetBuf, "$P712,TR,RSSI,2");
            
            				/*lq RSSI */
                            sprintf(ucTempBuf, "%d", SI446X_GetRssi(SI446X_CHIP_B));
            				strcat(chRetBuf, ",");               
                            strcat(chRetBuf, (char *)ucTempBuf);
                                                                                                                                                        
            				UartResponseMsg(chRetBuf);                        
                        }
                        else
                        {
                            return (FALSE);                        
                        }
                    }
                }
				
                /*lq machine state */
				if (strncmp(chTemp, "006", 3) == FALSE)
				{
    				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);//读取第3字段的内容    
    				if (ucCheck == FALSE)
    				{
                        return (FALSE);
    				}

                    /*lq 长度有效性判断*/
                    ucLen = strlen(chTemp);
        			if (ucLen != 1)
        			{
        				return (FALSE);
        			}
        
        			/*lq 字符有效性判断*/
                    ucCheck = IsAscStringOk(chTemp);
        			if (ucCheck == FALSE)
        			{
        				return (FALSE);
        			}
                    else
                    {
                        /*lq 取值范围有效性判断*/
                        ulTemp = strtoul(chTemp, &pEnd, 10);
                        if (ulTemp == 1)
                        {
                            sprintf(chRetBuf, "$P712,TR,006,1");
            
            				/*lq 状态查询 */
                            SI446X_REQUEST_DEVICE_STATE(buffer, SI446X_CHIP_A);
                            sprintf(ucTempBuf, "%d", buffer[1]);
            				strcat(chRetBuf, ",");               
                            strcat(chRetBuf, (char *)ucTempBuf);
                                                                                                                                                        
            				UartResponseMsg(chRetBuf);                        
                        }
                        else if (ulTemp == 2)
                        {
                            sprintf(chRetBuf, "$P712,TR,006,2");
            
            				/*lq 状态查询 */
                            SI446X_REQUEST_DEVICE_STATE(buffer, SI446X_CHIP_B);
                            sprintf(ucTempBuf, "%d", buffer[1]);
            				strcat(chRetBuf, ",");               
                            strcat(chRetBuf, (char *)ucTempBuf);
                                                                                                                                                        
            				UartResponseMsg(chRetBuf);                                                
                        }
                        else
                        {
                            return (FALSE);                        
                        }
                    }
                }

                /*lq SI4463当前输出功率值 */
				if (strncmp(chTemp, "007", 3) == FALSE)
				{
    				ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);//读取第3字段的内容    
    				if (ucCheck == FALSE)
    				{
                        return (FALSE);
    				}

                    /*lq 长度有效性判断*/
                    ucLen = strlen(chTemp);
        			if (ucLen != 1)
        			{
            		    /*lq 长度无效 */
        				return (FALSE);
        			}

            		/*lq 字符有效性判断 */
                    ulTemp = strtoul(chTemp, &pEnd, 10);
            		if ((*pEnd) != 0)
            		{
            		    /*lq 字符无效 */
            			return (FALSE);
            		}

                    /*lq 取值范围有效性判断 */
                    if ((ulTemp < 1) || (ulTemp > 2))
                    {
            		    /*lq 取值无效 */
            			return (FALSE);
                    }
        
        			if (ulTemp == 1)
                    {
                        ulTemp1 = SI446X_GetSi446xPaPwrLvl(SI446X_CHIP_A);
                    }
                    else
                    {
                        ulTemp1 = SI446X_GetSi446xPaPwrLvl(SI446X_CHIP_B);                    
                    }

                    sprintf(chRetBuf, "$P712,TR,007");  
                    sprintf(ucTempBuf, "%d", ulTemp);
    				strcat(chRetBuf, ",");               
                    strcat(chRetBuf, (char *)ucTempBuf);
                    sprintf(ucTempBuf, "%d", ulTemp1);
    				strcat(chRetBuf, ",");               
                    strcat(chRetBuf, (char *)ucTempBuf);
                                                                                                                                                
    				UartResponseMsg(chRetBuf);
                }

                /*lq 查询 */
                //lq...

                else
                {
                    return (FALSE);                                        
                }                                                                                                                                           			
            }
			
            /*lq 查询调试输出状态 */
			else if (strncmp(chTemp, "DBO", 3) == FALSE)
			{
                if (DebugOutputFlag == TRUE)
                {
                    sprintf(chRetBuf, "$P712,DBO,1");
                }
                else
                {
                    sprintf(chRetBuf, "$P712,DBO,0");                
                }
				
                UartResponseMsg(chRetBuf);		
			}

            /*lq 查询GPS串口输出频率 */
			else if (strncmp(chTemp, "GPSOP", 5) == FALSE)
			{
                /*lq 语句头 */
				sprintf(chRetBuf, "$P712,GPSOP");

				/*lq RMC */
                sprintf(chTemp, ",%03d", DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_RMC]);
				strcat(chRetBuf, chTemp);

				/*lq VTG */
                sprintf(chTemp, ",%03d", DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_VTG]);
				strcat(chRetBuf, chTemp);

				/*lq GGA */
                sprintf(chTemp, ",%03d", DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GGA]);
				strcat(chRetBuf, chTemp);

				/*lq GSA */
                sprintf(chTemp, ",%03d", DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSA]);
				strcat(chRetBuf, chTemp);

				/*lq GSV */
                sprintf(chTemp, ",%03d", DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GSV]);
				strcat(chRetBuf, chTemp);

				/*lq GLL */
                sprintf(chTemp, ",%03d", DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GLL]);
				strcat(chRetBuf, chTemp);

				/*lq GBS */
                sprintf(chTemp, ",%03d", DevDynamicPara.GpsOutputInterval[
                    DEV_INFO_GPS_OUTPUT_INTERVAL_INDEX_GBS]);
				strcat(chRetBuf, chTemp);

                UartResponseMsg(chRetBuf);		
			}

            /***************************************************************************/
			/***************************************************************************/
			/***************************************************************************/
			/***************************************************************************/
			
			//可配置时间参数查询
			else if (strncmp(chTemp, "050", 3) == FALSE)
			{
				ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);

				if (ucCheck == FALSE)
				{
					return (FALSE);
				}
                //未写完
			
            }
		}
		else if (DevDynamicPara.ucWorkMode == EQ_WORKMOD_TEST)
		{
			//查询频率准确度
			if (strncmp(chTemp, "FRE", 3) == FALSE)
			{
				//从EEPROM中读取POWERMOD
				//Para_Read(PARA_FREQADJ_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_FREQADJ_TYPE, ucTempBuf);
				
				//组报文返回POWERMOD
				sprintf(chRetBuf, "$P712,FREQ,");
				strcat(chRetBuf, (char *)ucTempBuf);

				UartResponseMsg(chRetBuf);		
			}
			
            //查询调制幅度
			else if (strncmp(chTemp, "AMP", 3) == FALSE)
			{
				//从EEPROM中读取POWERMOD
				//Para_Read(PARA_MODAMP_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_MODAMP_TYPE, ucTempBuf);
				
				//组报文返回POWERMOD
				sprintf(chRetBuf, "$P712,AMP,");
				strcat(chRetBuf, (char *)ucTempBuf);

				UartResponseMsg(chRetBuf);	
			}

            //查询栅压控制
			else if (strncmp(chTemp, "GRID", 4) == FALSE)
			{
				//从EEPROM中读取POWERMOD
				//Para_Read(PARA_RF_VOLT_ADJUST_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_RF_VOLT_ADJUST_TYPE, ucTempBuf);
				
				//组报文返回POWERMOD
				sprintf(chRetBuf, "$P712,GRID,");
				strcat(chRetBuf, (char *)ucTempBuf);

				UartResponseMsg(chRetBuf);	
			}			
            //查询序列号
			else if (strncmp(chTemp, "SNO", 3) == FALSE)
			{
				//从EEPROM中读取SERIAL
				//Para_Read(PARA_SERIAL_TYPE, ucTempBuf);
				FlashRead(STM32_FLASH_SERIAL_TYPE, ucTempBuf);
				
				//组报文返回SERIAL
				sprintf(chRetBuf, "$P712,SNO,");
				strcat(chRetBuf, (char *)ucTempBuf);

				UartResponseMsg(chRetBuf);	
			}
		}
	}
    //=====================================================================================
    /*lq AISSD开头，静态数据*/
	//  Process  type  3     :  note by Nathan
	else if (strncmp((char *)pDat, "AISSD", 5) == FALSE)
	{
		if (DevDynamicPara.ucWorkMode == EQ_WORKMOD_NORMAL)
		{
			/*lq **********************Ship name**************************** */
			ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
			
            /*lq 字段非空*/			
            if (ucCheck != FALSE)
			{
    			ucLen = strlen(chTemp);
    			if (ucLen > AIS_INFO_SHIP_NAME_LEN)
    			{
    				UartResponseMsg("$DUAIR,0,SSD");
    				return (FALSE);
    			}
    
    			/*lq 写入EEPROM */
    			//Para_Save(PARA_SHIP_NAME_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_SHIP_NAME_TYPE, (U8 *)chTemp,ucLen);
    
    			/*lq 更新变量 */
    			strcpy((char *)&AisStaticPara.ShipName[0], AIS_INFO_SHIP_NAME_DEFAULT);     //lq 使用默认值‘@’重写所有字符
    			strncpy((char *)&AisStaticPara.ShipName[0], chTemp, ucLen);                 //lq 使用目标值覆盖前面的‘@’

                AisStaticPara.Refresh = TRUE;
			}

			/*lq ************************ShipA****************************** */
			ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);

            /*lq 字段非空*/			
			if (ucCheck != FALSE)
			{
                /*lq 长度有效性判断*/
                ucLen = strlen(chTemp);
        		if (ucLen != 3)
        		{
    				UartResponseMsg("$DUAIR,0,SSD");
        			return (FALSE);
        		}
        
        		/*lq 字符有效性判断*/
                ulTemp = strtoul(chTemp, &pEnd, 10);
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,SSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq 取值范围有效性判断*/
                    if (ulTemp < STM32_FLASH_SHIPA_FLOOR
                        || ulTemp > STM32_FLASH_SHIPA_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,SSD");
            			return (FALSE);
                    }
                }
    
    			/*lq 写入EEPROM */
    			//Para_Save(PARA_SHIPA_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_SHIPA_TYPE, (U8 *)chTemp,ucLen);
    
    			/*lq 更新变量 */
    			AisStaticPara.ShipA = ulTemp;
			
                AisStaticPara.Refresh = TRUE;
            }

			/*lq ************************ShipB****************************** */
			ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);

            /*lq 字段非空*/			
			if (ucCheck != FALSE)
			{
                /*lq 长度有效性判断*/
                ucLen = strlen(chTemp);
        		if (ucLen != 3)
        		{
    				UartResponseMsg("$DUAIR,0,SSD");
        			return (FALSE);
        		}
        
        		/*lq 字符有效性判断*/
                ulTemp = strtoul(chTemp, &pEnd, 10);
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,SSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq 取值范围有效性判断*/
                    if (ulTemp < STM32_FLASH_SHIPB_FLOOR
                        || ulTemp > STM32_FLASH_SHIPB_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,SSD");
            			return (FALSE);
                    }
                }
                    
    			/*lq 写入EEPROM */
    			//Para_Save(PARA_SHIPB_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_SHIPB_TYPE, (U8 *)chTemp,ucLen);
    
    			/*lq 更新变量 */
    			AisStaticPara.ShipB = ulTemp;
			
                AisStaticPara.Refresh = TRUE;
			}

			/*lq ************************ShipC****************************** */
			ucCheck = GetSegment((char *)pDat, chTemp, 5, uiLen, 30);

            /*lq 字段非空*/			
			if (ucCheck != FALSE)
			{
                /*lq 长度有效性判断*/
                ucLen = strlen(chTemp);
        		if (ucLen != 2)
        		{
    				UartResponseMsg("$DUAIR,0,SSD");
        			return (FALSE);
        		}
        
        		/*lq 字符有效性判断*/
                ulTemp = strtoul(chTemp, &pEnd, 10);
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,SSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq 取值范围有效性判断*/
                    if (ulTemp < STM32_FLASH_SHIPC_FLOOR
                        || ulTemp > STM32_FLASH_SHIPC_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,SSD");
            			return (FALSE);
                    }
                }
                    
    			/*lq 写入EEPROM */
    			//Para_Save(PARA_SHIPC_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_SHIPC_TYPE, (U8 *)chTemp,ucLen);
    
    			/*lq 更新变量 */
    			AisStaticPara.ShipC = ulTemp;
			
                AisStaticPara.Refresh = TRUE;
			}

			/*lq ************************ShipD****************************** */
			ucCheck = GetSegment((char *)pDat, chTemp, 6, uiLen, 30);

            /*lq 字段非空*/			
			if (ucCheck != FALSE)
			{
                /*lq 长度有效性判断*/
                ucLen = strlen(chTemp);
        		if (ucLen != 2)
        		{
    				UartResponseMsg("$DUAIR,0,SSD");
        			return (FALSE);
        		}
        
        		/*lq 字符有效性判断*/
                ulTemp = strtoul(chTemp, &pEnd, 10);
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,SSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq 取值范围有效性判断*/
                    if (ulTemp < STM32_FLASH_SHIPD_FLOOR
                        || ulTemp > STM32_FLASH_SHIPD_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,SSD");
            			return (FALSE);
                    }
                }
                    
    			/*lq 写入EEPROM */
    			//Para_Save(PARA_SHIPD_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_SHIPD_TYPE, (U8 *)chTemp,ucLen);
    
    			/*lq 更新变量 */
    			AisStaticPara.ShipD = ulTemp;
			
                AisStaticPara.Refresh = TRUE;
			}

			/*lq *************************DTE******************************* */
			ucCheck = GetSegment((char *)pDat, chTemp, 7, uiLen, 30);

            /*lq 字段非空*/			
			if (ucCheck != FALSE)
			{
                /*lq 长度有效性判断*/
                ucLen = strlen(chTemp);
        		if (ucLen != 1)
        		{
    				UartResponseMsg("$DUAIR,0,SSD");
        			return (FALSE);
        		}
        
        		/*lq 字符有效性判断*/
                ulTemp = strtoul(chTemp, &pEnd, 10);
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,SSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq 取值范围有效性判断*/
                    if (ulTemp < STM32_FLASH_DTE_FLOOR
                        || ulTemp > STM32_FLASH_DTE_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,SSD");
            			return (FALSE);
                    }
                }
                    
    			/*lq 写入EEPROM */
    			//Para_Save(PARA_DTE_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_DTE_TYPE, (U8 *)chTemp,ucLen);
    
    			/*lq 更新变量 */
    			AisStaticPara.DTE = ulTemp;
			
                AisStaticPara.Refresh = TRUE;
			}

			UartResponseMsg("$DUAIR,1,SSD");
		}
	}
    //==========================================================================================
    /*lq AIVSD开头，船类型数据*/
	//  Process  type  4     :  note by Nathan
	else if (strncmp((char *)pDat, "AIVSD", 5) == FALSE)
	{
		if (DevDynamicPara.ucWorkMode == EQ_WORKMOD_NORMAL)
		{           
            /*lq ********Type of ship and cargo category, 0 to 255********** */
			ucCheck = GetSegment((char *)pDat, chTemp, 1, uiLen, 30);
            
            /*lq 字段非空*/			
            if (ucCheck != FALSE)
			{
        		/*lq 字符有效性判断*/
                ulTemp = (U32)strtod(chTemp, &pEnd);
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq 取值范围有效性判断*/
                    if (ulTemp < STM32_FLASH_SHIP_TYPE_FLOOR
                        || ulTemp > STM32_FLASH_SHIP_TYPE_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,VSD");
            			return (FALSE);
                    }
                }
    
    			/*lq 写入EEPROM */
    			sprintf(ucTempBuf, "%d", ulTemp);
                //Para_Save(PARA_SHIP_TYPE_TYPE, ucTempBuf, PARA_SHIP_TYPE_MAXLEN);
				FlashWrite(STM32_FLASH_SHIP_TYPE_TYPE, ucTempBuf,STM32_FLASH_SHIP_TYPE_MAXLEN);
    
    			/*lq 更新变量 */
    			AisStaticPara.TypeOfShip = ulTemp;

                AisStaticPara.Refresh = TRUE;
            }

            /*lq ********Maximum present static draught, 0 to 25,5 m******** */
			ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
            
            /*lq 字段非空*/			
            if (ucCheck != FALSE)
			{
        		/*lq 字符有效性判断*/
                ulTemp = (U32)(strtod(chTemp, &pEnd) * 10);   //lq 程序中使用值的单位为0.1m
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq 取值范围有效性判断*/
                    if (ulTemp < STM32_FLASH_MAX_DRAUGHT_FLOOR
                        || ulTemp > STM32_FLASH_MAX_DRAUGHT_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,VSD");
            			return (FALSE);
                    }
                }
    
    			/*lq 写入EEPROM */
                sprintf(ucTempBuf, "%d", ulTemp);
    			//Para_Save(PARA_MAX_DRAUGHT_TYPE, ucTempBuf, PARA_MAX_DRAUGHT_MAXLEN);
				FlashWrite(STM32_FLASH_MAX_DRAUGHT_TYPE, ucTempBuf,STM32_FLASH_MAX_DRAUGHT_MAXLEN);
    
    			/*lq 更新变量 */
    			AisStaticPara.MaxDraught = ulTemp;

                AisStaticPara.Refresh = TRUE;
            }

            /*lq *****************Persons on-board, 0 to 8191*************** */
			ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);
            
            /*lq 字段非空*/			
            if (ucCheck != FALSE)
			{
        		/*lq 字符有效性判断*/
                ulTemp = (U32)strtod(chTemp, &pEnd);
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq 取值范围有效性判断*/
                    if (ulTemp < PARA_PERSONS_FLOOR
                        || ulTemp > PARA_PERSONS_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,VSD");
            			return (FALSE);
                    }
                }
    
    			/*lq 写入EEPROM */
                sprintf(ucTempBuf, "%d", ulTemp);
    			Para_Save(PARA_PERSONS_TYPE, ucTempBuf, PARA_PERSONS_MAXLEN);
    
    			/*lq 更新变量 */
    			AisStaticPara.Persons = ulTemp;

                AisStaticPara.Refresh = TRUE;
            }

			/*lq **************Destination, 1-20 characters***************** */
			ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);
			
            /*lq 字段非空*/			
            if (ucCheck != FALSE)
			{
    			ucLen = strlen(chTemp);
    			if (ucLen > AIS_INFO_DESTINATION_LEN)
    			{
    				UartResponseMsg("$DUAIR,0,VSD");
    				return (FALSE);
    			}
    
    			/*lq 写入EEPROM */
    			//Para_Save(STM32_FLASH_DESTINATION_TYPE, (U8 *)chTemp, ucLen);
				FlashWrite(STM32_FLASH_DESTINATION_TYPE, (U8 *)chTemp,ucLen);
    
    			/*lq 更新变量 */
    			strcpy((char *)&AisStaticPara.Destination[0], AIS_INFO_DESTINATION_DEFAULT);    //lq 使用默认值‘@’重写所有字符
    			strncpy((char *)&AisStaticPara.Destination[0], chTemp, ucLen);                  //lq 使用目标值覆盖前面的‘@’

                AisStaticPara.Refresh = TRUE;
			}

			/*lq ********Estimated UTC of arrival at destination************ */
			ucCheck = GetSegment((char *)pDat, chTemp, 5, uiLen, 30);

            /*lq 字段非空*/			
			if (ucCheck != FALSE)
			{
                /*lq 长度有效性判断*/
                ucLen = strlen(chTemp);
        		if ((ucLen < 6) || (ucLen > 9))
        		{
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
        		}
        
        		/*lq 字符有效性判断*/
                /*lq Hour */
                ulTemp = (chTemp[0] - '0') * 10 + (chTemp[1] - '0');
                if ((ulTemp < PARA_ETA_HOUR_FLOOR)
                    || (ulTemp > PARA_ETA_HOUR_CEIL))
                {
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
                }
                else
                {
                    /*lq 写入EEPROM */
        			sprintf(chTemp2, "%d", ulTemp);
        			Para_Save(PARA_ETA_HOUR_TYPE, (U8 *)chTemp2, 2);
        
        			/*lq 更新变量 */
        			AisStaticPara.ETAHour = ulTemp;
    			
                    AisStaticPara.Refresh = TRUE;                
                }

                /*lq Minute */
                ulTemp = (chTemp[2] - '0') * 10 + (chTemp[3] - '0');
                if ((ulTemp < PARA_ETA_MINUTE_FLOOR)
                    || (ulTemp > PARA_ETA_MINUTE_CEIL))
                {
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
                }
                else
                {
                    /*lq 写入EEPROM */
        			sprintf(chTemp2, "%d", ulTemp);
        			Para_Save(PARA_ETA_MINUTE_TYPE, (U8 *)chTemp2, 2);
        
        			/*lq 更新变量 */
        			AisStaticPara.ETAMinute = ulTemp;
    			
                    AisStaticPara.Refresh = TRUE;                
                }   
            }

			/*lq **Estimated day of arrival at destination, 00 to 31 (UTC)** */
			ucCheck = GetSegment((char *)pDat, chTemp, 6, uiLen, 30);

            /*lq 字段非空*/			
			if (ucCheck != FALSE)
			{
                /*lq 长度有效性判断*/
                ucLen = strlen(chTemp);
        		if (ucLen != 2)
        		{
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
        		}
        
        		/*lq 字符有效性判断*/
                ulTemp = strtoul(chTemp, &pEnd, 10);
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq 取值范围有效性判断*/
                    if (ulTemp < PARA_ETA_DAY_FLOOR
                        || ulTemp > PARA_ETA_DAY_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,VSD");
            			return (FALSE);
                    }
                }
                    
    			/*lq 写入EEPROM */
    			Para_Save(PARA_ETA_DAY_TYPE, (U8 *)chTemp, ucLen);
    
    			/*lq 更新变量 */
    			AisStaticPara.ETADay = ulTemp;
			
                AisStaticPara.Refresh = TRUE;
			}

			/*lq *Estimated month of arrival at destination, 00 to 12 (UTC)* */
			ucCheck = GetSegment((char *)pDat, chTemp, 7, uiLen, 30);

            /*lq 字段非空*/			
			if (ucCheck != FALSE)
			{
                /*lq 长度有效性判断*/
                ucLen = strlen(chTemp);
        		if (ucLen != 2)
        		{
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
        		}
        
        		/*lq 字符有效性判断*/
                ulTemp = strtoul(chTemp, &pEnd, 10);
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq 取值范围有效性判断*/
                    if (ulTemp < PARA_ETA_MONTH_FLOOR
                        || ulTemp > PARA_ETA_MONTH_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,VSD");
            			return (FALSE);
                    }
                }
                    
    			/*lq 写入EEPROM */
    			Para_Save(PARA_ETA_MONTH_TYPE, (U8 *)chTemp, ucLen);
    
    			/*lq 更新变量 */
    			AisStaticPara.ETAMonth = ulTemp;
			
                AisStaticPara.Refresh = TRUE;
			}

			/*lq **********Navigational status, 0 to 15********************* */
			ucCheck = GetSegment((char *)pDat, chTemp, 8, uiLen, 30);

            /*lq 字段非空*/			
            if (ucCheck != FALSE)
			{
        		/*lq 字符有效性判断*/
                ulTemp = (U32)strtod(chTemp, &pEnd);
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq 取值范围有效性判断*/
                    if (ulTemp < PARA_NAVIGATION_STATUS_FLOOR
                        || ulTemp > PARA_NAVIGATION_STATUS_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,VSD");
            			return (FALSE);
                    }
                }
    
    			/*lq 写入EEPROM */
                sprintf(ucTempBuf, "%d", ulTemp);
    			Para_Save(PARA_NAVIGATION_STATUS_TYPE, ucTempBuf, PARA_NAVIGATION_STATUS_MAXLEN);
    
    			/*lq 更新变量 */
    			AisStaticPara.NavigationStatus = ulTemp;

                AisStaticPara.Refresh = TRUE;
            }

			/*lq **********Regional application flags, 0 to 15************** */
			ucCheck = GetSegment((char *)pDat, chTemp, 9, uiLen, 30);

            /*lq 字段非空*/			
            if (ucCheck != FALSE)
			{
        		/*lq 字符有效性判断*/
                ulTemp = (U32)strtod(chTemp, &pEnd);
        		if ((*pEnd) != 0)
        		{
    				UartResponseMsg("$DUAIR,0,VSD");
        			return (FALSE);
        		}
                else
                {
                    /*lq 取值范围有效性判断*/
                    if (ulTemp < PARA_REGION_APP_FLAG_FLOOR
                        || ulTemp > PARA_REGION_APP_FLAG_CEIL)
                    {
        				UartResponseMsg("$DUAIR,0,VSD");
            			return (FALSE);
                    }
                }
    
    			/*lq 写入EEPROM */
                sprintf(ucTempBuf, "%d", ulTemp);
    			Para_Save(PARA_REGION_APP_FLAG_TYPE, ucTempBuf, PARA_REGION_APP_FLAG_MAXLEN);
    
    			/*lq 更新变量 */
    			AisStaticPara.RegionAppFlag = ulTemp;

                AisStaticPara.Refresh = TRUE;
            }

			UartResponseMsg("$DUAIR,1,VSD");           		
		}
	}
#endif

    	
    /*lq light beacon protocol message */
    else
    {
#if 0
        /*lq light beacon protocol message*/
        LB1_MsgDealProcess(pDat, uiLen);    
#endif
    }
    return (TRUE);
}

/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: gpiospi.h
  Author: lq       Version : V1.00        Date: 2014.07.01
  Description:     通过GPIO模拟spi
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "def.h"
#include "stm32f10x.h"
#include "gpiospi.h"
#include "Include.h"

/*lq 模块实例定义*/
static GpioSpiStruct GpioSpiInstance1;     //lq 实例1

/*******************************************************************************
* 名称  : GPIOSPI_StructInit
* 描述  : GpioSpiStruct结构体初始化
* 输入  : pGpioSpiStruct - GpioSpiStruct类型指针
          cpol - 时钟极性,
          cpha - 时钟相位,
          baudRatePrescaler - 波特率分频值,
          fistBit - 发送第一位,
          dataSize - 发送数据大小)		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void GPIOSPI_StructInit(GpioSpiStruct *pGpioSpiStruct,
                               GpioSpiCPOLEnum cpol,
                               GpioSpiCPHAEnum cpha,
                               U16 baudRatePrescaler,
                               GpioSpiFirstBitEnum fistBit,
                               GpioSpiDataSizeEnum dataSize)
{
    pGpioSpiStruct->SPI_CPOL = cpol;
    pGpioSpiStruct->SPI_CPHA = cpha;
    pGpioSpiStruct->SPI_BaudRatePrescaler = baudRatePrescaler;
    pGpioSpiStruct->SPI_FirstBit = fistBit;
    pGpioSpiStruct->SPI_DataSize = dataSize;
    pGpioSpiStruct->SPI_ClockCounter = 0;  
}

/*******************************************************************************
* 名称  : GPIOSPI_InstanceInit
* 描述  : GpioSpi实例初始化
* 输入  : GpioSpiTypeEnum - 实例类型
          cpol - 时钟极性,
          cpha - 时钟相位,
          baudRatePrescaler - 波特率分频值,
          fistBit - 发送第一位,
          dataSize - 发送数据大小)		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void GPIOSPI_InstanceInit(GpioSpiTypeEnum spiTye,
                          GpioSpiCPOLEnum cpol,
                          GpioSpiCPHAEnum cpha,
                          U16 baudRatePrescaler,
                          GpioSpiFirstBitEnum fistBit,
                          GpioSpiDataSizeEnum dataSize)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    if (spiTye == GPIOSPI1)
    {
        GpioSpiInstance1.InstanceNum = GPIOSPI1;
        GPIOSPI_StructInit(&GpioSpiInstance1, cpol, cpha, baudRatePrescaler, fistBit, dataSize);

        GPIO_InitStructure.GPIO_Pin = GPIOSPI1_CS_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
        GPIO_Init(GPIOSPI1_CS_PORT, &GPIO_InitStructure);
        GPIOSPI1_CS_HIGH();                                //lq 片选失能
        
        GPIO_InitStructure.GPIO_Pin = GPIOSPI1_SCLK_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
        GPIO_Init(GPIOSPI1_SCLK_PORT, &GPIO_InitStructure);
        if (GpioSpiInstance1.SPI_CPOL == GPIOSPI_CPOL_Low)
        {
            GPIOSPI1_SCLK_LOW();
        }
        else
        {
            GPIOSPI1_SCLK_HIGH();
        }
    
        GPIO_InitStructure.GPIO_Pin = GPIOSPI1_MOSI_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
        GPIO_Init(GPIOSPI1_MOSI_PORT, &GPIO_InitStructure);
        GPIOSPI1_MOSI_LOW();                               
    
        GPIO_InitStructure.GPIO_Pin = GPIOSPI1_MISO_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    
        GPIO_Init(GPIOSPI1_MISO_PORT, &GPIO_InitStructure);
    }
}

/*******************************************************************************
* 名称  : GPIOSPI_WaitClockPeriod
* 描述  : 时钟周期
* 输入  : pGpioSpiStruct - GpioSpiStruct类型指针		  
* 输出  : 无
* 返回  : TRUE - 时钟周期定时到，FALSE - 时钟周期定时未到
*******************************************************************************/
static U8 GPIOSPI_WaitClockPeriod(GpioSpiStruct *pGpioSpiStruct)
{
    if (pGpioSpiStruct->SPI_ClockCounter >= pGpioSpiStruct->SPI_BaudRatePrescaler)
    {
        return TRUE;
    }        
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : GPIOSPI_InstWaitClockPeriod
* 描述  : 时钟周期
* 输入  : spiTye - SPI类型		  
* 输出  : 无
* 返回  : TRUE - 时钟周期定时到，FALSE - 时钟周期定时未到
*******************************************************************************/
U8 GPIOSPI_InstWaitClockPeriod(GpioSpiTypeEnum spiTye)
{
    if (spiTye == GPIOSPI1)
    {
        return GPIOSPI_WaitClockPeriod(&GpioSpiInstance1);
    }
}

/*******************************************************************************
* 名称  : GPIOSPI_WaitClockPeriod
* 描述  : 时钟第一相位
* 输入  : pGpioSpiStruct - GpioSpiStruct类型指针		  
* 输出  : 无
* 返回  : TRUE - 时钟周期定时到，FALSE - 时钟周期定时未到
*******************************************************************************/
static U8 GPIOSPI_WaitClockPhase(GpioSpiStruct *pGpioSpiStruct)
{
    if (pGpioSpiStruct->SPI_ClockCounter >= (pGpioSpiStruct->SPI_BaudRatePrescaler >> 1))
    {
        return TRUE;
    }        
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : GPIOSPI_ResetClockPeriod
* 描述  : 复位时钟计数
* 输入  : pGpioSpiStruct - GpioSpiStruct类型指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void GPIOSPI_ResetClockCounter(GpioSpiStruct *pGpioSpiStruct)
{
    pGpioSpiStruct->SPI_ClockCounter = 0;
}

/*******************************************************************************
* 名称  : GPIOSPI_InstResetClockCounter
* 描述  : 复位时钟计数
* 输入  : spiTye - SPI类型			  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void GPIOSPI_InstResetClockCounter(GpioSpiTypeEnum spiTye)
{
    if (spiTye == GPIOSPI1)
    {
        GPIOSPI_ResetClockCounter(&GpioSpiInstance1);
    }
}

/*******************************************************************************
* 名称  : GPIOSPI_ClockCount
* 描述  : 时钟计数
* 输入  : pGpioSpiStruct - GpioSpiStruct类型指针			  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void GPIOSPI_ClockCount(GpioSpiStruct *pGpioSpiStruct)
{
    if (pGpioSpiStruct->SPI_ClockCounter < pGpioSpiStruct->SPI_BaudRatePrescaler)    
    {
        pGpioSpiStruct->SPI_ClockCounter++;
    }
}

/*******************************************************************************
* 名称  : GPIOSPI_InstClockCount
* 描述  : 时钟计数
* 输入  : spiType - SPI类型			  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void GPIOSPI_InstClockCount(GpioSpiTypeEnum spiType)
{
    if (spiType == GPIOSPI1)
    {
        GPIOSPI_ClockCount(&GpioSpiInstance1);
    }
}

#if 0
/*******************************************************************************
* 名称  : GPIOSPI_SendData
* 描述  : 发送数据
* 输入  : pGpioSpiStruct - GpioSpiStruct类型指针
          data - 发送数据			  
* 输出  : 无
* 返回  : returnData - 接收数据
*******************************************************************************/
static U16 GPIOSPI_SendData(GpioSpiStruct *pGpioSpiStruct, U16 data)
{
    U8  dataSize;
    U16 dataMask;
    U16 i;
    U16 returnData;

    returnData = 0;

    if (pGpioSpiStruct->SPI_DataSize == GPIOSPI_DataSize_8b)
    {
        dataSize = 8;
        if (pGpioSpiStruct->SPI_FirstBit == GPIOSPI_FirstBit_MSB)
        {
            dataMask = 0x0080; 
        }
        else
        {
            dataMask = 0x0001;             
        }
    }
    else
    {
        dataSize = 16;
        if (pGpioSpiStruct->SPI_FirstBit == GPIOSPI_FirstBit_MSB)
        {
            dataMask = 0x8000; 
        }
        else
        {
            dataMask = 0x0001;             
        }
    }

    if (pGpioSpiStruct->InstanceNum == GPIOSPI1)    
    {
        /*lq 片选使能 */
        if (pGpioSpiStruct->SPI_CPOL == GPIOSPI_CPOL_Low)
        {
            GPIOSPI1_SCLK_LOW();
        }
        else
        {
            GPIOSPI1_SCLK_HIGH();        
        }
        GPIOSPI1_CS_LOW();
    
        /*lq 发送数据 */
        for (i = 0; i < dataSize; i++)
        {
            if (pGpioSpiStruct->SPI_CPHA == GPIOSPI_CPHA_1Edge)
            {
                /*lq 提供发送数据 */
                if ((data & dataMask) == dataMask)
                {
                    GPIOSPI1_MOSI_HIGH();   //lq bit = 1;
                }
                else
                {
                    GPIOSPI1_MOSI_LOW();    //lq bit = 0;
                }
                
                /*lq 提供时钟沿1 */ 
                if (pGpioSpiStruct->SPI_CPOL == GPIOSPI_CPOL_Low)
                {
                    GPIOSPI1_SCLK_HIGH();                    
                }
                else
                {
                    GPIOSPI1_SCLK_LOW();
                }
                GPIOSPI_ResetClockCounter(pGpioSpiStruct);

                while (GPIOSPI_WaitClockPhase(pGpioSpiStruct) == FALSE);            
                
                /*lq 获取返回数据（在提供时钟边沿2之前） */
                returnData <<= 1;
                returnData |= GPIOSPI1_MISO_IN_READ();

                /*lq 提供时钟沿2 */ 
                if (pGpioSpiStruct->SPI_CPOL == GPIOSPI_CPOL_Low)
                {
                    GPIOSPI1_SCLK_LOW();
                }
                else
                {
                    GPIOSPI1_SCLK_HIGH();        
                }
                
                while (GPIOSPI_WaitClockPeriod(pGpioSpiStruct) == FALSE);                            

                /*lq 发送数据移位 */
                if (pGpioSpiStruct->SPI_FirstBit == GPIOSPI_FirstBit_MSB)
                {
                    data <<= 1; 
                }
                else
                {
                    data >>= 1;             
                }
            }
            else
            {            
                /*lq 提供时钟沿1 */ 
                if (pGpioSpiStruct->SPI_CPOL == GPIOSPI_CPOL_Low)
                {
                    GPIOSPI1_SCLK_HIGH();                    
                }
                else
                {
                    GPIOSPI1_SCLK_LOW();
                }
                GPIOSPI_ResetClockCounter(pGpioSpiStruct);

                while (GPIOSPI_WaitClockPhase(pGpioSpiStruct) == FALSE);            
                
                /*lq 提供发送数据 */
                if ((data & dataMask) == dataMask)
                {
                    GPIOSPI1_MOSI_HIGH();   //lq bit = 1;
                }
                else
                {
                    GPIOSPI1_MOSI_LOW();    //lq bit = 0;
                }

                /*lq 提供时钟沿2 */ 
                if (pGpioSpiStruct->SPI_CPOL == GPIOSPI_CPOL_Low)
                {
                    GPIOSPI1_SCLK_LOW();
                }
                else
                {
                    GPIOSPI1_SCLK_HIGH();        
                }
                                                               
                while (GPIOSPI_WaitClockPeriod(pGpioSpiStruct) == FALSE);                            

                /*lq 获取返回数据（在提供时钟边沿1之前） */
                returnData <<= 1;
                returnData |= GPIOSPI1_MISO_IN_READ();

                /*lq 发送数据移位 */
                if (pGpioSpiStruct->SPI_FirstBit == GPIOSPI_FirstBit_MSB)
                {
                    data <<= 1; 
                }
                else
                {
                    data >>= 1;             
                }            
            }         
        }

        /*lq 片选失能 */
        GPIOSPI1_CS_HIGH();        
    }

    return returnData;
}
#else
/*******************************************************************************
* 名称  : GPIOSPI_SendData
* 描述  : 发送数据（修改设置时钟沿和读写数据位的顺序）
* 输入  : pGpioSpiStruct - GpioSpiStruct类型指针
          data - 发送数据			  
* 输出  : 无
* 返回  : returnData - 接收数据
*******************************************************************************/
static U16 GPIOSPI_SendData(GpioSpiStruct *pGpioSpiStruct, U16 data)
{
    U8  dataSize;
    U16 dataMask;
    U16 i;
    U16 returnData;

    returnData = 0;

    if (pGpioSpiStruct->SPI_DataSize == GPIOSPI_DataSize_8b)
    {
        dataSize = 8;
        if (pGpioSpiStruct->SPI_FirstBit == GPIOSPI_FirstBit_MSB)
        {
            dataMask = 0x0080; 
        }
        else
        {
            dataMask = 0x0001;             
        }
    }
    else
    {
        dataSize = 16;
        if (pGpioSpiStruct->SPI_FirstBit == GPIOSPI_FirstBit_MSB)
        {
            dataMask = 0x8000; 
        }
        else
        {
            dataMask = 0x0001;             
        }
    }

    if (pGpioSpiStruct->InstanceNum == GPIOSPI1)    
    {
        /*lq 片选使能 */
        if (pGpioSpiStruct->SPI_CPOL == GPIOSPI_CPOL_Low)
        {
            GPIOSPI1_SCLK_LOW();
        }
        else
        {
            GPIOSPI1_SCLK_HIGH();        
        }
        GPIOSPI1_CS_LOW();
    
        /*lq 发送数据 */
        for (i = 0; i < dataSize; i++)
        {
            if (pGpioSpiStruct->SPI_CPHA == GPIOSPI_CPHA_1Edge)
            {
                /*lq 提供发送数据位 */
                if ((data & dataMask) == dataMask)
                {
                    GPIOSPI1_MOSI_HIGH();   //lq bit = 1;
                }
                else
                {
                    GPIOSPI1_MOSI_LOW();    //lq bit = 0;
                }

                GPIOSPI_ResetClockCounter(pGpioSpiStruct);

                while (GPIOSPI_WaitClockPhase(pGpioSpiStruct) == FALSE);
                                
                /*lq 提供时钟沿1（指示从设备采样） */ 
                if (pGpioSpiStruct->SPI_CPOL == GPIOSPI_CPOL_Low)
                {
                    GPIOSPI1_SCLK_HIGH();                    
                }
                else
                {
                    GPIOSPI1_SCLK_LOW();
                }
                            
                /*lq 获取返回数据（主设备采样） */
                returnData <<= 1;
                returnData |= GPIOSPI1_MISO_IN_READ();

                while (GPIOSPI_WaitClockPeriod(pGpioSpiStruct) == FALSE);                            

                /*lq 提供时钟沿2（指示从设备发送新数据位）*/ 
                if (pGpioSpiStruct->SPI_CPOL == GPIOSPI_CPOL_Low)
                {
                    GPIOSPI1_SCLK_LOW();
                }
                else
                {
                    GPIOSPI1_SCLK_HIGH();        
                }
                
                /*lq 发送数据移位 */
                if (pGpioSpiStruct->SPI_FirstBit == GPIOSPI_FirstBit_MSB)
                {
                    data <<= 1; 
                }
                else
                {
                    data >>= 1;             
                }
            }
            else
            {            
                /*lq 提供时钟沿1（指示从设备发送新数据位） */ 
                if (pGpioSpiStruct->SPI_CPOL == GPIOSPI_CPOL_Low)
                {
                    GPIOSPI1_SCLK_HIGH();                    
                }
                else
                {
                    GPIOSPI1_SCLK_LOW();
                }
                
                /*lq 提供发送数据位 */
                if ((data & dataMask) == dataMask)
                {
                    GPIOSPI1_MOSI_HIGH();   //lq bit = 1;
                }
                else
                {
                    GPIOSPI1_MOSI_LOW();    //lq bit = 0;
                }

                GPIOSPI_ResetClockCounter(pGpioSpiStruct);

                while (GPIOSPI_WaitClockPhase(pGpioSpiStruct) == FALSE);            
                
                /*lq 提供时钟沿2（指示从设备采样） */ 
                if (pGpioSpiStruct->SPI_CPOL == GPIOSPI_CPOL_Low)
                {
                    GPIOSPI1_SCLK_LOW();
                }
                else
                {
                    GPIOSPI1_SCLK_HIGH();        
                }

                /*lq 获取返回数据（主设备采样） */
                returnData <<= 1;
                returnData |= GPIOSPI1_MISO_IN_READ();
                                                                               
                while (GPIOSPI_WaitClockPeriod(pGpioSpiStruct) == FALSE);                            

                /*lq 发送数据移位 */
                if (pGpioSpiStruct->SPI_FirstBit == GPIOSPI_FirstBit_MSB)
                {
                    data <<= 1; 
                }
                else
                {
                    data >>= 1;             
                }            
            }         
        }

        /*lq 片选失能 */
        GPIOSPI1_CS_HIGH();        
    }

    return returnData;
}
#endif
/*******************************************************************************
* 名称  : GPIOSPI_ResetClockPeriod
* 描述  : 复位时钟计数
* 输入  : spiTye - SPI类型
          data - 发送数据      			  
* 输出  : 无
* 返回  : 接收数据
*******************************************************************************/
U16 GPIOSPI_InstSendData(GpioSpiTypeEnum spiTye, U16 data)
{
    if (spiTye == GPIOSPI1)
    {
        return GPIOSPI_SendData(&GpioSpiInstance1, data);
    }
}
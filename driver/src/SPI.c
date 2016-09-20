/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: SPI.c
  Author: lq       Version : V1.00        Date: 2014.05.09
  Description:     SPI
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "stm32f10x.h"
#include "def.h"
#include "SPI.h"
#include "Include.h"

/*lq SPI通信结构体定义 */
#if 0
SPITxFIFOStruct SPI2TxFIFO;
SPIRxFIFOStruct SPI2RxFIFO;
#endif

/*******************************************************************************
* 名称  : SPI2_Init
* 描述  : 初始化SPI2
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPI2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    /* Configure SPI2 pins: NSS为普通IO ---------------------------------*/
    GPIO_InitStructure.GPIO_Pin = SPI2_NSS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(SPI2_PORT, &GPIO_InitStructure); 
    SPI2_NSS_HIGH();             
    
    /* Configure SPI2 pins: SCK, MISO and MOSI ---------------------------------*/
    GPIO_InitStructure.GPIO_Pin = SPI2_SCK_PIN | SPI2_MISO_PIN | SPI2_MOSI_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    
    GPIO_Init(SPI2_PORT, &GPIO_InitStructure);  

    /* SPI2 Full-duplex Config -------------------------------------------------------------*/
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                   //lq CMX7042的寄存器为16位，寄存器地址为8为，对1个CMX7042寄存器读写需两次SPI传输
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; //lq SPI 时钟频率为36M/X(CMX7042允许的最大时钟频率为5MHz)
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);

    /*lq NSS引脚输出使能 */
    //lq SPI_SSOutputCmd(SPI2, ENABLE);

    /* Disable SPI2 TXE, RXNE interrupt */
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);
    
    /* Disable SPI2 */
    SPI_Cmd(SPI2, ENABLE);     //lq SPI_Cmd(SPI2, DISABLE);
}

/*******************************************************************************
* 名称  : SPI2_SendByte
* 描述  : 使用SPI2读或写一个字节的数据
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 SPI2_SendByte(U8 byte)
{
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

	/* Send byte through the SPI2 peripheral */
	SPI_I2S_SendData(SPI2, byte);

	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI2);
}

/*******************************************************************************
* 名称  : GPIO_SPI1_ExchangeByte
* 描述  : 使用SPI1发送和接收一个字节的数据，上升沿采样（SPI_CPOL = SPI_CPOL_Low，
          SPI_CPHA = SPI_CPHA_1Edge），MSB优先
* 输入  : sendData - 发送字节          
* 输出  : 无
* 返回  : receivedData - 接收字节
*******************************************************************************/
U8 GPIO_SPI1_ExchangeByte(U8 sendData)
{
    U16 i;
    U8  receivedData = 0;

    GPIO_SPI1_SCLK_LOW();    
    for (i = 0; i < 8; i++)
    {    
        /*lq send data */
        if ((sendData & 0x80) != 0)     //lq MSB first
        {
            GPIO_SPI1_SDI_HIGH();        
        }
        else
        {
            GPIO_SPI1_SDI_LOW();        
        }        
        GPIO_SPI1_SCLK_HIGH();                       
        sendData <<= 1;

        /*lq receive data */
        receivedData <<= 1;
        if (GPIO_SPI1_SDO_IN() != 0)
        {
            receivedData |= 1;
        }

        GPIO_SPI1_SCLK_LOW();
    }

    return receivedData;
}

/*******************************************************************************
* 名称  : GPIO_SPI2_ExchangeByte
* 描述  : 使用SPI1发送和接收一个字节的数据，上升沿采样（SPI_CPOL = SPI_CPOL_Low，
          SPI_CPHA = SPI_CPHA_1Edge），MSB优先
* 输入  : sendData - 发送字节          
* 输出  : 无
* 返回  : receivedData - 接收字节
*******************************************************************************/
U8 GPIO_SPI2_ExchangeByte(U8 sendData)
{
    U16 i;
    U8  receivedData = 0;

    GPIO_SPI2_SCLK_LOW();    
    for (i = 0; i < 8; i++)
    {    
        /*lq send data */
        if ((sendData & 0x80) != 0)     //lq MSB first
        {
            GPIO_SPI2_SDI_HIGH();        
        }
        else
        {
            GPIO_SPI2_SDI_LOW();        
        }        
        GPIO_SPI2_SCLK_HIGH();                       
        sendData <<= 1;

        /*lq receive data */
        receivedData <<= 1;
        if (GPIO_SPI2_SDO_IN() != 0)
        {
            receivedData |= 1;
        }

        GPIO_SPI2_SCLK_LOW();
    }

    return receivedData;
}

#if 0
/*******************************************************************************
* 名称  : SPITxFIFOStructInit
* 描述  : 结构体初始化
* 输入  : pSPITxFIFOStruct-结构体指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPITxFIFOStructInit(SPITxFIFOStruct* pSPITxFIFOStruct)
{
	pSPITxFIFOStruct->TxFIFOInput = 0;
	pSPITxFIFOStruct->TxFIFOOutput = pSPITxFIFOStruct->TxFIFOInput;
	pSPITxFIFOStruct->TxState = FALSE;
}

/*******************************************************************************
* 名称  : SPIRxFIFOStructInit
* 描述  : 结构体初始化
* 输入  : pSPIRxFIFOStruct-结构体指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPIRxFIFOStructInit(SPIRxFIFOStruct* pSPIRxFIFOStruct)
{
	pSPIRxFIFOStruct->RxFIFOInput = 0;                                        //lq 行标
	pSPIRxFIFOStruct->RxFIFOOutput = pSPIRxFIFOStruct->RxFIFOInput;                                       
	pSPIRxFIFOStruct->RxEnFlg = FALSE;
	pSPIRxFIFOStruct->RxBuffIndex = 0;                                        //lq 列标
	pSPIRxFIFOStruct->Rx0x0dFlg = FALSE;	
    pSPIRxFIFOStruct->Rx0xb5Flg = FALSE;
	pSPIRxFIFOStruct->DoubleFlg = FALSE;
}

/*******************************************************************************
* 名称  : SPIRxFIFOStructInit
* 描述  : 结构体初始化
* 输入  : pSPIRxFIFOStruct-结构体指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPICommStructInit(U8 SPIInterface)
{
	if(SPIInterface == SPI_INTERFACE_2)
    {
        SPITxFIFOStructInit(&SPI2TxFIFO);
        SPIRxFIFOStructInit(&SPI2RxFIFO);    
    }
}

/*******************************************************************************
* 名称  : SPI2Interrupt
* 描述  : SPI2中断
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPI2Interrupt(void)
{

}

/*******************************************************************************
* 名称 ：SPITxIntProcess
* 描述 ：SPI发送过程，由串口发送中断调用
* 输入 ：pSPITxFIFOStruct：SPI发送结构体指针
* 输出 ：无
* 返回 ：无
*******************************************************************************/
void SPITxIntProcess(SPITxFIFOStruct *pSPITxFIFOStruct)
{
	pSPITxFIFOStruct->TxFIFOOutput++;
	pSPITxFIFOStruct->TxFIFOOutput &= SPI_TX_FIFO_DEPTH - 1;                    //lq 缓冲指针循环（模UART_TX_FIFO_DEPTH - 1取余）
		
	if(pSPITxFIFOStruct->TxFIFOOutput != pSPITxFIFOStruct->TxFIFOInput)         //lq 缓冲区（数据结构）不为空，有数据要发送
	{	
		pSPITxFIFOStruct->TxState = TRUE;	                                    //lq 缓冲区内有数据	

		if (pSPITxFIFOStruct == &SPI2TxFIFO)                                    //lq 如果是SPI2
		{
			SPI_I2S_SendData(SPI2, pSPITxFIFOStruct->TxFIFO[pSPITxFIFOStruct->TxFIFOOutput]); //lq 向SPI2的TDR写数据			
		}												
	}
	else                                                                        //lq 缓冲区（数据结构）无数据
	{
		if (pSPITxFIFOStruct == &SPI2TxFIFO)                                    //lq 如果是SPI2
	  	{
            SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);                    //lq 发送缓冲区（一个寄存器）空中断失能，
            SPI_Cmd(SPI2, DISABLE);			                                    //lq 关闭SPI2
            
            pSPITxFIFOStruct->TxState  = FALSE;                                 //lq 避免频繁的中断提醒，此时改由函数UartTxMainProcess()和
		                                                                        //lq 状态标志pUartTxFIFO->TxState循环检测有无数据要发送	  	
	    }
    }
}

/*******************************************************************************
* 名称 ：SPITxMainProcess
* 描述 ：SPI发送启动。在TXE中断失能的情况下，判断有无数据要发
* 输入 ：pSPITxFIFOStruct：SPI发送结构体指针
* 输出 ：无
* 返回 ：无
*******************************************************************************/
void SPITxMainProcess(SPITxFIFOStruct *pSPITxFIFOStruct)                       
{
	if(pSPITxFIFOStruct->TxState == FALSE)	//空闲                                  //lq SPI发送当前空
	{
		if(pSPITxFIFOStruct->TxFIFOInput != pSPITxFIFOStruct->TxFIFOOutput)         //lq 有数据可发
		{
			pSPITxFIFOStruct->TxState = TRUE;	//忙                                //lq 当前正发送数据
			
            if (pSPITxFIFOStruct == &SPI2TxFIFO)
			{
				/*lq 启动SPI2 */
                SPI_Cmd(SPI2, ENABLE);
                SPI_I2S_SendData(SPI2, pSPITxFIFOStruct->TxFIFO[pSPITxFIFOStruct->TxFIFOOutput]);
				SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, ENABLE); 
			}            							
		}
	}
}

/*******************************************************************************
* 名称 ：SPITxDataBuf
* 描述 ：SPI发送缓存数据
* 输入 ：SPIInterface：SPI接口标志
		 pData：数据缓存指针
		 Length：数据长度
* 输出 ：无
* 返回 ：无
*******************************************************************************/
U8 SPITxDataBuf(U8 SPIInterface, U8 *pData, U16 Length)
{				
	U16 i, lastcnt;
	U8 uchRet;
	SPITxFIFOStruct *pSPITxFIFO;
	
	if(SPIInterface == SPI_INTERFACE_2)
	{
		pSPITxFIFO = &SPI2TxFIFO;
	}
    else
    {
        return FALSE;
    }
	
	lastcnt = (SPI_TX_FIFO_DEPTH - 1) 
		- ((pSPITxFIFO->TxFIFOInput - pSPITxFIFO->TxFIFOOutput)                 //lq 剩余空间（字节）数 
		& (SPI_TX_FIFO_DEPTH - 1));                                             //lq?
	if (lastcnt >= Length)                                                      //lq 剩余空间够
	{
		for (i = 0; i < Length; i++)
		{			
			pSPITxFIFO->TxFIFO[pSPITxFIFO->TxFIFOInput++] = pData[i];
			
			pSPITxFIFO->TxFIFOInput &= (SPI_TX_FIFO_DEPTH - 1);		
		}	
		
		uchRet = TRUE;		
	}                                                                           
	else                                                                        //lq 剩余空间不够
	{
		pSPITxFIFO->TxFIFO[pSPITxFIFO->TxFIFOInput++] = 0xFF;                   //lq?
		pSPITxFIFO->TxFIFOInput &= (SPI_TX_FIFO_DEPTH - 1);		
		
        uchRet = FALSE;
	}
	
	UartTxMainProcess(pSPITxFIFO);                                             //lq 主动发起第一个数据的发送（因为每次发送完所有数据后TXE中断被失能），该次发送后续数据的发送通过TXE中断完成

	return uchRet;	
}

/*******************************************************************************
* 名称 ：SPIRxMainProcess
* 描述 ：SPI发送启动。在TXE中断失能的情况下，判断有无数据要发
* 输入 ：pSPITxFIFOStruct：SPI发送结构体指针
* 输出 ：无
* 返回 ：无
*******************************************************************************/
U8 SPIRxMainProcess(U8 SPIInterface)                       
{
	SPIRxFIFOStruct *pSPIRxFIFO;

	if(SPIInterface == SPI_INTERFACE_2)
	{
		pSPIRxFIFO = &SPI2RxFIFO;
	}
    else
    {
        return FALSE;
    }

	if(((pSPIRxFIFO->RxFIFOInput + 1) % SPI_RX_FIFO_DEPTH)
       != pSPIRxFIFO->RxFIFOOutput)                                     //lq 有接收空间
	{
		pSPIRxFIFO->RxEnFlg = TRUE;	                                    //lq 接收使能
		
        if (pSPIRxFIFO == &SPI2RxFIFO)
		{
			/*lq 启动SPI2 */
            SPI_Cmd(SPI2, ENABLE);
			SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE); 
		}            							
	}
}
#endif

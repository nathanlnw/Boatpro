/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: Usart.c
  Author: vozhj       Version : V1.00        Date: 2011.09.23
  Description:    串口底层驱动和应用处理程序
  Version:         V1.00
  Function List:   // 主要函数及其功能    	 
			1．函数名：InitUart0
    	   	   函数功能：初始化串口0
 
			2．函数名：Uart0Interrupt
    	   	   函数功能：串口0
    	   	 
    	   	3．函数名：UartTxMainProcess
    	   	   函数功能：串口发送程序
    	   	   
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "stm32f10x.h"
#include "def.h"
#include "usart.h"
#include "common.h"
#include "gps.h"
#include "uartmsg.h"
#include "gpsctrl.h"
#include "UbloxProtocol.h"  

/* Private variables ---------------------------------------------------------*/
struct UartTxFIFOStruct Uart1TxFIFO;
struct UartRxFIFOStruct Uart1RxFIFO;

UsartConfigStruct Uart1Config;

/* Private  function  ---------------------------------------------------------*/
void InitUsart1(U32 baud);
void UartTxIntProcess(struct UartTxFIFOStruct *pUartTxFIFO);
void UartRxProcess(struct UartRxFIFOStruct *pUartRxFIFOStruct, U8 data);

/*******************************************************************************
* 名称  : UsartConfigStructInit
* 描述  : 结构体初始化
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void UsartConfigStructInit(UsartConfigStruct *pUsartConfigStruct, U32 Baud, U8 Data, U8 Parity, U8 Stop)
{
    pUsartConfigStruct->Baud = Baud;
    pUsartConfigStruct->Data = Data;
    pUsartConfigStruct->Parity = Parity;
    pUsartConfigStruct->Stop = Stop;
}

/*******************************************************************************
* 名称  : USART1_UsartConfigStructInit
* 描述  : USART1结构体初始化
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void USART1_UsartConfigStructInit(U32 Baud, U8 Data, U8 Parity, U8 Stop)
{
    UsartConfigStructInit(&Uart1Config, Baud, Data, Parity, Stop);
}

/*******************************************************************************
* 名称  : Usart1Interrupt
* 描述  : 串口1中断服务程序
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void Usart1Interrupt(void)	 //串口中断函数                                     //lq 发送缓冲区（一个寄存器）空（表明可以发送，写数据） or   
{                                                                               //lq 接收缓冲区（一个寄存器）非空（表明可以接收，读数据）
	U8 Data;
	
	//溢出——如果发生溢出需要先读SR，再读DR寄存器，则可消除不断入中断的问题
	if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET)
	{
	 	USART_ClearFlag(USART1,USART_FLAG_ORE);			// 读SR其实就是消除标志
		USART_ReceiveData(USART1);						// 读DR
	}                                                                    //lq 时触发该中断

	//接收中断   判断是那一种中断
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)                       //lq 可接收（可读）中断
	{
		
		/* Read one byte from the receive data register */
		Data = USART_ReceiveData(USART1);
		
		UartRxProcess(&Uart1RxFIFO, Data);
//		USART_SendData(USART1, Data);
        
        /*lq MCU处于休眠时，用于唤醒MCU*/
        SetUsartFlg();	
	}
		
	//发送中断
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)                        //lq 可发送（可写）中断
	{   
		UartTxIntProcess(&Uart1TxFIFO);
	}
}

/*******************************************************************************
* 名称  : InitUart1
* 描述  : 初始化串口1
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitUsart1(U32 baud) 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USART1 configuration ------------------------------------------------------*/
	/* USART1 configured as follow:
		- BaudRate = 38400 baud  
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure USART1 */
  	USART_Init(USART1, &USART_InitStructure);

	/* Enable USART1 Receive and Transmit interrupts */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //使能串口中断               //lq 接收缓冲区非空中断使能，可以接收数据
	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);                              //lq 发送缓冲区空中断失能，不可发送数据
	

	/* Enable the USART1 */
  	USART_Cmd(USART1, ENABLE);
	//InitCommStruc(COMM1, 38400);           
}

/*******************************************************************************
* 名称  : Usart1_UsartConfig
* 描述  : 配置Usart1
* 输入  : Baud - 波特率
          Data - 数据位位数
          Parity - 奇偶校验方式
          Stop - 停止位位数
* 输出  : 无
* 返回  : FALSE：失败；TRUE：成功
*******************************************************************************/
U8 Usart1_UsartConfig(U32 Baud, U8 Data, U8 Parity, U8 Stop)
{
	USART_InitTypeDef USART_InitStructure;
    U16 WordLength;
    U16 StopBits;
    U16 ParitySelect;

	/* USART1 configuration ------------------------------------------------------*/
	/* USART1 configured as follow:
		- BaudRate = 38400 baud  
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
	*/

    if (Data == 8)
    {
        WordLength = USART_WordLength_8b;    
    }
    else if (Data == 9)
    {
        WordLength = USART_WordLength_9b;        
    }
    else
    {
        return FALSE;    
    }

    if (Parity == 0)
    {
        ParitySelect = USART_Parity_No;
    }
    else if (Parity == 1)
    {
        ParitySelect = USART_Parity_Even;    
    }
    else if (Parity == 2)
    {
        ParitySelect = USART_Parity_Odd;        
    }
    else
    {
        return FALSE;    
    }

    if (Stop == 1)
    {
        StopBits = USART_StopBits_1;    
    }
    else if (Stop == 2)
    {
        StopBits = USART_StopBits_2;        
    }
    else if (Stop == 3)
    {
        StopBits = USART_StopBits_0_5;        
    }
    else if (Stop == 4)
    {
        StopBits = USART_StopBits_1_5;        
    }
    else
    {
        return FALSE;    
    }

	USART_InitStructure.USART_BaudRate = Baud;
	USART_InitStructure.USART_WordLength = WordLength;
	USART_InitStructure.USART_StopBits = StopBits;
	USART_InitStructure.USART_Parity = ParitySelect;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure USART1 */
  	USART_Init(USART1, &USART_InitStructure);

	/* Enable USART1 Receive and Transmit interrupts */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);	

	/* Enable the USART1 */
  	USART_Cmd(USART1, ENABLE);  

    return TRUE;
}

/*******************************************************************************
* 名称  : UartTxFIFOStructInit
* 描述  : 结构体初始化
* 输入  : putCell-UartTxFIFOStruct类型指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void UartTxFIFOStructInit(struct UartTxFIFOStruct *putCell)
{
	putCell->TxFIFOInput = 0;
	putCell->TxFIFOOutput = 0;
	putCell->TxState = FALSE;
}

/*******************************************************************************
* 名称  : UartRxFIFOStructInit
* 描述  : 结构体初始化
* 输入  : pUartRxFIFO-UartRxFIFOStruct类型指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void UartRxFIFOStructInit(struct UartRxFIFOStruct *pUartRxFIFO)
{
	pUartRxFIFO->RxFIFOInput = 0;
	pUartRxFIFO->RxFIFOOutput = 0;
	pUartRxFIFO->RxEnFlg = FALSE;
	pUartRxFIFO->RxBuffIndex = 0;
	pUartRxFIFO->Rx0x0dFlg = FALSE;
    pUartRxFIFO->Rx0xb5Flg = FALSE;	
	pUartRxFIFO->DoubleFlg = FALSE;
}

/*********************************************************************************************************
;** 函数名称: InitCommStruc
;** 功能描述: 初始化串口发射结构体
;** 参    数: Comm : Comm地址
;** 返 回 值: 无
;********************************************************************************************************/
U8 InitCommStruc(U8 Comm, U32 band) 
{
	struct UartTxFIFOStruct *putCell;
	struct UartRxFIFOStruct *pUartRxFIFO;
	
	if(Comm == COMM1)
	{
		UartTxFIFOStructInit(&Uart1TxFIFO);
        UartRxFIFOStructInit(&Uart1RxFIFO);
        InitUsart1(band);
        USART1_UsartConfigStructInit(band, 8, 0, 1);
	}
	else if(Comm == COMM2)
	{
		UartTxFIFOStructInit(&Uart2TxFIFO);
        UartRxFIFOStructInit(&Uart2RxFIFO);
		InitUsart2(band);
	}
	else
	{
		return(COMM_ERR);
	}

	return(COMM_NO_ERR);
}

/*******************************************************************************
* 名称 ：UartTxIntProcess
* 描述 ：串口发送过程，由串口发送中断调用
* 输入 ：pUartTxFIFO：串口发送结构体指针
			&Uart0TxFIFO：
			&Uart1TxFIFO：
			&Uart2TxFIFO：
* 输出 ：无
* 返回 ：无
*******************************************************************************/
void UartTxIntProcess(struct UartTxFIFOStruct *pUartTxFIFO)
{
	pUartTxFIFO->TxFIFOOutput++;
	pUartTxFIFO->TxFIFOOutput &= UART_TX_FIFO_DEPTH - 1;//指针回到头            //lq 缓冲指针循环（模UART_TX_FIFO_DEPTH - 1取余）
		
	if(pUartTxFIFO->TxFIFOOutput != pUartTxFIFO->TxFIFOInput)             //lq 缓冲区（数据结构）不为空，有数据要发送
	{	
		pUartTxFIFO->TxState = TRUE;	//缓冲区内有数据	

		if (pUartTxFIFO == &Uart1TxFIFO)//如果是串口1
		{
			USART_SendData(USART1, pUartTxFIFO->TxFIFO[pUartTxFIFO->TxFIFOOutput]); //lq 向USART1的TDR写数据			
		}
		if(pUartTxFIFO == &Uart2TxFIFO)
		{
			USART_SendData(USART2, pUartTxFIFO->TxFIFO[pUartTxFIFO->TxFIFOOutput]);
		}
													
	}
	else                                                                        //lq 缓冲区（数据结构）无数据
	{
		if (pUartTxFIFO == &Uart1TxFIFO)//如果是串口1
	  	{
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);                      //lq 发送缓冲区（一个寄存器）空中断失能，
			pUartTxFIFO->TxState  = FALSE;	//空闲                              //lq 避免频繁的中断提醒，此时改由函数UartTxMainProcess()和
		}                                                                       //lq 状态标志pUartTxFIFO->TxState循环检测有无数据要发送
		if (pUartTxFIFO == &Uart2TxFIFO)//如果是串口1
		{
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
			pUartTxFIFO->TxState  = FALSE;	//空闲
		}
	  	
	}
}

/*******************************************************************************
* 名称 ：UartTxMainProcess
* 描述 ：串口发送启动
* 输入 ：pUartTxFIFO：串口发送结构体指针
			&Uart0TxFIFO：
			&Uart1TxFIFO：
			&Uart2TxFIFO：
* 输出 ：无
* 返回 ：无
*******************************************************************************/
void UartTxMainProcess(struct UartTxFIFOStruct *pUartTxFIFO)                    //在TXE中断失能的情况下，判断有无数据要发
{
	if(pUartTxFIFO->TxState == FALSE)	//空闲                                  //lq 串口发送当前空
	{
		if(pUartTxFIFO->TxFIFOInput != pUartTxFIFO->TxFIFOOutput)               //lq 有数据可发
		{
			pUartTxFIFO->TxState = TRUE;	//忙                                //lq 当前正发送数据
			
			if (pUartTxFIFO == &Uart1TxFIFO)
			{
				USART_SendData(USART1, pUartTxFIFO->TxFIFO[pUartTxFIFO->TxFIFOOutput]);	
				USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 
			}
			else if (pUartTxFIFO == &Uart2TxFIFO)
			{
				USART_SendData(USART2, pUartTxFIFO->TxFIFO[pUartTxFIFO->TxFIFOOutput]);
				USART_ITConfig(USART2, USART_IT_TXE, ENABLE); 
			}            							
		}
	}
}

/*******************************************************************************
* 名称 ：UartTxString
* 描述 ：串口发送字符串
* 输入 ：COMM：串口地址标志
			COMM1：
		 pStr：字符串指针
		 Length：字符串长度
* 输出 ：无
* 返回 ：无
*******************************************************************************/
U8 UartTxDataBuf(U8 COMM, U8 *pStr,U16 Length)
{				
	U16 i, lastcnt;
	U8 uchRet;
	struct UartTxFIFOStruct *pUartTxFIFO;
	
	if(COMM == COMM1)
	{
		pUartTxFIFO = &Uart1TxFIFO;
	}
	else if ((COMM == COMM2))
	{
		pUartTxFIFO = &Uart2TxFIFO;
	}
	
	lastcnt = (UART_TX_FIFO_DEPTH - 1) 
		- ((pUartTxFIFO->TxFIFOInput - pUartTxFIFO->TxFIFOOutput)               //lq 剩余空间（字节）数 
		& (UART_TX_FIFO_DEPTH - 1));                                            //lq?
	if (lastcnt >= Length)                                                      //lq 剩余空间够
	{
		for (i = 0; i < Length; i++)
		{			
			pUartTxFIFO->TxFIFO[pUartTxFIFO->TxFIFOInput++] = pStr[i];
			
			pUartTxFIFO->TxFIFOInput &= (UART_TX_FIFO_DEPTH - 1);		
		}	
		
		uchRet = TRUE;		
	}                                                                           
	else                                                                        //lq 剩余空间不够
	{
		pUartTxFIFO->TxFIFO[pUartTxFIFO->TxFIFOInput++] = 0xFF;                 //lq?
		pUartTxFIFO->TxFIFOInput &= (UART_TX_FIFO_DEPTH - 1);		
		uchRet = FALSE;
	}
	
	UartTxMainProcess(pUartTxFIFO);

	return uchRet;	
}

#if 0
/*******************************************************************************
* 名称 ：UartRxProcess
* 描述 ：串口接收单字节
* 输入 ：pUartRxFIFOStruct：串口接收结构体指针
		 data：数据
* 输出 ：无
* 返回 ：无
*******************************************************************************/
void UartRxProcess(struct UartRxFIFOStruct *pUartRxFIFOStruct, U8 data)
{	
	switch(data)
	{
		case '$':                                                               //lq 串口命令报文起始字符
			pUartRxFIFOStruct->RxBuffIndex = 1;	//接受结构体列标
			if (((pUartRxFIFOStruct->RxFIFOInput+1) & (UART_RX_FIFO_DEPTH-1)) // 判断接受结构体满还是空
					!= pUartRxFIFOStruct->RxFIFOOutput)                         //lq 接收缓冲区结构体还有空间
			{
				pUartRxFIFOStruct->RxEnFlg = TRUE;
			}
			else                                                                //lq 接收缓冲区结构体满
			{
				pUartRxFIFOStruct->RxEnFlg = FALSE;				
			}			
			pUartRxFIFOStruct->Rx0x0dFlg = FALSE;  //收到0x0d标志
			break;
		case '\r':		//0x0d
			pUartRxFIFOStruct->Rx0x0dFlg = TRUE;
			break;
		case '\n':		//0x0a					  //接受完毕
			if(pUartRxFIFOStruct->Rx0x0dFlg == TRUE)
			{
				pUartRxFIFOStruct->Rx0x0dFlg = FALSE;

				if (pUartRxFIFOStruct->RxEnFlg == TRUE)
				{
					pUartRxFIFOStruct->RxEnFlg = FALSE;
					
					pUartRxFIFOStruct->RxFIFO[pUartRxFIFOStruct->RxFIFOInput++][0]  //lq RxFIFO[ ][0]存放报文的长度 
								= pUartRxFIFOStruct->RxBuffIndex - 1;	//保存当前接受数据长度
					if (pUartRxFIFOStruct->RxFIFOInput >= UART_RX_FIFO_DEPTH)
					{
						pUartRxFIFOStruct->RxFIFOInput = 0;
					}
				}
			}
			break;
		default:
			if(pUartRxFIFOStruct->Rx0x0dFlg == TRUE)                            //lq 报文格式无效（'\r'后未紧跟出现'\n'）    
			{
				pUartRxFIFOStruct->Rx0x0dFlg = FALSE;
				pUartRxFIFOStruct->RxEnFlg = FALSE;
			}
			
			if (pUartRxFIFOStruct->RxEnFlg == TRUE)
			{
				pUartRxFIFOStruct->RxFIFO[pUartRxFIFOStruct->RxFIFOInput][pUartRxFIFOStruct->RxBuffIndex++] = data;
   				if (pUartRxFIFOStruct->RxBuffIndex >= UART_RX_BUFF_SIZE)        //lq 串口报文过长，该报文无效（会被后面的报文覆盖）
				{
					pUartRxFIFOStruct->RxBuffIndex = 0;
					pUartRxFIFOStruct->RxEnFlg = FALSE;
				}
			}
			break;	
	}
}
#else
/*******************************************************************************
* 名称 ：UartRxProcess
* 描述 ：串口接收单字节
* 输入 ：pUartRxFIFOStruct：串口接收结构体指针
		 data：数据
* 输出 ：无
* 返回 ：无
*******************************************************************************/
void UartRxProcess(struct UartRxFIFOStruct *pUartRxFIFOStruct, U8 data)
{	
	switch(data)
	{
        case '!':                                                               //lq 支持语句!--ABM
        case '$':                                                               //lq 串口命令报文起始字符
			pUartRxFIFOStruct->RxBuffIndex = 1;	//接受结构体列标
			if (((pUartRxFIFOStruct->RxFIFOInput+1) & (UART_RX_FIFO_DEPTH-1)) // 判断接受结构体满还是空
					!= pUartRxFIFOStruct->RxFIFOOutput)                         //lq 接收缓冲区结构体还有空间
			{
				pUartRxFIFOStruct->RxEnFlg = TRUE;
			}
			else                                                                //lq 接收缓冲区结构体满
			{
				pUartRxFIFOStruct->RxEnFlg = FALSE;				
			}			
			pUartRxFIFOStruct->Rx0x0dFlg = FALSE;  //收到0x0d标志
			break;
		case '\r':		//0x0d
			pUartRxFIFOStruct->Rx0x0dFlg = TRUE;
			break;
		case '\n':		//0x0a					  //接受完毕
			if(pUartRxFIFOStruct->Rx0x0dFlg == TRUE)
			{
				pUartRxFIFOStruct->Rx0x0dFlg = FALSE;

				if (pUartRxFIFOStruct->RxEnFlg == TRUE)
				{
					pUartRxFIFOStruct->RxEnFlg = FALSE;
					
					pUartRxFIFOStruct->RxFIFO[pUartRxFIFOStruct->RxFIFOInput++][0]  //lq RxFIFO[ ][0]存放报文的长度 
								= pUartRxFIFOStruct->RxBuffIndex - 1;	//保存当前接受数据长度
					if (pUartRxFIFOStruct->RxFIFOInput >= UART_RX_FIFO_DEPTH)
					{
						pUartRxFIFOStruct->RxFIFOInput = 0;
					}
				}
			}
			else																//lq [2013-01-27]'\n'之前未出现'\r',报文格式无效
			{
				pUartRxFIFOStruct->RxEnFlg = FALSE;
			}
			break;
		default:
			if(pUartRxFIFOStruct->Rx0x0dFlg == TRUE)                            //lq 报文格式无效（'\r'后未紧跟出现'\n'）    
			{
				pUartRxFIFOStruct->Rx0x0dFlg = FALSE;
				pUartRxFIFOStruct->RxEnFlg = FALSE;
			}
			
			if (pUartRxFIFOStruct->RxEnFlg == TRUE)
			{
				pUartRxFIFOStruct->RxFIFO[pUartRxFIFOStruct->RxFIFOInput][pUartRxFIFOStruct->RxBuffIndex++] = data;
   				if (pUartRxFIFOStruct->RxBuffIndex >= UART_RX_BUFF_SIZE)        //lq 串口报文过长，该报文无效（会被后面的报文覆盖）
				{
					pUartRxFIFOStruct->RxBuffIndex = 0;
					pUartRxFIFOStruct->RxEnFlg = FALSE;
				}
			}
			break;	
	}
}
#endif

/*******************************************************************************
* 名称 ：TXbufIsEmpty
* 描述 ：发射缓冲是否为空
* 输入 ：COMM：串口接收结构体指针
* 输出 ：无
* 返回 ：无
*******************************************************************************/
S16 TXbufIsEmpty(U8 COMM)
{
	struct UartTxFIFOStruct *pUartTxFIFO;
	
	if(COMM == COMM1)
	{
		pUartTxFIFO = &Uart1TxFIFO;
	}
	else
	{
		return FALSE;
	}
	
	if(pUartTxFIFO->TxFIFOInput == pUartTxFIFO->TxFIFOOutput)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*******************************************************************************
* 名称 ：GetCOMMMess
* 描述 ：获取串口信息
* 输入 ：COMM：串口接收结构体指针
* 输出 ：无
* 返回 ：无
*******************************************************************************/
S16 GetCOMMMess(U8 COMM)
{
	U8 uchCrc;
	U16 uiLength;
	U8 AscHig, AscLow;
	U16 Len;
	struct UartRxFIFOStruct *pUartRxFIFO;
	
	uiLength = 0;

	if(COMM == COMM1)
	{
		pUartRxFIFO = &Uart1RxFIFO;
	}
	else if (COMM == COMM2)
	{
		pUartRxFIFO = &Uart2RxFIFO;
	}
	else
	{
		return uiLength;	
	}
	
	if(pUartRxFIFO->RxFIFOOutput != pUartRxFIFO->RxFIFOInput)
	{
		//CRC校验正确
		Len = pUartRxFIFO->RxFIFO[pUartRxFIFO->RxFIFOOutput][0];

		if (Len > 3)
		{
			uchCrc = UartMsgCheckSum(&pUartRxFIFO->RxFIFO[pUartRxFIFO->RxFIFOOutput][1], Len - 3);
			AscHig = HexToAscII(uchCrc >> 4);
			AscLow = HexToAscII(uchCrc & 0xf);
						
			if ((AscHig == pUartRxFIFO->RxFIFO[pUartRxFIFO->RxFIFOOutput][Len - 1])
				&& (AscLow == pUartRxFIFO->RxFIFO[pUartRxFIFO->RxFIFOOutput][Len]))
			{
				uiLength = Len - 3;
				
				//此处加入串口报文解析函数
				if (pUartRxFIFO == &Uart1RxFIFO)
				{
					UartMsgProcess(&pUartRxFIFO->RxFIFO[pUartRxFIFO->RxFIFOOutput][1], uiLength);	
				}
				else if (pUartRxFIFO == &Uart2RxFIFO)//串口2是GPS
				{
					GpsMsgProcess(&pUartRxFIFO->RxFIFO[pUartRxFIFO->RxFIFOOutput][1], uiLength);
				}					
			}
		}
				
		pUartRxFIFO->RxFIFOOutput++;
		if (pUartRxFIFO->RxFIFOOutput >= UART_RX_FIFO_DEPTH)
		{
			pUartRxFIFO->RxFIFOOutput = 0;
		}
	}
	
	return uiLength;
}




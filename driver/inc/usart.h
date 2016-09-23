/*******************************************************************************
  Copyright (C), 2010, 712.Co.,Ltd.
  File name:	usart.h
  Author: 王伟      Version: V1.00       Date: 2011.09.23 
  Description:  
  				主要功能：
				1. 定义相关宏定义
				2. 变量声明
				3. 函数声明
				4. 定义结构体类型
				
  Others:         // 其它内容的说明
  Function List:  // 主要函数列表，每条记录应包括函数名及功能简要说明
  History:        // 修改历史记录列表，每条修改记录应包括修改日期、修改
                  // 者及修改内容简述  
    1. Date:
       Author:
       Modification:
*******************************************************************************/
#ifndef __USART_H__
#define __USART_H__

#include "def.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//串口序号
#define COMM0		(0)
#define COMM1		(1)
#define COMM2		(2)
//串口状态
#define COMM_NO_ERR		(0)
#define COMM_RX_EMPTY	(1)
#define COMM_TX_EMPTY	(2)
#define COMM_RX_FULL	(3)
#define COMM_TX_FULL	(4)
#define COMM_RX_TIMEOUT	(5)
#define COMM_TX_TIMEOUT	(6)
#define COMM_ERR		(7)
/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define UART_TX_FIFO_DEPTH		(1024)			//COMM缓冲大小

#define UART_RX_FIFO_DEPTH		(8)
#define UART_RX_BUFF_SIZE		(128)
/* Private define ------------------------------------------------------------*/
struct UartTxFIFOStruct
{
	U8 TxFIFO[UART_TX_FIFO_DEPTH];	 //发送缓冲
	U16 TxFIFOInput;
	U16 TxFIFOOutput;
	
	U8  TxState;	//FALSE:：空闲；TRUE：忙
};

struct UartRxFIFOStruct
{
	U8  RxFIFO[UART_RX_FIFO_DEPTH][UART_RX_BUFF_SIZE];	//接受缓冲              //lq RxFIFO[报文语句号][报文内部字符号]
	U16 RxFIFOInput;//存入时使用                                                //lq 行标
	U16 RxFIFOOutput;//指向行标
	U8  RxEnFlg;		//TRUE：有空间；FALSE：空
	U16 RxBuffIndex; //指向列标                                                 //lq 列标
	U8  Rx0x0dFlg;	
    U8  Rx0xb5Flg;	
	U8  DoubleFlg;
};

typedef struct UsartConfig
{
    U32 Baud;
    U8  Data;
    U8  Parity;
    U8  Stop;    
}UsartConfigStruct;

/* Public  function  ---------------------------------------------------------*/
extern UsartConfigStruct Uart1Config;

U8 InitCommStruc(U8 Comm, U32 band); 
U8 UartTxDataBuf(U8 COMM, U8 *pStr,U16 Length);
S16 TXbufIsEmpty(U8 COMM);
void Usart1Interrupt(void);
extern void InitUsart1(U32 baud);
extern void UartRxFIFOStructInit(struct UartRxFIFOStruct *pUartRxFIFO);
extern void UartTxMainProcess(struct UartTxFIFOStruct *pUartTxFIFO);
extern void UartRxProcess(struct UartRxFIFOStruct *pUartRxFIFOStruct, U8 data);
extern void UartTxIntProcess(struct UartTxFIFOStruct *pUartTxFIFO);
extern S16 GetCOMMMess(U8 COMM);

// lnw  add
extern void USART1_UsartConfigStructInit(U32 Baud, U8 Data, U8 Parity, U8 Stop);
extern void UartTxFIFOStructInit(struct UartTxFIFOStruct *putCell);


#endif	//__USART_H__





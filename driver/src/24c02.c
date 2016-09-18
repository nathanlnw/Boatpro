/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: I2c.c
  Author: vozhj      Version : V1.00        Date: 2011.10.18
  Description:    I2c控制24c02
  Version:         V1.00
  Function List:   // 主要函数及其功能
  History:         // 历史修改记录
      <author>  <time>   	<version >   <desc>

*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f10x.h"
#include "def.h"
#include "24c02.h"
#include "common.h"
#include "debug.h"
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
U16 EEPROM_ADDRESS;
U16 SR1_Tmp = 0;
U32 I2C_Count;
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : GPIO5_Configuration
* Description    : Configure the used I/O ports pin
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO5_Configuration(void)                                                  //lq 配置24C02的WP引脚
{
  GPIO_InitTypeDef  GPIO_InitStructure; 

  /* Configure I2C1 pins: SCL and SDA */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configure the used I/O ports pin
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO6_Configuration(void)                                                  //lq 配置24C02的SCL引脚
{
  GPIO_InitTypeDef  GPIO_InitStructure; 

  /* Configure I2C1 pins: SCL and SDA */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configure the used I/O ports pin
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO7_Configuration(U8 PIN_Dir)                                            //lq 配置24C02的SDA引脚
{
  GPIO_InitTypeDef  GPIO_InitStructure; 

  /* Configure I2C1 pins: SCL and SDA */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7 ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  if(PIN_Dir == PIN_OUT)
  {
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  }
  else
  {
 		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  }

  GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/*******************************************************************************
* Function Name  : I2C_EE_Init
* Description    : Initializes peripherals used by the I2C EEPROM driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_EE_Init()
{
  /* GPIO configuration */
  GPIO5_Configuration();                                                        //lq 配置24C02的WP引脚
  GPIO6_Configuration();                                                        //lq 配置24C02的SCL引脚
  GPIO7_Configuration(PIN_OUT);                                                 //lq 配置24C02的SDA引脚

  WP_H;                                                                         //lq+写保护开启    
}
/*******************************************************************************
* Function Name  : I2C function
* Description    : I2C relative driver functions.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_delay(void)
{	
   U8 i = 1; //等待1测试正常，之前20
   while(i) 
   { 
     	i--; 
   } 
}

U8 I2C_Start(void)
{                                                                               //lq 先给数据，再给时钟（上升沿）
	SDA_H;                                                                      //lq ARM的SDA引脚输出‘1’
	SCL_H;                                                                      //lq ARM的SCL引脚输出‘1’
	I2C_delay();
	if(!SDA_read)
	{
		return FALSE;	//SDA线为低电平则总线忙,退出
	}
	SDA_L;
	I2C_delay();
	if(SDA_read)
    {
		return FALSE;	//SDA线为高电平则总线出错,退出
	}
	SDA_L;                                                                      //lq 24C02的START condition "在SCL=1时，SDA由1变为0"
	I2C_delay();

	return TRUE;
}

void I2C_Stop(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;                                                                      //lq 先给数据，再给时钟（上升沿）
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;                                                                      //lq 24C02的STOP condition "在SCL=1时，SDA由0变为1"    
	I2C_delay();
}

void I2C_Ack(void)
{	
	SCL_L;
	I2C_delay();                                                                //lq 先给数据，再给时钟（上升沿）
	SDA_L;                                                                      //lq 24C02的ACK condition "在SCL=1时，SDA为0"
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

void I2C_NoAck(void)
{	
	SCL_L;
	I2C_delay();                                                                //lq 先给数据，再给时钟（上升沿）
	SDA_H;                                                                      //lq 24C02的ACK condition "在SCL=1时，SDA为1"
	I2C_delay();
	SCL_H;                                                                      
	I2C_delay();
	SCL_L;
	I2C_delay();
}

U8 I2C_WaitAck(void) 	 //返回为:=1有ACK,=0无ACK
{
	SCL_L;
	I2C_delay();
	SDA_H;			                                                            //lq 先给数据，再给时钟（上升沿）
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read)
	{
      SCL_L;                                                                    //lq 输出‘1’，读到‘1’，说明：(1)当前为写状态；或（2）当前为读状态，且数据为‘1’。
      return (FALSE);                                                           //lq (1)和（2）均说明24C02还未发送'ACK'
	}
	SCL_L;                                                                      //lq 输出‘1’，但读到的是‘0’，说明当前写无效，即当前为读状态，且数据为‘0’。
	return TRUE;                                                                //lq 说明24C02已发送'ACK'
}

void I2C_SendByte(U8 SendByte) //数据从高位到低位//
{
    U8 i=8;
    while(i--)
    {
        SCL_L;
        I2C_delay();
        if(SendByte&0x80)
        {
        	SDA_H;                                                              //lq 先给数据，再给时钟（上升沿），提示从设备24C02接收数据
        }
        else
        {
        	SDA_L;
        }
        SendByte<<=1;
        I2C_delay();
		SCL_H;
        I2C_delay();
    }
    SCL_L;
}

U8 I2C_ReceiveByte(void)  //数据从高位到低位//
{ 
    U8 i=8;
    U8 ReceiveByte=0;

    SDA_H;				
    while(i--)
    {
        ReceiveByte<<=1;      
        SCL_L;
        I2C_delay();
	    SCL_H;                                                                  //lq 提供时钟（上升沿），提示从设备24C02发送数据，Master接收数据
        I2C_delay();	
        if(SDA_read)
        {
        	ReceiveByte|=0x01;
        }
    }
    SCL_L;
    return ReceiveByte;
}

/*******************************************************************************
* Function Name  : I2C_EE_BufferWrite
* Description    : Writes buffer of data to the I2C EEPROM.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the EEPROM.
* Output         : None
* Return         : None
*******************************************************************************/
U8 I2C_EE_BufferWrite(char* pBuffer, U8 WriteAddress, U16 NumByteToWrite)
{
	U16 i;
	U16 wait_count=0;//等待时间的上限
    WP_L;                                                                       //lq+写保护关闭
    
    if(!I2C_Start())
	{
        WP_H;                                                                   //lq+写保护开启
		return FALSE;
	}
    I2C_SendByte(0xA0);//设置高起始地址+器件地址                                //lq 发送设备地址
    if(!I2C_WaitAck())
	{
		I2C_Stop(); 
        WP_H;                                                                   //lq+写保护开启
		return FALSE;
	} 
    I2C_SendByte((U8)(WriteAddress & 0x00FF));   //设置低起始地址               //lq 发送字节地址
    I2C_WaitAck();
	
	for(i=0; i<NumByteToWrite; i++)
	{	
    	I2C_SendByte(pBuffer[i]);                                               //lq 24C02内部有地址自动增1功能
    	I2C_WaitAck();                                                          //lq 每次(START和STOP之间)最多写16B
	}   
    I2C_Stop(); 
	//注意：因为这里要等待EEPROM写完，可以采用查询或延时方式(50ms)
	//Delay_ms(50);
    while (I2C_WaitAck()==FALSE)//无ACK                                         //lq?
	{
		wait_count++;
		if (wait_count>3200)//不响应保护
		{
            break;
        }
		
		if(!I2C_Start())
		{
            WP_H;                                                               //lq+写保护开启
			return FALSE;
		}
		I2C_SendByte(0xA0);//设置高起始地址+器件地址 	
	}

    WP_H;                                                                       //lq+写保护开启
    return TRUE;
 
}

/*******************************************************************************
* 名称  : EraseAllData
* 描述  : 擦除所有数据
* 输入  : 无
* 返回  : TRUE 成功 FALSE 失败
*******************************************************************************/
U8 EraseAllData(void)
{
	U16 i;
	char chTemp[10];
	U8 ucCheck;
	U8 ucResult;

	for (i=0 ; i < 10; i++)
	{
		chTemp[i] = 0;                                                          //lq 全写为‘0’
	}

	ucResult = TRUE;
	for (i=0; i < 32; i++)
	{
		ucCheck = I2C_EE_BufferWrite(chTemp, i * 8,8);                          //lq 每次写8B

		if (ucCheck == FALSE)
		{
			ucResult = FALSE;
		}
	}

	return (ucResult);
}
	
		

/***************************************************************************************
** 函数名称: I2C_EE_BufferRead
** 功能描述: I2C存储器单扇区多字节读取函数
** 参    数: pBuffer                / 数据指针          
** 参    数: WriteAddr                / 写入地址
** 参    数: NumByteToRead        / 写入字节数
** 返 回 值: None 
** 备    注: 所有的数据只能在一个扇区之内       
***************************************************************************************/
U8 I2C_EE_BufferRead(char* pBuffer, U8 ReadAddress, U16 NumByteToRead)
{
	if(!I2C_Start())
	{
		return FALSE;
	}
	I2C_SendByte(0xA0);//设置高起始地址+器件地址                                //lq 发送设备地址 
	 
	if(!I2C_WaitAck())
	{
		I2C_Stop(); 
		return FALSE;
	}
    I2C_SendByte((U8)(ReadAddress & 0x00FF));   //设置低起始地址                //lq 发送字节地址
    I2C_WaitAck();
    I2C_Start();                                                                //lq?
    I2C_SendByte(0xA1);                                                         //lq?
    I2C_WaitAck();
    while(NumByteToRead)
    {
		*pBuffer = I2C_ReceiveByte();
		if(NumByteToRead == 1)
		{
			I2C_NoAck();
		}
		else 
		{
			I2C_Ack(); 
		}
      	pBuffer++;
      	NumByteToRead--;
    }
    I2C_Stop();
    return TRUE;
}
#if 0
//I2C测试
/*******************************************************************************
* Function Name  : Buffercmp
* Description    : Compares two buffers.
* Input          : - pBuffer1, pBuffer2: buffers to be compared.
*                : - BufferLength: buffer's length
* Output         : None
* Return         : PASSED: pBuffer1 identical to pBuffer2
*                  FAILED: pBuffer1 differs from pBuffer2
*******************************************************************************/
U8 Buffercmp(char* pBuffer1, char* pBuffer2, U16 BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return 0;	//失败
    }

    pBuffer1++;
    pBuffer2++;
  }

  return 1;
}


/***************************************************************************************
** 函数名称: I2C_Test
** 功能描述: I2C测试函数
       
***************************************************************************************/
 void I2C_Test(void)
{
 
  /* Public  function  ---------------------------------------------------------*/

	char Tx1_Buffer[6] = "777777";
	char Tx2_Buffer[6] = "999999";
	char Rx1_Buffer[BufferSize1], Rx2_Buffer[BufferSize2];
	U8 status1=0;
	U8 status2=0;
	
	

	  I2C_EE_BufferWrite(Tx1_Buffer, EEPROM_WriteAddress0, BufferSize1); 
	
	
	  I2C_EE_BufferRead(Rx1_Buffer, EEPROM_ReadAddress0, BufferSize1); 
	  
	
	  /* Check if the data written to the memory is read correctly */
	  status1 = Buffercmp(Tx1_Buffer, Rx1_Buffer, BufferSize1);
	  
	  //红灯亮
	  if(status1 == 1)
	  {
	   	//UartResponseMsg(Tx1_Buffer);
		UartTxString(Tx1_Buffer);
		UartTxString("\r\n");

		UartTxString(Rx1_Buffer);
		//UartResponseMsg(Rx1_Buffer);	
	  }
	  if(status1 == 0)
	  {
	
	  }
	  
	  Delay_ms(500);
	  I2C_EE_BufferWrite(Tx2_Buffer, EEPROM_WriteAddress1, BufferSize2); 
	
	 
	  I2C_EE_BufferRead(Rx2_Buffer, EEPROM_ReadAddress1, BufferSize2);
	
	 
	  status2 = Buffercmp(Tx2_Buffer, Rx2_Buffer, BufferSize2);
	  if(status2 == 1)
	  {
	
	  }
	  if(status2 == 0)
	  {
		
	  }
}

#endif

/*******************************************************************************
* Function Name  : I2C_EE_BufferWrite
* Description    : Writes buffer of data to the I2C EEPROM.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the EEPROM.
*                  - WriteAddr : EEPROM's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the EEPROM.
                   - wpFlg : whether enable write protection or not.
* Output         : None
* Return         : None
*******************************************************************************/
U8 I2C_EE_BufferWriteSelfCheck(char* pBuffer, U8 WriteAddress, U16 NumByteToWrite, U8 wpFlg)
{
	U16 i;
	U16 wait_count=0;//等待时间的上限

    if (wpFlg == TRUE)
    {
        WP_L;                                                                   //lq+写保护关闭    
    }   
    else
    {
        WP_H;                                                                   //lq+写保护开启    
    }
        
    if(!I2C_Start())
	{
        WP_H;                                                                   //lq+写保护开启
		return FALSE;
	}
    I2C_SendByte(0xA0);//设置高起始地址+器件地址                                //lq 发送设备地址
    if(!I2C_WaitAck())
	{
		I2C_Stop(); 
        WP_H;                                                                   //lq+写保护开启
		return FALSE;
	} 
    I2C_SendByte((U8)(WriteAddress & 0x00FF));   //设置低起始地址               //lq 发送字节地址
    I2C_WaitAck();
	
	for(i=0; i<NumByteToWrite; i++)
	{	
    	I2C_SendByte(pBuffer[i]);                                               //lq 24C02内部有地址自动增1功能
    	I2C_WaitAck();                                                          //lq 每次(START和STOP之间)最多写16B
	}   
    I2C_Stop(); 
	//注意：因为这里要等待EEPROM写完，可以采用查询或延时方式(50ms)
	//Delay_ms(50);
    while (I2C_WaitAck()==FALSE)//无ACK                                         //lq?
	{
		wait_count++;
		if (wait_count>3200)//不响应保护
		{
            break;
        }
		
		if(!I2C_Start())
		{
            WP_H;                                                               //lq+写保护开启
			return FALSE;
		}
		I2C_SendByte(0xA0);//设置高起始地址+器件地址 	
	}

    WP_H;                                                                       //lq+写保护开启
    return TRUE;
 
}

/*******************************************************************************
* Function Name  : EEPROM_SelfCheck
* Description    : EEPROM自检
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EEPROM_SelfCheck(EepromSelfCheckStruct *pEepromSelfCheckStruct)
{
  	U8 temp1;
    U8 temp2;
    U8 testValue1;
    U8 testValue2;
    U8 testAddr;

    if (pEepromSelfCheckStruct->StartFlg == TRUE)
    {
        testValue1 = 0xE2;
        testValue2 = 0x00;
        testAddr = 255;
    
        /*lq 记录测试地址字节单元的原始内容 */
        I2C_EE_BufferRead((char *)&temp1, testAddr, 1);
    
        /*lq 写保护关闭，并向测试地址单元写入一字节数据 */
        I2C_EE_BufferWriteSelfCheck((char *)&testValue1, testAddr, 1, TRUE);
    
        /*lq 读取写入的数据 */
        I2C_EE_BufferRead((char *)&temp2, testAddr, 1);
    
        if (temp2 == testValue1)
        {
            pEepromSelfCheckStruct->DataCommOkFlg = TRUE;    
    
            /*lq 写保护开启，并向测试地址单元写入一字节新数据 */
            I2C_EE_BufferWriteSelfCheck((char *)&testValue2, testAddr, 1, FALSE);
    
            /*lq 读取写入的数据 */
            I2C_EE_BufferRead((char *)&temp2, testAddr, 1);
            
            /*lq 判断内容是否改变 */
            if (temp2 == testValue1)
            {
                pEepromSelfCheckStruct->WPOkFlg = TRUE;                    
            }
            else
            {
                pEepromSelfCheckStruct->WPOkFlg = FALSE;        
            }                 
        }
        else
        {
            /*lq 无法进一步确定问题的准确原因 */
            pEepromSelfCheckStruct->WPOkFlg = FALSE;
            pEepromSelfCheckStruct->DataCommOkFlg = FALSE;    
        }
    
        /*lq 写回最初的值 */
        I2C_EE_BufferWrite((char *)&temp1, testAddr, 1);               
    
        pEepromSelfCheckStruct->CompleteFlg = TRUE;        
        pEepromSelfCheckStruct->StartFlg = FALSE;    
    }
}

/*******************************************************************************
* 名称  : EEPROM_EepromSelfCheckStructInit
* 描述  : 初始化结构体
* 输入  : pEepromSelfCheckStruct - EepromSelfCheckStruct结构体指针	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void EEPROM_EepromSelfCheckStructInit(EepromSelfCheckStruct *pEepromSelfCheckStruct)
{
    pEepromSelfCheckStruct->WPOkFlg = FALSE;
    pEepromSelfCheckStruct->DataCommOkFlg = FALSE;
    pEepromSelfCheckStruct->StartFlg = TRUE;       
    pEepromSelfCheckStruct->CompleteFlg = FALSE;        
}

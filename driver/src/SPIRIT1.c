/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: SPIRIT1.c
  Author: lq       Version : V1.00        Date: 2014.06.30
  Description:     SPIRIT1驱动
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "stm32f10x_lib.h"
#include "def.h"
#include "SPIRIT1.h"
#include "gpiospi.h"
#include "spi.h"
#include "PhysicalLayer.h"
#include "DataLinkService.h"
#include "LED.h"
#include "initial.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
//lq static void SPIRIT1_ReadRegisters(SPIRIT1TypeEnum cs);

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* 名称  : SPIRIT1_InitPins
* 描述  : 初始化SPIRIT1引脚
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPIRIT1_InitPins(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

    /*lq ------------------SPIRIT1_A 引脚配置---------------------------*/
    /* Configure SPIRIT1_A SDN ---------------------------------*/
    GPIO_InitStructure.GPIO_Pin = SPIRIT1_A_SDN_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(SPIRIT1_A_SDN_PORT, &GPIO_InitStructure);
    SPIRIT1_A_SDN_LOW();                                    //lq 芯片开启        

    /* Configure SPIRIT1_A SPI ---------------------------------*/
#if 0
    GPIO_InitStructure.GPIO_Pin = SPIRIT1_A_SPI_CS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(SPIRIT1_A_SPI_CS_PORT, &GPIO_InitStructure);
    SPIRIT1_A_SPI_CS_HIGH();                                //lq 片选失能
    
    GPIO_InitStructure.GPIO_Pin = SPIRIT1_A_SPI_SCLK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(SPIRIT1_A_SPI_SCLK_PORT, &GPIO_InitStructure);
    SPIRIT1_A_SPI_SCLK_LOW();

    GPIO_InitStructure.GPIO_Pin = SPIRIT1_A_SPI_MOSI_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(SPIRIT1_A_SPI_MOSI_PORT, &GPIO_InitStructure);
    SPIRIT1_A_SPI_MOSI_LOW();                               

    GPIO_InitStructure.GPIO_Pin = SPIRIT1_A_SPI_MISO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    
    GPIO_Init(SPIRIT1_A_SPI_MISO_PORT, &GPIO_InitStructure);
#else
    SPI2_Init();
#endif
    /* Configure SPIRIT1_A GPIO ---------------------------------*/
    GPIO_InitStructure.GPIO_Pin = SPIRIT1_A_GPIO0_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //lq Tx Data
    GPIO_Init(SPIRIT1_A_GPIO0_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SPIRIT1_A_GPIO1_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //lq Rx Data 
    GPIO_Init(SPIRIT1_A_GPIO1_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SPIRIT1_A_GPIO2_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;           //lq Tx Clock    
    GPIO_Init(SPIRIT1_A_GPIO2_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SPIRIT1_A_GPIO3_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;           //lq Rx Clock
    GPIO_Init(SPIRIT1_A_GPIO3_PORT, &GPIO_InitStructure);

	/* Connect EXTI Line to GPIO Pin */
	GPIO_EXTILineConfig(SPIRIT1_A_GPIO2_SOURCE_PORT, SPIRIT1_A_GPIO2_SOURCE_PIN);   //lq TX clock
	
	/* Configure EXTI Line to generate an interrupt on falling edge */  
	EXTI_InitStructure.EXTI_Line = SPIRIT1_A_GPIO2_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //lq Tx时MCU在时钟下降沿更新数据
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 

	/* Connect EXTI Line to GPIO Pin */
	GPIO_EXTILineConfig(SPIRIT1_A_GPIO3_SOURCE_PORT, SPIRIT1_A_GPIO3_SOURCE_PIN);   //lq RX clock

	/* Configure EXTI Line to generate an interrupt on rising edge */  
	EXTI_InitStructure.EXTI_Line = SPIRIT1_A_GPIO3_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
#if 0
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //lq Rx时MCU在时钟上升沿采样数据
#else
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
#endif
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 

    /*lq ------------------SPIRIT1_B 引脚配置---------------------------*/
    /* Configure SPIRIT1_B SDN ---------------------------------*/
    GPIO_InitStructure.GPIO_Pin = SPIRIT1_B_SDN_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(SPIRIT1_B_SDN_PORT, &GPIO_InitStructure);
    SPIRIT1_B_SDN_LOW();                                    //lq 芯片开启        

    /* Configure SPIRIT1_B SPI ---------------------------------*/
#if 0
    GPIO_InitStructure.GPIO_Pin = SPIRIT1_B_SPI_CS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(SPIRIT1_B_SPI_CS_PORT, &GPIO_InitStructure);
    SPIRIT1_B_SPI_CS_HIGH();                                //lq 片选失能
    
    GPIO_InitStructure.GPIO_Pin = SPIRIT1_B_SPI_SCLK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(SPIRIT1_B_SPI_SCLK_PORT, &GPIO_InitStructure);
    SPIRIT1_B_SPI_SCLK_LOW();

    GPIO_InitStructure.GPIO_Pin = SPIRIT1_B_SPI_MOSI_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(SPIRIT1_B_SPI_MOSI_PORT, &GPIO_InitStructure);
    SPIRIT1_B_SPI_MOSI_LOW();                               

    GPIO_InitStructure.GPIO_Pin = SPIRIT1_B_SPI_MISO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    
    GPIO_Init(SPIRIT1_B_SPI_MISO_PORT, &GPIO_InitStructure);
#else
    GPIOSPI_InstanceInit(GPIOSPI1, GPIOSPI_CPOL_Low, GPIOSPI_CPHA_1Edge, 4, 
                         GPIOSPI_FirstBit_MSB, GPIOSPI_DataSize_8b);
#endif
    /* Configure SPIRIT1_B GPIO ---------------------------------*/
    GPIO_InitStructure.GPIO_Pin = SPIRIT1_B_GPIO0_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;           //lq Tx Clock    
    GPIO_Init(SPIRIT1_B_GPIO0_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SPIRIT1_B_GPIO1_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;           //lq Rx Clock
    GPIO_Init(SPIRIT1_B_GPIO1_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SPIRIT1_B_GPIO2_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //lq Tx Data
    GPIO_Init(SPIRIT1_B_GPIO2_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SPIRIT1_B_GPIO3_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   //lq Rx Data 
    GPIO_Init(SPIRIT1_B_GPIO3_PORT, &GPIO_InitStructure);

	/* Connect EXTI Line to GPIO Pin */
	GPIO_EXTILineConfig(SPIRIT1_B_GPIO0_SOURCE_PORT, SPIRIT1_B_GPIO0_SOURCE_PIN);   //lq TX clock
	
	/* Configure EXTI Line to generate an interrupt on falling edge */  
	EXTI_InitStructure.EXTI_Line = SPIRIT1_B_GPIO0_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //lq Tx时MCU在时钟下降沿更新数据
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 

	/* Connect EXTI Line to GPIO Pin */
	GPIO_EXTILineConfig(SPIRIT1_B_GPIO1_SOURCE_PORT, SPIRIT1_B_GPIO1_SOURCE_PIN);   //lq RX clock
	
	/* Configure EXTI Line to generate an interrupt on rising edge */  
	EXTI_InitStructure.EXTI_Line = SPIRIT1_B_GPIO1_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
#if 0
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //lq Rx时MCU在时钟上升沿采样数据
#else
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
#endif	
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

/*******************************************************************************
* 名称  : SPIRIT1_Init
* 描述  : 初始化SPIRIT1
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPIRIT1_Init(void)
{
    //lq CMX7042_InitInstance();
    SPIRIT1_InitPins();
}

/*******************************************************************************
* 名称  : SPIRIT1_A_RxClockIRQS
* 描述  : SPIRIT1_A接收时钟中断
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPIRIT1_A_RxClockIRQS(void)
{
    U8  currValue;
    U8  decodeValue;

	if(EXTI_GetITStatus(SPIRIT1_A_GPIO3_LINE) != RESET)
	{ 
		/* Clear the EXTI line pending bit */  
		EXTI_ClearITPendingBit(SPIRIT1_A_GPIO3_LINE);

        /*lq 读取RX data GPIO引脚 */
        currValue = SPIRIT1_A_GPIO1_IN_READ();
#if 1
        if (PHY_InstNRZIDecode(NRZIType1, currValue, &decodeValue) == TRUE)
        {
            DLS_InstBitStreamPutBit(PacketProbeType1, decodeValue);
        }
#else
        DLS_InstBitStreamPutBit(PacketProbeType1, currValue);
#endif
#if 1
        if (currValue == 0)
        {
            TEST_OUT_LOW_1();        
        }
        else
        {
            TEST_OUT_HIGH_1();
        }
#endif
	}
}

/*******************************************************************************
* 名称  : SPIRIT1_B_RxClockIRQS
* 描述  : SPIRIT1_B接收时钟中断
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPIRIT1_B_RxClockIRQS(void)
{
	if(EXTI_GetITStatus(SPIRIT1_B_GPIO1_LINE) != RESET)
	{ 
		/* Clear the EXTI line pending bit */  
		EXTI_ClearITPendingBit(SPIRIT1_B_GPIO1_LINE);

        /*lq */
        //lq... 读取RX data GPIO引脚
	}
}

/*******************************************************************************
* 名称  : SPIRIT1_A_TxClockIRQS
* 描述  : SPIRIT1_A发射时钟中断
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPIRIT1_A_TxClockIRQS(void)
{
	if(EXTI_GetITStatus(SPIRIT1_A_GPIO2_LINE) != RESET)
	{ 
		/* Clear the EXTI line pending bit */  
		EXTI_ClearITPendingBit(SPIRIT1_A_GPIO2_LINE);
#if 1
        /*lq 提供发射数据 */
        if ((eqEquipPara.ucWorkMode == EQ_WORKMOD_TEST)
            && (eqEquipPara.TestSta == EQ_MOD_TEST_STA_MAIN))
        {
            /*lq 测试态发射 */
            if (SPRTCTRL_InstGetTxTestFlg(SPIRIT1TypeA) == TRUE)
            {
                if (SPRTCTRL_InstTxTestIsTx(SPIRIT1TypeA) == TRUE)
                {
                    if (SPRTCTRL_InstTxTestGetSignalBit(SPIRIT1TypeA) == 0)
                    {
                        SPIRIT1_A_GPIO0_LOW(); 
                    }
                    else
                    {
                        SPIRIT1_A_GPIO0_HIGH();
                    }                
                }
            }        
        }
        else
        {
            /*lq 正常模式发射 */
            if (SPRTCTRL_InstTxGetSignalBit() == 0)
            {
                SPIRIT1_A_GPIO0_LOW(); 
            }
            else
            {
                SPIRIT1_A_GPIO0_HIGH();
            }
        }
#endif
	}
}

/*******************************************************************************
* 名称  : SPIRIT1_B_TxClockIRQS
* 描述  : SPIRIT1_B发射时钟中断
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPIRIT1_B_TxClockIRQS(void)
{
	if(EXTI_GetITStatus(SPIRIT1_B_GPIO0_LINE) != RESET)
	{ 
		/* Clear the EXTI line pending bit */  
		EXTI_ClearITPendingBit(SPIRIT1_B_GPIO0_LINE);

        /*lq 提供发射数据 */
        if (SPRTCTRL_InstGetTxTestFlg(SPIRIT1TypeB) == TRUE)
        {
            if (SPRTCTRL_InstTxTestIsTx(SPIRIT1TypeB) == TRUE)
            {
                if (SPRTCTRL_InstTxTestGetSignalBit(SPIRIT1TypeB) == 0)
                {
                    SPIRIT1_B_GPIO2_LOW(); 
                }
                else
                {
                    SPIRIT1_B_GPIO2_HIGH();
                }                
            }
        }
	}
}

/*******************************************************************************
* 名称  : SPIRIT1_WriteByte
* 描述  : 写字节数据
* 输入  : cs - SPIRIT1芯片类型
          addr - 地址
          data - 数据	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPIRIT1_WriteByte(SPIRIT1TypeEnum cs, U8 addr, U8 data)
{
    if (cs == SPIRIT1TypeA)
    {
        /*lq 启动SPI2 */
#if 0   
        SPI_Cmd(SPI2, ENABLE);
#endif    
        SPI2_NSS_LOW();             

        /*lq 发送头 */
        SPI2_SendByte(SPIRIT1_HEADER_ADDRESS_WRITE);
            
        /*lq 发送地址 */
        SPI2_SendByte(addr);
    
        /*lq 发送数据 */
        SPI2_SendByte(data); 
    
        /*lq 关闭SPI */
        SPI2_NSS_HIGH(); 
#if 0                
        SPI_Cmd(SPI2, DISABLE);
#endif 

        Delay_us(5);    //lq SPIRIT1 要求至少2us    
    }
    else
    {
        /*lq 发送头 */
        GPIOSPI_InstSendData(GPIOSPI1, SPIRIT1_HEADER_ADDRESS_WRITE);

        /*lq 发送地址 */
        GPIOSPI_InstSendData(GPIOSPI1, addr);

        /*lq 发送数据 */
        GPIOSPI_InstSendData(GPIOSPI1, data);        

        Delay_us(5);    //lq SPIRIT1 要求至少2us
    }   
}

/*******************************************************************************
* 名称  : SPIRIT1_WriteWord
* 描述  : 写字数据
* 输入  : cs - SPIRIT1芯片类型
          addr - 地址
          data - 数据	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPIRIT1_WriteWord(SPIRIT1TypeEnum cs, U8 addr, U16 data)
{
    if (cs == SPIRIT1TypeA)
    {
        /*lq 启动SPI2 */
#if 0   
        SPI_Cmd(SPI2, ENABLE);
#endif    
        SPI2_NSS_LOW();             

        /*lq 发送头 */
        SPI2_SendByte(SPIRIT1_HEADER_ADDRESS_WRITE);
            
        /*lq 发送地址 */
        SPI2_SendByte(addr);
    
        /*lq 发送高8位 */
        SPI2_SendByte(data >> 8); 
    
        /*lq 发送低8位 */
        SPI2_SendByte(data & 0xFF);  
    
        /*lq 关闭SPI */
        SPI2_NSS_HIGH(); 
#if 0                
        SPI_Cmd(SPI2, DISABLE);
#endif     
        Delay_us(5);    //lq SPIRIT1 要求至少2us
    }
    else
    {
        /*lq 发送头 */
        GPIOSPI_InstSendData(GPIOSPI1, SPIRIT1_HEADER_ADDRESS_WRITE);

        /*lq 发送地址 */
        GPIOSPI_InstSendData(GPIOSPI1, addr);

        /*lq 发送高8位 */
        GPIOSPI_InstSendData(GPIOSPI1, data >> 8); 

        /*lq 发送低8位 */
        GPIOSPI_InstSendData(GPIOSPI1, data & 0xFF);                

        Delay_us(5);    //lq SPIRIT1 要求至少2us
    }   
}

/*******************************************************************************
* 名称  : SPIRIT1_ReadByte
* 描述  : 读字节数据
* 输入  : cs - SPIRIT1芯片类型
          addr - 地址
* 输出  : 无
* 返回  : RegVal - 返回字节
*******************************************************************************/
U8 SPIRIT1_ReadByte(SPIRIT1TypeEnum cs, U8 addr)
{
    U8 RegVal;

    if (cs == SPIRIT1TypeA)
    {
        /*lq 启动SPI2 */
#if 0
        SPI_Cmd(SPI2, ENABLE);
#endif
        SPI2_NSS_LOW();             
    
        /*lq 发送头 */
        SPI2_SendByte(SPIRIT1_HEADER_ADDRESS_READ);

        /*lq 发送地址 */
        SPI2_SendByte(addr);
    
        /*lq 接收数据*/
        RegVal = SPI2_SendByte(SPI_DUMMY_BYTE); 
    
        /*lq 关闭SPI2 */
        SPI2_NSS_HIGH();
#if 0                 
        SPI_Cmd(SPI2, DISABLE);
#endif            
        Delay_us(5);    //lq SPIRIT1 要求至少2us
    }   
    else
    {
        /*lq 发送头 */
        GPIOSPI_InstSendData(GPIOSPI1, SPIRIT1_HEADER_ADDRESS_READ);

        /*lq 发送地址 */
        GPIOSPI_InstSendData(GPIOSPI1, addr); 
        
        /*lq 接收数据 */
        RegVal = GPIOSPI_InstSendData(GPIOSPI1, SPI_DUMMY_BYTE);   

        Delay_us(5);    //lq SPIRIT1 要求至少2us
    } 
   
    return RegVal;        
}

/*******************************************************************************
* 名称  : SPIRIT1_ReadWord
* 描述  : 读字数据
* 输入  : cs - SPIRIT1芯片类型
          addr - 地址
* 输出  : 无
* 返回  : RegVal - 返回字节
*******************************************************************************/
U16 SPIRIT1_ReadWord(SPIRIT1TypeEnum cs, U8 addr)
{
    U16 RegVal;

    if (cs == SPIRIT1TypeA)
    {
        /*lq 启动SPI2 */
#if 0
        SPI_Cmd(SPI2, ENABLE);
#endif
        SPI2_NSS_LOW();             
    
        /*lq 发送头 */
        SPI2_SendByte(SPIRIT1_HEADER_ADDRESS_READ);

        /*lq 发送地址 */
        SPI2_SendByte(addr);
    
        /*lq 接收数据*/
        RegVal = SPI2_SendByte(SPI_DUMMY_BYTE); 
        RegVal <<= 8;
        RegVal |= SPI2_SendByte(SPI_DUMMY_BYTE);
            
        /*lq 关闭SPI2 */
        SPI2_NSS_HIGH();
#if 0                 
        SPI_Cmd(SPI2, DISABLE);
#endif

        Delay_us(5);    //lq SPIRIT1 要求至少2us            
    }   
    else
    {
        /*lq 发送头 */
        GPIOSPI_InstSendData(GPIOSPI1, SPIRIT1_HEADER_ADDRESS_READ);

        /*lq 发送地址 */
        GPIOSPI_InstSendData(GPIOSPI1, addr); 
        
        /*lq 接收数据 */
        RegVal = GPIOSPI_InstSendData(GPIOSPI1, SPI_DUMMY_BYTE);
        RegVal <<= 8;
        RegVal |= GPIOSPI_InstSendData(GPIOSPI1, SPI_DUMMY_BYTE);           

        Delay_us(5);    //lq SPIRIT1 要求至少2us
    } 
   
    return RegVal;        
}

/*******************************************************************************
* 名称  : SPIRIT1_WriteCommand
* 描述  : 往SPIRIT1写一个命令字节
* 输入  : command - 命令字节		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPIRIT1_WriteCommand(SPIRIT1TypeEnum cs, U8 command)
{
    if (cs == SPIRIT1TypeA)
    {
        /*lq 启动SPI2 */
#if 0
        SPI_Cmd(SPI2, ENABLE);
#endif    
        SPI2_NSS_LOW();             

        /*lq 发送头 */
        SPI2_SendByte(SPIRIT1_HEADER_COMMAND_WRITE);
    
        /*lq 发送命令 */
        SPI2_SendByte(command);
    
        /*lq 关闭SPI */
        SPI2_NSS_HIGH();
#if 0                     
        SPI_Cmd(SPI2, DISABLE);
#endif    
        Delay_us(5);    //lq SPIRIT1 要求至少2us
    }
    else
    {
        /*lq 发送头 */
        GPIOSPI_InstSendData(GPIOSPI1, SPIRIT1_HEADER_COMMAND_WRITE);

        /*lq 发送命令 */
        GPIOSPI_InstSendData(GPIOSPI1, command);    

        Delay_us(5);    //lq SPIRIT1 要求至少2us
    }
}

/*******************************************************************************
* 名称  : SPIRIT1_Configuration
* 描述  : 配置SPIRIT1
* 输入  : cs - SPIRIT1片选		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void SPIRIT1_Configuration(SPIRIT1TypeEnum cs)
{
    U8  regValue;

    Feed_Dog();
    
    Delay_ms(50);   //lq 上电复位延时

    /*lq 主动复位*/
    SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_SRES);                 //lq 复位
    Delay_ms(50);   //lq 复位延时

    SPIRIT1_StateToReady(cs);

    /*lq 进入待机状态 */        
    SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_STANDBY);              //lq 配置数字时钟二分频时需进入standby状态

    /*lq -----------------General configuration registers----------------*/
    /*lq ANA_FUNC_CONF[1] */
    //lq 默认值

    /*lq ANA_FUNC_CONF[0] */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_GC_ANA_FUNC_CONF0, 0xD0); //lq reference signal from XIN pin

    if (cs == SPIRIT1TypeA)
    {
        /*lq GPIO3_CONF */
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_GC_GPIO3_CONF, 
                          (SPIRIT1_GPIO_FUC_OUT_RX_CLOCK | SPIRIT1_GPIO_MODE_DOUT_LOW));    //lq GPIO3: RX clock
    
        /*lq GPIO2_CONF */
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_GC_GPIO2_CONF, 
                          (SPIRIT1_GPIO_FUC_OUT_TX_CLOCK | SPIRIT1_GPIO_MODE_DOUT_HIGH));   //lq GPIO2: TX clock
    
        /*lq GPIO1_CONF */
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_GC_GPIO1_CONF, 
                          (SPIRIT1_GPIO_FUC_OUT_RX_DATA | SPIRIT1_GPIO_MODE_DOUT_LOW));     //lq GPIO1: RX data
    
        /*lq GPIO0_CONF */
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_GC_GPIO0_CONF, 
                          (SPIRIT1_GPIO_FUC_IN_TX_DATA | SPIRIT1_GPIO_MODE_DIN));           //lq GPIO0: TX data
    }
    else if (cs == SPIRIT1TypeB)
    {
        /*lq GPIO3_CONF */
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_GC_GPIO3_CONF, 
                          (SPIRIT1_GPIO_FUC_OUT_RX_DATA | SPIRIT1_GPIO_MODE_DOUT_LOW));     //lq GPIO3: RX data
    
        /*lq GPIO2_CONF */
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_GC_GPIO2_CONF, 
                          (SPIRIT1_GPIO_FUC_IN_TX_DATA | SPIRIT1_GPIO_MODE_DIN));           //lq GPIO2: TX data
    
        /*lq GPIO1_CONF */
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_GC_GPIO1_CONF, 
                          (SPIRIT1_GPIO_FUC_OUT_RX_CLOCK | SPIRIT1_GPIO_MODE_DOUT_LOW));    //lq GPIO1: RX clock
    
        /*lq GPIO0_CONF */
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_GC_GPIO0_CONF, 
                          (SPIRIT1_GPIO_FUC_OUT_TX_CLOCK | SPIRIT1_GPIO_MODE_DOUT_HIGH));   //lq GPIO0: TX clock

    }

    /*lq MCU_CK_CONF */
    //lq 默认值

    /*lq XO_RCO_TEST */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_GC_XO_RCO_TEST, 0x29);    //lq... PD_CLKDIV = 1(安全做法是先进入standby状态，见手册8.5)
    
    /*lq SYNTH_CONFIG[0] */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_GC_SYNTH_CONFIG0, 0xA0);  //lq split time: 3.47 ns

    /*lq SYNTH_CONFIG[1] */
    //lq 默认值

    /*lq IF_OFFSET_ANA */
    //lq 默认值

    /*lq -----------------Radio configuration registers (analog blocks)----------------*/
    /*lq SYNT3 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_SYNT3, 0x0C);     //lq SYNT = 26017792

    /*lq SYNT2 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_SYNT2, 0x68);     //lq SYNT = 26017792

    /*lq SYNT1 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_SYNT1, 0x00);     //lq SYNT = 26017792

    /*lq SYNT0 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_SYNT0, 0x05);     //lq SYNT = 26017792，BS = 5(B = 32)

    /*lq CHSPACE */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_CHSPACE, 0x3F);   //lq CHSPACE = 63

    /*lq IF_OFFSET_DIG */
    //lq 默认值

    /*lq FC_OFFSET1 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_FC_OFFSET1, 0x0F); //lq FC_OFFSET = -57

    /*lq FC_OFFSET0 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_FC_OFFSET0, 0xC7); //lq FC_OFFSET = -57

    /*lq PA_POWER8 */
    //lq 默认值    
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_PA_POWER(8), 0x03);//lq PA_LEVEL_7 = 

    /*lq PA_POWER7 */
    //lq 默认值    
    //lq SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_PA_POWER(7), 0x);//lq PA_LEVEL_6 = 

    /*lq PA_POWER6 */
    //lq 默认值    
    //lq SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_PA_POWER(6), 0x);//lq PA_LEVEL_5 = 

    /*lq PA_POWER5 */
    //lq 默认值    
    //lq SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_PA_POWER(5), 0x);//lq PA_LEVEL_4 = 

    /*lq PA_POWER4 */
    //lq 默认值    
    //lq SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_PA_POWER(4), 0x);//lq PA_LEVEL_3 = 

    /*lq PA_POWER3 */
    //lq 默认值    
    //lq SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_PA_POWER(3), 0x);//lq PA_LEVEL_2 = 

    /*lq PA_POWER2 */
    //lq 默认值    
    //lq SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_PA_POWER(2), 0x);//lq PA_LEVEL_1 = 

    /*lq PA_POWER1 */
    //lq 默认值    
    //lq SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_PA_POWER(1), 0x);//lq PA_LEVEL_0 = 

    /*lq PA_POWER0 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCA_PA_POWER(0), 0x3F);//lq enable PA power ramping, step width = 4 * (1/8 bit period), final level = 7

    /*lq -----------------Radio configuration registers (digital blocks)----------------*/
    /*lq MOD1 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCD_MOD1, 0x83);      //lq DATARATE_M = 131（datarate=9600）

    /*lq MOD0 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCD_MOD0, 0x58);      //lq BT = 0.5, MOD_TYPE = GFSK, DATARATE_E = 8（datarate=9600）

    /*lq FDEV0 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCD_FDEV0, 0x24);     //lq FDEV_M = 4, CLOCK_REC_ALGO_SEL = DLL, FDEV_E = 2

    /*lq CHFLT */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCD_CHFLT, 0x25);     //lq CHFLT_M = 2, CHFLT_E = 5

    /*lq AFC2 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCD_AFC2, 0x08);      //lq... AFC_ENABLE = 0

    /*lq AFC1 */
    //lq 默认值

    /*lq AFC0 */
    //lq 默认值

    /*lq RSSI_FLT */
    //lq 默认值

    /*lq RSSI_TH */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCD_RSSI_TH, 0x0E);     //lq RSSI_TH = -123

    /*lq CLOCKREC */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCD_CLOCKREC, 0xC8);    //lq... 

    /*lq AGCCTRL2 */
    //lq 默认值             

    /*lq AGCCTRL1 */
    //lq 默认值             

    /*lq AGCCTRL0 */
    //lq 默认值             

    /*lq ANT_SELECT_CONF */
    //lq 默认值 

    /*lq -----------------Packet/protocol configuration registers----------------*/
    /*lq PCKTCTRL4 */
    //lq 默认值

    /*lq PCKTCTRL3 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(3), 0x27);      //lq RX mode: direct through GPIO,

    /*lq PCKTCTRL2 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(2), 0x1A);      //lq SYNC_LENGTH = 1，

    /*lq PCKTCTRL1 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(1), 0x08);      //lq CRC: No CRC, TX source data: direct through GPIO,

    /*lq PCKTLEN1 */
    //lq 默认值

    /*lq PCKTLEN0 */
    //lq 默认值

    /*lq SYNC4 */
    //lq 默认值

    /*lq SYNC3 */
    //lq 默认值

    /*lq SYNC2 */
    //lq 默认值

    /*lq SYNC1 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_SYNC(1), 0x01);           //lq SYNC1: 0x01(0x7E的NRZI编码，也有可能是0xFE)

    /*lq QI */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_QI, 0x00);                //lq... SQI_EN: disable

    /*lq MBUS_PRMBL */
    //lq 默认值

    /*lq MBUS_PSTMBL */
    //lq 默认值

    /*lq MBUS_CTRL */
    //lq 默认值

    /*lq FIFO_CONFIG3 */
    //lq 默认值

    /*lq FIFO_CONFIG2 */
    //lq 默认值

    /*lq FIFO_CONFIG1 */
    //lq 默认值

    /*lq FIFO_CONFIG0 */
    //lq 默认值

    /*lq PCKT_FLT_GOALS[12-0] */
    //lq 默认值

    /*lq PCKT_FLT_OPTIONS */
    //lq 默认值

    /*lq PROTOCOL2 */
    //lq 默认值
#if 0
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_PROTOCOL(2), 0x00);      //lq disable the automatic VCO calibration
#endif
    /*lq PROTOCOL1 */
    //lq 默认值

    /*lq PROTOCOL0 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_PROTOCOL(0), 0x0A);      //lq PERS_RX: persistent reception enabled

    /*lq TIMERS[5-0] */
    //lq 默认值

    /*lq CSMA_CONFIG[3-0] */
    //lq 默认值

    /*lq TX_CTRL_FIELD[3-0] */
    //lq 默认值

    /*lq PM_CONFIG2 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_PM_CONFIG(2), 0x00);      //lq enable internal SMPS, SET_SMPS_VTUNE = 0, SET_SMPS_PLLBW = 0      

    /*lq PM_CONFIG1 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_PM_CONFIG(1), 0x93);      //lq EN_RM = 1, KRM = 5041(Fsw = 4MHz)

    /*lq PM_CONFIG0 */
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_PM_CONFIG(0), 0xB1);      //lq KRM = 5041(Fsw = 4MHz)

    /*lq XO_RCO_CONFIG */
    //lq 默认值

    /*lq TEST_SELECT */
    //lq 默认值

    /*lq PM_TEST */
    //lq 默认值

    /*lq -----------------Frequently used registers----------------*/
    /*lq CHNUM */
    if (cs == SPIRIT1TypeA)
    {
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_FU_CHNUM, SPIRIT1_PARA_CH_NUM_A); //lq CHNUM 对应161.975MHz    
    }
    else
    {
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_FU_CHNUM, SPIRIT1_PARA_CH_NUM_B); //lq CHNUM 对应162.025MHz        
    }

    /*lq VCO_CONFIG */
    //lq 默认值

    /*lq RCO_VCO_CALIBR_IN[2] */
    //lq 默认值

    /*lq RCO_VCO_CALIBR_IN[1] */
    //lq 默认值
#if 0
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_FU_RCO_VCO_CALIBR_IN(1), 0x46);      //lq Word value for the VCO to be used in TX mode
#endif
    /*lq RCO_VCO_CALIBR_IN[0] */
    //lq 默认值
#if 0
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_FU_RCO_VCO_CALIBR_IN(0), 0x46);      //lq Word value for the VCO to be used in RX mode
#endif
    /*lq AES_KEY_IN[15-0] */
    //lq 默认值

    /*lq AES_DATA_IN[15-0] */
    //lq 默认值

    /*lq IRQ_MASK[3-0] */
    //lq 默认值

    /*lq IRQ_MASK[3-0] */
    //lq 默认值

    /*lq DEM_CONFIG */
    //lq... 默认值

    /*lq PM_CONFIG */
    //lq 配置见"Packet/protocol configuration registers"部分

    /*lq -----------------General information----------------*/
#if 0
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GI_PARTNUM);
    if (cs == SPIRIT1TypeA)
    {
        usart_printf("$SPIRIT1_A: Device part number %02X!\r\n", regValue);        
    }
    else
    {
        usart_printf("$SPIRIT1_B: Device part number %02X!\r\n", regValue);            
    } 

    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GI_VERSION);
    if (cs == SPIRIT1TypeA)
    {
        usart_printf("$SPIRIT1_A: Device version number %02X!\r\n", regValue);        
    }
    else
    {
        usart_printf("$SPIRIT1_B: Device version number %02X!\r\n", regValue);            
    }
#endif
#if 0    
    /*lq 读取配置的寄存器 */
    SPIRIT1_ReadRegisters(cs); 
#endif
    /*lq 进入就绪状态 */        
    SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_READY);
}

/*******************************************************************************
* 名称  : SPIRIT1_InstConfiguration
* 描述  : 配置SPIRIT1
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPIRIT1_InstConfiguration(void)
{
    SPIRIT1_Configuration(SPIRIT1TypeA);
    SPIRIT1_Configuration(SPIRIT1TypeB);
#if 1
    /*lq 进入接收状态 */
    SPIRIT1_WriteCommand(SPIRIT1TypeA, SPIRIT1_COMMAND_RX); //lq SPIRIT1TypeB配置完成后，再进入RX态（因为若GPIO_RX_CLOCK时钟中断的优先级高于模拟SPI时钟定时（Tim）中断的优先级，会导致模拟SPI时钟频率变慢，引起看门狗复位）
    SPIRIT1_WriteCommand(SPIRIT1TypeB, SPIRIT1_COMMAND_RX); 
#endif
}

/*******************************************************************************
* 名称  : SPIRIT1_InstConfigure
* 描述  : 配置SPIRIT1
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPIRIT1_ReadRegisters(SPIRIT1TypeEnum cs)
{
    U8 regValue;
    U8 temp[2];

    if (cs == SPIRIT1TypeA)
    {
        temp[0] = 'A';
        temp[1] = '\0';
    }
    else
    {
        temp[0] = 'B';
        temp[1] = '\0';    
    }

    /*lq -----------------General configuration registers----------------*/
    /*lq ANA_FUNC_CONF[1] */
    //lq 默认值

    /*lq ANA_FUNC_CONF[0] */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_ANA_FUNC_CONF0); //lq reference signal from XIN pin
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_ANA_FUNC_CONF0,
                regValue, 0xD0);        

    /*lq GPIO3_CONF */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_GPIO3_CONF); //lq GPIO3: RX clock
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_GPIO3_CONF,
                regValue, (SPIRIT1_GPIO_FUC_OUT_RX_CLOCK | SPIRIT1_GPIO_MODE_DOUT_HIGH)); 

    /*lq GPIO2_CONF */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_GPIO2_CONF); //lq GPIO2: RX data
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_GPIO2_CONF,
                regValue, (SPIRIT1_GPIO_FUC_OUT_RX_DATA | SPIRIT1_GPIO_MODE_DOUT_LOW)); 

    /*lq GPIO1_CONF */
    //lq 默认值

    /*lq GPIO0_CONF */
    //lq 默认值

    if (cs == SPIRIT1TypeA)
    {
        /*lq GPIO3_CONF */
        regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_GPIO3_CONF);    //lq GPIO3: RX clock
        usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_GPIO3_CONF,
                    regValue, (SPIRIT1_GPIO_FUC_OUT_RX_CLOCK | SPIRIT1_GPIO_MODE_DOUT_LOW)); 
    
        /*lq GPIO2_CONF */
        regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_GPIO2_CONF);   //lq GPIO2: TX clock
        usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_GPIO2_CONF,
                    regValue, (SPIRIT1_GPIO_FUC_OUT_TX_CLOCK | SPIRIT1_GPIO_MODE_DOUT_HIGH));
                        
        /*lq GPIO1_CONF */
        regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_GPIO1_CONF);   //lq GPIO1: RX data
        usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_GPIO1_CONF,
                    regValue, (SPIRIT1_GPIO_FUC_OUT_RX_DATA | SPIRIT1_GPIO_MODE_DOUT_LOW));
    
        /*lq GPIO0_CONF */
        regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_GPIO0_CONF);   //lq GPIO0: TX data
        usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_GPIO0_CONF,
                    regValue, (SPIRIT1_GPIO_FUC_IN_TX_DATA | SPIRIT1_GPIO_MODE_DOUT_LOW));
    }
    else if (cs == SPIRIT1TypeB)
    {
        /*lq GPIO3_CONF */
        regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_GPIO3_CONF);     //lq GPIO2: RX data
        usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_GPIO3_CONF,
                    regValue, (SPIRIT1_GPIO_FUC_OUT_RX_DATA | SPIRIT1_GPIO_MODE_DOUT_LOW)); 
    
        /*lq GPIO2_CONF */
        regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_GPIO2_CONF);     //lq GPIO2: TX data
        usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_GPIO2_CONF,
                    regValue, (SPIRIT1_GPIO_FUC_IN_TX_DATA | SPIRIT1_GPIO_MODE_DOUT_LOW)); 
    
        /*lq GPIO1_CONF */
        regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_GPIO1_CONF);     //lq GPIO1: RX clock
        usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_GPIO1_CONF,
                    regValue, (SPIRIT1_GPIO_FUC_OUT_RX_CLOCK | SPIRIT1_GPIO_MODE_DOUT_LOW)); 
    
        /*lq GPIO0_CONF */
        regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_GPIO0_CONF);     //lq GPIO0: TX clock
        usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_GPIO0_CONF,
                    regValue, (SPIRIT1_GPIO_FUC_OUT_TX_CLOCK | SPIRIT1_GPIO_MODE_DOUT_HIGH)); 

    }

    /*lq MCU_CK_CONF */
    //lq 默认值

    /*lq XO_RCO_TEST */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_XO_RCO_TEST);    //lq... PD_CLKDIV = 1(安全做法是先进入standby状态，见手册8.5)
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_XO_RCO_TEST,
                regValue, 0x29);        
    
    /*lq SYNTH_CONFIG[0] */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_GC_SYNTH_CONFIG0);  //lq split time: 3.47 ns
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_GC_SYNTH_CONFIG0,
                regValue, 0xA0);        

    /*lq SYNTH_CONFIG[1] */
    //lq 默认值 

    /*lq IF_OFFSET_ANA */
    //lq 默认值

    /*lq -----------------Radio configuration registers (analog blocks)----------------*/
    /*lq SYNT3 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCA_SYNT3);     //lq SYNT = 26017792
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCA_SYNT3,
                regValue, 0x0C);        

    /*lq SYNT2 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCA_SYNT2);     //lq SYNT = 26017792
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCA_SYNT2,
                regValue, 0x68);        

    /*lq SYNT1 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCA_SYNT1);     //lq SYNT = 26017792
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCA_SYNT1,
                regValue, 0x00);        

    /*lq SYNT0 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCA_SYNT0);     //lq SYNT = 26017792，BS = 5(B = 32)
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCA_SYNT0,
                regValue, 0x05);        

    /*lq CHSPACE */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCA_CHSPACE);   //lq CHSPACE = 63
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCA_CHSPACE,
                regValue, 0x3F);        

    /*lq IF_OFFSET_DIG */
    //lq 默认值

    /*lq FC_OFFSET1 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCA_FC_OFFSET1); //lq FC_OFFSET = -57
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCA_FC_OFFSET1,
                regValue, 0x0F);        

    /*lq FC_OFFSET0 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCA_FC_OFFSET0); //lq FC_OFFSET = -57
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCA_FC_OFFSET0,
                regValue, 0xC7);        

    /*lq PA_POWER0-8 */
    //lq 默认值

    /*lq -----------------Radio configuration registers (digital blocks)----------------*/
    /*lq MOD1 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCD_MOD1);      //lq DATARATE_M = 131（datarate=9600）
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCD_MOD1,
                regValue, 0x83);        

    /*lq MOD0 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCD_MOD0);      //lq BT = 0.5, MOD_TYPE = GFSK, DATARATE_E = 8（datarate=9600）
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCD_MOD0,
                regValue, 0x58);        

    /*lq FDEV0 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCD_FDEV0);     //lq FDEV_M = 4, FDEV_E = 2
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCD_FDEV0,
                regValue, 0x24);        

    /*lq CHFLT */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCD_CHFLT);     //lq CHFLT_M = 2, CHFLT_E = 5
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCD_CHFLT,
                regValue, 0x25);        

    /*lq AFC2 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCD_AFC2);      //lq... AFC_ENABLE = 0
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCD_AFC2,
                regValue, 0x08);        

    /*lq AFC1 */
    //lq 默认值

    /*lq AFC0 */
    //lq 默认值

    /*lq RSSI_FLT */
    //lq 默认值

    /*lq RSSI_TH */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCD_RSSI_TH);     //lq RSSI_TH = -123
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_RCD_RSSI_TH,
                regValue, 0x0E);

    /*lq CLOCKREC */
    //lq 默认值

    /*lq AGCCTRL2 */
    //lq 默认值             

    /*lq AGCCTRL1 */
    //lq 默认值             

    /*lq AGCCTRL0 */
    //lq 默认值                   

    /*lq ANT_SELECT_CONF */
    //lq 默认值 

    /*lq -----------------Packet/protocol configuration registers----------------*/
    /*lq PCKTCTRL4 */
    //lq 默认值

    /*lq PCKTCTRL3 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(3));      //lq RX mode: direct through GPIO,
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_PPC_PCKTCTRL(3),
                regValue, 0x27);        

    /*lq PCKTCTRL2 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(2));      //lq SYNC_LENGTH = 1，
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_PPC_PCKTCTRL(2),
                regValue, 0x1A);        

    /*lq PCKTCTRL1 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(1));      //lq CRC: No CRC, TX source data: direct through GPIO,
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_PPC_PCKTCTRL(1),
                regValue, 0x08);        

    /*lq PCKTLEN1 */
    //lq 默认值

    /*lq PCKTLEN0 */
    //lq 默认值

    /*lq SYNC4 */
    //lq 默认值

    /*lq SYNC3 */
    //lq 默认值

    /*lq SYNC2 */
    //lq 默认值

    /*lq SYNC1 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_PPC_SYNC(1));           //lq SYNC1: 0x33(使用了NRZI编码，对应未使用NRZI的01010101)
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_PPC_SYNC(1),
                regValue, 0x01);        

    /*lq QI */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_PPC_QI);                //lq... SQI_EN: disable SQI
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_PPC_QI,
                regValue, 0x00);        


    /*lq MBUS_PRMBL */
    //lq 默认值

    /*lq MBUS_PSTMBL */
    //lq 默认值

    /*lq MBUS_CTRL */
    //lq 默认值

    /*lq FIFO_CONFIG3 */
    //lq 默认值

    /*lq FIFO_CONFIG2 */
    //lq 默认值

    /*lq FIFO_CONFIG1 */
    //lq 默认值

    /*lq FIFO_CONFIG0 */
    //lq 默认值

    /*lq PCKT_FLT_GOALS[12-0] */
    //lq 默认值

    /*lq PCKT_FLT_OPTIONS */
    //lq 默认值

    /*lq PROTOCOL2 */
    //lq 默认值

    /*lq PROTOCOL1 */
    //lq 默认值

    /*lq PROTOCOL0 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_PPC_PROTOCOL(0));      //lq PERS_RX: persistent reception enabled
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_PPC_PROTOCOL(0),
                regValue, 0x0A);        

    /*lq TIMERS[5-0] */
    //lq 默认值

    /*lq CSMA_CONFIG[3-0] */
    //lq 默认值

    /*lq TX_CTRL_FIELD[3-0] */
    //lq 默认值

    /*lq PM_CONFIG2 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_PPC_PM_CONFIG(2));      //lq disable internal SMPS      
    usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_PPC_PM_CONFIG(2),
                regValue, 0x2C);        

    /*lq PM_CONFIG1 */
    //lq 默认值

    /*lq PM_CONFIG0 */
    //lq 默认值

    /*lq XO_RCO_CONFIG */
    //lq 默认值

    /*lq TEST_SELECT */
    //lq 默认值

    /*lq PM_TEST */
    //lq 默认值

    /*lq -----------------Frequently used registers----------------*/
    /*lq CHNUM */
    if (cs == SPIRIT1TypeA)
    {
        regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_FU_CHNUM); //lq CHNUM 对应161.975MHz    
        usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_FU_CHNUM,
                     regValue, SPIRIT1_PARA_CH_NUM_A);        

    }
    else
    {
        regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_FU_CHNUM); //lq CHNUM 对应162.025MHz        
        usart_printf("$SPIRIT_%s: [%02X] %02X %02X!\r\n", temp, SPIRIT1_REGADDR_FU_CHNUM,
                     regValue, SPIRIT1_PARA_CH_NUM_B);  
    }

    /*lq VCO_CONFIG */
    //lq 默认值

    /*lq RCO_VCO_CALIBR_IN[2-0] */
    //lq 默认值

    /*lq AES_KEY_IN[15-0] */
    //lq 默认值

    /*lq AES_DATA_IN[15-0] */
    //lq 默认值

    /*lq IRQ_MASK[3-0] */
    //lq 默认值

    /*lq IRQ_MASK[3-0] */
    //lq 默认值

    /*lq DEM_CONFIG */
    //lq... 默认值

    /*lq PM_CONFIG */
    //lq 配置见"Packet/protocol configuration registers"部分                
}

/*******************************************************************************
* 名称  : SPIRIT1_GetBitNum
* 描述  : 根据掩码值返回位序号（从0开始）
* 输入  : bitMask - 位掩码(位掩码中的1必须连续)	  
* 输出  : 无
* 返回  : 掩码来对应的位序号（即位掩码中最低位的1的位序号）
*******************************************************************************/
U8 SPIRIT1_GetBitNum(const U8 bitMask)
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
* 名称  : SPIRIT1_GetRegFieldValue
* 描述  : 获取SPIRIT1的当前状态
* 输入  : cs - SPIRIT1芯片类型
          regAddr - 寄存器地址
          bitMask - 字段对应的位掩码	  
* 输出  : 无
* 返回  : regValue - 位掩码对应的值
*******************************************************************************/
U8 SPIRIT1_GetRegFieldValue(SPIRIT1TypeEnum cs, U8 regAddr, U8 bitMask)
{
    U8 regValue;

    regValue = SPIRIT1_ReadByte(cs, regAddr);
    regValue &= bitMask;
    regValue >>= SPIRIT1_GetBitNum(bitMask);
    
    return regValue;  
}

/*******************************************************************************
* 名称  : SPIRIT1_GetMcState
* 描述  : 获取SPIRIT1的当前状态
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : regValue - SPIRIT1的当前状态
*******************************************************************************/
U8 SPIRIT1_GetMcState(SPIRIT1TypeEnum cs)
{
    U8 value;

    value = SPIRIT1_GetRegFieldValue(cs, SPIRIT1_REGADDR_FU_MC_STATE(0),
                                     SPIRIT1_FU_MC_STATE0_BITMASK_STATE);
    
    return value;   
}

/*******************************************************************************
* 名称  : SPIRIT1_IsXoOn
* 描述  : 获取SPIRIT1的当前XO_ON状态
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : value - SPIRIT1的当前状态
*******************************************************************************/
U8 SPIRIT1_IsXoOn(SPIRIT1TypeEnum cs)
{
    U8 value;
    
    value = SPIRIT1_GetRegFieldValue(cs, SPIRIT1_REGADDR_FU_MC_STATE(0),
                                     SPIRIT1_FU_MC_STATE0_BITMASK_XO_ON);

    return value;                                             
}

/*******************************************************************************
* 名称  : SPIRIT1_GetAntSelect
* 描述  : 获取SPIRIT1的当前天线选择状态
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : value - SPIRIT1的当前状态
*******************************************************************************/
U8 SPIRIT1_GetAntSelect(SPIRIT1TypeEnum cs)
{
    U8 value;
    
    value = SPIRIT1_GetRegFieldValue(cs, SPIRIT1_REGADDR_FU_MC_STATE(1),
                                     SPIRIT1_FU_MC_STATE1_BITMASK_ANT_SELECT);

    return value;                                             
}

/*******************************************************************************
* 名称  : SPIRIT1_IsTxFifoFull
* 描述  : 获取SPIRIT1的当前TX_FIFO状态
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : value - SPIRIT1的当前状态
*******************************************************************************/
U8 SPIRIT1_IsTxFifoFull(SPIRIT1TypeEnum cs)
{
    U8 value;
    
    value = SPIRIT1_GetRegFieldValue(cs, SPIRIT1_REGADDR_FU_MC_STATE(1),
                                     SPIRIT1_FU_MC_STATE1_BITMASK_TX_FIFO_FULL);

    return value;                                             
}

/*******************************************************************************
* 名称  : SPIRIT1_IsTxFifoFull
* 描述  : 获取SPIRIT1的当前RX_FIFO状态
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : value - SPIRIT1的当前状态
*******************************************************************************/
U8 SPIRIT1_IsRxFifoEmpty(SPIRIT1TypeEnum cs)
{
    U8 value;
    
    value = SPIRIT1_GetRegFieldValue(cs, SPIRIT1_REGADDR_FU_MC_STATE(1),
                                     SPIRIT1_FU_MC_STATE1_BITMASK_RX_FIFO_EMPTY);

    return value;                                             
}

/*******************************************************************************
* 名称  : SPIRIT1_IsTxFifoFull
* 描述  : 获取SPIRIT1的当前Rco calibrator状态
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : value - SPIRIT1的当前状态
*******************************************************************************/
U8 SPIRIT1_IsRcoCalibError(SPIRIT1TypeEnum cs)
{
    U8 value;
    
    value = SPIRIT1_GetRegFieldValue(cs, SPIRIT1_REGADDR_FU_MC_STATE(1),
                                     SPIRIT1_FU_MC_STATE1_BITMASK_ERROR_LOCK);

    return value;                                             
}

/*******************************************************************************
* 名称  : SPIRIT1_GetVcoCalibrData
* 描述  : 获取SPIRIT1的当前VCO calibrator值
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : value - SPIRIT1的当前VCO calibrator值
*******************************************************************************/
U8 SPIRIT1_GetVcoCalibrData(SPIRIT1TypeEnum cs)
{
    U8 value;
    
    value = SPIRIT1_GetRegFieldValue(cs, SPIRIT1_REGADDR_FU_RCO_VCO_CALIBR_OUT(0),
                                     SPIRIT1_FU_RCO_VCO_CALIBR_OUT0_BITMASK_VCO_CALIBR_DATA);

    return value; 
}

/*******************************************************************************
* 名称  : SPIRIT1_GetVcoCalibrData
* 描述  : 获取SPIRIT1的当前VCO calibrator TX值
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : value - SPIRIT1的当前VCO calibrator TX值
*******************************************************************************/
U8 SPIRIT1_GetVcoCalibrDataTx(SPIRIT1TypeEnum cs)
{
    U8 value;
    
    value = SPIRIT1_GetRegFieldValue(cs, SPIRIT1_REGADDR_FU_RCO_VCO_CALIBR_IN(1),
                                     SPIRIT1_FU_RCO_VCO_CALIBR_IN1_BITMASK_VCO_CALIBR_TX);

    return value; 
}

/*******************************************************************************
* 名称  : SPIRIT1_GetVcoCalibrDataRx
* 描述  : 获取SPIRIT1的当前VCO calibrator RX值
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : value - SPIRIT1的当前VCO calibrator RX值
*******************************************************************************/
U8 SPIRIT1_GetVcoCalibrDataRx(SPIRIT1TypeEnum cs)
{
    U8 value;
    
    value = SPIRIT1_GetRegFieldValue(cs, SPIRIT1_REGADDR_FU_RCO_VCO_CALIBR_IN(0),
                                     SPIRIT1_FU_RCO_VCO_CALIBR_IN0_BITMASK_VCO_CALIBR_RX);

    return value; 
}

/*******************************************************************************
* 名称  : SPIRIT1_CfgCwTx
* 描述  : 连续载波发射配置
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPIRIT1_CfgCwTx(SPIRIT1TypeEnum cs)
{
    U8 regValue;

    /*lq 进入Ready态 */
    if (SPIRIT1_GetMcState(cs) == SPIRIT1_MC_STATE_RX
        || SPIRIT1_GetMcState(cs) == SPIRIT1_MC_STATE_TX)
    {
        /*lq 退出收发状态 */
        SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_SABORT);
    }
    else
    {
        SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_READY);
    }

    /*lq MOD0 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCD_MOD0);
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCD_MOD0,
                      regValue | SPIRIT1_RCD_MOD0_BITMASK_CW);              //lq CW: enable，BT = 0.5, MOD_TYPE = GFSK, DATARATE_E = 8（datarate=9600）
        
    /*lq PCKTCTRL1 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(1));
    regValue &= ~SPIRIT1_PPC_PCKTCTRL1_BITMASK_TXSOURCE;                    //lq TXSOURCE掩码位清0 
    regValue |= SPIRIT1_TX_SOURCE_DATA_PN9 << SPIRIT1_GetBitNum(SPIRIT1_PPC_PCKTCTRL1_BITMASK_TXSOURCE);     
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(1), regValue);       //lq CRC: No CRC, TX source data: PN9,

    /*lq 进入TX态 */
    SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_TX);        
}

/*******************************************************************************
* 名称  : SPIRIT1_CfgPN9Tx
* 描述  : 连续伪随机发射配置
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPIRIT1_CfgPN9Tx(SPIRIT1TypeEnum cs)
{
    U8 regValue;

    /*lq 进入Ready态 */
    if (SPIRIT1_GetMcState(cs) == SPIRIT1_MC_STATE_RX
        || SPIRIT1_GetMcState(cs) == SPIRIT1_MC_STATE_TX)
    {
        /*lq 退出收发状态 */
        SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_SABORT);
    }
    else
    {
        SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_READY);
    }

    /*lq MOD0 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCD_MOD0);
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCD_MOD0,
                      regValue & (~SPIRIT1_RCD_MOD0_BITMASK_CW));           //lq CW: disable，BT = 0.5, MOD_TYPE = GFSK, DATARATE_E = 8（datarate=9600）

    /*lq PCKTCTRL1 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(1));
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(1),
                      regValue | SPIRIT1_PPC_PCKTCTRL1_BITMASK_TXSOURCE);   //lq CRC: No CRC, TX source data: PN9,

    /*lq 进入TX态 */
    SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_TX);        
}

/*******************************************************************************
* 名称  : SPIRIT1_CfgGpioTx
* 描述  : GPIO直接发射配置
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPIRIT1_CfgGpioTx(SPIRIT1TypeEnum cs)
{
    U8 regValue;

    /*lq 进入Ready态 */
    if (SPIRIT1_GetMcState(cs) == SPIRIT1_MC_STATE_RX
        || SPIRIT1_GetMcState(cs) == SPIRIT1_MC_STATE_TX)
    {
        /*lq 退出收发状态 */
        SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_SABORT);
    }
    else
    {
        SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_READY);
    }

    /*lq MOD0 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_RCD_MOD0);
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_RCD_MOD0,
                      regValue & (~SPIRIT1_RCD_MOD0_BITMASK_CW));           //lq CW: disable，BT = 0.5, MOD_TYPE = GFSK, DATARATE_E = 8（datarate=9600）

    /*lq PCKTCTRL1 */
    regValue = SPIRIT1_ReadByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(1));
    regValue &= ~SPIRIT1_PPC_PCKTCTRL1_BITMASK_TXSOURCE;                    //lq TXSOURCE掩码位清0 
    regValue |= SPIRIT1_TX_SOURCE_DATA_GPIO << SPIRIT1_GetBitNum(SPIRIT1_PPC_PCKTCTRL1_BITMASK_TXSOURCE);     
    SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_PPC_PCKTCTRL(1), regValue);       //lq CRC: No CRC, TX source data: GPIO,

    /*lq 进入TX态 */
    SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_TX);        
}

/*******************************************************************************
* 名称  : SPIRIT1_ToReady
* 描述  : 进入ready态
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPIRIT1_StateToReady(SPIRIT1TypeEnum cs)
{
    if (SPIRIT1_GetMcState(cs) != SPIRIT1_MC_STATE_READY)
    {
        /*lq 进入Ready态 */
        if (SPIRIT1_GetMcState(cs) == SPIRIT1_MC_STATE_RX
            || SPIRIT1_GetMcState(cs) == SPIRIT1_MC_STATE_TX)
        {
            /*lq 退出收发状态 */
            SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_SABORT);
        }
        else
        {
            SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_READY);
        }     
    }       
} 

/*******************************************************************************
* 名称  : SPIRIT1_StateToTx
* 描述  : 进入Tx态
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPIRIT1_StateToTx(SPIRIT1TypeEnum cs)
{
    /*lq 进入Ready态 */
    SPIRIT1_StateToReady(cs);        

    /*lq 进入Tx态 */
    SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_TX);
}

/*******************************************************************************
* 名称  : SPIRIT1_StateToRx
* 描述  : 进入Rx态
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPIRIT1_StateToRx(SPIRIT1TypeEnum cs)
{
    /*lq 进入Ready态 */
    SPIRIT1_StateToReady(cs);        

    /*lq 进入Tx态 */
    SPIRIT1_WriteCommand(cs, SPIRIT1_COMMAND_RX);
}

/*******************************************************************************
* 名称  : SPIRIT1_CfgFreq
* 描述  : 配置频率
* 输入  : cs - SPIRIT1芯片类型
          channel - 信道	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPIRIT1_CfgFreq(SPIRIT1TypeEnum cs, U8 channel)
{
    if (channel == AIS_CHANNEL_A)
    {
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_FU_CHNUM, SPIRIT1_PARA_CH_NUM_A); //lq CHNUM 对应161.975MHz                
    }
    else if (channel == AIS_CHANNEL_B)
    {
        SPIRIT1_WriteByte(cs, SPIRIT1_REGADDR_FU_CHNUM, SPIRIT1_PARA_CH_NUM_B); //lq CHNUM 对应162.025MHz                    
    }
} 

/*******************************************************************************
* 名称  : SPIRIT1_TxCfg
* 描述  : 发射配置
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPIRIT1_TxCfg(SPIRIT1TypeEnum cs, U8 channel)
{
    SPIRIT1_StateToReady(cs);
    SPIRIT1_CfgFreq(cs, channel);
    SPIRIT1_StateToTx(cs);
}   

/*******************************************************************************
* 名称  : SPIRIT1_RxCfg
* 描述  : 接收配置
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void SPIRIT1_RxCfg(SPIRIT1TypeEnum cs, U8 channel)
{
    SPIRIT1_StateToReady(cs);
    SPIRIT1_CfgFreq(cs, channel);
    SPIRIT1_StateToRx(cs);
} 

/*******************************************************************************
* 名称  : SPIRIT1_IsTxDone
* 描述  : 判断发射是否完成
* 输入  : cs - SPIRIT1芯片类型	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 SPIRIT1_IsTxDone(SPIRIT1TypeEnum cs)
{
    return TRUE;    //lq... 发射完成表示暂时未定
}
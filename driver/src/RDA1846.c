/******************** (C) COPYRIGHT 712 ***************************************
* File Name          : RDA1846.c
* Author             : Mobile Team 5
* Version            : V0.0.1
* Date               : 2011/09/01
* Description        : 这个文件对RDA1846进行配置,实现射频的控收/控发。
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "def.h"
#include "RDA1846.h"
#include "sys.h"
#include "common.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//SPI端口定义
#define SEN1 PBout(1)	// PG14	
#define SEN2 PCout(6)	// PG14	
#define SCLK PBout(11)	// PG12
#define SDI PBout(10)	// PG13 //lq SDI以RDA1846为参考
#define SDO PBin(10)	// PG13 //lq SDO以RDA1846为参考
//RDA1846 模式设置管脚
#define PDN1 PBout(0)   // PG11
#define PDN2 PCout(7)   // PG11

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u16 Reg29_2A[2] = {0x0000, 0x0000};				//用于计算1846寄存器0x29 0x2A的值
u16 Rda1_Reg30 = 0x0000;								//记录睡眠时 收发的状态
u16 Rda2_Reg30 = 0x0000;								//记录睡眠时 收发的状态

stRDA1846Freq RDA1846Freq = {0, 0};
stRDA1846SQ RDA1846SQ = {Off, 0, 0};

/* Private function prototypes -----------------------------------------------*/
void WriteRda1846(u8 address, u16 data, u8 cs);		//往Rda1846寄存器里写数据,子函数，通过IO口模拟的方式
void Wr_RDA1846reg(u8 address, u16 data, u8 cs);		//写寄存器值函数
u16 ReadRda1846(u8 address, u8 cs);					//读数据从Rda1846寄存器，子函数，通过IO口模拟的方式
void Cal_TxRx_Freq(u32 Hz,u16 *Reg29_2A);		//计算发送和接收的频点
void RDA1846_SetRegVal(u8 address, u16 data, u8 cs);	//向1846寄存器打入数据
void RDA1846_Port_Init(void);					//RDA1846控制管脚初始化

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : RDA1846_Port_Init
* Description    : RDA1846控制管脚初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RDA1846_Port_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能PORTB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//使能PORTC时钟

    /*lq SCLK */
	GPIO_InitStructure.GPIO_Pin = RDA1846_SCLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RDA1846_SCLK_PORT, &GPIO_InitStructure);

    /*lq SDIO */
	GPIO_InitStructure.GPIO_Pin = RDA1846_SDIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RDA1846_SDIO_PORT, &GPIO_InitStructure);

    /*lq SEN1 */
	GPIO_InitStructure.GPIO_Pin = RDA1846_SEN1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RDA1846_SEN1_PORT, &GPIO_InitStructure);

    /*lq SEN2 */
	GPIO_InitStructure.GPIO_Pin = RDA1846_SEN2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RDA1846_SEN2_PORT, &GPIO_InitStructure);

    /*lq PDN1 */
	GPIO_InitStructure.GPIO_Pin = RDA1846_PDN1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RDA1846_PDN1_PORT, &GPIO_InitStructure);

    /*lq PDN2 */
	GPIO_InitStructure.GPIO_Pin = RDA1846_PDN2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RDA1846_PDN2_PORT, &GPIO_InitStructure);
                    	
	SCLK = 0;	//在不用的时候，将时钟信号拉低
	SDI = 1;	//数据输出，在不用的时候拉高 	
    SEN1 = 1;	//cs 无效
	PDN1 = 1;

	SEN2 = 1;	//cs 无效
	PDN2 = 1;
}

/*******************************************************************************
* Function Name  : WriteRda1846
* Description    : 往Rda1846寄存器里写数据,子函数，通过IO口模拟的方式
* Input          : 8bit address, 16bit data
* Output         : None
* Return         : None
*******************************************************************************/
void WriteRda1846(u8 address, u16 data, u8 cs)
{
	u8 i;
	u32 tmp_rda;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// SDIO (O)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能PORTG时钟
	GPIO_InitStructure.GPIO_Pin = RDA1846_SDIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //PG13推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RDA1846_SDIO_PORT, &GPIO_InitStructure);    
	
	tmp_rda = (((u32)address << 16) | data); // 写标志 + 地址 + 数据
	
	if (cs == RDA1846_CS_1)
    {
        SEN1 = 0;	//cs 有效    
    }
    else
    {
        SEN2 = 0;	//cs 有效        
    }

	for(i = 0; i < 24; i++)
	{
		SCLK = 0;
		if((tmp_rda & 0x800000) == 0x800000)
		{
			SDI = 1;
		}
		else
		{
			SDI = 0;
		}
		SCLK = 1;
		tmp_rda <<= 1;
	}


	SCLK = 0;

	if (cs == RDA1846_CS_1)
    {
	    SEN1 = 1;	//cs 无效
    }
    else
    {
	    SEN2 = 1;	//cs 无效
    }

	SDI = 1;
}

/*******************************************************************************
* Function Name  : Wr_RDA1846reg
* Description    : 往Rda1846寄存器里写数据
* Input          : 8bit address， 16bit data
* Output         : None
* Return         : None
*******************************************************************************/
void Wr_RDA1846reg(u8 address, u16 data, u8 cs)	//写函数
{
	if(address > 0x7F)	//如果地址大于0x7F
	{
		WriteRda1846(0x7F, 0x0001, cs);
		WriteRda1846((address-0x80), data, cs);
		WriteRda1846(0x7F, 0x0000, cs);
	}
	else				//正常情况下
	{
		WriteRda1846(address, data, cs);
	}
}

/*******************************************************************************
* Function Name  : ReadRda1846
* Description    : 读数据从Rda1846寄存器，子函数，通过IO口模拟的方式
* Input          : 8bit address
* Output         : 16bit data
* Return         : None
*******************************************************************************/
u16 ReadRda1846(u8 address, u8 cs)
{
	u8 i;
	u16 data;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// SDIO (O)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能PORTG时钟
	GPIO_InitStructure.GPIO_Pin = RDA1846_SDIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //PG13推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RDA1846_SDIO_PORT, &GPIO_InitStructure);
	
	//=====地址byte中的第一个bit代表读（1）或者写（0）====
	address |= 0x80;	// 读标志 + 地址

	if (cs == RDA1846_CS_1)
    {
        SEN1 = 0;	//cs 有效    
    }
    else
    {
        SEN2 = 0;	//cs 有效        
    }
    
    for (i = 0; i < 8; i++)
	{
		SCLK = 0;
		if((address & 0x80) == 0x80)
		{
			SDI = 1;
		}
		else
		{
			SDI = 0;
		}
		address <<=1;
		SCLK = 1;
	}
	SCLK = 0;
	
	// SDIO (I)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能PORTG时钟
	GPIO_InitStructure.GPIO_Pin = RDA1846_SDIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //PG13浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RDA1846_SDIO_PORT, &GPIO_InitStructure);
	
	data = 0x0000;	//将变量清零
	for(i = 0; i < 16; i++)	//获取数据
	{
    	SCLK = 1;
    	data <<= 1;
    	if(SDO)
    	{
    	  data += 1;
    	}
    	SCLK = 0;
	}

	if (cs == RDA1846_CS_1)
    {
	    SEN1 = 1;	//cs 无效
    }
    else
    {
	    SEN2 = 1;	//cs 无效
    }
    
    SDI = 1;
	
    return data;
}

/*******************************************************************************
* Function Name  : Rd_RDA1846reg
* Description    : 读数据从Rda1846寄存器
* Input          : 8bit address
* Output         : 16bit data
* Return         : None
*******************************************************************************/
u16 Rd_RDA1846reg(u8 address, u8 cs)	//读函数
{
	u16 temp = 0x0000;
	if(address > 0x7F)		//如果地址大于0x7F
	{
    	WriteRda1846(0x7F, 0x0001, cs);
    	temp = ReadRda1846(address-0x80, cs);
    	WriteRda1846(0x7F, 0x0000, cs);
	}
	else					//正常情况下
	{
	    temp = ReadRda1846(address, cs);
	}

	return temp;
}

/*******************************************************************************
* Function Name  : Cal_TxRx_Freq
* Description    : 计算发送和接收的频点
* Input          : 32bit XX Hz,16bit 数组指针
* Output         : 16bit 数组值
* Return         : None
*******************************************************************************/
void Cal_TxRx_Freq(u32 Hz,u16 *Reg29_2A)
{
	*(Reg29_2A++)=((u32)(Hz/125))>>16;
	*Reg29_2A=((u32)(Hz/125))&0xFFFF;
}

/*******************************************************************************
* Function Name  : RDA1846_SetRegVal
* Description    : 输入寄存器值,再读回来 验证读写正确 测试版程序使用 正式版只使用Wr_RDA1846reg函数
* Input          : u8 address, u16 data
* Output         : 打印返回值
* Return         : None
*******************************************************************************/
void RDA1846_SetRegVal(u8 address, u16 data, u8 cs)
{
	Wr_RDA1846reg(address, data, cs);
}

/*******************************************************************************
* Function Name  : Init_RDA1846
* Description    : 初始化RDA1846函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Init_RDA1846(void)
{
	RDA1846_Port_Init();
	
	//25k
    /*lq RDA1846-1*/	
	RDA1846_SetRegVal(0x30,0x0001, RDA1846_CS_1);
	RDA1846_SetRegVal(0x30,0x0004, RDA1846_CS_1);
	RDA1846_SetRegVal(0x02,0x0698, RDA1846_CS_1);
	RDA1846_SetRegVal(0x04,0x0FD0, RDA1846_CS_1);
	RDA1846_SetRegVal(0x08,0x02A0, RDA1846_CS_1);
	RDA1846_SetRegVal(0x0B,0x1A10, RDA1846_CS_1);
	RDA1846_SetRegVal(0x2B,0x32C8, RDA1846_CS_1);
	RDA1846_SetRegVal(0x2C,0x1964, RDA1846_CS_1);
	RDA1846_SetRegVal(0x32,0x627C, RDA1846_CS_1);
	RDA1846_SetRegVal(0x33,0x0AF2, RDA1846_CS_1);
	RDA1846_SetRegVal(0x47,0x2C2F, RDA1846_CS_1);
	RDA1846_SetRegVal(0x4E,0x293A, RDA1846_CS_1);
	RDA1846_SetRegVal(0x54,0x1D4C, RDA1846_CS_1);
	RDA1846_SetRegVal(0x56,0x0652, RDA1846_CS_1);
	RDA1846_SetRegVal(0x6E,0x062D, RDA1846_CS_1);
	RDA1846_SetRegVal(0x70,0x1029, RDA1846_CS_1);
	//If 0x85 is  0x001F, Rfoutput power is 8dBm , ACP is -63dB in 12.5KHz and -65dB in 25KHz
	//If 0x85 is  0x0018, Rfoutput power is 6dBm , ACP is -64dB in 12.5KHz and -66dB in 25KHz
	//If 0x85 is  0x0017, Rfoutput power is -3dBm , ACP is -68dBc in 12.5KHz and -68dBc in 25KHz
	RDA1846_SetRegVal(0x85,0x0018, RDA1846_CS_1);    	

#if 0	//高通normal  voice lpf bypass == normal
	RDA1846_SetRegVal(0x58,0x03CF, RDA1846_CS_1);
#else	//高通旁路
	RDA1846_SetRegVal(0x58,0x03FF, RDA1846_CS_1);
#endif
	RDA1846_SetRegVal(0x1F,0x1000, RDA1846_CS_1);	//set gpio sq on
	RDA1846_SetRegVal(0x0F,0x00E4, RDA1846_CS_1);	//lq 设置RF band 为134~174MHz
	
	RDA1846_SetRegVal(0x30,0x30A4, RDA1846_CS_1);
	Delay_ms(50);
	RDA1846_SetRegVal(0x30,0x30A6, RDA1846_CS_1);
	Delay_ms(100);
	RDA1846_SetRegVal(0x30,0x3006, RDA1846_CS_1);
	Delay_ms(100);
	
    Init_RDA1846_Rx(161975000, RDA1846_CS_1);
#if 0    
    Init_RDA1846_SQ(135 - 30, 135 - 70, RDA1846_CS_1);  //lq (135+ Sq_open_threshlod),(135+ Sq_shut_threshlod)
#endif
    
    /*lq RDA1846-2*/	
	RDA1846_SetRegVal(0x30,0x0001, RDA1846_CS_2);
	RDA1846_SetRegVal(0x30,0x0004, RDA1846_CS_2);
	RDA1846_SetRegVal(0x02,0x0698, RDA1846_CS_2);
	RDA1846_SetRegVal(0x04,0x0FD0, RDA1846_CS_2);
	RDA1846_SetRegVal(0x08,0x02A0, RDA1846_CS_2);
	RDA1846_SetRegVal(0x0B,0x1A10, RDA1846_CS_2);
	RDA1846_SetRegVal(0x2B,0x32C8, RDA1846_CS_2);
	RDA1846_SetRegVal(0x2C,0x1964, RDA1846_CS_2);
	RDA1846_SetRegVal(0x32,0x627C, RDA1846_CS_2);
	RDA1846_SetRegVal(0x33,0x0AF2, RDA1846_CS_2);
	RDA1846_SetRegVal(0x47,0x2C2F, RDA1846_CS_2);
	RDA1846_SetRegVal(0x4E,0x293A, RDA1846_CS_2);
	RDA1846_SetRegVal(0x54,0x1D4C, RDA1846_CS_2);
	RDA1846_SetRegVal(0x56,0x0652, RDA1846_CS_2);
	RDA1846_SetRegVal(0x6E,0x062D, RDA1846_CS_2);
	RDA1846_SetRegVal(0x70,0x1029, RDA1846_CS_2);
	//If 0x85 is  0x001F, Rfoutput power is 8dBm , ACP is -63dB in 12.5KHz and -65dB in 25KHz
	//If 0x85 is  0x0018, Rfoutput power is 6dBm , ACP is -64dB in 12.5KHz and -66dB in 25KHz
	//If 0x85 is  0x0017, Rfoutput power is -3dBm , ACP is -68dBc in 12.5KHz and -68dBc in 25KHz
	RDA1846_SetRegVal(0x85,0x0018, RDA1846_CS_2);    	

#if 0	//高通normal  voice lpf bypass == normal
	RDA1846_SetRegVal(0x58,0x03CF, RDA1846_CS_2);
#else	//高通旁路
	RDA1846_SetRegVal(0x58,0x03FF, RDA1846_CS_2);
#endif
	RDA1846_SetRegVal(0x1F,0x1000, RDA1846_CS_2);	//set gpio sq on
	RDA1846_SetRegVal(0x0F,0x00E4, RDA1846_CS_2);   //lq 设置RF band 为134~174MHz

	RDA1846_SetRegVal(0x30,0x30A4, RDA1846_CS_2);
	Delay_ms(50);
	RDA1846_SetRegVal(0x30,0x30A6, RDA1846_CS_2);
	Delay_ms(100);
	RDA1846_SetRegVal(0x30,0x3006, RDA1846_CS_2);
	Delay_ms(100);
	
    Init_RDA1846_Rx(162025000, RDA1846_CS_2);
#if 0
    Init_RDA1846_SQ(135 - 30, 135 - 70, RDA1846_CS_2);  //lq (135+ Sq_open_threshlod),(135+ Sq_shut_threshlod)    
#endif
}

/*******************************************************************************
* Function Name  : Init_RDA1846_Rx
* Description    : 控收
* Input          : 32bit Hz
* Output         : None
* Return         : None
*******************************************************************************/
void Init_RDA1846_Rx(u32 Hz, u8 cs)
{
  	U16 RegVal;

	Cal_TxRx_Freq(Hz,Reg29_2A);

	RegVal = Rd_RDA1846reg(0x30, cs);
	RDA1846_SetRegVal(0x30, RegVal & 0xFF9F, cs);   //lq 设置频率之前关闭Tx和Rx

    RDA1846_SetRegVal(0x29, Reg29_2A[0], cs);	
	RDA1846_SetRegVal(0x2A, Reg29_2A[1], cs);	

	//25k，	AutoSQ off
	RDA1846_SetRegVal(0x30, 0x3006, cs);
	RDA1846_SetRegVal(0x30, 0x3026, cs);
#if 0
	/*lq RX code mode */
    RegVal = Rd_RDA1846reg(0x45, cs);
    RegVal |= 0x0001;
    RegVal &= 0xFFF9;
	RDA1846_SetRegVal(0x45, RegVal, cs);

	RegVal = Rd_RDA1846reg(0x4D, cs);
    RegVal |= 0x0400;
    RegVal &= 0x07FF;
	RDA1846_SetRegVal(0x4D, RegVal, cs);
#endif
    Delay_ms(10);//等待10ms
	
	//查看是否锁定
	RegVal = Rd_RDA1846reg(0x5c, cs);
	RegVal = ((RegVal>>11) & 0x01);

	if(RegVal == 0)	//重新校准PLL
	{
		//检测到失锁
		RDA1846_SetRegVal(0x27, 0x14A0, cs);//0x27H＝0x14A0
		Delay_ms(1);					    //wait 1ms(时间可调） 
		RDA1846_SetRegVal(0x27, 0x1CA0, cs);//0x27H=0x1CA0
		RDA1846_SetRegVal(0x27, 0x04A0, cs);//0x27H=0x04A0 
	}  
}

/*******************************************************************************
* Function Name  : Init_RDA1846_Tx
* Description    : 控发
* Input          : 32bit Hz
* Output         : None
* Return         : None
*******************************************************************************/
void Init_RDA1846_Tx(u32 Hz, u8 cs)
{
  	U16 RegVal;
	
	Cal_TxRx_Freq(Hz,Reg29_2A);

	RegVal = Rd_RDA1846reg(0x30, cs);
	RDA1846_SetRegVal(0x30, RegVal & 0xFF9F, cs);   //lq 设置频率之前关闭Tx和Rx

	RDA1846_SetRegVal(0x29,Reg29_2A[0], cs);	
	RDA1846_SetRegVal(0x2A,Reg29_2A[1], cs);

	//25k，	AutoSQ off
	RDA1846_SetRegVal(0x30,0x3006, cs);
	RDA1846_SetRegVal(0x30,0x3046, cs);
#if 0
	/*lq TX code mode */
    RegVal = Rd_RDA1846reg(0x45, cs);
    RegVal |= 0x0002;
    RegVal &= 0xFFFA;
	RDA1846_SetRegVal(0x45, RegVal, cs);
#endif    	
	Delay_ms(10);//等待10ms
	
	//查看是否锁定
	RegVal = Rd_RDA1846reg(0x5c, cs);
	RegVal = ((RegVal>>11) & 0x01);

	if(RegVal == 0)	//重新校准PLL
	{
		//检测到失锁
		RDA1846_SetRegVal(0x27,0x14A0, cs); //0x27H＝0x14A0
		Delay_ms(1);					    //wait 1ms(时间可调） 
		RDA1846_SetRegVal(0x27,0x1CA0, cs); //0x27H=0x1CA0
		RDA1846_SetRegVal(0x27,0x04A0, cs); //0x27H=0x04A0 
	}  
}

/*******************************************************************************
* Function Name  : Init_RDA1846_SQ
* Description    : 初始化RDA1846的静噪
* Input          : u16 SQ_Open_threshold, u16 SQ_Shut_threshold
* Output         : None
* Return         : None
*******************************************************************************/
void Init_RDA1846_SQ(u16 SQ_Open_threshold, u16 SQ_Shut_threshold, u8 cs)
{   
    SQ_Open_threshold <<= 3;
    SQ_Shut_threshold <<= 3;
    SQ_Open_threshold &= 0x03ff;
    SQ_Shut_threshold &= 0x03ff;
      
    RDA1846_SetRegVal(0x48,SQ_Open_threshold, cs);
    RDA1846_SetRegVal(0x49,SQ_Shut_threshold, cs);
}

/*******************************************************************************
* Function Name  : RDA1846_Sleep
* Description    : 休眠RDA1846
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RDA1846_Sleep(u8 cs)
{
	u16 temp;
    u16 Reg30;
    	
    Reg30 = Rd_RDA1846reg(0x30, cs);
	temp = Reg30 & 0xFF9F;	//30H[5..6] == 00
	RDA1846_SetRegVal(0x30, temp, cs);
	
    if (cs == RDA1846_CS_1)
    {
        PDN1 = 0;
        Rda1_Reg30 = Reg30;    
    }
    else
    {
        PDN2 = 0;
        Rda2_Reg30 = Reg30;                    
    }
}

/*******************************************************************************
* Function Name  : RDA1846_Wakeup
* Description    : 唤醒RDA1846
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RDA1846_Wakeup(u8 cs)
{
    u16 Reg30;

	if (cs == RDA1846_CS_1)
    {
        PDN1 = 1;                                                                    //lq 唤醒RDA1846    
        Reg30 = Rda1_Reg30;
    }
    else
    {
        PDN2 = 1;                                                                    //lq 唤醒RDA1846        
        Reg30 = Rda2_Reg30;    
    }
	Delay_ms(10);//延时20ms
	
    RDA1846_SetRegVal(0x30, Reg30, cs);
}

/*******************************************************************************
* Function Name  : RDA1846_Reset
* Description    : 复位RDA1846寄存器
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RDA1846_Reset(u8 cs)
{
	RDA1846_SetRegVal(0x30, 0x0001, cs);	//软件复位
}

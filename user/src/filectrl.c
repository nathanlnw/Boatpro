/*******************************************************************************
  Copyright (C), 2015, 712.Co.,Ltd.
  FileName: filectrl.c
  Author: 张嘉旺       Version : V1.0.0        Date: 2015.11.10
  Description:     SD卡文件管理模块
  Version:         V1.0.0
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "Include.h"
#include "stm32f10x.h"
#include "filectrl.h"
#include "def.h"
#include "parastorage.h"
#include "usart.h"
#include "flashctrl.h"
#include "sdcard.h"
#include "ff.h"
#include "diskio.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define FILE_LENGTH_MAX						0x200000//1024*256//0x200000       //单个报文记录文件的最大为2MB

/* Private macro -------------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/
FATFS fs;            					// Work area (file system object) for logical drive
FIL fp,fprm, fdst;      				// file objects
BYTE buffer[512]; 						// file copy buffer
FRESULT res;         					// FatFs function common result code
UINT br, bw;         					// File R/W count
U8 FileName[13];			 			//FileName = "TCB"+五位五位数序编号（从零开始计数）
U8 RecordName[13]="FileName.txt";		//报文文件存储记录
FileHeadStruct FileHead;				//每个报文记录的文件头

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* 名称  : CreateFile
* 描述  : 创建文件函数
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CreateFile()
{
	FRESULT res;
	char cData[50];
	U8 Temp[17];
	UINT count=0;
	UINT a;
	U8 HardwareVersion[6]; 
	
	InitFileHead();				//初始化文件的头

	/**************向记录文件写入报文记录文件名************************************/
	res=f_mount(&fs,"",0);
	if(res)
	{
		//debug_printf("Error 1 %d\r\n",(U8)res);
		return ;
	}
	res=f_open(&fp, RecordName, FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
	if(res)
	{
		//debug_printf("Error 2 %d\r\n",(U8)res);
		return ;
	}
	//while (f_gets(cTemp, 13, &fsrc))		//存储文件名时不换行
	while (f_gets(cData, 15, &fp))		//存储文件名时换行
	{
		count++;
	}
	if(count>=10000)
	{
		sprintf(FileName, "TCB");
		sprintf(Temp,"%d",count);
		strcat(FileName, Temp);
		strcat(FileName, ".txt");
	}
	else if((count>=1000)&&(count<10000))
	{ 
		sprintf(FileName, "TCB0");
		sprintf(Temp,"%d",count);
		strcat(FileName, Temp);
		strcat(FileName, ".txt");	 
	}
	else if((count>=100)&&(count<1000))
	{
		sprintf(FileName, "TCB00");
		sprintf(Temp,"%d",count);
		strcat(FileName, Temp);
		strcat(FileName, ".txt"); 	 
	}
	else if((count>=10)&&(count<100))
	{ 
		sprintf(FileName, "TCB000");
		sprintf(Temp,"%d",count);
		strcat(FileName, Temp);
		strcat(FileName, ".txt");	 
	}
	else if((count>=0)&&(count<10))
	{
		sprintf(FileName, "TCB0000");
		sprintf(Temp,"%d",count);
		strcat(FileName, Temp);
		strcat(FileName, ".txt");
	}	
	strcat(FileName,"\r\n");			  //存储文件名时换行
	//res = f_write(&fsrc, FileName,12,&a);	 //存储文件名时不换行
	res = f_write(&fp, FileName,14,&a);	 //存储文件名时换行
	if(res)
	{
		f_close(&fp);
		return ;
	}		
	f_close(&fp);

	/***************创建报文记录***************************************/
	res=f_open(&fp, FileName, FA_READ | FA_WRITE | FA_OPEN_ALWAYS);
	if(res)
 	{
		//debug_printf("Error 4 %d\r\n",(U8)res);
		return ;
	}
	/****************加入文件的头**************************************/
	f_printf(&fp, "MMSI : %D\r\n", FileHead.MMSI);
	f_printf(&fp, "ShipName : %s\r\n", FileHead.ShipName);
	f_printf(&fp, "SerialNumber : %s\r\n", FileHead.SerialNumber);
	f_printf(&fp, "SoftwareVersion : %s\r\n", FileHead.SoftwareVersion);
	//f_printf(&fp, "HardwareVersion : %s\r\n", FileHead.HardwareVersion);
	//Para_Read(PARA_HWVERSION_TYPE, HardwareVersion);
	FlashRead(STM32_FLASH_HWVERSION_TYPE, HardwareVersion);
	f_printf(&fp, "HardwareVersion : %s\r\n", HardwareVersion);
	f_printf(&fp, "CallSign : %s\r\n", FileHead.CallSign); 
	f_printf(&fp, "ShipType : %d\r\n", FileHead.ShipType);
	f_printf(&fp, "IMONumber : %d\r\n", FileHead.IMONumber);  
	f_printf(&fp, "ShipParameter ：A=%d,B=%d,C=%d,D=%d\r\n",
			FileHead.ShipA, FileHead.ShipB, FileHead.ShipC, FileHead.ShipD);
	f_close(&fp);
}

/*******************************************************************************
* 名称  : WriteFile
* 描述  : 写入SD卡文件函数
* 输入  : buff写入的数据
		  bufflen写入数据的长度	
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void WriteFile(U8 * buff,UINT bufflen)
{
	FRESULT res;
	UINT a;
	
	f_mount(&fs,"",0);
	res = f_open(&fp, FileName, FA_WRITE | FA_OPEN_ALWAYS);
	/**************************对单个文件的大小进行限制，限制为2M***********************/
	if(f_size(&fp) > FILE_LENGTH_MAX)
	{
		f_close(&fp);
		CreateFile();
		res=f_open(&fp, FileName, FA_WRITE | FA_OPEN_ALWAYS);
	}
	res = f_lseek(&fp,f_size(&fp));					   		//将指针指向文件的尾部
	res = f_write(&fp, buff,bufflen,&a);				    //向文件尾部写入数据
	f_close(&fp);	
}

/*******************************************************************************
* 名称  : ReadFile
* 描述  :读取SD卡文件函数
* 输入  : FileNameBuff指向预读文件的指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ReadFile(char * FileNameBuff)
{
	FRESULT res;
	char cData[200];
	
	f_mount(&fs,"",0);
	res=f_open(&fp, FileNameBuff, FA_READ | FA_OPEN_EXISTING);		 //以只读方式打开，如果文件名不存在那么就提示打开失败
	
	//CtrlAISAndGPS(0);												  // AIS和GPS无效
		
	//AdjustUSART1Band(115200);
	strcat(FileNameBuff,"\r\n");
	UartTxDataBuf(1, (U8*)FileNameBuff, strlen(FileNameBuff));                                                // 配置USB串口
	while (f_gets(cData, 256, &fp))		
	{
		UartTxDataBuf(1, (U8*)cData, strlen(cData));
		Delay_ms(25);														//25ms的时延
		Feed_Dog();
	}
	f_close(&fp);
	//AdjustUSART1Band(38400);                                                //lq 配置USB串口

	//CtrlAISAndGPS(1);														// AIS和GPS有效

	Feed_Dog();
}

/*******************************************************************************
* 名称  : InitFileHead
* 描述  : 初始化文件头函数
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitFileHead()
{
	U8  SerialNumber[9];			//设备序列号
	U8  HardwareVersion[6];			//硬件版本号
	U8  MMSI[10];
	UINT i;

	/******************来自AIS静态参数************************************/
	FileHead.MMSI = AisStaticPara.MMSI;
	sprintf(FileHead.ShipName, AisStaticPara.ShipName);	
	sprintf(FileHead.CallSign, AisStaticPara.CallSign);
	FileHead.ShipType = AisStaticPara.TypeOfShip;
	FileHead.IMONumber = AisStaticPara.IMONumber;
	FileHead.ShipA = AisStaticPara.ShipA; 
	FileHead.ShipB = AisStaticPara.ShipB;
	FileHead.ShipC = AisStaticPara.ShipC;
	FileHead.ShipD = AisStaticPara.ShipD;
	
	/******************来自EEROM参数************************************/
	//Para_Read(PARA_MMSI_TYPE,MMSI);
	FlashRead(STM32_FLASH_MMSI_TYPE, MMSI);
	FileHead.MMSI = atol(MMSI);
	//Para_Read(PARA_SERIAL_TYPE,SerialNumber);
	FlashRead(STM32_FLASH_SERIAL_TYPE, SerialNumber);
	sprintf(FileHead.SerialNumber, SerialNumber);
	//Para_Read(PARA_HWVERSION_TYPE, HardwareVersion);
	FlashRead(STM32_FLASH_HWVERSION_TYPE, HardwareVersion);
	sprintf(FileHead.HardwareVersion, HardwareVersion);

	/******************来自自定义参数************************************/
	sprintf(FileHead.SoftwareVersion, "%s", VERSION); 
}

/*******************************************************************************
* 名称  : AdjustUSART1Band
* 描述  : 修改串口1的波特率
* 输入  : Band波特率
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AdjustUSART1Band(U32 Band)
{
	Feed_Dog();
	InitUsart1(Band);
	Feed_Dog();
    USART1_UsartConfigStructInit(Band, 8, 0, 1);
	Feed_Dog();
}

/*******************************************************************************
* 名称  : ReadFileName
* 描述  :读取SD卡文件名函数
* 输入  : iNumber文件名个数
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ReadFileName(UINT iNumber)
{
	FRESULT res;
	char cName[20];
	
	f_mount(&fs,"",0);
	res=f_open(&fp, RecordName, FA_READ | FA_OPEN_EXISTING);		 //以只读方式打开，如果文件名不存在那么就提示打开失败
	
	//CtrlAISAndGPS(0);														 // AIS和GPS无效
	
	if(iNumber != 0)														   // 导出全部文件名
	{
		f_lseek(&fp,(f_size(&fp)-14*iNumber));							// 找到第一个指定文件名的位置
	}
	while (f_gets(cName, 15, &fp))		
	{
		UartTxDataBuf(1, (U8*)cName, strlen(cName));
		Delay_ms(25);														//25ms的时延
		Feed_Dog();
	}
	f_close(&fp);

	//CtrlAISAndGPS(1);														// AIS和GPS有效

	Feed_Dog();	
}

/*******************************************************************************
* 名称  : ReadMulFile
* 描述  :读取SD卡文件名函数
* 输入  :iNumber文件个数 
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void ReadMulFile(UINT iNumber)
{
	FRESULT res;
	char cFileName[20]={""};
	char cName[20];
	char cData[200];
	
	f_mount(&fs,"",0);
	res=f_open(&fprm, RecordName, FA_READ | FA_OPEN_EXISTING);		 //以只读方式打开，如果文件名不存在那么就提示打开失败
	
	//CtrlAISAndGPS(0);												 // AIS和GPS无效

	if(14*iNumber < f_size(&fprm))											// 当文件个数大于iNumber时执行，否则不进行文件名位置的指定
	{ 
		f_lseek(&fprm,(f_size(&fprm)-14*iNumber));							// 找到第一个指定文件名的位置
	}
	while (f_gets(cName, 15, &fprm))									// 提取文件
	{
		strncpy(cFileName,cName,12);								// 提取去掉换行字符的文件名
		UartTxDataBuf(1, (U8*)cName, strlen(cName));				// 输出该文件的文件名

		/***********************读取目标文件*********************************/
		f_open(&fp, cFileName, FA_READ | FA_OPEN_EXISTING);		//以只读方式打开，如果文件名不存在那么就提示打开失败
		while (f_gets(cData, 256, &fp))		
		{
			UartTxDataBuf(1, (U8*)cData, strlen(cData));
			Delay_ms(25);														//25ms的时延
			Feed_Dog();
		}
		f_close(&fp);
		Delay_ms(25);												//25ms的时延
		Feed_Dog();
	}
	f_close(&fprm);

    //CtrlAISAndGPS(1);														// AIS和GPS有效
	
	Feed_Dog();		
}

/*******************************************************************************
* 名称  : CtrlAISAndGPS
* 描述  :控制AIS以及GPS功能函数
* 输入  : iFlag控制标志
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CtrlAISAndGPS(UINT iFlag)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	if(iFlag == 0)							  // AIS和GPS无效
	{ 	
		/*******************防止接收AIS消息以及GPS消息的数组越界**************************************/
		EXTI_InitStructure.EXTI_Line = SI446X_A_DCLK_LINE | SI446X_B_DCLK_LINE 
									   | PULSE_EN_LINE; 
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
	    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	    EXTI_Init(&EXTI_InitStructure);
	}
	else if(iFlag == 1)						 // AIS和GPS有效
	{
		/*******************初始化接收、发射以及GPS的存储结构体，剔除无效数据**************************************/
		LME_ModuleInit(); 															//发射和接收帧队列初始化
		DLS_InstPacketProbeStructInit();
		UartTxFIFOStructInit(&Uart2TxFIFO);											//初始化GPS结构体
	    UartRxFIFOStructInit(&Uart2RxFIFO);											//初始化GPS结构体
		UBX_ModuleInit();															//初始化GPS模块参数
	
		/*******************回复接收AIS消息以及GPS消息**************************************/
		EXTI_InitStructure.EXTI_Line = SI446X_A_DCLK_LINE | SI446X_B_DCLK_LINE 
									   | PULSE_EN_LINE; 
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
	    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	    EXTI_Init(&EXTI_InitStructure);
	}
	Feed_Dog();	
}

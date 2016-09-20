/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : sdcard.c
* Author             : MCD Application Team
* Version            : V2.0.3
* Date               : 09/22/2008
* Description        : This file provides all the SD Card driver firmware
*                      functions.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "sdcard.h"
#include "stm32f10x_sdio.h"
#include  "stm32f10x_dma.h"
#include  "stm32f10x_gpio.h"
#include  "stm32f10x_rcc.h"
#include  "Include.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define NULL 0
#define SDIO_STATIC_FLAGS               ((u32)0x000005FF)
#define SDIO_CMD0TIMEOUT                ((u32)0x00002710)
#define SDIO_FIFO_Address               ((u32)0x40018080)

/* Mask for errors Card Status R1 (OCR Register) */
#define SD_OCR_ADDR_OUT_OF_RANGE        ((u32)0x80000000)
#define SD_OCR_ADDR_MISALIGNED          ((u32)0x40000000)
#define SD_OCR_BLOCK_LEN_ERR            ((u32)0x20000000)
#define SD_OCR_ERASE_SEQ_ERR            ((u32)0x10000000)
#define SD_OCR_BAD_ERASE_PARAM          ((u32)0x08000000)
#define SD_OCR_WRITE_PROT_VIOLATION     ((u32)0x04000000)
#define SD_OCR_LOCK_UNLOCK_FAILED       ((u32)0x01000000)
#define SD_OCR_COM_CRC_FAILED           ((u32)0x00800000)
#define SD_OCR_ILLEGAL_CMD              ((u32)0x00400000)
#define SD_OCR_CARD_ECC_FAILED          ((u32)0x00200000)
#define SD_OCR_CC_ERROR                 ((u32)0x00100000)
#define SD_OCR_GENERAL_UNKNOWN_ERROR    ((u32)0x00080000)
#define SD_OCR_STREAM_READ_UNDERRUN     ((u32)0x00040000)
#define SD_OCR_STREAM_WRITE_OVERRUN     ((u32)0x00020000)
#define SD_OCR_CID_CSD_OVERWRIETE       ((u32)0x00010000)
#define SD_OCR_WP_ERASE_SKIP            ((u32)0x00008000)
#define SD_OCR_CARD_ECC_DISABLED        ((u32)0x00004000)
#define SD_OCR_ERASE_RESET              ((u32)0x00002000)
#define SD_OCR_AKE_SEQ_ERROR            ((u32)0x00000008)
#define SD_OCR_ERRORBITS                ((u32)0xFDFFE008)

/* Masks for R6 Response */
#define SD_R6_GENERAL_UNKNOWN_ERROR     ((u32)0x00002000)
#define SD_R6_ILLEGAL_CMD               ((u32)0x00004000)
#define SD_R6_COM_CRC_FAILED            ((u32)0x00008000)

#define SD_VOLTAGE_WINDOW_SD            ((u32)0x80100000)
#define SD_HIGH_CAPACITY                ((u32)0x40000000)
#define SD_STD_CAPACITY                 ((u32)0x00000000)
#define SD_CHECK_PATTERN                ((u32)0x000001AA)

#define SD_MAX_VOLT_TRIAL               ((u32)0x0000FFFF)
#define SD_ALLZERO                      ((u32)0x00000000)

#define SD_WIDE_BUS_SUPPORT             ((u32)0x00040000)
#define SD_SINGLE_BUS_SUPPORT           ((u32)0x00010000)
#define SD_CARD_LOCKED                  ((u32)0x02000000)
#define SD_CARD_PROGRAMMING             ((u32)0x00000007)
#define SD_CARD_RECEIVING               ((u32)0x00000006)
#define SD_DATATIMEOUT                  ((u32)0x000FFFFF)
#define SD_0TO7BITS                     ((u32)0x000000FF)
#define SD_8TO15BITS                    ((u32)0x0000FF00)
#define SD_16TO23BITS                   ((u32)0x00FF0000)
#define SD_24TO31BITS                   ((u32)0xFF000000)
#define SD_MAX_DATA_LENGTH              ((u32)0x01FFFFFF)

#define SD_HALFFIFO                     ((u32)0x00000008)
#define SD_HALFFIFOBYTES                ((u32)0x00000020)

/* Command Class Supported */
#define SD_CCCC_LOCK_UNLOCK             ((u32)0x00000080)
#define SD_CCCC_WRITE_PROT              ((u32)0x00000040)
#define SD_CCCC_ERASE                   ((u32)0x00000020)

/* Following commands are SD Card Specific commands.
   SDIO_APP_CMD should be sent before sending these commands. */
#define SDIO_SEND_IF_COND               ((u32)0x00000008)

#define SDIO_INIT_CLK_DIV                  ((u8)0xB2)
//#define SDIO_TRANSFER_CLK_DIV              ((u8)0x1) 	  //不支持，会出现SD_CMD_CRC_FAIL
#define SDIO_TRANSFER_CLK_DIV              ((u8)0x4)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u32 CardType =  SDIO_STD_CAPACITY_SD_CARD_V1_1;
static u32 CSD_Tab[4], CID_Tab[4], RCA = 0;
static u32 DeviceMode = SD_POLLING_MODE;
static u32 TotalNumberOfBytes = 0, StopCondition = 0;
u32 *SrcBuffer, *DestBuffer;
volatile SD_Error TransferError = SD_OK;
vu32 TransferEnd = 0;
vu32 NumberOfBytes = 0;
SDIO_InitTypeDef SDIO_InitStructure;
SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
SDIO_DataInitTypeDef SDIO_DataInitStructure;

SD_CardInfo SDCardInfo;

/* Private function prototypes -----------------------------------------------*/
static SD_Error CmdError(void);
static SD_Error CmdResp1Error(u8 cmd);
static SD_Error CmdResp7Error(void);
static SD_Error CmdResp3Error(void);
static SD_Error CmdResp2Error(void);
static SD_Error CmdResp6Error(u8 cmd, u16 *prca);
static SD_Error SDEnWideBus(FunctionalState NewState);
static SD_Error IsCardProgramming(u8 *pstatus);
static SD_Error FindSCR(u16 rca, u32 *pscr);
static u8 convert_from_bytes_to_power_of_two(u16 NumberOfBytes);
static void SDIO_GPIO_Configuration(void);
static void DMA_TxConfiguration(u32 *BufferSRC, u32 BufferSize);
static void DMA_RxConfiguration(u32 *BufferDST, u32 BufferSize);

/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : SD_Init
* Description    : Initializes the SD Card and put it into StandBy State (Ready 
*                  for data transfer).
* Input          : None
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
SD_Error SD_Init(void)
{
  SD_Error errorstatus = SD_OK;								   //zjw 重置 SD_Error 状态

  /* Configure SDIO interface GPIO */ 
  SDIO_GPIO_Configuration();  								   //zjw SDIO 外设底层引脚初始化 

  /* Enable the SDIO AHB Clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SDIO, ENABLE);		   //zjw 使能 SDIO AHB 时钟

  /* Enable the DMA2 Clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);		   //zjw 使能 DMA2 时钟

  SDIO_DeInit();											   //zjw 对 SDIO 的所有寄存器进行复位

  errorstatus = SD_PowerON();								   //zjw 上电并进行卡识别流程，确认卡的操作电压

  if (errorstatus != SD_OK)									   //zjw 如果上电，识别不成功，返回“响应超时”错误
  {															   
    /* CMD Response TimeOut (wait for CMDSENT flag) */
	//debug_printf("Error %d\r\n",errorstatus);  
	//debug_printf("Error is SD_PowerON defeat!\r\n");		   
    return(errorstatus);
  }

  errorstatus = SD_InitializeCards();						   //zjw 卡识别成功，进行卡初始化

  if (errorstatus != SD_OK)									   //zjw 失败返回
  {
    /* CMD Response TimeOut (wait for CMDSENT flag) */
	//debug_printf("Error is SD_InitializeCards defeat!\r\n");
    return(errorstatus);
  }
  /*!< Configure the SDIO peripheral     
 	  上电识别，卡初始化都完成后，进入数据传输模式，提高读写速度    
	   速度若超过 24M 要进入 bypass 模式         
	!< on STM32F2xx devices, SDIOCLK is fixed to 48MHz    
	!< SDIOCLK = HCLK, SDIO_CK = HCLK/(2 + SDIO_TRANSFER_CLK_DIV)  
  		HCLK = 72 MHz, SDIOCLK = 72 MHz, SDIO_CK = HCLK/(2 + 2) = 18 MHz */  
  SDIO_InitStructure.SDIO_ClockDiv = SDIO_TRANSFER_CLK_DIV; 
  SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;			 //zjw 上升沿采集数据
  SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;		 //zjw 时钟频率若超过 24M,要开启此模式
  SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;	 //zjw 若开启此功能，在总线空闲时关闭 sd_clk 时钟
  SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;					 //zjw 1位模式
  //SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable; //zjw 硬件流，若开启，在 FIFO 不能进行发送和接收数据时，数据传输暂停
  SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Enable;
  SDIO_Init(&SDIO_InitStructure);

  //debug_printf("Error is none\r\n");

  if(errorstatus == SD_OK)      
  {      
  /*----------------- Read CSD/CID MSD registers ------------------*/     
	errorstatus = SD_GetCardInfo(&SDCardInfo);
	//debug_printf( " \r\n CardType is ：%d ", SDCardInfo.CardType );   
    //debug_printf( " \r\n CardCapacity is ：%ld ", SDCardInfo.CardCapacity ); 
    //debug_printf( " \r\n CardBlockSize is ：%d ", SDCardInfo.CardBlockSize );   
    //debug_printf( " \r\n RCA is ：%d ", SDCardInfo.RCA);   
    //debug_printf( " \r\n ManufacturerID is ：%d \r\n", SDCardInfo.SD_cid.ManufacturerID ); 
  }
  //debug_printf("Error %d\r\n",errorstatus);     
  if(errorstatus == SD_OK)      
  {      
   /*----------------- Select Card --------------------------------*/     
     errorstatus = SD_SelectDeselect((U32) (SDCardInfo.RCA << 16));   //zjw 通过 cmd7  ,rca 选择要操作的卡      
  }
  //debug_printf("Error %d\r\n",errorstatus);     
  /*if(errorstatus == SD_OK)      
  {      
 	 errorstatus = SD_EnableWideBusOperation(SDIO_BusWide_4b);   //开启4bits 模式      
  }
  debug_printf("Error %d\r\n",errorstatus);*/
  /* Set Device Transfer Mode to DMA  */ 
  if(errorstatus == SD_OK)      
  {      
 	 errorstatus = SD_SetDeviceMode(SD_DMA_MODE);       
  }
  //debug_printf("Error %d\r\n",errorstatus);    
  return(errorstatus);
}

/*******************************************************************************
* Function Name  : SD_GetStatus
* Description    : Gets the cuurent sd card data transfer status.
* Input          : None
* Output         : None
* Return         : SDTransferState: Data Transfer state.
*   			   This value can be: 
*        		   - SD_TRANSFER_OK: No data transfer is acting
*       		   - SD_TRANSFER_BUSY: Data transfer is acting
*******************************************************************************/
SDTransferState SD_GetStatus(void)
{
  SDCardState cardstate =  SD_CARD_TRANSFER;

  cardstate = SD_GetState();
  
  if (cardstate == SD_CARD_TRANSFER)
  {
    return(SD_TRANSFER_OK);
  }
  else if(cardstate == SD_CARD_ERROR)
  {
    return (SD_TRANSFER_ERROR);
  }
  else
  {
    return(SD_TRANSFER_BUSY);
  }
}

/*******************************************************************************
* Function Name  : SD_GetState
* Description    : Returns the current card's state.
* Input          : None
* Output         : None
* Return         : SDCardState: SD Card Error or SD Card Current State.
*******************************************************************************/
SDCardState SD_GetState(void)
{
  U32 resp1 = 0;
  
  if(SD_Detect()== SD_PRESENT)
  {
    if (SD_SendStatus(&resp1) != SD_OK)
    {
      return SD_CARD_ERROR;
    }
    else
    {
      return (SDCardState)((resp1 >> 9) & 0x0F);
    }
  }
  else
  {
    return SD_CARD_ERROR;
  }
}

/*******************************************************************************
* Function Name  : SD_Detect
* Description    : Returns the current card's state.
* Input          : None
* Output         : None
* Return         : SDCardState: SD Card Error or SD Card Current State.
*******************************************************************************/
U8 SD_Detect(void)
{
  U8 status = SD_PRESENT;

  /*!< Check GPIO to detect SD */
  if (GPIO_ReadInputDataBit(SD_DETECT_GPIO_PORT, SD_DETECT_PIN) != Bit_RESET)
  {
    status = SD_NOT_PRESENT;
  }
  return status;
}

/******************************************************************************
* Function Name  : SD_PowerON
* Description    : Enquires cards about their operating voltage and configures 
*                  clock controls.
* Input          : None
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
SD_Error SD_PowerON(void)
{
  SD_Error errorstatus = SD_OK;
  u32 response = 0, count = 0;
  BOOL validvoltage = FALSE;
  u32 SDType = SD_STD_CAPACITY;

  /*!< Power ON Sequence -----------------------------------------------------*/  
  /*!< Configure the SDIO peripheral */  
  /*!< SDIOCLK = HCLK, SDIO_CK = HCLK/(2 + SDIO_INIT_CLK_DIV) */  
  /*!< on STM32F2xx devices, SDIOCLK is fixed to 48MHz */  
  /*!< SDIO_CK for initialization should not exceed 400 KHz */    
  /*初始化时的时钟不能大于 400KHz*/
  SDIO_InitStructure.SDIO_ClockDiv = SDIO_INIT_CLK_DIV; /* HCLK = 72MHz, SDIOCLK = 72MHz, SDIO_CK = HCLK/(178 + 2) = 400 KHz */
  SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
  SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;//不使用 bypass 模式，直接用 HCLK 进行分频得到 SDIO_CK
  SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;// 空闲时不关闭时钟电源
  SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;				  //1 位数据线
  SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;//硬件流
  SDIO_Init(&SDIO_InitStructure);

  /* Set Power State to ON */
  SDIO_SetPowerState(SDIO_PowerState_ON);

  /* Enable SDIO Clock */
  SDIO_ClockCmd(ENABLE);

  /*下面发送一系列命令,开始卡识别流程*/ 
  /* CMD0: GO_IDLE_STATE -------------------------------------------------------*/
  /* No CMD response required */
  SDIO_CmdInitStructure.SDIO_Argument = 0x0;			       //cmd0
  SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_GO_IDLE_STATE;	   //无响应
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_No;	   
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;		   //则 CPSM 在开始发送命令之前等待数据传输结束。
  SDIO_SendCommand(&SDIO_CmdInitStructure);					   //写命令进命令寄存器

  errorstatus = CmdError();									   //检测是否正确接收到 cmd0 

  if (errorstatus != SD_OK)									   //命令发送出错，返回
  {
    /* CMD Response TimeOut (wait for CMDSENT flag) */
    return(errorstatus);
  }

  /* CMD8: SEND_IF_COND --------------------------------------------------------*/
  /* Send CMD8 to verify SD card interface operating condition */
  /* Argument: - [31:12]: Reserved (shall be set to '0')
               - [11:8]: Supply Voltage (VHS) 0x1 (Range: 2.7-3.6 V)
               - [7:0]: Check Pattern (recommended 0xAA) */
  /* CMD Response: R7 */
  SDIO_CmdInitStructure.SDIO_Argument = SD_CHECK_PATTERN;		//接收到命令 sd会返回这个参数
  SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SEND_IF_COND;		//cmd8
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;	//R7
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;				//关闭等待中断
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  /*检查是否接收到命令*/
  errorstatus = CmdResp7Error();

  if (errorstatus == SD_OK)									   //有响应则 card 遵循 sd 协议 2.0 版本
  {
    CardType = SDIO_STD_CAPACITY_SD_CARD_V2_0; 				   /* SD Card 2.0 先把它定义会 sdsc 类型的卡 */
    SDType = SD_HIGH_CAPACITY;								   //这个变量用作 acmd41 的参数，用来询问是 sdsc 卡还是 sdhc 卡   
  }
  else														   //无响应，说明是 1.x 的或 mmc 的卡
  {
    /* CMD55 */
    SDIO_CmdInitStructure.SDIO_Argument = 0x00;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_APP_CMD;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short; 
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);
    errorstatus = CmdResp1Error(SDIO_APP_CMD);				      
  }
  /* CMD55 */ //为什么在 else 里和 else 外面都要发送 CMD55?    
  //发送 cmd55，用于检测是 sd 卡还是 mmc 卡，或是不支持的卡 
  SDIO_CmdInitStructure.SDIO_Argument = 0x00;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_APP_CMD;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;   //R1
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);
  errorstatus = CmdResp1Error(SDIO_APP_CMD);				   //是否响应，没响应的是 mmc 或 不支持的卡
 

  /* If errorstatus is Command TimeOut, it is a MMC card */
  /* If errorstatus is SD_OK it is a SD card: SD card 2.0 (voltage range mismatch)
     or SD card 1.x */
  if (errorstatus == SD_OK)									  //响应了 cmd55，是 sd 卡，可能为 1.x,可能为 2.0
  {
  	/*下面开始循环地发送 sdio 支持的电压范围，循环一定次数*/
    
	/* SD CARD */
    /* Send ACMD41 SD_APP_OP_COND with Argument 0x80100000 */
    while ((!validvoltage) && (count < SD_MAX_VOLT_TRIAL))
    {
	  //因为下面要用到 ACMD41，是 ACMD 命令，在发送 ACMD 命令前都要先向卡发送 CMD55

      /* SEND CMD55 APP_CMD with RCA as 0 */
      SDIO_CmdInitStructure.SDIO_Argument = 0x00;
      SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_APP_CMD;		//CMD55
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&SDIO_CmdInitStructure);

      errorstatus = CmdResp1Error(SDIO_APP_CMD);				//检测响应 

      if (errorstatus != SD_OK)
      {
        return(errorstatus);								    //没响应 CMD55，返回 
      }
	  //acmd41，命令参数由支持的电压范围及 HCS 位组成，HCS 位置一来区分卡是 SDSc还是 sdhc 
      SDIO_CmdInitStructure.SDIO_Argument = SD_VOLTAGE_WINDOW_SD | SDType; //参数为主机可供电压范围及 hcs 位 
      SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SD_APP_OP_COND;
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;		   //R3
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&SDIO_CmdInitStructure);

      errorstatus = CmdResp3Error();									   //检测是否正确接收到数据
      if (errorstatus != SD_OK)
      {
        return(errorstatus);											   //没正确接收到 acmd41，出错，返回
      }
	  /*若卡需求电压在 SDIO 的供电电压范围内，会自动上电并标志 pwr_up 位*/
      response = SDIO_GetResponse(SDIO_RESP1);							   //读取卡寄存器，卡状态   
      validvoltage = (BOOL) (((response >> 31) == 1) ? 1 : 0);			   //读取卡的 ocr寄存器的 pwr_up 位，看是否已工作在正常电压
      count++;															   //计算循环次数
    }
    if (count >= SD_MAX_VOLT_TRIAL)										   //循环检测超过一定次数还没上电
    {
      errorstatus = SD_INVALID_VOLTRANGE;								   //SDIO 不支持 card 的供电电压   
      return(errorstatus);
    }
	/*检查卡返回信息中的 HCS 位*/
    if (response &= SD_HIGH_CAPACITY)				//判断 ocr 中的 ccs 位 ，如果是 sdsc卡则不执行下面的语句   
    {
      CardType = SDIO_HIGH_CAPACITY_SD_CARD;		//把卡类型从初始化的 sdsc 型改为 sdhc 型
    }

  }/* else MMC Card */

  return(errorstatus);
}

/*******************************************************************************
* Function Name  : SD_PowerOFF
* Description    : Turns the SDIO output signals off.
* Input          : None
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
SD_Error SD_PowerOFF(void)
{
  SD_Error errorstatus = SD_OK;

  /* Set Power State to OFF */
  SDIO_SetPowerState(SDIO_PowerState_OFF);

  return(errorstatus);
}

/*******************************************************************************
* Function Name  : SD_InitializeCards
* Description    : Intialises all cards or single card as the case may be. 
*                  Card(s) come into standby state.
* Input          : None
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
SD_Error SD_InitializeCards(void)
{
  SD_Error errorstatus = SD_OK;
  u16 rca = 0x01;

  if (SDIO_GetPowerState() == SDIO_PowerState_OFF)
  {
    errorstatus = SD_REQUEST_NOT_APPLICABLE;
    return(errorstatus);
  }

  if (SDIO_SECURE_DIGITAL_IO_CARD != CardType)				 //判断卡的类型
  {
    /* Send CMD2 ALL_SEND_CID */
    SDIO_CmdInitStructure.SDIO_Argument = 0x0;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_ALL_SEND_CID;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp2Error();

    if (SD_OK != errorstatus)
    {
      return(errorstatus);
    }

    CID_Tab[0] = SDIO_GetResponse(SDIO_RESP1);
    CID_Tab[1] = SDIO_GetResponse(SDIO_RESP2);
    CID_Tab[2] = SDIO_GetResponse(SDIO_RESP3);
    CID_Tab[3] = SDIO_GetResponse(SDIO_RESP4);
  }
  if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) ||  (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) ||  (SDIO_SECURE_DIGITAL_IO_COMBO_CARD == CardType)
      ||  (SDIO_HIGH_CAPACITY_SD_CARD == CardType))	   //使用的是 2.0 的卡   
  {
    /* Send CMD3 SET_REL_ADDR with argument 0 */
    /* SD Card publishes its RCA. */
    SDIO_CmdInitStructure.SDIO_Argument = 0x00;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SET_REL_ADDR;	  //cmd3
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;	  //R6
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp6Error(SDIO_SET_REL_ADDR, &rca);		  //把接收到的卡相对地址存起来。   

    if (SD_OK != errorstatus)
    {
      return(errorstatus);
    }
  }

  if (SDIO_SECURE_DIGITAL_IO_CARD != CardType)
  {
    RCA = rca;

    /* Send CMD9 SEND_CSD with argument as card's RCA */
    SDIO_CmdInitStructure.SDIO_Argument = (u32)(rca << 16);
    SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SEND_CSD;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp2Error();

    if (SD_OK != errorstatus)
    {
      return(errorstatus);
    }

    CSD_Tab[0] = SDIO_GetResponse(SDIO_RESP1);
    CSD_Tab[1] = SDIO_GetResponse(SDIO_RESP2);
    CSD_Tab[2] = SDIO_GetResponse(SDIO_RESP3);
    CSD_Tab[3] = SDIO_GetResponse(SDIO_RESP4);
  }

  errorstatus = SD_OK; /* All cards get intialized */

  return(errorstatus);
}

/*******************************************************************************
* Function Name  : SD_GetCardInfo
* Description    : Returns information about specific card.
* Input          : cardinfo : pointer to a SD_CardInfo structure 
*                  that contains all SD card information.
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo)
{
  SD_Error errorstatus = SD_OK;
  u8 tmp = 0;

  cardinfo->CardType = (u8)CardType;
  cardinfo->RCA = (u16)RCA;

  /* Byte 0 */
  tmp = (u8)((CSD_Tab[0] & 0xFF000000) >> 24);
  cardinfo->SD_csd.CSDStruct = (tmp & 0xC0) >> 6;
  cardinfo->SD_csd.SysSpecVersion = (tmp & 0x3C) >> 2;
  cardinfo->SD_csd.Reserved1 = tmp & 0x03;

  /* Byte 1 */
  tmp = (u8)((CSD_Tab[0] & 0x00FF0000) >> 16);
  cardinfo->SD_csd.TAAC = tmp;

  /* Byte 2 */
  tmp = (u8)((CSD_Tab[0] & 0x0000FF00) >> 8);
  cardinfo->SD_csd.NSAC = tmp;

  /* Byte 3 */
  tmp = (u8)(CSD_Tab[0] & 0x000000FF);
  cardinfo->SD_csd.MaxBusClkFrec = tmp;

  /* Byte 4 */
  tmp = (u8)((CSD_Tab[1] & 0xFF000000) >> 24);
  cardinfo->SD_csd.CardComdClasses = tmp << 4;

  /* Byte 5 */
  tmp = (u8)((CSD_Tab[1] & 0x00FF0000) >> 16);
  cardinfo->SD_csd.CardComdClasses |= (tmp & 0xF0) >> 4;
  cardinfo->SD_csd.RdBlockLen = tmp & 0x0F;

  /* Byte 6 */
  tmp = (u8)((CSD_Tab[1] & 0x0000FF00) >> 8);
  cardinfo->SD_csd.PartBlockRead = (tmp & 0x80) >> 7;
  cardinfo->SD_csd.WrBlockMisalign = (tmp & 0x40) >> 6;
  cardinfo->SD_csd.RdBlockMisalign = (tmp & 0x20) >> 5;
  cardinfo->SD_csd.DSRImpl = (tmp & 0x10) >> 4;
  cardinfo->SD_csd.Reserved2 = 0; /* Reserved */

  if ((CardType == SDIO_STD_CAPACITY_SD_CARD_V1_1) || (CardType == SDIO_STD_CAPACITY_SD_CARD_V2_0))
  {
    cardinfo->SD_csd.DeviceSize = (tmp & 0x03) << 10;

    /* Byte 7 */
    tmp = (u8)(CSD_Tab[1] & 0x000000FF);
    cardinfo->SD_csd.DeviceSize |= (tmp) << 2;

    /* Byte 8 */
    tmp = (u8)((CSD_Tab[2] & 0xFF000000) >> 24);
    cardinfo->SD_csd.DeviceSize |= (tmp & 0xC0) >> 6;

    cardinfo->SD_csd.MaxRdCurrentVDDMin = (tmp & 0x38) >> 3;
    cardinfo->SD_csd.MaxRdCurrentVDDMax = (tmp & 0x07);

    /* Byte 9 */
    tmp = (u8)((CSD_Tab[2] & 0x00FF0000) >> 16);
    cardinfo->SD_csd.MaxWrCurrentVDDMin = (tmp & 0xE0) >> 5;
    cardinfo->SD_csd.MaxWrCurrentVDDMax = (tmp & 0x1C) >> 2;
    cardinfo->SD_csd.DeviceSizeMul = (tmp & 0x03) << 1;
    /* Byte 10 */
    tmp = (u8)((CSD_Tab[2] & 0x0000FF00) >> 8);
    cardinfo->SD_csd.DeviceSizeMul |= (tmp & 0x80) >> 7;
    
    cardinfo->CardCapacity = (cardinfo->SD_csd.DeviceSize + 1) ;
    cardinfo->CardCapacity *= (1 << (cardinfo->SD_csd.DeviceSizeMul + 2));
    cardinfo->CardBlockSize = 1 << (cardinfo->SD_csd.RdBlockLen);
    cardinfo->CardCapacity *= cardinfo->CardBlockSize;
  }
  else if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
  {
    /* Byte 7 */
    tmp = (u8)(CSD_Tab[1] & 0x000000FF);
    cardinfo->SD_csd.DeviceSize = (tmp & 0x3F) << 16;

    /* Byte 8 */
    tmp = (u8)((CSD_Tab[2] & 0xFF000000) >> 24);

    cardinfo->SD_csd.DeviceSize |= (tmp << 8);

    /* Byte 9 */
    tmp = (u8)((CSD_Tab[2] & 0x00FF0000) >> 16);

    cardinfo->SD_csd.DeviceSize |= (tmp);

    /* Byte 10 */
    tmp = (u8)((CSD_Tab[2] & 0x0000FF00) >> 8);
    
    cardinfo->CardCapacity = (cardinfo->SD_csd.DeviceSize + 1) * 512 * 1024;
    cardinfo->CardBlockSize = 512;    
  }


  cardinfo->SD_csd.EraseGrSize = (tmp & 0x40) >> 6;
  cardinfo->SD_csd.EraseGrMul = (tmp & 0x3F) << 1;

  /* Byte 11 */
  tmp = (u8)(CSD_Tab[2] & 0x000000FF);
  cardinfo->SD_csd.EraseGrMul |= (tmp & 0x80) >> 7;
  cardinfo->SD_csd.WrProtectGrSize = (tmp & 0x7F);

  /* Byte 12 */
  tmp = (u8)((CSD_Tab[3] & 0xFF000000) >> 24);
  cardinfo->SD_csd.WrProtectGrEnable = (tmp & 0x80) >> 7;
  cardinfo->SD_csd.ManDeflECC = (tmp & 0x60) >> 5;
  cardinfo->SD_csd.WrSpeedFact = (tmp & 0x1C) >> 2;
  cardinfo->SD_csd.MaxWrBlockLen = (tmp & 0x03) << 2;

  /* Byte 13 */
  tmp = (u8)((CSD_Tab[3] & 0x00FF0000) >> 16);
  cardinfo->SD_csd.MaxWrBlockLen |= (tmp & 0xC0) >> 6;
  cardinfo->SD_csd.WriteBlockPaPartial = (tmp & 0x20) >> 5;
  cardinfo->SD_csd.Reserved3 = 0;
  cardinfo->SD_csd.ContentProtectAppli = (tmp & 0x01);

  /* Byte 14 */
  tmp = (u8)((CSD_Tab[3] & 0x0000FF00) >> 8);
  cardinfo->SD_csd.FileFormatGrouop = (tmp & 0x80) >> 7;
  cardinfo->SD_csd.CopyFlag = (tmp & 0x40) >> 6;
  cardinfo->SD_csd.PermWrProtect = (tmp & 0x20) >> 5;
  cardinfo->SD_csd.TempWrProtect = (tmp & 0x10) >> 4;
  cardinfo->SD_csd.FileFormat = (tmp & 0x0C) >> 2;
  cardinfo->SD_csd.ECC = (tmp & 0x03);

  /* Byte 15 */
  tmp = (u8)(CSD_Tab[3] & 0x000000FF);
  cardinfo->SD_csd.CSD_CRC = (tmp & 0xFE) >> 1;
  cardinfo->SD_csd.Reserved4 = 1;


  /* Byte 0 */
  tmp = (u8)((CID_Tab[0] & 0xFF000000) >> 24);
  cardinfo->SD_cid.ManufacturerID = tmp;

  /* Byte 1 */
  tmp = (u8)((CID_Tab[0] & 0x00FF0000) >> 16);
  cardinfo->SD_cid.OEM_AppliID = tmp << 8;

  /* Byte 2 */
  tmp = (u8)((CID_Tab[0] & 0x000000FF00) >> 8);
  cardinfo->SD_cid.OEM_AppliID |= tmp;

  /* Byte 3 */
  tmp = (u8)(CID_Tab[0] & 0x000000FF);
  cardinfo->SD_cid.ProdName1 = tmp << 24;

  /* Byte 4 */
  tmp = (u8)((CID_Tab[1] & 0xFF000000) >> 24);
  cardinfo->SD_cid.ProdName1 |= tmp << 16;

  /* Byte 5 */
  tmp = (u8)((CID_Tab[1] & 0x00FF0000) >> 16);
  cardinfo->SD_cid.ProdName1 |= tmp << 8;

  /* Byte 6 */
  tmp = (u8)((CID_Tab[1] & 0x0000FF00) >> 8);
  cardinfo->SD_cid.ProdName1 |= tmp;

  /* Byte 7 */
  tmp = (u8)(CID_Tab[1] & 0x000000FF);
  cardinfo->SD_cid.ProdName2 = tmp;

  /* Byte 8 */
  tmp = (u8)((CID_Tab[2] & 0xFF000000) >> 24);
  cardinfo->SD_cid.ProdRev = tmp;

  /* Byte 9 */
  tmp = (u8)((CID_Tab[2] & 0x00FF0000) >> 16);
  cardinfo->SD_cid.ProdSN = tmp << 24;

  /* Byte 10 */
  tmp = (u8)((CID_Tab[2] & 0x0000FF00) >> 8);
  cardinfo->SD_cid.ProdSN |= tmp << 16;

  /* Byte 11 */
  tmp = (u8)(CID_Tab[2] & 0x000000FF);
  cardinfo->SD_cid.ProdSN |= tmp << 8;

  /* Byte 12 */
  tmp = (u8)((CID_Tab[3] & 0xFF000000) >> 24);
  cardinfo->SD_cid.ProdSN |= tmp;

  /* Byte 13 */
  tmp = (u8)((CID_Tab[3] & 0x00FF0000) >> 16);
  cardinfo->SD_cid.Reserved1 |= (tmp & 0xF0) >> 4;
  cardinfo->SD_cid.ManufactDate = (tmp & 0x0F) << 8;

  /* Byte 14 */
  tmp = (u8)((CID_Tab[3] & 0x0000FF00) >> 8);
  cardinfo->SD_cid.ManufactDate |= tmp;

  /* Byte 15 */
  tmp = (u8)(CID_Tab[3] & 0x000000FF);
  cardinfo->SD_cid.CID_CRC = (tmp & 0xFE) >> 1;
  cardinfo->SD_cid.Reserved2 = 1;
  
  return(errorstatus);
}

/*******************************************************************************
* Function Name  : SD_EnableWideBusOperation
* Description    : Enables wide bus opeartion for the requeseted card if 
*                  supported by card.
* Input          : WideMode: Specifies the SD card wide bus mode. 
*                     This parameter can be one of the following values:
*                       - SDIO_BusWide_8b: 8-bit data transfer (Only for MMC)
*                       - SDIO_BusWide_4b: 4-bit data transfer
*                       - SDIO_BusWide_1b: 1-bit data transfer
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
SD_Error SD_EnableWideBusOperation(u32 WideMode)
{
  SD_Error errorstatus = SD_OK;

  /* MMC Card doesn't support this feature */
  if (SDIO_MULTIMEDIA_CARD == CardType)
  {
    errorstatus = SD_UNSUPPORTED_FEATURE;
    return(errorstatus);
  }
  else if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType))
  {
    if (SDIO_BusWide_8b == WideMode)
    {
      errorstatus = SD_UNSUPPORTED_FEATURE;
      return(errorstatus);
    }
    else if (SDIO_BusWide_4b == WideMode)
    {
      errorstatus = SDEnWideBus(ENABLE);

      if (SD_OK == errorstatus)
      {
        /* Configure the SDIO peripheral */
        SDIO_InitStructure.SDIO_ClockDiv = SDIO_TRANSFER_CLK_DIV; 
        SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
        SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
        SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
        SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_4b;
        SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
        SDIO_Init(&SDIO_InitStructure);
      }
    }
    else
    {
      errorstatus = SDEnWideBus(DISABLE);

      if (SD_OK == errorstatus)
      {
        /* Configure the SDIO peripheral */
        SDIO_InitStructure.SDIO_ClockDiv = SDIO_TRANSFER_CLK_DIV; 
        SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
        SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
        SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
        SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;
        SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
        SDIO_Init(&SDIO_InitStructure);
      }
    }
  }

  return(errorstatus);
}

/*******************************************************************************
* Function Name  : SD_SetDeviceMode
* Description    : Sets device mode whether to operate in Polling, Interrupt or
*                  DMA mode.
* Input          : Mode: Specifies the Data Transfer mode.
*                     This parameter can be one of the following values:
*                       - SD_DMA_MODE: Data transfer using DMA.
*                       - SD_INTERRUPT_MODE: Data transfer using interrupts.
*                       - SD_POLLING_MODE: Data transfer using flags.
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
SD_Error SD_SetDeviceMode(u32 Mode)
{
  SD_Error errorstatus = SD_OK;

  if ((Mode == SD_DMA_MODE) || (Mode == SD_INTERRUPT_MODE) || (Mode == SD_POLLING_MODE))
  {
    DeviceMode = Mode;
  }
  else
  {
    errorstatus = SD_INVALID_PARAMETER;
  }
  return(errorstatus);

}

/*******************************************************************************
* Function Name  : SD_SelectDeselect
* Description    : Selects od Deselects the corresponding card.
* Input          : addr: Address of the Card to be selected.
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
SD_Error SD_SelectDeselect(u32 addr)
{
//  U32 delay;
  SD_Error errorstatus = SD_OK;

  /* Send CMD7 SDIO_SEL_DESEL_CARD */
  SDIO_CmdInitStructure.SDIO_Argument =  addr;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SEL_DESEL_CARD;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);
  
  errorstatus = CmdResp1Error(SDIO_SEL_DESEL_CARD);

  return(errorstatus);
}

/*******************************************************************************
* Function Name  : SD_ReadBlock
* Description    : Allows to read one block from a specified address in a card.
* Input          : - addr: Address from where data are to be read.
*                  - readbuff: pointer to the buffer that will contain the 
*                    received data
*                  - blocksize: the SD card Data block size.
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
SD_Error SD_ReadBlock(u32 *readbuff,u32 addr, u16 BlockSize)
{
  SD_Error errorstatus = SD_OK;
  u32 count = 0, *tempbuff = readbuff;
  u8 power = 0;

  /*if (NULL == readbuff)
  {
    errorstatus = SD_INVALID_PARAMETER;
    return(errorstatus);
  }*/

  TransferError = SD_OK;
  TransferEnd = 0;
  TotalNumberOfBytes = 0;

  /* Clear all DPSM configuration */
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  SDIO_DataInitStructure.SDIO_DataLength = 0;
  SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_1b;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Disable;
  SDIO_DataConfig(&SDIO_DataInitStructure);
  SDIO_DMACmd(DISABLE);
  
  if (SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED)
  {
    errorstatus = SD_LOCK_UNLOCK_FAILED;
    return(errorstatus);
  }
  
  if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512;
    addr /= 512;
  }
  if ((BlockSize > 0) && (BlockSize <= 2048) && ((BlockSize & (BlockSize - 1)) == 0))
  {
    power = convert_from_bytes_to_power_of_two(BlockSize);
	
    /* Set Block Size for Card */
    SDIO_CmdInitStructure.SDIO_Argument = (u32) BlockSize;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SET_BLOCKLEN;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);
	
    errorstatus = CmdResp1Error(SDIO_SET_BLOCKLEN);
	
    if (SD_OK != errorstatus)
    {
      return(errorstatus);
    }
  }
  else
  {
	errorstatus = SD_INVALID_PARAMETER;
    return(errorstatus);
  }
  
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  SDIO_DataInitStructure.SDIO_DataLength = BlockSize;
  SDIO_DataInitStructure.SDIO_DataBlockSize = (u32) power << 4;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(&SDIO_DataInitStructure);
  
  TotalNumberOfBytes = BlockSize;
  StopCondition = 0;
  DestBuffer = readbuff;

  /* Send CMD17 READ_SINGLE_BLOCK */
  SDIO_CmdInitStructure.SDIO_Argument = (u32)addr;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_READ_SINGLE_BLOCK;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);
  
  errorstatus = CmdResp1Error(SDIO_READ_SINGLE_BLOCK);
  
  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }
  /* In case of single block transfer, no need of stop transfer at all.*/
  if (DeviceMode == SD_POLLING_MODE)
  {
    /* Polling mode */
    while (!(SDIO->STA &(SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR)))
    {
      if (SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)
      {
        for (count = 0; count < 8; count++)
        {
          *(tempbuff + count) = SDIO_ReadData();
        }
        tempbuff += 8;
      }
    }

    if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
    {
	  SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
      errorstatus = SD_DATA_TIMEOUT;
      return(errorstatus);
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
    {
	  SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
      errorstatus = SD_DATA_CRC_FAIL;
      return(errorstatus);
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
    {
	  SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
      errorstatus = SD_RX_OVERRUN;
      return(errorstatus);
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
    {
      SDIO_ClearFlag(SDIO_FLAG_STBITERR);
      errorstatus = SD_START_BIT_ERR;
      return(errorstatus);
    }
    while (SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)
    {
      *tempbuff = SDIO_ReadData();
      tempbuff++;
    }
    /* Clear all the static flags */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);
  }
  else if (DeviceMode == SD_INTERRUPT_MODE)
  {
	SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND | SDIO_IT_RXOVERR | SDIO_IT_RXFIFOHF | SDIO_IT_STBITERR, ENABLE);
	while ((TransferEnd == 0) && (TransferError == SD_OK))
    {}
    if (TransferError != SD_OK)
    {
      return(TransferError);
    }
  }
  else if (DeviceMode == SD_DMA_MODE)
  {
	SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND | SDIO_IT_RXOVERR | SDIO_IT_STBITERR, ENABLE);
	SDIO_DMACmd(ENABLE);
    DMA_RxConfiguration(readbuff, BlockSize);
    while ((SD_DMAEndOfTransferStatus() == RESET) && (TransferEnd == 0) && (TransferError == SD_OK))
    {}
    if (TransferError != SD_OK)
    {
      return(TransferError);
    }
  }
  return(errorstatus);
}

/*******************************************************************************
* Function Name  : SD_ReadMultiBlocks
* Description    : Allows to read blocks from a specified address  in a card.
* Input          : - addr: Address from where data are to be read.
*                  - readbuff: pointer to the buffer that will contain the 
*                    received data.
*                  - BlockSize: the SD card Data block size.
*                  - NumberOfBlocks: number of blocks to be read.
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
SD_Error SD_ReadMultiBlocks(u32 *readbuff, u32 addr, u16 BlockSize, u32 NumberOfBlocks)
{
  SD_Error errorstatus = SD_OK;
  u32 count = 0, *tempbuff = readbuff;
  u8 power = 0;

  /*if (NULL == readbuff)
  {
    errorstatus = SD_INVALID_PARAMETER;
    return(errorstatus);
  }*/

  TransferError = SD_OK;
  TransferEnd = 0;
  TotalNumberOfBytes = 0;

  /* Clear all DPSM configuration */
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  SDIO_DataInitStructure.SDIO_DataLength = 0;
  SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_1b;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Disable;
  SDIO_DataConfig(&SDIO_DataInitStructure);
  SDIO_DMACmd(DISABLE);

  if (SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED)
  {
    errorstatus = SD_LOCK_UNLOCK_FAILED;
    return(errorstatus);
  }

  if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512;
    addr /= 512;
  }
  
  if ((BlockSize > 0) && (BlockSize <= 2048) && (0 == (BlockSize & (BlockSize - 1))))
  {
    power = convert_from_bytes_to_power_of_two(BlockSize);

    /* Set Block Size for Card */
    SDIO_CmdInitStructure.SDIO_Argument = (u32) BlockSize;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SET_BLOCKLEN;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SDIO_SET_BLOCKLEN);

    if (SD_OK != errorstatus)
    {
      return(errorstatus);
    }
  }
  else
  {
    errorstatus = SD_INVALID_PARAMETER;
    return(errorstatus);
  }

  if (NumberOfBlocks > 1)
  {
    /* Common to all modes */
    if (NumberOfBlocks * BlockSize > SD_MAX_DATA_LENGTH)
    {
      errorstatus = SD_INVALID_PARAMETER;
      return(errorstatus);
    }

    TotalNumberOfBytes = NumberOfBlocks * BlockSize;
    StopCondition = 1;
    DestBuffer = readbuff;

    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DataLength = NumberOfBlocks * BlockSize;
    SDIO_DataInitStructure.SDIO_DataBlockSize = (u32) power << 4;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    /* Send CMD18 READ_MULT_BLOCK with argument data address */
    SDIO_CmdInitStructure.SDIO_Argument = (u32)addr;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_READ_MULT_BLOCK;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SDIO_READ_MULT_BLOCK);

    if (errorstatus != SD_OK)
    {
      return(errorstatus);
    }

    if (DeviceMode == SD_POLLING_MODE)
    {
      /* Polling mode */
      while (!(SDIO->STA &(SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DATAEND | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_STBITERR)))
      {
        if (SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)
        {
          for (count = 0; count < SD_HALFFIFO; count++)
          {
            *(tempbuff + count) = SDIO_ReadData();
          }
          tempbuff += SD_HALFFIFO;
        }
      }

      if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
      {
        SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
        errorstatus = SD_DATA_TIMEOUT;
        return(errorstatus);
      }
      else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
      {
        SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
        errorstatus = SD_DATA_CRC_FAIL;
        return(errorstatus);
      }
      else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
      {
        SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
        errorstatus = SD_RX_OVERRUN;
        return(errorstatus);
      }
      else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
      {
        SDIO_ClearFlag(SDIO_FLAG_STBITERR);
        errorstatus = SD_START_BIT_ERR;
        return(errorstatus);
      }
      while (SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)
      {
        *tempbuff = SDIO_ReadData();
        tempbuff++;
      }

      if (SDIO_GetFlagStatus(SDIO_FLAG_DATAEND) != RESET)
      {
        /* In Case Of SD-CARD Send Command STOP_TRANSMISSION */
        if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType))
        {
          /* Send CMD12 STOP_TRANSMISSION */
          SDIO_CmdInitStructure.SDIO_Argument = 0x0;
          SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_STOP_TRANSMISSION;
          SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
          SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
          SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
          SDIO_SendCommand(&SDIO_CmdInitStructure);

          errorstatus = CmdResp1Error(SDIO_STOP_TRANSMISSION);

          if (errorstatus != SD_OK)
          {
            return(errorstatus);
          }
        }
      }
      /* Clear all the static flags */
      SDIO_ClearFlag(SDIO_STATIC_FLAGS);
    }
    else if (DeviceMode == SD_INTERRUPT_MODE)
    {
      SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND | SDIO_IT_RXOVERR | SDIO_IT_RXFIFOHF | SDIO_IT_STBITERR, ENABLE);
      while ((TransferEnd == 0) && (TransferError == SD_OK))
      {}
      if (TransferError != SD_OK)
      {
        return(TransferError);
      }
    }
    else if (DeviceMode == SD_DMA_MODE)
    {
      SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND | SDIO_IT_RXOVERR | SDIO_IT_STBITERR, ENABLE);
      SDIO_DMACmd(ENABLE);
      DMA_RxConfiguration(readbuff, (NumberOfBlocks * BlockSize));
      while ((SD_DMAEndOfTransferStatus() == RESET) && (TransferEnd == 0) && (TransferError == SD_OK))
      {}
      if (TransferError != SD_OK)
      {
        return(TransferError);
      }
    }
  }
  return(errorstatus);
}

/*******************************************************************************
* Function Name  : SD_WaitReadOperation
* Description    : This function waits until the SDIO DMA data transfer is finished. 
*		           This function should be called after SDIO_ReadMultiBlocks() function
*   	           to insure that all data sent by the card are already transferred by 
*    		       the DMA controller.
* Input          : None
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
SD_Error SD_WaitReadOperation(void)
{
  SD_Error errorstatus = SD_OK;

  while ((SD_DMAEndOfTransferStatus() == RESET) && (TransferEnd == 0) && (TransferError == SD_OK))
  {}

  if (TransferError != SD_OK)
  {
    return(TransferError);
  }

  return(errorstatus);
}

/*******************************************************************************
* Function Name  : SD_WriteBlock
* Description    : Allows to write one block starting from a specified address 
*                  in a card.
* Input          : - addr: Address from where data are to be read.
*                  - writebuff: pointer to the buffer that contain the data to be
*                    transferred.
*                  - BlockSize: the SD card Data block size.
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
SD_Error SD_WriteBlock(u32 *writebuff, u32 addr, u16 BlockSize)
{
  SD_Error errorstatus = SD_OK;
  u8  power = 0, cardstate = 0;
  u32 timeout = 0, bytestransferred = 0;
  u32 cardstatus = 0, count = 0, restwords = 0;
  u32 *tempbuff = writebuff;

  if (writebuff == NULL)
  {
    errorstatus = SD_INVALID_PARAMETER;
    return(errorstatus);
  }

  TransferError = SD_OK;
  TransferEnd = 0;
  TotalNumberOfBytes = 0;

  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  SDIO_DataInitStructure.SDIO_DataLength = 0;
  SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_1b;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Disable;
  SDIO_DataConfig(&SDIO_DataInitStructure);
  SDIO_DMACmd(DISABLE);

  if (SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED)
  {
    errorstatus = SD_LOCK_UNLOCK_FAILED;
    return(errorstatus);
  }

  if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512;
    addr /= 512;
  }
  
  /* Set the block size, both on controller and card */
  if ((BlockSize > 0) && (BlockSize <= 2048) && ((BlockSize & (BlockSize - 1)) == 0))
  {
    power = convert_from_bytes_to_power_of_two(BlockSize);

    SDIO_CmdInitStructure.SDIO_Argument = (u32) BlockSize;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SET_BLOCKLEN;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SDIO_SET_BLOCKLEN);
	
    if (errorstatus != SD_OK)
    {
      return(errorstatus);
    }
  }
  else
  {
    errorstatus = SD_INVALID_PARAMETER;
    return(errorstatus);
  }

  /* Wait till card is ready for data Added */
  SDIO_CmdInitStructure.SDIO_Argument = (u32) (RCA << 16);
  SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SEND_STATUS;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SDIO_SEND_STATUS);
  
  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  cardstatus = SDIO_GetResponse(SDIO_RESP1);

  timeout = SD_DATATIMEOUT;

  while (((cardstatus & 0x00000100) == 0) && (timeout > 0))
  {
    timeout--;
    SDIO_CmdInitStructure.SDIO_Argument = (u32) (RCA << 16);
    SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SEND_STATUS;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SDIO_SEND_STATUS);
    if (errorstatus != SD_OK)
    {
      return(errorstatus);
    }
    cardstatus = SDIO_GetResponse(SDIO_RESP1);
  }

  if (timeout == 0)
  {
    return(SD_ERROR);
  }

  /* Send CMD24 WRITE_SINGLE_BLOCK */
  SDIO_CmdInitStructure.SDIO_Argument = addr;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_WRITE_SINGLE_BLOCK;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SDIO_WRITE_SINGLE_BLOCK);
  
  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  TotalNumberOfBytes = BlockSize;
  StopCondition = 0;
  SrcBuffer = writebuff;

  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  SDIO_DataInitStructure.SDIO_DataLength = BlockSize;
  SDIO_DataInitStructure.SDIO_DataBlockSize = (u32) power << 4;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(&SDIO_DataInitStructure);

  /* In case of single data block transfer no need of stop command at all */
  if (DeviceMode == SD_POLLING_MODE)
  {
    while (!(SDIO->STA & (SDIO_FLAG_DBCKEND | SDIO_FLAG_TXUNDERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_STBITERR)))
    {
      if (SDIO_GetFlagStatus(SDIO_FLAG_TXFIFOHE) != RESET)
      {
        if ((TotalNumberOfBytes - bytestransferred) < 32)
        {
          restwords = ((TotalNumberOfBytes - bytestransferred) % 4 == 0) ? ((TotalNumberOfBytes - bytestransferred) / 4) : (( TotalNumberOfBytes -  bytestransferred) / 4 + 1);

          for (count = 0; count < restwords; count++, tempbuff++, bytestransferred += 4)
          {
            SDIO_WriteData(*tempbuff);
          }
        }
        else
        {
          for (count = 0; count < 8; count++)
          {
            SDIO_WriteData(*(tempbuff + count));
          }
          tempbuff += 8;
          bytestransferred += 32;
        }
      }
    }
    if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
    {
      SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
      errorstatus = SD_DATA_TIMEOUT;
      return(errorstatus);
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
    {
      SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
      errorstatus = SD_DATA_CRC_FAIL;
      return(errorstatus);
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET)
    {
      SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);
      errorstatus = SD_TX_UNDERRUN;
      return(errorstatus);
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
    {
      SDIO_ClearFlag(SDIO_FLAG_STBITERR);
      errorstatus = SD_START_BIT_ERR;
      return(errorstatus);
    }
  }
  else if (DeviceMode == SD_INTERRUPT_MODE)
  {    
	SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND | SDIO_FLAG_TXFIFOHE | SDIO_IT_TXUNDERR | SDIO_IT_STBITERR, ENABLE);
    while ((TransferEnd == 0) && (TransferError == SD_OK))
    {}
    if (TransferError != SD_OK)
    {
      return(TransferError);
    }
  }
  else if (DeviceMode == SD_DMA_MODE)
  {
    SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND | SDIO_IT_TXUNDERR | SDIO_IT_STBITERR, ENABLE);
    DMA_TxConfiguration(writebuff, BlockSize);
    SDIO_DMACmd(ENABLE);
    while (DMA_GetFlagStatus(DMA2_FLAG_TC4) == RESET)	//测试发现其实这句话是可以注释掉的，但是读扇区时却不可以这样做，初步认为是由于读速度脚快引起的	  
    {}
//  while ((TransferEnd == 0) && (TransferError == SD_OK))	
 // {
//	}
    if (TransferError != SD_OK)
    {
      return(TransferError);
    }
  }

  /* Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  /* Wait till the card is in programming state */
  errorstatus = IsCardProgramming(&cardstate);

  while ((errorstatus == SD_OK) && ((cardstate == SD_CARD_PROGRAMMING) || (cardstate == SD_CARD_RECEIVING)))
  {
    errorstatus = IsCardProgramming(&cardstate);
  }

  return(errorstatus);
}

/*******************************************************************************
* Function Name  : SD_WriteMultiBlocks
* Description    : Allows to write blocks starting from a specified address in 
*                  a card.
* Input          : - addr: Address from where data are to be read.
*                  - writebuff: pointer to the buffer that contain the data to be
*                    transferred.
*                  - BlockSize: the SD card Data block size.
*                  - NumberOfBlocks: number of blocks to be written.
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
SD_Error SD_WriteMultiBlocks(u32 *writebuff, u32 addr, u16 BlockSize, u32 NumberOfBlocks)
{
  SD_Error errorstatus = SD_OK;
  u8  power = 0, cardstate = 0;
  u32 bytestransferred = 0;
  u32 count = 0, restwords = 0;
  u32 *tempbuff = writebuff;

  if (writebuff == NULL)
  {
    errorstatus = SD_INVALID_PARAMETER;
    return(errorstatus);
  }

  TransferError = SD_OK;
  TransferEnd = 0;
  TotalNumberOfBytes = 0;

  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  SDIO_DataInitStructure.SDIO_DataLength = 0;
  SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_1b;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Disable;
  SDIO_DataConfig(&SDIO_DataInitStructure);
  SDIO_DMACmd(DISABLE);

  if (SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED)
  {
    errorstatus = SD_LOCK_UNLOCK_FAILED;
    return(errorstatus);
  }

  if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
  {
    BlockSize = 512;
    addr /= 512;
  }
  
  /* Set the block size, both on controller and card */
  if ((BlockSize > 0) && (BlockSize <= 2048) && ((BlockSize & (BlockSize - 1)) == 0))
  {
    power = convert_from_bytes_to_power_of_two(BlockSize);

    SDIO_CmdInitStructure.SDIO_Argument = (u32) BlockSize;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SET_BLOCKLEN;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SDIO_SET_BLOCKLEN);

    if (errorstatus != SD_OK)
    {
      return(errorstatus);
    }
  }
  else
  {
    errorstatus = SD_INVALID_PARAMETER;
    return(errorstatus);
  }

  /* Wait till card is ready for data Added */
  SDIO_CmdInitStructure.SDIO_Argument = (u32) (RCA << 16);
  SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SEND_STATUS;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SDIO_SEND_STATUS);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  if (NumberOfBlocks > 1)
  {
    /* Common to all modes */
    if (NumberOfBlocks * BlockSize > SD_MAX_DATA_LENGTH)
    {
      errorstatus = SD_INVALID_PARAMETER;
      return(errorstatus);
    }

    if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType))
    {
      /* To improve performance */
      SDIO_CmdInitStructure.SDIO_Argument = (u32) (RCA << 16);
      SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_APP_CMD;
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&SDIO_CmdInitStructure);


      errorstatus = CmdResp1Error(SDIO_APP_CMD);

      if (errorstatus != SD_OK)
      {
        return(errorstatus);
      }
      /* To improve performance */
      SDIO_CmdInitStructure.SDIO_Argument = (u32)NumberOfBlocks;
      SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SET_BLOCK_COUNT;
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&SDIO_CmdInitStructure);

      errorstatus = CmdResp1Error(SDIO_SET_BLOCK_COUNT);

      if (errorstatus != SD_OK)
      {
        return(errorstatus);
      }
    }

    /* Send CMD25 WRITE_MULT_BLOCK with argument data address */
    SDIO_CmdInitStructure.SDIO_Argument = (u32)addr;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_WRITE_MULT_BLOCK;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SDIO_WRITE_MULT_BLOCK);

    if (SD_OK != errorstatus)
    {
      return(errorstatus);
    }

    TotalNumberOfBytes = NumberOfBlocks * BlockSize;
    StopCondition = 1;
    SrcBuffer = writebuff;

    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DataLength = NumberOfBlocks * BlockSize;
    SDIO_DataInitStructure.SDIO_DataBlockSize = (u32) power << 4;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    if (DeviceMode == SD_POLLING_MODE)
    {
      while (!(SDIO->STA & (SDIO_FLAG_TXUNDERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DATAEND | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_STBITERR)))
      {
        if (SDIO_GetFlagStatus(SDIO_FLAG_TXFIFOHE) != RESET)
        {
          if (!((TotalNumberOfBytes - bytestransferred) < SD_HALFFIFOBYTES))
          {
            for (count = 0; count < SD_HALFFIFO; count++)
            {
              SDIO_WriteData(*(tempbuff + count));
            }
            tempbuff += SD_HALFFIFO;
            bytestransferred += SD_HALFFIFOBYTES;
          }
          else
          {
            restwords = ((TotalNumberOfBytes - bytestransferred) % 4 == 0) ? ((TotalNumberOfBytes - bytestransferred) / 4) :
                        ((TotalNumberOfBytes - bytestransferred) / 4 + 1);

            for (count = 0; count < restwords; count++, tempbuff++, bytestransferred += 4)
            {
              SDIO_WriteData(*tempbuff);
            }
          }
        }
      }

      if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
      {
        SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
        errorstatus = SD_DATA_TIMEOUT;
        return(errorstatus);
      }
      else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
      {
        SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
        errorstatus = SD_DATA_CRC_FAIL;
        return(errorstatus);
      }
      else if (SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET)
      {
        SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);
        errorstatus = SD_TX_UNDERRUN;
        return(errorstatus);
      }
      else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
      {
        SDIO_ClearFlag(SDIO_FLAG_STBITERR);
        errorstatus = SD_START_BIT_ERR;
        return(errorstatus);
      }

      if (SDIO_GetFlagStatus(SDIO_FLAG_DATAEND) != RESET)
      {
       if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType))
        {
          /* Send CMD12 STOP_TRANSMISSION */
          SDIO_CmdInitStructure.SDIO_Argument = 0x0;
          SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_STOP_TRANSMISSION;
          SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
          SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
          SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
          SDIO_SendCommand(&SDIO_CmdInitStructure);


          errorstatus = CmdResp1Error(SDIO_STOP_TRANSMISSION);

          if (errorstatus != SD_OK)
          {
            return(errorstatus);
          }
        }
      }
    }
    else if (DeviceMode == SD_INTERRUPT_MODE)
    {
      SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND | SDIO_IT_TXFIFOHE | SDIO_IT_TXUNDERR | SDIO_IT_STBITERR, ENABLE);
      while ((TransferEnd == 0) && (TransferError == SD_OK))
      {}
      if (TransferError != SD_OK)
      {
        return(TransferError);
      }
    }
    else if (DeviceMode == SD_DMA_MODE)
    {
      SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND | SDIO_IT_TXUNDERR | SDIO_IT_STBITERR, ENABLE);
      SDIO_DMACmd(ENABLE);
      DMA_TxConfiguration(writebuff, (NumberOfBlocks * BlockSize));
      while (DMA_GetFlagStatus(DMA2_FLAG_TC4) == RESET)	//等待DMA2通道4传送完成，用户可以让CPU此时做别的事
      {}
      while ((TransferEnd == 0) && (TransferError == SD_OK))
      {}
      if (TransferError != SD_OK)
      {
        return(TransferError);
      }
    }
  }
  /* Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  /*!< Add some delay before checking the Card Status */
  for(count = 0; count < 0xFFFF; count++)
  {
  }	  
  /* Wait till the card is in programming state */
  errorstatus = IsCardProgramming(&cardstate);

  while ((errorstatus == SD_OK) && ((cardstate == SD_CARD_PROGRAMMING) || (cardstate == SD_CARD_RECEIVING)))
  {
    errorstatus = IsCardProgramming(&cardstate);
  }

  return(errorstatus);
}

/*******************************************************************************
* Function Name  : SD_WaitWriteOperation
* Description    : This function waits until the SDIO DMA data transfer is finished. 
*  		           This function should be called after SDIO_WriteBlock() and
*  		           SDIO_WriteMultiBlocks() function to insure that all data sent by the 
*  	               card are already transferred by the DMA controller
* Input          : None
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
SD_Error SD_WaitWriteOperation(void)
{
  SD_Error errorstatus = SD_OK;

  while ((SD_DMAEndOfTransferStatus() == RESET) && (TransferEnd == 0) && (TransferError == SD_OK))
  {}

  if (TransferError != SD_OK)
  {
    return(TransferError);
  }

  /*!< Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  return(errorstatus);
}
 
/*******************************************************************************
* Function Name  : SD_GetTransferState
* Description    : Gets the cuurent data transfer state.
* Input          : None
* Output         : None
* Return         : SDTransferState: Data Transfer state.
*                  This value can be: 
*                   - SD_NO_TRANSFER: No data transfer is acting
*                   - SD_TRANSFER_IN_PROGRESS: Data transfer is acting
*******************************************************************************/
SDTransferState SD_GetTransferState(void)
{
  if (SDIO->STA & (SDIO_FLAG_TXACT | SDIO_FLAG_RXACT))
  {
    return(SD_TRANSFER_BUSY);
  }
  else
  {
    return(SD_TRANSFER_OK);
  }
}

/*******************************************************************************
* Function Name  : SD_StopTransfer
* Description    : Aborts an ongoing data transfer.
* Input          : None
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
SD_Error SD_StopTransfer(void)
{
  SD_Error errorstatus = SD_OK;

  /* Send CMD12 STOP_TRANSMISSION  */
  SDIO_CmdInitStructure.SDIO_Argument = 0x0;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_STOP_TRANSMISSION;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SDIO_STOP_TRANSMISSION);

  return(errorstatus);
}

/*******************************************************************************
* Function Name  : SD_Erase
* Description    : Allows to erase memory area specified for the given card.
* Input          : - startaddr: the start address.
*                  - endaddr: the end address.
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
SD_Error SD_Erase(u32 startaddr, u32 endaddr)
{
  SD_Error errorstatus = SD_OK;
//  u32 delay = 0;
  vu32 maxdelay = 0;
  u8 cardstate = 0;

  /* Check if the card coomnd class supports erase command */
  if (((CSD_Tab[1] >> 20) & SD_CCCC_ERASE) == 0)
  {
    errorstatus = SD_REQUEST_NOT_APPLICABLE;
    return(errorstatus);
  }

  //maxdelay = 72000 / ((SDIO->CLKCR & 0xFF) + 2);
  maxdelay = 120000 / ((SDIO->CLKCR & 0xFF) + 2);

  if (SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED)
  {
    errorstatus = SD_LOCK_UNLOCK_FAILED;
    return(errorstatus);
  }

  if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
  {
    startaddr /= 512;
    endaddr /= 512;
  }
  
  /* According to sd-card spec 1.0 ERASE_GROUP_START (CMD32) and erase_group_end(CMD33) */
  if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType))
  {
    /* Send CMD32 SD_ERASE_GRP_START with argument as addr  */
    SDIO_CmdInitStructure.SDIO_Argument = startaddr;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SD_ERASE_GRP_START;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SDIO_SD_ERASE_GRP_START);
    if (errorstatus != SD_OK)
    {
      return(errorstatus);
    }

    /* Send CMD33 SD_ERASE_GRP_END with argument as addr  */
    SDIO_CmdInitStructure.SDIO_Argument = endaddr;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SD_ERASE_GRP_END;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SDIO_SD_ERASE_GRP_END);
    if (errorstatus != SD_OK)
    {
      return(errorstatus);
    }
  }

  /* Send CMD38 ERASE */
  SDIO_CmdInitStructure.SDIO_Argument = 0;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_ERASE;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SDIO_ERASE);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  /* Wait till the card is in programming state */
  errorstatus = IsCardProgramming(&cardstate);

  while ((errorstatus == SD_OK) && ((SD_CARD_PROGRAMMING == cardstate) || (SD_CARD_RECEIVING == cardstate)))
  {
    errorstatus = IsCardProgramming(&cardstate);
  }

  return(errorstatus);
}

/*******************************************************************************
* Function Name  : SD_SendStatus
* Description    : Returns the current card's status.
* Input          : pcardstatus: pointer to the buffer that will contain the SD 
*                  card status (Card Status register).
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
SD_Error SD_SendStatus(u32 *pcardstatus)
{
  SD_Error errorstatus = SD_OK;

  if (pcardstatus == NULL)
  {
    errorstatus = SD_INVALID_PARAMETER;
    return(errorstatus);
  }

  SDIO_CmdInitStructure.SDIO_Argument = (u32) RCA << 16;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SEND_STATUS;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);


  errorstatus = CmdResp1Error(SDIO_SEND_STATUS);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  *pcardstatus = SDIO_GetResponse(SDIO_RESP1);

  return(errorstatus);
}

/*******************************************************************************
* Function Name  : SD_SendSDStatus
* Description    : Returns the current SD card's status.
* Input          : psdstatus: pointer to the buffer that will contain the SD 
*                  card status (SD Status register).
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
SD_Error SD_SendSDStatus(u32 *psdstatus)
{
  SD_Error errorstatus = SD_OK;
  u32 count = 0;

  if (SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED)
  {
    errorstatus = SD_LOCK_UNLOCK_FAILED;
    return(errorstatus);
  }

  /* Set block size for card if it is not equal to current block size for card. */
  SDIO_CmdInitStructure.SDIO_Argument = 64;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SET_BLOCKLEN;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SDIO_SET_BLOCKLEN);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  /* CMD55 */
  SDIO_CmdInitStructure.SDIO_Argument = (u32) RCA << 16;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_APP_CMD;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);
  errorstatus = CmdResp1Error(SDIO_APP_CMD);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  SDIO_DataInitStructure.SDIO_DataLength = 64;
  SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_64b;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(&SDIO_DataInitStructure);


  /* Send ACMD13 SD_APP_STAUS  with argument as card's RCA.*/
  SDIO_CmdInitStructure.SDIO_Argument = 0;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SD_APP_STAUS;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);
  errorstatus = CmdResp1Error(SDIO_SD_APP_STAUS);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  while (!(SDIO->STA &(SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR)))
  {
    if (SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)
    {
      for (count = 0; count < 8; count++)
      {
        *(psdstatus + count) = SDIO_ReadData();
      }
      psdstatus += 8;
    }
  }

  if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
    errorstatus = SD_DATA_TIMEOUT;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
    errorstatus = SD_DATA_CRC_FAIL;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
    errorstatus = SD_RX_OVERRUN;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_STBITERR);
    errorstatus = SD_START_BIT_ERR;
    return(errorstatus);
  }

  while (SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)
  {
    *psdstatus = SDIO_ReadData();
    psdstatus++;
  }

  /* Clear all the static status flags*/
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);
  psdstatus -= 16;
  for (count = 0; count < 16; count++)
  {
    psdstatus[count] = ((psdstatus[count] & SD_0TO7BITS) << 24) |((psdstatus[count] & SD_8TO15BITS) << 8) |
                       ((psdstatus[count] & SD_16TO23BITS) >> 8) |((psdstatus[count] & SD_24TO31BITS) >> 24);
  }
  return(errorstatus);
}

/*******************************************************************************
* Function Name  : SD_ProcessIRQSrc
* Description    : Allows to process all the interrupts that are high.
* Input          : None
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
SD_Error SD_ProcessIRQSrc(void)
{
  u32 count = 0, restwords = 0;

  if (DeviceMode == SD_INTERRUPT_MODE)
  {
    if (SDIO_GetITStatus(SDIO_IT_RXFIFOHF) != RESET)
    {
      for (count = 0; count < SD_HALFFIFO; count++)
      {
        *(DestBuffer + count) = SDIO_ReadData();
      }
      DestBuffer += SD_HALFFIFO;
      NumberOfBytes += SD_HALFFIFOBYTES;
    }
    else if (SDIO_GetITStatus(SDIO_IT_TXFIFOHE) != RESET)
    {
      if ((TotalNumberOfBytes - NumberOfBytes) < SD_HALFFIFOBYTES)
      {
        restwords = ((TotalNumberOfBytes - NumberOfBytes) %  4 == 0) ?
                    ((TotalNumberOfBytes - NumberOfBytes) / 4) :
                    ((TotalNumberOfBytes - NumberOfBytes) / 4 + 1);

        for (count = 0; count < restwords;  count++, SrcBuffer++, NumberOfBytes += 4)
        {
          SDIO_WriteData(*SrcBuffer);
        }
      }
      else
      {
        for (count = 0; count < SD_HALFFIFO; count++)
        {
          SDIO_WriteData(*(SrcBuffer + count));
        }

        SrcBuffer += SD_HALFFIFO;
        NumberOfBytes += SD_HALFFIFOBYTES;
      }
    }
  }

  if (SDIO_GetITStatus(SDIO_IT_DATAEND) != RESET)
  {
    if (DeviceMode != SD_DMA_MODE)
    {
      while ((SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)  &&  (NumberOfBytes < TotalNumberOfBytes))
      {
        *DestBuffer = SDIO_ReadData();
        DestBuffer++;
        NumberOfBytes += 4;
      }
    }

    if (StopCondition == 1)
    {
      TransferError = SD_StopTransfer();
    }
    else
    {
      TransferError = SD_OK;
    }
    SDIO_ClearITPendingBit(SDIO_IT_DATAEND);
    SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND |
                  SDIO_IT_TXFIFOHE | SDIO_IT_RXFIFOHF | SDIO_IT_TXUNDERR |
                  SDIO_IT_RXOVERR | SDIO_IT_STBITERR, DISABLE);
    TransferEnd = 1;
    NumberOfBytes = 0;
    return(TransferError);
  }

  if (SDIO_GetITStatus(SDIO_IT_DCRCFAIL) != RESET)
  {
    SDIO_ClearITPendingBit(SDIO_IT_DCRCFAIL);
    SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND |
                  SDIO_IT_TXFIFOHE | SDIO_IT_RXFIFOHF | SDIO_IT_TXUNDERR |
                  SDIO_IT_RXOVERR | SDIO_IT_STBITERR, DISABLE);
    NumberOfBytes = 0;
    TransferError = SD_DATA_CRC_FAIL;
    return(SD_DATA_CRC_FAIL);
  }

  if (SDIO_GetITStatus(SDIO_IT_DTIMEOUT) != RESET)
  {
    SDIO_ClearITPendingBit(SDIO_IT_DTIMEOUT);
    SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND |
                  SDIO_IT_TXFIFOHE | SDIO_IT_RXFIFOHF | SDIO_IT_TXUNDERR |
                  SDIO_IT_RXOVERR | SDIO_IT_STBITERR, DISABLE);
    NumberOfBytes = 0;
    TransferError = SD_DATA_TIMEOUT;
    return(SD_DATA_TIMEOUT);
  }

  if (SDIO_GetITStatus(SDIO_IT_RXOVERR) != RESET)
  {
    SDIO_ClearITPendingBit(SDIO_IT_RXOVERR);
    SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND |
                  SDIO_IT_TXFIFOHE | SDIO_IT_RXFIFOHF | SDIO_IT_TXUNDERR |
                  SDIO_IT_RXOVERR | SDIO_IT_STBITERR, DISABLE);
    NumberOfBytes = 0;
    TransferError = SD_RX_OVERRUN;
    return(SD_RX_OVERRUN);
  }

  if (SDIO_GetITStatus(SDIO_IT_TXUNDERR) != RESET)
  {
    SDIO_ClearITPendingBit(SDIO_IT_TXUNDERR);
    SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND |
                  SDIO_IT_TXFIFOHE | SDIO_IT_RXFIFOHF | SDIO_IT_TXUNDERR |
                  SDIO_IT_RXOVERR | SDIO_IT_STBITERR, DISABLE);
    NumberOfBytes = 0;
    TransferError = SD_TX_UNDERRUN;
    return(SD_TX_UNDERRUN);
  }

  if (SDIO_GetITStatus(SDIO_IT_STBITERR) != RESET)
  {
    SDIO_ClearITPendingBit(SDIO_IT_STBITERR);
    SDIO_ITConfig(SDIO_IT_DCRCFAIL | SDIO_IT_DTIMEOUT | SDIO_IT_DATAEND |
                  SDIO_IT_TXFIFOHE | SDIO_IT_RXFIFOHF | SDIO_IT_TXUNDERR |
                  SDIO_IT_RXOVERR | SDIO_IT_STBITERR, DISABLE);
    NumberOfBytes = 0;
    TransferError = SD_START_BIT_ERR;
    return(SD_START_BIT_ERR);
  }

  return(SD_OK);
}

/*******************************************************************************
* Function Name  : CmdError
* Description    : Checks for error conditions for CMD0.
* Input          : None
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
static SD_Error CmdError(void)
{
  SD_Error errorstatus = SD_OK;
  u32 timeout;

  timeout = SDIO_CMD0TIMEOUT; /* 10000 */

  while ((timeout > 0) && (SDIO_GetFlagStatus(SDIO_FLAG_CMDSENT) == RESET))
  {
    timeout--;
  }

  if (timeout == 0)
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    return(errorstatus);
  }

  /* Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  return(errorstatus);
}

/*******************************************************************************
* Function Name  : CmdResp7Error
* Description    : Checks for error conditions for R7.
*                  response.
* Input          : None
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
static SD_Error CmdResp7Error(void)
{
  SD_Error errorstatus = SD_OK;
  u32 status;
  u32 timeout = SDIO_CMD0TIMEOUT;

  status = SDIO->STA;

  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)) && (timeout > 0))
  {
    timeout--;
    status = SDIO->STA;
  }

  if ((timeout == 0) || (status & SDIO_FLAG_CTIMEOUT))
  {
    /* Card is not V2.0 complient or card does not support the set voltage range */
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }

  if (status & SDIO_FLAG_CMDREND)
  {
    /* Card is SD V2.0 compliant */
    errorstatus = SD_OK;
    SDIO_ClearFlag(SDIO_FLAG_CMDREND);
    return(errorstatus);
  }
  return(errorstatus);
}

/*******************************************************************************
* Function Name  : CmdResp1Error
* Description    : Checks for error conditions for R1.
*                  response
* Input          : cmd: The sent command index.
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
static SD_Error CmdResp1Error(u8 cmd)
{
  SD_Error errorstatus = SD_OK;
  u32 status;
  u32 response_r1;

  status = SDIO->STA;

  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
    status = SDIO->STA;
  }

  if (status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }
  else if (status & SDIO_FLAG_CCRCFAIL)
  {
    errorstatus = SD_CMD_CRC_FAIL;
    SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
    return(errorstatus);
  }

  /* Check response received is of desired command */
  if (SDIO_GetCommandResponse() != cmd)
  {
    errorstatus = SD_ILLEGAL_CMD;
    return(errorstatus);
  }

  /* Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  /* We have received response, retrieve it for analysis  */
  response_r1 = SDIO_GetResponse(SDIO_RESP1);

  if ((response_r1 & SD_OCR_ERRORBITS) == SD_ALLZERO)
  {
    return(errorstatus);
  }

  if (response_r1 & SD_OCR_ADDR_OUT_OF_RANGE)
  {
    return(SD_ADDR_OUT_OF_RANGE);
  }

  if (response_r1 & SD_OCR_ADDR_MISALIGNED)
  {
    return(SD_ADDR_MISALIGNED);
  }

  if (response_r1 & SD_OCR_BLOCK_LEN_ERR)
  {
    return(SD_BLOCK_LEN_ERR);
  }

  if (response_r1 & SD_OCR_ERASE_SEQ_ERR)
  {
    return(SD_ERASE_SEQ_ERR);
  }

  if (response_r1 & SD_OCR_BAD_ERASE_PARAM)
  {
    return(SD_BAD_ERASE_PARAM);
  }

  if (response_r1 & SD_OCR_WRITE_PROT_VIOLATION)
  {
    return(SD_WRITE_PROT_VIOLATION);
  }

  if (response_r1 & SD_OCR_LOCK_UNLOCK_FAILED)
  {
    return(SD_LOCK_UNLOCK_FAILED);
  }

  if (response_r1 & SD_OCR_COM_CRC_FAILED)
  {
    return(SD_COM_CRC_FAILED);
  }

  if (response_r1 & SD_OCR_ILLEGAL_CMD)
  {
    return(SD_ILLEGAL_CMD);
  }

  if (response_r1 & SD_OCR_CARD_ECC_FAILED)
  {
    return(SD_CARD_ECC_FAILED);
  }

  if (response_r1 & SD_OCR_CC_ERROR)
  {
    return(SD_CC_ERROR);
  }

  if (response_r1 & SD_OCR_GENERAL_UNKNOWN_ERROR)
  {
    return(SD_GENERAL_UNKNOWN_ERROR);
  }

  if (response_r1 & SD_OCR_STREAM_READ_UNDERRUN)
  {
    return(SD_STREAM_READ_UNDERRUN);
  }

  if (response_r1 & SD_OCR_STREAM_WRITE_OVERRUN)
  {
    return(SD_STREAM_WRITE_OVERRUN);
  }

  if (response_r1 & SD_OCR_CID_CSD_OVERWRIETE)
  {
    return(SD_CID_CSD_OVERWRITE);
  }

  if (response_r1 & SD_OCR_WP_ERASE_SKIP)
  {
    return(SD_WP_ERASE_SKIP);
  }

  if (response_r1 & SD_OCR_CARD_ECC_DISABLED)
  {
    return(SD_CARD_ECC_DISABLED);
  }

  if (response_r1 & SD_OCR_ERASE_RESET)
  {
    return(SD_ERASE_RESET);
  }

  if (response_r1 & SD_OCR_AKE_SEQ_ERROR)
  {
    return(SD_AKE_SEQ_ERROR);
  }
  return(errorstatus);
}

/*******************************************************************************
* Function Name  : CmdResp3Error
* Description    : Checks for error conditions for R3 (OCR).
*                  response.
* Input          : None
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
static SD_Error CmdResp3Error(void)
{
  SD_Error errorstatus = SD_OK;
  u32 status;

  status = SDIO->STA;

  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
    status = SDIO->STA;
  }

  if (status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }
  /* Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);
  return(errorstatus);
}

/*******************************************************************************
* Function Name  : CmdResp2Error
* Description    : Checks for error conditions for R2 (CID or CSD).
*                  response.
* Input          : None
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
static SD_Error CmdResp2Error(void)
{
  SD_Error errorstatus = SD_OK;
  u32 status;

  status = SDIO->STA;

  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CTIMEOUT | SDIO_FLAG_CMDREND)))
  {
    status = SDIO->STA;
  }

  if (status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }
  else if (status & SDIO_FLAG_CCRCFAIL)
  {
    errorstatus = SD_CMD_CRC_FAIL;
    SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
    return(errorstatus);
  }

  /* Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  return(errorstatus);
}

/*******************************************************************************
* Function Name  : CmdResp6Error
* Description    : Checks for error conditions for R6 (RCA).
*                  response.
* Input          : - cmd: The sent command index.
*                  - prca: pointer to the variable that will contain the SD
*                    card relative address RCA. 
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
static SD_Error CmdResp6Error(u8 cmd, u16 *prca)
{
  SD_Error errorstatus = SD_OK;
  u32 status;
  u32 response_r1;

  status = SDIO->STA;

  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CTIMEOUT | SDIO_FLAG_CMDREND)))
  {
    status = SDIO->STA;
  }

  if (status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }
  else if (status & SDIO_FLAG_CCRCFAIL)
  {
    errorstatus = SD_CMD_CRC_FAIL;
    SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
    return(errorstatus);
  }

  /* Check response received is of desired command */
  if (SDIO_GetCommandResponse() != cmd)
  {
    errorstatus = SD_ILLEGAL_CMD;
    return(errorstatus);
  }

  /* Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  /* We have received response, retrieve it.  */
  response_r1 = SDIO_GetResponse(SDIO_RESP1);

  if (SD_ALLZERO == (response_r1 & (SD_R6_GENERAL_UNKNOWN_ERROR | SD_R6_ILLEGAL_CMD | SD_R6_COM_CRC_FAILED)))
  {
    *prca = (u16) (response_r1 >> 16);
    return(errorstatus);
  }

  if (response_r1 & SD_R6_GENERAL_UNKNOWN_ERROR)
  {
    return(SD_GENERAL_UNKNOWN_ERROR);
  }

  if (response_r1 & SD_R6_ILLEGAL_CMD)
  {
    return(SD_ILLEGAL_CMD);
  }

  if (response_r1 & SD_R6_COM_CRC_FAILED)
  {
    return(SD_COM_CRC_FAILED);
  }

  return(errorstatus);
}

/*******************************************************************************
* Function Name  : SDEnWideBus
* Description    : Enables or disables the SDIO wide bus mode.
* Input          : NewState: new state of the SDIO wide bus mode.
*                  This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
static SD_Error SDEnWideBus(FunctionalState NewState)
{
  SD_Error errorstatus = SD_OK;

  u32 scr[2] = {0, 0};

  if (SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED)
  {
    errorstatus = SD_LOCK_UNLOCK_FAILED;
    return(errorstatus);
  }

  /* Get SCR Register */
  errorstatus = FindSCR(RCA, scr);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  /* If wide bus operation to be enabled */
  if (NewState == ENABLE)
  {
    /* If requested card supports wide bus operation */
    if ((scr[1] & SD_WIDE_BUS_SUPPORT) != SD_ALLZERO)
    {
      /* Send CMD55 APP_CMD with argument as card's RCA.*/
      SDIO_CmdInitStructure.SDIO_Argument = (u32) RCA << 16;
      SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_APP_CMD;
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&SDIO_CmdInitStructure);

      errorstatus = CmdResp1Error(SDIO_APP_CMD);

      if (errorstatus != SD_OK)
      {
        return(errorstatus);
      }

      /* Send ACMD6 APP_CMD with argument as 2 for wide bus mode */
      SDIO_CmdInitStructure.SDIO_Argument = 0x2;
      SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_APP_SD_SET_BUSWIDTH;
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&SDIO_CmdInitStructure);

      errorstatus = CmdResp1Error(SDIO_APP_SD_SET_BUSWIDTH);

      if (errorstatus != SD_OK)
      {
        return(errorstatus);
      }
      return(errorstatus);
    }
    else
    {
      errorstatus = SD_REQUEST_NOT_APPLICABLE;
      return(errorstatus);
    }
  }   /* If wide bus operation to be disabled */
  else
  {
    /* If requested card supports 1 bit mode operation */
    if ((scr[1] & SD_SINGLE_BUS_SUPPORT) != SD_ALLZERO)
    {
      /* Send CMD55 APP_CMD with argument as card's RCA.*/
      SDIO_CmdInitStructure.SDIO_Argument = (u32) RCA << 16;
      SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_APP_CMD;
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&SDIO_CmdInitStructure);


      errorstatus = CmdResp1Error(SDIO_APP_CMD);

      if (errorstatus != SD_OK)
      {
        return(errorstatus);
      }

      /* Send ACMD6 APP_CMD with argument as 2 for wide bus mode */
      SDIO_CmdInitStructure.SDIO_Argument = 0x00;
      SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_APP_SD_SET_BUSWIDTH;
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&SDIO_CmdInitStructure);

      errorstatus = CmdResp1Error(SDIO_APP_SD_SET_BUSWIDTH);

      if (errorstatus != SD_OK)
      {
        return(errorstatus);
      }

      return(errorstatus);
    }
    else
    {
      errorstatus = SD_REQUEST_NOT_APPLICABLE;
      return(errorstatus);
    }
  }
}

/*******************************************************************************
* Function Name  : IsCardProgramming
* Description    : Checks if the SD card is in programming state.
* Input          : pstatus: pointer to the variable that will contain the SD
*                  card state.
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
static SD_Error IsCardProgramming(u8 *pstatus)
{
  SD_Error errorstatus = SD_OK;
  vu32 respR1 = 0, status = 0;

  SDIO_CmdInitStructure.SDIO_Argument = (u32) RCA << 16;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SEND_STATUS;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  status = SDIO->STA;
  while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
  {
    status = SDIO->STA;
  }

  if (status & SDIO_FLAG_CTIMEOUT)
  {
    errorstatus = SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    return(errorstatus);
  }
  else if (status & SDIO_FLAG_CCRCFAIL)
  {
    errorstatus = SD_CMD_CRC_FAIL;
    SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
    return(errorstatus);
  }

  status = (u32)SDIO_GetCommandResponse();

  /* Check response received is of desired command */
  if (status != SDIO_SEND_STATUS)
  {
    errorstatus = SD_ILLEGAL_CMD;
    return(errorstatus);
  }

  /* Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);


  /* We have received response, retrieve it for analysis  */
  respR1 = SDIO_GetResponse(SDIO_RESP1);

  /* Find out card status */
  *pstatus = (u8) ((respR1 >> 9) & 0x0000000F);

  if ((respR1 & SD_OCR_ERRORBITS) == SD_ALLZERO)
  {
    return(errorstatus);
  }

  if (respR1 & SD_OCR_ADDR_OUT_OF_RANGE)
  {
    return(SD_ADDR_OUT_OF_RANGE);
  }

  if (respR1 & SD_OCR_ADDR_MISALIGNED)
  {
    return(SD_ADDR_MISALIGNED);
  }

  if (respR1 & SD_OCR_BLOCK_LEN_ERR)
  {
    return(SD_BLOCK_LEN_ERR);
  }

  if (respR1 & SD_OCR_ERASE_SEQ_ERR)
  {
    return(SD_ERASE_SEQ_ERR);
  }

  if (respR1 & SD_OCR_BAD_ERASE_PARAM)
  {
    return(SD_BAD_ERASE_PARAM);
  }

  if (respR1 & SD_OCR_WRITE_PROT_VIOLATION)
  {
    return(SD_WRITE_PROT_VIOLATION);
  }

  if (respR1 & SD_OCR_LOCK_UNLOCK_FAILED)
  {
    return(SD_LOCK_UNLOCK_FAILED);
  }

  if (respR1 & SD_OCR_COM_CRC_FAILED)
  {
    return(SD_COM_CRC_FAILED);
  }

  if (respR1 & SD_OCR_ILLEGAL_CMD)
  {
    return(SD_ILLEGAL_CMD);
  }

  if (respR1 & SD_OCR_CARD_ECC_FAILED)
  {
    return(SD_CARD_ECC_FAILED);
  }

  if (respR1 & SD_OCR_CC_ERROR)
  {
    return(SD_CC_ERROR);
  }

  if (respR1 & SD_OCR_GENERAL_UNKNOWN_ERROR)
  {
    return(SD_GENERAL_UNKNOWN_ERROR);
  }

  if (respR1 & SD_OCR_STREAM_READ_UNDERRUN)
  {
    return(SD_STREAM_READ_UNDERRUN);
  }

  if (respR1 & SD_OCR_STREAM_WRITE_OVERRUN)
  {
    return(SD_STREAM_WRITE_OVERRUN);
  }

  if (respR1 & SD_OCR_CID_CSD_OVERWRIETE)
  {
    return(SD_CID_CSD_OVERWRITE);
  }

  if (respR1 & SD_OCR_WP_ERASE_SKIP)
  {
    return(SD_WP_ERASE_SKIP);
  }

  if (respR1 & SD_OCR_CARD_ECC_DISABLED)
  {
    return(SD_CARD_ECC_DISABLED);
  }

  if (respR1 & SD_OCR_ERASE_RESET)
  {
    return(SD_ERASE_RESET);
  }

  if (respR1 & SD_OCR_AKE_SEQ_ERROR)
  {
    return(SD_AKE_SEQ_ERROR);
  }

  return(errorstatus);
}

/*******************************************************************************
* Function Name  : FindSCR
* Description    : Find the SD card SCR register value.
* Input          : - rca: selected card address.
*                  - pscr: pointer to the buffer that will contain the SCR value.
* Output         : None
* Return         : SD_Error: SD Card Error code.
*******************************************************************************/
static SD_Error FindSCR(u16 rca, u32 *pscr)
{
  u32 index = 0;
  SD_Error errorstatus = SD_OK;
  u32 tempscr[2] = {0, 0};

  /* Set Block Size To 8 Bytes */
  /* Send CMD55 APP_CMD with argument as card's RCA */
  SDIO_CmdInitStructure.SDIO_Argument = (u32)8;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SET_BLOCKLEN;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SDIO_SET_BLOCKLEN);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  /* Send CMD55 APP_CMD with argument as card's RCA */
  SDIO_CmdInitStructure.SDIO_Argument = (u32) RCA << 16;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_APP_CMD;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SDIO_APP_CMD);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }
  SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
  SDIO_DataInitStructure.SDIO_DataLength = 8;
  SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_8b;
  SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
  SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
  SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
  SDIO_DataConfig(&SDIO_DataInitStructure);


  /* Send ACMD51 SD_APP_SEND_SCR with argument as 0 */
  SDIO_CmdInitStructure.SDIO_Argument = 0x0;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SD_APP_SEND_SCR;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);

  errorstatus = CmdResp1Error(SDIO_SD_APP_SEND_SCR);

  if (errorstatus != SD_OK)
  {
    return(errorstatus);
  }

  while (!(SDIO->STA & (SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR)))
  {
    if (SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)
    {
      *(tempscr + index) = SDIO_ReadData();
      index++;
    }
  }

  if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
    errorstatus = SD_DATA_TIMEOUT;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
    errorstatus = SD_DATA_CRC_FAIL;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
    errorstatus = SD_RX_OVERRUN;
    return(errorstatus);
  }
  else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
  {
    SDIO_ClearFlag(SDIO_FLAG_STBITERR);
    errorstatus = SD_START_BIT_ERR;
    return(errorstatus);
  }

  /* Clear all the static flags */
  SDIO_ClearFlag(SDIO_STATIC_FLAGS);

  *(pscr + 1) = ((tempscr[0] & SD_0TO7BITS) << 24) | ((tempscr[0] & SD_8TO15BITS) << 8) | ((tempscr[0] & SD_16TO23BITS) >> 8) | ((tempscr[0] & SD_24TO31BITS) >> 24);

  *(pscr) = ((tempscr[1] & SD_0TO7BITS) << 24) | ((tempscr[1] & SD_8TO15BITS) << 8) | ((tempscr[1] & SD_16TO23BITS) >> 8) | ((tempscr[1] & SD_24TO31BITS) >> 24);

  return(errorstatus);
}

/*******************************************************************************
* Function Name  : convert_from_bytes_to_power_of_two
* Description    : Converts the number of bytes in power of two and returns the
*                  power.
* Input          : NumberOfBytes: number of bytes.
* Output         : None
* Return         : None
*******************************************************************************/
static u8 convert_from_bytes_to_power_of_two(u16 NumberOfBytes)
{
  u8 count = 0;

  while (NumberOfBytes != 1)
  {
    NumberOfBytes >>= 1;
    count++;
  }
  return(count);
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the SDIO Corresponding GPIO Ports
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void SDIO_GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* GPIOC and GPIOD Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);

  /* Configure PC.08, PC.09, PC.10, PC.11, PC.12 pin: D0, D1, D2, D3, CLK pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Configure PD.02 CMD line */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOD, &GPIO_InitStructure);  
}

/*******************************************************************************
* Function Name  : DMA_TxConfiguration
* Description    : Configures the DMA2 Channel4 for SDIO Tx request.
* Input          : - BufferSRC: pointer to the source buffer
*                  - BufferSize: buffer size
* Output         : None
* Return         : None
*******************************************************************************/
static void DMA_TxConfiguration(u32 *BufferSRC, u32 BufferSize)
{
  DMA_InitTypeDef DMA_InitStructure;

  DMA_ClearFlag(DMA2_FLAG_TC4 | DMA2_FLAG_TE4 | DMA2_FLAG_HT4 | DMA2_FLAG_GL4);

  /* DMA2 Channel4 disable */
  DMA_Cmd(DMA2_Channel4, DISABLE);

  /* DMA2 Channel4 Config */
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)SDIO_FIFO_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)BufferSRC;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = BufferSize / 4;//数据宽度为32位，故除以四
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA2_Channel4, &DMA_InitStructure);

  /* DMA2 Channel4 enable */
  DMA_Cmd(DMA2_Channel4, ENABLE);
}

/*******************************************************************************
* Function Name  : DMA_RxConfiguration
* Description    : Configures the DMA2 Channel4 for SDIO Rx request.
* Input          : - BufferDST: pointer to the destination buffer
*                  - BufferSize: buffer size
* Output         : None
* Return         : None
*******************************************************************************/
static void DMA_RxConfiguration(u32 *BufferDST, u32 BufferSize)
{
  DMA_InitTypeDef DMA_InitStructure;

  DMA_ClearFlag(DMA2_FLAG_TC4 | DMA2_FLAG_TE4 | DMA2_FLAG_HT4 | DMA2_FLAG_GL4);

  /* DMA2 Channel4 disable */
  DMA_Cmd(DMA2_Channel4, DISABLE);

  /* DMA2 Channel4 Config */
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)SDIO_FIFO_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)BufferDST;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = BufferSize / 4; 					 	//数据宽度为32位，故除以四
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;	//外设数据宽度为32位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA2_Channel4, &DMA_InitStructure);

  /* DMA2 Channel4 enable */
  DMA_Cmd(DMA2_Channel4, ENABLE);
}

/*******************************************************************************
* Function Name  : SD_DMAEndOfTransferStatus
* Description    : Returns the DMA End Of Transfer Status.
* Input          : None
* Output         : None
* Return         : DMA_GetFlagStatus(DMA2_FLAG_TC4);
*******************************************************************************/
/**
  * @brief  Returns the DMA End Of Transfer Status.
  * @param  None
  * @retval DMA SDIO Channel Status.
  */
U32 SD_DMAEndOfTransferStatus(void)
{
  return (U32)DMA_GetFlagStatus(DMA2_FLAG_TC4);
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

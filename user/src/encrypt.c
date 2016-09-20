/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "Include.h"
#include "stm32f10x_flash.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/ 

U32 Decode(U32 ulSrc, U32 *ulKeyBuff, U8 KeySize)
{ 
	U8  ucTemp;
	U32 ulObj;
	
	for(ucTemp = 0;ucTemp < KeySize;ucTemp++)
	{
		ulSrc ^= ulKeyBuff[ucTemp];	
	}
	
	ulObj = ulSrc - SI446X_MODEM_DATA_RATE;	
	
	return ulObj;
}

void ReadDeviceID(U32 *ucIDBuff)
{
	U8  ucBuff[10];
	U32 ulBuff[4];
	U8  ucTemp1;
	U8  ucTemp2;
	u32 ulTemp;

	*ucIDBuff++ = *(U32 *)(0x1FFFF7E8);		 
	*ucIDBuff++ = *(U32 *)(0x1FFFF7EC);
	*ucIDBuff++ = *(U32 *)(0x1FFFF7F0);		 

	SI446X_PART_INFO(ucBuff, SI446X_CHIP_A);


	for(ucTemp1 = 0;ucTemp1 < 2;ucTemp1++)
	{
		ulTemp = 0;
		for(ucTemp2 = 1;ucTemp2 <5;ucTemp2++)
		{
			ulTemp += (ucBuff[(ucTemp1 * 4 + ucTemp2)] << (ucTemp2 * 8));
		}
		*ucIDBuff++ = ulTemp;
	}

	SI446X_PART_INFO(ucBuff, SI446X_CHIP_B);

	for(ucTemp1 = 0;ucTemp1 < 2;ucTemp1++)
	{
		ulTemp = 0;
		for(ucTemp2 = 1;ucTemp2 < 5;ucTemp2++)
		{
			ulTemp |= (ucBuff[(ucTemp1 * 4 + ucTemp2)] << ((ucTemp2-1) * 8));
		}
		*ucIDBuff++ = ulTemp;
	}

	SI446X_FUNC_INFO(ucBuff, SI446X_CHIP_A);

	ulTemp = 0;
	ulTemp = ucBuff[1];
	ulTemp += ucBuff[2] << 8;
	ulTemp += ucBuff[3] << 16;
	ulTemp += ucBuff[4] << 24;
	*ucIDBuff++ = ulTemp;
	ulTemp = ucBuff[5];
	ulTemp += ucBuff[6] << 8;

	SI446X_FUNC_INFO(ucBuff, SI446X_CHIP_B);

	ulTemp += ucBuff[1] << 16;
	ulTemp += ucBuff[2] << 24;
	*ucIDBuff++ = ulTemp;
	ulTemp = ucBuff[3];
	ulTemp += ucBuff[4] << 8;
	ulTemp += ucBuff[5] << 16;
	ulTemp += ucBuff[6] << 24;
	*ucIDBuff++ = ulTemp;

}

U32 ReadCry(CryTypeEnum cryType)
{
	U8  ucTempBuff[7];
	U8  ucCRCBuff[3];
	U8  ucTemp;
	U16 usCRC;
	U16 usCRCTemp;
	U32 ulSrc;
	U32 ulAddr;	

	if(cryType == CRY_MAIN)
	{
		ulAddr = STM32_FLASH_CRY_0_7_ADDR;
	}
	else if(cryType == CRY_BACKUP1)
	{
		ulAddr = STM32_FLASH_CRY_0_7_BACKUP1_ADDR;
	}
	else if(cryType == CRY_BACKUP2)
	{
		ulAddr = STM32_FLASH_CRY_0_7_BACKUP2_ADDR;
	}

	FlashReadCry(ucTempBuff,ulAddr);

	usCRC = (U16)((ucTempBuff[4]<< 8) + ucTempBuff[5]);
	usCRCTemp = AIS_Cal_CRC16(ucTempBuff, 4);

	ulSrc = 0;
	if(usCRC == usCRCTemp)
	{  		
		for(ucTemp = 0;ucTemp < 4;ucTemp++)
		{
			ulSrc <<= 8;
			ulSrc |= (ucTempBuff[(3-ucTemp)] & 0xFF);
		}
	}
	return ulSrc;
}

U8 WriteCry(CryTypeEnum cryType, U8 *ucCryBuff)
{	
	U8 ucTempBuff[7];
	U8 ucTemp;
	U16 usCRC;		  
	U32 ulAddr;	
	U32 ulTemp;
	char *pEnd;
    

	if(cryType == CRY_MAIN)
	{
		ulAddr = STM32_FLASH_CRY_0_7_ADDR;
	}
	else if(cryType == CRY_BACKUP1)
	{
		ulAddr = STM32_FLASH_CRY_0_7_BACKUP1_ADDR;
	}
	else if(cryType == CRY_BACKUP2)
	{
		ulAddr = STM32_FLASH_CRY_0_7_BACKUP2_ADDR;
	}

	ulTemp = strtoul((const char*)ucCryBuff, &pEnd, 10);	
	for(ucTemp = 0;ucTemp < 4;ucTemp++)
	{ 
		ucTempBuff[ucTemp] = (U8)(ulTemp & 0xFF);
		ulTemp >>= 8; 
	}

	usCRC = AIS_Cal_CRC16(ucTempBuff, 4);
	ucTempBuff[4] = ((usCRC >> 8) & 0x00FF);
	ucTempBuff[5] = ((usCRC) & 0x00FF);

	FlashWriteCry(ucTempBuff,ulAddr);

}

void FlashReadCry(U8 *ucCryBuff, U32 ulAddr)
{
	U8 ucTemp;
	 
	for(ucTemp = 0;ucTemp < 6;ucTemp++)
	{
	   ucCryBuff[ucTemp] = (U8)FLASH_ReadHalfWord((ulAddr + ucTemp * 6));
	} 	
}

void FlashWriteCry(U8 *ucCryBuff, U32 ulAddr)
{	
	int i;
	U8 ucFlashDataLen;
	U8 ucStar;
	U8 ucDebug;
	U16 usFlashData;
	U16 usFlashDataBuf[STM32_FLASH_PAGE_DATA_MAXLEN];
	U32 ulFlashPageAddr;
	U32 ulFlashPageDataAddr;

	FLASH_Unlock();

	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);

	ulFlashPageAddr = ((ulAddr-STM32_FLASH_BASE)/STM32_FLASH_EVERY_PAGE_SIZE)*STM32_FLASH_EVERY_PAGE_SIZE;
	ulFlashPageDataAddr = ulFlashPageAddr;
	i=0;
	do
	{
		usFlashDataBuf[i] = FLASH_ReadHalfWord(ulFlashPageDataAddr);
		i++;
		ulFlashPageDataAddr +=2;
	}while(i < STM32_FLASH_PAGE_DATA_MAXLEN+1);
	ucFlashDataLen = STM32_FLASH_PAGE_DATA_MAXLEN;
	FLASH_ErasePage(ulFlashPageAddr);

	ucStar = (ulAddr - ulFlashPageAddr)/2;
	for(i = 0;i < 6;i++)						 
	{
		usFlashDataBuf[(i * 3)+ucStar] = ucCryBuff[i]; 		
	}
	if(((i * 3)+ucStar)>ucFlashDataLen)
	{
		ucFlashDataLen = ((i * 3)+ucStar); 
	}
	for(i=0;i<ucFlashDataLen;i++)
	{
		ucDebug = FLASH_ProgramHalfWord(ulFlashPageAddr,usFlashDataBuf[i]);
		usFlashData = FLASH_ReadHalfWord(ulFlashPageAddr);
		if(usFlashData != usFlashDataBuf[i] || ucDebug != FLASH_COMPLETE)
		{
			break;
		}
		ulFlashPageAddr += 2;
		Feed_Dog();
	}
	FLASH_Lock();	

}

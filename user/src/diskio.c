/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: Header file of existing USB MSD control module */
//#include "atadrive.h"	/* Example: Header file of existing ATA harddisk control module */
#include "sdcard.h"		/* Example: Header file of existing MMC/SDC contorl module */
#include "Include.h"

/* Definitions of physical drive number for each drive */
#define ATA		0	/* Example: Map ATA harddisk to physical drive 0 */
#define MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define USB		2	/* Example: Map USB MSD to physical drive 2 */
#define SECTOR_SIZE 		512U /* Block Size in Bytes */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	if(pdrv)
	{
		//debug_printf("Only support drv 0\n");
		return STA_NOINIT;
	}	

	return 0;
//	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{	
	if (SD_Init() == SD_OK)
	{
		//debug_printf("Init success\r\n");
	  	return STA_OK;           //初始化成功
	}
	
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	SD_Error res = SD_OK;

	if (pdrv || !count)
    {    
        return RES_PARERR;  //仅支持单磁盘操作，count不能等于0，否则返回参数错误
    }
		
	if(count==1)
    {
		res = SD_ReadBlock((u32 *)buff,sector << 9  ,SECTOR_SIZE);        
	}
	else
    {
		res =  SD_ReadMultiBlocks((u32 *)buff,sector << 9 ,SECTOR_SIZE,count);        
	} 
	
	if(res == SD_OK)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    } 

	//return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	SD_Error res = SD_OK;

    if (pdrv || !count)
    {    
        return RES_PARERR;  //仅支持单磁盘操作，count不能等于0，否则返回参数错误
    }

	//debug_printf("begin write\n");

	if(count==1)
    {          
    	res =SD_WriteBlock((u32 *)buff,sector << 9 ,SECTOR_SIZE);
	}
	else
    {          
    	res = SD_WriteMultiBlocks((u32 *)buff,sector <<9 ,SECTOR_SIZE,count);
	}
        
  	if(res == SD_OK)
    {
		//debug_printf("write_ok\n");
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }

	//return RES_PARERR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	return RES_OK;
}


DWORD get_fattime (void)
{
//	struct tm t;
    DWORD fattime = 0;
//    t = Time_GetCalendarTime();
//    GpsPara.GPS_TimeY,
//    GpsPara.GPS_TimeMon,
//    GpsPara.GPS_TimeD,
//    GpsPara.GPS_TimeH,
//    GpsPara.GPS_TimeM,
//    GpsPara.GPS_TimeS

//    t.tm_year -= 1980;		//年份改为1980年起
//    t.tm_mon++;         	//0-11月改为1-12月
//    t.tm_sec /= 2;      	//将秒数改为0-29
    
	if(IsGpsFix())													//如果GPS已经定位，由于GpsPara.GPS_TimeY只显示20XX的后两位“XX”，故而应加上20
	{
	    fattime = ((GpsPara.GPS_TimeY+20) << 25)|((GpsPara.GPS_TimeMon) << 21)|((GpsPara.GPS_TimeD) << 16)|
	              ((GpsPara.GPS_TimeH << 11))|((GpsPara.GPS_TimeM) << 5)|((GpsPara.GPS_TimeS));
	}
	else															//如果GPS未定位
	{
		fattime = (20 << 25)|(1 << 21)|(1 << 16)|
	              (( 0 << 11))|(0 << 5)|((0));
	}

    return fattime;
}

/*DWORD get_fattime(void)
{
	return 0;
} 
*/
#endif


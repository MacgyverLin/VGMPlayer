/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "mmc_sd.h"
#include "malloc.h"
//#include "usbdisk.h"	/* Example: USB drive control */
//#include "atadrive.h"	/* Example: ATA drive control */
//#include "sdcard.h"		/* Example: MMC/SDC contorl */

/* Definitions of physical drive number for each media */
#define SD_Card		0
#define SFI_Flash		1
#define USB		2


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv) {
	case SD_Card :
		stat = SD_Initialize();//添加SD卡的初始化

		// translate the reslut code here

		return stat;

	case SFI_Flash :
		//result = MMC_disk_initialize();

		// translate the reslut code here

		return stat;

	case USB :
		//result = USB_disk_initialize();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv) {
	case SD_Card :
		stat = 0;//ATA_disk_status();

		// translate the reslut code here

		return stat;

	case SFI_Flash :
		//result = MMC_disk_status();

		// translate the reslut code here

		return stat;

	case USB :
		//result = USB_disk_status();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case SD_Card :
		// translate the arguments here

		res = SD_ReadDisk(buff, sector, count);//添加SD的读扇区功能

		// translate the reslut code here

		return res;

	case SFI_Flash :
		// translate the arguments here

		//result = MMC_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case USB :
		// translate the arguments here

		//result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;
	}
	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case SD_Card :
		// translate the arguments here

		res = SD_WriteDisk((u8*)buff, sector, count);//添加SD卡的写扇区功能
		
		// translate the reslut code here

		break;

	case SFI_Flash :
		// translate the arguments here

		//result = MMC_disk_write(buff, sector, count);

		// translate the reslut code here

		break;

	case USB :
		// translate the arguments here

		//result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		break;
	}
	if(res==0x00)
	{
		return RES_OK;
	}
	else return RES_ERROR;
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
	DRESULT res;
	//int result;
	if(pdrv==SD_Card)
	{
		switch(cmd)
		{
			case CTRL_SYNC:
				res=RES_OK;
				break;
			case GET_SECTOR_SIZE:
				*(DWORD*)buff=512;
				res=RES_OK;
				break;
			case GET_BLOCK_SIZE:
				*(DWORD*)buff=8;
				res=RES_OK;
				break;
			case GET_SECTOR_COUNT:
				*(DWORD*)buff = SD_GetSectorCount();
				res=RES_OK;
				break;
		  default:
		    res = RES_PARERR;
        break;
				
		}
	}
//	switch (pdrv) {
//	case SD_Card :
//		// pre-process here

//		//result = ATA_disk_ioctl(cmd, buff);

//		// post-process here

//		return res;

//	case SFI_Flash :
//		// pre-process here

//		//result = MMC_disk_ioctl(cmd, buff);

//		// post-process here

//		return res;

//	case USB :
//		// pre-process here

//		//result = USB_disk_ioctl(cmd, buff);

//		// post-process here

//		return res;
//	}
//	return RES_PARERR;
}






DWORD get_fattime (void)
{				 
	return 0;
}			 
//动态分配内存
void *ff_memalloc (UINT size)			
{
	return (void*)mymalloc(size);
}
//释放内存
void ff_memfree (void* mf)		 
{
	myfree(mf);
}

#endif

/*
*********************************************************************************************************
*
*	模块名称 : fatfs和sd卡的接口文件
*	文件名称 : diskio.c
*	版    本 : V1.0
*	说    明 : 支持SD卡
*
*	修改记录 :
*		版本号   日期         作者           说明
*       v1.0    2014-06-19   Eric2013        首发
*
*	Copyright (C), 2014-2015, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "diskio.h"		    /* FatFs lower layer API */
#include "bsp.h"

#define ATA			1     //SD卡的磁盘逻辑卷
#define SPI_FLASH		0 //外部Flash的磁盘逻辑卷

//对于W25Q128
//前14M字节给fatfs用,14M字节后,给客户自己用	 	
#define FLASH_SECTOR_SIZE 	512			  		    
u16	    FLASH_SECTOR_COUNT = 2048*14;	//W25Q128,14M字节全给FATFS占用
#define FLASH_BLOCK_SIZE   	8     	//每个BLOCK有8个扇区
/*-------------------------------------------------------------------------------------------*/
/* Inidialize a Drive                                                                        */
/*-------------------------------------------------------------------------------------------*/
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	DSTATUS status = STA_NOINIT;
	switch (pdrv) {
		case ATA:	/* SD CARD */
			#if FATFS_USE_SDIO == 1
				  status = SD_Init();
					if (status!=SD_OK )
					{
						status = STA_NOINIT;
					}
					else
					{
						status = RES_OK;
					}
			#endif
			break;
		
		case SPI_FLASH:
						bsp_FlashInit();
						status = RES_OK;
			break;

		default:
			status = STA_NOINIT;
	}
		return status;

}

/*-------------------------------------------------------------------------------------------*/
/* Get Disk Status                                                                           */
/*-------------------------------------------------------------------------------------------*/
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
	return RES_OK;
}

/*-------------------------------------------------------------------------------------------*/
/* Read Sector(s)                                                                            */
/*-------------------------------------------------------------------------------------------*/
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	DRESULT status = RES_PARERR;

	switch (pdrv)
	{
		case ATA:	/* SD CARD */
			#if FATFS_USE_SDIO == 1
				if (count > 1)
				{
					SD_ReadMultiBlocks(buff, sector*SD_BLOCK_SIZE, SD_BLOCK_SIZE, count);				
					/* Check if the Transfer is finished */
					SD_WaitReadOperation();  //循环查询dma传输是否结束				
					/* Wait until end of DMA transfer */
					while(SD_GetStatus() != SD_TRANSFER_OK);
				}
				else
				{					
					SD_ReadBlock(buff, sector*SD_BLOCK_SIZE, SD_BLOCK_SIZE);
					/* Check if the Transfer is finished */
					SD_WaitReadOperation();  //循环查询dma传输是否结束				
					/* Wait until end of DMA transfer */
					while(SD_GetStatus() != SD_TRANSFER_OK);
				}
				status = RES_OK;
			#endif
			break;

		case SPI_FLASH:
					for(;count>0;count--)
					{
						W25QXX_Read(buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
						sector++;
						buff+=FLASH_SECTOR_SIZE;
					}
					status = RES_OK;
		break;
		default:
			status = RES_PARERR;
	}
	return status;
}

/*-------------------------------------------------------------------------------------------*/
/* Write Sector(s)                                                                           */
/*-------------------------------------------------------------------------------------------*/
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
 DRESULT res;

	switch (pdrv)
	{
		case ATA:	/* SD CARD */
			#if FATFS_USE_SDIO == 1
					if (count > 1)
					{
						SD_WriteMultiBlocks((uint8_t *)buff, sector*SD_BLOCK_SIZE,SD_BLOCK_SIZE, count);						
						/* Check if the Transfer is finished */
						SD_WaitWriteOperation();	   //等待dma传输结束
						while(SD_GetStatus() != SD_TRANSFER_OK); //等待sdio到sd卡传输结束
					}
					else
					{
						SD_WriteBlock((uint8_t *)buff,sector*SD_BLOCK_SIZE,SD_BLOCK_SIZE);						
						/* Check if the Transfer is finished */
						SD_WaitWriteOperation();	   //等待dma传输结束
						while(SD_GetStatus() != SD_TRANSFER_OK); //等待sdio到sd卡传输结束
					}
					res = RES_OK;
			#endif
			break;

		case SPI_FLASH:
					for(;count>0;count--)
					{										    
						W25QXX_Write((u8*)buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
						sector++;
						buff+=FLASH_SECTOR_SIZE;
					}

					res = RES_OK;
		break;
		default:
			res = RES_PARERR;
	}
	return res;
}
#endif


/*-------------------------------------------------------------------------------------------*/
/* Miscellaneous Functions                                                                   */
/*-------------------------------------------------------------------------------------------*/
#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;

	switch (pdrv) {
			case ATA:	/* SD CARD */
			#if FATFS_USE_SDIO == 1
				switch (cmd) 
				{				
					case GET_SECTOR_SIZE :     // Get R/W sector size (WORD) 
						*(WORD * )buff = 512;
					break;
					case GET_BLOCK_SIZE:      // Get erase block size in unit of sector (DWORD)
						*(DWORD * )buff = SDCardInfo.CardBlockSize;
					break;
					case GET_SECTOR_COUNT:
						*(DWORD * )buff = SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize;
					case CTRL_SYNC :
					break;
				}
				res = RES_OK;
			#endif
			break;

		case SPI_FLASH:
				switch (cmd) 
				{
					case GET_SECTOR_SIZE :     // Get R/W sector size (WORD)
						*(WORD * )buff = FLASH_SECTOR_SIZE;		//flash最小写单元为页，256字节，此处取2页为一个读写单位
					break;
					case GET_BLOCK_SIZE :      // Get erase block size in unit of sector (DWORD)
						*(DWORD * )buff = FLASH_BLOCK_SIZE;		//flash以4k为最小擦除单位,512bytes为一个扇区，则一个块里有8个扇区
					break;
					case GET_SECTOR_COUNT:
						*(DWORD * )buff = FLASH_SECTOR_COUNT;		//用于fatfs的sector数量
					break;
					case CTRL_SYNC :
					break;
					default:break;
				}
				res = RES_OK;
		break;
		default:
			res = RES_PARERR;
	}

	return res;
}
#endif

/*
*********************************************************************************************************
*	函 数 名: get_fattime
*	功能说明: 获得系统时间，用于改写文件的创建和修改时间。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
DWORD get_fattime (void)
{
	/* 如果有全局时钟，可按下面的格式进行时钟转换. 这个例子是2013-01-01 00:00:00 */

	return	  ((DWORD)(2013 - 1980) << 25)	/* Year = 2013 */
			| ((DWORD)1 << 21)				/* Month = 1 */
			| ((DWORD)1 << 16)				/* Day_m = 1*/
			| ((DWORD)0 << 11)				/* Hour = 0 */
			| ((DWORD)0 << 5)				/* Min = 0 */
			| ((DWORD)0 >> 1);				/* Sec = 0 */
}

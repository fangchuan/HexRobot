/*
*********************************************************************************************************
*
*	ģ������ : fatfs��sd���Ľӿ��ļ�
*	�ļ����� : diskio.c
*	��    �� : V1.0
*	˵    �� : ֧��SD��
*
*	�޸ļ�¼ :
*		�汾��   ����         ����           ˵��
*       v1.0    2014-06-19   Eric2013        �׷�
*
*	Copyright (C), 2014-2015, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "diskio.h"		    /* FatFs lower layer API */
#include "bsp.h"

#define ATA			1     //SD���Ĵ����߼���
#define SPI_FLASH		0 //�ⲿFlash�Ĵ����߼���

//����W25Q128
//ǰ14M�ֽڸ�fatfs��,14M�ֽں�,���ͻ��Լ���	 	
#define FLASH_SECTOR_SIZE 	512			  		    
u16	    FLASH_SECTOR_COUNT = 2048*14;	//W25Q128,14M�ֽ�ȫ��FATFSռ��
#define FLASH_BLOCK_SIZE   	8     	//ÿ��BLOCK��8������
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
					SD_WaitReadOperation();  //ѭ����ѯdma�����Ƿ����				
					/* Wait until end of DMA transfer */
					while(SD_GetStatus() != SD_TRANSFER_OK);
				}
				else
				{					
					SD_ReadBlock(buff, sector*SD_BLOCK_SIZE, SD_BLOCK_SIZE);
					/* Check if the Transfer is finished */
					SD_WaitReadOperation();  //ѭ����ѯdma�����Ƿ����				
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
						SD_WaitWriteOperation();	   //�ȴ�dma�������
						while(SD_GetStatus() != SD_TRANSFER_OK); //�ȴ�sdio��sd���������
					}
					else
					{
						SD_WriteBlock((uint8_t *)buff,sector*SD_BLOCK_SIZE,SD_BLOCK_SIZE);						
						/* Check if the Transfer is finished */
						SD_WaitWriteOperation();	   //�ȴ�dma�������
						while(SD_GetStatus() != SD_TRANSFER_OK); //�ȴ�sdio��sd���������
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
						*(WORD * )buff = FLASH_SECTOR_SIZE;		//flash��Сд��ԪΪҳ��256�ֽڣ��˴�ȡ2ҳΪһ����д��λ
					break;
					case GET_BLOCK_SIZE :      // Get erase block size in unit of sector (DWORD)
						*(DWORD * )buff = FLASH_BLOCK_SIZE;		//flash��4kΪ��С������λ,512bytesΪһ����������һ��������8������
					break;
					case GET_SECTOR_COUNT:
						*(DWORD * )buff = FLASH_SECTOR_COUNT;		//����fatfs��sector����
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
*	�� �� ��: get_fattime
*	����˵��: ���ϵͳʱ�䣬���ڸ�д�ļ��Ĵ������޸�ʱ�䡣
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
DWORD get_fattime (void)
{
	/* �����ȫ��ʱ�ӣ��ɰ�����ĸ�ʽ����ʱ��ת��. ���������2013-01-01 00:00:00 */

	return	  ((DWORD)(2013 - 1980) << 25)	/* Year = 2013 */
			| ((DWORD)1 << 21)				/* Month = 1 */
			| ((DWORD)1 << 16)				/* Day_m = 1*/
			| ((DWORD)0 << 11)				/* Hour = 0 */
			| ((DWORD)0 << 5)				/* Min = 0 */
			| ((DWORD)0 >> 1);				/* Sec = 0 */
}

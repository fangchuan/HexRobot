#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "stm32f4xx.h"
#include <stdio.h>

#include "diskio.h"
#include "integer.h"

//W25X系列/Q系列芯片列表	   
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16	
//W25Q128 ID  0XEF17	
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17

/* Private typedef -----------------------------------------------------------*/
#define SPI_FLASH_PageSize              256
#define SPI_FLASH_PerWritePageSize      256

/* Private define ------------------------------------------------------------*/
#define W25X_WriteEnable		      0x06 
#define W25X_WriteDisable		      0x04 
#define W25X_ReadStatusReg		    0x05 
#define W25X_WriteStatusReg		    0x01 
#define W25X_ReadData			        0x03 
#define W25X_FastReadData		      0x0B 
#define W25X_FastReadDual		      0x3B 
#define W25X_PageProgram		      0x02 
#define W25X_BlockErase			      0xD8 
#define W25X_SectorErase		      0x20 
#define W25X_ChipErase			      0xC7 
#define W25X_PowerDown			      0xB9 
#define W25X_ReleasePowerDown	    0xAB 
#define W25X_DeviceID			        0xAB 
#define W25X_ManufactDeviceID   	0x90 
#define W25X_JedecDeviceID		    0x9F 

#define WIP_Flag                  0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte                0xFF



/*SPI接口*/
#define FLASH_SPI                           SPI2
#define FLASH_SPI_AF                        GPIO_AF_SPI2
#define FLASH_SPI_CLK                       RCC_APB1Periph_SPI2
#define FLASH_SPI_GPIO_CLK              		RCC_AHB1Periph_GPIOB

#define FLASH_SPI_SCK_PIN                   GPIO_Pin_13                  /* PB.13 */
#define FLASH_SPI_SCK_PinSource             GPIO_PinSource13
#define FLASH_SPI_MISO_PIN                  GPIO_Pin_14                  /* PB.14 */
#define FLASH_SPI_MISO_PinSource            GPIO_PinSource14
#define FLASH_SPI_MOSI_PIN                  GPIO_Pin_15                  /* PB.15 */
#define FLASH_SPI_MOSI_PinSource            GPIO_PinSource15
#define FLASH_CS_PIN                        GPIO_Pin_12                 /* PB.12 */
#define FLASH_SPI_GPIO_PORT                 GPIOB                       /* GPIOB */


#define SPI_FLASH_CS_LOW()      GPIO_ResetBits(GPIOB, GPIO_Pin_12)				//输出低电平 GPIO_ResetBits(FLASH_CS_GPIO_PORT, FLASH_CS_PIN)
#define SPI_FLASH_CS_HIGH()     GPIO_SetBits(GPIOB, GPIO_Pin_12)			  //设置为高电平 GPIO_SetBits(FLASH_CS_GPIO_PORT, FLASH_CS_PIN)

extern void bsp_FlashInit(void);
u16  W25QXX_ReadID(void);  	    		//读取FLASH ID
u8	 W25QXX_ReadSR(void);        		//读取状态寄存器 
void W25QXX_Write_SR(u8 sr);  			//写状态寄存器
void W25QXX_Write_Enable(void);  		//写使能 
void W25QXX_Write_Disable(void);		//写保护
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   //读取flash
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//写入flash
void W25QXX_Erase_Chip(void);    	  	//整片擦除
void W25QXX_Erase_Sector(u32 Dst_Addr);	//扇区擦除
void W25QXX_Wait_Busy(void);           	//等待空闲
void W25QXX_PowerDown(void);        	//进入掉电模式
void W25QXX_WAKEUP(void);				//唤醒
#endif /* __SPI_FLASH_H */

/***************************** 阿波罗科技 www.apollorobot.com (END OF FILE) *********************************/

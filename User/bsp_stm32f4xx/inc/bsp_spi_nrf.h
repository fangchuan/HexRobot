/*
*********************************************************************************************************
*
*	模块名称 : NRF24L01驱动头文件
*	文件名称 : bsp_spi_nrf.h
*	版    本 : V1.0
*	说    明 : 
*	修改记录 :
*		版本号  日期       作者    说明
*		V1.0    2016-03-30 方川  正式发布
*
*	Copyright (C), 2015-2020, 阿波罗科技 www.apollorobot.com
*
*********************************************************************************************************
*/
#ifndef __SPI_NRF_H
#define __SPI_NRF_H

#include "stm32f4xx.h"

//24L01配置相关
#define CHANAL 40	//频道选择 


//接收数据类型
typedef struct _NRF_PKT {
	
				uint8_t car_speed;   //小车前进还是后退
	      uint8_t car_angle;   //小车左转还是右转
	      uint8_t key_value;   //键值
	      int X_angle;         //MPU6050   roll
				int Y_angle;         //MPU6050   pitch
	      
}_nrf_pkt;

#define MAX_RT      0x10 //达到最大重发次数中断标志位
#define TX_DS		    0x20 //发送完成中断标志位	  // 

#define RX_DR		    0x40 //接收到数据中断标志位

#define  NRF_ROCKER_FORWARD   1
#define  NRF_ROCKER_BACKWARD  255
#define  NRF_ROCKER_LEFT      1
#define  NRF_ROCKER_RIGHT     255
#define  NRF_STOP             0
#define  NRF_KEY_FORWARD      5
#define  NRF_KEY_BACKWARD     8
#define  NRF_KEY_LEFT         6
#define  NRF_KEY_RIGHT        7
#define  NRF_EULER_THRE       15   //体感模式下的阈值是15度
#define  NRF_EULER_SAFE       10   //体感模式下的安全值是10度
//
#define ADR_WIDTH 	5  	//发射地址宽度
#define TX_PLOAD_WIDTH  sizeof(_nrf_pkt)   //发射数据通道有效数据宽度0~32Byte 
#define RX_PLOAD_WIDTH  sizeof(_nrf_pkt)

#define NRF_CE_PIN          GPIO_Pin_0
#define NRF_CE_PORT         GPIOB
#define NRF_CS_PIN          GPIO_Pin_1
#define NRF_CS_PORT         GPIOB
#define NRF_SPI             SPI1
#define NRF_SPI_CLK         RCC_APB2Periph_SPI1
#define NRF_SPI_SCK         GPIO_Pin_3
#define NRF_SPI_MISO        GPIO_Pin_4
#define NRF_SPI_MOSI        GPIO_Pin_5
#define NRF_SPI_PORT        GPIOB
#define NRF_IRQ_PIN         GPIO_Pin_2
#define NRF_IRQ_PORT        GPIOB
#define NRF_PORT_CLK        (RCC_AHB1Periph_GPIOB)

#define NRF_CSN_HIGH()      GPIO_SetBits(NRF_CS_PORT, NRF_CS_PIN)
#define NRF_CSN_LOW()       GPIO_ResetBits(NRF_CS_PORT, NRF_CS_PIN)		        //csn置低
#define NRF_CE_HIGH()	      GPIO_SetBits(NRF_CE_PORT,NRF_CE_PIN)
#define NRF_CE_LOW()	      GPIO_ResetBits(NRF_CE_PORT,NRF_CE_PIN)			      //CE置低
#define NRF_Read_IRQ()		  GPIO_ReadInputDataBit( NRF_IRQ_PORT, NRF_IRQ_PIN)  //中断引脚

extern void bsp_InitNRF(void);
extern void NRF_TX_Mode(void);
extern void NRF_RX_Mode(void);
extern u8 NRF_Rx_Dat(u8 *rxbuf);
extern u8 NRF_Tx_Dat(u8 *txbuf);
extern u8 NRF_Check(void); 

#endif /* __SPI_NRF_H */

/***************************** 阿波罗科技 www.apollorobot.com (END OF FILE) *********************************/

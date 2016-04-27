/*
*********************************************************************************************************
*
*	模块名称 : io引脚位操作头文件
*	文件名称 : bsp_ioctl.h
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
#ifndef  _IO_CTL_H
#define  _IO_CTL_H

//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))  

//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+0x14) //0x40020014 
#define GPIOB_ODR_Addr    (GPIOB_BASE+0x14) //0x40020414 
#define GPIOC_ODR_Addr    (GPIOC_BASE+0x14) //0x40020814 
#define GPIOD_ODR_Addr    (GPIOD_BASE+0x14) //0x40020C14 
#define GPIOE_ODR_Addr    (GPIOE_BASE+0x14) //0x40021014 
#define GPIOF_ODR_Addr    (GPIOF_BASE+0x14) //0x40021414    
#define GPIOG_ODR_Addr    (GPIOG_BASE+0x14) //0x40021814    

#define GPIOA_IDR_Addr    (GPIOA_BASE+0x10) //0x40020010 
#define GPIOB_IDR_Addr    (GPIOB_BASE+0x10) //0x40020010 
#define GPIOC_IDR_Addr    (GPIOC_BASE+0x10) //0x40020010 
#define GPIOD_IDR_Addr    (GPIOD_BASE+0x10) //0x40020010
#define GPIOE_IDR_Addr    (GPIOE_BASE+0x10) //0x40020010 
#define GPIOF_IDR_Addr    (GPIOF_BASE+0x10) //0x40020010
#define GPIOG_IDR_Addr    (GPIOG_BASE+0x10) //0x40020010 

#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Adrr,n)  //输出
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入


/* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)			{p->BSRRL=i;}			//设置为高电平		
#define digitalLo(p,i)			{p->BSRRH=i;}				//输出低电平
#define digitalToggle(p,i)		{p->ODR ^=i;}			//输出反转状态

#endif /*_IO_OPERATION_H*/

/***************************** 阿波罗科技 www.apollorobot.com (END OF FILE) *********************************/


/*
*********************************************************************************************************
*
*	ģ������ : io����λ����ͷ�ļ�
*	�ļ����� : bsp_ioctl.h
*	��    �� : V1.0
*	˵    �� : 
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		V1.0    2016-03-30 ����  ��ʽ����
*
*	Copyright (C), 2015-2020, �����޿Ƽ� www.apollorobot.com
*
*********************************************************************************************************
*/
#ifndef  _IO_CTL_H
#define  _IO_CTL_H

//IO�ڲ����궨��
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))  

//IO�ڵ�ַӳ��
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

#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //���
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //����

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //����

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //��� 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //���� 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //��� 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //����

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //���
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //����

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Adrr,n)  //���
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //����

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //���
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //����


/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)			{p->BSRRL=i;}			//����Ϊ�ߵ�ƽ		
#define digitalLo(p,i)			{p->BSRRH=i;}				//����͵�ƽ
#define digitalToggle(p,i)		{p->ODR ^=i;}			//�����ת״̬

#endif /*_IO_OPERATION_H*/

/***************************** �����޿Ƽ� www.apollorobot.com (END OF FILE) *********************************/


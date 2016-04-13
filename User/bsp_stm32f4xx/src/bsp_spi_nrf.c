/**
  ******************************************************************************
  * @file    bsp_xxx.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   2.4g����ģ��/nrf24l01+/master Ӧ��bsp
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "bsp_spi_nrf.h"
#include "bsp_uart_fifo.h"


// SPI(nRF24L01) commands ,	NRF��SPI����궨�壬���NRF����ʹ���ĵ�
#define NRF_READ_REG    0x00  // Define read command to register
#define NRF_WRITE_REG   0x20  // Define write command to register
#define RD_RX_PLOAD 0x61  // Define RX payload register address
#define WR_TX_PLOAD 0xA0  // Define TX payload register address
#define FLUSH_TX    0xE1  // Define flush TX register command
#define FLUSH_RX    0xE2  // Define flush RX register command
#define REUSE_TX_PL 0xE3  // Define reuse TX payload register command
#define NOP         0xFF  // Define No Operation, might be used to read status register

// SPI(nRF24L01) registers(addresses) ��NRF24L01 ��ؼĴ�����ַ�ĺ궨��
#define CONFIG      0x00  // 'Config' register address
#define EN_AA       0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR   0x02  // 'Enabled RX addresses' register address
#define SETUP_AW    0x03  // 'Setup address width' register address
#define SETUP_RETR  0x04  // 'Setup Auto. Retrans' register address
#define RF_CH       0x05  // 'RF channel' register address
#define RF_SETUP    0x06  // 'RF setup' register address
#define STATUS      0x07  // 'Status' register address
#define OBSERVE_TX  0x08  // 'Observe TX' register address
#define CD          0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0  0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1  0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2  0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3  0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4  0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5  0x0F  // 'RX address pipe5' register address
#define TX_ADDR     0x10  // 'TX address' register address
#define RX_PW_P0    0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1    0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2    0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3    0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4    0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5    0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS 0x17  // 'FIFO Status Register' register address


 /*********************************************************************
*
*       Global data
*
**********************************************************************
*/

// u8 nrf_tx[TX_PLOAD_WIDTH];		//�������ݻ���
__IO u8 NRF_ADDRESS[ADR_WIDTH] = {0x00,0x01,0x02,0x03,0x14};  // ����һ�����͵�ַ,Ĭ��Ϊ0��ַ

 /*********************************************************************
*
*       Static code
*
**********************************************************************
*/
static void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--);
} 


/**
  * @brief   ������NRF��/дһ�ֽ�����
  * @param   д�������
  *		@arg dat 
  * @retval  ��ȡ�õ�����
  */
static u8 SPI_NRF_RW(u8 dat)
{  	
   /* �� SPI���ͻ������ǿ�ʱ�ȴ� */
  while (SPI_I2S_GetFlagStatus(NRF_SPI, SPI_I2S_FLAG_TXE) == RESET);
  
   /* ͨ�� SPI2����һ�ֽ����� */
  SPI_I2S_SendData(NRF_SPI, dat);		
 
   /* ��SPI���ջ�����Ϊ��ʱ�ȴ� */
  while (SPI_I2S_GetFlagStatus(NRF_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(NRF_SPI);
}

/**
  * @brief   ������NRF�ض��ļĴ���д������
  * @param   
  *		@arg reg:NRF������+�Ĵ�����ַ
  *		@arg dat:��Ҫ��Ĵ���д�������
  * @retval  NRF��status�Ĵ�����״̬
  */
static u8 SPI_NRF_WriteReg(u8 reg,u8 dat)
{
		 u8 status;
		 NRF_CE_LOW();
		/*�õ�CSN��ʹ��SPI����*/
		 NRF_CSN_LOW();
					
		/*��������Ĵ����� */
		 status = SPI_NRF_RW(reg);
			 
		 /*��Ĵ���д������*/
		 SPI_NRF_RW(dat); 
							
		/*CSN���ߣ����*/	   
		 NRF_CSN_HIGH();	
			
		/*����״̬�Ĵ�����ֵ*/
			return(status);
}

/**
  * @brief   ���ڴ�NRF�ض��ļĴ�����������
  * @param   
  *		@arg reg:NRF������+�Ĵ�����ַ
  * @retval  �Ĵ����е�����
  */
static u8 SPI_NRF_ReadReg(u8 reg)
{
		u8 reg_val;

		NRF_CE_LOW();
		/*�õ�CSN��ʹ��SPI����*/
		NRF_CSN_LOW();
					
			 /*���ͼĴ�����*/
		SPI_NRF_RW(reg); 

		 /*��ȡ�Ĵ�����ֵ */
		reg_val = SPI_NRF_RW(NOP);
								
			/*CSN���ߣ����*/
		NRF_CSN_HIGH();		
			
		return reg_val;
}	

/**
  * @brief   ������NRF�ļĴ�����д��һ������
  * @param   
  *		@arg reg : NRF������+�Ĵ�����ַ
  *		@arg pBuf�����ڴ洢���������ļĴ������ݵ����飬�ⲿ����
  * 	@arg bytes: pBuf�����ݳ���
  * @retval  NRF��status�Ĵ�����״̬
  */
static u8 SPI_NRF_ReadBuf(u8 reg,u8 *pBuf,u8 bytes)
{
		u8 status, byte_cnt;

		NRF_CE_LOW();
		/*�õ�CSN��ʹ��SPI����*/
		NRF_CSN_LOW();
			
		/*���ͼĴ�����*/		
		status = SPI_NRF_RW(reg); 

		/*��ȡ����������*/
		for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)		  
			 pBuf[byte_cnt] = SPI_NRF_RW(NOP); //��NRF24L01��ȡ����  

		 /*CSN���ߣ����*/
		NRF_CSN_HIGH();	
			
		return status;		//���ؼĴ���״ֵ̬
}

/**
  * @brief   ������NRF�ļĴ�����д��һ������
  * @param   
  *		@arg reg : NRF������+�Ĵ�����ַ
  *		@arg pBuf���洢�˽�Ҫд��д�Ĵ������ݵ����飬�ⲿ����
  * 	@arg bytes: pBuf�����ݳ���
  * @retval  NRF��status�Ĵ�����״̬
  */
static u8 SPI_NRF_WriteBuf(u8 reg ,u8 *pBuf,u8 bytes)
{
		 u8 status,byte_cnt;
		 NRF_CE_LOW();
			 /*�õ�CSN��ʹ��SPI����*/
		 NRF_CSN_LOW();			

		 /*���ͼĴ�����*/	
		 status = SPI_NRF_RW(reg); 
		
				/*�򻺳���д������*/
		 for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)
			SPI_NRF_RW(*pBuf++);	//д���ݵ������� 	 
					 
		/*CSN���ߣ����*/
		 NRF_CSN_HIGH();			
		
			return (status);	//����NRF24L01��״̬ 		
}


/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/**
  * @brief  SPI�� I/O����
  * @param  ��
  * @retval ��
  */
void bsp_InitNRF(void)
{
		SPI_InitTypeDef  SPI_InitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;
		
		/*������ӦIO�˿ڵ�ʱ��*/
		RCC_APB2PeriphClockCmd(NRF_PORT_CLK,ENABLE);

		/*ʹ��SPI1ʱ��*/
		RCC_APB2PeriphClockCmd(NRF_SPI_CLK, ENABLE);

		 /*���� SPI_NRF_SPI�� SCK,MISO,MOSI���ţ�GPIOB^3,GPIOB^4,GPIOB^5 */
		GPIO_PinAFConfig(NRF_SPI_PORT, GPIO_PinSource3, GPIO_AF_SPI1);
		GPIO_PinAFConfig(NRF_SPI_PORT, GPIO_PinSource4, GPIO_AF_SPI1);
		GPIO_PinAFConfig(NRF_SPI_PORT, GPIO_PinSource5, GPIO_AF_SPI1);
		GPIO_InitStructure.GPIO_Pin = NRF_SPI_SCK | NRF_SPI_MISO | NRF_SPI_MOSI;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
		GPIO_Init(NRF_SPI_PORT, &GPIO_InitStructure);  

		/*����SPI_NRF_SPI��CE����,��SPI_NRF_SPI�� CSN ����*/
		GPIO_InitStructure.GPIO_Pin = NRF_CE_PIN | NRF_CS_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* ��Ϊ����� */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* ���������費ʹ�� */
		GPIO_Init(NRF_CS_PORT, &GPIO_InitStructure);


		 /*����SPI_NRF_SPI��IRQ����*/
		GPIO_InitStructure.GPIO_Pin = NRF_IRQ_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN ;  //��������
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
		GPIO_Init(NRF_IRQ_PORT, &GPIO_InitStructure); 
				
		/* �����Զ���ĺ꣬��������csn���ţ�NRF�������״̬ */
		NRF_CSN_HIGH(); 
	 
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //˫��ȫ˫��
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	 					//��ģʽ
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	 				//���ݴ�С8λ
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		 				//ʱ�Ӽ��ԣ�����ʱΪ��
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						//��1��������Ч��������Ϊ����ʱ��
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		   					//NSS�ź����������
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;  //8��Ƶ��9MHz
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  				//��λ��ǰ
		SPI_InitStructure.SPI_CRCPolynomial = 7;
		SPI_Init(NRF_SPI, &SPI_InitStructure);

		/* Enable SPI1  */
		SPI_Cmd(NRF_SPI, ENABLE);
}

/**
  * @brief  ���ò��������ģʽ
  * @param  ��
  * @retval ��
  */
void NRF_RX_Mode(void)

{
	 NRF_CE_LOW();	

   SPI_NRF_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0,(u8 *)NRF_ADDRESS,ADR_WIDTH);//дRX�ڵ��ַ

   SPI_NRF_WriteReg(NRF_WRITE_REG+EN_AA,0x01);    //ʹ��ͨ��0���Զ�Ӧ��    

   SPI_NRF_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01);//ʹ��ͨ��0�Ľ��յ�ַ    

   SPI_NRF_WriteReg(NRF_WRITE_REG+RF_CH,CHANAL);      //����RFͨ��Ƶ��    

   SPI_NRF_WriteReg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ��      

   SPI_NRF_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f); //����TX�������,0db����,2Mbps,���������濪��   

   SPI_NRF_WriteReg(NRF_WRITE_REG+CONFIG, 0x0f);  //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 

	/*CE���ߣ��������ģʽ*/	
		NRF_CE_HIGH();

}    

/**
  * @brief  ���÷���ģʽ
  * @param  ��
  * @retval ��
  */
void NRF_TX_Mode(void)
{  
	 NRF_CE_LOW();		

   SPI_NRF_WriteBuf(NRF_WRITE_REG+TX_ADDR,(u8 *)NRF_ADDRESS,ADR_WIDTH);    //дTX�ڵ��ַ 

   SPI_NRF_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0,(u8 *)NRF_ADDRESS,ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK   

   SPI_NRF_WriteReg(NRF_WRITE_REG+EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��    

   SPI_NRF_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  

   SPI_NRF_WriteReg(NRF_WRITE_REG+SETUP_RETR,0x1a);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��

   SPI_NRF_WriteReg(NRF_WRITE_REG+RF_CH,CHANAL);       //����RFͨ��ΪCHANAL

   SPI_NRF_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f);  //����TX�������,0db����,2Mbps,���������濪��   
	
   SPI_NRF_WriteReg(NRF_WRITE_REG+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�

	/*CE���ߣ����뷢��ģʽ*/	
		NRF_CE_HIGH();
    Delay(0xffff); //CEҪ����һ��ʱ��Ž��뷢��ģʽ
}

/**
  * @brief  ��Ҫ����NRF��MCU�Ƿ���������
  * @param  ��
  * @retval SUCCESS/ERROR ��������/����ʧ��
  */
u8 NRF_Check(void)
{
		u8 buf[5]={0xC2,0xC2,0xC2,0xC2,0xC2};
		u8 buf1[5];
		u8 i; 
		 
		/*д��5���ֽڵĵ�ַ.  */  
		SPI_NRF_WriteBuf(NRF_WRITE_REG+TX_ADDR,buf,5);

		/*����д��ĵ�ַ */
		SPI_NRF_ReadBuf(TX_ADDR,buf1,5); 
		 
		/*�Ƚ�*/               
		for(i=0;i<5;i++)
		{
			if(buf1[i]!=0xC2)
				break;
		} 
					 
		if(i==5)
			return SUCCESS ;        //MCU��NRF�ɹ����� 
		else
			return ERROR ;        //MCU��NRF����������
}

/**
  * @brief   ������NRF�ķ��ͻ�������д������
  * @param   
  *		@arg txBuf���洢�˽�Ҫ���͵����ݵ����飬�ⲿ����	
  * @retval  ���ͽ�����ɹ�����TXDS,ʧ�ܷ���MAXRT��ERROR
  */
u8 NRF_Tx_Dat(u8 *txbuf)
{
		u8 state;  

		 /*ceΪ�ͣ��������ģʽ1*/
		NRF_CE_LOW();

		/*д���ݵ�TX BUF ��� 32���ֽ�*/						
		SPI_NRF_WriteBuf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);

		/*CEΪ�ߣ�txbuf�ǿգ��������ݰ� */   
		NRF_CE_HIGH();
				
		/*�ȴ���������ж� */                            
		while(NRF_Read_IRQ()!=0); 	
		
		/*��ȡ״̬�Ĵ�����ֵ */                              
		state = SPI_NRF_ReadReg(STATUS);

		 /*���TX_DS��MAX_RT�жϱ�־*/                  
		SPI_NRF_WriteReg(NRF_WRITE_REG+STATUS,state); 	

		SPI_NRF_WriteReg(FLUSH_TX,NOP);    //���TX FIFO�Ĵ��� 

		 /*�ж��ж�����*/    
		if(state&MAX_RT)                     //�ﵽ����ط�����
				 return MAX_RT; 

		else if(state&TX_DS)                  //�������
				return TX_DS;
		 else						  
				return ERROR;                 //����ԭ����ʧ��
} 

/**
  * @brief   ���ڴ�NRF�Ľ��ջ������ж�������
  * @param   
  *		@arg rxBuf �����ڽ��ո����ݵ����飬�ⲿ����	
  * @retval 
  *		@arg ���ս��
  */
u8 NRF_Rx_Dat(u8 *rxbuf)
{
		u8          state; 
	  u16  link_count=0;
		NRF_CE_HIGH();	 //�������״̬
		 /*�ȴ������ж�*/
		while(NRF_Read_IRQ()!=0)
		{
			  link_count ++;
			  if(link_count == 5000)  //5000Ϊ�����������
					return ERROR;
		}			
		
		NRF_CE_LOW();  	 //�������״̬
		/*��ȡstatus�Ĵ�����ֵ  */               
		state=SPI_NRF_ReadReg(STATUS);
		 
		/* ����жϱ�־*/      
		SPI_NRF_WriteReg(NRF_WRITE_REG+STATUS,state);

		/*�ж��Ƿ���յ�����*/
		if(state&RX_DR)                                 //���յ�����
		{
			SPI_NRF_ReadBuf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
			SPI_NRF_WriteReg(FLUSH_RX,NOP);          //���RX FIFO�Ĵ���
			return RX_DR; 
		}
		else    
			return ERROR;                    //û�յ��κ�����
}
/***************************** �����޿Ƽ� www.apollorobot.com (END OF FILE) *********************************/

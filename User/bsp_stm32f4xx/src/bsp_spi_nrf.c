/**
  ******************************************************************************
  * @file    bsp_xxx.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   2.4g无线模块/nrf24l01+/master 应用bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "bsp_spi_nrf.h"
#include "bsp_uart_fifo.h"


// SPI(nRF24L01) commands ,	NRF的SPI命令宏定义，详见NRF功能使用文档
#define NRF_READ_REG    0x00  // Define read command to register
#define NRF_WRITE_REG   0x20  // Define write command to register
#define RD_RX_PLOAD 0x61  // Define RX payload register address
#define WR_TX_PLOAD 0xA0  // Define TX payload register address
#define FLUSH_TX    0xE1  // Define flush TX register command
#define FLUSH_RX    0xE2  // Define flush RX register command
#define REUSE_TX_PL 0xE3  // Define reuse TX payload register command
#define NOP         0xFF  // Define No Operation, might be used to read status register

// SPI(nRF24L01) registers(addresses) ，NRF24L01 相关寄存器地址的宏定义
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

// u8 nrf_tx[TX_PLOAD_WIDTH];		//发射数据缓存
__IO u8 NRF_ADDRESS[ADR_WIDTH] = {0x00,0x01,0x02,0x03,0x14};  // 定义一个发送地址,默认为0地址

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
  * @brief   用于向NRF读/写一字节数据
  * @param   写入的数据
  *		@arg dat 
  * @retval  读取得的数据
  */
static u8 SPI_NRF_RW(u8 dat)
{  	
   /* 当 SPI发送缓冲器非空时等待 */
  while (SPI_I2S_GetFlagStatus(NRF_SPI, SPI_I2S_FLAG_TXE) == RESET);
  
   /* 通过 SPI2发送一字节数据 */
  SPI_I2S_SendData(NRF_SPI, dat);		
 
   /* 当SPI接收缓冲器为空时等待 */
  while (SPI_I2S_GetFlagStatus(NRF_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(NRF_SPI);
}

/**
  * @brief   用于向NRF特定的寄存器写入数据
  * @param   
  *		@arg reg:NRF的命令+寄存器地址
  *		@arg dat:将要向寄存器写入的数据
  * @retval  NRF的status寄存器的状态
  */
static u8 SPI_NRF_WriteReg(u8 reg,u8 dat)
{
		 u8 status;
		 NRF_CE_LOW();
		/*置低CSN，使能SPI传输*/
		 NRF_CSN_LOW();
					
		/*发送命令及寄存器号 */
		 status = SPI_NRF_RW(reg);
			 
		 /*向寄存器写入数据*/
		 SPI_NRF_RW(dat); 
							
		/*CSN拉高，完成*/	   
		 NRF_CSN_HIGH();	
			
		/*返回状态寄存器的值*/
			return(status);
}

/**
  * @brief   用于从NRF特定的寄存器读出数据
  * @param   
  *		@arg reg:NRF的命令+寄存器地址
  * @retval  寄存器中的数据
  */
static u8 SPI_NRF_ReadReg(u8 reg)
{
		u8 reg_val;

		NRF_CE_LOW();
		/*置低CSN，使能SPI传输*/
		NRF_CSN_LOW();
					
			 /*发送寄存器号*/
		SPI_NRF_RW(reg); 

		 /*读取寄存器的值 */
		reg_val = SPI_NRF_RW(NOP);
								
			/*CSN拉高，完成*/
		NRF_CSN_HIGH();		
			
		return reg_val;
}	

/**
  * @brief   用于向NRF的寄存器中写入一串数据
  * @param   
  *		@arg reg : NRF的命令+寄存器地址
  *		@arg pBuf：用于存储将被读出的寄存器数据的数组，外部定义
  * 	@arg bytes: pBuf的数据长度
  * @retval  NRF的status寄存器的状态
  */
static u8 SPI_NRF_ReadBuf(u8 reg,u8 *pBuf,u8 bytes)
{
		u8 status, byte_cnt;

		NRF_CE_LOW();
		/*置低CSN，使能SPI传输*/
		NRF_CSN_LOW();
			
		/*发送寄存器号*/		
		status = SPI_NRF_RW(reg); 

		/*读取缓冲区数据*/
		for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)		  
			 pBuf[byte_cnt] = SPI_NRF_RW(NOP); //从NRF24L01读取数据  

		 /*CSN拉高，完成*/
		NRF_CSN_HIGH();	
			
		return status;		//返回寄存器状态值
}

/**
  * @brief   用于向NRF的寄存器中写入一串数据
  * @param   
  *		@arg reg : NRF的命令+寄存器地址
  *		@arg pBuf：存储了将要写入写寄存器数据的数组，外部定义
  * 	@arg bytes: pBuf的数据长度
  * @retval  NRF的status寄存器的状态
  */
static u8 SPI_NRF_WriteBuf(u8 reg ,u8 *pBuf,u8 bytes)
{
		 u8 status,byte_cnt;
		 NRF_CE_LOW();
			 /*置低CSN，使能SPI传输*/
		 NRF_CSN_LOW();			

		 /*发送寄存器号*/	
		 status = SPI_NRF_RW(reg); 
		
				/*向缓冲区写入数据*/
		 for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)
			SPI_NRF_RW(*pBuf++);	//写数据到缓冲区 	 
					 
		/*CSN拉高，完成*/
		 NRF_CSN_HIGH();			
		
			return (status);	//返回NRF24L01的状态 		
}


/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/**
  * @brief  SPI的 I/O配置
  * @param  无
  * @retval 无
  */
void bsp_InitNRF(void)
{
		SPI_InitTypeDef  SPI_InitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;
		
		/*开启相应IO端口的时钟*/
		RCC_APB2PeriphClockCmd(NRF_PORT_CLK,ENABLE);

		/*使能SPI1时钟*/
		RCC_APB2PeriphClockCmd(NRF_SPI_CLK, ENABLE);

		 /*配置 SPI_NRF_SPI的 SCK,MISO,MOSI引脚，GPIOB^3,GPIOB^4,GPIOB^5 */
		GPIO_PinAFConfig(NRF_SPI_PORT, GPIO_PinSource3, GPIO_AF_SPI1);
		GPIO_PinAFConfig(NRF_SPI_PORT, GPIO_PinSource4, GPIO_AF_SPI1);
		GPIO_PinAFConfig(NRF_SPI_PORT, GPIO_PinSource5, GPIO_AF_SPI1);
		GPIO_InitStructure.GPIO_Pin = NRF_SPI_SCK | NRF_SPI_MISO | NRF_SPI_MOSI;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
		GPIO_Init(NRF_SPI_PORT, &GPIO_InitStructure);  

		/*配置SPI_NRF_SPI的CE引脚,和SPI_NRF_SPI的 CSN 引脚*/
		GPIO_InitStructure.GPIO_Pin = NRF_CE_PIN | NRF_CS_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* 设为输出口 */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* 设为推挽模式 */
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* 上下拉电阻不使能 */
		GPIO_Init(NRF_CS_PORT, &GPIO_InitStructure);


		 /*配置SPI_NRF_SPI的IRQ引脚*/
		GPIO_InitStructure.GPIO_Pin = NRF_IRQ_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN ;  //上拉输入
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
		GPIO_Init(NRF_IRQ_PORT, &GPIO_InitStructure); 
				
		/* 这是自定义的宏，用于拉高csn引脚，NRF进入空闲状态 */
		NRF_CSN_HIGH(); 
	 
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //双线全双工
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	 					//主模式
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	 				//数据大小8位
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		 				//时钟极性，空闲时为低
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						//第1个边沿有效，上升沿为采样时刻
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		   					//NSS信号由软件产生
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;  //8分频，9MHz
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  				//高位在前
		SPI_InitStructure.SPI_CRCPolynomial = 7;
		SPI_Init(NRF_SPI, &SPI_InitStructure);

		/* Enable SPI1  */
		SPI_Cmd(NRF_SPI, ENABLE);
}

/**
  * @brief  配置并进入接收模式
  * @param  无
  * @retval 无
  */
void NRF_RX_Mode(void)

{
	 NRF_CE_LOW();	

   SPI_NRF_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0,(u8 *)NRF_ADDRESS,ADR_WIDTH);//写RX节点地址

   SPI_NRF_WriteReg(NRF_WRITE_REG+EN_AA,0x01);    //使能通道0的自动应答    

   SPI_NRF_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01);//使能通道0的接收地址    

   SPI_NRF_WriteReg(NRF_WRITE_REG+RF_CH,CHANAL);      //设置RF通信频率    

   SPI_NRF_WriteReg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度      

   SPI_NRF_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f); //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   

   SPI_NRF_WriteReg(NRF_WRITE_REG+CONFIG, 0x0f);  //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 

	/*CE拉高，进入接收模式*/	
		NRF_CE_HIGH();

}    

/**
  * @brief  配置发送模式
  * @param  无
  * @retval 无
  */
void NRF_TX_Mode(void)
{  
	 NRF_CE_LOW();		

   SPI_NRF_WriteBuf(NRF_WRITE_REG+TX_ADDR,(u8 *)NRF_ADDRESS,ADR_WIDTH);    //写TX节点地址 

   SPI_NRF_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0,(u8 *)NRF_ADDRESS,ADR_WIDTH); //设置TX节点地址,主要为了使能ACK   

   SPI_NRF_WriteReg(NRF_WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答    

   SPI_NRF_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址  

   SPI_NRF_WriteReg(NRF_WRITE_REG+SETUP_RETR,0x1a);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次

   SPI_NRF_WriteReg(NRF_WRITE_REG+RF_CH,CHANAL);       //设置RF通道为CHANAL

   SPI_NRF_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
	
   SPI_NRF_WriteReg(NRF_WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,发射模式,开启所有中断

	/*CE拉高，进入发送模式*/	
		NRF_CE_HIGH();
    Delay(0xffff); //CE要拉高一段时间才进入发送模式
}

/**
  * @brief  主要用于NRF与MCU是否正常连接
  * @param  无
  * @retval SUCCESS/ERROR 连接正常/连接失败
  */
u8 NRF_Check(void)
{
		u8 buf[5]={0xC2,0xC2,0xC2,0xC2,0xC2};
		u8 buf1[5];
		u8 i; 
		 
		/*写入5个字节的地址.  */  
		SPI_NRF_WriteBuf(NRF_WRITE_REG+TX_ADDR,buf,5);

		/*读出写入的地址 */
		SPI_NRF_ReadBuf(TX_ADDR,buf1,5); 
		 
		/*比较*/               
		for(i=0;i<5;i++)
		{
			if(buf1[i]!=0xC2)
				break;
		} 
					 
		if(i==5)
			return SUCCESS ;        //MCU与NRF成功连接 
		else
			return ERROR ;        //MCU与NRF不正常连接
}

/**
  * @brief   用于向NRF的发送缓冲区中写入数据
  * @param   
  *		@arg txBuf：存储了将要发送的数据的数组，外部定义	
  * @retval  发送结果，成功返回TXDS,失败返回MAXRT或ERROR
  */
u8 NRF_Tx_Dat(u8 *txbuf)
{
		u8 state;  

		 /*ce为低，进入待机模式1*/
		NRF_CE_LOW();

		/*写数据到TX BUF 最大 32个字节*/						
		SPI_NRF_WriteBuf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);

		/*CE为高，txbuf非空，发送数据包 */   
		NRF_CE_HIGH();
				
		/*等待发送完成中断 */                            
		while(NRF_Read_IRQ()!=0); 	
		
		/*读取状态寄存器的值 */                              
		state = SPI_NRF_ReadReg(STATUS);

		 /*清除TX_DS或MAX_RT中断标志*/                  
		SPI_NRF_WriteReg(NRF_WRITE_REG+STATUS,state); 	

		SPI_NRF_WriteReg(FLUSH_TX,NOP);    //清除TX FIFO寄存器 

		 /*判断中断类型*/    
		if(state&MAX_RT)                     //达到最大重发次数
				 return MAX_RT; 

		else if(state&TX_DS)                  //发送完成
				return TX_DS;
		 else						  
				return ERROR;                 //其他原因发送失败
} 

/**
  * @brief   用于从NRF的接收缓冲区中读出数据
  * @param   
  *		@arg rxBuf ：用于接收该数据的数组，外部定义	
  * @retval 
  *		@arg 接收结果
  */
u8 NRF_Rx_Dat(u8 *rxbuf)
{
		u8          state; 
	  u16  link_count=0;
		NRF_CE_HIGH();	 //进入接收状态
		 /*等待接收中断*/
		while(NRF_Read_IRQ()!=0)
		{
			  link_count ++;
			  if(link_count == 5000)  //5000为最大重连次数
					return ERROR;
		}			
		
		NRF_CE_LOW();  	 //进入待机状态
		/*读取status寄存器的值  */               
		state=SPI_NRF_ReadReg(STATUS);
		 
		/* 清除中断标志*/      
		SPI_NRF_WriteReg(NRF_WRITE_REG+STATUS,state);

		/*判断是否接收到数据*/
		if(state&RX_DR)                                 //接收到数据
		{
			SPI_NRF_ReadBuf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
			SPI_NRF_WriteReg(FLUSH_RX,NOP);          //清除RX FIFO寄存器
			return RX_DR; 
		}
		else    
			return ERROR;                    //没收到任何数据
}
/***************************** 阿波罗科技 www.apollorobot.com (END OF FILE) *********************************/

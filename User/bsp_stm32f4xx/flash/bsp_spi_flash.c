#include "bsp_spi_flash.h"

//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
static u8 SPI2_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(FLASH_SPI, TxData); //ͨ������SPIx����һ������
	retry=0;

	while (SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET)//���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(FLASH_SPI); //����ͨ��SPIx������յ�����					    
}

void bsp_FlashInit(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
  	SPI_InitTypeDef  SPI_InitStructure;

		RCC_AHB1PeriphClockCmd(	FLASH_SPI_GPIO_CLK, ENABLE );//PORTBʱ��ʹ�� 
		RCC_APB1PeriphClockCmd(	FLASH_SPI_CLK,  ENABLE );//SPI2ʱ��ʹ�� 	
	 
		GPIO_PinAFConfig(FLASH_SPI_GPIO_PORT, FLASH_SPI_SCK_PinSource, FLASH_SPI_AF);
	  GPIO_PinAFConfig(FLASH_SPI_GPIO_PORT, FLASH_SPI_MISO_PinSource, FLASH_SPI_AF);
		GPIO_PinAFConfig(FLASH_SPI_GPIO_PORT, FLASH_SPI_MOSI_PinSource, FLASH_SPI_AF);
	
		GPIO_InitStructure.GPIO_Pin = FLASH_SPI_SCK_PIN | FLASH_SPI_MISO_PIN | FLASH_SPI_MOSI_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  //PB13/14/15����������� 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
		GPIO_Init(FLASH_SPI_GPIO_PORT, &GPIO_InitStructure);//��ʼ��GPIOB
	
		GPIO_SetBits(FLASH_SPI_GPIO_PORT,FLASH_SPI_SCK_PIN | FLASH_SPI_MISO_PIN | FLASH_SPI_MOSI_PIN);  //PB13/14/15����
		
		GPIO_InitStructure.GPIO_Pin = FLASH_CS_PIN;  // PB12 ���� 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(FLASH_SPI_GPIO_PORT, &GPIO_InitStructure);
	 
		SPI_FLASH_CS_HIGH();				//SPI FLASH��ѡ��
		
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ2
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
		SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
		SPI_Init(FLASH_SPI, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	 
		SPI_Cmd(FLASH_SPI, ENABLE); //ʹ��SPI����
		
		SPI2_ReadWriteByte(0xff);//��������		 
	 

}   

//��ȡW25QXX��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
u8 W25QXX_ReadSR(void)   
{  
	u8 byte=0;   
	SPI_FLASH_CS_LOW();                            //ʹ������   
	SPI2_ReadWriteByte(W25X_ReadStatusReg); //���Ͷ�ȡ״̬�Ĵ�������    
	byte=SPI2_ReadWriteByte(0Xff);          //��ȡһ���ֽ�  
	SPI_FLASH_CS_HIGH();                            //ȡ��Ƭѡ     
	return byte;   
} 
//дW25QXX״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void W25QXX_Write_SR(u8 sr)   
{   
	SPI_FLASH_CS_LOW();                     //ʹ������   
	SPI2_ReadWriteByte(W25X_WriteStatusReg);//����дȡ״̬�Ĵ�������    
	SPI2_ReadWriteByte(sr);               	//д��һ���ֽ�  
	SPI_FLASH_CS_HIGH();                    //ȡ��Ƭѡ     	      
}   
//W25QXXдʹ��	
//��WEL��λ   
void W25QXX_Write_Enable(void)   
{
		SPI_FLASH_CS_LOW();                         	//ʹ������   
    SPI2_ReadWriteByte(W25X_WriteEnable); 	//����дʹ��  
		SPI_FLASH_CS_HIGH();                          	//ȡ��Ƭѡ     	      
} 
//W25QXXд��ֹ	
//��WEL����  
void W25QXX_Write_Disable(void)   
{  
		SPI_FLASH_CS_LOW();                            //ʹ������   
    SPI2_ReadWriteByte(W25X_WriteDisable);  //����д��ָֹ��    
		SPI_FLASH_CS_HIGH();                           //ȡ��Ƭѡ     	      
} 	

//�ȴ�����
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01);  		// �ȴ�BUSYλ���
}  

//��ȡоƬID
//����ֵ����:				   
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128 	  
u16 W25QXX_ReadID(void)
{
	u16 Temp = 0;	  
	SPI_FLASH_CS_LOW();				    
	SPI2_ReadWriteByte(0x90);//���Ͷ�ȡID����	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	 			   
	Temp|=SPI2_ReadWriteByte(0xFF)<<8;  
	Temp|=SPI2_ReadWriteByte(0xFF);	 
	SPI_FLASH_CS_HIGH();				    
	return Temp;
}   		    
//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
		u16 i;   										    
		SPI_FLASH_CS_LOW();                            	//ʹ������   
    SPI2_ReadWriteByte(W25X_ReadData);         	//���Ͷ�ȡ����   
    SPI2_ReadWriteByte((u8)((ReadAddr)>>16));  	//����24bit��ַ    
    SPI2_ReadWriteByte((u8)((ReadAddr)>>8));   
    SPI2_ReadWriteByte((u8)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
		{ 
        pBuffer[i]=SPI2_ReadWriteByte(0XFF);   	//ѭ������  
    }
		SPI_FLASH_CS_HIGH();  				    	      
}  
//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 
void W25QXX_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
		u16 i;  
    W25QXX_Write_Enable();                  	//SET WEL 
		SPI_FLASH_CS_LOW();                          	//ʹ������   
    SPI2_ReadWriteByte(W25X_PageProgram);      	//����дҳ����   
    SPI2_ReadWriteByte((u8)((WriteAddr)>>16)); 	//����24bit��ַ    
    SPI2_ReadWriteByte((u8)((WriteAddr)>>8));   
    SPI2_ReadWriteByte((u8)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)
			SPI2_ReadWriteByte(pBuffer[i]);//ѭ��д��  
		SPI_FLASH_CS_HIGH();                           	//ȡ��Ƭѡ 
		W25QXX_Wait_Busy();					   		//�ȴ�д�����
} 

//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)
		pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)
			break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;	

			NumByteToWrite -= pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite > 256)
					pageremain = 256; //һ�ο���д��256���ֽ�
			else
					pageremain = NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
} 
//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)						
//NumByteToWrite:Ҫд����ֽ���(���16384)   
u8 W25QXX_BUFFER[4096];		 
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
		u32 secpos;
		u16 secoff;
		u16 secremain;	   
		u16 i;    
		u8 * W25QXX_BUF;	  
   	W25QXX_BUF=W25QXX_BUFFER;	     
		secpos=WriteAddr/4096;//������ַ  
		secoff=WriteAddr%4096;//�������ڵ�ƫ��
		secremain=4096-secoff;//����ʣ��ռ��С   
		//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
		if(NumByteToWrite<=secremain)
			secremain=NumByteToWrite;//������4096���ֽ�
		while(1) 
		{	
			W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//������������������
			for(i=0;i<secremain;i++)//У������
			{
				if(W25QXX_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
			}
			if(i < secremain)//��Ҫ����
			{
				W25QXX_Erase_Sector(secpos);		//�����������
				for(i=0;i<secremain;i++)	   		//����
				{
					W25QXX_BUF[i+secoff]=pBuffer[i];	  
				}
				W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//д����������  
			}
			else 
				W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
			if(NumByteToWrite == secremain)
						break;//д�������
			else//д��δ����
			{
				secpos++;//������ַ��1
				secoff=0;//ƫ��λ��Ϊ0 	 

				pBuffer += secremain;  				//ָ��ƫ��
				WriteAddr += secremain;				//д��ַƫ��	   
				NumByteToWrite -= secremain;			//�ֽ����ݼ�
				if(NumByteToWrite>4096)
					secremain=4096;//��һ����������д����
				else 
					secremain = NumByteToWrite;		//��һ����������д����
			}	 
		}	 
}
//��������оƬ		  
//�ȴ�ʱ�䳬��...
void W25QXX_Erase_Chip(void)   
{                                   
    W25QXX_Write_Enable();                 	 	//SET WEL 
    W25QXX_Wait_Busy();   
  	SPI_FLASH_CS_LOW();                            	//ʹ������   
    SPI2_ReadWriteByte(W25X_ChipErase);        	//����Ƭ��������  
		SPI_FLASH_CS_HIGH();                            	//ȡ��Ƭѡ     	      
		W25QXX_Wait_Busy();   				   		//�ȴ�оƬ��������
}   
//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ��ɽ��������ʱ��:150ms
void W25QXX_Erase_Sector(u32 Dst_Addr)   
{  
	//����falsh�������,������   
// 	printf("fe:%x\r\n",Dst_Addr);	  
		Dst_Addr*=4096;
    W25QXX_Write_Enable();                  	//SET WEL 	 
    W25QXX_Wait_Busy();   
  	SPI_FLASH_CS_LOW();                            	//ʹ������   
    SPI2_ReadWriteByte(W25X_SectorErase);      	//������������ָ�� 
    SPI2_ReadWriteByte((u8)((Dst_Addr)>>16));  	//����24bit��ַ    
    SPI2_ReadWriteByte((u8)((Dst_Addr)>>8));   
    SPI2_ReadWriteByte((u8)Dst_Addr);  
		SPI_FLASH_CS_HIGH();                            	//ȡ��Ƭѡ     	      
    W25QXX_Wait_Busy();   				   		//�ȴ��������
}  

//�������ģʽ
void W25QXX_PowerDown(void)   
{ 
  	SPI_FLASH_CS_LOW();                           	 	//ʹ������   
    SPI2_ReadWriteByte(W25X_PowerDown);        //���͵�������  
		SPI_FLASH_CS_HIGH();                            	//ȡ��Ƭѡ     	      
//    delay_us(3);                               //�ȴ�TPD  
}   
//����
void W25QXX_WAKEUP(void)   
{  
  	SPI_FLASH_CS_LOW();                            	//ʹ������   
    SPI2_ReadWriteByte(W25X_ReleasePowerDown);	//  send W25X_PowerDown command 0xAB    
		SPI_FLASH_CS_HIGH();                           	//ȡ��Ƭѡ     	      
//    delay_us(3);                            	//�ȴ�TRES1
}   

/***************************** �����޿Ƽ� www.apollorobot.com (END OF FILE) *********************************/

/*
*********************************************************************************************************
*
*	ģ������ : ft5x0x���ݴ���оƬ��������
*	�ļ����� : bsp_ts_ft5x06.c
*	��    �� : V1.0
*	˵    �� : FocalTech ft5x0x����оƬ��������4.3����480*272.
*	�޸ļ�¼ :
*		�汾��   ����        ����     ˵��
*		V1.0    2015-10-30  armfly   ��ʽ����
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "GUI.h"

#if 1
	#define printf_dbg printf
#else
	#define printf_dbg(...)
#endif

FT5X06_T g_tFT5X06;

/* ����GPIO�˿� */
#define RCC_TP_INT	RCC_AHB1Periph_GPIOI
#define PORT_TP_INT	GPIOI
#define PIN_TP_INT	GPIO_Pin_3

static void FT5X06_ReadReg(uint16_t _usRegAddr, uint8_t *_pRegBuf, uint8_t _ucLen);

/*
*********************************************************************************************************
*	�� �� ��: FT5X06_InitHard
*	����˵��: ���ô���оƬ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void FT5X06_InitHard(void)
{	
	/* ���ô������ж�����Ϊ���� */
	{
		GPIO_InitTypeDef GPIO_InitStructure;

		/* ��1������GPIOʱ�� */
		RCC_AHB1PeriphClockCmd(RCC_TP_INT, ENABLE);

		/* ��2�����������еİ���GPIOΪ��������ģʽ(ʵ����CPU��λ���������״̬) */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;		/* ��Ϊ����� */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* �������������� */
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/* IO������ٶ� */

		GPIO_InitStructure.GPIO_Pin = PIN_TP_INT;
		GPIO_Init(PORT_TP_INT, &GPIO_InitStructure);		
	}
	
#if 0	/* ��ӡȫ���ļĴ������� for debug */
	{
		uint8_t i;
		uint8_t reg_value;
		
		printf_dbg("[FTS] Touch Chip\r\n");
		
		bsp_DelayMS(10);
		
		for (i = 0; i < 255; i++)
		{
			FT5X06_ReadReg(i, &reg_value, 1);
			printf_dbg(" 0x%02X = 0x%02X\r\n", i, reg_value);
		}
	}
#endif	
	
#if 0
	{
		uint8_t reg_addr;
		uint8_t reg_value;
		
		//get some register information
		reg_addr = FT5X06_REG_FW_VER;
		FT5X06_ReadReg(reg_addr, &reg_value, 1);
		printf_dbg("[FTS] Firmware version = 0x%x\r\n", reg_value);
		
		reg_addr = FT5X06_REG_POINT_RATE;
		FT5X06_ReadReg(reg_addr, &reg_value, 1);
		printf_dbg("[FTS] report rate is %dHz.\r\n", reg_value * 10);
		
		reg_addr = FT5X06_REG_THGROUP;
		FT5X06_ReadReg(reg_addr, &reg_value, 1);
		printf_dbg("[FTS] touch threshold is %d.\r\n", reg_value * 4);
	}
#endif	
	
	g_tFT5X06.TimerCount = 0;
	g_tFT5X06.Enable = 1;
}

/*
*********************************************************************************************************
*	�� �� ��: FT5X06_PenInt
*	����˵��: �жϴ�������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t FT5X06_PenInt(void)
{
	if ((PORT_TP_INT->IDR & PIN_TP_INT) == 0)
	{
		return 1;
	}
	return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: FT5X06_ReadReg
*	����˵��: ��1���������Ķ���Ĵ���
*	��    ��: _usRegAddr : �Ĵ�����ַ
*			  _pRegBuf : �Ĵ������ݻ�����
*			 _ucLen : ���ݳ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void FT5X06_ReadReg(uint16_t _usRegAddr, uint8_t *_pRegBuf, uint8_t _ucLen)
{
	uint8_t i;

    i2c_Start();					/* ���߿�ʼ�ź� */

    i2c_SendByte(FT5X06_I2C_ADDR);	/* �����豸��ַ+д�ź� */
	i2c_WaitAck();

    //i2c_SendByte(_usRegAddr >> 8);	/* ��ַ��8λ */
	//i2c_WaitAck();

    i2c_SendByte(_usRegAddr);		/* ��ַ��8λ */
	i2c_WaitAck();

	i2c_Start();
    i2c_SendByte(FT5X06_I2C_ADDR + 0x01);	/* �����豸��ַ+���ź� */
	i2c_WaitAck();

	for (i = 0; i < _ucLen - 1; i++)
	{
	    _pRegBuf[i] = i2c_ReadByte();	/* ���Ĵ������� */
		i2c_Ack();
	}

	/* ���һ������ */
	 _pRegBuf[i] = i2c_ReadByte();		/* ���Ĵ������� */
	i2c_NAck();

    i2c_Stop();							/* ����ֹͣ�ź� */
}

/*
*********************************************************************************************************
*	�� �� ��: FT5X06_Timer1ms
*	����˵��: ÿ��1ms����1��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void FT5X06_Timer1ms(void)
{
	g_tFT5X06.TimerCount++;
}

/*
*********************************************************************************************************
*	�� �� ��: FT5X06_OnePiontScan
*	����˵��: FT5X06_OnePiontScan��������ȡ�˵��ݴ�������һ�δ�������bsp_ts_ft5x06.h�ļ��������ò���
*             #define FT5X06_TOUCH_POINTS   1
*             ר������emWin��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
extern GUI_PID_STATE State;
void FT5X06_OnePiontScan(void)
{
	uint8_t buf[CFG_POINT_READ_BUF];
	uint8_t i;
	static uint8_t s_tp_down = 0;
	uint16_t x, y;

	if (g_tFT5X06.Enable == 0)
	{
		return;
	}
	
	if (FT5X06_PenInt() == 0)
	{
		return;
	}
	
	FT5X06_ReadReg(2, buf, 1);	
    /* �ж��Ƿ��д������� */	
	if ((buf[0] & 0x07) == 0)
	{
		if (s_tp_down == 1)
		{
			/* State.x��State.y����ֵ������£�State��ȫ�ֱ���������ľ������һ�ε���ֵ */
			s_tp_down = 0;
			State.Pressed = 0;
			GUI_PID_StoreState(&State);	/* �ͷ� */
		}
		return;
	}
	
	/* �д�������ȡ���������ݣ������ȡ��һ�� */
	FT5X06_ReadReg(0, buf, CFG_POINT_READ_BUF);
	for (i = 0; i < FT5X06_TOUCH_POINTS; i++)
	{
		uint8_t pointid;
		
		pointid = (buf[5 + 6*i]) >> 4;
		if (pointid >= 0x0f)
		{
			break;
		}
		else
		{
        	g_tFT5X06.X[i] = (int16_t)(buf[3 + 6*i] & 0x0F)<<8 | (int16_t)buf[4 + 6*i];
        	g_tFT5X06.Y[i] = (int16_t)(buf[5 + 6*i] & 0x0F)<<8 | (int16_t)buf[6 + 6*i];
        	g_tFT5X06.Event[i] = buf[0x3 + 6*i] >> 6;
        	g_tFT5X06.id[i] = (buf[5 + 6*i])>>4;
    	}
    }

	/* ��ⰴ�� */
	{
		if ((g_tFT5X06.ChipID == 0x55)||(g_tFT5X06.ChipID == 0xa3))       /* 4.3�� 480 * 272 */
		{
			x = g_tFT5X06.Y[0];
			y = g_tFT5X06.X[0];	
			
			/* �ж�ֵ�� */
			if (x > 479)
			{
				x = 479;
			}
			
			if (y > 271)
			{
				y = 271;
			}			
		}
		else if (g_tFT5X06.ChipID == 0x0A)	/* 5.0�� 800 * 480 */
		{
			x = g_tFT5X06.X[0];
			y = g_tFT5X06.Y[0];	
			
			/* �ж�ֵ�� */
			if (x > 799)
			{
				x = 799;
			}			
			if (y > 479)
			{
				y = 479;
			}			
		}
		else	/* id == 0x06 ��ʾ7���������FTоƬ�� */
		{
			x = g_tFT5X06.X[0];
			y = g_tFT5X06.Y[0];	
			
			/* �ж�ֵ�� */
			if (x > 799)
			{
				x = 799;
			}			
			if (y > 479)
			{
				y = 479;
			}			
		}
	}
	
	if (s_tp_down == 0)
	{
		s_tp_down = 1;
		State.x = x;
		State.y = y;
		State.Pressed = 1;
		GUI_PID_StoreState(&State);	
	}
	else
	{
		State.x = x;
		State.y = y;
		State.Pressed = 1;
		GUI_PID_StoreState(&State);	
	}

#if 0
	for (i = 0; i < CFG_POINT_READ_BUF; i++)
	{
		printf("%02X ", buf[i]);
	}
	printf("\r\n");
#endif

#if 0  /* ��ӡ5����������� */	
	printf("(%5d,%5d,%3d,%3d) ",  g_tFT5X06.X[0], g_tFT5X06.Y[0], g_tFT5X06.Event[0],  g_tFT5X06.id[0]);
	printf("(%5d,%5d,%3d,%3d) ",  g_tFT5X06.X[1], g_tFT5X06.Y[1], g_tFT5X06.Event[1],  g_tFT5X06.id[1]);
	printf("(%5d,%5d,%3d,%3d) ",  g_tFT5X06.X[2], g_tFT5X06.Y[2], g_tFT5X06.Event[2],  g_tFT5X06.id[2]);
	printf("(%5d,%5d,%3d,%3d) ",  g_tFT5X06.X[3], g_tFT5X06.Y[3], g_tFT5X06.Event[3],  g_tFT5X06.id[3]);
	printf("(%5d,%5d,%3d,%3d) ",  g_tFT5X06.X[4], g_tFT5X06.Y[4], g_tFT5X06.Event[4],  g_tFT5X06.id[4]);
	printf("\r\n");
#endif	
}

/*
*********************************************************************************************************
*	�� �� ��: FT5X06_Scan
*	����˵��: ��ȡ�������ݡ���ȡȫ�������ݣ���Ҫ 720us���ҡ����������� bsp_Idle()��ִ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void FT5X06_Scan(void)
{
	uint8_t buf[CFG_POINT_READ_BUF];
	uint8_t i;
	static uint8_t s_tp_down = 0;
	uint16_t x, y;
	static uint16_t x_save, y_save;

	if (g_tFT5X06.Enable == 0)
	{
		return;
	}
	
	/* 10ms ִ��һ�� */
	if (g_tFT5X06.TimerCount < 10)
	{
		return;
	}

	g_tFT5X06.TimerCount = 0;
	
	if (FT5X06_PenInt() == 0)
	{
		return;
	}
	
	FT5X06_ReadReg(2, buf, 1);		
	if ((buf[0] & 0x07) == 0)
	{
		if (s_tp_down == 1)
		{
			s_tp_down = 0;
			TOUCH_PutKey(TOUCH_RELEASE, x_save, y_save);	/* �ͷ� */
		}
		return;
	}
	
	/* �д�������ȡ���������� */
	FT5X06_ReadReg(0, buf, CFG_POINT_READ_BUF);
	
	g_tFT5X06.Count = buf[2] & 0x07; 
	if (g_tFT5X06.Count > FT5X06_TOUCH_POINTS)
	{
    	g_tFT5X06.Count = FT5X06_TOUCH_POINTS;
	}
	
	g_tFT5X06.Count = 0;
	for (i = 0; i < FT5X06_TOUCH_POINTS; i++)
	{
		uint8_t pointid;
		
		pointid = (buf[5 + 6*i]) >> 4;
		if (pointid >= 0x0f)
		{
			break;
		}
		else
		{
			g_tFT5X06.Count++;
        	g_tFT5X06.X[i] = (int16_t)(buf[3 + 6*i] & 0x0F)<<8 | (int16_t)buf[4 + 6*i];
        	g_tFT5X06.Y[i] = (int16_t)(buf[5 + 6*i] & 0x0F)<<8 | (int16_t)buf[6 + 6*i];
        	g_tFT5X06.Event[i] = buf[0x3 + 6*i] >> 6;
        	g_tFT5X06.id[i] = (buf[5 + 6*i])>>4;
    	}
    }

	/* ��ⰴ�� */
	{
		if (g_tFT5X06.ChipID == 0x55)       /* 4.3�� 480 * 272 */
		{
			x = g_tFT5X06.Y[0];
			y = g_tFT5X06.X[0];	
			
			/* �ж�ֵ�� */
			if (x > 479)
			{
				x = 479;
			}
			
			if (y > 271)
			{
				y = 271;
			}			
		}
		else if (g_tFT5X06.ChipID == 0x0A)	/* 5.0�� 800 * 480 */
		{
			x = g_tFT5X06.X[0];
			y = g_tFT5X06.Y[0];	
			
			/* �ж�ֵ�� */
			if (x > 799)
			{
				x = 799;
			}			
			if (y > 479)
			{
				y = 479;
			}			
		}
		else	/* id == 0x06 ��ʾ7���������FTоƬ�� */
		{
			x = g_tFT5X06.X[0];
			y = g_tFT5X06.Y[0];	
			
			/* �ж�ֵ�� */
			if (x > 799)
			{
				x = 799;
			}			
			if (y > 479)
			{
				y = 479;
			}			
		}
	}
	
	if (s_tp_down == 0)
	{
		s_tp_down = 1;
		
		TOUCH_PutKey(TOUCH_DOWN, x, y);
	}
	else
	{
		TOUCH_PutKey(TOUCH_MOVE, x, y);
	}
	x_save = x;	/* �������꣬�����ͷ��¼� */
	y_save = y;

#if 0
	for (i = 0; i < CFG_POINT_READ_BUF; i++)
	{
		printf("%02X ", buf[i]);
	}
	printf("\r\n");
#endif

#if 0  /* ��ӡ5����������� */	
	printf("(%5d,%5d,%3d,%3d) ",  g_tFT5X06.X[0], g_tFT5X06.Y[0], g_tFT5X06.Event[0],  g_tFT5X06.id[0]);
	printf("(%5d,%5d,%3d,%3d) ",  g_tFT5X06.X[1], g_tFT5X06.Y[1], g_tFT5X06.Event[1],  g_tFT5X06.id[1]);
	printf("(%5d,%5d,%3d,%3d) ",  g_tFT5X06.X[2], g_tFT5X06.Y[2], g_tFT5X06.Event[2],  g_tFT5X06.id[2]);
	printf("(%5d,%5d,%3d,%3d) ",  g_tFT5X06.X[3], g_tFT5X06.Y[3], g_tFT5X06.Event[3],  g_tFT5X06.id[3]);
	printf("(%5d,%5d,%3d,%3d) ",  g_tFT5X06.X[4], g_tFT5X06.Y[4], g_tFT5X06.Event[4],  g_tFT5X06.id[4]);
	printf("\r\n");
#endif	
}

/*
*********************************************************************************************************
*	�� �� ��: FT5X06_ReadID
*	����˵��: ��оƬID, ʶ����4.3��5.0��7.0�紥����
*			ʵ����:  4.3��id = 0x55    5.0��id = 0x0A  7.0��id = 0x06
*	��    ��: ��
*	�� �� ֵ: 1�ֽ�оƬID
*********************************************************************************************************
*/
uint8_t FT5X06_ReadID(void)
{
	uint8_t id;
	
	FT5X06_ReadReg(FTS_REG_CHIP_ID, &id, 1);
	g_tFT5X06.ChipID = id;		/* ����id */
	return id;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

/*
*********************************************************************************************************
*
*	ģ������ : LEDָʾ������ģ��
*	�ļ����� : bsp_led.c
*	��    �� : V1.0
*	˵    �� : ����LEDָʾ��
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2016-03-30 ����  ��ʽ����
*
*	Copyright (C), 2015-2020, �����޿Ƽ� www.apollorobot.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/*
	�ó��������ڰ�����STM32-X4��STM32-F4������

	�����������Ӳ�������޸�GPIO����� IsKeyDown1 - IsKeyDown8 ����

	����û���LEDָʾ�Ƹ���С��4�������Խ������LEDȫ������Ϊ�͵�1��LEDһ��������Ӱ�������
*/

#ifdef STM32_X3		/* ������ STM32-X4 ������ */
	/*
		������STM32-X4 LED���߷��䣺
			LED1       : PE2 (�͵�ƽ�������ߵ�ƽϨ��)
			LED2       : PE3 (�͵�ƽ�������ߵ�ƽϨ��)
			LED3       : PE4 (�͵�ƽ�������ߵ�ƽϨ��)
			LED4       : PE5 (�͵�ƽ�������ߵ�ƽϨ��)
	*/
	#define RCC_ALL_LED 	RCC_AHB1Periph_GPIOE	/* �����ڶ�Ӧ��RCCʱ�� */

	#define GPIO_PORT_LED1  GPIOE
	#define GPIO_PIN_LED1	GPIO_Pin_0

	#define GPIO_PORT_LED2  GPIOE
	#define GPIO_PIN_LED2	GPIO_Pin_1

	#define GPIO_PORT_LED3  GPIOE
	#define GPIO_PIN_LED3	GPIO_Pin_2

	#define GPIO_PORT_LED4  GPIOE
	#define GPIO_PIN_LED4	GPIO_Pin_3

#else	/* STM32_F4 */
	/*
		������STM32-V5 ������LED���߷��䣺
			LD1     : PI10/TP_NCS          (�͵�ƽ�������ߵ�ƽϨ��)
			LD2     : PF7/NRF24L01_CSN     (�͵�ƽ�������ߵ�ƽϨ��)
			LD3     : PF8/SF_CS            (�͵�ƽ�������ߵ�ƽϨ��)
			LD4     : PC2/NRF905_CSN/VS1053_XCS  (�͵�ƽ�������ߵ�ƽϨ��)
	*/

	/* �����ڶ�Ӧ��RCCʱ�� */
	#define RCC_ALL_LED 	RCC_AHB1Periph_GPIOE

	#define GPIO_PORT_LED1  GPIOE
	#define GPIO_PIN_LED1	GPIO_Pin_0

	#define GPIO_PORT_LED2  GPIOE
	#define GPIO_PIN_LED2	GPIO_Pin_1

	#define GPIO_PORT_LED3  GPIOE
	#define GPIO_PIN_LED3	GPIO_Pin_2

	#define GPIO_PORT_LED4  GPIOE
	#define GPIO_PIN_LED4	GPIO_Pin_3
#endif

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitLed
*	����˵��: ����LEDָʾ����ص�GPIO,  �ú����� bsp_Init() ���á�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitLed(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ��GPIOʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_ALL_LED, ENABLE);

	/*
		�������е�LEDָʾ��GPIOΪ�������ģʽ
		���ڽ�GPIO����Ϊ���ʱ��GPIO����Ĵ�����ֵȱʡ��0����˻�����LED����.
		�����Ҳ�ϣ���ģ�����ڸı�GPIOΪ���ǰ���ȹر�LEDָʾ��
	*/
	bsp_LedOff(1);
	bsp_LedOff(2);
	bsp_LedOff(3);
	bsp_LedOff(4);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* ��Ϊ����� */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* ���������費ʹ�� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED1;
	GPIO_Init(GPIO_PORT_LED1, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED2;
	GPIO_Init(GPIO_PORT_LED2, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED3;
	GPIO_Init(GPIO_PORT_LED3, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED4;
	GPIO_Init(GPIO_PORT_LED4, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_LedOn
*	����˵��: ����ָ����LEDָʾ�ơ�
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 4
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_LedOn(uint8_t _no)
{
	_no--;

	if (_no == 0)
	{
		GPIO_PORT_LED1->BSRRH = GPIO_PIN_LED1;
	}
	else if (_no == 1)
	{
		GPIO_PORT_LED2->BSRRH = GPIO_PIN_LED2;
	}
	else if (_no == 2)
	{
		GPIO_PORT_LED3->BSRRH = GPIO_PIN_LED3;
	}
	else if (_no == 3)
	{
		GPIO_PORT_LED4->BSRRH = GPIO_PIN_LED4;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_LedOff
*	����˵��: Ϩ��ָ����LEDָʾ�ơ�
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 4
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_LedOff(uint8_t _no)
{
	_no--;

	if (_no == 0)
	{
		GPIO_PORT_LED1->BSRRL = GPIO_PIN_LED1;
	}
	else if (_no == 1)
	{
		GPIO_PORT_LED2->BSRRL = GPIO_PIN_LED2;
	}
	else if (_no == 2)
	{
		GPIO_PORT_LED3->BSRRL = GPIO_PIN_LED3;
	}
	else if (_no == 3)
	{
		GPIO_PORT_LED4->BSRRL = GPIO_PIN_LED4;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_LedToggle
*	����˵��: ��תָ����LEDָʾ�ơ�
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 4
*	�� �� ֵ: ��������
*********************************************************************************************************
*/
void bsp_LedToggle(uint8_t _no)
{
	if (_no == 1)
	{
		GPIO_PORT_LED1->ODR ^= GPIO_PIN_LED1;
	}
	else if (_no == 2)
	{
		GPIO_PORT_LED2->ODR ^= GPIO_PIN_LED2;
	}
	else if (_no == 3)
	{
		GPIO_PORT_LED3->ODR ^= GPIO_PIN_LED3;
	}
	else if (_no == 4)
	{
		GPIO_PORT_LED4->ODR ^= GPIO_PIN_LED4;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_IsLedOn
*	����˵��: �ж�LEDָʾ���Ƿ��Ѿ�������
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 4
*	�� �� ֵ: 1��ʾ�Ѿ�������0��ʾδ����
*********************************************************************************************************
*/
uint8_t bsp_IsLedOn(uint8_t _no)
{
	if (_no == 1)
	{
		if ((GPIO_PORT_LED1->ODR & GPIO_PIN_LED1) == 0)
		{
			return 1;
		}
		return 0;
	}
	else if (_no == 2)
	{
		if ((GPIO_PORT_LED2->ODR & GPIO_PIN_LED2) == 0)
		{
			return 1;
		}
		return 0;
	}
	else if (_no == 3)
	{
		if ((GPIO_PORT_LED3->ODR & GPIO_PIN_LED3) == 0)
		{
			return 1;
		}
		return 0;
	}
	else if (_no == 4)
	{
		if ((GPIO_PORT_LED4->ODR & GPIO_PIN_LED4) == 0)
		{
			return 1;
		}
		return 0;
	}

	return 0;
}

/***************************** �����޿Ƽ� www.apollorobot.com (END OF FILE) *********************************/

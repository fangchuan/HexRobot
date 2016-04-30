/*
*********************************************************************************************************
*	                                  
*	ģ������ : ������ģ�顣
*	�ļ����� : main.c
*	��    �� : V4.0
*	˵    �� : ��ʵ����Ҫʵ��uCOS-III+STemWin+FatFS�ۺ�ʵ��
*              ʵ��Ŀ�ģ�
*                1. ѧϰuCOS-III+STemWin+FatFS���ۺ�ʹ�á�
*              ʵ�����ݣ�
*                1. ���°���K1����ͨ�����ڴ�ӡ����ִ�����
*                 ���ȼ� ʹ��ջ ʣ��ջ �ٷֱ� ������   ������
*                  Prio   Used  Free   Per    CPU     Taskname
*                   59     42    982   04%    0.00%   App Task GUIRefresh
*                   60    366    658   35%    0.73%   App Task GUI
*                    5    134    890   13%    0.03%   App Task UserIF
*                    4     42    982   04%    0.00%   App Task COM
*                    3     48    976   04%    0.00%   App Task Update
*                    2     44    980   04%    0.66%   App Task Start
*                   62     31     97   24%    0.00%   uC/OS-III Timer Task
*                   62     31     97   24%    0.26%   uC/OS-III Stat Task
*                    1     31     97   24%    1.00%   uC/OS-III Tick Task
*                   63     19    109   14%   97.28%   uC/OS-III Idle Task
*                    �����������ʹ��SecureCRT��V5���������д�������鿴��ӡ��Ϣ��
*                    App Task Start����     ���������ʹ������
*                    App Task Update����    ��ʵ�ֽ�ͼ���ܣ���ͼƬ��BMP��ʽ���浽SD����
*                    App Task COM����       ��LED��˸
*                    App Task UserIF����    ����������Ϣ
*                    App Task GUIRefresh����δʹ��
*                    App Task GUI����       ��GUI����
*                2. K2�������£�ʵ�ֽ�ͼ���ܣ���ͼƬ��BMP��ʽ���浽SD���С�
*                3. emWin���ܵ�ʵ����MainTask.C�ļ����档
*              ע�����
*                1. ��ʵ���Ƽ�ʹ�ô������SecureCRT��Ҫ�����ڴ�ӡЧ�������롣�������
*                   V5��������������С�
*                2. ��ؽ��༭��������������TAB����Ϊ4���Ķ����ļ���Ҫ��������ʾ�����롣
*                3. ʹ��3.5��ili9488��ʱ���������LED1��LED3����ΪLED1�ʹ���оƬTSC2046
*                   ��Ƭѡ���ã��ʲ���ʹ�á���LED3��������SPI Flash��Ƭѡ������TSC2046��SPI Flash
*                   �ǹ���SPI�ģ������������LED3����ֹͬʱ����SPI Flash��TSC2046���Ӷ���ɳ�ͻ��
*
*	�޸ļ�¼ :
*		�汾��  ����         ����          ˵��
*		V1.0    2013-03-26  Eric2013     ST�̼���V1.0.2�汾,�׷�
*       V2.0    2014-02-23  Eric2013     1. ST�̼���V1.3.0�汾��
*                                        2. emWin��5.16������5.22.
*                                        3. ����UCOS��MDK4.7�������ϰ汾���и�����ջ�ͳ�ջ�����⣬���Խ�F4��
*                                           FPU���ܹرգ����޸�UCOS��ֲ�����и��㲿�֣������߰汾Ҳ���������ˡ�
*                                        4. ����ST�ٷ���װ��STemWin_CM4�汾����ʹ�û��ĸ���ָ������ǽ�FPU
*                                           �ر��ˣ�����ֱ�����еĻ�����뵽Ӳ���쳣�����ڴ�����ʹ�õ���STemWin
*                                           ��CM3�汾��
*	    V3.0    2015-04-08  Eric2013     1. �����̼��⵽V1.5.0
*                                        2. ����BSP�弶֧�ְ� 
*                                        3. ����fatfs��0.11
*                                        4. ����STemWin��5.26
*                                        5. ����Ϊ�µ��ĵ㴥��У׼�㷨
*                                        6. ���7��800*480�ֱ��ʵ�����֧�֣����3.5��480*320��ILI9488֧�֡�
*                                        7. FPU�����Ѿ��õ��������������ʹ�ø����汾MDK��IAR���С�
*                                        8. ���±���uCOS-III������
*	    V4.0    2015-12-18  Eric2013     1. ����BSP�弶֧�ְ� 
*                                        2. ����STemWin��5.28
*                                        3. ���4.3���5�������֧�֡�
*
*      Copyright (C), 2016.3.25, �����޿Ƽ� www.apollorobot.com
*
*********************************************************************************************************
*/
#include  <includes.h>

/*
*********************************************************************************************************
*                                       �ļ��ڲ�ʹ�õ�ȫ�ֱ���
*********************************************************************************************************
*/                                                          
static  OS_TCB   AppTaskStartTCB;
static  CPU_STK  AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB   AppTaskNRFTCB;
static  CPU_STK  AppTaskNRFStk[APP_CFG_TASK_NRF_STK_SIZE];

static  OS_TCB   AppTaskCOMTCB;
static  CPU_STK  AppTaskCOMStk[APP_CFG_TASK_COM_STK_SIZE];

static  OS_TCB   AppTaskUserIFTCB;
static  CPU_STK  AppTaskUserIFStk[APP_CFG_TASK_USER_IF_STK_SIZE];

static  OS_TCB   AppTaskSensorTCB;
static  CPU_STK  AppTaskSensorStk[APP_CFG_TASK_SENSOR_STK_SIZE];

static  OS_TCB   AppTaskRobotTCB;
static  CPU_STK  AppTaskRobotStk[APP_CFG_TASK_ROBOT_STK_SIZE];


//static  OS_SEM     	SEM_NRFRX;	   //����NRF��������

static _nrf_pkt   nrf_receiver_buffer;  //NRF���ݰ�����
static _Order     _order = STOP;     //nrf��������
static uint8_t android_order[PROTOCOL_MAXLENGTH];//Android�������ݱ�
static _ControlMode  manual_mode ;
static uint32_t   timestamp_route_init ;//·����������ʱ���--44s

/*********************************************************************
*
*       Global data
*
**********************************************************************
*/

//��ʱ������
OS_TMR             Tmr_1s;

_Sensor sensor;//
/*
*********************************************************************************************************
*                                      ��������
*********************************************************************************************************
*/
static void   AppTaskStart          (void     *p_arg);
static void   AppTaskCreate         (void);
static void   AppTaskUserIF         (void     *p_arg);
//static void   AppTaskGUI            (void     *p_arg);
static void   AppTaskRobotControl   (void     *p_arg);
static void   AppTaskCOM			      (void 	  *p_arg);
static void   DispTaskInfo          (void);
static void   AppTaskNrfReceiver    (void);


/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: ��׼c������ڡ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int main(void)
{
    OS_ERR  err;                                         
    
	  
	/* ��ʼ��uC/OS-III �ں� */
    OSInit(&err);  


	/* ����һ����������Ҳ���������񣩡���������ᴴ�����е�Ӧ�ó������� */
	OSTaskCreate((OS_TCB       *)&AppTaskStartTCB,  /* ������ƿ��ַ */           
                 (CPU_CHAR     *)"App Task Start",  /* ������ */
                 (OS_TASK_PTR   )AppTaskStart,      /* ������������ַ */
                 (void         *)0,                 /* ���ݸ�����Ĳ��� */
                 (OS_PRIO       )APP_CFG_TASK_START_PRIO, /* �������ȼ� */
                 (CPU_STK      *)&AppTaskStartStk[0],     /* ��ջ����ַ */
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE / 10, /* ��ջ������������ʾ��10%��Ϊ����� */
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,  /* ��ջ�ռ��С */
                 (OS_MSG_QTY    )0,  /* ������֧�ֽ��ܵ������Ϣ�� */
                 (OS_TICK       )0,  /* ����ʱ��Ƭ */
                 (void         *)0,  /* ��ջ�ռ��С */  
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
		
				 /*  �������£�
					OS_TASK_OPT_STK_CHK      ʹ�ܼ������ջ��ͳ������ջ���õĺ�δ�õ�
					OS_TASK_OPT_STK_CLR      �ڴ�������ʱ����������ջ
					OS_TASK_OPT_SAVE_FP      ���CPU�и���Ĵ��������������л�ʱ���渡��Ĵ���������
				 */  
                 (OS_ERR       *)&err);

	/* ����������ϵͳ������Ȩ����uC/OS-III */
    OSStart(&err);                                             
    
    (void)&err;
    
    return (0);
}
/*
*********************************************************************************************************
*	�� �� ��: DispTaskInfo
*	����˵��: ��uCOS-III������Ϣͨ�����ڴ�ӡ����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispTaskInfo(void)
{
	OS_TCB      *p_tcb;	        /* ����һ��������ƿ�ָ��, TCB = TASK CONTROL BLOCK */
	float CPU;
	CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();
	p_tcb = OSTaskDbgListPtr;
	CPU_CRITICAL_EXIT();
	
	/* ��ӡ���� */
	printf("===============================================================\r\n");
	printf(" ���ȼ� ʹ��ջ ʣ��ջ �ٷֱ� ������   ������\r\n");
	printf("  Prio   Used  Free   Per    CPU     Taskname\r\n");

	/* ����������ƿ��б�(TCB list)����ӡ���е���������ȼ������� */
	while (p_tcb != (OS_TCB *)0) 
	{
		CPU = (float)p_tcb->CPUUsage / 100;
		printf("   %2d  %5d  %5d   %02d%%   %5.2f%%   %s\r\n", 
		p_tcb->Prio, 
		p_tcb->StkUsed, 
		p_tcb->StkFree, 
		(p_tcb->StkUsed * 100) / (p_tcb->StkUsed + p_tcb->StkFree),
		CPU,
		p_tcb->NamePtr);		
	 	
		CPU_CRITICAL_ENTER();
		p_tcb = p_tcb->DbgNextPtr;
		CPU_CRITICAL_EXIT();
	}
}
/*
*********************************************************************************************************
*	�� �� ��: _cbOfTmr1
*	����˵��: �����ʱ���Ļص�����
*           �����ڿ���44S��ʹ��UART1����Ϊ��44S��·����������ʱ��
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbOfTmr1(OS_TMR *p_tmr, void *p_arg)
{
    OS_ERR   err;
    (void)p_arg;
    
	   timestamp_route_init ++;
		 if(timestamp_route_init >= 44)
		 {
			  OSTmrDel(&Tmr_1s, &err); 
			  bsp_LedOff(LED0);
			  bsp_LedOff(LED1);
			  bsp_LedOff(LED2);
			  bsp_LedOff(LED3);
			  //����UART1
	      USART_Cmd(USART1, ENABLE);	
		 }
		 else
		 {
					bsp_LedToggle(LED0);
					bsp_LedToggle(LED1);
					bsp_LedToggle(LED2);
					bsp_LedToggle(LED3);
					
		 }
}
/*
*********************************************************************************************************
*	�� �� ��: AppTaskStart
*	����˵��: ����һ�����������ڶ�����ϵͳ�����󣬱����ʼ���δ������(��BSP_Init��ʵ��)
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*	�� �� ��: 2
*********************************************************************************************************
*/
static  void  AppTaskStart (void *p_arg)
{

	  OS_ERR      err;

   (void)p_arg;
	
	  CPU_Init();
   	bsp_Init();
	  BSP_Tick_Init();                      

#if OS_CFG_STAT_TASK_EN > 0u
     OSStatTaskCPUUsageInit(&err);   
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif
    
	  /* �������� */
    AppTaskCreate(); 
	    //������ʱ��  OS_CFG_TMR_TASK_RATE_HZ = 10HZ
    OSTmrCreate ((OS_TMR              *)&Tmr_1s,
                 (CPU_CHAR            *)"MyTimer 1s",
                 (OS_TICK              )10,                  //��һ����ʱ����Ϊ1s��
                 (OS_TICK              )10,                  //��ʱ������1s
                 (OS_OPT               )OS_OPT_TMR_PERIODIC,//ģʽ����Ϊ�ظ�ģʽ
                 (OS_TMR_CALLBACK_PTR  )_cbOfTmr1,          //�ص�����
                 (void                *)0,                  //��������Ϊ0
                 (OS_ERR              *)err);
								 
		//������ʱ��
    OSTmrStart((OS_TMR *)&Tmr_1s,(OS_ERR *)err);
								 
    /*Delete task*/
    OSTaskDel(&AppTaskStartTCB,&err);
}
/*
*********************************************************************************************************
*	�� �� ��: AppTaskCom
*	����˵��: LED��˸
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*	�� �� ��: 5
*********************************************************************************************************
*/
static void AppTaskCOM(void *p_arg)
{	
	OS_ERR  err;
	(void)p_arg;
	 
	 
	while(1)
	{
		  if(manual_mode == ANDROID_MODE)
			{
//		DispTaskInfo();
		
		  //����ɹ����յ�һ֡����,��ִ�����½�������
				if(!get_order_from_uart(android_order))
				{
					switch(android_order[PROTOCOL_PAYLOAD_BIT])
					{
					 case PROTOCOL_FORWARD:
						    _order = GO_FORWARD;
						 break;
					 case PROTOCOL_BACKWARD:
						    _order = GO_BACKWARD;
						 break;
					 case PROTOCOL_TURNLEFT:
						    _order = TURN_LEFT;
						 break;
					 case PROTOCOL_TURNRIGHT:
						 		_order = TURN_RIGHT;
						 break;
					 case PROTOCOL_STOP:
						    _order = STOP;
						 break;
					 case PROTOCOL_STAMP:
						    _order = STAMP;
						 break;
					 case PROTOCOL_HEAD_LEFT:
						    _order = TURN_HEAD_LEFT;
						 break;
					 case PROTOCOL_HEAD_RIGHT:
						    _order = TURN_HEAD_RIGHT;
						 break;
					 
					 
					 default:
						 break;
				 }
		   }
			 bsp_LedOff(LED0);
			 bsp_LedOff(LED2);
			 bsp_LedToggle(LED1);
		 }
			
			OSTimeDlyHMSM(0, 0, 0, 50,OS_OPT_TIME_HMSM_STRICT, &err);		
	} 						  	 	       											   
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskUserIF
*	����˵��: ��������Ҫ���ڵõ������ļ�ֵ��
*             1. ����K1������ӡ����ִ�����
*             2. ����K2�������н�ͼ������BMPͼƬ��SD����
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*	�� �� ��: 6
*********************************************************************************************************
*/
static void AppTaskUserIF(void *p_arg)
{
	uint8_t ucKeyCode;
  OS_ERR        err;
	uint8_t down_count =0;
	(void)p_arg;	               /* ������������� */

	while (1) 
	{   	
        
				ucKeyCode = GetKeyState();
				
				if (ucKeyCode == KEY_STATE_DOWN)
				{ //�ȴ���������
					while(GetKeyState() == KEY_STATE_DOWN);
					
					down_count ++;
					if(down_count == 1)
					{
						manual_mode = NRF_MODE;
					}
					if(down_count == 2)
					{
						manual_mode = ANDROID_MODE;
					}
					if(down_count == 3)
					{
						manual_mode = PC_MODE;
						down_count = 0;
					}
				}
	
        OSTimeDlyHMSM(0, 0, 0, 20,OS_OPT_TIME_HMSM_STRICT, &err);	   
	}
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskNrfReceiver
*	����˵��: ������NRF2401ͨѶ������
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
* �� �� ��: 5
*********************************************************************************************************
*/
static  void  AppTaskNrfReceiver (void)
{
    OS_ERR   err;
	  u8    status;
	
	  NRF_RX_Mode();
    while(1)
		{
//				OSSemPend(&SEM_NRFRX, 0, OS_OPT_PEND_BLOCKING,(CPU_TS*)0, &err);
			  if(manual_mode == NRF_MODE)
				{
					status = NRF_Rx_Dat((u8*)&nrf_receiver_buffer);
			    /*�жϽ���״̬*/
          if(status == RX_DR)

            {
                if(nrf_receiver_buffer.car_speed == NRF_ROCKER_FORWARD ||
                        nrf_receiver_buffer.key_value  == NRF_KEY_FORWARD ||
                        nrf_receiver_buffer.Y_angle < -NRF_EULER_THRE)
                    {
                        _order = GO_FORWARD;
										}
                if(nrf_receiver_buffer.car_speed == NRF_ROCKER_BACKWARD ||
                        nrf_receiver_buffer.key_value  == NRF_KEY_BACKWARD ||
                        nrf_receiver_buffer.Y_angle > NRF_EULER_THRE)
                    {
                        _order = GO_BACKWARD;
										}
                if(nrf_receiver_buffer.car_angle == NRF_ROCKER_LEFT ||
                        nrf_receiver_buffer.key_value  == NRF_KEY_LEFT ||
                        nrf_receiver_buffer.X_angle > NRF_EULER_THRE)
                    {
											  _order = TURN_LEFT;
										}
                if(nrf_receiver_buffer.car_angle == NRF_ROCKER_RIGHT ||
                        nrf_receiver_buffer.key_value == NRF_KEY_RIGHT ||
                        nrf_receiver_buffer.X_angle < -NRF_EULER_THRE)
                    {
                        _order = TURN_RIGHT;
										}
//								if(nrf_receiver_buffer.key_value == NRF_KEY_LEG1_UP)
//								   {
//									      _order = LIFT_LEG1;
//								   }
//								if(nrf_receiver_buffer.key_value == NRF_KEY_LEG3_UP)
//								   {
//									      _order = LIFT_LEG3;
//								   }
//								if(nrf_receiver_buffer.key_value == NRF_KEY_LEG4_UP)
//								   {
//									      _order = LIFT_LEG4;
//								   }
//								if(nrf_receiver_buffer.key_value == NRF_KEY_LEG6_UP)
//								   {
//									      _order = LIFT_LEG6;
//								   }
                if(nrf_receiver_buffer.car_speed == NRF_STOP && 
									 nrf_receiver_buffer.car_angle == NRF_STOP && 
								   nrf_receiver_buffer.key_value == NRF_STOP && 
								   fabs(nrf_receiver_buffer.X_angle) < NRF_EULER_SAFE && 
								   fabs(nrf_receiver_buffer.Y_angle) < NRF_EULER_SAFE )
                    {
                        _order = STOP;
										}

            }
						bsp_LedOff(LED1);
						bsp_LedOff(LED2);
						bsp_LedToggle(LED0);
//			  OSSemPost(&SEM_NRFRX, OS_OPT_POST_1, &err);
				}	
				
				OSTimeDlyHMSM(0, 0, 0, 50, OS_OPT_TIME_HMSM_STRICT, &err);	
	}				
}
/*
*********************************************************************************************************
*	�� �� ��: AppTaskSensorUpdate
*	����˵��: ��������������
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*	�� �� ��: 3
*********************************************************************************************************
*/
static void AppTaskSensorUpdate(void *p_arg)
{
	  OS_ERR    err;
	  (void )p_arg;
	
		while(1)
		{
			Ultrasnio_1_update();
			Ultrasnio_2_update();
			sensor.candela_1 = Get_adc(ANOLOG_Sensor_1);
			sensor.candela_2 = Get_adc(ANOLOG_Sensor_2);
			sensor.candela_3 = Get_adc(ANOLOG_Sensor_3);
			sensor.candela_4 = Get_adc(ANOLOG_Sensor_4);
			OSTimeDlyHMSM(0, 0, 0, 100, OS_OPT_TIME_HMSM_STRICT, &err);	
		}
}
/*
*********************************************************************************************************
*	�� �� ��: AppTaskRobotControl
*	����˵��: �������˶���������
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*	�� �� ��: 2
*********************************************************************************************************
*/
static void AppTaskRobotControl(void *p_arg)
{	
	OS_ERR   err;
	(void)p_arg;
	 
	/*վ������*/
	 Stand_Up();
   
	 while(1)
	{
		  switch(_order)									
			{
				case GO_FORWARD:
   		                 /*��ǰ��1��������180mm��ռ�ձ�50%*/
											 Go_Straight(STRAIGHT_FORWARD, DUTY_50, STRIDE, 1);
				break;
				case GO_BACKWARD:
					             /*�����1��������180mm��ռ�ձ�50%*/
		                   Go_Straight(STRAIGHT_BACKWARD, DUTY_50, STRIDE, 1);			
				break;
				case TURN_LEFT:
					             /*����ת20��*/
											 Turn_Around(DIRECTION_CC, DEF_TURN_ANGLE, 1);
				break;
				case TURN_RIGHT:
					             /*����ת20��*/
											 Turn_Around(DIRECTION_C, DEF_TURN_ANGLE, 1);
				break;
				case LIFT_LEG1:
											 Lift_down_legx(ID_LEG_1);
					break;
				case LIFT_LEG3:
											 Lift_down_legx(ID_LEG_3);
					break;
				case LIFT_LEG4:
											 Lift_down_legx(ID_LEG_4);
					break;
				case LIFT_LEG6:
											 Lift_down_legx(ID_LEG_6);
					break;
				case STOP:
					             /*ֹͣ*/
											 Stand_Up();
				  break;
				case STAMP:   /*���*/
					            Stamp(1);
					break;
				case TURN_HEAD_LEFT:
											Shake_Head(DIRECTION_CC);
					break;
				case TURN_HEAD_RIGHT:
											Shake_Head(DIRECTION_C);
					break;
				default:
					break;
			}
		  
	  	OSTimeDlyHMSM(0, 0, 0, 100, OS_OPT_TIME_HMSM_STRICT, &err);	
	} 						  	 	       											   
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void  AppTaskCreate (void)
{
			OS_ERR      err;
			

//			OSSemCreate(&SEM_NRFRX, "SEM_NRFReceiver", 0, &err);
			/**************����COM����*********************/
			OSTaskCreate((OS_TCB       *)&AppTaskCOMTCB,            
										 (CPU_CHAR     *)"App Task COM",
										 (OS_TASK_PTR   )AppTaskCOM, 
										 (void         *)0,
										 (OS_PRIO       )APP_CFG_TASK_COM_PRIO,
										 (CPU_STK      *)&AppTaskCOMStk[0],
										 (CPU_STK_SIZE  )APP_CFG_TASK_COM_STK_SIZE / 10,
										 (CPU_STK_SIZE  )APP_CFG_TASK_COM_STK_SIZE,
										 (OS_MSG_QTY    )0,
										 (OS_TICK       )5,
										 (void         *)0,
										 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
										 (OS_ERR       *)&err);
			
			/**************����USER IF����*********************/
			OSTaskCreate((OS_TCB       *)&AppTaskUserIFTCB,             
										 (CPU_CHAR     *)"App Task UserIF",
										 (OS_TASK_PTR   )AppTaskUserIF, 
										 (void         *)0,
										 (OS_PRIO       )APP_CFG_TASK_USER_IF_PRIO,
										 (CPU_STK      *)&AppTaskUserIFStk[0],
										 (CPU_STK_SIZE  )APP_CFG_TASK_USER_IF_STK_SIZE / 10,
										 (CPU_STK_SIZE  )APP_CFG_TASK_USER_IF_STK_SIZE,
										 (OS_MSG_QTY    )0,
										 (OS_TICK       )0,  //��Ϊ��NRF������ͬ���ȼ�����������������תʱ��ƬΪ5ms
										 (void         *)0,
										 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
										 (OS_ERR       *)&err);
										 
			/**************����NRF��������*********************/
			OSTaskCreate((OS_TCB       *)&AppTaskNRFTCB,             
										 (CPU_CHAR     *)"App Task NRFReceiver",
										 (OS_TASK_PTR   )AppTaskNrfReceiver, 
										 (void         *)0,
										 (OS_PRIO       )APP_CFG_TASK_NRF_PRIO,
										 (CPU_STK      *)&AppTaskNRFStk[0],
										 (CPU_STK_SIZE  )APP_CFG_TASK_NRF_STK_SIZE / 10,
										 (CPU_STK_SIZE  )APP_CFG_TASK_NRF_STK_SIZE,
										 (OS_MSG_QTY    )0,
										 (OS_TICK       )5,   //��Ϊ�봮��������ͬ���ȼ�����������������תʱ��ƬΪ5ms
										 (void         *)0,
										 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
										 (OS_ERR       *)&err);
										 
			/**************������������������*********************/
			OSTaskCreate((OS_TCB       *)&AppTaskSensorTCB,             
										 (CPU_CHAR     *)"App Task Sensor",
										 (OS_TASK_PTR   )AppTaskSensorUpdate, 
										 (void         *)0,
										 (OS_PRIO       )APP_CFG_TASK_SENSOR_PRIO,
										 (CPU_STK      *)&AppTaskSensorStk[0],
										 (CPU_STK_SIZE  )APP_CFG_TASK_SENSOR_STK_SIZE / 10,
										 (CPU_STK_SIZE  )APP_CFG_TASK_SENSOR_STK_SIZE,
										 (OS_MSG_QTY    )0,
										 (OS_TICK       )0,
										 (void         *)0,
										 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
										 (OS_ERR       *)&err);
 			/**************�����������˶���������*********************/
			OSTaskCreate((OS_TCB       *)&AppTaskRobotTCB,             
										 (CPU_CHAR     *)"App Task RobotControl",
										 (OS_TASK_PTR   )AppTaskRobotControl, 
										 (void         *)0,
										 (OS_PRIO       )APP_CFG_TASK_ROBOT_PRIO,
										 (CPU_STK      *)&AppTaskRobotStk[0],
										 (CPU_STK_SIZE  )APP_CFG_TASK_ROBOT_STK_SIZE / 10,
										 (CPU_STK_SIZE  )APP_CFG_TASK_ROBOT_STK_SIZE,
										 (OS_MSG_QTY    )0,
										 (OS_TICK       )0,
										 (void         *)0,
										 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
										 (OS_ERR       *)&err);
}


/***************************** �����޿Ƽ� www.apollorobot.com (END OF FILE) *********************************/

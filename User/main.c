/*
*********************************************************************************************************
*	                                  
*	模块名称 : 主程序模块。
*	文件名称 : main.c
*	版    本 : V4.0
*	说    明 : 本实验主要实现uCOS-III+STemWin+FatFS综合实验
*              实验目的：
*                1. 学习uCOS-III+STemWin+FatFS的综合使用。
*              实验内容：
*                1. 按下按键K1可以通过串口打印任务执行情况
*                 优先级 使用栈 剩余栈 百分比 利用率   任务名
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
*                    串口软件建议使用SecureCRT（V5光盘里面有此软件）查看打印信息。
*                    App Task Start任务     ：按键检测和触摸检测
*                    App Task Update任务    ：实现截图功能，将图片以BMP格式保存到SD卡中
*                    App Task COM任务       ：LED闪烁
*                    App Task UserIF任务    ：处理按键消息
*                    App Task GUIRefresh任务：未使用
*                    App Task GUI任务       ：GUI任务
*                2. K2按键按下，实现截图功能，将图片以BMP格式保存到SD卡中。
*                3. emWin功能的实现在MainTask.C文件里面。
*              注意事项：
*                1. 本实验推荐使用串口软件SecureCRT，要不串口打印效果不整齐。此软件在
*                   V5开发板光盘里面有。
*                2. 务必将编辑器的缩进参数和TAB设置为4来阅读本文件，要不代码显示不整齐。
*                3. 使用3.5寸ili9488屏时，切勿操作LED1和LED3，因为LED1和触摸芯片TSC2046
*                   的片选公用，故不可使用。而LED3的引脚是SPI Flash的片选，而且TSC2046和SPI Flash
*                   是共用SPI的，所以切勿操作LED3，防止同时访问SPI Flash和TSC2046，从而造成冲突。
*
*	修改记录 :
*		版本号  日期         作者          说明
*		V1.0    2013-03-26  Eric2013     ST固件库V1.0.2版本,首发
*       V2.0    2014-02-23  Eric2013     1. ST固件库V1.3.0版本。
*                                        2. emWin从5.16升级到5.22.
*                                        3. 鉴于UCOS在MDK4.7及其以上版本运行浮点入栈和出栈的问题，所以将F4的
*                                           FPU功能关闭，并修改UCOS移植代码中浮点部分，这样高版本也可以运行了。
*                                        4. 由于ST官方封装的STemWin_CM4版本中有使用汇编的浮点指令，而我们将FPU
*                                           关闭了，所以直接运行的话会进入到硬件异常，鉴于此我们使用的是STemWin
*                                           的CM3版本。
*	    V3.0    2015-04-08  Eric2013     1. 升级固件库到V1.5.0
*                                        2. 升级BSP板级支持包 
*                                        3. 升级fatfs到0.11
*                                        4. 升级STemWin到5.26
*                                        5. 更改为新的四点触摸校准算法
*                                        6. 添加7寸800*480分辨率电容屏支持，添加3.5寸480*320的ILI9488支持。
*                                        7. FPU问题已经得到完美解决，可以使用各个版本MDK和IAR运行。
*                                        8. 重新编排uCOS-III的任务。
*	    V4.0    2015-12-18  Eric2013     1. 升级BSP板级支持包 
*                                        2. 升级STemWin到5.28
*                                        3. 添加4.3寸和5寸电容屏支持。
*
*      Copyright (C), 2016.3.25, 阿波罗科技 www.apollorobot.com
*
*********************************************************************************************************
*/
#include  <includes.h>

/*
*********************************************************************************************************
*                                       文件内部使用的全局变量
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


//static  OS_SEM     	SEM_NRFRX;	   //用于NRF接收数据

static _nrf_pkt   nrf_receiver_buffer;  //NRF数据包缓存
static _Order     _order = STOP;     //nrf命令类型
static uint8_t android_order[PROTOCOL_MAXLENGTH];//Android命令数据报
static _ControlMode  manual_mode ;
static uint32_t   timestamp_route_init ;//路由器启动的时间戳--44s

/*********************************************************************
*
*       Global data
*
**********************************************************************
*/

//定时器变量
OS_TMR             Tmr_1s;

_Sensor sensor;//
/*
*********************************************************************************************************
*                                      函数声明
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
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int main(void)
{
    OS_ERR  err;                                         
    
	  
	/* 初始化uC/OS-III 内核 */
    OSInit(&err);  


	/* 创建一个启动任务（也就是主任务）。启动任务会创建所有的应用程序任务 */
	OSTaskCreate((OS_TCB       *)&AppTaskStartTCB,  /* 任务控制块地址 */           
                 (CPU_CHAR     *)"App Task Start",  /* 任务名 */
                 (OS_TASK_PTR   )AppTaskStart,      /* 启动任务函数地址 */
                 (void         *)0,                 /* 传递给任务的参数 */
                 (OS_PRIO       )APP_CFG_TASK_START_PRIO, /* 任务优先级 */
                 (CPU_STK      *)&AppTaskStartStk[0],     /* 堆栈基地址 */
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE / 10, /* 堆栈监测区，这里表示后10%作为监测区 */
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,  /* 堆栈空间大小 */
                 (OS_MSG_QTY    )0,  /* 本任务支持接受的最大消息数 */
                 (OS_TICK       )0,  /* 设置时间片 */
                 (void         *)0,  /* 堆栈空间大小 */  
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
		
				 /*  定义如下：
					OS_TASK_OPT_STK_CHK      使能检测任务栈，统计任务栈已用的和未用的
					OS_TASK_OPT_STK_CLR      在创建任务时，清零任务栈
					OS_TASK_OPT_SAVE_FP      如果CPU有浮点寄存器，则在任务切换时保存浮点寄存器的内容
				 */  
                 (OS_ERR       *)&err);

	/* 启动多任务系统，控制权交给uC/OS-III */
    OSStart(&err);                                             
    
    (void)&err;
    
    return (0);
}
/*
*********************************************************************************************************
*	函 数 名: DispTaskInfo
*	功能说明: 将uCOS-III任务信息通过串口打印出来
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispTaskInfo(void)
{
	OS_TCB      *p_tcb;	        /* 定义一个任务控制块指针, TCB = TASK CONTROL BLOCK */
	float CPU;
	CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();
	p_tcb = OSTaskDbgListPtr;
	CPU_CRITICAL_EXIT();
	
	/* 打印标题 */
	printf("===============================================================\r\n");
	printf(" 优先级 使用栈 剩余栈 百分比 利用率   任务名\r\n");
	printf("  Prio   Used  Free   Per    CPU     Taskname\r\n");

	/* 遍历任务控制块列表(TCB list)，打印所有的任务的优先级和名称 */
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
*	函 数 名: _cbOfTmr1
*	功能说明: 软件定时器的回调函数
*           负责在开机44S后使能UART1，因为这44S是路由器启动的时间
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
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
			  //开启UART1
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
*	函 数 名: AppTaskStart
*	功能说明: 这是一个启动任务，在多任务系统启动后，必须初始化滴答计数器(在BSP_Init中实现)
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*	优 先 级: 2
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
    
	  /* 创建任务 */
    AppTaskCreate(); 
	    //创建定时器  OS_CFG_TMR_TASK_RATE_HZ = 10HZ
    OSTmrCreate ((OS_TMR              *)&Tmr_1s,
                 (CPU_CHAR            *)"MyTimer 1s",
                 (OS_TICK              )10,                  //第一次延时设置为1s，
                 (OS_TICK              )10,                  //定时器周期1s
                 (OS_OPT               )OS_OPT_TMR_PERIODIC,//模式设置为重复模式
                 (OS_TMR_CALLBACK_PTR  )_cbOfTmr1,          //回调函数
                 (void                *)0,                  //参数设置为0
                 (OS_ERR              *)err);
								 
		//启动定时器
    OSTmrStart((OS_TMR *)&Tmr_1s,(OS_ERR *)err);
								 
    /*Delete task*/
    OSTaskDel(&AppTaskStartTCB,&err);
}
/*
*********************************************************************************************************
*	函 数 名: AppTaskCom
*	功能说明: LED闪烁
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*	优 先 级: 5
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
		
		  //如果成功接收到一帧命令,则执行以下解析操作
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
*	函 数 名: AppTaskUserIF
*	功能说明: 此任务主要用于得到按键的键值。
*             1. 按下K1按键打印任务执行情况
*             2. 按下K2按键进行截图，保存BMP图片到SD卡中
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*	优 先 级: 6
*********************************************************************************************************
*/
static void AppTaskUserIF(void *p_arg)
{
	uint8_t ucKeyCode;
  OS_ERR        err;
	uint8_t down_count =0;
	(void)p_arg;	               /* 避免编译器报警 */

	while (1) 
	{   	
        
				ucKeyCode = GetKeyState();
				
				if (ucKeyCode == KEY_STATE_DOWN)
				{ //等待按键弹起
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
*	函 数 名: AppTaskNrfReceiver
*	功能说明: 创建与NRF2401通讯的任务
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
* 优 先 级: 5
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
			    /*判断接收状态*/
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
*	函 数 名: AppTaskSensorUpdate
*	功能说明: 传感器更新任务
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*	优 先 级: 3
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
*	函 数 名: AppTaskRobotControl
*	功能说明: 机器人运动控制任务
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*	优 先 级: 2
*********************************************************************************************************
*/
static void AppTaskRobotControl(void *p_arg)
{	
	OS_ERR   err;
	(void)p_arg;
	 
	/*站立动作*/
	 Stand_Up();
   
	 while(1)
	{
		  switch(_order)									
			{
				case GO_FORWARD:
   		                 /*向前走1步，步长180mm，占空比50%*/
											 Go_Straight(STRAIGHT_FORWARD, DUTY_50, STRIDE, 1);
				break;
				case GO_BACKWARD:
					             /*向后走1步，步长180mm，占空比50%*/
		                   Go_Straight(STRAIGHT_BACKWARD, DUTY_50, STRIDE, 1);			
				break;
				case TURN_LEFT:
					             /*向左转20°*/
											 Turn_Around(DIRECTION_CC, DEF_TURN_ANGLE, 1);
				break;
				case TURN_RIGHT:
					             /*向右转20°*/
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
					             /*停止*/
											 Stand_Up();
				  break;
				case STAMP:   /*跺脚*/
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
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*********************************************************************************************************
*/
static  void  AppTaskCreate (void)
{
			OS_ERR      err;
			

//			OSSemCreate(&SEM_NRFRX, "SEM_NRFReceiver", 0, &err);
			/**************创建COM任务*********************/
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
			
			/**************创建USER IF任务*********************/
			OSTaskCreate((OS_TCB       *)&AppTaskUserIFTCB,             
										 (CPU_CHAR     *)"App Task UserIF",
										 (OS_TASK_PTR   )AppTaskUserIF, 
										 (void         *)0,
										 (OS_PRIO       )APP_CFG_TASK_USER_IF_PRIO,
										 (CPU_STK      *)&AppTaskUserIFStk[0],
										 (CPU_STK_SIZE  )APP_CFG_TASK_USER_IF_STK_SIZE / 10,
										 (CPU_STK_SIZE  )APP_CFG_TASK_USER_IF_STK_SIZE,
										 (OS_MSG_QTY    )0,
										 (OS_TICK       )0,  //因为与NRF任务相同优先级，所以设置他的轮转时间片为5ms
										 (void         *)0,
										 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
										 (OS_ERR       *)&err);
										 
			/**************创建NRF接收任务*********************/
			OSTaskCreate((OS_TCB       *)&AppTaskNRFTCB,             
										 (CPU_CHAR     *)"App Task NRFReceiver",
										 (OS_TASK_PTR   )AppTaskNrfReceiver, 
										 (void         *)0,
										 (OS_PRIO       )APP_CFG_TASK_NRF_PRIO,
										 (CPU_STK      *)&AppTaskNRFStk[0],
										 (CPU_STK_SIZE  )APP_CFG_TASK_NRF_STK_SIZE / 10,
										 (CPU_STK_SIZE  )APP_CFG_TASK_NRF_STK_SIZE,
										 (OS_MSG_QTY    )0,
										 (OS_TICK       )5,   //因为与串口任务相同优先级，所以设置他的轮转时间片为5ms
										 (void         *)0,
										 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
										 (OS_ERR       *)&err);
										 
			/**************创建传感器更新任务*********************/
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
 			/**************创建机器人运动控制任务*********************/
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


/***************************** 阿波罗科技 www.apollorobot.com (END OF FILE) *********************************/

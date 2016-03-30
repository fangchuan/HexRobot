/*
*********************************************************************************************************
*	                                  
*	模块名称 : GUI界面任务
*	文件名称 : MainTask.c
*	版    本 : V4.0
*	说    明 : GUI任务
*
*	修改记录 :
*		版本号    日期          作者               说明
*		V1.0    2013-03-26    Eric2013        版本emWin5.16
*       V2.0    2014-02-23    Eric2013        版本STemWin5.22
*	    V3.0    2015-04-08    Eric2013        1. 升级固件库到V1.5.0
*                                             2. 升级BSP板级支持包 
*                                             3. 升级fatfs到0.11
*                                             4. 升级STemWin到5.26
*                                             5. 更改为新的四点触摸校准算法
*                                             6. 添加7寸800*480分辨率电容屏支持，添加3.5寸480*320的ILI9488支持。
*                                             7. 修改对话框显示大小固定为480*272，以此来适应所有屏。
*                                             8. 重新整理注释，使其更加规范。
*	    V4.0    2015-12-18  Eric2013          1. 升级BSP板级支持包 
*                                             2. 升级STemWin到5.28
*                                             3. 添加4.3寸和5寸电容屏支持。
*
*      Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"
#include "MainTask.h"


/*
*********************************************************************************************************
*				                         变量
*********************************************************************************************************
*/
static const char *TaskProfile[7]={" "," "," "," "," "," "," "};
static GRAPH_SCALE_Handle hScaleV;     
static GRAPH_DATA_Handle  ahData[1];   /* GRAPH_DATA 的句柄 */
static GUI_COLOR _aColor[] = {GUI_BLUE};

/*
*********************************************************************************************************
*				                         对话框初始化选项
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { FRAMEWIN_CreateIndirect,  "uCOS-III V3.03.01",           0,     0,  0,  480,272,0,0},
};

/*
*********************************************************************************************************
*	函 数 名: InitDialog
*	功能说明: 初始化函数 
*	形    参：pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
void InitDialog(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
    //
    //FRAMEWIN
    //
	FRAMEWIN_SetTextColor(hWin,0x0000ff);
	FRAMEWIN_SetFont(hWin,&GUI_FontHZ_SimSun_1616);
    FRAMEWIN_SetTextAlign(hWin,GUI_TA_VCENTER|GUI_TA_CENTER);
    FRAMEWIN_SetTitleHeight(hWin,25);
}

/*
*********************************************************************************************************
*	函 数 名: _cbCallback
*	功能说明: 主对话框回调函数 
*	形    参：pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbCallback(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
		/* 重绘消息 */
        case WM_PAINT:
            break;
		
		/* 初始化消息 */
        case WM_INIT_DIALOG:
            InitDialog(pMsg);
            break;
		
		/* 按键消息 */
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_ESCAPE:
                    GUI_EndDialog(hWin, 1);
                    break;
                case GUI_KEY_ENTER:
                    GUI_EndDialog(hWin, 0);
                    break;
            }
            break;
		
		/* 通知消息 */
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
                case GUI_ID_OK:
                    if(NCode==WM_NOTIFICATION_RELEASED)
                        GUI_EndDialog(hWin, 0);
                    break;
                case GUI_ID_CANCEL:
                    if(NCode==WM_NOTIFICATION_RELEASED)
                        GUI_EndDialog(hWin, 0);
                    break;
            }
            break;

        default:
            WM_DefaultProc(pMsg);
    }
}

/*
*********************************************************************************************************
*				                         任务对话框初始化选项
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateTask[] = {
  { WINDOW_CreateIndirect,    NULL,              0,                   0,   0, 479, 271, FRAMEWIN_CF_MOVEABLE },
  { LISTVIEW_CreateIndirect,   NULL,             GUI_ID_LISTVIEW0,        0,  0,  474,254,0,0}
};

/*
*********************************************************************************************************
*	函 数 名: InitDialog
*	功能说明: 初始化函数 
*	形    参：pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
void InitDialogTask(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin = pMsg->hWin;
	unsigned char i;
	HEADER_Handle hHeader;

    //
    //GUI_ID_LISTVIEW0
    //
	hHeader = LISTVIEW_GetHeader(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0));
	HEADER_SetFont(hHeader,&GUI_FontHZ_SimSun_1616);
	HEADER_SetHeight(hHeader,20);

    LISTVIEW_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),0,0x000000);
    LISTVIEW_SetTextColor(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),0,0x00ff00);
    LISTVIEW_SetFont(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),&GUI_FontHZ_SimSun_1616);

    LISTVIEW_AddColumn(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),60,"优先级",GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),60,"堆栈使用",GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),60,"堆栈剩余",GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),60,"堆栈比",GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),60,"CPU",GUI_TA_VCENTER|GUI_TA_LEFT);

    LISTVIEW_AddColumn(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),100,"名字",GUI_TA_VCENTER|GUI_TA_LEFT);
    
	LISTVIEW_SetColumnWidth(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 0, 50);
	LISTVIEW_SetColumnWidth(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 1, 70);
	LISTVIEW_SetColumnWidth(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 2, 70);
	LISTVIEW_SetColumnWidth(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 3, 55);
	LISTVIEW_SetColumnWidth(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 4, 60);
	LISTVIEW_SetColumnWidth(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 5, 170);
	
	for(i = 0; i < OSTaskQty; i++)
	{
		LISTVIEW_AddRow(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), (GUI_ConstString *)TaskProfile);
		LISTVIEW_SetItemText(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 0, i, "1");		
		LISTVIEW_SetItemText(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 1, i, "2");
		LISTVIEW_SetItemText(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 2, i, "3");		
		LISTVIEW_SetItemText(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 3, i, "4");
		LISTVIEW_SetItemText(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 4, i, "5");		
		LISTVIEW_SetItemText(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0), 5, i, "6");					
	}
}

/*
*********************************************************************************************************
*	函 数 名: _cbCallback
*	功能说明: 任务对话框回调函数 
*	形    参：pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbCallbackTask(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
        /* 重绘消息 */
        case WM_PAINT:
            break;
		
		/* 初始化消息 */
        case WM_INIT_DIALOG:
            InitDialogTask(pMsg);
            break;
		
		/* 按键消息 */
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_ESCAPE:
                    GUI_EndDialog(hWin, 1);
                    break;
                case GUI_KEY_ENTER:
                    GUI_EndDialog(hWin, 0);
                    break;
            }
            break;
		
		/* 通知消息 */
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
                case GUI_ID_OK:
                    if(NCode==WM_NOTIFICATION_RELEASED)
                        GUI_EndDialog(hWin, 0);
                    break;
                case GUI_ID_CANCEL:
                    if(NCode==WM_NOTIFICATION_RELEASED)
                        GUI_EndDialog(hWin, 0);
                    break;
            }
            break;

        default:
            WM_DefaultProc(pMsg);
    }
}

/*
*********************************************************************************************************
*				                         对话框初始化选项
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateCPU[] = {
   { WINDOW_CreateIndirect,    NULL,              0,                   0,   0, 479, 271, FRAMEWIN_CF_MOVEABLE },
   { GRAPH_CreateIndirect,     0,                 GUI_ID_GRAPH0,       0,   0, 466, 231, 0, 0}
};

/*
*********************************************************************************************************
*	函 数 名: InitDialog
*	功能说明: CPU利用率对话框初始化函数 
*	形    参：pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
void InitDialogCPU(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
	WM_HWIN hItem;
	unsigned char i;

  	hItem = WM_GetDialogItem(hWin, GUI_ID_GRAPH0);

	for (i = 0; i < GUI_COUNTOF(_aColor); i++)
	{
		/* 创建成功的话，返回数据句柄 */
		/* 显示的曲线颜色，可以显示的最大数据个数，数据指针，要添加的数据个数 */
		ahData[i] = GRAPH_DATA_YT_Create(_aColor[i], 480, 0, 0);
		
		/* 为绘图控件添加数据对象 */
		GRAPH_AttachData(hItem, ahData[i]);
	}

	GRAPH_SetGridDistY(hItem, 20);
	
	/* 栅格是否可见 */
    GRAPH_SetGridVis(hItem, 1);
	
	/* 固定X轴的栅格 */
    GRAPH_SetGridFixedX(hItem, 1);

	/* 创建和增加垂直范围尺度标签  */
	/* 离左边的尺度位置，对齐方式，垂直或水平标签，标签的间距 */
    hScaleV = GRAPH_SCALE_Create( 20, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 20);
	
	/* 用于设置比例因子 */
	GRAPH_SCALE_SetFactor(hScaleV, 0.5);
	
	/* 设置标签字体颜色 */
    GRAPH_SCALE_SetTextColor(hScaleV, GUI_RED);
	
	/* 将标签添加到垂直方向 */
    GRAPH_AttachScale(hItem, hScaleV);
	
    /* 用于设置左上右下边界 */
	GRAPH_SetBorder(hItem,25,0,0,10);

}

/*
*********************************************************************************************************
*	函 数 名: _cbCallback
*	功能说明: CPU利用率回调函数 
*	形    参：pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbCallbackCPU(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
        /* 重绘消息 */
        case WM_PAINT:
            break;
		
		/* 初始化消息 */
        case WM_INIT_DIALOG:
            InitDialogCPU(pMsg);
            break;
		
		/* 按键消息 */
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_ESCAPE:
                    GUI_EndDialog(hWin, 1);
                    break;
                case GUI_KEY_ENTER:
                    GUI_EndDialog(hWin, 0);
                    break;
            }
            break;
		
		/* 通知消息 */
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
                case GUI_ID_OK:
                    if(NCode==WM_NOTIFICATION_RELEASED)
                        GUI_EndDialog(hWin, 0);
                    break;
                case GUI_ID_CANCEL:
                    if(NCode==WM_NOTIFICATION_RELEASED)
                        GUI_EndDialog(hWin, 0);
                    break;
            }
            break;

        default:
            WM_DefaultProc(pMsg);
    }
}

/*
*********************************************************************************************************
*	函 数 名: MainTask
*	功能说明: GUI主函数 
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void MainTask(void) 
{ 
    WM_HWIN hDialog; 
	WM_HWIN hMultiPage, hDialogTemp;
	OS_TCB      *p_tcb;	        /* 定义一个任务控制块指针, TCB = TASK CONTROL BLOCK */
	float CPU;
	char buf[10],i;
	CPU_SR_ALLOC();

	/* 初始化 */
	GUI_Init();
	WM_SetCreateFlags(WM_CF_MEMDEV);  
    WM_SetDesktopColor(GUI_BLUE); 
	
	/* 使能相关控件的皮肤 */
    PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
	DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
	SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
	HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
	MULTIPAGE_SetDefaultSkin(MULTIPAGE_SKIN_FLEX);

	/* 创建对话框 */
    hDialog = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, 0, 0, 0);
	/* 创建多页控件 */
	hMultiPage = MULTIPAGE_CreateEx(0, 0, WM_GetWindowSizeX(WM_GetClientWindow(hDialog)), WM_GetWindowSizeY(WM_GetClientWindow(hDialog)), WM_GetClientWindow(hDialog), WM_CF_SHOW, 0, 0);
	/* 设置多页控件字体 */
	MULTIPAGE_SetFont(hMultiPage, &GUI_FontHZ_SimSun_1616);		
	/* 创建多页控件第一项 */
	hDialogTemp = GUI_CreateDialogBox(_aDialogCreateCPU, GUI_COUNTOF(_aDialogCreateCPU), &_cbCallbackCPU, WM_UNATTACHED, 0, 0);  		
 	MULTIPAGE_AddPage(hMultiPage, hDialogTemp, "性能");	
	/* 创建多页控件第二项 */
	hDialogTemp = GUI_CreateDialogBox(_aDialogCreateTask, GUI_COUNTOF(_aDialogCreateTask), &_cbCallbackTask, WM_UNATTACHED, 0, 0);   		
	MULTIPAGE_AddPage(hMultiPage, hDialogTemp, "进程");

	while(1)
	{
		/* 选择多页控件的第2页 */
		if(MULTIPAGE_GetSelection(hMultiPage) == 1)
		{
			CPU_CRITICAL_ENTER();
			p_tcb = OSTaskDbgListPtr;
			CPU_CRITICAL_EXIT();
			
			i = 0;
			/* 遍历任务控制块列表(TCB list)，打印所有的任务的优先级和名称 */
			while (p_tcb != (OS_TCB *)0) 
			{
				CPU = (float)p_tcb->CPUUsage / 100;
				sprintf(buf, "%d", p_tcb->Prio);
				LISTVIEW_SetItemText(WM_GetDialogItem(MULTIPAGE_GetWindow(hMultiPage, 1), GUI_ID_LISTVIEW0), 0, i, buf);
				sprintf(buf, "%d", p_tcb->StkUsed);
				LISTVIEW_SetItemText(WM_GetDialogItem(MULTIPAGE_GetWindow(hMultiPage, 1), GUI_ID_LISTVIEW0), 1, i, buf);
				sprintf(buf, "%d", p_tcb->StkFree);
				LISTVIEW_SetItemText(WM_GetDialogItem(MULTIPAGE_GetWindow(hMultiPage, 1), GUI_ID_LISTVIEW0), 2, i, buf);
				sprintf(buf, "%d%%", (p_tcb->StkUsed * 100) / (p_tcb->StkUsed + p_tcb->StkFree));
				LISTVIEW_SetItemText(WM_GetDialogItem(MULTIPAGE_GetWindow(hMultiPage, 1), GUI_ID_LISTVIEW0), 3, i, buf);
				sprintf(buf, "%5.2f%% ", CPU);
				LISTVIEW_SetItemText(WM_GetDialogItem(MULTIPAGE_GetWindow(hMultiPage, 1), GUI_ID_LISTVIEW0), 4, i, buf);
				sprintf(buf, "%s", p_tcb->NamePtr);	
				LISTVIEW_SetItemText(WM_GetDialogItem(MULTIPAGE_GetWindow(hMultiPage, 1), GUI_ID_LISTVIEW0), 5, i, buf);
								 	
				CPU_CRITICAL_ENTER();
			    p_tcb = p_tcb->DbgNextPtr;
			    CPU_CRITICAL_EXIT();
				i++;
			}	
		}
		/* 选择多页控件的第1页 */
		else
		{
			GRAPH_DATA_YT_AddValue(ahData[0], OSStatTaskCPUUsage/50);
		}
		
		/* 延迟时间不能太快，要不会造成刷新listiew控件的时候，这个任务的利用率很高 */
		GUI_Delay(300);	
	}

}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

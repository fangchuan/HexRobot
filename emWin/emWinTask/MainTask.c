/*
*********************************************************************************************************
*	                                  
*	ģ������ : GUI��������
*	�ļ����� : MainTask.c
*	��    �� : V4.0
*	˵    �� : GUI����
*
*	�޸ļ�¼ :
*		�汾��    ����          ����               ˵��
*		V1.0    2013-03-26    Eric2013        �汾emWin5.16
*       V2.0    2014-02-23    Eric2013        �汾STemWin5.22
*	    V3.0    2015-04-08    Eric2013        1. �����̼��⵽V1.5.0
*                                             2. ����BSP�弶֧�ְ� 
*                                             3. ����fatfs��0.11
*                                             4. ����STemWin��5.26
*                                             5. ����Ϊ�µ��ĵ㴥��У׼�㷨
*                                             6. ���7��800*480�ֱ��ʵ�����֧�֣����3.5��480*320��ILI9488֧�֡�
*                                             7. �޸ĶԻ�����ʾ��С�̶�Ϊ480*272���Դ�����Ӧ��������
*                                             8. ��������ע�ͣ�ʹ����ӹ淶��
*	    V4.0    2015-12-18  Eric2013          1. ����BSP�弶֧�ְ� 
*                                             2. ����STemWin��5.28
*                                             3. ���4.3���5�������֧�֡�
*
*      Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"
#include "MainTask.h"


/*
*********************************************************************************************************
*				                         ����
*********************************************************************************************************
*/
static const char *TaskProfile[7]={" "," "," "," "," "," "," "};
static GRAPH_SCALE_Handle hScaleV;     
static GRAPH_DATA_Handle  ahData[1];   /* GRAPH_DATA �ľ�� */
static GUI_COLOR _aColor[] = {GUI_BLUE};

/*
*********************************************************************************************************
*				                         �Ի����ʼ��ѡ��
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { FRAMEWIN_CreateIndirect,  "uCOS-III V3.03.01",           0,     0,  0,  480,272,0,0},
};

/*
*********************************************************************************************************
*	�� �� ��: InitDialog
*	����˵��: ��ʼ������ 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
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
*	�� �� ��: _cbCallback
*	����˵��: ���Ի���ص����� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbCallback(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
		/* �ػ���Ϣ */
        case WM_PAINT:
            break;
		
		/* ��ʼ����Ϣ */
        case WM_INIT_DIALOG:
            InitDialog(pMsg);
            break;
		
		/* ������Ϣ */
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
		
		/* ֪ͨ��Ϣ */
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
*				                         ����Ի����ʼ��ѡ��
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateTask[] = {
  { WINDOW_CreateIndirect,    NULL,              0,                   0,   0, 479, 271, FRAMEWIN_CF_MOVEABLE },
  { LISTVIEW_CreateIndirect,   NULL,             GUI_ID_LISTVIEW0,        0,  0,  474,254,0,0}
};

/*
*********************************************************************************************************
*	�� �� ��: InitDialog
*	����˵��: ��ʼ������ 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
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

    LISTVIEW_AddColumn(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),60,"���ȼ�",GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),60,"��ջʹ��",GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),60,"��ջʣ��",GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),60,"��ջ��",GUI_TA_VCENTER|GUI_TA_LEFT);
    LISTVIEW_AddColumn(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),60,"CPU",GUI_TA_VCENTER|GUI_TA_LEFT);

    LISTVIEW_AddColumn(WM_GetDialogItem(hWin,GUI_ID_LISTVIEW0),100,"����",GUI_TA_VCENTER|GUI_TA_LEFT);
    
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
*	�� �� ��: _cbCallback
*	����˵��: ����Ի���ص����� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbCallbackTask(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
        /* �ػ���Ϣ */
        case WM_PAINT:
            break;
		
		/* ��ʼ����Ϣ */
        case WM_INIT_DIALOG:
            InitDialogTask(pMsg);
            break;
		
		/* ������Ϣ */
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
		
		/* ֪ͨ��Ϣ */
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
*				                         �Ի����ʼ��ѡ��
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateCPU[] = {
   { WINDOW_CreateIndirect,    NULL,              0,                   0,   0, 479, 271, FRAMEWIN_CF_MOVEABLE },
   { GRAPH_CreateIndirect,     0,                 GUI_ID_GRAPH0,       0,   0, 466, 231, 0, 0}
};

/*
*********************************************************************************************************
*	�� �� ��: InitDialog
*	����˵��: CPU�����ʶԻ����ʼ������ 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
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
		/* �����ɹ��Ļ����������ݾ�� */
		/* ��ʾ��������ɫ��������ʾ��������ݸ���������ָ�룬Ҫ��ӵ����ݸ��� */
		ahData[i] = GRAPH_DATA_YT_Create(_aColor[i], 480, 0, 0);
		
		/* Ϊ��ͼ�ؼ�������ݶ��� */
		GRAPH_AttachData(hItem, ahData[i]);
	}

	GRAPH_SetGridDistY(hItem, 20);
	
	/* դ���Ƿ�ɼ� */
    GRAPH_SetGridVis(hItem, 1);
	
	/* �̶�X���դ�� */
    GRAPH_SetGridFixedX(hItem, 1);

	/* ���������Ӵ�ֱ��Χ�߶ȱ�ǩ  */
	/* ����ߵĳ߶�λ�ã����뷽ʽ����ֱ��ˮƽ��ǩ����ǩ�ļ�� */
    hScaleV = GRAPH_SCALE_Create( 20, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 20);
	
	/* �������ñ������� */
	GRAPH_SCALE_SetFactor(hScaleV, 0.5);
	
	/* ���ñ�ǩ������ɫ */
    GRAPH_SCALE_SetTextColor(hScaleV, GUI_RED);
	
	/* ����ǩ��ӵ���ֱ���� */
    GRAPH_AttachScale(hItem, hScaleV);
	
    /* ���������������±߽� */
	GRAPH_SetBorder(hItem,25,0,0,10);

}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallback
*	����˵��: CPU�����ʻص����� 
*	��    �Σ�pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbCallbackCPU(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
        /* �ػ���Ϣ */
        case WM_PAINT:
            break;
		
		/* ��ʼ����Ϣ */
        case WM_INIT_DIALOG:
            InitDialogCPU(pMsg);
            break;
		
		/* ������Ϣ */
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
		
		/* ֪ͨ��Ϣ */
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
*	�� �� ��: MainTask
*	����˵��: GUI������ 
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MainTask(void) 
{ 
    WM_HWIN hDialog; 
	WM_HWIN hMultiPage, hDialogTemp;
	OS_TCB      *p_tcb;	        /* ����һ��������ƿ�ָ��, TCB = TASK CONTROL BLOCK */
	float CPU;
	char buf[10],i;
	CPU_SR_ALLOC();

	/* ��ʼ�� */
	GUI_Init();
	WM_SetCreateFlags(WM_CF_MEMDEV);  
    WM_SetDesktopColor(GUI_BLUE); 
	
	/* ʹ����ؿؼ���Ƥ�� */
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

	/* �����Ի��� */
    hDialog = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, 0, 0, 0);
	/* ������ҳ�ؼ� */
	hMultiPage = MULTIPAGE_CreateEx(0, 0, WM_GetWindowSizeX(WM_GetClientWindow(hDialog)), WM_GetWindowSizeY(WM_GetClientWindow(hDialog)), WM_GetClientWindow(hDialog), WM_CF_SHOW, 0, 0);
	/* ���ö�ҳ�ؼ����� */
	MULTIPAGE_SetFont(hMultiPage, &GUI_FontHZ_SimSun_1616);		
	/* ������ҳ�ؼ���һ�� */
	hDialogTemp = GUI_CreateDialogBox(_aDialogCreateCPU, GUI_COUNTOF(_aDialogCreateCPU), &_cbCallbackCPU, WM_UNATTACHED, 0, 0);  		
 	MULTIPAGE_AddPage(hMultiPage, hDialogTemp, "����");	
	/* ������ҳ�ؼ��ڶ��� */
	hDialogTemp = GUI_CreateDialogBox(_aDialogCreateTask, GUI_COUNTOF(_aDialogCreateTask), &_cbCallbackTask, WM_UNATTACHED, 0, 0);   		
	MULTIPAGE_AddPage(hMultiPage, hDialogTemp, "����");

	while(1)
	{
		/* ѡ���ҳ�ؼ��ĵ�2ҳ */
		if(MULTIPAGE_GetSelection(hMultiPage) == 1)
		{
			CPU_CRITICAL_ENTER();
			p_tcb = OSTaskDbgListPtr;
			CPU_CRITICAL_EXIT();
			
			i = 0;
			/* ����������ƿ��б�(TCB list)����ӡ���е���������ȼ������� */
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
		/* ѡ���ҳ�ؼ��ĵ�1ҳ */
		else
		{
			GRAPH_DATA_YT_AddValue(ahData[0], OSStatTaskCPUUsage/50);
		}
		
		/* �ӳ�ʱ�䲻��̫�죬Ҫ�������ˢ��listiew�ؼ���ʱ���������������ʺܸ� */
		GUI_Delay(300);	
	}

}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

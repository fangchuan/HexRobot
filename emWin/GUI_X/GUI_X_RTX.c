/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2012  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.16 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  ARM LIMITED whose registered office
is situated at  110 Fulbourn Road,  Cambridge CB1 9NJ,  England solely
for  the  purposes  of  creating  libraries  for  ARM7, ARM9, Cortex-M
series,  and   Cortex-R4   processor-based  devices,  sublicensed  and
distributed as part of the  MDK-ARM  Professional  under the terms and
conditions  of  the   End  User  License  supplied  with  the  MDK-ARM
Professional. 
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUI_X.C
Purpose     : Config / System dependent externals for GUI
---------------------------END-OF-HEADER------------------------------
*/

#include <RTL.h>
#include "GUI.h"

/*********************************************************************
*
*       Global data
*/
volatile int OS_TimeMS;

/*********************************************************************
*
*      Timing:
*                 GUI_X_GetTime()
*                 GUI_X_Delay(int)

  Some timing dependent routines require a GetTime
  and delay function. Default time unit (tick), normally is
  1 ms.
*/

int GUI_X_GetTime(void) { 
  return OS_TimeMS; 
}

void GUI_X_Delay(int ms) { 
  while (ms > 0xFFFE) {
    ms -= 0xFFFE;
    os_dly_wait(0xFFFE);
  }
  os_dly_wait(ms);
}

/*********************************************************************
*
*       GUI_X_Init()
*
* Note:
*     GUI_X_Init() is called from GUI_Init is a possibility to init
*     some hardware which needs to be up and running before the GUI.
*     If not required, leave this routine blank.
*/

__task void SysTick_Task (void);

void GUI_X_Init(void) {
  os_tsk_create(SysTick_Task, 1);
}

/*********************************************************************
*
*       GUI_X_ExecIdle
*
* Note:
*  Called if WM is in idle state
*/

void GUI_X_ExecIdle(void) {
  os_dly_wait(1);
}

/*********************************************************************
*
*      Logging: OS dependent

Note:
  Logging is used in higher debug levels only. The typical target
  build does not use logging and does therefor not require any of
  the logging routines below. For a release build without logging
  the routines below may be eliminated to save some space.
  (If the linker is not function aware and eliminates unreferenced
  functions automatically)

*/

void GUI_X_Log     (const char *s) { GUI_USE_PARA(s); }
void GUI_X_Warn    (const char *s) { GUI_USE_PARA(s); }
void GUI_X_ErrorOut(const char *s) { GUI_USE_PARA(s); }

/*********************************************************************
*
*      Multitasking:
*
*                 GUI_X_InitOS()
*                 GUI_X_GetTaskId()
*                 GUI_X_Lock()
*                 GUI_X_Unlock()
*
* Note:
*   The following routines are required only if emWin is used in a
*   true multi task environment, which means you have more than one
*   thread using the emWin API.
*   In this case the
*                       #define GUI_OS 1
*  needs to be in GUIConf.h
*/

OS_SEM GUI_Semaphore;

void GUI_X_InitOS(void)    { os_sem_init(GUI_Semaphore, 1); }
void GUI_X_Unlock(void)    { os_sem_send(GUI_Semaphore); }
void GUI_X_Lock(void)      { os_sem_wait(GUI_Semaphore, 0xFFFF); }
U32  GUI_X_GetTaskId(void) { return os_tsk_self(); }

/*********************************************************************
*
*      Event driving (optional with multitasking)
*
*                 GUI_X_WaitEvent()
*                 GUI_X_WaitEventTimed()
*                 GUI_X_SignalEvent()
*/

OS_TID GUI_Task;

void GUI_X_WaitEvent(void) {
  GUI_Task = os_tsk_self();
  os_evt_wait_or (0x0001, 0xFFFF); 
}

void GUI_X_WaitEventTimed(int Period) {
  GUI_Task = os_tsk_self();
  os_evt_wait_or (0x0001, (Period > 0xFFFE) ? 0xFFFE : Period); 
}

void GUI_X_SignalEvent(void) {
  if (GUI_Task) {
    os_evt_set (0x0001, GUI_Task);
  }
}

/*********************************************************************
*
*       SysTick_Task
*
*/

__task void SysTick_Task (void) {

  os_itv_set(10);
  for (;;) {
    os_itv_wait();
    OS_TimeMS += 10;
  }
}

/*************************** End of file ****************************/

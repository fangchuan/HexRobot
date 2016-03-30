/*
*********************************************************************************************************
*	                                  
*	模块名称 : GUI界面主函数
*	文件名称 : MainTask.c
*	版    本 : V4.0
*	说    明 : GUI界面主函数
*
*	修改记录 :
*		版本号   日期         作者            说明
*		v1.0    2013-11-17  Eric2013  	 1. ST固件库V1.0.2版本
*       v2.0    2014-03-22  Eric2013     1. 升级ST固件库到V1.3.0版本
*                                        2. 解决uCOS-II开启FPU的问题
*	    V3.0    2015-03-27   Eric2013    1. 升级固件库到V1.5.0
*                                        2. 升级BSP板级支持包 
*                                        3. 升级fatfs到0.11
*                                        4. 升级STemWin到5.26
*                                        5. 更改为新的四点触摸校准算法，并重查编排触摸检测任务
*                                        6. 添加7寸800*480分辨率电容屏支持，添加3.5寸480*320的ILI9488支持。
*	    V4.0    2015-12-18  Eric2013     1. 升级BSP板级支持包 
*                                        2. 升级STemWin到5.28
*                                        3. 添加4.3寸和5寸电容屏支持。
*                                     
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __MainTask_H
#define __MainTask_H

#include "GUI.h"
#include "DIALOG.h"
#include "WM.h"
#include "BUTTON.h"
#include "CHECKBOX.h"
#include "DROPDOWN.h"
#include "EDIT.h"
#include "FRAMEWIN.h"
#include "LISTBOX.h"
#include "MULTIEDIT.h"
#include "RADIO.h"
#include "SLIDER.h"
#include "TEXT.h"
#include "PROGBAR.h"
#include "SCROLLBAR.h"
#include "LISTVIEW.h"
#include "GRAPH.h"
#include "MENU.h"
#include "MULTIPAGE.h"
#include "ICONVIEW.h"
#include "TREEVIEW.h"

#include "ff.h"

/*
************************************************************************
*						  FatFs
************************************************************************
*/
extern FRESULT result;
extern FIL file;
extern FILINFO finfo;
extern DIR DirInf;
extern UINT bw;
extern FATFS fs;

extern void _WriteByte2File(U8 Data, void * p); 
/*
************************************************************************
*						emWin字体
************************************************************************
*/
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_Song_12;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_FangSong_16;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_Song_16;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_Hei_24;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_Kai_24;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_Song_24;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_SimSun_1616;
extern GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ_SimSun_2424;

#endif


/*****************************(END OF FILE) *********************************/

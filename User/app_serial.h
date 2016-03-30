/*
*********************************************************************************************************
*                                               uC/PROBE
*
*                          (c) Copyright 2003-2009; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                               uC/Serial
*                                           Application Code
*
* Filename      : app_serial.h
* Version       : V1.00
* Programmer(s) : FT
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               APP_SERIAL_MODULE_PRESENT present pre-processor macro definition.
*********************************************************************************************************
*/

#ifndef  APP_SERIAL_MODULE_PRESENT                              /* See Note #1.                                         */
#define  APP_SERIAL_MODULE_PRESENT


/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include  <lib_def.h>

#include  <cpu.h>

#include  <app_cfg.h>


/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   APP_SERIAL_MODULE
#define  APP_SERIAL_MODULE_EXT
#else
#define  APP_SERIAL_MODULE_EXT  extern
#endif

/*
*********************************************************************************************************
*                                        DEFAULT CONFIGURATION
*********************************************************************************************************
*/

#ifndef  APP_CFG_SERIAL_EN                   
#define  APP_CFG_SERIAL_EN                   DEF_DISABLED  
#endif

#ifndef  APP_SERIAL_CFG_TRACE_EN
#define  APP_SERIAL_CFG_TRACE_EN             DEF_DISABLED
#endif


/*
*********************************************************************************************************
*                                      CONDITIONAL INCLUDE FILES
*********************************************************************************************************
*/


#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
#include  <serial.h>
#include  <serial_bsp_stm3240x.h>
#endif

#if (APP_SERIAL_CFG_TRACE_EN == DEF_ENABLED)
#include  <stdarg.h>
#include  <stdio.h>
#include  <lib_str.h>
#include  <serial_line_tty.h>
#endif

/*
*********************************************************************************************************
*                                                 DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                                 MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
CPU_BOOLEAN  App_SerialInit (void);

#if (APP_SERIAL_CFG_TRACE_EN == DEF_ENABLED)  
void         App_SerPrintf  (CPU_CHAR *format, ...);
void         App_SerStrRd   (CPU_CHAR   *p_str,
                             CPU_SIZE_T  len);
void         App_SerStrWr   (CPU_CHAR   *p_str);                             
#endif
#endif

/*
*********************************************************************************************************
*                                          CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef  APP_CFG_SERIAL_EN
#error  "APP_CFG_SERIAL_EN                              not #define'd in 'app_cfg.h'     "
#error  "                                   [MUST be  DEF_DISABLED]                      "
#error  "                                   [     ||  DEF_ENABLED ]                      "

#elif  ((APP_CFG_SERIAL_EN != DEF_DISABLED) && \
        (APP_CFG_SERIAL_EN != DEF_ENABLED ))
#error  "APP_CFG_SERIAL_EN                        illegally #define'd in 'app_cfg.h'     "
#error  "                                   [MUST be  DEF_DISABLED]                      "
#error  "                                   [     ||  DEF_ENABLED ]                      "

#elif  (APP_CFG_SERIAL_EN == DEF_ENABLED)
#ifndef  APP_SERIAL_CFG_TRACE_EN
#error  "APP_SERIAL_CFG_TRACE_EN                        not #define'd in 'app_cfg.h'     "
#error  "                                   [MUST be  DEF_DISABLED]                      "
#error  "                                   [     ||  DEF_ENABLED ]                      "

#elif  ((APP_SERIAL_CFG_TRACE_EN != DEF_DISABLED) && \
        (APP_SERIAL_CFG_TRACE_EN != DEF_ENABLED ))
#error  "APP_SERIAL_CFG_TRACE_EN                  illegally #define'd in 'app_cfg.h'     "
#error  "                                   [MUST be  DEF_DISABLED]                      "
#error  "                                   [     ||  DEF_ENABLED ]                      "

#elif  (APP_SERIAL_CFG_TRACE_EN == DEF_ENABLED)
#if    !defined(APP_SERIAL_CFG_TRACE_PORT_NAME)
#error  "APP_SERIAL_CFG_TRACE_PORT_NAME                 not #define'd in 'app_cfg.h'     "
#endif
#endif
#endif


/*
*********************************************************************************************************
*                                              MODULE END
*
* Note(s) : See 'MODULE  Note #1'.
*********************************************************************************************************
*/

#endif                                                          /* End of APP_SERIAL_MODULE_PRESENT module include ...  */
                                                                /* ... (see Note #1).                                   */

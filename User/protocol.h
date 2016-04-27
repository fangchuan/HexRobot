/*
*********************************************************************************************************
*
*	模块名称 : 与上位机协议模块
*	文件名称 : protocol.h
*	版    本 : V1.0
*	说    明 : 
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2016-04-2 方川  正式发布
*
*	Copyright (C), 2015-2020, 阿波罗科技 www.apollorobot.com
*
*********************************************************************************************************
*/
#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#define  PROTOCOL_VERSION   1

#define  PROTOCOL_STX    0xAA 
#define  PROTOCOL_END    0xBB
#define  PROTOCOL_VER    PROTOCOL_VERSION
#define  PROTOCOL_MAXLENGTH  19

#define  PROTOCOL_LENGTH_BIT  3
#define  PROTOCOL_PAYLOAD_BIT 4

#define  PROTOCOL_FORWARD    0x01
#define  PROTOCOL_BACKWARD   0x02
#define  PROTOCOL_TURNLEFT   0x03
#define  PROTOCOL_TURNRIGHT  0x04
#define  PROTOCOL_STOP       0x05
#define  PROTOCOL_STAMP      0x06
#define  PROTOCOL_DOWN       0x07
#define  PROTOCOL_UP         0x08
#define  PROTOCOL_HEAD_LEFT  0x09
#define  PROTOCOL_HEAD_RIGHT 0x0A


#endif
/***************************** 阿波罗科技 www.apollorobot.com (END OF FILE) *********************************/

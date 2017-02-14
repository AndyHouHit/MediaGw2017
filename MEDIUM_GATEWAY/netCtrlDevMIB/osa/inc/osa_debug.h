/******************************************************************************
* Copyright lctx Co.,Ltd.
* FileName: 	 osa_debug.h
* Desc: ��װƽ̨�޹ص� Debug��� ����
*
*
* Author: 	 Andy-wei.hou
* Date: 	 2016/09/30
* Notes:
*
* -----------------------------------------------------------------
* Histroy: v1.0   2016/09/30, Andy-wei.hou create this file
*
******************************************************************************/
#ifndef _OSA_DEBUG_H_
#define _OSA_DEBUG_H_

/*-------------------------------- Includes ----------------------------------*/
#include <stdio.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

/*------------------------------ Global Defines ------------------------------*/
#define OSA_DEBUG_MODE // enable OSA_printf, OSA_assert

//#define _DEBUG
/*------------------------------ Global Typedefs -----------------------------*/


/*------------------------------ Extern Variables ---------------------------*/



/*------------------------- Global Function Prototypes -----------------------*/
#define FUN_STR __func__





#ifdef _DEBUG
#define OSA_ERROR(...) \
  do \
  { \
  snmp_log(LOG_WARNING, "\n ERROR  (%s | %s | %d): \n\t", __FILE__, FUN_STR, __LINE__); \
  snmp_log(LOG_WARNING, __VA_ARGS__); \
  snmp_log(LOG_WARNING, "\n"); \
  } \
  while(0);
#else
#define OSA_ERROR(...)
#endif

#ifdef _DEBUG
#define OSA_DBG_MSG(...) \
  do \
  { \
  snmp_log(LOG_WARNING, "\n"); \
  snmp_log(LOG_WARNING, __VA_ARGS__); \
  } \
  while(0);
#else
#define OSA_DBG_MSG(...)
#endif

#ifdef _DEBUG //��debug���
#define OSA_DBG_MSGX(...) \
  do \
  { \
  snmp_log(LOG_WARNING, "\n (%s | %s | %d): \t", __FILE__, FUN_STR, __LINE__); \
  snmp_log(LOG_WARNING, __VA_ARGS__); \
  snmp_log(LOG_WARNING, "\n"); \
  } \
  while(0);
#else
#define OSA_DBG_MSGX(...)
#endif

#define OSA_DBG_MSGXX OSA_DBG_MSGX

///Ϊ���� OSA_printf �л�������Ӱ�죬��OSA_printf�ض���
#ifdef OSA_printf
#undef OSA_printf
#endif



#define OSA_printf OSA_DBG_MSG
#endif  //_OSA_DEBUG_H_



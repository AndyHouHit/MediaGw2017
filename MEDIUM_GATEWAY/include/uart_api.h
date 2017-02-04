/****************************************************************************************************
* Copyright @,  2010-2015,  LCTX Co., Ltd. 
* Filename:     uart_api.h   
* Version:      1.00
* Date:         2010-06-02	    
* Description:  ����ģ���û�����ͷ�ļ�����ϸ�μ�<����˵����.doc> 1.0
* Modification History: ��ʯ�� 2010-06-02 �½��ļ�

*****************************************************************************************************/
//header: uart_api.h
#ifndef __UART_API_H__
#define __UART_API_H__

#include "common.h"

#if defined(__cplusplus) 
extern "C" 
{ 
#endif



/*****************************************************************************************************
 * �궨��
*****************************************************************************************************/


/*****************************************************************************************************
 * �ṹ�嶨��
*****************************************************************************************************/
#define 	MAX_UARTLEN	1024
#define		MAX_DATLEN		200
#define 	MAX_DATA_LEN   	600
#define  	MAX_CNT			1

typedef struct _UART_DATA
{
	int iTxHead;
	int iTxTail;
	int iRxHead;
	int iRxTail;
	unsigned char aTxBuff[MAX_UARTLEN];
	unsigned char aRxBuff[MAX_UARTLEN];	
}__attribute__((packed)) ST_UART_DATA;

typedef struct __UART_I__
{
	uint8 type;
	uint8 cmd;
	uint8 len;
	uint8 dat[MAX_DATLEN];	
	uint8 flg_packtimeout;
	uint8 packtimeCt;
}__attribute__((packed)) ST_UART_I;

typedef struct _uart_data_head
{
	uint8 use_flg;
	uint8 proc_flg;
	uint16 length;
	uint32 time_cnt;
}__attribute__((packed)) UART_DATA_HEAD;

#define HEAD_LEN sizeof(UART_DATA_HEAD)

typedef struct _uart_data 
{
	UART_DATA_HEAD head;
	uint8 data[MAX_DATA_LEN];
}__attribute__((packed)) UART_DATA;



/*****************************************************************************************************
 * �ص���������
*****************************************************************************************************/


/*****************************************************************************************************
 * �ڲ���������
*****************************************************************************************************/
extern int32 Uart_init(void);
extern int32 Uart_Close(void);
extern void Uart_Pthread(void);
extern int32 Uart_data_process_20ms(void);
extern int32 Uart_Write(uint8 bPort,unsigned char *p,int bLen);


/*****************************************************************************************************
 * ������������
*****************************************************************************************************/


#if defined(__cplusplus) 
} 
#endif 

#endif // __UART_API_H__


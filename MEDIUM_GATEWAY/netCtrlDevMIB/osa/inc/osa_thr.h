
/******************************************************************************
* Copyright 2010-2011 ABB Genway Co.,Ltd.
* FileName: 	 osa_mutex.h
* Desc: ƽ̨�޹ص�  OSA ������ Thread �����ķ�װ����ͷ�ļ�

*
*
* Author: 	 Andy-wei.hou
* Date: 	 2016/10/26
* Notes: 	 For Windows platform, based on  pthreadVc2 open source project.
*			 if used, should link with flag -lpthreadVc2 for win32 application
*
* -----------------------------------------------------------------
* Histroy: v1.0   2016/10/26, Andy-wei.hou create this file
*
******************************************************************************/
#ifndef __OSA_THR_H__
#define __OSA_THR_H__

#include <pthread.h>
#include "commonTypes.h"

/*------------------------------ Global Defines ------------------------------*/
#define OSA_THREAD_HANDLE_INVLALID ((pthread_t)(0)) // -1�ɵ��� Linux ��Ӧ�ó���ֱ��crash


/*------------------------------ Global Typedefs -----------------------------*/
typedef void * (*OSA_ThrEntryFunc)(void *);

typedef struct {

  pthread_t      hndl;

} OSA_ThrHndl;


/*�̼߳�ʹ��pthread_cond ����ͬ��*/
typedef struct _OSACONDEVENTHANDLEtag
{
    pthread_cond_t stCond;
    pthread_mutex_t stMutex;
}OSA_COND_EVENT_HANDLE;


/*------------------------- Global Function Prototypes -----------------------*/


extern int OSA_ThreadCreate(OSA_ThrHndl *hndl, OSA_ThrEntryFunc ThreadFunc,void * pParamter);
extern int OSA_thrJoin(OSA_ThrHndl *hndl);
extern int OSA_thrDelete(OSA_ThrHndl *hndl);


extern int OSA_CondEventInit(OSA_COND_EVENT_HANDLE *pOSA_COND_EVENT_HANDLE);
extern int OSA_CondEventDestroy(OSA_COND_EVENT_HANDLE *pOSA_COND_EVENT_HANDLE);
extern int OSA_SetCondEvent(OSA_COND_EVENT_HANDLE *pOSA_COND_EVENT_HANDLE);
/*
* 0, return success
*-1, system API errror or invalid parameter
*-2, timetout
*/
extern int OSA_WaitCondEvent(OSA_COND_EVENT_HANDLE *pOSA_COND_EVENT_HANDLE, int iTimeOutSec);

#endif

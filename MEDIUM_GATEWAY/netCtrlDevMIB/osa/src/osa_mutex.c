/******************************************************************************
* Copyright 2010-2011 ABB Genway Co.,Ltd.
* FileName: 	 osa_mutex.c 
* Desc:
* 
* 
* Author: 	 Davis Chen
* Date: 	 2010/06/18
* Notes: 
* 
* -----------------------------------------------------------------
* Histroy: v1.0   2010/06/18, Davis Chen create this file
* 
******************************************************************************/
/*-------------------------------- Includes ----------------------------------*/
#include "commonTypes.h"
#include "osa_mutex.h"
#include "osa_debug.h"



/*-------------------- Global Definitions and Declarations -------------------*/


/*----------------------- Constant / Macro Definitions -----------------------*/


/*------------------------ Type Declarations ---------------------------------*/


/*------------------------ Variable Declarations -----------------------------*/


/*------------------------ LOCAL Function Prototype --------------------------*/


/*------------------------ Function Implement --------------------------------*/

/******************************************************************************
* Name: 	 OSA_mutexCreate 
*
* Desc: 	 ���� Mutex (������)
* Param: 	 hndl ��in�� -- ����Ļ�������ָ��
* Return: 	 �����ɹ� ���� OSA_SOK
             ����ֵ��ʾʧ��
* Global: 	 
* Note: 	 
* Author: 	 Andy-wei.hou
* -------------------------------------
* Log: 	 2016/10/26, Create this function by Andy-wei.hou
 ******************************************************************************/
int OSA_mutexCreate(OSA_MutexHndl *hndl)
{
  pthread_mutexattr_t mutex_attr;
  int status=OSA_SOK;
 
  status |= pthread_mutexattr_init(&mutex_attr);
  status |= pthread_mutex_init(&hndl->lock, &mutex_attr);
  
  if(status!=OSA_SOK)
    OSA_ERROR("OSA_mutexCreate() = %d \r\n", status);

  pthread_mutexattr_destroy(&mutex_attr);
    
  return status;
}
/******************************************************************************
* Name: 	 OSA_mutexDelete 
*
* Desc: 	 ɾ��Mutex ����
* Param: 	 hndl [in] -- Ҫɾ���� mutex ������
* Return: 	 
* Global: 	 
* Note: 	 
* Author: 	 Andy-wei.hou
* -------------------------------------
* Log: 	 2016/10/26, Create this function by Andy-wei.hou
 ******************************************************************************/
int OSA_mutexDelete(OSA_MutexHndl *hndl)
{
  pthread_mutex_destroy(&hndl->lock);  

  return OSA_SOK;
}


/******************************************************************************
* Name: 	 OSA_mutexLock 
*
* Desc: 	 ����Lock
* Param: 	 hndl [in] -- Ҫlock�� mutex ������
* Return: 	 
* Global: 	 
* Note: 	 
* Author: 	 Andy-wei.hou
* -------------------------------------
* Log: 	 2016/10/26, Create this function by Andy-wei.hou
 ******************************************************************************/
int OSA_mutexLock(OSA_MutexHndl *hndl)
{
  return pthread_mutex_lock(&hndl->lock);
}

/******************************************************************************
* Name: 	 OSA_mutexUnlock 
*
* Desc: 	 ����Unlock
* Param: 	 hndl [in] -- ҪUnlock�� mutex ������
* Return: 	 
* Global: 	 
* Note: 	 
* Author: 	 Andy-wei.hou
* -------------------------------------
* Log: 	 2016/10/26, Create this function by Andy-wei.hou
 ******************************************************************************/
int OSA_mutexUnlock(OSA_MutexHndl *hndl)
{
  return pthread_mutex_unlock(&hndl->lock);
}


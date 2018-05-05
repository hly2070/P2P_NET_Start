/**************************************************************************
* ��    Ȩ�� 	Copyright (c) 2018 FTC.
* �ļ����ƣ�	FTCTask.c
* �ļ���ʶ�� 
* ����ժҪ��	�̴߳���ӿڷ�װ
* ����˵���� 
* ��ǰ�汾��	V1.0
* ��    �ߣ�	hly2070
* ������ڣ�	2018��05�� 03��
*
* �޸ļ�¼1	��
* �޸����ڣ�
* �� �� �ţ�
* �� �� �ˣ�
* �޸����ݣ�
**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/prctl.h>
#include "FTCTask.h"

/***********************************************************
 *						��������		                  *
 **********************************************************/
#define TSK_DELETE_TRY_TIMES		100

/***********************************************************
	*	�ļ��ڲ�ʹ�õ��������� 	*
**********************************************************/
/* ����ͳһ��ں����Ĳ����ṹ*/
typedef struct tagCommonParam
{
	TSK_Handle	hTsk;		/* ������*/
	Fxn			pfProc;		/* ������������*/
	void		*pParam;	/* ����*/
}T_CommonParam;

/***********************************************************
 * 						���غ���						*
 **********************************************************/
/**********************************************************************
* �������ƣ� TSK_wait
* ���������� ˯�ߵȴ�
* ��������� uiMS -������
* ��������� ��
* �� �� ֵ�� ��
* ����˵���� 
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2018/05/04	  V1.0	     hly2070      
***********************************************************************/
static void TSK_wait(S32 uiMS)
{
	struct timespec tDelayTime;
	struct timespec tElaspedTime;

	tDelayTime.tv_sec  = uiMS / 1000;
	tDelayTime.tv_nsec = (uiMS % 1000) * 1000000;

	nanosleep(&tDelayTime, &tElaspedTime);
}

/**********************************************************************
* �������ƣ�TskEntry
* ��������������ͳһ��ں���
* ���������pParam		�������
* �����������
* �� �� ֵ��void *
* ����˵���� 
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2018/05/04	  V1.0	     hly2070      
***********************************************************************/
static void *TskEntry(void *pParam)
{
	T_CommonParam *ptPrivate = (T_CommonParam *)pParam;

	ptPrivate->hTsk->eTskState = TSK_STATE_RUNNING;
	
	if (NULL != ptPrivate->pfProc)
	{
		ptPrivate->pfProc(ptPrivate->pParam);
	}

	while (TSK_STATE_RUNNING == ptPrivate->hTsk->eTskState)
	{
		TSK_wait(10);
	}
	
	ptPrivate->hTsk->eTskState = TSK_STATE_IDLE;

	free(pParam);
	
	return pParam;
}

/***********************************************************
 * 						ȫ�ֺ���						*
 **********************************************************/ 
/**********************************************************************
* �������ƣ�TSK_create
* ������������������
* ���������pTskEntry		������ں���
*			ptAttrs		��������
*			pParam		��ں����Ĳ���
* �����������
* �� �� ֵ��������
* ����˵���� 
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2018/05/04	  V1.0	     hly2070      
***********************************************************************/
TSK_Handle TSK_create(Fxn pTskEntry, TSK_Attrs *ptAttrs, void *pParam)
{
	TSK_Handle			handle = NULL;
	T_CommonParam		*ptPrivate;
	pthread_attr_t 		thread_attr;
	struct sched_param 	schedprm;
	S32				status = 0;
	U32				uiPri = 0;

	handle = malloc(sizeof(TSK_Handle));
	if (NULL == handle)
	{
		return NULL;
	}

	// initialize thread attributes structure
	status = pthread_attr_init(&thread_attr);

	if(status!=FTC_OK) 
	{
		printf("TSK_create() - Could not initialize thread attributes 1111\n");
		free(handle);
		return NULL;
	}

	if(TSK_STACK_SIZE_DEFAULT != ptAttrs->stacksize)
	{
		pthread_attr_setstacksize(&thread_attr, ptAttrs->stacksize);
	}

	if (ptAttrs->bFifo)
	{
	  	status |= pthread_attr_setinheritsched(&thread_attr, PTHREAD_EXPLICIT_SCHED);
	 	status |= pthread_attr_setschedpolicy(&thread_attr, SCHED_RR);
		uiPri = (U32)ptAttrs->priority;
		if(uiPri > TSK_PRI_MAX)
		{
			uiPri = TSK_PRI_MAX;
		}
		else if(uiPri < TSK_PRI_MIN)
		{
			uiPri = TSK_PRI_MIN;
		}
	    
	 	schedprm.sched_priority = uiPri;
	 	status |= pthread_attr_setschedparam(&thread_attr, &schedprm);
		if(status != FTC_OK) 
		{
			printf("TSK_create() - Could not initialize thread attributes %d %m 2222\n", status);
			pthread_attr_destroy(&thread_attr);
			free(handle);
			return NULL;
		}
	}
	
	handle->eTskState = TSK_STATE_IDLE;

	ptPrivate = malloc(sizeof(T_CommonParam));
	if (NULL == ptPrivate)
	{
		printf("malloc failed!\n");
		pthread_attr_destroy(&thread_attr);
		free(handle);
		return NULL;
	}
	
	ptPrivate->hTsk = handle;
	ptPrivate->pParam = pParam;
	ptPrivate->pfProc = pTskEntry;
	status = pthread_create(&handle->tThread, &thread_attr, TskEntry, ptPrivate);

	if(status!=FTC_OK) 
	{
		printf("TSK_create() - Could not create thread [%d]\n", status);
		free(ptPrivate);
		pthread_attr_destroy(&thread_attr);
		free(handle);
		return NULL;
	}
	
	pthread_attr_destroy(&thread_attr);

	return handle;
}

/**********************************************************************
* �������ƣ�TSK_delete
* ����������ɾ������
* ���������tTsk		������
* �����������
* �� �� ֵ����
* ����˵����״̬��
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2018/05/04	  V1.0	     hly2070      
***********************************************************************/
E_StateCode TSK_delete(TSK_Handle hTsk)
{
	S32	iLoop = 0;
	S32	status = 0;
	void *returnVal;
    	
	if((NULL != hTsk) && (TSK_STATE_IDLE != hTsk->eTskState))
	{
		hTsk->eTskState = TSK_STATE_TERMINATING;

		while(TSK_STATE_IDLE != hTsk->eTskState)
		{
			TSK_wait(10);

			iLoop++;
			if (TSK_DELETE_TRY_TIMES == iLoop)
			{
				break;
			}
		}

		printf("Delete task use pthread_cancel call!\n");
		status = pthread_cancel(hTsk->tThread); 
		status |= pthread_join(hTsk->tThread, &returnVal);
	}

	free(hTsk);
	return ((0 == status) ? STATE_CODE_NO_ERROR : STATE_CODE_UNDEFINED_ERROR);

}

/**********************************************************************
* �������ƣ�TSK_WaitforReady
* �����������ȴ��������
* ���������tTsk		������
*			uiTimeOut ��ʱֵ������
* �����������
* �� �� ֵ����
* ����˵����״̬��
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2018/05/04	  V1.0	     hly2070      
***********************************************************************/
E_StateCode TSK_WaitforReady(TSK_Handle hTsk, S32 uiTimeOut)
{
	S32 uiTimes = 0;

	while(TSK_STATE_IDLE == hTsk->eTskState)
	{
		TSK_wait(10);

		if (-1 != uiTimeOut)
		{
			uiTimes += 10;
			if (uiTimes > uiTimeOut)
			{
				return STATE_CODE_TIME_OUT;
			}
		}
	}
	
	return STATE_CODE_NO_ERROR;
}

/**********************************************************************
* �������ƣ�TSK_WaitforReady
* �����������ȴ��������
* ���������name ��������
* �����������
* �� �� ֵ����
* ����˵����״̬��
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2018/05/04	  V1.0	     hly2070      
***********************************************************************/
S32 FTC_SetTaskName(S8 *name)
{
	return prctl(PR_SET_NAME, name);
}



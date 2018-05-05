/**************************************************************************
* ��    Ȩ�� 	Copyright (c) 2018 FTC.
* �ļ����ƣ�	FTCTask.h
* �ļ���ʶ�� 
* ����ժҪ��	�̲߳����ӿڷ�װ
* ����˵���� 
* ��ǰ�汾��	V1.0
* ��    �ߣ�	hly2070
* ������ڣ�	2018��05�� 04��
*
* �޸ļ�¼1	��
* �޸����ڣ�
* �� �� �ţ�
* �� �� �ˣ�
* �޸����ݣ�
**************************************************************************/
#ifndef __FTC_TASK_H__
#define __FTC_TASK_H__

#ifdef		__cplusplus
extern		"C"
{
#endif

/**************************************************************************
 *                         ͷ�ļ�����                                     *
 **************************************************************************/
#include <pthread.h>
#include "FTCTypedef.h"

/**************************************************************************
 *                        ��������                                   *
 **************************************************************************/
#define TSK_PRI_MAX					sched_get_priority_max(SCHED_FIFO)
#define TSK_PRI_MIN					sched_get_priority_min(SCHED_FIFO)
	
#define TSK_PRI_DEFAULT			(TSK_PRI_MIN + (TSK_PRI_MAX - TSK_PRI_MIN) / 2)
	
#define TSK_STACK_SIZE_DEFAULT	0

/* thread func define */
#define	FTC_PTHREAD_DETACH			do{ pthread_detach(pthread_self()); }while(0)
#define	FTC_PTHREAD_EXIT			do{ pthread_exit((void*)pthread_self()); }while(0)
	
	
#define	FTC_CREATE_THREAD(Func, Args)	do{					\
			pthread_t		__pth__;									\
			if(pthread_create(&__pth__, NULL, (void *)Func, (void *)Args))	\
			return FALSE; \
		  }while(0)
		
#define	FTC_CREATE_THREADEX(Func, Args,Ret)	do{					\
			pthread_t		__pth__;									\
			if (0 == pthread_create(&__pth__, NULL, Func, (void *)Args))	\
				Ret = TRUE; \
			else \
				Ret = FALSE; \
		  }while(0)

/**************************************************************************
 *                         ��������                                    *
 **************************************************************************/
/* ����״̬����*/
typedef enum
{
	TSK_STATE_IDLE				= 0,
	TSK_STATE_RUNNING			= 1,
	TSK_STATE_TERMINATING		= 2
} E_TskState;

typedef void (*Fxn)(void *);

/* ����ṹ*/
typedef struct tagTask
{
	pthread_t		tThread;
	E_TskState		eTskState;
}T_Tsk;

typedef T_Tsk *TSK_Handle;

/* ��������*/
typedef struct tagTSK_Attrs 
{
	S32		priority;			/* execution priority */ 
	void	*stack; 		/* pre-allocated stack */ 
	U32		stacksize;		/* stack size in MADUs */
	S32		stackseg;		/* mem seg for stack alloc */ 
	void	*environ;		/* global environ data struct */ 
	char	*name;			/* printable name */ 
	BOOL	exitflag;			/* prog termination requires */ 
	BOOL	initstackflag;	/* initialize task stack? */ 
	BOOL	bFifo;			/* is SCHED_FIFO*/
} TSK_Attrs;
				
#define DEFAULT_TSK_ATTR \
				{ \
					20,\
					NULL,\
					8 * 1024,\
					0,\
					NULL,\
					NULL,\
					0,\
					0,\
					0\
				}\

/**************************************************************************
 *                        ȫ�ֺ���                                *
 **************************************************************************/
/**********************************************************************
* �������ƣ�TSK_create
* ������������������
* ���������pTskEntry		������ں���
*			ptAttrs 	��������
*			pParam		��ں����Ĳ���
* �����������
* �� �� ֵ��������
* ����˵���� 
* �޸�����		   �汾��	   �޸��� 	   �޸�����
* -----------------------------------------------
* 2018/05/04	   V1.0		   hly2070		
***********************************************************************/
TSK_Handle TSK_create(Fxn pTskEntry, TSK_Attrs *ptAttrs, void *pParam);

/**********************************************************************
* �������ƣ�TSK_delete
* ����������ɾ������
* ���������hTsk		������
* �����������
* �� �� ֵ����
* ����˵����״̬��
* �޸�����		   �汾��	   �޸��� 	  �޸�����
* -----------------------------------------------
* 2018/05/04	   V1.0		   hly2070		
***********************************************************************/
E_StateCode TSK_delete(TSK_Handle hTsk);

/**********************************************************************
* �������ƣ�TSK_WaitforReady
* �����������ȴ��������
* ���������hTsk		������
*			uiTimeOut ��ʱֵ������
* �����������
* �� �� ֵ����
* ����˵����״̬��
* �޸�����		   �汾��	   �޸��� 	  �޸�����
* -----------------------------------------------
* 2018/05/04	   V1.0		   hly2070		
***********************************************************************/
E_StateCode TSK_WaitforReady(TSK_Handle hTsk, S32 uiTimeOut);

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

S32 FTC_SetTaskName(S8 *name);

#ifdef		__cplusplus
}
#endif

#endif


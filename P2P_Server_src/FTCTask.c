/**************************************************************************
* 版    权： 	Copyright (c) 2018 FTC.
* 文件名称：	FTCTask.c
* 文件标识： 
* 内容摘要：	线程处理接口封装
* 其它说明： 
* 当前版本：	V1.0
* 作    者：	hly2070
* 完成日期：	2018年05月 03日
*
* 修改记录1	：
* 修改日期：
* 版 本 号：
* 修 改 人：
* 修改内容：
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
 *						常量定义		                  *
 **********************************************************/
#define TSK_DELETE_TRY_TIMES		100

/***********************************************************
	*	文件内部使用的数据类型 	*
**********************************************************/
/* 任务统一入口函数的参数结构*/
typedef struct tagCommonParam
{
	TSK_Handle	hTsk;		/* 任务句柄*/
	Fxn			pfProc;		/* 任务处理主函数*/
	void		*pParam;	/* 参数*/
}T_CommonParam;

/***********************************************************
 * 						本地函数						*
 **********************************************************/
/**********************************************************************
* 函数名称： TSK_wait
* 功能描述： 睡眠等待
* 输入参数： uiMS -毫秒数
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 
* 修改日期        版本号     修改人	      修改内容
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
* 函数名称：TskEntry
* 功能描述：任务统一入口函数
* 输入参数：pParam		任务参数
* 输出参数：无
* 返 回 值：void *
* 其它说明： 
* 修改日期        版本号     修改人	      修改内容
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
 * 						全局函数						*
 **********************************************************/ 
/**********************************************************************
* 函数名称：TSK_create
* 功能描述：创建任务
* 输入参数：pTskEntry		任务入口函数
*			ptAttrs		任务属性
*			pParam		入口函数的参数
* 输出参数：无
* 返 回 值：任务句柄
* 其它说明： 
* 修改日期        版本号     修改人	      修改内容
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
* 函数名称：TSK_delete
* 功能描述：删除任务
* 输入参数：tTsk		任务句柄
* 输出参数：无
* 返 回 值：无
* 其它说明：状态码
* 修改日期        版本号     修改人	      修改内容
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
* 函数名称：TSK_WaitforReady
* 功能描述：等待任务就绪
* 输入参数：tTsk		任务句柄
*			uiTimeOut 超时值，毫秒
* 输出参数：无
* 返 回 值：无
* 其它说明：状态码
* 修改日期        版本号     修改人	      修改内容
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
* 函数名称：TSK_WaitforReady
* 功能描述：等待任务就绪
* 输入参数：name 任务名称
* 输出参数：无
* 返 回 值：无
* 其它说明：状态码
* 修改日期        版本号     修改人	      修改内容
* -----------------------------------------------
* 2018/05/04	  V1.0	     hly2070      
***********************************************************************/
S32 FTC_SetTaskName(S8 *name)
{
	return prctl(PR_SET_NAME, name);
}



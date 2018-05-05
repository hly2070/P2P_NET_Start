/**************************************************************************
* 版    权： 	Copyright (c) 2018 FTC.
* 文件名称：	FTCTask.h
* 文件标识： 
* 内容摘要：	线程操作接口封装
* 其它说明： 
* 当前版本：	V1.0
* 作    者：	hly2070
* 完成日期：	2018年05月 04日
*
* 修改记录1	：
* 修改日期：
* 版 本 号：
* 修 改 人：
* 修改内容：
**************************************************************************/
#ifndef __FTC_TASK_H__
#define __FTC_TASK_H__

#ifdef		__cplusplus
extern		"C"
{
#endif

/**************************************************************************
 *                         头文件引用                                     *
 **************************************************************************/
#include <pthread.h>
#include "FTCTypedef.h"

/**************************************************************************
 *                        常量定义                                   *
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
 *                         数据类型                                    *
 **************************************************************************/
/* 任务状态常量*/
typedef enum
{
	TSK_STATE_IDLE				= 0,
	TSK_STATE_RUNNING			= 1,
	TSK_STATE_TERMINATING		= 2
} E_TskState;

typedef void (*Fxn)(void *);

/* 任务结构*/
typedef struct tagTask
{
	pthread_t		tThread;
	E_TskState		eTskState;
}T_Tsk;

typedef T_Tsk *TSK_Handle;

/* 任务属性*/
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
 *                        全局函数                                *
 **************************************************************************/
/**********************************************************************
* 函数名称：TSK_create
* 功能描述：创建任务
* 输入参数：pTskEntry		任务入口函数
*			ptAttrs 	任务属性
*			pParam		入口函数的参数
* 输出参数：无
* 返 回 值：任务句柄
* 其它说明： 
* 修改日期		   版本号	   修改人 	   修改内容
* -----------------------------------------------
* 2018/05/04	   V1.0		   hly2070		
***********************************************************************/
TSK_Handle TSK_create(Fxn pTskEntry, TSK_Attrs *ptAttrs, void *pParam);

/**********************************************************************
* 函数名称：TSK_delete
* 功能描述：删除任务
* 输入参数：hTsk		任务句柄
* 输出参数：无
* 返 回 值：无
* 其它说明：状态码
* 修改日期		   版本号	   修改人 	  修改内容
* -----------------------------------------------
* 2018/05/04	   V1.0		   hly2070		
***********************************************************************/
E_StateCode TSK_delete(TSK_Handle hTsk);

/**********************************************************************
* 函数名称：TSK_WaitforReady
* 功能描述：等待任务就绪
* 输入参数：hTsk		任务句柄
*			uiTimeOut 超时值，毫秒
* 输出参数：无
* 返 回 值：无
* 其它说明：状态码
* 修改日期		   版本号	   修改人 	  修改内容
* -----------------------------------------------
* 2018/05/04	   V1.0		   hly2070		
***********************************************************************/
E_StateCode TSK_WaitforReady(TSK_Handle hTsk, S32 uiTimeOut);

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

S32 FTC_SetTaskName(S8 *name);

#ifdef		__cplusplus
}
#endif

#endif


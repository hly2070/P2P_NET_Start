/**************************************************************************
* 版    权： 		Copyright (c) 2018 FTC.
* 文件名称：	FTCMutex.c
* 文件标识： 
* 内容摘要：	Mutex接口封装
* 其它说明： 
* 当前版本：	V1.0
* 作    者：		hly2070
* 完成日期：	2018年05月 06日
*
* 修改记录1	：
* 修改日期：
* 版 本 号：
* 修 改 人：
* 修改内容：
**************************************************************************/
#include <stdlib.h>
#include "FTCMutex.h"

/**********************************************************************
* 函数名称 :   FTC_MutexCreate
* 功能描述：创建锁
* 输入参数：无
* 输出参数： 无
* 返 回 值：锁句柄
* 其它说明： 
* 修改日期        版本号     修改人	      修改内容
* -----------------------------------------------
* 2018/05/06	  		V1.0		 	 hly2070
***********************************************************************/
HMUTEX FTC_MutexCreate()
{
	HMUTEX result = HMUTEX_INVALID;
	
	#ifdef __WIN32
		return CreateMutex(NULL,false,NULL);
	#else
		result = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
		if (NULL != result)
		{
			pthread_mutex_init(result, NULL);
		}
	#endif

	return result;
}

/**********************************************************************
* 函数名称：FTC_MutexDestroy
* 功能描述：销毁锁
* 输入参数：pHandle 锁句柄
* 输出参数： 无
* 返 回 值：无
* 其它说明： 
* 修改日期        版本号     修改人	      修改内容
* -----------------------------------------------
* 2018/05/06	  		V1.0		 	 hly2070 
***********************************************************************/
void FTC_MutexDestroy(HMUTEX pHandle)
{
	if (HMUTEX_INVALID == pHandle)
	{
		return ;
	}

	#ifdef __WIN32
		CloseHandle(pHandle);
	#else
		pthread_mutex_destroy(pHandle);
		free(pHandle);
		pHandle = HMUTEX_INVALID;
	#endif
}

/**********************************************************************
* 函数名称：FTC_MutexLock
* 功能描述：加锁
* 输入参数：pHandle 锁句柄
* 输出参数： 无
* 返 回 值：0:成功-1:失败
* 其它说明： 
* 修改日期        版本号     修改人	      修改内容
* -----------------------------------------------
* 2018/05/06	  		V1.0		 	 hly2070  
***********************************************************************/
S32 FTC_MutexLock(HMUTEX pHandle)
{
	S32 ret = -1;
	
	if (HMUTEX_INVALID == pHandle)
	{
		return ret;
	}

	#ifdef __WIN32
		DWORD r;
		r = WaitForSingleObject(pHandle,INFINITE);
		if (WAIT_OBJECT_0 == r)
		{
			ret = 0;
		}
	#else
		ret = pthread_mutex_lock(pHandle);
	#endif

	return ret;
}

/**********************************************************************
* 函数名称：FTC_MutexUnLock
* 功能描述：解锁
* 输入参数：pHandle 锁句柄
* 输出参数： 无
* 返 回 值：0:成功-1:失败
* 其它说明： 
* 修改日期        版本号     修改人	      修改内容
* -----------------------------------------------
* 2018/05/06	  		V1.0		 	 hly2070  
***********************************************************************/
S32 FTC_MutexUnLock(HMUTEX pHandle)
{
	S32 ret = -1;
	
	if (HMUTEX_INVALID == pHandle)
	{
		return ret;
	}

	#ifdef __WIN32
		DWORD r;
		r = ReleaseMutex(pHandle);
		if (WAIT_OBJECT_0 == r)
		{
			ret = 0;
		}
	#else
		ret = pthread_mutex_unlock(pHandle);
	#endif

	return ret;
}


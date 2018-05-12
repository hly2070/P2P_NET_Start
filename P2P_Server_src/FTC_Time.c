/**************************************************************************
* 版    权：Copyright (c) 2018 FTC.
* 文件名称：FTC_Time.c
* 文件标识： 
* 内容摘要： 
* 其它说明：时间操作接口函数
* 当前版本： 
* 作    者：hly2070
* 完成日期：2018年5月 11日
*
* 修改记录1	：
* 修改日期：
* 版 本 号：
* 修 改 人：
* 修改内容：
**************************************************************************/
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/times.h>
#include <stdio.h>
#include <unistd.h>
#include "FTC_Time.h"

/**********************************************************************
* 函数名称：FTC_AttachSignal
* 功能描述：关联信号处理函数
* 输入参数：uiSegId 	- 信号ID
			handler		- 处理函数
* 输出参数：无
* 返 回 值：状态码
* 其它说明： 
* 修改日期        版本号     修改人	      修改内容
* -----------------------------------------------
* 2018/05/11	  V1.0	     hly2070      
***********************************************************************/
E_StateCode FTC_AttachSignal(U32 uiSegId, void (*handler)(S32 ))
{
	struct sigaction tSigaction;

	/* insure a clean shutdown if user types ctrl-c */
	tSigaction.sa_handler = handler;
	sigemptyset(&tSigaction.sa_mask);
	tSigaction.sa_flags = 0;
	sigaction(uiSegId, &tSigaction, NULL);

	return STATE_CODE_NO_ERROR;
}

/**********************************************************************
* 函数名称：FTC_GetCurStartInMsec
* 功能描述：获取cpu 启动时间(ms)
* 输入参数：无
* 输出参数：无
* 返 回 值：返回当前时间
* 其它说明： 
* 修改日期        版本号     修改人	      修改内容
* -----------------------------------------------
* 2018/05/11	  V1.0	     hly2070      
***********************************************************************/
U32 FTC_GetCpuStartInMsec()
{
	U32 uiRet = 0;
	clock_t tNow = 0;
	static S32 niClkSec = 0;
	static S32 niMsPreClk = 0;
	struct tms tTms;

	if (0 == niClkSec)
	{
    	niClkSec = sysconf(_SC_CLK_TCK);
		if (-1 == niClkSec)
		{
			printf("OSAL_GetCpuStartInMsec call sysconf fail\n");
			niClkSec = 0;
		}
		else
		{
			niMsPreClk = 1000/niClkSec;
		}
	}

	if (0 != niClkSec)
	{
        tNow = times(&tTms);
		if (((clock_t)-1) != tNow)
		{
			uiRet = niMsPreClk * tNow;
		}
		else
		{
			printf("OSAL_GetCpuStartInMsec call times fail\n");
		}
	}
	
	return uiRet;
}

/**********************************************************************
* 函数名称：FTC_GetCurTimeInMsec
* 功能描述：获取当前的时间(ms)
* 输入参数：无
* 输出参数：无
* 返 回 值：返回当前时间
* 其它说明： 
* 修改日期        版本号      修改人	      修改内容
* -----------------------------------------------
* 2018/05/11	  V1.0	      hly2070      
***********************************************************************/
U32 FTC_GetCurTimeInMsec()
{
	struct timeval tv;

	if (gettimeofday(&tv, NULL) < 0) 
	{
		return 0;
	}

	return tv.tv_sec * 1000 + tv.tv_usec/1000;
}

/**********************************************************************
* 函数名称：FTC_Wait
* 功能描述：睡眠等待
* 输入参数：uiMS -毫秒数
* 输出参数：无
* 返 回 值：无
* 其它说明： 
* 修改日期        版本号      修改人	      修改内容
* -----------------------------------------------
* 2018/05/11	  V1.0	      hly2070      
***********************************************************************/
void FTC_Sleep(U32 uiMS)
{
	struct timespec tDelayTime;
	struct timespec tElaspedTime;

	tDelayTime.tv_sec  = uiMS / 1000;
	tDelayTime.tv_nsec = (uiMS % 1000) * 1000000;

	nanosleep(&tDelayTime, &tElaspedTime);
}



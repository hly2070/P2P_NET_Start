/**************************************************************************
* ��    Ȩ��Copyright (c) 2018 FTC.
* �ļ����ƣ�FTC_Time.c
* �ļ���ʶ�� 
* ����ժҪ�� 
* ����˵����ʱ������ӿں���
* ��ǰ�汾�� 
* ��    �ߣ�hly2070
* ������ڣ�2018��5�� 11��
*
* �޸ļ�¼1	��
* �޸����ڣ�
* �� �� �ţ�
* �� �� �ˣ�
* �޸����ݣ�
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
* �������ƣ�FTC_AttachSignal
* ���������������źŴ�����
* ���������uiSegId 	- �ź�ID
			handler		- ������
* �����������
* �� �� ֵ��״̬��
* ����˵���� 
* �޸�����        �汾��     �޸���	      �޸�����
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
* �������ƣ�FTC_GetCurStartInMsec
* ������������ȡcpu ����ʱ��(ms)
* �����������
* �����������
* �� �� ֵ�����ص�ǰʱ��
* ����˵���� 
* �޸�����        �汾��     �޸���	      �޸�����
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
* �������ƣ�FTC_GetCurTimeInMsec
* ������������ȡ��ǰ��ʱ��(ms)
* �����������
* �����������
* �� �� ֵ�����ص�ǰʱ��
* ����˵���� 
* �޸�����        �汾��      �޸���	      �޸�����
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
* �������ƣ�FTC_Wait
* ����������˯�ߵȴ�
* ���������uiMS -������
* �����������
* �� �� ֵ����
* ����˵���� 
* �޸�����        �汾��      �޸���	      �޸�����
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



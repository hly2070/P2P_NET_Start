/**************************************************************************
* ��    Ȩ�� 		Copyright (c) 2018 FTC.
* �ļ����ƣ�	FTCMutex.c
* �ļ���ʶ�� 
* ����ժҪ��	Mutex�ӿڷ�װ
* ����˵���� 
* ��ǰ�汾��	V1.0
* ��    �ߣ�		hly2070
* ������ڣ�	2018��05�� 06��
*
* �޸ļ�¼1	��
* �޸����ڣ�
* �� �� �ţ�
* �� �� �ˣ�
* �޸����ݣ�
**************************************************************************/
#include <stdlib.h>
#include "FTCMutex.h"

/**********************************************************************
* �������� :   FTC_MutexCreate
* ����������������
* �����������
* ��������� ��
* �� �� ֵ�������
* ����˵���� 
* �޸�����        �汾��     �޸���	      �޸�����
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
* �������ƣ�FTC_MutexDestroy
* ����������������
* ���������pHandle �����
* ��������� ��
* �� �� ֵ����
* ����˵���� 
* �޸�����        �汾��     �޸���	      �޸�����
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
* �������ƣ�FTC_MutexLock
* ��������������
* ���������pHandle �����
* ��������� ��
* �� �� ֵ��0:�ɹ�-1:ʧ��
* ����˵���� 
* �޸�����        �汾��     �޸���	      �޸�����
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
* �������ƣ�FTC_MutexUnLock
* ��������������
* ���������pHandle �����
* ��������� ��
* �� �� ֵ��0:�ɹ�-1:ʧ��
* ����˵���� 
* �޸�����        �汾��     �޸���	      �޸�����
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


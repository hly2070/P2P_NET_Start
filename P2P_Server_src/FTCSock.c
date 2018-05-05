/**************************************************************************
* ��    Ȩ�� 	Copyright (c) 2018 FTC.
* �ļ����ƣ�	FTCSock.c
* �ļ���ʶ�� 
* ����ժҪ��	socket�ӿڷ�װ
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
#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#include "FTCSock.h"

/**********************************************************************
* �������ƣ�FTC_InetAddr
* �����������ַ���ip��ַת����ip��ַ
* ���������pStrIp	�ַ���ip��ַָ��
* �����������
* �� �� ֵ������ip��ַ
* ����˵���� 
* �޸�����		  �汾��	 �޸��� 	  �޸�����
* -----------------------------------------------
* 2018/05/03	  V1.0		 hly2070	
***********************************************************************/
U32 FTC_InetAddr(S8 *pStrIp)
{
	return inet_addr(pStrIp);
}

/**********************************************************************
* �������ƣ�FTC_Htons
* ����������������������ת�����ֽ���
* ���������nsVal ������ֵ
* �����������
* �� �� ֵ��������ֵ
* ����˵���� 
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2018/05/04      V1.0	     hly2070  
***********************************************************************/
S16 FTC_Htons(S16 nsVal)
{
	return htons(nsVal);
}

static int CreateUdpSockUnix(unsigned int uiLocalIp, unsigned short usLocalPort)
{
	S32 sockfd = -1;
	struct sockaddr_in tLocalAddr;
	socklen_t len = sizeof(struct sockaddr_in);
	S32 flag = 0;
	S32 ret = 0;
	
    if (0xffffffff == uiLocalIp)
    {
    	fprintf(stderr, "MTCreateUdpSockUnix parameter invalid");
    	return sockfd;
    }

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (0 > sockfd)
	{
		fprintf(stderr, "create socket fail:%m");
		return sockfd;
	}

	tLocalAddr.sin_family = AF_INET;
	tLocalAddr.sin_addr.s_addr = uiLocalIp;
	tLocalAddr.sin_port = usLocalPort;

	if (0 != bind(sockfd, (struct sockaddr *)&tLocalAddr, len))
	{
		fprintf(stderr, "socket bind fail %m");
		close(sockfd);
		sockfd = -1;
		return sockfd;
	}

	/*����Ϊ������*/
	ret = -1;
	flag = fcntl(sockfd, F_GETFL);
	if (-1 != flag)
	{
	   ret = fcntl(sockfd, F_SETFL, flag|O_NONBLOCK);
	}

	if (0 != ret)
	{
		fprintf(stderr, "socket set nonblock fail %m");
		close(sockfd);
		sockfd = -1;
	}

	return sockfd;
}

/**********************************************************************
* �������ƣ�FTC_CreateUdpSock
* ��������������udp�׽ӿ�
* ���������uiLocalIp	   ����ip,�����ֽ���
*			usLocalPort    ���ض˿ڣ������ֽ���
* �����������
* �� �� ֵ���׽ӿ�������
* ����˵���� 
* �޸�����		  �汾��	 �޸��� 	  �޸�����
* -----------------------------------------------
* 2018/05/03	  V1.0		 hly2070	
***********************************************************************/
S32 FTC_CreateUdpSock(U32 uiLocalIp, U16 usLocalPort)
{
	S32 sockfd = -1;
	
	#ifdef __WIN32
	
	#else
		sockfd = CreateUdpSockUnix(uiLocalIp, usLocalPort);
	#endif

	return sockfd;
}

/**********************************************************************
* �������ƣ�FTC_SelectRead
* ������������ʱ�ȴ��׽ӿڿɶ�
* ���������niFd   �׽ӿ�������
*           niTimeout     �ȴ�ʱ�䣬��λ:΢��
* �����������
* �� �� ֵ��0:��ʾ�ɶ� -1:��ʾ���ɶ�
* ����˵���� 
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2018/05/04	  V1.0		 hly2070  
***********************************************************************/
S32 FTC_SelectRead(SOCKET niFd, S32 niTimeout)
{
	struct timeval tv;
	int ret = -1;
	fd_set tReadSet;

	if (0 > niFd)
	{
		return ret;
	}

	memset(&tv, 0, sizeof(tv));
	tv.tv_sec = niTimeout/1000000;
	tv.tv_usec = niTimeout%1000000;

	FD_ZERO(&tReadSet);
	FD_SET(niFd, &tReadSet);

	ret = select(niFd+1, &tReadSet, NULL, NULL, &tv);
	if (ret > 0)
	{
		if (FD_ISSET(niFd, &tReadSet))
		{
			ret = 0;
		}
	}
	else
	{
	/*	if ((-1 == ret) && (EINTR == errno))
		{
			ret = -2;  //�ź��жϣ���Ҫ�ϲ��ٴδ���
		}
		else
		{
			ret = -1;
		} */
		ret = -1;
	}

	return ret;
}

/**********************************************************************
* �������ƣ�FTC_Recvfrom
* ��������������udp����
* ���������sockfd   �׽ӿ�������
*           pBuf     �������ݻ�����
*           niSize   �������ݻ�������С
*           pRemoteIp  Զ��ip��ַָ��
*			pRemotePort Զ��portָ��
* ���������pBuf  �������ݻ�����
*      		pRemoteIp Զ��ip��ַ�������ֽ���
*			pRemotePort Զ�˶˿ڣ������ֽ���
* �� �� ֵ���������ݳ��ȣ�-1:��ʾʧ��
* ����˵���� 
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2018/05/04	  V1.0		 hly2070  
***********************************************************************/
S32 FTC_Recvfrom(SOCKET sockfd, S8 *pBuf, S32 niSize, U32 *pRemoteIp, U16 *pRemotePort)
{
	S32 ret = 0;
	struct sockaddr_in tRem;
	socklen_t tLen = sizeof(struct sockaddr_in);
	
	if ((0 > sockfd) || (NULL == pBuf) || (0 >= niSize))
	{
		return ret;
	}

	memset(&tRem, 0, sizeof(tRem));
	tRem.sin_family = AF_INET;
    do
    {
		ret = recvfrom(sockfd, pBuf, niSize, 0, (struct sockaddr *)&tRem, &tLen);
		if (-1 == ret)
		{
			continue;
		}
		else
		{
			break;
		}
    }while (1);
	
	if (0 < ret)
	{
		if (NULL != pRemoteIp)
		{
			memcpy(pRemoteIp, &tRem.sin_addr.s_addr, sizeof(int));
		}

		if (NULL != pRemotePort)
		{
			memcpy(pRemotePort, &tRem.sin_port, sizeof(short));
		}

		#ifdef DEBUG
			int niCount = 0;
			fprintf(stderr, "MSGTRANS RECV:\n");
			for (niCount=0; niCount < ret; niCount++)
			{
				fprintf(stderr, "0x%02x ", (unsigned char)pBuf[niCount]);
				if ((niCount+1)%16 == 0)
				{
					fprintf(stderr, "\n");
				}
			}
			fprintf(stderr, "\n");
		#endif
	}

	return ret;
}

/**********************************************************************
* �������ƣ�FTC_Recvfrom2
* ��������������udp����
* ���������sockfd   �׽ӿ�������
*           pBuf     �������ݻ�����
*           niSize   �������ݻ�������С
*           readAddr ����Զ��socket��ַ
* ���������pBuf  �������ݻ�����
*      		readAddr ����Զ��socket��ַ
* �� �� ֵ���������ݳ��ȣ�-1:��ʾʧ��
* ����˵���� 
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2018/05/04	  V1.0		 hly2070  
***********************************************************************/
S32 FTC_Recvfrom2(SOCKET sockfd, S8 *pBuf, S32 niSize, struct sockaddr_in *readAddr)
{
	S32 ret = 0;
	socklen_t tLen = sizeof(struct sockaddr_in);
	
	if ((0 > sockfd) || (NULL == pBuf) || (0 >= niSize))
	{
		return ret;
	}

	readAddr->sin_family = AF_INET;
    do
    {
		ret = recvfrom(sockfd, pBuf, niSize, 0, (struct sockaddr *)readAddr, &tLen);
		if (-1 == ret)
		{
			continue;
		}
		else
		{
			break;
		}
    }while (1);

	return ret;
}

/**********************************************************************
* �������ƣ�FTC_Sendto
* ��������������udp����
* ���������sockfd   �׽ӿ�������
*           pBuf      �������ݻ�����
*			niSize    ���������ֽ���
*			niRemoteIp  Ŀ�Ķ�ip, �����ֽ���
*           usRemotePort Ŀ�Ķ˶˿ڣ������ֽ���
* �����������
* �� �� ֵ���������ݳ���
* ����˵���� 
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2018/05/05	  V1.0		 hly2070  
***********************************************************************/
S32 FTC_Sendto(SOCKET sockfd, S8*pBuf, S32 niSize, U32 niRemoteIp, U16 usRemotePort)
{
	S32 ret = -1;
	struct sockaddr_in tRem;
	socklen_t len = sizeof(struct sockaddr_in);

	if ((0 > sockfd) || (NULL == pBuf) || (0 >= niSize))
	{
		return ret;
	}

#ifdef DEBUG
	int niCount = 0;
	fprintf(stderr, "MSGTRANS SEND:\n");
	for (niCount=0; niCount < niSize; niCount++)
	{
		fprintf(stderr, "0x%02x ", (unsigned char)pBuf[niCount]);
		if ((niCount+1)%16 == 0)
		{
			fprintf(stderr, "\n");
		}
	}
	fprintf(stderr, "\n");
#endif

	memset(&tRem, 0, sizeof(tRem));
	tRem.sin_family = AF_INET;
	tRem.sin_addr.s_addr = niRemoteIp;
	tRem.sin_port = usRemotePort;
	do
	{
		ret = sendto(sockfd, pBuf, niSize, 0, (struct sockaddr *)&tRem, len);
		if (-1 == ret)
		{
			continue;
		}
		else
		{
			break;
		}
    } while (1);
	return ret;
}

/**********************************************************************
* �������ƣ�FTC_Sendto2
* ��������������udp����
* ���������sockfd   �׽ӿ�������
*           pBuf      �������ݻ�����
*			niSize    ���������ֽ���
*			remoteAddr Ŀ���ַ
* �����������
* �� �� ֵ���������ݳ���
* ����˵���� 
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2018/05/05	  V1.0		 hly2070  
***********************************************************************/
S32 FTC_Sendto2(SOCKET sockfd, S8*pBuf, S32 niSize, struct sockaddr_in *remoteAddr)
{
	S32 ret = -1;
	socklen_t len = sizeof(struct sockaddr_in);

	if ((0 > sockfd) || (NULL == pBuf) || (0 >= niSize))
	{
		return ret;
	}

#ifdef DEBUG
	int niCount = 0;
	fprintf(stderr, "MSGTRANS SEND:\n");
	for (niCount=0; niCount < niSize; niCount++)
	{
		fprintf(stderr, "0x%02x ", (unsigned char)pBuf[niCount]);
		if ((niCount+1)%16 == 0)
		{
			fprintf(stderr, "\n");
		}
	}
	fprintf(stderr, "\n");
#endif

	do
	{
		ret = sendto(sockfd, pBuf, niSize, 0, (struct sockaddr *)remoteAddr, len);
		if (-1 == ret)
		{
			continue;
		}
		else
		{
			break;
		}
    } while (1);
	return ret;
}

/**********************************************************************
* �������ƣ�FTC_CloseSock
* �����������ر��׽ӿ�
* ���������niSockfd   �׽ӿ�������
* �����������
* �� �� ֵ����
* ����˵���� 
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2018/05/04	  V1.0		 hly2070  
***********************************************************************/
void FTC_CloseSock(SOCKET niSockfd)
{
	if (0 <= niSockfd)
	{
		close(niSockfd);
	}
}



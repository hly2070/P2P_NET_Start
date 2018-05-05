/**************************************************************************
* 版    权： 	Copyright (c) 2018 FTC.
* 文件名称：	FTCSock.c
* 文件标识： 
* 内容摘要：	socket接口封装
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
#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#include "FTCSock.h"

/**********************************************************************
* 函数名称：FTC_InetAddr
* 功能描述：字符串ip地址转整形ip地址
* 输入参数：pStrIp	字符串ip地址指针
* 输出参数：无
* 返 回 值：整形ip地址
* 其它说明： 
* 修改日期		  版本号	 修改人 	  修改内容
* -----------------------------------------------
* 2018/05/03	  V1.0		 hly2070	
***********************************************************************/
U32 FTC_InetAddr(S8 *pStrIp)
{
	return inet_addr(pStrIp);
}

/**********************************************************************
* 函数名称：FTC_Htons
* 功能描述：短整形主机序转网络字节序
* 输入参数：nsVal 主机序值
* 输出参数：无
* 返 回 值：网络序值
* 其它说明： 
* 修改日期        版本号     修改人	      修改内容
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

	/*设置为非阻塞*/
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
* 函数名称：FTC_CreateUdpSock
* 功能描述：创建udp套接口
* 输入参数：uiLocalIp	   本地ip,网络字节序
*			usLocalPort    本地端口，网络字节序
* 输出参数：无
* 返 回 值：套接口描述符
* 其它说明： 
* 修改日期		  版本号	 修改人 	  修改内容
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
* 函数名称：FTC_SelectRead
* 功能描述：超时等待套接口可读
* 输入参数：niFd   套接口描述符
*           niTimeout     等待时间，单位:微妙
* 输出参数：无
* 返 回 值：0:表示可读 -1:表示不可读
* 其它说明： 
* 修改日期        版本号     修改人	      修改内容
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
			ret = -2;  //信号中断，需要上层再次触发
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
* 函数名称：FTC_Recvfrom
* 功能描述：接收udp数据
* 输入参数：sockfd   套接口描述符
*           pBuf     接收数据缓冲区
*           niSize   接收数据缓冲区大小
*           pRemoteIp  远端ip地址指针
*			pRemotePort 远端port指针
* 输出参数：pBuf  接收数据缓冲区
*      		pRemoteIp 远端ip地址，网络字节序
*			pRemotePort 远端端口，网络字节序
* 返 回 值：接收数据长度，-1:表示失败
* 其它说明： 
* 修改日期        版本号     修改人	      修改内容
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
* 函数名称：FTC_Recvfrom2
* 功能描述：接收udp数据
* 输入参数：sockfd   套接口描述符
*           pBuf     接收数据缓冲区
*           niSize   接收数据缓冲区大小
*           readAddr 保存远端socket地址
* 输出参数：pBuf  接收数据缓冲区
*      		readAddr 保存远端socket地址
* 返 回 值：接收数据长度，-1:表示失败
* 其它说明： 
* 修改日期        版本号     修改人	      修改内容
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
* 函数名称：FTC_Sendto
* 功能描述：发送udp数据
* 输入参数：sockfd   套接口描述符
*           pBuf      发送数据缓冲区
*			niSize    发送数据字节数
*			niRemoteIp  目的端ip, 网络字节序
*           usRemotePort 目的端端口，网络字节序
* 输出参数：无
* 返 回 值：发送数据长度
* 其它说明： 
* 修改日期        版本号     修改人	      修改内容
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
* 函数名称：FTC_Sendto2
* 功能描述：发送udp数据
* 输入参数：sockfd   套接口描述符
*           pBuf      发送数据缓冲区
*			niSize    发送数据字节数
*			remoteAddr 目标地址
* 输出参数：无
* 返 回 值：发送数据长度
* 其它说明： 
* 修改日期        版本号     修改人	      修改内容
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
* 函数名称：FTC_CloseSock
* 功能描述：关闭套接口
* 输入参数：niSockfd   套接口描述符
* 输出参数：无
* 返 回 值：无
* 其它说明： 
* 修改日期        版本号     修改人	      修改内容
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



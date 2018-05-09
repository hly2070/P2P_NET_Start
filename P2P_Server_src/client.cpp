/**************************************************************************
* 版    权： 	Copyright (c) 2018 FTC.
* 文件名称：	server.cpp
* 文件标识： 
* 内容摘要：	P2P peer function define
* 其它说明： 
* 当前版本：	V1.0
* 作    者：	hly2070
* 完成日期：	2018年05月 09日
*
* 修改记录1	：
* 修改日期：
* 版 本 号：
* 修 改 人：
* 修改内容：
**************************************************************************/
#include <cassert>
#include <cstring>
#include <iostream>
#include <cstdlib>
#ifndef WIN32
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wspiapi.h>
#endif
#include <iostream>
//#include <udt.h>
//#include "udp.h"
//#include "stun.h"
#include "FTCDebug.h"
#include "FTCSock.h"
#include "FTCMessage.h"

#define SERVER_IP	"43.252.231.67"
#define SERVER_PORT	(8888)
#define LAN_PORT	(8889)

using namespace std;

SOCKET sockCli;		/* P2P通信socket */
SOCKET sockLocal;	/* 局域网通信socket */
struct sockaddr_in remoteAddr;		/* 远端通信地址 */

void usage()
{
//	printf("usage: ./client -u name -s serverip -p severport\n");
	printf("usage: ./client -u myName -r targetName\n");
}

S8 LoginToServer(S8 *strMyName, S8 *strLanIp, S8 *strID, U16 uLanPort)
{
	T_Msg stMsg;
	T_MsgLoginReq stLoginMsg;

	if((strMyName == NULL) || (strLanIp == NULL) || (strID == NULL))
	{
		P2P_DBG_ERROR("Invalid input!");
	}

//	memset(&stMsg, 0, sizeof(T_Msg));
//	memset(&stLoginMsg, 0, sizeof(T_MsgLoginReq));

	strcpy(stLoginMsg.name, strMyName);
	strcpy(stLoginMsg.MyLanIP, strLanIp);
	strcpy(stLoginMsg.ID, strID);
	stLoginMsg.MyLanPort = uLanPort;

	stMsg.tMsgHead.uiMsgType = MSG_TYPE_REQUEST;
	stMsg.tMsgHead.uiMsgId = MSG_C_LOGIN;
	stMsg.tMsgHead.usParaLength = sizeof(T_MsgLoginReq);
	memcpy((T_MsgLoginReq *)stMsg.aucParam, &stLoginMsg, sizeof(stLoginMsg));

	FTC_Sendto2(sockCli, (S8 *)&stMsg, sizeof(stMsg), &remoteAddr);
	
	return 0;
}
	
int main(int argc, char* argv[])
{
	U16 niIndex = 1;
	S8 sInput[20];
//	U16 uiSrvPort;
//	S8 sSrvIP[16];
	S8 sMyName[32];
	S8 sToName[32];

	if(argc != 5) // if 7 set p2p server ip and port by manual.
	{
		usage();
		return -1;
	}
	
	/*命令行解析*/
	while(niIndex < argc)
	{
		if (0 == strncmp(argv[niIndex], "-u", 2))
		{
			niIndex++;
			if (niIndex < argc)
			{
				strcpy(sMyName, argv[niIndex]);
			}
		}
		else if (0 == strncmp(argv[niIndex], "-r", 2))
		{
			niIndex++;
			if (niIndex < argc)
			{
				strcpy(sToName, argv[niIndex]);
			}
		}
	/*	else if (0 == strncmp(argv[niIndex], "-s", 2))
		{
			niIndex++;
			if (niIndex < argc)
			{
				strcpy(sSrvIP, argv[niIndex]);
			}
		}
		else if (0 == strncmp(argv[niIndex], "-p", 2))
		{
			niIndex++;
			if (niIndex < argc)
			{
				uiSrvPort = (U16)atoi(argv[niIndex]);
			}
		} */ //set p2p server ip and port by manual.
		else
		{
		//	fprintf(stderr, "unknow cmdline parameter:%s\n", argv[niIndex]);
			usage();
		}

		niIndex++;
	}

	sockCli = FTC_CreateUdpSock(FTC_InetAddr("0.0.0.0"), FTC_Htons(0));
	if (0 > sockCli)
	{
		P2P_DBG_ERROR("FTC_CreateUdpSock create p2p udp sock fail.");
		return -1;
	}

	sockLocal = FTC_CreateUdpSock(FTC_InetAddr("0.0.0.0"), FTC_Htons(LAN_PORT));
	if (0 > sockLocal)
	{
		P2P_DBG_ERROR("FTC_CreateUdpSock create lan mode udp sock fail.");
		return -1;
	}

	//init remote addr
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = FTC_Htons(SERVER_PORT); 	//FTC_Htons(uiSrvPort);
	remoteAddr.sin_addr.s_addr  = FTC_InetAddr(SERVER_IP);	//FTC_InetAddr(sSrvIP);

	printf("input your command : ");

	while(fgets(sInput, 20, stdin))
	{
		if(strncmp(sInput, "login", 5) == 0)
		{
			P2P_DBG_DEBUG("send login request to server.");
			LoginToServer(sMyName, "192.168.1.165", "AAAAAAAA", LAN_PORT);
		}
		else
		{
			printf("Invalid command!!!\n");
		}
		
		usleep(100000);
	}
	
	return 0;
}


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
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
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
#include "FTCTask.h"
#include "FTCPeer.h"

#define SERVER_IP	"43.252.231.67"
#define SERVER_PORT	(8888)
#define LAN_PORT	(8889)

using namespace std;

typedef struct
{
	BOOL bIsLogin;
	U8 bCorD;		/* 0:client 1:device */
	S8 ID[8];
	S8 sMyName[MAX_NAME_SIZE];
	S8 sToName[MAX_NAME_SIZE];
	SOCKET sockCli;		/* P2P通信socket */
	SOCKET sockLocal;	/* 局域网通信socket */
	struct sockaddr_in serverAddr;		/* 远端通信地址 */
//	PeerList ClientList;
}T_PeerLocal;

T_PeerLocal stPeerLocal;
PeerList ClientList;

static S8 DoInput();

void usage()
{
//	printf("usage: ./client -u name -s serverip -p severport\n");
	printf("usage: ./client -c(d) -u myName -r targetName\n");
	printf("-c:client -d:device -u:youname -r:remotename\n");
}

S8 LoginToServer(S8 *strMyName, S8 *strLanIp, S8 *strID, U16 uLanPort)
{
	T_Msg stMsg;
	T_MsgLoginReq stLoginMsg;

	if((strMyName == NULL) || (strLanIp == NULL) || (strID == NULL))
	{
		P2P_DBG_ERROR("Invalid input!");
	}

	memset(&stMsg, 0, sizeof(T_Msg));
	memset(&stLoginMsg, 0, sizeof(T_MsgLoginReq));

	strcpy(stLoginMsg.name, strMyName);
	strcpy(stLoginMsg.MyLanIP, strLanIp);
	strcpy(stLoginMsg.ID, strID);
	stLoginMsg.MyLanPort = uLanPort;
	stLoginMsg.bCorD = stPeerLocal.bCorD;

	stMsg.tMsgHead.uiMsgType = MSG_TYPE_REQUEST;
	stMsg.tMsgHead.uiMsgId = MSG_C_LOGIN;
	stMsg.tMsgHead.usParaLength = sizeof(T_MsgLoginReq);
	memcpy((T_MsgLoginReq *)stMsg.aucParam, &stLoginMsg, sizeof(stLoginMsg));

	FTC_Sendto2(stPeerLocal.sockCli, (S8 *)&stMsg, sizeof(stMsg), &stPeerLocal.serverAddr);
	
	return 0;
}

int get_ip_addr(char *name, char *net_ip)
{
	struct ifreq ifr;
	int ret = 0;
	int fd; 
	
	strcpy(ifr.ifr_name, name);
	ifr.ifr_addr.sa_family = AF_INET;
	
	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		ret = -1;
		
	if (ioctl(fd, SIOCGIFADDR, &ifr) < 0) 
	{
		ret = -1;
	}
	
	strcpy(net_ip, inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr));

	close(fd);

	printf("get_ip_addr net_ip:%s\n",net_ip);
	return	ret;
}

void * SendHeartbeat(void *arg)
{
	T_Msg stMsg;

	memset(&stMsg, 0, sizeof(T_Msg));

	stMsg.tMsgHead.uiMsgType = MSG_TYPE_REQUEST;
	stMsg.tMsgHead.uiMsgId = MSG_C_HEART_BEAT;
	stMsg.tMsgHead.usParaLength = strlen(stPeerLocal.sMyName);
	strcpy((S8 *)stMsg.aucParam, stPeerLocal.sMyName);

	FTC_PTHREAD_DETACH;
	
	while(1)
	{
		usleep(30000000);
		
		if(stPeerLocal.bIsLogin)
			FTC_Sendto2(stPeerLocal.sockCli, (S8 *)&stMsg, sizeof(stMsg), &stPeerLocal.serverAddr);
		
	}

	FTC_PTHREAD_EXIT;
}

void *P2PClientProc(void *arg)
{
	S32 ret;
	S32 niTimeout = 0;
	struct sockaddr_in fromAddr;
	T_Msg stMsg;
	U32 mMsgID = 0;
	T_PeerInfo tmpPeer;

	memset(&fromAddr, 0, sizeof(fromAddr));
	memset(&stMsg, 0, sizeof(T_Msg));
	
	FTC_PTHREAD_DETACH;

	while(1)
	{
		if(0 == FTC_SelectRead(stPeerLocal.sockCli, niTimeout))
		{
			S32 ret = FTC_Recvfrom2(stPeerLocal.sockCli, (S8 *)&stMsg, sizeof(stMsg), &fromAddr);
			if (ret < 0)
			{
				continue;
			}
			else
			{
				mMsgID = stMsg.tMsgHead.uiMsgId;
				
				switch (mMsgID)
				{
					case MSG_R_LOGIN:
						{
							T_MsgLoginResp *ptLoginMsg;
							ptLoginMsg = (T_MsgLoginResp *)stMsg.aucParam;

							if(ptLoginMsg->result == 0)
							{
								printf("Login P2P server seccuss!\n");
								stPeerLocal.bIsLogin = TRUE;
							}
							else
							{
								printf("Login P2P server failed!\n");
								stPeerLocal.bIsLogin = FALSE;
							}
						}
						break;
						
					case MSG_R_GET_PEERS:
						{
							T_MsgGetPeerListResp *ptSubMsg;
							ptSubMsg = (T_MsgGetPeerListResp *)stMsg.aucParam;

							printf("%d peers on server:\n", ptSubMsg->uiPeerNums);

							if(ClientList.empty()!=NULL && ClientList.size() > 0)
							{
								ClientList.clear();
							}
							
							for(int i=0; i<ptSubMsg->uiPeerNums; i++)
							{
							//	printf("%s\n", ptSubMsg->peerList[i].name);
								
								memset(&tmpPeer, 0, sizeof(T_PeerInfo));
								
								strcpy(tmpPeer.name, ptSubMsg->peerList[i].name);
								strcpy(tmpPeer.ID, ptSubMsg->peerList[i].ID);
								tmpPeer.bCorD = ptSubMsg->peerList[i].bCorD;
								strcpy(tmpPeer.sPubIp, ptSubMsg->peerList[i].sPubIp);
								tmpPeer.usPubPort = ptSubMsg->peerList[i].usPubPort;
								strcpy(tmpPeer.sLanIp, ptSubMsg->peerList[i].sLanIp);
								tmpPeer.usLanPort = ptSubMsg->peerList[i].usLanPort;
								
								printf("%s\n", tmpPeer.name);
								
								ClientList.push_back(&tmpPeer);
							}
						}
						break;

					case MSG_R_HOLE:
						{
							T_MsgHoleResp *pHoleResp;
							pHoleResp = (T_MsgHoleResp*)stMsg.aucParam;

							if(pHoleResp->result == 1)
							{
								printf("P2P hole failed!\n");
							}
						}
						break;

					case MSG_C_HOLE_REQ:
						{
							T_MsgHoleFromSrvReq *pMsg;
							pMsg = (T_MsgHoleFromSrvReq*)stMsg.aucParam;
							
							printf("%s[%s:%d] want to connect to you with p2p.\n", pMsg->srcName, pMsg->srcPubIP, pMsg->srcPubPort);

							sockaddr_in remote;
							remote.sin_family = AF_INET;
							remote.sin_addr.s_addr = FTC_InetAddr(pMsg->srcPubIP);
							remote.sin_port = FTC_Htons(pMsg->srcPubPort);

							//发送P2P  消息给对端
							T_Msg stMsgSend;
							T_MsgP2PConnReq stMsgSub;

							memset(&stMsgSend, 0, sizeof(T_Msg));
							memset(&stMsgSub, 0, sizeof(T_MsgHoleFromSrvReq));

							strcpy(stMsgSub.myName, stPeerLocal.sMyName);

							stMsgSend.tMsgHead.uiMsgType = MSG_TYPE_REQUEST;
							stMsgSend.tMsgHead.uiMsgId = MSG_C_P2P_CONN_REQ;
							stMsgSend.tMsgHead.usParaLength = sizeof(stMsgSub);
							memcpy(stMsgSend.aucParam, &stMsgSub, sizeof(stMsgSub));

							FTC_Sendto2(stPeerLocal.sockCli, (S8*)&stMsgSend, sizeof(stMsgSend), &remote);

							usleep(50000);

							//通知服务端已给对方发送了打洞消息
							T_MsgHaveSendP2PReq stMsgSub2;

							memset(&stMsgSend, 0, sizeof(T_Msg));
							memset(&stMsgSub2.toName, 0, sizeof(T_MsgHaveSendP2PReq));

							strcpy(stMsgSub2.myName, stPeerLocal.sMyName);
							strcpy(stMsgSub2.toName, stPeerLocal.sToName);

							stMsgSend.tMsgHead.uiMsgType = MSG_TYPE_INDICATE;
							stMsgSend.tMsgHead.uiMsgId = MSG_C_HAVE_SEND_P2P_REQ;
							stMsgSend.tMsgHead.usParaLength = sizeof(stMsgSub2);
							memcpy(stMsgSend.aucParam, &stMsgSub2, sizeof(stMsgSub2));
							
							FTC_Sendto2(stPeerLocal.sockCli, (S8*)&stMsgSend, sizeof(stMsgSend), &fromAddr);
						}
						break;

					case MSG_C_P2P_CONN_REQ:
						{
							T_MsgP2PConnReq *ptMsgSub;
							ptMsgSub = (T_MsgP2PConnReq*)stMsg.aucParam;

							printf("recv P2P connect request from %s [%s:%d]\n", ptMsgSub->myName, inet_ntoa(fromAddr.sin_addr), 
								htons(fromAddr.sin_port));
						}
						break;

					case MSG_C_P2P_CAN_START_REQ:
						{
							T_MsgP2PCanStartReq *ptMsgSub;
							ptMsgSub = (T_MsgP2PCanStartReq*)stMsg.aucParam;

							T_PeerInfo toPeer = GetPeerByName(&ClientList, ptMsgSub->myName);
								
							sockaddr_in remote;
							remote.sin_family = AF_INET;
							remote.sin_addr.s_addr = FTC_InetAddr(toPeer.sPubIp);
							remote.sin_port = FTC_Htons(toPeer.usPubPort);

							T_Msg stSendMsg;
							T_MsgP2PStartReq stSubMsg;

							memset(&stSendMsg, 0, sizeof(T_Msg));
							memset(&stSubMsg, 0, sizeof(T_MsgP2PStartReq));

							strcpy(stSubMsg.myName, stPeerLocal.sMyName);
							stSendMsg.tMsgHead.uiMsgType = MSG_TYPE_REQUEST;
							stSendMsg.tMsgHead.uiMsgId = MSG_C_P2P_START_REQ;
							stSendMsg.tMsgHead.usParaLength = sizeof(stSubMsg);
							memcpy(stSendMsg.aucParam, &stSubMsg, sizeof(stSubMsg));

							FTC_Sendto2(stPeerLocal.sockCli, (S8*)&stSendMsg, sizeof(stSendMsg), &remote);
						}
						break;

					case MSG_C_P2P_START_REQ:
						{
							T_MsgP2PStartReq *ptMsgSub;
							ptMsgSub = (T_MsgP2PStartReq*)stMsg.aucParam;

							T_Msg stSendMsg;
							T_MsgP2PStartResp stSubMsg;

							memset(&stSendMsg, 0, sizeof(T_Msg));
							memset(&stSubMsg, 0, sizeof(T_MsgP2PStartResp));
							
							stSendMsg.tMsgHead.uiMsgType = MSG_TYPE_RESPONSE;
							stSendMsg.tMsgHead.uiMsgId = MSG_R_P2P_START_RESP;
							stSendMsg.tMsgHead.usParaLength = sizeof(stSubMsg);
							memcpy(stSendMsg.aucParam, &stSubMsg, sizeof(stSubMsg));

							FTC_Sendto2(stPeerLocal.sockCli, (S8*)&stSendMsg, sizeof(stSendMsg), &fromAddr);
						}
						break;

					case MSG_R_P2P_START_RESP:
						{
							printf("P2P connect ok!\n");
						}
						break;

					case MSG_R_LOGOUT:
						{
							T_MsgLogoutResp *ptSubMsg;
							ptSubMsg = (T_MsgLogoutResp*)stMsg.aucParam;

							if(ptSubMsg->result == 0)
							{
								printf("logout done!\n");
								stPeerLocal.bIsLogin = FALSE;
							}
						}
						break;
						
					default:
						printf("recv msgid:%d\n",mMsgID);
						break;
				}
			}
		}
		else
		{
			usleep(100000);
			continue;
		}
		
		usleep(100000);
	}
	
	FTC_PTHREAD_EXIT;
}

S8 GetPeerListFromServer()
{
	T_Msg stMsg;
	T_MsgGetPeerListReq stGetPeersMsg;

	memset(&stMsg, 0, sizeof(T_Msg));
	memset(&stGetPeersMsg, 0, sizeof(T_MsgGetPeerListReq));

	strcpy(stGetPeersMsg.name, stPeerLocal.sMyName);

	stMsg.tMsgHead.uiMsgType = MSG_TYPE_REQUEST;
	stMsg.tMsgHead.uiMsgId = MSG_C_GET_PEERS;
	stMsg.tMsgHead.usParaLength = sizeof(T_MsgGetPeerListReq);
	memcpy((T_MsgGetPeerListReq *)stMsg.aucParam, &stGetPeersMsg, sizeof(stGetPeersMsg));

	FTC_Sendto2(stPeerLocal.sockCli, (S8 *)&stMsg, sizeof(stMsg), &stPeerLocal.serverAddr);
	
	return 0;
}

S8 SendP2PHoleMsg()
{	
	T_Msg stMsg;
	T_MsgHoleReq stHoleMsg;

	memset(&stMsg, 0, sizeof(T_Msg));
	memset(&stHoleMsg, 0, sizeof(T_MsgHoleReq));

	strcpy(stHoleMsg.myName, stPeerLocal.sMyName);
	strcpy(stHoleMsg.toName, stPeerLocal.sToName);

	stMsg.tMsgHead.uiMsgType = MSG_TYPE_REQUEST;
	stMsg.tMsgHead.uiMsgId = MSG_C_HOLE;
	stMsg.tMsgHead.usParaLength = sizeof(T_MsgHoleReq);
	memcpy((T_MsgHoleReq *)stMsg.aucParam, &stHoleMsg, sizeof(stHoleMsg));

	FTC_Sendto2(stPeerLocal.sockCli, (S8 *)&stMsg, sizeof(stMsg), &stPeerLocal.serverAddr);
	printf("P2P holeing, wait...\n");
	
	return 0;
}

S8 SendLogoutMsg()
{
	T_Msg stMsg;
	T_MsgLogoutReq stSubMsg;

	memset(&stMsg, 0, sizeof(T_Msg));
	memset(&stSubMsg, 0, sizeof(T_MsgLogoutReq));

	strcpy(stSubMsg.myName, stPeerLocal.sMyName);

	stMsg.tMsgHead.uiMsgType = MSG_TYPE_REQUEST;
	stMsg.tMsgHead.uiMsgId = MSG_C_LOGOUT;
	stMsg.tMsgHead.usParaLength = sizeof(stSubMsg);
	memcpy((T_MsgLogoutReq *)stMsg.aucParam, &stSubMsg, sizeof(stSubMsg));

	FTC_Sendto2(stPeerLocal.sockCli, (S8 *)&stMsg, sizeof(stMsg), &stPeerLocal.serverAddr);
	
	return 0;
}

static S8 DoInput()
{
	S8 sInput[20];
	
	printf("input your command : ");
	fgets(sInput, 20, stdin);
	if(NULL != sInput)
	{
		if(strncmp(sInput, "login", 5) == 0)
		{
			P2P_DBG_DEBUG("send login request to server.");
			char strLanIp[16] = {0};
			get_ip_addr("eth0", strLanIp);
			LoginToServer(stPeerLocal.sMyName, strLanIp, stPeerLocal.ID, LAN_PORT);
		}
		else if(strncmp(sInput, "get", 3) == 0)
		{
			P2P_DBG_DEBUG("send get peer list request to server.");
			GetPeerListFromServer();
		}
		else if(strncmp(sInput, "connect", 7) == 0)
		{
			P2P_DBG_DEBUG("send p2p hole request to %s.", stPeerLocal.sToName);
			SendP2PHoleMsg();
		}
		else if(strncmp(sInput, "logout", 6) == 0)
		{
			P2P_DBG_DEBUG("send logout message to server.");
			SendLogoutMsg();
		}
		else
		{
			printf("Invalid command!!!\n");
		}
	}
}

int main(int argc, char* argv[])
{
	S32 ret;
	U16 niIndex = 1;
	S8 sInput[20];
//	U16 uiSrvPort;
//	S8 sSrvIP[16];
//	S8 sMyName[32];
//	S8 sToName[32];

	if(argc != 5) // if 7 set p2p server ip and port by manual.
	{
		usage();
		return -1;
	}

	memset(&stPeerLocal, 0, sizeof(T_PeerLocal));
	stPeerLocal.bIsLogin = FALSE;
	strcpy(stPeerLocal.ID, "AAAAAAA");
	
	/*命令行解析*/
	while(niIndex < argc)
	{
		if (0 == strncmp(argv[niIndex], "-u", 2))	/* local peer name */
		{
			niIndex++;
			if (niIndex < argc)
			{
				strcpy(stPeerLocal.sMyName, argv[niIndex]);
			}
		}
		else if (0 == strncmp(argv[niIndex], "-r", 2))	/* remote peer name */
		{
			niIndex++;
			if (niIndex < argc)
			{
				strcpy(stPeerLocal.sToName, argv[niIndex]);
			}
		}
		else if (0 == strncmp(argv[niIndex], "-d", 2))  /* i am a device */
		{
			niIndex++;
			if (niIndex < argc)
			{
				stPeerLocal.bCorD = 1;
			}
		}
		else if (0 == strncmp(argv[niIndex], "-c", 2)) /* i am a client */
		{
			niIndex++;
			if (niIndex < argc)
			{
				stPeerLocal.bCorD = 0;
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
	
	stPeerLocal.sockCli = FTC_CreateUdpSock(FTC_InetAddr("0.0.0.0"), FTC_Htons(0));
	if (0 > stPeerLocal.sockCli)
	{
		P2P_DBG_ERROR("FTC_CreateUdpSock create p2p udp sock fail.");
		return -1;
	}

	stPeerLocal.sockLocal = FTC_CreateUdpSock(FTC_InetAddr("0.0.0.0"), FTC_Htons(LAN_PORT));
	if (0 > stPeerLocal.sockLocal)
	{
		P2P_DBG_ERROR("FTC_CreateUdpSock create lan mode udp sock fail.");
		return -1;
	}

	/* start hert beat thread. */
	FTC_CREATE_THREADEX(SendHeartbeat, NULL, ret); 
	if (FALSE == ret)
	{
    		P2P_DBG_ERROR("SendHeartbeat start failed!");
		return -1;
	}

	//p2p server addr
	stPeerLocal.serverAddr.sin_family = AF_INET;
	stPeerLocal.serverAddr.sin_addr.s_addr  = FTC_InetAddr(SERVER_IP);	//FTC_InetAddr(sSrvIP);
	stPeerLocal.serverAddr.sin_port = FTC_Htons(SERVER_PORT); 	//FTC_Htons(uiSrvPort);

	/* P2P message recv proc */
	FTC_CREATE_THREADEX(P2PClientProc, NULL, ret); 
	if (FALSE == ret)
	{
    	P2P_DBG_ERROR("P2PClientProc start failed!");
		return -1;
	} 
	
	/* main function*/
	
	while(1)
	{
		DoInput();
		usleep(100000);
	}
	
	return 0;
}


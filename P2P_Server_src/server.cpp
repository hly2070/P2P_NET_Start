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
#include <list>
#include <pthread.h>
#include <stdio.h>
//#include <udt.h>

#ifndef WIN32
#include <cstdlib>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/epoll.h>            	  
#include <fcntl.h>                     
#include <sys/resource.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wspiapi.h>
#endif

//#include "udp.h"
//#include "stun.h"
//#include "test_util.h"
#include "FTCDebug.h"
#include "nettool.h"
#include "FTCMessage.h"
#include "FTCSock.h"
#include "FTCTask.h"
#include "FTCPeer.h"
#include "FTCMutex.h"

#include "db_sqlite.h"


#define SERVER_PORT 8888	//服务器端口
#define MAX_COMMAND 256
#define MAXRETRY 5

using namespace std;

BOOL gRunning = TRUE;
//PeerList ClientList;	
//SOCKET sockSrv;

typedef struct
{
	SOCKET sockSrv;
//	PeerList ClientList;
}T_LocalInfo;

T_LocalInfo stLocal;
PeerList ClientList;



#define P2P_DB_NAME "p2pinfo.db"

char * table_create_sql[] = {
"create table p2p_peerinfo(id integer primary key autoincrement,username varchar(48),pubipaddr varchar(16),\
pubport integer,lanipaddr varchar(16),lanport integer,devicetype integer,loginstatus integer)",
"create table p2p_dbversion(version integer)",
};

int initbasicdata(void)
{
	int i=0;
	for(i=0; i<sizeof(table_create_sql)/sizeof(table_create_sql[0]);i++)
	{
		if(sqllite_insert(table_create_sql[i],NULL,0,NULL,1,NULL)<0)
		{
			return -1;
		}
	}

	return 0;
}

void ResponseLoginmsg(struct sockaddr_in *remoteAddr,int value)
{
	T_Msg stMsgSend;
	T_MsgLoginResp stLoginMsg;

	memset(&stMsgSend, 0, sizeof(T_Msg));
	memset(&stLoginMsg, 0, sizeof(T_MsgLoginResp));

	stLoginMsg.result = value;

	stMsgSend.tMsgHead.uiMsgType = MSG_TYPE_RESPONSE;
	stMsgSend.tMsgHead.uiMsgId = MSG_R_LOGIN;
	stMsgSend.tMsgHead.usParaLength = sizeof(T_MsgLoginResp);
	memcpy((T_MsgLoginResp *)stMsgSend.aucParam, &stLoginMsg, sizeof(stLoginMsg));

	FTC_Sendto2(stLocal.sockSrv, (S8 *)&stMsgSend, sizeof(stMsgSend), remoteAddr);	
}


void *P2PSrvProc(void *arg)
{
	S32 niTimeout = 0;
	struct sockaddr_in fromAddr;
	T_Msg stMsg;
	U32 mMsgID;
	S8 buf[MAX_PACKET_SIZE] = {0};
	S8 test_sql[256]={0};
	T_PeerInfo* ptPeer = new T_PeerInfo;
	stCommMsg* mRecvMsg = (stCommMsg*)buf;
	memset(&fromAddr, 0, sizeof(fromAddr));

	FTC_PTHREAD_DETACH;
	
	while (gRunning)
	{
		if(0 == FTC_SelectRead(stLocal.sockSrv, niTimeout))
		{
			S32 ret = FTC_Recvfrom2(stLocal.sockSrv, (S8 *)&stMsg, sizeof(stMsg), &fromAddr);
			if (ret < 0)
			{
				std::cout << "recv error " << std::endl;
				continue;
			}
			else
			{
				mMsgID = stMsg.tMsgHead.uiMsgId;
				switch (mMsgID)
				{
					case MSG_C_LOGIN:
						{
							T_MsgLoginReq *ptLoginMsg;
							ptLoginMsg = (T_MsgLoginReq *)stMsg.aucParam;
							
							printf("user %s login, CorD: %d, Public info: [%s:%d], LAN info: [%s:%d], ID: %s\n", ptLoginMsg->name, ptLoginMsg->bCorD, 
								inet_ntoa(fromAddr.sin_addr), htons(fromAddr.sin_port), ptLoginMsg->MyLanIP, ptLoginMsg->MyLanPort, ptLoginMsg->ID);
							strcpy(ptPeer->name, ptLoginMsg->name);
							strcpy(ptPeer->ID, ptLoginMsg->ID);
							strcpy(ptPeer->sLanIp, ptLoginMsg->MyLanIP);
							ptPeer->usLanPort = ptLoginMsg->MyLanPort;
							strcpy(ptPeer->sPubIp, inet_ntoa(fromAddr.sin_addr));
							ptPeer->usPubPort = htons(fromAddr.sin_port);
							ptPeer->bCorD = ptLoginMsg->bCorD;
								
							sprintf(test_sql,"select id,loginstatus from p2p_peerinfo where username=\"%s\";",ptLoginMsg->name);
							sqlite3_stmt *stmt = NULL;
							S32 t_id = 0;
							S32 t_status = 0;
 							if(sqllite_query(test_sql,NULL,0,&stmt)==0)
							{
								PSS_DB_CPY_INT(t_id,stmt,0);
								PSS_DB_CPY_INT(t_status,stmt,1);
								sqlite3_finalize(stmt);
								//check if exist
								if(t_status == 1)
								{
									ResponseLoginmsg(&fromAddr,1);
								}
								else
								{
									strcpy(test_sql,"update p2p_peerinfo set pubipaddr=?,pubport=?,lanipaddr=?,lanport=?,devicetype=?,loginstatus=1 where username=? ;");
									db_val_t params[6];
									params[0].type = DB_STRING;
									params[0].val.string_val = ptPeer->sPubIp;
									params[1].type = DB_INT;
									params[1].val.int_val = ptPeer->usPubPort;
									params[2].type = DB_STRING;
									params[2].val.string_val = ptPeer->sLanIp;
									params[3].type = DB_INT;
									params[3].val.int_val = ptPeer->usLanPort;
									params[4].type = DB_INT;
									params[4].val.int_val = ptPeer->bCorD;
									params[5].type = DB_STRING;
									params[5].val.string_val = ptPeer->name;

									if(sqllite_update(test_sql,params,6)!=0)
									{
										printf("sqllite_update MSG_C_LOGIN err\n");
										ResponseLoginmsg(&fromAddr,1);
										
									}
									else
									{
										ResponseLoginmsg(&fromAddr,0);
									}
								}
							}
							else
							{
								strcpy(test_sql,"insert into p2p_peerinfo (pubipaddr,pubport,lanipaddr,lanport,devicetype,loginstatus,username) values(?,?,?,?,?,1,?) ;");
								db_val_t params[6];
								params[0].type = DB_STRING;
								params[0].val.string_val = ptPeer->sPubIp;
								params[1].type = DB_INT;
								params[1].val.int_val = ptPeer->usPubPort;
								params[2].type = DB_STRING;
								params[2].val.string_val = ptPeer->sLanIp;
								params[3].type = DB_INT;
								params[3].val.int_val = ptPeer->usLanPort;
								params[4].type = DB_INT;
								params[4].val.int_val = ptPeer->bCorD;
								params[5].type = DB_STRING;
								params[5].val.string_val = ptPeer->name;

								if(sqllite_insert(test_sql,params,6,NULL,1,NULL)!=0)
								{
									ResponseLoginmsg(&fromAddr,1);
								}
								else
								{
									ResponseLoginmsg(&fromAddr,0);
								}
							}

							
							#if 0
							/* 只有设备类型为device才需要保存起来 */
						//	if(ptLoginMsg->bCorD == 1)
							{
								strcpy(ptPeer->name, ptLoginMsg->name);
								strcpy(ptPeer->ID, ptLoginMsg->ID);
								strcpy(ptPeer->sLanIp, ptLoginMsg->MyLanIP);
								ptPeer->usLanPort = ptLoginMsg->MyLanPort;
								strcpy(ptPeer->sPubIp, inet_ntoa(fromAddr.sin_addr));
								ptPeer->usPubPort = htons(fromAddr.sin_port);
								ptPeer->bCorD = ptLoginMsg->bCorD;
								
								if(ClientList.empty() != true && ClientList.size() > 0) //
								{
									//check if exist
									BOOL isExist = FALSE;

									isExist = CheckPeerListByName(&ClientList, ptLoginMsg->name);
									if(!isExist)
									{
										ClientList.push_back(ptPeer);			//  do not exclude same name user
									}
									else
									{
										T_Msg stMsgSend;
										T_MsgLoginResp stLoginMsg;

										memset(&stMsgSend, 0, sizeof(T_Msg));
										memset(&stLoginMsg, 0, sizeof(T_MsgLoginResp));

										stLoginMsg.result = 1;

										stMsgSend.tMsgHead.uiMsgType = MSG_TYPE_RESPONSE;
										stMsgSend.tMsgHead.uiMsgId = MSG_R_LOGIN;
										stMsgSend.tMsgHead.usParaLength = sizeof(T_MsgLoginResp);
										memcpy((T_MsgLoginResp *)stMsgSend.aucParam, &stLoginMsg, sizeof(stLoginMsg));

										FTC_Sendto2(stLocal.sockSrv, (S8 *)&stMsgSend, sizeof(stMsgSend), &fromAddr);
									}
								}
								else
								{
									//printf("new user.\n");
									ClientList.push_back(ptPeer);			//  do not exclude same name user
									T_Msg stMsgSend;
									T_MsgLoginResp stLoginMsg;

									memset(&stMsgSend, 0, sizeof(T_Msg));
									memset(&stLoginMsg, 0, sizeof(T_MsgLoginResp));

									stLoginMsg.result = 0;

									stMsgSend.tMsgHead.uiMsgType = MSG_TYPE_RESPONSE;
									stMsgSend.tMsgHead.uiMsgId = MSG_R_LOGIN;
									stMsgSend.tMsgHead.usParaLength = sizeof(T_MsgLoginResp);
									memcpy((T_MsgLoginResp *)stMsgSend.aucParam, &stLoginMsg, sizeof(stLoginMsg));

									FTC_Sendto2(stLocal.sockSrv, (S8 *)&stMsgSend, sizeof(stMsgSend), &fromAddr);
								}
							}
							#endif
							break;
						}
					
					case MSG_C_LOGOUT:
						{
							T_MsgLogoutReq *ptSubMsg;
							ptSubMsg = (T_MsgLogoutReq*)stMsg.aucParam;
							
							strcpy(test_sql,"update p2p_peerinfo set loginstatus=0 where username=? ;");
							db_val_t params[1];
							params[0].type = DB_STRING;
							params[0].val.string_val = ptSubMsg->myName;

							if(sqllite_update(test_sql,params,1)==0)
							{
								T_Msg stMsgSend;
								T_MsgLogoutResp stLogoutMsg;

								memset(&stMsgSend, 0, sizeof(T_Msg));
								memset(&stLogoutMsg, 0, sizeof(T_MsgLogoutResp));

								stLogoutMsg.result = 0;

								stMsgSend.tMsgHead.uiMsgType = MSG_TYPE_RESPONSE;
								stMsgSend.tMsgHead.uiMsgId = MSG_R_LOGOUT;
								stMsgSend.tMsgHead.usParaLength = sizeof(T_MsgLoginResp);
								memcpy((T_MsgLogoutResp *)stMsgSend.aucParam, &stLogoutMsg, sizeof(stLogoutMsg));

								FTC_Sendto2(stLocal.sockSrv, (S8 *)&stMsgSend, sizeof(stMsgSend), &fromAddr);
								
							}
							
							#if 0
							T_MsgLogoutReq *ptSubMsg;
							ptSubMsg = (T_MsgLogoutReq*)stMsg.aucParam;

							printf("recv logout message from: %s\n", ptSubMsg->myName);

							RemovePeerByName(&ClientList, ptSubMsg->myName);

							T_Msg stMsgSend;
							T_MsgLogoutResp stLogoutMsg;

							memset(&stMsgSend, 0, sizeof(T_Msg));
							memset(&stLogoutMsg, 0, sizeof(T_MsgLogoutResp));

							stLogoutMsg.result = 0;

							stMsgSend.tMsgHead.uiMsgType = MSG_TYPE_RESPONSE;
							stMsgSend.tMsgHead.uiMsgId = MSG_R_LOGOUT;
							stMsgSend.tMsgHead.usParaLength = sizeof(T_MsgLoginResp);
							memcpy((T_MsgLogoutResp *)stMsgSend.aucParam, &stLogoutMsg, sizeof(stLogoutMsg));

							FTC_Sendto2(stLocal.sockSrv, (S8 *)&stMsgSend, sizeof(stMsgSend), &fromAddr);	
							#endif
							break;
						}
					
					case MSG_C_GET_PEERS:
						{
							sqlite3_stmt *stmt = NULL;
							int t_id=0,t_i=0;
							S8 tmp_str[256]={0}; 
		
							strcpy(test_sql,"select id,pubipaddr,pubport,lanipaddr,lanport,username from p2p_peerinfo where devicetype=1,loginstatus=1;");	
								
							if(sqllite_query(test_sql,NULL,0,&stmt)!=0)
							{
								printf("MSG_C_GET_PEERS query err\n");
								return -1;
							}

							do{
								
								PSS_DB_CPY_INT(t_id,stmt,0);
								PSS_DB_CPY_STR(tmp_str,stmt,1);
								printf("t_i:%d t_id:%d\n",t_i++,t_id);
							}while(sqlite3_step(stmt) == SQLITE_ROW);
							sqlite3_finalize(stmt);
		
							#if 0
							T_MsgGetPeerListReq *ptMsgReq;
							ptMsgReq = (T_MsgGetPeerListReq *)stMsg.aucParam;
							
							printf("user %s want to get all peers info.\n", ptMsgReq->name);

							T_Msg stMsgSend;
							T_MsgGetPeerListResp *stSubMsg = NULL;

							memset(&stMsgSend, 0, sizeof(T_Msg));
							stSubMsg = (T_MsgGetPeerListResp *)stMsgSend.aucParam;
							
							for (PeerList::iterator  ClientList_iter = ClientList.begin(); ClientList_iter != ClientList.end(); ++ClientList_iter)
							{
								strcpy(stSubMsg->peerList[stSubMsg->uiPeerNums].name, (*ClientList_iter)->name);
								strcpy(stSubMsg->peerList[stSubMsg->uiPeerNums].ID, (*ClientList_iter)->ID);
								stSubMsg->peerList[stSubMsg->uiPeerNums].bCorD = (*ClientList_iter)->bCorD;
								strcpy(stSubMsg->peerList[stSubMsg->uiPeerNums].sPubIp, (*ClientList_iter)->sPubIp);
								stSubMsg->peerList[stSubMsg->uiPeerNums].usPubPort = (*ClientList_iter)->usPubPort;
								strcpy(stSubMsg->peerList[stSubMsg->uiPeerNums].sLanIp, (*ClientList_iter)->sLanIp);
								stSubMsg->peerList[stSubMsg->uiPeerNums].usLanPort = (*ClientList_iter)->usLanPort;
								
							//	printf("name:%s PubIP:%s\n",stSubMsg->peerList[stSubMsg->uiPeerNums].name,stSubMsg->peerList[stSubMsg->uiPeerNums].sPubIp);
							//	printf("send peer list name: from:%s to:%s\n", (*ClientList_iter)->name, stSubMsg->peerList[stSubMsg->uiPeerNums].name);
								stSubMsg->uiPeerNums++;
							}

							stMsgSend.tMsgHead.uiMsgType = MSG_TYPE_RESPONSE;
							stMsgSend.tMsgHead.uiMsgId = MSG_R_GET_PEERS;
							//stMsgSend.tMsgHead.usParaLength = sizeof(stSubMsg)*stSubMsg->uiPeerNums;
							stMsgSend.tMsgHead.usParaLength = sizeof(T_MsgGetPeerListResp)+sizeof(T_PeerInfo)*stSubMsg->uiPeerNums;

							FTC_Sendto2(stLocal.sockSrv, (S8*)&stMsgSend, sizeof(stMsgSend), &fromAddr);

							printf("MSG_C_GET_PEERS FTC_Sendto2\n");
							#endif
							break;
						}

					case MSG_C_HEART_BEAT:
						{
						/*	char sendBuf[MAX_PACKET_SIZE] = {0};
							stCommMsg* sendbuf = (stCommMsg*) sendBuf;
							
							sendbuf->uiMsgType = MSG_R_HEART_BEAT;
							for (UserList::iterator  ClientList_iter = ClientList.begin(); ClientList_iter != ClientList.end(); ++ClientList_iter)
							{
								memcpy(sendbuf->userList[sendbuf->userNums].userName, (*ClientList_iter)->userName,MAX_NAME_SIZE);
								sendbuf->userList[sendbuf->userNums].uiIP = (*ClientList_iter)->uiIP;
								sendbuf->userList[sendbuf->userNums].usPORT = (*ClientList_iter)->usPORT;
								++sendbuf->userNums;
							}

							FTC_Sendto2(sockSrv, (S8*)sendbuf, sendbuf->getSize(), &sender); 
						
							unsigned int nodecount = ClientList.size(); */
							
							std::cout << "recv heart baet msg from " << (S8 *)stMsg.aucParam << std::endl;
							
							break;
						}
					
					case MSG_C_HOLE:
						{
							T_MsgHoleReq *pHoleReq;
							pHoleReq = (T_MsgHoleReq *)stMsg.aucParam;
							
							//recv connet remote request.
							BOOL isExist = CheckPeerListByName(&ClientList, pHoleReq->toName);
							if(!isExist)
							{
								P2P_DBG_DEBUG("user does not exist!");
								
								T_Msg stMsgSend;
								T_MsgHoleResp stHoleResp;

								memset(&stMsgSend, 0, sizeof(T_Msg));
								memset(&stHoleResp, 0, sizeof(T_MsgHoleResp));
								stHoleResp.result = 1;

								stMsgSend.tMsgHead.uiMsgType = MSG_TYPE_RESPONSE;
								stMsgSend.tMsgHead.uiMsgId = MSG_R_HOLE;
								stMsgSend.tMsgHead.usParaLength = sizeof(stHoleResp);
								memcpy(stMsgSend.aucParam, &stHoleResp, sizeof(stHoleResp));

								FTC_Sendto2(stLocal.sockSrv, (S8*)&stMsgSend, sizeof(stMsgSend), &fromAddr);
							}
							else
							{
								T_PeerInfo toPeer = GetPeerByName(&ClientList, pHoleReq->toName);

								//check if in LAN first.
								//...
								
								sockaddr_in remote;
								remote.sin_family = AF_INET;
								remote.sin_addr.s_addr = FTC_InetAddr(toPeer.sPubIp);
								remote.sin_port = FTC_Htons(toPeer.usPubPort);
								
	 							printf("%s [%s:%d] want to connect to %s [%s:%d]\n", pHoleReq->myName, inet_ntoa(fromAddr.sin_addr), 
									htons(fromAddr.sin_port), toPeer.name, toPeer.sPubIp, toPeer.usPubPort);

								T_Msg stMsgSend;
								T_MsgHoleFromSrvReq stMsgSub;

								memset(&stMsgSend, 0, sizeof(T_Msg));
								memset(&stMsgSub, 0, sizeof(T_MsgHoleFromSrvReq));

								strcpy(stMsgSub.srcName, pHoleReq->myName);
								strcpy(stMsgSub.srcPubIP, inet_ntoa(fromAddr.sin_addr));
								stMsgSub.srcPubPort = htons(fromAddr.sin_port);

								stMsgSend.tMsgHead.uiMsgType = MSG_TYPE_INDICATE;
								stMsgSend.tMsgHead.uiMsgId = MSG_C_HOLE_REQ;
								stMsgSend.tMsgHead.usParaLength = sizeof(stMsgSub);
								memcpy(stMsgSend.aucParam, &stMsgSub, sizeof(stMsgSub));

								FTC_Sendto2(stLocal.sockSrv, (S8*)&stMsgSend, sizeof(stMsgSend), &remote);
							}
							break;
						}

					case MSG_C_HAVE_SEND_P2P_REQ:
						{
							T_MsgHaveSendP2PReq *ptSubMsg;
							ptSubMsg = (T_MsgHaveSendP2PReq *)stMsg.aucParam;
							
							T_PeerInfo toPeer = GetPeerByName(&ClientList, ptSubMsg->toName);
							
							sockaddr_in remote;
							remote.sin_family = AF_INET;
							remote.sin_addr.s_addr = FTC_InetAddr(toPeer.sPubIp);
							remote.sin_port = FTC_Htons(toPeer.usPubPort);

							T_Msg stMsgSend;
							T_MsgP2PCanStartReq stMsgSub;

							memset(&stMsgSend, 0, sizeof(T_Msg));
							memset(&stMsgSub, 0, sizeof(T_MsgP2PCanStartReq));

							strcpy(stMsgSub.myName, ptSubMsg->myName);

							stMsgSend.tMsgHead.uiMsgType = MSG_TYPE_INDICATE;
							stMsgSend.tMsgHead.uiMsgId = MSG_C_P2P_CAN_START_REQ;
							stMsgSend.tMsgHead.usParaLength = sizeof(stMsgSub);
							memcpy(stMsgSend.aucParam, &stMsgSub, sizeof(stMsgSub));

							FTC_Sendto2(stLocal.sockSrv, (S8*)&stMsgSend, sizeof(stMsgSend), &remote);
						}
						break;
						
					case RELAYMESSAGE:
					{
						T_PeerInfo toPeer = GetPeerByName(&ClientList, mRecvMsg->cToName);

						sockaddr_in remote;
						remote.sin_family = AF_INET;
					//	remote.sin_port = htons(toPeer.usPORT);
					//	remote.sin_addr.s_addr = htonl(toPeer.uiIP);
						
						stCommMsg MessageHead;
						MessageHead.uiMsgType = RELAYMESSAGE2;
						strcpy(MessageHead.cMyName, mRecvMsg->cMyName);
						strcpy(MessageHead.cToName, mRecvMsg->cToName);
					//	MessageHead.p2pMsg.uiSendLen = (int)strlen(recvmessage)+1;
					//	strcpy(MessageHead.p2pMsg.cP2PCommUserName, mRecvMsg->cMyName);
						printf("%s -> %s, %d\n", mRecvMsg->cMyName, mRecvMsg->cToName, mRecvMsg->rlyMsg.uiSendLen);
							
						char *recvmessage = new char[mRecvMsg->rlyMsg.uiSendLen];
						int recv = FTC_Recvfrom2(stLocal.sockSrv, recvmessage, MAX_PACKET_SIZE, &fromAddr);
						printf("\r\nrecv data len = %d\r\n", recv);
						recvmessage[recv - 1] = '\0';
						if (recv <= 0)
						{
							printf("recv message error!\n");
						}
						else
						{
							if(mRecvMsg->rlyMsg.uiSendLen == recv)
							{
								MessageHead.rlyMsg.uiSendLen = (int)strlen(recvmessage)+1;
								strcpy(MessageHead.rlyMsg.sUserName, mRecvMsg->cMyName);
								
								//发送p2p消息头
								FTC_Sendto2(stLocal.sockSrv, (S8*)&MessageHead, sizeof(MessageHead), &remote);

								printf("recv relay message : %s from : %s to : %s\n", recvmessage, mRecvMsg->cMyName, 
								mRecvMsg->cToName);
								
								//发送p2p消息体
								FTC_Sendto2(stLocal.sockSrv, (S8*)&recvmessage, MessageHead.rlyMsg.uiSendLen, &remote);
							}
						}

						delete[] recvmessage;
						
						break;
					}
				}
				
			}
		}
		else
		{
			continue;  //信号中断
		}

		usleep(10000);
	}

	delete ptPeer;
	FTC_PTHREAD_EXIT;
}

int main(int argc, char* argv[])
{
	S32 ret;

	char test_sql[256]={0};
	ret = sqllite_init(P2P_DB_NAME,0);
	if(ret <0)
	{
		printf("sqllite_init %s err\n",P2P_DB_NAME);
		return -1;
	}
	else if(ret == 1)
	{
		printf("sqllite_init value:%d\n",ret);
		if(initbasicdata()< 0)
		{
			printf("initbasicdata err 1\n");
			return -1;
		}
		strcpy(test_sql,"insert into p2p_dbversion(version) values(1);");
		if(sqllite_insert(test_sql,NULL,0,NULL,1,NULL)<0)
		{
			printf("initbasicdata err 1\n");
			return -1;
		}
	}
	
	memset(&stLocal, 0, sizeof(T_LocalInfo));
	
	stLocal.sockSrv = FTC_CreateUdpSock(FTC_InetAddr("0.0.0.0"), FTC_Htons(SERVER_PORT));
	if (0 > stLocal.sockSrv)
	{
		P2P_DBG_ERROR("FTC_CreateUdpSock create udp sock fail port:%d", SERVER_PORT);
		return -1;
	}

	FTC_CREATE_THREADEX(P2PSrvProc, NULL, ret); 
	if (FALSE == ret)
	{
    	P2P_DBG_ERROR("P2PSrvProc start failed!");
		return -1;
	} 
	
	printf("***** P2P server started. *****\n");

	while(gRunning)
	{
		usleep(100000);
	}

	FTC_CloseSock(stLocal.sockSrv);

   	return 0;
}


#if 0 //old
int setSrvNetInfo(char *ips, SERVER_INFO *srvInfo)
{
	const char * split = ",";
	char *p;
	p = strtok (ips,split);
	int i=0;
	char addr[10][16];

	while(p!=NULL) {
	//	printf ("%s\n",p);
		if(strcmp(p, "127.0.0.1") != 0)
		{
			strcpy(addr[i], p);
		//	printf("%s\n", addr[i]);
			i++;
		}
		
		p = strtok(NULL,split);
	}

//	printf("num=%d\n", i);
	if(i < 2)
		return -1;

	strcpy(srvInfo->srvIpAddr1, addr[0]);
	strcpy(srvInfo->srvIpAddr2, addr[1]);

//	printf("server ip addr1:%s\n", srvInfo->srvIpAddr1);
//	printf("server ip addr2:%s\n", srvInfo->srvIpAddr2);

	return 0;
}

int setnonblocking( int fd )  
{  
    if( fcntl( fd, F_SETFL, fcntl( fd, F_GETFD, 0 )|O_NONBLOCK ) == -1 )  
    {  
        printf("Set blocking error : %d\n", errno);  
        return -1;  
    }  
    return 0;  
} 

void* pthread_handle_message(void* para)
{
//    char recvBuf[1024] = {0}; 
    int ret = 0;
    int rs = 1;
    PEER_INFO peer = *(PEER_INFO *)para;
    char *buffer;
    buffer = (char *)malloc(NET_SEND_IO_BUFFER_SIZE*2);

    while(rs)
    {
        ret = recv(peer.peerSkt, (char *)&buffer, sizeof(buffer), 0);

        if(ret < 0)
        {
            if(errno == EAGAIN)
            {
                printf("EAGAIN\n");
                break;
            }
            else{
                printf("recv error! errno:%d\n", errno);
        
                close(peer.peerSkt);
                break;
            }
        }
        else if(ret == 0)
        {
            rs = 0;
        }
	  else
        {
            rs = 1;
		
		P2P_STREAM_IO_HEAD * pStreamHead = NULL;
    		pStreamHead = (P2P_STREAM_IO_HEAD *)buffer;
	
		if( pStreamHead->u8StreamIOType == SIO_TYPE_IOCTRL)
		{
			P2P_IO_CTRL_HEAD * pstIoCtrlHead = NULL;
			pstIoCtrlHead = (P2P_IO_CTRL_HEAD *)(buffer + sizeof(P2P_STREAM_IO_HEAD));
			
			switch(pstIoCtrlHead->u16IOCtrlType)
			{
				case IOCTRL_TYPE_LOGIN_REQ:
					printf(YELLOW "Receive IOCTRL_TYPE_LOGIN_REQ message from %s:%d\n" NONE, inet_ntoa(peer.peer_addr.sin_addr)
				, htons(peer.peer_addr.sin_port));
					break;

				default:
					break;
			}
			//	logResp.status = 1;
			//	send(peer.peerSkt, (char *)&logResp, sizeof(logResp), 0);
		}
        }
    }
    free(buffer);
    close(peer.peerSkt);
}
#endif //old

#if 0 //for stun server
void usage()
{
   cerr << "Usage: " << endl
        << " ./server [-v] [-h] [-h IP_Address] [-a IP_Address] [-p port] [-o port] [-m mediaport]" << endl
        << " " << endl
        << " If the IP addresses of your NIC are 10.0.1.150 and 10.0.1.151, run this program with" << endl
        << "    ./server -v  -h 10.0.1.150 -a 10.0.1.151" << endl
        << " STUN servers need two IP addresses and two ports, these can be specified with:" << endl
        << "  -h sets the primary IP" << endl
        << "  -a sets the secondary IP" << endl
        << "  -p sets the primary port and defaults to 3478" << endl
        << "  -o sets the secondary port and defaults to 3479" << endl
        << "  -b makes the program run in the backgroud" << endl
        << "  -m sets up a STERN server starting at port m" << endl
        << "  -v runs in verbose mode" << endl
      // in makefile too
        << endl;
}

#ifndef WIN32
void *stunServerThreadProc(void *arg)
#else
DWORD WINAPI stunServerThreadProc(LPVOID);
#endif
{
	printf("====>startSTUNserver thread start!!!\n"); 
	
	SERVER_INFO *st = (SERVER_INFO*)arg;
	
	assert( sizeof(UInt8 ) == 1 );
	assert( sizeof(UInt16) == 2 );
	assert( sizeof(UInt32) == 4 );

	initNetwork(); //just for windows os

//	clog << "STUN server version "  <<  STUN_VERSION << endl;
	  
	StunAddress4 myAddr;
	StunAddress4 altAddr;
	bool verbose=false;

	myAddr.addr = 0;
	altAddr.addr = 0;
	myAddr.port = STUN_PORT;
	altAddr.port = STUN_PORT+1;
//	int myPort = 0;
//	int altPort = 0;
	int myMediaPort = 0;

	stunParseServerName(st->srvIpAddr1, myAddr);
	stunParseServerName(st->srvIpAddr2, altAddr);

	if ( myAddr.port == altAddr.port )
	{
		altAddr.port = myAddr.port+1;
	}

	StunServerInfo info;
	bool ok = stunInitServer(info, myAddr, altAddr, myMediaPort, verbose);

	int c=0;
	while (ok)
	{
		ok = stunServerProcess(info, verbose);
		c++;
		if ( verbose && (c%1000 == 0) ) 
		{
			clog << "*";
		}
	}
}
#endif //for stun server


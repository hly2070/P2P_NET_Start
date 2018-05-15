/**************************************************************************
* 版    权： 		Copyright (c) 2018 FTC.
* 文件名称：	FTCPeer.h
* 文件标识： 
* 内容摘要：	Peer端信息维护接口封装
* 其它说明： 
* 当前版本：	V1.0
* 作    者：		hly2070
* 完成日期：	2018年05月 05日
*
* 修改记录1	：
* 修改日期：
* 版 本 号：
* 修 改 人：
* 修改内容：
**************************************************************************/
#ifndef __FTCMESSAGE_H__
#define __FTCMESSAGE_H__

//#ifdef __cplusplus
//extern "C" {
//#endif

/**************************************************************************
 *                         头文件引用                                     *
 **************************************************************************/
#include <string>
#include <string.h>
#include "FTCTypedef.h"
#include "FTCPeer.h"

/**************************************************************************
 *                        常量定义                                   *
 **************************************************************************/
/* 消息头长度*/
#define MSG_HEAD_LEN			16
#define MSG_PARAM_LEN			1424

/**************************************************************************
 *                         数据类型                                    *
 **************************************************************************/
/*p2p通信协议*/
/* 消息ID */
enum 
{
	MSG_C_LOGIN,
	MSG_R_LOGIN,     

	MSG_C_HEART_BEAT,			/* 客户端到服务器的心跳信息*/
	MSG_R_HEART_BEAT,	

	MSG_C_GET_PEERS,	    
 	MSG_R_GET_PEERS,      

	MSG_C_HOLE,    
	MSG_R_HOLE,

	MSG_C_HOLE_REQ,		//server send about A want connect to B
	MSG_R_HOLE_RESP,

	MSG_C_P2P_CONN_REQ,
	MSG_R_P2P_CONN_RESP,

	MSG_C_HAVE_SEND_P2P_REQ,
	MSG_R_HAVE_SEND_P2P_RESP,

	MSG_C_P2P_CAN_START_REQ,
	MSG_R_P2P_CAN_START_RESP,
	
	MSG_C_P2P_START_REQ,
	MSG_R_P2P_START_RESP,

	MSG_C_LOGOUT,	  
	MSG_R_LOGOUT,     
};	


//用于p2p客户端之间通信
#define P2PMESSAGE 100//发送消息
#define P2PMESSAGEACK 101 //收到消息的应答
#define P2PSOMEONEWANTTOCALLYOU 102//服务器向客户端发送，要求此客户端发送UDP打洞包
#define P2PPUNCH 103//客户端发送的打洞包，接收端应忽略此消息
#define P2PHAVECONNECT1 104 //从对端发往服务器告知服务器已经往请求端发送过P2P消息
#define P2PCANSTART 105     //从服务器发往请求端，告知对方可以开始想对方发P2P 消息了
#define P2PPUNCH2	106	 //直接向对方发送P2P消息
#define P2PPUNCHOK	107	//告诉对方收到了P2P消息
#define RELAYMESSAGE	108	//
#define RELAYMESSAGE2	109	//


#define MAX_PACKET_SIZE 1024

//Server向客户端发送打洞请求消息
struct stTransMsg
{
    char userName[MAX_NAME_SIZE];			// 节点的名字
    unsigned int uiIP;			// 将要发送给客户端的 IP
    unsigned short usPORT;		// 将要发送给客户端的 PORT

    stTransMsg()
    {
//        uiIP = 0;
//        usPORT = 0;
        bzero(this, sizeof(*this));
    }
};

//用于客户端之间的通信
struct stP2PMsg
{
	unsigned int uiSendLen;				// p2p 发送的消息长度
	char cP2PCommUserName[MAX_NAME_SIZE];		// 记录发送消息的用户名

	stP2PMsg()
	{
		bzero(this, sizeof(*this));
	}
};

//relay message
struct stRelayMsg
{
	unsigned int uiSendLen; //message lenght.
	char sUserName[MAX_NAME_SIZE];

	stRelayMsg()
	{
		memset(this, 0, sizeof(*this));
	}
};

//通用消息格式
struct stCommMsg
{
    unsigned int uiMsgType;				// 客户端和服务器通信的类型
    unsigned int result;  //0:success 1:failed
    char cMyName[MAX_NAME_SIZE];		// 本客户端自己的名字	
    char cToName[MAX_NAME_SIZE];		// 想要p2p 通信的 客户端名字

    stTransMsg  	transMsg;			// 服务器转发的 
//    union
//    {
    	stP2PMsg		p2pMsg;				// 客户端之间的p2p通信
    	stRelayMsg		rlyMsg;			//relay message.
//    };

    unsigned int   userNums;			// 全部的客户端数量
    T_PeerInfo userList[0];			//用于server向客户端发送客户列表
        
    stCommMsg()
    {
    	bzero(this, sizeof(*this));
    }
    
    unsigned int getSize() const { return sizeof(*this) + userNums * sizeof(T_PeerInfo); }
};

/* 字节序*/
typedef enum
{
	ORDER_TYPE_NET,
	ORDER_TYPE_HOST
}E_OrderType;

/* 消息类型*/
typedef enum
{
	MSG_TYPE_REQUEST		= 0,		/* 请求命令*/
	MSG_TYPE_INDICATE		= 1,		/* 指示命令*/
	MSG_TYPE_QUERY		= 2,		/* 查询命令*/
	MSG_TYPE_RESPONSE	= 3,		/* 上报命令*/
	MSG_TYPE_ALL
}E_MsgType;

/* 消息头定义*/ 
typedef struct
{
	U32	uiMsgType;		/* 消息类型*/
	U32	uiMsgId;
	U32	uiAppPrivate;
	U8	ucOrginator;		/* 消息发起者*/
	U8	ucParamOrder;	/*参数的字节序，0:网络字节序 1:主机字节序E_OrderType*/
	U16	usParaLength;
}T_MessageHead;

/* 消息结构定义*/
typedef struct
{
	T_MessageHead tMsgHead;
	U8 aucParam[MSG_PARAM_LEN];
}T_Msg;

/* 登录消息 */
typedef struct
{
	S8 name[MAX_NAME_SIZE];
	S8 MyLanIP[16];
	U16 MyLanPort;
	S8 ID[8];
	U8 bCorD; /* 0:client 1:device*/
}T_MsgLoginReq;

typedef struct
{
	U8 result;      /* 0:seccuess 1:failed */
	U8 reserve[7];
}T_MsgLoginResp;

/* 获取服务器端终端列表信息 */
typedef struct
{
	S8 name[MAX_NAME_SIZE];
	U8 reserve[8];
}T_MsgGetPeerListReq;

typedef struct
{
	U32 uiPeerNums;
	U8 reserve[8];
	T_PeerInfo peerList[0];			//用于server向客户端发送客户列表
}T_MsgGetPeerListResp;

/* P2P打洞请求消息 */
typedef struct
{
	S8 myName[MAX_NAME_SIZE];
	S8 toName[MAX_NAME_SIZE];
	U8 reserve[8];
}T_MsgHoleReq;

typedef struct
{
	U8 result;      /* 0:seccuess 1:failed */
	U8 reserve[7];
}T_MsgHoleResp;

/* P2P打洞请求服务器转发消息 */
typedef struct
{
	S8 srcName[MAX_NAME_SIZE];
	S8 srcPubIP[16];
	U16 srcPubPort;
}T_MsgHoleFromSrvReq;

/* Peer间P2P 打洞请求消息 */
typedef struct
{
	S8 myName[MAX_NAME_SIZE];
}T_MsgP2PConnReq;

typedef struct
{
	U8 result;      /* 0:seccuess 1:failed */
	U8 reserve[7];
}T_MsgP2PConnResp;

/* Peer通知服务器已经往对端发送了P2P连接请求 */
typedef struct
{
	S8 myName[MAX_NAME_SIZE];
	S8 toName[MAX_NAME_SIZE];
}T_MsgHaveSendP2PReq;

/* Peer尝试给对端发送P2P消息 */
typedef struct
{
	S8 myName[MAX_NAME_SIZE];
}T_MsgP2PStartReq;

typedef struct
{
	U8 reserve[8];
}T_MsgP2PStartResp;

/* P2P server通知对端可以开始给对端发P2P请求消息 */
typedef struct
{
	S8 myName[MAX_NAME_SIZE];
}T_MsgP2PCanStartReq;

/* Peer端发送给服务器的注销登录消息 */
typedef struct
{
	S8 myName[MAX_NAME_SIZE];
}T_MsgLogoutReq;

typedef struct
{
	U8 result;      /* 0:seccuess 1:failed */
	U8 reserve[7];
}T_MsgLogoutResp;

//#ifdef __cplusplus
//}
//#endif

#endif


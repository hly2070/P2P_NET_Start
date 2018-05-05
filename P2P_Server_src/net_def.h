#ifndef _NET_DEF_H_
#define _NET_DEF_H_

#include <string>
#include <string.h>
#include "FTCTypedef.h"
#include "FTCPeer.h"

#define SERVER_PORT 8888	//服务器端口
#define MAX_COMMAND 256
#define MAXRETRY 5

/*p2p通信协议*/
//iMessageType值
enum {
    MSG_LOGIN_REQ,	    
    MSG_LOGIN_RESP,     

    MSG_HEART_CHECK_REQ,
    MSG_HEART_CHECK_RESP,

    MSG_GETALLUSER_REQ,	    
    MSG_GETALLUSER_RESP,      

    MSG_CONNECT_REMOTE_REQ,	    
    MSG_CONNECT_REMOTE_RESP,      

    MSG_LOGOUT_REQ,	    
    MSG_LOGOUT_RESP,      
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

//手动调用构造函数，不分配内存
template<class _T1> 
inline	void constructInPlace(_T1  *_Ptr)
{
	new (static_cast<void*>(_Ptr)) _T1();
}
/// 声明变长指令
#define BUFFER_CMD(cmd,name,len) char buffer##name[len]={0};\
														cmd *name=(cmd *)buffer##name;constructInPlace(name);

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
    stUserListNode userList[0];			//用于server向客户端发送客户列表
        
    stCommMsg()
    {
    	bzero(this, sizeof(*this));
    }
    
    unsigned int getSize() const { return sizeof(*this) + userNums * sizeof(stUserListNode); }
};

#endif

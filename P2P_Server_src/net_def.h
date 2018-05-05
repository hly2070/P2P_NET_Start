#ifndef _NET_DEF_H_
#define _NET_DEF_H_

#include <string>
#include <string.h>
#include "FTCTypedef.h"
#include "FTCPeer.h"

#define SERVER_PORT 8888	//�������˿�
#define MAX_COMMAND 256
#define MAXRETRY 5

/*p2pͨ��Э��*/
//iMessageTypeֵ
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


//����p2p�ͻ���֮��ͨ��
#define P2PMESSAGE 100//������Ϣ
#define P2PMESSAGEACK 101 //�յ���Ϣ��Ӧ��
#define P2PSOMEONEWANTTOCALLYOU 102//��������ͻ��˷��ͣ�Ҫ��˿ͻ��˷���UDP�򶴰�
#define P2PPUNCH 103//�ͻ��˷��͵Ĵ򶴰������ն�Ӧ���Դ���Ϣ
#define P2PHAVECONNECT1 104 //�ӶԶ˷�����������֪�������Ѿ�������˷��͹�P2P��Ϣ
#define P2PCANSTART 105     //�ӷ�������������ˣ���֪�Է����Կ�ʼ��Է���P2P ��Ϣ��
#define P2PPUNCH2	106	 //ֱ����Է�����P2P��Ϣ
#define P2PPUNCHOK	107	//���߶Է��յ���P2P��Ϣ
#define RELAYMESSAGE	108	//
#define RELAYMESSAGE2	109	//


#define MAX_PACKET_SIZE 1024

//�ֶ����ù��캯�����������ڴ�
template<class _T1> 
inline	void constructInPlace(_T1  *_Ptr)
{
	new (static_cast<void*>(_Ptr)) _T1();
}
/// �����䳤ָ��
#define BUFFER_CMD(cmd,name,len) char buffer##name[len]={0};\
														cmd *name=(cmd *)buffer##name;constructInPlace(name);

//Server��ͻ��˷��ʹ�������Ϣ
struct stTransMsg
{
    char userName[MAX_NAME_SIZE];			// �ڵ������
    unsigned int uiIP;			// ��Ҫ���͸��ͻ��˵� IP
    unsigned short usPORT;		// ��Ҫ���͸��ͻ��˵� PORT

    stTransMsg()
    {
//        uiIP = 0;
//        usPORT = 0;
        bzero(this, sizeof(*this));
    }
};

//���ڿͻ���֮���ͨ��
struct stP2PMsg
{
	unsigned int uiSendLen;				// p2p ���͵���Ϣ����
	char cP2PCommUserName[MAX_NAME_SIZE];		// ��¼������Ϣ���û���

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

//ͨ����Ϣ��ʽ
struct stCommMsg
{
    unsigned int uiMsgType;				// �ͻ��˺ͷ�����ͨ�ŵ�����
    unsigned int result;  //0:success 1:failed
    char cMyName[MAX_NAME_SIZE];		// ���ͻ����Լ�������	
    char cToName[MAX_NAME_SIZE];		// ��Ҫp2p ͨ�ŵ� �ͻ�������

    stTransMsg  	transMsg;			// ������ת���� 
//    union
//    {
    	stP2PMsg		p2pMsg;				// �ͻ���֮���p2pͨ��
    	stRelayMsg		rlyMsg;			//relay message.
//    };

    unsigned int   userNums;			// ȫ���Ŀͻ�������
    stUserListNode userList[0];			//����server��ͻ��˷��Ϳͻ��б�
        
    stCommMsg()
    {
    	bzero(this, sizeof(*this));
    }
    
    unsigned int getSize() const { return sizeof(*this) + userNums * sizeof(stUserListNode); }
};

#endif

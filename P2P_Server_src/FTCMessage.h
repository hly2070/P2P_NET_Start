/**************************************************************************
* ��    Ȩ�� 		Copyright (c) 2018 FTC.
* �ļ����ƣ�	FTCPeer.h
* �ļ���ʶ�� 
* ����ժҪ��	Peer����Ϣά���ӿڷ�װ
* ����˵���� 
* ��ǰ�汾��	V1.0
* ��    �ߣ�		hly2070
* ������ڣ�	2018��05�� 05��
*
* �޸ļ�¼1	��
* �޸����ڣ�
* �� �� �ţ�
* �� �� �ˣ�
* �޸����ݣ�
**************************************************************************/
#ifndef __FTCMESSAGE_H__
#define __FTCMESSAGE_H__

//#ifdef __cplusplus
//extern "C" {
//#endif

/**************************************************************************
 *                         ͷ�ļ�����                                     *
 **************************************************************************/
#include <string>
#include <string.h>
#include "FTCTypedef.h"
#include "FTCPeer.h"

/**************************************************************************
 *                        ��������                                   *
 **************************************************************************/
/* ��Ϣͷ����*/
#define MSG_HEAD_LEN			16
#define MSG_PARAM_LEN			1424

/**************************************************************************
 *                         ��������                                    *
 **************************************************************************/
/*p2pͨ��Э��*/
//iMessageTypeֵ
enum {
	MSG_C_LOGIN,
	MSG_R_LOGIN,     

	MSG_C_HEART_BEAT,			/* �ͻ��˵���������������Ϣ*/
	MSG_R_HEART_BEAT,	

	MSG_C_GET_PEERS,	    
 	MSG_R_GET_PEERS,      

	MSG_C_HOLE,    
	MSG_R_HOLE,      

	MSG_C_LOGOUT,	  
	MSG_R_LOGOUT,     
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

/* �ֽ���*/
typedef enum
{
	ORDER_TYPE_NET,
	ORDER_TYPE_HOST
}E_OrderType;

/* ��Ϣ����*/
typedef enum
{
	MSG_TYPE_REQUEST		= 0,		/* ��������*/
	MSG_TYPE_INDICATE		= 1,		/* ָʾ����*/
	MSG_TYPE_QUERY		= 2,		/* ��ѯ����*/
	MSG_TYPE_RESPONSE	= 3,		/* �ϱ�����*/
	MSG_TYPE_ALL
}E_MsgType;

/* ��Ϣͷ����*/ 
typedef struct
{
	U32	uiMsgType;		/* ��Ϣ����*/
	U32	uiMsgId;
	U32	uiAppPrivate;
	U8	ucOrginator;		/* ��Ϣ������*/
	U8	ucParamOrder;	/*�������ֽ���0:�����ֽ��� 1:�����ֽ���E_OrderType*/
	U16	usParaLength;
}T_MessageHead;

/* ��Ϣ�ṹ����*/
typedef struct
{
	T_MessageHead tMsgHead;
	U8 aucParam[MSG_PARAM_LEN];
}T_Msg;

//#ifdef __cplusplus
//}
//#endif

#endif


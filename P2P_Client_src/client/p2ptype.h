#ifndef __TYPE_H__
#define __TYPE_H__

#if 0 //old
#define P2P_BUF_LEN_NOT_ENOUGH 			(-1000)   
#define P2P_NOT_SUPPORT_CHIP				(-1001)
#define P2P_UNKNOW_SENSOR_CHIP				(-1002)
#define P2P_INPUT_PARAM_ERR					(-1003)
#define P2P_MORE_THAN_MAX_MEDIA_NUM		(-1004)
#define P2P_DEV_INIT_ERR						(-1005)
#define P2P_MEDIA_SYS_NOT_INIT				(-1006)
#define P2P_UNKNOW_ERR						(-1007)
#define P2P_NOT_CREATE_JPEG_SNAP_MEDIA	(-1008)
#define P2P_OP_TIME_OUT						(-1009)
#define P2P_NOT_CREATE_AUDIO_MEDIA			(-1010)
#define P2P_WORNG_MEDIA_ID					(-1011)
#define P2P_MEM_MALLOC_ERR					(-1012)
#define P2P_NULL_PTR_ERR						(-1013)

#define NET_SEND_BUFFER_SIZE 1024*1024*2
#define NET_SEND_IO_BUFFER_SIZE 1024*5

//��Ϣ����
//Message for P2P IO contrl
typedef enum {
	IOCTRL_TYPE_LOGIN_REQ,	    //From App to Device
	IOCTRL_TYPE_LOGIN_RESP,      //From Device to App

	IOCTRL_TYPE_CONNECT_REQ,	    //From App to Device
	IOCTRL_TYPE_CONNECT_RESP,      //From Device to App
}P2P_IOCTRL_TYPE;

typedef enum {
	SIO_TYPE_UNKN,
	SIO_TYPE_VIDEO_AUDIO_FRAME,	
	SIO_TYPE_IOCTRL,
	SIO_TYPE_DOWNLOAD_DATA,
	SIO_TYPE_HEART_ALIVE_PACKET,
	SIO_TYPE_UPGRADE_PACKET,
}P2P_STREAM_IO_TYPE;

typedef struct
{
        unsigned char u8StreamIOType; 			//��д RJONE_STREAM_IO_TYPE  �������
        unsigned char  u8Reserved[3];
        unsigned int u32DataSize;	                    //�������ݰ�����
}P2P_STREAM_IO_HEAD;

//for SIO_TYPE_IOCTRL
typedef struct{
	unsigned short u16IOCtrlType;					//�ο� P2P_IOCTRL_TYPE
	unsigned short u16IOCtrlDataSize;
	unsigned char   u8Reserve[4];
}P2P_IO_CTRL_HEAD;

typedef struct _VIDEO_FRAME_HEADER_
{	
	unsigned int nTimeStampMSec;				// ʱ��� ����	
	unsigned int u32FrameDataLen;			       // ֡���ݳ���
	unsigned char   u8Code;						//ѹ����ʽ RJONE_CODE_TYPE
	unsigned char   u8VideoFrameType;			// I֡ or  p֡or b֡  RJONE_VIDEO_FRAME
	unsigned char   u8FrameIndex;			//֡����������I֡ʱ��u8rameIndex = 0��p֡ʱu8rameIndex=u8rameIndex+1. ���������Ҫ�����ж϶�֡�����
	unsigned char   u8Reserved[9];
}P2P_VIDEO_FRAME_HEADER;

typedef struct _AUDIO_FRAME_HEADER_
{
	unsigned char u8Code;							//ѹ����ʽRJONE_CODE_TYPE
	unsigned char u8AudioSamplerate;				//������ RJONE_AUDIO_SAMPLERATE
	unsigned char u8AudioBits;						//����λ��RJONE_AUDIO_DATABITS
	unsigned char u8AudioChannel;					//ͨ��		RJONE_AUDIO_CHANNEL					
	unsigned int nTimeStampMSec;				// ʱ��� ����	
	unsigned int u32FrameDataLen;			// ֡���ݳ���	
	unsigned char   u8FrameIndex;			//֡������ÿ��һ֡ u8rameIndex=u8rameIndex+1. ���������Ҫ�����ж϶�֡�����
	unsigned char   u8Reserved[7];
}P2P_AUDIO_FRAME_HEADER;


// for SIO_TYPE_VIDEO_AUDIO_FRAME
typedef struct _FRAME_HEAD_
{	
	unsigned char u8FrameType;  				       // �ο� RJONE_FRAME_TYPE
	unsigned char u8FrameUseType;				// �ο� FRAME_USE_TYPE	
	unsigned short u16FrameSplitPackTotalNum;	       // ��֡���ݴ�С�������ͻ���ʱ�����зְ���u8FramePackNum��ʾ�ְ����ܸ�����С�ڵ���1  ʱ����ʾδ�ְ���
	unsigned short u16SplitPackNo;					// ��ǰ�ְ������		
	unsigned char   u8Reserved[2];
	union
	{
		P2P_VIDEO_FRAME_HEADER stVideoFrameHead; 
		P2P_AUDIO_FRAME_HEADER stAudioFrameHead;
	};		
}P2P_FRAME_HEAD;

//***********
typedef struct _LIST_WIFI_AP_REQ
{
	unsigned char u8Reserved[8];
}P2P_LOGIN_REQ;
#else
#include <string>
#include <list>
#include <string.h>

#define MAX_COMMAND 256
#define MAXRETRY 3

#define MAX_NAME_SIZE 32

/*p2pͨ��Э��*/

//iMessageTypeֵ
enum {
	MSG_C_LOGIN,
	MSG_R_LOGIN,

	MSG_C_HEART_BEAT,	/* �ͻ��˵���������������Ϣ */
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

//�ֶ����ù��캯�����������ڴ�
template<class _T1>
inline	void constructInPlace(_T1  *_Ptr)
{
	new (static_cast<void*>(_Ptr)) _T1();
}
/// �����䳤ָ��
#define BUFFER_CMD(cmd,name,len) char buffer##name[len]={0};\


//�ͻ��ڵ���Ϣ
struct stUserListNode
{
	char userName[MAX_NAME_SIZE];			// �ڵ������
	unsigned int uiIP;				//  �ڵ��IP
	unsigned short usPORT;			// �ڵ�� PORT

	stUserListNode()
	{
		memset(this, 0, sizeof(*this));
	}
};

//Server��ͻ��˷��ʹ�������Ϣ
struct stTransMsg
{
	char userName[MAX_NAME_SIZE];			// �ڵ������
	unsigned int uiIP;			// ��Ҫ���͸��ͻ��˵� IP
	unsigned short usPORT;		// ��Ҫ���͸��ͻ��˵� PORT

	stTransMsg()
	{
	//	uiIP = 0;
	//	usPORT = 0;
		memset(this, 0, sizeof(*this));
	}
};

//���ڿͻ���֮���ͨ��
struct stP2PMsg
{
	unsigned int uiSendLen;				// p2p ���͵���Ϣ����
	char cP2PCommUserName[MAX_NAME_SIZE];		// ��¼������Ϣ���û���

	stP2PMsg()
	{
		memset(this, 0, sizeof(*this));
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
	char  cToName[MAX_NAME_SIZE];		// ��Ҫp2p ͨ�ŵ� �ͻ�������

	stTransMsg  	transMsg;			// ������ת���� 
//	union
//    {
    	stP2PMsg		p2pMsg;				// �ͻ���֮���p2pͨ��
    	stRelayMsg		rlyMsg;			//relay message.
//    };

	unsigned int   userNums;			// ȫ���Ŀͻ�������
	stUserListNode userList[0];			//����server��ͻ��˷��Ϳͻ��б�

	stCommMsg()
	{
		memset(this, 0, sizeof(*this));
	}

	unsigned int getSize() const { return sizeof(*this) + userNums * sizeof(stUserListNode); }
};

using namespace std;
typedef list<stUserListNode*> UserList;

#endif

#endif
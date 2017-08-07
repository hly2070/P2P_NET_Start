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

//消息解析
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
        unsigned char u8StreamIOType; 			//填写 RJONE_STREAM_IO_TYPE  中相关项
        unsigned char  u8Reserved[3];
        unsigned int u32DataSize;	                    //整个数据包长度
}P2P_STREAM_IO_HEAD;

//for SIO_TYPE_IOCTRL
typedef struct{
	unsigned short u16IOCtrlType;					//参考 P2P_IOCTRL_TYPE
	unsigned short u16IOCtrlDataSize;
	unsigned char   u8Reserve[4];
}P2P_IO_CTRL_HEAD;

typedef struct _VIDEO_FRAME_HEADER_
{	
	unsigned int nTimeStampMSec;				// 时间戳 毫秒	
	unsigned int u32FrameDataLen;			       // 帧数据长度
	unsigned char   u8Code;						//压缩格式 RJONE_CODE_TYPE
	unsigned char   u8VideoFrameType;			// I帧 or  p帧or b帧  RJONE_VIDEO_FRAME
	unsigned char   u8FrameIndex;			//帧计数，当是I帧时，u8rameIndex = 0，p帧时u8rameIndex=u8rameIndex+1. 这个变量主要用来判断丢帧情况。
	unsigned char   u8Reserved[9];
}P2P_VIDEO_FRAME_HEADER;

typedef struct _AUDIO_FRAME_HEADER_
{
	unsigned char u8Code;							//压缩格式RJONE_CODE_TYPE
	unsigned char u8AudioSamplerate;				//采样率 RJONE_AUDIO_SAMPLERATE
	unsigned char u8AudioBits;						//采样位宽RJONE_AUDIO_DATABITS
	unsigned char u8AudioChannel;					//通道		RJONE_AUDIO_CHANNEL					
	unsigned int nTimeStampMSec;				// 时间戳 毫秒	
	unsigned int u32FrameDataLen;			// 帧数据长度	
	unsigned char   u8FrameIndex;			//帧计数，每来一帧 u8rameIndex=u8rameIndex+1. 这个变量主要用来判断丢帧情况。
	unsigned char   u8Reserved[7];
}P2P_AUDIO_FRAME_HEADER;


// for SIO_TYPE_VIDEO_AUDIO_FRAME
typedef struct _FRAME_HEAD_
{	
	unsigned char u8FrameType;  				       // 参考 RJONE_FRAME_TYPE
	unsigned char u8FrameUseType;				// 参考 FRAME_USE_TYPE	
	unsigned short u16FrameSplitPackTotalNum;	       // 单帧数据大小超过发送缓冲时，进行分包，u8FramePackNum表示分包的总个数。小于等于1  时，表示未分包。
	unsigned short u16SplitPackNo;					// 当前分包的序号		
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

#define MAX_NAME_SIZE 15

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


//客户节点信息
struct stUserListNode
{
	char userName[MAX_NAME_SIZE];			// 节点的名字
	unsigned int uiIP;				//  节点的IP
	unsigned short usPORT;			// 节点的 PORT

	stUserListNode()
	{
		memset(this, 0, sizeof(*this));
	}
};

//Server向客户端发送打洞请求消息
struct stTransMsg
{
	char userName[MAX_NAME_SIZE];			// 节点的名字
	unsigned int uiIP;			// 将要发送给客户端的 IP
	unsigned short usPORT;		// 将要发送给客户端的 PORT

	stTransMsg()
	{
	//	uiIP = 0;
	//	usPORT = 0;
		memset(this, 0, sizeof(*this));
	}
};

//用于客户端之间的通信
struct stP2PMsg
{
	unsigned int uiSendLen;				// p2p 发送的消息长度
	char cP2PCommUserName[MAX_NAME_SIZE];		// 记录发送消息的用户名

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

//通用消息格式
struct stCommMsg
{
	unsigned int uiMsgType;				// 客户端和服务器通信的类型
	unsigned int result;  //0:success 1:failed
	char cMyName[MAX_NAME_SIZE];		// 本客户端自己的名字	
	char  cToName[MAX_NAME_SIZE];		// 想要p2p 通信的 客户端名字

	stTransMsg  	transMsg;			// 服务器转发的 
//	union
//    {
    	stP2PMsg		p2pMsg;				// 客户端之间的p2p通信
    	stRelayMsg		rlyMsg;			//relay message.
//    };

	unsigned int   userNums;			// 全部的客户端数量
	stUserListNode userList[0];			//用于server向客户端发送客户列表

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
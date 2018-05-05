/**************************************************************************
* ��    Ȩ�� 	Copyright (c) 2018 FTC.
* �ļ����ƣ�	typedef.h
* �ļ���ʶ�� 
* ����ժҪ�� 
* ����˵���� 
* ��ǰ�汾��	V1.0
* ��    �ߣ�	hly2070
* ������ڣ�	2018��5�� 3��
*
* �޸ļ�¼1	��
* �޸����ڣ�
* �� �� �ţ�
* �� �� �ˣ�
* �޸����ݣ�
**************************************************************************/
#ifndef __FTCTYPEDEF_H__
#define __FTCTYPEDEF_H__

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                         ͷ�ļ�����                                     *
 **************************************************************************/

/**************************************************************************
 *						  ��������									 *
 **************************************************************************/
#define	TRUE			(1)
#define	FALSE			(0)
	
#define	FTC_OK				(0)
#define	FTC_FAIL			(-1)

#define	INVALID_SOCKET		(-1)

#define MAX_NAME_SIZE 32

/**************************************************************************
 *                         ��������                                    *
 **************************************************************************/
#ifndef     VOID
#define     VOID	void
#endif

#ifndef 	U8
#define 	U8		unsigned char
#endif

#ifndef 	BYTE
#define 	BYTE	unsigned char
#endif

#ifndef 	BOOL
#define 	BOOL	unsigned char
#endif

#ifndef 	S8
#define 	S8		char
#endif

#ifndef 	U16
#define 	U16		unsigned short
#endif

#ifndef 	S16
#define 	S16		short
#endif

#ifndef 	U32
#define 	U32		unsigned int
#endif

#ifndef 	S32
#define 	S32		int
#endif

#ifndef		F32
#define 	F32 	float
#endif

#ifndef		U64
#define		U64		unsigned long long
#endif

#ifndef 	S64
#define 	S64		long long
#endif

#ifndef 	F64
#define 	F64		double
#endif
		
#ifndef     LONG
#define     LONG	long
#endif
	
#ifndef     ULONG
#define     ULONG	unsigned long
#endif
	
#ifndef 	SOCKET
#define 	SOCKET	int
#endif
	
#ifndef     STATIC
#define     STATIC  static
#endif
	
#ifndef     CONST
#define     CONST   const
#endif

#define P2P_BUF_LEN_NOT_ENOUGH 				(-1000)   
#define P2P_NOT_SUPPORT_CHIP				(-1001)
#define P2P_UNKNOW_SENSOR_CHIP				(-1002)
#define P2P_INPUT_PARAM_ERR					(-1003)
#define P2P_MORE_THAN_MAX_MEDIA_NUM			(-1004)
#define P2P_DEV_INIT_ERR					(-1005)
#define P2P_MEDIA_SYS_NOT_INIT				(-1006)
#define P2P_UNKNOW_ERR						(-1007)
#define P2P_NOT_CREATE_JPEG_SNAP_MEDIA		(-1008)
#define P2P_OP_TIME_OUT						(-1009)
#define P2P_NOT_CREATE_AUDIO_MEDIA			(-1010)
#define P2P_WORNG_MEDIA_ID					(-1011)
#define P2P_MEM_MALLOC_ERR					(-1012)
#define P2P_NULL_PTR_ERR					(-1013)

/* ״̬�볣������*/
typedef enum
{
	STATE_CODE_NO_ERROR							= 0,
	STATE_CODE_INIT_FAILURE						= 1,
	STATE_CODE_INVALID_HANDLE					= 2,
	STATE_CODE_UNABLE_TO_OPEN_FILE				= 3,
	STATE_CODE_CONFIG_ERROR						= 4,
	STATE_CODE_ALLOCATION_FAILURE				= 5,
	STATE_CODE_FREE_MEMORY_FAILURE				= 6,
	STATE_CODE_SOCKET_CREATE_FAILURE			= 7,
	STATE_CODE_SOCKET_CLOSE_FAILURE				= 8,
	STATE_CODE_SOCKET_BIND_FAILURE				= 9,
	STATE_CODE_FAILED_TO_PROCEED_COMMAND 		= 10,
	STATE_CODE_INVALID_COMMAND					= 11,
	STATE_CODE_INVALID_PARAM					= 12,
	STATE_CODE_TIME_OUT							= 13,
	STATE_CODE_OBJECT_EXISTED					= 14,
	STATE_CODE_OBJECT_NOT_EXIST					= 15,
	STATE_CODE_OBJECT_BEYOND					= 16, /* ������Χ*/ 
	STATE_CODE_OBJECT_BUSY						= 17,
	STATE_CODE_CONNECT_AUTH                     = 18,  /*���Ӽ�Ȩ*/
	STATE_CODE_USER_NOT_EXIST					= 19, /*�û�������*/
	STATE_CODE_PASSWD_ERROR						= 20, /*�������*/
	STATE_CODE_TF_NOT_EXIST						= 21,//TF��������
	STATE_CODE_IO_ERROR							= 22,//��д����
	STATE_CODE_ENCPYPTION_ERR					= 23,//���μ���У��ʧ��
	STATE_CODE_UNDEFINED_ERROR					= -1,
	STATE_CODE_XMSDK_TIMEOUT                    = -10005
}E_StateCode;

#ifdef __cplusplus
}
#endif


#endif


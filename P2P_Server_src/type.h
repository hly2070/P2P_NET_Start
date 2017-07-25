#ifndef __TYPE_H__
#define __TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

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

typedef char				CHAR;
typedef char				INT8;
typedef short				INT16;
typedef int				INT32;
typedef unsigned char 		UCHAR;
typedef unsigned short		UINT16;
typedef unsigned int		UINT32;
typedef unsigned long long		UINT64;

typedef void *			HANDLE;
typedef void				VOID;

#ifdef __cplusplus
}
#endif


#endif


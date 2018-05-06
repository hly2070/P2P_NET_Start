/**************************************************************************
* 版    权： 	Copyright (c) 2018 FTC.
* 文件名称：	FTCDebug.h
* 文件标识： 
* 内容摘要：	调试打印接口封装
* 其它说明： 
* 当前版本：	V1.0
* 作    者：	hly2070
* 完成日期：	2018年05月 04日
*
* 修改记录1	：
* 修改日期：
* 版 本 号：
* 修 改 人：
* 修改内容：
**************************************************************************/
#ifndef __FTC_DEBUG_H__
#define __FTC_DEBUG_H__

#ifdef		__cplusplus
extern		"C"
{
#endif
#include <stdio.h>

#define NONE         "\033[m"
#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"

#define DPRINTK(fmt, args...)	printf("(%s,%d)%s: " fmt,__FILE__,__LINE__, __FUNCTION__ , ## args)
#define P2P_DBG_TRACE         printf("[ %s, %d ]=> ",__FILE__,  __LINE__);printf("\r\n");
#define P2P_DBG_DEBUG(msg...) printf("[ %s, %d ]=> ",__FILE__,  __LINE__);printf(msg);printf("\r\n");
#define P2P_DBG_ERROR(msg...) fprintf(stderr, "[ %s, %d ]=> ",__FILE__,  __LINE__);printf(msg);printf("\r\n")
#define P2P_DBG_ASSERT(exp...) assert(exp)

#ifdef		__cplusplus
}
#endif

#endif

/**************************************************************************
* 版    权： 	Copyright (c) 2018 FTC.
* 文件名称：	FTCPeer.c
* 文件标识： 
* 内容摘要：	Peer端信息维护接口封装
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
#include <iostream>
#include "FTCPeer.h"

using namespace std;

/**********************************************************************
* 函数名称：GetPeerByName
* 功能描述：获取peer节点信息
* 输入参数：userName Peer节点携带的用户ID
* 输出参数：无
* 返 回 值：节点
* 其它说明： 
* 修改日期		  版本号	 修改人 	  修改内容
* -----------------------------------------------
* 2018/05/04	  V1.0		 hly2070	
***********************************************************************/
stUserListNode GetPeerByName(UserList *ptPeerList, const S8 *userName)
{
	for (UserList::iterator ClientList_iter = ptPeerList->begin(); ClientList_iter != ptPeerList->end(); ++ClientList_iter)
	{
		if (strcmp((*ClientList_iter)->userName, userName) == 0)
			return *(*ClientList_iter);
	}
	
	std::cout << "can not find user: " << userName << std::endl;
}


/**********************************************************************
* 函数名称：CheckPeerListByName
* 功能描述：查看用户是否存在节点链表中
* 输入参数：userName	Peer节点携带的用户ID
* 输出参数：无
* 返 回 值：节点是否存在的BOOL类型变量
* 其它说明： 
* 修改日期		  版本号	 修改人 	  修改内容
* -----------------------------------------------
* 2018/05/04	  V1.0		 hly2070	
***********************************************************************/
BOOL CheckPeerListByName(UserList *ptPeerList, const S8 *userName)
{
	BOOL isExist = FALSE;

	for (UserList::iterator  ClientList_iter = ptPeerList->begin(); ClientList_iter != ptPeerList->end(); ++ClientList_iter)
	{
		if (strcmp((*ClientList_iter)->userName, userName) == 0)
		{
			isExist = TRUE;
			break;
		}
	}

	return isExist;
}

/**********************************************************************
* 函数名称：RemovePeerByName
* 功能描述：从peer链表中删除一个节点
* 输入参数：userName	Peer节点携带的用户ID
* 输出参数：无
* 返 回 值：无
* 其它说明： 
* 修改日期		  版本号	 修改人 	  修改内容
* -----------------------------------------------
* 2018/05/04	  V1.0		 hly2070	
***********************************************************************/
void RemovePeerByName(UserList *ptPeerList, const S8 *userName)
{

	for (UserList::iterator  ClientList_iter = ptPeerList->begin(); ClientList_iter != ptPeerList->end(); ++ClientList_iter)
	{
		if (strcmp((*ClientList_iter)->userName, userName) == 0)
		{
			ClientList_iter = ptPeerList->erase(ClientList_iter);
		}
	}
}



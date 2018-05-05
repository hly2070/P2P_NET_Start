/**************************************************************************
* ��    Ȩ�� 	Copyright (c) 2018 FTC.
* �ļ����ƣ�	FTCPeer.c
* �ļ���ʶ�� 
* ����ժҪ��	Peer����Ϣά���ӿڷ�װ
* ����˵���� 
* ��ǰ�汾��	V1.0
* ��    �ߣ�	hly2070
* ������ڣ�	2018��05�� 04��
*
* �޸ļ�¼1	��
* �޸����ڣ�
* �� �� �ţ�
* �� �� �ˣ�
* �޸����ݣ�
**************************************************************************/
#include <iostream>
#include "FTCPeer.h"

using namespace std;

/**********************************************************************
* �������ƣ�GetPeerByName
* ������������ȡpeer�ڵ���Ϣ
* ���������userName Peer�ڵ�Я�����û�ID
* �����������
* �� �� ֵ���ڵ�
* ����˵���� 
* �޸�����		  �汾��	 �޸��� 	  �޸�����
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
* �������ƣ�CheckPeerListByName
* �����������鿴�û��Ƿ���ڽڵ�������
* ���������userName	Peer�ڵ�Я�����û�ID
* �����������
* �� �� ֵ���ڵ��Ƿ���ڵ�BOOL���ͱ���
* ����˵���� 
* �޸�����		  �汾��	 �޸��� 	  �޸�����
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
* �������ƣ�RemovePeerByName
* ������������peer������ɾ��һ���ڵ�
* ���������userName	Peer�ڵ�Я�����û�ID
* �����������
* �� �� ֵ����
* ����˵���� 
* �޸�����		  �汾��	 �޸��� 	  �޸�����
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



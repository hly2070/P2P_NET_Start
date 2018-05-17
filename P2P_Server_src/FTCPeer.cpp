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
#include "FTCDebug.h"

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
T_PeerInfo GetPeerByName(PeerList *ptPeerList, S8 *userName)
{
	for (PeerList::iterator ClientList_iter = ptPeerList->begin(); ClientList_iter != ptPeerList->end(); ++ClientList_iter)
	{
		if (strcmp((*ClientList_iter)->name, userName) == 0)
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
BOOL CheckPeerListByName(PeerList *ptPeerList, S8 *strUserName)
{
	BOOL isExist = FALSE;
	
	if(NULL == ptPeerList || NULL == strUserName)
	{
		P2P_DBG_ERROR("invalid value input!");
		return isExist;
	}
	
	for (PeerList::iterator  ClientList_iter = ptPeerList->begin(); ClientList_iter != ptPeerList->end(); ++ClientList_iter)
	{
		if (strcmp((*ClientList_iter)->name, strUserName) == 0)
		{
			isExist = TRUE;
			//P2P_DBG_DEBUG("find name: %s in PeerList", strUserName);
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
void RemovePeerByName(PeerList *ptPeerList, S8 *strUserName)
{
	if(NULL == ptPeerList || NULL == strUserName)
	{
		P2P_DBG_ERROR("invalid value input!");
		return;
	}
	
	for (PeerList::iterator  ClientList_iter = ptPeerList->begin(); ClientList_iter != ptPeerList->end(); ++ClientList_iter)
	{
		if (strcmp((*ClientList_iter)->name, strUserName) == 0)
		{
			ClientList_iter = ptPeerList->erase(ClientList_iter);
		}
	}
}



// clientDlg.h : 头文件
//
#include "p2ptype.h"

#pragma once

// CclientDlg 对话框
class CclientDlg : public CDialog
{
// 构造
public:
	CclientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
//	int ClientSetCmdSendBuf(char * pSendBuf, int iBufMaxLen, P2P_IOCTRL_TYPE u16IOCtrlType, char * pData, int iDataLen);

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	BOOL bLogin;
	BOOL bConnect;
	BOOL bRelay;
	int m_nServerPort;
	CString m_strServerAddr;	
	CString m_strMyID;
	CString m_strToID;
	UserList ClientList;//客户节点列表
	SOCKET m_cliSocket;
	sockaddr_in remoteAddr;
	sockaddr_in localAddr;
	char cliIpAddr[32];
	HANDLE h_recv;

	CEdit m_EditRecv;
	CEdit m_EditInfo;

	void GetUserList();
	void GetLocalIpAddr();
	void UpdateCtrlStates();
	stUserListNode GetLocalUser(const char *userName);

	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnLogin();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedBtnConnet();
	afx_msg void OnBnClickedBtnDisconnect();
	afx_msg void OnBnClickedBtnGetUsers();
	afx_msg void OnBnClickedBtnSend();
	afx_msg void OnBnClickedBtnClear();
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

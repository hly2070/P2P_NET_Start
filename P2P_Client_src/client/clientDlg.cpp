// clientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "client.h"
#include "clientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool RecvedACK;//�յ���Ϣ��Ӧ��

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CclientDlg �Ի���




CclientDlg::CclientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CclientDlg::IDD, pParent)
	, m_strServerAddr(_T("43.252.231.67")) 
	, m_nServerPort(8888)
	, m_strMyID(_T("andy"))
	, m_strToID(_T("john"))
	, m_cliSocket(_T(INVALID_SOCKET))
	, h_recv(NULL)
	, bConnect(FALSE)
	, bRelay(FALSE)
	, bLogin(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
//	memset(cliIpAddr, NULL, sizeof(char));
}

void CclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SERVER, m_strServerAddr);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nServerPort);
	DDX_Text(pDX, IDC_EDIT_MYID, m_strMyID);
	DDX_Text(pDX, IDC_EDIT_TOID, m_strToID);

	DDX_Control(pDX, IDC_EDIT_RECV, m_EditRecv);
	DDX_Control(pDX, IDC_EDIT_LOG, m_EditInfo);
}

BEGIN_MESSAGE_MAP(CclientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON2, &CclientDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BTN_CONNET, &CclientDlg::OnBnClickedBtnConnet)
	ON_BN_CLICKED(IDC_BTN_DISCONNECT, &CclientDlg::OnBnClickedBtnDisconnect)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_LOGIN, &CclientDlg::OnBnClickedBtnLogin)
	ON_BN_CLICKED(IDC_BTN_GET_USERS, &CclientDlg::OnBnClickedBtnGetUsers)
	ON_BN_CLICKED(IDC_BTN_SEND, &CclientDlg::OnBnClickedBtnSend)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CclientDlg::OnBnClickedBtnClear)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CclientDlg ��Ϣ�������

BOOL CclientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
//	GetLocalIpAddr();

	UpdateData(true);

	//init socket
	m_cliSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_cliSocket<0)
	{
		MessageBox("Create socket error!");
	}

	//init local addr
	localAddr.sin_family = AF_INET;
	localAddr.sin_port = htons(0);
	localAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	bind(m_cliSocket, (struct sockaddr*)&localAddr, sizeof(localAddr)); 		// todo �ж��Ƿ�ɹ�
	{

	}

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CclientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CclientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CclientDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnBnClickedBtnDisconnect();

	//release something...
	if (h_recv != NULL)
	{
		TerminateThread(h_recv, 0);
		h_recv = NULL;
	}

	if (m_cliSocket != INVALID_SOCKET)
	{
		closesocket(m_cliSocket);
		m_cliSocket = INVALID_SOCKET;
	}

	CDialog::OnOK();
}

DWORD thread_heart_proc(LPVOID arg)
{
	CclientDlg *p_dlg = (CclientDlg *)arg;

	while(1)
	{
		if(!p_dlg->bLogin)
		{
			TerminateThread(p_dlg->h_recv, 0);
			p_dlg->h_recv = NULL;
			break;
		}
		else
		{
			p_dlg->GetUserList();
			Sleep(30000);
		}
	}
		
	return 0;
}

DWORD WINAPI thread_recv_proc(LPVOID arg)
{
	CclientDlg *p_dlg = (CclientDlg *)arg;

	sockaddr_in remote;
	int nLen = sizeof(remote);
	char buf[MAX_PACKET_SIZE];

	while(true)
	{
		int ret = recvfrom(p_dlg->m_cliSocket, (char*)buf, MAX_PACKET_SIZE, 0, (sockaddr*)&remote, &nLen);
		stCommMsg* recvbuf = (stCommMsg*)buf;
		if (ret <= 0)
		{
			continue;
		}
		else
		{
			unsigned int mMsgType = recvbuf->uiMsgType;
			switch (mMsgType)
			{
			case MSG_R_LOGIN:
			{
				if (recvbuf->result == 0)
				{
					p_dlg->bLogin = TRUE;
					p_dlg->UpdateCtrlStates(); //update button states
				}
				else
				{
					p_dlg->GetDlgItem(IDC_EDIT_LOG)->SetWindowText(CString("Login failed!, you have logined in."));
				}
				
				CString str;
				p_dlg->GetDlgItem(IDC_EDIT_LOG)->GetWindowText(str);
				if(!str.IsEmpty())
					str += "\r\n";

				unsigned int usercount = recvbuf->userNums;
				p_dlg->ClientList.clear();
				CString strTmp;
				strTmp.Format("Have %d users logined server:\r\n", usercount);
				str += strTmp;
				p_dlg->GetDlgItem(IDC_EDIT_LOG)->SetWindowText(str);
				p_dlg->m_EditInfo.LineScroll(p_dlg->m_EditInfo.GetLineCount(), 0);	//�Զ����������һ��

				for (int i = 0; i < usercount; i++)
				{
					stUserListNode* user = new stUserListNode();
					memcpy(user->userName, recvbuf->userList[i].userName, MAX_NAME_SIZE);
					user->uiIP = recvbuf->userList[i].uiIP;
					user->usPORT = recvbuf->userList[i].usPORT;
					p_dlg->ClientList.push_back(user);

					in_addr temp;
					temp.S_un.S_addr = htonl(user->uiIP);

					strTmp.Format("User: %s %s:%d", user->userName, inet_ntoa(temp), user->usPORT);
					str += strTmp;
					str += "\r\n";
					p_dlg->GetDlgItem(IDC_EDIT_LOG)->SetWindowText(str);
				}
				
				HANDLE h_getList = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_heart_proc, p_dlg, 0, NULL);

				break;
			}

			case MSG_R_LOGOUT:
			{
				if (recvbuf->result == 0)
				{
					p_dlg->bLogin = FALSE;
				}
				else
				{
					AfxMessageBox("Logout failed!");
				}
				break;
			}

			case MSG_R_HEART_BEAT:
			{
				p_dlg->ClientList.clear();
				unsigned int usercount = recvbuf->userNums;

				for (int i = 0; i < usercount; i++)
				{
					stUserListNode* user = new stUserListNode();
					memcpy(user->userName, recvbuf->userList[i].userName, MAX_NAME_SIZE);
					user->uiIP = recvbuf->userList[i].uiIP;
					user->usPORT = recvbuf->userList[i].usPORT;
					p_dlg->ClientList.push_back(user);
				}

				break;
			}

			case MSG_R_GET_PEERS:
			{
				CString str;
				p_dlg->GetDlgItem(IDC_EDIT_LOG)->GetWindowText(str);
				if(!str.IsEmpty())
					str += "\r\n";

				unsigned int usercount = recvbuf->userNums;
				p_dlg->ClientList.clear();
				CString strTmp;
				strTmp.Format("Have %d users login server:\r\n", usercount);
				str += strTmp;
			//	p_dlg->GetDlgItem(IDC_EDIT_LOG)->SetWindowText(str);

				for (int i = 0; i < usercount; i++)
				{
					stUserListNode* user = new stUserListNode();
					memcpy(user->userName, recvbuf->userList[i].userName, MAX_NAME_SIZE);
					user->uiIP = recvbuf->userList[i].uiIP;
					user->usPORT = recvbuf->userList[i].usPORT;
					p_dlg->ClientList.push_back(user);

					in_addr temp;
					temp.S_un.S_addr = htonl(user->uiIP);

					strTmp.Format("User:%s %s:%d\r\n", user->userName, inet_ntoa(temp), user->usPORT);
					str += strTmp;
					p_dlg->GetDlgItem(IDC_EDIT_LOG)->SetWindowText(str);
					p_dlg->m_EditInfo.LineScroll(p_dlg->m_EditInfo.GetLineCount(), 0);	//�Զ����������һ��
				}
				break;
			}

			case MSG_R_HOLE:
			{
				if (recvbuf->result == 1)
				{
					AfxMessageBox("Romote user does not exist!!!");	
					p_dlg->GetDlgItem(IDC_BTN_CONNET)->EnableWindow(TRUE);
				}
				else
				{
					p_dlg->GetDlgItem(IDC_BTN_CONNET)->EnableWindow(FALSE);
				}
				break;
			}

			case P2PMESSAGE:
			{
				char *recvmessage = new char[recvbuf->p2pMsg.uiSendLen];
				int recv = recvfrom(p_dlg->m_cliSocket, recvmessage, MAX_PACKET_SIZE, 0, (sockaddr*)&remote, &nLen);
				recvmessage[recv - 1] = '\0';
				if (recv <= 0)
					AfxMessageBox("Recv Message Error!");
				else
				{
					CString str;
					p_dlg->GetDlgItem(IDC_EDIT_RECV)->GetWindowText(str);
					if(!str.IsEmpty())
						str += "\r\n";

					CString strTmp;
					in_addr tmp;
					tmp.S_un.S_addr = remote.sin_addr.S_un.S_addr;
					strTmp.Format("%s (%s) say : \r\n  %s", recvbuf->p2pMsg.cP2PCommUserName, inet_ntoa(tmp), recvmessage);
					strTmp += "\r\n";
					str += strTmp;
					p_dlg->GetDlgItem(IDC_EDIT_RECV)->SetWindowText(str);

					p_dlg->m_EditRecv.LineScroll(p_dlg->m_EditRecv.GetLineCount(), 0);

					stCommMsg sendbuf;
					sendbuf.uiMsgType = P2PMESSAGEACK;
					sendto(p_dlg->m_cliSocket, (const char*)&sendbuf, sendbuf.getSize(), 0, (const sockaddr*)&remote, sizeof(remote));
				}
				delete[]recvmessage;
				break;
			}

			case RELAYMESSAGE2:
			{
				if(recvbuf->rlyMsg.uiSendLen > 0)
				{
					char *recvmessage = new char[recvbuf->rlyMsg.uiSendLen];
					int recv = recvfrom(p_dlg->m_cliSocket, recvmessage, MAX_PACKET_SIZE, 0, (sockaddr*)&remote, &nLen);
					recvmessage[recv - 1] = '\0';
				//	AfxMessageBox(CString(recvmessage));
					if (recv <= 0)
						AfxMessageBox("Recv Message Error!");
					else
					{
						if(recvbuf->rlyMsg.uiSendLen == recv)
						{
							char message[256];
							strcpy(message, recvmessage);
						//	AfxMessageBox(CString(message));
							
							stUserListNode fromPeer = p_dlg->GetLocalUser(recvbuf->cMyName);
						//	AfxMessageBox(CString(recvmessage));
							
							sockaddr_in fromAddr;
							fromAddr.sin_family = AF_INET;
							fromAddr.sin_port = htons(fromPeer.usPORT);
							fromAddr.sin_addr.s_addr = htonl(fromPeer.uiIP);
							
							CString str;
							p_dlg->GetDlgItem(IDC_EDIT_RECV)->GetWindowText(str);
							if (!str.IsEmpty())
								str += "\r\n";

							CString strTmp;
							in_addr tmp;
							tmp.S_un.S_addr = fromAddr.sin_addr.S_un.S_addr;
							strTmp.Format("%s (%s) say : \r\n  %s", recvbuf->rlyMsg.sUserName, inet_ntoa(tmp), 
								message);
							strTmp += "\r\n";
							str += strTmp;
							p_dlg->GetDlgItem(IDC_EDIT_RECV)->SetWindowText(str);

							p_dlg->m_EditRecv.LineScroll(p_dlg->m_EditRecv.GetLineCount(), 0);
						}
					}
					delete[]recvmessage;
				}
				
				break;
			}

			case P2PMESSAGEACK://�յ���Ϣ��Ӧ��
			{
				RecvedACK = true;
				break;
			}

			case P2PSOMEONEWANTTOCALLYOU:
			{
				//���յ��������ָ��IP��
				CString str;
				p_dlg->GetDlgItem(IDC_EDIT_LOG)->GetWindowText(str);
				str += "\r\n";

				sockaddr_in peerCli;
				peerCli.sin_family = AF_INET;
				peerCli.sin_port = htons(recvbuf->transMsg.usPORT);
				peerCli.sin_addr.S_un.S_addr = htonl(recvbuf->transMsg.uiIP);

				CString strTmp;
				strTmp.Format("%s[%s:%d] want to connect to you.", recvbuf->transMsg.userName, 
					inet_ntoa(peerCli.sin_addr), htons(peerCli.sin_port));
				str += strTmp;
				p_dlg->GetDlgItem(IDC_EDIT_LOG)->SetWindowText(str);
				p_dlg->m_EditInfo.LineScroll(p_dlg->m_EditInfo.GetLineCount(), 0);	//�Զ����������һ��

				//unblock
			/*	int re = 0;
				unsigned long ul = 1;
				re = ioctlsocket(p_dlg->m_cliSocket, FIONBIO, (unsigned long*)&ul);*/

				//UDP hole punching
				stCommMsg holeMessage;
				holeMessage.uiMsgType = P2PPUNCH;
				sendto(p_dlg->m_cliSocket, (const char*)&holeMessage, sizeof(holeMessage), 0, 
					(const sockaddr*)&peerCli, sizeof(peerCli));

				Sleep(500);

				stCommMsg sendbuf;
				sendbuf.uiMsgType = P2PHAVECONNECT1;
				strcpy(sendbuf.cMyName, p_dlg->m_strMyID.GetBuffer());
				strcpy(sendbuf.cToName, p_dlg->m_strToID.GetBuffer());

				sendto(p_dlg->m_cliSocket, (const char*)&sendbuf, sendbuf.getSize(), 0, 
					(const sockaddr*)&p_dlg->remoteAddr, sizeof(p_dlg->remoteAddr));

				//block
			/*	ul = 0;
				re = ioctlsocket(p_dlg->m_cliSocket, FIONBIO, (unsigned long*)&ul);*/

				break;
			}

			case P2PPUNCH:
			{
				// �Է����͵Ĵ���Ϣ������
				CString str;
				CString strTmp;

				p_dlg->GetDlgItem(IDC_EDIT_LOG)->GetWindowText(str);
				if (!str.IsEmpty())
					str += "\r\n";
				strTmp.Format("connected %s(%s:%d) by P2P", recvbuf->cMyName, inet_ntoa(remote.sin_addr), 
					htons(remote.sin_port));
				str += strTmp;
				p_dlg->GetDlgItem(IDC_EDIT_LOG)->SetWindowText(str);
				p_dlg->m_EditInfo.LineScroll(p_dlg->m_EditInfo.GetLineCount(), 0);	//�Զ����������һ��
			/*
				stCommMsg sendbuf;
				sendbuf.uiMsgType = P2PPUNCHOK;
				strcpy(sendbuf.cMyName, p_dlg->m_strMyID.GetBuffer());
				strcpy(sendbuf.cToName, p_dlg->m_strToID.GetBuffer());

				sendto(p_dlg->m_cliSocket, (const char*)&sendbuf, sendbuf.getSize(), 0, (const sockaddr*)&p_dlg->remoteAddr, sizeof(p_dlg->remoteAddr));
			*/
				break;
			}

			case P2PCANSTART:
			{
				stUserListNode toPeer = p_dlg->GetLocalUser(p_dlg->m_strToID.GetBuffer());

				sockaddr_in remotePeer;
				remotePeer.sin_family = AF_INET;
				remotePeer.sin_port = htons(toPeer.usPORT);
				remotePeer.sin_addr.s_addr = htonl(toPeer.uiIP);

				in_addr temp;
				temp.s_addr = htonl(toPeer.uiIP);

				stCommMsg mTransMsg;
				mTransMsg.uiMsgType = P2PPUNCH2;
				sendto(p_dlg->m_cliSocket, (const char*)&mTransMsg, sizeof(stCommMsg), 0, 
					(const sockaddr*)&remotePeer, sizeof(remotePeer));

				p_dlg->SetTimer(1, 2000, 0);

				break;
			}

			case P2PPUNCH2:
			{
				stCommMsg mTransMsg;
				mTransMsg.uiMsgType = P2PPUNCHOK;
				sendto(p_dlg->m_cliSocket, (const char*)&mTransMsg, sizeof(stCommMsg), 0, 
					(const sockaddr*)&remote, sizeof(remote));
				break;
			}

			case P2PPUNCHOK:
			{
				CString str;
				p_dlg->GetDlgItem(IDC_EDIT_LOG)->GetWindowText(str);
				if (!str.IsEmpty())
					str += "\r\n";
				str += CString("has setup P2P connect!");
				p_dlg->GetDlgItem(IDC_EDIT_LOG)->SetWindowText(str);
				p_dlg->m_EditInfo.LineScroll(p_dlg->m_EditInfo.GetLineCount(), 0);	//�Զ����������һ��

				p_dlg->bConnect = TRUE;

				break;
			}

			}
		}
	}

	return 0;
}

void CclientDlg::OnBnClickedBtnConnet()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);

	stCommMsg sendbuf;
	sendbuf.uiMsgType = MSG_C_HOLE;
	strcpy(sendbuf.cMyName, m_strMyID.GetBuffer());
	strcpy(sendbuf.cToName, m_strToID.GetBuffer());

	sendto(m_cliSocket, (const char*)&sendbuf, sendbuf.getSize(), 0, (const sockaddr*)&remoteAddr, sizeof(remoteAddr));
//	GetDlgItem(IDC_BTN_CONNET)->EnableWindow(FALSE);
}

void CclientDlg::OnBnClickedBtnDisconnect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_cliSocket == INVALID_SOCKET)
	{
		return;
	}

	UpdateData(TRUE);

	stCommMsg sendbuf;
	sendbuf.uiMsgType = MSG_C_LOGOUT;
	strcpy(sendbuf.cMyName, m_strMyID.GetBuffer());

	sendto(m_cliSocket, (const char*)&sendbuf, sendbuf.getSize(), 0, (const sockaddr*)&remoteAddr, sizeof(remoteAddr));	

	GetDlgItem(IDC_BTN_LOGIN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_CONNET)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_GET_USERS)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_LOG)->SetWindowTextA("");
	bLogin = TRUE;
}

void CclientDlg::GetLocalIpAddr()
{
	char HostName[100];
	gethostname(HostName, sizeof(HostName));// ��ñ���������.

	hostent* hn;
	hn = gethostbyname(HostName);//���ݱ����������õ�����ip

	//CString strIPAddr = inet_ntoa(*(struct in_addr *)hn->h_addr_list[0]);//��ip�����ַ�����ʽ
//	CString strIPAddr(inet_ntoa(*(struct in_addr *)hn->h_addr_list[0]));//��ip�����ַ�����ʽ

	strcpy(cliIpAddr, inet_ntoa(*(struct in_addr *)hn->h_addr_list[0]));

//	MessageBox(strIPAddr);
}

void CclientDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	if(bLogin)
	{
		OnBnClickedBtnDisconnect();
	}

	//release something...
	if(h_recv != NULL)
	{
		TerminateThread(h_recv, 0);
		h_recv = NULL;
	}

	if(m_cliSocket != INVALID_SOCKET)
	{
		closesocket(m_cliSocket);
		m_cliSocket = INVALID_SOCKET;
	}
}


void CclientDlg::OnBnClickedBtnLogin()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);

	if (m_strMyID.IsEmpty())
	{
		MessageBox("User id is empty!");
		return;
	}

	if (m_strServerAddr.IsEmpty())
	{
		MessageBox("P2P server ip is empty!");
		return;
	}

	CString strPort;
	strPort.Format("%d", m_nServerPort);
	if (strPort.IsEmpty())
	{
		MessageBox("P2P server port is empty!");
		return;
	}

	if (m_cliSocket == INVALID_SOCKET)
	{
		MessageBox("socket error!");
		return;
	}

	//init remote addr
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons(m_nServerPort);
	remoteAddr.sin_addr.S_un.S_addr = inet_addr(m_strServerAddr);

	stCommMsg sendbuf;
	sendbuf.uiMsgType = MSG_C_LOGIN;
	memcpy(sendbuf.cMyName, m_strMyID.GetBuffer(), m_strMyID.GetLength());

	int send_count;//�˱������ڵ���
	send_count = sendto(m_cliSocket, (const char*)&sendbuf, sendbuf.getSize(), 0, (const sockaddr*)&remoteAddr, sizeof(remoteAddr));	

	if (h_recv == NULL)
		h_recv = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_recv_proc, this, 0, NULL);
}

void CclientDlg::UpdateCtrlStates()
{
	GetDlgItem(IDC_BTN_LOGIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_CONNET)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_GET_USERS)->EnableWindow(TRUE);

	GetDlgItem(IDC_EDIT_LOG)->SetWindowTextA("Login on P2P server success.");
}

void CclientDlg::GetUserList()
{
	stCommMsg sendbuf;
	sendbuf.uiMsgType = MSG_C_HEART_BEAT;
	strcpy(sendbuf.cMyName, m_strMyID.GetBuffer());

	sendto(m_cliSocket, (const char*)&sendbuf, sendbuf.getSize(), 0, (const sockaddr*)&remoteAddr, sizeof(remoteAddr));
}

void CclientDlg::OnBnClickedBtnGetUsers()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	stCommMsg sendbuf;
	sendbuf.uiMsgType = MSG_C_GET_PEERS;
	strcpy(sendbuf.cMyName, m_strMyID.GetBuffer());

	sendto(m_cliSocket, (const char*)&sendbuf, sendbuf.getSize(), 0, (const sockaddr*)&remoteAddr, sizeof(remoteAddr));
}

stUserListNode CclientDlg::GetLocalUser(const char *userName)
{
	for (UserList::iterator ClientList_iter = ClientList.begin(); ClientList_iter != ClientList.end(); ++ClientList_iter)
	{
		if (strcmp((*ClientList_iter)->userName, userName) == 0)
			return *(*ClientList_iter);
	}
}

void CclientDlg::OnBnClickedBtnSend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������	
	UpdateData(true);

	if (m_cliSocket == INVALID_SOCKET)
	{
		MessageBox("socket error!");
		return;
	}

	if (m_strToID.IsEmpty())
	{
		MessageBox("Please set to peer name");
		return;
	}

	unsigned int UserIP;
	unsigned short UserPort;
	char message[MAX_COMMAND];
	CString strMsg;
	GetDlgItem(IDC_EDIT_SEND)->GetWindowText(strMsg);
	if(strMsg.IsEmpty())
	{
		MessageBox("Send message is empty!");
		return;
	}

	//
	CString str;
	GetDlgItem(IDC_EDIT_RECV)->GetWindowText(str);
	if (!str.IsEmpty())
		str += "\r\n";

	CString strTmp;
	strTmp.Format("I say : \r\n  %s", strMsg);
	strTmp += "\r\n";
	str += strTmp;
	GetDlgItem(IDC_EDIT_RECV)->SetWindowText(str);

	m_EditRecv.LineScroll(m_EditRecv.GetLineCount(), 0);	//�Զ����������һ��

	strcpy(message, strMsg.GetBuffer());
	
	if (!bRelay)
	{
		//start to send message.
	//	strcpy(message, strMsg.GetBuffer());
		char userName[15];
		strcpy(userName, m_strToID.GetBuffer());

		stUserListNode toPeer = GetLocalUser(userName);
		if (&toPeer == NULL)
		{
			MessageBox("peer does not exist!");
			return;
		}
		else
		{
			UserIP = toPeer.uiIP;
			UserPort = toPeer.usPORT;
		}

		//	for (int trytime = 0; trytime<MAXRETRY; trytime++)
		//	{
		RecvedACK = false;

		sockaddr_in remotePeer;
		remotePeer.sin_family = AF_INET;
		remotePeer.sin_port = htons(UserPort);
		remotePeer.sin_addr.S_un.S_addr = htonl(UserIP);

		stCommMsg MessageHead;
		MessageHead.uiMsgType = P2PMESSAGE;
		MessageHead.p2pMsg.uiSendLen = (int)strlen(message) + 1;
		strcpy(MessageHead.p2pMsg.cP2PCommUserName, m_strMyID.GetBuffer(m_strMyID.GetLength()));

		GetDlgItem(IDC_EDIT_SEND)->SetWindowText("");

		//����p2p��Ϣͷ
		int send_count = sendto(m_cliSocket, (const char*)&MessageHead, sizeof(MessageHead), 0, (const sockaddr*)&remotePeer, sizeof(remotePeer));
		//����p2p��Ϣ��
		send_count = sendto(m_cliSocket, (const char*)&message, MessageHead.p2pMsg.uiSendLen, 0, (const sockaddr*)&remotePeer, sizeof(remotePeer));

		//�ȴ�������Ϣ�̸߳�RecvedACK��־
		for (int i = 0; i < 10; i++)
		{
			if (RecvedACK)
			{
				return;
			}
			else
				Sleep(300);
		}

		//���û�н��յ�Ŀ�������Ļ�Ӧ����ΪĿ�������Ķ˿�
		//ӳ��û�д򿪣���ô�������󵽷�����Ҫ�󡰴򶴡���
		/*	stCommMsg transMessage;
		transMessage.uiMsgType = MSG_CONNECT_REMOTE_REQ;
		strcpy(transMessage.cToName, m_strToID.GetBuffer(m_strToID.GetLength()));

		sendto(m_cliSocket, (const char*)&transMessage, sizeof(transMessage), 0, (const sockaddr*)&remoteAddr, sizeof(remoteAddr));
		Sleep(100);//�ȴ��Է��ȷ�����Ϣ
		*/
		//	}

		//	GetDlgItem(IDC_EDIT_SEND)->SetWindowText("");
	}
	else
	{
		stCommMsg MessageHead;
		MessageHead.uiMsgType = RELAYMESSAGE;
		strcpy(MessageHead.cMyName, m_strMyID.GetBuffer());
		strcpy(MessageHead.cToName, m_strToID.GetBuffer());
		MessageHead.rlyMsg.uiSendLen = (int)strlen(message)+1;
		strcpy(MessageHead.rlyMsg.sUserName, m_strMyID.GetBuffer(m_strMyID.GetLength()));

		GetDlgItem(IDC_EDIT_SEND)->SetWindowText("");

		//����p2p��Ϣͷ
		int send_count = sendto(m_cliSocket, (const char*)&MessageHead, sizeof(MessageHead), 0, 
		(const sockaddr*)&remoteAddr, sizeof(remoteAddr));
		
		//����p2p��Ϣ��
		send_count = sendto(m_cliSocket, (const char*)&message, MessageHead.rlyMsg.uiSendLen, 0, 
		(const sockaddr*)&remoteAddr, sizeof(remoteAddr));
	}
}


void CclientDlg::OnBnClickedBtnClear()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	GetDlgItem(IDC_EDIT_LOG)->SetWindowText("");
}


BOOL CclientDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:    // ���λس�
			OnBnClickedBtnSend();
				return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}


void CclientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch (nIDEvent)
	{
	case 1:
		if (!bConnect)
		{
			CString str;
			CString strTmp;

			GetDlgItem(IDC_EDIT_LOG)->GetWindowText(str);
			if (!str.IsEmpty())
				str += "\r\n";
			str += CString("P2P connect failed! start relay mode.");
			GetDlgItem(IDC_EDIT_LOG)->SetWindowText(str);

			bRelay = TRUE;

			KillTimer(1);
		}
		break;
	}
	CDialog::OnTimer(nIDEvent);
}

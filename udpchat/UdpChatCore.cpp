#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "resource.h"
#include "WinSockWrappers.h"
#include "UdpChatCore.h"
#include "MsgFormat.h"
#include <time.h>
#include <windowsx.h>
#include <process.h>
#include <list>

using namespace std;

//--------------------------------
// ���C�u����
//--------------------------------
#pragma	comment(lib, "ws2_32.lib")

//--------------------------------
// �\���̐錾
//--------------------------------
typedef struct
{
	char	szName[NAME_LENGTH];
	short	nUserID;
	bool	bEnter;
}UserBuffer, *LPUserBuffer;

typedef struct
{
	char	*pszChatStr;
	int		nChatBuffSize;
}ChatBuffer, *LPChatBuffer;

#define MAX_USER (32)

//--------------------------------
// �O���[�o���ϐ�
//--------------------------------
HWND		g_hWnd;
SOCKET		g_sockNo;
sockaddr_in g_myAddr;
sockaddr_in g_serverAddr;
UserBuffer	g_userList[MAX_USER];
INT			g_userId = 0;
INT			g_userNum = 0;
unsigned short g_portNo;

HANDLE		g_recvThread;

bool		g_mainLoop;

//���b�Z�[�W�ۑ��p�ϒ��z��
list<LPChatBuffer> g_messages;

ChatMsgData g_sendBuff;

//--------------------------------
// �v���g�^�C�v
//--------------------------------
unsigned int __stdcall RecvServerThread(void *);
unsigned int __stdcall RecvClientThread(void *);

LPUserBuffer GetIdToUser(short);

void SetMessageToBufferChat(LPChatMsgData, LPChatBuffer);
void SetMessageToBufferLog(LPChatMsgData, LPChatBuffer);
void MakeChatString(LPChatBuffer, char *, char *);
void MakeLogString(LPChatBuffer, char *);
void InsertData(LPChatBuffer);
void DeleteChatData(LPChatBuffer);

void SendLogData(int);
void DeleteUserData(int);

// WinSock������/�I��
bool InitNetwork()
{
	g_sockNo = INVALID_SOCKET;
	g_mainLoop = false;

	// WinSock�̏��������s�����ʂ�߂�l�ŕԂ�
	return InitWinSock();
}

void UninitNetwork()
{
	// WinSock�̏I������
	UninitWinSock();
}

// �`���b�g�V�X�e��������/�I��
bool InitSystem(HWND hWnd)
{
	PHOSTENT	pHostEnt;
	IN_ADDR		inAddr;
	char		szHostName[256];

	g_hWnd = hWnd;

	// �\�P�b�g�̏����������s���Ė߂�l���O���[�o���ϐ��ɕۑ�
	g_sockNo = InitSocket();

	if (g_sockNo != INVALID_SOCKET)
	{
		// �u���[�h�L���X�g�\�ɐݒ�
		SetLimitedBroadCastMode(g_sockNo, true);

		// �|�[�g�ԍ���ݒ�
		SetAddressToSockAddrIn(&g_myAddr, INADDR_ANY, (unsigned short)PORTNO);

		// IP�A�h���X��ݒ�
		if (gethostname(szHostName, 256) == 0)
		{
			if ((pHostEnt = gethostbyname(szHostName)) != NULL)
			{
				memcpy(&inAddr, pHostEnt->h_addr, 4);
				g_myAddr.sin_addr = inAddr;
			}
		}

		// �\�P�b�g�̏I������
		UninitSocket(&g_sockNo);

		return true;
	}

	return false;
}

bool InitServer(unsigned short port_no)
{
	g_portNo = port_no;

	// �\�P�b�g�̏����������s���Ė߂�l���O���[�o���ϐ��ɕۑ�
	g_sockNo = InitSocket();

	if (g_sockNo != INVALID_SOCKET)
	{
		bool flg = true;

		// �o�C���h�ς݃|�[�g�ԍ��ł̃o�C���h���\�ɂ���
		setsockopt(g_sockNo, SOL_SOCKET, SO_REUSEADDR, (char*)&flg, sizeof(bool));

		// ���������ƂɁAIP�A�h���X�A�|�[�g�ԍ���ݒ�
		SetAddressToSockAddrIn(&g_myAddr, g_myAddr.sin_addr.S_un.S_addr, port_no);

		// �o�C���h
		if (!BindToSocket(g_sockNo, &g_myAddr))
			return false;

		// �X���b�h�J�n
		g_recvThread = (HANDLE)_beginthreadex(NULL, 0, RecvServerThread, NULL, 0, NULL);
		if (g_recvThread == 0)
			return false;

		g_mainLoop = true;

		return true;
	}

	return false;
}
void UninitServer()
{
	// �T�[�o��~���b�Z�[�W�\������
	LPChatBuffer message_data;
	message_data = (LPChatBuffer)malloc(sizeof(ChatBuffer));
	MakeChatString(message_data, "�T�[�o���", "�T�[�o�͒�~���܂���...");
	InsertData(message_data);		//LPChatBuffer��\����	//���O��\�������
	DeleteChatData(message_data);	//ChatBuffer�폜

	if (g_mainLoop)
		g_mainLoop = false;

	// �`���b�g�����폜
	DeleteLogData();
	// �\�P�b�g�̏I������ 
	UninitSocket(&g_sockNo);
	// �X���b�h�ҋ@
	WaitForSingleObject(g_recvThread, INFINITE);

}

bool LoginChat(unsigned short port_no, unsigned long addr)
{
	// �\�P�b�g�̏����������s���Ė߂�l���O���[�o���ϐ��ɕۑ�
	g_sockNo = InitSocket();

	if (g_sockNo != INVALID_SOCKET)
	{
		bool flg = true;
		// �o�C���h�ς݃|�[�g�ԍ��ł̃o�C���h���\�ɂ���
		setsockopt(g_sockNo, SOL_SOCKET, SO_REUSEADDR, (char*)&flg, sizeof(bool));
		// ���������ƂɁAIP�A�h���X�A�|�[�g�ԍ���ݒ�
		SetAddressToSockAddrIn(&g_myAddr, g_myAddr.sin_addr.S_un.S_addr, port_no);

		SetAddressToSockAddrIn(&g_serverAddr, addr, port_no);

		// �o�C���h
		if (!BindToSocket(g_sockNo, &g_myAddr))
			return false;

		// �X���b�h�J�n
		g_recvThread = (HANDLE)_beginthreadex(NULL, 0, RecvClientThread, NULL, 0, NULL);
		if (g_recvThread == 0)
			return false;

		g_mainLoop = true;

		return true;
	}
	return false;
}
void LogoutChat()
{
	if (g_mainLoop)
		g_mainLoop = false;

	g_sendBuff.msgData.Chat.nUserID = 0;
	// �\�P�b�g�̏I������
	UninitSocket(&g_sockNo);
	// �X���b�h�ҋ@
	WaitForSingleObject(g_recvThread, INFINITE);
}

bool EnterRoom(char* name)
{
	if (strlen(name) > NAME_LENGTH)
		return false;

	// ���������T�[�o�֑��M
	SendEnterMessage(name);

	return true;
}

void ExitRoom()
{
	// �ގ������T�[�o�֑��M
	SendExitMessage();
}

// �`���b�g���b�Z�[�W�̍쐬
void SetMessageToBufferChat(LPChatMsgData msg, LPChatBuffer buffer)
{
	if (msg->msgData.Chat.szMessage == NULL)
		return;

	LPUserBuffer user;
	user = GetIdToUser(msg->msgData.Chat.nUserID);
	if (user != NULL)
		MakeChatString(buffer, user->szName, msg->msgData.Chat.szMessage);

}

// �`���b�g�����̍쐬
void SetMessageToBufferLog(LPChatMsgData msg, LPChatBuffer buffer)
{
	if (msg->msgData.Log.szMessage == NULL)
		return;

	MakeLogString(buffer, msg->msgData.Log.szMessage);
}

// �\���f�[�^�̍쐬
void MakeChatString(LPChatBuffer buffer, char* name, char* message)
{
#pragma warning ( disable : 4996 )
	// ���Ԏ擾
	time_t timer;
	tm *local;

	/* ���ݎ������擾 */
	timer = time(NULL);
	local = localtime(&timer);

	int size = 0;
	char temp_buffer[512];

	size = sprintf_s(temp_buffer, "%s: %s %2d	��%2d��%2d�b�@\r\n", name, message, local->tm_hour, local->tm_min, local->tm_sec);
	temp_buffer[size] = NULL;

	buffer->nChatBuffSize = size + 1;
	buffer->pszChatStr = (char*)malloc(sizeof(char) * (size + 1));

	strcpy_s(buffer->pszChatStr, size + 1, temp_buffer);
}

void MakeLogString(LPChatBuffer buffer, char* data)
{
	int size = 0;
	char temp_buffer[512];

	size = sprintf_s(temp_buffer, "%s\r\n", data);
	temp_buffer[size] = NULL;

	buffer->nChatBuffSize = size + 1;
	buffer->pszChatStr = (char*)malloc(sizeof(char) * (size + 1));

	strcpy_s(buffer->pszChatStr, buffer->nChatBuffSize, temp_buffer);

}

// �`���b�g�����̏���
void DeleteChatData(LPChatBuffer buffer)
{
	free(buffer->pszChatStr);
	free(buffer);
}

// �`���b�g�E�B���h�E�ւ̃��b�Z�[�W�\������
void InsertData(LPChatBuffer chat_buff)
{
	int i = 0;

	SendMessage(GetDlgItem(g_hWnd, IDC_MESSAGELIST), LB_INSERTSTRING, (WPARAM)0, (LPARAM)chat_buff->pszChatStr);

	// �`���b�g�����ւ̏������ݏ���
	for (i = 0; i < g_userNum; i++)
		SendLogMessage(chat_buff->pszChatStr, g_userList[i].nUserID);

}

// �T�[�o��M�����X���b�h
unsigned int __stdcall RecvServerThread(void* args)
{
	ChatMsgData recvbuff;
	LPChatBuffer message_data;
	sockaddr_in fromaddr;

	message_data = NULL;

	while (g_mainLoop)
	{
		if (RecvDataFrom(g_sockNo, recvbuff.szData, sizeof(recvbuff.szData), (sockaddr*)&fromaddr) == SOCKET_ERROR)
			break;

		switch (recvbuff.msgData.Header.nType)
		{
			// �`���b�g���b�Z�[�W��M
		case MSGTYPE_CHAT:
			if (recvbuff.msgData.Chat.szMessage != NULL)
			{
				message_data = (LPChatBuffer)malloc(sizeof(ChatBuffer));
				SetMessageToBufferChat(&recvbuff, message_data);
				InsertData(message_data);		//LPChatBuffer��\����	//���O��\������ׂ�
				g_messages.push_back(message_data);
			}
			break;

			// ���O�C������M
		case MSGTYPE_LOGIN:
			g_userId += 1;	//��ӂ�ID�쐬
			g_userList[g_userNum].nUserID = g_userId;
			g_userList[g_userNum].bEnter = false;
			g_userNum += 1;
			SendPermitMessage(g_userId);
			SendLogData(g_userId);	//�ߋ��̃��O�f�[�^��S���M
			SendUserListMessage(g_userId);
			break;

			// ���O�A�E�g����M
		case MSGTYPE_LOGOUT:
			DeleteUserData(recvbuff.msgData.Logout.nUserID);
			break;

			// ��������M
		case MSGTYPE_ENTER:
			if (recvbuff.msgData.Enter.szName != NULL)
			{
				int i;
				LPUserBuffer user;

				message_data = (LPChatBuffer)malloc(sizeof(ChatBuffer));
				MakeChatString(message_data, recvbuff.msgData.Enter.szName, "���񂪓������܂���");
				InsertData(message_data);		//LPChatBuffer��\����	//���O��\������ׂ�
				g_messages.push_back(message_data);

				user = GetIdToUser(recvbuff.msgData.Enter.nUserID);
				if (user != NULL)
				{	//���O�̓o�^

					// �������O�̐l�����O�C������Ă���������s��
					//for (i = 0; i < g_userNum; i++)
					//{
					//	if (strcmp(g_userList[i].szName, recvbuff.msgData.Enter.szName))
					//	{
					//		return false;
					//	}
					//}

					strcpy_s(user->szName, NAME_LENGTH, recvbuff.msgData.Enter.szName);
					user->bEnter = true;


					for (i = 0; i < g_userNum; i++)
						SendUserListMessage(g_userList[i].nUserID);

				}

				ListBox_ResetContent(GetDlgItem(g_hWnd, IDC_USERLIST));
				for (i = 0; i < g_userNum; i++)
				{
					if (g_userList[i].bEnter)
						SendMessage(GetDlgItem(g_hWnd, IDC_USERLIST), LB_INSERTSTRING, (WPARAM)0, (LPARAM)g_userList[i].szName);

				}
			}

			break;

			// �ގ�����M
		case MSGTYPE_EXIT:
			int i;
			LPUserBuffer user;

			user = GetIdToUser(recvbuff.msgData.Enter.nUserID);
			if (user != NULL)
			{
				message_data = (LPChatBuffer)malloc(sizeof(ChatBuffer));

				MakeChatString(message_data, user->szName, "���񂪑ގ����܂���");
				InsertData(message_data);
				g_messages.push_back(message_data);
				user->bEnter = false;
				for (i = 0; i < g_userNum; i++)
					SendUserListMessage(g_userList[i].nUserID);

			}
			ListBox_ResetContent(GetDlgItem(g_hWnd, IDC_USERLIST));
			for (i = 0; i < g_userNum; i++)
			{
				if (g_userList[i].bEnter)
					SendMessage(GetDlgItem(g_hWnd, IDC_USERLIST), LB_INSERTSTRING, (WPARAM)0, (LPARAM)g_userList[i].szName);

			}
			break;

		default:
			break;
		}
	}
	return NULL;
}

// �N���C�A���g��M����
unsigned int __stdcall RecvClientThread(void* args)
{
	ChatMsgData recvbuff;
	LPChatBuffer message_data;
	sockaddr_in fromaddr;

	message_data = NULL;

	while (g_mainLoop)
	{
		if (RecvDataFrom(g_sockNo, recvbuff.szData, sizeof(recvbuff.szData), (sockaddr*)&fromaddr) == SOCKET_ERROR)
			break;

		switch (recvbuff.msgData.Header.nType)
		{
			// �`���b�g������M
		case MSGTYPE_LOG:
			if (recvbuff.msgData.Log.szMessage != NULL && g_sendBuff.msgData.Chat.nUserID == recvbuff.msgData.Log.nUserID)
			{
				message_data = (LPChatBuffer)malloc(sizeof(ChatBuffer));
				SetMessageToBufferLog(&recvbuff, message_data);
				SendMessage(GetDlgItem(g_hWnd, IDC_MESSAGELIST), LB_INSERTSTRING, (WPARAM)0, (LPARAM)message_data->pszChatStr);
				DeleteChatData(message_data);
			}
			break;

			// �����(�������[�UID)��M
		case MSGTYPE_PERMIT:
			if (g_sendBuff.msgData.Chat.nUserID == 0)
				g_sendBuff.msgData.Chat.nUserID = recvbuff.msgData.Permit.nUserID;

			break;

			// ���[�U�ꗗ��M
		case MSGTYPE_USERRESET:
			if (g_sendBuff.msgData.Chat.nUserID == recvbuff.msgData.UserReset.nUserID)
				ListBox_ResetContent(GetDlgItem(g_hWnd, IDC_USERLIST));

			break;

		case MSGTYPE_USERLOG:
			if (recvbuff.msgData.UserLog.szName != NULL && g_sendBuff.msgData.Chat.nUserID == recvbuff.msgData.UserLog.nUserID)
				SendMessage(GetDlgItem(g_hWnd, IDC_USERLIST), LB_INSERTSTRING, (WPARAM)0, (LPARAM)recvbuff.msgData.UserLog.szName);

			break;
		default:
			break;
		}
	}

	return NULL;
}

//=========================================
// ���M���b�Z�[�W�쐬��
//=========================================

void SendChatMessage(char* chat)
{
	g_sendBuff.msgData.Header.nType = MSGTYPE_CHAT;
	strcpy_s(g_sendBuff.msgData.Chat.szMessage, CHAT_LENGTH, chat);

	SendDataTo(g_sockNo, g_sendBuff.szData, sizeof(g_sendBuff.szData), (sockaddr*)&g_serverAddr);
}

void SendLogInMessage()
{
	g_sendBuff.msgData.Header.nType = MSGTYPE_LOGIN;
	SendDataTo(g_sockNo, g_sendBuff.szData, sizeof(g_sendBuff.szData), (sockaddr*)&g_serverAddr);
}

void SendLogOutMessage()
{
	g_sendBuff.msgData.Header.nType = MSGTYPE_LOGOUT;
	SendDataTo(g_sockNo, g_sendBuff.szData, sizeof(g_sendBuff.szData), (sockaddr*)&g_serverAddr);
	g_sendBuff.msgData.Chat.nUserID = 0;
}

void SendEnterMessage(char* chat)
{
	g_sendBuff.msgData.Header.nType = MSGTYPE_ENTER;
	strcpy_s(g_sendBuff.msgData.Enter.szName, CHAT_LENGTH, chat);
	SendDataTo(g_sockNo, g_sendBuff.szData, sizeof(g_sendBuff.szData), (sockaddr*)&g_serverAddr);
}

void SendExitMessage()
{
	g_sendBuff.msgData.Header.nType = MSGTYPE_EXIT;
	SendDataTo(g_sockNo, g_sendBuff.szData, sizeof(g_sendBuff.szData), (sockaddr*)&g_serverAddr);
}

void SendLogMessage(char* chat, int id)
{
	int i = 0;
	ChatMsgData sendlog;
	sockaddr_in broadaddr;

	SetAddressToSockAddrIn(&broadaddr, INADDR_BROADCAST, g_portNo);

	sendlog.msgData.Log.szMessage[0] = 0;
	strcpy_s(sendlog.msgData.Log.szMessage, NAME_LENGTH + CHAT_LENGTH, chat);

	SetLimitedBroadCastMode(g_sockNo, true);

	sendlog.msgData.Header.nType = MSGTYPE_LOG;
	sendlog.msgData.Log.nUserID = id;
	SendDataTo(g_sockNo, sendlog.szData, sizeof(sendlog.szData), (sockaddr*)&broadaddr);
}

void SendPermitMessage(int id)
{
	ChatMsgData g_sendId;
	sockaddr_in broadaddr;

	SetLimitedBroadCastMode(g_sockNo, true);

	g_sendId.msgData.Header.nType = MSGTYPE_PERMIT;
	g_sendId.msgData.Permit.nUserID = id;
	SetAddressToSockAddrIn(&broadaddr, INADDR_BROADCAST, g_portNo);
	SendDataTo(g_sockNo, g_sendId.szData, sizeof(g_sendId.szData), (sockaddr*)&broadaddr);
}


void SendUserListMessage(int id)
{
	int i;
	ChatMsgData g_sendId;
	sockaddr_in broadaddr;

	SetLimitedBroadCastMode(g_sockNo, true);

	g_sendId.msgData.Header.nType = MSGTYPE_USERRESET;
	g_sendId.msgData.UserReset.nUserID = id;
	SetAddressToSockAddrIn(&broadaddr, INADDR_BROADCAST, g_portNo);
	SendDataTo(g_sockNo, g_sendId.szData, sizeof(g_sendId.szData), (sockaddr*)&broadaddr);

	g_sendId.msgData.Header.nType = MSGTYPE_USERLOG;
	g_sendId.msgData.UserLog.nUserID = id;
	for (i = 0; i < g_userNum; i++)
	{
		if (g_userList[i].bEnter)
		{
			strcpy_s(g_sendId.msgData.UserLog.szName, NAME_LENGTH, g_userList[i].szName);
			SendDataTo(g_sockNo, g_sendId.szData, sizeof(g_sendId.szData), (sockaddr*)&broadaddr);
		}
	}
}

void DeleteUserData(int id)
{
	INT i = 0;
	g_userNum -= 1;
	for (i = 0; i < g_userNum; i++)
	{

		if (g_userList[i].nUserID == id)
		{
			UserBuffer work = g_userList[i + 1];
			g_userList[i + 1] = g_userList[i];
			g_userList[i] = work;
		}
	}
}


void SendLogData(int id)
{
	for (std::list<LPChatBuffer>::iterator itr = g_messages.begin(); itr != g_messages.end(); ++itr)
		SendLogMessage((*itr)->pszChatStr, id);

}
void DeleteLogData()
{
	for (std::list<LPChatBuffer>::iterator itr = g_messages.begin(); itr != g_messages.end(); ++itr)
		DeleteChatData(*itr);	//ChatBuffer�폜

	g_messages.clear();
}

LPUserBuffer GetIdToUser(short user_id)
{
	int i;

	for (i = 0; i < g_userNum; i++)
	{
		if (g_userList[i].nUserID == user_id)
		{
			return &g_userList[i];
		}
	}
	return NULL;
}

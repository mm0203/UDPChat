#pragma once

#define PORTNO (20250)

extern sockaddr_in g_myAddr;
//extern unsigned short g_portNo;

// WinSock������/�I��
bool InitNetwork();
void UninitNetwork();

// �`���b�g�V�X�e��������
bool InitSystem(HWND);

// �T�[�o���[�hON/OFF
bool InitServer(unsigned short);
void UninitServer();

// �`���b�g�����폜
void DeleteLogData(void);

// ���O�C��/���O�A�E�g
bool LoginChat(unsigned short, unsigned long);
void LogoutChat(void);

// ����/�ގ�
bool EnterRoom(char *);
void ExitRoom(void);

// ���b�Z�[�W���M
void SendChatMessage(char *);

void SendLogInMessage(void);
void SendLogOutMessage(void);
void SendEnterMessage(char *);
void SendExitMessage(void);

void SendLogMessage(char *, int);
void SendPermitMessage(int);
void SendUserListMessage(int);


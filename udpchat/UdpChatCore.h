#pragma once

#define PORTNO (20250)

extern sockaddr_in g_myAddr;
//extern unsigned short g_portNo;

// WinSock初期化/終了
bool InitNetwork();
void UninitNetwork();

// チャットシステム初期化
bool InitSystem(HWND);

// サーバモードON/OFF
bool InitServer(unsigned short);
void UninitServer();

// チャット履歴削除
void DeleteLogData(void);

// ログイン/ログアウト
bool LoginChat(unsigned short, unsigned long);
void LogoutChat(void);

// 入室/退室
bool EnterRoom(char *);
void ExitRoom(void);

// メッセージ送信
void SendChatMessage(char *);

void SendLogInMessage(void);
void SendLogOutMessage(void);
void SendEnterMessage(char *);
void SendExitMessage(void);

void SendLogMessage(char *, int);
void SendPermitMessage(int);
void SendUserListMessage(int);


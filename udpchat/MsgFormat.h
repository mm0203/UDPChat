#pragma once

//=================================
// データサイズ
//=================================
#define NAME_LENGTH		 (32)
#define CHAT_LENGTH		(120)

//=================================
// クライアントメッセージ
//=================================
#define MSGTYPE_CHAT		(0)

#define MSGTYPE_LOGIN		(1)
#define MSGTYPE_LOGOUT		(2)
#define MSGTYPE_ENTER		(3)
#define MSGTYPE_EXIT		(4)

//=================================
// サーバメッセージ
//=================================
#define MSGTYPE_LOG			(6)
#define MSGTYPE_PERMIT		(7)
#define MSGTYPE_USERRESET	(8)
#define MSGTYPE_USERLOG		(9)

//=================================
// メッセージ構造体
//=================================

//ヘッダー
typedef struct
{
	int		nType;
}MSGHEADER, *LPMSGHEADER;

//入室
typedef struct
{
	int		nUserID;
	char	szName[CHAT_LENGTH];
}MSGBODY_ENTER, *LPMSGBODY_ENTER;

//IDのみのメッセージ
typedef struct
{
	int		nUserID;
}MSGBODY_LOGOUT, *LPMSGBODY_LOGOUT,
MSGBODY_EXIT, *LPMSGBODY_EXIT,
MSGBODY_PERMIT, *LPMSGBODY_PERMIT,
MSGBODY_USERRESET, *LPMSGBODY_USERRESET;

//チャット
typedef struct
{
	int		nUserID;
	char	szMessage[CHAT_LENGTH];
}MSGBODY_CHAT, *LPMSGBODY_CHAT;

//ログ
typedef struct
{
	int		nUserID;
	char	szMessage[NAME_LENGTH + CHAT_LENGTH];
}MSGBODY_LOG, *LPMSGBODY_LOG;

typedef struct
{
	int		nUserID;
	char	szName[NAME_LENGTH];
}MSGBODY_USERLOG, *LPMSGBODY_USERLOG;

//ログアウト
typedef struct
{
	MSGHEADER Header;
	union
	{
		MSGBODY_LOGOUT		Logout;
		MSGBODY_ENTER		Enter;
		MSGBODY_EXIT		Exit;
		MSGBODY_CHAT		Chat;
		MSGBODY_LOG			Log;
		MSGBODY_PERMIT		Permit;
		MSGBODY_USERLOG		UserLog;
		MSGBODY_USERRESET	UserReset;
	};
}MSGDATA, *LPMSGDATA;

//送信データ
typedef struct
{
	union
	{
		MSGDATA msgData;
		char	szData[sizeof(MSGDATA)];
	};
}ChatMsgData, *LPChatMsgData;

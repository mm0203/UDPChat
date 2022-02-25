#pragma once

//=================================
// �f�[�^�T�C�Y
//=================================
#define NAME_LENGTH		 (32)
#define CHAT_LENGTH		(120)

//=================================
// �N���C�A���g���b�Z�[�W
//=================================
#define MSGTYPE_CHAT		(0)

#define MSGTYPE_LOGIN		(1)
#define MSGTYPE_LOGOUT		(2)
#define MSGTYPE_ENTER		(3)
#define MSGTYPE_EXIT		(4)

//=================================
// �T�[�o���b�Z�[�W
//=================================
#define MSGTYPE_LOG			(6)
#define MSGTYPE_PERMIT		(7)
#define MSGTYPE_USERRESET	(8)
#define MSGTYPE_USERLOG		(9)

//=================================
// ���b�Z�[�W�\����
//=================================

//�w�b�_�[
typedef struct
{
	int		nType;
}MSGHEADER, *LPMSGHEADER;

//����
typedef struct
{
	int		nUserID;
	char	szName[CHAT_LENGTH];
}MSGBODY_ENTER, *LPMSGBODY_ENTER;

//ID�݂̂̃��b�Z�[�W
typedef struct
{
	int		nUserID;
}MSGBODY_LOGOUT, *LPMSGBODY_LOGOUT,
MSGBODY_EXIT, *LPMSGBODY_EXIT,
MSGBODY_PERMIT, *LPMSGBODY_PERMIT,
MSGBODY_USERRESET, *LPMSGBODY_USERRESET;

//�`���b�g
typedef struct
{
	int		nUserID;
	char	szMessage[CHAT_LENGTH];
}MSGBODY_CHAT, *LPMSGBODY_CHAT;

//���O
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

//���O�A�E�g
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

//���M�f�[�^
typedef struct
{
	union
	{
		MSGDATA msgData;
		char	szData[sizeof(MSGDATA)];
	};
}ChatMsgData, *LPChatMsgData;

// ========================================================
// GN31 �ۑ�No.08 UDP�`���b�g
// --------------------------------------------------------
// UDP�ɂ��`���b�g�V�X�e��(�T�[�o/�N���C�A���g���p)
// ========================================================

//------ �x���}�~
#define _CRT_SECURE_NO_WARNINGS

// ================================
// �C���N���[�h��
// ================================
#include	<Windows.h>
#include	<windowsx.h>
#include	<stdio.h>
#include	<CommCtrl.h>
#include	"resource.h"
#include	"UdpChatCore.h"
#include	<time.h>
#include	<stdio.h>

// ================================
// �֐��v���g�^�C�v�錾��
// ================================
BOOL CALLBACK DialogProc(HWND,UINT,WPARAM,LPARAM);		// �_�C�A���O�v���V�[�W��
void StringCheck(char* str1, char* str2);	//�@�����̔�r

// ================================
// �}�N����`
// ================================
#define ACTIVE_TIME	  (0.01f)
#define RIMIT_TIME	   (1800)

// ��^�����i�[
typedef struct
{
	TCHAR fixed[255];
} LIST;

// ��^���ꗗ
LIST FixedChar[] =
{
	{TEXT("��낵�����肢���܂�")},
	{TEXT("���肪�Ƃ��������܂�")},
	{TEXT("���ǂ����Ă�������")},
	{TEXT("�����ς����b���܂��傤")},
	{TEXT("�����l�ł�")},
	{TEXT("���₷�݂Ȃ���")},
};

// ================================
// �G���g���|�C���g
// ================================
int APIENTRY WinMain(HINSTANCE 	hInstance, 		// �A�v���P�[�V�����C���X�^���X�l
		     HINSTANCE 	hPrevInstance,			// �Ӗ��Ȃ�
		     LPSTR 	lpszArgs, 					// �N�����̈���������
		     int 	nWinMode)					// �E�C���h�E�\�����[�h
{
	// WinSock������
	if (InitNetwork())
	{
		// �_�C�A���O�{�b�N�X�̍쐬
		DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINFORM), 0, DialogProc);

		//  WinSock�I��
		UninitNetwork();
	}

	return 0;
}

// ================================
// �_�C�A���O�v���V�[�W��
// ================================
BOOL CALLBACK DialogProc(	HWND hwndDlg,		// �_�C�A���O�{�b�N�X�̃n���h��
							UINT uMsg,			// ���b�Z�[�W���ʎq
							WPARAM wParam,		// �t�я��1
							LPARAM lParam)		// �t�я��2
{
	static bool bServerFlg = false;	// �T�[�o�[�t���O
	static bool bLoginFlg = false;	// ���O�C���t���O
	static bool bEnterFlg = false;	// �G���^�[�t���O
	static bool bPassFlg = false;	// �p�X���[�h�t���O


	static char szBuff[512];
	static char szCurrentBuff[512];	// ���͂��ꂽ�������ۑ�
	const char szPass[16] = "123";	// �p�X���[�h

	// ���݂̓��t�������擾
	char date[64];
	time_t t = time(NULL);
	strftime(date, sizeof(date), "%Y/%m/%d %a %H:%M:%S", localtime(&t));

	// �E��Ɏ��Ԃ�\��(�}�E�X�������ĂȂ��ƌv������Ȃ��H)
	Edit_SetText(GetDlgItem(hwndDlg, IDC_TIME), date);

	switch (uMsg)
	{
	case WM_INITDIALOG:
		if (!InitSystem(hwndDlg))
		{
			EndDialog(hwndDlg, IDEXIT);
		}
		else
		{
			char szPortNo[6];
			LPARAM IpAddress;

			sprintf(szPortNo, "%d", PORTNO);
			IpAddress = MAKEIPADDRESS(g_myAddr.sin_addr.S_un.S_un_b.s_b1, 
									  g_myAddr.sin_addr.S_un.S_un_b.s_b2, 
									  g_myAddr.sin_addr.S_un.S_un_b.s_b3, 
									  g_myAddr.sin_addr.S_un.S_un_b.s_b4);

			EnableWindow(GetDlgItem(hwndDlg, IDC_SRVMODEPORT), true);	// �|�[�g�ԍ���ł����ޏꏊ(Server Mode�g��)
			EnableWindow(GetDlgItem(hwndDlg, IDC_SRVSTART), true);		// �T�[�o�N���{�^��
			EnableWindow(GetDlgItem(hwndDlg, IDC_SERVERPORT), true);	// �|�[�g�ԍ���ł����ޏꏊ
			EnableWindow(GetDlgItem(hwndDlg, IDC_LOGIN), true);			// ���O�C���{�^��
			EnableWindow(GetDlgItem(hwndDlg, IDC_NAME), false);			// ���[�U����ł����ޏꏊ
			EnableWindow(GetDlgItem(hwndDlg, IDSTART), false);			// �����{�^��
			EnableWindow(GetDlgItem(hwndDlg, IDC_USERLIST), false);		// ���[�U���X�g�\���ӏ�
			EnableWindow(GetDlgItem(hwndDlg, IDC_MESSAGELIST), false);	// ���b�Z�[�W�\���ӏ�
			EnableWindow(GetDlgItem(hwndDlg, IDC_CHATINPUT), false);	// ���b�Z�[�W��ł����ޏꏊ
			EnableWindow(GetDlgItem(hwndDlg, IDC_SEND), false);			// ���b�Z�[�W���M�{�^��
			EnableWindow(GetDlgItem(hwndDlg, IDEXIT), true);			// �I���{�^��
			EnableWindow(GetDlgItem(hwndDlg, IDC_PASSWARD), false);		// �p�X���[�h�̕�������
			EnableWindow(GetDlgItem(hwndDlg, IDC_SENDPASS), false);		// �p�X���[�h�̕������M

			Edit_SetText(GetDlgItem(hwndDlg, IDC_SRVMODEPORT), szPortNo);
			Edit_SetText(GetDlgItem(hwndDlg, IDC_SERVERPORT), szPortNo);

			EnableWindow(GetDlgItem(hwndDlg, IDC_IPADDRESS), true);
			SendMessage(GetDlgItem(hwndDlg, IDC_IPADDRESS), IPM_SETADDRESS, 0, IpAddress);
			EnableWindow(GetDlgItem(hwndDlg, IDC_FIXED), false);
			HWND hwndList = GetDlgItem(hwndDlg, IDC_FIXED);

			// ��^�����X�g
			for (int i = 0; i < ARRAYSIZE(FixedChar); i++)
			{
				int pos = (int)SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)FixedChar[i].fixed);
				SendMessage(hwndList, LB_SETITEMDATA, pos, (LPARAM)i);
			}
			SetFocus(hwndList);
			return true;
		}

		bServerFlg = false;
		return true;

	// �{�^���������ꂽ��
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_SRVSTART:	// �T�[�o�N��/��~�{�^��
			if (bServerFlg)		// �������T�[�o
			{
				UninitServer();

				Edit_SetText(GetDlgItem(hwndDlg, IDC_SRVSTART), "�T�[�o�N��");
				EnableWindow(GetDlgItem(hwndDlg, IDC_SRVMODEPORT), true);
				EnableWindow(GetDlgItem(hwndDlg, IDC_SERVERPORT), true);
				EnableWindow(GetDlgItem(hwndDlg, IDC_LOGIN), true);
				EnableWindow(GetDlgItem(hwndDlg, IDC_IPADDRESS), true);

				bServerFlg = false;
			}
			else	// �������T�[�o���Ⴏ���
			{
				unsigned short nPortNo;

				GetDlgItemText(hwndDlg, IDC_SRVMODEPORT, szBuff, sizeof(szBuff));

				nPortNo = atoi(szBuff);

				if (InitServer(nPortNo))
				{
					Edit_SetText(GetDlgItem(hwndDlg, IDC_SRVSTART), "�T�[�o��~");
					bServerFlg = true;

					EnableWindow(GetDlgItem(hwndDlg, IDC_SRVMODEPORT), false);
					EnableWindow(GetDlgItem(hwndDlg, IDC_SERVERPORT), false);
					EnableWindow(GetDlgItem(hwndDlg, IDC_LOGIN), false);
					EnableWindow(GetDlgItem(hwndDlg, IDC_IPADDRESS), false);
				}
			}
			return true;

		case IDC_LOGIN:		// ���O�C���{�^������������
			if (bLoginFlg)	// ���O�C����
			{

				SendLogOutMessage();
				LogoutChat();

				Edit_SetText(GetDlgItem(hwndDlg, IDC_LOGIN), "���O�C��");
				EnableWindow(GetDlgItem(hwndDlg, IDC_SRVMODEPORT), true);
				EnableWindow(GetDlgItem(hwndDlg, IDC_SRVSTART), true);
				EnableWindow(GetDlgItem(hwndDlg, IDC_SERVERPORT), true);
				EnableWindow(GetDlgItem(hwndDlg, IDC_NAME), false);
				EnableWindow(GetDlgItem(hwndDlg, IDSTART), false);
				EnableWindow(GetDlgItem(hwndDlg, IDC_IPADDRESS), true);
				EnableWindow(GetDlgItem(hwndDlg, IDC_USERLIST), false);
				EnableWindow(GetDlgItem(hwndDlg, IDC_MESSAGELIST), false);
				EnableWindow(GetDlgItem(hwndDlg, IDC_CHATINPUT), false);
				EnableWindow(GetDlgItem(hwndDlg, IDC_PASSWARD), false);
				EnableWindow(GetDlgItem(hwndDlg, IDC_SENDPASS), false);
				Edit_SetText(GetDlgItem(hwndDlg, IDC_PASSWARD), "");
				bPassFlg = false;
				bLoginFlg = false;
			}
			else	// ���O�C��������Ȃ����
			{
				unsigned short nPortNo;
				unsigned long addr;

				GetDlgItemText(hwndDlg, IDC_SERVERPORT, szBuff, sizeof(szBuff));
				nPortNo = atoi(szBuff);

				GetDlgItemText(hwndDlg, IDC_IPADDRESS, szBuff, sizeof(szBuff));
				addr = inet_addr(szBuff);

				if (LoginChat(nPortNo, addr))
				{
					Edit_SetText(GetDlgItem(hwndDlg, IDC_LOGIN), "���O�A�E�g");
					EnableWindow(GetDlgItem(hwndDlg, IDC_SRVMODEPORT), false);
					EnableWindow(GetDlgItem(hwndDlg, IDC_SRVSTART), false);
					EnableWindow(GetDlgItem(hwndDlg, IDC_SERVERPORT), false);
					EnableWindow(GetDlgItem(hwndDlg, IDC_NAME), true);
					EnableWindow(GetDlgItem(hwndDlg, IDSTART), true);
					EnableWindow(GetDlgItem(hwndDlg, IDC_IPADDRESS), false);
					EnableWindow(GetDlgItem(hwndDlg, IDC_USERLIST), true);
					EnableWindow(GetDlgItem(hwndDlg, IDC_MESSAGELIST), true);
					EnableWindow(GetDlgItem(hwndDlg, IDC_PASSWARD), true);
					EnableWindow(GetDlgItem(hwndDlg, IDC_SENDPASS), true);
					ListBox_ResetContent(GetDlgItem(hwndDlg, IDC_USERLIST));
					ListBox_ResetContent(GetDlgItem(hwndDlg, IDC_MESSAGELIST));

					SendLogInMessage();

					bLoginFlg = true;
				}
			}
			return true;

		case IDC_SENDPASS:	//�p�X���[�h���F�������ꂽ��

			int naStrLen;
			naStrLen = GetDlgItemText(hwndDlg, IDC_PASSWARD, szBuff, sizeof(szBuff));

			// ���͂��ꂽ�p�X���[�h�Ɛݒ�ς̃p�X���[�h���r
			strcpy(szCurrentBuff, szBuff);

			// ���͂��ꂽ�p�X���[�h�Ɛݒ肳�ꂽ�p�X���[�h�̈�v���m�F
			if (strcmp(szPass, szCurrentBuff))
			{
				// �p�X���[�h���s�̃_�C�A���O��\��
				MessageBox(NULL, "�p�X���[�h���F���s�B", "�p�X���[�h�m�F", MB_OK);			
				Edit_SetText(GetDlgItem(hwndDlg, IDC_PASSWARD), "");
			}
			else
			{
				// �p�X���[�h���F����
				MessageBox(NULL, "�p�X���[�h���F�����B\n�����{�^���������ē������Ă��������B", "�p�X���[�h�m�F", MB_OK);
				EnableWindow(GetDlgItem(hwndDlg, IDC_PASSWARD), false);
				EnableWindow(GetDlgItem(hwndDlg, IDC_SENDPASS), false);
				Edit_SetText(GetDlgItem(hwndDlg, IDC_PASSWARD), "");
				bPassFlg = true;
			}
			return true;

		case IDSTART:		// �����{�^��			
			if (bEnterFlg)	// �p�X���[�h���F���������Ă��Ȃ��Ɠ����s��
			{
				ExitRoom();

				Edit_SetText(GetDlgItem(hwndDlg, IDSTART), "����");
				EnableWindow(GetDlgItem(hwndDlg, IDC_LOGIN), true);
				EnableWindow(GetDlgItem(hwndDlg, IDC_NAME), true);
				EnableWindow(GetDlgItem(hwndDlg, IDC_CHATINPUT), false);
				EnableWindow(GetDlgItem(hwndDlg, IDC_SEND), false);
				EnableWindow(GetDlgItem(hwndDlg, IDC_PASSWARD), true);
				EnableWindow(GetDlgItem(hwndDlg, IDC_FIXED), false);
				EnableWindow(GetDlgItem(hwndDlg, IDC_SENDPASS), true);		
				Edit_SetText(GetDlgItem(hwndDlg, IDC_PASSWARD), "");

				bPassFlg = false;
				bEnterFlg = false;
			}
			else if(bPassFlg)	// �������Ă��Ȃ�
			{
				int nStrLen;
				nStrLen = GetDlgItemText(hwndDlg,IDC_NAME,szBuff,sizeof(szBuff));

				if (nStrLen > 0)
				{
					if (EnterRoom(szBuff))
					{
						Edit_SetText(GetDlgItem(hwndDlg, IDSTART), "�ގ�");
						EnableWindow(GetDlgItem(hwndDlg, IDC_LOGIN), false);
						EnableWindow(GetDlgItem(hwndDlg, IDC_NAME), false);
						EnableWindow(GetDlgItem(hwndDlg, IDC_CHATINPUT), true);
						EnableWindow(GetDlgItem(hwndDlg, IDC_SEND), true);
						EnableWindow(GetDlgItem(hwndDlg, IDC_PASSWARD), false);
						EnableWindow(GetDlgItem(hwndDlg, IDC_FIXED), true);
						EnableWindow(GetDlgItem(hwndDlg, IDC_SENDPASS), false);
						Edit_SetText(GetDlgItem(hwndDlg, IDC_PASSWARD), "");

						bEnterFlg = true;
					}
				}
			}
			// �p�X���[�h���F�O�ɓ����{�^������������
			else if (!bPassFlg)
			{
				MessageBox(NULL, "�p�X���[�h���F���������Ă��������B", "�p�X���[�h�m�F", MB_OK);
			}
			return true;

		// �`���b�g���M
		case IDC_SEND:
			int nStrLen;
			nStrLen = GetDlgItemText(hwndDlg, IDC_CHATINPUT, szBuff, sizeof(szBuff));

			//�`���b�g���͂̑��M
			if (nStrLen > 0)
			{
				// �����̔�r
				StringCheck(szCurrentBuff, szBuff);			

				//�e�L�X�g�{�b�N�X���N���A
				Edit_SetText(GetDlgItem(hwndDlg, IDC_CHATINPUT), "");
			}

			return true;
		case IDC_FIXED:
		{
			HWND hwndList = GetDlgItem(hwndDlg, IDC_FIXED);
			int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0);
			int i = (int)SendMessage(hwndList, LB_GETITEMDATA, lbItem, 0);
			
			// �����̔�r
			StringCheck(szCurrentBuff, FixedChar[i].fixed);

			return true;
		}
		//�I���{�^���������ꂽ
		case IDEXIT:

			//�`���b�g�����̏���
			DeleteLogData();

			//���O�C�����Ȃ�
			if (bLoginFlg)
			{
				if (bEnterFlg)
					ExitRoom();

				LogoutChat();
			}

			//�T�[�o���[�h�Ȃ�
			if (bServerFlg)
			{
				UninitServer();
			}

			EndDialog(hwndDlg, IDEXIT);
			return true;

		default:
			return true;
		}

	case WM_CLOSE:
		EndDialog(hwndDlg, IDEXIT);
		return true;
	}

	return false;
}
//==========================================
// 
// ������r�֐�
//
//==========================================
void StringCheck(char* str1, char* str2)
{
	// �O����͂��ꂽ�����ƍ�����͂����������r
	if (strcmp(str1, str2))
	{
		// ��������Ȃ������瑗�M�\
		SendChatMessage(str2);
	}
	// ����������������G���[��
	else
	{
		SendChatMessage("***���������͓��͂ł��܂���***");
	}

	// ���͂��ꂽ������ϐ��ɕۑ�
	strcpy(str1, str2);
}

// ========================================================
// GN31 課題No.08 UDPチャット
// --------------------------------------------------------
// UDPによるチャットシステム(サーバ/クライアント兼用)
// ========================================================

//------ 警告抑止
#define _CRT_SECURE_NO_WARNINGS

// ================================
// インクルード部
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
// 関数プロトタイプ宣言部
// ================================
BOOL CALLBACK DialogProc(HWND,UINT,WPARAM,LPARAM);		// ダイアログプロシージャ
void StringCheck(char* str1, char* str2);	//　文字の比較

// ================================
// マクロ定義
// ================================
#define ACTIVE_TIME	  (0.01f)
#define RIMIT_TIME	   (1800)

// 定型文を格納
typedef struct
{
	TCHAR fixed[255];
} LIST;

// 定型文一覧
LIST FixedChar[] =
{
	{TEXT("よろしくお願いします")},
	{TEXT("ありがとうございます")},
	{TEXT("仲良くしてください")},
	{TEXT("いっぱいお話しましょう")},
	{TEXT("お疲れ様です")},
	{TEXT("おやすみなさい")},
};

// ================================
// エントリポイント
// ================================
int APIENTRY WinMain(HINSTANCE 	hInstance, 		// アプリケーションインスタンス値
		     HINSTANCE 	hPrevInstance,			// 意味なし
		     LPSTR 	lpszArgs, 					// 起動時の引数文字列
		     int 	nWinMode)					// ウインドウ表示モード
{
	// WinSock初期化
	if (InitNetwork())
	{
		// ダイアログボックスの作成
		DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINFORM), 0, DialogProc);

		//  WinSock終了
		UninitNetwork();
	}

	return 0;
}

// ================================
// ダイアログプロシージャ
// ================================
BOOL CALLBACK DialogProc(	HWND hwndDlg,		// ダイアログボックスのハンドル
							UINT uMsg,			// メッセージ識別子
							WPARAM wParam,		// 付帯情報1
							LPARAM lParam)		// 付帯情報2
{
	static bool bServerFlg = false;	// サーバーフラグ
	static bool bLoginFlg = false;	// ログインフラグ
	static bool bEnterFlg = false;	// エンターフラグ
	static bool bPassFlg = false;	// パスワードフラグ


	static char szBuff[512];
	static char szCurrentBuff[512];	// 入力された文字列を保存
	const char szPass[16] = "123";	// パスワード

	// 現在の日付時刻を取得
	char date[64];
	time_t t = time(NULL);
	strftime(date, sizeof(date), "%Y/%m/%d %a %H:%M:%S", localtime(&t));

	// 右上に時間を表示(マウス動かしてないと計測されない？)
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

			EnableWindow(GetDlgItem(hwndDlg, IDC_SRVMODEPORT), true);	// ポート番号を打ち込む場所(Server Mode枠内)
			EnableWindow(GetDlgItem(hwndDlg, IDC_SRVSTART), true);		// サーバ起動ボタン
			EnableWindow(GetDlgItem(hwndDlg, IDC_SERVERPORT), true);	// ポート番号を打ち込む場所
			EnableWindow(GetDlgItem(hwndDlg, IDC_LOGIN), true);			// ログインボタン
			EnableWindow(GetDlgItem(hwndDlg, IDC_NAME), false);			// ユーザ名を打ち込む場所
			EnableWindow(GetDlgItem(hwndDlg, IDSTART), false);			// 入室ボタン
			EnableWindow(GetDlgItem(hwndDlg, IDC_USERLIST), false);		// ユーザリスト表示箇所
			EnableWindow(GetDlgItem(hwndDlg, IDC_MESSAGELIST), false);	// メッセージ表示箇所
			EnableWindow(GetDlgItem(hwndDlg, IDC_CHATINPUT), false);	// メッセージを打ち込む場所
			EnableWindow(GetDlgItem(hwndDlg, IDC_SEND), false);			// メッセージ送信ボタン
			EnableWindow(GetDlgItem(hwndDlg, IDEXIT), true);			// 終了ボタン
			EnableWindow(GetDlgItem(hwndDlg, IDC_PASSWARD), false);		// パスワードの文字入力
			EnableWindow(GetDlgItem(hwndDlg, IDC_SENDPASS), false);		// パスワードの文字送信

			Edit_SetText(GetDlgItem(hwndDlg, IDC_SRVMODEPORT), szPortNo);
			Edit_SetText(GetDlgItem(hwndDlg, IDC_SERVERPORT), szPortNo);

			EnableWindow(GetDlgItem(hwndDlg, IDC_IPADDRESS), true);
			SendMessage(GetDlgItem(hwndDlg, IDC_IPADDRESS), IPM_SETADDRESS, 0, IpAddress);
			EnableWindow(GetDlgItem(hwndDlg, IDC_FIXED), false);
			HWND hwndList = GetDlgItem(hwndDlg, IDC_FIXED);

			// 定型文リスト
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

	// ボタンが押されたら
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_SRVSTART:	// サーバ起動/停止ボタン
			if (bServerFlg)		// 自分がサーバ
			{
				UninitServer();

				Edit_SetText(GetDlgItem(hwndDlg, IDC_SRVSTART), "サーバ起動");
				EnableWindow(GetDlgItem(hwndDlg, IDC_SRVMODEPORT), true);
				EnableWindow(GetDlgItem(hwndDlg, IDC_SERVERPORT), true);
				EnableWindow(GetDlgItem(hwndDlg, IDC_LOGIN), true);
				EnableWindow(GetDlgItem(hwndDlg, IDC_IPADDRESS), true);

				bServerFlg = false;
			}
			else	// 自分がサーバじゃければ
			{
				unsigned short nPortNo;

				GetDlgItemText(hwndDlg, IDC_SRVMODEPORT, szBuff, sizeof(szBuff));

				nPortNo = atoi(szBuff);

				if (InitServer(nPortNo))
				{
					Edit_SetText(GetDlgItem(hwndDlg, IDC_SRVSTART), "サーバ停止");
					bServerFlg = true;

					EnableWindow(GetDlgItem(hwndDlg, IDC_SRVMODEPORT), false);
					EnableWindow(GetDlgItem(hwndDlg, IDC_SERVERPORT), false);
					EnableWindow(GetDlgItem(hwndDlg, IDC_LOGIN), false);
					EnableWindow(GetDlgItem(hwndDlg, IDC_IPADDRESS), false);
				}
			}
			return true;

		case IDC_LOGIN:		// ログインボタンを押したら
			if (bLoginFlg)	// ログイン中
			{

				SendLogOutMessage();
				LogoutChat();

				Edit_SetText(GetDlgItem(hwndDlg, IDC_LOGIN), "ログイン");
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
			else	// ログイン中じゃなければ
			{
				unsigned short nPortNo;
				unsigned long addr;

				GetDlgItemText(hwndDlg, IDC_SERVERPORT, szBuff, sizeof(szBuff));
				nPortNo = atoi(szBuff);

				GetDlgItemText(hwndDlg, IDC_IPADDRESS, szBuff, sizeof(szBuff));
				addr = inet_addr(szBuff);

				if (LoginChat(nPortNo, addr))
				{
					Edit_SetText(GetDlgItem(hwndDlg, IDC_LOGIN), "ログアウト");
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

		case IDC_SENDPASS:	//パスワード承認が押されたら

			int naStrLen;
			naStrLen = GetDlgItemText(hwndDlg, IDC_PASSWARD, szBuff, sizeof(szBuff));

			// 入力されたパスワードと設定済のパスワードを比較
			strcpy(szCurrentBuff, szBuff);

			// 入力されたパスワードと設定されたパスワードの一致を確認
			if (strcmp(szPass, szCurrentBuff))
			{
				// パスワード失敗のダイアログを表示
				MessageBox(NULL, "パスワード承認失敗。", "パスワード確認", MB_OK);			
				Edit_SetText(GetDlgItem(hwndDlg, IDC_PASSWARD), "");
			}
			else
			{
				// パスワード承認成功
				MessageBox(NULL, "パスワード承認完了。\n入室ボタンを押して入室してください。", "パスワード確認", MB_OK);
				EnableWindow(GetDlgItem(hwndDlg, IDC_PASSWARD), false);
				EnableWindow(GetDlgItem(hwndDlg, IDC_SENDPASS), false);
				Edit_SetText(GetDlgItem(hwndDlg, IDC_PASSWARD), "");
				bPassFlg = true;
			}
			return true;

		case IDSTART:		// 入室ボタン			
			if (bEnterFlg)	// パスワード承認が完了していないと入室不可
			{
				ExitRoom();

				Edit_SetText(GetDlgItem(hwndDlg, IDSTART), "入室");
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
			else if(bPassFlg)	// 入室していない
			{
				int nStrLen;
				nStrLen = GetDlgItemText(hwndDlg,IDC_NAME,szBuff,sizeof(szBuff));

				if (nStrLen > 0)
				{
					if (EnterRoom(szBuff))
					{
						Edit_SetText(GetDlgItem(hwndDlg, IDSTART), "退室");
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
			// パスワード承認前に入室ボタンを押したら
			else if (!bPassFlg)
			{
				MessageBox(NULL, "パスワード承認を完了してください。", "パスワード確認", MB_OK);
			}
			return true;

		// チャット送信
		case IDC_SEND:
			int nStrLen;
			nStrLen = GetDlgItemText(hwndDlg, IDC_CHATINPUT, szBuff, sizeof(szBuff));

			//チャット文章の送信
			if (nStrLen > 0)
			{
				// 文字の比較
				StringCheck(szCurrentBuff, szBuff);			

				//テキストボックスをクリア
				Edit_SetText(GetDlgItem(hwndDlg, IDC_CHATINPUT), "");
			}

			return true;
		case IDC_FIXED:
		{
			HWND hwndList = GetDlgItem(hwndDlg, IDC_FIXED);
			int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0);
			int i = (int)SendMessage(hwndList, LB_GETITEMDATA, lbItem, 0);
			
			// 文字の比較
			StringCheck(szCurrentBuff, FixedChar[i].fixed);

			return true;
		}
		//終了ボタンが押された
		case IDEXIT:

			//チャット履歴の消去
			DeleteLogData();

			//ログイン中なら
			if (bLoginFlg)
			{
				if (bEnterFlg)
					ExitRoom();

				LogoutChat();
			}

			//サーバモードなら
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
// 文字比較関数
//
//==========================================
void StringCheck(char* str1, char* str2)
{
	// 前回入力された文字と今回入力した文字を比較
	if (strcmp(str1, str2))
	{
		// 同じじゃなかったら送信可能
		SendChatMessage(str2);
	}
	// 同じ文字だったらエラー報告
	else
	{
		SendChatMessage("***同じ文字は入力できません***");
	}

	// 入力された文字を変数に保存
	strcpy(str1, str2);
}

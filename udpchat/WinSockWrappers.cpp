#include "WinSockWrappers.h"
#include "WsaErr.h"

// ======================================================================
// 	WinSock初期化
// 		引数
// 			なし
// 		戻り値
// 			bool true:成功 false:失敗
// ======================================================================
bool InitWinSock()
{
	WSADATA wsaData;
	int nErrCode;

	nErrCode = WSAStartup(MAKEWORD(2, 2), &wsaData);
	//エラーチェック
	if (nErrCode != 0)
	{
		MessageBox(NULL, "WinSock初期化されませんでした。", "エラー報告", MB_OK);
		return false;
	}

	return true;
}

// ======================================================================
// 	WinSock終了
// 		引数
// 			なし
// 		戻り値
// 			なし
// ======================================================================
void UninitWinSock()
{
	WSACleanup();
}

// ======================================================================
// 	ソケット初期化
// 		引数
// 			なし
// 		戻り値
// 			SOCKET
// ======================================================================
SOCKET InitSocket()
{
	SOCKET sockNo;

	// ソケットの生成(IPv4, データグラム通信, UDP)
	sockNo = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (sockNo == SOCKET_ERROR)
		MessageBox(NULL, "ソケット初期化がされませんでした。", "エラー報告", MB_OK);

	return sockNo;
}

// ======================================================================
// 	ソケット終了
// 		引数
// 			SOCKET *
// 		戻り値
// 			なし
// ======================================================================
void UninitSocket(SOCKET* pSockNo)
{
	if (*pSockNo != INVALID_SOCKET)
	{
		closesocket(*pSockNo);
	}
}

// ======================================================================
// 	IPアドレス設定
// 		引数
// 			sockaddr_in *,unsigned long,unsigned short
// 		戻り値
// 			なし
// ======================================================================
void SetAddressToSockAddrIn(sockaddr_in* pSockAddrIn, unsigned long Addr, unsigned short portNo)
{
	pSockAddrIn->sin_addr.S_un.S_addr = Addr;
	pSockAddrIn->sin_family = AF_INET;
	pSockAddrIn->sin_port = htons(portNo);
}

// ======================================================================
// 	バインド
// 		引数
// 			SOCKET,sockaddr_in*
// 		戻り値
// 			bool true:成功 false:失敗
// ======================================================================
bool BindToSocket(SOCKET sockNo, sockaddr_in* pMyAddr)
{
	int nState;
	nState = bind(sockNo, (sockaddr*)pMyAddr, sizeof(sockaddr));

	if (nState == SOCKET_ERROR)
	{
		MessageBox(NULL, "ソケットバインドにエラーが起きました。", "エラー報告", MB_OK);
		return false;
	}

	return true;
}

// ======================================================================
// 	リミテッドブロードキャストモード設定
// 		引数
// 			SOCKET,bool
// 		戻り値
// 			bool true:成功 false:失敗
// ======================================================================
bool SetLimitedBroadCastMode(SOCKET sockNo, bool bFlg)
{
	int nState;
	nState = setsockopt(sockNo, SOL_SOCKET, SO_BROADCAST, (char*)&bFlg, sizeof(bFlg));
	if (nState == SOCKET_ERROR)
	{
		MessageBox(NULL, "リミテッドブロードキャストモードにエラーが出ました。", "エラー報告", MB_OK);
		return false;
	}

	return true;
}

// ======================================================================
// 	sendToラッパー
// 		引数
// 			SOCKET,const char *,int,sockaddr*
// 		戻り値
// 			bool true:成功 false:失敗
// ======================================================================
bool SendDataTo(SOCKET sockNo, const char* pszData, int nDataSize, sockaddr* toAddr)
{
	int nState;
	nState = sendto(sockNo, pszData, nDataSize, 0, toAddr, sizeof(sockaddr));
	if (nState == SOCKET_ERROR)
	{

		MessageBox(NULL, "SendToラッパーでエラーが出ました。", "エラー報告", MB_OK);
		return false;
	}

	return true;
}

// ======================================================================
// 	recvFromラッパー
// 		引数
// 			SOCKET,char *,int,sockaddr*
// 		戻り値
// 			int SOCKET_ERRORまたは受信バイト数
// ======================================================================
int RecvDataFrom(SOCKET sockNo, char* pszRecvBuff, int nBuffSize, sockaddr* fromAddr)
{
	int nLen;		// 送信元アドレス長
	int nStatus;
	int nErrCode;

	nLen = sizeof(sockaddr);

	nStatus = recvfrom(sockNo, pszRecvBuff, nBuffSize, 0, fromAddr, &nLen);

	if (nStatus == SOCKET_ERROR)
	{
		nErrCode = WSAGetLastError();

		if (nErrCode != WSAEINTR)
		{
			DispErr(nErrCode);
			return SOCKET_ERROR;
		}
	}

	return nStatus;
}


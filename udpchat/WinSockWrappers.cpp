#include "WinSockWrappers.h"
#include "WsaErr.h"

// ======================================================================
// 	WinSock������
// 		����
// 			�Ȃ�
// 		�߂�l
// 			bool true:���� false:���s
// ======================================================================
bool InitWinSock()
{
	WSADATA wsaData;
	int nErrCode;

	nErrCode = WSAStartup(MAKEWORD(2, 2), &wsaData);
	//�G���[�`�F�b�N
	if (nErrCode != 0)
	{
		MessageBox(NULL, "WinSock����������܂���ł����B", "�G���[��", MB_OK);
		return false;
	}

	return true;
}

// ======================================================================
// 	WinSock�I��
// 		����
// 			�Ȃ�
// 		�߂�l
// 			�Ȃ�
// ======================================================================
void UninitWinSock()
{
	WSACleanup();
}

// ======================================================================
// 	�\�P�b�g������
// 		����
// 			�Ȃ�
// 		�߂�l
// 			SOCKET
// ======================================================================
SOCKET InitSocket()
{
	SOCKET sockNo;

	// �\�P�b�g�̐���(IPv4, �f�[�^�O�����ʐM, UDP)
	sockNo = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (sockNo == SOCKET_ERROR)
		MessageBox(NULL, "�\�P�b�g������������܂���ł����B", "�G���[��", MB_OK);

	return sockNo;
}

// ======================================================================
// 	�\�P�b�g�I��
// 		����
// 			SOCKET *
// 		�߂�l
// 			�Ȃ�
// ======================================================================
void UninitSocket(SOCKET* pSockNo)
{
	if (*pSockNo != INVALID_SOCKET)
	{
		closesocket(*pSockNo);
	}
}

// ======================================================================
// 	IP�A�h���X�ݒ�
// 		����
// 			sockaddr_in *,unsigned long,unsigned short
// 		�߂�l
// 			�Ȃ�
// ======================================================================
void SetAddressToSockAddrIn(sockaddr_in* pSockAddrIn, unsigned long Addr, unsigned short portNo)
{
	pSockAddrIn->sin_addr.S_un.S_addr = Addr;
	pSockAddrIn->sin_family = AF_INET;
	pSockAddrIn->sin_port = htons(portNo);
}

// ======================================================================
// 	�o�C���h
// 		����
// 			SOCKET,sockaddr_in*
// 		�߂�l
// 			bool true:���� false:���s
// ======================================================================
bool BindToSocket(SOCKET sockNo, sockaddr_in* pMyAddr)
{
	int nState;
	nState = bind(sockNo, (sockaddr*)pMyAddr, sizeof(sockaddr));

	if (nState == SOCKET_ERROR)
	{
		MessageBox(NULL, "�\�P�b�g�o�C���h�ɃG���[���N���܂����B", "�G���[��", MB_OK);
		return false;
	}

	return true;
}

// ======================================================================
// 	���~�e�b�h�u���[�h�L���X�g���[�h�ݒ�
// 		����
// 			SOCKET,bool
// 		�߂�l
// 			bool true:���� false:���s
// ======================================================================
bool SetLimitedBroadCastMode(SOCKET sockNo, bool bFlg)
{
	int nState;
	nState = setsockopt(sockNo, SOL_SOCKET, SO_BROADCAST, (char*)&bFlg, sizeof(bFlg));
	if (nState == SOCKET_ERROR)
	{
		MessageBox(NULL, "���~�e�b�h�u���[�h�L���X�g���[�h�ɃG���[���o�܂����B", "�G���[��", MB_OK);
		return false;
	}

	return true;
}

// ======================================================================
// 	sendTo���b�p�[
// 		����
// 			SOCKET,const char *,int,sockaddr*
// 		�߂�l
// 			bool true:���� false:���s
// ======================================================================
bool SendDataTo(SOCKET sockNo, const char* pszData, int nDataSize, sockaddr* toAddr)
{
	int nState;
	nState = sendto(sockNo, pszData, nDataSize, 0, toAddr, sizeof(sockaddr));
	if (nState == SOCKET_ERROR)
	{

		MessageBox(NULL, "SendTo���b�p�[�ŃG���[���o�܂����B", "�G���[��", MB_OK);
		return false;
	}

	return true;
}

// ======================================================================
// 	recvFrom���b�p�[
// 		����
// 			SOCKET,char *,int,sockaddr*
// 		�߂�l
// 			int SOCKET_ERROR�܂��͎�M�o�C�g��
// ======================================================================
int RecvDataFrom(SOCKET sockNo, char* pszRecvBuff, int nBuffSize, sockaddr* fromAddr)
{
	int nLen;		// ���M���A�h���X��
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


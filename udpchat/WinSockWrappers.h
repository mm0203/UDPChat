#pragma once

#include <winsock2.h>

bool InitWinSock();
void UninitWinSock();
SOCKET InitSocket();
void UninitSocket(SOCKET *);

void SetAddressToSockAddrIn(sockaddr_in *,unsigned long,unsigned short);
bool BindToSocket(SOCKET,sockaddr_in *);
bool SetLimitedBroadCastMode(SOCKET, bool);

bool SendDataTo(SOCKET,const char *,int,sockaddr *);
int RecvDataFrom(SOCKET,char *,int,sockaddr *);


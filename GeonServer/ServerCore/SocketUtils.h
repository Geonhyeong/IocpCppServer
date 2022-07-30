#pragma once
#include "NetAddress.h"

/*
	WSAStartup 같은 소켓과 관련된 라이브러리
*/

class SocketUtils
{
public:
	/* 
		함수 포인터
		WSASend와 WSARecv는 송수신 완료 통지를 IOCP를 통해 받을 수 있지만,
		accept와 connect는 받을 수 없다. 이를 위해 MS에서 확장API를 제공함.
		비동기 입출력 방식으로 작동하는 함수이다.
	*/
	static LPFN_CONNECTEX		ConnectEx;
	static LPFN_DISCONNECTEX	DisconnectEx;
	static LPFN_ACCEPTEX		AcceptEx;
	
public:
	static void Init();
	static void Clear();

	static bool BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn);
	static SOCKET CreateSocket();

	static bool SetLinger(SOCKET socket, uint16 onoff, uint16 linger);
	static bool SetReuseAddress(SOCKET socket, bool flag);
	static bool SetRecvBufferSize(SOCKET socket, int32 size);
	static bool SetSendBufferSize(SOCKET socket, int32 size);
	static bool SetTcpNoDelay(SOCKET socket, bool flag);
	static bool SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket);

	static bool Bind(SOCKET socket, NetAddress netAddr);
	static bool BindAnyAddress(SOCKET socket, uint16 port);
	static bool Listen(SOCKET socket, int32 backlog = SOMAXCONN);
	static void Close(SOCKET socket);
};

template<typename T>
static inline bool SetSockOpt(SOCKET socket, int32 level, int32 optName, T optVal)
{
	return SOCKET_ERROR != ::setsockopt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(T));
}
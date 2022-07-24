#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <WinSock2.h>
#include <process.h>

#define BUFSIZE 1024

typedef struct  
{
	SOCKET		socket;
	SOCKADDR_IN	addr;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

typedef struct  
{
	OVERLAPPED	overlapped;
	char		buffer[BUFSIZE];
	WSABUF		wsaBuf;
} PER_IO_DATA, *LPPER_IO_DATA;

unsigned int __stdcall WorkThread(LPVOID _hComPort);

int main()
{
	WSADATA		wsaData;
	HANDLE		hComPort;
	SOCKET		serverSocket;
	SOCKADDR_IN serverAddr;
	SYSTEM_INFO	sysInfo;
	LPPER_HANDLE_DATA	PerHandleData;
	LPPER_IO_DATA		PerIoData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		std::cerr << "WSAStartup() Failed!" << std::endl;

	hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	GetSystemInfo(&sysInfo);

	for (unsigned int i = 0; i < sysInfo.dwNumberOfProcessors; i++)
		_beginthreadex(NULL, 0, WorkThread, (LPVOID)hComPort, 0, NULL);

	serverSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(atoi("7777"));

	bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	listen(serverSocket, 10);

	while (1)
	{
		SOCKET		clientSocket;
		SOCKADDR_IN	clientAddr;
		int addrLen = sizeof(clientAddr);
		int recvBytes, flags;

		clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &addrLen);

		PerHandleData = (LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));
		PerHandleData->socket = clientSocket;
		memcpy(&(PerHandleData->addr), &clientAddr, addrLen);

		CreateIoCompletionPort((HANDLE)clientSocket, hComPort, (DWORD)PerHandleData, 0);

		PerIoData = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		memset(&(PerIoData->overlapped), 0, sizeof(OVERLAPPED));
		PerIoData->wsaBuf.len = BUFSIZE;
		PerIoData->wsaBuf.buf = PerIoData->buffer;

		WSARecv(PerHandleData->socket, &(PerIoData->wsaBuf), 1, (LPDWORD)&recvBytes, (LPDWORD)&flags, &(PerIoData->overlapped), NULL);
	}

	return 0;
}

unsigned int __stdcall WorkThread(LPVOID _hComPort)
{
	HANDLE hComport = _hComPort;

	DWORD				bytesTransferred;
	LPPER_HANDLE_DATA	PerHandleData;
	LPPER_IO_DATA		PerIoData;
	DWORD				flags;

	while (1)
	{
		GetQueuedCompletionStatus(hComport, &bytesTransferred, (LPDWORD)&PerHandleData, (LPOVERLAPPED*)&PerIoData, INFINITE);

		if (bytesTransferred == 0)
		{
			closesocket(PerHandleData->socket);
			free(PerHandleData);
			free(PerIoData);
			continue;
		}

		PerIoData->wsaBuf.buf[bytesTransferred] = '\0';
		printf("Recv[%s]\n", PerIoData->wsaBuf.buf);
		
		PerIoData->wsaBuf.len = bytesTransferred;
		WSASend(PerHandleData->socket, &(PerIoData->wsaBuf), 1, NULL, 0, NULL, NULL);

		memset(&(PerIoData->overlapped), 0, sizeof(OVERLAPPED));
		PerIoData->wsaBuf.len = BUFSIZE;
		PerIoData->wsaBuf.buf = PerIoData->buffer;

		flags = 0;
		WSARecv(PerHandleData->socket, &(PerIoData->wsaBuf), 1, NULL, &flags, &(PerIoData->overlapped), NULL);
	}
	return 0;
}
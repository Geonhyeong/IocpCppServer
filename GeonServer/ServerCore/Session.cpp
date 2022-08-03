#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"

Session::Session()
{
	_socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
	SocketUtils::Close(_socket);
}

void Session::Send(BYTE* buffer, int32 len)
{
	// 생각할 문제
	// 1) 버퍼 관리?
	// 2) sendEvent 관리? 단일? 여러개? WSASend 중첩?
	// Send는 Recv와 다르게 수없이 호출되기 때문에 정책이 다르다.
	// Broadcast할 때 세션마다 sendBuffer의 복사비용이 들면 너무 무겁기 때문에 복사비용도 고려해야함.

	// TEMP
	SendEvent* sendEvent = new SendEvent();
	sendEvent->owner = shared_from_this();	// ADD_REF
	sendEvent->buffer.resize(len);
	::memcpy(sendEvent->buffer.data(), buffer, len);

	WRITE_LOCK;
	RegisterSend(sendEvent);
}

void Session::Disconnect(const WCHAR* cause)
{
	if (_connected.exchange(false) == false)
		return;

	// TEMP
	wcout << "Disconnect : " << cause << endl;

	OnDisconnected();	// 컨텐츠 코드에서 오버로딩
	SocketUtils::Close(_socket);
	GetService()->ReleaseSession(GetSessionRef());
}

HANDLE Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	switch (iocpEvent->eventType)
	{
	case EventType::Conncet:
		ProcessConnect();
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(static_cast<SendEvent*>(iocpEvent), numOfBytes);
		break;
	default:
		break;
	}
}

void Session::RegisterConnect()
{
}

void Session::RegisterRecv()
{
	if (IsConnected() == false)
		return;

	_recvEvent.Init();
	_recvEvent.owner = shared_from_this(); // ADD_REF

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer);
	wsaBuf.len = len32(_recvBuffer);

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT &numOfBytes, OUT &flags, &_recvEvent, nullptr))
	{
		int32 errCode = ::WSAGetLastError();
		if (errCode != WSA_IO_PENDING)
		{
			HandleError(errCode);
			_recvEvent.owner = nullptr;	// RELEASE_REF
		}
	}
}

void Session::RegisterSend(SendEvent* sendEvent)
{
	if (IsConnected() == false)
		return;

	WSABUF wsaBuf;
	wsaBuf.buf = (char*)sendEvent->buffer.data();
	wsaBuf.len = (ULONG)sendEvent->buffer.size();

	DWORD numOfBytes = 0;
	if (SOCKET_ERROR == ::WSASend(_socket, &wsaBuf, 1, OUT &numOfBytes, 0, sendEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			sendEvent->owner = nullptr;	// RELEASE_REF
			delete(sendEvent);
		}
	}

}

void Session::ProcessConnect()
{
	_connected = true;

	// 세션 등록
	GetService()->AddSession(GetSessionRef());

	// 콘텐츠 코드에서 오버로딩
	OnConnected();

	// 수신 등록
	RegisterRecv();
}

void Session::ProcessRecv(int32 numOfBytes)
{
	_recvEvent.owner = nullptr;	// RELEASE_REF

	if (numOfBytes == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}

	// 컨텐츠 코드에서 오버로딩
	OnRecv(_recvBuffer, numOfBytes);

	// 수신 등록
	RegisterRecv();
}

void Session::ProcessSend(SendEvent* sendEvent, int32 numOfBytes)
{
	/*
		WSASend는 thread_safe한가? WSASend할 때는 순서가 보장이 되지만
		GQCS할 때는 순서가 보장되지 않는다. 그래서 문제가 생길 수 있다.
		또한 커널의 sendbuffer가 가득차서 pending이 발생할텐데 막 WSASend를 거는게 맞는것일까?
		크게 묶어서 한번에 Send를 보내는게 더 좋다.
	*/

	sendEvent->owner = nullptr;	// RELEASE_REF
	delete(sendEvent);

	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	// 컨텐츠 코드에서 오버로딩
	OnSend(numOfBytes);
}

void Session::HandleError(int32 errorCode)
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"HandleError");
		break;
	default:
		// TODO : Log
		cout << "Handle Error : " << errorCode << endl;
		break;
	}
}

#pragma once

class Session;

enum class EventType : uint8
{
	Conncet,
	Accept,
	// PreRecv, 0 byte recv
	Recv,
	Send
};

/*
	Overlapped 구조체를 상속받아서 0번 offset에는 Overlapped 구조체의 내용이 들어가있기 때문에
	IocpEvent 포인터를 사용하면 Overlapped 구조체 포인터를 사용하는 것과 다름이 없다.
	virtual 함수를 사용하면 안된다. 0번 offset에 가상 테이블이 들어가기 때문에
*/
class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void			Init();

public:
	EventType		eventType;
	IocpObjectRef	owner;
};

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Conncet) { }
};

class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) { }

public:
	/* AcceptEx에 client socket을 미리 생성해서 넘겨줘야하기 때문에 인자가 미리 필요하다. */
	SessionRef session = nullptr;
};

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) { }
};

class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) { }
};
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
	Overlapped ����ü�� ��ӹ޾Ƽ� 0�� offset���� Overlapped ����ü�� ������ ���ֱ� ������
	IocpEvent �����͸� ����ϸ� Overlapped ����ü �����͸� ����ϴ� �Ͱ� �ٸ��� ����.
	virtual �Լ��� ����ϸ� �ȵȴ�. 0�� offset�� ���� ���̺��� ���� ������
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
	/* AcceptEx�� client socket�� �̸� �����ؼ� �Ѱ�����ϱ� ������ ���ڰ� �̸� �ʿ��ϴ�. */
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
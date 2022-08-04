#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "ThreadManager.h"

// TEMP
char sendBuffer[] = "Hello World";

class ServerSession : public Session
{
public:
	~ServerSession()
	{
		cout << "~ServerSession" << endl;
	}

	virtual void OnConnected() override
	{
		cout << "Connected To Server" << endl;
		Send((BYTE*)sendBuffer, sizeof(sendBuffer));
	}

	virtual int32 OnRecv(BYTE* buffer, int32 len) override
	{
		cout << "OnRecv Len = " << len << endl;

		this_thread::sleep_for(1s);

		Send((BYTE*)sendBuffer, sizeof(sendBuffer));
		return len;
	}

	virtual void OnSend(int32 len) override
	{
		cout << "OnSend Len = " << len << endl;
	}

	virtual void OnDisconnected() override
	{
		cout << "Disconnected" << endl;
	}

};

int main()
{
	this_thread::sleep_for(1s);

	ClientServiceRef service = make_shared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		make_shared<IocpCore>(),
		[]() -> SessionRef { return make_shared<ServerSession>(); },	// TODO : SessionManager µî
		1);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	GThreadManager->Join();
}
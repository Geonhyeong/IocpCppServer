#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "ThreadManager.h"

// TEMP
char sendData[] = "Hello World";

class ServerSession : public PacketSession
{
public:
	~ServerSession()
	{
		cout << "~ServerSession" << endl;
	}

	virtual void OnConnected() override
	{
		//cout << "Connected To Server" << endl;
	}

	virtual int32 OnRecvPacket(BYTE* buffer, int32 len) override
	{
		PacketHeader header = *((PacketHeader*)buffer);
		// cout << "Packet ID : " << header.id << " Size : " << header.size << endl;

		char recvBuffer[4096];
		::memcpy(recvBuffer, &buffer[4], header.size - sizeof(PacketHeader));
		cout << recvBuffer << endl;

		return len;
	}

	virtual void OnSend(int32 len) override
	{
		// cout << "OnSend Len = " << len << endl;
	}

	virtual void OnDisconnected() override
	{
		// cout << "Disconnected" << endl;
	}

};

int main()
{
	this_thread::sleep_for(1s);

	ClientServiceRef service = make_shared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		make_shared<IocpCore>(),
		[]() -> SessionRef { return make_shared<ServerSession>(); },	// TODO : SessionManager ��
		1000);

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
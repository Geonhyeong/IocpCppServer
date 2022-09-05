#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "GameSession.h"
#include "GameSessionManager.h"

int main()
{
	ServerServiceRef service = make_shared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		make_shared<IocpCore>(),
		[]() -> SessionRef { return make_shared<GameSession>(); },	// TODO : SessionManager 등
		10);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	char sendData[1000] = "Hello World";
	while (true)
	{
		SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

		BYTE* buffer = sendBuffer->Buffer();
		((PacketHeader*)buffer)->size = (sizeof(sendData) + sizeof(PacketHeader));
		((PacketHeader*)buffer)->id = 1;	// 1 : Hello Msg
		::memcpy(&buffer[4], sendData, sizeof(sendData));
		sendBuffer->Close(sizeof(sendData) + sizeof(PacketHeader));

		GSessionManager.Broadcast(sendBuffer);

		this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();
}
#include "pch.h"
#include "ThreadManager.h"
#include "Listener.h"

int main()
{
	Listener listener;
	listener.StartAccept(NetAddress(L"127.0.0.1", 7777));

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					GIocpCore.Dispatch();
				}
			});
	}

	GThreadManager->Join();
}
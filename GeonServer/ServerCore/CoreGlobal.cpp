#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
#include "SocketUtils.h"
#include "SendBuffer.h"

ThreadManager*		GThreadManager = nullptr;
SendBufferManager*	GSendBufferManager = nullptr;
DeadLockProfiler*	GDeadLockProfiler = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
		GSendBufferManager = new SendBufferManager();
		GDeadLockProfiler = new DeadLockProfiler();
		SocketUtils::Init();
	}
	~CoreGlobal()
	{
		delete GThreadManager;
		delete GSendBufferManager;
		delete GDeadLockProfiler;
		SocketUtils::Clear();
	}
} GCoreGlobal;	// �����԰� ���ÿ� ������ü�� ���������ν� �������ܿ��� �ǵ帮�� �ʾƵ� ���������� �۵��Ѵ�.
#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
#include "SocketUtils.h"
#include "SendBuffer.h"
#include "GlobalQueue.h"

ThreadManager*		GThreadManager = nullptr;
SendBufferManager*	GSendBufferManager = nullptr;
DeadLockProfiler*	GDeadLockProfiler = nullptr;
GlobalQueue*		GGlobalQueue = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
		GSendBufferManager = new SendBufferManager();
		GGlobalQueue = new GlobalQueue();
		GDeadLockProfiler = new DeadLockProfiler();
		SocketUtils::Init();
	}
	~CoreGlobal()
	{
		delete GThreadManager;
		delete GSendBufferManager;
		delete GGlobalQueue;
		delete GDeadLockProfiler;
		SocketUtils::Clear();
	}
} GCoreGlobal;	// �����԰� ���ÿ� ������ü�� ���������ν� �������ܿ��� �ǵ帮�� �ʾƵ� ���������� �۵��Ѵ�.
#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"

ThreadManager* GThreadManager = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
	}
	~CoreGlobal()
	{
		delete GThreadManager;
	}
} GCoreGlobal;	// 선언함과 동시에 전역객체를 생성함으로써 컨텐츠단에서 건드리지 않아도 정상적으로 작동한다.
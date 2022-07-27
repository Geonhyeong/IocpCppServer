#pragma once
/*
	표준 mutex lock은 재귀적으로 락을 잡을 수 없다.
	99.999999999 % 일정
	0.0000000001 % 변경이 있을 수 있음
	이럴 때 매번마다 상호배타적으로 접근을 1명만 하도록 막으면 너무 아깝다.
	이럴 때를 위해서 Reader/Writer Lock이 필요하다.
*/
#include "Types.h"

/*------------------
	RW SpinLock
------------------*/

/*
	[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR] 32bit
	W : WriteFlag (Exclusive Lock Owner ThreadId)
	R : ReadFlag (Shared Lock Count)
	
	[재귀 보장 순서]
	W -> R (O)
	R -> W (X)
*/

class Lock
{
	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK = 10000,	// 10000ms = 10s
		MAX_SPIN_COUNT = 5000,
		WRITE_THREAD_MASK = 0xFFFF'0000,
		READ_COUNT_MASK = 0x0000'FFFF,
		EMPTY_FLAG = 0x0000'0000
	};

public:
	void WriteLock(const char* name);
	void WriteUnlock(const char* name);
	void ReadLock(const char* name);
	void ReadUnlock(const char* name);

private:
	Atomic<uint32> _lockFlag = EMPTY_FLAG;
	uint16 _writeCount = 0;
};

/*-----------------
	LockGuards
-----------------*/

class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock, const char* name) : _lock(lock), _name(name) {_lock.ReadLock(name); }
	~ReadLockGuard() {_lock.ReadUnlock(_name); }

private:
	Lock& _lock;
	const char* _name;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock, const char* name) : _lock(lock), _name(name) { _lock.WriteLock(name); }
	~WriteLockGuard() { _lock.WriteUnlock(_name); }

private:
	Lock& _lock;
	const char* _name;
};
#pragma once
/*
	ǥ�� mutex lock�� ��������� ���� ���� �� ����.
	99.999999999 % ����
	0.0000000001 % ������ ���� �� ����
	�̷� �� �Ź����� ��ȣ��Ÿ������ ������ 1�� �ϵ��� ������ �ʹ� �Ʊ���.
	�̷� ���� ���ؼ� Reader/Writer Lock�� �ʿ��ϴ�.
*/
#include "Types.h"

/*------------------
	RW SpinLock
------------------*/

/*
	[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR] 32bit
	W : WriteFlag (Exclusive Lock Owner ThreadId)
	R : ReadFlag (Shared Lock Count)
	
	[��� ���� ����]
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
	void WriteLock();
	void WriteUnlock();
	void ReadLock();
	void ReadUnlock();

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
	ReadLockGuard(Lock& lock) : _lock(lock) {_lock.ReadLock(); }
	~ReadLockGuard() {_lock.ReadUnlock(); }

private:
	Lock& _lock;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock) : _lock(lock) { _lock.WriteLock(); }
	~WriteLockGuard() { _lock.WriteUnlock(); }

private:
	Lock& _lock;
};
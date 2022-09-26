#pragma once
#include "Job.h"

class Room
{
public:
	// �̱۾����� ȯ���θ��� �ڵ�
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void BroadCast(SendBufferRef sendBuffer);

public:
	// ��Ƽ������ ȯ�濡���� �ϰ����� ����
	void FlushJob();

	template<typename T, typename Ret, typename... Args>
	void PushJob(Ret(T::*memFunc)(Args...), Args... args)
	{
		auto job = make_shared<MemberJob<T, Ret, Args...>>(static_cast<T*>(this), memFunc, args...);
		_jobs.Push(job);
	}

private:
	map<uint64, PlayerRef> _players;
	JobQueue _jobs;
};

extern Room GRoom; // for test
#pragma once
#include "Job.h"

class Room
{
	friend class EnterJob;
	friend class LeaveJob;
	friend class BroadcastJob;

private:
	// �̱۾����� ȯ���θ��� �ڵ�
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void BroadCast(SendBufferRef sendBuffer);

public:
	// ��Ƽ������ ȯ�濡���� �ϰ����� ����
	void PushJob(JobRef job) { _jobs.Push(job); }
	void FlushJob();

private:
	map<uint64, PlayerRef> _players;
	JobQueue _jobs;
};

extern Room GRoom; // for test

// Room Jobs
class EnterJob : public IJob
{
public:
	EnterJob(Room& room, PlayerRef player) : _room(room), _player(player)
	{
	}

	virtual void Execute() override
	{
		_room.Enter(_player);
	}

public:
	Room& _room;
	PlayerRef _player;
};

class LeaveJob : public IJob
{
public:
	LeaveJob(Room& room, PlayerRef player) : _room(room), _player(player)
	{
	}

	virtual void Execute() override
	{
		_room.Leave(_player);
	}

public:
	Room& _room;
	PlayerRef _player;
};

class BroadcastJob : public IJob
{
public:
	BroadcastJob(Room& room, SendBufferRef sendBuffer) : _room(room), _sendBuffer(sendBuffer)
	{
	}

	virtual void Execute() override
	{
		_room.BroadCast(_sendBuffer);
	}

public:
	Room& _room;
	SendBufferRef _sendBuffer;
};
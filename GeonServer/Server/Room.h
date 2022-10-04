#pragma once
#include "JobQueue.h"

class Room : public JobQueue
{
public:
	// �̱۾����� ȯ���θ��� �ڵ�
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void BroadCast(SendBufferRef sendBuffer);

private:
	map<uint64, PlayerRef> _players;
};

extern shared_ptr<Room> GRoom; // for test
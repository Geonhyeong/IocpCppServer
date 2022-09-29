#pragma once
#include "JobSerializer.h"

class Room : public JobSerializer
{
public:
	// �̱۾����� ȯ���θ��� �ڵ�
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void BroadCast(SendBufferRef sendBuffer);

public:
	// ��Ƽ������ ȯ�濡���� �ϰ����� ����
	virtual void FlushJob() override;

private:
	map<uint64, PlayerRef> _players;
};

extern shared_ptr<Room> GRoom; // for test
#pragma once

class Room
{
public:
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void BroadCast(SendBufferRef sendBuffer);

private:
	USE_LOCK;
	map<uint64, PlayerRef> _players;
};

extern Room GRoom; // for test
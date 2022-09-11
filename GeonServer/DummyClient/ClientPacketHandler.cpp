#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"

void ClientPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	switch (header.id)
	{
	case S_TEST:
		Handle_S_TEST(buffer, len);
		break;
	}
}

// ��Ŷ ���� TEMP
// �޸𸮿� ������ �� �ڵ����� ���� ū ����Ʈ ������ ���߾� ���� ���� �ִµ� �̸� 1�� �����Ѵ�.
#pragma pack(1)
// [PKT_S_TEST][BuffsListItem BuffsListItem BuffsListItem]
struct PKT_S_TEST
{
	struct BuffsListItem
	{
		uint64 buffId;
		float remainTime;
	};

	uint16 packetSize; // ���� ���
	uint16 packetId; // ���� ���
	uint64 id; // 8
	uint32 hp; // 4
	uint16 attack; // 2
	uint16 buffsOffset;
	uint16 buffsCount;

	bool Validate()
	{
		uint32 size = 0;
		size += sizeof(PKT_S_TEST);
		if (packetSize < size)
			return false;

		size += buffsCount * sizeof(BuffsListItem);

		if (size != packetSize)
			return false;

		if (buffsOffset + buffsCount * sizeof(BuffsListItem) > packetSize)
			return false;
		
		return true;
	}

	using BuffsList = PacketList<PKT_S_TEST::BuffsListItem>;

	BuffsList GetBuffList()
	{
		BYTE* data = reinterpret_cast<BYTE*>(this);
		data += buffsOffset;
		return BuffsList(reinterpret_cast<PKT_S_TEST::BuffsListItem*>(data), buffsCount);
	}
	//vector<BuffData> buffs;
	//wstring name;
};
#pragma pack()

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	if (len < sizeof(PKT_S_TEST))
		return;

	PKT_S_TEST* pkt = reinterpret_cast<PKT_S_TEST*>(buffer);

	if (pkt->Validate() == false)
		return;

	//cout << "ID : " << id << " HP : " << hp << " ATT : " << attack << endl;

	PKT_S_TEST::BuffsList buffs = pkt->GetBuffList();
	
	cout << "BuffCount : " << buffs.Count() << endl;
	for (int32 i = 0; i < buffs.Count(); i++)
	{
		cout << "Buff Info : " << buffs[i].buffId << " " << buffs[i].remainTime << endl;
	}

	for (auto it = buffs.begin(); it != buffs.end(); ++it)
	{
		cout << "Buff Info : " << it->buffId << " " << it->remainTime << endl;
	}

	for (auto& buff : buffs)
	{
		cout << "Buff Info : " << buff.buffId << " " << buff.remainTime << endl;
	}
}
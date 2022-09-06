#pragma once
/*
	생각할 문제
	1) 버퍼 관리?
	2) sendEvent 관리? 단일? 여러개? WSASend 중첩?
	Send는 Recv와 다르게 수없이 호출되기 때문에 정책이 다르다.
	Broadcast할 때 세션마다 sendBuffer의 복사비용이 들면 너무 무겁기 때문에 복사비용도 고려해야함.
*/

class SendBufferChunk;

class SendBuffer
{
public:
	SendBuffer(SendBufferChunkRef owner, BYTE* buffer, uint32 allocSize);
	~SendBuffer();

	BYTE*	Buffer() { return _buffer; }
	uint32	Allocsize() { return _allocSize; }
	uint32	WriteSize() { return _writeSize; }
	void	Close(uint32 writeSize);

private:
	BYTE*				_buffer;
	uint32				_allocSize = 0;
	uint32				_writeSize = 0;
	SendBufferChunkRef	_owner;
};

/*
	버퍼들을 필요할 때마다 하나씩 만드는 게 아니라 굉장히 큰 덩어리를 일단 한번 할당 받은 후
	그걸 쪼개서 SendBuffer로 사용하기 위함.
*/
class SendBufferChunk : public enable_shared_from_this<SendBufferChunk>
{
	enum
	{
		SEND_BUFFER_CHUNK_SIZE = 6000
	};

public:
	SendBufferChunk();
	~SendBufferChunk();

	void Reset();
	SendBufferRef Open(uint32 allocSize);
	void Close(uint32 writeSize);

	bool IsOpen() { return _open; }
	BYTE* Buffer() { return &_buffer[_usedSize]; }
	uint32 FreeSize() { return static_cast<uint32>(_buffer.size()) - _usedSize; }

private:
	array<BYTE, SEND_BUFFER_CHUNK_SIZE>	_buffer = {};
	bool								_open = false;
	uint32								_usedSize = 0;
};

class SendBufferManager
{
public:
	SendBufferRef		Open(uint32 size);

private:
	SendBufferChunkRef	Pop();
	void				Push(SendBufferChunkRef buffer);

	static void			PushGlobal(SendBufferChunk* buffer);
	
private:
	USE_LOCK;
	vector<SendBufferChunkRef> _sendBufferChunks;
};
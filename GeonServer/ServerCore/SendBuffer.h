#pragma once
/*
	생각할 문제
	1) 버퍼 관리?
	2) sendEvent 관리? 단일? 여러개? WSASend 중첩?
	Send는 Recv와 다르게 수없이 호출되기 때문에 정책이 다르다.
	Broadcast할 때 세션마다 sendBuffer의 복사비용이 들면 너무 무겁기 때문에 복사비용도 고려해야함.
*/

class SendBuffer : enable_shared_from_this<SendBuffer>
{
public:
	SendBuffer(int32 bufferSize);
	~SendBuffer();

	BYTE* Buffer() { return _buffer.data(); }
	int32 WriteSize() { return _writeSize; }
	int32 Capacity() { return static_cast<int32>(_buffer.size()); }

	void CopyData(void* data, int32 len);

private:
	vector<BYTE>	_buffer;
	int32			_writeSize = 0;
};


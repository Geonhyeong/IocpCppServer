#pragma once
/*
	������ ����
	1) ���� ����?
	2) sendEvent ����? ����? ������? WSASend ��ø?
	Send�� Recv�� �ٸ��� ������ ȣ��Ǳ� ������ ��å�� �ٸ���.
	Broadcast�� �� ���Ǹ��� sendBuffer�� �������� ��� �ʹ� ���̱� ������ �����뵵 ����ؾ���.
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


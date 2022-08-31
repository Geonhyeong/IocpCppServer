#pragma once
/*
	Recv는 멀티스레드 환경에서 안전하다. 왜냐하면 딱 하나의 쓰레드만 Recv를 수행하기 때문이다.
	하지만 Session에 지역적으로 할당하여 사용하는 것은 문제가 있다.
	왜냐하면, TCP의 특성 상 수신된 패킷의 버퍼가 온전히 제대로 도착한 것인지 알 수가 없다.
	바운더리의 개념이 없기 때문에 패킷이 잘려서 왔을 수도 있고 더 길게 왔을 수도 있다.
	이는 후에 패킷헤더의 패킷사이즈 정보를 받아서 확인할 수 있지만 매번 버퍼를 덮어씌울 수 없다.
	그래서 패킷이 완전하지 않다면 뒤이어 버퍼를 덧붙여 데이터를 복사하는 식으로 만들어야 한다.
*/

// [][][][][][][][rw][][]
class RecvBuffer
{
	enum { BUFFER_COUNT = 10 };

public:
	RecvBuffer(int32 bufferSize);
	~RecvBuffer();

	void			Clean();
	bool			OnRead(int32 numOfBytes);
	bool			OnWrite(int32 numOfBytes);

	BYTE*			ReadPos() { return &_buffer[_readPos]; }
	BYTE*			WritePos() { return &_buffer[_writePos]; }
	int32			DataSize() { return _writePos - _readPos; }
	int32			FreeSize() { return _capacity - _writePos; }



private:
	int32			_capacity = 0;
	int32			_bufferSize = 0;
	int32			_readPos = 0;
	int32			_writePos = 0;
	vector<BYTE>	_buffer;
};


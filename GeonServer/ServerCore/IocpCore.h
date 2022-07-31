#pragma once

/* 
	세션의 상위 클래스
*/
class IocpObject : public enable_shared_from_this<IocpObject>
{
public:
	virtual HANDLE	GetHandle() abstract;
	virtual void	Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;
};

class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	HANDLE	GetHandle() { return _iocpHandle; }

	bool	Register(class IocpObject* iocpObject);
	bool	Dispatch(uint32 timeoutMs = INFINITE);

private:
	HANDLE _iocpHandle;
};

// TEMP
extern IocpCore GIocpCore;
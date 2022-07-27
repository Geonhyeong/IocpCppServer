#pragma once
#include <stack>
#include <map>
#include <vector>

/*
	Debug 모드에서 멀티쓰레드 내에 데드락이 있는지 DFS를 통해 사이클을 찾아 판별하는 프로파일러이다.
*/

class DeadLockProfiler
{
public:
	void PushLock(const char* name);
	void PopLock(const char* name);
	void CheckCycle();

private:
	void Dfs(int32 here);

private:
	unordered_map<const char*, int32>	_nameToId;
	unordered_map<int32, const char*>	_idToName;
	map<int32, set<int32>>				_lockHistory;

	Mutex _lock;

private:
	vector<int32>	_discoveredOrder;		// 노드가 발견된 순서를 기록하는 배열
	int32			_discoveredCount = 0;	// 노드가 발견된 순서
	vector<bool>	_finished;				// Dfs(i)가 종료 되었는지 여부
	vector<int32>	_parent;
};


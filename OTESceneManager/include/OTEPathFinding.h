#pragma once
#include "OTEStaticInclude.h"
#include "OTECollisionMap.h"

namespace OTE
{
// 历史纪录

struct HistoryNode_t
{ 
	HistoryNode_t(int x, int y, int F, int G)
	{
		_X = x;
		_Y = y;
		_F = F;
		_G = G;
	}
	// 下面为浮点型 为了客户端能精确到达指定点
	float _X; 
	float _Y;
	float _F;
	float _G;
};

// 节点

struct WayNode_t
{ 
	WayNode_t(int x, int y, int G, int H)
	{
		_X = x;
		_Y = y;
		_G = G;
		_H = H;
		_F = _G + _H;
	}

	int		_X; 
	int		_Y;
	int		_G;
	int		_H;
	int		_F;		

};

// *****************************************
// COTEPathFinding
// 寻路算法
// *****************************************

class _OTEExport COTEPathFinding
{
public:

	static void Init(COTECollisionMap* gmap);

	static void Clear();
	
	// 寻路接口

	static bool FindPath(COTECollisionMap* gmap, int x, int y, int tx, int ty, std::list<HistoryNode_t>& history);

};	


}

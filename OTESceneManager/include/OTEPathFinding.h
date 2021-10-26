#pragma once
#include "OTEStaticInclude.h"
#include "OTECollisionMap.h"

namespace OTE
{
// ��ʷ��¼

struct HistoryNode_t
{ 
	HistoryNode_t(int x, int y, int F, int G)
	{
		_X = x;
		_Y = y;
		_F = F;
		_G = G;
	}
	// ����Ϊ������ Ϊ�˿ͻ����ܾ�ȷ����ָ����
	float _X; 
	float _Y;
	float _F;
	float _G;
};

// �ڵ�

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
// Ѱ·�㷨
// *****************************************

class _OTEExport COTEPathFinding
{
public:

	static void Init(COTECollisionMap* gmap);

	static void Clear();
	
	// Ѱ·�ӿ�

	static bool FindPath(COTECollisionMap* gmap, int x, int y, int tx, int ty, std::list<HistoryNode_t>& history);

};	


}

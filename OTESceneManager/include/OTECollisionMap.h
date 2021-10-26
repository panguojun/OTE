#pragma once
#include "OTEStaticInclude.h"

namespace OTE
{
// *****************************************
// COTECollisionMap
// Åö×²Í¼
// *****************************************

class _OTEExport COTECollisionMap
{
public:
	COTECollisionMap(void);
	~COTECollisionMap(void);

public:

	bool ReadFromBmp(const std::string& rFileName);

	unsigned int GetPixel(int x, int y);

	void Clear();

	void Refresh();

	int GetWidth() { return m_SizeX; }

	int GetHeight(){ return m_SizeZ; }

protected:

	unsigned int*	m_pData;

	std::string		m_ColliFile;

	int				m_SizeX;

	int				m_SizeZ;
};

// *****************************************
// CCollision2D
// 2DÅö×²½Ó¿Ú
// *****************************************

class _OTEExport CCollision2D
{
public:		
	
	static bool HitTest(COTECollisionMap* gmap, int x, int y);

	static bool HitTest(COTECollisionMap* gmap, int sx, int sy, int ex, int ey, float step = 1.0f, int hitCount = 1);
	
};

}
#pragma once

#include "OTETile.h"
#include "OTETilePage.h"

// *********************************************
// 山体高度管理器
// *********************************************
namespace OTE
{

class _OTEExport COTETerrainHeightMgr
{
public:	
	static COTETerrainHeightMgr* GetInstance();

public:
	enum {
		eTerrain_Edit_Raise			= 0, 
		eTerrain_Edit_Low			= 1,
		eTerrain_Edit_Flat			= 2, 
		eTerrain_Edit_Smooth		= 3, 
		eTerrain_Edit_SingleVertex	= 4,
		eTerrain_Edit_ManualHeight	= 5, 
		eTerrain_Edit_ManualHeight_Rect = 6
	};

	/// 编辑山体

	void SetHeightAt(COTETilePage* page, float cx, float cz,float dragHeight,float dragIntensity, float topSize, float outRad, int type = eTerrain_Edit_Raise);

	void SetHeightAt(COTETilePage* page, float x, float z, float y);

	// 单点操作
	void SetHeightAt(COTETile* tile, float x, float z, float height );
	
	// 区域设置高度
	
	void SetTileHeightAt(COTETile* tile, float height);

	///　其他

	void updateTerrainAllHeight(COTETilePage* page, TileCreateData_t* createData);
	
	void getVertexHeightData(COTETilePage* page, TileCreateData_t * createData);
	
private:

	// (x，z，拖拽高度，强度，外圈，内圈，操作类型)

	void SetHeightAt(COTETile* tile, float xo, float zo, float dragHeight, float dragIntensity, float topSize, float outRad, int type );
	
	void SetHeightAt(COTETile* tile, int i, int j);	//用于平滑地形

	void SetTileHeightAt(COTETile* tile, CRectA<float>& rect, int dire, float topSize, float intensity, int type);
	void SetTileHeightAt(COTETile* tile, CRectA<float>& rect, Ogre::Vector3 startPos, Ogre::Vector3 endPos, int dire, float heightValue, bool isRaise);

	void proTerrain(COTETile* tile, CRectA<float>& rect,float topSize, float intensity, int type);

	void ExportHeightData(COTETile* tile, int startx, int startz, TileCreateData_t* pData);
		
	void updateTerrainAllHeight(COTETile* tile, int i, int j, TileCreateData_t* createData);	

public:
	
	// 上一次编辑tile列表

	std::list<COTETile*>			m_LastEditTileList;

private:

	static COTETerrainHeightMgr s_Inst;
};


}

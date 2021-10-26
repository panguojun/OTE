#pragma once

#include "OTETile.h"
#include "OTETilePage.h"

// *********************************************
// ɽ��߶ȹ�����
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

	/// �༭ɽ��

	void SetHeightAt(COTETilePage* page, float cx, float cz,float dragHeight,float dragIntensity, float topSize, float outRad, int type = eTerrain_Edit_Raise);

	void SetHeightAt(COTETilePage* page, float x, float z, float y);

	// �������
	void SetHeightAt(COTETile* tile, float x, float z, float height );
	
	// �������ø߶�
	
	void SetTileHeightAt(COTETile* tile, float height);

	///������

	void updateTerrainAllHeight(COTETilePage* page, TileCreateData_t* createData);
	
	void getVertexHeightData(COTETilePage* page, TileCreateData_t * createData);
	
private:

	// (x��z����ק�߶ȣ�ǿ�ȣ���Ȧ����Ȧ����������)

	void SetHeightAt(COTETile* tile, float xo, float zo, float dragHeight, float dragIntensity, float topSize, float outRad, int type );
	
	void SetHeightAt(COTETile* tile, int i, int j);	//����ƽ������

	void SetTileHeightAt(COTETile* tile, CRectA<float>& rect, int dire, float topSize, float intensity, int type);
	void SetTileHeightAt(COTETile* tile, CRectA<float>& rect, Ogre::Vector3 startPos, Ogre::Vector3 endPos, int dire, float heightValue, bool isRaise);

	void proTerrain(COTETile* tile, CRectA<float>& rect,float topSize, float intensity, int type);

	void ExportHeightData(COTETile* tile, int startx, int startz, TileCreateData_t* pData);
		
	void updateTerrainAllHeight(COTETile* tile, int i, int j, TileCreateData_t* createData);	

public:
	
	// ��һ�α༭tile�б�

	std::list<COTETile*>			m_LastEditTileList;

private:

	static COTETerrainHeightMgr s_Inst;
};


}

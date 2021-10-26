#pragma once
#include "OTETile.h"
#include "OTETilePage.h"

// *******************************************
namespace OTE
{

// *******************************************
// COTETerrainLightmapMgr
// 山体光照图管理器
// *******************************************
class _OTEExport COTETerrainLightmapMgr
{
public:

	// 获得光源信息的函数, 
	// 通过它可以获得光源信息，默认情况下调用原来的

	typedef void (*GetLightInforHndler_t)(const Ogre::Vector3& paintPos, Ogre::Vector3& lightPos, Ogre::Vector3& direction, Ogre::ColourValue& diffuse, float& reflectDis, float& reflectDis0/*最大影响范围*/, float& alpha/*混合强度　SRC * (1 - alpha) + DES * alpha*/);

public:	
	static COTETerrainLightmapMgr* GetInstance();

public:
	
	enum eCoverMode{E_COVER, E_ADD, E_SUB, E_ADDGRAY, E_BLIND};	// coverMode : 颜色覆盖方式

	// 计算光照图

	void GenLightmap( COTETilePage* page, 
							const Ogre::Vector3&	lightDir,
							const Ogre::ColourValue& diffuse, const Ogre::ColourValue& ambient,
							eCoverMode coverMode = E_COVER,
							bool bRenderShadow = true,
							GetLightInforHndler_t	 pLightInforHndler = NULL);

	// 保存

	void SaveLightMapFile(COTETilePage* page);
	
	void SaveLightMapFile(COTETilePage* page, 
							const std::string& lightmapPath,
							const std::string& lightmapName );

	// 重置光照图

	void SetLightMapMaterial(COTETilePage* page, const std::string& lightMapName,
												   const std::string& lightmapPath);

	void SetLightMapMaterial(COTETilePage* page);

	void SetLightMapMaterial(COTETile* tile, const std::string& rLightmapName);	//重新设置地形光照图

	// 加载光照图

	bool LoadLightMapData(COTETilePage* page);

	bool LoadLightMapData(COTETilePage* page, std::string strFileName);

	// 释放用完的空光照图静态空间

	void releaseEmptyLightMap(COTETilePage* page)
	{
		delete [] page->m_LightmapData; 
		page->m_LightmapData = NULL;
	}

	// 注: x, z 为像素单位
	void setLightmapColorAt(COTETilePage* page, int x, int z, unsigned char nRed,unsigned char nGreen,unsigned char nBlue,  
																	float bindFactor,
																	eCoverMode CoverMode, 
																	unsigned char* pBase0);
	// 设置地形光照

	void setLightmapColorAt(COTETilePage* page, float x, float z, const Ogre::ColourValue& rCor, eCoverMode CoverMode, int brushSize = 2);
	
	void setLightmapColorAt(COTETilePage* page, float x, float z, const Ogre::ColourValue& rCor, eCoverMode CoverMode, 
												const unsigned char* pCorRawData, int brushSize, float brushStrength);
	
	void RefreshLightmapMaterial(COTETilePage* page, COTETile* tile);

	void RefreshLightmapMaterial(COTETilePage* page);

	Ogre::ColourValue PickLightmapColor(COTETilePage* page, float x, float z);

	// 碰撞图

	void GenCollisionMap(COTETilePage* page, unsigned char* pCollisionMap);

	void GenCollisionMap(COTETilePage* page);
	
	// 模糊算法 

	void Blur(COTETilePage* page, float cx = 0, float cz = 0, int brushSize = -1);

	void GenLightmapOnTiles( 
				COTETilePage* page, float x, float z, int brushSize,
				const Ogre::Vector3& lightDir,
				const Ogre::ColourValue& diffuse, const Ogre::ColourValue& ambient,		
				eCoverMode coverMode = E_COVER 
				);

protected:
	
	// 碰撞测试

	bool _IntersectObjects(const Ogre::Ray& r, float raylen = MAX_FLOAT, const std::string& unselName = "");

	bool _IntersectObjects(const Ogre::Ray& r, Ogre::Vector3& hitPos, float raylen = MAX_FLOAT, const std::string& unselName = "");

	void updateLightmapBuffer(int offset_in_buffer, unsigned char* pBase0, 
												  unsigned char red, unsigned char green, unsigned char blue);

	// 光照图计算实现

	void GenLightmapA(COTETilePage* page,  
						const CRectA<int>& lmRect, // 光照图像素方形
						const Ogre::Vector3& fSunDir,
						const Ogre::ColourValue& diffuse, const Ogre::ColourValue& ambient, 							
						eCoverMode CoverMode, 
						unsigned char* pBase0,
						bool bRenderShadow = false,
						GetLightInforHndler_t	 pLightInforHndler = NULL);

public:

	static int s_TileSerialNumbe;	//tile的编号

	static int  m_lightMapDataSize;

	// 状态数据

	Ogre::ColourValue		m_Ambient;

	Ogre::ColourValue		m_Diffuse;

	Ogre::Vector3			m_lightDir;

protected:

	static COTETerrainLightmapMgr s_Inst;

};



}

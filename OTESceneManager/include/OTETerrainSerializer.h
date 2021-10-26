#pragma once
// ------------------------------------------------------
#include "OTEStaticInclude.h"
#include <vector>
#include "OTETilePage.h"
#include "OgreDataStream.h"

namespace OTE
{
// *******************************************
// COTETerrainXmlSerializer
// *******************************************
class _OTEExport COTETerrainXmlSerializer : public Ogre::Serializer
{
public:	

	/// 单体

	static COTETerrainXmlSerializer* GetSingleton();

	static void DestroySingleton();	

	// 清理

	static void Clear();

public:		

	// 从文件加载

	bool LoadTerrain(const std::string& szFileName, COTETilePage* tp = NULL);

	bool ParseTerrainXml(const std::string& szXmlString, COTETilePage* tp = NULL);

	void LoadTerrainByBinary(const std::string& szFileName, COTETilePage* tp = NULL);

	// 加载高度图

	void LoadTerrainHeight(const std::string& szFileName);

	//加载法线
	void LoadTerrainNormal(const std::string&szFileName);

	// 加载UV图

	void LoadTerrainTextureInfo(const std::string& szFileName);

	// 创建terrainPage

	void BuildTerrainPage(COTETilePage* tp, TileCreateData_t& createData);

	// 保存数据

	void SaveTerrain(const std::string& szFileName, COTETilePage* tp);

	void ExportTerrainPage(COTETilePage* tp);

	void SaveTerrainHeight(int width, int height, unsigned char* pData, const std::string& szFileName);//lyhraw

	void SaveTerrainNormal(int width, int height, Ogre::Vector3* pData, const std::string& szFileName);

	void SaveTerrainTextureInfo(COTETilePage* tp, unsigned char* pData, const std::string& szFileName);

	//去除高度图灰尘
	void heightMapDeleteDirt(int iHeight, int iWidth, int Sensitivity, COTETilePage* tp);

	/// 新建一个场景向导 

	void CreateNewTerrainPage(TileCreateData_t& createData);

protected:

	COTETerrainXmlSerializer(void);
	~COTETerrainXmlSerializer(void);

private:

	// 高度图文件名

	static std::string					s_strHeightMapFileName;

	// 法线文件名

	static std::string					s_strNormalFileName;

	// UV信息文件名

	static std::string					s_strUVInfoFileName;

	// 光照图

	static std::string					s_strLightMapFileName;

	// Alpha图

	static std::string					s_strAlphaMapFileName;

	// 地表贴图

	static std::string					s_TerrainLayerTexes[3];//lyh三层,多层贴图层数改2
	
	// 工作目录

	static std::string					s_WorkPath;

	// 高度图数据

	Ogre::MemoryDataStreamPtr			m_HeightMapRawDataPtr;

	//法线
	Ogre::MemoryDataStreamPtr			m_NormalMapRawDataPtr;
	
	// uv数据

	Ogre::MemoryDataStreamPtr			m_UVMapRawDataPtr;

protected:

	static COTETerrainXmlSerializer*	s_pSingleton;

};



}

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

	/// ����

	static COTETerrainXmlSerializer* GetSingleton();

	static void DestroySingleton();	

	// ����

	static void Clear();

public:		

	// ���ļ�����

	bool LoadTerrain(const std::string& szFileName, COTETilePage* tp = NULL);

	bool ParseTerrainXml(const std::string& szXmlString, COTETilePage* tp = NULL);

	void LoadTerrainByBinary(const std::string& szFileName, COTETilePage* tp = NULL);

	// ���ظ߶�ͼ

	void LoadTerrainHeight(const std::string& szFileName);

	//���ط���
	void LoadTerrainNormal(const std::string&szFileName);

	// ����UVͼ

	void LoadTerrainTextureInfo(const std::string& szFileName);

	// ����terrainPage

	void BuildTerrainPage(COTETilePage* tp, TileCreateData_t& createData);

	// ��������

	void SaveTerrain(const std::string& szFileName, COTETilePage* tp);

	void ExportTerrainPage(COTETilePage* tp);

	void SaveTerrainHeight(int width, int height, unsigned char* pData, const std::string& szFileName);//lyhraw

	void SaveTerrainNormal(int width, int height, Ogre::Vector3* pData, const std::string& szFileName);

	void SaveTerrainTextureInfo(COTETilePage* tp, unsigned char* pData, const std::string& szFileName);

	//ȥ���߶�ͼ�ҳ�
	void heightMapDeleteDirt(int iHeight, int iWidth, int Sensitivity, COTETilePage* tp);

	/// �½�һ�������� 

	void CreateNewTerrainPage(TileCreateData_t& createData);

protected:

	COTETerrainXmlSerializer(void);
	~COTETerrainXmlSerializer(void);

private:

	// �߶�ͼ�ļ���

	static std::string					s_strHeightMapFileName;

	// �����ļ���

	static std::string					s_strNormalFileName;

	// UV��Ϣ�ļ���

	static std::string					s_strUVInfoFileName;

	// ����ͼ

	static std::string					s_strLightMapFileName;

	// Alphaͼ

	static std::string					s_strAlphaMapFileName;

	// �ر���ͼ

	static std::string					s_TerrainLayerTexes[3];//lyh����,�����ͼ������2
	
	// ����Ŀ¼

	static std::string					s_WorkPath;

	// �߶�ͼ����

	Ogre::MemoryDataStreamPtr			m_HeightMapRawDataPtr;

	//����
	Ogre::MemoryDataStreamPtr			m_NormalMapRawDataPtr;
	
	// uv����

	Ogre::MemoryDataStreamPtr			m_UVMapRawDataPtr;

protected:

	static COTETerrainXmlSerializer*	s_pSingleton;

};



}

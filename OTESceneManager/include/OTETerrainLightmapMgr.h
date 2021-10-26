#pragma once
#include "OTETile.h"
#include "OTETilePage.h"

// *******************************************
namespace OTE
{

// *******************************************
// COTETerrainLightmapMgr
// ɽ�����ͼ������
// *******************************************
class _OTEExport COTETerrainLightmapMgr
{
public:

	// ��ù�Դ��Ϣ�ĺ���, 
	// ͨ�������Ի�ù�Դ��Ϣ��Ĭ������µ���ԭ����

	typedef void (*GetLightInforHndler_t)(const Ogre::Vector3& paintPos, Ogre::Vector3& lightPos, Ogre::Vector3& direction, Ogre::ColourValue& diffuse, float& reflectDis, float& reflectDis0/*���Ӱ�췶Χ*/, float& alpha/*���ǿ�ȡ�SRC * (1 - alpha) + DES * alpha*/);

public:	
	static COTETerrainLightmapMgr* GetInstance();

public:
	
	enum eCoverMode{E_COVER, E_ADD, E_SUB, E_ADDGRAY, E_BLIND};	// coverMode : ��ɫ���Ƿ�ʽ

	// �������ͼ

	void GenLightmap( COTETilePage* page, 
							const Ogre::Vector3&	lightDir,
							const Ogre::ColourValue& diffuse, const Ogre::ColourValue& ambient,
							eCoverMode coverMode = E_COVER,
							bool bRenderShadow = true,
							GetLightInforHndler_t	 pLightInforHndler = NULL);

	// ����

	void SaveLightMapFile(COTETilePage* page);
	
	void SaveLightMapFile(COTETilePage* page, 
							const std::string& lightmapPath,
							const std::string& lightmapName );

	// ���ù���ͼ

	void SetLightMapMaterial(COTETilePage* page, const std::string& lightMapName,
												   const std::string& lightmapPath);

	void SetLightMapMaterial(COTETilePage* page);

	void SetLightMapMaterial(COTETile* tile, const std::string& rLightmapName);	//�������õ��ι���ͼ

	// ���ع���ͼ

	bool LoadLightMapData(COTETilePage* page);

	bool LoadLightMapData(COTETilePage* page, std::string strFileName);

	// �ͷ�����Ŀչ���ͼ��̬�ռ�

	void releaseEmptyLightMap(COTETilePage* page)
	{
		delete [] page->m_LightmapData; 
		page->m_LightmapData = NULL;
	}

	// ע: x, z Ϊ���ص�λ
	void setLightmapColorAt(COTETilePage* page, int x, int z, unsigned char nRed,unsigned char nGreen,unsigned char nBlue,  
																	float bindFactor,
																	eCoverMode CoverMode, 
																	unsigned char* pBase0);
	// ���õ��ι���

	void setLightmapColorAt(COTETilePage* page, float x, float z, const Ogre::ColourValue& rCor, eCoverMode CoverMode, int brushSize = 2);
	
	void setLightmapColorAt(COTETilePage* page, float x, float z, const Ogre::ColourValue& rCor, eCoverMode CoverMode, 
												const unsigned char* pCorRawData, int brushSize, float brushStrength);
	
	void RefreshLightmapMaterial(COTETilePage* page, COTETile* tile);

	void RefreshLightmapMaterial(COTETilePage* page);

	Ogre::ColourValue PickLightmapColor(COTETilePage* page, float x, float z);

	// ��ײͼ

	void GenCollisionMap(COTETilePage* page, unsigned char* pCollisionMap);

	void GenCollisionMap(COTETilePage* page);
	
	// ģ���㷨 

	void Blur(COTETilePage* page, float cx = 0, float cz = 0, int brushSize = -1);

	void GenLightmapOnTiles( 
				COTETilePage* page, float x, float z, int brushSize,
				const Ogre::Vector3& lightDir,
				const Ogre::ColourValue& diffuse, const Ogre::ColourValue& ambient,		
				eCoverMode coverMode = E_COVER 
				);

protected:
	
	// ��ײ����

	bool _IntersectObjects(const Ogre::Ray& r, float raylen = MAX_FLOAT, const std::string& unselName = "");

	bool _IntersectObjects(const Ogre::Ray& r, Ogre::Vector3& hitPos, float raylen = MAX_FLOAT, const std::string& unselName = "");

	void updateLightmapBuffer(int offset_in_buffer, unsigned char* pBase0, 
												  unsigned char red, unsigned char green, unsigned char blue);

	// ����ͼ����ʵ��

	void GenLightmapA(COTETilePage* page,  
						const CRectA<int>& lmRect, // ����ͼ���ط���
						const Ogre::Vector3& fSunDir,
						const Ogre::ColourValue& diffuse, const Ogre::ColourValue& ambient, 							
						eCoverMode CoverMode, 
						unsigned char* pBase0,
						bool bRenderShadow = false,
						GetLightInforHndler_t	 pLightInforHndler = NULL);

public:

	static int s_TileSerialNumbe;	//tile�ı��

	static int  m_lightMapDataSize;

	// ״̬����

	Ogre::ColourValue		m_Ambient;

	Ogre::ColourValue		m_Diffuse;

	Ogre::Vector3			m_lightDir;

protected:

	static COTETerrainLightmapMgr s_Inst;

};



}

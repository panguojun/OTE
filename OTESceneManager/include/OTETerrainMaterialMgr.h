#pragma once
#include "OTETile.h"
#include "OTETilePage.h"

// *******************************************
// ɽ�����
// *******************************************
namespace OTE
{

class _OTEExport COTETerrainMaterialMgr
{
public:	
	static COTETerrainMaterialMgr* GetInstance();

public:	

	// ������ɫ 

	void SetAlphamapColorAt(COTETilePage* page, float x, float z, 
							const Ogre::ColourValue& rCor, int CoverMode, int brushSize, float brushStrength);
	
	void SetAlphamapColorAt(COTETilePage* page, float x, float z, const Ogre::ColourValue& rCor, int CoverMode, 
							const unsigned char* pCorRawData, int brushSize, float brushStrength);


	/// ���ʹ���

	bool UpdateMaterialTexture(COTETilePage* page,
				float x, float z, std::string fileName, int layer)
	{
		COTETile* t = page->GetTileAt(x, z);
		if(t)
		{
			return t->SetMaterialTexture(fileName, layer);
		}
		return false;
	}

	Ogre::String GetTextureFileAtTile(COTETilePage* page, float x, float z, int layer);
	

	bool CheckTileTextureExist(COTETilePage* page, std::string tileName, int layer);

	void UpdateTileTexture(COTETilePage* page, std::string tileName, std::string fileName, int layer);

	void DeleteTileTexture(COTETilePage* page, std::string tileName, int layer);

	bool LoadAlphamapData(COTETilePage* page, std::string strFileName = "");
	
	bool SaveAlphamap(COTETilePage* page);

	bool SaveAlphamap( COTETilePage* page, 
							const std::string& alphamapPath,
							const std::string& alphamapName );	

protected:

	void SetAlphamapColorAt(COTETilePage* page,  int x, int z,
							unsigned char nRed, unsigned char nGreen,unsigned char nBlue, float bindFactor,
							int CoverMode, unsigned char* pBase0);	
	inline void updateAlphamapBuffer(int offset_in_buffer, unsigned char* pBase0, 
 						    unsigned char red, unsigned char green, unsigned char blue);
	
public:
	
	// ��һ�α༭tile�б�

	std::list<COTETile*>			m_LastEditTileList;

protected:

	static COTETerrainMaterialMgr s_Inst;

};

}


#include "oteterrainmaterialmgr.h"

// ----------------------------------------------
using namespace Ogre;
using namespace OTE;

// ----------------------------------------------
COTETerrainMaterialMgr COTETerrainMaterialMgr::s_Inst;
COTETerrainMaterialMgr* COTETerrainMaterialMgr::GetInstance(){return &s_Inst;}


// ----------------------------------------------------
// 更新缓存
// x, z 为像素单位
void COTETerrainMaterialMgr::SetAlphamapColorAt(COTETilePage* page,  int x, int z,
												unsigned char nRed, unsigned char nGreen,unsigned char nBlue, float bindFactor,
												int CoverMode, unsigned char* pBase0)
{	
	int offset = (page->m_Alphabmih.biHeight - z - 1) * (page->m_Alphabmih.biWidth) + x;
	if(offset < 0 || offset >= page->m_Alphabmih.biHeight * page->m_Alphabmih.biWidth)
	{
		return;
	}

	unsigned char* a = &page->m_AlphaBmpdata[offset * 3];

	int tBlue = (*(a	));
	int tGreen= (*(a + 1));
	int tRed  = (*(a + 2));

	if(CoverMode==-1)//sub
	{
		tRed-=nRed,tGreen-=nGreen,tBlue-=nBlue;
	}
	else if(CoverMode==0)//replace
	{
		tRed = nRed,tGreen = nGreen,tBlue = nBlue;
	}
	else if(CoverMode==1)//add
	{
		tRed += nRed,tGreen += nGreen,tBlue += nBlue;
	}
	else if(CoverMode==2)//add the gray
	{
		int tgray=(tRed+tGreen+tBlue) / 3;
		tRed = tgray+nRed,tGreen = tgray+nGreen,tBlue = tgray+nBlue;
	}
	else if(CoverMode==3)//bind
	{
		tRed = tRed * (1 - bindFactor) + nRed * bindFactor;
		tGreen = tGreen * (1 - bindFactor) + nGreen * bindFactor;
		tBlue = tBlue * (1 - bindFactor) + nBlue * bindFactor;
	}

	tRed  = tRed<255 ? tRed:255; 
	tRed  = tRed>0 ? tRed:0;
	tGreen= tGreen<255 ? tGreen:255; 
	tGreen= tGreen>0 ? tGreen:0;
	tBlue = tBlue<255 ? tBlue:255; 
	tBlue = tBlue>0 ? tBlue:0;

	//更新数据颜色	
	
	*a       = (unsigned char)tBlue; //蓝	
	*(a + 1) = (unsigned char)tGreen;//绿	
	*(a + 2) = (unsigned char)tRed;  //红

	// 更新显存中数据

	int offset_in_buffer = z * (page->m_Alphabmih.biWidth) + x;

	if(offset_in_buffer < 0 || offset_in_buffer >= page->m_Alphabmih.biHeight * page->m_Alphabmih.biWidth)
	{
		return;
	}	

	updateAlphamapBuffer(offset_in_buffer, pBase0, (unsigned char)tRed, (unsigned char)tGreen, (unsigned char)tBlue);	

}

// ----------------------------------------------------------------------
void COTETerrainMaterialMgr::updateAlphamapBuffer(int offset_in_buffer, unsigned char* pBase0, 
												  unsigned char red, unsigned char green, unsigned char blue)
{		
	unsigned char* pBase = pBase0 + offset_in_buffer * 4;    

	*(pBase)	=	blue;
	*(pBase+1)	=	green;
	*(pBase+2)	=	red;	
}

// ----------------------------------------------------------------------
void COTETerrainMaterialMgr::SetAlphamapColorAt(COTETilePage* page, float x, float z, 
												const Ogre::ColourValue& rCor, int CoverMode, int brushSize, float brushStrength)
{
	if(!page->m_AlphaBmpdata || page->m_AlphamapPtr.isNull())
		return;

	x = x / page->m_Scale.x; 
	z = z / page->m_Scale.z;

	unsigned char* pBase0 = static_cast<unsigned char*>(page->m_AlphamapPtr->getBuffer(0, 0)->lock(HardwareBuffer::HBL_DISCARD));
 	
	for( int i = 0; i < brushSize; i ++)
	for( int j = 0; j < brushSize; j ++)
	{		
		SetAlphamapColorAt(page, COTETilePage::c_AlphamapScale * x + i,	COTETilePage::c_AlphamapScale * z + j, rCor.r * 255, rCor.g * 255, rCor.b * 255, brushStrength, CoverMode, pBase0);
	}

	page->m_AlphamapPtr->getBuffer(0, 0)->unlock();
	
}

// ----------------------------------------------------------------------
void COTETerrainMaterialMgr::SetAlphamapColorAt(COTETilePage* page, float x, float z, const Ogre::ColourValue& rCor, int CoverMode, 
												const unsigned char* pCorRawData, int brushSize, float brushStrength)
{
	if(!page->m_AlphaBmpdata || page->m_AlphamapPtr.isNull())
		return;
	
	x = x / page->m_Scale.x; 
	z = z / page->m_Scale.z;

	unsigned char* pBase0 = static_cast<unsigned char*>(page->m_AlphamapPtr->getBuffer(0, 0)->lock(HardwareBuffer::HBL_DISCARD));
 
	for( int j = 0; j < brushSize; j ++)
	for( int i = 0; i < brushSize; i ++)		
	{
		float f = pCorRawData[i + j * brushSize] / 255.0f;

		SetAlphamapColorAt(page, COTETilePage::c_AlphamapScale * x + i,	
			COTETilePage::c_AlphamapScale * z + j, rCor.r * 255, rCor.g * 255, rCor.b * 255, f * brushStrength, CoverMode, pBase0);
	}

	page->m_AlphamapPtr->getBuffer(0, 0)->unlock();
	
}
//-----------------------------------------------------------------------
bool COTETerrainMaterialMgr::CheckTileTextureExist(COTETilePage* page, std::string tileName, int layer)
{
	assert(layer < TEXTURE_LAYER_COUNT);

	TileCreateData_t::TileTextureMap_t::iterator itr;
	itr = page->m_TileTexes[layer].find(tileName);  
	if( itr != page->m_TileTexes[layer].end() )    
	{
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------

void COTETerrainMaterialMgr::UpdateTileTexture(COTETilePage* page, std::string tileName, std::string fileName, int layer)
{
	assert(layer < TEXTURE_LAYER_COUNT);

	page->m_TileTexes[layer][tileName] = fileName;
	
}

//-----------------------------------------------------------------------
void COTETerrainMaterialMgr::DeleteTileTexture(COTETilePage* page, std::string tileName, int layer)
{
	assert(layer < TEXTURE_LAYER_COUNT);

	page->m_TileTexes[layer].erase(tileName);
}

// -------------------------------------------------
Ogre::String COTETerrainMaterialMgr::GetTextureFileAtTile(
				COTETilePage* page, float x, float z, int layer)
{
	std::string strLayerTex = "";

	COTETile* t = page->GetTileAt(x, z);
	if(t)
	{
		bool noEmpty = true;
			
		TileCreateData_t::TileTextureMap_t::iterator itr = page->m_TileTexes[layer].find(t->getName());
		if( itr == page->m_TileTexes[layer].end() )
			noEmpty = false;
		
		if(noEmpty)		
			strLayerTex = itr->second;	
	}

	return strLayerTex;
}
//------------------------------------------------------------------------------------
bool COTETerrainMaterialMgr::LoadAlphamapData(COTETilePage* page, std::string strFileName)
{
	return page->LoadAlphamapData(strFileName);
}

//------------------------------------------------------------------------------------
bool COTETerrainMaterialMgr::SaveAlphamap( COTETilePage* page, 
												 const std::string& alphamapPath,
												 const std::string& alphamapName )
{
	page->m_AlphamapPath  = alphamapPath;

	page->m_AlphamapName  = alphamapName;

	return SaveAlphamap(page);
}


//------------------------------------------------------------------------------------
bool COTETerrainMaterialMgr::SaveAlphamap(COTETilePage* page)
{
	if(!page->m_AlphaBmpdata)
	{
		::MessageBox(NULL, "Alpha数据为空！", "失败", MB_OK);
		return false;
	}

	// 文件

	std::string str = page->m_AlphamapPath + page->m_AlphamapName;
	
	FILE * p;
	p = fopen( str.c_str() , "wb" );
	if(!p)	
	{			
		::MessageBox(NULL, "写Alpha数据时文件打开失败！", "失败", MB_OK);
		return false;
	}

	fwrite(&page->m_Alphabmfh, sizeof(BITMAPFILEHEADER), 1, p);
	fwrite(&page->m_Alphabmih,  sizeof(BITMAPINFOHEADER), 1, p);
	fwrite(page->m_AlphaBmpdata, sizeof(unsigned char) * page->m_Alphabmih.biWidth * page->m_Alphabmih.biHeight * 3, 1, p);
	
	fclose(p);

	return true;
}



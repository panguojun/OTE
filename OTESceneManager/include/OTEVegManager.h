#pragma once

#ifdef __OTE
#include "OTETile.h"
#include "OTETilePage.h"
#endif

#include "OTEStaticInclude.h" 
#include "OgreD3D9RenderSystem.h" 
#include "OTEGrass.h" 

// *********************************************
#ifdef __OTE
	#define GET_TERRIN_HGT_AT(x, z)		COTETilePage::s_CurrentPage->GetHeightAt(x, z)
	#define GRASS_PAGE_SIZEX			(COTETilePage::s_CurrentPage->m_PageSizeX)
	#define GRASS_PAGE_SIZEZ			(COTETilePage::s_CurrentPage->m_PageSizeZ)
	#define TILE_PIXELSIZE					(COTETilePage::s_CurrentPage->m_TileSize)

	#define SCALEX						COTETilePage::s_CurrentPage->m_Scale.x
	#define SCALEZ						COTETilePage::s_CurrentPage->m_Scale.z
	#define GET_TERRAIN_HGT_AT(x, z)	COTETilePage::s_CurrentPage->GetHeightAt(x, z)
	#define GET_LM_COR_AT(x, z)			COTETilePage::s_CurrentPage->GetLightMapColorAt(x, z)
#endif

#ifdef __ZH
	#define GRASS_PAGE_SIZE				512
	#define TILE_PIXELSIZE					8
	#define SCALEX						2.0f
	#define SCALEZ						2.0f
	#define GET_TERRAIN_HGT_AT(x, z)	(*pGetHgtHdler)(x, z)
	#define GET_LM_COR_AT(x, z)			(*pGetColorHdler)(x, z)
#endif

namespace OTE
{
class CVegRQListener;

// *********************************************
// COTEVegManager
// *********************************************
class _OTEExport COTEVegManager
{
public:
	typedef Ogre::ColourValue (*GetLMColorHnd_t)(float x, float z);
	typedef float (*GetHgtHnd_t)(float x, float z);

public:

	static COTEVegManager* GetInstance();

	static void Destroy();

public:	

	void SetVegAt(	std::vector<SingleGrassVertex>* grassList, 
					GetLMColorHnd_t pGetColorHdler,
					GetHgtHnd_t pGetHgtHdler,
					float x, float z, unsigned char val
				  );

	// 更新一个tile上的草(草的一个储存单元)

	void UpdateTileVeg(	float startX, float startZ,
						std::vector<OTE::SingleGrassVertex>* grassList, 
						GetLMColorHnd_t pGetColorHdler,
						GetHgtHnd_t pGetHgtHdler,						
						bool manualClear = false
						);

	bool CreateFromFile(const std::string& rFileName);
	
	bool SaveToFile(const std::string& rFileName);

	

	void SetGrassTexture( const std::string& grassTexture );

	const Ogre::String& GetGrassTextureName();	

	void ClearContents();

private:

	COTEVegManager();
	
	// 草的分布关键函数

	static void veg_grow(	std::vector<OTE::SingleGrassVertex>* grassList,
							GetLMColorHnd_t pGetColorHdler,
							GetHgtHnd_t pGetHgtHdler,
							const Ogre::Vector2& Base, const Ogre::Vector2& Ps, const Ogre::Vector2 T[], float dm, int n, int grassIndex, int maxSteps );
	
	void Init();

	void Clear();	

public:

	CVegRQListener*		m_pVegRQListener;

protected:	

	unsigned char*		m_pVegMap;

private:

	static COTEVegManager s_Inst;

	static Ogre::Vector2 s_SeedByGod_Pos;

};


// *********************************************
// CVegRQListener
// *********************************************

class CVegRQListener : public Ogre::RenderQueueListener
{
public:
	CVegRQListener(Ogre::uint8 queue_id, bool post_queue)
	{		
		m_queue_id		= queue_id;
		m_post_queue	= post_queue;
		Init();
	}

	virtual ~CVegRQListener() 
	{
		Destroy();
	}
	
	void Init();
	void Destroy();

	virtual void renderQueueStarted(Ogre::RenderQueueGroupID id, bool& skipThisQueue);
    virtual void renderQueueEnded(Ogre::RenderQueueGroupID id, bool& repeatThisQueue);

	// methods for adjusting target queue settings
	void	setTargetRenderQueue(Ogre::uint8 queue_id)		{m_queue_id = queue_id;}
	void	setPostRenderQueue(bool post_queue)		{m_post_queue = post_queue;}
	
private:
	
	Ogre::uint8						m_queue_id;		
	bool							m_post_queue;	

};


}

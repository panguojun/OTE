/************************************************************
 ************************************************************

 模块名称： 草管理类
 相关模块： OTEGrass
						
 描述    ： 草的分布为了实现密集而自然的效果这里使用分形算法(IFS)
            对于精确位置的草 这里也可以进行单棵设定

 *************************************************************
 *************************************************************/

#include "otevegmanager.h"
#include "OgreD3D9RenderSystem.h" 
#include "OgreNoMemoryMacros.h"
#include "OTEGrass.h"
#include "OgreMemoryMacros.h"
#include "OTED3DManager.h"

#ifdef __OTE
#include "OTECommon.h"
#include "OTERenderLight.h"
#include "OTEQTSceneManager.h"
#endif

using namespace Ogre;
using namespace OTE;

// 最大草贴图名称长度

#define MAX_GRASSTEXTURENAME_LEN	 64

// =============================================
// COTEVegManager
// =============================================
Ogre::Vector2 COTEVegManager::s_SeedByGod_Pos;

// ----------------------------------------------
COTEVegManager COTEVegManager::s_Inst;
COTEVegManager* COTEVegManager::GetInstance()
{
	return &s_Inst;
}

// ----------------------------------------------
COTEVegManager::COTEVegManager() :
m_pVegRQListener(NULL),
m_pVegMap(NULL)
{
	
}

// ----------------------------------------------	
void COTEVegManager::Destroy()
{	
	if(s_Inst.m_pVegRQListener)
	{
#	ifdef __OTE
		SCENE_MGR_OGRE->removeRenderQueueListener(s_Inst.m_pVegRQListener);
		SAFE_DELETE(s_Inst.m_pVegRQListener)	
#   endif
	}

	// 清理资源

	s_Inst.Clear();

	// 删除草单体

	COTEGrass::DestroySingleton();
}

// ----------------------------------------------
// 清理内容
void COTEVegManager::ClearContents()
{
	if(m_pVegMap)
	{
		int size = GRASS_PAGE_SIZEX * GRASS_PAGE_SIZEZ;
		::ZeroMemory(m_pVegMap, size);
	}

	COTEGrass::GetSingleton()->ClearRenderContents();
}

// ----------------------------------------------
void COTEVegManager::Clear()
{
	SAFE_DELETE_ARRAY(s_Inst.m_pVegMap)	

	// 清理草单体

	COTEGrass::GetSingleton()->Clear();
}

// ----------------------------------------------
void COTEVegManager::Init()
{
	if(!m_pVegRQListener)
	{
#	ifdef __OTE

		m_pVegRQListener = new CVegRQListener(Ogre::RENDER_QUEUE_MAIN, true);
		SCENE_MGR_OGRE->addRenderQueueListener(m_pVegRQListener);

#   endif
	}

	// VegMap

	if(!m_pVegMap)
	{
		int size = GRASS_PAGE_SIZEX * GRASS_PAGE_SIZEZ;

		m_pVegMap = new unsigned char [size];
		::ZeroMemory(m_pVegMap, size);
	}
}	

// ---------------------------------------------
// 分形草 IFS

void COTEVegManager::veg_grow(std::vector<SingleGrassVertex>* grassList,							  
							  COTEVegManager::GetLMColorHnd_t pGetColorHdler,
							  COTEVegManager::GetHgtHnd_t pGetHgtHdler,
							  const Ogre::Vector2& Base, const Ogre::Vector2& Ps, const Ogre::Vector2 T[], float dm, int n, int grassIndex, int maxSteps )
{
	/// 大面积草采用分形算法

	Ogre::Vector2 P[3];
	
	Ogre::Vector2 thisT[3];

	for(int i = 0; i < 3; i ++)
	{
		// 图形

		Ogre::Radian rad = Ogre::Math::ATan2(-Base.x, Base.y);

		thisT[i].x = T[i].x * Ogre::Math::Cos(rad) - T[i].y * Ogre::Math::Sin(rad);
		thisT[i].y = T[i].x * Ogre::Math::Sin(rad) + T[i].y * Ogre::Math::Cos(rad);

		P[i] = Ps + (dm/* / (n + 1)*/) * thisT[i];
				
		if(n == maxSteps - 1)
		{
			float realPosX  = s_SeedByGod_Pos.x + P[i].x * Ogre::Math::RangeRandom(-dm, dm);
			float realPosZ  = s_SeedByGod_Pos.y + P[i].y * Ogre::Math::RangeRandom(-dm, dm);
	
			float pos[] = {realPosX, GET_TERRAIN_HGT_AT(realPosX, realPosZ),  realPosZ};		
			
			Ogre::ColourValue cor = GET_LM_COR_AT(realPosX, realPosZ);
			float corVal = (cor.r + cor.g + cor.b) / 3.0f;	

			SingleGrassVertex gv;
			gv.SetGrass(pos, grassIndex, unsigned char(255.0f * (corVal > 1.0f ? 1.0f : corVal))); // 颜色平均值作为亮度		
			grassList->push_back(gv);
			
		}
	}
	
	n ++;
	if(n < maxSteps)
	{
		if(rand() - rand() < 0)
			veg_grow(grassList, pGetColorHdler, pGetHgtHdler, P[0] - Ps, P[0],  T, dm, n, grassIndex, maxSteps);
		if(rand() - rand() < 0)
			veg_grow(grassList, pGetColorHdler, pGetHgtHdler, P[1] - Ps, P[1],  T, dm, n, grassIndex, maxSteps);
		if(rand() - rand() < 0)
			veg_grow(grassList, pGetColorHdler, pGetHgtHdler, P[2] - Ps, P[2],  T, dm, n, grassIndex, maxSteps);
	}
}

// ---------------------------------------------
// 000000 - 00 前六位是草的种类 后两位是随机种子参数

void COTEVegManager::SetVegAt(std::vector<SingleGrassVertex>* grassList, 
							  COTEVegManager::GetLMColorHnd_t pGetColorHdler,
							  COTEVegManager::GetHgtHnd_t pGetHgtHdler,
							  float x, float z,
							  unsigned char val)
{
	if(grassList)
	{
		s_SeedByGod_Pos.x =  int(x / SCALEX) * SCALEX;
		s_SeedByGod_Pos.y =  int(z / SCALEZ) * SCALEZ;

		unsigned char& reVal = m_pVegMap[int(z / SCALEZ) * GRASS_PAGE_SIZEX + int(x / SCALEX)];
		
		// 如果存在则擦掉

		if(/*reVal || */val == 0) 
		{	
			reVal = 0; // 擦除

			x = int(x / SCALEX / TILE_PIXELSIZE) * TILE_PIXELSIZE * SCALEX;
			z = int(z / SCALEZ / TILE_PIXELSIZE) * TILE_PIXELSIZE * SCALEZ;
			UpdateTileVeg(x, z, grassList, pGetColorHdler, pGetHgtHdler, false);

		}

		if(val > 0 && reVal == 0)
		{			
			unsigned char randSeed = val & 0x03;
			if(randSeed > 0)
			{
				const static Ogre::Vector2 T[] = { Ogre::Vector2(-0.707f, 0.707f), Ogre::Vector2(0.707f, 0.707f), Ogre::Vector2(0.0f, -1.0f) };

				srand( randSeed * (int(s_SeedByGod_Pos.y) * GRASS_PAGE_SIZEX + int(s_SeedByGod_Pos.x)) ); 

				veg_grow( grassList, pGetColorHdler, pGetHgtHdler, Ogre::Vector2(0.0f, 0.1f), Ogre::Vector2(0.0f, 0.0f), T, 1.5, 0, (val >> 2), 3 );
			}
			else
			{
				const static Ogre::Vector2 T[] = { Ogre::Vector2(-1.0f, 0.0f), Ogre::Vector2(1.0f, 0.0f), Ogre::Vector2(0.0f, 0.0f) };

				srand(0/* 1 * (s_SeedByGod_Pos.y * GRASS_PAGE_SIZEX + s_SeedByGod_Pos.x)*/ ); 

				veg_grow( grassList, pGetColorHdler, pGetHgtHdler, Ogre::Vector2(0.0f, 0.0f), Ogre::Vector2(0.0f, 0.0f), T, 1.0, 0, (val >> 2), 4 );

			}
		}

		// 写内存以便保存

		reVal = val;
	}
}

// ---------------------------------------------
void COTEVegManager::UpdateTileVeg(float startX, float startZ,
								   std::vector<OTE::SingleGrassVertex>* grassList, 
								   COTEVegManager::GetLMColorHnd_t pGetColorHdler,
								   COTEVegManager::GetHgtHnd_t pGetHgtHdler,								   
								   bool manualClear)
{
#ifdef G_DEBUG
if(::GetKeyState('G') & 0x80 && ::GetKeyState(VK_CONTROL) & 0x80)
{	
	return;
}
#endif

	if(!m_pVegMap)
		return;

	grassList->clear(); // 先清理	

	for(int x = 0; x < TILE_PIXELSIZE; x ++)
	for(int z = 0; z < TILE_PIXELSIZE; z ++)
	{
		s_SeedByGod_Pos.x =  startX + x * SCALEX;
		s_SeedByGod_Pos.y =  startZ + z * SCALEZ;

		int offsetX = int(s_SeedByGod_Pos.x / SCALEX) % GRASS_PAGE_SIZEX;
		int offsetZ = int(s_SeedByGod_Pos.y / SCALEZ) % GRASS_PAGE_SIZEZ;

		assert(offsetX < GRASS_PAGE_SIZEX && offsetZ < GRASS_PAGE_SIZEZ);
		unsigned char& val = m_pVegMap[offsetZ * GRASS_PAGE_SIZEX + offsetX];

		if(!manualClear && val > 0)
		{
			unsigned char randSeed = val & 0x03;
			if(randSeed > 0)
			{
				const static Ogre::Vector2 T[] = { Ogre::Vector2(-0.707f, 0.707f), Ogre::Vector2(0.707f, 0.707f), Ogre::Vector2(0.0f, -1.0f) };

				srand( randSeed * (int(s_SeedByGod_Pos.y) * GRASS_PAGE_SIZEX + int(s_SeedByGod_Pos.x)) ); 

				veg_grow( grassList, pGetColorHdler, pGetHgtHdler, Ogre::Vector2(0.0f, 0.1f), Ogre::Vector2(0.0f, 0.0f), T, 1.5, 0, (val >> 2), 3 );
			}
			else
			{
				const static Ogre::Vector2 T[] = { Ogre::Vector2(-1.0f, 0.0f), Ogre::Vector2(1.0f, 0.0f), Ogre::Vector2(0.0f, 0.0f) };

				srand(0/* 1 * (s_SeedByGod_Pos.y * GRASS_PAGE_SIZEX + s_SeedByGod_Pos.x)*/ ); 

				veg_grow( grassList, pGetColorHdler, pGetHgtHdler, Ogre::Vector2(0.0f, 0.0f), Ogre::Vector2(0.0f, 0.0f), T, 1.0, 0, (val >> 2), 4 );

			}
		}
		else
		{
			val = 0;
		}
	}	
}

// ---------------------------------------------
const Ogre::String& COTEVegManager::GetGrassTextureName()
{
	return COTEGrass::GetSingleton()->GetTextureName();
}

// ---------------------------------------------
void COTEVegManager::SetGrassTexture( const std::string& grassTexture )
{
	COTEGrass::GetSingleton()->SetTexture(grassTexture); // 要从文件里读 todo
}


// ---------------------------------------------
// 文件
// ---------------------------------------------
// 从文件创建

bool COTEVegManager::CreateFromFile(const std::string& rFileName )
{	
	if(!rFileName.empty())
	{	
		// 初始化

		Init();

		if(!CHECK_RES(rFileName))
		{
			OTE_LOGMSG("从文件加载草失败!" << rFileName) 	
#ifdef __OTE
			goto ExitHere;
#else	
			COTEGrass::GetSingleton()->CloseGrassRendering();
			return false;
#endif	
			
		}

		Ogre::DataStreamPtr stream = RESMGR_LOAD(rFileName);	

		if(stream.isNull())
		{
			OTE_LOGMSG("从文件加载草失败!" << rFileName) 	
			COTEGrass::GetSingleton()->CloseGrassRendering();
			return false;
		}

		char grassTextureName[MAX_GRASSTEXTURENAME_LEN];
		stream->read(grassTextureName, MAX_GRASSTEXTURENAME_LEN);
		stream->read(m_pVegMap, GRASS_PAGE_SIZEX * GRASS_PAGE_SIZEZ * sizeof(unsigned char));

		// 清理草对象
		
		OTE::COTEGrass::GetSingleton()->Clear();

		// 设置草的贴图

		if(grassTextureName[0] != '\0')
			OTE::COTEGrass::GetSingleton()->SetTexture(grassTextureName);

	}
	else
	{
		COTEGrass::GetSingleton()->CloseGrassRendering();
	}

#ifdef __OTE

ExitHere:

	OTE::COTEGrass::GetSingleton()->Clear();

	// OTE::COTEGrass::GetSingleton()->SetTexture("grassTexture.tga"); // 默认贴图
	
	int tilesPerLine = GRASS_PAGE_SIZEX / TILE_PIXELSIZE;	

	// 全部更新?

	for(int j = 0; j < tilesPerLine; j ++)
	for(int i = 0; i < tilesPerLine; i ++)
	{
		COTETile* t = COTETilePage::s_CurrentPage->GetTileAt(i, j);
		if(t)		
			t->m_GrassNeedUpdate = true;
	}

#endif	
	
	return true;
}

// ---------------------------------------------
// 保存到文件

bool COTEVegManager::SaveToFile(const std::string& rFileName )
{
	if(!m_pVegMap)
		return false;	

	// 保存草数据 

	FILE* file = fopen(rFileName.c_str(), "wb");
	if(!file)
	{
		OTE_LOGMSG("植被数据文件打开失败！"); 
		return false;
	}

	if(COTEGrass::GetSingleton()->GetTextureName().length() > MAX_GRASSTEXTURENAME_LEN)
	{
		OTE_MSG("贴图名称过长(必须小于64个英文字符)!", "错误")
		fclose(file);	
		return false;
	}
	fwrite( COTEGrass::GetSingleton()->GetTextureName().c_str(), MAX_GRASSTEXTURENAME_LEN, 1, file );	
	fwrite( m_pVegMap, GRASS_PAGE_SIZEX * GRASS_PAGE_SIZEZ * sizeof(unsigned char), 1, file );	

	fclose(file);	

	return true;
}

namespace OTE
{
// ============================================
// CFrameListener
// ============================================
class CVegFrameListener : public Ogre::FrameListener
{
public:

	CVegFrameListener(){};
	virtual ~CVegFrameListener(){}

public:	 

	void showDebugOverlay(bool show){}

	bool frameStarted(const Ogre::FrameEvent& evt)
	{
		if(IS_DEVICELOST) // 设备丢失
		{
			COTEGrass::GetSingleton()->InvalidateDevice();
		}

		return true;
	}

	// --------------------------------------------
	bool frameEnded(const Ogre::FrameEvent& evt)
	{			
		
		return true;
	}

public:

	static CVegFrameListener* s_pVegFrameListener;
};

}

CVegFrameListener* CVegFrameListener::s_pVegFrameListener = NULL;

// =============================================
// CVegRQListener
// =============================================
void CVegRQListener::Init()
{		
	if(!CVegFrameListener::s_pVegFrameListener)
	{	
		CVegFrameListener::s_pVegFrameListener = new CVegFrameListener();
		Ogre::Root::getSingleton().addFrameListener(CVegFrameListener::s_pVegFrameListener);
	}		
}

// ---------------------------------------------
void CVegRQListener::Destroy()
{
	Ogre::Root::getSingleton().removeFrameListener(CVegFrameListener::s_pVegFrameListener);
	SAFE_DELETE(CVegFrameListener::s_pVegFrameListener)
}

// ---------------------------------------------
void CVegRQListener::renderQueueStarted(Ogre::RenderQueueGroupID id, bool& skipThisQueue)
{
	if ((m_post_queue) && m_queue_id == id)
	{
#	ifdef GAME_DEBUG
	#ifdef __OTE

		if(!COTETilePage::s_CurrentPage || !COTETilePage::s_CurrentPage->IsVisible())
			return;

	#   endif
#   endif

		// 视图矩阵 根据D3D 文档给的方法转化		

		D3DXMATRIX* vpmat = COTED3DManager::GetD3DViewPrjMatrix(SCENE_CAM);

		COTEGrass::GetSingleton()->RenderGrass(vpmat);
	}

}
// ---------------------------------------------
void CVegRQListener::renderQueueEnded(Ogre::RenderQueueGroupID id, bool& repeatThisQueue)
{
	if ((m_post_queue) && (m_queue_id == id))
	{
	}
}
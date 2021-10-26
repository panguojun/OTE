#pragma once
#include "OgreD3D9RenderSystem.h" 
#include "OgreD3D9Texture.h"
#include "OTETile.h"
#include "OTETilePage.h"
#include "OTEShadowRender.h"

// 水的类型

namespace OTE
{

class COTELiquidManager;

// *****************************************
// 渲染结构
// *****************************************

struct LiquidVertex_t
{
	enum { FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEX2};

	D3DXVECTOR3 pos;
	DWORD       color;
	D3DXVECTOR2 uv;
	D3DXVECTOR2 uv1;
};

struct LiquidPlane_t
{
	LiquidVertex_t vertice[4];	
};

// 水单元

struct CLiquidElm_t
{
	LiquidPlane_t				m_waterPlane;
	bool						m_isInitSucceed;
	DWORD						m_OldTickCount;

	// 池索引

	int							m_PoolIndex;

	COTETile*					m_Tile;

	// 构造函数

	CLiquidElm_t() :
	m_isInitSucceed(false),
	m_PoolIndex(0)
	{
	}
};

// *****************************************
// COTELiquid
// 水的高度不能高于 MAX_WATER_HGT
// 水的高度包含着两个信息 一个是"物理"高度 一个是类型
// 以0 - MAX_WATER_HGT为水的"物理"高度范围
// 而其倍数为水的类型
// *****************************************

#define MAX_WATER_HGT				500
#define GET_WATERTYPE(val)			(int(val) / MAX_WATER_HGT)
#define GET_WATERHGT(val)			(int(val) % MAX_WATER_HGT)
#define GET_WATERREALHGT(val)		(GET_WATERHGT(val) * (COTETilePage::GetCurrentPage()->m_Scale.y / MAX_WATER_HGT))

#define MAKE_WATERVAL(type, hgt)	(type * MAX_WATER_HGT + int(hgt * (MAX_WATER_HGT / COTETilePage::GetCurrentPage()->m_Scale.y)))


class CLiquidRender;

class _OTEExport COTELiquid 
{
friend class COTELiquidManager;
friend class COTELakeLiquidRender;
friend class COTEOceanWaterRender;

public:

	// 水的类型

	enum 
	{
		eOcean_water = 0, 
		eLake_Water	 = 1
	};

	void Update(COTETile* pTile);
	
	bool AddLiquidPlane(COTETile* pTile, int waterValue);	

	void EraseLiquidPlane(COTETile* pTile);

	void Clear();

	void SetLiquidPlaneVisible(bool vis)
	{
		m_LiquidPlaneVisible = vis;
	}

	bool IsLiquidPlaneVisible()
	{
		return m_LiquidPlaneVisible;
	}	

	CLiquidRender* GetRender()
	{
		return m_Render;
	}

protected:	
	
	COTELiquid(CLiquidRender* render);

	~COTELiquid(void);	
	
	//　水元素列表

	HashMap<COTETile*, CLiquidElm_t*>			m_LiquidElms;

	bool										m_LiquidPlaneVisible;

	CLiquidRender*								m_Render;

	float										m_Height;

};

// *****************************************
// CLiquidRender
// 水渲染的基类
// *****************************************

class _OTEExport CLiquidRender
{
friend class COTELiquid;
public:

	virtual void Render() = 0;

	virtual void Update(CLiquidElm_t*) = 0;

	virtual bool Init(COTELiquid* lq) = 0;

	virtual void InvalidateDevice() = 0;

	virtual void Clear(bool invalidDevice = false);

public:

	bool CreateIndexBuffer(int maxPlanes);

	bool CreateVertexBuffer(int maxPlanes);
	
	void SetHeightMap(const Ogre::String& rHeightMapName);

	bool HasUpdate()
	{
		return m_PoolCount > 0;
	}

protected:

	/// hardware buffers

	Ogre::HardwareVertexBufferSharedPtr			m_VertexBuffer;

	LPDIRECT3DVERTEXBUFFER9						m_waterVertexBuffer;

	Ogre::HardwareIndexBufferSharedPtr			m_IndexBuffer;

	LPDIRECT3DINDEXBUFFER9						m_waterIndexBuffer;

	/// texture

	Ogre::D3D9TexturePtr						m_waterTexture;

	LPD3DXEFFECT								m_pEffect;
										
	Ogre::D3D9TexturePtr						m_HeightMap;
	
	COTELiquid*									m_BindLiquid;

	int											m_PoolCount;
												
	bool										m_VertexBufferPoolLocked;
												
	char*										m_pVBuf;	

public:

	CLiquidRender();
	virtual ~CLiquidRender(){	Clear();	}
};

// *****************************************
// COTELakeLiquidRender
// 用于 湖水的渲染
// *****************************************
class _OTEExport COTELakeLiquidRender : public CLiquidRender
{
friend class					COTELiquid;
friend class					COTELiquidManager;
public:
	static const int			c_MaxLiquidPlanes;		// 最大水片数量

public:

	// 成员函数

	bool Init(COTELiquid* lq);

	void Render();

	void Clear(bool invalidDevice = false);	

	void Update(CLiquidElm_t* le);	

	void InvalidateDevice();	

	/// Get/Set Operations

	DWORD GetLiquidColor()
	{
		return m_Color;
	}

	void SetLiquidColor(DWORD color)
	{
		m_Color = color;
	}

	// 环境贴图 （反光）

	void SetEnviMap(float hgt, const Ogre::String& rEnviMapName = "");

	void SetBaseTexture(const std::string& rBaseTextureName);

	// 渲染回调

	static void OnBeforeRenderLiquidRT(int cam);

protected:

	bool CreateD3DRes();

	COTELakeLiquidRender();
	virtual ~COTELakeLiquidRender();

protected:

	/// 属性 

	DWORD										m_Color;			// 水体颜色	

	/// textures

	Ogre::D3D9TexturePtr						m_EnviMap;

	COTERenderTexture*							m_RTexture;

};

// *****************************************
// COTEOceanWaterRender
// 用于 海洋的渲染
// *****************************************
class _OTEExport COTEOceanWaterRender : public CLiquidRender
{
friend class					COTELiquid;
friend class					COTELiquidManager;
public:
	static const int			c_LiquidFrames;			// 最多水片
	static const int			c_MaxLiquidPlanes;		// 最大水片数量

public:

	// 成员函数

	bool Init(COTELiquid* lq);

	void Render();

	void Clear(bool invalidDevice = false);	

	void Update(CLiquidElm_t* le);

	void InvalidateDevice();	

protected:

	bool CreateD3DRes();

	COTEOceanWaterRender();
	virtual ~COTEOceanWaterRender()
	{
		Clear();
	}

protected:

	/// textures										

	// base map

	std::vector<Ogre::D3D9TexturePtr>			m_waterTextures;

	// normal map

	//std::vector<Ogre::D3D9TexturePtr>			m_waterNormalTextures;

	//Ogre::D3D9TexturePtr						m_waterNormalTexture;
										
};

// *****************************************
// COTELiquidManager
// *****************************************

class _OTEExport COTELiquidManager
{
friend class COTELiquid;
friend class CLiquidRQListener;
public:

	/// 单体

	static COTELiquidManager* GetSingleton();

	static void DestroySingleton();	

public:

	bool Init();

	void Clear();	

	void SetLiquidAt(float x, float z, unsigned int waterValue);

	void LoadLiquidFromFile(const std::string& rFileName);

	void SaveLiquidToFile(const std::string& rFileName);

	void CreateEmptyFile(const std::string& rFileName, int sizeX, int sizeZ);
	
	bool GetLiquidValue( unsigned int &waterValue, COTETile* tile );

	bool GetLiquidValue( unsigned int &waterValue, float x , float z );	

	COTELiquid* GetLiquid(int waterValue);

private:

	COTELiquidManager();

	COTELiquid* CreateLiquid(int height);

	void InvalidateDevice();

	void Render();

public:

	HashMap<int, COTELiquid*>			m_LiquidMap;

private:

	static	COTELiquidManager*			s_pSingleton;		// 单体	

	unsigned int*						m_pLiquidMap;

};


}


// 宏
// *****************************************

#define		WATER_MGR			COTELiquidManager::GetSingleton()

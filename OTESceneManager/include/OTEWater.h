#pragma once
#include "OgreD3D9RenderSystem.h" 
#include "OgreD3D9Texture.h"
#include "OTETile.h"
#include "OTETilePage.h"
#include "OTEShadowRender.h"

// ˮ������

namespace OTE
{

class COTELiquidManager;

// *****************************************
// ��Ⱦ�ṹ
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

// ˮ��Ԫ

struct CLiquidElm_t
{
	LiquidPlane_t				m_waterPlane;
	bool						m_isInitSucceed;
	DWORD						m_OldTickCount;

	// ������

	int							m_PoolIndex;

	COTETile*					m_Tile;

	// ���캯��

	CLiquidElm_t() :
	m_isInitSucceed(false),
	m_PoolIndex(0)
	{
	}
};

// *****************************************
// COTELiquid
// ˮ�ĸ߶Ȳ��ܸ��� MAX_WATER_HGT
// ˮ�ĸ߶Ȱ�����������Ϣ һ����"����"�߶� һ��������
// ��0 - MAX_WATER_HGTΪˮ��"����"�߶ȷ�Χ
// ���䱶��Ϊˮ������
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

	// ˮ������

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
	
	//��ˮԪ���б�

	HashMap<COTETile*, CLiquidElm_t*>			m_LiquidElms;

	bool										m_LiquidPlaneVisible;

	CLiquidRender*								m_Render;

	float										m_Height;

};

// *****************************************
// CLiquidRender
// ˮ��Ⱦ�Ļ���
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
// ���� ��ˮ����Ⱦ
// *****************************************
class _OTEExport COTELakeLiquidRender : public CLiquidRender
{
friend class					COTELiquid;
friend class					COTELiquidManager;
public:
	static const int			c_MaxLiquidPlanes;		// ���ˮƬ����

public:

	// ��Ա����

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

	// ������ͼ �����⣩

	void SetEnviMap(float hgt, const Ogre::String& rEnviMapName = "");

	void SetBaseTexture(const std::string& rBaseTextureName);

	// ��Ⱦ�ص�

	static void OnBeforeRenderLiquidRT(int cam);

protected:

	bool CreateD3DRes();

	COTELakeLiquidRender();
	virtual ~COTELakeLiquidRender();

protected:

	/// ���� 

	DWORD										m_Color;			// ˮ����ɫ	

	/// textures

	Ogre::D3D9TexturePtr						m_EnviMap;

	COTERenderTexture*							m_RTexture;

};

// *****************************************
// COTEOceanWaterRender
// ���� �������Ⱦ
// *****************************************
class _OTEExport COTEOceanWaterRender : public CLiquidRender
{
friend class					COTELiquid;
friend class					COTELiquidManager;
public:
	static const int			c_LiquidFrames;			// ���ˮƬ
	static const int			c_MaxLiquidPlanes;		// ���ˮƬ����

public:

	// ��Ա����

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

	/// ����

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

	static	COTELiquidManager*			s_pSingleton;		// ����	

	unsigned int*						m_pLiquidMap;

};


}


// ��
// *****************************************

#define		WATER_MGR			COTELiquidManager::GetSingleton()

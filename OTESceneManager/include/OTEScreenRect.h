#pragma once
#include "OTEStaticInclude.h"
#include "OgreD3D9RenderSystem.h" 
#include "OgreD3D9HardwareVertexBuffer.h" 
#include "OgreD3D9HardwareIndexBuffer.h" 
#include "OgreD3D9Texture.h" 

namespace OTE
{
// ********************************************
// 用于屏幕2D渲染用的顶点
// ********************************************

struct Vertex2D_t
{
	enum { FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1 };

	D3DXVECTOR3 pos;
	DWORD       color;
	D3DXVECTOR2 uv;
};

// ********************************************
// 屏幕2D矩形单元
// ********************************************

// 2D片

class CRect2DElm_t
{
public:	

	// x, y 都是在 0 - 1 范围内变化  

	// 0(x1,y1)   ------   1(x2,y1)
	//    |					 |
	//    |                  |
	// 2(x1,y2)   ------   3(x2,y2)

	CRect2DElm_t( float x1, float y1, 
				  float x2, float y2,
				  const Ogre::ColourValue& color) 
				  : m_isInitSucceed(false)
	{
		// 单元
		
		m_isInitSucceed = true;

		m_Plane.RectVertices[0].pos[0]	= x1;
		m_Plane.RectVertices[0].pos[1]	= 0.0f;
		m_Plane.RectVertices[0].pos[2]	= y1;
		m_Plane.RectVertices[0].uv.x	= 0.0f;
		m_Plane.RectVertices[0].uv.y	= 0.0f;

		m_Plane.RectVertices[1].pos[0]  = x1;
		m_Plane.RectVertices[1].pos[1]	= 0.0f;
		m_Plane.RectVertices[1].pos[2]	= y2;
		m_Plane.RectVertices[1].uv.x	= 0.0f;
		m_Plane.RectVertices[1].uv.y	= 1.0f;

		m_Plane.RectVertices[2].pos[0]	= x2;
		m_Plane.RectVertices[2].pos[1]	= 0.0f;
		m_Plane.RectVertices[2].pos[2]	= y2;
		m_Plane.RectVertices[2].uv.x	= 1.0f;
		m_Plane.RectVertices[2].uv.y	= 1.0f;

		m_Plane.RectVertices[3].pos[0] = x2;
		m_Plane.RectVertices[3].pos[1] = 0.0f;
		m_Plane.RectVertices[3].pos[2] = y1;
		m_Plane.RectVertices[3].uv.x = 1.0f;
		m_Plane.RectVertices[3].uv.y = 0.0f;

	}

public:	

	struct Rect2DPlane_t {Vertex2D_t RectVertices[4];};

	// 平面 

	Rect2DPlane_t				m_Plane;

	bool						m_isInitSucceed;

	// 池索引

	int							m_PoolIndex;

};


// ********************************************
// COTERect2DRender
// 2D矩形渲染器
// ********************************************
class _OTEExport COTERect2DRender
{
public:

	/// 单体

	static COTERect2DRender* GetSingleton();

	static void DestroySingleton();	

public:
	// 创建

	void CreateRect(const std::string& rName,
					float x1, float y1, 
					float x2, float y2,
					const Ogre::ColourValue& color
					);
	
	// 删除 

	void DeleteRect(const std::string& rName);

	// 初始化 

	void Init();

	// 渲染

	void Render();

	// 摧毁 

	void Destroy();

	// 清理

	void Clear();

	// 更新

	void Update(CRect2DElm_t* we);


protected:

	// 状态类型

	enum eInitState{E_UNINIT, E_SUCCESS, E_FAILED, E_LOSTDEVICE};

	eInitState										m_State;	

	// 创建缓存 

	bool CreateIndexBuffer(int maxPlanes);

	// 无效设备

	void InvalidateDevice();

private:

	COTERect2DRender();

public:

	~COTERect2DRender();

private:									

	// 最多片数量  

	static	int									c_MaxPlanes;		// 最大片数量

	static COTERect2DRender*					s_pSingleton;		// 单体

	// 2D方形列表

	std::map<std::string, CRect2DElm_t*>		m_Rect2DList;

	Ogre::HardwareIndexBufferSharedPtr			m_IndexBuffer;

	LPDIRECT3DINDEXBUFFER9						m_D3D9IndexBuffer;
	
	Ogre::HardwareVertexBufferSharedPtr			m_VertexBuffer;
	
	LPDIRECT3DVERTEXBUFFER9						m_D3D9VertexBuffer;

	Ogre::D3D9TexturePtr						m_Texture;

	int											m_PoolCount;
												
	bool										m_VertexBufferPoolLocked;

	char*										m_pVBuf;

};

extern class COTE2DRenderListener;

}
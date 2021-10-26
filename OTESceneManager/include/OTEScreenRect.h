#pragma once
#include "OTEStaticInclude.h"
#include "OgreD3D9RenderSystem.h" 
#include "OgreD3D9HardwareVertexBuffer.h" 
#include "OgreD3D9HardwareIndexBuffer.h" 
#include "OgreD3D9Texture.h" 

namespace OTE
{
// ********************************************
// ������Ļ2D��Ⱦ�õĶ���
// ********************************************

struct Vertex2D_t
{
	enum { FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1 };

	D3DXVECTOR3 pos;
	DWORD       color;
	D3DXVECTOR2 uv;
};

// ********************************************
// ��Ļ2D���ε�Ԫ
// ********************************************

// 2DƬ

class CRect2DElm_t
{
public:	

	// x, y ������ 0 - 1 ��Χ�ڱ仯  

	// 0(x1,y1)   ------   1(x2,y1)
	//    |					 |
	//    |                  |
	// 2(x1,y2)   ------   3(x2,y2)

	CRect2DElm_t( float x1, float y1, 
				  float x2, float y2,
				  const Ogre::ColourValue& color) 
				  : m_isInitSucceed(false)
	{
		// ��Ԫ
		
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

	// ƽ�� 

	Rect2DPlane_t				m_Plane;

	bool						m_isInitSucceed;

	// ������

	int							m_PoolIndex;

};


// ********************************************
// COTERect2DRender
// 2D������Ⱦ��
// ********************************************
class _OTEExport COTERect2DRender
{
public:

	/// ����

	static COTERect2DRender* GetSingleton();

	static void DestroySingleton();	

public:
	// ����

	void CreateRect(const std::string& rName,
					float x1, float y1, 
					float x2, float y2,
					const Ogre::ColourValue& color
					);
	
	// ɾ�� 

	void DeleteRect(const std::string& rName);

	// ��ʼ�� 

	void Init();

	// ��Ⱦ

	void Render();

	// �ݻ� 

	void Destroy();

	// ����

	void Clear();

	// ����

	void Update(CRect2DElm_t* we);


protected:

	// ״̬����

	enum eInitState{E_UNINIT, E_SUCCESS, E_FAILED, E_LOSTDEVICE};

	eInitState										m_State;	

	// �������� 

	bool CreateIndexBuffer(int maxPlanes);

	// ��Ч�豸

	void InvalidateDevice();

private:

	COTERect2DRender();

public:

	~COTERect2DRender();

private:									

	// ���Ƭ����  

	static	int									c_MaxPlanes;		// ���Ƭ����

	static COTERect2DRender*					s_pSingleton;		// ����

	// 2D�����б�

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
#pragma once
#include "OTEStaticInclude.h"
#include "OTETile.h"

namespace OTE
{

// *****************************************
// 渲染结构
// *****************************************

// 顶点

struct SBWVertex_t
{
	enum {FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1};

	D3DXVECTOR3		pos;
	DWORD			color;
	D3DXVECTOR2		uv;
};

#define		SBW_NUM_TRIS		5			// 三角形数量
#define		SBW_NUM_VERTS		7			// 顶点数量(SBW_NUM_TRIS + 2)

// 面单元

struct SBWPlane_t	
{
	SBWVertex_t vertice[SBW_NUM_VERTS];		
};

// 元素

struct SBWElm_t	
{
	SBWVertex_t vertice[SBW_NUM_VERTS];		
};

// ******************************************
// COTESeaboardWave
// 海岸波浪的数据对象类
// ******************************************
class _OTEExport COTESeaboardWave
{
public:

	COTESeaboardWave();

	~COTESeaboardWave();

public:

	void Render();

	void Update();

	bool Init();

	void InvalidateDevice();

	void Clear(bool invalidDevice = false);

protected:

	bool CreateVertexBuffer(int maxPlanes);

protected:

	// 面一级的

	typedef	std::vector<SBWPlane_t*>			SBWPlaneList_t;
	static HashMap<COTETile*, SBWPlaneList_t>	s_BWPlanes;

	/// hardware buffers

	static Ogre::HardwareVertexBufferSharedPtr	s_VertexBuffer;

	static LPDIRECT3DVERTEXBUFFER9				s_pD3DVertexBuffer;

	/// texture

	static Ogre::D3D9TexturePtr					s_Texture;

	static LPD3DXEFFECT							s_pEffect;

	static int									s_PoolCount;
	

};

// ******************************************
// COTESeaboardWaveRender
// 海岸波浪的渲染类
// ******************************************
class COTESeaboardWaveRender
{
public:

	static void RenderSeaboardWaves();

protected:



};

// ******************************************
// 鼠标操作的标示

#define		AUTOLINK_OPER_ADD			0xF0000000		// 添加操作
#define		AUTOLINK_RAISE				0x00F00000		// 升高
#define		AUTOLINK_UPDATETERRAIN		0x0000000F		// 更新山体

// ******************************************
// COTESeaboardLine
// ******************************************

class _OTEExport COTESeaboardLine
{
public:	
	
	// 自动连接调用接口

	static void AutoLink(float X, float Z, unsigned int operFlag);

protected:	

	static void AutoLink(int uX, int uZ, int uIndex, int vIndex, short cornerType, unsigned int operFlag);

	static void UpdateEntities(int uX, int uZ, int u, int v, unsigned int operFlag);

	static void GetWorldPosition(int uX, int uZ, Ogre::Vector3& position);

	static Ogre::Entity* GetALEntityAtPoint(int uX, int uZ, int& u, int& v);

	static void UpdateBorder(int uX, int uZ, int offsetX, int offsetZ, unsigned int operFlag);	
	
};


}

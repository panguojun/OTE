#pragma once
#include "OTEStaticInclude.h"
#include "OTETile.h"

namespace OTE
{

// *****************************************
// ��Ⱦ�ṹ
// *****************************************

// ����

struct SBWVertex_t
{
	enum {FVF = D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1};

	D3DXVECTOR3		pos;
	DWORD			color;
	D3DXVECTOR2		uv;
};

#define		SBW_NUM_TRIS		5			// ����������
#define		SBW_NUM_VERTS		7			// ��������(SBW_NUM_TRIS + 2)

// �浥Ԫ

struct SBWPlane_t	
{
	SBWVertex_t vertice[SBW_NUM_VERTS];		
};

// Ԫ��

struct SBWElm_t	
{
	SBWVertex_t vertice[SBW_NUM_VERTS];		
};

// ******************************************
// COTESeaboardWave
// �������˵����ݶ�����
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

	// ��һ����

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
// �������˵���Ⱦ��
// ******************************************
class COTESeaboardWaveRender
{
public:

	static void RenderSeaboardWaves();

protected:



};

// ******************************************
// �������ı�ʾ

#define		AUTOLINK_OPER_ADD			0xF0000000		// ��Ӳ���
#define		AUTOLINK_RAISE				0x00F00000		// ����
#define		AUTOLINK_UPDATETERRAIN		0x0000000F		// ����ɽ��

// ******************************************
// COTESeaboardLine
// ******************************************

class _OTEExport COTESeaboardLine
{
public:	
	
	// �Զ����ӵ��ýӿ�

	static void AutoLink(float X, float Z, unsigned int operFlag);

protected:	

	static void AutoLink(int uX, int uZ, int uIndex, int vIndex, short cornerType, unsigned int operFlag);

	static void UpdateEntities(int uX, int uZ, int u, int v, unsigned int operFlag);

	static void GetWorldPosition(int uX, int uZ, Ogre::Vector3& position);

	static Ogre::Entity* GetALEntityAtPoint(int uX, int uZ, int& u, int& v);

	static void UpdateBorder(int uX, int uZ, int offsetX, int offsetZ, unsigned int operFlag);	
	
};


}

#pragma once
#include "OTEStaticInclude.h"
#include "OgreD3D9Texture.h"
#include "Ogre.h"

namespace OTE
{
// *********************************************

#define GRASS_GROUP_SIZE			100			// һ��ݵ�����

// *********************************************
// SingleGrassVertex
// *********************************************

struct SingleGrassVertex
{
	D3DXVECTOR3		Pos;
	unsigned char	Index;
	unsigned char	Lum;

	void SetGrass(const void* pos, unsigned char grassID, unsigned char lum);

};

// *********************************************
// CGrassGroup
// ����Ⱦ��Ԫ���ݽṹ
// *********************************************

class COTEGrass;

class CGrassGroup
{
friend class COTEGrass;

public:
	static const int					c_ElmCount;	// �����"����"������

public:

	D3DXVECTOR4							m_ShaderData[GRASS_GROUP_SIZE];

	LPDIRECT3DVERTEXBUFFER9				m_GrassVertexBuffer;
	
	Ogre::HardwareVertexBufferSharedPtr m_MainBuf;

	float*								m_LockPtr;
	
	int									m_GroupIndex;

	// ����ߴ�

	Ogre::Vector3						m_BoundMin;

	Ogre::Vector3						m_BoundMax;

public:
	CGrassGroup(int groupIndex, COTEGrass* parentGrass)
		: m_GrassVertexBuffer(NULL),
		  m_GroupIndex(groupIndex),
		  m_ParentGrass(parentGrass),
		  m_BoundMin( MAX_FLOAT,  MAX_FLOAT,  MAX_FLOAT),
		  m_BoundMax(-MAX_FLOAT, -MAX_FLOAT, -MAX_FLOAT)
	{	
		InitBuffer();	
	}	
	~CGrassGroup() 
	{
		m_MainBuf.setNull();
	}

private:

	// ��ʼ������ 

	bool  InitBuffer();	

	// ���û���

	void GenVertices(	const Ogre::Vector3& position, 
						const Ogre::Radian& rotation,
						int groupIndex,
						int IndexInGroup);

	void BeginBillboards(void);

	void EndBillboards(void);


private:

	COTEGrass*		m_ParentGrass;

};

// *****************************************
// COTEGrassRenderBase
// ��Ⱦ��
// *****************************************

class COTEGrassRenderBase
{
public:
	
	virtual bool CreateEffect(COTEGrass* pGrass){ return false;}

	virtual void RenderGrass(void* pViewProjMat, COTEGrass* pGrass) = 0;
};


// *****************************************
// COTEGrassCPURender
// *****************************************

class COTEGrassCPURender : public COTEGrassRenderBase
{
public:

	virtual bool CreateEffect(COTEGrass* pGrass);

	virtual void RenderGrass(void* pViewProjMat, COTEGrass* pGrass);

};

// *****************************************
// COTEGrassGPURender
// *****************************************

class COTEGrassGPURender : public COTEGrassRenderBase
{
public:

	virtual bool CreateEffect(COTEGrass* pGrass);

	virtual void RenderGrass(void* pViewProjMat, COTEGrass* pGrass);

};

// *****************************************
// COTEGrass
// �ӿ���
// *****************************************

class COTEGrass
{
friend class COTEVegManager;
friend class CVegFrameListener;
friend class CVegRQListener;
friend class COTETile;

friend class CGrassGroup;
friend class COTEGrassCPURender;
friend class COTEGrassGPURender;
friend class COTERender;

public:

	/// ����

	static COTEGrass* GetSingleton();

	static void DestroySingleton();	

	// ���ýӿ�

	void AddGrass(const SingleGrassVertex* pRenderGrassList, int grassListCount); 

	void RenderGrass(void* pViewProjMat);

	const Ogre::String& GetTextureName();	

	void SetTexture(const Ogre::String& rTextureName);

	float GetMaxSquredDistance(){return m_MaxSquredDistance;}

	float SetMaxSquredDistance(float maxSquredDistance){m_MaxSquredDistance = maxSquredDistance;}

	// �رղ���Ⱦ

	void CloseGrassRendering(){m_GrassState = E_GRASSINIT_FAIL;}

	// ����

	void ClearRenderContents();

protected:

	COTEGrass();

	~COTEGrass();	
	
	bool CreateIndexBuffer();

	void CreateGrassShape(CGrassGroup* group, int index);

	// for lost device 

	void InvalidateDevice();

	void RestoreDevice();

	void InitGrass();
	
	void Clear();

protected:

	// ��ʼ���������

	enum eGrassState { E_GRASSINIT_UNINIT, E_GRASSINIT_SUCCESS, E_GRASSINIT_FAIL, E_LOSTDEVICE };	

	eGrassState					m_GrassState;

	CGrassGroup*				m_pCurGrass;

	std::vector<CGrassGroup*>	m_GrassGroups;	
	
	LPDIRECT3DINDEXBUFFER9		m_GrassIDBuffer;
	
	LPD3DXEFFECT                m_pEffect;

	int							m_GrassCount;

	bool						m_DeviceLosted;

	Ogre::D3D9TexturePtr		m_GrassTexture;

	COTEGrassRenderBase*		m_GrassRender;

	float						m_MaxSquredDistance;	// ��Զ�ɼ�����

private:

	static COTEGrass*			s_pSingleton;			// ����	
};

}

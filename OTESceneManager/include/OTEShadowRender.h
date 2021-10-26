#pragma once
#include "OTEStaticInclude.h"
#include "OgreRenderTexture.h"
#include "OTED3DManager.h"
#include "OTEActorEntity.h"
#include "OTETile.h"

namespace OTE
{

// ***************************************
// COTERenderTexture
// ��Ⱦ��ͼ, һ��ͨ�õ���Ⱦ����
// ***************************************

class _OTEExport COTERenderTexture
{
public:		
	
	static COTERenderTexture* Create(const std::string& rTexName, 
		int width, int height, Ogre::Camera* cam = NULL, Ogre::PixelFormat PF = Ogre::PF_A8R8G8B8);

	static void Remove(COTERenderTexture* rt);

	static void Destory();

public:

	void BindObj(COTEEntity* e);

	Ogre::TexturePtr GetMMTexture();

	void EnableFrameUpdate(bool update = true);

protected:

	COTERenderTexture();
	~COTERenderTexture();

public:	

	Ogre::Camera*			m_pCamera;			// ������Ⱦ�������

	Ogre::RenderTexture*	m_RenderTexture;	// ��ȾͼƬ

protected:

	// ����
	
	static std::vector<COTERenderTexture*>		m_RenderTextureList;
	
	// Ĭ�������

	static Ogre::Camera*	s_CommonCamra;
	
	// ϵͳ�ڴ��е���ͼ����

	Ogre::TexturePtr		m_MMTexCach;
};

// ***************************************
// COTEShadowMap
// ShadowMap��Ӱ����
// ***************************************

class _OTEExport COTEShadowMap
{
public:
	enum eAlignment{eFIRST_OBJ, eSCENE_CENTER};

public:

	// init / destroy

	static void Init();

	static void Destroy();

	// �����Ӱ�ӵ�����

	static void AddShadowObj(COTEActorEntity* ae);	

	static void RemoveShadowObj(COTEActorEntity* ae);	

	// ���Ӱ��Ͷ���������

	static void AddShadowReceiverObj(COTEActorEntity* ae);	

	static void RemoveShadowReceiverObj(COTEActorEntity* ae);	

	// ���浽�ļ�

	static void SaveMap(const std::string& fileName);

	static void SetAlignment(eAlignment alignment);

protected:

	// ��Դ

	static bool CreateRes();

	static void ClearRes();	

	// ��ȾӰ��Ͷ�����	

	static void SetRenderStates();

	static void ResetRenderStates();

	static void UpdateTerrain();

	static void RenderShadowTerrain();

	static void RenderShadowReceiverObjs();

private:

	// �¼���Ӧ

	static void OnRenderShadowReceivers(int cam);

	static void OnBeforeRenderShadowMap(int cam);

	static void OnEndRenderShadowMap(int cam);

	static void OnBeforeRenderShadowObj(COTEActorEntity* ae);

	static void OnEndRenderShadowObj(COTEActorEntity* ae);

	static void OnBeforeEntityDestroy(const CEventObjBase::EventArg_t& arg);

	static void OnBeforeReceiverEntityDestroy(const CEventObjBase::EventArg_t& arg);

protected:

	static const int						c_ShadowMapSize;	

	static COTERenderTexture*				s_RenderTexture;

	static std::vector<COTEActorEntity*>	s_ShadowObjList;

	static LPD3DXEFFECT						s_pSMapEffect;

	static std::list<COTETile*>				s_RenderTileList;

	static std::vector<COTEActorEntity*>	s_RenderObjList;

	static std::string						s_MapFileName;

	static eAlignment						s_Alignment;

private:

	static bool								s_BeginShadowMapRendering;

	static bool								s_NeedUpdateTerrain;

	static int								s_FrameCount;

};


}
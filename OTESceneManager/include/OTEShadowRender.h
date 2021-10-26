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
// 渲染贴图, 一种通用的渲染技术
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

	Ogre::Camera*			m_pCamera;			// 用于渲染得摄像机

	Ogre::RenderTexture*	m_RenderTexture;	// 渲染图片

protected:

	// 容器
	
	static std::vector<COTERenderTexture*>		m_RenderTextureList;
	
	// 默认摄像机

	static Ogre::Camera*	s_CommonCamra;
	
	// 系统内存中的贴图　　

	Ogre::TexturePtr		m_MMTexCach;
};

// ***************************************
// COTEShadowMap
// ShadowMap阴影技术
// ***************************************

class _OTEExport COTEShadowMap
{
public:
	enum eAlignment{eFIRST_OBJ, eSCENE_CENTER};

public:

	// init / destroy

	static void Init();

	static void Destroy();

	// 添加有影子的物体

	static void AddShadowObj(COTEActorEntity* ae);	

	static void RemoveShadowObj(COTEActorEntity* ae);	

	// 添加影子投射对象物体

	static void AddShadowReceiverObj(COTEActorEntity* ae);	

	static void RemoveShadowReceiverObj(COTEActorEntity* ae);	

	// 保存到文件

	static void SaveMap(const std::string& fileName);

	static void SetAlignment(eAlignment alignment);

protected:

	// 资源

	static bool CreateRes();

	static void ClearRes();	

	// 渲染影子投射对象	

	static void SetRenderStates();

	static void ResetRenderStates();

	static void UpdateTerrain();

	static void RenderShadowTerrain();

	static void RenderShadowReceiverObjs();

private:

	// 事件响应

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
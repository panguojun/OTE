#include "oteshadowrender.h"
#include "OTEQTSceneManager.h"
#include "OTED3DManager.h"
#include "OTESky.h"
#include "OTEFog.h"
#include "OgreD3D9Texture.h"

using namespace Ogre;
using namespace OTE;

// ==============================================
// COTERenderTexture
// 渲染贴图
// ==============================================

std::vector<COTERenderTexture*> COTERenderTexture::m_RenderTextureList;
Ogre::Camera*	COTERenderTexture::s_CommonCamra = NULL;

// ----------------------------------------------
COTERenderTexture* COTERenderTexture::Create(const std::string& rTexName, int width, int height, Ogre::Camera* cam, Ogre::PixelFormat PF)
{
	COTERenderTexture* rt = new COTERenderTexture();

	// Setup Render To Texture for preview window

	rt->m_RenderTexture = Root::getSingleton().getRenderSystem()->createRenderTexture( rTexName, width, height, Ogre::TEX_TYPE_2D, PF );
	{
		if(cam)
		{
			rt->m_pCamera = cam;	
		}
		else
		{
			if(!s_CommonCamra)
			{
				s_CommonCamra = SCENE_MGR->createCamera("RT_CAM");
				Ogre::SceneNode* camNode = 
					SCENE_MGR->getRootSceneNode()->createChildSceneNode("RT_CAM");
				camNode->attachObject(s_CommonCamra);

				s_CommonCamra->setPosition(0.0f, 0.0f, 5.0f);
				s_CommonCamra->lookAt(0.0f, 0.0f, 0.0f);
				s_CommonCamra->setNearClipDistance(SCENE_CAM->getNearClipDistance());
				s_CommonCamra->setFarClipDistance (SCENE_CAM->getFarClipDistance() );	
			}

			rt->m_pCamera = s_CommonCamra;
		}
		
		Ogre::Viewport* pViewport = rt->m_RenderTexture->addViewport( rt->m_pCamera );
		pViewport->setOverlaysEnabled(false);
		pViewport->setClearEveryFrame(true);
		pViewport->setBackgroundColour( Ogre::ColourValue(0, 0, 0, 0) );	

	}	

	m_RenderTextureList.push_back(rt);

	return rt;
}

// ----------------------------------------------
void COTERenderTexture::EnableFrameUpdate(bool update)
{
	if(!m_RenderTexture)
		return;
	m_RenderTexture->getViewport(0)->setClearEveryFrame(update);
}
// ----------------------------------------------
void COTERenderTexture::Remove(COTERenderTexture* rt)
{
	std::vector<COTERenderTexture*>::iterator it = m_RenderTextureList.begin();
	while(it != m_RenderTextureList.end())
	{
		if(*it == rt)
		{
			m_RenderTextureList.erase(it);
			break;
		}
		++ it;
	}
	delete rt;	
}

// ----------------------------------------------
void COTERenderTexture::Destory()
{
	std::vector<COTERenderTexture*>::iterator it = m_RenderTextureList.begin();
	while(it != m_RenderTextureList.end())
	{
		delete *it;
		++ it;
	}	

	if(s_CommonCamra)
	{
		SCENE_MGR->removeCamera(s_CommonCamra);
		s_CommonCamra = NULL;
	}
}

// ----------------------------------------------
// 需要每桢调用
void COTERenderTexture::BindObj(COTEEntity* e)
{
	assert(m_RenderTexture && "没有创建渲染贴图");

	// 渲染这个实体

	COTEEntity::OTESubEntityList::iterator i, iend;
    iend = e->mSubEntityList.end();
    for (i = e->mSubEntityList.begin(); i != iend; ++i)
	{		
		COTESubEntity* se = *i;
		this->m_pCamera->m_RenderableList.push_back((Ogre::Renderable*)se);
	}		
}

// ----------------------------------------------
Ogre::TexturePtr COTERenderTexture::GetMMTexture()
{
	assert(m_RenderTexture && "没有创建渲染贴图");

	if(m_MMTexCach.isNull())
	{
		m_MMTexCach =  Ogre::TextureManager::getSingleton().createManual(
			m_RenderTexture->getName() + "_cach", 
			RES_GROUP,
			Ogre::TEX_TYPE_2D, 
			m_RenderTexture->getWidth(), 
			m_RenderTexture->getHeight(), 
			24, 0, 
			Ogre::PF_A8R8G8B8, TU_SYSTEMMEM);
	}
	m_RenderTexture->_copyToTexture(m_MMTexCach);

	return m_MMTexCach;
}

// ----------------------------------------------
COTERenderTexture::COTERenderTexture() :
m_pCamera(NULL),
m_RenderTexture(NULL)
{	
}

// ----------------------------------------------
COTERenderTexture::~COTERenderTexture()
{
	if(m_RenderTexture)
	{
		Root::getSingleton().getRenderSystem()->destroyRenderTexture(m_RenderTexture->getName());		
	}

	if(m_pCamera && m_pCamera != s_CommonCamra)
	{
		SCENE_MGR->removeCamera(m_pCamera);
	}
}

// ==============================================
// COTEShadowMap
// ShadowMap 技术实现
// ==============================================

const int							COTEShadowMap::c_ShadowMapSize = 512;	// 场景光照图的尺寸

COTERenderTexture*					COTEShadowMap::s_RenderTexture = NULL;

std::vector<COTEActorEntity*>		COTEShadowMap::s_ShadowObjList;

LPD3DXEFFECT						COTEShadowMap::s_pSMapEffect = NULL;

bool								COTEShadowMap::s_BeginShadowMapRendering = false;

bool								COTEShadowMap::s_NeedUpdateTerrain = false;

std::list<COTETile*>				COTEShadowMap::s_RenderTileList;

std::vector<COTEActorEntity*>		COTEShadowMap::s_RenderObjList;

std::string							COTEShadowMap::s_MapFileName;

COTEShadowMap::eAlignment			COTEShadowMap::s_Alignment = eFIRST_OBJ;

int									COTEShadowMap::s_FrameCount = 0;

// ----------------------------------------------
// 渲染阴影贴图前后
void COTEShadowMap::OnBeforeRenderShadowMap(int cam)
{	
	if(s_ShadowObjList.empty())
		return;

	if(!s_RenderTexture || !s_RenderTexture->m_RenderTexture->getViewport(0)->getClearEveryFrame())
		return;

	if(int(s_RenderTexture->m_pCamera) == cam)
	{	
		s_BeginShadowMapRendering = true;			

		// 用列表中第一个作为中心 方向与光源方向一致

		if(s_Alignment == eFIRST_OBJ)
		{
			COTEActorEntity* ae = (*s_ShadowObjList.begin());

			const Ogre::Vector3& lgtdir = SCENE_MAINLIGHT->getDirection();
			s_RenderTexture->m_pCamera->setPosition(
				ae->getParentSceneNode()->getPosition() - lgtdir * 250.0f);

			s_RenderTexture->m_pCamera->setDirection(lgtdir);
		}
		else if(s_Alignment == eSCENE_CENTER)
		{
			const Ogre::Vector3& lgtdir = SCENE_MAINLIGHT->getDirection();
			s_RenderTexture->m_pCamera->setPosition(
				Ogre::Vector3(
				COTETilePage::s_CurrentPage->m_PageSizeX * COTETilePage::s_CurrentPage->m_Scale.x / 2.0f,
				COTETilePage::s_CurrentPage->m_Scale.y / 2.0f,
				COTETilePage::s_CurrentPage->m_PageSizeZ * COTETilePage::s_CurrentPage->m_Scale.z / 2.0f)
				- lgtdir * 250.0f
				);

			s_RenderTexture->m_pCamera->setDirection(lgtdir);			
		}

		// 把影子物体邦定到摄像机

		std::vector<COTEActorEntity*>::iterator it = s_ShadowObjList.begin();
		while(it != s_ShadowObjList.end())
		{		
			s_RenderTexture->BindObj(*it ++);		
		}

		SCENE_MGR->GetRenderSystem()->_disableTextureUnitsFrom(0);	
		g_pDevice->SetPixelShader(0);
		g_pDevice->SetVertexShader(0);

		g_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, true );
		g_pDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );	
		RENDERSYSTEM->_setAlphaRejectSettings(Ogre::CMPF_GREATER, 180);

		g_pDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		g_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	
	}
}

// ----------------------------------------------
void COTEShadowMap::OnEndRenderShadowMap(int cam)
{
	if(!s_RenderTexture || !s_RenderTexture->m_RenderTexture->getViewport(0)->getClearEveryFrame())
		return;

	if(int(s_RenderTexture->m_pCamera) == cam)
	{			
		if(++ s_FrameCount == 10 && !s_MapFileName.empty())
		{
			((Ogre::D3D9Texture*)s_RenderTexture->GetMMTexture().getPointer())->writeContentsToFile(s_MapFileName);
			s_MapFileName = "";
		}		

		s_BeginShadowMapRendering = false;	
		COTEFog::GetInstance()->SetFogEnable(true);		
		
	}
}

// ----------------------------------------------
// 在渲染阴影贴图时 设置渲染效果

void COTEShadowMap::OnBeforeRenderShadowObj(COTEActorEntity* ae)
{
	if(s_pSMapEffect && s_BeginShadowMapRendering)
	{
		COTEFog::GetInstance()->SetFogEnable(false);

		UINT pass;	
		s_pSMapEffect->Begin(&pass, D3DXFX_DONOTSAVESTATE);		 
		s_pSMapEffect->BeginPass(0);
	}
}

// ----------------------------------------------
void COTEShadowMap::OnEndRenderShadowObj(COTEActorEntity* ae)
{
	if(s_pSMapEffect && s_BeginShadowMapRendering)
	{
		s_pSMapEffect->EndPass();		 
		s_pSMapEffect->End();
	}	
}

// ----------------------------------------------
// 删除物件之前从影子列表中删除该物件

void COTEShadowMap::OnBeforeEntityDestroy(const CEventObjBase::EventArg_t& arg)
{
	std::vector<COTEActorEntity*>::iterator it = s_ShadowObjList.begin();
	while(it != s_ShadowObjList.end())
	{
		if((*it)->getName() == arg.strObjName)
		{
			s_ShadowObjList.erase(it);
			break;
		}			
		++ it;
	}

	if(s_RenderTexture && s_RenderTexture->m_pCamera)
		s_RenderTexture->m_pCamera->m_RenderableList.clear();

	// 列表为空

	if(s_ShadowObjList.empty())
	{
		s_RenderTexture->m_RenderTexture->getViewport(0)->setClearEveryFrame(false);
	}

}

// ----------------------------------------------
void COTEShadowMap::RemoveShadowObj(COTEActorEntity* ae)
{
	if(!ae)
		return;

	CEventObjBase::EventArg_t arg;
	arg.strObjName = ae->GetName();

	OnBeforeEntityDestroy(arg);
}


//////////////////////////////////////////////////
//			渲染影子投射对象
//////////////////////////////////////////////////

void COTEShadowMap::SetRenderStates()
{
	if(!s_RenderTexture)
		return;

	// 设置世界矩阵

	D3DXMATRIX matrix;
	D3DXMatrixIdentity(&matrix);

	g_pDevice->SetTransform(D3DTS_WORLD,	&matrix);

	// 得到视图矩阵
		
	D3DXMATRIX viewMat;
	g_pDevice->GetTransform(D3DTS_VIEW,		&viewMat); 
	
	// 得到光源摄像机视图矩阵

	D3DXMATRIX* prjMatD3D = COTED3DManager::GetD3DViewPrjMatrix(
									s_RenderTexture->m_pCamera, true);

	COTED3DManager::_notifyUpdater();

	// 用于纠正位置

	static D3DXMATRIX mat;		
	if(mat._11 != 0.5f)
	{
		mat._11 = 0.50f ; mat._12 = 0.00f ; mat._13 = 0.00f; mat._14 = 0.00f;
		mat._21 = 0.00f ; mat._22 =-0.50f ; mat._23 = 0.00f; mat._24 = 0.00f;
		mat._31 = 0.00f ; mat._32 = 0.00f ; mat._33 = 1.00f; mat._34 = 0.00f;
		mat._41 = 0.50f ; mat._42 = 0.50f ; mat._43 = 0.00f; mat._44 = 1.00f;	
	}
	
	D3DXMatrixInverse(&viewMat, NULL, &viewMat);

	D3DXMATRIX matTrans = viewMat * (*prjMatD3D) * mat;

	g_pDevice->SetTransform( D3DTS_TEXTURE0, &matTrans );

	g_pDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, 
	    	D3DTTFF_COUNT3|D3DTTFF_PROJECTED );		

   g_pDevice->SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX, 
	    	D3DTSS_TCI_CAMERASPACEPOSITION | 1 );  

	const static float c_zp = -0.000003f;
	g_pDevice->SetRenderState (D3DRS_DEPTHBIAS, *(DWORD*)&c_zp);

	g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,	false);
	g_pDevice->SetRenderState(D3DRS_ZENABLE,			true );
	g_pDevice->SetRenderState(D3DRS_ZWRITEENABLE,		false);
	g_pDevice->SetRenderState(D3DRS_LIGHTING,			false);

	g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,	true		);
	g_pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA		);
	g_pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA	);	

	g_pDevice->SetTextureStageState(0,   D3DTSS_COLOROP,     D3DTOP_SELECTARG1);  
	g_pDevice->SetTextureStageState(0,   D3DTSS_COLORARG1,   D3DTA_TEXTURE	  );  

	g_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP	);
	g_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP	);
	g_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR		);
    g_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR		);
    g_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR		);	
	
	g_pDevice->SetPixelShader (0);
	g_pDevice->SetVertexShader(0);	
}

// ----------------------------------------------
// 还原渲染状态

void COTEShadowMap::ResetRenderStates()
{
	RENDERSYSTEM->shutTextureTransform(0);
	g_pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	// 关闭shader
	g_pDevice->SetPixelShader(0);
	g_pDevice->SetVertexShader(0);
}

// ----------------------------------------------
void COTEShadowMap::RenderShadowTerrain()
{
	if(!COTETilePage::s_CurrentPage || s_RenderTileList.empty())
		return;

	// 设置渲染状态

	SetRenderStates();
	
	RENDERSYSTEM->_setTexture(0, true, "shadowmap_rt");

	// 渲染 地面

	std::list<COTETile*>::iterator it = s_RenderTileList.begin();
	while(it != s_RenderTileList.end())
	{
		COTETile* t = *it;

		// 渲染状态

#ifndef GAME_DEBUG1
		OTE::eRenderState renderState = t->m_ParentRenderUnit->m_RenderState;
#endif
		Ogre::RenderOperation ro ;	
		t->getRenderOperation(ro);	
		
		int vertexCount = ro.vertexData->vertexCount;
		int indexCount	= ro.indexData->indexCount  ;

#ifndef GAME_DEBUG1	
		int vertexStart = ro.vertexData->vertexStart  ;
		ro.vertexData->vertexStart =  t->m_VertexStart;
#endif		
		ro.vertexData->vertexCount =  t->GetTileVertexCount();		
		ro.indexData->indexCount   =  t->GetTileIndexCount() ;			

		// 逐个渲染

		RENDERSYSTEM->_render(ro);

		// 还原
		
		ro.vertexData->vertexCount = vertexCount;
		ro.indexData->indexCount   = indexCount	;

#ifndef GAME_DEBUG1
		ro.vertexData->vertexStart = vertexStart;
		t->m_ParentRenderUnit->m_RenderState = renderState;
#endif

		++ it;
	}	

	ResetRenderStates();
}

// ----------------------------------------------
void COTEShadowMap::RenderShadowReceiverObjs()
{
	if(s_RenderObjList.empty())
		return;

	// 设置渲染状态

	SetRenderStates();
	
	RENDERSYSTEM->_setTexture(0, true, "shadowmap_rt");

	//  渲染影子接受物体

	std::vector<COTEActorEntity*>::iterator it1 = s_RenderObjList.begin();
	while(it1 != s_RenderObjList.end())
	{
		COTEActorEntity* ae = *it1;

		// subentity

		int NumEnts = ae->getNumSubEntities();
		for(int i = 0; i < NumEnts; i ++)
		{
			COTESubEntity* se = ae->getSubEntity(i);	

			// 逐个渲染

			Ogre::RenderOperation  ro ;
			se->getRenderOperation(ro);

			RENDERSYSTEM->_render(ro);
		}

		++ it1;
	}
	

	ResetRenderStates();
}


// ----------------------------------------------
void COTEShadowMap::UpdateTerrain()
{
	if(!COTETilePage::s_CurrentPage)
		return;

	// 先清理

	s_RenderTileList.clear();


	// 计算一个矩形范围

	Ogre::Vector3 _min( 200.0f, 0.0f,  200.0f);
	Ogre::Vector3 _max(-200.0f, 0.0f, -200.0f); 

	std::vector<COTEActorEntity*>::iterator it = s_ShadowObjList.begin();
	while(it != s_ShadowObjList.end())
	{	
		Ogre::Vector3 min = (*it)->getParentSceneNode()->_getWorldAABB().getMinimum();
		Ogre::Vector3 max = (*it)->getParentSceneNode()->_getWorldAABB().getMaximum();

		Ogre::Ray ray(max, s_RenderTexture->m_pCamera->getDirection());

		// 计算物体aabb的投影	

		std::pair<bool, float> result = ray.intersects(Ogre::Plane(Ogre::Vector3::UNIT_Y, (*it)->getParentSceneNode()->_getWorldAABB().getMaximum()));
		if(!result.first)
		{			
			return;
		}

		max = ray.getPoint(result.second);

		float temp;

		max.x < min.x ? temp = max.x, max.x = min.x, min.x = temp : NULL;	
		max.z < min.z ? temp = max.z, max.z = min.z, min.z = temp : NULL;		

		_max.x < max.x ? _max.x = max.x : NULL;
		_max.z < max.z ? _max.z = max.z : NULL;	

		_min.x > min.x ? _min.x = min.x : NULL;
		_min.z > min.z ? _min.z = min.z : NULL;	

		++ it;	
	}

	// 矩形范围内tile添加到渲染列表

	float tileSize = (COTETilePage::s_CurrentPage->m_TileSize) * COTETilePage::s_CurrentPage->m_Scale.x;
	float pageSizeX = (COTETilePage::s_CurrentPage->m_PageSizeX) * COTETilePage::s_CurrentPage->m_Scale.x;
	float pageSizeZ = (COTETilePage::s_CurrentPage->m_PageSizeZ) * COTETilePage::s_CurrentPage->m_Scale.z;
	
	_max.x > pageSizeX ? _max.x = pageSizeX : NULL;	
	_max.z > pageSizeZ ? _max.z = pageSizeZ : NULL;

	_min.x < 0 ? _min.x = 0 : NULL;	
	_min.z < 0 ? _min.z = 0 : NULL;
	COTETile* lastTile = 0;
	for(float x = _min.x; x < _max.x; x += tileSize)
	for(float z = _min.z; z < _max.z; z += tileSize)
	{
		COTETile* tile = COTETilePage::s_CurrentPage->GetTileAt(x, z);
		if(tile && tile != lastTile)
		{	
			// 查找是否已经存在

			bool isFind = false;
			std::list<COTETile*>::iterator it = s_RenderTileList.begin();
			while(it != s_RenderTileList.end())
			{
				if(tile == (*it))
				{
					isFind = true;
					break;
				}				
				++ it;
			}

			// 添加

			if(!isFind)
				s_RenderTileList.push_back(tile);

			lastTile = tile;
		}
	}

}

// ----------------------------------------------
void COTEShadowMap::OnRenderShadowReceivers(int cam)
{
	// 更新山体

	if(s_NeedUpdateTerrain)
	{
		UpdateTerrain();
		s_NeedUpdateTerrain = false;
	}

	// 渲染带阴影的山体

	RenderShadowTerrain();

	// 渲染带阴影的物体

	RenderShadowReceiverObjs();

}

// ----------------------------------------------
bool COTEShadowMap::CreateRes()
{
	if(!s_pSMapEffect)
	{
		Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource("shadowmaprender.fx");
		std::string effectstring = stream->getAsString();
	 
		// 创建shader

		LPD3DXBUFFER errBuf;
		D3DXCreateEffect(g_pDevice,
			effectstring.c_str(),
			effectstring.length(),
			NULL,
			NULL,
			D3DXSHADER_DEBUG,
			NULL,
			&s_pSMapEffect,
			&errBuf);

		if(errBuf)
		{
			OTE_MSG("COTEShadowMap::CreateRes 加载" << "shadowmaprender.fx" << (char*)errBuf->GetBufferPointer(), "失败");
			return false;	
		}	
	}	

	return true;
}

// ----------------------------------------------
void COTEShadowMap::ClearRes()
{
	SAFE_RELEASE(s_pSMapEffect)
}


// ----------------------------------------------
void COTEShadowMap::AddShadowObj(COTEActorEntity* ae)
{
	if(!ae)
		return;

	if(!s_RenderTexture)
		COTEShadowMap::Init();		

	// 查找是否已经存在

	std::vector<COTEActorEntity*>::iterator it = s_ShadowObjList.begin();
	while(it != s_ShadowObjList.end())
	{
		if(ae == (*it))
			return;
		++ it;
	}

	WAIT_LOADING_RES(ae)

	s_ShadowObjList.push_back(ae);	

	ae->AddBeforeRenderCB(OnBeforeRenderShadowObj);

	ae->AddEndRenderCB(OnEndRenderShadowObj);	

	ae->AddEventHandler("OnBeforeDestroy", OnBeforeEntityDestroy);

	s_RenderTexture->m_RenderTexture->getViewport(0)->setClearEveryFrame(true);

	s_NeedUpdateTerrain = true;

}


// ----------------------------------------------
// 删除物件之前从影子投射对象列表中删除该物件

void COTEShadowMap::OnBeforeReceiverEntityDestroy(const CEventObjBase::EventArg_t& arg)
{
	std::vector<COTEActorEntity*>::iterator it = s_RenderObjList.begin();
	while(it != s_RenderObjList.end())
	{
		if((*it)->getName() == arg.strObjName)
		{
			s_RenderObjList.erase(it);
			break;
		}			
		++ it;
	}
	
	// 列表为空

	if(s_RenderObjList.empty())
	{
		s_RenderTexture->m_RenderTexture->getViewport(0)->setClearEveryFrame(false);
	}

}

// ----------------------------------------------
void COTEShadowMap::AddShadowReceiverObj(COTEActorEntity* ae)
{
	if(!ae)
		return;

	if(!s_RenderTexture)
		COTEShadowMap::Init();		

	// 查找是否已经存在

	std::vector<COTEActorEntity*>::iterator it = s_RenderObjList.begin();
	while(it != s_RenderObjList.end())
	{
		if(ae == (*it))
			return;
		++ it;
	}

	WAIT_LOADING_RES(ae)

	s_RenderObjList.push_back(ae);	

	ae->AddEventHandler("OnBeforeDestroy", OnBeforeReceiverEntityDestroy);

	s_RenderTexture->m_RenderTexture->getViewport(0)->setClearEveryFrame(true);
}

// ----------------------------------------------
void COTEShadowMap::RemoveShadowReceiverObj(COTEActorEntity* ae)
{
	if(!ae)
		return;

	CEventObjBase::EventArg_t arg;
	arg.strObjName = ae->GetName();

	OnBeforeReceiverEntityDestroy(arg);
}

// ----------------------------------------------
void COTEShadowMap::SetAlignment(eAlignment alignment)
{
	s_Alignment = alignment;
}

// ----------------------------------------------
void COTEShadowMap::SaveMap(const std::string& fileName)
{	
	s_MapFileName = fileName;
}

// ----------------------------------------------
void COTEShadowMap::Destroy()
{
	if(s_RenderTexture)
		COTERenderTexture::Remove(s_RenderTexture);

	ClearRes();
}

// ----------------------------------------------
void COTEShadowMap::Init()
{
	// 创建相关资源

	if(!CreateRes())
		return;	

	// 创建一个摄像机
			
	Ogre::Camera* cam = SCENE_MGR->createCamera("SM_RT_CAM");
	Ogre::SceneNode* camNode = SCENE_MGR->getRootSceneNode()->createChildSceneNode("SM_RT_CAM");
	camNode->attachObject(cam);

	((Ogre::Camera*)cam)->m_RenderFlag = 0; // 清理摄像机渲染标志	

	cam->setNearClipDistance(SCENE_CAM->getNearClipDistance());
	cam->setFarClipDistance (SCENE_CAM->getFarClipDistance() );


	// 渲染到贴图

	s_RenderTexture = COTERenderTexture::Create("shadowmap_rt", c_ShadowMapSize, c_ShadowMapSize, cam);
	s_RenderTexture->m_RenderTexture->getViewport(0)->setBackgroundColour(ColourValue(0.3f, 0.3f, 0.3f, 0.0f));

	// 添加渲染回调

	SCENE_MGR->AddListener(COTEQTSceneManager::eBeforeRenderObjs,	OnBeforeRenderShadowMap);
	
	SCENE_MGR->AddListener(COTEQTSceneManager::eEndRenderObjs	,	OnEndRenderShadowMap	);

	SCENE_MGR->AddListener(COTEQTSceneManager::eRenderCB		,	OnRenderShadowReceivers	);
	
}

#include "OTEQTSceneManager.h"
#include "oteprjtexture.h"
#include "OTED3DManager.h"

using namespace Ogre;
using namespace OTE;


// =============================================
// COTEPrjTexture
// =============================================
const int c_camra_height = 5.0f/*摄像机在其头顶暂定5米*/;
const int c_max_triangle_num = 32/*最多三角形数*/;

// --------------------------------------------
void COTEPrjTexture::Init(const std::string& texture, const std::string& name)
{
	m_strName = name;

	// 创建摄像机	
	{
		m_pRTTCamera = SCENE_MGR->createCamera(m_strName + "_CamPrj");
		
		m_pRTTCamera->lookAt(Ogre::Vector3(0, -1.0f, 0.001f));

		m_pRTTCamera->setNearClipDistance( 0.1 );
		m_pRTTCamera->setFarClipDistance( 10 );	
	}
	
	// 贴图

	m_strTextureFile = texture;
	m_Texture = Ogre::TextureManager::getSingleton().getByName(m_strTextureFile);
	if(m_Texture.isNull())
	{
		m_Texture = TextureManager::getSingleton().load(
			m_strTextureFile,
			RES_GROUP);
	}

	m_IsVisible = true;

	// 贴图大小系数

	m_Width = 5.0f;		
	m_Height = 5.0f;

	m_BoundingRadius = 5.0f;

	m_AABB = AxisAlignedBox(- m_Width, 0, - m_Height, m_Width, 1.0f, m_Height); 

	OTE_TRACE("创建COTEPrjTexture name : " << name)
}

// -------------------------------------- 
void COTEPrjTexture::setMaterialName(const String& name)
{
	m_pMaterial = MaterialManager::getSingleton().getByName(name);

	if (m_pMaterial.isNull())
		OGRE_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Could not find material " + name,
			"BillboardSet::setMaterialName" );
}

// -------------------------------------- 
void COTEPrjTexture::_notifyCurrentCamera(Ogre::Camera* cam)
{	
	// 反应器更新

	if(m_BindReactor && !(((CReactorBase*)m_BindReactor)->m_StateFlag & 0xFF000000))
	{
		((CReactorBase*)m_BindReactor)->Update(this, COTED3DManager::s_FrameTime);	
	}	

}

// -------------------------------------- 
void COTEPrjTexture::_updateRenderQueue(RenderQueue* queue)
{
	queue->addRenderable(this);
}

//-----------------------------------------------------------------------
void COTEPrjTexture::getWorldTransforms( Matrix4* xform ) const
{     
     *xform = _getParentNodeFullTransform(); 
}

//-----------------------------------------------------------------------
const Quaternion& COTEPrjTexture::getWorldOrientation(void) const
{
    return mParentNode->_getDerivedOrientation();
}

//-----------------------------------------------------------------------
const Vector3& COTEPrjTexture::getWorldPosition(void) const
{
    return mParentNode->_getDerivedPosition();
}

//-----------------------------------------------------------------------
void COTEPrjTexture::getRenderOperation(Ogre::RenderOperation& op)
{
	/*op.operationType = RenderOperation::OT_TRIANGLE_LIST;
	op.useIndexes = true;

	op.vertexData = m_ResPtr->mVertexData;
	op.vertexData->vertexCount = m_ResPtr->mNumVisibleBillboards * 4;
	op.vertexData->vertexStart = 0;

	op.indexData = m_ResPtr->mIndexData;
	op.indexData->indexCount = m_ResPtr->mNumVisibleBillboards * 6;
	op.indexData->indexStart = 0;*/
}

//-----------------------------------------------------------------------
Real COTEPrjTexture::getSquaredViewDepth(const Camera* const cam) const
{  
    return mParentNode->getSquaredViewDepth(cam);
}

//-----------------------------------------------------------------------
const LightList& COTEPrjTexture::getLights(void) const
{
    return getParentSceneNode()->findLights(this->getBoundingRadius());
}

// --------------------------------------------
void COTEPrjTexture::Destroy()
{	
	SCENE_MGR->removeCamera(m_pRTTCamera);

	OTE_TRACE("删除COTEPrjTexture name : " << this->m_strName)

};

// --------------------------------------------
void COTEPrjTexture::Update(const Ogre::Vector3& TargetPos)
{
	if(!m_pRTTCamera)
		return;

	COTETilePage* page = COTETilePage::s_CurrentPage;

	// 计算其占有的Tile	

	float left		= TargetPos.x - m_Width  / 2.0f;
	float right		= TargetPos.x + m_Width  / 2.0f;	
	float top		= TargetPos.z - m_Height / 2.0f;
	float bottom	= TargetPos.z + m_Height / 2.0f;

	float searchSize = page->m_TileSize;
	
	m_RenderTileList.clear();	

	for( float x = left; x < right; x += searchSize )
	for( float z = top;  z < bottom; z += searchSize )
	{
		COTETile* t = page->GetTileAt(x, z);	
		if(t)
		{
			m_RenderTileList.push_back(t);		
		}
	}

	m_pRTTCamera->setPosition (
		TargetPos.x, TargetPos.y + c_camra_height, TargetPos.z);

	
}

// --------------------------------------------
void COTEPrjTexture::Render()
{return;
	if(m_RenderTileList.empty() || !m_IsVisible)
		return;

	// 设置世界矩阵

	D3DXMATRIX matrix;
	D3DXMatrixIdentity(&matrix);

	g_pDevice->SetTransform(D3DTS_WORLD,		&matrix);
	
	g_pDevice->SetPixelShader(0);
	g_pDevice->SetVertexShader(0);

	// 得到视图矩阵
		
	D3DXMATRIX oldViewMat;
	g_pDevice->GetTransform(D3DTS_VIEW,		&oldViewMat); 

	// 用于纠正位置

	D3DXMATRIXA16 mat;

	mat._11 = 0.25f; mat._12 = 0.00f; mat._13 = 0.00f; mat._14 = 0.00f;
    mat._21 = 0.00f; mat._22 =-0.25f; mat._23 = 0.00f; mat._24 = 0.00f;
    mat._31 = 0.00f; mat._32 = 0.00f; mat._33 = 1.00f; mat._34 = 0.00f;
    mat._41 = 0.50f; mat._42 = 0.50f; mat._43 = 0.00f; mat._44 = 1.00f;
	
	// 得到透视摄像机视图矩阵

	Matrix4 viwMat = m_pRTTCamera->getViewMatrix();
	viwMat[2][0] =  - viwMat[2][0];	
	viwMat[2][1] =  - viwMat[2][1];	
	viwMat[2][2] =  - viwMat[2][2];	
	viwMat[2][3] =  - viwMat[2][3]; 	

	D3DXMATRIXA16 viwMatD3D;
	COTED3DManager::MAKED3DXMatrix(viwMat, viwMatD3D);	 

	// matTrans = V^(-1) * V' * mat

	D3DXMATRIXA16 matTrans;
	D3DXMATRIXA16 oldViewMatInv;

	D3DXMatrixInverse(&oldViewMatInv, NULL,&oldViewMat);
	D3DXMatrixMultiply(&matTrans, &oldViewMatInv, &viwMatD3D);
	D3DXMatrixMultiply(&matTrans, &matTrans, &mat);

	g_pDevice->SetTransform( D3DTS_TEXTURE0, &matTrans );

    g_pDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, 
	    	D3DTTFF_COUNT2 );		

    g_pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 
	    	D3DTSS_TCI_CAMERASPACEPOSITION | 1 );


	g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,0);
	g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	g_pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	g_pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	g_pDevice->SetRenderState(D3DRS_LIGHTING, false);		 

	Ogre::D3D9RenderSystem* rs = ( (Ogre::D3D9RenderSystem*)(Ogre::Root::getSingleton().getRenderSystem( ) ) );	
	rs->_setTexture(0, true, m_strTextureFile);

	g_pDevice->SetTextureStageState(0,   D3DTSS_COLOROP,     D3DTOP_SELECTARG1);  
	g_pDevice->SetTextureStageState(0,   D3DTSS_COLORARG1,   D3DTA_TEXTURE);  

	g_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP );
	g_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP );
	g_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    g_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    g_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);		

	// 渲染

	std::list<void*>::iterator it = m_RenderTileList.begin();
	while(it != m_RenderTileList.end())
	{
		Ogre::RenderOperation ro;	
		((COTETile*)(*it))->getRenderOperation(ro);		

		ro.vertexData->vertexCount =  ((COTETile*)(*it))->GetTileVertexCount();
		ro.indexData->indexCount   =  ((COTETile*)(*it))->GetTileIndexCount();		
		
		RENDERSYSTEM->_render(ro);
		++ it;
	}	
	
	
	// 关闭自动纹理坐标

	g_pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);

}


// =============================================
// COTEPrjTextureRQListener
// =============================================
COTEPrjTextureRQListener*    COTEPrjTextureRQListener::m_pInst = NULL;

// ---------------------------------------------
void COTEPrjTextureRQListener::Destroy()
{
	if(m_pInst)
	{	
		SCENE_MGR->removeRenderQueueListener(m_pInst);

		HashMap<std::string, COTEPrjTexture*>::iterator it = m_pInst->m_pPrjTextureList.begin();
		while( it != m_pInst->m_pPrjTextureList.end() )
		{
			it->second->Destroy();
			delete it->second;

			++ it;
		}		

		SAFE_DELETE(m_pInst)		
	}

}


// ---------------------------------------------
// 创建贴图
COTEPrjTexture* COTEPrjTextureRQListener::CreatePrjTex(const std::string& name, const std::string& texture)
{
	if(!m_pInst)	
		m_pInst->Init();	

	COTEPrjTexture* pt = new COTEPrjTexture();	

	pt->Init(texture, name);

	m_pInst->m_pPrjTextureList[name] = pt;
	
	return pt;

}

// ---------------------------------------------
// 摧毁贴图
void COTEPrjTextureRQListener::DestroyPrjTex(COTEPrjTexture* pt)
{		
	DestroyPrjTex(pt->m_strName);	

}

// ---------------------------------------------
void COTEPrjTextureRQListener::DestroyPrjTex(const std::string& name)
{
	OTE_ASSERT(m_pInst);

	HashMap<std::string, COTEPrjTexture*>::iterator it = m_pInst->m_pPrjTextureList.find(name);
	if(it != m_pInst->m_pPrjTextureList.end())
	{
		COTEPrjTexture* pt = it->second;
		pt->Destroy();
		delete pt;

		m_pInst->m_pPrjTextureList.erase(it);
		
	}	

}

// ---------------------------------------------
void COTEPrjTextureRQListener::renderQueueStarted(Ogre::RenderQueueGroupID id, bool& skipThisQueue)
{	
	if ((m_post_queue) && m_queue_id == id)
	{	
		TIMECOST_START

		Ogre::Root::getSingleton().getRenderSystem( )->_disableTextureUnitsFrom(0);

		// 渲染

		HashMap<std::string, COTEPrjTexture*>::iterator it = m_pPrjTextureList.begin();
		while( it != m_pPrjTextureList.end() )
		{
			it->second->Render();

			++ it;
		}

		TIMECOST_END
	}
}

// ---------------------------------------------
void COTEPrjTextureRQListener::renderQueueEnded(Ogre::RenderQueueGroupID id, bool& repeatThisQueue)
{
	if ((m_post_queue) && (m_queue_id == id))
	{
	}
}


// ---------------------------------------------
void COTEPrjTextureRQListener::Init()
{	
	// D3D借口初始化		
		
	m_pInst = new COTEPrjTextureRQListener(Ogre::RENDER_QUEUE_SKIES_LATE, true);
	
	SCENE_MGR->addRenderQueueListener(m_pInst);
}
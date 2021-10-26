#include "OTEQTSceneManager.h"
#include "otewater.h"
#include "OTETilePage.h"
#include "OgreD3D9RenderSystem.h" 
#include "OgreD3D9RenderWindow.h"
#include "OgreD3D9HardwareVertexBuffer.h" 
#include "OgreD3D9HardwareIndexBuffer.h" 

#include "OgreD3D9Texture.h"
#include "OTECommon.h"
#include "OTED3DManager.h"
#include "OTEFog.h"

using namespace Ogre;
using namespace OTE;

// =============================================
// COTELiquid
// =============================================

COTELiquid::COTELiquid(CLiquidRender* render) : 
m_LiquidPlaneVisible(true),
m_Render(render),
m_Height(0)
{
}

COTELiquid::~COTELiquid(void)
{	
	if(m_Render)
	{
		m_Render->m_BindLiquid = NULL;	
		SAFE_DELETE(m_Render)
	}
}

// ----------------------------------------------
bool COTELiquid::AddLiquidPlane(COTETile* pTile, int waterValue)
{		
	if(!IsLiquidPlaneVisible())
		return false;
	
	m_Height = GET_WATERREALHGT(waterValue);

	// 水单元

	CLiquidElm_t* we = new CLiquidElm_t();
	we->m_isInitSucceed = true;
	we->m_Tile = pTile;
	we->m_OldTickCount = ::GetTickCount();

	// 初始化顶点

	float pageWidth  = (COTETilePage::s_CurrentPage->m_PageSizeX) * COTETilePage::s_CurrentPage->m_Scale.x;
	float pageHeight = (COTETilePage::s_CurrentPage->m_PageSizeZ) * COTETilePage::s_CurrentPage->m_Scale.z;
	
	COTETile* t = pTile;
	int size = t->m_pParentPage->m_TileSize;

	we->m_waterPlane.vertice[0].pos[0] = t->_vertex(0, 0, 0);
	we->m_waterPlane.vertice[0].pos[1] = m_Height;
	we->m_waterPlane.vertice[0].pos[2] = t->_vertex(0, 0, 2);
	we->m_waterPlane.vertice[0].uv.x = t->_vertex(0, 0, 0) / pageWidth;
	we->m_waterPlane.vertice[0].uv.y = t->_vertex(0, 0, 2) / pageHeight;
	we->m_waterPlane.vertice[0].uv1.x = 0.0f;
	we->m_waterPlane.vertice[0].uv1.y = 0.0f;

	we->m_waterPlane.vertice[1].pos[0] = t->_vertex(0, size, 0);
	we->m_waterPlane.vertice[1].pos[1] = m_Height;
	we->m_waterPlane.vertice[1].pos[2] = t->_vertex(0, size, 2);
	we->m_waterPlane.vertice[1].uv.x = t->_vertex(0, size, 0) / pageWidth ;
	we->m_waterPlane.vertice[1].uv.y = t->_vertex(0, size, 2) / pageHeight;
	we->m_waterPlane.vertice[1].uv1.x = 0.0f;
	we->m_waterPlane.vertice[1].uv1.y = 1.0f;

	we->m_waterPlane.vertice[2].pos[0] = t->_vertex(size, size, 0);
	we->m_waterPlane.vertice[2].pos[1] = m_Height;
	we->m_waterPlane.vertice[2].pos[2] = t->_vertex(size, size, 2);
	we->m_waterPlane.vertice[2].uv.x = t->_vertex(size, size, 0) / pageWidth;
	we->m_waterPlane.vertice[2].uv.y = t->_vertex(size, size, 2) / pageHeight;
	we->m_waterPlane.vertice[2].uv1.x = 1.0f;
	we->m_waterPlane.vertice[2].uv1.y = 1.0f;

	we->m_waterPlane.vertice[3].pos[0] = t->_vertex(size, 0, 0);
	we->m_waterPlane.vertice[3].pos[1] = m_Height;
	we->m_waterPlane.vertice[3].pos[2] = t->_vertex(size, 0, 2);
	we->m_waterPlane.vertice[3].uv.x = t->_vertex(size, 0, 0) / pageWidth ;
	we->m_waterPlane.vertice[3].uv.y = t->_vertex(size, 0, 2) / pageHeight;
	we->m_waterPlane.vertice[3].uv1.x = 1.0f;
	we->m_waterPlane.vertice[3].uv1.y = 0.0f;

	m_LiquidElms[we->m_Tile] = we;

	// 渲染器初始化

	return m_Render->Init(this);
}

// --------------------------------------------------------
void COTELiquid::EraseLiquidPlane(COTETile* pTile)
{
	HashMap<COTETile*, CLiquidElm_t*>::iterator it = m_LiquidElms.find((COTETile*)pTile);
	if(it !=  m_LiquidElms.end())
	{
		delete(it->second);
		m_LiquidElms.erase(it);		
	}	
}

// ----------------------------------------------
void COTELiquid::Update(COTETile* pTile)
{
	if(!m_LiquidPlaneVisible)
		return;

	CLiquidElm_t* we = m_LiquidElms[pTile];
	assert(we);

	if(!we->m_isInitSucceed)
		return;	
	
	m_Render->Update(we);
}

// ----------------------------------------------
void COTELiquid::Clear()
{
	HashMap<COTETile*, CLiquidElm_t*>::iterator it = m_LiquidElms.begin();
	while(it !=  m_LiquidElms.end())
	{
		delete(it->second);
		++ it;
	}
	m_LiquidElms.clear();
}

// ========================================================
// CLiquidRender
// ========================================================
CLiquidRender::CLiquidRender() :
m_pEffect(NULL),
m_waterIndexBuffer(NULL),
m_waterVertexBuffer(NULL),
m_BindLiquid(NULL),
m_PoolCount(0),
m_VertexBufferPoolLocked(false),
m_pVBuf(NULL)
{	
}



// ----------------------------------------------
bool CLiquidRender::CreateIndexBuffer(int maxPlanes)
{
	if(m_waterIndexBuffer == NULL)
	{
		m_IndexBuffer = 
			HardwareBufferManager::getSingleton().createIndexBuffer(
			HardwareIndexBuffer::IT_16BIT,
			maxPlanes * 6, 
			HardwareBuffer::HBU_STATIC_WRITE_ONLY);
		
		D3D9HardwareIndexBuffer* ibuf =  static_cast<D3D9HardwareIndexBuffer*>(m_IndexBuffer.getPointer());
	
		m_waterIndexBuffer = ibuf->getD3DIndexBuffer();		

		WORD* indices = 0;
		m_waterIndexBuffer->Lock(0, 0, (void**)&indices, 0);		

		for( int i = 0; i < maxPlanes; i ++)
		{
			// 填充索引缓冲
			indices[0] = 0 + i * 4;
			indices[1] = 1 + i * 4;
			indices[2] = 2 + i * 4;
			indices[3] = 2 + i * 4;
			indices[4] = 3 + i * 4;
			indices[5] = 0 + i * 4;

			indices += 6;
		}

		m_waterIndexBuffer->Unlock(); 
	}
	
	return true;

}

// ----------------------------------------------
bool CLiquidRender::CreateVertexBuffer(int maxPlanes)
{	
	if(m_waterVertexBuffer == NULL)
	{
		m_VertexBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
					sizeof(LiquidVertex_t), 
					maxPlanes * 4, 
					HardwareBuffer::HBU_STATIC_WRITE_ONLY);	
		
		const Ogre::D3D9HardwareVertexBuffer* d3dvb = static_cast<const Ogre::D3D9HardwareVertexBuffer*>(m_VertexBuffer.getPointer());
		m_waterVertexBuffer = d3dvb->getD3D9VertexBuffer();
	}

	return true;
}
// --------------------------------------------------------
void CLiquidRender::SetHeightMap(const Ogre::String& rHeightMapName)
{
	if(m_HeightMap.isNull())
	{
		m_HeightMap = Ogre::TextureManager::getSingleton().load(rHeightMapName, 
									RES_GROUP);
		
	}
}

// ----------------------------------------------
void CLiquidRender::Clear(bool invalidDevice)
{		
	if(!invalidDevice)
	{
		if(m_BindLiquid)
			m_BindLiquid->Clear();

		m_waterTexture.setNull();

		m_VertexBuffer.setNull();

		m_IndexBuffer.setNull();	
	}

	// clear state values

	
	if(m_VertexBufferPoolLocked && m_waterIndexBuffer)
	{
		m_waterVertexBuffer->Unlock();
		m_VertexBufferPoolLocked = false;
		m_waterVertexBuffer = NULL;
	}

	m_waterIndexBuffer = NULL;	

	SAFE_RELEASE(m_pEffect);
		
	m_PoolCount = 0;
	m_pVBuf = 0;
	m_VertexBufferPoolLocked = false;

}

// ========================================================
// COTELakeLiquidRender
// ========================================================

const int				COTELakeLiquidRender::c_MaxLiquidPlanes	= 512;			// 最大水片数量

// ----------------------------------------------
COTELakeLiquidRender::COTELakeLiquidRender() : 
CLiquidRender(),
m_Color(0xffffffff),
m_RTexture(NULL)
{	
}

// ----------------------------------------------
COTELakeLiquidRender::~COTELakeLiquidRender()
{
	this->Clear();
}


// ----------------------------------------------
void COTELakeLiquidRender::OnBeforeRenderLiquidRT(int cam)
{
	COTELiquidManager* mgr = COTELiquidManager::GetSingleton();

	HashMap<int, COTELiquid*>::iterator it = mgr->m_LiquidMap.begin();
	while(it != mgr->m_LiquidMap.end())
	{
		// 只考虑湖水

		int type = GET_WATERTYPE(it->first);
		COTELiquid* ld = it->second;

		if( type == COTELiquid::eLake_Water && 
			ld->m_Render->HasUpdate()			// 是否正在渲染
			)
		{			
			COTERenderTexture* rt = ((COTELakeLiquidRender*)ld->m_Render)->m_RTexture;
			if(rt)
			{
				// 在不渲染水的时候把倒影渲染到贴图关闭

				if(!ld->m_Render->HasUpdate())
				{
					ld->SetLiquidPlaneVisible(true);
					rt->EnableFrameUpdate(false);
					return;
				}
				else
				{
					rt->EnableFrameUpdate(true);
				}
				
				if(int(rt->m_pCamera) == cam)
				{
					ld->SetLiquidPlaneVisible(false);
						
					Ogre::Plane MirrorPlane(Ogre::Vector3(0.0f,1.0f,0.0f), ld->m_Height);		
					rt->m_pCamera->enableCustomNearClipPlane(MirrorPlane);
						
					return;
				}
				else
				{
					rt->m_pCamera->disableCustomNearClipPlane();
				}
			}
				
			ld->SetLiquidPlaneVisible(true);
		}

		++ it;
	}

}	

// ----------------------------------------------
bool COTELakeLiquidRender::Init(COTELiquid* lq)
{
	if(m_BindLiquid)
		return false;

	m_BindLiquid = lq;

	// 索引

	if(!CreateIndexBuffer(c_MaxLiquidPlanes))
		return false;

	// 顶点

	if(!CreateVertexBuffer(c_MaxLiquidPlanes))
		return false;

	// 高度图

	SetHeightMap("heightmap.tga"); // 如果外部没有制定则指定默认的

	// 基本贴图

	SetBaseTexture("lake_a0.dds");
		
	SCENE_MGR->AddListener(COTEQTSceneManager::eBeforeRenderObjs, OnBeforeRenderLiquidRT);

	return true;
}

// --------------------------------------------------------
bool COTELakeLiquidRender::CreateD3DRes()
{
	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource("LakeWaterMaterial.fx");
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
		&m_pEffect,
		&errBuf);

	if(errBuf)
	{
		MessageBox(0,(char*)errBuf->GetBufferPointer(),0,0);
		return false;	
	}

	return true;
}

// --------------------------------------------------------
void COTELakeLiquidRender::SetEnviMap(float hgt, const Ogre::String& rEnviMapName)
{
	if(m_EnviMap.isNull())
	{
		if(!rEnviMapName.empty())
		{
			m_EnviMap = Ogre::TextureManager::getSingleton().load(rEnviMapName, 
									RES_GROUP);
		}
		else
		{
			// 创建一个摄像机

			static int s_Cnt = 0;	std::stringstream ss; 
			ss.str(""); ss << "WTREF_RT_CAM" << s_Cnt ++;
			
			Ogre::Camera* cam = SCENE_MGR->createCamera(ss.str());
			Ogre::SceneNode* camNode = 
				SCENE_MGR->getRootSceneNode()->createChildSceneNode(ss.str());
			camNode->attachObject(cam);

			cam->setNearClipDistance(SCENE_CAM->getNearClipDistance());
			cam->setFarClipDistance(SCENE_CAM->getFarClipDistance());			
			
			ss.str(""); ss << "water_reflection_rt" << s_Cnt;

			m_RTexture = COTERenderTexture::Create(ss.str(), 512, 512, cam);

			m_EnviMap = Ogre::TextureManager::getSingleton().getByName(m_RTexture->m_RenderTexture->getName());
		}
		
	}	
	
	if(m_RTexture)
	{
		const Ogre::Vector3& camPos = SCENE_CAM->getPosition();
		m_RTexture->m_pCamera->setPosition(Ogre::Vector3(camPos.x, 2 * hgt - camPos.y, camPos.z));
		const Ogre::Vector3& camDir = SCENE_CAM->getDirection();
		m_RTexture->m_pCamera->setDirection(camDir.x, - camDir.y, camDir.z);		
	
		m_RTexture->m_pCamera->setAspectRatio(1.38f);
		
	}
}

// ----------------------------------------------
void COTELakeLiquidRender::SetBaseTexture(const std::string& rBaseTextureName)
{	
	if(m_waterTexture.isNull())
	{
		m_waterTexture = Ogre::TextureManager::getSingleton().getByName(rBaseTextureName);

		if(m_waterTexture.isNull())
			m_waterTexture = Ogre::TextureManager::getSingleton().load(rBaseTextureName, 
												RES_GROUP);
	}
}

// ----------------------------------------------
void COTELakeLiquidRender::Update(CLiquidElm_t* we)
{
	TIMECOST_START

	if(!m_BindLiquid)
		return;

	// 顶点缓存池是否已经被锁定

	if(!m_waterVertexBuffer)
	{
		const Ogre::D3D9HardwareVertexBuffer* d3dvb = static_cast<const Ogre::D3D9HardwareVertexBuffer*>(m_VertexBuffer.getPointer());
		m_waterVertexBuffer = d3dvb->getD3D9VertexBuffer();
		D3D9HardwareIndexBuffer* ibuf =  static_cast<D3D9HardwareIndexBuffer*>(m_IndexBuffer.getPointer());
		m_waterIndexBuffer = ibuf->getD3DIndexBuffer();	
	}

	if(!m_VertexBufferPoolLocked)
	{	
		int copyByte = sizeof(LiquidPlane_t) * c_MaxLiquidPlanes;
		m_waterVertexBuffer->Lock(0, copyByte, (void**)&m_pVBuf, 0);	
		m_VertexBufferPoolLocked = true;
	}	

	// 环境贴图

	SetEnviMap(m_BindLiquid->m_Height);
	
	// 拷贝到显存

	we->m_PoolIndex = m_PoolCount;	

	if(we->m_PoolIndex < c_MaxLiquidPlanes && m_pVBuf) // 小于池容积?
	{
		memcpy(m_pVBuf + we->m_PoolIndex * sizeof(LiquidPlane_t), &we->m_waterPlane, sizeof(LiquidPlane_t));
		m_PoolCount ++;	
	}
	else
	{
		/*OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR, "水块数量大于池容积！",
											"COTELiquid::Update");*/
	}

	// Effect

	if(!m_pEffect)
	{
		CreateD3DRes();
	}

	TIMECOST_END
}

// ----------------------------------------------
void COTELakeLiquidRender::Render()
{	
	TIMECOST_START

	if(m_RTexture)
		m_RTexture->EnableFrameUpdate(false);	// 关闭更新

	if( !m_BindLiquid							||
		!m_BindLiquid->IsLiquidPlaneVisible()	||
		m_PoolCount == 0						||
		!m_waterVertexBuffer
		)
		return;

	
	if(m_VertexBufferPoolLocked)
	{
		m_waterVertexBuffer->Unlock();
		m_VertexBufferPoolLocked = false;
	}

	g_pDevice->SetPixelShader(0);
	g_pDevice->SetVertexShader(0);

	D3DXMATRIX matrix;
	D3DXMatrixIdentity(&matrix);
	g_pDevice->SetTransform(D3DTS_WORLD,		&matrix);

	g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
	g_pDevice->SetRenderState(D3DRS_ZENABLE, true);
	g_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
	g_pDevice->SetRenderState(D3DRS_LIGHTING, false);
	g_pDevice->SetRenderState(D3DRS_FOGENABLE, COTEFog::GetInstance()->IsFogEnabled());

	g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	g_pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	g_pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	
	// 贴图

	g_pDevice->SetTexture(0, m_waterTexture->getNormTexture());
	g_pDevice->SetTexture(1, m_EnviMap->getNormTexture());

	g_pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	g_pDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );	
		
	g_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);	

	g_pDevice->SetSamplerState(1, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
	g_pDevice->SetSamplerState(1, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);

	g_pDevice->SetStreamSource(0, m_waterVertexBuffer, 0, sizeof(LiquidVertex_t));
	g_pDevice->SetIndices(m_waterIndexBuffer);
	g_pDevice->SetFVF( LiquidVertex_t::FVF);

	UINT pass;	

	m_pEffect->SetFloat("gTime", ::GetTickCount() / 1000.0f);

	D3DXMATRIX* vpmat = COTED3DManager::GetD3DViewPrjMatrix(SCENE_CAM);
	m_pEffect->SetMatrix("matVP", vpmat);

	m_pEffect->SetTexture("heightMap", m_HeightMap->getNormTexture());

	m_pEffect->SetFloat("water_hgt", m_BindLiquid->m_Height);	

	const Ogre::Vector3& rtCamPos = m_RTexture->m_pCamera->getPosition();
	m_pEffect->SetVector("rt_cam_pos", &D3DXVECTOR4(rtCamPos.x, rtCamPos.y, rtCamPos.z, 1.0f));

	m_pEffect->Begin(&pass, D3DXFX_DONOTSAVESTATE);		 
	m_pEffect->BeginPass(0);

	g_pDevice->DrawIndexedPrimitive(
								D3DPT_TRIANGLELIST,
								0,
								0,
								m_PoolCount * 4,
								0,
								m_PoolCount * 2
								);

	m_pEffect->EndPass();	 
	m_pEffect->End();

	m_PoolCount = 0;

	m_RTexture->EnableFrameUpdate(true);	// 打开更新

	g_pDevice->SetPixelShader(0);
	g_pDevice->SetVertexShader(0);

	TIMECOST_END

}

// ----------------------------------------------
void COTELakeLiquidRender::Clear(bool invalidDevice)
{	
	if(!m_BindLiquid)
		return;

	if(!invalidDevice)
	{
		m_HeightMap.setNull();

		m_EnviMap.setNull();

		if(m_RTexture)
		{
			SCENE_MGR->removeCamera(m_RTexture->m_pCamera);
			COTERenderTexture::Remove(m_RTexture);
			m_RTexture = NULL;
		}
	}

	// clear state values

	CLiquidRender::Clear(invalidDevice);
	
}

// -------------------------------------------------------
void COTELakeLiquidRender::InvalidateDevice()
{	
	Clear(true);	
}

// =============================================
// COTEOceanWaterRender
// =============================================

const int	COTEOceanWaterRender::c_MaxLiquidPlanes	= 512;			// 最大水片数量
const int	COTEOceanWaterRender::c_LiquidFrames	= 95;			// 最多水片

// ----------------------------------------------
COTEOceanWaterRender::COTEOceanWaterRender() : 
CLiquidRender()
{

}

// ----------------------------------------------
bool COTEOceanWaterRender::Init(COTELiquid* lq)
{
	if(m_BindLiquid)
		return false;

	m_BindLiquid = lq;

	// 索引

	if(!CreateIndexBuffer(c_MaxLiquidPlanes))
		return false;

	// 顶点

	if(!CreateVertexBuffer(c_MaxLiquidPlanes))
		return false;

	// 高度图

	SetHeightMap("heightmap.tga");

	return true;
}

// --------------------------------------------------------
bool COTEOceanWaterRender::CreateD3DRes()
{
	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource("OceanWaterMaterial.fx");
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
		&m_pEffect,
		&errBuf);

	if(errBuf)
	{
		MessageBox(0,(char*)errBuf->GetBufferPointer(),0,0);
		return false;	
	}

	return true;
}

// ----------------------------------------------
void COTEOceanWaterRender::Update(CLiquidElm_t* we)
{
	//TIMECOST_START

	if(!m_BindLiquid)
		return;

	// 顶点缓存池是否已经被锁定
	
	if(!m_waterVertexBuffer)
	{
		const Ogre::D3D9HardwareVertexBuffer* d3dvb = static_cast<const Ogre::D3D9HardwareVertexBuffer*>(m_VertexBuffer.getPointer());
		m_waterVertexBuffer = d3dvb->getD3D9VertexBuffer();
		D3D9HardwareIndexBuffer* ibuf =  static_cast<D3D9HardwareIndexBuffer*>(m_IndexBuffer.getPointer());
		m_waterIndexBuffer = ibuf->getD3DIndexBuffer();	
	}

	if(!m_VertexBufferPoolLocked)
	{
		int copyByte = sizeof(LiquidPlane_t) * c_MaxLiquidPlanes;
		m_waterVertexBuffer->Lock(0, copyByte, (void**)&m_pVBuf, 0);	
		m_VertexBufferPoolLocked = true;
	}		

	/// 多桢动画
	
	// 基本贴图

	if( m_waterTextures.empty() )
	{		
		for(int i = 0; i < c_LiquidFrames; i ++)
		{
			std::stringstream ss;
			ss << "water_" << i << ".dds";		//  固定的序列贴图

			Ogre::D3D9TexturePtr tp = Ogre::TextureManager::getSingleton().getByName(ss.str());
			if(tp.isNull())
				tp = Ogre::TextureManager::getSingleton().load(ss.str(), 
								RES_GROUP);

			m_waterTextures.push_back( tp );
		}
	}	

	if( m_waterTextures.size() > 0 )
		m_waterTexture = m_waterTextures[( (int)((::GetTickCount() - we->m_OldTickCount) / 200.0f) ) % c_LiquidFrames];	
		
	//// 法线贴图

	//if( m_waterNormalTextures.empty() )
	//{		
	//	for(int i = 0; i < c_LiquidFrames; i ++)
	//	{
	//		std::stringstream ss;
	//		ss << "waterNormal" << i << ".tga";		//  固定的序列贴图

	//		Ogre::D3D9TexturePtr tp = Ogre::TextureManager::getSingleton().getByName(ss.str());
	//		if(tp.isNull())
	//			tp = Ogre::TextureManager::getSingleton().load(ss.str(), 
	//							RES_GROUP);

	//		m_waterNormalTextures.push_back( tp );
	//	}
	//}	

	//if( m_waterNormalTextures.size() > 0 )
	//	m_waterNormalTexture = m_waterNormalTextures[( (int)((::GetTickCount() - we->m_OldTickCount) / 200.0f) ) % c_LiquidFrames];	
	

	// 拷贝到显存

	we->m_PoolIndex = m_PoolCount;	

	if(we->m_PoolIndex < c_MaxLiquidPlanes && m_pVBuf) // 小于池容积?
	{
		memcpy(m_pVBuf + we->m_PoolIndex * sizeof(LiquidPlane_t), &we->m_waterPlane, sizeof(LiquidPlane_t));
		m_PoolCount ++;	
	}
	else
	{
		/*OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR, "水块数量大于池容积！",
											"COTELiquid::Update");*/
	}

	// Effect

	if(!m_pEffect)
	{
		CreateD3DRes();
	}	

	//TIMECOST_END

}

// ----------------------------------------------
void COTEOceanWaterRender::Render()
{
	TIMECOST_START

	if( !m_BindLiquid						  ||
		!m_BindLiquid->IsLiquidPlaneVisible() ||
		m_PoolCount == 0					  ||
		!m_waterVertexBuffer
		)
		return;

	if(m_VertexBufferPoolLocked)
	{
		m_waterVertexBuffer->Unlock();
		m_VertexBufferPoolLocked = false;
	}
		
	g_pDevice->SetPixelShader(0);
	g_pDevice->SetVertexShader(0);

	D3DXMATRIX matrix;
	D3DXMatrixIdentity(&matrix);
	
	g_pDevice->SetTransform(D3DTS_WORLD,		&matrix);

	g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
	g_pDevice->SetRenderState(D3DRS_ZENABLE, true);
	g_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
	g_pDevice->SetRenderState(D3DRS_LIGHTING, false);

	g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	g_pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	g_pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	
	g_pDevice->SetTexture(0, m_HeightMap->getNormTexture());
	g_pDevice->SetTexture(1, m_waterTexture->getNormTexture());
	//g_pDevice->SetTexture(2, m_waterNormalTexture->getNormTexture());

	g_pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	g_pDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
		
	g_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);	

	g_pDevice->SetStreamSource(0, m_waterVertexBuffer, 0, sizeof(LiquidVertex_t));
	g_pDevice->SetIndices(m_waterIndexBuffer);
	g_pDevice->SetFVF( LiquidVertex_t::FVF);

	UINT pass;	

	const Ogre::Vector3& lightPos = SCENE_CAM->getPosition();	
	m_pEffect->SetVector("light_pos", &D3DXVECTOR4(lightPos.x, lightPos.y, lightPos.z, 1.0f));	
	
	const Ogre::Vector3& lightDir = SCENE_MAINLIGHT->getDirection();
	m_pEffect->SetVector("light_dir", &D3DXVECTOR4(lightDir.x, lightDir.y, lightDir.z, 1.0f));	
	
	D3DXMATRIX* vpmat = COTED3DManager::GetD3DViewPrjMatrix(SCENE_CAM);
	m_pEffect->SetMatrix("matVP", vpmat);

	m_pEffect->SetFloat("water_hgt", m_BindLiquid->m_Height);	

	m_pEffect->Begin(&pass, D3DXFX_DONOTSAVESTATE);		 
	m_pEffect->BeginPass(0);

	g_pDevice->DrawIndexedPrimitive(
								D3DPT_TRIANGLELIST,
								0,
								0,
								m_PoolCount * 4,
								0,
								m_PoolCount * 2
								);

	m_pEffect->EndPass();	 
	m_pEffect->End();

	m_PoolCount = 0;

	g_pDevice->SetPixelShader(0);
	g_pDevice->SetVertexShader(0);

	TIMECOST_END

}

// ----------------------------------------------
void COTEOceanWaterRender::Clear(bool invalidDevice)
{
	m_waterTextures.clear();	
	CLiquidRender::Clear(invalidDevice);
}


// ----------------------------------------------
void COTEOceanWaterRender::InvalidateDevice()
{
	Clear(true);
}


// =============================================
// CLiquidRQListener
// =============================================
namespace OTE
{
class CLiquidRQListener : public Ogre::RenderQueueListener
{
public:	

	CLiquidRQListener(Ogre::uint8 queue_id, bool post_queue)
	{		
		m_queue_id		= queue_id;
		m_post_queue	= post_queue;
	}

	virtual ~CLiquidRQListener() 
	{
	}

	virtual void renderQueueStarted(Ogre::RenderQueueGroupID id, bool& skipThisQueue)
	{
		if ((m_post_queue) && m_queue_id == id)
		{	
			// 渲染水 

			if( !COTETilePage::s_CurrentPage->IsVisible())
				return;

			TIMECOST_START

			if(IS_DEVICELOST) // 设备丢失
			{
				COTELiquidManager::GetSingleton()->InvalidateDevice();			
			}

			// 渲染水

			COTELiquidManager::GetSingleton()->Render();	

			TIMECOST_END
		}
	}

	virtual void renderQueueEnded(Ogre::RenderQueueGroupID id, bool& repeatThisQueue){}

	// methods for adjusting target queue settings
	void	setTargetRenderQueue(Ogre::uint8 queue_id)		{m_queue_id = queue_id;}
	void	setPostRenderQueue(bool post_queue)		{m_post_queue = post_queue;}

public:

	static CLiquidRQListener*	s_pLiquidRQListener;

private:
	
	Ogre::uint8						m_queue_id;		
	bool							m_post_queue;	

};

}

CLiquidRQListener*	CLiquidRQListener::s_pLiquidRQListener = NULL;

// =============================================
// COTELiquidManager
// =============================================
COTELiquidManager* COTELiquidManager::s_pSingleton = NULL;
COTELiquidManager* COTELiquidManager::GetSingleton()
{
	if(!s_pSingleton) 
	{
		s_pSingleton = new COTELiquidManager();		
	}
	return s_pSingleton; 
}

// ----------------------------------------------
void COTELiquidManager::DestroySingleton()
{	
	if(!s_pSingleton)
		return;

	if(CLiquidRQListener::s_pLiquidRQListener)
	{
		SCENE_MGR->removeRenderQueueListener(CLiquidRQListener::s_pLiquidRQListener);

		SAFE_DELETE(CLiquidRQListener::s_pLiquidRQListener)	
	}	

	s_pSingleton->Clear();	
	SAFE_DELETE(s_pSingleton)	
}

// ----------------------------------------------	
void COTELiquidManager::Clear()
{
	SAFE_DELETE_ARRAY(m_pLiquidMap)
	
	// 删除液体列表

	HashMap<int, COTELiquid*>::iterator it = m_LiquidMap.begin();
	while(it != m_LiquidMap.end())
	{
		delete(it->second);
		++ it;
	}
	m_LiquidMap.clear();
	
}

// ----------------------------------------------

COTELiquidManager::COTELiquidManager() :
m_pLiquidMap(NULL)
{	
}

// ----------------------------------------------
bool COTELiquidManager::Init()
{
	if(!CLiquidRQListener::s_pLiquidRQListener)
	{
		CLiquidRQListener::s_pLiquidRQListener = new CLiquidRQListener(Ogre::RENDER_QUEUE_SKIES_LATE, true);
		SCENE_MGR->addRenderQueueListener(CLiquidRQListener::s_pLiquidRQListener);
		
	}

	if(!m_pLiquidMap)
	{		
		int sizeX = ( COTETilePage::s_CurrentPage->m_PageSizeX ) / ( COTETilePage::s_CurrentPage->m_TileSize );
		int sizeZ = ( COTETilePage::s_CurrentPage->m_PageSizeZ ) / ( COTETilePage::s_CurrentPage->m_TileSize );

		m_pLiquidMap = new unsigned int [sizeX * sizeZ];
		::ZeroMemory(m_pLiquidMap, sizeX * sizeZ * sizeof(unsigned int));
	}

	return true;
}	


// ----------------------------------------------	
bool COTELiquidManager::GetLiquidValue(unsigned int &waterValue, float x , float z)
{
	if(!COTETilePage::s_CurrentPage)
		return false;

	COTETile* tile = COTETilePage::s_CurrentPage->GetTileAt(x, z);
	if(tile)	
	{
		return GetLiquidValue(waterValue, tile);	
	}
	return false;
}

// ----------------------------------------------	
bool COTELiquidManager::GetLiquidValue(unsigned int &waterValue, COTETile* tile)
{
	if(!m_pLiquidMap)
		return false;

	waterValue = m_pLiquidMap[tile->m_IndexInPage];
	return true;
}	

// ----------------------------------------------	
void COTELiquidManager::SetLiquidAt(float x, float z, unsigned int waterValue)
{	
	if(!m_pLiquidMap)
		return;

	COTETile* t = COTETilePage::s_CurrentPage->GetTileAt(x, z);	
	if(t)
	{	
		t->SetWater(waterValue);
		
		int size = ( COTETilePage::s_CurrentPage->m_PageSizeX ) / ( COTETilePage::s_CurrentPage->m_TileSize );
		Ogre::Vector3 tileScl = COTETilePage::s_CurrentPage->m_Scale * ( COTETilePage::s_CurrentPage->m_TileSize );
		
		m_pLiquidMap[int(z / tileScl.z) * size + int(x / tileScl.x)] = unsigned int(waterValue);
	}	

}

// ----------------------------------------------	
void COTELiquidManager::LoadLiquidFromFile(const std::string& rFileName)
{
	if(!m_pLiquidMap && !Init())
		return;

	int sizeX = ( COTETilePage::s_CurrentPage->m_PageSizeX ) / ( COTETilePage::s_CurrentPage->m_TileSize );
	int sizeZ = ( COTETilePage::s_CurrentPage->m_PageSizeZ ) / ( COTETilePage::s_CurrentPage->m_TileSize );

	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(rFileName);
	stream->read(m_pLiquidMap, sizeX * sizeZ * sizeof(unsigned int));
		
	const Ogre::Vector3& scl = COTETilePage::s_CurrentPage->m_Scale;

	for(int j = 0; j < sizeZ; j ++)
	for(int i = 0; i < sizeX; i ++)
	{
		unsigned int val = m_pLiquidMap[j * sizeX + i];

		if(val != 0) // 值
		{		
			SetLiquidAt( i * ( COTETilePage::s_CurrentPage->m_TileSize ) * scl.x, 
				j * ( COTETilePage::s_CurrentPage->m_TileSize ) * scl.z,	val );			
		}

	}
}

// ----------------------------------------------	
void COTELiquidManager::SaveLiquidToFile(const std::string& rFileName)
{
	if(!m_pLiquidMap)
		return;

	FILE* file = fopen(rFileName.c_str(), "wb");
	if(!file || !m_pLiquidMap)
	{
		OTE_MSG("水数据文件保存失败！", "失败")
		return;
	}

	int sizeX = ( COTETilePage::s_CurrentPage->m_PageSizeX ) / ( COTETilePage::s_CurrentPage->m_TileSize );
	int sizeZ = ( COTETilePage::s_CurrentPage->m_PageSizeZ ) / ( COTETilePage::s_CurrentPage->m_TileSize );
	fwrite( m_pLiquidMap, sizeX * sizeZ * sizeof(unsigned int), 1, file );	

	fclose(file);	
}

// ----------------------------------------------
// 创建一个默认的水信息图

void COTELiquidManager::CreateEmptyFile(const std::string& rFileName, int sizeX, int sizeZ)
{	
	FILE* file = fopen(rFileName.c_str(), "wb");
	if(!file)
	{
		OTE_MSG("创建水数据文件失败！", "失败")
		return;
	}

	int size_x = sizeX / ( DEFAULT_TILESIZE - 1 );
	int size_z = sizeZ / ( DEFAULT_TILESIZE - 1 );

	if(!m_pLiquidMap)
	{
		m_pLiquidMap = new unsigned int [size_x * size_z];
		::ZeroMemory(m_pLiquidMap, size_x * size_z * sizeof(unsigned int));
		
	}

	fwrite( m_pLiquidMap, size_x * size_z * sizeof(unsigned int), 1, file );

	fclose(file);	
}

// ----------------------------------------------	
void COTELiquidManager::InvalidateDevice()
{
	HashMap<int, COTELiquid*>::iterator it = m_LiquidMap.begin();
	while(it != m_LiquidMap.end())
	{
		it->second->m_Render->InvalidateDevice();
		++ it;
	}
}
// ----------------------------------------------	
void COTELiquidManager::Render()
{	
	SCENE_MGR->GetRenderSystem()->_disableTextureUnitsFrom(0);			
	COTEFog::GetInstance()->SetFogEnable(true);

	HashMap<int, COTELiquid*>::iterator it = m_LiquidMap.begin();
	while(it != m_LiquidMap.end())
	{
		it->second->m_Render->Render();
		++ it;
	}	
}

// ----------------------------------------------	
COTELiquid* COTELiquidManager::GetLiquid(int waterValue)
{
	COTELiquid* ret;
	
	HashMap<int, COTELiquid*>::iterator it = m_LiquidMap.find(waterValue);
	if(it == m_LiquidMap.end())
		ret = CreateLiquid(waterValue);
	else
		ret = it->second;

	return ret;
}


// ----------------------------------------------	
// 创建液体 根据类型的不同

COTELiquid* COTELiquidManager::CreateLiquid(int waterValue)
{	
	int type = GET_WATERTYPE(waterValue);

	CLiquidRender* lr = NULL;

	if(type == COTELiquid::eOcean_water)
	{
		lr = new COTEOceanWaterRender();
		OTE_TRACE("创建海水");
	}

	// 其他的都是湖水

	else
	{
		lr = new COTELakeLiquidRender();
		OTE_TRACE("创建湖水" << type);			
	}

	// 创建到类型列表

	COTELiquid* ld = new COTELiquid(lr);

	m_LiquidMap[waterValue] = ld;

	return ld;
}

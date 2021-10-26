#include "OTEScreenRect.h"
#include "OTED3DManager.h" 
#include "OgreD3D9RenderWindow.h"

using namespace Ogre;
using namespace OTE;

class COTE2DRenderListener;

// ============================================
int	COTERect2DRender::c_MaxPlanes	= 512;			// 最大片数量

COTERect2DRender* COTERect2DRender::s_pSingleton = NULL;
// --------------------------------------------
COTERect2DRender* COTERect2DRender::GetSingleton()
{
	if(!s_pSingleton) 
	{
		s_pSingleton = new COTERect2DRender();		
	}
	return s_pSingleton; 
}

// --------------------------------------------
void COTERect2DRender::DestroySingleton()
{
	if(!s_pSingleton)
		return;

	/*if(COTE2DRenderListener::s_p2DRenderListener)
	{
		SCENE_MGR->removeRenderQueueListener(COTE2DRenderListener::s_p2DRenderListener);

		SAFE_DELETE(COTE2DRenderListener::s_p2DRenderListener)	
	}	*/

	s_pSingleton->Clear();	
	SAFE_DELETE(s_pSingleton)	

}

// --------------------------------------------
COTERect2DRender::COTERect2DRender() :
m_D3D9IndexBuffer(NULL)
{
	
}

// --------------------------------------------
COTERect2DRender::~COTERect2DRender()
{
	
}

// --------------------------------------------
// 初始化 

void COTERect2DRender::Init()
{
	m_State					=	E_FAILED;

	m_D3D9IndexBuffer		=	NULL;

}

// --------------------------------------------
// 渲染

void COTERect2DRender::Render()
{
	g_pDevice->SetPixelShader(0);
	g_pDevice->SetVertexShader(0);

	D3DXMATRIX matrix;
	D3DXMatrixIdentity(&matrix);
	g_pDevice->SetTransform(D3DTS_WORLD,		&matrix);

	g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
	g_pDevice->SetRenderState(D3DRS_ZENABLE, true);
	g_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
	g_pDevice->SetRenderState(D3DRS_LIGHTING, false);
	g_pDevice->SetRenderState(D3DRS_FOGENABLE, true);

	g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	g_pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	g_pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	
	// 贴图

	g_pDevice->SetTexture(0, m_Texture->getNormTexture());

	g_pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
		
	g_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);	

	g_pDevice->SetSamplerState(1, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
	g_pDevice->SetSamplerState(1, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);

	g_pDevice->SetStreamSource(0, m_D3D9VertexBuffer, 0, sizeof(Vertex2D_t));
	g_pDevice->SetIndices(m_D3D9IndexBuffer);
	g_pDevice->SetFVF( Vertex2D_t::FVF);	

	g_pDevice->DrawIndexedPrimitive(
								D3DPT_TRIANGLELIST,
								0,
								0,
								m_PoolCount * 4,
								0,
								m_PoolCount * 2
								);

	g_pDevice->SetPixelShader(0);
	g_pDevice->SetVertexShader(0);
}

// -------------------------------------------------------
void COTERect2DRender::InvalidateDevice()
{		
	m_State = E_LOSTDEVICE;
}

// --------------------------------------------
// 摧毁 

void COTERect2DRender::Destroy()
{
	std::map<std::string, CRect2DElm_t*>::iterator it = m_Rect2DList.begin();
	while(it != m_Rect2DList.end())
	{
		delete (*it).second;

		++it;
	}

	m_Rect2DList.clear();
}

// --------------------------------------------
// 创建矩形

void COTERect2DRender::CreateRect(const std::string& rName,
								  float x1, float y1, 
								  float x2, float y2,
								  const Ogre::ColourValue& color
								  )
{
	CRect2DElm_t* re = new CRect2DElm_t(x1, y1, x2, y2, color);
	m_Rect2DList[rName] = re;
}

// ----------------------------------------------
void COTERect2DRender::DeleteRect(const std::string& rName)
{
	std::map<std::string, CRect2DElm_t*>::iterator it = m_Rect2DList.find(rName);
	if(it == m_Rect2DList.end())
	{
		delete (*it).second;
		m_Rect2DList.erase(it);
	}
}

// ----------------------------------------------
bool COTERect2DRender::CreateIndexBuffer(int maxPlanes)
{
	if(m_D3D9IndexBuffer == NULL)
	{
		m_IndexBuffer = 
			HardwareBufferManager::getSingleton().createIndexBuffer(
			HardwareIndexBuffer::IT_16BIT,
			maxPlanes * 6, 
			HardwareBuffer::HBU_STATIC_WRITE_ONLY);
		
		D3D9HardwareIndexBuffer* ibuf =  static_cast<D3D9HardwareIndexBuffer*>(m_IndexBuffer.getPointer());
	
		m_D3D9IndexBuffer = ibuf->getD3DIndexBuffer();		

		WORD* indices = 0;
		m_D3D9IndexBuffer->Lock(0, 0, (void**)&indices, 0);		

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

		m_D3D9IndexBuffer->Unlock(); 
	}
	
	return true;

}

// --------------------------------------------------------
void COTERect2DRender::Clear()
{
	// 清理

	m_IndexBuffer.setNull();

	m_D3D9IndexBuffer = NULL;
	
	m_VertexBuffer.setNull();
	
	m_D3D9VertexBuffer = NULL;

	m_State = E_UNINIT;
}

// --------------------------------------------------------
void COTERect2DRender::Update(CRect2DElm_t* we)
{
	//TIMECOST_START

	// 顶点缓存池是否已经被锁定

	if(!m_D3D9VertexBuffer)
	{
		const Ogre::D3D9HardwareVertexBuffer* d3dvb = static_cast<const Ogre::D3D9HardwareVertexBuffer*>(m_VertexBuffer.getPointer());
		m_D3D9VertexBuffer = d3dvb->getD3D9VertexBuffer();
		D3D9HardwareIndexBuffer* ibuf =  static_cast<D3D9HardwareIndexBuffer*>(m_IndexBuffer.getPointer());
		m_D3D9IndexBuffer = ibuf->getD3DIndexBuffer();	
	}

	if(!m_VertexBufferPoolLocked)
	{	
		int copyByte = sizeof(CRect2DElm_t) * c_MaxPlanes;
		m_D3D9VertexBuffer->Lock(0, copyByte, (void**)&m_pVBuf, 0);	
		m_VertexBufferPoolLocked = true;
	}	
	
	// 拷贝到显存

	we->m_PoolIndex = m_PoolCount;	

	if(we->m_PoolIndex < c_MaxPlanes && m_pVBuf) // 小于池容积?
	{
		memcpy(m_pVBuf + we->m_PoolIndex * sizeof(CRect2DElm_t::Rect2DPlane_t), 
						&we->m_Plane, sizeof(CRect2DElm_t::Rect2DPlane_t)
						);
		m_PoolCount ++;	
	}
	else
	{
		OTE_LOGMSG("片数量大于池容积！");
	}

	
	//TIMECOST_END
}

//
//
//// =============================================
//// COTE2DRenderListener
//// =============================================
//namespace OTE
//{
//class COTE2DRenderListener : public Ogre::RenderQueueListener
//{
//public:	
//
//	COTE2DRenderListener(Ogre::uint8 queue_id, bool post_queue)
//	{		
//		m_queue_id		= queue_id;
//		m_post_queue	= post_queue;
//	}
//
//	virtual ~COTE2DRenderListener() 
//	{
//	}
//
//	virtual void renderQueueStarted(Ogre::RenderQueueGroupID id, bool& skipThisQueue)
//	{
//		if ((m_post_queue) && m_queue_id == id)
//		{				
//
//			TIMECOST_START
//
//			if(IS_DEVICELOST) // 设备丢失
//			{
//				//COTERect2DRender
//			}
//
//			// 渲染水
//
//			COTELiquidManager::GetSingleton()->Render();	
//
//			TIMECOST_END
//		}
//	}
//
//	virtual void renderQueueEnded(Ogre::RenderQueueGroupID id, bool& repeatThisQueue){}
//
//	// methods for adjusting target queue settings
//	void	setTargetRenderQueue(Ogre::uint8 queue_id)		{m_queue_id = queue_id;}
//	void	setPostRenderQueue(bool post_queue)		{m_post_queue = post_queue;}
//
//public:
//
//	static COTE2DRenderListener*	s_p2DRenderListener;
//
//private:
//	
//	Ogre::uint8						m_queue_id;		
//	bool							m_post_queue;	
//
//};
//
//}
//
//COTE2DRenderListener*	COTE2DRenderListener::s_p2DRenderListener = NULL;



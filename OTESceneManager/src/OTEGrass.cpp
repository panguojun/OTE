/************************************************************
 ************************************************************

 模块名称： 草效果
 相关模块： 
						
 描述    ： 制作草效果 采用alpha test方式，把草分成多簇 每一簇
            为一个渲染单元，并且把每一株草的位置在shader里实现，
			簇的数量固定,多余部分的草被放到零点!
			这样一个寄存器可以存储一株草信息。
			这种方法的优点是避免了lock，缺点是由于分成多簇，如果
			采用alpha 混合的方式 排序可能会有问题。

 *************************************************************
 *************************************************************/

#include "OTEGrass.h"
#include "OTED3DManager.h"
#include "OgreD3D9HardwareVertexBuffer.h" 
#include "OgreD3D9HardwareIndexBuffer.h" 

#ifdef __OTE

#include "OTEQTSceneManager.h" 

#endif

using namespace Ogre; 
using namespace	OTE;

// --------------------------------------------------------
#define				G_DEBUG

// =========================================================
// SingleGrassVertex
// =========================================================

void SingleGrassVertex::SetGrass(const void* pos, unsigned char grassID, unsigned char lum)
{	
	Pos = *(D3DXVECTOR3*) pos;
	Index =	grassID - 1; // 传入ID从1开始,而零代表无效
	Lum = lum;
}

// =========================================================
// CGrassGroup
// =========================================================

const int	  CGrassGroup::c_ElmCount =	3;

// --------------------------------------------------------
bool  CGrassGroup::InitBuffer()
{
	VertexData vertexData;

	vertexData.vertexCount = c_ElmCount * 4 * GRASS_GROUP_SIZE;
    vertexData.vertexStart = 0;

    // Vertex declaration
    VertexDeclaration* decl = vertexData.vertexDeclaration;
    VertexBufferBinding* binding = vertexData.vertexBufferBinding;

    size_t offset = 0;
    decl->addElement(0, offset, VET_FLOAT3, VES_POSITION);
    offset += VertexElement::getTypeSize(VET_FLOAT3);
	decl->addElement(0, offset, VET_FLOAT3, VES_NORMAL);
    offset += VertexElement::getTypeSize(VET_FLOAT3);
    decl->addElement(0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);

    m_MainBuf = HardwareBufferManager::getSingleton().createVertexBuffer(
						decl->getVertexSize(0),
						vertexData.vertexCount, 
						HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

    // bind position and diffuses
    binding->setBinding(0, m_MainBuf);

	const Ogre::D3D9HardwareVertexBuffer* d3dvb = static_cast<const Ogre::D3D9HardwareVertexBuffer*>(m_MainBuf.getPointer());
	m_GrassVertexBuffer = d3dvb->getD3D9VertexBuffer();	
	
	BeginBillboards();

		for(int i = 0; i < GRASS_GROUP_SIZE; i ++)	// 生成64个单元草
		{
			m_ParentGrass->CreateGrassShape(this, i);	
		}

	EndBillboards();

	return true;
}

//-----------------------------------------------------------------------
void CGrassGroup::BeginBillboards(void)
{    
	m_LockPtr = static_cast<float*>( m_MainBuf->lock(HardwareBuffer::HBL_DISCARD) );
	
}

// -----------------------------------------------------------------------
void CGrassGroup::EndBillboards(void)
{
	m_MainBuf->unlock();	
}

// -----------------------------------------------------------------------
void CGrassGroup::GenVertices(  const Ogre::Vector3& position, 
								const Ogre::Radian& rotation,
								int groupIndex,
								int IndexInGroup)
{	
	static const float		c_grass_unit_cnt    = 4.0f; // 1/4的默认单元尺寸
	
	// 尺寸

	static const float boardHeight = 1.6f;

	static const float boardWidth  = 1.6f;


	// 一个贴图单元的UV

    static float basicTexData[8] = {
        0.0f,							0.0f,
        1.0f / c_grass_unit_cnt,		0.0f,
		0.0f,							1.0f / c_grass_unit_cnt,
        1.0f / c_grass_unit_cnt,		1.0f / c_grass_unit_cnt
	}; 

	float* pTexData = basicTexData;

    const float  cos_rot  ( Math::Cos(rotation) );
    const float  sin_rot  ( Math::Sin(rotation) );	

	//RGBA* pCol;	

    // Left-top
	// Positions
	*m_LockPtr++ = position.x - boardWidth / 2.0f * cos_rot;
    *m_LockPtr++ = position.y + boardHeight;
    *m_LockPtr++ = position.z - boardWidth / 2.0f * sin_rot;

	// normal
    *m_LockPtr++ = sin_rot;
    *m_LockPtr++ = IndexInGroup;
    *m_LockPtr++ = -cos_rot; 

	// Texture coords
	*m_LockPtr++ = *pTexData++;
	*m_LockPtr++ = *pTexData++;

	// Right-top
	// Positions
    *m_LockPtr++ = position.x + boardWidth / 2.0f * cos_rot;
    *m_LockPtr++ = position.y + boardHeight;
    *m_LockPtr++ = position.z + boardWidth / 2.0f * sin_rot;

	// normal
    *m_LockPtr++ = sin_rot;
    *m_LockPtr++ = IndexInGroup;
    *m_LockPtr++ = -cos_rot; 

	// Texture coords
	*m_LockPtr++ = *pTexData++;
	*m_LockPtr++ = *pTexData++;

	// Left-bottom
	// Positions
    *m_LockPtr++ = position.x - boardWidth / 2.0f * cos_rot;
    *m_LockPtr++ = position.y;
    *m_LockPtr++ = position.z - boardWidth / 2.0f * sin_rot;

	// normal
    *m_LockPtr++ = sin_rot;
    *m_LockPtr++ = IndexInGroup;
    *m_LockPtr++ = -cos_rot; 

	// Texture coords
	*m_LockPtr++ = *pTexData++;
	*m_LockPtr++ = *pTexData++;

	// Right-bottom
	// Positions
    *m_LockPtr++ = position.x + boardWidth / 2.0f * cos_rot;
    *m_LockPtr++ = position.y;
    *m_LockPtr++ = position.z + boardWidth / 2.0f * sin_rot;

	// normal
    *m_LockPtr++ = sin_rot;
    *m_LockPtr++ = IndexInGroup;
    *m_LockPtr++ = -cos_rot; 

	// Texture coords
	*m_LockPtr++ = *pTexData++;
	*m_LockPtr++ = *pTexData++;

}

// =========================================================
// COTEGrassCPURender
// =========================================================

bool COTEGrassCPURender::CreateEffect(COTEGrass* pGrass)
{
	if(pGrass->m_pEffect)
		return true;

	static const std::string c_grass_fx			= "grassMaterial_cpuRender.fx";	// 草的shader
	
	Ogre::DataStreamPtr stream = RESMGR_LOAD(c_grass_fx);
	
	if(stream.isNull())
	{
		OTE_LOGMSG("没有找到草的FX文件: " << c_grass_fx)
		return false;
	}

	// 创建shader

	std::string effectstring = RESMGR_LOAD(c_grass_fx)->getAsString();
 
	LPD3DXBUFFER errBuf;
	D3DXCreateEffect(g_pDevice,
		effectstring.c_str(),
		effectstring.length(),
		NULL,
		NULL,
		D3DXSHADER_DEBUG,
		NULL,
		&pGrass->m_pEffect,
		&errBuf);

	if(errBuf)
	{
		 OTE_MSG((char*)errBuf->GetBufferPointer(), "失败")
		 return false;	
	}	

	return true;
}

// ---------------------------------------------------------
void COTEGrassCPURender::RenderGrass(void* pViewProjMat, COTEGrass* pGrass)
{
	// TODO
}

// =========================================================
// COTEGrassGPURender
// =========================================================

bool COTEGrassGPURender::CreateEffect(COTEGrass* pGrass)
{
	if(pGrass->m_pEffect)
		return true;

	static const std::string c_grass_fx			= "grassMaterial.fx";	// 草的shader

	Ogre::DataStreamPtr stream = RESMGR_LOAD(c_grass_fx);
	
	if(stream.isNull())
	{
		OTE_LOGMSG("没有找到草的FX文件: " << c_grass_fx)
		return false;
	}

	// 创建shader

	std::string effectstring = stream->getAsString();
 
	LPD3DXBUFFER errBuf;
	D3DXCreateEffect(g_pDevice,
		effectstring.c_str(),
		effectstring.length(),
		NULL,
		NULL,
		D3DXSHADER_DEBUG,
		NULL,
		&pGrass->m_pEffect,
		&errBuf);

	if(errBuf)
	{
		 OTE_MSG((char*)errBuf->GetBufferPointer(), "失败")
		 return false;	
	}	

	return true;
}

// ---------------------------------------------------------
void COTEGrassGPURender::RenderGrass(void* pViewProjMat, COTEGrass* pGrass)
{
	float squard_dis0 = pGrass->m_MaxSquredDistance * 0.5f;

	// 渲染状态

	RENDERSYSTEM->_disableTextureUnitsFrom(0);	

	g_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	g_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE		);	
	
	g_pDevice->SetRenderState( D3DRS_AMBIENT, 0xFFFFFFFF	);

	g_pDevice->SetRenderState( D3DRS_FOGENABLE, false		);	

	g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false); // 半透明! 比较影响效率的地方
	//g_pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA );
	//g_pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);	

	g_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, true		);
	g_pDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER	);		
	g_pDevice->SetRenderState( D3DRS_ALPHAREF,		40			);		

	g_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, 1		); 
	g_pDevice->SetRenderState( D3DRS_ZENABLE, 1				); 
	g_pDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	g_pDevice->SetSamplerState( 0,D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    g_pDevice->SetSamplerState( 0,D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    g_pDevice->SetSamplerState( 0,D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	g_pDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DTA_TEXTURE	);

	g_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE		);
	g_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE		);
	g_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE2X	); 

	g_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE		);
	g_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE		);
	g_pDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE2X	);

	g_pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0				);

	g_pDevice->SetFVF( D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1					); // 手工

	/// Effect参数

	pGrass->m_pEffect->SetMatrix ("matVP",(D3DXMATRIX*)pViewProjMat			);

	pGrass->m_pEffect->SetFloat  ("time", COTED3DManager::s_ElapsedTime	); 

	if(!pGrass->m_GrassTexture.isNull())
		pGrass->m_pEffect->SetTexture("picTex",	pGrass->m_GrassTexture->getNormTexture());	

//#ifdef G_DEBUG
//char buff[32];
//int grassCnt = 0;	
//int triCnt = 0;
//#endif
	
	const Ogre::Vector3& camPos = SCENE_CAM->getPosition();	

	for(int i = 0; i < pGrass->m_GrassCount / GRASS_GROUP_SIZE; i ++)
	{		
		CGrassGroup* gg = pGrass->m_GrassGroups[i];

		const D3DXVECTOR4& groupPos = gg->m_ShaderData[0];

		float square_dis =
			(camPos.x - groupPos.x) * (camPos.x - groupPos.x) +
			(camPos.y - groupPos.y) * (camPos.y - groupPos.y) +
			(camPos.z - groupPos.z) * (camPos.z - groupPos.z) ;
		
		// 根据距离调整透明度

		pGrass->m_pEffect->SetFloat("alpha", square_dis <= squard_dis0 ? 1.0f : squard_dis0 / square_dis);

		pGrass->m_pEffect->SetVectorArray("grassData", gg->m_ShaderData, GRASS_GROUP_SIZE);
        		 
		UINT pass;
		pGrass->m_pEffect->Begin(&pass, D3DXFX_DONOTSAVESTATE);		 
		pGrass->m_pEffect->BeginPass(0);

		g_pDevice->SetStreamSource(0, gg->m_GrassVertexBuffer, 0, gg->m_MainBuf->getVertexSize() );
		g_pDevice->SetIndices(pGrass->m_GrassIDBuffer);
		g_pDevice->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			0,
			0,
			GRASS_GROUP_SIZE * CGrassGroup::c_ElmCount * 4,
			0,
			GRASS_GROUP_SIZE * CGrassGroup::c_ElmCount * 2);		

		pGrass->m_pEffect->EndPass();	 
		pGrass->m_pEffect->End();

//#ifdef G_DEBUG
//triCnt += GRASS_GROUP_SIZE * CGrassGroup::c_ElmCount * 2;
//grassCnt += GRASS_GROUP_SIZE;
//#endif
	}

//#ifdef G_DEBUG
//sprintf(buff, " grassCnt: %d grassTriCnt: %d", grassCnt, triCnt);
//SCENE_MGR_OGRE->m_RenderInfoList["triCnt"] = buff;
//
//sprintf(buff, " grassRenderGroupCnt: %d", m_GrassCount / GRASS_GROUP_SIZE);
//SCENE_MGR_OGRE->m_RenderInfoList["grassRenderGroupCnt"] = buff;
//#endif
	
	pGrass->m_GrassCount	 = 0;

	g_pDevice->SetPixelShader(0);
	g_pDevice->SetVertexShader(0);

}

// ========================================================
// COTEGrass
// ========================================================
COTEGrass* COTEGrass::s_pSingleton = NULL;
COTEGrass* COTEGrass::GetSingleton()
{
	if(!s_pSingleton) 
	{
		s_pSingleton = new COTEGrass();		
	}
	return s_pSingleton; 
}

// ----------------------------------------------
void COTEGrass::DestroySingleton()
{	
	SAFE_DELETE(s_pSingleton)	
}


// --------------------------------------------------------
COTEGrass::COTEGrass()
{	
	m_GrassCount			= 0; 
	m_pEffect				= NULL;
	m_GrassState			= E_GRASSINIT_FAIL;
	
	m_GrassIDBuffer			= NULL;
	m_pCurGrass				= NULL;
	m_GrassRender			= NULL;

	m_MaxSquredDistance		= 10000.0f;	// 最远可视距离
}

// --------------------------------------------------------
COTEGrass::~COTEGrass()
{
	Clear();
}

// --------------------------------------------------------
bool COTEGrass::CreateIndexBuffer()
{
	if(m_GrassIDBuffer == NULL)
	{
		if(FAILED(g_pDevice->CreateIndexBuffer(
			GRASS_GROUP_SIZE * CGrassGroup::c_ElmCount * 6 * sizeof(WORD),
			0,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&m_GrassIDBuffer,
			0)))
		{			
			return false;
		}			

		/* 
			2-----3
			|    /|
			|  /  |
			|/    |
			0-----1
		*/
		
		WORD* pIdx;

		m_GrassIDBuffer->Lock(0, 0, (void**)&pIdx, 0);

		for(int idx, idxOff, bboard = 0;  bboard < GRASS_GROUP_SIZE * 3; ++ bboard)
		{
			// Do indexes
			idx    = bboard * 6;
			idxOff = bboard * 4;

			pIdx[idx]   = static_cast<unsigned short>(idxOff	);
			pIdx[idx+1] = static_cast<unsigned short>(idxOff + 1);
			pIdx[idx+2] = static_cast<unsigned short>(idxOff + 3);
			pIdx[idx+3] = static_cast<unsigned short>(idxOff + 0);
			pIdx[idx+4] = static_cast<unsigned short>(idxOff + 3);
			pIdx[idx+5] = static_cast<unsigned short>(idxOff + 2);

		}

		m_GrassIDBuffer->Unlock();

	}

	return true;
}

// -------------------------------------------------------
void COTEGrass::InvalidateDevice()
{	
	for(unsigned int i = 0; i < m_GrassGroups.size(); i ++)
	{
		delete m_GrassGroups[i];
	}
	m_GrassGroups.clear();

	SAFE_RELEASE(m_GrassIDBuffer);
	SAFE_RELEASE(m_pEffect);
	
	m_GrassState = E_LOSTDEVICE;
}

// --------------------------------------------------------
void COTEGrass::ClearRenderContents()
{
	m_GrassTexture.setNull();	

	for(unsigned int i = 0; i < m_GrassGroups.size(); i ++)
	{
		delete m_GrassGroups[i];
	}
	m_GrassGroups.clear();
}

// --------------------------------------------------------
void COTEGrass::Clear()
{	
	m_GrassTexture.setNull();	

	for(unsigned int i = 0; i < m_GrassGroups.size(); i ++)
	{
		delete m_GrassGroups[i];
	}
	m_GrassGroups.clear();

	SAFE_RELEASE(m_GrassIDBuffer);
	SAFE_RELEASE(m_pEffect);	

	m_GrassState = E_GRASSINIT_UNINIT; // 清理后需要初始化
}

// --------------------------------------------------------
// 注意：这个函数会在设备丢失时多次调用!

void COTEGrass::InitGrass()
{
	// 检测设备

	if(!COTED3DManager::CheckDevCap(2, 0))	
	{
		m_GrassState = E_GRASSINIT_FAIL;
		return;
	}
	
	// 索引

	if(!CreateIndexBuffer())
	{
		m_GrassState = E_GRASSINIT_FAIL;
		return;
	}

	// 设置渲染器

	if(!m_GrassRender)
	{
		// 这里根据需要模块内部自己决定使用哪个渲染器

		if(true)
			m_GrassRender = new COTEGrassGPURender();	
		else
			m_GrassRender = new COTEGrassCPURender();
	}

	// 创建effect

	if(!m_GrassRender->CreateEffect(this))
	{
		m_GrassState = E_GRASSINIT_FAIL;
		return;
	}

	m_GrassState = E_GRASSINIT_SUCCESS;
}

// -------------------------------------------------------
void COTEGrass::AddGrass(const SingleGrassVertex* pRenderGrassList, int grassListCount)
{
#ifdef G_DEBUG
if(::GetKeyState('G') & 0x80 && ::GetKeyState(VK_CONTROL) & 0x80)
{	
	return;
}
#endif

	if	(m_GrassState == E_GRASSINIT_UNINIT	)
		InitGrass();

	if	(m_GrassState == E_GRASSINIT_FAIL	)
		return;	

	int end = (grassListCount / GRASS_GROUP_SIZE + int(grassListCount % GRASS_GROUP_SIZE > 0)) * GRASS_GROUP_SIZE;

	for(int i = 0; i < end; i ++)
	{	
		// 计算当前grassgroup

		int grassGroupCnt = m_GrassCount / GRASS_GROUP_SIZE;
		int grassGroupInd = m_GrassCount % GRASS_GROUP_SIZE;	

		if(grassGroupCnt >= int(m_GrassGroups.size()))
		{
			m_pCurGrass = new CGrassGroup(grassGroupCnt, this);
			m_GrassGroups.push_back(m_pCurGrass);			
		}
		else
		{	
			m_pCurGrass = m_GrassGroups[grassGroupCnt];	
			
		}			
	
		if(i < grassListCount)
		{
			const D3DXVECTOR3& pos = pRenderGrassList[i].Pos;

			m_pCurGrass->m_ShaderData[grassGroupInd].x = pos.x; 
			m_pCurGrass->m_ShaderData[grassGroupInd].y = pos.y; 
			m_pCurGrass->m_ShaderData[grassGroupInd].z = pos.z; 

			// 按照 0xFFFFFF(亮度)FF(索引) 方式组织
			// 注意这里限制了Index必须小于 256!

			m_pCurGrass->m_ShaderData[grassGroupInd].w = 
										pRenderGrassList[i].Index + 
								256 *   pRenderGrassList[i].Lum
								; 
		}
		else
		{
			m_pCurGrass->m_ShaderData[grassGroupInd].x = 0;
			m_pCurGrass->m_ShaderData[grassGroupInd].y = 0;
			m_pCurGrass->m_ShaderData[grassGroupInd].z = 0;
			m_pCurGrass->m_ShaderData[grassGroupInd].w = 0; // ?
		}

		m_GrassCount += 1;		
	}
	
}

// -------------------------------------------------------
void COTEGrass::RestoreDevice()
{	
	InitGrass();		
}

// -------------------------------------------------------
const Ogre::String& COTEGrass::GetTextureName()
{
	static const Ogre::String c_emptyString = "";
	if(m_GrassTexture.isNull())
		return c_emptyString;

	return m_GrassTexture->getName();
}
// --------------------------------------------------------
void COTEGrass::SetTexture(const Ogre::String& rTextureName)
{	
	if(!rTextureName.empty())
	{		
		m_GrassTexture = TEX_RESMGR_LOAD(rTextureName);
	}
}

// --------------------------------------------------------
void COTEGrass::RenderGrass(void* pMat)
{
#ifdef G_DEBUG
if(::GetKeyState('G') & 0x80 && ::GetKeyState(VK_CONTROL) & 0x80)
{
	m_GrassCount	 = 0;
	return;
}
#endif
	if(m_GrassGroups.empty())
		return;

	if(m_GrassState == E_LOSTDEVICE)
		RestoreDevice();

	if(m_GrassState != E_GRASSINIT_SUCCESS)
		return;
	
	m_GrassRender->RenderGrass(pMat, this);
}

// ---------------------------------------------------------
void COTEGrass::CreateGrassShape(CGrassGroup* group, int index)
{	
	group->GenVertices(Ogre::Vector3( -0.4f - 0.2f,  0.0f,  0.0f),  Ogre::Radian(Ogre::Degree(0.0f   /*+ Math::RangeRandom(- 0.0f, 0.0f)*/)),	group->m_GroupIndex, index);
	group->GenVertices(Ogre::Vector3(  0.0f,  0.0f,  0.4f - 0.2f),  Ogre::Radian(Ogre::Degree(60.0f  /*+ Math::RangeRandom(- 0.0f, 0.0f)*/)),	group->m_GroupIndex, index);
	group->GenVertices(Ogre::Vector3(  0.4f + 0.2f,  0.0f,  0.0f),  Ogre::Radian(Ogre::Degree(120.0f /*+ Math::RangeRandom(- 0.0f, 0.0f)*/)),	group->m_GroupIndex, index);
}

#include "OTESeaboardLine.h"
#include "OTEEntityAutoLink.h"
#include "OTEQTSceneManager.h"
#include "OTETerrainHeightMgr.h"
#include "OTECollisionManager.h"

// ------------------------------------------
using namespace Ogre;
using namespace OTE;


// ==========================================
// COTESeaboardWave
// ==========================================

HardwareVertexBufferSharedPtr	COTESeaboardWave::s_VertexBuffer;
LPDIRECT3DVERTEXBUFFER9			COTESeaboardWave::s_pD3DVertexBuffer	= NULL;
LPDIRECT3DVERTEXBUFFER9			COTESeaboardWave::s_pEffect				= NULL;
int								COTESeaboardWave::c_MaxSBWPlanes		= 200;

// ------------------------------------------
COTESeaboardWave::COTESeaboardWave()
{
}

COTESeaboardWave::~COTESeaboardWave()
{
	Clear();
}

// ------------------------------------------
bool COTESeaboardWave::CreateVertexBuffer(int maxPlanes)
{
	if(!s_pD3DVertexBuffer)
	{
		s_VertexBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
					sizeof(LiquidVertex_t), 
					maxPlanes * SBW_NUM_VERTS, 
					HardwareBuffer::HBU_STATIC_WRITE_ONLY);	
		
		const Ogre::D3D9HardwareVertexBuffer* d3dvb = static_cast<const Ogre::D3D9HardwareVertexBuffer*>(s_VertexBuffer.getPointer());
		s_pD3DVertexBuffer = d3dvb->getD3D9VertexBuffer();
	}

	return true;
}

// ------------------------------------------
bool COTESeaboardWave::Init()
{
	// Effect

	if(!s_pEffect)
	{
		Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource("SeaboardWave.fx");
		std::string effectstring = stream->getAsString();
	 
		// ����shader

		LPD3DXBUFFER errBuf;
		D3DXCreateEffect(g_pDevice,
			effectstring.c_str(),
			effectstring.length(),
			NULL,
			NULL,
			D3DXSHADER_DEBUG,
			NULL,
			&s_pEffect,
			&errBuf);

		if(errBuf)
		{
			OTE_MSG_ERR((char*)errBuf->GetBufferPointer())
			return false;	
		}	
	}

	return true;
}


// ------------------------------------------
void COTESeaboardWave::Render()
{
	g_pDevice->SetPixelShader(0);
	g_pDevice->SetVertexShader(0);

	D3DXMATRIX matrix;
	D3DXMatrixIdentity(&matrix);
	g_pDevice->SetTransform(D3DTS_WORLD,		&matrix);

	g_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, false );
	g_pDevice->SetRenderState(D3DRS_ZENABLE, true);
	g_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
	g_pDevice->SetRenderState(D3DRS_LIGHTING, false);

	g_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	g_pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	g_pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	
	// ��ͼ

	g_pDevice->SetTexture(0, s_Texture->getNormTexture());

	g_pDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
		
	g_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);	

	g_pDevice->SetStreamSource(0, s_VertexBuffer, 0, sizeof(SBWVertex_t));
	g_pDevice->SetIndices(m_pD3DVertexBuffer);
	g_pDevice->SetFVF(SBWVertex_t::FVF);

	UINT pass;	

	m_pEffect->Begin(&pass, D3DXFX_DONOTSAVESTATE);		 
	m_pEffect->BeginPass(0);

	g_pDevice->DrawPrimitive(	D3DPT_LINESTRIP,
								0,
								0,
								m_PoolCount * SBW_NUM_VERTS,
								0,
								m_PoolCount * SBW_NUM_TRIS
								);

	m_pEffect->EndPass();	 
	m_pEffect->End();

	m_PoolCount = 0;

	g_pDevice->SetPixelShader(0);
	g_pDevice->SetVertexShader(0);

}

// ------------------------------------------
void COTESeaboardWave::Update(SBWPlane_t* sbwp)
{	
	// �������Դ�

	sbwp->poolIndex = s_PoolCount;	

	if(sbwp->poolIndex < c_MaxSBWPlanes) // С�ڳ��ݻ�?
	{
		memcpy(s_pD3DVertexBuffer + sbwp->poolIndex * sizeof(SBWPlane_t), sbwp, sizeof(SBWPlane_t));
		s_PoolCount ++;	
	}
}

// ------------------------------------------
void COTESeaboardWave::InvalidateDevice()
{
	
}

// ------------------------------------------
void COTESeaboardWave::Clear(bool invalidDevice = false)
{
	
}

// ==========================================
// COTESeaboardWaveRender
// ==========================================

#define SET_UV_AT(ux, uz, u, v, operFlag){UpdateEntities(ux, uz, u, v, operFlag);}
#define GET_UV_AT(ux, uz, u, v) {GetALEntityAtPoint(ux, uz, u, v);}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 ���ƴ�Ӳ���

       |    
	LT | RT    
   --------- 
    LB | RB	   
 	   |	 

  c_SmpEntUMap : u = fu(u', v')
  c_SmpEntVMap : v = fv(u', v')

  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
const float c_SmpEntUMap[][20] = {
	{0, 1, 1, 1,    
	 0, 1, 3, 3,   
	 0, 2, 0, 0,   
	 0, 3, 2, 3},	// ����LT	һ���Ӧ��ϵ�е�U����
	
	{1, 1, 2, 3,   
	 3, 1, 2, 3,    
	 1, 3, 2, 2,   
	 3, 1, 1, 3},	// ���� RT
	
	{0, 3, 1, 3,   
	 0, 1, 3, 1,  
	 0, 1, 1, 0,  
	 2, 1, 2, 3},	// ����	LB
	
	{0, 3, 2, 3, 
	 2, 1, 2, 3,  
	 1, 1, 2, 2,  
	 0, 3, 2, 1}	// ���� RB
};

const float c_SmpEntVMap[][20] = {
	{0, 0, 0, 1,    
	 1, 1, 1, 1,   
	 1, 3, 3, 0,   
	 3, 3, 3, 3},	// ����	һ���Ӧ��ϵ�е�V����
	
	{0, 0, 0, 0,  
	 3, 1, 1, 1,    
	 3, 0, 1, 0,    
	 1, 3, 1, 3},	// ����
	
	{1, 3, 3, 0,    
	 1, 1, 0, 1,   
	 2, 2, 2, 2,   
	 3, 3, 3, 3},	// ����
	
	{3, 1, 1, 0,    
	 3, 1, 1, 1,   
	 2, 2, 2, 2,   
	 3, 0, 3, 1}	// ����
};

// ---------------------------------------------------------------------
const float c_DecSmpEntUMap[][20] = {	
	{3, 2, 2, 3,   
	 0, 3, 2, 2,   
	 0, 1, 2, 3,    
	 2, 1, 1, 1},	// ����
	
	{0, 0, 3, 1,    
	 0, 2, 2, 0,    
	 0, 1, 2, 3,    
	 0, 0, 2, 0},	// ����
	
	{0, 1, 2, 2,    
	 0, 3, 2, 3,    
	 3, 2, 2, 3,    
	 0, 2, 0, 1},	// ����
	
	{0, 1, 2, 1,   
	 0, 3, 2, 1,    
	 0, 0, 3, 3,   
	 0, 1, 0, 3}	// ����
};

const float c_DecSmpEntVMap[][20] = {
	{2, 0, 0, 0,    
	 2, 0, 1, 1,    
	 2, 2, 2, 2,  
	 2, 3, 2, 3},	// ����
	
	{0, 0, 2, 2,  
	 1, 3, 2, 3, 
	 2, 2, 2, 2, 
	 3, 2, 3, 1},	// ����
	
	{0, 0, 0, 1, 
	 0, 1, 1, 1,  
	 2, 2, 2, 2,  
	 3, 0, 3, 0},	// ����
	
	{0, 0, 0, 3,   
	 1, 3, 0, 0,    
	 2, 2, 2, 2,    
	 0, 3, 1, 3}	// ����
};

// ======================================
// COTESeaboardLine
// ======================================

// �õ����λ���ϵ�ʵ��

Ogre::Entity* COTESeaboardLine::GetALEntityAtPoint(int uX, int uZ, int& u, int& v)
{	
	std::stringstream ent_ss;
	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;	    // entity name

	if( SCENE_MGR->IsEntityPresent(ent_ss.str()) )
	{
		Ogre::Entity* e = SCENE_MGR->GetEntity(ent_ss.str());	
		std::string mn = e->getMesh()->getName();			
		int tu, tv;
		
		if(sscanf(mn.c_str(), (m_MeshGroupNamePrefix + "_%d_%d").c_str(), &tu, &tv) == 2)
		{
			u = tu;
			v = tv;
			return e;
		}
	}
	else
	{
		Ogre::Vector3 pos;
		GetWorldPosition(uX, uZ, pos);

		if(m_LastBottomHgt < 0 || Ogre::Math::Abs(m_LastBottomHgt - pos.y) < 1.0f)
		{
			// ����Χ

			u = 3;
			v = 2;
		}
		else
		{
			// ���ڲ�

			u = 1;
			v = 1;
		}
	}
	return NULL;
}

// --------------------------------------	
// �ⲿ�ӿ�
void COTESeaboardLine::AutoLink(float X, float Z, unsigned int operFlag)
{
	// ����

	COTETilePage* page = COTETilePage::GetCurrentPage();
	if(page)
	{
		float tileSize = page->m_TileSize - 1.0f;
		m_Scale = page->m_Scale * Ogre::Vector3(tileSize, 1.0f, tileSize);
	}
	else
	{
		m_Scale = Ogre::Vector3::UNIT_SCALE;
	}

	// �Զ�����

	AutoLink(int(X / m_Scale.x),	 int(Z / m_Scale.z),  0, 0, CTile::LEFT , operFlag);
	AutoLink(int(X / m_Scale.x) + 1, int(Z / m_Scale.z),  0, 0, CTile::RIGHT, operFlag);
	AutoLink(int(X / m_Scale.x),	 int(Z / m_Scale.z) + 1, 0, 0, CTile::LEFTBOTTOM, operFlag);
	AutoLink(int(X / m_Scale.x) + 1, int(Z / m_Scale.z) + 1, 0, 0, CTile::RIGHTBOTTOM, operFlag);

}

// --------------------------------------
// ʵ������

void COTESeaboardLine::AutoLink(int uX, int uZ, int u, int v, short cornerType, unsigned int operFlag)
{
	// ��Ч��

	if(m_LastBottomHgt < 0				|| 
		m_MeshGroupNamePrefix.empty()	||
		m_EntGroupNamePrefix.empty() 
		)
		return;

	if( !(u  >= 0 && u  < 4) ||
		!(v  >= 0 && v  < 4)  )
		return;

	int uInd, vInd;	//���u vֵ����
			
	// ���һ�� 

	if(operFlag & AUTOLINK_OPER_ADD)
	{
		GET_UV_AT(uX, uZ, uInd, vInd) 
		if(uInd >= 4 && vInd > 1 && uInd < 8 && vInd < 4)	
		{
			uInd = 1;
			vInd = 1;
		}
		if(uInd < 4 && vInd < 4)
		{ 
			int u_Ind = c_SmpEntUMap[cornerType][uInd + vInd * 4];
			int v_Ind = c_SmpEntVMap[cornerType][uInd + vInd * 4];

			SET_UV_AT(uX, uZ, u_Ind, v_Ind, operFlag)			
		}
		else
		{		
			SET_UV_AT(uX, uZ, u, v, operFlag)
		}
	}
	
	// ɾ��һ��

	else
	{	
		int uInd, vInd;
		GET_UV_AT(uX, uZ, uInd, vInd) 
		if(uInd >= 4 && vInd > 1 && uInd < 8 && vInd < 4)
		{
			uInd = 1;
			vInd = 1;
		}
		if(uInd < 4 && vInd < 4)
		{
			int u_Ind = c_DecSmpEntUMap[cornerType][uInd + vInd * 4];
			int v_Ind = c_DecSmpEntVMap[cornerType][uInd + vInd * 4];
		
			SET_UV_AT(uX, uZ, u_Ind, v_Ind, operFlag)			
		}		
	}
}

// --------------------------------------
// ����ƴ�ӵ�ʵ��ģ��
// ģ��Ԫ�����Ʒ��ӡ�%s_IndexU_IndexV��ʽ
// --------------------------------------
void COTESeaboardLine::UpdateEntities(int uX, int uZ, int u, int v, unsigned int operFlag)
{
	std::stringstream ss;
	ss << m_MeshGroupNamePrefix << "_" << u << "_" << v << ".mesh"; // mesh name
	std::stringstream ent_ss;
	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;	    // entity name	
	
	if( SCENE_MGR->IsEntityPresent(ent_ss.str()) )
	{
		SCENE_MGR->RemoveEntity(ent_ss.str());
	}	

	// ���µײ��߶�

	Ogre::Vector3 pos;
	GetWorldPosition(uX, uZ, pos);			
	
	//m_LastBottomHgt < 0 ? m_LastBottomHgt = pos.y : NULL;

	// ���� ����ת

	COTEActorEntity* ae = NULL;
	if(CHECK_RES(ss.str())) // ������
	{
		ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());
	}

	else	
	
	//////////// �����Ӧ����Դû���ҵ�����������Դƴ ////////////

	{
		/// ��[00]��ת�õ�[20],[02],[22]
		if(u == 2 && v == 0)
		{
			ss.str("");		ss << m_MeshGroupNamePrefix << "_" << 0 << "_" << 0 << ".mesh";
			ent_ss.str("");	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;
			ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());
			ae->getParentSceneNode()->yaw(Degree(90.0f));				
		}
		if(u == 0 && v == 2)
		{
			ss.str("");		ss << m_MeshGroupNamePrefix << "_" << 0 << "_" << 0 << ".mesh";
			ent_ss.str("");	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;
			ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());
			ae->getParentSceneNode()->yaw(Degree(-90.0f));				
		}
		if(u == 2 && v == 2)
		{
			ss.str("");		ss << m_MeshGroupNamePrefix << "_" << 0 << "_" << 0 << ".mesh";
			ent_ss.str("");	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;
			ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());
			ae->getParentSceneNode()->yaw(Degree(180.0f));				
		}

		/// ��[10]��ת�õ�[01],[12],[21]
		if(u == 0 && v == 1)
		{
			ss.str("");		ss << m_MeshGroupNamePrefix << "_" << 1 << "_" << 0 << ".mesh";	
			ent_ss.str("");	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;
			ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());
			ae->getParentSceneNode()->yaw(Degree(-90.0f));				
		}
		if(u == 1 && v == 2)
		{
			ss.str("");		ss << m_MeshGroupNamePrefix << "_" << 1 << "_" << 0 << ".mesh";	
			ent_ss.str("");	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;
			ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());
			ae->getParentSceneNode()->yaw(Degree(180.0f));			
		}
		if(u == 2 && v == 1)
		{
			ss.str("");		ss << m_MeshGroupNamePrefix << "_" << 1 << "_" << 0 << ".mesh";	
			ent_ss.str("");	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;
			ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());
			ae->getParentSceneNode()->yaw(Degree(-90.0f));				
		}

		/// ��[30]��ת�õ�[31],[33],[32]
		if(u == 3 && v == 1)
		{
			ss.str("");		ss << m_MeshGroupNamePrefix << "_" << 3 << "_" << 0 << ".mesh";	
			ent_ss.str("");	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;
			ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());
			ae->getParentSceneNode()->yaw(Degree(90.0f));				
		}
		if(u == 3 && v == 3)
		{
			ss.str("");		ss << m_MeshGroupNamePrefix << "_" << 3 << "_" << 0 << ".mesh";	
			ent_ss.str("");	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;
			ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());
			ae->getParentSceneNode()->yaw(Degree(180.0f));			
		}
		if(u == 2 && v == 3)
		{
			ss.str("");		ss << m_MeshGroupNamePrefix << "_" << 3 << "_" << 0 << ".mesh";	
			ent_ss.str("");	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;
			ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());
			ae->getParentSceneNode()->yaw(Degree(-90.0f));				
		}

		/// ��[03]��ת�õ�[13]
		if(u == 1 && v == 3)
		{
			ss.str("");		ss << m_MeshGroupNamePrefix << "_" << 0 << "_" << 3 << ".mesh";	
			ent_ss.str("");	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;
			ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());
			ae->getParentSceneNode()->yaw(Degree(90.0f));				
		}		

	}

	// λ��
	
	if( u == 1 && v == 1 && (operFlag & AUTOLINK_UPDATETERRAIN) )
	{			
		// ���� terrain tile �ĸ߶�

		COTETile* tile = COTETilePage::s_CurrentPage->GetTileAt(uX, uZ);
		if(tile)
		{
			float size = COTETilePage::s_CurrentPage->m_TileSize * COTETilePage::s_CurrentPage->m_Scale.x * 2.0f;
			float uhgt = COTETilePage::s_CurrentPage->m_Scale.y / 255.0f;

			COTETerrainHeightMgr::GetInstance()->SetHeightAt(
				COTETilePage::s_CurrentPage, 
				pos.x,				//x����
				pos.z,				//z����
				m_LastHgt - uhgt,	//y�߶�
				m_LastHgt - uhgt,	//ǿ��
				size,				//��Ȧ
				size,				//��Ȧ
				COTETerrainHeightMgr::eTerrain_Edit_ManualHeight_Rect);	//��������			
		}	

		// ��Ե��������

		UpdateBorder(uX, uZ, -1, 0, AUTOLINK_OPER_ADD);		// ��
		UpdateBorder(uX, uZ, 1, 0,  AUTOLINK_OPER_ADD);		// ��
		UpdateBorder(uX, uZ, 0, -1, AUTOLINK_OPER_ADD);		// ��
		UpdateBorder(uX, uZ, 0, 1,  AUTOLINK_OPER_ADD);		// ��
	}

	else	
	{	
		// �������е�

		if( !(operFlag & AUTOLINK_OPER_ADD) && 
			 (operFlag & AUTOLINK_UPDATETERRAIN)
			)
		{
			// ����  terrain tile �ĸ߶�
			
			COTETile* tile = COTETilePage::s_CurrentPage->GetTileAt(uX, uZ);
			if(tile)
			{
				float size = COTETilePage::s_CurrentPage->m_TileSize * COTETilePage::s_CurrentPage->m_Scale.x * 2.5f;
				
				COTETerrainHeightMgr::GetInstance()->SetHeightAt(
					COTETilePage::s_CurrentPage, 
					pos.x,				//x����
					pos.z,				//z����
					m_LastBottomHgt,	//y�߶�
					m_LastBottomHgt,	//ǿ��
					size,				//��Ȧ
					size,				//��Ȧ
					COTETerrainHeightMgr::eTerrain_Edit_ManualHeight_Rect);	//��������					
			}	

			UpdateBorder(uX, uZ, -1,  0,  0);  // ��
			UpdateBorder(uX, uZ,  1,  0,  0);  // ��
			UpdateBorder(uX, uZ,  0, -1,  0);  // ��
			UpdateBorder(uX, uZ,  0,  1,  0);  // ��		
		}
		
		if(ae)
		{			
			float yscale = m_Scale.x; /*temp*/
			ae->getParentSceneNode()->scale(m_Scale.x, yscale, m_Scale.z);
			
			if(operFlag & AUTOLINK_RAISE)
				m_LastHgt < 0 ? m_LastHgt = m_LastBottomHgt + ae->getBoundingBox().getMaximum().y * yscale : NULL;	
			else
				m_LastHgt < 0 ? m_LastHgt = m_LastBottomHgt - ae->getBoundingBox().getMinimum().y * yscale : NULL;	
			
			pos.y = m_LastBottomHgt;
			ae->getParentSceneNode()->setPosition(pos);	
		}
	}
}

// --------------------------------------
// ���±�Ե
void COTESeaboardLine::UpdateBorder(int uX, int uZ, int offsetX, int offsetZ, unsigned int operFlag)
{
	std::stringstream ent_ss;	ent_ss	<< m_EntGroupNamePrefix << "_border_" << offsetX << offsetZ << "_" << uX << "_" << uZ;

	if( SCENE_MGR->IsEntityPresent(ent_ss.str()) )
	{		
		SCENE_MGR->RemoveEntity(ent_ss.str());		
	}

	if(!(operFlag & AUTOLINK_OPER_ADD))
		return;

	Ogre::Vector3 pos;
	GetWorldPosition(uX, uZ, pos);	

	int u, v;
	if(GetALEntityAtPoint(uX + offsetX, uZ + offsetZ, u, v))
	{		
		// �ȼ���

		if( (offsetX == -1 && offsetZ == 0 ) && (u == 2 && v == 3 || u == 3 && v == 3) ){ u = 0; v = 1; }
		if( (offsetX == 0  && offsetZ == -1) && (u == 3 && v == 1 || u == 3 && v == 3) ){ u = 1; v = 0; }
		if( (offsetX == 1  && offsetZ == 0 ) && (u == 3 && v == 1 || u == 3 && v == 0) ){ u = 2; v = 1; }
		if( (offsetX == 0  && offsetZ == 1 ) && (u == 3 && v == 0 || u == 2 && v == 3) ){ u = 1; v = 2; }

		// ������Ե

		if( u == 0 && v == 1 ||
			u == 1 && v == 0 ||
			u == 2 && v == 1 ||
			u == 1 && v == 2)
		{
			std::stringstream ss;	ss << m_MeshGroupNamePrefix << "_border_" << u << "_" << v << ".mesh";	
			pos.y = m_LastBottomHgt;

			COTEActorEntity* ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());	
			ae->getParentSceneNode()->setPosition(pos);		
			ae->getParentSceneNode()->scale(m_Scale.x, m_Scale.x/*temp*/, m_Scale.z);
		}		
	}
}

// --------------------------------------
void COTESeaboardLine::GetWorldPosition(int uX, int uZ, Ogre::Vector3& position)
{
	position.x = float(uX) * m_Scale.x + m_Scale.x * 0.5f;
	position.z = float(uZ) * m_Scale.z + m_Scale.z * 0.5f;

	if(COTETilePage::GetCurrentPage())
		position.y = COTETilePage::GetCurrentPage()->GetHeightAt(position.x, position.z);
	else
		position.y = 0;	 // temp!
}
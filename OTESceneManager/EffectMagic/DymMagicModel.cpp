/***************************************************************************************************
文件名称:	DymMagicModel.cpp
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	模型
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#include ".\dymmagicmodel.h"
#include <stdio.h>

CDymMagicModel::CDymMagicModel(CDymRenderSystem *pDevice):CDymMagicBaseObj(pDevice,CDymMagicBaseObj::MOT_MODEL)
{
	m_strName[0] = 0;
	m_strMeshName[0] = 0;
	materialtype = MT_Effect_sceneno;
	materialres[0] = 0;
	// For model
	initsize = D3DXVECTOR3( 1 , 1 , 1 );
	isinfacex = false;
	isinfacey = false;
	isinfacez = false;
	m_nMeshBillBoardType = 0;
	facenum = 0;
	isskybox = false;
	ishavort = false;

	m_fAddTime = 0.0f;
	m_pVB = NULL;
	m_pVB2 = NULL;
	m_pMesh = NULL;
	m_pBillboard = NULL;
	mytype = MT_quad;
	m_pShadowVB = NULL;
	flyspeed = 5.0f;
	m_bShadowNeedSetPos = TRUE;
	shadow_render_type = 0;

	bUsingMeshVertexColor = false;

	m_pDevice = pDevice;
}

CDymMagicModel::~CDymMagicModel(void)
{
	if( m_pVB ) m_pVB->Release ();
	if( m_pVB2 ) m_pVB2->Release ();
	if( m_pBillboard ) delete m_pBillboard;
	if( m_pMesh ) m_pMesh->Release ();
	if( m_pShadowVB ) delete[] m_pShadowVB;
}

void CDymMagicModel::CopyModel( CDymMagicModel *p )
{
	_tcscpy( m_strName , p->m_strName );
	_tcscpy( m_strMeshName , p->m_strMeshName );
	materialtype = p->materialtype;
	_tcscpy( materialres , p->materialres );
	// For model
	initsize = p->initsize;
	isinfacex = p->isinfacex;
	isinfacey = p->isinfacey;
	isinfacez = p->isinfacez;
	facenum = p->facenum;
	isskybox = p->isskybox;
	ishavort = p->ishavort;

	mytype = p->mytype;
	CDymMagicBaseObj::CopyBaseObj ( p );
}

void CDymMagicModel::SetParticlesName(char* strName)
{
	strcpy( m_strName , strName );
	if( stricmp( m_strName , "spire" ) == 0 )
		mytype = MT_spire;
	else if( stricmp( m_strName , "quad" ) == 0 )
		mytype = MT_quad;
	else if( stricmp( m_strName , "mesh" ) == 0 )
		mytype = MT_mesh;
	else if( stricmp( m_strName , "shadow" ) == 0 )
		mytype = MT_shadow;
	else if( stricmp( m_strName , "spire_shadow" ) == 0 )
		mytype = MT_spire_shadow;
}

void CDymMagicModel::FrameMove(float fCurTime , float fElapsedTime)
{
	FrameMoveBase( fCurTime , fElapsedTime );
	if( mytype == MT_shadow )
	{
		D3DXVECTOR3 v1 = m_pShadowVB[0].p ;
		D3DXVECTOR3 v2 = m_pShadowVB[1].p ;

		for( int i=1;i<facenum+1;i++ )
		{
			m_pShadowVB[i*2+0].p -= ( m_pShadowVB[i*2+0].p - v1 ) / flyspeed ;
			m_pShadowVB[i*2+1].p -= ( m_pShadowVB[i*2+1].p - v2 ) / flyspeed ;
			v1 = m_pShadowVB[i*2+0].p;
			v2 = m_pShadowVB[i*2+1].p;
			/*D3DXVECTOR3 t1 = v1 , t2 = v2;
			v1 = m_pShadowVB[i*2+0].p;
			v2 = m_pShadowVB[i*2+1].p;
			m_pShadowVB[i*2+0].p = t1;
			m_pShadowVB[i*2+1].p = t2;*/
		}
	}
}

BOOL CDymMagicModel::IsAlphaRender()
{
	if( materialtype == MT_Effect_sceneno || materialtype == MT_Effect_sceneab )
		return FALSE;
	return TRUE;
}

HRESULT CDymMagicModel::Render(DYM_WORLD_MATRIX world , DWORD flag)
{
	CDymMagicKeyFrame *pKeyFrame = m_KeyFrameManager.GetKeyFrameNow ();
	if( pKeyFrame )
		if( pKeyFrame->animate == false && !(flag&MAGIC_RENDER_NO_HIDE) )return S_OK;
	if( !(flag&MAGIC_RENDER_DO_NOT_SETUP_RENDERSTATE) )
		SetupRenderState( world );

	if( flag == MAGIC_RENDER_SHADOW )
	{
		if( mytype == MT_spire_shadow )
		{
			if( this->shadow_render_type == 0 )
			{
				if( m_pDevice->m_pShadowEffect && m_pDevice->m_pRtShadow0 && m_pDevice->m_pRtShadow1 )
				{
					DWORD cullmode;
					m_pDevice->GetRenderState( D3DRS_CULLMODE , &cullmode );
					m_pDevice->SetRenderState ( D3DRS_ZWRITEENABLE , FALSE );
					LPDIRECT3DSURFACE9 pSurfaceOld , pSurface0 , pSurface1;
					m_pDevice->GetRenderTarget ( 0 , &pSurfaceOld );
					m_pDevice->m_pRtShadow0->GetSurfaceLevel ( 0 , &pSurface0 );
					m_pDevice->SetRenderTarget ( 0 , pSurface0 );
					m_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET, 
										D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0 );
		//			D3DXMATRIX matWorld,matScaling;
		//			D3DXMatrixTranslation( &matWorld , vPos.x , vPos.y-1 , vPos.z );
		//			D3DXMatrixScaling( &matScaling , fShadowRadio , 50.0f , fShadowRadio );
					
		//			m_pDevice->SetTransform ( D3DTS_WORLD , &(matScaling*matWorld) );
					m_pDevice->SetTexture ( 0 , NULL );

					m_pDevice->SetFVF(D3DFVF_XYZB1|D3DFVF_TEX1);
					m_pDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CCW );
					m_pDevice->SetStreamSource(0, m_pVB2,0,sizeof(SPIRE_FVF));
					m_pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN , 0 , facenum+0 );
					m_pDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CCW );
					m_pDevice->SetStreamSource(0, m_pVB, 0,sizeof(SPIRE_FVF));
					m_pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, facenum*2);

					m_pDevice->m_pRtShadow1->GetSurfaceLevel ( 0 , &pSurface1 );
					m_pDevice->SetRenderTarget ( 0 , pSurface1 );
					m_pDevice->Clear( 0, NULL, D3DCLEAR_TARGET, 
										D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0 );
		//			D3DXMATRIX matRot;
		//			D3DXMatrixRotationY( &matRot , D3DX_PI );
					m_pDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CW );
					m_pDevice->SetStreamSource(0, m_pVB2,0,sizeof(SPIRE_FVF));
					m_pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN , 0 , facenum+0 );
					m_pDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CW );
					m_pDevice->SetStreamSource(0, m_pVB, 0,sizeof(SPIRE_FVF));
					m_pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, facenum*2);
					

					m_pDevice->SetRenderTarget ( 0 , pSurfaceOld );
					pSurface0->Release();
					pSurface1->Release();
					m_pDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_NONE );

					//BlurTexture();

					m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE);
					m_pDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ZERO);

					//BlurTexture();

					UINT pass;
					m_pDevice->m_pShadowEffect->Begin ( &pass , 0 );
					m_pDevice->m_pShadowEffect->BeginPass ( 0 );
					m_pDevice->m_pd3dDevice->SetTexture ( 0 , m_pDevice->m_pRtShadow0 );
					m_pDevice->m_pd3dDevice->SetTexture ( 1 , m_pDevice->m_pRtShadow1 );
					m_pDevice->DrawFullScreenQuad();
					m_pDevice->m_pShadowEffect->EndPass ();
					m_pDevice->m_pShadowEffect->End ();

					m_pDevice->SetRenderState ( D3DRS_ZWRITEENABLE , TRUE );
					m_pDevice->SetRenderState( D3DRS_CULLMODE , cullmode );
				}
			}
		}
	}
	else
	{
		if( mytype == MT_spire )
		{
			m_pDevice->SetFVF(D3DFVF_XYZB1|D3DFVF_TEX1);
			m_pDevice->SetStreamSource(0, m_pVB, 0,sizeof(SPIRE_FVF));
			m_pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, facenum*2);
		}
		else if( mytype == MT_spire_shadow )
		{
			if( this->shadow_render_type == 1 )
			{
				m_pDevice->SetFVF(D3DFVF_XYZB1|D3DFVF_TEX1);
				m_pDevice->SetStreamSource(0, m_pVB, 0,sizeof(SPIRE_FVF));
				m_pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, facenum*2);
				m_pDevice->SetStreamSource(0, m_pVB2,0,sizeof(SPIRE_FVF));
				m_pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN , 0 , facenum+0 );
			}
		}
		else if( mytype == MT_quad )
		{
			m_pBillboard->Render();
		}
		else if( mytype == MT_mesh )
		{
			if( m_pMesh )
			{
				
				if( m_nMeshBillBoardType!=0 )
				{
					D3DXMATRIX world;
					m_pDevice->GetTransform ( D3DTS_WORLD , &world );
					D3DXMATRIX world2;
					D3DXMatrixTranslation(&world2,world._41,world._42,world._43);
					D3DXVECTOR3 vSca = m_pDevice->GetScaling ();
					D3DXMATRIX sca;
					D3DXMatrixScaling(&sca , vSca.x , vSca.y , vSca.z );
					D3DXMATRIX rot;
					D3DXMatrixRotationZ( &rot , m_pDevice->GetRotation () );
					D3DXMATRIX matBillboard = CDymMagicBillboard::GetBillboardMatrix(m_nMeshBillBoardType);
					m_pDevice->SetTransform ( D3DTS_WORLD , &(sca*rot*matBillboard*world2) );
				}
				if( bUsingMeshVertexColor )
				{
					m_pDevice->SetTextureStageState (0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
					m_pDevice->SetTextureStageState (0,D3DTSS_COLORARG2,D3DTA_TEXTURE);
					m_pDevice->SetTextureStageState (0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
					m_pDevice->SetTextureStageState (0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);
					m_pDevice->SetTextureStageState (0,D3DTSS_COLOROP,D3DTOP_MODULATE);
					m_pDevice->SetTextureStageState (0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
				}
				m_pMesh->Render ();
			}
		}
		else if( mytype == MT_shadow )
		{
			if( m_pShadowVB )
			{
				m_pDevice->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1);
				D3DXMATRIX matWorld , worldsaved;
				m_pDevice->GetTransform ( D3DTS_WORLD , &matWorld );
				worldsaved = matWorld;
				D3DXVECTOR4 v4;
				D3DXVec3Transform( &v4 , &D3DXVECTOR3( 0 , 0 , 0 ) , &matWorld );
				m_pShadowVB[0].p = D3DXVECTOR3( v4.x/v4.w , v4.y/v4.w , v4.z/v4.w );
				D3DXMATRIX trans;
				D3DXMatrixTranslation( &trans , initsize.x , initsize.y , initsize.z );
				matWorld = trans * matWorld ;
				D3DXVec3Transform( &v4 , &D3DXVECTOR3( 0 , 0 , 0 ) , &matWorld );
				m_pShadowVB[1].p = D3DXVECTOR3( v4.x/v4.w , v4.y/v4.w , v4.z/v4.w );

				if( m_bShadowNeedSetPos )
				{
					for( int i=1;i<facenum+1;i++ )
					{
						m_pShadowVB[i*2+0].p = m_pShadowVB[0].p ;
						m_pShadowVB[i*2+1].p = m_pShadowVB[1].p ;
					}
					m_bShadowNeedSetPos = FALSE;
				}

				if( m_bShadowNeedSetPos == FALSE )
				{
					D3DXMatrixIdentity( &matWorld );
					m_pDevice->SetTransform ( D3DTS_WORLD , &matWorld );
					m_pDevice->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP , facenum*2 , m_pShadowVB , sizeof( SHADOW_FVF ) );
					m_pDevice->SetTransform ( D3DTS_WORLD , &worldsaved );
				}
			}
		}
	}
	return S_OK;
}
HRESULT CDymMagicModel::init(void)
{
	//m_pDevice->CreateTextureFromFile ( "E:\\工程\\我的工程\\Demo3D\\Demo03\\pic\\fangzimaps.bmp" , &m_pTex );
	if( mytype == MT_spire || mytype == MT_spire_shadow )
	{
		if(m_pTex)m_pTex->Release ();
		m_pDevice->CreateTextureFromFile ( materialres , &m_pTex );
		if(m_pVB)m_pVB->Release ();
		m_pDevice->CreateVertexBuffer ( (facenum*2+2)*sizeof(SPIRE_FVF) , D3DUSAGE_DYNAMIC,0, D3DPOOL_DEFAULT, &m_pVB,NULL);
		SPIRE_FVF* pVertices = NULL;
		D3DXVECTOR3 v1 = D3DXVECTOR3( initsize.x/2.0f ,  initsize.y/2.0f  , initsize.z/2.0f );
		D3DXVECTOR3 v2 = D3DXVECTOR3( initsize.x/2.0f , -initsize.y/2.0f  , initsize.z/2.0f );
		D3DXMATRIX rot;
		float angle = D3DX_PI*2.0f / (float)facenum ;
		D3DXMatrixRotationY( &rot , angle );
		m_pVB->Lock(0, 0, (void**)&pVertices, 0);
		for(int i=0;i<facenum+1;i++)
		{
			pVertices[i*2].p = v1;
			pVertices[i*2].u = (float)i/(float)facenum;
			pVertices[i*2].v = 0.0f;
			pVertices[i*2].fBlend = 0.0f;

			pVertices[i*2+1].p = v2;
			pVertices[i*2+1].u = pVertices[i*2].u;
			pVertices[i*2+1].v = 1.0f;
			pVertices[i*2+1].fBlend = 1.0f;

			D3DXVECTOR4 v4;
			D3DXVec3Transform( &v4 , &v1 , &rot );
			v1 = D3DXVECTOR3( v4.x/v4.w , v4.y/v4.w , v4.z/v4.w );
			D3DXVec3Transform( &v4 , &v2 , &rot );
			v2 = D3DXVECTOR3( v4.x/v4.w , v4.y/v4.w , v4.z/v4.w );
		}
		m_pVB->Unlock ();

		if( mytype == MT_spire_shadow )
		{
			if( m_pVB2 )m_pVB2->Release ();
			m_pDevice->CreateVertexBuffer ( (facenum+2)*sizeof( SPIRE_FVF ) , D3DUSAGE_DYNAMIC,0, D3DPOOL_DEFAULT, &m_pVB2,NULL);
			SPIRE_FVF* pVertices = NULL;
			D3DXVECTOR3 v1 = D3DXVECTOR3( initsize.x/2.0f ,  initsize.y/2.0f  , initsize.z/2.0f );
			D3DXMATRIX rot;
			float angle = D3DX_PI*2.0f / (float)facenum ;
			D3DXMatrixRotationY( &rot , angle );
			m_pVB2->Lock(0, 0, (void**)&pVertices, 0);
			pVertices[0].p = D3DXVECTOR3( 0 , initsize.y/2.0f , 0 );
			pVertices[0].u = 0.0f;
			pVertices[0].v = 0.0f;
			pVertices[0].fBlend = 0.0f;
			for(int i=1;i<facenum+2;i++)
			{
				pVertices[i].p = v1;
				pVertices[i].u = (float)i/(float)facenum;
				pVertices[i].v = 0.0f;
				pVertices[i].fBlend = 0.0f;

				D3DXVECTOR4 v4;
				D3DXVec3Transform( &v4 , &v1 , &rot );
				v1 = D3DXVECTOR3( v4.x/v4.w , v4.y/v4.w , v4.z/v4.w );
			}
			m_pVB2->Unlock ();
		}
	}
	else if( mytype == MT_shadow )
	{
		m_pShadowVB = new SHADOW_FVF[ facenum*2+2 ];

		D3DXVECTOR3 vbegin1 = D3DXVECTOR3( 0 , 0 , 0 ) , vbegin2 = D3DXVECTOR3( 0 , 0 , 0 );
/*		D3DXMATRIX matWorld;
		m_pDevice->GetTransform ( D3DTS_WORLD , &matWorld );
		D3DXVECTOR4 v4;
		D3DXVec3Transform( &v4 , &D3DXVECTOR3( 0 , 0 , 0 ) , &matWorld );
		vbegin1 = D3DXVECTOR3( v4.x/v4.w , v4.y/v4.w , v4.z/v4.w );
		D3DXMATRIX trans;
		D3DXMatrixTranslation( &trans , initsize.x , initsize.y , initsize.z );
		matWorld = trans * matWorld ;
		D3DXVec3Transform( &v4 , &D3DXVECTOR3( 0 , 0 , 0 ) , &matWorld );
		vbegin2 = D3DXVECTOR3( v4.x/v4.w , v4.y/v4.w , v4.z/v4.w );*/
		for(int i=0;i<facenum+1;i++)
		{
			m_pShadowVB[i*2].p = vbegin1;
			m_pShadowVB[i*2].u = (float)i/(float)facenum;
			m_pShadowVB[i*2].v = 1.0f;

			m_pShadowVB[i*2+1].p = vbegin2;
			m_pShadowVB[i*2+1].u = m_pShadowVB[i*2].u;
			m_pShadowVB[i*2+1].v = 0.0f;
		}
		m_bShadowNeedSetPos = TRUE;
	}
	else if( mytype == MT_quad )
	{
		if(m_pBillboard)delete m_pBillboard;
		if(m_pTex)m_pTex->Release ();
		m_pDevice->CreateTextureFromFile ( materialres , &m_pTex );
		D3DXVECTOR2 v1 = D3DXVECTOR2( -initsize.x/2.0f ,  -initsize.y/2.0f );
		D3DXVECTOR2 v2 = D3DXVECTOR2( initsize.x/2.0f ,  initsize.y/2.0f );
		m_pBillboard = new CDymMagicBillboard( m_pDevice );
		m_pBillboard->init ( v1 , v2 );
		if( isinfacex && isinfacey && isinfacez )m_pBillboard->SetBillboardType ( 1 );
		else if( isinfacey ) m_pBillboard->SetBillboardType ( 2 );
	}
	else if( mytype == MT_mesh )
	{
		//CDymMagicMesh * pMesh = new CDymMagicMesh( m_pDevice );
		//HRESULT hr = pMesh->LoadFromMeshFile ( m_strMeshName );
		CDymMagicMesh *pMesh = NULL;
		HRESULT hr = m_pDevice->CreateMeshFromFile ( m_strMeshName , &pMesh );
		if( FAILED( hr ) )
		{
			// 文件没有找到
			if( hr == E_POINTER )return S_OK;
			return hr;
		}
		if( m_pMesh )
			m_pMesh->Release ();
		m_pMesh = pMesh;
	}
	return S_OK;
}

HRESULT CDymMagicModel::ImportMeshFromXML( LPCTSTR xmlfilename )
{
	if( m_pMesh )m_pMesh->Release ();
	m_pMesh = new CDymMagicModelMesh( m_pDevice , &this->m_KeyFrameManager );
	HRESULT hr = m_pMesh->LoadFromFile ( xmlfilename );
	if( FAILED( hr ) )return hr;

	TCHAR chardrive[_MAX_DRIVE];
	TCHAR chardir[_MAX_DIR];
	TCHAR charext[_MAX_EXT];
	TCHAR charname[_MAX_PATH];
	_splitpath(xmlfilename,chardrive,chardir,charname,charext); 
	TCHAR tmp[MAX_PATH];
	_stprintf( tmp , "%s.dmd" , charname );

	_tcscpy( m_strMeshName , tmp );
	return m_pMesh->SaveToMeshFile ( m_strMeshName );
}

D3DXVECTOR3* CDymMagicModel::GetVertexStreamZeroData(int *pfacenum,int *pStride)
{
	if( mytype == MT_spire || mytype == MT_spire_shadow )
	{
		SPIRE_FVF* pVerticesNew = new SPIRE_FVF[ facenum*6+6 ];
		SPIRE_FVF* pVertices = NULL;
		m_pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_READONLY);
		for(int i=0;i<facenum*2+2;i++)
		{
			pVerticesNew[i*3+0] = pVertices[i];
			pVerticesNew[i*3+1] = pVertices[i+1];
			pVerticesNew[i*3+2] = pVertices[i+2];
		}
		m_pVB->Unlock ();
		*pfacenum = facenum*2;
		*pStride = sizeof(SPIRE_FVF);
		return (D3DXVECTOR3*)pVerticesNew;
	}
	else if( mytype == MT_shadow )
	{
		SHADOW_FVF* pVerticesNew = new SHADOW_FVF[ facenum*2*3 ];
		for(int i=0;i<facenum*2;i++)
		{
			pVerticesNew[i*3+0] = m_pShadowVB[i];
			pVerticesNew[i*3+1] = m_pShadowVB[i+1];
			pVerticesNew[i*3+2] = m_pShadowVB[i+2];
		}
		*pfacenum = facenum*2;
		*pStride = sizeof(SHADOW_FVF);
		return (D3DXVECTOR3*)pVerticesNew;
	}
	else if( mytype == MT_quad )
	{
		return m_pBillboard->GetVertexStreamZeroData ( pfacenum , pStride );
	}
	else if( mytype == MT_mesh )
	{
		if( !m_pMesh )return NULL;
		return m_pMesh->GetVertexStreamZeroData ( pfacenum , pStride );
	}
	return NULL;
}
void CDymMagicModel::_ComputeBoundingBox( CDymMagicKeyFrame *pKeyFrame , DYM_WORLD_MATRIX world )
{
	//DYM_WORLD_MATRIX world;
	float fCurTime = 0.0f;
	if( pKeyFrame )fCurTime = pKeyFrame->m_fTime ;
	FrameMove( fCurTime , 100.0f );
	BOOL bRender = TRUE;
	if( mytype == MT_quad )
	{
		if(m_pBillboard->GetBillboardType ()!=0 )
		{
			bRender = FALSE;
			/*D3DXVECTOR3 v;
			float radio;
			m_pBillboard->GetBoundingSphere ( &v , &radio );
			m_pDevice->AddBoundingSphereToBox ( v , radio );*/
		}
	}
	if( bRender )
		Render( world , 0 );
	else
	{
		m_pDevice->TrackBoundingBox ( FALSE );
		m_pDevice->TrackBoundingSphere ( TRUE );
		Render( world , 0 );
		m_pDevice->TrackBoundingSphere ( FALSE );
		m_pDevice->TrackBoundingBox ( TRUE );
		for( std::list< CDymRenderSystem::BOUNDING_SPHERE * >::iterator it = m_pDevice->m_listBoundingSphere.begin ();
		it != m_pDevice->m_listBoundingSphere.end () ; it++ )
		{
			CDymRenderSystem::BOUNDING_SPHERE *p = *it;
			m_pDevice->AddBoundingSphereToBox ( p->vCenter , p->fRadio );
		}
		m_pDevice->CleraBoundingSphere ();
	}
}
void CDymMagicModel::GetBoundingBoxRealTime( D3DXVECTOR3 *pMin , D3DXVECTOR3 *pMax )
{
	if( mytype != MT_shadow )
	{
		*pMin = m_vBoundingBox1 ;
		*pMax = m_vBoundingBox2 ;
	}
	else
	{
		D3DXVECTOR3 v1 = m_vBoundingBox1 ;
		D3DXVECTOR3 v2 = m_vBoundingBox2 ;

		D3DXVECTOR3 vp1;
		D3DXVECTOR3 vp2;
		int facenum , stride ;
		D3DXVECTOR3 *pStream = GetVertexStreamZeroData( &facenum , &stride );
		D3DXComputeBoundingBox( pStream , facenum*3 , stride , &vp1 , &vp2 );
		delete []pStream;
		if( v1.x > vp1.x )
			v1.x = vp1.x;
		if( v1.y > vp1.y )
			v1.y = vp1.y;
		if( v1.z > vp1.z )
			v1.z = vp1.z;
		if( v2.x < vp2.x )
			v2.x = vp2.x;
		if( v2.y < vp2.y )
			v2.y = vp2.y;
		if( v2.z < vp2.z )
			v2.z = vp2.z;
		*pMin = v1;
		*pMax = v2;
	}
}
void CDymMagicModel::ComputeBoundingBox( DYM_WORLD_MATRIX world )
{
	m_pDevice->TrackBoundingBox ( TRUE );
	_ComputeBoundingBox( NULL , world );
	for( float i = 0 ; i <= this->m_KeyFrameManager.GetKeyMaxTime () ; i += 100.0f )
	{
		CDymMagicKeyFrame *pKeyFrame = m_KeyFrameManager.GetKeyFrame ( i );
		_ComputeBoundingBox( pKeyFrame , world );
	}
	m_pDevice->TrackBoundingBox ( FALSE );

	m_vBoundingBox1 = D3DXVECTOR3( 100000 , 100000 , 100000 );
	m_vBoundingBox2 = D3DXVECTOR3( -100000 , -100000 , -100000 );
	
	for( std::list< CDymRenderSystem::BOUNDING_BOX * >::iterator it = m_pDevice->m_listBoundingBox.begin ();
		it != m_pDevice->m_listBoundingBox.end () ; it++ )
	{
		CDymRenderSystem::BOUNDING_BOX * pBoundingBox = *it;
		if( m_vBoundingBox1.x > pBoundingBox->vMin.x )
			m_vBoundingBox1.x = pBoundingBox->vMin.x;
		if( m_vBoundingBox1.y > pBoundingBox->vMin.y )
			m_vBoundingBox1.y = pBoundingBox->vMin.y;
		if( m_vBoundingBox1.z > pBoundingBox->vMin.z )
			m_vBoundingBox1.z = pBoundingBox->vMin.z;
		if( m_vBoundingBox2.x < pBoundingBox->vMax.x )
			m_vBoundingBox2.x = pBoundingBox->vMax.x;
		if( m_vBoundingBox2.y < pBoundingBox->vMax.y )
			m_vBoundingBox2.y = pBoundingBox->vMax.y;
		if( m_vBoundingBox2.z < pBoundingBox->vMax.z )
			m_vBoundingBox2.z = pBoundingBox->vMax.z;
	}
	m_pDevice->ClearBoundingBoxList ();
}

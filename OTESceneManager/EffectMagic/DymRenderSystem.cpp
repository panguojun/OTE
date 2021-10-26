/***************************************************************************************************
文件名称:	DymRenderSystem.cpp
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	渲染管理
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#include ".\dymrendersystem.h"
#include "DymMagicMesh.h"
#include <stdio.h>

//----------------------------------------------------------------------------------------------
// CDymRenderSystem
//----------------------------------------------------------------------------------------------
CDymRenderSystem::CDymRenderSystem(void)
{
	m_pRtTexture1 = NULL;
	m_bTrackingBoundingBox = FALSE;
	m_bTrackingBoundingSphere = FALSE;
	m_pRtShadow0 = NULL;
	m_pRtShadow1 = NULL;
	m_pShadowEffect = NULL;
	m_bFastRender = FALSE;
	m_pGrassEffect = NULL;
	m_bGrass = FALSE;
}

CDymRenderSystem::~CDymRenderSystem(void)
{
	if( m_pRtTexture1 ) m_pRtTexture1->Release ();
	if( m_pRtShadow0 ) m_pRtShadow0->Release ();
	if( m_pRtShadow1 ) m_pRtShadow1->Release ();
	if( m_pShadowEffect ) m_pShadowEffect->Release ();
	Release();
}

HRESULT CDymRenderSystem::BeginGrass()
{
	/*if( m_pGrassEffect == NULL )return E_FAIL;
	m_pGrassEffect->SetFloat ("g_fTime",(float)timeGetTime()/3000.0f);

	D3DXMATRIX matView,matProj,matWorld;
	m_pd3dDevice->GetTransform(D3DTS_VIEW,&matView);
	m_pd3dDevice->GetTransform(D3DTS_PROJECTION,&matProj);
	m_pd3dDevice->GetTransform(D3DTS_WORLD,&matWorld);
	m_pGrassEffect->SetMatrix ("g_WVP",&(matWorld*matView*matProj));
	m_pGrassEffect->SetMatrix ("g_matWorld",&matWorld);
	m_pGrassEffect->SetTechnique(m_pGrassEffect->GetTechniqueByName("grass"));
	UINT i;
	m_pGrassEffect->Begin (&i , 0 );
	m_pGrassEffect->BeginPass(0);*/
	m_bGrass = TRUE;
	return S_OK;
}
HRESULT CDymRenderSystem::EndGrass()
{
	/*if( m_pGrassEffect == NULL )return E_FAIL;
	m_pGrassEffect->EndPass();
	m_pGrassEffect->End ();*/
	m_bGrass = FALSE;
	return S_OK;
}
void CDymRenderSystem::GrassSetBegin()
{
	if( !m_bGrass )return;
	D3DXMATRIX matWorld,rot;
	m_pd3dDevice->GetTransform(D3DTS_WORLD,&matWorld);

	float a = sinf( (float)timeGetTime()/1000.0f );
	D3DXMatrixRotationAxis( &rot, &D3DXVECTOR3(-0.012,0,0.707), a/5.0f );
   	D3DXMatrixMultiply( &matWorld, &matWorld, &rot);

	m_pd3dDevice->SetTransform ( D3DTS_WORLD1 , &matWorld );
	m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_1WEIGHTS );
}
void CDymRenderSystem::GrassSetEnd()
{
	if( !m_bGrass )return;
	m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_DISABLE );
}
HRESULT CDymRenderSystem::BeginRender(int mipLever)
{
	m_pd3dDevice->SetRenderState ( D3DRS_ZENABLE , FALSE );
	m_pd3dDevice->GetRenderTarget ( 0 , &m_pBkSurface );
	m_pRtTexture1->GetSurfaceLevel ( 0 , &m_pRtSurface1 );
	m_pd3dDevice->SetRenderTarget ( 0 , m_pRtSurface1 );
	m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, 0x00000000 , 0.0f, 0 );
	/*D3DVIEWPORT9 v;
	v.X = 200;
	v.Y = 200;
	v.Width = 800+200;
	v.Height = 600+200;
	v.MinZ = 0.0f;
	v.MaxZ = 1.0f;
	HRESULT hr=m_pd3dDevice->SetViewport ( &v );*/
	return S_OK;
}
HRESULT CDymRenderSystem::EndRender()
{
	
	m_pd3dDevice->SetRenderState ( D3DRS_ZENABLE , TRUE );
	m_pd3dDevice->SetRenderTarget ( 0 , m_pBkSurface );
	m_pd3dDevice->SetTexture ( 0 , m_pRtTexture1 );
	DWORD alphaenable;
	m_pd3dDevice->GetRenderState ( D3DRS_ALPHABLENDENABLE  , &alphaenable );
	m_pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE  , TRUE );
	m_pd3dDevice->SetTextureStageState (0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState (0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState (0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState (0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE);
	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA);
	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE);

	DrawFullScreenQuad();
	m_pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE  , alphaenable );
	if( m_pRtSurface1 )m_pRtSurface1->Release ();
	return S_OK;
}
void CDymRenderSystem::End()
{
	this->m_renderPUO .EndRender ();
}

void CDymRenderSystem::init( LPDIRECT3DDEVICE9 pDevice )
{
	TCHAR strEffectScript[] = 
	{
		"\n\
sampler s0 : register(s0);\n\
sampler s1 : register(s1);\n\
sampler s2 : register(s2);\n\
\n\
float4 psDrawShadow( in float2 vScreenPosition : TEXCOORD0 ) : COLOR\n\
{\n\
	float4 color0 = tex2D( s0 , vScreenPosition );\n\
	float4 color1 = tex2D( s1 , vScreenPosition );\n\
	float4 color = color0-color1;\n\
	color = max( color , 0.0f );\n\
	return color/2.0f;\n\
}\n\
\n\
technique BrightPass\n\
{\n\
	pass p0\n\
	{\n\
		PixelShader = compile ps_2_0 psDrawShadow();\n\
	}\n\
}"
	};

	TCHAR strGrassScript1[] =
	{
		"sampler s0 : register(s0);							\n\
float g_fTime;							\n\
matrix g_WVP;//world*view*project							\n\
matrix g_matWorld;							\n\
float g_fWindPower=1.0f;							\n\
float g_fAlpha = 1.0f;							\n\
float g_fLightPower=1.0f;							\n\
float4 g_vLightColor={1,1,1,1};							\n\
float4 ps_Grass(in float2 vTexPos : TEXCOORD0 ,							\n\
                in float4 color : COLOR):COLOR							\n\
{							\n\
	float4 r=tex2D( s0 , vTexPos );							\n\
	r *= color*2.1f;							\n\
	r.a*=g_fAlpha;							\n\
#ifdef USING_HDR							\n\
	r.rgb*=LIGHT_POWER;							\n\
#endif							\n\
	return r;							\n\
}							\n\
void vs_Grass( float4 InputPos : POSITION ,							\n\
               float2 Tex : TEXCOORD0 ,							\n\
               out float4 oPos : POSITION ,							\n\
               out float2 oTex : TEXCOORD0 ,							\n\
               out float4 oColor : COLOR)							\n\
{							\n\
	float fNoise = -0.02f;							\n\
	float4 waveDirx = float4( -0.006+fNoise, -0.012, 0.024, 0.048 );							\n\
	float3 p = mul(InputPos,g_matWorld);							\n\
	float4 R0 = mul( waveDirx , p.x );							\n\
	float4 waveDiry = float4( -0.003+fNoise, -0.006, -0.012, -0.048 );							\n\
	R0 = mul( waveDiry , p.z ) + R0 ;							\n\
	R0 = g_fTime + R0;							\n\
	R0 =  R0 - floor(R0);							\n\
	R0 *= 1.07f;							\n\
	R0 -= 0.5f;							\n\
	float4 wavev = R0*6.28318530f;							\n\
	float4 wavew[5];							\n\
	wavew[0] = wavev * wavev;							\n\
	wavew[1] = wavew[0] * wavev;							\n\
	wavew[2] = wavew[1] * wavew[0];							\n\
	wavew[3] = wavew[2] * wavew[0];							\n\
	wavew[4] = wavew[3] * wavew[0];							\n\
	float4 v = ( wavew[1] * (-0.16161616f) ) + wavev;							\n\
	v = ( wavew[2] * 0.0083333f ) + v;							\n\
	v = ( wavew[3] * (-0.00019841f) ) + v;							\n\
	v = ( wavew[4] * 0.000002755731f ) + v;							\n\
	float4 v2;							\n\
	"
	};
	TCHAR strGrassScript2[] =
	{
		"			\n\
	v2.x = dot(v , float4( 3.0, 0.4, 0.0, 0.3) );							\n\
	v2.z = dot(v , float4( 3.0, 0.4, 0.0, 0.3) );							\n\
	v2.y = dot(v , float4( -1.0, -0.133, -0.333, -0.10 ) );							\n\
	v2.w = v2.z;							\n\
	float s = 1.0f - Tex.y;							\n\
	v2 = v2 * ( s*s );							\n\
	wavew[0].w = InputPos.w;							\n\
	wavew[0].xyz = v2/((InputPos.y-1.0f)*2-1.0f)*g_fWindPower + InputPos;							\n\
	oPos = mul(wavew[0],g_WVP);							\n\
	float f= dot(v,float4( 0.35, 0.10, 0.10, 0.03) );							\n\
	oColor = 1.0f-f/2.0f;							\n\
	oColor.rgb*=g_fLightPower+s/2.0f;							\n\
	oColor*=g_vLightColor;							\n\
	oColor.a = 1.0f;							\n\
	oTex = Tex;							\n\
}							\n\
technique grass							\n\
{							\n\
	pass p0							\n\
	{							\n\
		VertexShader = compile vs_1_1 vs_Grass();							\n\
		PixelShader = compile ps_2_0 ps_Grass();							\n\
	}							\n\
}"
	};
	m_pd3dDevice = pDevice ;

	D3DSURFACE_DESC dtdsdRT;
    PDIRECT3DSURFACE9 pSurfRT;
    // Acquire render target width and height
    m_pd3dDevice->GetRenderTarget(0, &pSurfRT);
    pSurfRT->GetDesc(&dtdsdRT);
    pSurfRT->Release();

	m_pd3dDevice->CreateTexture( dtdsdRT.Width/4 ,dtdsdRT.Height/4 , 
                    1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, 
                    D3DPOOL_DEFAULT, &m_pRtTexture1, NULL );
	LPD3DXBUFFER errorBuffer;             // Buffer with error messages

	D3DXCreateEffect(m_pd3dDevice, strEffectScript , _tcslen( strEffectScript )+1 , NULL , NULL, 0,//D3DXSHADER_DEBUG,
		NULL,&m_pShadowEffect,&errorBuffer);

	if( errorBuffer==NULL && m_pShadowEffect!=NULL )
	{
		m_pd3dDevice->CreateTexture( dtdsdRT.Width ,dtdsdRT.Height , 
						1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, 
						D3DPOOL_DEFAULT, &m_pRtShadow0, NULL );
		m_pd3dDevice->CreateTexture( dtdsdRT.Width ,dtdsdRT.Height , 
						1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, 
						D3DPOOL_DEFAULT, &m_pRtShadow1, NULL );
	}
	
	TCHAR *strGrassScript = new TCHAR[ _tcslen( strGrassScript1 )+_tcslen( strGrassScript2 )+1 ];
	_tcscpy( strGrassScript , strGrassScript1 );
	_tcscpy( strGrassScript + _tcslen( strGrassScript1 ) , strGrassScript2 );
	D3DXCreateEffect(m_pd3dDevice, strGrassScript , _tcslen( strGrassScript )+1 , NULL , NULL, 0,//D3DXSHADER_DEBUG,
		NULL,&m_pGrassEffect,&errorBuffer);
	delete[] strGrassScript;
	m_renderPUO.init( m_pd3dDevice );
}
HRESULT CDymRenderSystem::GetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9 ** ppRenderTarget)
{
	return m_pd3dDevice->GetRenderTarget ( RenderTargetIndex , ppRenderTarget );
}
HRESULT CDymRenderSystem::SetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9 * pRenderTarget)
{
	return m_pd3dDevice->SetRenderTarget ( RenderTargetIndex , pRenderTarget );
}
HRESULT CDymRenderSystem::Clear(DWORD Count,CONST D3DRECT * pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil)
{
	return m_pd3dDevice->Clear ( Count , pRects , Flags , Color , Z , Stencil );
}
HRESULT CDymRenderSystem::SetTransform( D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX * pMatrix )
{
	return m_pd3dDevice->SetTransform ( State , pMatrix );
}
HRESULT CDymRenderSystem::GetTransform( D3DTRANSFORMSTATETYPE State,D3DMATRIX * pMatrix)
{
	return m_pd3dDevice->GetTransform ( State , pMatrix );
}
HRESULT CDymRenderSystem::SetRenderState( D3DRENDERSTATETYPE State,DWORD Value )
{
	return m_pd3dDevice->SetRenderState( State , Value );
}
HRESULT CDymRenderSystem::GetRenderState( D3DRENDERSTATETYPE State,DWORD *Value )
{
	return m_pd3dDevice->GetRenderState( State , Value );
}
HRESULT CDymRenderSystem::SetTexture( DWORD Sampler, CDymD3DTexture * pTexture )
{
	if( pTexture == NULL )return m_pd3dDevice->SetTexture( Sampler , NULL );
	return m_pd3dDevice->SetTexture( Sampler , pTexture->m_pTex );
}
HRESULT CDymRenderSystem::SetIndices(CDymD3DIndexBuffer * pIndexData)
{
	if( pIndexData == NULL )return S_OK;
	return m_pd3dDevice->SetIndices ( pIndexData->m_pIB );
}
HRESULT CDymRenderSystem::CreateVertexBuffer( UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,CDymD3DVertexBuffer** ppVertexBuffer,HANDLE* pSharedHandle)
{
	LPDIRECT3DVERTEXBUFFER9 pVB;
	HRESULT hr = m_pd3dDevice->CreateVertexBuffer ( Length , Usage , FVF , Pool , &pVB , pSharedHandle );
	if(FAILED(hr))return hr;
	*ppVertexBuffer = new CDymD3DVertexBuffer();
	(*ppVertexBuffer)->m_pVB = pVB;
	return S_OK;
}
HRESULT CDymRenderSystem::CreateIndexBuffer(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,CDymD3DIndexBuffer** ppIndexBuffer,HANDLE* pSharedHandle)
{
	LPDIRECT3DINDEXBUFFER9 pIB;
	HRESULT hr = m_pd3dDevice->CreateIndexBuffer ( Length , Usage , Format , Pool , &pIB , pSharedHandle );
	if(FAILED(hr))return hr;
	*ppIndexBuffer = new CDymD3DIndexBuffer();
	(*ppIndexBuffer)->m_pIB = pIB;
	return S_OK;
}

HRESULT CDymRenderSystem::SetTextureStageState( DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value)
{
	return m_pd3dDevice->SetTextureStageState ( Stage , Type , Value );
}
HRESULT CDymRenderSystem::SetFVF( DWORD FVF)
{
	return m_pd3dDevice->SetFVF( FVF );
}
void CDymRenderSystem::TrackBoundingBox( BOOL bTrack )
{
	m_bTrackingBoundingBox = bTrack;
}
void CDymRenderSystem::TrackBoundingSphere( BOOL bTrack )
{
	m_bTrackingBoundingSphere = bTrack;
}
void CDymRenderSystem::ClearBoundingBoxList()
{
	for( std::list< BOUNDING_BOX* >::iterator it = m_listBoundingBox.begin () ; it != m_listBoundingBox.end () ; it ++ )
	{
		BOUNDING_BOX *p = *it;
		delete p;
	}
	m_listBoundingBox.clear ();
}
void CDymRenderSystem::CleraBoundingSphere()
{
	for( std::list< BOUNDING_SPHERE* >::iterator it = m_listBoundingSphere.begin () ; it != m_listBoundingSphere.end () ; it ++ )
	{
		BOUNDING_SPHERE *p = *it;
		delete p;
	}
	m_listBoundingSphere.clear ();
}
void CDymRenderSystem::_TrackBounding( D3DPRIMITIVETYPE PrimitiveType , UINT PrimitiveCount , CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride )
{
	if( m_bTrackingBoundingBox || m_bTrackingBoundingSphere )
	{
		UINT vertexnum = 0;
		if( PrimitiveType == D3DPT_TRIANGLELIST )
			vertexnum = PrimitiveCount*3;
		else if( PrimitiveType == D3DPT_TRIANGLESTRIP )
			vertexnum = PrimitiveCount+2;
		else if( PrimitiveType == D3DPT_TRIANGLEFAN )
			vertexnum = PrimitiveCount+2;
		if( vertexnum == 0 )return;
		BYTE *pStreamNewData = NULL;
		if( pVertexStreamZeroData && VertexStreamZeroStride > 0 )
		{
			pStreamNewData = new BYTE[ vertexnum*VertexStreamZeroStride ];
			memcpy( pStreamNewData , pVertexStreamZeroData , vertexnum*VertexStreamZeroStride );
		}
		else
		{
			UINT offsetInBytes;
			LPDIRECT3DVERTEXBUFFER9 vb;
			m_pd3dDevice->GetStreamSource ( 0 , &vb , &offsetInBytes , &VertexStreamZeroStride );
			pStreamNewData = new BYTE[ vertexnum*VertexStreamZeroStride ];
			BYTE *vbdata = NULL;
			vb->Lock ( 0 , 0 , (void**)&vbdata , D3DLOCK_READONLY );
			memcpy( pStreamNewData , vbdata , vertexnum*VertexStreamZeroStride );
			vb->Unlock ();
			vb->Release ();
		}
		D3DXMATRIX world;
		m_pd3dDevice->GetTransform ( D3DTS_WORLD , &world );
		D3DXVECTOR4 v4;
		for( int i=0;i<vertexnum;i++ )
		{
			D3DXVec3Transform( &v4 , (D3DXVECTOR3*)(pStreamNewData+VertexStreamZeroStride*i) , &world );
			((D3DXVECTOR3*)(pStreamNewData+VertexStreamZeroStride*i))->x = v4.x/v4.w;
			((D3DXVECTOR3*)(pStreamNewData+VertexStreamZeroStride*i))->y = v4.y/v4.w;
			((D3DXVECTOR3*)(pStreamNewData+VertexStreamZeroStride*i))->z = v4.z/v4.w;
		}
		if( m_bTrackingBoundingSphere )
		{
			BOUNDING_SPHERE * pBoundingSphere = new BOUNDING_SPHERE;
			D3DXComputeBoundingSphere( (CONST D3DXVECTOR3*)pStreamNewData , vertexnum , 
				VertexStreamZeroStride , &pBoundingSphere->vCenter , &pBoundingSphere->fRadio );
			m_listBoundingSphere.push_back ( pBoundingSphere );
		}
		else
		{
			BOUNDING_BOX * pBoundingBox = new BOUNDING_BOX;
			D3DXComputeBoundingBox( (CONST D3DXVECTOR3*)pStreamNewData , vertexnum ,
				VertexStreamZeroStride , &pBoundingBox->vMin , &pBoundingBox->vMax );
			m_listBoundingBox.push_back ( pBoundingBox );
		}
		delete[]pStreamNewData;
	}
}
void CDymRenderSystem::AddBoundingSphereToBox( D3DXVECTOR3 v , float r )
{
	D3DXVECTOR3 vMax = D3DXVECTOR3( v.x+r , v.y+r , v.z+r );
	D3DXVECTOR3 vMin = D3DXVECTOR3( v.x-r , v.y-r , v.z-r );
	BOUNDING_BOX * pBoundingBox = new BOUNDING_BOX;
	pBoundingBox->vMin = vMin;
	pBoundingBox->vMax = vMax;
	m_listBoundingBox.push_back ( pBoundingBox );
}
HRESULT CDymRenderSystem::SetStreamSource( UINT StreamNumber,CDymD3DVertexBuffer * pStreamData,UINT OffsetInBytes,UINT Stride)
{
	if( pStreamData == NULL )return S_OK;
	return m_pd3dDevice->SetStreamSource ( StreamNumber , pStreamData->m_pVB , OffsetInBytes , Stride );
}
HRESULT CDymRenderSystem::DrawPrimitive( D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount)
{
	_TrackBounding( PrimitiveType , PrimitiveCount , NULL , 0 );
	return m_pd3dDevice->DrawPrimitive ( PrimitiveType , StartVertex , PrimitiveCount );
}
HRESULT CDymRenderSystem::DrawPrimitiveUP( D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	//GrassSetBegin();
	_TrackBounding( PrimitiveType , PrimitiveCount , pVertexStreamZeroData , VertexStreamZeroStride );
	HRESULT hr = S_OK;
	hr = m_pd3dDevice->DrawPrimitiveUP( PrimitiveType , PrimitiveCount , pVertexStreamZeroData , VertexStreamZeroStride );
	//hr = m_renderPUO.DrawPrimitiveUp ( PrimitiveType , PrimitiveCount , pVertexStreamZeroData , VertexStreamZeroStride );
	//GrassSetEnd();
	return hr;
}

HRESULT CDymRenderSystem::DrawIndexedPrimitive( D3DPRIMITIVETYPE Type,INT BaseVertexIndex,UINT MinIndex,UINT NumVertices,UINT StartIndex,UINT PrimitiveCount)
{
	_TrackBounding( Type , NumVertices/3 , NULL , NULL );
	return m_pd3dDevice->DrawIndexedPrimitive ( Type , BaseVertexIndex , MinIndex , NumVertices , StartIndex , PrimitiveCount );
}
HRESULT CDymRenderSystem::SetSamplerState( DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value)
{
	return m_pd3dDevice->SetSamplerState( Sampler , Type , Value );
}
HRESULT CDymRenderSystem::Release(void)
{
	m_dataBuffer.ClearBuffer ();
	m_renderPUO.Release ();
	return S_OK;
}

void CDymRenderSystem::SetScaling(D3DXVECTOR3 vSca)
{
	m_vSca = vSca;
}

D3DXVECTOR3 CDymRenderSystem::GetScaling(void)
{
	return m_vSca;
}
void CDymRenderSystem::SetRotation(float fRot)
{
	m_fRot = fRot;
}
float CDymRenderSystem::GetRotation(void)
{
	return m_fRot;
}
void CDymRenderSystem::DrawLineBox( D3DXVECTOR3 b1 , D3DXVECTOR3 b2 , DWORD color )
{
	//   0   1
	//   /---/|
	// 2/---/3|
	//  | 6 | /7
	// 4|---|/5
	D3DXVECTOR3 v0 = D3DXVECTOR3( b1.x , b2.y , b2.z );
	D3DXVECTOR3 v1 = D3DXVECTOR3( b2.x , b2.y , b2.z );
	D3DXVECTOR3 v2 = D3DXVECTOR3( b1.x , b2.y , b1.z );
	D3DXVECTOR3 v3 = D3DXVECTOR3( b2.x , b2.y , b1.z );
	D3DXVECTOR3 v4 = D3DXVECTOR3( b1.x , b1.y , b1.z );
	D3DXVECTOR3 v5 = D3DXVECTOR3( b2.x , b1.y , b1.z );
	D3DXVECTOR3 v6 = D3DXVECTOR3( b1.x , b1.y , b2.z );
	D3DXVECTOR3 v7 = D3DXVECTOR3( b2.x , b1.y , b2.z );
	DrawLine ( v0 , v1 , color );
	DrawLine ( v0 , v2 , color );
	DrawLine ( v1 , v3 , color );
	DrawLine ( v2 , v3 , color );
	DrawLine ( v2 , v4 , color );
	DrawLine ( v3 , v5 , color );
	DrawLine ( v4 , v5 , color );
	DrawLine ( v1 , v7 , color );
	DrawLine ( v5 , v7 , color );
	DrawLine ( v0 , v6 , color );
	DrawLine ( v4 , v6 , color );
	DrawLine ( v6 , v7 , color );
}
BOOL CDymRenderSystem::DrawCircle( D3DXVECTOR3 origal , float radio , DWORD color)
{
	D3DXMATRIX idt;
	D3DXMatrixIdentity( &idt );
	m_pd3dDevice->SetTransform ( D3DTS_WORLD , &idt );
#define LINE_NUM 32
	D3DXMATRIX mat;
	D3DXMatrixRotationY( &mat , D3DX_PI*2.0f/(float)LINE_NUM );
	D3DXVECTOR3 v = D3DXVECTOR3( radio , 0 , 0 );
	for(int i=0;i<LINE_NUM;i++)
	{
		D3DXVECTOR3 v2;
		D3DXVECTOR4 v4;
		D3DXVec3Transform( &v4 , &v , &mat );
		v2 = D3DXVECTOR3( v4.x/v4.w , v4.y/v4.w , v4.z/v4.w );
		DrawLine( v+origal , v2+origal , color );
		v=v2;
	}
	return 1;
}
void CDymRenderSystem::DrawLine(D3DXVECTOR3 v1,D3DXVECTOR3 v2,DWORD color)
{
	m_pd3dDevice->SetTexture ( 0 , NULL );
	struct VERTEX_LINE
	{
		D3DXVECTOR3 pos;
		DWORD color;
	};
	// Draw the quad
    VERTEX_LINE svQuad[2];

    svQuad[0].pos = v1;
    svQuad[0].color = color;

    svQuad[1].pos = v2;
    svQuad[1].color = color;

    m_pd3dDevice->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
    m_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, svQuad, sizeof(VERTEX_LINE));
}
void CDymRenderSystem::DrawFullScreenQuad(int x,int y,int w,int h,float fLeftU, float fTopV, float fRightU, float fBottomV)
{
	DWORD zwritevalue;
	DWORD value;
	m_pd3dDevice->GetRenderState (D3DRS_ZENABLE,&value);
	m_pd3dDevice->GetRenderState (D3DRS_ZWRITEENABLE,&zwritevalue);
	struct SCREEN_VERTEX 
	{
		D3DXVECTOR4 pos;
		D3DXVECTOR2 tex;
	};
    D3DSURFACE_DESC dtdsdRT;
    PDIRECT3DSURFACE9 pSurfRT;

    // Acquire render target width and height
    m_pd3dDevice->GetRenderTarget(0, &pSurfRT);
    pSurfRT->GetDesc(&dtdsdRT);
    pSurfRT->Release();

    // Ensure that we're directly mapping texels to pixels by offset by 0.5
    // For more info see the doc page titled "Directly Mapping Texels to Pixels"
    FLOAT fWidth5 = (FLOAT)dtdsdRT.Width - 0.5f;
    FLOAT fHeight5 = (FLOAT)dtdsdRT.Height - 0.5f;
	if(w!=0)fWidth5=(FLOAT)(w+x) - 0.5f;
	if(h!=0)fHeight5=(FLOAT)(h+y) - 0.5f;

    // Draw the quad
    SCREEN_VERTEX svQuad[4];

    svQuad[0].pos = D3DXVECTOR4(-0.5f+x, -0.5f+y, 0.5f, 1.0f);
    svQuad[0].tex = D3DXVECTOR2(fLeftU, fTopV);

    svQuad[1].pos = D3DXVECTOR4(fWidth5, -0.5f+y, 0.5f, 1.0f);
    svQuad[1].tex = D3DXVECTOR2(fRightU, fTopV);

    svQuad[2].pos = D3DXVECTOR4(-0.5f+x, fHeight5, 0.5f, 1.0f);
    svQuad[2].tex = D3DXVECTOR2(fLeftU, fBottomV);

    svQuad[3].pos = D3DXVECTOR4(fWidth5, fHeight5, 0.5f, 1.0f);
    svQuad[3].tex = D3DXVECTOR2(fRightU, fBottomV);

    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE , FALSE );
    m_pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
    m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, svQuad, sizeof(SCREEN_VERTEX));
    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, value);
	m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE , zwritevalue );
}
HRESULT CDymRenderSystem::CreateTextureFromFile(LPCTSTR pSrcFile,CDymD3DTexture ** ppTexture)
{
	*ppTexture = (CDymD3DTexture*)m_dataBuffer.LoadBuffer ( pSrcFile , MRT_TEXTURE , NULL );
	if( *ppTexture )return S_OK;
	LPDIRECT3DTEXTURE9 pTex;
	HRESULT hr = D3DXCreateTextureFromFile( m_pd3dDevice , pSrcFile , &pTex );
	if(FAILED(hr))return hr;
	*ppTexture = new CDymD3DTexture( this );
	(*ppTexture)->m_pTex = pTex;
	m_dataBuffer.AddBuffer ( *ppTexture , 0 , MRT_TEXTURE , pSrcFile );
	return S_OK;
}
HRESULT CDymRenderSystem::CreateTexture( CDymD3DTexture **ppTexture , int width , int height )
{
	LPDIRECT3DTEXTURE9 pTex;
	HRESULT hr = D3DXCreateTexture( m_pd3dDevice , width , height , 1, NULL , D3DFMT_A8R8G8B8 , D3DPOOL_MANAGED , &pTex );
	if(FAILED(hr))return hr;
	*ppTexture = new CDymD3DTexture( this );
	(*ppTexture)->m_pTex = pTex;
	return S_OK;
}
HRESULT CDymRenderSystem::CreateMeshFromFile(LPCTSTR pSrcFile , CDymMagicMesh **ppMesh )
{
	*ppMesh = (CDymMagicMesh*)m_dataBuffer.LoadBuffer ( pSrcFile , MRT_MESH , NULL );
	if( *ppMesh )return S_OK;

	*ppMesh = new CDymMagicMesh( this );
	HRESULT hr = (*ppMesh)->LoadFromMeshFile ( pSrcFile );
	if( FAILED( hr ) )return hr;
	m_dataBuffer.AddBuffer ( *ppMesh , 0 , MRT_MESH , pSrcFile );
	return S_OK;
}
void *CDymRenderSystem::LoadFromFile( LPCTSTR fullpathfilename , int *pSizeOut )
{
	int nFileSize;
	FILE *p = fopen( fullpathfilename , "rb" );
	if(!p)return NULL ;
	fseek(p,0,SEEK_END);
	nFileSize = ftell(p);
	fseek(p,0,SEEK_SET);
	char *pFileBuffer = new char[nFileSize+1];
	fread(pFileBuffer,nFileSize,1,p);
	fclose(p);
	*pSizeOut = nFileSize ;
	return pFileBuffer;
}
HRESULT CDymRenderSystem::ReleaseTexture( CDymD3DTexture *pTexture )
{
	if( !m_dataBuffer.UnLoadBuffer ( pTexture , MRT_TEXTURE , NULL ) )
		return E_FAIL;
	return S_OK;
}
HRESULT CDymRenderSystem::ReleaseMesh( CDymMagicMesh *pMesh )
{
	if( !m_dataBuffer.UnLoadBuffer ( pMesh , MRT_MESH , NULL ) )
		return E_FAIL;
	return S_OK;
}
void CDymRenderSystem::SetFastRender ( BOOL bFast )
{
	m_bFastRender = bFast ;
}
//----------------------------------------------------------------------------------------------
//
// CDymMaigcResourceBuffer
//
//----------------------------------------------------------------------------------------------
CDymMaigcResourceBuffer:: ~CDymMaigcResourceBuffer()
{
	ClearBuffer ();
}
void CDymMaigcResourceBuffer::DelObj( CDymResourceBuffer::BUFFER_LIST *pDel )
{
	if( pDel->datatype == MRT_TEXTURE )
		delete ((CDymD3DTexture*)(pDel->pData) );
	else if( pDel->datatype == MRT_MESH )
		delete ((CDymMagicMesh*)(pDel->pData) );
	delete pDel;
}
//----------------------------------------------------------------------------------------------
//
// CDymD3DTexture
//
//----------------------------------------------------------------------------------------------
CDymD3DTexture::CDymD3DTexture(CDymRenderSystem *pDevice)
{
	m_pDevice = pDevice;
	m_pTex = NULL;
}

CDymD3DTexture::~CDymD3DTexture(void)
{
	if(m_pTex)
		m_pTex->Release ();
}

HRESULT CDymD3DTexture::Release(void)
{
	return m_pDevice->ReleaseTexture ( this );
}
D3DSURFACE_DESC CDymD3DTexture::GetDesc()
{
	D3DSURFACE_DESC desc;
	ZeroMemory( &desc , sizeof( D3DSURFACE_DESC ) );
	if( m_pTex )
	{
		LPDIRECT3DSURFACE9 pSur;
		m_pTex->GetSurfaceLevel ( 0 , &pSur );
		pSur->GetDesc ( &desc );
		pSur->Release ();
	}
	return desc;
}
//----------------------------------------------------------------------------------------------
// CDymD3DVertexBuffer
//----------------------------------------------------------------------------------------------
CDymD3DVertexBuffer::CDymD3DVertexBuffer(void)
{
	m_pVB = NULL;
}

CDymD3DVertexBuffer::~CDymD3DVertexBuffer(void)
{
}

HRESULT CDymD3DVertexBuffer::Lock( UINT OffsetToLock,UINT SizeToLock,VOID ** ppbData,DWORD Flags)
{
	return m_pVB->Lock ( OffsetToLock , SizeToLock , ppbData , Flags );
}
HRESULT CDymD3DVertexBuffer::Unlock()
{
	return m_pVB->Unlock();
}
HRESULT CDymD3DVertexBuffer::Release(void)
{
	if(m_pVB)
		m_pVB->Release ();
	delete this;
	return S_OK;
}
//----------------------------------------------------------------------------------------------
// CDymD3DIndexBuffer
//----------------------------------------------------------------------------------------------
CDymD3DIndexBuffer::CDymD3DIndexBuffer(void)
{
	m_pIB = NULL;
}

CDymD3DIndexBuffer::~CDymD3DIndexBuffer(void)
{
}

HRESULT CDymD3DIndexBuffer::Lock( UINT OffsetToLock,UINT SizeToLock,VOID ** ppbData,DWORD Flags)
{
	return m_pIB->Lock ( OffsetToLock , SizeToLock , ppbData , Flags );
}
HRESULT CDymD3DIndexBuffer::Unlock()
{
	return m_pIB->Unlock();
}
HRESULT CDymD3DIndexBuffer::Release(void)
{
	if(m_pIB)
		m_pIB->Release ();
	delete this;
	return S_OK;
}

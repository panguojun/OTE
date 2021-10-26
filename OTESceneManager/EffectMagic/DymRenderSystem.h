/***************************************************************************************************
文件名称:	DymRenderSystem.h
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	渲染管理
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <list>
#include "DymResourceBuffer.h"

class CDymRenderSystem;
class CDymMagicMesh;

class CDymD3DTexture 
{
	CDymRenderSystem *m_pDevice;
public:
	IDirect3DTexture9 *m_pTex;
	CDymD3DTexture(CDymRenderSystem *pDevice);
	~CDymD3DTexture(void);
	HRESULT Release(void);
	D3DSURFACE_DESC CDymD3DTexture::GetDesc();
};

class CDymD3DIndexBuffer
{
public:
	IDirect3DIndexBuffer9 *m_pIB;

	CDymD3DIndexBuffer(void);
	~CDymD3DIndexBuffer(void);
	HRESULT Lock( UINT OffsetToLock,UINT SizeToLock,VOID ** ppbData,DWORD Flags);
	HRESULT Unlock();
	HRESULT Release(void);
};

class CDymD3DVertexBuffer
{
public:
	IDirect3DVertexBuffer9 * m_pVB;

	CDymD3DVertexBuffer(void);
	~CDymD3DVertexBuffer(void);
	HRESULT Lock( UINT OffsetToLock,UINT SizeToLock,VOID ** ppbData,DWORD Flags);
	HRESULT Unlock();
	HRESULT Release(void);
};

enum
{
	MRT_TEXTURE ,
	MRT_MESH ,
};
class __declspec( dllexport ) CDymMaigcResourceBuffer :public CDymResourceBuffer
{
public:
	virtual ~CDymMaigcResourceBuffer();
	virtual void DelObj( CDymResourceBuffer::BUFFER_LIST *pDel );
};
class RenderPrimitveUpOptimize
{
#define RPO_BUFFER_SIZE (512*1024)
public:
	BOOL m_bUsingOpt;
	LPDIRECT3DBASETEXTURE9 m_pTexture;
	DWORD m_dwFVF;
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	char *m_pVBBuffer;
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	DWORD m_dwStride;
	DWORD m_dwPrimitiveCount;

	RenderPrimitveUpOptimize()
	{
		m_bUsingOpt = FALSE;
		m_pTexture = NULL;
		m_dwFVF = 0;
		m_pVBBuffer = NULL;
		m_pVB = NULL;
		m_dwStride = 0;
		m_dwPrimitiveCount = 0;
	}
	HRESULT init( LPDIRECT3DDEVICE9 pDevice )
	{
		pDevice->CreateVertexBuffer ( RPO_BUFFER_SIZE , D3DUSAGE_WRITEONLY,
			0, D3DPOOL_DEFAULT, &m_pVB,NULL);
		m_pVBBuffer = new char[ RPO_BUFFER_SIZE ];
		m_pd3dDevice = pDevice;
		return S_OK;
	}
	HRESULT Release()
	{
		if( m_pVB ){m_pVB->Release ();m_pVB = NULL;}
		if( m_pVBBuffer ){delete[] m_pVBBuffer;m_pVBBuffer = NULL;}
		return S_OK;
	}
	HRESULT DrawPrimitiveUp( D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
	{
		DWORD fillmode;
		m_pd3dDevice->GetRenderState( D3DRS_FILLMODE,	&fillmode);
		if( PrimitiveType != D3DPT_TRIANGLELIST || fillmode != D3DFILL_SOLID )
			return m_pd3dDevice->DrawPrimitiveUP ( PrimitiveType , PrimitiveCount , pVertexStreamZeroData , VertexStreamZeroStride );
		DWORD fvf;
		m_pd3dDevice->GetFVF ( &fvf );
		LPDIRECT3DBASETEXTURE9 pTexture;
		m_pd3dDevice->GetTexture ( 0 , &pTexture );
		if( m_bUsingOpt )
		{
			if( fvf != m_dwFVF )goto END_DRAW;
			if( pTexture != m_pTexture )goto END_DRAW;
		}
		else
		{
			m_bUsingOpt = TRUE;
			m_dwFVF = fvf;
			m_pTexture = pTexture;
		}
		DWORD stride = 0;
		if( fvf & D3DFVF_XYZ )stride += sizeof( float )*3;
		if( fvf & D3DFVF_NORMAL )stride += sizeof( float )*3;
		if( fvf & D3DFVF_DIFFUSE )stride += sizeof( DWORD );
		if( fvf & D3DFVF_XYZB1 )stride += sizeof( float );
		if( fvf & D3DFVF_TEX1 )stride += sizeof( float )*2;
		m_dwStride = stride;
		if( (m_dwPrimitiveCount+PrimitiveCount)*VertexStreamZeroStride*3 + stride > RPO_BUFFER_SIZE )goto END_DRAW;
		for( int i=0;i<(int)PrimitiveCount*3;i++ )
		{
			memcpy( m_pVBBuffer+(m_dwPrimitiveCount*3+i)*stride , (char*)(pVertexStreamZeroData)+i*VertexStreamZeroStride , stride );
			D3DXVECTOR3 *p = (D3DXVECTOR3*)(m_pVBBuffer+(m_dwPrimitiveCount*3+i)*stride);
			D3DXMATRIX matWorld;
			m_pd3dDevice->GetTransform ( D3DTS_WORLD , &matWorld );
			D3DXVECTOR4 v4;
			D3DXVec3Transform( &v4 , p , &matWorld );

			DWORD dwVertexBlend ;
			m_pd3dDevice->GetRenderState( D3DRS_VERTEXBLEND, &dwVertexBlend );
			if( dwVertexBlend == D3DVBF_1WEIGHTS )
			{
				D3DXMATRIX matWorld2;
				m_pd3dDevice->GetTransform ( D3DTS_WORLD1 , &matWorld2 );
				D3DXVECTOR4 v4_2;
				D3DXVec3Transform( &v4_2 , p , &matWorld2 );
				float blend = *(float*)((char*)p + sizeof( float )*3 );
				p->x = v4.x/v4.w*blend + v4_2.x/v4_2.w*(1.0f-blend) ;
				p->y = v4.y/v4.w*blend + v4_2.y/v4_2.w*(1.0f-blend) ;
				p->z = v4.z/v4.w*blend + v4_2.z/v4_2.w*(1.0f-blend) ;
			}
			else
			{
				p->x = v4.x/v4.w ;
				p->y = v4.y/v4.w ;
				p->z = v4.z/v4.w ;
			}
		}
		m_dwPrimitiveCount += PrimitiveCount;
		return S_OK;
END_DRAW:
		Draw();
		return DrawPrimitiveUp( PrimitiveType , PrimitiveCount , pVertexStreamZeroData , VertexStreamZeroStride );
	}
	void EndRender()
	{
		Draw();
	}
	void Draw()
	{
		if( m_dwPrimitiveCount>0 )
		{
			LPDIRECT3DVERTEXBUFFER9 poldvb;
			UINT offset , oldstride;
			D3DXMATRIX oldworldmatrix,matworld;
			m_pd3dDevice->GetTransform ( D3DTS_WORLD , &oldworldmatrix );
			D3DXMatrixIdentity( &matworld );
			m_pd3dDevice->SetTransform ( D3DTS_WORLD , &matworld );

			DWORD dwTextureFactor , dwVertexBlend ;
			m_pd3dDevice->GetRenderState ( D3DRS_TEXTUREFACTOR , &dwTextureFactor );
			m_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR , 0xFFFFFFFF );
			m_pd3dDevice->GetRenderState( D3DRS_VERTEXBLEND, &dwVertexBlend );
			m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_DISABLE );

			void *pvbdata;
			m_pVB->Lock ( 0 , 0 , &pvbdata , D3DLOCK_DISCARD );
			memcpy( pvbdata , m_pVBBuffer , m_dwPrimitiveCount * m_dwStride * 3 );
			m_pVB->Unlock ();
			m_pd3dDevice->GetStreamSource ( 0 , &poldvb , &offset , &oldstride );
			m_pd3dDevice->SetStreamSource ( 0 , m_pVB , 0 , m_dwStride );
			LPDIRECT3DBASETEXTURE9 oldtex;
			m_pd3dDevice->GetTexture ( 0 , &oldtex );
			m_pd3dDevice->SetTexture ( 0 , m_pTexture );
			if( m_pTexture == NULL )
				rand();
			m_pd3dDevice->DrawPrimitive ( D3DPT_TRIANGLELIST , 0 , m_dwPrimitiveCount );
			m_pd3dDevice->SetTexture ( 0 , oldtex );
			m_pd3dDevice->SetStreamSource ( 0 , poldvb , offset , oldstride );
			m_pd3dDevice->SetTransform ( D3DTS_WORLD , &oldworldmatrix );

			m_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR , dwTextureFactor );
			m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, dwVertexBlend );

			m_bUsingOpt = FALSE;
			m_pTexture = NULL;
			m_dwFVF = 0;
			m_dwStride = 0;
			m_dwPrimitiveCount = 0;
		}
	}
};
class __declspec( dllexport ) CDymRenderSystem
{
protected:
	D3DXVECTOR3 m_vSca;
	float m_fRot;
	LPDIRECT3DTEXTURE9 m_pRtTexture1;
	LPDIRECT3DSURFACE9		m_pBkSurface;
	LPDIRECT3DSURFACE9		m_pRtSurface1;
	BOOL m_bTrackingBoundingBox;
	BOOL m_bTrackingBoundingSphere;
	BOOL m_bFastRender;

	/*struct RESOURCE_BUFFER
	{
		int nType;
		void *pResource;
		int nSize;
	};
	std::list< RESOURCE_BUFFER* > m_listResourceBuffer;*/
	CDymMaigcResourceBuffer m_dataBuffer;
	RenderPrimitveUpOptimize m_renderPUO;

	void _TrackBounding( D3DPRIMITIVETYPE PrimitiveType , UINT PrimitiveCount , CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride );
public:
	LPDIRECT3DTEXTURE9 m_pRtShadow0;
	LPDIRECT3DTEXTURE9 m_pRtShadow1;

	LPDIRECT3DDEVICE9 m_pd3dDevice;

	LPD3DXEFFECT m_pShadowEffect;
	LPD3DXEFFECT m_pGrassEffect;
	BOOL m_bGrass;

	struct BOUNDING_BOX
	{
		D3DXVECTOR3 vMin;
		D3DXVECTOR3 vMax;
	};
	struct BOUNDING_SPHERE
	{
		D3DXVECTOR3 vCenter;
		float fRadio;
	};
	std::list< BOUNDING_BOX* > m_listBoundingBox;
	std::list< BOUNDING_SPHERE * > m_listBoundingSphere;

	CDymRenderSystem(void);
	~CDymRenderSystem(void);
	void init( LPDIRECT3DDEVICE9 pDevice );
	HRESULT GetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9 ** ppRenderTarget);
	HRESULT SetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9 * pRenderTarget);
	HRESULT Clear(DWORD Count,CONST D3DRECT * pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil);
	virtual HRESULT SetTransform( D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX * pMatrix );
	virtual HRESULT GetTransform( D3DTRANSFORMSTATETYPE State,D3DMATRIX * pMatrix);
	HRESULT SetRenderState( D3DRENDERSTATETYPE State,DWORD Value );
	HRESULT GetRenderState( D3DRENDERSTATETYPE State,DWORD *Value );
	HRESULT SetTexture( DWORD Sampler, CDymD3DTexture * pTexture );
	HRESULT CreateVertexBuffer( UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,CDymD3DVertexBuffer** ppVertexBuffer,HANDLE* pSharedHandle);
	HRESULT CreateIndexBuffer(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,CDymD3DIndexBuffer** ppIndexBuffer,HANDLE* pSharedHandle);
	HRESULT SetIndices(CDymD3DIndexBuffer * pIndexData);
	HRESULT SetTextureStageState( DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value);
	HRESULT SetFVF( DWORD FVF);
	HRESULT SetStreamSource( UINT StreamNumber,CDymD3DVertexBuffer * pStreamData,UINT OffsetInBytes,UINT Stride);
	HRESULT DrawPrimitive( D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount);
	HRESULT DrawPrimitiveUP( D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	HRESULT DrawIndexedPrimitive( D3DPRIMITIVETYPE Type,INT BaseVertexIndex,UINT MinIndex,UINT NumVertices,UINT StartIndex,UINT PrimitiveCount);
	void DrawFullScreenQuad(int x=0,int y=0,int w=0,int h=0,float fLeftU=0.0f, float fTopV=0.0f, float fRightU=1.0f, float fBottomV=1.0f);

	HRESULT SetSamplerState( DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value);

	virtual HRESULT CreateTextureFromFile( LPCTSTR pSrcFile,CDymD3DTexture ** ppTexture);
	virtual HRESULT CreateTexture( CDymD3DTexture **ppTexture , int width , int height );
	HRESULT CreateMeshFromFile(LPCTSTR pSrcFile , CDymMagicMesh **ppMesh );
	virtual void *LoadFromFile( LPCTSTR fullpathfilename , int *pSizeOut );
	HRESULT ReleaseTexture( CDymD3DTexture *pTexture );
	HRESULT ReleaseMesh( CDymMagicMesh *pMesh );
	HRESULT Release(void);
	void SetScaling( D3DXVECTOR3 vSca );
	D3DXVECTOR3 GetScaling(void);
	void SetRotation(float fRot);
	float GetRotation(void);
	void DrawLine(D3DXVECTOR3 v1,D3DXVECTOR3 v2,DWORD color);
	void DrawLineBox( D3DXVECTOR3 v1 , D3DXVECTOR3 v2 , DWORD color );
	HRESULT BeginRender(int mipLever);
	HRESULT EndRender();
	void TrackBoundingBox( BOOL bTrack );
	void TrackBoundingSphere( BOOL bTrack );
	void ClearBoundingBoxList();
	void CleraBoundingSphere();
	void AddBoundingSphereToBox( D3DXVECTOR3 v , float r );
	void SetFastRender( BOOL bFast );
	void End();
	HRESULT BeginGrass();
	HRESULT EndGrass();
	void GrassSetBegin();
	void GrassSetEnd();
	BOOL DrawCircle( D3DXVECTOR3 origal , float radio , DWORD color);
};
/***************************************************************************************************
文件名称:	DymMagicToOTE.cpp
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	特效插件转换到OTE
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#include ".\dymmagictoote.h"

CDymMagicToOTE g_dymMagicToOTE;

//---------------------------------------------------------------------------------------------
//
// CDymMagicRenderSystemForOTE
//
//---------------------------------------------------------------------------------------------
HRESULT CDymMagicRenderSystemForOTE::CreateTextureFromFile( LPCTSTR pSrcFile,CDymD3DTexture ** ppTexture)
{
	if( pSrcFile[0] == 0 )return E_FAIL;

	*ppTexture = (CDymD3DTexture*)m_dataBuffer.LoadBuffer ( pSrcFile , MRT_TEXTURE , NULL );
	if( *ppTexture )return S_OK;

	LPDIRECT3DTEXTURE9 pTex;
	DataStreamPtr stream = ResourceGroupManager::getSingleton().openResource(pSrcFile, "MagicGroup");
	if( stream.isNull () )return E_FAIL;
	if( stream->size() <=0 )return E_FAIL;
	char *pData = new char[stream->size()];
	stream->read ( pData , stream->size () );
	HRESULT hr = D3DXCreateTextureFromFileInMemory( m_pd3dDevice , pData , stream->size() , &pTex );
	delete[]pData;
	if(FAILED(hr))return hr;
	*ppTexture = new CDymD3DTexture( this );
	(*ppTexture)->m_pTex = pTex;
	m_dataBuffer.AddBuffer ( *ppTexture , 0 , MRT_TEXTURE , pSrcFile );
	return S_OK;

	/**ppTexture = (CDymD3DTexture*)m_dataBuffer.LoadBuffer ( pSrcFile , MRT_TEXTURE , NULL );
	if( *ppTexture )return S_OK;
	LPDIRECT3DTEXTURE9 pTex;
	HRESULT hr = D3DXCreateTextureFromFile( m_pd3dDevice , pSrcFile , &pTex );
	if(FAILED(hr))return hr;
	*ppTexture = new CDymD3DTexture( this );
	(*ppTexture)->m_pTex = pTex;
	m_dataBuffer.AddBuffer ( *ppTexture , 0 , MRT_TEXTURE , pSrcFile );*/
}
void *CDymMagicRenderSystemForOTE::LoadFromFile( LPCTSTR fullpathfilename , int *pSizeOut )
{
	DataStreamPtr stream = ResourceGroupManager::getSingleton().openResource(fullpathfilename, "MagicGroup");
	if( stream.isNull () )return NULL;
	if( stream->size() <=0 )return NULL;
	*pSizeOut = stream->size();
	char *pBuffer = new char[stream->size()+1];
	char *pData = new char[stream->size()];
	stream->read ( pData , stream->size () );
	memcpy( pBuffer , pData , stream->size() );
	delete[] pData;
	return pBuffer;
}
//---------------------------------------------------------------------------------------------
//
// CDymMagicSoundSystemForOTE
//
//---------------------------------------------------------------------------------------------
HRESULT CDymMagicSoundSystemForOTE::CreateSoundFromFile( LPTSTR filename , CDymSoundObj *pObj )
{return 0; // by romeo
	DataStreamPtr stream = ResourceGroupManager::getSingleton().openResource(filename, "MagicGroup");
	if( stream.isNull () )return NULL;
	if( stream->size() <=0 )return NULL;
	char *pData = new char[stream->size() ];
	stream->read (pData , stream->size () );
	HRESULT hr = pObj->CreateFromMemory ( m_pSoundBuffer , m_pDsd ,  pData , stream->size() , filename );
	delete[]pData;
	return hr;
}
//---------------------------------------------------------------------------------------
//
// CDymMagicToOTE_Obj
//
//---------------------------------------------------------------------------------------
CDymMagicToOTE_Obj::CDymMagicToOTE_Obj( LPCTSTR strID , CDymMagicRenderSystemForOTE *pDevice , LPCTSTR strMagicFileName , CDymSoundManager *pSound )
{
	m_bVisible = false;
	m_vPosOTE = D3DXVECTOR3( 0 , 0 , 0 );
	m_vScaOTE = D3DXVECTOR3( 1 , 1 , 1 );
	m_vAxisOTE = D3DXVECTOR3( 0 , 0 , 0 );
	m_fAngleOTE = 0.0f;
	m_bRenderBoundingBox = FALSE;
	m_dwBoundingBoxColor = 0;

	m_pStrID = new TCHAR[_tcslen(strID)+1];
	_tcscpy( m_pStrID , strID );
	m_pMagic = NULL ;
	DataStreamPtr stream = ResourceGroupManager::getSingleton().openResource(strMagicFileName, "MagicGroup");
	if( !stream.isNull () )
	{
		m_pMagic = new CDymMagic( pDevice , pSound );
		char *pData = new char[stream->size() ];
		stream->read (pData , stream->size () );
		m_pMagic->LoadFromMemory ( pData , stream->size () );
		delete[]pData;

		m_pMagic->Play ();
	}
}
CDymMagicToOTE_Obj::~CDymMagicToOTE_Obj()
{
	if( m_pStrID )delete[] m_pStrID;
	if( m_pMagic )delete m_pMagic;
}
TCHAR *CDymMagicToOTE_Obj::GetStrID()
{
	return m_pStrID;
}
void CDymMagicToOTE_Obj::SetPosition( float x , float y , float z )
{
	if( m_pMagic )
	{
		m_pMagic->SetPosition ( D3DXVECTOR3( -x , y , z ) );
		m_vPosOTE = D3DXVECTOR3( x , y , z );
	}
}
void CDymMagicToOTE_Obj::GetPosition( float *px , float *py , float *pz )
{
	if( px )*px = m_vPosOTE.x;
	if( py )*py = m_vPosOTE.y;
	if( pz )*pz = m_vPosOTE.z;
}
void CDymMagicToOTE_Obj::SetScaling( float x , float y , float z )
{
	if( m_pMagic )
	{
		m_pMagic->SetScaling ( D3DXVECTOR3( x , y , z ) );
		m_vScaOTE = D3DXVECTOR3( x , y , z );
	}
}
void CDymMagicToOTE_Obj::GetScaling( float *px , float *py , float *pz )
{
	if( px )*px = m_vScaOTE.x;
	if( py )*py = m_vScaOTE.y;
	if( pz )*pz = m_vScaOTE.z;
}
void CDymMagicToOTE_Obj::SetRotation( float angle , float axisX , float axisY , float axisZ )
{
	if( m_pMagic )
	{
		m_pMagic->SetRotation ( angle );
		m_pMagic->SetRotationAxis ( D3DXVECTOR3( axisX , -axisY , -axisZ ) );
		m_vAxisOTE = D3DXVECTOR3( axisX , axisY , axisZ );
		m_fAngleOTE = angle ;
	}
}
void CDymMagicToOTE_Obj::GetRotation( float *pAngle , float *pAxisX , float *pAxisY , float *pAxisZ )
{
	if( pAngle )*pAngle = m_fAngleOTE;
	if( pAxisX )*pAxisX = m_vAxisOTE.x;
	if( pAxisY )*pAxisY = m_vAxisOTE.y;
	if( pAxisZ )*pAxisZ = m_vAxisOTE.z;
}
void CDymMagicToOTE_Obj::GetBoundingBox( float *pX1 , float *pY1 , float *pZ1 , float *pX2 , float *pY2 , float *pZ2 )
{
	D3DXVECTOR3 vMin , vMax;
	if( m_pMagic )m_pMagic->GetBoundingBoxRealTime ( &vMin , &vMax );
	*pX1 = vMin.x ;
	*pY1 = vMin.y ;
	*pZ1 = vMin.z ;
	*pX2 = vMax.x ;
	*pY2 = vMax.y ;
	*pZ2 = vMax.z ;
}
void CDymMagicToOTE_Obj::SetRenderBoundingBox( BOOL bRender , DWORD color )
{
	m_bRenderBoundingBox = bRender;
	m_dwBoundingBoxColor = color;
}
void CDymMagicToOTE_Obj::Render( DWORD flag )
{
	if(!m_bVisible)
		return;

	D3DXVECTOR3 vMin , vMax;
	if( m_pMagic )m_pMagic->Render (flag);
}
void CDymMagicToOTE_Obj::FrameMove( float fElapsedTime )
{
	if( m_pMagic )m_pMagic->FrameMove (fElapsedTime);
}
float CDymMagicToOTE_Obj::GetSoundArea()
{
	if( m_pMagic )return m_pMagic->GetSoundArea();
	return 0.0f;
}
//---------------------------------------------------------------------------------------
//
// CDymMagicToOTE
//
//---------------------------------------------------------------------------------------
CDymMagicToOTE::CDymMagicToOTE(void)
{
}

CDymMagicToOTE::~CDymMagicToOTE(void)
{
	for( std::list< CDymMagicToOTE_Obj* >::iterator it = m_listObj.begin () ; it != m_listObj.end () ; it++ )
	{
		CDymMagicToOTE_Obj *pObj = *it;
		delete pObj;
	}
	m_listObj.clear ();
}
CDymMagicToOTE_Obj *CDymMagicToOTE::_FindObj( LPCTSTR strID )
{
	for( std::list< CDymMagicToOTE_Obj* >::iterator it = m_listObj.begin () ; it != m_listObj.end () ; it++ )
	{
		CDymMagicToOTE_Obj *pObj = *it;
		if( strID == pObj->GetStrID() )return pObj;
		if( _tcscmp( strID , pObj->GetStrID () ) == 0 )
			return pObj;
	}
	return NULL;
}
void CDymMagicToOTE::init( LPDIRECT3DDEVICE9 pDevice )
{
	m_renderSystem.init ( pDevice );
	m_soundSystem.init ( GetActiveWindow() );
}
void CDymMagicToOTE::CreateMagic( LPCTSTR strMagicFileName , LPCTSTR strID )
{
	CDymMagicToOTE_Obj *pMagic = new CDymMagicToOTE_Obj( strID , &m_renderSystem , strMagicFileName , &m_soundSystem );
	m_listObj.push_back ( pMagic );
}
void CDymMagicToOTE::DeleteMagic( LPCTSTR strID )
{
	CDymMagicToOTE_Obj *pMagic = _FindObj( strID );
	if( pMagic != NULL )
	{
		m_listObj.remove ( pMagic );
		delete pMagic;
	}
}
bool CDymMagicToOTE::FindMagic( LPCTSTR strName)
{
	CDymMagicToOTE_Obj *pMagic = _FindObj( strName );
	if( pMagic != NULL )
	{
		return true;
	}

	return false;
}
void CDymMagicToOTE::SetPosition( LPCTSTR strID , float x , float y , float z )
{
	CDymMagicToOTE_Obj *pMagic = _FindObj( strID );
	if( pMagic )pMagic->SetPosition ( x , y , z );
}
void CDymMagicToOTE::SetScaling( LPCTSTR strID , float x , float y , float z )
{
	CDymMagicToOTE_Obj *pMagic = _FindObj( strID );
	if( pMagic )pMagic->SetScaling ( x , y , z );
}
void CDymMagicToOTE::SetRotation( LPCTSTR strID , float angle , float axisX , float axisY , float axisZ )
{
	CDymMagicToOTE_Obj *pMagic = _FindObj( strID );
	if( pMagic )pMagic->SetRotation ( angle , axisX , axisY , axisZ );
}
void CDymMagicToOTE::GetBoundingBox( LPCTSTR strID , float *pX1 , float *pY1 , float *pZ1 , float *pX2 , float *pY2 , float *pZ2 )
{
	CDymMagicToOTE_Obj *pMagic = _FindObj( strID );
	if( pMagic )pMagic->GetBoundingBox ( pX1 , pY1 , pZ1 , pX2 , pY2 , pZ2 );
}
void CDymMagicToOTE::GetPosition( LPCTSTR strID ,float *px , float *py , float *pz )
{
	CDymMagicToOTE_Obj *pMagic = _FindObj( strID );
	if( pMagic )pMagic->GetPosition ( px , py , pz );
}
void CDymMagicToOTE::GetScaling( LPCTSTR strID ,float *px , float *py , float *pz )
{
	CDymMagicToOTE_Obj *pMagic = _FindObj( strID );
	if( pMagic )pMagic->GetScaling ( px , py , pz );
}
void CDymMagicToOTE::GetRotation( LPCTSTR strID ,float *pAngle , float *pAxisX , float *pAxisY , float *pAxisZ )
{
	CDymMagicToOTE_Obj *pMagic = _FindObj( strID );
	if( pMagic )pMagic->GetRotation ( pAngle , pAxisX , pAxisY , pAxisZ );
}
void CDymMagicToOTE::SetRenderBoundingBox( LPCTSTR strID ,BOOL bRender , DWORD color )
{
	CDymMagicToOTE_Obj *pMagic = _FindObj( strID );
	if( pMagic )pMagic->SetRenderBoundingBox ( bRender , color );
}
void CDymMagicToOTE::FrameMove( float fElapsedTime )
{
	for( std::list< CDymMagicToOTE_Obj* >::iterator it = m_listObj.begin () ; it != m_listObj.end () ; it++ )
	{
		CDymMagicToOTE_Obj *pMagic = *it;
		pMagic->FrameMove ( fElapsedTime );
	}
}
inline D3DXMATRIX makeD3DXMatrix( const Matrix4& mat )
{
	// Transpose matrix
	// D3D9 uses row vectors i.e. V*M
	// Ogre, OpenGL and everything else uses column vectors i.e. M*V
	D3DXMATRIX d3dMat;
	d3dMat.m[0][0] = mat[0][0];
	d3dMat.m[0][1] = mat[1][0];
	d3dMat.m[0][2] = mat[2][0];
	d3dMat.m[0][3] = mat[3][0];

	d3dMat.m[1][0] = mat[0][1];
	d3dMat.m[1][1] = mat[1][1];
	d3dMat.m[1][2] = mat[2][1];
	d3dMat.m[1][3] = mat[3][1];

	d3dMat.m[2][0] = mat[0][2];
	d3dMat.m[2][1] = mat[1][2];
	d3dMat.m[2][2] = mat[2][2];
	d3dMat.m[2][3] = mat[3][2];

	d3dMat.m[3][0] = mat[0][3];
	d3dMat.m[3][1] = mat[1][3];
	d3dMat.m[3][2] = mat[2][3];
	d3dMat.m[3][3] = mat[3][3];

	return d3dMat;
}
void ResetRenderState( LPDIRECT3DDEVICE9 m_pDevice )
{
/*g_pDevice->SetRenderState( D3DRS_ZENABLE , D3DZB_TRUE );
g_pDevice->SetRenderState( D3DRS_FILLMODE , D3DFILL_SOLID);
g_pDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
g_pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
g_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE);
g_pDevice->SetRenderState( D3DRS_LASTPIXEL, TRUE);
g_pDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE);
g_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO);
g_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW);
g_pDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
g_pDevice->SetRenderState( D3DRS_ALPHAREF, 0);
g_pDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
g_pDevice->SetRenderState( D3DRS_DITHERENABLE, FALSE);
g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
g_pDevice->SetRenderState( D3DRS_FOGENABLE, FALSE);
g_pDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE);
g_pDevice->SetRenderState( D3DRS_FOGCOLOR, 0);
g_pDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE);
g_pDevice->SetRenderState( D3DRS_RANGEFOGENABLE, FALSE);
g_pDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE);
g_pDevice->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
g_pDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
g_pDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
g_pDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
g_pDevice->SetRenderState( D3DRS_STENCILREF, 0);
g_pDevice->SetRenderState( D3DRS_STENCILMASK, 0xFFFFFFFF);
g_pDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
g_pDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
g_pDevice->SetRenderState( D3DRS_WRAP0, 0);
g_pDevice->SetRenderState( D3DRS_CLIPPING, TRUE);
g_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE);
g_pDevice->SetRenderState( D3DRS_AMBIENT, 0);
g_pDevice->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
g_pDevice->SetRenderState( D3DRS_COLORVERTEX, TRUE);
g_pDevice->SetRenderState( D3DRS_LOCALVIEWER, TRUE);
g_pDevice->SetRenderState( D3DRS_NORMALIZENORMALS, FALSE);
g_pDevice->SetRenderState( D3DRS_NORMALIZENORMALS, D3DMCS_COLOR1);
g_pDevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2);
g_pDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
g_pDevice->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
g_pDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
g_pDevice->SetRenderState( D3DRS_CLIPPLANEENABLE, 0);
g_pDevice->SetRenderState( D3DRS_POINTSPRITEENABLE , FALSE);
g_pDevice->SetRenderState( D3DRS_POINTSCALEENABLE, FALSE);
g_pDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, TRUE);
g_pDevice->SetRenderState( D3DRS_MULTISAMPLEMASK, 0xFFFFFFFF);
g_pDevice->SetRenderState( D3DRS_PATCHEDGESTYLE, D3DPATCHEDGE_DISCRETE);
g_pDevice->SetRenderState( D3DRS_DEBUGMONITORTOKEN, D3DDMT_ENABLE);
g_pDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
g_pDevice->SetRenderState( D3DRS_COLORWRITEENABLE, 0x0000000F);
g_pDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD);
g_pDevice->SetRenderState( D3DRS_POSITIONDEGREE, D3DDEGREE_CUBIC);
g_pDevice->SetRenderState( D3DRS_NORMALDEGREE, D3DDEGREE_LINEAR);
g_pDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE);
g_pDevice->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, 0);
g_pDevice->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, FALSE);
g_pDevice->SetRenderState( D3DRS_CCW_STENCILFAIL, D3DSTENCILOP_KEEP);
g_pDevice->SetRenderState( D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_KEEP);
g_pDevice->SetRenderState( D3DRS_CCW_STENCILPASS, D3DSTENCILOP_KEEP);
g_pDevice->SetRenderState( D3DRS_CCW_STENCILFUNC, D3DCMP_ALWAYS);
g_pDevice->SetRenderState( D3DRS_COLORWRITEENABLE1, 0x0000000F);
g_pDevice->SetRenderState( D3DRS_COLORWRITEENABLE2, 0x0000000F);
g_pDevice->SetRenderState( D3DRS_COLORWRITEENABLE3, 0x0000000F);
g_pDevice->SetRenderState( D3DRS_BLENDFACTOR, 0xFFFFFFFF);
g_pDevice->SetRenderState( D3DRS_SRGBWRITEENABLE, 0);
g_pDevice->SetRenderState( D3DRS_DEPTHBIAS, 0);
g_pDevice->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, 0);
g_pDevice->SetRenderState( D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
g_pDevice->SetRenderState( D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO);
g_pDevice->SetRenderState( D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);*/

D3DXMATRIX matIdentity;
D3DXMatrixIdentity( &matIdentity );
m_pDevice->SetTransform ( D3DTS_TEXTURE0 , &matIdentity );
m_pDevice->SetTransform ( D3DTS_TEXTURE1 , &matIdentity );
m_pDevice->SetTransform ( D3DTS_TEXTURE2 , &matIdentity );


m_pDevice->SetTextureStageState( 0 , D3DTSS_COLOROP , D3DTOP_DISABLE );
m_pDevice->SetTextureStageState( 0 , D3DTSS_COLORARG1, D3DTA_TEXTURE);
m_pDevice->SetTextureStageState( 0 , D3DTSS_COLORARG2, D3DTA_CURRENT);
m_pDevice->SetTextureStageState( 0 , D3DTSS_ALPHAOP, D3DTOP_DISABLE);
m_pDevice->SetTextureStageState( 0 , D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
m_pDevice->SetTextureStageState( 0 , D3DTSS_ALPHAARG2, D3DTA_CURRENT);
float f=0.0f;
m_pDevice->SetTextureStageState( 0 , D3DTSS_BUMPENVMAT00 , (DWORD)&f );
m_pDevice->SetTextureStageState( 0 , D3DTSS_BUMPENVMAT01, (DWORD)&f );
m_pDevice->SetTextureStageState( 0 , D3DTSS_BUMPENVMAT10, (DWORD)&f );
m_pDevice->SetTextureStageState( 0 , D3DTSS_BUMPENVMAT11, (DWORD)&f );
m_pDevice->SetTextureStageState( 0 , D3DTSS_TEXCOORDINDEX , 0);
m_pDevice->SetTextureStageState( 0 , D3DTSS_BUMPENVLSCALE, (DWORD)&f);
m_pDevice->SetTextureStageState( 0 , D3DTSS_BUMPENVLOFFSET , (DWORD)&f );
m_pDevice->SetTextureStageState( 0 , D3DTSS_TEXTURETRANSFORMFLAGS , D3DTTFF_DISABLE);
m_pDevice->SetTextureStageState( 0 , D3DTSS_COLORARG0, 0);
m_pDevice->SetTextureStageState( 0 , D3DTSS_ALPHAARG0, 0);
m_pDevice->SetTextureStageState( 0 , D3DTSS_RESULTARG , D3DTA_CURRENT);
m_pDevice->SetTextureStageState( 0 , D3DTSS_CONSTANT, 0);


}

void CDymMagicToOTE::Render( Camera *pCamera )
{
	m_renderSystem.m_pd3dDevice->SetPixelShader ( NULL );
	m_renderSystem.m_pd3dDevice->SetVertexShader ( NULL );
	// 备份视矩阵和投影矩阵
	D3DXMATRIX matViewOld , matProjOld;
	m_renderSystem.m_pd3dDevice ->GetTransform( D3DTS_VIEW, &matViewOld );
	m_renderSystem.m_pd3dDevice ->GetTransform( D3DTS_PROJECTION, &matProjOld );
	DWORD dwFog;
	m_renderSystem.m_pd3dDevice->GetRenderState ( D3DRS_FOGENABLE , &dwFog );
	m_renderSystem.m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE , FALSE );

	ResetRenderState( m_renderSystem.m_pd3dDevice );
	// 设置视矩阵和投影矩阵
	Vector3 vEye = pCamera->getPosition ();
	D3DXVECTOR3 vEyePt( -vEye.x , vEye.y ,vEye.z );
	Vector3 vLookat = pCamera->getDirection ();
	
	D3DXVECTOR3 vLookatPt( -vLookat.x , vLookat.y , vLookat.z );
	vLookatPt += vEyePt;

	Vector3 vUp = pCamera->getUp ();
	D3DXVECTOR3 vUpVec( -vUp.x , vUp.y , vUp.z );

	D3DXMATRIXA16 matView;
	D3DXMATRIXA16 matProj;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	//matView._44 *=-1;
	
	float fNear = (float)pCamera->getNearClipDistance ();
	float fFar = (float)pCamera->getFarClipDistance ();

	if( fFar > fNear )
	{
		D3DXMatrixPerspectiveFovLH( &matProj, (float)pCamera->getFOVy ().valueRadians(),
			(float)pCamera->getAspectRatio(), 
			fNear, 
			fFar );
	}
	else
	{			
		D3DXMatrixPerspectiveFovLH( &matProj, (float)pCamera->getFOVy ().valueRadians(),
			(float)pCamera->getAspectRatio(), 
			fNear, 
			1000000 );	
		
		matProj(2,2) = 1;
		matProj(3,2) = -fNear;			
	}

	//if( g_pdymSceneManager->needUpdataViewProMatrices () )
	{
		{
			Matrix4 matrix4 = pCamera->getViewMatrix();
			matrix4[2][0] = -matrix4[2][0];
			matrix4[2][1] = -matrix4[2][1];
			matrix4[2][2] = -matrix4[2][2];
			matrix4[2][3] = -matrix4[2][3];
			D3DXMATRIX d3dmat = makeD3DXMatrix( matrix4 );
			//d3dmat._43 *= -1.0f;
			m_renderSystem.m_pd3dDevice ->SetTransform( D3DTS_VIEW, &matView );
		}

		{
			Matrix4 matrix4 = pCamera->getProjectionMatrix ();
			D3DXMATRIX d3dmat = makeD3DXMatrix( matrix4 );
			//d3dmat._43 *= -1.0f;
			m_renderSystem.m_pd3dDevice ->SetTransform( D3DTS_PROJECTION, &matProj );
		}
	}
	// 计算公告牌矩阵
	CDymMagicBillboard::ComputeBillboardMatrix ( vEyePt , vLookatPt , vUpVec );

	m_soundSystem.FrameMove ( vEyePt.x , vEyePt.y , vEyePt.z , 
		vLookatPt.x , vLookatPt.y , vLookatPt.z );

	m_renderSystem.m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	m_renderSystem.m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	m_renderSystem.m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE , TRUE );
	m_renderSystem.m_pd3dDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	m_renderSystem.m_pd3dDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);

	// Turn off ambient lighting 
	DWORD dwAmbient;
	DWORD lighting;
	m_renderSystem.m_pd3dDevice->GetRenderState( D3DRS_AMBIENT , &dwAmbient );
	m_renderSystem.m_pd3dDevice->GetRenderState( D3DRS_LIGHTING , &lighting );
	m_renderSystem.m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );
	m_renderSystem.m_pd3dDevice->SetRenderState ( D3DRS_LIGHTING , 0 );

	m_renderSystem.m_pd3dDevice->BeginScene ();
	for( std::list< CDymMagicToOTE_Obj* >::iterator it = m_listObj.begin() ; it != m_listObj.end() ; it++ )
	{
		CDymMagicToOTE_Obj *pMagic = *it;
		if( pMagic->m_bVisible)
		{
			pMagic->Render( 0 );
			//pMagic->m_nRender = 0;
			if( pMagic->m_bRenderBoundingBox )
			{
				D3DXVECTOR3 v1,v2;
				pMagic->GetBoundingBox ( &v1.x , &v1.y , &v1.z , &v2.x , &v2.y , &v2.z );
				m_renderSystem.DrawLineBox ( v1 , v2 , pMagic->m_dwBoundingBoxColor );
			}
		}
	}
	for( std::list< CDymMagicToOTE_Obj* >::iterator it = m_listObj.begin() ; it != m_listObj.end() ; it++ )
	{
		CDymMagicToOTE_Obj *pMagic = *it;
		//if( pMagic->m_nRender == 1 )
		{
			pMagic->Render( MAGIC_RENDER_SHADOW );
//			pMagic->m_nRender = 0;
		}
	}
	//m_renderSystem.m_pd3dDevice->SetTexture (0,0);
	//m_renderSystem.DrawFullScreenQuad (0,0,100,100);
	m_renderSystem.m_pd3dDevice->EndScene();
	//m_listObj.clear();
	// 恢复视矩阵和投影矩阵
	m_renderSystem.m_pd3dDevice ->SetTransform( D3DTS_VIEW, &matViewOld );
	m_renderSystem.m_pd3dDevice ->SetTransform( D3DTS_PROJECTION, &matProjOld );

	m_renderSystem.m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE , dwFog );

	//还原OGRE默认值
	m_renderSystem.m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
	m_renderSystem.m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE,   TRUE);
	m_renderSystem.m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, dwAmbient );
	m_renderSystem.m_pd3dDevice->SetRenderState ( D3DRS_LIGHTING , lighting );
	m_renderSystem.m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_DISABLE );
}
int CDymMagicToOTE::GetMagicNum()
{
	return m_listObj.size ();
}
LPCTSTR CDymMagicToOTE::GetMagicID( int num )
{
	num = num % m_listObj.size ();
	int i=0;
	for( std::list< CDymMagicToOTE_Obj* >::iterator it = m_listObj.begin () ; it != m_listObj.end () ; it++ )
	{
		if( i == num )
		{
			CDymMagicToOTE_Obj *pObj = *it;
			return pObj->GetStrID ();
		}
		i++;
	}
	return NULL;
}
float CDymMagicToOTE::GetSoundArea( LPCTSTR strID )
{
	CDymMagicToOTE_Obj *pMagic = _FindObj( strID );
	if( pMagic )return pMagic->GetSoundArea();
	return 0.0f;
}

// ------------------------------------------------------
// by romeo
CDymMagicToOTE_Obj* CDymMagicToOTE::GetMagicObj(LPCTSTR strID)
{
	return _FindObj(strID);
}
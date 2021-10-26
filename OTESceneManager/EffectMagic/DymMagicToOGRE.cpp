/***************************************************************************************************
文件名称:	DymMagicToOGRE.cpp
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	特效插件转换到OGRE
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#include "DymMagicToOGRE.h"

CDymMagicRenderSystemForOGRE *g_pMagicRenderSystem = NULL;
CDymMagicOGREManager g_dymMagicOGREManager;
SceneManager *g_pdymSceneManager;
CDymMagicSoundSystemForOGRE *g_pMagicSoundSystem = NULL;

void Play3DSound( char* soundname , float x , float y , float z )
{
	if( g_pMagicSoundSystem )
	{
		CDymSoundObj *pObj = g_pMagicSoundSystem->CreateSound ( soundname );
		pObj->SetPos ( x , y , z );
	}
}

void DymCreateMagicRenderSystem( void *pd3dDevice , SceneManager *pdymSceneManager )
{
	g_pdymSceneManager = pdymSceneManager;
	g_pMagicRenderSystem = new CDymMagicRenderSystemForOGRE();
	g_pMagicRenderSystem->init ( (LPDIRECT3DDEVICE9)pd3dDevice );

	g_pMagicSoundSystem = new CDymMagicSoundSystemForOGRE();
	g_pMagicSoundSystem->init( GetActiveWindow() );
}
void DymDestroyMagicRenderSystem()
{
	if( g_pMagicRenderSystem ) delete g_pMagicRenderSystem;
	if( g_pMagicSoundSystem ) delete g_pMagicSoundSystem;
}

//---------------------------------------------------------------------------------------------
//
// CDymMagicRenderQueueListener
//
//---------------------------------------------------------------------------------------------
void CDymMagicRenderQueueListener::renderQueueStarted(RenderQueueGroupID id, bool& skipThisQueue)
{
	skipThisQueue = false ;
	if(id == RENDER_QUEUE_8 )//RENDER_QUEUE_8 )
	{
		skipThisQueue = true;
		// render
		//g_pMagicRenderSystem->DrawLine2D( D3DXVECTOR2( 0,0 ) , D3DXVECTOR2( 100,100 ) , 0xFFFFFFFF );
		g_dymMagicOGREManager.BeginRender ();
	}
}
void CDymMagicRenderQueueListener::renderQueueEnded(RenderQueueGroupID id, bool& repeatThisQueue)
{
}

//---------------------------------------------------------------------------------------------
//
// CDymMagicFrameListener
//
//---------------------------------------------------------------------------------------------
bool CDymMagicFrameListener::frameStarted(const FrameEvent& evt)
{
	g_dymMagicOGREManager.FrameMove( evt.timeSinceLastFrame*1000 );
	return true;
}

//---------------------------------------------------------------------------------------------
//
// CDymMagicOGRE
//
//---------------------------------------------------------------------------------------------
CDymMagicOGRE::CDymMagicOGRE(const std::string& objName,const std::string& fileName):ISObject( objName )
{
	m_nRender = 0;
	m_pMagic = NULL ;
	m_fAlpha = 1.0f;
	DataStreamPtr stream = ResourceGroupManager::getSingleton().openResource(fileName, "MagicGroup");
	if( !stream.isNull () )
	{
		m_pMagic = new CDymMagic( g_pMagicRenderSystem , g_pMagicSoundSystem );
		m_pMagic->LoadFromMemory ( stream->GetBuffer () , stream->size () );
		m_pMagic->Play();
	}
	m_fCreateTime = g_dymMagicOGREManager.GetElapsedTime ();
	m_fCreateTimeFirst = m_fCreateTime;
}
CDymMagicOGRE::~CDymMagicOGRE()
{
	g_dymMagicOGREManager.DelMagic ( this );
	if(m_pMagic)delete m_pMagic;
}
void CDymMagicOGRE::addRenderQueue ( RenderQueue *queue )
{
	if( m_pMagic )
	{
		g_dymMagicOGREManager.AddMagic ( this );
		m_nRender = 1;
	}
}
const sAABox& CDymMagicOGRE::getLocalAABB()
{
	static sAABox boundingBox;
	boundingBox.min = Vector3( 0 , 0 , 0 );
	boundingBox.max = Vector3( 0 , 0 , 0 );
	if( m_pMagic )
	{
		D3DXVECTOR3 v1 , v2;
		m_pMagic->GetBoundingBoxRealTime( &v1 , &v2 );
		boundingBox.min = Vector3( v1.x , v1.y , v1.z );
		boundingBox.max = Vector3( v2.x , v2.y , v2.z );
	}
	return boundingBox;
}
void CDymMagicOGRE::Render( DWORD flag )
{
	if( m_pMagic )
	{
		m_pMagic->Render ( flag );
	}
}
void CDymMagicOGRE::FrameMove( float fElapsedTime )
{
	if( m_pMagic )
	{
		m_pMagic->SetPosition ( D3DXVECTOR3( -(float)this->getWorldPosition().x , 
											(float)this->getWorldPosition().y , 
											(float)this->getWorldPosition().z ) );
		m_pMagic->SetScaling ( D3DXVECTOR3( (float)this->getWorldScale().x ,
											(float)this->getWorldScale().y ,
											(float)this->getWorldScale().z ) );
		Radian angle;
		Vector3 axis;
		this->getWorldRotation().ToAngleAxis ( angle , axis );
		m_pMagic->SetRotation ( angle.valueRadians ()  );
		//m_pMagic->SetRotation ( aaa+=0.01f );
		m_pMagic->SetRotationAxis ( D3DXVECTOR3( axis.x , -axis.y , -axis.z ) );
		//if( m_fCreateTime != 0.0f )
		{
			m_pMagic->FrameMove ( g_dymMagicOGREManager.GetElapsedTime ()-m_fCreateTime );
			m_fCreateTime = g_dymMagicOGREManager.GetElapsedTime ();
		}
		//m_pMagic->FrameMove ( fElapsedTime );
		//if( fElapsedTime-m_fCreateTime<0 )
		//	rand();
	}
	
}
void CDymMagicOGRE::notifyCameraLookYou(Camera* camera)
{
	g_dymMagicOGREManager.SetCamera( camera );
}
bool CDymMagicOGRE::isValid ()
{
	if( m_pMagic == NULL )return false;
	if( m_pMagic->isPlaying ( g_dymMagicOGREManager.GetElapsedTime ()-m_fCreateTime ) )
		return true;
	return false;
}
void CDymMagicOGRE::queryInterface( void** pPtr,const IGUID& rGuid )
{
	if ( rGuid == IID_IDymMagicInterface )
		*pPtr = static_cast<IDymMagicInterface*>( this );
	else
		*pPtr = NULL;
	//else
	//	tParent::queryInterface( pPtr,rGuid );
}
void CDymMagicOGRE::SetDefaultShadowPos( float directionX , float directionY , float directionZ ,
										float centerX , float centerY , float centerZ )
{
	if( m_pMagic )
		m_pMagic->SetDefaultShadowPos ( D3DXVECTOR3(  -directionX , directionY, directionZ ) , 
								D3DXVECTOR3( -centerX , centerY , centerZ ) );
}
void CDymMagicOGRE::SetDefaultBillboardString( LPCTSTR str , DWORD color , void *pUnknow )
{
	if( m_pMagic )
		m_pMagic->SetDefaultBillboardString ( str , color , pUnknow );
}
void CDymMagicOGRE::SetAllKeyTexture( LPCTSTR texFileName )
{
	if( m_pMagic )
		m_pMagic->SetAllKeyTexture ( texFileName );
}
void CDymMagicOGRE::SetLoop( BOOL bLoop )
{
	if( m_pMagic )
		m_pMagic->SetLoop ( bLoop );
}
BOOL CDymMagicOGRE::GetLoop()
{
	if( m_pMagic )
		return m_pMagic->GetLoop ();
	return FALSE;
}
int CDymMagicOGRE::GetMagicExData ()
{
	if( m_pMagic == NULL )return 0;
	return m_pMagic->m_nExData;
}
void CDymMagicOGRE::SetFastRender( BOOL bFast , BOOL bAll )
{
	if( !bAll )
	{
		if( m_pMagic )
			m_pMagic->SetFastRender( bFast );
	}
	else
		g_dymMagicOGREManager.SetFastRender( bFast );
}
void CDymMagicOGRE::setAlpha(float alpha)
{
	m_fAlpha = alpha;
	if( m_pMagic )
		m_pMagic->SetMagicColor ( 1 , 1 , 1 , m_fAlpha );
}
float CDymMagicOGRE::getAlpha()
{
	return m_fAlpha;
}
void CDymMagicOGRE::CreateShadow( float followspeed , int facenum )
{
	if( m_pMagic == NULL && facenum>0 )	
	{
		m_pMagic = new CDymMagic( g_pMagicRenderSystem , g_pMagicSoundSystem );
		int id = m_pMagic->CreateModel ( CREATE_SHADOW , NULL );
		CDymMagicBaseObj *pObj = m_pMagic->GetObj ( id );
		CDymMagicKeyFrame *pKey = new CDymMagicKeyFrame();
		CDymMagicModel *pModel = (CDymMagicModel*)pObj;
		pModel->flyspeed = followspeed;
		pModel->facenum = facenum;
		pKey->nVersion = 2;
		pObj->m_KeyFrameManager.AddKeyFrame ( pKey );
		pObj->materialtype = CDymMagicBaseObj::MATERIAL_TYPE ::MT_Effect_sceneaddalpha;
		pModel->init();
		m_pMagic->Play();
		//m_pMagic->SetLoop ( TRUE );
	}
}
void CDymMagicOGRE::SetMaxTime( float time , BOOL bReplay )
{
	if( m_pMagic )
	{
		m_pMagic->SetKeyMaxTime ( time );
		if( bReplay )m_pMagic->SetCurTime ( 0.0f );
	}
}
void CDymMagicOGRE::SetPlayTime( float time )
{
	if( m_pMagic )
		m_pMagic->SetCurTime ( time );
}
//---------------------------------------------------------------------------------------------
//
// CDymMagicFactoryOGRE
//
//---------------------------------------------------------------------------------------------
ISObject*  CDymMagicFactoryOGRE::createObject(const std::string& objName,const std::string& fileName)
{
	CDymMagicOGRE * p = new CDymMagicOGRE(objName,fileName);
	return p;
}
ISObject*  CDymMagicFactoryOGRE::createObject(const std::string& objName)
{
	CDymMagicOGRE * p = new CDymMagicOGRE(objName,"");
	return p;
}
void CDymMagicFactoryOGRE::destroyObject(ISObject* obj)
{
	CDymMagicOGRE *p = (CDymMagicOGRE*)obj;
	delete p;
}

//---------------------------------------------------------------------------------------------
//
// CDymMagicOGREManager
//
//---------------------------------------------------------------------------------------------
CDymMagicOGREManager::CDymMagicOGREManager()
{
	m_fElapsedTime = 0.0f;
	m_fElapsedTime2 = 0.0f;
	m_bFastRender = FALSE;
}
void CDymMagicOGREManager::SetFastRender( BOOL bFast )
{
	m_bFastRender = bFast;
}
void CDymMagicOGREManager::SetCamera( Camera *pCamera )
{
	m_pMagicCamera = pCamera;
}
void CDymMagicOGREManager::AddMagic( CDymMagicOGRE *pMagic )
{
	m_listMagic.push_back ( pMagic );
}
void CDymMagicOGREManager::DelMagic( CDymMagicOGRE *pMagic )
{
	m_listMagic.remove( pMagic );
}
void CDymMagicOGREManager::FrameMove( float fElapsedTime )
{
	m_fElapsedTime += fElapsedTime;
}
D3DXMATRIX CDymMagicOGREManager::makeD3DXMatrix( const Matrix4& mat )
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
float CDymMagicOGREManager::GetElapsedTime()
{
	return m_fElapsedTime;
}
void CDymMagicOGREManager::BeginRender()
{
	if( m_pMagicCamera == NULL )return;
	g_pMagicSoundSystem->FrameMove ( m_pMagicCamera->getPosition ().x , m_pMagicCamera->getPosition ().y ,
		m_pMagicCamera->getPosition ().z , m_pMagicCamera->getDirection ().x + m_pMagicCamera->getPosition ().x,
		m_pMagicCamera->getDirection ().y + m_pMagicCamera->getPosition ().y,
		m_pMagicCamera->getDirection ().z + m_pMagicCamera->getPosition ().z );

	for( std::list< CDymMagicOGRE* >::iterator it = m_listMagic.begin() ; it != m_listMagic.end() ; it++ )
	{
		CDymMagicOGRE *pMagic = *it;
		pMagic->FrameMove( m_fElapsedTime-m_fElapsedTime2 );
	}
	m_fElapsedTime2 = m_fElapsedTime;

	// 备份视矩阵和投影矩阵
	D3DXMATRIX matViewOld , matProjOld;
	g_pMagicRenderSystem->m_pd3dDevice ->GetTransform( D3DTS_VIEW, &matViewOld );
	g_pMagicRenderSystem->m_pd3dDevice ->GetTransform( D3DTS_PROJECTION, &matProjOld );
	DWORD dwFog;
	g_pMagicRenderSystem->m_pd3dDevice->GetRenderState ( D3DRS_FOGENABLE , &dwFog );
	g_pMagicRenderSystem->m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE , FALSE );
	// 设置视矩阵和投影矩阵
	Vector3 vEye = m_pMagicCamera->getPosition ();
	D3DXVECTOR3 vEyePt( -vEye.x , vEye.y ,vEye.z );
	Vector3 vLookat = m_pMagicCamera->getDirection ();
	
	D3DXVECTOR3 vLookatPt( -vLookat.x , vLookat.y , vLookat.z );
	vLookatPt += vEyePt;

	Vector3 vUp = m_pMagicCamera->getUp ();
	D3DXVECTOR3 vUpVec( -vUp.x , vUp.y , vUp.z );


	/*static float aaa=0.0f;

	float savedy = vEyePt.y;
	D3DXMATRIX rottmp;
	D3DXVECTOR3 vLookat2 = vEyePt+vLookatPt;
	D3DXVECTOR3 vn1 , vn2 , vn3;
	vn3 = vLookatPt-vEyePt; 
	D3DXVec3Normalize( &vn1 , &D3DXVECTOR3( -vn3.x , 0 , vn3.z ) );
	vn2 = D3DXVECTOR3( 0 , 0 , 1 );
	float a = asinf( D3DXVec3Dot( &vn1 , &vn2 ) );
	if( -vn3.x < 0 )a=-a;
	D3DXMatrixRotationY(&rottmp   , a*1.0f+D3DX_PI );
	D3DXVECTOR4 v4;
	D3DXVec3Transform( &v4 , &vEyePt , &rottmp );
	vEyePt = D3DXVECTOR3( v4.x/v4.w , v4.y/v4.w , v4.z/v4.w );
	//vEyePt.y = savedy;

	savedy = vLookatPt.y ;
	D3DXVec3Transform( &v4 , &vLookatPt , &rottmp );
	vLookatPt = D3DXVECTOR3( v4.x/v4.w , v4.y/v4.w , v4.z/v4.w );
	//vLookatPt.y = savedy;
	D3DXVec3Transform( &v4 , &vUpVec , &rottmp );
	vUpVec = D3DXVECTOR3( v4.x/v4.w , v4.y/v4.w , v4.z/v4.w );*/
	
	//D3DXVec3Cross( &vLookatPt , &vLookatPt , &D3DXVECTOR3(0,1,0) );
	

	//swap( vEyePt.x , vEyePt.z );
	//swap( vLookatPt.x , vLookatPt.z );
	//swap( vUpVec.x , vUpVec.z );

	D3DXMATRIXA16 matView;
	D3DXMATRIXA16 matProj;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	//matView._44 *=-1;
	
	D3DXMatrixPerspectiveFovLH( &matProj, (float)m_pMagicCamera->getFOVy ().valueRadians(),
		(float)m_pMagicCamera->getAspectRatio(), 
		(float)m_pMagicCamera->getNearClipDistance (), 
		(float)m_pMagicCamera->getFarClipDistance () );

	//if( g_pdymSceneManager->needUpdataViewProMatrices () )
	{
		{
			Matrix4 matrix4 = m_pMagicCamera->getViewMatrix();
			matrix4[2][0] = -matrix4[2][0];
			matrix4[2][1] = -matrix4[2][1];
			matrix4[2][2] = -matrix4[2][2];
			matrix4[2][3] = -matrix4[2][3];
			D3DXMATRIX d3dmat = makeD3DXMatrix( matrix4 );
			//d3dmat._43 *= -1.0f;
			g_pMagicRenderSystem->m_pd3dDevice ->SetTransform( D3DTS_VIEW, &matView );
		}

		{
			Matrix4 matrix4 = m_pMagicCamera->getProjectionMatrix ();
			D3DXMATRIX d3dmat = makeD3DXMatrix( matrix4 );
			//d3dmat._43 *= -1.0f;
			g_pMagicRenderSystem->m_pd3dDevice ->SetTransform( D3DTS_PROJECTION, &matProj );
		}
	}
	// 计算公告牌矩阵
	CDymMagicBillboard::ComputeBillboardMatrix ( vEyePt , vLookatPt , vUpVec );

	g_pMagicRenderSystem->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	g_pMagicRenderSystem->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	g_pMagicRenderSystem->m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE , TRUE );
	g_pMagicRenderSystem->m_pd3dDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	g_pMagicRenderSystem->m_pd3dDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);

	// Turn off ambient lighting 
	DWORD dwAmbient;
	DWORD lighting;
	g_pMagicRenderSystem->m_pd3dDevice->GetRenderState( D3DRS_AMBIENT , &dwAmbient );
	g_pMagicRenderSystem->m_pd3dDevice->GetRenderState( D3DRS_LIGHTING , &lighting );
	g_pMagicRenderSystem->m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );
	g_pMagicRenderSystem->m_pd3dDevice->SetRenderState ( D3DRS_LIGHTING , 0 );
	g_pMagicRenderSystem->m_pd3dDevice->SetVertexShader(0);
	g_pMagicRenderSystem->m_pd3dDevice->SetPixelShader(0);

	DWORD dwFlag = 0;
	if( m_bFastRender ) dwFlag &= MAGIC_RENDER_FAST;
	for( std::list< CDymMagicOGRE* >::iterator it = m_listMagic.begin() ; it != m_listMagic.end() ; it++ )
	{
		CDymMagicOGRE *pMagic = *it;
		if( pMagic->m_nRender == 1 )
		{
			pMagic->Render( 0 | dwFlag );
			//pMagic->m_nRender = 0;
		}
	}
	for( std::list< CDymMagicOGRE* >::iterator it = m_listMagic.begin() ; it != m_listMagic.end() ; it++ )
	{
		CDymMagicOGRE *pMagic = *it;
		if( pMagic->m_nRender == 1 )
		{
			pMagic->Render( MAGIC_RENDER_SHADOW | dwFlag );
			pMagic->m_nRender = 0;
		}
	}
	m_listMagic.clear();
	// 恢复视矩阵和投影矩阵
	g_pMagicRenderSystem->m_pd3dDevice ->SetTransform( D3DTS_VIEW, &matViewOld );
	g_pMagicRenderSystem->m_pd3dDevice ->SetTransform( D3DTS_PROJECTION, &matProjOld );
	m_pMagicCamera = NULL;
	g_pMagicRenderSystem->m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE , dwFog );

	//还原OGRE默认值
	g_pMagicRenderSystem->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
	g_pMagicRenderSystem->m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE,   TRUE);
	g_pMagicRenderSystem->m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, dwAmbient );
	g_pMagicRenderSystem->m_pd3dDevice->SetRenderState ( D3DRS_LIGHTING , lighting );
	g_pMagicRenderSystem->m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_DISABLE );
}
//---------------------------------------------------------------------------------------------
//
// CDymMagicSoundSystemForOGRE
//
//---------------------------------------------------------------------------------------------
HRESULT CDymMagicSoundSystemForOGRE::CreateSoundFromFile( LPTSTR filename , CDymSoundObj *pObj )
{
	DataStreamPtr stream = ResourceGroupManager::getSingleton().openResource(filename, "MagicGroup");
	if( stream.isNull () )return NULL;
	if( stream->size() <=0 )return NULL;
	return pObj->CreateFromMemory ( m_pSoundBuffer , m_pDsd ,  stream->GetBuffer () , stream->size() , filename );
}
//---------------------------------------------------------------------------------------------
//
// CDymMagicRenderSystemForOGRE
//
//---------------------------------------------------------------------------------------------
HRESULT CDymMagicRenderSystemForOGRE::CreateTextureFromFile( LPCTSTR pSrcFile,CDymD3DTexture ** ppTexture)
{
	if( pSrcFile[0] == 0 )return E_FAIL;

	*ppTexture = (CDymD3DTexture*)m_dataBuffer.LoadBuffer ( pSrcFile , MRT_TEXTURE , NULL );
	if( *ppTexture )return S_OK;

	LPDIRECT3DTEXTURE9 pTex;
	DataStreamPtr stream = ResourceGroupManager::getSingleton().openResource(pSrcFile, "MagicGroup");
	if( stream.isNull () )return E_FAIL;
	if( stream->size() <=0 )return E_FAIL;
	HRESULT hr = D3DXCreateTextureFromFileInMemory( m_pd3dDevice , stream->GetBuffer () , stream->size () , &pTex );
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
void *CDymMagicRenderSystemForOGRE::LoadFromFile( LPCTSTR fullpathfilename , int *pSizeOut )
{
	DataStreamPtr stream = ResourceGroupManager::getSingleton().openResource(fullpathfilename, "MagicGroup");
	if( stream.isNull () )return NULL;
	if( stream->size() <=0 )return NULL;
	*pSizeOut = stream->size();
	char *pBuffer = new char[stream->size()+1];
	memcpy( pBuffer , stream->GetBuffer () , stream->size() );
	return pBuffer;
}
HRESULT CDymMagicRenderSystemForOGRE::SetTransform( D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX * pMatrix )
{
	return CDymRenderSystem::SetTransform ( State , pMatrix );
	/*D3DXMATRIX mat = *pMatrix;
	if( State == D3DTS_WORLD )
	{
		this->m_worldSaved = *pMatrix;
		D3DXMATRIX rot;
		D3DXMatrixRotationY( &rot , D3DX_PI );
		mat = mat * rot;
		mat._43 = -mat._43;
	}
	return m_pd3dDevice->SetTransform ( State , &mat );*/
}
HRESULT CDymMagicRenderSystemForOGRE::GetTransform( D3DTRANSFORMSTATETYPE State,D3DMATRIX * pMatrix)
{
	/*if( State == D3DTS_WORLD )
	{
		*pMatrix = m_worldSaved;
		return S_OK;
	}*/
	return CDymRenderSystem::GetTransform ( State , pMatrix );
}
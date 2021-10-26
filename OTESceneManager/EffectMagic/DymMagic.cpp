/***************************************************************************************************
文件名称:	DymMagic.cpp
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	特效主模块
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#include ".\dymmagic.h"
CDymFontFactory *g_pMagicFontFactory = NULL;
int g_nMagicFactoryCreateTimes = 0;

CDymMagic::CDymMagic( CDymRenderSystem *pDevice , CDymSoundManager *pSoundManager )
{
	isloop = false ;
	issametime = false ;
	m_pDevice = pDevice;
	m_pSoundManager = pSoundManager;
	m_bDisableRotation = FALSE;
	Clear();
	if( g_pMagicFontFactory == NULL )
		g_pMagicFontFactory = new CDymFontFactory( m_pDevice );
	g_nMagicFactoryCreateTimes ++;
	m_vGlobleScaling = D3DXVECTOR3( 1.0f , 1.0f , 1.0f );
	m_nExData = 0xFF;
	m_bFastRender = FALSE;
	m_fLastFrameTime = 0.0f;
	m_bFirstPlay = TRUE;
}

CDymMagic::~CDymMagic(void)
{
	Clear();
	for( std::list< SOUND_OBJ* >::iterator it = m_listSoundObj.begin () ; it != m_listSoundObj.end () ; it ++ )
	{
		SOUND_OBJ *pObj = *it;
		delete pObj;
	}
	g_nMagicFactoryCreateTimes--;
	if( g_nMagicFactoryCreateTimes <=0 && g_pMagicFontFactory )
	{
		delete g_pMagicFontFactory;
		g_pMagicFontFactory = NULL;
		g_nMagicFactoryCreateTimes = 0;
	}
}

BOOL CDymMagic::LoadFromFile(LPCTSTR fileName)
{
	Clear();
	CDymScriptLoader loader;
	if(!loader.LoadFromFile ( fileName ))return FALSE;
	return LoadFromLoader( &loader );
}

BOOL CDymMagic::Import( CDymMagic *pMagic )
{
	int num = pMagic->GetObjNum ();
	int *idbuffer = new int[num];
	pMagic->GetAllObj ( idbuffer , num );
	BOOL bReturn = TRUE;
	for(int i=0;i<num;i++)
	{
		//if( idbuffer[i]>0 )
		{
			int idr = 0;
			idr = this->CopyCreate ( pMagic->GetObj ( idbuffer[i] ) , NULL );
			if( idr < 0 )bReturn = FALSE;
			if( idbuffer[i]==0 )this->ChangeID ( idr , 0 );
		}
	}
	delete[] idbuffer;
	return bReturn;
}

BOOL CDymMagic::LoadFromMemory( void *pData , int datalen )
{
	Clear();
	CDymScriptLoader loader;
	if(!loader.LoadFromMemory ( pData , datalen ))return FALSE;
	return LoadFromLoader( &loader );
}
void CDymMagic::GetBoundingBox( D3DXVECTOR3 *pMin , D3DXVECTOR3 *pMax )
{
	*pMin = m_vBoundingBox1;
	*pMax = m_vBoundingBox2;
}
void CDymMagic::SetMagicColor( float r , float g , float b , float a )
{
	m_world.vColor = D3DXVECTOR4( r , g , b , a );
}
void CDymMagic::GetBoundingBoxRealTime( D3DXVECTOR3 *pMin , D3DXVECTOR3 *pMax )
{
	D3DXVECTOR3 v1 = m_vBoundingBox1;
	D3DXVECTOR3 v2 = m_vBoundingBox2;
	for( std::list< CDymMagicModel* >::iterator it = m_listModel.begin () ; it != m_listModel.end () ; it++ )
	{
		D3DXVECTOR3 vp1;
		D3DXVECTOR3 vp2;
		CDymMagicModel * p = *it;
		if( p->m_nID == 0 )continue;
		p->GetBoundingBoxRealTime (&vp1,&vp2);

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
	}
	for( std::list< CDymMagicParticles* >::iterator it = m_listParticles.begin () ; it != m_listParticles.end () ; it++ )
	{
		D3DXVECTOR3 vp1;
		D3DXVECTOR3 vp2;
		CDymMagicParticles * p = *it;
		p->GetBoundingBoxRealTime (&vp1,&vp2);

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
	}
	*pMin = v1;
	*pMax = v2;
}
BOOL CDymMagic::LoadFromLoader(CDymScriptLoader* pLoader)
{
	Clear();
	char *strKey = NULL;
	BOOL bLineEnd;
	m_vBoundingBox1 = D3DXVECTOR3( -10000 , -10000 , -10000 );
	m_vBoundingBox2 = D3DXVECTOR3( -10000 , -10000 , -10000 );
	while( strKey = pLoader->GetKeyString ( &bLineEnd ) )
	{
		if( stricmp( strKey , "isloop" ) == 0 && !bLineEnd )
			isloop = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "issametime" ) == 0 && !bLineEnd )
			issametime = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "magic_version" ) == 0 && !bLineEnd )
			m_nVersion = pLoader->GetKeyInt ( NULL );
		else if( stricmp( strKey , "maxtime" ) == 0 && !bLineEnd )
			m_fKeyMaxTime = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "bounding_box1" ) == 0 && !bLineEnd )
			m_vBoundingBox1 = pLoader->GetKeyVector3 ( NULL );
		else if( stricmp( strKey , "bounding_box2" ) == 0 && !bLineEnd )
			m_vBoundingBox2 = pLoader->GetKeyVector3 ( NULL );
		else if( stricmp( strKey , "disable_rotation" ) == 0 && !bLineEnd )
			m_bDisableRotation = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "globle_scaling" ) == 0 && !bLineEnd )
			//m_vGlobleScaling = pLoader->GetKeyVector3 ( NULL );
			SetGlobleScaling( pLoader->GetKeyVector3 ( NULL ) );
		else if( stricmp( strKey , "ex_data" ) == 0 && !bLineEnd )
			m_nExData = pLoader->GetKeyInt ( NULL );
		else if( stricmp( strKey , "grass_method" ) == 0 && !bLineEnd )
			m_world.bGrassMethod = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "model" ) == 0 )
		{
			CDymMagicModel *p = new CDymMagicModel(m_pDevice);
			if( !bLineEnd )p->m_nID = pLoader->GetKeyInt ( NULL );
			if( strcmp( pLoader->GetKeyString ( NULL ) , "{" ) == 0 )
			{
				LoadModel( p , pLoader );
				if( p->IsAlphaRender() )
					m_listModel.push_back ( p );
				else
					m_listModel.push_front ( p );
			}
			else delete p;
		}
		else if( stricmp( strKey , "particles" ) == 0 || stricmp( strKey , "model" ) == 0 )
		{
			CDymMagicParticles *p = new CDymMagicParticles(m_pDevice);
			if( !bLineEnd )p->m_nID = pLoader->GetKeyInt ( NULL );
			if( strcmp( pLoader->GetKeyString ( NULL ) , "{" ) == 0 )
			{
				LoadParticles( p , pLoader );
				m_listParticles.push_back ( p );
			}
			else delete p;
		}
	}
	ComputeRotationID();
	if( m_nVersion == 1 )
		ComputeKeyMaxTime();
	else
		SetKeyMaxTime( m_fKeyMaxTime );
	if( m_vBoundingBox1.x == -10000 || m_vBoundingBox2.x == -10000 )
		ComputeBoundingBox ();
	ComputeSound();
	return TRUE;
}

void CDymMagic::ComputeRotationID()
{
	for( std::list< CDymMagicParticles* >::iterator it = m_listParticles.begin () ; it != m_listParticles.end () ; it++ )
	{
		CDymMagicParticles * p = *it;
		if( p->m_nID_follow_with == 0 )
			p->m_nID_follow_with = p->m_nID ;
		if( p->m_nID_rotation_with == 0 )
			p->m_nID_rotation_with = p->m_nID ;
		p->SetRotationWithID ( GetObj( p->m_nID_rotation_with ) );
		p->SetFollowWithID ( GetObj( p->m_nID_follow_with ) );
		p->SetParticlesAcceptID ( GetObj( p->accept_id ) );
	}
	for( std::list< CDymMagicModel* >::iterator it = m_listModel.begin () ; it != m_listModel.end () ; it++ )
	{
		CDymMagicModel * p = *it;
		//if( p->m_nID == 0 )continue;
		if( p->m_nID_follow_with == 0 )
			p->m_nID_follow_with = p->m_nID ;
		if( p->m_nID_rotation_with == 0 )
			p->m_nID_rotation_with = p->m_nID ;
		p->SetRotationWithID ( GetObj( p->m_nID_rotation_with ) );
		p->SetFollowWithID ( GetObj( p->m_nID_follow_with ) );
	}
}
BOOL CDymMagic::LoadModel(CDymMagicModel *pModel,CDymScriptLoader* pLoader)
{
	char *strKey = NULL;
	BOOL bLineEnd;
	CDymMagicKeyFrame *pOldKey = NULL;
	while( ( strKey = pLoader->GetKeyString ( &bLineEnd ) )!=NULL && strcmp( strKey , "}" )!=0 )
	{
		if( stricmp( strKey , "typename" ) == 0 && !bLineEnd )
			//strcpy( pParticles->m_strName , pLoader->GetKeyString ( NULL ) );
			pModel->SetParticlesName ( pLoader->GetKeyString ( NULL ) );
		else if( stricmp( strKey , "strobjname" ) == 0 && !bLineEnd )
			_tcscpy( pModel->m_strObjName , pLoader->GetKeyString ( NULL ) );
		else if( stricmp( strKey , "rotation_id" ) == 0 && !bLineEnd )
			pModel->m_nID_rotation_with = pLoader->GetKeyInt ( NULL );
		else if( stricmp( strKey , "follow_id" ) == 0 && !bLineEnd )
			pModel->m_nID_follow_with = pLoader->GetKeyInt ( NULL );
		else if( stricmp( strKey , "meshfilename" ) == 0 && !bLineEnd )
			strcpy( pModel->m_strMeshName , pLoader->GetKeyString ( NULL ) );
		else if( stricmp( strKey , "mattypename" ) == 0 && !bLineEnd )
			pModel->SetMaterialType ( pLoader->GetKeyString ( NULL ) );
		else if( stricmp( strKey , "matresname" ) == 0 && !bLineEnd )
			strcpy( pModel->materialres , pLoader->GetKeyString ( NULL ) );
		else if( stricmp( strKey , "initsize" ) == 0 && !bLineEnd )
			pModel->initsize = pLoader->GetKeyVector3 ( NULL );
		else if( stricmp( strKey , "isinfacex" ) == 0 && !bLineEnd )
			pModel->isinfacex = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "isinfacey" ) == 0 && !bLineEnd )
			pModel->isinfacey = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "isinfacez" ) == 0 && !bLineEnd )
			pModel->isinfacez = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "meshbillboardtype" ) == 0 && !bLineEnd )
			pModel->m_nMeshBillBoardType = pLoader->GetKeyInt ( NULL );
		else if( stricmp( strKey , "facenum" ) == 0 && !bLineEnd )
			pModel->facenum = pLoader->GetKeyInt ( NULL );
		else if( stricmp( strKey , "flyspeed" ) == 0 && !bLineEnd )
			pModel->flyspeed = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "isskybox" ) == 0 && !bLineEnd )
			pModel->isskybox = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "ishavort" ) == 0 && !bLineEnd )
			pModel->ishavort = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "bounding_box1" ) == 0 && !bLineEnd )
			pModel->m_vBoundingBox1 = pLoader->GetKeyVector3 ( NULL );
		else if( stricmp( strKey , "bounding_box2" ) == 0 && !bLineEnd )
			pModel->m_vBoundingBox2 = pLoader->GetKeyVector3 ( NULL );
		else if( stricmp( strKey , "bUsingMeshVertexColor" ) == 0 && !bLineEnd )
			pModel->bUsingMeshVertexColor = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "keyframe" ) == 0 )
		{
			CDymMagicKeyFrame *p = new CDymMagicKeyFrame();
			if( pOldKey )*p = *pOldKey;
			if( !bLineEnd )p->m_fTime = pLoader->GetKeyFloat ( NULL );
			if( strcmp( pLoader->GetKeyString ( NULL ) , "{" ) == 0 )
			{
				LoadKeyFrame( p , pLoader );
				p->SetMatresName ( p->matresname , this->m_pDevice );
				pModel->m_KeyFrameManager.AddKeyFrame( p );
			}
			pOldKey = p;
		}
	}
	pModel->init ();
	return TRUE;
}

BOOL CDymMagic::LoadParticles2(CDymMagicParticles *pParticles,CDymScriptLoader* pLoader)
{
	/*char *strKey = NULL;
	BOOL bLineEnd;
	while( ( strKey = pLoader->GetKeyString ( &bLineEnd ) )!=NULL )
	{
		if( stricmp( strKey , "materialtype" ) == 0 && !bLineEnd )
			pParticles->SetMaterialType ( pLoader->GetKeyString ( NULL ) );
		else if( stricmp( strKey , "billboard_type" ) == 0 && !bLineEnd )
			pParticles->billboard_type = pLoader->GetKeyInt ( NULL );
		else if( stricmp( strKey , "materialres" ) == 0 && !bLineEnd )
			strcpy( pParticles->materialres , pLoader->GetKeyString ( NULL ) );
		else if( stricmp( strKey , "quota" ) == 0 && !bLineEnd )
			pParticles->quota = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "particle_width" ) == 0 && !bLineEnd )
			pParticles->particle_width2 = pParticles->particle_width = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "particle_height" ) == 0 && !bLineEnd )
			pParticles->particle_height2 = pParticles->particle_height = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "particle_width2" ) == 0 && !bLineEnd )
			pParticles->particle_width2 = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "particle_height2" ) == 0 && !bLineEnd )
			pParticles->particle_height2 = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "cull_each" ) == 0 && !bLineEnd )
			pParticles->cull_each = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "ishav_qua" ) == 0 && !bLineEnd )
			pParticles->ishav_qua = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "force_vector" ) == 0 && !bLineEnd )
			pParticles->force_vector = pLoader->GetKeyVector3 ( NULL );
		else if( stricmp( strKey , "width" ) == 0 && !bLineEnd )
			pParticles->ellipsoid_size .x = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "height" ) == 0 && !bLineEnd )
			pParticles->ellipsoid_size .y = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "depth" ) == 0 && !bLineEnd )
			pParticles->ellipsoid_size .z = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "time_to_live_min" ) == 0 && !bLineEnd )
			pParticles->time_to_live_min = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "time_to_live_max" ) == 0 && !bLineEnd )
			pParticles->time_to_live_max = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "colour_range_start" ) == 0 && !bLineEnd )
			pParticles->colour_range_start = pLoader->GetKeyVector4 ( NULL );
		else if( stricmp( strKey , "colour_range_end" ) == 0 && !bLineEnd )
			pParticles->colour_range_end = pLoader->GetKeyVector4 ( NULL );
		else if( stricmp( strKey , "velocity_min" ) == 0 && !bLineEnd )
			pParticles->velocity_min = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "velocity_max" ) == 0 && !bLineEnd )
			pParticles->velocity_max = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "direction" ) == 0 && !bLineEnd )
			pParticles->direction = pLoader->GetKeyVector3 ( NULL );
		else if( stricmp( strKey , "angle" ) == 0 && !bLineEnd )
			pParticles->angle = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "israndangle" ) == 0 && !bLineEnd )
			pParticles->israndangle = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "isonlydirection" ) == 0 && !bLineEnd )
			pParticles->isonlydirection = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "emission_rate" ) == 0 && !bLineEnd )
			pParticles->emission_rate = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "red" ) == 0 && !bLineEnd )
			pParticles->ColourFader.x = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "green" ) == 0 && !bLineEnd )
			pParticles->ColourFader.y = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "blue" ) == 0 && !bLineEnd )
			pParticles->ColourFader.z = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "alpha" ) == 0 && !bLineEnd )
			pParticles->ColourFader.w = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "ColourFader" ) == 0 && !bLineEnd )
			pParticles->ColourFader = pLoader->GetKeyVector4 ( NULL );
		else if( stricmp( strKey , "scale_add" ) == 0 && !bLineEnd )
			pParticles->scale_add_x = pParticles->scale_add_y = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "scale_add_x" ) == 0 && !bLineEnd )
			pParticles->scale_add_x = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "scale_add_y" ) == 0 && !bLineEnd )
			pParticles->scale_add_y = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "rotate_add" ) == 0 && !bLineEnd )
			pParticles->rotate_add = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "bounding_box1" ) == 0 && !bLineEnd )
			pParticles->m_vBoundingBox1 = pLoader->GetKeyVector3 ( NULL );
		else if( stricmp( strKey , "bounding_box2" ) == 0 && !bLineEnd )
			pParticles->m_vBoundingBox2 = pLoader->GetKeyVector3 ( NULL );
	}*/
	return TRUE;
}
BOOL CDymMagic::LoadParticles(CDymMagicParticles *pParticles,CDymScriptLoader* pLoader)
{
	char *strKey = NULL;
	BOOL bLineEnd;
	CDymMagicKeyFrame *pOldKey = NULL;
	while( ( strKey = pLoader->GetKeyString ( &bLineEnd ) )!=NULL && strcmp( strKey , "}" )!=0 )
	{
		if( stricmp( strKey , "particlename" ) == 0 && !bLineEnd )
		{
			//strcpy( pParticles->m_strName , pLoader->GetKeyString ( NULL ) );
			pParticles->SetParticlesName ( pLoader->GetKeyString ( NULL ) );
			CDymScriptLoader loader;
			if( loader.LoadFromFile ( pParticles->m_strName ) )
			{
				//LoadParticles2( pParticles , &loader );
			}
		}
		else if( stricmp( strKey , "strobjname" ) == 0 && !bLineEnd )
			_tcscpy( pParticles->m_strObjName , pLoader->GetKeyString ( NULL ) );
		else if( stricmp( strKey , "rotation_id" ) == 0 && !bLineEnd )
			pParticles->m_nID_rotation_with = pLoader->GetKeyInt ( NULL );
		else if( stricmp( strKey , "follow_id" ) == 0 && !bLineEnd )
			pParticles->m_nID_follow_with = pLoader->GetKeyInt ( NULL );
		else if( stricmp( strKey , "particleversion" ) == 0 && !bLineEnd )
			pParticles->m_nVersion = pLoader->GetKeyInt ( NULL );
		else if( stricmp( strKey , "particles_type" ) == 0 && !bLineEnd )
			pParticles->particles_type = pLoader->GetKeyInt ( NULL );
		else if( stricmp( strKey , "materialtype" ) == 0 && !bLineEnd )
			pParticles->SetMaterialType ( pLoader->GetKeyString ( NULL ) );
		else if( stricmp( strKey , "billboard_type" ) == 0 && !bLineEnd )
			//pParticles->SetBillboardType ( pLoader->GetKeyString ( NULL ) );
			pParticles->billboard_type = pLoader->GetKeyInt ( NULL );
		else if( stricmp( strKey , "materialres" ) == 0 && !bLineEnd )
			strcpy( pParticles->materialres , pLoader->GetKeyString ( NULL ) );
		else if( stricmp( strKey , "quota" ) == 0 && !bLineEnd )
			pParticles->quota = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "particle_width" ) == 0 && !bLineEnd )
			pParticles->particle_width2 = pParticles->particle_width = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "particle_height" ) == 0 && !bLineEnd )
			pParticles->particle_height2 = pParticles->particle_height = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "particle_width2" ) == 0 && !bLineEnd )
			pParticles->particle_width2 = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "particle_height2" ) == 0 && !bLineEnd )
			pParticles->particle_height2 = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "cull_each" ) == 0 && !bLineEnd )
			pParticles->cull_each = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "ishav_qua" ) == 0 && !bLineEnd )
			pParticles->ishav_qua = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "force_vector" ) == 0 && !bLineEnd )
			pParticles->force_vector = pLoader->GetKeyVector3 ( NULL );
		else if( stricmp( strKey , "width" ) == 0 && !bLineEnd )
			pParticles->ellipsoid_size .x = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "height" ) == 0 && !bLineEnd )
			pParticles->ellipsoid_size .y = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "depth" ) == 0 && !bLineEnd )
			pParticles->ellipsoid_size .z = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "time_to_live_min" ) == 0 && !bLineEnd )
			pParticles->time_to_live_min = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "time_to_live_max" ) == 0 && !bLineEnd )
			pParticles->time_to_live_max = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "colour_range_start" ) == 0 && !bLineEnd )
			pParticles->colour_range_start = pLoader->GetKeyVector4 ( NULL );
		else if( stricmp( strKey , "colour_range_end" ) == 0 && !bLineEnd )
			pParticles->colour_range_end = pLoader->GetKeyVector4 ( NULL );
		else if( stricmp( strKey , "velocity_min" ) == 0 && !bLineEnd )
			pParticles->velocity_min = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "velocity_max" ) == 0 && !bLineEnd )
			pParticles->velocity_max = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "direction" ) == 0 && !bLineEnd )
			pParticles->direction = pLoader->GetKeyVector3 ( NULL );
		else if( stricmp( strKey , "angle" ) == 0 && !bLineEnd )
			pParticles->angle = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "israndangle" ) == 0 && !bLineEnd )
			pParticles->israndangle = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "isonlydirection" ) == 0 && !bLineEnd )
			pParticles->isonlydirection = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "emission_rate" ) == 0 && !bLineEnd )
			pParticles->emission_rate = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "emission_time" ) == 0 && !bLineEnd )
			pParticles->emission_time = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "emission_num" ) == 0 && !bLineEnd )
			pParticles->emission_num = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "bUseRate" ) == 0 && !bLineEnd )
			pParticles->m_bUseRate = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "red" ) == 0 && !bLineEnd )
			pParticles->ColourFader.x = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "green" ) == 0 && !bLineEnd )
			pParticles->ColourFader.y = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "blue" ) == 0 && !bLineEnd )
			pParticles->ColourFader.z = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "alpha" ) == 0 && !bLineEnd )
			pParticles->ColourFader.w = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "ColourFader" ) == 0 && !bLineEnd )
			pParticles->ColourFader = pLoader->GetKeyVector4 ( NULL );
		else if( stricmp( strKey , "scale_add" ) == 0 && !bLineEnd )
			pParticles->scale_add_x = pParticles->scale_add_y = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "scale_add_x" ) == 0 && !bLineEnd )
			pParticles->scale_add_x = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "scale_add_y" ) == 0 && !bLineEnd )
			pParticles->scale_add_y = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "rotate_add" ) == 0 && !bLineEnd )
			pParticles->rotate_add = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "iswithfather" ) == 0 && !bLineEnd )
			pParticles->iswithfather = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "force_type" ) == 0 && !bLineEnd )
			pParticles->force_type = pLoader->GetKeyInt ( NULL );
		else if( stricmp( strKey , "accept_power" ) == 0 && !bLineEnd )
			pParticles->accept_power = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "force_add_type" ) == 0 && !bLineEnd )
			pParticles->force_add_type = pLoader->GetKeyInt ( NULL );
		else if( stricmp( strKey , "bounding_box1" ) == 0 && !bLineEnd )
			pParticles->m_vBoundingBox1 = pLoader->GetKeyVector3 ( NULL );
		else if( stricmp( strKey , "bounding_box2" ) == 0 && !bLineEnd )
			pParticles->m_vBoundingBox2 = pLoader->GetKeyVector3 ( NULL );
		else if( stricmp( strKey , "accept_id" ) == 0 && !bLineEnd )
			pParticles->accept_id = pLoader->GetKeyInt ( NULL );
		else if( stricmp( strKey , "limit_vector" ) == 0 && !bLineEnd )
			pParticles->limit_vector = pLoader->GetKeyInt ( NULL );
		else if( stricmp( strKey , "keyframe" ) == 0 )
		{
			CDymMagicKeyFrame *p = new CDymMagicKeyFrame();
			if( pOldKey )*p = *pOldKey;
			if( !bLineEnd )p->m_fTime = pLoader->GetKeyFloat ( NULL );
			if( strcmp( pLoader->GetKeyString ( NULL ) , "{" ) == 0 )
			{
				LoadKeyFrame( p , pLoader );
				p->SetMatresName ( p->matresname , this->m_pDevice );
				pParticles->m_KeyFrameManager.AddKeyFrame( p );
			}
			pOldKey = p;
		}
	}
	pParticles->init ();
	return TRUE;
}

BOOL CDymMagic::LoadKeyFrame(CDymMagicKeyFrame *p,CDymScriptLoader* pLoader)
{
	char *strKey = NULL;
	BOOL bLineEnd;
	while( ( strKey = pLoader->GetKeyString ( &bLineEnd ) )!=NULL && strcmp( strKey , "}" )!=0 )
	{
		if( stricmp( strKey , "position" ) == 0 && !bLineEnd )
			p->position = pLoader->GetKeyVector3 ( NULL );
		else if( stricmp( strKey , "animate" ) == 0 && !bLineEnd )
			p->animate = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "scale" ) == 0 && !bLineEnd )
			p->scale = pLoader->GetKeyVector3 ( NULL );
		else if( stricmp( strKey , "direction" ) == 0 && !bLineEnd )
			p->direction = pLoader->GetKeyVector3 ( NULL );
		else if( stricmp( strKey , "spinaxis" ) == 0 && !bLineEnd )
			p->spinaxis = pLoader->GetKeyVector3 ( NULL );
		else if( stricmp( strKey , "spinangle" ) == 0 && !bLineEnd )
			p->spinangle = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "spinradius" ) == 0 && !bLineEnd )
			p->spinradius = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "colour" ) == 0 && !bLineEnd )
			p->colour = pLoader->GetKeyVector4 ( NULL );
		else if( stricmp( strKey , "matresname" ) == 0 && !bLineEnd )
			strcpy( p->matresname , pLoader->GetKeyString ( NULL ) );
			//p->SetMatresName ( pLoader->GetKeyString ( NULL ) , m_pDevice );
		else if( stricmp( strKey , "matUVtile" ) == 0 && !bLineEnd )
			p->matUVtile = pLoader->GetKeyVector2 ( NULL );
		else if( stricmp( strKey , "scroll_anim" ) == 0 && !bLineEnd )
			p->scroll_anim = pLoader->GetKeyVector2 ( NULL );
		else if( stricmp( strKey , "rotate_anim" ) == 0 && !bLineEnd )
			p->rotate_anim = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "topscale" ) == 0 && !bLineEnd )
			p->topscale = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "bottomscale" ) == 0 && !bLineEnd )
			p->bottomscale = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "animname" ) == 0 && !bLineEnd )
			strcpy( p->animname , pLoader->GetKeyString ( NULL ) );
		else if( stricmp( strKey , "animspeed" ) == 0 && !bLineEnd )
			p->animspeed = pLoader->GetKeyFloat ( NULL );
		// for magic 2.0
		else if( stricmp( strKey , "key_version" ) == 0 && !bLineEnd )
			p->nVersion = pLoader->GetKeyInt ( NULL );
		else if( stricmp( strKey , "rotationV2" ) == 0 && !bLineEnd )
			p->rotation2 = pLoader->GetKeyVector4 ( NULL );
		else if( stricmp( strKey , "rotation3V2" ) == 0 && !bLineEnd )
			p->rotation3 = pLoader->GetKeyVector4 ( NULL );
		/*{
			D3DXVECTOR4 v4 = pLoader->GetKeyVector4 ( NULL );
			D3DXQUATERNION q;
			D3DXQuaternionRotationYawPitchRoll( &q , v4.y*D3DX_PI*2.0f/360.0f , v4.x*D3DX_PI*2.0f/360.0f , v4.z*D3DX_PI*2.0f/360.0f );
			p->rotation2 = D3DXVECTOR4( q.x , q.y , q.z , q.w );
		}
		else if( stricmp( strKey , "rotationQtV2" ) == 0 && !bLineEnd )
		{
			p->rotation2 = pLoader->GetKeyVector4 ( NULL );
			p->rotation3 = p->rotation2;
		}*/
		else if( stricmp( strKey , "bUwrapV2" ) == 0 && !bLineEnd )
			p->bUwrap2 = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "bVwrapV2" ) == 0 && !bLineEnd )
			p->bVwrap2 = pLoader->GetKeyBool ( NULL );
		else if( stricmp( strKey , "uvOffsetV2" ) == 0 && !bLineEnd )
			p->uvOffset2 = pLoader->GetKeyVector2 ( NULL );
		else if( stricmp( strKey , "uvRotationAngleV2" ) == 0 && !bLineEnd )
			p->uvRotationAngle2 = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "soundname" ) == 0 && !bLineEnd )
			strcpy( p->soundname , pLoader->GetKeyString ( NULL ) );
		else if( stricmp( strKey , "soundradio" ) == 0 && !bLineEnd )
			p->soundradio = pLoader->GetKeyFloat ( NULL );
		else if( stricmp( strKey , "bLoopPlay" ) == 0 && !bLineEnd )
			p->bLoopPlay = pLoader->GetKeyBool ( NULL );
	}
	return TRUE;
}
void CDymMagic::ResetParticlesPlayTime()
{
	for( std::list< CDymMagicParticles* >::iterator it = m_listParticles.begin () ; it != m_listParticles.end () ; it++ )
	{
		CDymMagicParticles * p = *it;
		p->m_fPlayTimeForAddObj = p->m_fAddTime ;
	}
}
BOOL CDymMagic::Play()
{
	if( !m_bOverTime )return FALSE;
	m_bOverTime = FALSE;
	m_fKeyAddTime = 0.0f;
	m_bFirstPlay = TRUE;
	ResetParticlesPlayTime();
	return TRUE;
}
BOOL CDymMagic::Stop()
{
	if( m_bOverTime )return FALSE;
	m_bOverTime = TRUE;
	return TRUE;
}
BOOL CDymMagic::isPlaying( float fElapsedTime )
{
//	if( m_bOverTime )return FALSE;
//	return TRUE;
	if( isloop )return TRUE;
	if( m_fKeyAddTime + fElapsedTime >= m_fKeyMaxTime )
		return FALSE;
	return TRUE;
}
float CDymMagic::GetCurTime()
{
	return m_fKeyAddTime;
}
void CDymMagic::SetCurTime(float t)
{
	m_fKeyAddTime = t;
	FrameMove( 0.0f );
}
void CDymMagic::SetLoop( BOOL bLoop )
{
	this->isloop = bLoop ;
}
BOOL CDymMagic::GetLoop()
{
	return isloop;
}
BOOL CDymMagic::FrameMove(float fElapsedTime)
{
	if( !m_bOverTime )
	{
		m_fKeyAddTime += fElapsedTime;
		FrameMoveForSound( fElapsedTime );
	}
	if( m_fKeyAddTime > m_fKeyMaxTime )
	{
		if( isloop )
		{
			if( m_fKeyMaxTime == 0.0f )m_fKeyAddTime = 0.0f;
			else m_fKeyAddTime = fmodf( m_fKeyAddTime , m_fKeyMaxTime );
			m_bFirstPlay = FALSE;

			m_fLastFrameTime = 0.0f;
			m_itSoundKey = m_listSoundKey.begin ();
			ResetParticlesPlayTime();
		}
		else
		{
			m_fKeyAddTime = m_fKeyMaxTime;
			m_bOverTime = TRUE;
			StopSound();
		}
		
	}
	for( std::list< CDymMagicParticles* >::iterator it = m_listParticles.begin () ; it != m_listParticles.end () ; it++ )
	{
		CDymMagicParticles * p = *it;
		p->FrameMove ( m_fKeyAddTime , fElapsedTime , m_world );
	}
	for( std::list< CDymMagicModel* >::iterator it = m_listModel.begin () ; it != m_listModel.end () ; it++ )
	{
		CDymMagicModel * p = *it;
		p->FrameMove ( m_fKeyAddTime , fElapsedTime );
	}
	if( m_bOverTime )return TRUE;
	return FALSE;
}

void CDymMagic::SetFastRender( BOOL bFast )
{
	m_bFastRender = bFast ;
	m_pDevice->SetFastRender ( bFast );
}
HRESULT CDymMagic::Render( DWORD flag )
{
	//m_pDevice->BeginGrass ();
	for( std::list< CDymMagicModel* >::iterator it = m_listModel.begin () ; it != m_listModel.end () ; it++ )
	{
		CDymMagicModel * p = *it;
		p->Render ( m_world , flag );
	}

	if( m_bFastRender || (flag & MAGIC_RENDER_FAST) )
		m_pDevice->BeginRender ( 0 );
	for( std::list< CDymMagicParticles* >::iterator it = m_listParticles.begin () ; it != m_listParticles.end () ; it++ )
	{
		CDymMagicParticles * p = *it;
		p->Render ( m_world , flag );
	}
	if( m_bFastRender || (flag & MAGIC_RENDER_FAST) )
		m_pDevice->EndRender ();
	m_pDevice->End();
	//m_pDevice->EndGrass ();
	/*D3DVIEWPORT9 v;
	v.X = 0;
	v.Y = 0;
	v.Width = 800;
	v.Height = 600;
	v.MinZ = 0.0f;
	v.MaxZ = 1.0f;
	m_pDevice->m_pd3dDevice->SetViewport ( &v );*/
	return S_OK;
}
void CDymMagic::SetGlobleScaling( D3DXVECTOR3 vSca )
{
	m_vGlobleScaling = vSca;
	m_world.vSca2 = vSca;
}
D3DXVECTOR3 CDymMagic::GetGlobleScaling()
{
	return m_vGlobleScaling;
}
void CDymMagic::ComputeBoundingBox()
{
	m_vBoundingBox1 = D3DXVECTOR3( 100000 , 100000 , 100000 );
	m_vBoundingBox2 = D3DXVECTOR3( -100000 , -100000 , -100000 );

	for( std::list< CDymMagicModel* >::iterator it = m_listModel.begin () ; it != m_listModel.end () ; it++ )
	{
		CDymMagicModel * p = *it;
		if( p->m_nID == 0 )continue;
		p->ComputeBoundingBox( m_world );

		if( m_vBoundingBox1.x > p->m_vBoundingBox1.x )
			m_vBoundingBox1.x = p->m_vBoundingBox1.x;
		if( m_vBoundingBox1.y > p->m_vBoundingBox1.y )
			m_vBoundingBox1.y = p->m_vBoundingBox1.y;
		if( m_vBoundingBox1.z > p->m_vBoundingBox1.z )
			m_vBoundingBox1.z = p->m_vBoundingBox1.z;
		if( m_vBoundingBox2.x < p->m_vBoundingBox2.x )
			m_vBoundingBox2.x = p->m_vBoundingBox2.x;
		if( m_vBoundingBox2.y < p->m_vBoundingBox2.y )
			m_vBoundingBox2.y = p->m_vBoundingBox2.y;
		if( m_vBoundingBox2.z < p->m_vBoundingBox2.z )
			m_vBoundingBox2.z = p->m_vBoundingBox2.z;
	}
	for( std::list< CDymMagicParticles* >::iterator it = m_listParticles.begin () ; it != m_listParticles.end () ; it++ )
	{
		CDymMagicParticles * p = *it;
		p->ComputeBoundingBox ( m_world );

		if( m_vBoundingBox1.x > p->m_vBoundingBox1.x )
			m_vBoundingBox1.x = p->m_vBoundingBox1.x;
		if( m_vBoundingBox1.y > p->m_vBoundingBox1.y )
			m_vBoundingBox1.y = p->m_vBoundingBox1.y;
		if( m_vBoundingBox1.z > p->m_vBoundingBox1.z )
			m_vBoundingBox1.z = p->m_vBoundingBox1.z;
		if( m_vBoundingBox2.x < p->m_vBoundingBox2.x )
			m_vBoundingBox2.x = p->m_vBoundingBox2.x;
		if( m_vBoundingBox2.y < p->m_vBoundingBox2.y )
			m_vBoundingBox2.y = p->m_vBoundingBox2.y;
		if( m_vBoundingBox2.z < p->m_vBoundingBox2.z )
			m_vBoundingBox2.z = p->m_vBoundingBox2.z;
	}
}

void CDymMagic::Clear(void)
{
	StopSound();
	for( std::list< CDymMagicParticles* >::iterator it = m_listParticles.begin () ; it != m_listParticles.end () ; it++ )
	{
		CDymMagicParticles * p = *it;
		delete p;
	}
	for( std::list< CDymMagicModel* >::iterator it = m_listModel.begin () ; it != m_listModel.end () ; it++ )
	{
		CDymMagicModel * p = *it;
		delete p;
	}
	m_listParticles.clear ();
	m_listModel.clear ();
	m_fKeyMaxTime = 2000.0f;
	m_fKeyAddTime = 0.0f;
	m_bOverTime = TRUE;
	m_nVersion = 1;
	m_vBoundingBox1 = D3DXVECTOR3( 0 , 0 , 0 ) ;
	m_vBoundingBox2 = D3DXVECTOR3( 0 , 0 , 0 ) ;
}
void CDymMagic::SetRotationWithID( int objid , int rotationid )
{
	CDymMagicBaseObj * pObj = GetObj( objid );
	if( pObj )
	{
		CDymMagicBaseObj *pRot = GetObj( rotationid );
		pObj->SetRotationWithID ( pRot );
	}
}
void CDymMagic::SetFollowWithID( int objid , int followid )
{
	CDymMagicBaseObj * pObj = GetObj( objid );
	if( pObj )
	{
		CDymMagicBaseObj *pRot = GetObj( followid );
		pObj->SetFollowWithID ( pRot );
	}
}
void CDymMagic::SetParticlesAcceptID( int particlesobjid )
{
	CDymMagicBaseObj * pObj = GetObj( particlesobjid );
	if( pObj )
	{
		if( pObj->GetObjType () == CDymMagicBaseObj::MAGICE_OBJ_TYPE::MOT_PARTICLE )
		{
			CDymMagicParticles *pParticles = (CDymMagicParticles*)pObj;
			pParticles->SetParticlesAcceptID( GetObj( pParticles->accept_id ) );
		}
	}
}
void CDymMagic::SetKeyMaxTime(float t)
{
	// Set
	for( std::list< CDymMagicParticles* >::iterator it = m_listParticles.begin () ; it != m_listParticles.end () ; it++ )
	{
		CDymMagicParticles * p = *it;
		p->m_KeyFrameManager .SetKeyMaxTime ( t );
	}
	for( std::list< CDymMagicModel* >::iterator it = m_listModel.begin () ; it != m_listModel.end () ; it++ )
	{
		CDymMagicModel * p = *it;
		p->m_KeyFrameManager .SetKeyMaxTime ( t );
	}
	m_fKeyMaxTime = t;
}
float CDymMagic::GetKeyMaxTime()
{
	return m_fKeyMaxTime;
}
void CDymMagic::ComputeKeyMaxTime(void)
{
	float fMaxTime = 0.0f;
	for( std::list< CDymMagicParticles* >::iterator it = m_listParticles.begin () ; it != m_listParticles.end () ; it++ )
	{
		CDymMagicParticles * p = *it;
		fMaxTime = max( p->m_KeyFrameManager .GetLastKeyFrame ()->m_fTime , fMaxTime );
	}
	for( std::list< CDymMagicModel* >::iterator it = m_listModel.begin () ; it != m_listModel.end () ; it++ )
	{
		CDymMagicModel * p = *it;
		if( p->m_nID == 0 )continue;
		fMaxTime = max( p->m_KeyFrameManager .GetLastKeyFrame ()->m_fTime , fMaxTime );
	}
	SetKeyMaxTime( fMaxTime );
}

int CDymMagic::CreateModel( MAGIC_CREATE_TYPE type , void *pUnknow )
{
	CDymMagicModel *p = new CDymMagicModel(m_pDevice);
	if( type == CREATE_BILLBOARD )
		p->SetParticlesName ( "quad" );
	else if( type == CREATE_SPIRE )
	{
		p->SetParticlesName ( "spire" );
		p->facenum = 16;
	}
	else if( type == CREATE_MESH )
	{
		p->SetParticlesName ( "mesh" );
		p->ImportMeshFromXML ( (LPCTSTR)pUnknow );
	}
	else if( type == CREATE_SHADOW )
	{
		p->SetParticlesName ( "shadow" );
		p->facenum = 4;
		p->initsize = D3DXVECTOR3( 0 , 1 , 0 );
	}
	else if( type == CREATE_SPIRE_SHADOW )
	{
		p->SetParticlesName ( "spire_shadow" );
		p->facenum = 32;
	}
	if( FAILED( p->init () ) )
	{
		delete p;
		return -1;
	}
	m_listModel.push_back ( p );
	p->m_nID = NewID();
	p->m_nID_rotation_with = p->m_nID ;
	p->m_nID_follow_with = p->m_nID ;
	//p->m_pBillboard ->SetBillboardType ( 1 );
	return p->m_nID ;
}

int CDymMagic::CreateParticles( void *pUnknow )
{
	CDymMagicParticles *p = new CDymMagicParticles(m_pDevice);
	p->init ();
	m_listParticles.push_back ( p );
	p->m_nID = NewID();
	p->m_nID_rotation_with = p->m_nID ;
	p->m_nID_follow_with = p->m_nID ;
	p->m_nVersion = 2;
	return p->m_nID ;
}

int CDymMagic::CopyCreate( CDymMagicBaseObj *pObj , void *pUnkonw )
{
	int rid = 0;
	if( pObj )
	{
		rid = NewID();
		CDymMagicBaseObj::MAGICE_OBJ_TYPE objtype = pObj->GetObjType ();
		if( objtype == CDymMagicBaseObj::MAGICE_OBJ_TYPE::MOT_MODEL )
		{
			CDymMagicModel *p = new CDymMagicModel(m_pDevice);
			p->CopyModel ( (CDymMagicModel*)pObj );
			p->m_nID = rid;
			p->m_nID_rotation_with = p->m_nID ;
			p->m_nID_follow_with = p->m_nID ;
			p->init ();
			m_listModel.push_back ( p );
		}
		else if( objtype == CDymMagicBaseObj::MAGICE_OBJ_TYPE::MOT_PARTICLE )
		{
			CDymMagicParticles *p = new CDymMagicParticles(m_pDevice);
			p->CopyParticles ( (CDymMagicParticles*)pObj );
			p->m_nID = rid;
			p->m_nID_rotation_with = p->m_nID ;
			p->m_nID_follow_with = p->m_nID ;
			p->init ();
			m_listParticles.push_back ( p );
		}
	}
	return rid;
}

CDymMagicKeyFrame* CDymMagic::CopyKeyFrame( CDymMagicKeyFrame *pKeyFrame )
{
	CDymMagicKeyFrame *pnew = NULL;
	CDymMagicKeyFrameManager *pManager = this->FindKeyManager ( pKeyFrame );
	if( pManager )
		pnew = pManager->CopyKey ( pKeyFrame , m_pDevice );
	return pnew;
}

int CDymMagic::CopyCreate( int id , void *pUnknow )
{
	CDymMagicBaseObj *pObj = GetObj ( id );
	if( pObj )return CopyCreate( pObj , pUnknow );
	return 0;
}

int CDymMagic::NewID()
{
	int id = rand()%0xFFFE+1;
	while( GetObj( id ) )
	{
		id = rand()%0xFFFE+1;
	}
	return id;
}

void CDymMagic::SetDefaultBillboardString( LPCTSTR str , DWORD color , void *pUnknow )
{
	if( g_pMagicFontFactory == NULL )return;
	CDymMagicModel *pModel = NULL;
	for( std::list< CDymMagicModel* >::iterator it = m_listModel.begin () ; it != m_listModel.end () ; it++ )
	{
		CDymMagicModel * p = *it;
		if( p->mytype == CDymMagicModel::MODEL_TYPE::MT_quad )
		{
			pModel = p;
			break;
		}
	}
	if( pModel )
	{
		CDymMagicKeyFrame *pKey = pModel->m_KeyFrameManager .GetKeyFrame2( 0.0f , 0.0f );
		if( pKey )
		{
			if( pKey->m_pTexture )
				if(FAILED(pKey->m_pTexture->Release ()))delete pKey->m_pTexture ;
			int width , int height;
			pKey->m_pTexture = g_pMagicFontFactory->NewTexture ( str , &width , &height , pUnknow );
			pKey->colour = D3DXVECTOR4( ((float)((color>>16)%256))/255.0f , ((float)((color>>8)%256))/255.0f ,
				((float)((color)%256))/255.0f , ((float)((color>>24)%256))/255.0f );
			pModel->initsize = D3DXVECTOR3( (float)width/100.0f , (float)height/100.0f , 1.0f );
			pModel->init ();
			ComputeBoundingBox ();
		}
	}
}
void CDymMagic::SetDefaultShadowPos( D3DXVECTOR3 v1 , D3DXVECTOR3 v2 )
{
	CDymMagicModel *pModel = NULL;
	for( std::list< CDymMagicModel* >::iterator it = m_listModel.begin () ; it != m_listModel.end () ; it++ )
	{
		CDymMagicModel * p = *it;
		if( p->mytype == CDymMagicModel::MODEL_TYPE::MT_shadow )
		{
			pModel = p;
			break;
		}
	}
	if( pModel )
	{
		pModel->initsize = v2-v1;
	//	CDymMagicKeyFrame *pKey = pModel->m_KeyFrameManager .GetKeyFrame2( 0.0f , 0.0f );
	//	if( pKey )pKey->position = v1;
		for( std::list< CDymMagicKeyFrame* >::iterator it = pModel->m_KeyFrameManager.m_listKeyFrame.begin ();
			it != pModel->m_KeyFrameManager.m_listKeyFrame.end () ; it++ )
		{
			CDymMagicKeyFrame *pKey = *it;
			if( pKey )pKey->position = v1;
		}
		pModel->Render ( m_world , 0 );
		pModel->init ();
	}
}
void CDymMagic::SetAllKeyTexture( LPCTSTR texFileName )
{
	TCHAR chardrive[_MAX_DRIVE];
	TCHAR chardir[_MAX_DIR];
	TCHAR charext[_MAX_EXT];
	TCHAR charname[_MAX_PATH];
	_splitpath(texFileName,chardrive,chardir,charname,charext); 
	TCHAR name[MAX_PATH];
	_stprintf(name , "%s%s",charname,charext );

	CDymMagicModel *pModel = NULL;
	for( std::list< CDymMagicModel* >::iterator it = m_listModel.begin () ; it != m_listModel.end () ; it++ )
	{
		CDymMagicModel * p = *it;
		//if( p->mytype == CDymMagicModel::MODEL_TYPE::MT_quad )
		{
			pModel = p;
			if( pModel )
			{
				for( std::list< CDymMagicKeyFrame* >::iterator it = pModel->m_KeyFrameManager.m_listKeyFrame.begin ();
					it != pModel->m_KeyFrameManager.m_listKeyFrame.end () ; it++ )
				{
					CDymMagicKeyFrame *pKey = *it;
					if( pKey )pKey->SetMatresName ( name , m_pDevice );
				}
			}
		}
	}
	
}
int CDymMagic::Collision(int moustx,int mousty)
{
	POINT pt = {moustx , mousty};
	float fDist = -1.0f;
	float fDistTmp;
	int id=-1;
	for( std::list< CDymMagicModel* >::iterator it = m_listModel.begin () ; it != m_listModel.end () ; it++ )
	{
		CDymMagicModel * p = *it;
		int facenum , Stride;
		D3DXVECTOR3 *pVertex = p->GetVertexStreamZeroData ( &facenum , &Stride );
		if( pVertex == NULL )continue;
		p->ComputeWorldMatrix ( m_world );
		if( Pick( pVertex , Stride , facenum , &pt , &p->GetWorld1 () , &fDistTmp , NULL ) )
		{
			if( fDist==-1.0f || fDist>fDistTmp )
			{
				fDist = fDistTmp;
				id = p->m_nID ;
			}
		}
		delete[] pVertex;
	}
	for( std::list< CDymMagicParticles* >::iterator it = m_listParticles.begin () ; it != m_listParticles.end () ; it++ )
	{
		CDymMagicParticles * p = *it;
		int facenum , Stride;
		D3DXVECTOR3 *pVertex = p->GetVertexStreamZeroData ( &facenum , &Stride );
		p->ComputeWorldMatrix ( m_world );
		if( Pick( pVertex , Stride , facenum , &pt , &p->GetWorld1 () , &fDistTmp , NULL ) )
		{
			if( fDist==-1.0f || fDist>fDistTmp )
			{
				fDist = fDistTmp;
				id = p->m_nID ;
			}
		}
		delete[] pVertex;
	}
	return id;
}
int CDymMagic::GetObjNum()
{
	return m_listParticles.size () + m_listModel.size ();
}
void CDymMagic::GetAllObj(int*idBuffer,int buffersize)
{
	int i=0;
	for( std::list< CDymMagicParticles* >::iterator it = m_listParticles.begin () ; it != m_listParticles.end () ; it++ )
	{
		CDymMagicParticles * p = *it;
		if(i>buffersize)return;
		idBuffer[i] = p->m_nID ;
		i++;
	}
	for( std::list< CDymMagicModel* >::iterator it = m_listModel.begin () ; it != m_listModel.end () ; it++ )
	{
		CDymMagicModel * p = *it;

		if(i>buffersize)return;
		idBuffer[i] = p->m_nID ;
		i++;
	}
}
CDymMagicKeyFrameManager *CDymMagic::FindKeyManager( CDymMagicKeyFrame *pKey )
{
	for( std::list< CDymMagicParticles* >::iterator it = m_listParticles.begin () ; it != m_listParticles.end () ; it++ )
	{
		CDymMagicParticles * p = *it;
		for( std::list< CDymMagicKeyFrame* >::iterator it2 = p->m_KeyFrameManager .m_listKeyFrame .begin ();
			it2 != p->m_KeyFrameManager .m_listKeyFrame .end (); it2++ )
			if( pKey == (*it2) )
				return &p->m_KeyFrameManager ;
	}
	for( std::list< CDymMagicModel* >::iterator it = m_listModel.begin () ; it != m_listModel.end () ; it++ )
	{
		CDymMagicModel * p = *it;
		for( std::list< CDymMagicKeyFrame* >::iterator it2 = p->m_KeyFrameManager .m_listKeyFrame .begin ();
			it2 != p->m_KeyFrameManager .m_listKeyFrame .end (); it2++ )
			if( pKey == (*it2) )
				return &p->m_KeyFrameManager ;
	}
	return NULL;
}
CDymMagicBaseObj *CDymMagic::GetObj(int id)
{
	for( std::list< CDymMagicParticles* >::iterator it = m_listParticles.begin () ; it != m_listParticles.end () ; it++ )
	{
		CDymMagicParticles * p = *it;
		if( p->m_nID == id )return p;
	}
	for( std::list< CDymMagicModel* >::iterator it = m_listModel.begin () ; it != m_listModel.end () ; it++ )
	{
		CDymMagicModel * p = *it;
		if( p->m_nID == id )return p;
	}
	return NULL;
}
void CDymMagic::_DelObj( CDymMagicBaseObj *pDel )
{
	for( std::list< CDymMagicParticles* >::iterator it = m_listParticles.begin () ; it != m_listParticles.end () ; it++ )
	{
		CDymMagicParticles * p = *it;
		if( p->m_nID_rotation_with == pDel->m_nID )
			p->SetRotationWithID ( NULL ) ;
		if( p->m_nID_follow_with == pDel->m_nID )
			p->SetFollowWithID ( NULL );
		if( p->accept_id == pDel->m_nID )
			p->SetParticlesAcceptID ( NULL );
	}
	for( std::list< CDymMagicModel* >::iterator it = m_listModel.begin () ; it != m_listModel.end () ; it++ )
	{
		CDymMagicModel * p = *it;
		if( p->m_nID_rotation_with == pDel->m_nID )
			p->SetRotationWithID ( NULL ) ;
		if( p->m_nID_follow_with == pDel->m_nID )
			p->SetFollowWithID ( NULL );
	}
}
BOOL CDymMagic::DelObj(int id )
{
	for( std::list< CDymMagicParticles* >::iterator it = m_listParticles.begin () ; it != m_listParticles.end () ; it++ )
	{
		CDymMagicParticles * p = *it;
		if( p->m_nID == id )
		{
			_DelObj( p );
			m_listParticles.remove ( p );
			delete p;
			return TRUE;
		}
	}
	for( std::list< CDymMagicModel* >::iterator it = m_listModel.begin () ; it != m_listModel.end () ; it++ )
	{
		CDymMagicModel * p = *it;
		if( p->m_nID == id )
		{
			_DelObj( p );
			m_listModel.remove ( p );
			delete p;
			return TRUE;
		}
	}
	return FALSE;
}
HRESULT CDymMagic::_D3DXIntersect3(CONST void *pDataStream,CONST int facenum,CONST int Stride,CONST D3DXVECTOR3 * pRayPos,CONST D3DXVECTOR3 * pRayDir,
  BOOL * pHit,DWORD * pFaceIndex,FLOAT * pU,FLOAT * pV,FLOAT * pDist,LPD3DXBUFFER * ppAllHits,DWORD * pCountOfHits,
  D3DXMATRIX *pmatWorld1,D3DXMATRIX *pmatWorld2,int StrideForBlend)
{
	LPD3DXMESH pMesh;
	D3DXCreateMeshFVF( facenum , facenum*3 , D3DXMESH_DYNAMIC , D3DFVF_XYZ , m_pDevice->m_pd3dDevice , &pMesh );
	D3DXVECTOR3 *pVertex;
	pMesh->LockVertexBuffer ( 0 , (void**)&pVertex );
	//memcpy( pVertex , pDataStream , sizeof(D3DXVECTOR3)*facenum*3 );
	for(int i=0;i<facenum*3;i++)
		pVertex[i] = *(D3DXVECTOR3*)((BYTE*)pDataStream+Stride*i);
	pMesh->UnlockVertexBuffer ();
	WORD *idx;
	pMesh->LockIndexBuffer ( 0 , (void**)&idx );
	for(int i=0;i<facenum*3;i++)
		idx[i]=i;
	pMesh->UnlockIndexBuffer ();
	HRESULT hr=D3DXIntersect( pMesh , pRayPos , pRayDir , pHit , pFaceIndex , pU , pV , pDist , ppAllHits , pCountOfHits );
	pMesh->Release ();
	return S_OK;
}
BOOL CDymMagic::Pick(const D3DXVECTOR3* pVertexStreamZeroData,UINT VertexStreamZeroStride,int facenum,
					 POINT *pmousePt,D3DXMATRIX *pMul,float *pDisOut,D3DXVECTOR3 *pPosOut)
{
	int nWindowWidth = 800;
	int nWindowHeight = 600;
	D3DXVECTOR3 vPickRayDir;
	D3DXVECTOR3 vPickRayOrig;

	// Get the pick ray from the mouse position
	//D3DXMATRIXA16 matProj;
	//m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
    D3DXMATRIX matProj;
	m_pDevice->GetTransform ( D3DTS_PROJECTION , &matProj );
	// Compute the vector of the pick ray in screen space
	D3DXVECTOR3 v;
	v.x =  ( ( ( 2.0f * pmousePt->x ) / nWindowWidth  ) - 1 ) / matProj._11;
	v.y = -( ( ( 2.0f * pmousePt->y ) / nWindowHeight ) - 1 ) / matProj._22;
	v.z =  1.0f;

	// Get the inverse view matrix
	D3DXMATRIXA16 matView, m;
	m_pDevice->GetTransform ( D3DTS_VIEW , &matView );
	D3DXMatrixInverse( &m, NULL, &matView );

	if(pMul)
	{
		D3DXMATRIX inv;
		D3DXMatrixInverse(&inv,NULL,pMul);
		D3DXMatrixMultiply(&m,&m,&inv);
	}

	// Transform the screen space pick ray into 3D space
	vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
	vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
	vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
	vPickRayOrig.x = m._41;
	vPickRayOrig.y = m._42;
	vPickRayOrig.z = m._43;

	// When calling D3DXIntersect, one can get just the closest intersection and not
	// need to work with a D3DXBUFFER.  Or, to get all intersections between the ray and 
	// the mesh, one can use a D3DXBUFFER to receive all intersections.  We show both
	// methods.
	// Collect only the closest intersection
	BOOL bHit;
	DWORD dwFace;
	FLOAT fBary1, fBary2, fDist;
	_D3DXIntersect3(pVertexStreamZeroData , facenum,VertexStreamZeroStride, &vPickRayOrig, &vPickRayDir, &bHit, &dwFace, &fBary1, &fBary2, &fDist, 
		NULL, NULL);
	if( bHit )
	{
		if(pDisOut)*pDisOut=fDist;
		return TRUE;
	}

	/*BOOL bHit = FALSE;
	FLOAT t,u,vv, fDist;
	FLOAT fDist2=10000.0f;
	for( DWORD i=0; i<facenum; i++ )
    {
        D3DXVECTOR3 v0 = pVertexStreamZeroData[i*3+0];
        D3DXVECTOR3 v1 = pVertexStreamZeroData[i*3+1];
        D3DXVECTOR3 v2 = pVertexStreamZeroData[i*3+2];

        // Check if the pick ray passes through this point
        if( M3D_IntersectTriangle( vPickRayOrig, vPickRayDir, v0, v1, v2,
                                &t,&u,&vv,NULL ) )
        {
			rand();
        }
    }*/
	return FALSE;
}

BOOL CDymMagic::M3D_IntersectTriangle( const D3DXVECTOR3& orig,
                                       const D3DXVECTOR3& dir, D3DXVECTOR3& v0,
                                       D3DXVECTOR3& v1, D3DXVECTOR3& v2,
                                       FLOAT* t, FLOAT* u, FLOAT* v ,D3DXVECTOR3 *pOut)
{
    // Find vectors for two edges sharing vert0
    D3DXVECTOR3 edge1 = v1 - v0;
    D3DXVECTOR3 edge2 = v2 - v0;

    // Begin calculating determinant - also used to calculate U parameter
    D3DXVECTOR3 pvec;
    D3DXVec3Cross( &pvec, &dir, &edge2 );

    // If determinant is near zero, ray lies in plane of triangle
    FLOAT det = D3DXVec3Dot( &edge1, &pvec );

    D3DXVECTOR3 tvec;
    if( det > 0 )
    {
        tvec = orig - v0;
    }
    else
    {
        tvec = v0 - orig;
        det = -det;
    }

    if( det < 0.0001f )
        return FALSE;

    // Calculate U parameter and test bounds
    *u = D3DXVec3Dot( &tvec, &pvec );
    if( *u < 0.0f || *u > det )
        return FALSE;

    // Prepare to test V parameter
    D3DXVECTOR3 qvec;
    D3DXVec3Cross( &qvec, &tvec, &edge1 );

    // Calculate V parameter and test bounds
    *v = D3DXVec3Dot( &dir, &qvec );
    if( *v < 0.0f || *u + *v > det )
        return FALSE;

    // Calculate t, scale parameters, ray intersects triangle
    *t = D3DXVec3Dot( &edge2, &qvec );
    FLOAT fInvDet = 1.0f / det;
    *t *= fInvDet;
    *u *= fInvDet;
    *v *= fInvDet;

	// 求交点
	D3DXVECTOR3 t_normal;
	D3DXVec3Cross( &t_normal, &edge1, &edge2 );//求法线

	D3DXVec3Normalize(&t_normal,&t_normal);//正规化

	float t_a=t_normal.x;
	float t_b=t_normal.y;
	float t_c=t_normal.z;
	float t_d=-D3DXVec3Dot(&t_normal,&v1);//求平面方程

	D3DXVECTOR3 t_dir;
	D3DXVec3Normalize(&t_dir,&dir);

	float t_t=(t_d-D3DXVec3Dot(&t_normal,&orig))/D3DXVec3Dot(&t_normal,&t_dir);//p=s+t*v;求t

	D3DXVECTOR3 t_point=orig+t_t*t_dir;
	if(pOut)*pOut=t_point;

	/*char t_char[255];
	sprintf(t_char,"x:%f,y:%f,z:%f\n",t_point.x,t_point.y,t_point.z);
	OutputDebugString(t_char);*/

    return TRUE;
}
void CDymMagic::SetPosition(D3DXVECTOR3 &v)
{
	m_world.vPos = v;
}
void CDymMagic::SetScaling(D3DXVECTOR3 v)
{
	m_world.vSca = v;
}
void CDymMagic::SetRotation(float a)
{
	if( !m_bDisableRotation )
		m_world.fAngle = a;
}
void CDymMagic::SetRotationAxis(D3DXVECTOR3 &v)
{
	if( !m_bDisableRotation )
		m_world.vAxis = v;
}
D3DXVECTOR3 CDymMagic::GetPosition()
{
	return m_world.vPos ;
}
D3DXVECTOR3 CDymMagic::GetScaling()
{
	return m_world.vSca ;
}
D3DXVECTOR3 CDymMagic::GetRotationAxis()
{
	return m_world.vAxis ;
}
float CDymMagic::GetRotation()
{
	return m_world.fAngle ;
}

void CDymMagic::RenderSelection(int* ObjIDindex, int indexSize)
{
	m_pDevice->SetRenderState ( D3DRS_ZENABLE , FALSE );
	for(int i=0;i<indexSize;i++)
	{
		CDymMagicBaseObj *pObj = GetObj( ObjIDindex[i] );
		if(pObj)
		{
			pObj->SetupRenderState ( m_world );
			m_pDevice->SetTexture ( 0 , NULL );
			m_pDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xFF0000FF );
			m_pDevice->SetTextureStageState (0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
			m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE);
			m_pDevice->SetRenderState( D3DRS_FILLMODE,	D3DFILL_WIREFRAME);
			pObj->Render ( m_world , MAGIC_RENDER_DO_NOT_SETUP_RENDERSTATE | MAGIC_RENDER_NO_HIDE | MAGIC_RENDER_SELECTION);
		}
	}
	m_pDevice->SetRenderState ( D3DRS_ZENABLE , TRUE );
}

BOOL CDymMagic::isRepeat()
{
	return (BOOL)isloop;
}
void CDymMagic::SetRepeat( BOOL bRepeat )
{
	isloop = (bool)bRepeat;
}
void CDymMagic::SetTexture( CDymMagicKeyFrame *pKey , LPCTSTR filename )
{
	pKey->SetMatresName ( filename , m_pDevice );
}
void CDymMagic::SetSound( CDymMagicKeyFrame *pKey , LPCTSTR filename )
{
	pKey->SetSound ( filename );
}

BOOL CDymMagic::ChangeID( int oldid , int newid )
{
	if( GetObj( newid ) )return FALSE;
	CDymMagicBaseObj *pObj = GetObj( oldid );
	if( pObj == NULL ) return FALSE;
	pObj->m_nID = newid;
	return TRUE;
}

void CDymMagic::SaveKeyFrame( CDymScriptLoader *pLoader , CDymMagicKeyFrameManager *pKeyMana )
{
#define CHECK_KEY( a , b , c , d )if(!a)d;else if(Check_PrevKey(b,c))d;
#define CHECK_KEY_STR( a , b , c , d )if(!a)d;else if(Check_PrevKey_Str(b,c))d;
	CDymMagicKeyFrame *pPrevKey = NULL;
	for( std::list<CDymMagicKeyFrame*>::iterator it = pKeyMana->m_listKeyFrame .begin () ;
		it != pKeyMana->m_listKeyFrame.end () ; it++ )
	{
		CDymMagicKeyFrame *pKey = *it;
		pLoader->SaveKeyFloat ( "keyframe" , pKey->m_fTime );
		pLoader->SaveBracket_L ();
		CHECK_KEY( pPrevKey , pPrevKey->nVersion  , pKey->nVersion ,
			pLoader->SaveKeyInt ("key_version" , pKey->nVersion ) );
		CHECK_KEY( pPrevKey , pPrevKey->position  , pKey->position ,
			pLoader->SaveKeyVector3 ( "position" , pKey->position ) );
		CHECK_KEY( pPrevKey , pPrevKey->animate  , pKey->animate ,
			pLoader->SaveKeyBool ("animate" , pKey->animate ) );
		CHECK_KEY( pPrevKey , pPrevKey->scale  , pKey->scale ,
			pLoader->SaveKeyVector3 ( "scale" , pKey->scale ) );
		CHECK_KEY( pPrevKey , pPrevKey->direction  , pKey->direction ,
			pLoader->SaveKeyVector3 ( "direction" , pKey->direction ) );
		CHECK_KEY( pPrevKey , pPrevKey->colour  , pKey->colour ,
			pLoader->SaveKeyVector4 ( "colour" , pKey->colour ) );
		CHECK_KEY_STR( pPrevKey , pPrevKey->matresname  , pKey->matresname ,
			pLoader->SaveKeyString( "matresname" , pKey->matresname ) );
		CHECK_KEY( pPrevKey , pPrevKey->matUVtile  , pKey->matUVtile ,
			pLoader->SaveKeyVector2 ( "matUVtile" , pKey->matUVtile ) );
		CHECK_KEY( pPrevKey , pPrevKey->topscale  , pKey->topscale ,
			pLoader->SaveKeyFloat ("topscale" , pKey->topscale ) );
		CHECK_KEY( pPrevKey , pPrevKey->bottomscale  , pKey->bottomscale ,
			pLoader->SaveKeyFloat ("bottomscale" , pKey->bottomscale ) );
		CHECK_KEY( pPrevKey , pPrevKey->rotation2  , pKey->rotation2 ,
			pLoader->SaveKeyVector4 ("rotationV2" , pKey->rotation2 ) );
		CHECK_KEY( pPrevKey , pPrevKey->rotation3  , pKey->rotation3 ,
			pLoader->SaveKeyVector4 ("rotation3V2" , pKey->rotation3 ) );
		CHECK_KEY( pPrevKey , pPrevKey->bUwrap2  , pKey->bUwrap2 ,
			pLoader->SaveKeyBool ( "bUwrapV2" , pKey->bUwrap2 ) );
		CHECK_KEY( pPrevKey , pPrevKey->bVwrap2  , pKey->bVwrap2 ,
			pLoader->SaveKeyBool ( "bVwrapV2" , pKey->bVwrap2 ) );
		CHECK_KEY( pPrevKey , pPrevKey->uvOffset2  , pKey->uvOffset2 ,
			pLoader->SaveKeyVector2 ( "uvOffsetV2" , pKey->uvOffset2 ) );
		CHECK_KEY( pPrevKey , pPrevKey->uvRotationAngle2  , pKey->uvRotationAngle2 ,
			pLoader->SaveKeyFloat ("uvRotationAngleV2" , pKey->uvRotationAngle2 ) );
		CHECK_KEY_STR( pPrevKey , pPrevKey->soundname  , pKey->soundname ,
			pLoader->SaveKeyString( "soundname" , pKey->soundname ) );
		CHECK_KEY( pPrevKey , pPrevKey->bLoopPlay , pKey->bLoopPlay ,
			pLoader->SaveKeyBool ("bLoopPlay" , pKey->bLoopPlay ) );
		CHECK_KEY( pPrevKey , pPrevKey->soundradio , pKey->soundradio ,
			pLoader->SaveKeyFloat ("soundradio" , pKey->soundradio ) );
		pLoader->SaveBracket_R ();

		pPrevKey = pKey;
	}
}

BOOL CDymMagic::Save( LPCTSTR fullpathfilename , void **ppSaveBuffer , int *pBufferSize )
{
	if( fullpathfilename )
		ComputeBoundingBox ();
	TCHAR chardrive[_MAX_DRIVE] = "" ;
	TCHAR chardir[_MAX_DIR] = "" ;
	TCHAR charext[_MAX_EXT] = "" ;
	TCHAR charname[_MAX_PATH] = "" ;
	if( fullpathfilename )
		_splitpath(fullpathfilename,chardrive,chardir,charname,charext);

	CDymScriptLoader loader;
	if( fullpathfilename )
		loader.SaveBegin ( fullpathfilename );
	else
		loader.SaveToMemoryBegin ();
	loader.SaveKeyString ( "//" , "特效编辑器脚本文件，作者:dongyeming，版本:V2.0" );
	loader.SaveKeyString ( "magic_name" , charname );
	loader.SaveKeyString ("magic_version" , "2" );
	loader.SaveBracket_L ();
	loader.SaveKeyFloat ( "maxtime" , this->m_fKeyMaxTime );
	loader.SaveKeyBool ( "isloop" , this->isloop );
	loader.SaveKeyVector3 ( "bounding_box1" , this->m_vBoundingBox1 );
	loader.SaveKeyVector3 ( "bounding_box2" , this->m_vBoundingBox2 );
	loader.SaveKeyBool ( "disable_rotation" , m_bDisableRotation );
	loader.SaveKeyVector3 ( "globle_scaling" , m_vGlobleScaling );
	loader.SaveKeyInt ( "ex_data" , m_nExData );
	loader.SaveKeyBool( "grass_method" , m_world.bGrassMethod );
	for( std::list< CDymMagicParticles* >::iterator it = m_listParticles.begin () ; it != m_listParticles.end () ; it++ )
	{
		CDymMagicParticles * p = *it;
		loader.SaveKeyInt ( "particles" , p->m_nID );
		loader.SaveBracket_L ();
		loader.SaveKeyString ( "strobjname" , p->m_strObjName );
		loader.SaveKeyInt ( "rotation_id" , p->m_nID_rotation_with );
		loader.SaveKeyInt ( "follow_id" , p->m_nID_follow_with );
		loader.SaveKeyInt ( "particleversion" , p->m_nVersion );
		loader.SaveKeyInt ( "particles_type" , p->particles_type );
		loader.SaveKeyString( "materialtype" , p->GetMaterialType () );
		loader.SaveKeyFloat ( "particle_width" , p->particle_width );
		loader.SaveKeyFloat ( "particle_height" , p->particle_height );
		loader.SaveKeyFloat ( "particle_width2" , p->particle_width2 );
		loader.SaveKeyFloat ( "particle_height2" , p->particle_height2 );
		loader.SaveKeyFloat ( "width" , p->ellipsoid_size.x );
		loader.SaveKeyFloat ( "height" , p->ellipsoid_size.y );
		loader.SaveKeyFloat ( "depth" , p->ellipsoid_size.z );
		loader.SaveKeyVector3 ( "direction" , p->direction );
		loader.SaveKeyFloat ( "angle" , p->angle );
		loader.SaveKeyFloat ( "emission_rate" , p->emission_rate );
		loader.SaveKeyFloat ( "emission_time" , p->emission_time );
		loader.SaveKeyFloat ( "emission_num" , p->emission_num );
		loader.SaveKeyBool ( "bUseRate" , p->m_bUseRate );
		loader.SaveKeyFloat ( "velocity_min" , p->velocity_min );
		loader.SaveKeyFloat ( "velocity_max" , p->velocity_max );
		loader.SaveKeyFloat ( "time_to_live_min" , p->time_to_live_min );
		loader.SaveKeyFloat ( "time_to_live_max" , p->time_to_live_max );
		loader.SaveKeyVector4 ( "colour_range_start" , p->colour_range_start );
		loader.SaveKeyVector4 ( "colour_range_end" , p->colour_range_end );
		loader.SaveKeyVector4 ( "ColourFader" , p->ColourFader );
		loader.SaveKeyVector3 ( "force_vector" , p->force_vector );
		loader.SaveKeyFloat ( "scale_add_x" , p->scale_add_x );
		loader.SaveKeyFloat ( "scale_add_y" , p->scale_add_y );
		loader.SaveKeyFloat ( "rotate_add" , p->rotate_add );
		loader.SaveKeyBool ( "iswithfather" , p->iswithfather );
		loader.SaveKeyInt ( "force_type" , p->force_type );
		loader.SaveKeyFloat ( "accept_power" , p->accept_power );
		loader.SaveKeyInt ( "force_add_type" , p->force_add_type );
		loader.SaveKeyInt ( "billboard_type" , p->billboard_type );
		loader.SaveKeyVector3 ( "bounding_box1" , p->m_vBoundingBox1 );
		loader.SaveKeyVector3 ( "bounding_box2" , p->m_vBoundingBox2 );
		loader.SaveKeyInt ( "accept_id" , p->accept_id );
		loader.SaveKeyInt ( "limit_vector" , p->limit_vector );
		SaveKeyFrame( &loader , &p->m_KeyFrameManager ); 
		loader.SaveBracket_R ();
	}
	for( std::list< CDymMagicModel* >::iterator it = m_listModel.begin () ; it != m_listModel.end () ; it++ )
	{
		CDymMagicModel * p = *it;
		if( p->m_nID == 0 )continue;
		loader.SaveKeyInt ( "model" , p->m_nID );
		loader.SaveBracket_L ();
		loader.SaveKeyString ( "strobjname" , p->m_strObjName );
		loader.SaveKeyInt ( "rotation_id" , p->m_nID_rotation_with );
		loader.SaveKeyInt ( "follow_id" , p->m_nID_follow_with );
		loader.SaveKeyString( "typename" , p->m_strName );
		loader.SaveKeyString( "meshfilename" , p->m_strMeshName );
		loader.SaveKeyString( "mattypename" , p->GetMaterialType () );
		loader.SaveKeyString( "matresname" , p->materialres );
		loader.SaveKeyVector3 ( "initsize" , p->initsize );
		loader.SaveKeyBool ("isinfacex" , p->isinfacex );
		loader.SaveKeyBool ("isinfacey" , p->isinfacey );
		loader.SaveKeyBool ("isinfacez" , p->isinfacez );
		loader.SaveKeyInt ( "meshbillboardtype" , p->m_nMeshBillBoardType );
		loader.SaveKeyInt ( "facenum" , p->facenum );
		loader.SaveKeyFloat ( "flyspeed" , p->flyspeed );
		loader.SaveKeyVector3 ( "bounding_box1" , p->m_vBoundingBox1 );
		loader.SaveKeyVector3 ( "bounding_box2" , p->m_vBoundingBox2 );
		loader.SaveKeyBool ( "bUsingMeshVertexColor" , p->bUsingMeshVertexColor );
		SaveKeyFrame( &loader , &p->m_KeyFrameManager ); 
		loader.SaveBracket_R ();
	}
	loader.SaveBracket_R ();
	if( fullpathfilename )
		loader.SaveEnd ();
	else
		*ppSaveBuffer = loader.SaveToMemoryEnd ( pBufferSize );
	return FALSE;
}
void CDymMagic::ComputeSound()
{
	m_listSoundKey.clear ();
	for( std::list< CDymMagicParticles* >::iterator it = m_listParticles.begin () ; it != m_listParticles.end () ; it++ )
	{
		CDymMagicParticles * p = *it;
		_AddSound( &p->m_KeyFrameManager );
	}
	for( std::list< CDymMagicModel* >::iterator it = m_listModel.begin () ; it != m_listModel.end () ; it++ )
	{
		CDymMagicModel * p = *it;
		_AddSound( &p->m_KeyFrameManager );
	}
	m_itSoundKey = m_listSoundKey.begin ();
}
void CDymMagic::_AddSound( CDymMagicKeyFrameManager *pManager )
{
	for( std::list< CDymMagicKeyFrame* >::iterator it = pManager->m_listKeyFrame.begin () ; it != pManager->m_listKeyFrame.end () ; it++ )
	{
		CDymMagicKeyFrame *p = *it;
		if( p->soundname [0] != 0 )
		m_listSoundKey.push_back ( p );
	}
}
CDymMagicBaseObj *CDymMagic::GetObj ( CDymMagicKeyFrame *pKey )
{
	for( std::list< CDymMagicParticles* >::iterator it = m_listParticles.begin () ; it != m_listParticles.end () ; it++ )
	{
		CDymMagicParticles * p = *it;
		for( std::list< CDymMagicKeyFrame *>::iterator it = p->m_KeyFrameManager.m_listKeyFrame.begin () ; 
			it != p->m_KeyFrameManager.m_listKeyFrame.end () ; it ++ )
		{
			if( pKey == (*it) )return p;
		}
	}
	for( std::list< CDymMagicModel* >::iterator it = m_listModel.begin () ; it != m_listModel.end () ; it++ )
	{
		CDymMagicModel * p = *it;
		for( std::list< CDymMagicKeyFrame *>::iterator it = p->m_KeyFrameManager.m_listKeyFrame.begin () ; 
			it != p->m_KeyFrameManager.m_listKeyFrame.end () ; it ++ )
		{
			if( pKey == (*it) )return p;
		}
	}
	return NULL;
}
void CDymMagic::FrameMoveForSound( float skiptime )
{
	m_fLastFrameTime += skiptime ;
	if( m_listSoundKey.size() > 0 )
	{
		if( m_itSoundKey != m_listSoundKey.end () )
		{
			CDymMagicKeyFrame *p = *m_itSoundKey;
			float time = p->m_fTime ;
			if( m_fLastFrameTime > time && m_pSoundManager )
			{
				if( m_bFirstPlay || p->bLoopPlay == FALSE )
				{
					CDymSoundObj *pSoundObj = m_pSoundManager->CreateSound ( p->soundname );
					CDymMagicBaseObj *pBaseObj = GetObj( p );
					if( pBaseObj && pSoundObj )
					{
						SOUND_OBJ *pSound = new SOUND_OBJ;
						pSound->bLoop = p->bLoopPlay ;
						pSound->pObj = pBaseObj;
						pSound->pSound = pSoundObj;
						pSound->radio = p->soundradio ;
						m_listSoundObj.push_back( pSound );
						D3DXVECTOR3 vPos = *(pBaseObj->GetPos ());
						pSoundObj->SetPos ( vPos.x+m_world.vPos.x , vPos.y+m_world.vPos.y , vPos.z+m_world.vPos.z );
						pSoundObj->SetRadio( p->soundradio );
						pSoundObj->Play ( p->bLoopPlay );
					}
				}
				m_itSoundKey++;
			}
		}
	}
	for( std::list< SOUND_OBJ* >::iterator it = m_listSoundObj.begin () ; it != m_listSoundObj.end () ; it ++ )
	{
		SOUND_OBJ *pObj = *it;
		if( pObj->pSound->isPlaying () )
		{
			D3DXVECTOR3 vPos = *(pObj->pObj->GetPos ());
			pObj->pSound->SetPos ( vPos.x+m_world.vPos.x , vPos.y+m_world.vPos.y , vPos.z+m_world.vPos.z );
			//pObj->pSound->SetRadio ( pObj->radio );
		}
		else
		{
			it--;
			m_listSoundObj.remove ( pObj );
			m_pSoundManager->RemoveSound ( pObj->pSound );
			delete pObj->pSound;
			delete pObj;
		}
	}
}
void CDymMagic::StopSound()
{
	for( std::list< SOUND_OBJ* >::iterator it = m_listSoundObj.begin () ; it != m_listSoundObj.end () ; it ++ )
	{
		SOUND_OBJ *pObj = *it;
		if( pObj->bLoop )// 非循环播放的音乐会被自动删除，所以这里只删循环播放的音乐
		{
			m_pSoundManager->RemoveSound ( pObj->pSound );
			delete pObj->pSound ;
			delete pObj;
		}
		else
		{
			pObj->pSound->m_bAutoDelete = TRUE;
			delete pObj;
		}
	}
	m_listSoundObj.clear ();
	m_fLastFrameTime = 0.0f;
	m_itSoundKey = m_listSoundKey.begin ();
}
void CDymMagic::RenderSoundRadio()
{
	m_pDevice->SetTexture ( 0 , NULL );
	m_pDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xFF0000FF );
	m_pDevice->SetTextureStageState (0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
	m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE);
	m_pDevice->SetRenderState( D3DRS_FILLMODE,	D3DFILL_WIREFRAME);

	for( std::list< CDymMagicParticles* >::iterator it = m_listParticles.begin () ; it != m_listParticles.end () ; it++ )
	{
		CDymMagicParticles * p = *it;
		_RenderSoundRadio( &p->m_KeyFrameManager );
	}
	for( std::list< CDymMagicModel* >::iterator it = m_listModel.begin () ; it != m_listModel.end () ; it++ )
	{
		CDymMagicModel * p = *it;
		_RenderSoundRadio( &p->m_KeyFrameManager );
	}

}
void CDymMagic::_RenderSoundRadio( CDymMagicKeyFrameManager *pManager )
{
	for( std::list< CDymMagicKeyFrame* >::iterator it = pManager->m_listKeyFrame.begin () ; it != pManager->m_listKeyFrame.end () ; it++ )
	{
		CDymMagicKeyFrame *p = *it;
		if( p->soundname [0] != 0 )
			m_pDevice->DrawCircle ( *GetObj( p )->GetPos () , p->soundradio , 0xFF0000FF );
	}
}
float CDymMagic::_GetSoundArea( CDymMagicKeyFrameManager *pManager )
{
	float radio = 0.0f;
	for( std::list< CDymMagicKeyFrame* >::iterator it = pManager->m_listKeyFrame.begin () ; it != pManager->m_listKeyFrame.end () ; it++ )
	{
		CDymMagicKeyFrame *p = *it;
		if( radio < p->soundradio ) radio = p->soundradio ;
	}
	return radio;
}
float CDymMagic::GetSoundArea()
{
	float radio = 0.0f;
	for( std::list< CDymMagicParticles* >::iterator it = m_listParticles.begin () ; it != m_listParticles.end () ; it++ )
	{
		CDymMagicParticles * p = *it;
		float f = _GetSoundArea( &p->m_KeyFrameManager );
		if( radio < f )radio = f;
	}
	for( std::list< CDymMagicModel* >::iterator it = m_listModel.begin () ; it != m_listModel.end () ; it++ )
	{
		CDymMagicModel * p = *it;
		float f = _GetSoundArea( &p->m_KeyFrameManager );
		if( radio < f )radio = f;
	}
	return radio;

}
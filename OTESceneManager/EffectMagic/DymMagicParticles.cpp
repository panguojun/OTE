/***************************************************************************************************
文件名称:	DymMagicParticles.cpp
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	粒子
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#include ".\dymmagicparticles.h"

CDymMagicParticles::CDymMagicParticles(CDymRenderSystem *pDevice):CDymMagicBaseObj(pDevice,CDymMagicBaseObj::MOT_PARTICLE)
{
	m_strName[0] = 0;
	materialtype = MT_Effect_sceneadd;
	materialres[0] = 0;
	// For particles
	quota = 0;
	particle_width = 0.5f;
	particle_height = 0.5f;
	particle_width2 = 0.5f;
	particle_height2 = 0.5f;
	cull_each = false;
	ishav_qua = false;
	iswithfather = true;
	force_vector = D3DXVECTOR3( 0 , 0 , 0 );
	ellipsoid_size = D3DXVECTOR3( 1 , 1 , 1 );
	time_to_live_min = 2.0f;
	time_to_live_max = 2.0f;
	colour_range_start = D3DXVECTOR4( 1 , 1 , 1 , 1 );
	colour_range_end = D3DXVECTOR4( 1 , 1 , 1 , 1 );
	velocity_min = 1.0f;
	velocity_max = 1.0f;
	direction = D3DXVECTOR3( 0 , 1 , 0 );
	angle = 0.0f;
	israndangle = false;
	isonlydirection = false;
	
	ColourFader = D3DXVECTOR4( 0 , 0 , 0 , 0 );
	scale_add_x = 0.0f;
	scale_add_y = 0.0f;
	rotate_add = 0.0f;
	m_nVersion = 0;
	force_type = 0;
	accept_power = 1.0f;
	force_add_type = 0;
	billboard_type = 1;

	m_fAddTimeLast = 0.0f;
	m_fAddTime = 0.0f;
	m_fPlayTimeForAddObj = 0.0f;

	m_bTrackRand = 0;
	m_nTrackRandNum = 0;
	m_nTrackRandNum2 = 0;
	m_pTrackRandArray = NULL;
	accept_id = 0;
	m_pAcceptObj = NULL;
	particles_type = 0;
	limit_vector = 0;

	m_bUseRate = true;
	emission_rate = 100.0f;
	emission_num = 10;
	emission_time = 1000.0f;
}

CDymMagicParticles::~CDymMagicParticles(void)
{
	this->Clear();
}

void CDymMagicParticles::Clear()
{
	for( std::list< CDymMagicParticlesObj* >::iterator it = m_listObj.begin () ; it != m_listObj.end () ; it++ )
	{
		CDymMagicParticlesObj *pObj = *it;
		delete pObj;
	}
	m_listObj.clear ();
}
int CDymMagicParticles::GetBillboardType()
{
	return billboard_type;
}
void CDymMagicParticles::SetBillboardType( int type )
{
	billboard_type = type;
}
void CDymMagicParticles::SetParticlesName(char* strName)
{
	strcpy( m_strName , strName );
}
void CDymMagicParticles::SetParticlesAcceptID( CDymMagicBaseObj *pObj )
{
	m_pAcceptObj = pObj;
}

void CDymMagicParticles::FrameMove(float fCurTime , float fElapsedTime , DYM_WORLD_MATRIX world , BOOL bAddObj)
{
	m_fAddTime += fElapsedTime;
	FrameMoveBase( fCurTime , fElapsedTime );
	for( std::list< CDymMagicParticlesObj* >::iterator it = m_listObj.begin () ; it != m_listObj.end () ; it++ )
	{
		CDymMagicParticlesObj *pObj = *it;
		if( this->iswithfather )
		{
			if( this->m_pAcceptObj )
				pObj->m_vAcceptPos = (*m_pAcceptObj->GetPos ())-(*GetPos());
		}
		if( pObj->isOverTime ( m_fAddTime ) )
		{
			if(it != m_listObj.begin ())
			{
				it--;
				m_listObj.remove ( pObj );
				delete pObj;
			}
		}
		else
			pObj->FrameMove ( fElapsedTime );
	}
	// Add object for particles.
	if( bAddObj )
	{
		float rate = emission_rate;
		if(rate<10.0f)rate=10.0f;
		if( !m_bUseRate )
		{
			if( m_fAddTime-fElapsedTime-m_fPlayTimeForAddObj > emission_time )return;
			if( fElapsedTime > emission_time )fElapsedTime=emission_time;
			if( emission_num != 0 && emission_time != 0.0f )
				rate = 1000.0f/((float)(emission_num+1)/(emission_time/1000.0f));
			else
				rate = 1000000.0f;
		}
		float at;
		if( m_nVersion == 2 )
			at = rate;
		else
			at = 1000.0f / rate ;
		m_fAddTimeLast += fElapsedTime;
		CDymMagicKeyFrame *pKeyFrame = m_KeyFrameManager.GetKeyFrameNow ();
		while ( m_fAddTimeLast > at )
		{
			m_fAddTimeLast -= at ;
			BOOL bAdd = TRUE;
			if( pKeyFrame )
				if( !pKeyFrame->animate )
					bAdd = FALSE;
			if( bAdd )
			{
				D3DXMATRIX *mat = NULL;
				D3DXMATRIX m;
				CDymMagicKeyFrame *pKey = NULL;
				if( !this->iswithfather )
				{
					ComputeWorldMatrix( world );
					m = GetWorld1 ();
					mat = &m;
					pKey = pKeyFrame;
				}
				AddObj( m_fAddTime , m_fAddTime - m_fAddTimeLast , mat , pKeyFrame , 0.0f , &world );
			}
		}
	}
}

float CDymMagicParticles::RandF()
{
	/*if( m_bTrackRand == 1 )m_nTrackRandNum++;
	else if( m_bTrackRand == 2 )
	{
		int step2 = (int)powf( (float)3 , (float)m_nTrackRandNum );
		float f = m_pTrackRandArray[ m_nTrackRandNum2%step2 ];
		m_nTrackRandNum2++;
		return f;
	}*/
	return (float)rand()/RAND_MAX;
}

void CDymMagicParticles::AddObj(float fCurTime , float CreateTime , D3DXMATRIX *pworldnow , CDymMagicKeyFrame *pKey , float LongTime , DYM_WORLD_MATRIX *pDymWorld )
{
	BOOL bUp = FALSE;
	if( billboard_type == 0 )bUp = TRUE;
	float r1 = RandF() ;
	CDymMagicParticlesObj *pObj = new CDymMagicParticlesObj( 
		particle_width*r1 + (1.0f-r1)*particle_width2 , 
		particle_height*r1 + (1.0f-r1)*particle_height2 , m_pDevice , CreateTime , bUp );
	pObj->SetBillboardType ( billboard_type );
//	if( billboard_type==3 && iswithfather )
//		pObj->SetBillboardType ( -1 );

	D3DXVECTOR3 vWorldPos = D3DXVECTOR3(0,0,0);
	//if( pworldnow ) vWorldPos = D3DXVECTOR3( pworldnow->_41 , pworldnow->_42 , pworldnow->_43 );
	if( particles_type == 0 )
	{
		pObj->m_vPos = D3DXVECTOR3( ( RandF()-0.5f )*ellipsoid_size.x +vWorldPos.x,
			( RandF()-0.5f )*ellipsoid_size.y +vWorldPos.y,
			( RandF()-0.5f )*ellipsoid_size.z +vWorldPos.z);
	}
	else
	{
		float height = ( RandF()-0.5f )*ellipsoid_size.y ;
		float a = RandF()*D3DX_PI*2.0f/180.0f;
		float radio = ellipsoid_size.x/2.0f;
		D3DXVECTOR3 v = D3DXVECTOR3( radio , height , radio );
		D3DXMATRIX matRot;
		D3DXMatrixRotationY( &matRot , a*angle );
		D3DXVECTOR4 v4;
		D3DXVec3Transform( &v4 , &v , &matRot );
		pObj->m_vPos = D3DXVECTOR3( v4.x/v4.w , v4.y/v4.w , v4.z/v4.w );
	}
	D3DXVECTOR3 vSca = m_pDevice->GetScaling ();
	if( pworldnow )
	{
		D3DXVECTOR4 vPos4;
		D3DXVec3Transform( &vPos4 , &pObj->m_vPos , pworldnow );
		pObj->m_vPos = D3DXVECTOR3( vPos4.x / vPos4.w , vPos4.y / vPos4.w , vPos4.z / vPos4.w );
	}
	pObj->time_to_live = RandF()*(time_to_live_max-time_to_live_min) + time_to_live_min + LongTime/1000.0f ;
	pObj->colour  = D3DXVECTOR4( RandF()*(colour_range_end.x -colour_range_start.x) + colour_range_start.x , 
								 RandF()*(colour_range_end.y -colour_range_start.y) + colour_range_start.y ,
								 RandF()*(colour_range_end.z -colour_range_start.z) + colour_range_start.z ,
								 RandF()*(colour_range_end.w -colour_range_start.w) + colour_range_start.w );
	pObj->force_type = force_type;
	pObj->accept_power = accept_power;
	pObj->force_add_type = force_add_type;
	if( pworldnow && pKey )
	{
		pObj->colour.x *= pKey->colour.x;
		pObj->colour.y *= pKey->colour.y;
		pObj->colour.z *= pKey->colour.z;
		pObj->colour.w *= pKey->colour.w;
	}
	pObj->velocity = RandF()*(velocity_max -velocity_min) + velocity_min ;
	pObj->force_vector = force_vector ;
	pObj->colour_end = ColourFader ;
	pObj->scale_add_x = scale_add_x;
	pObj->scale_add_y = scale_add_y;
	pObj->rotate_add = rotate_add;
	pObj->iswithfather = iswithfather;
	pObj->limit_vector = limit_vector;
	
	// 计算随机角度与方向
	/*D3DXVECTOR3 vDirRand = D3DXVECTOR3( RandF() * 2.0f - 1.0f , 
		RandF() * 2.0f - 1.0f , RandF() * 2.0f - 1.0f );
	D3DXMATRIX matrix;
	D3DXMatrixRotationAxis( &matrix , &vDirRand , RandF()*angle/180.0f*D3DX_PI );*/
	if( particles_type == 0 )
	{
		if( limit_vector == 0 )
		{
			D3DXMATRIX mx,my,mz;
			D3DXMatrixRotationX( &mx , (RandF()*2.0f-1.0f)*angle/360.0f*D3DX_PI );
			D3DXMatrixRotationY( &my , (RandF()*2.0f-1.0f)*angle/360.0f*D3DX_PI );
			D3DXMatrixRotationZ( &mz , (RandF()*2.0f-1.0f)*angle/360.0f*D3DX_PI );
			mx = mx*my;
			D3DXVECTOR4 v4;
			D3DXVec3Transform( &v4 , &direction , &mx );
			pObj->direction = D3DXVECTOR3( v4.x/v4.w , v4.y/v4.w , v4.z/v4.w );
		}
		else
		{
			D3DXMATRIX mx,my;
			D3DXMatrixRotationAxis( &mx , &D3DXVECTOR3( direction.y , direction.z , direction.x ) , angle/360.0f*D3DX_PI );
			D3DXMatrixRotationAxis( &my , &direction , (RandF()*2.0f-1.0f)*D3DX_PI*2.0f );
			mx = mx*my;
			D3DXVECTOR4 v4;
			D3DXVec3Transform( &v4 , &direction , &mx );
			pObj->direction = D3DXVECTOR3( v4.x/v4.w , v4.y/v4.w , v4.z/v4.w );
		}
	}
	else if( particles_type == 1 )
		pObj->direction = direction;
	/*if( pKey )
	{
		pObj->direction.x *= pKey->scale.x ;
		pObj->direction.y *= pKey->scale.y ;
		pObj->direction.z *= pKey->scale.z ;
	}*/
	if( pworldnow )
	{
		/*pObj->direction.x *= vSca.x ;
		pObj->direction.y *= vSca.y ;
		pObj->direction.z *= vSca.z ;*/
		pObj->force_vector.x *= vSca.x ;
		pObj->force_vector.y *= vSca.y ;
		pObj->force_vector.z *= vSca.z ;

		D3DXVECTOR3 vDir = pObj->direction;
		D3DXVECTOR4 vDir4;
		D3DXMATRIX rotmul,rotmul2;
		D3DXMatrixRotationAxis( &rotmul , &pDymWorld->vAxis , pDymWorld->fAngle );
		if( pworldnow && pKey )
		{
			D3DXQUATERNION qm = pKey->GetQuaternion ();
			D3DXMatrixRotationQuaternion( &rotmul2 , &qm );
			rotmul = rotmul * rotmul2;
		}
		D3DXVec3Transform( &vDir4 , &vDir , &rotmul );
		vDir = D3DXVECTOR3( vDir4.x / vDir4.w , vDir4.y / vDir4.w , vDir4.z / vDir4.w );
		/*D3DXVec3Normalize( &vDir , &vDir );
		pObj->direction.x *= vDir.x;
		pObj->direction.y *= vDir.y;
		pObj->direction.z *= vDir.z;*/
		pObj->direction = vDir;
	}
	if( billboard_type == 3 && ( pObj->direction.x != 0 || pObj->direction.z !=0 ) )//设置旋转
	{
		D3DXMATRIX m_matBillboard2,m_matBillboard;
		D3DXVECTOR3 vLookatPt = pObj->direction;
		D3DXVECTOR3 vEyePt = D3DXVECTOR3(0,0,0);
		D3DXVECTOR3 vUpVec = D3DXVECTOR3(0,1,0);
		D3DXVECTOR3 vNormal(0,0,1);
		D3DXVECTOR3 vDir = vLookatPt - vEyePt ;	
		D3DXVec3Normalize(&vUpVec,&vUpVec);
		D3DXVec3Normalize(&vDir,&vDir);
		D3DXVECTOR3 ttt=D3DXVECTOR3(vDir.x,0.0001f,vDir.z);
		D3DXVec3Normalize(&ttt,&ttt);
		float ft2=D3DX_PI;
		if( vLookatPt.x >= vEyePt.x )ft2=0;
		D3DXMatrixRotationY( &m_matBillboard2, -atanf(vDir.z/vDir.x)+D3DX_PI/2+ft2 );
		//return S_OK;

		float tt=D3DXVec3Dot(&ttt,&vNormal);
		float ft=0.0f;
		if(vUpVec.y<0.0f)ft=D3DX_PI;
		if( vLookatPt.x > vEyePt.x )
			D3DXMatrixRotationY(&m_matBillboard,acosf(tt)+ft);
		else
			D3DXMatrixRotationY(&m_matBillboard,-acosf(tt)+ft);

		D3DXVECTOR3 t_temp,t2;
		D3DXVec3Cross(&t_temp,&vUpVec,&vDir);
		D3DXVec3Normalize(&t_temp,&t_temp);
		D3DXVec3Cross(&t2,&vDir,&t_temp);
		D3DXVec3Normalize(&t2,&t2);

		float f=D3DXVec3Dot(&t2,&D3DXVECTOR3(0,1,0)); 
		D3DXMATRIX mx;
		if( vLookatPt.y > vEyePt.y)
			D3DXMatrixRotationAxis( &mx, &t_temp ,-acosf(f));
		else
			D3DXMatrixRotationAxis( &mx, &t_temp ,acosf(f));
		D3DXMatrixMultiply( &m_matBillboard  , &m_matBillboard, &mx );

		D3DXVECTOR3 vCross;
		D3DXVec3Cross( &vCross , &vLookatPt , &vUpVec );
		D3DXMATRIX mul;
		D3DXMatrixRotationAxis( &mul , &vCross , D3DX_PI/2.0f );
		//return S_OK;
		pObj->SetRotMatrix ( m_matBillboard*mul );
	}
	if( billboard_type == 3 && iswithfather )
	{
		D3DXMATRIX rotmul;
		if( pKey )
		{
			D3DXMatrixRotationQuaternion( &rotmul , &pKey->GetQuaternion () );
			pObj->SetRotMatrix ( rotmul );
		}
	}
	if( billboard_type == 4 )
	{
		D3DXMATRIX r1,r2;
		D3DXVECTOR3 vCross;
		D3DXVec3Cross(&vCross , &pObj->direction , &D3DXVECTOR3( 0 , 1 , 0 ) );
		D3DXMatrixRotationAxis( &r1 , &vCross , -acosf( D3DXVec3Dot( &pObj->direction , &D3DXVECTOR3( 0 , 1 , 0 ) ) ) );
		pObj->SetRotMatrix ( r1 );
	}
	if( pworldnow )
	{
		if( this->m_pAcceptObj )
			pObj->m_vAcceptPos = (*m_pAcceptObj->GetPos ());
	}

	if( pObj->isOverTime ( fCurTime ) )
		delete pObj;
	else
	{
		pObj->FrameMove ( fCurTime-CreateTime );
		m_listObj.push_front ( pObj );
	}
}

D3DXVECTOR3* CDymMagicParticles::GetVertexStreamZeroData(int *pfacenum,int *pStride)
{
	//   0   1
	//   /---/|
	// 2/---/3|
	//  | 6 | /7
	// 4|---|/5
#define V_TO_TRI(n,va,vb,vc) p[n*3+0] = va;p[n*3+1] = vb;p[n*3+2] = vc;
	D3DXVECTOR3 v0 = D3DXVECTOR3( -ellipsoid_size.x/2.0f ,  ellipsoid_size.y/2.0f ,  ellipsoid_size.z/2.0f );
	D3DXVECTOR3 v1 = D3DXVECTOR3(  ellipsoid_size.x/2.0f ,  ellipsoid_size.y/2.0f ,  ellipsoid_size.z/2.0f );
	D3DXVECTOR3 v2 = D3DXVECTOR3( -ellipsoid_size.x/2.0f ,  ellipsoid_size.y/2.0f , -ellipsoid_size.z/2.0f );
	D3DXVECTOR3 v3 = D3DXVECTOR3(  ellipsoid_size.x/2.0f ,  ellipsoid_size.y/2.0f , -ellipsoid_size.z/2.0f );
	D3DXVECTOR3 v4 = D3DXVECTOR3( -ellipsoid_size.x/2.0f , -ellipsoid_size.y/2.0f , -ellipsoid_size.z/2.0f );
	D3DXVECTOR3 v5 = D3DXVECTOR3(  ellipsoid_size.x/2.0f , -ellipsoid_size.y/2.0f , -ellipsoid_size.z/2.0f );
	D3DXVECTOR3 v6 = D3DXVECTOR3( -ellipsoid_size.x/2.0f , -ellipsoid_size.y/2.0f ,  ellipsoid_size.z/2.0f );
	D3DXVECTOR3 v7 = D3DXVECTOR3(  ellipsoid_size.x/2.0f , -ellipsoid_size.y/2.0f ,  ellipsoid_size.z/2.0f );
	D3DXVECTOR3 *p = new D3DXVECTOR3[12*3];
	V_TO_TRI( 0 , v0 , v1 , v2 );
	V_TO_TRI( 1 , v2 , v1 , v3 );
	V_TO_TRI( 2 , v4 , v2 , v3 );
	V_TO_TRI( 3 , v4 , v3 , v5 );
	V_TO_TRI( 4 , v5 , v3 , v1 );
	V_TO_TRI( 5 , v5 , v1 , v7 );
	V_TO_TRI( 6 , v7 , v1 , v0 );
	V_TO_TRI( 7 , v7 , v0 , v6 );
	V_TO_TRI( 8 , v6 , v0 , v2 );
	V_TO_TRI( 9 , v6 , v2 , v4 );
	V_TO_TRI( 10 , v5 , v7 , v6 );
	V_TO_TRI( 11 , v5 , v6 , v4 );
	*pStride = sizeof( D3DXVECTOR3 );
	*pfacenum = 12;
	return p;
}

void CDymMagicParticles::CopyParticles( CDymMagicParticles *p )
{
	_tcscpy( m_strName , p->m_strName );
	_tcscpy( materialres , p->materialres );
	quota = p->quota ;
	particle_width = p->particle_width ;
	particle_height = p->particle_height ;
	particle_width2 = p->particle_width2 ;
	particle_height2 = p->particle_height2 ;
	cull_each = p->cull_each ;
	ishav_qua = p->ishav_qua ;
	billboard_type = p->billboard_type ;
	force_vector = p->force_vector ;
	ellipsoid_size = p->ellipsoid_size ;
	time_to_live_min = p->time_to_live_min ;
	time_to_live_max = p->time_to_live_max ;
	colour_range_start = p->colour_range_start ;
	colour_range_end = p->colour_range_end ;
	velocity_min = p->velocity_min ;
	velocity_max = p->velocity_max ;
	direction = p->direction ;
	angle = p->angle ;
	israndangle = p->israndangle ;
	isonlydirection = p->isonlydirection ;
	emission_rate = p->emission_rate ;
	emission_num = p->emission_num ;
	emission_time = p->emission_time ;
	ColourFader = p->ColourFader ;
	scale_add_x = p->scale_add_x ;
	scale_add_y = p->scale_add_y ;
	rotate_add = p->rotate_add ;
	iswithfather = p->iswithfather ;
	force_type = p->force_type ;
	accept_power = p->accept_power ;
	m_fAddTimeLast = p->m_fAddTimeLast ;
	m_fAddTime = p->m_fAddTime ;
	m_fPlayTimeForAddObj = p->m_fPlayTimeForAddObj;
	m_nVersion = p->m_nVersion ;
	force_add_type = p->force_add_type ;
	m_vBoundingBox1 = p->m_vBoundingBox1 ;
	m_vBoundingBox2 = p->m_vBoundingBox2 ;
	accept_id = p->accept_id ;
	m_pAcceptObj = p->m_pAcceptObj ;
	particles_type = p->particles_type;

	CDymMagicBaseObj::CopyBaseObj ( p );
}

void CDymMagicParticles::_ComputeBoundingBox_AddObj( CDymMagicKeyFrame *pKeyFrame , DYM_WORLD_MATRIX world )
{
	D3DXMATRIX *mat = NULL;
	CDymMagicKeyFrame *pKey = NULL;
	if( !this->iswithfather )
	{
		ComputeWorldMatrix( world );
		mat = &GetWorld1 ();
		pKey = pKeyFrame;
	}
	AddObj( m_fAddTime , m_fAddTime , mat , pKey , 100.0f , &world );
}
void CDymMagicParticles::_ComputeBoundingBox( CDymMagicKeyFrame *pKeyFrame , int addnum , DYM_WORLD_MATRIX world )
{
	float fCurTime = 0.0f;
	if( pKeyFrame )fCurTime = pKeyFrame->m_fTime ;
	FrameMove( fCurTime , 100.0f , world , FALSE );
	for( int i=0;i<addnum;i++ )
		_ComputeBoundingBox_AddObj( pKeyFrame , world );
	Render( world , 0 );
	while( m_listObj.size () > 0 )
	{
		FrameMove( 0 , 100.0f , world , FALSE );
		Render( world , 0 );
	}
}
void CDymMagicParticles::GetBoundingBoxRealTime( D3DXVECTOR3 *pMin , D3DXVECTOR3 *pMax )
{
	if( iswithfather )
	{
		*pMin = m_vBoundingBox1 ;
		*pMax = m_vBoundingBox2 ;
	}
	else
	{
		D3DXVECTOR3 v1 = m_vBoundingBox1 ;
		D3DXVECTOR3 v2 = m_vBoundingBox2 ;
		for( std::list< CDymMagicParticlesObj* >::iterator it = m_listObj.begin () ; it != m_listObj.end () ; it++)
		{
			CDymMagicParticlesObj *pObj = *it;
			D3DXVECTOR3 vCenter;
			float fRadio;
			if( pObj->GetBoundingSphere( &vCenter , &fRadio ) )
			{
				if( v1.x > vCenter.x-fRadio )
					v1.x = vCenter.x-fRadio;
				if( v1.y > vCenter.y-fRadio )
					v1.y = vCenter.y-fRadio;
				if( v1.z > vCenter.z-fRadio )
					v1.z = vCenter.z-fRadio;
				if( v2.x < vCenter.x+fRadio )
					v2.x = vCenter.x+fRadio;
				if( v2.y < vCenter.y+fRadio )
					v2.y = vCenter.y+fRadio;
				if( v2.z < vCenter.z+fRadio )
					v2.z = vCenter.z+fRadio;
			}
		}
		*pMin = v1;
		*pMax = v2;
	}
}
void CDymMagicParticles::ComputeBoundingBox( DYM_WORLD_MATRIX world )
{
	// 备份
	float fAddTimeOld = m_fAddTime;
	std::list< CDymMagicParticlesObj* > listObjOld;
	for( std::list< CDymMagicParticlesObj* >::iterator it = m_listObj.begin () ; it != m_listObj.end () ; it++)
		listObjOld.push_back ( *it );
	m_listObj.clear ();

	m_vBoundingBox1 = D3DXVECTOR3( 100000 , 100000 , 100000 );
	m_vBoundingBox2 = D3DXVECTOR3( -100000 , -100000 , -100000 );

	// 跟踪调用了几次rand
	m_bTrackRand = 1;
	_ComputeBoundingBox_AddObj( NULL , world );
	for( std::list< CDymMagicParticlesObj* >::iterator it = m_listObj.begin () ; it != m_listObj.end () ; it++ )
	{
		CDymMagicParticlesObj *pObj = *it;
		delete pObj;
	}
	m_listObj.clear ();
	//m_nTrackRandNum;//这个是调用了几次rand
//	if( m_nTrackRandNum <=0 )goto END;

	m_pDevice->TrackBoundingSphere ( TRUE );

/*	// 开始特定的随机数
	m_bTrackRand = 2;
	m_nTrackRandNum2 = 0;
	int step = 3;//从0到1之间共计算3次
	int step2 = (int)powf( (float)step , (float)m_nTrackRandNum );
	step2 *= m_nTrackRandNum;
	m_pTrackRandArray = new float[ step2 ];
	char s[512];
	ZeroMemory( s , sizeof(char)*512 );
	for( int i=0;i<step2;i+=m_nTrackRandNum)
	{
		for( int j=0;j<m_nTrackRandNum;j++ )
		{
			m_pTrackRandArray[ i+j ] = (float)s[j]/(float)(step-1);
		}
		s[0]++;
		int k=0;
		while( s[k]>step-1 )
		{
			s[k] = 0;
			s[k+1]++;
			k++;
		}
		if(s[10]!=0)
			rand();
	}*/
	
	// 采样100个特效
	_ComputeBoundingBox( NULL , 100 , world );//step2/m_nTrackRandNum );
	m_nTrackRandNum2 = 0;

	int keynum = m_KeyFrameManager.m_listKeyFrame.size ();
	if( keynum > 0 )
	{
		float *ptime = new float[ (keynum-1)*2+1 ];
		int kk = 0;

		CDymMagicKeyFrame *pKeyOld = NULL;
		for( std::list< CDymMagicKeyFrame* >::iterator it = m_KeyFrameManager.m_listKeyFrame.begin () ;
			it != m_KeyFrameManager.m_listKeyFrame.end () ; it ++ )
		{
			CDymMagicKeyFrame *pKeyFrame = *it;
			if( pKeyOld == NULL )
			{
				ptime[kk] = pKeyFrame->m_fTime ;
				kk++;
			}
			else
			{
				ptime[kk] = (pKeyOld->m_fTime + pKeyFrame->m_fTime )/2.0f;
				ptime[kk+1] = pKeyFrame->m_fTime ;
				kk+=2;
			}
			pKeyOld = pKeyFrame;
		}

		for( int i = 0 ; i < kk ; i ++ )
		{
			CDymMagicKeyFrame *pKeyFrame = m_KeyFrameManager.GetKeyFrame ( ptime[i] );
			_ComputeBoundingBox( pKeyFrame , 100 , world );//step2/m_nTrackRandNum);
			m_nTrackRandNum2 = 0;
		}
		delete[]ptime;
	}
	
	/*
	for( std::list< CDymMagicKeyFrame* >::iterator it = m_KeyFrameManager.m_listKeyFrame.begin () ;
		it != m_KeyFrameManager.m_listKeyFrame.end () ; it ++ )
	{
		CDymMagicKeyFrame *pKeyFrame = *it;
		_ComputeBoundingBox( pKeyFrame , 1000 );//step2/m_nTrackRandNum);
		m_nTrackRandNum2 = 0;
	}*/
	m_pDevice->TrackBoundingSphere ( FALSE );
	
	for( std::list< CDymRenderSystem::BOUNDING_SPHERE * >::iterator it = m_pDevice->m_listBoundingSphere.begin ();
		it != m_pDevice->m_listBoundingSphere.end () ; it++ )
	{
		CDymRenderSystem::BOUNDING_SPHERE * pBoundingSphere = *it;
		if( m_vBoundingBox1.x > pBoundingSphere->vCenter.x-pBoundingSphere->fRadio )
			m_vBoundingBox1.x = pBoundingSphere->vCenter.x-pBoundingSphere->fRadio;
		if( m_vBoundingBox1.y > pBoundingSphere->vCenter.y-pBoundingSphere->fRadio )
			m_vBoundingBox1.y = pBoundingSphere->vCenter.y-pBoundingSphere->fRadio;
		if( m_vBoundingBox1.z > pBoundingSphere->vCenter.z-pBoundingSphere->fRadio )
			m_vBoundingBox1.z = pBoundingSphere->vCenter.z-pBoundingSphere->fRadio;
		if( m_vBoundingBox2.x < pBoundingSphere->vCenter.x+pBoundingSphere->fRadio )
			m_vBoundingBox2.x = pBoundingSphere->vCenter.x+pBoundingSphere->fRadio;
		if( m_vBoundingBox2.y < pBoundingSphere->vCenter.y+pBoundingSphere->fRadio )
			m_vBoundingBox2.y = pBoundingSphere->vCenter.y+pBoundingSphere->fRadio;
		if( m_vBoundingBox2.z < pBoundingSphere->vCenter.z+pBoundingSphere->fRadio )
			m_vBoundingBox2.z = pBoundingSphere->vCenter.z+pBoundingSphere->fRadio;
	}
	m_pDevice->CleraBoundingSphere ();

	// 恢复备份
	m_fAddTime = fAddTimeOld;
	for( std::list< CDymMagicParticlesObj* >::iterator it = listObjOld.begin () ; it != listObjOld.end () ; it++)
		m_listObj.push_back ( *it );
	listObjOld.clear ();
//	delete[]m_pTrackRandArray;
//	m_pTrackRandArray = NULL;
//	m_nTrackRandNum = 0;
//	m_nTrackRandNum2 = 0;
//	m_bTrackRand = 0;
}

HRESULT CDymMagicParticles::Render(DYM_WORLD_MATRIX world , DWORD flag)
{
	if( flag == MAGIC_RENDER_SHADOW )return S_OK;
	CDymMagicKeyFrame *pKeyFrame = m_KeyFrameManager.GetKeyFrameNow ();
	if( pKeyFrame && this->iswithfather )
		if( pKeyFrame->animate == false && !(flag&MAGIC_RENDER_NO_HIDE) )return S_OK;

	if( !(flag&MAGIC_RENDER_DO_NOT_SETUP_RENDERSTATE) )
	{
		if( iswithfather )
			SetupRenderState( world );
		else
		{
			SetupRenderState( world );
			m_pDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xFFFFFFFF );
			D3DXMATRIX mat;
			D3DXMatrixIdentity( &mat );
			m_pDevice->SetTransform ( D3DTS_WORLD , &mat );
		}
	}
	if( !(flag&MAGIC_RENDER_SELECTION) )
	{
		for( std::list< CDymMagicParticlesObj* >::iterator it = m_listObj.begin () ; it != m_listObj.end () ; it++ )
		{
			CDymMagicParticlesObj *pObj = *it;
			D3DXVECTOR3 vSavedSca = m_pDevice->GetScaling ();
			//D3DXMATRIX mat = pObj->m_matWorld * matWorld ;
			/*if( !iswithfather )
				m_pDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	D3DCOLOR_COLORVALUE( pObj->colour.x , 
					pObj->colour.y, pObj->colour.z , pObj->colour.w ));*/
			/*if( pKeyFrame )
				m_pDevice->SetScaling ( pKeyFrame->scale );
			else
				m_pDevice->SetScaling ( D3DXVECTOR3( 1 , 1 , 1 ) );
			if( !pKeyFrame )
				m_pDevice->SetScaling ( D3DXVECTOR3( 1 , 1 , 1 ) );*/
			pObj->Render();
			m_pDevice->SetScaling ( vSavedSca );
		}
	}
	else
	{
		if( particles_type == 0 )
		{
			D3DXVECTOR3 b1 = D3DXVECTOR3( -ellipsoid_size.x/2.0f , -ellipsoid_size.y/2.0f , -ellipsoid_size.z/2.0f );
			D3DXVECTOR3 b2 = D3DXVECTOR3( ellipsoid_size.x/2.0f , ellipsoid_size.y/2.0f , ellipsoid_size.z/2.0f );
			m_pDevice->DrawLineBox ( b1 , b2 , 0xFFFFFFFF );
		}
		else if( particles_type == 1 )
		{
			D3DXVECTOR3 b1 = D3DXVECTOR3( -ellipsoid_size.x/2.0f , -ellipsoid_size.y/2.0f , -ellipsoid_size.z/2.0f );
			D3DXVECTOR3 b2 = D3DXVECTOR3( -ellipsoid_size.x/2.0f , ellipsoid_size.y/2.0f , -ellipsoid_size.z/2.0f );
			D3DXMATRIX matRot;
			D3DXMatrixRotationY( &matRot , D3DX_PI*2.0f/32.0f );
			for( int i=0;i<32;i++)
			{
				D3DXVECTOR3 v1,v2;
				D3DXVECTOR4 v4;
				D3DXVec3Transform( &v4 , &b1 , &matRot );
				v1 = D3DXVECTOR3( v4.x/v4.w , v4.y/v4.w , v4.z/v4.w );
				m_pDevice->DrawLine ( b1 , v1 , 0xFFFFFFFF );
				D3DXVec3Transform( &v4 , &b2 , &matRot );
				v2 = D3DXVECTOR3( v4.x/v4.w , v4.y/v4.w , v4.z/v4.w );
				m_pDevice->DrawLine ( b2 , v2 , 0xFFFFFFFF );
				if( i%4==0 )
				{
					m_pDevice->DrawLine ( b1 , b2 , 0xFFFFFFFF );
				}
				b1 = v1;
				b2 = v2;
			}
		}
	}
	return S_OK;
}

HRESULT CDymMagicParticles::init(void)
{
	m_pDevice->CreateTextureFromFile ( materialres , &m_pTex );
	return S_OK;
}

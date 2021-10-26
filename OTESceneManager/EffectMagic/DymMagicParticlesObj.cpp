/***************************************************************************************************
文件名称:	DymMagicParticlesObj.cpp
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	粒子子物体
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#include ".\dymmagicparticlesobj.h"

CDymMagicParticlesObj::CDymMagicParticlesObj( float fWidth , float fHeight , CDymRenderSystem *pDevice , float create_time , BOOL bUp )
{
	fCreateTime = create_time;
	m_fTimeLiftTime = 0;
	m_pDevice = pDevice ;
	D3DXMatrixTranslation( &m_matWorld , 0 , 0 , 0 );
	m_vForceAdd = D3DXVECTOR3( 0 , 0 , 0 );
	m_pBillboard = new CDymMagicBillboard( pDevice );
	D3DXVECTOR2 v1 = D3DXVECTOR2(-fWidth/2.0f ,-fHeight/2.0f );
	D3DXVECTOR2 v2 = D3DXVECTOR2( fWidth/2.0f , fHeight/2.0f );
	m_pBillboard->init ( v1 , v2 , bUp );
	m_vSca = D3DXVECTOR2( 1.0f , 1.0f );
	m_fRot = 0.0f;

	m_vPos = D3DXVECTOR3( 0 , 0 , 0 );
	m_vPos2 = D3DXVECTOR3( 0 , 0 , 0 );
	m_vAcceptPos = D3DXVECTOR3( 0 , 0 , 0 );
}

CDymMagicParticlesObj::~CDymMagicParticlesObj(void)
{
	delete m_pBillboard;
}

void CDymMagicParticlesObj::SetBillboardType( int nType )
{
	return m_pBillboard->SetBillboardType ( nType );
}

void CDymMagicParticlesObj::SetRotMatrix( D3DXMATRIX matRot )
{
	return m_pBillboard->SetRotationMatrix ( matRot );
}

void CDymMagicParticlesObj::FrameMove( float fTime )
{
	// 力方向
	if( force_type == 0 )
	{
		if( force_add_type == 0 )
		{
			m_vForceAdd.x += fTime/1000.0f ;
			m_vPos2.x =  0.5f * m_vForceAdd.x * m_vForceAdd.x * force_vector.x ;
			m_vPos2.y =  0.5f * m_vForceAdd.x * m_vForceAdd.x * force_vector.y ;
			m_vPos2.z =  0.5f * m_vForceAdd.x * m_vForceAdd.x * force_vector.z ;
		}
		else if( force_add_type == 1 )
			m_vForceAdd = force_vector /100.0f / 2.0f ;
	}
	else if( force_type == 1 )//向中心吸引
	{
		D3DXVECTOR3 v;
		D3DXVec3Normalize( &v , &(m_vPos - m_vAcceptPos) );
		if( force_add_type == 0 )
		{
			//m_vForceAdd -= v / 100000.0f * accept_power;
			m_vForceAdd.x += fTime/1000.0f ;
			m_vPos2.x =  -0.5f * m_vForceAdd.x * m_vForceAdd.x * v.x * accept_power;
			m_vPos2.y =  -0.5f * m_vForceAdd.x * m_vForceAdd.x * v.y * accept_power;
			m_vPos2.z =  -0.5f * m_vForceAdd.x * m_vForceAdd.x * v.z * accept_power;
		}
		else if( force_add_type == 1 )
			m_vForceAdd = -v / 1000.0f * accept_power;
	}
	if( force_add_type == 1 )
		m_vPos += m_vForceAdd* fTime;
	m_vPos += direction * ( velocity/1000.0f * fTime / 2.0f );
	colour += colour_end/1000.0f*fTime;
	if( colour.x >1.0f )colour.x = 1.0f;
	if( colour.y >1.0f )colour.y = 1.0f;
	if( colour.z >1.0f )colour.z = 1.0f;
	if( colour.w >1.0f )colour.w = 1.0f;
	if( colour.x <0.0f )colour.x = 0.0f;
	if( colour.y <0.0f )colour.y = 0.0f;
	if( colour.z <0.0f )colour.z = 0.0f;
	if( colour.w <0.0f )colour.w = 0.0f;
	m_fTimeLiftTime += fTime;
	m_vSca.x += scale_add_x/10000.0f*fTime;
	m_vSca.y += scale_add_y/10000.0f*fTime;
	m_fRot += rotate_add/1000.0f*fTime;

	/*D3DXMATRIX mat;
	D3DXMatrixTranslation( &mat , m_vForceAdd.x , m_vForceAdd.y , m_vForceAdd.z );
	m_matWorld = m_matWorld * mat ;*/
}

BOOL CDymMagicParticlesObj::GetBoundingSphere( D3DXVECTOR3 *pCenter , float *pRadio )
{
	if( m_pBillboard == NULL )return FALSE;
	D3DXVECTOR3 vCenter = D3DXVECTOR3( 0 , 0 , 0 );
	float radio = 0.0f;
	m_pBillboard->GetBoundingSphere( &vCenter , &radio );
	vCenter += m_vPos+m_vPos2;
	radio *= max( m_vSca.x , m_vSca.y );
	*pCenter = vCenter;
	*pRadio = radio;
	return TRUE;
}
void CDymMagicParticlesObj::Render(void)
{
	// Get obj position
	D3DXMATRIX world,worldSaved;
	m_pDevice->GetTransform ( D3DTS_WORLD , &worldSaved );
	world = worldSaved;

	D3DXMATRIX pos;
	D3DXMatrixTranslation( &pos , m_vPos.x+m_vPos2.x , m_vPos.y+m_vPos2.y , m_vPos.z+m_vPos2.z );
	D3DXMATRIX sca;
	D3DXMatrixScaling( &sca , m_vSca.x , m_vSca.y , 1.0f );

	// Get color
	D3DXVECTOR4 cnow ;
	DWORD dwColor;
	m_pDevice->GetRenderState ( D3DRS_TEXTUREFACTOR , &dwColor );
	D3DXVECTOR4 vColor ;
	vColor.x = (float)( (dwColor&0x00FF0000) >> 16 )/255.0f * colour.x;
	vColor.y = (float)( (dwColor&0x0000FF00) >> 8 )/255.0f * colour.y;
	vColor.z = (float)( (dwColor&0x000000FF) >> 0 )/255.0f * colour.z;
	vColor.w = (float)( (dwColor&0xFF000000) >> 24 )/255.0f * colour.w;
	m_pDevice->SetRenderState ( D3DRS_TEXTUREFACTOR , D3DCOLOR_COLORVALUE( vColor.x , 
		vColor.y, vColor.z , vColor.w ) );

	// rotation
	m_pDevice->SetRotation ( m_fRot*D3DX_PI*2.0f/360.0f );
	// scaling
	D3DXVECTOR3 vSGet = m_pDevice->GetScaling ();
	m_pDevice->SetScaling ( D3DXVECTOR3( m_vSca.x*vSGet.x , m_vSca.y*vSGet.y , 1.0f ) );
	// set
	//m_pDevice->SetTransform ( D3DTS_WORLD , &(sca*pos*world*m_matWorld) );
	m_pDevice->SetTransform ( D3DTS_WORLD , &(sca*pos*world) );
	m_pBillboard->Render ();
	m_pDevice->SetTransform ( D3DTS_WORLD , &worldSaved );
	m_pDevice->SetRenderState ( D3DRS_TEXTUREFACTOR , dwColor );
}

BOOL CDymMagicParticlesObj::isOverTime(float fTime)
{
	if( this->colour .w <=0.0f && this->colour_end.w<0.0f )return TRUE;
	if( force_type == 1 )//向中心吸引
	{
		if( force_add_type == 1 )
		{
			D3DXVECTOR3 v;
			D3DXVec3Normalize( &v , &(m_vPos - m_vAcceptPos) );
			float f = D3DXVec3Length( &(-v / 1000.0f * accept_power * 30.0f) ) ;
			if( fabsf( m_vPos.x - m_vAcceptPos.x ) < f && fabsf( m_vPos.y - m_vAcceptPos.y ) < f
				&& fabsf( m_vPos.z - m_vAcceptPos.z ) < f )
				return TRUE;
		}
	}
	return ( fTime > fCreateTime + time_to_live*1000.0f );
}

/***************************************************************************************************
文件名称:	DymMagicKeyFrame.cpp
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	关键帧
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#include ".\dymmagickeyframe.h"

CDymMagicKeyFrame::CDymMagicKeyFrame(void)
{
	m_fTime = 0;
	position = D3DXVECTOR3( 0 , 0 , 0 );
	animate = true;
	scale = D3DXVECTOR3( 1 , 1 , 1 );
	direction = D3DXVECTOR3( 0 , 0 , 0 );
	spinaxis = D3DXVECTOR3( 0 , 0 , 0 );
	spinangle = 0.0f;
	spinradius = 0.0f;
	colour = D3DXVECTOR4( 1 , 1 , 1 , 1 );
	matresname[0] = 0;	
	soundname[0] = 0;
	matUVtile = D3DXVECTOR2( 1 , 1 );
	scroll_anim = D3DXVECTOR2( 0 , 0 );
	rotate_anim = 0;
	topscale = 1.0f;
	bottomscale = 1.0f;
	animname[0] = 0;	
	animspeed = 0;
	m_pTexture = NULL;

	// for magic 2.0
	nVersion = 1;
	
	D3DXQUATERNION q;
	D3DXQuaternionIdentity( &q );
	rotation2 = D3DXVECTOR4( 0 , 0 , 0 , 0 );
	rotation3 = D3DXVECTOR4( q.x , q.y , q.z , q.w );

	uvOffset2 = D3DXVECTOR2( 0 , 0 );
	uvRotationAngle2 = 0.0f ;
	bUwrap2 = TRUE;
	bVwrap2 = TRUE;

	soundradio = 10.0f;
	bLoopPlay = FALSE;
}

CDymMagicKeyFrame::~CDymMagicKeyFrame(void)
{
	if( m_pTexture )
	{
		if(FAILED(m_pTexture->Release ()))
			delete m_pTexture;
	}
}

CDymMagicKeyFrame& CDymMagicKeyFrame::operator=(const CDymMagicKeyFrame& a)
{
	memcpy( this , &a , sizeof( CDymMagicKeyFrame ) );
	m_pTexture = NULL;
	return *this;
}
float CDymMagicKeyFrame::_GetRotationX( D3DXQUATERNION q )
{
	/*float f = asinf( rotation2.x );
	if( rotation2.w < 0 )
		f = -f;
	if( f<0 )f=(D3DX_PI/2.0f-fabsf(f))+D3DX_PI/2.0f;
	f = f/D3DX_PI*360.0f;
	return f;*/
	float x = q.x;
	float y = q.y;
	float z = q.z;
	float w = q.w;
	float f = atan2f(2*(y*z + w*x), w*w - x*x - y*y + z*z)/D3DX_PI/2.0f*360.0f;
	if( f<0 )f = (180.0f - fabsf(f))+180.0f;
	return f;
}
float CDymMagicKeyFrame::_GetRotationY( D3DXQUATERNION q )
{
	/*float f = asinf( rotation2.y );
	if( rotation2.w < 0 )
		f = -f;
	if( f<0 )f=(D3DX_PI/2.0f-fabsf(f))+D3DX_PI/2.0f;
	f = f/D3DX_PI*360.0f;
	return f;*/
	float x = q.x;
	float y = q.y;
	float z = q.z;
	float w = q.w;
	float f= asinf(-2*(x*z - w*y))/D3DX_PI/2.0f*360.0f;
	if( f<0 )f = (180.0f - fabsf(f))+180.0f;
	return f;
}
float CDymMagicKeyFrame::_GetRotationZ( D3DXQUATERNION q )
{
	/*float f = asinf( rotation2.z );
	if( rotation2.w < 0 )
		f = -f;
	if( f<0 )f=(D3DX_PI/2.0f-fabsf(f))+D3DX_PI/2.0f;
	f = f/D3DX_PI*360.0f;
	return f;*/
	float x = q.x;
	float y = q.y;
	float z = q.z;
	float w = q.w;
	float f= atan2f(2*(x*y + w*z), w*w + x*x - y*y - z*z)/D3DX_PI/2.0f*360.0f;
	if( f<0 )f = (180.0f - fabsf(f))+180.0f;
	return f;
}
float CDymMagicKeyFrame::GetRotationX()
{
	return rotation2.x ;
}
float CDymMagicKeyFrame::GetRotationY()
{
	return rotation2.y ;
}
float CDymMagicKeyFrame::GetRotationZ()
{
	return rotation2.z ;
}
D3DXQUATERNION CDymMagicKeyFrame::GetQuaternion()
{
	D3DXQUATERNION qm;
	D3DXQuaternionRotationYawPitchRoll( &qm ,rotation2.y*D3DX_PI*2.0f/360.0f
				, rotation2.x*D3DX_PI*2.0f/360.0f
				, rotation2.z*D3DX_PI*2.0f/360.0f );
	D3DXQUATERNION q;
	/*D3DXQuaternionRotationYawPitchRoll( &q ,rotation3.y*D3DX_PI*2.0f/360.0f
				, rotation3.x*D3DX_PI*2.0f/360.0f
				, rotation3.z*D3DX_PI*2.0f/360.0f );*/
	q = D3DXQUATERNION( rotation3.x , rotation3.y , rotation3.z , rotation3.w );
	return qm*q;
}
D3DXVECTOR4 CDymMagicKeyFrame::GetRotation()
{
	return rotation2;
}
void CDymMagicKeyFrame::SetRotation( float x , float y , float z )
{
	rotation2 = D3DXVECTOR4( x , y , z , 1.0f );
//	D3DXQUATERNION qm;
//	D3DXQuaternionRotationYawPitchRoll( &qm ,y*D3DX_PI*2.0f/360.0f, x*D3DX_PI*2.0f/360.0f, z*D3DX_PI*2.0f/360.0f );
//	rotation2 = D3DXVECTOR4( qm.x , qm.y , qm.z , qm.w );
}
void CDymMagicKeyFrame::AddRotationX( float volume )
{
	//rotation3.x += volume;
	D3DXQUATERNION q;
	D3DXQuaternionRotationAxis( &q , &D3DXVECTOR3( 1 , 0 , 0 ) , volume *D3DX_PI*2.0f/360.0f );
	D3DXQUATERNION q2 = D3DXQUATERNION( rotation3.x , rotation3.y , rotation3.z , rotation3.w );
	q2 = q2 * q;
	rotation3 = D3DXVECTOR4( q2.x , q2.y , q2.z , q2.w );
/*	rotation2 = D3DXVECTOR4( _GetRotationX( q2 ) , _GetRotationY( q2 ) , _GetRotationZ( q2 ) , 1.0f );*/
	/*D3DXQUATERNION qm = GetQuaternion();
	D3DXQUATERNION q;
	D3DXQuaternionRotationAxis( &q , &D3DXVECTOR3( 1 , 0 , 0 ) , volume *D3DX_PI*2.0f/360.0f );
	D3DXQUATERNION q2 = qm*q;
	rotation2 = D3DXVECTOR4( _GetRotationX( q2 ) , _GetRotationY( q2 ) , _GetRotationZ( q2 ) , 1.0f );*/
}
void CDymMagicKeyFrame::AddRotationY( float volume )
{
	D3DXQUATERNION q;
	D3DXQuaternionRotationAxis( &q , &D3DXVECTOR3( 0 , 1 , 0 ) , volume *D3DX_PI*2.0f/360.0f );
	D3DXQUATERNION q2 = D3DXQUATERNION( rotation3.x , rotation3.y , rotation3.z , rotation3.w );
	q2 = q2 * q;
	rotation3 = D3DXVECTOR4( q2.x , q2.y , q2.z , q2.w );
}
void CDymMagicKeyFrame::AddRotationZ( float volume )
{
	D3DXQUATERNION q;
	D3DXQuaternionRotationAxis( &q , &D3DXVECTOR3( 0 , 0 , 1 ) , volume *D3DX_PI*2.0f/360.0f );
	D3DXQUATERNION q2 = D3DXQUATERNION( rotation3.x , rotation3.y , rotation3.z , rotation3.w );
	q2 = q2 * q;
	rotation3 = D3DXVECTOR4( q2.x , q2.y , q2.z , q2.w );
}
D3DXQUATERNION D3DXQuaternionNlerp(D3DXQUATERNION *pOut, const D3DXQUATERNION& rkP, 
        const D3DXQUATERNION& rkQ, float fT,bool shortestPath)
{
	D3DXQUATERNION result;
    float fCos = D3DXQuaternionDot ( &rkP,&rkQ );
	if (fCos < 0.0f && shortestPath)
	{
		result = rkP + fT * ((-rkQ) - rkP);
	}
	else
	{
		result = rkP + fT * (rkQ - rkP);
	}
    D3DXQuaternionNormalize(&result,&result);
	*pOut = result;
    return result;
}
void CDymMagicKeyFrame::Lerp(const CDymMagicKeyFrame * p1, const CDymMagicKeyFrame * p2, CDymMagicKeyFrame * pKeyOut, float fLerpTime)
{
	float fLerp = ( fLerpTime - p1->m_fTime ) / ( p2->m_fTime - p1->m_fTime );
	if(fLerp<0.0f)fLerp=0.0f;
	else if(fLerp>1.0f)fLerp=1.0f;
	pKeyOut->m_fTime = p2->m_fTime * fLerp + p1->m_fTime * ( 1.0f - fLerp );
	pKeyOut->position = p2->position * fLerp + p1->position * ( 1.0f - fLerp );
	pKeyOut->scale = p2->scale * fLerp + p1->scale * ( 1.0f - fLerp );
	pKeyOut->direction = p2->direction * fLerp + p1->direction * ( 1.0f - fLerp );
	pKeyOut->spinaxis = p2->spinaxis * fLerp + p1->spinaxis * ( 1.0f - fLerp );
	pKeyOut->spinangle = p2->spinangle * fLerp + p1->spinangle * ( 1.0f - fLerp );
	pKeyOut->spinradius = p2->spinradius * fLerp + p1->spinradius * ( 1.0f - fLerp );
	pKeyOut->colour = p2->colour * fLerp + p1->colour * ( 1.0f - fLerp );
	pKeyOut->matUVtile = p2->matUVtile * fLerp + p1->matUVtile * ( 1.0f - fLerp );
	pKeyOut->scroll_anim = p2->scroll_anim * fLerp + p1->scroll_anim * ( 1.0f - fLerp );
	pKeyOut->rotate_anim = p2->rotate_anim * fLerp + p1->rotate_anim * ( 1.0f - fLerp );
	pKeyOut->topscale = p2->topscale * fLerp + p1->topscale * ( 1.0f - fLerp );
	pKeyOut->bottomscale = p2->bottomscale * fLerp + p1->bottomscale * ( 1.0f - fLerp );
	pKeyOut->animspeed = p2->animspeed * fLerp + p1->animspeed * ( 1.0f - fLerp );

	strcpy( pKeyOut->matresname , p1->matresname );
	pKeyOut->animate = p1->animate ;
	pKeyOut->m_pTexture = p1->m_pTexture ;
	strcpy( pKeyOut->animname , p1->animname );

	// for magic 2.0
	pKeyOut->nVersion = p1->nVersion;
	pKeyOut->rotation2 = p2->rotation2 * fLerp + p1->rotation2 * ( 1.0f - fLerp );
	D3DXQUATERNION q, q1 = D3DXQUATERNION( p1->rotation3.x , p1->rotation3.y , p1->rotation3.z , p1->rotation3.w ) ,
		q2 = D3DXQUATERNION( p2->rotation3.x , p2->rotation3.y , p2->rotation3.z , p2->rotation3.w ) ;
	D3DXQuaternionNlerp( &q , q1 , q2 , fLerp , 0 );
	pKeyOut->rotation3 = D3DXVECTOR4( q.x , q.y , q.z , q.w );
	//pKeyOut->rotation3 = pKeyOut->rotation2;
	pKeyOut->uvOffset2 = p2->uvOffset2 * fLerp + p1->uvOffset2 * ( 1.0f - fLerp );
	pKeyOut->uvRotationAngle2 = p2->uvRotationAngle2 * fLerp + p1->uvRotationAngle2 * ( 1.0f - fLerp );
	pKeyOut->bUwrap2 = p1->bUwrap2;
	pKeyOut->bVwrap2 = p1->bVwrap2;
}

void CDymMagicKeyFrame::SetMatresName(const char* strName,CDymRenderSystem *pDevice)
{
	if( pDevice && strName[0]!=0 )
	{
		CDymD3DTexture *pTex = NULL;
		if(SUCCEEDED(pDevice->CreateTextureFromFile ( strName , &pTex )))
		{
			if(m_pTexture)m_pTexture->Release ();
			m_pTexture = pTex;
		}
	}
	if( matresname != strName )
		strcpy( matresname , strName );
}
void CDymMagicKeyFrame::SetSound(LPCTSTR strName )
{
	if( strName == NULL )soundname[0]=0;
	else strcpy( soundname , strName );
}

BOOL CDymMagicKeyFrame::isSameKey( const CDymMagicKeyFrame * p1 , const CDymMagicKeyFrame * p2 )
{
	CDymMagicKeyFrame k1 = *p1;
	CDymMagicKeyFrame k2 = *p2;
	k1.m_fTime = k2.m_fTime ;
	k1.direction = k2.direction ;
	k1.position = k2.position ;
	k1.rotation2 = k2.rotation2 ;
	k1.rotation3 = k2.rotation3 ;
	k1.scale = k2.scale ;
	k1.m_pTexture = k2.m_pTexture ;
	ZeroMemory( k1.matresname , 32 );
	ZeroMemory( k2.matresname , 32 );
	if( memcmp( &k1 , &k2 , sizeof( CDymMagicKeyFrame ) ) == 0 )return TRUE;
	return FALSE;
}
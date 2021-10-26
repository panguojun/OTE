/***************************************************************************************************
文件名称:	DymMagicBillBoard.cpp
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	公告牌
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#include ".\dymmagicbillboard.h"

D3DXMATRIX m_matBillboard = D3DXMATRIX( 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 );
D3DXMATRIX m_matBillboard2 = D3DXMATRIX( 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 );

CDymMagicBillboard::CDymMagicBillboard(CDymRenderSystem *pDevice)
{
	m_pQuadVB = NULL;
	m_pDevice = pDevice;
	m_nBillboardType = 0;
	D3DXMatrixIdentity( &m_matRot );
}

CDymMagicBillboard::~CDymMagicBillboard(void)
{
	if( m_pQuadVB )delete[] m_pQuadVB;
}

void CDymMagicBillboard::SetBillboardType( int nType )
{
	m_nBillboardType = nType;
}
int CDymMagicBillboard::GetBillboardType()
{
	return m_nBillboardType;
}
void CDymMagicBillboard::SetRotationMatrix( D3DXMATRIX rot )
{
	m_matRot = rot ;
}
void CDymMagicBillboard::Render ()
{
	if( m_pQuadVB && m_pDevice )
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
		if( m_nBillboardType == 1 )
			m_pDevice->SetTransform ( D3DTS_WORLD , &(sca*rot*m_matBillboard*world2) );
		else if( m_nBillboardType == 2 )
			m_pDevice->SetTransform ( D3DTS_WORLD , &(sca*rot*m_matBillboard2*world2) );
		else if( m_nBillboardType == 3 )
		{
			/*D3DXVECTOR3 vPos = D3DXVECTOR3( world._41,world._42,world._43 );
			float a = D3DXVec3Dot( &vPos , &D3DXVECTOR3( 0 , 1 , 0 ) );
			D3DXMATRIX rot2;
			D3DXMatrixRotationZ( &rot2 , a );*/
			m_pDevice->SetTransform ( D3DTS_WORLD , &(sca*m_matRot*world2) );
		}
		
		m_pDevice->SetFVF(D3DFVF_XYZB1|D3DFVF_TEX1);
		m_pDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 2, m_pQuadVB , sizeof(QUAD_FVF) );
		if( m_nBillboardType != 0 )
			m_pDevice->SetTransform ( D3DTS_WORLD , &world );
	}
}

D3DXVECTOR3* CDymMagicBillboard::GetVertexStreamZeroData(int *pfacenum,int *pStride)
{
	QUAD_FVF *p = new QUAD_FVF[6];
	memcpy( p , m_pQuadVB , sizeof(QUAD_FVF)*6 );
	if(pfacenum)*pfacenum = 2;
	if(pStride)*pStride = sizeof( QUAD_FVF );
	return (D3DXVECTOR3*)p;
}

void CDymMagicBillboard::GetBoundingSphere( D3DXVECTOR3 *pCenter , float *pRadio )
{
	if( m_pQuadVB )
		D3DXComputeBoundingSphere( (D3DXVECTOR3*)&m_pQuadVB[0] , 6 , sizeof( QUAD_FVF ) , pCenter , pRadio );
}

void CDymMagicBillboard::init( D3DXVECTOR2 v1 , D3DXVECTOR2 v2 , BOOL bUp )
{
	// 顶点顺序：
	//  1   2 4
	//   |---|
	//   |---|
	//  0 3   5
	m_pQuadVB = new QUAD_FVF[6];
	if( bUp )
	{
		m_pQuadVB[0].p = D3DXVECTOR3( v1.x , 0.0f , v1.y );
		m_pQuadVB[1].p = D3DXVECTOR3( v1.x , 0.0f , v2.y );
		m_pQuadVB[2].p = D3DXVECTOR3( v2.x , 0.0f , v2.y );
		m_pQuadVB[5].p = D3DXVECTOR3( v2.x , 0.0f , v1.y );
	}
	else
	{
		m_pQuadVB[0].p = D3DXVECTOR3( v1.x , v1.y , 0.0f );
		m_pQuadVB[1].p = D3DXVECTOR3( v1.x , v2.y , 0.0f );
		m_pQuadVB[2].p = D3DXVECTOR3( v2.x , v2.y , 0.0f );
		m_pQuadVB[5].p = D3DXVECTOR3( v2.x , v1.y , 0.0f );
	}

	m_pQuadVB[0].u = 0.0f;
	m_pQuadVB[0].v = 1.0f;
	m_pQuadVB[0].fBlend = 1.0f;
	
	m_pQuadVB[1].u = 0.0f;
	m_pQuadVB[1].v = 0.0f;
	m_pQuadVB[1].fBlend = 0.0f;
	
	m_pQuadVB[2].u = 1.0f;
	m_pQuadVB[2].v = 0.0f;
	m_pQuadVB[2].fBlend = 0.0f;
	
	m_pQuadVB[5].u = 1.0f;
	m_pQuadVB[5].v = 1.0f;
	m_pQuadVB[5].fBlend = 1.0f;

	m_pQuadVB[4] = m_pQuadVB[2];
	m_pQuadVB[3] = m_pQuadVB[0];
}
D3DXMATRIX CDymMagicBillboard::GetBillboardMatrix( int billboardtype )
{
	if( billboardtype == 1 )return m_matBillboard2;
	return m_matBillboard;
}

void CDymMagicBillboard::ComputeBillboardMatrix(D3DXVECTOR3 vEyePt, D3DXVECTOR3 vLookatPt, D3DXVECTOR3 vUpVec)
{
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
}

/***************************************************************************************************
文件名称:	DymMagicParticlesObj.h
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	粒子子物体
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#pragma once
#include <d3dx9.h>
#include "DymMagicBillboard.h"

class CDymMagicParticlesObj
{
	CDymMagicBillboard *m_pBillboard;
	CDymRenderSystem * m_pDevice;
	float m_fTimeLiftTime;
public:
	D3DXMATRIX m_matWorld;
	D3DXVECTOR3 m_vForceAdd;

	float fCreateTime;
	D3DXVECTOR3 m_vPos;
	D3DXVECTOR3 m_vPos2;
	D3DXVECTOR2 m_vSca;
	float m_fRot;
	float time_to_live;
	D3DXVECTOR3 force_vector;
	D3DXVECTOR4 colour;
	D3DXVECTOR4 colour_end;
	float velocity;
	D3DXVECTOR3 direction;
	float scale_add_x;
	float scale_add_y;
	float rotate_add;
	bool iswithfather;
	int force_type;
	float accept_power;
	int force_add_type;
	D3DXVECTOR3 m_vAcceptPos;
	int limit_vector;

	CDymMagicParticlesObj( float fWidth , float fHeight , CDymRenderSystem *pDevice , float create_time , BOOL bUp = FALSE );
	~CDymMagicParticlesObj(void);
	void FrameMove( float fTime );
	void Render(void);
	BOOL isOverTime(float fTime);
	void SetBillboardType( int nType );
	BOOL GetBoundingSphere( D3DXVECTOR3 *pCenter , float *pRadio );
	void CDymMagicParticlesObj::SetRotMatrix( D3DXMATRIX matRot );
};

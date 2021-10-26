/***************************************************************************************************
�ļ�����:	DymMagicParticlesObj.h
ϵͳ����:	�ݺ�ʱ��
ģ������:	��Чģ��
����˵��:	����������
����ĵ�:	
��    ��:	��ҹ��
��������:	2006-7
��    ��:	��ǿ
�����Ȩ:	׿��ʱ��
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

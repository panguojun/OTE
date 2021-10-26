/***************************************************************************************************
�ļ�����:	DymMagicBillBoard.h
ϵͳ����:	�ݺ�ʱ��
ģ������:	��Чģ��
����˵��:	������
����ĵ�:	
��    ��:	��ҹ��
��������:	2006-7
��    ��:	��ǿ
�����Ȩ:	׿��ʱ��
***************************************************************************************************/
#pragma once
#include "DymRenderSystem.h"

//----------------------------------------------------------------------------------
//
// ��������
//
//----------------------------------------------------------------------------------
class __declspec( dllexport ) CDymMagicBillboard
{
	// ����FVF��ʽ
	struct QUAD_FVF
	{
		D3DXVECTOR3 p;
		float fBlend;
		float u,v;
	};
	// ���㻺��
	QUAD_FVF * m_pQuadVB;
	// D3D�豸
	CDymRenderSystem * m_pDevice;
	// ����������
	int m_nBillboardType;
	// ��ת���Ӿ���
	D3DXMATRIX m_matRot;

public:
	CDymMagicBillboard(CDymRenderSystem *pDevice);
	~CDymMagicBillboard(void);
	// ���ù���������
	void SetBillboardType( int nType );
	// ��ù���������
	int GetBillboardType();
	// ��Ⱦ
	void Render();
	// �����������С
	void init( D3DXVECTOR2 v1 , D3DXVECTOR2 v2 , BOOL bUp = FALSE );
	// ȫ����Ⱦ֮ǰ������һ�ι����ƾ���
	static void ComputeBillboardMatrix(D3DXVECTOR3 vEyePt, D3DXVECTOR3 vLookatPt, D3DXVECTOR3 vUpVec);
	// ��ù����ƾ���
	static D3DXMATRIX GetBillboardMatrix( int billboardtype );
	// ��ù����ƶ��㻺������
	D3DXVECTOR3* GetVertexStreamZeroData(int *pfacenum,int *pStride);
	// ��ð���
	void GetBoundingSphere( D3DXVECTOR3 *pCenter , float *pRadio );
	// ������ת
	void CDymMagicBillboard::SetRotationMatrix( D3DXMATRIX rot );
};

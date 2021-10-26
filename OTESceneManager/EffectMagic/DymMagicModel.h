/***************************************************************************************************
�ļ�����:	DymMagicModel.h
ϵͳ����:	�ݺ�ʱ��
ģ������:	��Чģ��
����˵��:	ģ��
����ĵ�:	
��    ��:	��ҹ��
��������:	2006-7
��    ��:	��ǿ
�����Ȩ:	׿��ʱ��
***************************************************************************************************/
#pragma once
#include "DymMagicBaseObj.h"
#include "DymMagicBillboard.h"
#include "DymMagicMesh.h"

//--------------------------------------------------------------------------------
//
// ģ�Ͷ���
//
//--------------------------------------------------------------------------------
class CDymMagicModel:public CDymMagicBaseObj
{
	// �������񴴽���ͼ����
	class CDymMagicModelMesh:public CDymMagicMesh
	{
		CDymMagicKeyFrameManager *m_pKeyManager;
	public:
		CDymMagicModelMesh( CDymRenderSystem *pRenderSystem , CDymMagicKeyFrameManager *pKeyManager ):CDymMagicMesh( pRenderSystem )
		{
			m_pKeyManager = pKeyManager;
		}
		virtual void OnCreateTexture( LPCTSTR strTextureName )
		{
			if( strTextureName == NULL )return;
			if( strTextureName[0] == 0 )return;
			CDymMagicKeyFrame *pKey = m_pKeyManager->GetKeyFrame ( 0.0f );
			if( pKey == NULL )
			{
				pKey = new CDymMagicKeyFrame();
				m_pKeyManager->AddKeyFrame ( pKey );
			}
			pKey->nVersion = 2;
			pKey->SetMatresName ( strTextureName , this->m_pDevice );
		}
	};
	// D3D�豸
	CDymRenderSystem *m_pDevice;
	// Բ��FVF
	struct SPIRE_FVF
	{
		D3DXVECTOR3 p;
		float fBlend;
		float u,v;
	};
	
	void _ComputeBoundingBox( CDymMagicKeyFrame *pKeyFrame , DYM_WORLD_MATRIX world );
	// ����FVF
	struct SHADOW_FVF
	{
		D3DXVECTOR3 p;
		float u,v;
	};
	SHADOW_FVF *m_pShadowVB;
	BOOL m_bShadowNeedSetPos;

public:
	// ģ������
	enum MODEL_TYPE
	{
		MT_spire , //Բ��
		MT_quad ,// ������
		MT_mesh ,// ����
		MT_shadow ,//����
		MT_spire_shadow,//Ӱ��
	};
	
	char m_strName[32];
	TCHAR m_strMeshName[32];
	MODEL_TYPE mytype;// ģ������

	D3DXVECTOR3 initsize;
	bool isinfacex;
	bool isinfacey;
	bool isinfacez;
	int m_nMeshBillBoardType;
	int facenum;
	bool isskybox;
	bool ishavort;
	float flyspeed;
	bool bUsingMeshVertexColor;

	float m_fAddTime;
	int shadow_render_type;

	CDymD3DVertexBuffer* m_pVB;
	CDymD3DVertexBuffer* m_pVB2;
	CDymMagicBillboard* m_pBillboard;
	CDymMagicMesh* m_pMesh;

	CDymMagicModel(CDymRenderSystem *pDevice);
	~CDymMagicModel(void);
	HRESULT Render(DYM_WORLD_MATRIX world , DWORD flag);
	void FrameMove(float fCurTime,float fElapsedTime);
	void SetParticlesName(char* strName);
	HRESULT init(void);
	BOOL IsAlphaRender();
	D3DXVECTOR3* GetVertexStreamZeroData(int *pfacenum,int *pStride);
	void CopyModel( CDymMagicModel *p );
	HRESULT ImportMeshFromXML( LPCTSTR xmlfilename );
	void ComputeBoundingBox( DYM_WORLD_MATRIX world );
	void GetBoundingBoxRealTime( D3DXVECTOR3 *pMin , D3DXVECTOR3 *pMax );
};

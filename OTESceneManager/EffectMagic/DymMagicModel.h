/***************************************************************************************************
文件名称:	DymMagicModel.h
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	模型
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#pragma once
#include "DymMagicBaseObj.h"
#include "DymMagicBillboard.h"
#include "DymMagicMesh.h"

//--------------------------------------------------------------------------------
//
// 模型对象
//
//--------------------------------------------------------------------------------
class CDymMagicModel:public CDymMagicBaseObj
{
	// 重载网格创建贴图函数
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
	// D3D设备
	CDymRenderSystem *m_pDevice;
	// 圆环FVF
	struct SPIRE_FVF
	{
		D3DXVECTOR3 p;
		float fBlend;
		float u,v;
	};
	
	void _ComputeBoundingBox( CDymMagicKeyFrame *pKeyFrame , DYM_WORLD_MATRIX world );
	// 残像FVF
	struct SHADOW_FVF
	{
		D3DXVECTOR3 p;
		float u,v;
	};
	SHADOW_FVF *m_pShadowVB;
	BOOL m_bShadowNeedSetPos;

public:
	// 模型类型
	enum MODEL_TYPE
	{
		MT_spire , //圆环
		MT_quad ,// 公告牌
		MT_mesh ,// 网格
		MT_shadow ,//残像
		MT_spire_shadow,//影子
	};
	
	char m_strName[32];
	TCHAR m_strMeshName[32];
	MODEL_TYPE mytype;// 模型类型

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

/***************************************************************************************************
�ļ�����:	DymMagicBaseObj.h
ϵͳ����:	�ݺ�ʱ��
ģ������:	��Чģ��
����˵��:	��Ч�������
����ĵ�:	
��    ��:	��ҹ��
��������:	2006-7
��    ��:	��ǿ
�����Ȩ:	׿��ʱ��
***************************************************************************************************/
#pragma once
#include <tchar.h>
#include "DymMagicKeyFrameManager.h"
#include "DymRenderSystem.h"

#define MAGIC_RENDER_DO_NOT_SETUP_RENDERSTATE 0x0010
#define MAGIC_RENDER_NO_HIDE 0x0020
#define MAGIC_RENDER_SELECTION 0x0040
#define MAGIC_RENDER_SHADOW 0x0080
#define MAGIC_RENDER_FAST 0x0100

//-----------------------------------------------------------------------------------
//
// �����������
//
//-----------------------------------------------------------------------------------
struct DYM_WORLD_MATRIX
{
	D3DXVECTOR3 vPos;		// ����
	D3DXVECTOR3 vSca;		// ����
	D3DXVECTOR3 vSca2;		// ����2�����������1��ˣ�
	D3DXVECTOR3 vAxis;		// ��ת��
	D3DXVECTOR4 vColor;		// ��ɫ
	float fAngle;			// ��ת�Ƕ�
	BOOL bGrassMethod;		// �Ƿ��òݷ���

	DYM_WORLD_MATRIX()
	{
		vPos = D3DXVECTOR3( 0 , 0 , 0 );
		vSca = D3DXVECTOR3( 1 , 1 , 1 );
		vAxis = D3DXVECTOR3( 0 , 1 , 0 );
		vSca2 = D3DXVECTOR3( 1 , 1 , 1 );
		vColor = D3DXVECTOR4( 1 , 1 , 1 , 1 );
		fAngle = 0.0f;
		bGrassMethod = FALSE;
	}
	// �����������
	D3DXMATRIX GetWorldMatrix()
	{
		D3DXMATRIX matWorld,matPos,matSca,matRot;
		D3DXMatrixIdentity(&matWorld);
		if(vSca.x!=1.0f || vSca.y!=1.0f || vSca.z!=1.0f || vSca2.x!=1.0f || vSca2.y!=1.0f || vSca2.z!=1.0f )
		{
			D3DXMatrixScaling(&matSca , vSca.x*vSca2.x , vSca.y*vSca2.y , vSca.z*vSca2.z );
			matWorld = matWorld * matSca;
		}
		if( fAngle!=0.0f )
		{
			D3DXMatrixRotationAxis( &matRot , &vAxis , fAngle );
			matWorld = matWorld*matRot;
		}
		if(vPos.x !=0.0f || vPos.y !=0.0f || vPos.z !=0.0f )
		{
			D3DXMatrixTranslation( &matPos , vPos.x , vPos.y , vPos.z );
			matWorld = matWorld * matPos;
		}
		return matWorld;
	};
};

//--------------------------------------------------------------------------------
//
// ��Ч���������
//
//--------------------------------------------------------------------------------
class __declspec( dllexport ) CDymMagicBaseObj
{
protected:
	CDymRenderSystem *m_pDevice;	// D3D�豸
	CDymD3DTexture* m_pTex;			// ��������ͼ�������ã�
	D3DXMATRIX m_matWorld1;			// �����������1
	D3DXMATRIX m_matWorld2;			// �����������2
	
public:
	// �������
	enum MATERIAL_TYPE
	{
		MT_Effect_sceneno,
		MT_Effect_sceneab,
		MT_Effect_sceneadd,
		MT_Effect_sceneaddalpha,
	};
	// ����������
	enum MAGICE_OBJ_TYPE
	{
		MOT_MODEL ,
		MOT_PARTICLE ,
	};

	MAGICE_OBJ_TYPE m_objType;	// ����������
	MATERIAL_TYPE materialtype;	// �������
	char materialres[32];		// ������ͣ��ű���
	CDymMagicKeyFrameManager m_KeyFrameManager;// �ؼ�֡������
	int m_nID;// ������ID
	int m_nID_rotation_with;// Χ����תID
	int m_nID_follow_with;// ����ID
	CDymMagicBaseObj *m_pRotationObj;// Χ����תID����ָ��
	CDymMagicBaseObj *m_pFollowObj;// ����ID����ָʮ
	char m_strObjName[32];// ��������

	D3DXVECTOR3 m_vBoundingBox1;// ��С�󶨺�
	D3DXVECTOR3 m_vBoundingBox2;// ���󶨺�

	CDymMagicBaseObj(CDymRenderSystem *pDevice,MAGICE_OBJ_TYPE type);
	~CDymMagicBaseObj(void);
	// ���û�����ͣ��ű��ַ���
	void SetMaterialType(char* strType);
	// ��û�����ͣ��ű��ַ���
	char*  GetMaterialType();
	// ������Ⱦ״̬
	void SetupRenderState( DYM_WORLD_MATRIX world);
	// ֡�ƶ�
	void FrameMoveBase(float fCurTime,float fElapsedTime);
	// �����������
	void ComputeWorldMatrix( DYM_WORLD_MATRIX world );
	// ����������
	D3DXMATRIX GetWorld1(){return m_matWorld1;};
	D3DXMATRIX GetWorld2(){return m_matWorld2;};
	// ��Ⱦ
	virtual HRESULT Render(DYM_WORLD_MATRIX world , DWORD flag);
	// �������������
	MAGICE_OBJ_TYPE GetObjType();
	// ����������
	void CopyBaseObj( CDymMagicBaseObj *p );
	// ������תID
	void SetRotationWithID( CDymMagicBaseObj *p );
	// ���ø���ID
	void SetFollowWithID( CDymMagicBaseObj *p );
	// �������
	D3DXVECTOR3 *GetPos();
};

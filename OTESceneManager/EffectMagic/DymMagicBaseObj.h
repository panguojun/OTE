/***************************************************************************************************
文件名称:	DymMagicBaseObj.h
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	特效物件基类
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
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
// 世界矩阵数据
//
//-----------------------------------------------------------------------------------
struct DYM_WORLD_MATRIX
{
	D3DXVECTOR3 vPos;		// 坐标
	D3DXVECTOR3 vSca;		// 缩放
	D3DXVECTOR3 vSca2;		// 缩放2（最后与缩放1相乘）
	D3DXVECTOR3 vAxis;		// 旋转轴
	D3DXVECTOR4 vColor;		// 颜色
	float fAngle;			// 旋转角度
	BOOL bGrassMethod;		// 是否用草方法

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
	// 计算世界矩阵
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
// 特效子物体基类
//
//--------------------------------------------------------------------------------
class __declspec( dllexport ) CDymMagicBaseObj
{
protected:
	CDymRenderSystem *m_pDevice;	// D3D设备
	CDymD3DTexture* m_pTex;			// 子物体贴图（已无用）
	D3DXMATRIX m_matWorld1;			// 保存世界矩阵1
	D3DXMATRIX m_matWorld2;			// 保存世界矩阵2
	
public:
	// 混合类型
	enum MATERIAL_TYPE
	{
		MT_Effect_sceneno,
		MT_Effect_sceneab,
		MT_Effect_sceneadd,
		MT_Effect_sceneaddalpha,
	};
	// 子物体类型
	enum MAGICE_OBJ_TYPE
	{
		MOT_MODEL ,
		MOT_PARTICLE ,
	};

	MAGICE_OBJ_TYPE m_objType;	// 子物体类型
	MATERIAL_TYPE materialtype;	// 混合类型
	char materialres[32];		// 混合类型（脚本）
	CDymMagicKeyFrameManager m_KeyFrameManager;// 关键帧管理器
	int m_nID;// 子物体ID
	int m_nID_rotation_with;// 围绕旋转ID
	int m_nID_follow_with;// 跟随ID
	CDymMagicBaseObj *m_pRotationObj;// 围绕旋转ID对象指针
	CDymMagicBaseObj *m_pFollowObj;// 跟随ID对象指十
	char m_strObjName[32];// 子物体名

	D3DXVECTOR3 m_vBoundingBox1;// 最小绑定盒
	D3DXVECTOR3 m_vBoundingBox2;// 最大绑定盒

	CDymMagicBaseObj(CDymRenderSystem *pDevice,MAGICE_OBJ_TYPE type);
	~CDymMagicBaseObj(void);
	// 设置混合类型（脚本字符）
	void SetMaterialType(char* strType);
	// 获得混合类型（脚本字符）
	char*  GetMaterialType();
	// 设置渲染状态
	void SetupRenderState( DYM_WORLD_MATRIX world);
	// 帧移动
	void FrameMoveBase(float fCurTime,float fElapsedTime);
	// 计算世界矩阵
	void ComputeWorldMatrix( DYM_WORLD_MATRIX world );
	// 获得世界矩阵
	D3DXMATRIX GetWorld1(){return m_matWorld1;};
	D3DXMATRIX GetWorld2(){return m_matWorld2;};
	// 渲染
	virtual HRESULT Render(DYM_WORLD_MATRIX world , DWORD flag);
	// 获得子物体类型
	MAGICE_OBJ_TYPE GetObjType();
	// 复制子物体
	void CopyBaseObj( CDymMagicBaseObj *p );
	// 设置旋转ID
	void SetRotationWithID( CDymMagicBaseObj *p );
	// 设置跟随ID
	void SetFollowWithID( CDymMagicBaseObj *p );
	// 获得坐标
	D3DXVECTOR3 *GetPos();
};

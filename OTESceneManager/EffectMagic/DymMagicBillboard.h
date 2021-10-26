/***************************************************************************************************
文件名称:	DymMagicBillBoard.h
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	公告牌
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#pragma once
#include "DymRenderSystem.h"

//----------------------------------------------------------------------------------
//
// 公告牌类
//
//----------------------------------------------------------------------------------
class __declspec( dllexport ) CDymMagicBillboard
{
	// 顶点FVF格式
	struct QUAD_FVF
	{
		D3DXVECTOR3 p;
		float fBlend;
		float u,v;
	};
	// 顶点缓冲
	QUAD_FVF * m_pQuadVB;
	// D3D设备
	CDymRenderSystem * m_pDevice;
	// 公告牌类型
	int m_nBillboardType;
	// 旋转叠加矩阵
	D3DXMATRIX m_matRot;

public:
	CDymMagicBillboard(CDymRenderSystem *pDevice);
	~CDymMagicBillboard(void);
	// 设置公告牌类型
	void SetBillboardType( int nType );
	// 获得公告牌类型
	int GetBillboardType();
	// 渲染
	void Render();
	// 创建，传入大小
	void init( D3DXVECTOR2 v1 , D3DXVECTOR2 v2 , BOOL bUp = FALSE );
	// 全局渲染之前，计算一次公告牌矩阵
	static void ComputeBillboardMatrix(D3DXVECTOR3 vEyePt, D3DXVECTOR3 vLookatPt, D3DXVECTOR3 vUpVec);
	// 获得公告牌矩阵
	static D3DXMATRIX GetBillboardMatrix( int billboardtype );
	// 获得公告牌顶点缓冲数据
	D3DXVECTOR3* GetVertexStreamZeroData(int *pfacenum,int *pStride);
	// 获得绑定球
	void GetBoundingSphere( D3DXVECTOR3 *pCenter , float *pRadio );
	// 设置旋转
	void CDymMagicBillboard::SetRotationMatrix( D3DXMATRIX rot );
};

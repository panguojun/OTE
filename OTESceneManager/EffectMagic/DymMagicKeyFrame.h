/***************************************************************************************************
文件名称:	DymMagicKeyFrame.h
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	关键帧
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#pragma once
#include "DymRenderSystem.h"

//------------------------------------------------------------------------------
//
// 关键帧
//
//------------------------------------------------------------------------------
class __declspec( dllexport ) CDymMagicKeyFrame
{
	
public:
	D3DXVECTOR4 rotation2;// 旋转角度
	D3DXVECTOR4 rotation3;// 旋转四元数

	float m_fTime;// 关键帧时间 
	D3DXVECTOR3 position;// 坐标
	bool animate;// 是否显示
	D3DXVECTOR3 scale;// 缩放
	D3DXVECTOR3 direction;// 方向
	D3DXVECTOR3 spinaxis;// 旋转轴
	float spinangle;// 旋转角度
	float spinradius;// 无用
	D3DXVECTOR4 colour; // 颜色
	char matresname[32];	// 贴图文件名
	char soundname[32];// 声音文件名
	D3DXVECTOR2 matUVtile;// 帖图坐标
	D3DXVECTOR2 scroll_anim;// 无用
	float rotate_anim;// 贴图旋转角度
	float topscale;// 顶缩放
	float bottomscale;// 底缩放
	char animname[32];	// 动画名，已无用
	float animspeed;// 速度，已无用
	float soundradio;// 声音范围
	BOOL bLoopPlay;// 是否循环

	// for magic 2.0
	int nVersion;//版本
	D3DXVECTOR2 uvOffset2;// UV缩放
	float uvRotationAngle2;// UV旋转
	BOOL bUwrap2;// U重复
	BOOL bVwrap2;// V重复

	CDymD3DTexture *m_pTexture;// 贴图

	CDymMagicKeyFrame(void);
	~CDymMagicKeyFrame(void);
	// 线性插值
	static void Lerp(const CDymMagicKeyFrame * p1, const CDymMagicKeyFrame * p2, CDymMagicKeyFrame * pKeyOut, float fLerpTime);
	// 两帧是否相同
	static BOOL isSameKey( const CDymMagicKeyFrame * p1 , const CDymMagicKeyFrame * p2 );
	// 设置贴图
	void SetMatresName(const char* strName,CDymRenderSystem *pDevice);
	// 设置声音
	void SetSound(LPCTSTR strName );
	// 赋值操作符
	CDymMagicKeyFrame& operator=(const CDymMagicKeyFrame& a);
	// 添加旋转
	void AddRotationX( float volume );
	void AddRotationY( float volume );
	void AddRotationZ( float volume );
	// 获得旋转
	float GetRotationX();
	float GetRotationY();
	float GetRotationZ();
	float _GetRotationX( D3DXQUATERNION q );
	float _GetRotationY( D3DXQUATERNION q );
	float _GetRotationZ( D3DXQUATERNION q );
	// 设置旋转
	void SetRotation( float x , float y , float z );
	// 获得旋转四元数
	D3DXQUATERNION GetQuaternion();
	D3DXVECTOR4 GetRotation();
};

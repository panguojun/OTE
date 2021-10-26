/***************************************************************************************************
文件名称:	DymMagicKeyFrameManager.h
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	关键帧管理
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#pragma once
#include "DymMagicKeyFrame.h"
#include <list>

//-----------------------------------------------------------------------------------------
//
// 关键帧管理器
//
//-----------------------------------------------------------------------------------------
class __declspec( dllexport ) CDymMagicKeyFrameManager
{
	CDymMagicKeyFrame m_keyFrameForLerp;//用来做插值的帧
	CDymMagicKeyFrame* m_pLastKeyFrame;// 指向最后一帧
	CDymMagicKeyFrame* m_pKeyFrameNow;// 指向当前播放帧
	float m_fCurTime ;// 当前时间
	float m_fElapsedTime;// 经过时间
	D3DXMATRIX m_matUVPos;// UV坐标
	D3DXMATRIX m_matUVRot;// UV旋转
	D3DXMATRIX m_matUVSca;// UV缩放
	float m_fMaxTime;// 最大时间
	float m_fLastFindKeyMinMax;// 最后获得的关键帧时间（如果要找的时间与这个时间相同，则直接反回插值关键帧）
protected:
	// 给一个时间，反回最近的两个关键帧，这两个关键帧在所给时间之间，如果所给时间正好是关键帧时间，则min和max相同
	void FindKeyFrameMinMax(CDymMagicKeyFrame** ppMin, CDymMagicKeyFrame** ppMax,float fTime);
	// 插值两个关键帧，结果保存在m_keyFrameForLerp;
	void LerpKeyFrame(CDymMagicKeyFrame* p1, CDymMagicKeyFrame* p2, float fLerpTime);
public:
	CDymMagicKeyFrame* m_pKeyMin;
	CDymMagicKeyFrame* m_pKeyMax;
	std::list<CDymMagicKeyFrame*> m_listKeyFrame;//关键帧列表

	CDymMagicKeyFrameManager(void);
	~CDymMagicKeyFrameManager(void);
	// 添加关键帧
	void AddKeyFrame(CDymMagicKeyFrame * pKeyFrame);
	// 获得关键帧
	CDymMagicKeyFrame* GetKeyFrame(float fTime);
	// 获得最后一个关键帧
	CDymMagicKeyFrame *GetLastKeyFrame();
	// 帧移动
	void FrameMove(float fCurTime,float fElapsedTime);
	// 获得当前播放的关键帧
	CDymMagicKeyFrame* GetKeyFrameNow(void);
	// 获得关键帧的UV矩阵
	D3DXMATRIX GetUVAniMatrix(void);
	// 设置关键帧的最大时间
	void SetKeyMaxTime(float fMaxTime);
	// 获得关键帧，第二个值是时间容差
	CDymMagicKeyFrame* GetKeyFrame2(float fTime , float fOffset);
	// 排序
	void SortKey();
	// 删除所有帧
	void ClearKey();
	// 删除一帧
	void DeleteKey( CDymMagicKeyFrame *pKey );
	// 从传入的管理器中考贝所有帧
	void CopyKeyFrame( CDymMagicKeyFrameManager *p , CDymRenderSystem *pDevice , BOOL bCopySound = FALSE );
	// 考贝一帧
	CDymMagicKeyFrame* CopyKey( CDymMagicKeyFrame *pKey , CDymRenderSystem *pDevice );
	// 获得帧最大时间
	float GetKeyMaxTime();
};

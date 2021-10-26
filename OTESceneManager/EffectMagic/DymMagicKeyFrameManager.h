/***************************************************************************************************
�ļ�����:	DymMagicKeyFrameManager.h
ϵͳ����:	�ݺ�ʱ��
ģ������:	��Чģ��
����˵��:	�ؼ�֡����
����ĵ�:	
��    ��:	��ҹ��
��������:	2006-7
��    ��:	��ǿ
�����Ȩ:	׿��ʱ��
***************************************************************************************************/
#pragma once
#include "DymMagicKeyFrame.h"
#include <list>

//-----------------------------------------------------------------------------------------
//
// �ؼ�֡������
//
//-----------------------------------------------------------------------------------------
class __declspec( dllexport ) CDymMagicKeyFrameManager
{
	CDymMagicKeyFrame m_keyFrameForLerp;//��������ֵ��֡
	CDymMagicKeyFrame* m_pLastKeyFrame;// ָ�����һ֡
	CDymMagicKeyFrame* m_pKeyFrameNow;// ָ��ǰ����֡
	float m_fCurTime ;// ��ǰʱ��
	float m_fElapsedTime;// ����ʱ��
	D3DXMATRIX m_matUVPos;// UV����
	D3DXMATRIX m_matUVRot;// UV��ת
	D3DXMATRIX m_matUVSca;// UV����
	float m_fMaxTime;// ���ʱ��
	float m_fLastFindKeyMinMax;// ����õĹؼ�֡ʱ�䣨���Ҫ�ҵ�ʱ�������ʱ����ͬ����ֱ�ӷ��ز�ֵ�ؼ�֡��
protected:
	// ��һ��ʱ�䣬��������������ؼ�֡���������ؼ�֡������ʱ��֮�䣬�������ʱ�������ǹؼ�֡ʱ�䣬��min��max��ͬ
	void FindKeyFrameMinMax(CDymMagicKeyFrame** ppMin, CDymMagicKeyFrame** ppMax,float fTime);
	// ��ֵ�����ؼ�֡�����������m_keyFrameForLerp;
	void LerpKeyFrame(CDymMagicKeyFrame* p1, CDymMagicKeyFrame* p2, float fLerpTime);
public:
	CDymMagicKeyFrame* m_pKeyMin;
	CDymMagicKeyFrame* m_pKeyMax;
	std::list<CDymMagicKeyFrame*> m_listKeyFrame;//�ؼ�֡�б�

	CDymMagicKeyFrameManager(void);
	~CDymMagicKeyFrameManager(void);
	// ��ӹؼ�֡
	void AddKeyFrame(CDymMagicKeyFrame * pKeyFrame);
	// ��ùؼ�֡
	CDymMagicKeyFrame* GetKeyFrame(float fTime);
	// ������һ���ؼ�֡
	CDymMagicKeyFrame *GetLastKeyFrame();
	// ֡�ƶ�
	void FrameMove(float fCurTime,float fElapsedTime);
	// ��õ�ǰ���ŵĹؼ�֡
	CDymMagicKeyFrame* GetKeyFrameNow(void);
	// ��ùؼ�֡��UV����
	D3DXMATRIX GetUVAniMatrix(void);
	// ���ùؼ�֡�����ʱ��
	void SetKeyMaxTime(float fMaxTime);
	// ��ùؼ�֡���ڶ���ֵ��ʱ���ݲ�
	CDymMagicKeyFrame* GetKeyFrame2(float fTime , float fOffset);
	// ����
	void SortKey();
	// ɾ������֡
	void ClearKey();
	// ɾ��һ֡
	void DeleteKey( CDymMagicKeyFrame *pKey );
	// �Ӵ���Ĺ������п�������֡
	void CopyKeyFrame( CDymMagicKeyFrameManager *p , CDymRenderSystem *pDevice , BOOL bCopySound = FALSE );
	// ����һ֡
	CDymMagicKeyFrame* CopyKey( CDymMagicKeyFrame *pKey , CDymRenderSystem *pDevice );
	// ���֡���ʱ��
	float GetKeyMaxTime();
};

/***************************************************************************************************
�ļ�����:	DymMagicKeyFrame.h
ϵͳ����:	�ݺ�ʱ��
ģ������:	��Чģ��
����˵��:	�ؼ�֡
����ĵ�:	
��    ��:	��ҹ��
��������:	2006-7
��    ��:	��ǿ
�����Ȩ:	׿��ʱ��
***************************************************************************************************/
#pragma once
#include "DymRenderSystem.h"

//------------------------------------------------------------------------------
//
// �ؼ�֡
//
//------------------------------------------------------------------------------
class __declspec( dllexport ) CDymMagicKeyFrame
{
	
public:
	D3DXVECTOR4 rotation2;// ��ת�Ƕ�
	D3DXVECTOR4 rotation3;// ��ת��Ԫ��

	float m_fTime;// �ؼ�֡ʱ�� 
	D3DXVECTOR3 position;// ����
	bool animate;// �Ƿ���ʾ
	D3DXVECTOR3 scale;// ����
	D3DXVECTOR3 direction;// ����
	D3DXVECTOR3 spinaxis;// ��ת��
	float spinangle;// ��ת�Ƕ�
	float spinradius;// ����
	D3DXVECTOR4 colour; // ��ɫ
	char matresname[32];	// ��ͼ�ļ���
	char soundname[32];// �����ļ���
	D3DXVECTOR2 matUVtile;// ��ͼ����
	D3DXVECTOR2 scroll_anim;// ����
	float rotate_anim;// ��ͼ��ת�Ƕ�
	float topscale;// ������
	float bottomscale;// ������
	char animname[32];	// ��������������
	float animspeed;// �ٶȣ�������
	float soundradio;// ������Χ
	BOOL bLoopPlay;// �Ƿ�ѭ��

	// for magic 2.0
	int nVersion;//�汾
	D3DXVECTOR2 uvOffset2;// UV����
	float uvRotationAngle2;// UV��ת
	BOOL bUwrap2;// U�ظ�
	BOOL bVwrap2;// V�ظ�

	CDymD3DTexture *m_pTexture;// ��ͼ

	CDymMagicKeyFrame(void);
	~CDymMagicKeyFrame(void);
	// ���Բ�ֵ
	static void Lerp(const CDymMagicKeyFrame * p1, const CDymMagicKeyFrame * p2, CDymMagicKeyFrame * pKeyOut, float fLerpTime);
	// ��֡�Ƿ���ͬ
	static BOOL isSameKey( const CDymMagicKeyFrame * p1 , const CDymMagicKeyFrame * p2 );
	// ������ͼ
	void SetMatresName(const char* strName,CDymRenderSystem *pDevice);
	// ��������
	void SetSound(LPCTSTR strName );
	// ��ֵ������
	CDymMagicKeyFrame& operator=(const CDymMagicKeyFrame& a);
	// �����ת
	void AddRotationX( float volume );
	void AddRotationY( float volume );
	void AddRotationZ( float volume );
	// �����ת
	float GetRotationX();
	float GetRotationY();
	float GetRotationZ();
	float _GetRotationX( D3DXQUATERNION q );
	float _GetRotationY( D3DXQUATERNION q );
	float _GetRotationZ( D3DXQUATERNION q );
	// ������ת
	void SetRotation( float x , float y , float z );
	// �����ת��Ԫ��
	D3DXQUATERNION GetQuaternion();
	D3DXVECTOR4 GetRotation();
};

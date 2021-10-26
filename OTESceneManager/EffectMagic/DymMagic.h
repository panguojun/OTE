/***************************************************************************************************
�ļ�����:	DymMagic.h
ϵͳ����:	�ݺ�ʱ��
ģ������:	��Чģ��
����˵��:	��Ч��ģ��
����ĵ�:	
��    ��:	��ҹ��
��������:	2006-7
��    ��:	��ǿ
�����Ȩ:	׿��ʱ��
***************************************************************************************************/
#pragma once
#include "DymScriptLoader.h"
#include "DymMagicModel.h"
#include "DymMagicParticles.h"
#include "DymFontFactory.h"
#include "DymSoundManager.h"

/// �༭����������������
enum MAGIC_CREATE_TYPE
{
	CREATE_BILLBOARD ,
	CREATE_SPIRE ,
	CREATE_MESH ,
	CREATE_SHADOW ,
	CREATE_SPIRE_SHADOW ,
};
class __declspec( dllexport ) CDymMagic
{
	/// ��������ؼ�֡���ĳ��ֵ�Ƿ���ͬ
	template<class Type>inline
	BOOL Check_PrevKey( Type v1 , Type v2 )
	{
		return v1!=v2;
	}
	/// ��������ؼ�֡���ĳ���ַ����Ƿ���ͬ
	BOOL Check_PrevKey_Str( TCHAR* v1 , TCHAR* v2 )
	{
		if( _tcsicmp( v1 , v2 ) == 0 )return FALSE;
		return TRUE;
	}

	/// ��Ч�Ƿ�ѭ������
	bool isloop;
	/// �ɰ汾ʹ�ã�������
	bool issametime;
	/// ģ�������б�
	std::list<CDymMagicModel*> m_listModel;
	/// ���ӷ������б�
	std::list<CDymMagicParticles*> m_listParticles;
	/// �ؼ�֡�б�
	std::list<CDymMagicKeyFrame*> m_listSoundKey;
	/// ���������б�������Чʱ���ӹؼ�֡�ж��������������ݣ����뵽���б���
	struct SOUND_OBJ
	{
		CDymSoundObj* pSound;
		CDymMagicBaseObj *pObj;
		BOOL bLoop;
		float radio;
	};
	std::list<SOUND_OBJ*> m_listSoundObj;
	/// ��Ⱦ�豸
	CDymRenderSystem * m_pDevice;
	/// ���ֹ����豸
	CDymSoundManager * m_pSoundManager;
	/// ������������
	DYM_WORLD_MATRIX m_world;
	/// �������ʱ��
	float m_fKeyMaxTime;
	/// ���ŵ�ǰʱ��
	float m_fKeyAddTime;
	/// �Ƿ񲥷����
	BOOL m_bOverTime;
	/// ��Ч�汾��
	int m_nVersion;
	/// ����������ȫ������
	D3DXVECTOR3 m_vGlobleScaling;
	/// �Ƿ������Ⱦ��������Ⱦ���Զ��ر�Z-BUFFER
	BOOL m_bFastRender;
	/// �Ƿ��ǵ�һ�β��ţ���һ�β���ʱ��Ҫ�������������ڶ����Ժ󲥷�ʱ��ѭ�����ŵ���������������
	BOOL m_bFirstPlay;
	
	
	/// �󶨺�
	D3DXVECTOR3 m_vBoundingBox1;
	D3DXVECTOR3 m_vBoundingBox2;

	/// �ӽű��ж�ȡģ��
	BOOL LoadModel(CDymMagicModel *pModel,CDymScriptLoader* pLoader);
	/// �ӽű��ж�ȡ���ӷ�����
	BOOL LoadParticles(CDymMagicParticles *pParticles,CDymScriptLoader* pLoader);
	BOOL LoadParticles2(CDymMagicParticles *pParticles,CDymScriptLoader* pLoader);
	/// �ӽű��ж�ȡ�ؼ�֡
	BOOL LoadKeyFrame(CDymMagicKeyFrame *p,CDymScriptLoader* pLoader);

	void _DelObj( CDymMagicBaseObj *p );
	void ComputeRotationID();
	void ResetParticlesPlayTime();
public:
	/// ������������Ϊ�ⲿʹ�ã���Ч����ʹ�ø�����
	int m_nExData;
	/// ������ת��������
	BOOL m_bDisableRotation;

	CDymMagic( CDymRenderSystem *pDevice , CDymSoundManager *pSoundManager );
	~CDymMagic(void);
	/// ���ļ��д���
	BOOL LoadFromFile(LPCTSTR fileName);
	/// ���ڴ��д���
	BOOL LoadFromMemory( void *pData , int datalen );
	/// �ӽű��д���
	BOOL LoadFromLoader(CDymScriptLoader* pLoader);
	/// ������һ����Ч���ںϣ�
	BOOL Import( CDymMagic *pMagic );
	/// ֡ѭ��
	BOOL FrameMove(float fElapsedTime);
	/// ��Ⱦ
	HRESULT Render( DWORD flag );
	/// �ͷ�
	void Clear(void);
	/// ����Ϊ������Ⱦ
	void SetFastRender( BOOL bFast );

	// �����������
	void SetPosition(D3DXVECTOR3 &v);
	void SetScaling(D3DXVECTOR3 v);
	void SetRotation(float a);
	void SetRotationAxis(D3DXVECTOR3 &v);
	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetScaling();
	D3DXVECTOR3 GetRotationAxis();
	float GetRotation();
	void SetMagicColor( float r , float g , float b , float a );

	// ��������
	std::list<CDymMagicKeyFrame*>::iterator m_itSoundKey;
	float m_fLastFrameTime;

	void ComputeSound();
	void _AddSound( CDymMagicKeyFrameManager *pManager );
	void FrameMoveForSound( float skiptime );
	void StopSound();
	void RenderSoundRadio();
	void _RenderSoundRadio( CDymMagicKeyFrameManager *pManager );

private:
	void ComputeKeyMaxTime(void);

	// ������ײ���
	BOOL CDymMagic::Pick(const D3DXVECTOR3* pVertexStreamZeroData,UINT VertexStreamZeroStride,int facenum,POINT *pmousePt,D3DXMATRIX *pMul,float *pDisOut,D3DXVECTOR3 *pPosOut);
	BOOL CDymMagic::M3D_IntersectTriangle( const D3DXVECTOR3& orig,
                                       const D3DXVECTOR3& dir, D3DXVECTOR3& v0,
                                       D3DXVECTOR3& v1, D3DXVECTOR3& v2,
                                       FLOAT* t, FLOAT* u, FLOAT* v ,D3DXVECTOR3 *pOut);
	HRESULT CDymMagic::_D3DXIntersect3(CONST void *pDataStream,CONST int facenum,CONST int Stride,CONST D3DXVECTOR3 * pRayPos,CONST D3DXVECTOR3 * pRayDir,
		BOOL * pHit,DWORD * pFaceIndex,FLOAT * pU,FLOAT * pV,FLOAT * pDist,LPD3DXBUFFER * ppAllHits,DWORD * pCountOfHits ,
		D3DXMATRIX *pmatWorld1 = NULL ,D3DXMATRIX *pmatWorld2 = NULL ,int StrideForBlend = 0);

	int NewID();
	float _GetSoundArea( CDymMagicKeyFrameManager *pManager );
public:
	/// ����ģ��
	int CreateModel( MAGIC_CREATE_TYPE type , void *pUnknow );
	/// �������ӷ�����
	int CreateParticles( void *pUnknow );
	/// ��������������һ������
	int CopyCreate( int id , void *pUnknow );
	int CopyCreate( CDymMagicBaseObj *pObj , void *pUnkonw );
	/// ����֡������֡��
	CDymMagicKeyFrame* CopyKeyFrame( CDymMagicKeyFrame *pKeyFrame );
	/// ��ײ��⣬���������Ļ���꣬���ض���ID��δ��ײ������
	int Collision(int moustx,int mousty);
	/// ɾ������
	BOOL DelObj(int id );
	/// ��ȡ����
	CDymMagicBaseObj *GetObj(int id);
	CDymMagicBaseObj *GetObj ( CDymMagicKeyFrame *pKey );
	/// ��ȡ��������
	int GetObjNum();
	/// ��ȡ���ж���ID����������ָ��ʹ�С
	void GetAllObj(int*idBuffer,int buffersize);
	/// ��Ⱦѡȡ��
	void RenderSelection(int* ObjIDindex, int indexSize);
	/// ��ʼ����
	BOOL Play();
	/// ֹͣ����
	BOOL Stop();
	/// �Ƿ����ڲ���
	BOOL isPlaying( float fElapsedTime );
	/// ��ȡ��ǰ����ʱ�� 
	float GetCurTime();
	/// ���õ�ǰ����ʱ�� 
	void SetCurTime(float t);
	/// ͨ���ؼ�֡�����ȡ�ùؼ�֡�Ĺ�����
	CDymMagicKeyFrameManager *FindKeyManager( CDymMagicKeyFrame *pKey );
	/// ������󲥷�ʱ��
	void SetKeyMaxTime(float t);
	/// �����󲥷�ʱ��
	float GetKeyMaxTime();
	/// ����Ƿ��ظ�����
	BOOL isRepeat();
	/// �����Ƿ��ظ�����
	void SetRepeat( BOOL bRepeat );
	/// ���ؼ�֡����һ����ͼ
	void SetTexture( CDymMagicKeyFrame *pKey , LPCTSTR filename );
	/// ���ؼ�֡����һ������
	void SetSound( CDymMagicKeyFrame *pKey , LPCTSTR filename );
	/// ������Ч�ű�
	BOOL Save( LPCTSTR fullpathfilename , void **ppSaveBuffer , int *pBufferSize );
	/// ����ؼ�֡
	void SaveKeyFrame( CDymScriptLoader *pLoader , CDymMagicKeyFrameManager *pKeyMana );
	/// �ı����ID
	BOOL ChangeID( int oldid , int newid );
	/// ���ö�������һ��������ת
	void SetRotationWithID( int objid , int rotationid );
	/// ���ö��������һ������
	void SetFollowWithID( int objid , int followid );
	/// ����󶨺�
	void ComputeBoundingBox();
	/// ��ýű���ľ�̬�󶨺�
	void GetBoundingBox( D3DXVECTOR3 *pMin , D3DXVECTOR3 *pMax );
	/// ���ʵʱ����󶨺�
	void GetBoundingBoxRealTime( D3DXVECTOR3 *pMin , D3DXVECTOR3 *pMax );
	/// ���ò��������������
	void SetDefaultShadowPos( D3DXVECTOR3 v1 , D3DXVECTOR3 v2 );
	/// ����Ĭ�Ϲ����Ƶ�������ͼ
	void SetDefaultBillboardString( LPCTSTR str , DWORD color , void *pUnknow );
	/// ����ȫ������
	void SetGlobleScaling( D3DXVECTOR3 vSca );
	/// ���ȫ������
	D3DXVECTOR3 GetGlobleScaling();
	/// �������ӷ��������������ӱ��ĸ���������
	void SetParticlesAcceptID( int particlesobjid );
	/// �������ж�������йؼ�֡��ͼ
	void SetAllKeyTexture( LPCTSTR texFileName );
	/// �����Ƿ�ѭ��������Ч
	void SetLoop( BOOL bLoop );
	/// ����Ƿ��ػ�������Ч
	BOOL GetLoop();
	/// ���������Χ
	float GetSoundArea();
};

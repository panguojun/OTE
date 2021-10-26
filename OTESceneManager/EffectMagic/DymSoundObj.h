#pragma once
#include <mmsystem.h>
#include <dsound.h>
#include "DymSoundBuffer.h"

class CDymSoundObj
{
	LPDIRECTSOUNDBUFFER     m_pDSBuffer     ;        //����������ָ��
	LPDIRECTSOUND3DBUFFER   m_pDS3DBuffer   ;   // 3D ��Դ����ָ��
	DS3DBUFFER              m_dsBufferParams;               // 3D buffer properties
	DS3DLISTENER            m_dsListenerParams;             // Listener properties
	CWaveFile * m_pWaveFile;
	/// ���������
	float m_fCameraX;
	float m_fCameraY;
	float m_fCameraZ;
	/// ��Դ����
	float m_fPosX;
	float m_fPosY;
	float m_fPosZ;
	/// ��Դ��Χ
	float m_fRadio;
	/// ��Դ����
	CDymSoundBuffer *m_pSoundBuffer;
public:
	/// ��������Ƿ��Զ�ɾ��
	BOOL m_bAutoDelete;

	CDymSoundObj(void);
	~CDymSoundObj(void);
	/// ���ڴ��д�������
	HRESULT CreateFromMemory( CDymSoundBuffer *pBuffer , LPDIRECTSOUND8 pDirectSound , void *pData , int len , LPTSTR nameID );
	/// ���ļ��д�������
	HRESULT Create( CDymSoundBuffer *pBuffer , LPDIRECTSOUND8 pDirectSound , LPTSTR strFileName );
	/// ����������˽�У�
	HRESULT _Create( CDymSoundBuffer *pBuffer , LPDIRECTSOUND8 pDirectSound );

	int Play( BOOL bLoop );
	void Stop();
	void SetPos( float x , float y ,float z );
	void SetCameraPos( float x , float y , float z );
	/// ������Դ��Χ
	void SetRadio( float r );
	void Release();
	/// �Ƿ����ڲ���
	BOOL isPlaying();
};

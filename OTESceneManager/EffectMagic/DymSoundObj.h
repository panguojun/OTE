#pragma once
#include <mmsystem.h>
#include <dsound.h>
#include "DymSoundBuffer.h"

class CDymSoundObj
{
	LPDIRECTSOUNDBUFFER     m_pDSBuffer     ;        //辅助缓冲区指针
	LPDIRECTSOUND3DBUFFER   m_pDS3DBuffer   ;   // 3D 声源对象指针
	DS3DBUFFER              m_dsBufferParams;               // 3D buffer properties
	DS3DLISTENER            m_dsListenerParams;             // Listener properties
	CWaveFile * m_pWaveFile;
	/// 摄像机坐标
	float m_fCameraX;
	float m_fCameraY;
	float m_fCameraZ;
	/// 声源坐标
	float m_fPosX;
	float m_fPosY;
	float m_fPosZ;
	/// 声源范围
	float m_fRadio;
	/// 资源缓冲
	CDymSoundBuffer *m_pSoundBuffer;
public:
	/// 播放完后是否自动删除
	BOOL m_bAutoDelete;

	CDymSoundObj(void);
	~CDymSoundObj(void);
	/// 从内存中创建声音
	HRESULT CreateFromMemory( CDymSoundBuffer *pBuffer , LPDIRECTSOUND8 pDirectSound , void *pData , int len , LPTSTR nameID );
	/// 从文件中创建声音
	HRESULT Create( CDymSoundBuffer *pBuffer , LPDIRECTSOUND8 pDirectSound , LPTSTR strFileName );
	/// 创建声音（私有）
	HRESULT _Create( CDymSoundBuffer *pBuffer , LPDIRECTSOUND8 pDirectSound );

	int Play( BOOL bLoop );
	void Stop();
	void SetPos( float x , float y ,float z );
	void SetCameraPos( float x , float y , float z );
	/// 设置声源范围
	void SetRadio( float r );
	void Release();
	/// 是否正在播放
	BOOL isPlaying();
};

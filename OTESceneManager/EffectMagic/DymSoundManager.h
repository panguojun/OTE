#pragma once

#include "DymSoundObj.h"
#include <list>

class __declspec( dllexport ) CDymSoundManager
{
protected:
	LPDIRECTSOUND8          m_pDsd          ;   //Directsound 对象指针
	LPDIRECTSOUND3DLISTENER m_pDSListener   ;   // 3D 听者之中呢
	std::list< CDymSoundObj* > m_listSoundObj;
	float m_fCameraX , m_fCameraY , m_fCameraZ;
	CDymSoundBuffer *m_pSoundBuffer;
public:
	CDymSoundManager(void);
	~CDymSoundManager(void);
	HRESULT init( HWND hWnd );
	CDymSoundObj *CreateSound( LPTSTR filename );
	void Release();
	void SetPos( float x , float y , float z );
	void RemoveSound( CDymSoundObj *p );
	void FrameMove( float camera_x , float camera_y , float camera_z , float lookat_x , float lookat_y , float lookat_z );
	virtual HRESULT CreateSoundFromFile( LPTSTR filename , CDymSoundObj *pObj );
};

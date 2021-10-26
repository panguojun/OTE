#include <windows.h>
// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include ".\dymsoundmanager.h"

#define   WAVECHANNEL   2
#define   WAVESAMPLEPERSEC  44100
#define   WAVEBITSPERSAMPLE 16
#define   MAX_AUDIO_BUF 3
#define   BUFFERNOTIFYSIZE  7680

CDymSoundManager::CDymSoundManager(void)
{
	m_pDsd               = NULL;   //Directsound 对象指针
	m_pDSListener         = NULL;   // 3D 听者之中呢
	m_fCameraX = 0.0f;
	m_fCameraY = 0.0f;
	m_fCameraZ = 0.0f;
	m_pSoundBuffer = new CDymSoundBuffer();
}

CDymSoundManager::~CDymSoundManager(void)
{
	Release();
}

HRESULT CDymSoundManager::init ( HWND hWnd )
{
	//初始化Dsound
	HRESULT hr;
	if(FAILED(hr = DirectSoundCreate8(NULL,&m_pDsd,NULL)))
		return hr;
	if(FAILED(hr = m_pDsd->SetCooperativeLevel(hWnd,DSSCL_PRIORITY)))
		return hr;
	//初始化Directsound 的主缓冲区，并设置格式
	LPDIRECTSOUNDBUFFER  pDSBPrimary = NULL;
	DSBUFFERDESC   dsbdesc ;
	ZeroMemory(&dsbdesc,sizeof(DSBUFFERDESC));
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
	if(FAILED(hr = m_pDsd->CreateSoundBuffer(&dsbdesc,&pDSBPrimary ,NULL)))
		return hr;

	if( FAILED( hr = pDSBPrimary->QueryInterface( IID_IDirectSound3DListener, (VOID**)&m_pDSListener ) ) )
		return hr;

	WAVEFORMATEX wfx;
	ZeroMemory( &wfx, sizeof(WAVEFORMATEX) ); 
	wfx.wFormatTag      = (WORD) WAVE_FORMAT_PCM; 
	wfx.nChannels       = WAVECHANNEL   ; 
	wfx.nSamplesPerSec  = WAVESAMPLEPERSEC  ; 
	wfx.wBitsPerSample  = WAVEBITSPERSAMPLE ; 
	wfx.nBlockAlign     = (WORD) (wfx.wBitsPerSample / 8 * wfx.nChannels);
	wfx.nAvgBytesPerSec = (DWORD) (wfx.nSamplesPerSec * wfx.nBlockAlign);
	if( FAILED( hr = pDSBPrimary->SetFormat(&wfx) ) )
		return hr;

	pDSBPrimary->Release ();

	return S_OK;
}
HRESULT CDymSoundManager::CreateSoundFromFile( LPTSTR filename , CDymSoundObj *pObj )
{
	return pObj->Create ( m_pSoundBuffer , m_pDsd , filename );
}
CDymSoundObj *CDymSoundManager::CreateSound( LPTSTR filename )
{
	CDymSoundObj *pObj = new CDymSoundObj();
	if( FAILED( CreateSoundFromFile( filename , pObj ) ) )
	{
		delete pObj ;
		return NULL;
	}
	pObj->SetCameraPos ( m_fCameraX , m_fCameraY , m_fCameraZ );
	m_listSoundObj.push_back ( pObj );
	return pObj;
}
void CDymSoundManager::Release()
{
	for( std::list< CDymSoundObj * >::iterator it = m_listSoundObj.begin () ; it != m_listSoundObj.end ();it++ )
	{
		CDymSoundObj *p = *it;
		delete p;
	}
	m_listSoundObj.clear ();
	if( m_pSoundBuffer ){delete m_pSoundBuffer;m_pSoundBuffer=NULL;}
	if( m_pDSListener ){m_pDSListener->Release ();m_pDSListener=NULL;}
	if( m_pDsd ){m_pDsd->Release ();m_pDsd=NULL;}
}
void CDymSoundManager::SetPos( float x , float y , float z )
{
	DS3DLISTENER buffer;
	buffer.dwSize = sizeof(DS3DBUFFER);
	m_pDSListener->GetAllParameters( &buffer );
	buffer.vPosition.x = x;
	buffer.vPosition.y = y;
	buffer.vPosition.z = z;
	m_pDSListener->SetAllParameters( &buffer , DS3D_IMMEDIATE );
}
void CDymSoundManager::FrameMove( float camera_x , float camera_y , float camera_z , float lookat_x , float lookat_y , float lookat_z )
{
	m_fCameraX = camera_x;
	m_fCameraY = camera_y;
	m_fCameraZ = camera_z;
	for( std::list< CDymSoundObj * >::iterator it = m_listSoundObj.begin () ; it != m_listSoundObj.end ();it++ )
	{
		CDymSoundObj *p = *it;
		if( p->isPlaying () == FALSE )
		{
			if( p->m_bAutoDelete )
			{
				it--;
				m_listSoundObj.remove ( p );
				delete p;
			}
		}
		else
			p->SetCameraPos ( camera_x , camera_y , camera_z );
	}
}
void CDymSoundManager::RemoveSound( CDymSoundObj *p )
{
	m_listSoundObj.remove ( p );
	//delete p;
}
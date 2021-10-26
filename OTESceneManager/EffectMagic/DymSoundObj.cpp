#include <windows.h>
// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include ".\dymsoundobj.h"

CDymSoundObj::CDymSoundObj(void)
{
	m_pDSBuffer     = NULL;        //辅助缓冲区指针
	m_pDS3DBuffer         = NULL;   // 3D 声源对象指针
	m_pWaveFile = NULL;
	m_fCameraX = 0.0f ;
	m_fCameraY = 0.0f ;
	m_fCameraZ = 0.0f ;
	m_fPosX = 0.0f ;
	m_fPosY = 0.0f ;
	m_fPosZ = 0.0f ;
	m_fRadio = 0.0f;
	m_bAutoDelete = FALSE;
	m_pSoundBuffer = NULL;
}

CDymSoundObj::~CDymSoundObj(void)
{
	Release();
}
HRESULT CDymSoundObj::_Create( CDymSoundBuffer *pBuffer , LPDIRECTSOUND8 pDirectSound )
{
	WAVEFORMATEX* pwfx  = m_pWaveFile->GetFormat();
	if(pwfx == NULL)
		return E_FAIL;
	if(pwfx->nChannels > 1)
	{
		//AfxMessageBox("only sourport wave file with one channel ");
		return E_FAIL;
	}
	if( pwfx->wFormatTag != WAVE_FORMAT_PCM )
	{
         //AfxMessageBox("not wave file ");
		 return E_FAIL;
	}
	HRESULT hr;
	DSBUFFERDESC dsbd;
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRL3D| DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY |DSBCAPS_GETCURRENTPOSITION2;
	//看看创建辅助缓冲区的buffer时设置的标志，3D的标志自然是需要的，还有一个标志需要注意，DSBCAPS_CTRLPOSITIONNOTIFY，如果你采用的流（stream）buffer的话，就需要边播放，边向buffer中填充数据，就需要设置这个标志，这样，在directsound播放到指定位置时，就会触发事件。
	//dsbd.dwBufferBytes =MAX_AUDIO_BUF * BUFFERNOTIFYSIZE ;//如果采用流buffer，可以设置适当的buffer大小。
	dsbd.dwBufferBytes =m_pWaveFile->GetSize(); // 如果采用静态buffer，那么buffer的大小就是文件的大小了。
	dsbd.guid3DAlgorithm = DS3DALG_NO_VIRTUALIZATION;
	dsbd.lpwfxFormat = m_pWaveFile->m_pwfx;
	if(FAILED(hr = pDirectSound->CreateSoundBuffer(&dsbd,&m_pDSBuffer,NULL)))
		return hr;
	//通过辅助缓冲区，来获取3D buffer的指针。
	if(FAILED(hr = m_pDSBuffer->QueryInterface(IID_IDirectSound3DBuffer, (VOID**)&m_pDS3DBuffer )))
		return hr;

	m_dsBufferParams.dwSize = sizeof(DS3DBUFFER);
	m_pDS3DBuffer->GetAllParameters( &m_dsBufferParams );

	//设置3Dbuffer的属性。
	m_dsBufferParams.dwMode = DS3DMODE_HEADRELATIVE;
	m_pDS3DBuffer->SetAllParameters( &m_dsBufferParams, DS3D_IMMEDIATE );

	return S_OK;
}
HRESULT CDymSoundObj::CreateFromMemory( CDymSoundBuffer *pBuffer , LPDIRECTSOUND8 pDirectSound , void *pData , int len , LPTSTR nameID )
{
	m_pSoundBuffer = pBuffer;
	TCHAR path[MAX_PATH];
	::GetTempPath ( MAX_PATH , path );
	_tcscat( path , "\\tmps~.tmp" );
	FILE *pFile = fopen( path , "wb" );
	fwrite( pData , len , 1 , pFile );
	fclose( pFile );
	m_pWaveFile = pBuffer->CreateSound ( path , nameID );//new CWaveFile;
//	m_pWaveFile->Open(path,NULL,WAVEFILE_READ);
	DeleteFile( path );
	return _Create( pBuffer , pDirectSound );
}
HRESULT CDymSoundObj::Create( CDymSoundBuffer *pBuffer , LPDIRECTSOUND8 pDirectSound , LPTSTR strFileName )
{
	m_pSoundBuffer = pBuffer;
	m_pWaveFile = pBuffer->CreateSound ( strFileName , strFileName );//new CWaveFile;
//	m_pWaveFile->Open(strFileName,NULL,WAVEFILE_READ);
	/*FILE *pFile = fopen( strFileName , "rb" );
	if(!pFile)return FALSE;
	fseek(pFile,0,SEEK_END);
	int nFileSize = ftell(pFile);
	fseek(pFile,0,SEEK_SET);
	char *pFileBuffer = new char[nFileSize+1];
	fread(pFileBuffer,nFileSize,1,pFile);
	fclose(pFile);
	return CreateFromMemory( pBuffer , pDirectSound , pFileBuffer , nFileSize );*/

	return _Create( pBuffer , pDirectSound );
}
int CDymSoundObj::Play( BOOL bLoop )
{
	if( m_pDSBuffer == NULL || m_pWaveFile == NULL )return 0;
	LPVOID lplockbuf;
	DWORD len;
	DWORD dwWrite;

	m_pDSBuffer->Lock(0,0,&lplockbuf,&len,NULL,NULL,DSBLOCK_ENTIREBUFFER);
	m_pWaveFile->Read((BYTE*)lplockbuf,len,&dwWrite);
	m_pDSBuffer->Unlock(lplockbuf,len,NULL,0);
	m_pDSBuffer->SetCurrentPosition(0);
	DWORD dwFlag = 0;
	if( bLoop )dwFlag |= DSBPLAY_LOOPING;
	m_pDSBuffer->Play(0,0,dwFlag);
	return 1;
}
void CDymSoundObj::SetCameraPos( float x , float y , float z )
{
	/*if( m_fRadio != 0.0f )
	{
		x /= m_fRadio/4.0f;
		y /= m_fRadio/4.0f;
		z /= m_fRadio/4.0f;
	}*/
	if( m_pDS3DBuffer == NULL )return;
	if( m_fCameraX != x || m_fCameraY != y || m_fCameraZ != z )
	{
		m_fCameraX = x;
		m_fCameraY = y;
		m_fCameraZ = z;
		DS3DBUFFER buffer;
		buffer.dwSize = sizeof(DS3DBUFFER);
		m_pDS3DBuffer->GetAllParameters( &buffer );
		buffer.vPosition.x = m_fPosX - m_fCameraX;
		buffer.vPosition.y = m_fPosY - m_fCameraY;
		buffer.vPosition.z = m_fPosZ - m_fCameraZ;
		m_pDS3DBuffer->SetAllParameters( &buffer , DS3D_IMMEDIATE );
	}
}

void CDymSoundObj::SetPos( float x , float y ,float z )
{
	if( m_pDS3DBuffer == NULL )return;
	m_fPosX = x;
	m_fPosY = y;
	m_fPosZ = z;
	/*if( m_fRadio != 0.0f )
	{
		m_fPosX = x / m_fRadio/4.0f;
		m_fPosY = y / m_fRadio/4.0f;
		m_fPosZ = z / m_fRadio/4.0f;
	}*/
	DS3DBUFFER buffer;
	buffer.dwSize = sizeof(DS3DBUFFER);
	m_pDS3DBuffer->GetAllParameters( &buffer );
	buffer.vPosition.x = m_fPosX - m_fCameraX;
	buffer.vPosition.y = m_fPosY - m_fCameraY;
	buffer.vPosition.z = m_fPosZ - m_fCameraZ;
	m_pDS3DBuffer->SetAllParameters( &buffer , DS3D_IMMEDIATE );
}
void CDymSoundObj::SetRadio( float r )
{
	if( m_pDS3DBuffer == NULL )return;
	m_fRadio = r;
	DS3DBUFFER buffer;
	buffer.dwSize = sizeof(DS3DBUFFER);
	m_pDS3DBuffer->GetAllParameters( &buffer );
	buffer.flMinDistance = r/10.0f+0.5f;
	//buffer.dwOutsideConeAngle = 360000;
	m_pDS3DBuffer->SetAllParameters( &buffer , DS3D_IMMEDIATE );
}
void CDymSoundObj::Stop()
{
	if( m_pDSBuffer )
		m_pDSBuffer->Stop();
}
void CDymSoundObj::Release()
{
	Stop();
	if(m_pDSBuffer){m_pDSBuffer->Release ();m_pDSBuffer=NULL;}
	if(m_pDS3DBuffer){m_pDS3DBuffer->Release ();m_pDS3DBuffer=NULL;}
	if(m_pWaveFile){m_pSoundBuffer->ReleaseSound ( m_pWaveFile );m_pWaveFile=NULL;}
}
BOOL CDymSoundObj::isPlaying()
{
	if( m_pDSBuffer )
	{
		DWORD dwStatus=0;
		m_pDSBuffer->GetStatus(&dwStatus);   
		return dwStatus & DSBSTATUS_PLAYING;   
	}
	return FALSE;
}


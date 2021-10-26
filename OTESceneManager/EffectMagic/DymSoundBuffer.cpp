#include ".\dymsoundbuffer.h"

CDymSoundBuffer::CDymSoundBuffer(void)
{
}

CDymSoundBuffer::~CDymSoundBuffer(void)
{
	for( std::list< CDymSoundBuffer::SOUND_BUFFER* >::iterator it = m_listBuffer.begin () ; it != m_listBuffer.end () ; it ++ )
	{
		CDymSoundBuffer::SOUND_BUFFER *p = *it;
		p->pWave->Close ();
		delete p->pWave ;
		delete p;
	}
	m_listBuffer.clear ();
}

CWaveFile *CDymSoundBuffer::CreateSound( LPTSTR filename , LPTSTR fileid )
{
	CDymSoundBuffer::SOUND_BUFFER *pSB2 = FindBuffer( filename );
	if( pSB2 )
	{
		if( pSB2->nCount <= 0 )
		{
			pSB2->nCount ++;
			pSB2->pWave->ResetFile ();
			return pSB2->pWave ;
		}
	}
	CWaveFile *pNew = new CWaveFile();
	CDymSoundBuffer::SOUND_BUFFER *pSB = new CDymSoundBuffer::SOUND_BUFFER;
	ZeroMemory( pSB , sizeof( CDymSoundBuffer::SOUND_BUFFER ) );
	_tcscpy( pSB->filename , fileid );
	pSB->nCount = 1;
	pSB->pWave = pNew;
	pNew->Open(filename,NULL,WAVEFILE_READ);
	AddBuffer( pSB );
	return pNew;
}
void CDymSoundBuffer::AddBuffer( CDymSoundBuffer::SOUND_BUFFER *pSB )
{
	m_listBuffer.push_back ( pSB );
	if( m_listBuffer.size () > 50 )
	{
		std::list< CDymSoundBuffer::SOUND_BUFFER* >::iterator it = m_listBuffer.end () ;
		it--;
		CDymSoundBuffer::SOUND_BUFFER *p = *it;
		if( p->nCount <= 0 )
		{
			p->pWave->Close ();
			delete p->pWave ;
			m_listBuffer.remove ( p );
			delete p;
		}
	}
}
void CDymSoundBuffer::ReleaseSound( CWaveFile *pWave )
{
	//delete pWave;
	CDymSoundBuffer::SOUND_BUFFER *p = FindBuffer( pWave );
	if( p )
	{
		p->nCount --;
		if( p->nCount <=0 )
		{
			//p->pWave->Close ();
		}
	}
}

CDymSoundBuffer::SOUND_BUFFER *CDymSoundBuffer::FindBuffer( LPTSTR filename )
{
	for( std::list< CDymSoundBuffer::SOUND_BUFFER* >::iterator it = m_listBuffer.begin () ; it != m_listBuffer.end () ; it ++ )
	{
		CDymSoundBuffer::SOUND_BUFFER *p = *it;
		if( p->nCount <= 0 )
			if( _tcscmp( p->filename , filename ) == 0 )
				return p;
	}
	return NULL;
}

CDymSoundBuffer::SOUND_BUFFER *CDymSoundBuffer::FindBuffer( CWaveFile *pWave )
{
	for( std::list< CDymSoundBuffer::SOUND_BUFFER* >::iterator it = m_listBuffer.begin () ; it != m_listBuffer.end () ; it ++ )
	{
		CDymSoundBuffer::SOUND_BUFFER *p = *it;
		if( p->pWave == pWave )
			return p;
	}
	return NULL;
}
#pragma once
#include "DymWaveFile.h"
#include <list>

class CDymSoundBuffer
{
	struct SOUND_BUFFER
	{
		CWaveFile *pWave;
		int nCount;
		TCHAR filename[32];
	};
	std::list< SOUND_BUFFER* > m_listBuffer;

	SOUND_BUFFER *FindBuffer( LPTSTR filename );
	SOUND_BUFFER *FindBuffer( CWaveFile *pWave );
	void AddBuffer( CDymSoundBuffer::SOUND_BUFFER *pSB );
public:
	CDymSoundBuffer(void);
	~CDymSoundBuffer(void);
	CWaveFile *CreateSound( LPTSTR filename , LPTSTR fileid );
	void ReleaseSound( CWaveFile *pWave );
};

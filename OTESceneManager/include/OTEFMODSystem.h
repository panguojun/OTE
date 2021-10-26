#pragma once
#include "OTEStaticInclude.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#include "math.h"

#define MAX_CHANNELS		4				// ���ͬʱ���ű���������Ŀ

namespace OTE
{
// *********************************************
// COTEFMODSystem
// FMODϵͳ
// *********************************************

class _OTEExport COTEFMODSystem
{
public:

	// ��ʼ��

	void  InitSystem();

	// ����

	void Clear();
	
	// ����ѭ��

	void Proccess();	

public: /// 3D ��Ч

	// ������Ч
	
	FMOD::Sound* Create3DSound(const std::string& fileName, bool isLoop = true);

	void Play3DSoundAt(FMOD::Sound* sd, const FMOD_VECTOR& pos, 
							const FMOD_VECTOR& vel, float volume = 1.0f);

	// ���ò���λ��

	void Set3DSoundPos(FMOD::Sound* sd, const FMOD_VECTOR& pos,
							const FMOD_VECTOR& vel);

	// �Ƴ�����

	void Stop3DSound(FMOD::Sound* sd);

	// ���ö���λ��

	void SetListener(const FMOD_VECTOR& pos, const FMOD_VECTOR& vel);


public: /// ��������

	// ���ű�������
		
	FMOD::Sound* CreateBKSound(const std::string& fileName, int channelIndex);

	void PlayBKSound(int channelIndex, float volume = 0.5f, bool bPlay = true, bool isLoop = false);

	void StopBKSound(int channelIndex);

	bool IsBKSoundPlaying(int channelIndex);

	void SetBKSoundVolume(int channelIndex, float volume);

	void GetBKSoundVolume(int channelIndex, float* volume);

	void SetBKSoundPaused(int channelIndex);

	void SetBKSoundContinue(int channelIndex);

public:

	static COTEFMODSystem		s_Inst;

protected:	

	struct BKMusicAttr_t 
	{
		FMOD::Sound*	sd;
		FMOD::Channel*	chl;
	};

	// ����
	
	BKMusicAttr_t				m_BKMusics[MAX_CHANNELS];

	// Ƶ��

	typedef std::pair<FMOD::Sound*, int>	Channel_t;

	std::list<Channel_t>		m_ChannelList;

	std::list<Channel_t>		m_GraveChannelList;
	
	FMOD::System*				m_System;

};

// *********************************************
// CSimpleBKSoundPlayer
// �򵥵ı�������������
// *********************************************

class _OTEExport CSimpleBKSoundPlayer
{
public:

	static void PlaySoundA(const std::string& fileName, float volume);
	
	static void StopSound();

};


}
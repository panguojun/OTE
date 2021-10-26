#pragma once
#include "OTEStaticInclude.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#include "math.h"

#define MAX_CHANNELS		4				// 最大同时播放背景音乐数目

namespace OTE
{
// *********************************************
// COTEFMODSystem
// FMOD系统
// *********************************************

class _OTEExport COTEFMODSystem
{
public:

	// 初始化

	void  InitSystem();

	// 清理

	void Clear();
	
	// 播放循环

	void Proccess();	

public: /// 3D 生效

	// 创建声效
	
	FMOD::Sound* Create3DSound(const std::string& fileName, bool isLoop = true);

	void Play3DSoundAt(FMOD::Sound* sd, const FMOD_VECTOR& pos, 
							const FMOD_VECTOR& vel, float volume = 1.0f);

	// 设置播放位置

	void Set3DSoundPos(FMOD::Sound* sd, const FMOD_VECTOR& pos,
							const FMOD_VECTOR& vel);

	// 移出声音

	void Stop3DSound(FMOD::Sound* sd);

	// 设置耳朵位置

	void SetListener(const FMOD_VECTOR& pos, const FMOD_VECTOR& vel);


public: /// 背景音乐

	// 播放背景音乐
		
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

	// 音乐
	
	BKMusicAttr_t				m_BKMusics[MAX_CHANNELS];

	// 频道

	typedef std::pair<FMOD::Sound*, int>	Channel_t;

	std::list<Channel_t>		m_ChannelList;

	std::list<Channel_t>		m_GraveChannelList;
	
	FMOD::System*				m_System;

};

// *********************************************
// CSimpleBKSoundPlayer
// 简单的背景声音播放器
// *********************************************

class _OTEExport CSimpleBKSoundPlayer
{
public:

	static void PlaySoundA(const std::string& fileName, float volume);
	
	static void StopSound();

};


}
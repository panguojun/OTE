#include "OTEQTSceneManager.h"
#include "OTEFMODSystem.h"
#include "fmod.hpp"
#include "fmod_errors.h"

// --------------------------------------
using namespace Ogre;
using namespace OTE;

// ==============================================
// COTEFMODSystem
// ==============================================

// 错误报告
static void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
		char err[256];
        sprintf(err, "FMOD error! (%d) %s\n", result, (char*)FMOD_ErrorString(result));
		OTE_LOGMSG("FMOD 错误！" << err );
		//throw;
        return;
    }
}

// --------------------------------------
// 渲染回调

void FMODSystem_RenderCB(int ud)
{
	TIMECOST_START
	COTEFMODSystem::s_Inst.Proccess();
	TIMECOST_END
}

// --------------------------------------
COTEFMODSystem		COTEFMODSystem::s_Inst;

// --------------------------------------
void COTEFMODSystem::InitSystem()
{
	// 清理

	memset(m_BKMusics, 0, sizeof(BKMusicAttr_t) * MAX_CHANNELS);

	// 初始化系统

	unsigned int    version;
	FMOD_RESULT		result;
    FMOD_SPEAKERMODE speakermode;
    FMOD_CAPS        caps;
    char             name[256];	
	
    result = FMOD::System_Create(&m_System);
    ERRCHECK(result);
    
    result = m_System->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
		char err[256];
        sprintf(err, "Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
		MessageBox(NULL, err, "FMOD版本错误！", MB_OK);
		return;
    }
    
    result = m_System->getDriverCaps(0, &caps, 0, 0, &speakermode);
    ERRCHECK(result);

    result = m_System->setSpeakerMode(speakermode);       /* Set the user selected speaker mode. */
    ERRCHECK(result);

    if (caps & FMOD_CAPS_HARDWARE_EMULATED)             /* The user has the 'Acceleration' slider set to off!  This is really bad for latency!. */
    {                                                   /* You might want to warn the user about this. */
        result = m_System->setDSPBufferSize(1024, 10);
        ERRCHECK(result);
    }

    result = m_System->getDriverInfo(0, name, 256, 0);
    ERRCHECK(result);

    if (strstr(name, "SigmaTel"))   /* Sigmatel sound devices crackle for some reason if the format is PCM 16bit.  PCM floating point output seems to solve it. */
    {
        result = m_System->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD_DSP_RESAMPLER_LINEAR);
        ERRCHECK(result);
    }

    result = m_System->init(100, FMOD_INIT_NORMAL, 0);
    if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)         /* Ok, the speaker mode selected isn't supported by this soundcard.  Switch it back to stereo... */
    {
        result = m_System->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
        ERRCHECK(result);
                
        result = m_System->init(100, FMOD_INIT_NORMAL, 0);/* ... and re-init. */
        ERRCHECK(result);
    }
		
	// 设置3d音响的距离单位

    result = m_System->set3DSettings(1.0, 1.0f, 1.0f);
    ERRCHECK(result);

	// 渲染回调

	SCENE_MGR->AddListener(COTEQTSceneManager::eRenderCB, FMODSystem_RenderCB);

	OTE_LOGMSG("fmod 声音模块初始化完成")

}

// --------------------------------------
void COTEFMODSystem::Proccess()
{	
	m_System->update();

	// 把耳朵放到摄像机上

	Ogre::Camera* cam = SCENE_CAM;
	FMOD_VECTOR listenerPos = { cam->getPosition().x, cam->getPosition().y, cam->getPosition().z };
	FMOD_VECTOR speed = { 0.0f, 0.0f, 0.0f };

	this->SetListener(listenerPos, speed);


}

// --------------------------------------
void COTEFMODSystem::Clear()
{
	FMOD_RESULT result;

	//for(int i = 0; i < MAX_CHANNELS; i ++)
	//{
	//	if(m_BKMusics[i].sd)
	//		m_BKMusics[i].sd->release();
	//}

	// 系统清理

    result = m_System->close();
    ERRCHECK(result);
    result = m_System->release();
    ERRCHECK(result);

	OTE_LOGMSG("fmod 声音模块系统清理完成")

}


// --------------------------------------
// 3D音效
// --------------------------------------
FMOD::Sound* COTEFMODSystem::Create3DSound(const std::string& fileName, bool isLoop)
{	
	FMOD_RESULT result;

	// 创建

	FMOD::Sound*		sd;

	Ogre::String filePath;
	Ogre::String fileFull;
	OTEHelper::GetFullPath(fileName, filePath, fileFull);

	result = m_System->createSound(filePath.c_str(), FMOD_3D, 0, &sd);
    ERRCHECK(result);

	// 默认设置

    result = sd->set3DMinMaxDistance(0.5f, 50.0f);		// 远近距离
    ERRCHECK(result);
	if(isLoop)
		result = sd->setMode(FMOD_LOOP_NORMAL);			// 播放模式
	else
		result = sd->setMode(FMOD_LOOP_OFF);			// 播放模式

    ERRCHECK(result);

	OTE_LOGMSG("创建3D音效: " << fileName)

	return sd;

}

// --------------------------------------
void COTEFMODSystem::Play3DSoundAt(FMOD::Sound* sd, const FMOD_VECTOR& pos,
										const FMOD_VECTOR& vel, float volume)
{	
	FMOD_RESULT result;
	FMOD::Channel*	channel;
		
	result = m_System->playSound(FMOD_CHANNEL_FREE, sd, true, &channel);
	ERRCHECK(result);		

	channel->setVolume(volume);
	
	// 把频道放到userdata 里 

	sd->setUserData((void*)channel);

	Set3DSoundPos(sd, pos, vel);

	result = channel->setPaused(false);

	ERRCHECK(result);
}

// --------------------------------------
void COTEFMODSystem::Set3DSoundPos(FMOD::Sound* sd, const FMOD_VECTOR& pos,
								const FMOD_VECTOR& vel)
{
	FMOD_RESULT		result;	
	void*	channel;

	result = sd->getUserData( &channel );
	ERRCHECK(result);

	if(channel)
	{
		bool isPlaying = true;
		((FMOD::Channel*)channel)->isPlaying(&isPlaying);
		if(isPlaying)
		{
			result = ((FMOD::Channel*)channel)->set3DAttributes(&pos, &vel);
			ERRCHECK(result);
		}
	}
}

// --------------------------------------
void COTEFMODSystem::Stop3DSound(FMOD::Sound* sd)
{				
	sd->release();	
}

// --------------------------------------	
void COTEFMODSystem::SetListener(const FMOD_VECTOR& pos, const FMOD_VECTOR& vel)
{
	FMOD_VECTOR forward        = { 0.0f, 0.0f, 1.0f };
    FMOD_VECTOR up             = { 0.0f, 1.0f, 0.0f };

	FMOD_RESULT result = FMOD_System_Set3DListenerAttributes((FMOD_SYSTEM*)m_System, 0, &pos, &vel, &forward, &up);
    ERRCHECK(result);
}


// --------------------------------------
// 背景音乐
// --------------------------------------
FMOD::Sound* COTEFMODSystem::CreateBKSound(const std::string& fileName, int channelIndex)
{
	assert(channelIndex < MAX_CHANNELS);

	FMOD_RESULT result;
	
	FMOD::Sound* sd = m_BKMusics[channelIndex].sd;
	
	if(sd)
	{
		char soundName[256];
		result = sd->getName(soundName, 256);
		ERRCHECK(result);
		
		if(fileName == soundName)		
			return sd;

		sd->release();
		
	}

	// 创建
		
	Ogre::String filePath;
	Ogre::String fileFull;
	OTEHelper::GetFullPath(fileName, filePath, fileFull);

	result = m_System->createSound(fileFull.c_str(), FMOD_DEFAULT, 0, &m_BKMusics[channelIndex].sd);
    ERRCHECK(result);

	OTE_LOGMSG("创建背景音乐: " << fileName << " 频道索引 = " << channelIndex)

	return sd;

}

// --------------------------------------
void COTEFMODSystem::PlayBKSound(int channelIndex, float volume, bool bPlay, bool isLoop)
{
	assert(channelIndex < MAX_CHANNELS);
	
	FMOD_RESULT		result;

	if(bPlay)
	{
		if(isLoop)
			result = m_BKMusics[channelIndex].sd->setMode(FMOD_LOOP_NORMAL);		
		else
			result = m_BKMusics[channelIndex].sd->setMode(FMOD_LOOP_OFF);	

		result = m_System->playSound(FMOD_CHANNEL_FREE, m_BKMusics[channelIndex].sd, true, &m_BKMusics[channelIndex].chl);
		ERRCHECK(result);

		m_BKMusics[channelIndex].chl->setVolume(volume);

		result = m_BKMusics[channelIndex].chl->setPaused(false);

		ERRCHECK(result);
	}
	else
	{
		StopBKSound(channelIndex);
	}

	char name[32];
	m_BKMusics[channelIndex].sd->getName(name, 32);
	OTE_LOGMSG("播放背景音乐: " << name << " 频道索引 = " << channelIndex)

}

// --------------------------------------
void COTEFMODSystem::StopBKSound(int channelIndex)
{	
	assert(channelIndex < MAX_CHANNELS);

	FMOD::Sound* sd = m_BKMusics[channelIndex].sd;
	
	if(sd)
	{
		char name[32];
		m_BKMusics[channelIndex].sd->getName(name, 32);

		OTE_LOGMSG("停止背景音乐: " << name << " 频道索引 = " << channelIndex)
		
		sd->release();	
		m_BKMusics[channelIndex].sd = NULL;
	}	
}

// --------------------------------------
void COTEFMODSystem::SetBKSoundVolume(int channelIndex, float volume)
{
	assert(channelIndex < MAX_CHANNELS);
	m_BKMusics[channelIndex].chl->setVolume(volume);
}

// --------------------------------------
void COTEFMODSystem::GetBKSoundVolume(int channelIndex, float* volume)
{
	assert(channelIndex < MAX_CHANNELS);

	m_BKMusics[channelIndex].chl->getVolume(volume);
}

// --------------------------------------
void COTEFMODSystem::SetBKSoundPaused(int channelIndex)
{
	assert(channelIndex < MAX_CHANNELS);
	m_BKMusics[channelIndex].chl->setPaused(true);
}
// --------------------------------------
void COTEFMODSystem::SetBKSoundContinue(int channelIndex)
{
	assert(channelIndex < MAX_CHANNELS);
	m_BKMusics[channelIndex].chl->setPaused(false);
}

// --------------------------------------
bool COTEFMODSystem::IsBKSoundPlaying(int channelIndex)
{
	assert(channelIndex < MAX_CHANNELS);

	FMOD::Channel* chl = m_BKMusics[channelIndex].chl;
	
	if(chl)
	{
		bool isPlaying;
		chl->isPlaying( &isPlaying );

		return isPlaying;
	}

	return false;
}


// ==============================================
// CSimpleBKSoundPlayer
// ==============================================

void CSimpleBKSoundPlayer::PlaySoundA(const std::string& fileName, float volume)
{
	COTEFMODSystem::s_Inst.CreateBKSound(fileName, 0);
	COTEFMODSystem::s_Inst.PlayBKSound(0, volume);

	OTE_LOGMSG("播放背景音乐: " << fileName)

}

// ----------------------------------------------
void CSimpleBKSoundPlayer::StopSound()
{
	COTEFMODSystem::s_Inst.StopBKSound(0);
}
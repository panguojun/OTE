#pragma once
#include "OTEStaticInclude.h"
#include "OTEActTrackUsage.h"
#include "OTETrackCtrller.h"
#include "EventObjBase.h"
#include "OTEEntityReactor.h"

/********************************************
 ********************************************

	Entity, particlesystem, xyEffect, sound ... 统称为magic 或者成为 "效果对象"。
	magic(可以是被邦定对象本身) + Track/Reactor = effect，既 "效果"
	效果effect的概念还可以延伸成： "一个单元的变化", 将包括摄像机动画,地表动画等。
	{effect} = Action， 是一个effect的集合。

	Action 是整个引擎事件驱动机制的核心

 ********************************************
 ********************************************/

namespace OTE
{
class COTEActorEntity;

// ********************************************
// ActMagicData_t
// magic 的数据对象
// ********************************************

struct ActMagicData_t
{
	enum EType{eTrack, eReactor};

	/// 属性

	char					MagicFile[256];	// magic资源文件

	char					InterData[256];	// 作用数据（暂时是反应器数据）
	EType					InterType;		// 作用类型
	int						InterID;		// 作用ID

	int						ID;				// 标号

	float					PlaySpeed;		// 播放速度
	float					PlayTime;		// 播放时刻

	Ogre::MovableObject*	pMovableObj;	// 对象

	/// 方法

	ActMagicData_t(EType interType) : InterType(interType)
	{
		::ZeroMemory(MagicFile, sizeof(MagicFile));
		::ZeroMemory(InterData, sizeof(InterData));

		InterType = eTrack;
		InterID = 0;

		
		PlayTime = 0.0f;
		PlaySpeed = 1.0f;
		ID = 0;

		pMovableObj = NULL;		
	}	

	inline ActMagicData_t& operator= (const ActMagicData_t& other)
	{	
		strcpy(MagicFile, other.MagicFile);
		strcpy(InterData, other.InterData);
		InterType = other.InterType;
		InterID = other.InterID;
		PlaySpeed = other.PlaySpeed;
		PlayTime = other.PlayTime;

		return *this;
	}

	static ActMagicData_t* CreateActMagic(EType interType = eTrack)
	{
		return new ActMagicData_t(interType);
	}
		
	static void DeleteActMagic(ActMagicData_t* am)
	{
		delete am;
	}
};

// ************************************************ 
//特效绑定资源列表<ActMagicData_t, id>
typedef HashMap<int, ActMagicData_t*>				MagBindTrackData_t;

//特效轨道绑定实体列表			
typedef std::map<Ogre::MovableObject*, CInterBase*>		MagBindInterList_t; 


// ************************************************ 
// COTEActionData
// ************************************************

class _OTEExport COTEActionData
{	
public:
	
	/// 属性
		
	std::string							m_ResName;			//	名字	

	std::string							m_AniName;			//	使用动作的名字
		
	float								m_AniSpeed;			//	动作的速度	

	bool								m_IsAutoCreate;		//	是否为动态创建之行动(自动创建的行动不会保存文件)

	float								m_InureTime;		//	行动生效时间		

	float								m_Length;			//	行动长度

	float								m_IsAutoPlay;		//	是否自动播放 	

	float								m_CurrentDelay; 	//	当前延迟时间 
			
	std::string							m_3DSoundName;		//	3D声音

	float								m_3DSoundVolume;	//	声音大小	

	bool								m_Is3DSoundLoop;	//	声音是否循环

	MagBindTrackData_t					m_MagicInterMapData;//	特效与轨道的资源绑定表

public:

	COTEActionData()
	{
		m_InureTime = 10.0f; // 默认10s击中目标   			
		m_AniSpeed = 1.0f;	
		m_3DSoundVolume = 1.0f;
		m_Is3DSoundLoop = false;

		m_CurrentDelay = 0;	
		m_IsAutoPlay = false;
		m_IsAutoCreate = false;
	}	
	
	// 克隆

	void Clone(COTEActionData* des);

	static bool CloneInterDatas(COTEActionData* srcAct, COTEActionData* desAct);

	// 添加/移除 Effect数据

	void AddEffectData(ActMagicData_t* Mag, int InterId);

	void RemoveEffectData(int magID);

	// 设置声音属性

	void SetSound(const std::string& soundName, float soundVol, bool isLoop = true)
	{
		m_3DSoundName = soundName;
		m_3DSoundVolume = soundVol;
		m_Is3DSoundLoop = isLoop;
	}

	bool static GetIDFromName(const std::string rActionName, int& rID, std::string& rTail);

private:	

	// 添加/移除 Effect数据
	
	static void AddEffectData(ActMagicData_t* Mag, MagBindTrackData_t& magicDataMap);
	
	static void RemoveEffectData(int magID, MagBindTrackData_t& magicDataMap);


};


// ********************************************
// COTEAction 行动类
// 是一种使得物件呈现出动态效果变化的动画
// 或可能叫做COTEAnimation更贴切一点
// 这个名称有一定的被动含义
// ********************************************

class _OTEExport COTEAction : public COTEActionData
{
public:
	//行动的播放模式
	typedef enum{ePLAY_ONCE, ePAUSE, eCONTINUES, eSTOP} PlayMode;

	//构造函数 / 析构函数
	COTEAction(COTEActorEntity* creator);	

	~COTEAction();

public:

	// 创建
	static COTEAction* CreateAction(COTEActorEntity* ae);

	//播放行动
	void Play(PlayMode mode = ePLAY_ONCE);

	//停止行动
	void Stop(bool isInvorkCB = true);

	//暂停行动
	void Pause();

	//帧处理
	PlayMode Proccess(float time);

	//将动作调整到指定时间点
	void MoveFrameTo(float time);

	//停止特效播放
	void ClearEffects();

	/// 当前动画播放状态

	PlayMode GetMode()
	{
		return	m_Mode;
	}
	void SetMode(PlayMode pm )
	{
		m_Mode = pm;
	}

	///名字

	std::string GetName()
	{
		return	m_Name;
	}
	void SetName(const std::string& name)
	{
		m_Name = name;
	}		
	
	// 设置当前延时

	void SetCurDelay(float delay)
	{
		m_CurrentDelay = delay;
	}	
	
	/// 动画

	bool SetAnimation(const std::string& aniName);

	const std::string& GetAnimation()
	{
		return m_AniName;
	}

	void SetCreator(COTEActorEntity* ae)
	{
		m_Creator = ae;
	}

	// 将资源一次性创建到内存

	void FlushRes(bool forceLoad = false);

public:	

	//得到一个Magic (注意：InterId 可以是轨道动画的轨道id，也可以是反应器id)
	Ogre::MovableObject* GetMagByInterID(int InterId);

	//得到一个作用ID
	int GetInterIDByMag(Ogre::MovableObject* mo);

	/// 效果属性设置

	float GetPlaySpeed(int magID);

	void SetPlaySpeed(int magID, float speed = 1.0f);	
	
	float GetPlayTime(int magID);
	
	void SetPlayTime(int magID, float time = 0.0f);	

	/// 动态轨道

	COTETrackCtrller* GetTrackCtrllerInstance();

	bool HasTrackCtrller()
	{
		return m_TrackCtrller != NULL;
	}

	void BindEffectToTarget(int interID, Ogre::MovableObject* target);

	void BindEffectsToTarget(Ogre::MovableObject* Target);	

	void UnbindEffectFromTarget(int InterID);

	void UnbindEffectsFromTarget();

	// 更新特效与轨道的位置

	void UpdateTrackMagicPos();

	// 更新反应器

	void UpdateReactors(float dTime);

private:

	// 添加magic + track　实体

	void AddMagicTrack(Ogre::MovableObject* Mag, COTETrack* Track);

	// 反应器帮定

	void BindReactors();

	void UnbindReactors();		
	
public:	

	/// 状态

	std::string							m_Name;				//	行动的名字		
	
	MagBindInterList_t					m_MagicInterMap;	//	特效与作用绑定表

    float								m_Time;				//	行动执行的时间

	PlayMode							m_Mode;				//	行动的播放模式	

	COTEActorEntity*					m_Creator;			//	行动创建者	

protected:

	COTETrackCtrller*					m_TrackCtrller;		//	远程目标管理

	void*								m_3DSoundPtr;		//	3D声音ＩＤ	

	int									m_PlayStepFlag;		//  播放分段记录

};

}
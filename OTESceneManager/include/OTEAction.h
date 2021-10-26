#pragma once
#include "OTEStaticInclude.h"
#include "OTEActTrackUsage.h"
#include "OTETrackCtrller.h"
#include "EventObjBase.h"
#include "OTEEntityReactor.h"

/********************************************
 ********************************************

	Entity, particlesystem, xyEffect, sound ... ͳ��Ϊmagic ���߳�Ϊ "Ч������"��
	magic(�����Ǳ��������) + Track/Reactor = effect���� "Ч��"
	Ч��effect�ĸ����������ɣ� "һ����Ԫ�ı仯", ���������������,�ر����ȡ�
	{effect} = Action�� ��һ��effect�ļ��ϡ�

	Action �����������¼��������Ƶĺ���

 ********************************************
 ********************************************/

namespace OTE
{
class COTEActorEntity;

// ********************************************
// ActMagicData_t
// magic �����ݶ���
// ********************************************

struct ActMagicData_t
{
	enum EType{eTrack, eReactor};

	/// ����

	char					MagicFile[256];	// magic��Դ�ļ�

	char					InterData[256];	// �������ݣ���ʱ�Ƿ�Ӧ�����ݣ�
	EType					InterType;		// ��������
	int						InterID;		// ����ID

	int						ID;				// ���

	float					PlaySpeed;		// �����ٶ�
	float					PlayTime;		// ����ʱ��

	Ogre::MovableObject*	pMovableObj;	// ����

	/// ����

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
//��Ч����Դ�б�<ActMagicData_t, id>
typedef HashMap<int, ActMagicData_t*>				MagBindTrackData_t;

//��Ч�����ʵ���б�			
typedef std::map<Ogre::MovableObject*, CInterBase*>		MagBindInterList_t; 


// ************************************************ 
// COTEActionData
// ************************************************

class _OTEExport COTEActionData
{	
public:
	
	/// ����
		
	std::string							m_ResName;			//	����	

	std::string							m_AniName;			//	ʹ�ö���������
		
	float								m_AniSpeed;			//	�������ٶ�	

	bool								m_IsAutoCreate;		//	�Ƿ�Ϊ��̬����֮�ж�(�Զ��������ж����ᱣ���ļ�)

	float								m_InureTime;		//	�ж���Чʱ��		

	float								m_Length;			//	�ж�����

	float								m_IsAutoPlay;		//	�Ƿ��Զ����� 	

	float								m_CurrentDelay; 	//	��ǰ�ӳ�ʱ�� 
			
	std::string							m_3DSoundName;		//	3D����

	float								m_3DSoundVolume;	//	������С	

	bool								m_Is3DSoundLoop;	//	�����Ƿ�ѭ��

	MagBindTrackData_t					m_MagicInterMapData;//	��Ч��������Դ�󶨱�

public:

	COTEActionData()
	{
		m_InureTime = 10.0f; // Ĭ��10s����Ŀ��   			
		m_AniSpeed = 1.0f;	
		m_3DSoundVolume = 1.0f;
		m_Is3DSoundLoop = false;

		m_CurrentDelay = 0;	
		m_IsAutoPlay = false;
		m_IsAutoCreate = false;
	}	
	
	// ��¡

	void Clone(COTEActionData* des);

	static bool CloneInterDatas(COTEActionData* srcAct, COTEActionData* desAct);

	// ���/�Ƴ� Effect����

	void AddEffectData(ActMagicData_t* Mag, int InterId);

	void RemoveEffectData(int magID);

	// ������������

	void SetSound(const std::string& soundName, float soundVol, bool isLoop = true)
	{
		m_3DSoundName = soundName;
		m_3DSoundVolume = soundVol;
		m_Is3DSoundLoop = isLoop;
	}

	bool static GetIDFromName(const std::string rActionName, int& rID, std::string& rTail);

private:	

	// ���/�Ƴ� Effect����
	
	static void AddEffectData(ActMagicData_t* Mag, MagBindTrackData_t& magicDataMap);
	
	static void RemoveEffectData(int magID, MagBindTrackData_t& magicDataMap);


};


// ********************************************
// COTEAction �ж���
// ��һ��ʹ��������ֳ���̬Ч���仯�Ķ���
// ����ܽ���COTEAnimation������һ��
// ���������һ���ı�������
// ********************************************

class _OTEExport COTEAction : public COTEActionData
{
public:
	//�ж��Ĳ���ģʽ
	typedef enum{ePLAY_ONCE, ePAUSE, eCONTINUES, eSTOP} PlayMode;

	//���캯�� / ��������
	COTEAction(COTEActorEntity* creator);	

	~COTEAction();

public:

	// ����
	static COTEAction* CreateAction(COTEActorEntity* ae);

	//�����ж�
	void Play(PlayMode mode = ePLAY_ONCE);

	//ֹͣ�ж�
	void Stop(bool isInvorkCB = true);

	//��ͣ�ж�
	void Pause();

	//֡����
	PlayMode Proccess(float time);

	//������������ָ��ʱ���
	void MoveFrameTo(float time);

	//ֹͣ��Ч����
	void ClearEffects();

	/// ��ǰ��������״̬

	PlayMode GetMode()
	{
		return	m_Mode;
	}
	void SetMode(PlayMode pm )
	{
		m_Mode = pm;
	}

	///����

	std::string GetName()
	{
		return	m_Name;
	}
	void SetName(const std::string& name)
	{
		m_Name = name;
	}		
	
	// ���õ�ǰ��ʱ

	void SetCurDelay(float delay)
	{
		m_CurrentDelay = delay;
	}	
	
	/// ����

	bool SetAnimation(const std::string& aniName);

	const std::string& GetAnimation()
	{
		return m_AniName;
	}

	void SetCreator(COTEActorEntity* ae)
	{
		m_Creator = ae;
	}

	// ����Դһ���Դ������ڴ�

	void FlushRes(bool forceLoad = false);

public:	

	//�õ�һ��Magic (ע�⣺InterId �����ǹ�������Ĺ��id��Ҳ�����Ƿ�Ӧ��id)
	Ogre::MovableObject* GetMagByInterID(int InterId);

	//�õ�һ������ID
	int GetInterIDByMag(Ogre::MovableObject* mo);

	/// Ч����������

	float GetPlaySpeed(int magID);

	void SetPlaySpeed(int magID, float speed = 1.0f);	
	
	float GetPlayTime(int magID);
	
	void SetPlayTime(int magID, float time = 0.0f);	

	/// ��̬���

	COTETrackCtrller* GetTrackCtrllerInstance();

	bool HasTrackCtrller()
	{
		return m_TrackCtrller != NULL;
	}

	void BindEffectToTarget(int interID, Ogre::MovableObject* target);

	void BindEffectsToTarget(Ogre::MovableObject* Target);	

	void UnbindEffectFromTarget(int InterID);

	void UnbindEffectsFromTarget();

	// ������Ч������λ��

	void UpdateTrackMagicPos();

	// ���·�Ӧ��

	void UpdateReactors(float dTime);

private:

	// ���magic + track��ʵ��

	void AddMagicTrack(Ogre::MovableObject* Mag, COTETrack* Track);

	// ��Ӧ���ﶨ

	void BindReactors();

	void UnbindReactors();		
	
public:	

	/// ״̬

	std::string							m_Name;				//	�ж�������		
	
	MagBindInterList_t					m_MagicInterMap;	//	��Ч�����ð󶨱�

    float								m_Time;				//	�ж�ִ�е�ʱ��

	PlayMode							m_Mode;				//	�ж��Ĳ���ģʽ	

	COTEActorEntity*					m_Creator;			//	�ж�������	

protected:

	COTETrackCtrller*					m_TrackCtrller;		//	Զ��Ŀ�����

	void*								m_3DSoundPtr;		//	3D�����ɣ�	

	int									m_PlayStepFlag;		//  ���ŷֶμ�¼

};

}
#pragma once
#include "otereactorbase.h"
#include "OTEPathFinding.h"
#include "OTEActorEntity.h"

namespace OTE
{
// ***********************************************
// COTEActorMoveAIReactor 
// ***********************************************
class _OTEExport COTEActorMoveAIReactor : public CReactorBase
{
public:
	// ����
	virtual void Init(Ogre::MovableObject* mo);
	// ����
	virtual void Update(Ogre::MovableObject* mo, float dTime);	

	// ����Ŀ���
	void SetTargetPos(const Ogre::Vector3& targetPos)
	{
		m_TargetPos = targetPos;
	}

	// ����������λ��
	void SetCursorPos(const Ogre::Vector2& cursorPos)
	{
		m_CursorPos = cursorPos;
	}
	void SetCursorPos(float x, float y)
	{
		m_CursorPos.x = x;
		m_CursorPos.y = y;
	}

protected:
	
	// Ѱ·�߷�
	bool UpdatePathFindingTarget(COTEActorEntity* ae);

	// ����������
	bool UpdateCursorTarget(COTEActorEntity* ae);

	// ����λ��
	bool SetPosition(COTEActorEntity* ae, const Ogre::Vector3& pos);

protected:

	Ogre::Vector3				m_TargetPos;				// Ŀ���

	Ogre::Vector2				m_CursorPos;				// ����

	std::list<HistoryNode_t>	m_WayMap;					// Ѱ·���

};

// ***********************************************
// COTEActorMoveAIReactorFactory 
// ***********************************************
class COTEActorMoveAIReactorFactory : public CReactorFactoryBase
{
public:

	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
	{
		if(!mo || mo->getMovableType() == "OTEEntity")
			return new COTEActorMoveAIReactor();
		return NULL;
	}
	
	virtual void Delete(CReactorBase* rb)
	{
		SAFE_DELETE(rb)
	}

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "��ɫ�ƶ�����1";
	}
};


// ***********************************************
// COTEActorMoveToTargetAIReactor 
// ��ս���о������н�ɫ�ƶ���Ŀ�������
// ��������½�ɫһ�㲻������ײ����
// ***********************************************
class _OTEExport COTEActorMoveToTargetAIReactor : public CReactorBase
{
public:

	typedef void(*EventCB_t)(); 

public:

	// ���� 
	virtual void Create(){}
	// ����
	virtual void Init(Ogre::MovableObject* mo);
	// ����
	virtual void Update(Ogre::MovableObject* mo, float dTime);	

	// ����Ŀ���
	void SetTargetPos(const Ogre::Vector3& targetPos)
	{
		m_TargetPos = targetPos;
	}	
	
	// �ص�����
	void AddEventCB(EventCB_t cb)
	{
		m_EventCBs.push_back(cb);
	}

	void ClearEventCBs()
	{
		m_EventCBs.clear();
	}

protected:

	Ogre::Vector3			m_TargetPos;				// Ŀ���

	std::list<EventCB_t>	m_EventCBs;	

};

// ***********************************************
// COTEActorMoveToTargetAIReactorFactory 
// ***********************************************
class COTEActorMoveToTargetAIReactorFactory : public CReactorFactoryBase
{
public:

	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
	{
		if(!mo || mo->getMovableType() == "OTEEntity")
			return new COTEActorMoveToTargetAIReactor();
		return NULL;
	}
	
	virtual void Delete(CReactorBase* rb)
	{
		SAFE_DELETE(rb)
	}
	
	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "��ɫ�ƶ�����2";
	}
};

}
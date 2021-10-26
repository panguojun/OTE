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
	// 触发
	virtual void Init(Ogre::MovableObject* mo);
	// 更新
	virtual void Update(Ogre::MovableObject* mo, float dTime);	

	// 设置目标点
	void SetTargetPos(const Ogre::Vector3& targetPos)
	{
		m_TargetPos = targetPos;
	}

	// 设置鼠标控制位置
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
	
	// 寻路走法
	bool UpdatePathFindingTarget(COTEActorEntity* ae);

	// 跟随鼠标鼠标
	bool UpdateCursorTarget(COTEActorEntity* ae);

	// 设置位置
	bool SetPosition(COTEActorEntity* ae, const Ogre::Vector3& pos);

protected:

	Ogre::Vector3				m_TargetPos;				// 目标点

	Ogre::Vector2				m_CursorPos;				// 鼠标点

	std::list<HistoryNode_t>	m_WayMap;					// 寻路结果

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
		rDesc = "角色移动控制1";
	}
};


// ***********************************************
// COTEActorMoveToTargetAIReactor 
// 在战斗中经常会有角色移动到目标点的情况
// 这种情况下角色一般不考虑碰撞问题
// ***********************************************
class _OTEExport COTEActorMoveToTargetAIReactor : public CReactorBase
{
public:

	typedef void(*EventCB_t)(); 

public:

	// 创建 
	virtual void Create(){}
	// 触发
	virtual void Init(Ogre::MovableObject* mo);
	// 更新
	virtual void Update(Ogre::MovableObject* mo, float dTime);	

	// 设置目标点
	void SetTargetPos(const Ogre::Vector3& targetPos)
	{
		m_TargetPos = targetPos;
	}	
	
	// 回调函数
	void AddEventCB(EventCB_t cb)
	{
		m_EventCBs.push_back(cb);
	}

	void ClearEventCBs()
	{
		m_EventCBs.clear();
	}

protected:

	Ogre::Vector3			m_TargetPos;				// 目标点

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
		rDesc = "角色移动控制2";
	}
};

}
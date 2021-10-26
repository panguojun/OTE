#pragma once
#include "otereactorbase.h"
#include "OTEActorEntity.h"

namespace OTE
{
// ***********************************************
// COTEActorMoveAIReactor 
// ***********************************************
class _OTEExport COTEActorStreightMoveAIReactor : public CReactorBase
{
public:
	// 更新
	virtual void Update(Ogre::MovableObject* mo, float dTime);	

public:

	// 初始化
	void Init(Ogre::MovableObject* mo, float maxMoveVec, float maxMoveAcc, float maxFrasc);

	// 设置移动范围
	void SetMoveRange(const Ogre::Vector3& pt1, const Ogre::Vector3& pt2)
	{
		m_Pt1 = pt1;
		m_Pt2 = pt2;
	}

	// 移动速度
	Ogre::Vector3 GetMoveVec()
	{
		if(m_MoveVec.length() > m_MaxMoveVec)
			m_MoveVec = m_MoveVec.normalisedCopy() * m_MaxMoveVec;
		return m_MoveVec;
	}

	// 摩擦力
	void SetMaxFrasc(float maxFrasc)
	{
		m_MaxFrasc = maxFrasc;
	}

	// 左移
	void MoveLeft(Ogre::MovableObject* mo);

    // 右移
	void MoveRight(Ogre::MovableObject* mo);

    // 停止
	void Stop(float maxFrasc = -1.0f);

public:

	///	数据

	// 第一点
	Ogre::Vector3		m_Pt1;		

	// 第二点
	Ogre::Vector3		m_Pt2;

	/// 状态

	// 加速度
	Ogre::Vector3       m_MoveAcc;	

	// 速度
	Ogre::Vector3       m_MoveVec;	

	// 摩擦因子
	float				m_Frasc;

protected:
	
	/// 最大值

	float				m_MaxMoveVec;

	float				m_MaxMoveAcc;

	float				m_MaxFrasc;

};


// ***********************************************
// COTEActorStreightMoveAIReactorFactory 
// ***********************************************
class COTEActorStreightMoveAIReactorFactory : public CReactorFactoryBase
{
public:

	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
	{
		if(!mo || mo->getMovableType() == "OTEEntity")
			return new COTEActorStreightMoveAIReactor();
		return NULL;
	}
	
	virtual void Delete(CReactorBase* rb)
	{
		SAFE_DELETE(rb)
	}

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "角色移动控制3";
	}
};

// ***********************************************
// COTENpcActorMoveAIReactor 
// ***********************************************
class _OTEExport COTENpcActorStreightMoveAIReactor : public COTEActorStreightMoveAIReactor
{
public:	

	// 更新

	virtual void Update(Ogre::MovableObject* mo, float dTime);	

	/// 目标位置(人工智能)

	void SetTargetPos(const Ogre::Vector3& targetPos){m_TargetPos = targetPos;}

	const Ogre::Vector3& GetTargetPos(){return m_TargetPos;}

protected:

	// 人工智能

	void NpcThink(Ogre::MovableObject* );

protected:
	
	// 目标位置
	Ogre::Vector3	m_TargetPos;

};


// ***********************************************
// COTENpcActorStreightMoveAIReactorFactory 
// ***********************************************
class COTENpcActorStreightMoveAIReactorFactory : public CReactorFactoryBase
{
public:

	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
	{
		if(!mo || mo->getMovableType() == "OTEEntity")
			return new COTENpcActorStreightMoveAIReactor();
		return NULL;
	}
	
	virtual void Delete(CReactorBase* rb)
	{
		SAFE_DELETE(rb)
	}

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "角色移动控制４";
	}
};

}

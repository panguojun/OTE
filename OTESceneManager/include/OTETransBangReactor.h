#pragma once
#include "OTETransBangReactor.h"
#include "OTEReactorBase.h"
#include "OTEActorEntity.h" 

namespace OTE
{
// ***********************************
// class COTETransBangReactor
// 震动反应器
// ***********************************

class _OTEExport COTETransBangReactor : public CReactorBase
{
public:
	// 初始化
	virtual void Init(Ogre::MovableObject* mo);
	// 更新
	virtual void Update(Ogre::MovableObject* mo, float dTime);

protected:

	float				m_InterTime;
	
};


// ***********************************************
// COTETransBangReactorFactory 
// ***********************************************
class COTETransBangReactorFactory : public CReactorFactoryBase
{
public:

	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
	{		
		return new COTETransBangReactor();
	}
	
	virtual void Delete(CReactorBase* rb)
	{
		SAFE_DELETE(rb)
	}

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "震动1";
	}
};

// ***********************************
// class COTEFreeDropReactor
// 震动反应器
// ***********************************

class _OTEExport COTEFreeDropReactor : public CReactorBase
{
public:	
	COTEFreeDropReactor() : m_InterTime(0.0f){}
	// 初始化
	virtual void Init(Ogre::MovableObject* mo);
	// 更新
	virtual void Update(Ogre::MovableObject* mo, float dTime);

	void SetInterTime(float interTime){m_InterTime = interTime;}

	void SetGravity(const Ogre::Vector3& g){m_Gravity = g;}

	void SetPlane(float d, const Ogre::Vector3& n){m_Plane = Ogre::Plane(n, d);}

protected:

	// 状态 
	
	Ogre::Vector3		m_Vec;			// 运动速度

	float				m_TimeEsp;		// 作用时间

	// 数据

	float				m_InterTime;	// 作用时间
	
	Ogre::Vector3		m_Gravity;		// 重力加速度

	Ogre::Plane         m_Plane;		// 碰撞平面 
	
};


// ***********************************************
// COTEFreeDropReactorFactory 
// ***********************************************
class COTEFreeDropReactorFactory : public CReactorFactoryBase
{
public:

	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
	{		
		return new COTETransBangReactor();
	}
	
	virtual void Delete(CReactorBase* rb)
	{
		SAFE_DELETE(rb)
	}

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "自由落体";
	}
};

}

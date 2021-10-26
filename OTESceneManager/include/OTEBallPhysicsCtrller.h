#pragma once

#include "OTEStaticInclude.h"
#include "OTEReactorBase.h"

namespace OTE
{		
	// ***********************************
	// COTEBallPhysicsCtrller
	// 钢球物理学
	// ***********************************
	class _OTEExport COTEBallPhysicsCtrller : public CReactorBase
	{
	public:

		// 初始化
		virtual void Init(Ogre::MovableObject* mo);

		// 更新
		virtual void Update(Ogre::MovableObject* mo, float dTime);	

	public:

		// 反弹

		void Hit(const Ogre::Vector3& normal, float strength = 0);

		// 设置速度

		void SetVec(const Ogre::Vector3& vec);
		
		const Ogre::Vector3& GetVec(){return m_Vec;}

		// 摩擦

		void SetFra(float frac){m_Frac = frac;}
		
		float GetFra(){return m_Frac;}

		// 最大速度

		void SetMaxVec(float maxVec){m_MaxVec = maxVec;}
		
		float GetMaxVec(){return m_MaxVec;}

		// 水平线

		void SetGroundHgt(float groundHgt){m_GroundHgt = groundHgt;}

		float GetGroundHgt(){return m_GroundHgt;}

	protected:

		// 速度

		Ogre::Vector3		m_Vec;

		// 摩擦

		float				m_Frac;

		// 最大速度限制

		float				m_MaxVec;

		// 水平线高度

		float				m_GroundHgt;

	};

	
// ***********************************************
// COTEBallPhysicsCtrllerFactory 
// ***********************************************
class COTEBallPhysicsCtrllerFactory : public CReactorFactoryBase
{
public:

	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
	{
		if(!mo || mo->getMovableType() == "OTEEntity")
			return new COTEBallPhysicsCtrller();
		return NULL;
	}
	
	virtual void Delete(CReactorBase* rb)
	{
		SAFE_DELETE(rb)
	}

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "钢球控制1";
	}
};


}
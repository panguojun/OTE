#include "otetransbangreactor.h"
#include "OTED3DManager.h"
#include "OTEQTSceneManager.h"

using namespace OTE;
using namespace Ogre;

// ====================================
// COTETransBangReactor
// ====================================
void COTETransBangReactor::Init(Ogre::MovableObject* mo)
{
	m_InterTime = 0;
}

// ------------------------------------
void COTETransBangReactor::Update(Ogre::MovableObject* mo, float dTime)
{
	const float c_LoopTime = 0.1f;  // 周期
	const float c_A = 0.02f;		// 振幅

	if(m_InterTime > c_LoopTime)
		return;	

	if(mo->getMovableType() == "Camera"	)
	{		
		Ogre::Vector3 Pos = ((Ogre::Camera*)mo)->getPosition();
		Pos.y += c_A * Ogre::Math::Sin(m_InterTime * (2.0f * Ogre::Math::PI) / c_LoopTime);
		((Ogre::Camera*)mo)->setPosition(Pos);		
	}
	else
	{
		Ogre::Vector3 Pos = mo->getParentSceneNode()->getPosition();
		Pos.y += c_A * Ogre::Math::Sin(m_InterTime * (2.0f * Ogre::Math::PI) / c_LoopTime);
		mo->getParentSceneNode()->setPosition(Pos);			
	}

	m_InterTime += dTime;
}


// ====================================
// COTEFreeDropReactor
// ====================================

void COTEFreeDropReactor::Init(Ogre::MovableObject* mo)
{
	m_Vec = Ogre::Vector3::ZERO;
	m_TimeEsp = 0;
}

// ------------------------------------
void COTEFreeDropReactor::Update(Ogre::MovableObject* mo, float dTime)
{
	if(m_TimeEsp > m_InterTime)
		return;

	m_TimeEsp += dTime;

	// 位置更新

	Ogre::Vector3 pos = mo->getParentSceneNode()->getPosition();
	m_Vec += m_Gravity * dTime;
	Ogre::Vector3 newPos = pos + m_Vec * dTime;

	if(m_Plane.getSide(newPos) != m_Plane.getSide(pos))
	{
		// 跟平面碰撞

		Ogre::Ray r(pos, (newPos - pos).normalisedCopy());
		std::pair<bool, Real> hitResult = r.intersects(m_Plane);
		if(hitResult.first)
		{
			// 计算位置

			Ogre::Vector3 hitPos = r.getPoint(hitResult.second);
			mo->getParentSceneNode()->setPosition(hitPos);
			
			// 计算碰撞速度变化

			float vec = m_Vec.length();
			m_Vec = m_Vec + m_Plane.normal * (m_Vec.dotProduct(m_Plane.normal) * 2.0f);		
		}
	
	}
	else
		mo->getParentSceneNode()->setPosition(newPos);
}
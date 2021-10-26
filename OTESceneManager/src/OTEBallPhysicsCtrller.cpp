#include "oteballphysicsctrller.h"
// ------------------------------------
using namespace OTE;
using namespace Ogre;

// ----------------------------------------
// 初始化
void COTEBallPhysicsCtrller::Init(Ogre::MovableObject* mo)
{
	m_Vec = Ogre::Vector3::ZERO;
	//m_MaxVec = 50.0f;
	//m_GroundHgt = 0.0f;
}

// ----------------------------------------
// 更新
void COTEBallPhysicsCtrller::Update(Ogre::MovableObject* mo, float dTime)
{
	if(m_Vec.isZeroLength())
		return;

	// 最大速度限制 

	if(m_Vec.length() > m_MaxVec)
	{
		m_Vec = m_Vec.normalisedCopy() * m_MaxVec;
	}

	// 移动

	m_Vec -= m_Vec.normalisedCopy() * m_Vec.length() * m_Frac;

	/*Ogre::Vector3 dir = m_Vec.normalisedCopy();

	m_Vec -= dir * dTime * m_Frac;
	
	if(dir.dotProduct(m_Vec) < 0)
	{
		m_Vec = Ogre::Vector3::ZERO;
	}*/

	// 重力

	const Ogre::Vector3& pos = mo->getParentSceneNode()->getPosition();
	float dHgt = pos.y - m_GroundHgt;
	if(dHgt > 0.01f)
	{
		m_Vec.y -= 9.8f * dTime; // 模拟地球引力

	}
	else if(dHgt < - 0.01f)
	{
		m_Vec.y = Ogre::Math::Abs(m_Vec.y) * 0.5f/*损失系数*/;
	}
	
	// 强行保证球的高度在地面以上

	if(pos.y < m_GroundHgt)
		mo->getParentSceneNode()->setPosition(pos.x, m_GroundHgt, pos.z);

	Ogre::Vector3 moved = m_Vec * dTime;

	mo->getParentSceneNode()->setPosition(pos + moved);

	// 旋转		
	
	//mo->getParentSceneNode()->yaw(Ogre::Radian(moved.length() / ballRadious));
}

// ----------------------------------------
void COTEBallPhysicsCtrller::Hit(const Ogre::Vector3& normal, float strength)
{
	float length = m_Vec.length();
	m_Vec.normalise();

	m_Vec += normal * 1.414f;

	m_Vec = m_Vec.normalisedCopy() * (length + strength);
}

// ----------------------------------------
void COTEBallPhysicsCtrller::SetVec(const Ogre::Vector3& vec)
{
	m_Vec = vec;
}
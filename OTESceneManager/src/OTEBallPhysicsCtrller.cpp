#include "oteballphysicsctrller.h"
// ------------------------------------
using namespace OTE;
using namespace Ogre;

// ----------------------------------------
// ��ʼ��
void COTEBallPhysicsCtrller::Init(Ogre::MovableObject* mo)
{
	m_Vec = Ogre::Vector3::ZERO;
	//m_MaxVec = 50.0f;
	//m_GroundHgt = 0.0f;
}

// ----------------------------------------
// ����
void COTEBallPhysicsCtrller::Update(Ogre::MovableObject* mo, float dTime)
{
	if(m_Vec.isZeroLength())
		return;

	// ����ٶ����� 

	if(m_Vec.length() > m_MaxVec)
	{
		m_Vec = m_Vec.normalisedCopy() * m_MaxVec;
	}

	// �ƶ�

	m_Vec -= m_Vec.normalisedCopy() * m_Vec.length() * m_Frac;

	/*Ogre::Vector3 dir = m_Vec.normalisedCopy();

	m_Vec -= dir * dTime * m_Frac;
	
	if(dir.dotProduct(m_Vec) < 0)
	{
		m_Vec = Ogre::Vector3::ZERO;
	}*/

	// ����

	const Ogre::Vector3& pos = mo->getParentSceneNode()->getPosition();
	float dHgt = pos.y - m_GroundHgt;
	if(dHgt > 0.01f)
	{
		m_Vec.y -= 9.8f * dTime; // ģ���������

	}
	else if(dHgt < - 0.01f)
	{
		m_Vec.y = Ogre::Math::Abs(m_Vec.y) * 0.5f/*��ʧϵ��*/;
	}
	
	// ǿ�б�֤��ĸ߶��ڵ�������

	if(pos.y < m_GroundHgt)
		mo->getParentSceneNode()->setPosition(pos.x, m_GroundHgt, pos.z);

	Ogre::Vector3 moved = m_Vec * dTime;

	mo->getParentSceneNode()->setPosition(pos + moved);

	// ��ת		
	
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
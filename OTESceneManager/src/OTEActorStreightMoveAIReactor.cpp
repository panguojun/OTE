#include "oteactorstreightmoveaireactor.h"
#include "OTEQTSceneManager.h"
#include "OTEActorActionMgr.h"

// ------------------------------------
using namespace OTE;
using namespace Ogre;

// ====================================
// COTEActorStreightMoveAIReactor
// ====================================
void COTEActorStreightMoveAIReactor::Init(Ogre::MovableObject* mo, float maxMoveVec, float maxMoveAcc, float maxFrasc)
{
	m_MoveAcc = Ogre::Vector3::ZERO;

	m_MoveVec = Ogre::Vector3::ZERO;
		
	m_MaxMoveVec = maxMoveVec;

	m_MaxMoveAcc = maxMoveAcc;

	m_Frasc = m_MaxFrasc = maxFrasc;

	m_Pt2 = m_Pt1 = Ogre::Vector3::ZERO;

}

// ------------------------------------
void COTEActorStreightMoveAIReactor::Update(Ogre::MovableObject* mo, float dTime)
{
	OTE_ASSERT(mo);

	COTEActorEntity* ae = (COTEActorEntity*)mo;
	
	// 速度更新

	if(m_MoveVec.length() > m_MaxMoveVec)
		m_MoveVec = m_MoveVec.normalisedCopy() * m_MaxMoveVec;

	m_MoveVec += m_MoveAcc * dTime;
	
	Ogre::Vector3 nextVec = ae->GetParentSceneNode()->getPosition() + m_MoveVec * dTime; 
	
	// 限制在两点之间

	float dot = (nextVec - m_Pt1).dotProduct((m_Pt2 - m_Pt1).normalisedCopy());
	if(dot < 0)
	{
		ae->GetParentSceneNode()->setPosition(m_Pt1);
		m_MoveVec = Ogre::Vector3::ZERO;
	}
	else if(dot > (m_Pt2 - m_Pt1).length())
	{
		ae->GetParentSceneNode()->setPosition(m_Pt2);
		m_MoveVec = Ogre::Vector3::ZERO;
	}
	else
	{
		ae->GetParentSceneNode()->setPosition(nextVec);
	}

	Ogre::Vector3 dir = m_MoveVec.normalisedCopy();

	m_MoveVec -= dir * m_Frasc * dTime;

	if(dir.dotProduct(m_MoveVec) < 0)
	{
		m_MoveVec = Ogre::Vector3::ZERO;
	}	
}

// ------------------------------------
// 左移
void COTEActorStreightMoveAIReactor::MoveLeft(Ogre::MovableObject* mo)
{	
	m_MoveAcc = (m_Pt2 - m_Pt1).normalisedCopy() * m_MaxMoveAcc;	
}

// ------------------------------------
// 右移
void COTEActorStreightMoveAIReactor::MoveRight(Ogre::MovableObject* mo)
{
	m_MoveAcc = (m_Pt1 - m_Pt2).normalisedCopy() * m_MaxMoveAcc;
}

// ------------------------------------
// 停止
void COTEActorStreightMoveAIReactor::Stop(float maxFrasc)
{
	m_MoveAcc = Ogre::Vector3::ZERO;

	if(maxFrasc > 0)
		m_Frasc = maxFrasc;
	else
		m_Frasc = m_MaxFrasc;
}

// ====================================
// COTEActorStreightMoveAIReactor
// ====================================
void COTENpcActorStreightMoveAIReactor::Update(Ogre::MovableObject* mo, float dTime)
{
	NpcThink(mo);

	COTEActorStreightMoveAIReactor::Update(mo, dTime);
}

// ------------------------------------
// 机器人思维

void COTENpcActorStreightMoveAIReactor::NpcThink(Ogre::MovableObject* mo)
{
	// 奔向目标点

	float dot = (m_Pt2 - m_Pt1).dotProduct(m_TargetPos - mo->getParentSceneNode()->getPosition());
	if(dot > 0)
		MoveLeft(mo);
	else if(dot < 0)
		MoveRight(mo);

}

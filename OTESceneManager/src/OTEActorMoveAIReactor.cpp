#include "oteactormoveaireactor.h"
#include "OTEQTSceneManager.h"
#include "OTECollisionMap.h"
#include "OTECollision3D.h"
#include "OTEActorActionMgr.h"

// ------------------------------------
using namespace OTE;
using namespace Ogre;

#define MIN_STEP		0.1f

static COTECollisionMap		sColliMap;
// ------------------------------------
void COTEActorMoveAIReactor::Init(Ogre::MovableObject* mo)
{
	if(!mo)
	{
		OTE_MSG("COTEActorMoveAIReactor::Trigger �����������ΪNULL", "ʧ��")
		return;
	}
	COTEActorEntity* ae = (COTEActorEntity*)mo;	

	std::string nm = OTEHelper::GetOTESetting("ActorHelperConfig", "ActorHelperAction_Run");
	
	if(!COTEActorActionMgr::GetAction(ae, nm))
	{
		COTEActorActionMgr::AddAction(ae, nm);
	}

	nm = OTEHelper::GetOTESetting("ActorHelperConfig", "ActorHelperAction_Idle");

	if(!COTEActorActionMgr::GetAction(ae, nm))
	{
		COTEActorActionMgr::AddAction(ae, nm);
	}

	sColliMap.ReadFromBmp("defaultColliMap.bmp");
}
// ------------------------------------
// Ѱ·�߷�
bool COTEActorMoveAIReactor::UpdatePathFindingTarget(COTEActorEntity* ae)
{
	Ogre::Vector3 curPos = ae->GetParentSceneNode()->getPosition();

	bool result = true;

	// ��ɫ��Ѱ·Ŀ��㿿��
	{		
		if((Ogre::Vector2(m_TargetPos.x, m_TargetPos.z) - 
								Ogre::Vector2(curPos.x, curPos.z)).length() < MIN_STEP) // �ѵ���Ŀ���?
		{
			// ��һ��Ŀ���
			if(!m_WayMap.empty())
			{
				Ogre::Vector3 cp = m_TargetPos;
				Ogre::Vector3 hitPos;

				std::list<HistoryNode_t>::iterator it = m_WayMap.begin();
				while(!m_WayMap.empty())
				{
					// ����һ�δ�Խ
					if(!CCollision2D::HitTest(&sColliMap, cp.x, cp.z, (*it)._X, (*it)._Y, 0.2f, 2))
					{					
						SetTargetPos(Ogre::Vector3((*it)._X, curPos.y, (*it)._Y));			
						m_TargetPos.y = COTECollision3D::GetHeightAt((*it)._X, (*it)._Y, m_TargetPos.y);	
					}
					else
					{
						break;
					}

					m_WayMap.erase( m_WayMap.begin() );
					it = m_WayMap.begin();
				}			
			}
			// ����Ŀ���
			else
			{
				result = false;
			}
		}
	}

	return result;

}

// ------------------------------------
// �������
bool COTEActorMoveAIReactor::UpdateCursorTarget(COTEActorEntity* ae)
{
	OTE_ASSERT(ae);
	Ogre::Vector3 curPos = ae->GetParentSceneNode()->getPosition();

	bool result = false;

	// �������ֱ������

	if(m_CursorPos.x > 0 && m_CursorPos.y > 0)		
	{		
		Ogre::Ray ray = SCENE_CAM->getCameraToViewportRay( m_CursorPos.x, m_CursorPos.y);		
		Ogre::Vector3 hitPos;

		COTECollision3D::PickFloor(ray, curPos.y, hitPos);
	
		if(	!CCollision2D::HitTest(&sColliMap,hitPos.x, hitPos.z) &&
			!CCollision2D::HitTest(&sColliMap,curPos.x, curPos.z, hitPos.x, hitPos.z, 0.2f, 1) && 
			(Ogre::Vector2(hitPos.x, hitPos.z) - Ogre::Vector2(curPos.x, curPos.z)).length() > 1.0f ) // ���Ʒ�Χ
		{			
			SetTargetPos(hitPos); 
			result = true;
		}
		
	}

	return result;
}

// ---------------------------------------------
bool COTEActorMoveAIReactor::SetPosition(COTEActorEntity* ae, const Ogre::Vector3& pos)
{
	static bool s_LastResult = false;
	static Ogre::Vector3 s_LastPos;
	static Ogre::Vector3 s_LastTargetPos;

	if((pos - s_LastPos).length() < 1.0f && 
		(m_TargetPos - s_LastTargetPos).length() < 1.0f)
	{		
		if(s_LastResult)
			ae->GetParentSceneNode()->setPosition(pos);

		return s_LastResult;
	}

	s_LastPos = pos;
	s_LastTargetPos = m_TargetPos;

	ae->GetParentSceneNode()->setPosition(pos);

	s_LastResult = true;

	return true; // �ɴ�
}

// ------------------------------------
void COTEActorMoveAIReactor::Update(Ogre::MovableObject* mo, float dTime)
{
	OTE_ASSERT(mo);

	COTEActorEntity* ae = (COTEActorEntity*)mo;

	// ����Ŀ���
	
	bool isTargetValid = true;

	if(!UpdateCursorTarget(ae) && !UpdatePathFindingTarget(ae))
	{
		isTargetValid = false;
	}

	/// �ж��Ƿ��Ѿ�����Ŀ���

	Ogre::Vector3 curPos = ae->GetParentSceneNode()->getPosition();		

	if((Ogre::Vector2(m_TargetPos.x, m_TargetPos.z) - 
								Ogre::Vector2(curPos.x, curPos.z)).length() <= 0.0001f)
	{
		isTargetValid = false;
	}

	// ���½�ɫλ��

	if(isTargetValid)
	{
		// �ж��Ƿ����һ��

		dTime == 0 ? dTime = 0.001f : NULL;
		float a = (Ogre::Vector2(m_TargetPos.x, m_TargetPos.z) - Ogre::Vector2(curPos.x, curPos.z)).length() / (dTime * 4.5f)/*�ٶ�*/;
		if(int(a) <= 0)
		{
			SetPosition(ae, m_TargetPos);
		}
		else
		{
			// λ��

			Ogre::Vector3 nextPos = curPos + (m_TargetPos - curPos).normalisedCopy() * dTime * 4.5f/*�ٶ�*/;
			float hgt = COTECollision3D::GetHeightAt(nextPos.x, nextPos.z, curPos.y);
			nextPos = Ogre::Vector3(nextPos.x, hgt, nextPos.z);
			SetPosition(ae, nextPos);	
		}
		

		// ����

		Ogre::Vector3 curDir = ae->GetDirection();
		Ogre::Vector3 targetDir = (m_TargetPos - curPos).normalisedCopy();

		Ogre::Real cos = curDir.dotProduct(targetDir);
		Ogre::Real sin = curDir.crossProduct(targetDir).dotProduct(Ogre::Vector3::UNIT_Y);
		if(cos < 0.95)
		{
			ae->SetRadian(Ogre::Math::ATan2(sin, cos));
		}		

		// ���ö���

		COTEActorActionMgr::SetAction(ae, OTEHelper::GetOTESetting("ActorHelperConfig", "ActorHelperAction_Run"));
	}
	else
	// �Ѿ�����
	{		
		// ����״̬

		m_CursorPos.x = -1.0f;

		// ���ö���

		COTEAction* act = ae->GetCurAction();
		if(!act || 
			act->GetMode() == COTEAction::eSTOP || 
			act->GetName() == OTEHelper::GetOTESetting("ActorHelperConfig", "ActorHelperAction_Run"))
			COTEActorActionMgr::SetAction(ae, OTEHelper::GetOTESetting("ActorHelperConfig", "ActorHelperAction_Idle"));
	}	

}

// ==================================================
// COTEActorMoveToTargetAIReactor
// ==================================================

// ����
void COTEActorMoveToTargetAIReactor::Init(Ogre::MovableObject* mo)
{
	COTEActorEntity* actor = (COTEActorEntity*)mo;
	COTEActorActionMgr::SetAction(
		actor, 
		OTEHelper::GetOTESetting("ActorAIConfig", "ActorAction_Run")
		);	
}

// --------------------------------------------------
// ����
void COTEActorMoveToTargetAIReactor::Update(Ogre::MovableObject* mo, float dTime)
{
	if(m_TargetPos == Ogre::Vector3(-1.0f, -1.0f, -1.0f))
		return;

	COTEActorEntity* actor = (COTEActorEntity*)mo;
	Ogre::Vector3 vec = m_TargetPos - actor->GetParentSceneNode()->getPosition();
	vec.normalise();
	const Ogre::Vector3& curPos = actor->getParentSceneNode()->getPosition();

	// �ƶ��ٶ�

	const float c_MoveSpeed = 6.0f;
	Ogre::Vector3 dMove = vec * c_MoveSpeed * dTime;
	
	if( (curPos - m_TargetPos).length() < dMove.length() )
	{
		std::list<EventCB_t>::iterator it = m_EventCBs.begin();
		while(it != m_EventCBs.end())
		{
			(*(*it))();

			++ it;
		}

		actor->getParentSceneNode()->setPosition(m_TargetPos);
		m_TargetPos = Ogre::Vector3(-1.0f, -1.0f, -1.0f);
	}
	else
	{
		actor->getParentSceneNode()->setPosition(
			actor->getParentSceneNode()->getPosition() + 
			dMove
			);
	}

}

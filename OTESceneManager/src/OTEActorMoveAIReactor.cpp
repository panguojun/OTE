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
		OTE_MSG("COTEActorMoveAIReactor::Trigger 传入参数不能为NULL", "失败")
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
// 寻路走法
bool COTEActorMoveAIReactor::UpdatePathFindingTarget(COTEActorEntity* ae)
{
	Ogre::Vector3 curPos = ae->GetParentSceneNode()->getPosition();

	bool result = true;

	// 角色向寻路目标点靠近
	{		
		if((Ogre::Vector2(m_TargetPos.x, m_TargetPos.z) - 
								Ogre::Vector2(curPos.x, curPos.z)).length() < MIN_STEP) // 已到达目标点?
		{
			// 下一个目标点
			if(!m_WayMap.empty())
			{
				Ogre::Vector3 cp = m_TargetPos;
				Ogre::Vector3 hitPos;

				std::list<HistoryNode_t>::iterator it = m_WayMap.begin();
				while(!m_WayMap.empty())
				{
					// 允许一次穿越
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
			// 到达目标点
			else
			{
				result = false;
			}
		}
	}

	return result;

}

// ------------------------------------
// 跟随鼠标
bool COTEActorMoveAIReactor::UpdateCursorTarget(COTEActorEntity* ae)
{
	OTE_ASSERT(ae);
	Ogre::Vector3 curPos = ae->GetParentSceneNode()->getPosition();

	bool result = false;

	// 左键按下直接行走

	if(m_CursorPos.x > 0 && m_CursorPos.y > 0)		
	{		
		Ogre::Ray ray = SCENE_CAM->getCameraToViewportRay( m_CursorPos.x, m_CursorPos.y);		
		Ogre::Vector3 hitPos;

		COTECollision3D::PickFloor(ray, curPos.y, hitPos);
	
		if(	!CCollision2D::HitTest(&sColliMap,hitPos.x, hitPos.z) &&
			!CCollision2D::HitTest(&sColliMap,curPos.x, curPos.z, hitPos.x, hitPos.z, 0.2f, 1) && 
			(Ogre::Vector2(hitPos.x, hitPos.z) - Ogre::Vector2(curPos.x, curPos.z)).length() > 1.0f ) // 控制范围
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

	return true; // 可达
}

// ------------------------------------
void COTEActorMoveAIReactor::Update(Ogre::MovableObject* mo, float dTime)
{
	OTE_ASSERT(mo);

	COTEActorEntity* ae = (COTEActorEntity*)mo;

	// 更新目标点
	
	bool isTargetValid = true;

	if(!UpdateCursorTarget(ae) && !UpdatePathFindingTarget(ae))
	{
		isTargetValid = false;
	}

	/// 判断是否已经到达目标点

	Ogre::Vector3 curPos = ae->GetParentSceneNode()->getPosition();		

	if((Ogre::Vector2(m_TargetPos.x, m_TargetPos.z) - 
								Ogre::Vector2(curPos.x, curPos.z)).length() <= 0.0001f)
	{
		isTargetValid = false;
	}

	// 更新角色位置

	if(isTargetValid)
	{
		// 判断是否最后一步

		dTime == 0 ? dTime = 0.001f : NULL;
		float a = (Ogre::Vector2(m_TargetPos.x, m_TargetPos.z) - Ogre::Vector2(curPos.x, curPos.z)).length() / (dTime * 4.5f)/*速度*/;
		if(int(a) <= 0)
		{
			SetPosition(ae, m_TargetPos);
		}
		else
		{
			// 位置

			Ogre::Vector3 nextPos = curPos + (m_TargetPos - curPos).normalisedCopy() * dTime * 4.5f/*速度*/;
			float hgt = COTECollision3D::GetHeightAt(nextPos.x, nextPos.z, curPos.y);
			nextPos = Ogre::Vector3(nextPos.x, hgt, nextPos.z);
			SetPosition(ae, nextPos);	
		}
		

		// 方向

		Ogre::Vector3 curDir = ae->GetDirection();
		Ogre::Vector3 targetDir = (m_TargetPos - curPos).normalisedCopy();

		Ogre::Real cos = curDir.dotProduct(targetDir);
		Ogre::Real sin = curDir.crossProduct(targetDir).dotProduct(Ogre::Vector3::UNIT_Y);
		if(cos < 0.95)
		{
			ae->SetRadian(Ogre::Math::ATan2(sin, cos));
		}		

		// 设置动画

		COTEActorActionMgr::SetAction(ae, OTEHelper::GetOTESetting("ActorHelperConfig", "ActorHelperAction_Run"));
	}
	else
	// 已经到达
	{		
		// 清理状态

		m_CursorPos.x = -1.0f;

		// 设置动画

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

// 触发
void COTEActorMoveToTargetAIReactor::Init(Ogre::MovableObject* mo)
{
	COTEActorEntity* actor = (COTEActorEntity*)mo;
	COTEActorActionMgr::SetAction(
		actor, 
		OTEHelper::GetOTESetting("ActorAIConfig", "ActorAction_Run")
		);	
}

// --------------------------------------------------
// 更新
void COTEActorMoveToTargetAIReactor::Update(Ogre::MovableObject* mo, float dTime)
{
	if(m_TargetPos == Ogre::Vector3(-1.0f, -1.0f, -1.0f))
		return;

	COTEActorEntity* actor = (COTEActorEntity*)mo;
	Ogre::Vector3 vec = m_TargetPos - actor->GetParentSceneNode()->getPosition();
	vec.normalise();
	const Ogre::Vector3& curPos = actor->getParentSceneNode()->getPosition();

	// 移动速度

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

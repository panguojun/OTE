#include "OTETrackCtrller.h"
#include "OTEMagicManager.h"

using namespace Ogre;
using namespace OTE;

// ----------------------------------------
//清除全部目标
void COTETrackCtrller::Clear()
{		
	m_TargetList.clear();
	OTE_TRACE("清理 对象 - 目标")
}
// ------------------------------------
void COTETrackCtrller::Trigger(Ogre::MovableObject* mo){}
// ----------------------------------------
//更新轨道
void COTETrackCtrller::Update(Ogre::MovableObject* mo, float time)
{
	const float c_hit_dis = 0.25f;
	
	TIMECOST_START

	m_time -= time;

	std::vector< ObjTargetPair_t >::iterator it = m_TargetList.begin();
	while(it != m_TargetList.end()) 
	{
		Ogre::MovableObject* flyMO = it->first;
		
		// 更新

		if(flyMO)
		{
			Ogre::Vector3 Pos, tPos, v;
		
			Pos  = flyMO->getParentNode()->getWorldPosition()/* + Ogre::Vector3(0.0f, 1.0f, 0.0f)*/;
			tPos = it->second.Pos + Ogre::Vector3(0.001f, 1.001f, 0.001f) /*偏移*/;				

			// 击中目标

			if(m_time <= 0 || (tPos - Pos).length() <= c_hit_dis)
			{	
				std::string targetObjName = it->second.TargetObjName;

				m_TargetList.erase(it);
				it = m_TargetList.begin();	

				/// 事件触发
				
				if((tPos - Pos).length() <= c_hit_dis)
				{
					((COTEActorEntity*)m_Creator)->OnHitTarget();					
				
					COTEActorEntity* Target = SCENE_MGR->GetEntity(targetObjName);

					if(Target)
						Target->OnBeHit((COTEActorEntity*)m_Creator);					
				}
				else
				{
					((COTEActorEntity*)m_Creator)->OnMissTarget();	
				}				

				TIMECOST_END
				return;
			}
			
			// 自动计算插值

			float factor = time / m_time;

			float y = Pos.y;
			Pos = Pos * (1.0f - factor) + tPos * factor;
			
			float g = 19.8f;
			y += (-g * (m_time0 - m_time) + g / 2.0f * m_time0) * time;
			Pos.y = y;

			flyMO->getParentNode()->setScale(Ogre::Vector3::UNIT_SCALE);

			flyMO->getParentNode()->setPosition(Pos);

			if((tPos - Pos).length() > 0.01f)
			{
				Ogre::Quaternion q = Ogre::Vector3::UNIT_Z.getRotationTo(tPos - Pos);

				flyMO->getParentNode()->setOrientation(q);
			}
		}
		it++;	
	}
	TIMECOST_END
}
// ----------------------------------------
void COTETrackCtrller::AddMagicTarget(Ogre::MovableObject* mag, Ogre::MovableObject* target)
{	
	// 先移除
	RemoveMagicTarget(mag);

	m_TargetList.push_back(
		ObjTargetPair_t(mag,
		Target_t(target->getName(), target->getParentSceneNode()->getPosition())
		)
		);

	OTE_TRACE("添加 对象 - 目标 obj: " << mag->getName() << " target: " << (target ? target->getName() : "none"))
}

// ----------------------------------------
void COTETrackCtrller::RemoveMagicTarget(Ogre::MovableObject* mag)
{
	std::vector< ObjTargetPair_t >::iterator it = m_TargetList.begin();
	while(it != m_TargetList.end())
	{
		if(it->first == mag)
		{
			OTE_TRACE("移出 对象 - 目标 obj: " << mag->getName())

			m_TargetList.erase(it);
			break;
		}
		++ it;
	}
}

// =========================================
// COTEParabolaTrackCtrller
// 抛物线轨道控制器
// 这个曾经应用于一个小游戏(西瓜守卫者)里
// =========================================
// 触发
void COTEParabolaTrackCtrller::Trigger(Ogre::MovableObject* mo)
{
	m_TimeEsp = 0.0f;								
	m_time = 10.0f;									// 最长时间					

	SetGravity(9.8f);								// 模拟地球引力
	SetStartVec(Ogre::Vector3(10.5f, 5.5f, 0.0f));  // 默认初始速度
}
// ----------------------------------------
// 更新
void COTEParabolaTrackCtrller::UpdateEx(Ogre::MovableObject* mo, float dTime, HitCB_t hitcb, float floorHgt)
{
	TIMECOST_START

	if(m_time <= 0.0f)
		return;

	// 时间

	m_time -= dTime;
	m_TimeEsp += dTime;
	Ogre::MovableObject* lastFlyMO = NULL;

	std::vector< std::pair<Ogre::MovableObject*, Ogre::MovableObject*> >::iterator it = m_TargetList.begin();
	while(it != m_TargetList.end()) 
	{
		Ogre::MovableObject* flyMO = it->first;
		Ogre::MovableObject* Target = it->second;		

		if(!flyMO || !flyMO->getParentNode())
		{
			++ it;
			continue;
		}	

		// 击中目标

		const Ogre::Vector3& Pos = flyMO->getParentNode()->getWorldPosition();	

		bool isHit = Target != NULL ? Target->getWorldBoundingBox().intersects(Pos) : false;
		
		if(Pos.y < floorHgt || m_time <= 0 || isHit)
		{
			m_TargetList.erase(it);
			it = m_TargetList.begin();				
			

			if(isHit)
			{
				/// 事件触发

				if(m_Creator)
					((COTEActorEntity*)m_Creator)->OnHitTarget();
				
				if(Target->getMovableType() == "OTEEntity")
				{
					((COTEActorEntity*)Target)->OnBeHit((COTEActorEntity*)m_Creator);
				}

				if(hitcb) (*hitcb)(eHitTarget, Target);
			}
			else if(Pos.y < floorHgt)
			{
				if(hitcb) (*hitcb)(eHgtOut, NULL);
			}
			else if(m_time <= 0)
			{
				if(hitcb) (*hitcb)(eTimeOut, NULL);
			}

			continue;
		}
		
		// 自动计算插值
		
		if(lastFlyMO != flyMO)
		{
			flyMO->getParentNode()->setPosition(
				flyMO->getParentNode()->getPosition() + 
				Ogre::Vector3(
				m_StartVec.x, 
				-2.0f * m_Gravity * m_TimeEsp + m_StartVec.y, 
				m_StartVec.z) * dTime
				);		
		}

		lastFlyMO = flyMO;		

		it++;
	}

	TIMECOST_END
}

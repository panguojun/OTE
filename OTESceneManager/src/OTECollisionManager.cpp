#include "oteqtscenemanager.h"
#include "otecollisionmanager.h"
#include "OTEActorEntity.h"
#include "OTEParticlesystem.h"
#include "OTEEntityPhysicsMgr.h"

// ---------------------------------------------
using namespace Ogre;
using namespace OTE;


// ===================================================
// COTEObjectIntersection
// 场景实体碰撞
// ===================================================

std::pair<Ogre::MovableObject*, Ogre::Vector3> COTEObjectIntersection::TriangleIntersection(
	const Ogre::Ray& ray, const Ogre::String& selGroupName,	const Ogre::String& unselName, 
	float distance, unsigned int flag)
{	
	std::pair<Ogre::MovableObject*, Ogre::Vector3> ret;
	ret.first = NULL; 
	IntersectInfo_t intersectResult;	
	
	std::list <Ogre::MovableObject*> intersectObjList;	
	AABBIntersection(ray, selGroupName, unselName, intersectObjList);

	std::list <Ogre::MovableObject*>::iterator it = intersectObjList.begin();
	while(it != intersectObjList.end())
	{	

		// 每一个box都要进行碰撞检测

		MovableObject* 	tpObject = (*it);

		if( 
			tpObject->getMovableType().find("OTEEntity") == std::string::npos				
			)		
		{
			++ it;
			continue;
		}

		intersectResult = COTEEntityPhysicsMgr::GetTriangleIntersection(
								static_cast<COTEEntity*>(tpObject), ray, flag);  
		
		if(intersectResult.isHit)
		{
			if(flag & E_HITPOS)
			{
				float hitDis = (intersectResult.hitPos - ray.getOrigin()).length();

				if(hitDis < distance)
				{
					ret.first  = tpObject;
					ret.second = intersectResult.hitPos;	

					distance   = hitDis;
				}
			}
			else
			{
				ret.first  = tpObject; // 这样相当于返回是否碰撞到物件
				return ret;
			}			
		}

		++ it;
	}   


	return ret;
}  

//------------------------------------------------------------------------------------
void COTEObjectIntersection::AABBIntersection(const Ray& ray, const String& selGroupName, 
											  const String& unselName, std::list <Ogre::MovableObject*>& result)
{
	std::vector<Ogre::MovableObject*> eList;
	SCENE_MGR->GetEntityList(&eList);

	std::vector<Ogre::MovableObject*>::iterator it = eList.begin();
    while( it != eList.end() )
    {  	   
	   // 基于movable object 的碰撞检测	  
	
		Ogre::MovableObject* m = *it;

		const Ogre::AxisAlignedBox& box = m->getWorldBoundingBox();

        if(	/*m->isInScene() &&*/
			/*SCENE_MGR->m_Box.intersects( m->getWorldBoundingBox() ) &&*/
			ray.intersects(box).first
			)
        {
			if (
				(selGroupName.empty() || strncmp (m->getName().c_str(), selGroupName.c_str(), selGroupName.length()) == 0) &&
				(unselName.empty()    || m->getName().find(unselName) == std::string::npos)
			)
			{				
				result.insert(result.end(), m);		
			}
        }    

        ++it;
    }
	
}


// ---------------------------------------------------
// 三角形精确碰撞 查询SubEntity

bool COTEObjectIntersection::GetSubList_TriangleIntersection(	const Ogre::Ray& rRay,  
																const String& selGroupName, 
																std::list< COTESubEntity* >& subEntList,
																float maxDis)
{	
	std::list <Ogre::MovableObject*> intersectObjList;	
	AABBIntersection(rRay, selGroupName, "", intersectObjList);

	std::list <Ogre::MovableObject*>::iterator it = intersectObjList.begin();
    while(it != intersectObjList.end())
	{	
		COTEEntityPhysicsMgr::GetSubList_TriangleIntersection(
							static_cast<COTEActorEntity*>((*it)), 
							rRay, subEntList, 
							maxDis
							); 
		++ it;
	}
	
	return subEntList.size() > 0;
}


// ===================================================
// COTECollisionManager
// 只包括实体
// ===================================================

bool COTECollisionManager::PickAABB( 
							const Ogre::Ray& rRay, 
							std::list <Ogre::MovableObject*>& rMovebleObjectList, const Ogre::String& rSelGroupName)
{
	std::list < Ogre::SceneNode * > list;

	SCENE_MGR->FindNodes(list);	 	

    //grab all moveables from the node that intersect...
	std::list <Ogre::SceneNode *>::iterator it = list.begin();
    while( it != list.end() )
    {  	   
	   // 基于movable object 的碰撞检测
	  
	   Ogre::SceneNode::ObjectIterator oit = (*it) -> getAttachedObjectIterator();
       while( oit.hasMoreElements() )
       {		  
		   Ogre::MovableObject * m = oit.getNext();
		   const Ogre::AxisAlignedBox& box = m->getWorldBoundingBox();

           if(	/*m->isInScene() &&*/		
				rRay.intersects(box).first &&
				(rSelGroupName.empty() || m->getName().find(rSelGroupName) != std::string::npos)
				)
           {
               rMovebleObjectList.insert(rMovebleObjectList.end(), m);		
           }
       }

        ++it;
    }
	
	return rMovebleObjectList.size() > 0;
}

// ----------------------------------------------------
Ogre::MovableObject* COTECollisionManager::PickAABB_DisPrio(const Ogre::Ray& rRay)
{
	std::list < Ogre::SceneNode * > list;
	SCENE_MGR->FindNodes(list);	 

    // grab all moveables from the node that intersect...

	float nearestDis = MAX_FLOAT;
	Ogre::MovableObject * resultMovable = NULL;

	std::list <Ogre::SceneNode *>::iterator it = list.begin();
    while( it != list.end() )
    {  		  
	   Ogre::SceneNode::ObjectIterator oit = (*it) -> getAttachedObjectIterator();
       while( oit.hasMoreElements() )
       {
		   Ogre::MovableObject * m = oit.getNext();
           if(	/*m->isInScene() &&*/
				/*SCENE_MGR->m_Box.intersects(m->getWorldBoundingBox()) &&*/
				m->getMovableType().find("SimpleRenderable") == std::string::npos
				)
           {
                std::pair<bool, Real> result = rRay.intersects(m->getWorldBoundingBox());
				if(result.first)
				{
					if(result.second < nearestDis)
					{
						nearestDis = result.second;
						resultMovable = m;
					}
				}
           }
       }

        ++it;
    }

	return resultMovable;	
}

// ----------------------------------------------------
bool COTECollisionManager::AABBIntersection(const Ogre::Ray& rRay, Ogre::Vector3& rHitPos)
{
	std::list<Ogre::MovableObject*> ml;
	PickAABB(rRay, ml);

	if(ml.size() > 0)
	{
		rHitPos = rRay.getPoint(rRay.intersects((*ml.begin())->getWorldBoundingBox()).second);

		return true;
	}
	return false;
}


// ----------------------------------------------------
bool COTECollisionManager::TriangleIntersection(const Ogre::Ray& rRay, Ogre::Vector3& rHitPos, float distance, unsigned int flag)
{
	std::pair<Ogre::MovableObject*, Ogre::Vector3> rs = COTEObjectIntersection::TriangleIntersection(rRay, "Ent", "", distance, flag);
	if(rs.first)
	{
		if(flag & E_HITPOS)
		{
			rHitPos = rs.second;
			return (rHitPos - rRay.getOrigin()).squaredLength() < distance * distance;
		}
		else
		{
			return true;
		}
	}
	return false;
}

// ----------------------------------------------------
bool COTECollisionManager::PickZeroFloor(const Ogre::Ray& rRay, Ogre::Vector3& rHitPos)
{
	Ogre::Plane zeroFloor(Ogre::Vector3::UNIT_Y, Ogre::Vector3::ZERO);
	std::pair<bool, float> result = rRay.intersects(zeroFloor);
	rHitPos = rRay.getPoint(result.second);

	return result.first;

}

// ----------------------------------------------------
bool COTECollisionManager::PickFloor(const Ogre::Ray& rRay, float hgt, Ogre::Vector3& rHitPos)
{
	Ogre::Plane tFloor(Ogre::Vector3::UNIT_Y, Ogre::Vector3(0, hgt, 0));
	std::pair<bool, float> result = rRay.intersects(tFloor);
	rHitPos = rRay.getPoint(result.second);

	return result.first;

}

// ----------------------------------------------
// 测试山体碰撞

bool COTECollisionManager::IntersectMap(Ogre::Vector3& lastHitPos, COTETilePage* page, 
				const Ogre::Vector3& iv, const Ogre::Ray& r, float rough, int& Hits, float distance)
{
	if(!page)
		return false;

	int hits = 0;
	Vector3 v = iv - r.getDirection() * rough;  
	float terheight=0;	

	while ((v - r.getOrigin()).dotProduct(r.getDirection()) > 0	&& terheight > -1 )
	{
		if(distance)
		{
			if((v - r.getOrigin()).squaredLength() > distance * distance)
				break;
		}

		terheight = page->GetHeightAt(v.x, v.z);
		if(terheight - v.y > 0.2f)
		{
			hits++;
			lastHitPos = v;
		}
		v -= r.getDirection() * rough;
	}	

	Hits = hits;

	if(hits == 0)
		return false;	

	return true;
} 

// ----------------------------------------------
bool COTECollisionManager::IntersectMap(COTETilePage* page, 
				const Ogre::Ray& r/*from ground point to sun*/, float rough, float distance, int& Hits, Ogre::Vector3& lastHitPos)
{
	rough = 1.0f; // 暂时的

	if(!page)
		return false;

	int hits = 0;
	Vector3 v = r.getPoint(rough);  
	float terheight=0;	

	while (terheight > -1 )
	{		
		if((v - r.getOrigin()).squaredLength() > distance * distance)
			break;	

		terheight = page->GetHeightAt(v.x, v.z);
		if(terheight - v.y > 0.2f)
		{
			hits++;
			lastHitPos = v;
		}
		v += r.getDirection() * rough;
	}	

	Hits = hits;

	if(hits == 0)
		return false;	

	return true;
} 
// ===================================================
// COTEActCollisionManager
// ===================================================

// 点选AABB

bool COTEActCollisionManager::PickAABB( 
			const Ogre::Ray& rRay, std::list <Ogre::MovableObject*>& rMovebleObjectList,
			const std::string& group)
{	
	std::vector<Ogre::MovableObject*> eList;
	SCENE_MGR->GetEntityList(&eList);
	for(unsigned int i = 0; i < eList.size(); ++i)
	{
		if( eList[i] &&
			eList[i]->getParentSceneNode()->getParentSceneNode() == (Ogre::SceneNode*)SCENE_MGR->m_EntSNRootMap[group])
		{
			Ogre::String name = eList[i]->getName();	

			Ogre::SceneNode* sn = (Ogre::SceneNode*)SCENE_MGR->m_EntSNRootMap[group]->getChild(name);
			
			Ogre::MovableObject * m = eList[i];
			if(		m->isVisible() &&
					/*SCENE_MGR->m_Box.intersects( m->getWorldBoundingBox() ) &&*/
					rRay.intersects(m->getWorldBoundingBox()).first
					)
			{
				rMovebleObjectList.insert(rMovebleObjectList.end(), m);				
			}

		}
	}
	
	return rMovebleObjectList.size() > 0;
}

// ---------------------------------------------------
// 三角形精确碰撞

Ogre::MovableObject* COTEActCollisionManager::PickTriangleIntersection( 
					const Ogre::Ray& rRay, const std::string& group, float maxDis)
{	
	Ogre::MovableObject* ret_mo = NULL;	

	std::list <Ogre::MovableObject*> rMovebleObjectList;

	std::vector<Ogre::MovableObject*> eList; SCENE_MGR->GetEntityList(&eList);
	std::vector<Ogre::MovableObject*>::iterator lit = eList.begin();
	while(lit != eList.end())
	{
		Ogre::MovableObject* m = (*lit);

		if( m &&
			m->getParentSceneNode()->getParentSceneNode() == (Ogre::SceneNode*)SCENE_MGR->m_EntSNRootMap[group])
		{
			Ogre::String name = m->getName();	
			Ogre::SceneNode* sn = (Ogre::SceneNode*)SCENE_MGR->m_EntSNRootMap[group]->getChild(name);
					
			const Ogre::AxisAlignedBox& box =  m->getWorldBoundingBox();

			if(		m->isVisible() &&
					//SCENE_MGR->m_Box.intersects(box) &&
					rRay.intersects(box).first
					)
			{
				rMovebleObjectList.insert(rMovebleObjectList.end(), m);	

				ret_mo = m;
			}

		}

		++ lit;
	}

	if(rMovebleObjectList.size() > 1)
	{
		std::list <Ogre::MovableObject*>::iterator it = rMovebleObjectList.begin();
		while(it != rMovebleObjectList.end())
		{	
			IntersectInfo_t intersectResult = 
				COTEEntityPhysicsMgr::GetTriangleIntersection(
							static_cast<COTEActorEntity*>(*it), rRay, 0); 

			float hitDis = (intersectResult.hitPos - rRay.getOrigin()).length();

			if(intersectResult.isHit/* && hitDis < maxDis*/)
			{
				ret_mo = *it;

				maxDis = hitDis;
			}

			++ it;
		}
	}
	
	return ret_mo;
}

// ===================================================
// COTEMagCollisionManager
// ===================================================

bool COTEMagCollisionManager::PickAABB( 
		const Ogre::Ray& rRay, std::list <Ogre::MovableObject*>& rMovebleObjectList,
		const Ogre::String& group)
{	
	HashMap<std::string, COTEParticleSystem*>::iterator it = COTEParticleSystemManager::GetInstance()->m_ParticleSystems.begin();
	while(it != COTEParticleSystemManager::GetInstance()->m_ParticleSystems.end())
	{			
		Ogre::MovableObject * m = it->second;
		if(
			m->isVisible() &&
				/*SCENE_MGR->m_Box.intersects( m->getWorldBoundingBox() ) &&*/
				rRay.intersects(m->getWorldBoundingBox()).first
				)
		{
			rMovebleObjectList.insert(rMovebleObjectList.end(), m);				
		}

		++ it;
	}
	
	return rMovebleObjectList.size() > 0;
}

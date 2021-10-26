#pragma once
#include "Ogre.h"
#include "OTEEntityPhysicsMgr.h"
#include "OTETilePage.h"

namespace OTE
{
class COTEQTSceneManager;
// *********************************************
// COTEObjectIntersection
// 场景物件的碰撞实现
// *********************************************

class _OTEExport COTEObjectIntersection
{
public:

	// 三角形碰撞检测

	static std::pair<Ogre::MovableObject*, Ogre::Vector3> 
				TriangleIntersection(
						const Ogre::Ray& ray, const Ogre::String& selGroupName = "",	const Ogre::String& unselName = "", 
						float distance = LONG_MAX, unsigned int flag = E_HITPOS);

	// 方盒碰撞检测

	static void AABBIntersection(	const Ogre::Ray& ray, 
							const Ogre::String& selGroupName, 
							const Ogre::String& unselName, 
							std::list <Ogre::MovableObject*>& result);

	// 点选材质

	static bool GetSubList_TriangleIntersection(
							const Ogre::Ray& rRay, 								
							const Ogre::String& selGroupName, 
							std::list< COTESubEntity* >& subEntList,
							float maxDis
							);

};

// ***********************************************
// 场景碰撞管理器
// 包括场景里的实体，地皮
// ***********************************************	

class _OTEExport COTECollisionManager
{
public:

	// 场景AABB碰撞调查

	static bool PickAABB( 
			const Ogre::Ray& rRay, std::list <Ogre::MovableObject*>& rMovebleObjectList,
			const Ogre::String& rSelGroupName = "");

	static Ogre::MovableObject* PickAABB_DisPrio(const Ogre::Ray& rRay);

	// 场景碰撞点(aabb)

	static bool AABBIntersection(const Ogre::Ray& rRay, Ogre::Vector3& rHitPos);
	
	static bool TriangleIntersection(const Ogre::Ray& rRay, Ogre::Vector3& rHitPos, 
														float distance = MAX_FLOAT,
														unsigned int flag = E_HITPOS);

	static bool PickZeroFloor(const Ogre::Ray& rRay, Ogre::Vector3& rHitPos);

	static bool PickFloor(const Ogre::Ray& rRay, float hgt, Ogre::Vector3& rHitPos);

	// 地面的碰撞

	static bool IntersectMap(Ogre::Vector3& lastHitPos, COTETilePage* page, 
						const Ogre::Vector3& iv, const Ogre::Ray& r, float rough, int& Hits, float distance = 1000000.0f);
	static bool COTECollisionManager::IntersectMap(COTETilePage* page, 
						const Ogre::Ray& r/*from ground point to sun*/, float rough, float distance, int& Hits, Ogre::Vector3& lastHitPos);

};

// ***********************************************
// 场景碰撞管理器(角色)
// ***********************************************	
class _OTEExport COTEActCollisionManager
{
public:
	// 场景AABB碰撞调查
	static bool PickAABB(  
			const Ogre::Ray& rRay, std::list <Ogre::MovableObject*>& rMovebleObjectList,
			const std::string& group = "default");

	static Ogre::MovableObject* PickTriangleIntersection( 
			const Ogre::Ray& rRay, const std::string& group = "default", float maxDis = MAX_FLOAT);
	
};

// ***********************************************
// 场景碰撞管理器(特效实体)
// ***********************************************	
class _OTEExport COTEMagCollisionManager
{
public:
	// 场景AABB碰撞调查
	static bool PickAABB( 
		const Ogre::Ray& rRay, std::list <Ogre::MovableObject*>& rMovebleObjectList,
		const Ogre::String& group = "particle");
};

}
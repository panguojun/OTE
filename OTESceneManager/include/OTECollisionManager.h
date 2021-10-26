#pragma once
#include "Ogre.h"
#include "OTEEntityPhysicsMgr.h"
#include "OTETilePage.h"

namespace OTE
{
class COTEQTSceneManager;
// *********************************************
// COTEObjectIntersection
// �����������ײʵ��
// *********************************************

class _OTEExport COTEObjectIntersection
{
public:

	// ��������ײ���

	static std::pair<Ogre::MovableObject*, Ogre::Vector3> 
				TriangleIntersection(
						const Ogre::Ray& ray, const Ogre::String& selGroupName = "",	const Ogre::String& unselName = "", 
						float distance = LONG_MAX, unsigned int flag = E_HITPOS);

	// ������ײ���

	static void AABBIntersection(	const Ogre::Ray& ray, 
							const Ogre::String& selGroupName, 
							const Ogre::String& unselName, 
							std::list <Ogre::MovableObject*>& result);

	// ��ѡ����

	static bool GetSubList_TriangleIntersection(
							const Ogre::Ray& rRay, 								
							const Ogre::String& selGroupName, 
							std::list< COTESubEntity* >& subEntList,
							float maxDis
							);

};

// ***********************************************
// ������ײ������
// �����������ʵ�壬��Ƥ
// ***********************************************	

class _OTEExport COTECollisionManager
{
public:

	// ����AABB��ײ����

	static bool PickAABB( 
			const Ogre::Ray& rRay, std::list <Ogre::MovableObject*>& rMovebleObjectList,
			const Ogre::String& rSelGroupName = "");

	static Ogre::MovableObject* PickAABB_DisPrio(const Ogre::Ray& rRay);

	// ������ײ��(aabb)

	static bool AABBIntersection(const Ogre::Ray& rRay, Ogre::Vector3& rHitPos);
	
	static bool TriangleIntersection(const Ogre::Ray& rRay, Ogre::Vector3& rHitPos, 
														float distance = MAX_FLOAT,
														unsigned int flag = E_HITPOS);

	static bool PickZeroFloor(const Ogre::Ray& rRay, Ogre::Vector3& rHitPos);

	static bool PickFloor(const Ogre::Ray& rRay, float hgt, Ogre::Vector3& rHitPos);

	// �������ײ

	static bool IntersectMap(Ogre::Vector3& lastHitPos, COTETilePage* page, 
						const Ogre::Vector3& iv, const Ogre::Ray& r, float rough, int& Hits, float distance = 1000000.0f);
	static bool COTECollisionManager::IntersectMap(COTETilePage* page, 
						const Ogre::Ray& r/*from ground point to sun*/, float rough, float distance, int& Hits, Ogre::Vector3& lastHitPos);

};

// ***********************************************
// ������ײ������(��ɫ)
// ***********************************************	
class _OTEExport COTEActCollisionManager
{
public:
	// ����AABB��ײ����
	static bool PickAABB(  
			const Ogre::Ray& rRay, std::list <Ogre::MovableObject*>& rMovebleObjectList,
			const std::string& group = "default");

	static Ogre::MovableObject* PickTriangleIntersection( 
			const Ogre::Ray& rRay, const std::string& group = "default", float maxDis = MAX_FLOAT);
	
};

// ***********************************************
// ������ײ������(��Чʵ��)
// ***********************************************	
class _OTEExport COTEMagCollisionManager
{
public:
	// ����AABB��ײ����
	static bool PickAABB( 
		const Ogre::Ray& rRay, std::list <Ogre::MovableObject*>& rMovebleObjectList,
		const Ogre::String& group = "particle");
};

}
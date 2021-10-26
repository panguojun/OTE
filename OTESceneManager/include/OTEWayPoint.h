#pragma once
#include "OTEStaticInclude.h"

// ********************************************
// COTEWayPoint
// ********************************************
class COTEWayPoint
{
friend class COTEWayPointMgr;
public:
	COTEWayPoint(void){}
	~COTEWayPoint(void){}

protected:
	
	Ogre::Vector3					m_Position;

	std::list<COTEWayPoint*>		m_Children;

};

// ********************************************
// COTEWayPointRender
// ********************************************
class COTEWayPointRender
{
public:

	static void Render(){}
};

// ********************************************
// COTEWayPointMgr
// ********************************************
class COTEWayPointMgr
{
public:

	// 在某处添加路点

	bool InsertWayPoint(const Ogre::Vector3& position);

	// 添加路点

	COTEWayPoint* CreateWayPoint(){}

	// 删除路点

	void RemoveWayPoint(){}

	// 根据碰撞图自动生成 

	bool AutoGenerateWayPoints(unsigned int* pCollisionMap);

public:

	std::vector<COTEWayPoint*>	    m_WayPoints;	// 路点列表

};


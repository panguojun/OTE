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

	// ��ĳ�����·��

	bool InsertWayPoint(const Ogre::Vector3& position);

	// ���·��

	COTEWayPoint* CreateWayPoint(){}

	// ɾ��·��

	void RemoveWayPoint(){}

	// ������ײͼ�Զ����� 

	bool AutoGenerateWayPoints(unsigned int* pCollisionMap);

public:

	std::vector<COTEWayPoint*>	    m_WayPoints;	// ·���б�

};


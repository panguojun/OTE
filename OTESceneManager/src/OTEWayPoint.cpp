#include ".\otewaypoint.h"
// ----------------------------------------------

#define MAX_WAYPOINT_DIS			5.0f		// ·������

// ----------------------------------------------
bool COTEWayPointMgr::InsertWayPoint(const Ogre::Vector3& position)
{
	// ��λ���Ƿ�������� 

	std::vector<COTEWayPoint*>::iterator it =  m_WayPoints.begin();
	while(it != m_WayPoints.end())
	{
		// ��·�������·��������

		if(((*it)->m_Position - position).squaredLength() < MAX_WAYPOINT_DIS * MAX_WAYPOINT_DIS)
		{
			return false;
		}

		++ it;
	}

	return true;
}

// ----------------------------------------------
bool COTEWayPointMgr::AutoGenerateWayPoints(unsigned int* pCollisionMap)
{
	return true;
}


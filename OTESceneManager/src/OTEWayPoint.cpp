#include ".\otewaypoint.h"
// ----------------------------------------------

#define MAX_WAYPOINT_DIS			5.0f		// 路点间距离

// ----------------------------------------------
bool COTEWayPointMgr::InsertWayPoint(const Ogre::Vector3& position)
{
	// 此位置是否符合条件 

	std::vector<COTEWayPoint*>::iterator it =  m_WayPoints.begin();
	while(it != m_WayPoints.end())
	{
		// 该路点跟附近路点间隔过密

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


#include "trackball.h"

std::list<EntityEditData_t*>	g_pTrackBallGroup;

// ---------------------------------------------------------------
void TrackBallTrack(EntityEditData_t* ee)
{
	if(g_pTrackBallGroup.size() > 100)
		g_pTrackBallGroup.clear();

	g_pTrackBallGroup.push_back(ee);

}


// ---------------------------------------------------------------
// 清理	
void ClearTrackBall()
{
	std::list<EntityEditData_t*>::iterator it = g_pTrackBallGroup.begin();
	while(it != g_pTrackBallGroup.end())
	{	
		delete *it;		
		++ it;
	}	

	g_pTrackBallGroup.clear();
}


// ---------------------------------------------------------------
// 清理特定类型的	
void ClearTrackBall(const std::string& type)
{
	std::list<EntityEditData_t*>::iterator it = g_pTrackBallGroup.begin();
	while(it != g_pTrackBallGroup.end())
	{	
		if(type == (*it)->EditDataType)
		{
			delete *it;	
			g_pTrackBallGroup.erase(it);
			it = g_pTrackBallGroup.begin();
			continue;
		}
		++ it;
	}	
}
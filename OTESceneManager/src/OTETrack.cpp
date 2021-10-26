#include "OTETrackXmlLoader.h"
#include "OTETrack.h"
#include "OTEMathLib.h"

// -------------------------------------------
using namespace OTE;
using namespace Ogre;

// -------------------------------------------
COTETrack::COTETrack() : 
CInterBase(),
m_AnimMinTime(0.0f),
m_AnimMaxTime(2.0f),
m_LastDelayTimePos(-1.0f),
m_IsBezierCurves(false),
m_TexAddrMode(Ogre::TextureUnitState::TextureAddressingMode::TAM_CLAMP)
{	
	m_IsBind = true; // 轨道是常绑定的
}	

COTETrack::~COTETrack()
{
	Clear();	
}
// -------------------------------------------
//清理轨迹
void COTETrack::Clear()
{
	std::list <COTEKeyFrame*>::iterator it = m_KeyFrameList.begin();
	while(it != m_KeyFrameList.end())
	{
		if(*it != NULL)			
			delete *it;			

		++ it;
	}

	m_KeyFrameList.clear();
}

// -------------------------------------------
//添加关键桢	
bool COTETrack::AddKeyFrame(COTEKeyFrame* K)
{
	assert(K);	

	OTE_TRACE("关键桢: " << K->m_Time)

	// 如果列表为空
	if(m_KeyFrameList.empty())
	{
		m_KeyFrameList.push_back(K);
		return true;
	}

	// 列表中第一桢时间大于添加的桢	
	if((*m_KeyFrameList.begin())->m_Time > K->m_Time)
	{
		m_KeyFrameList.push_front(K);
		return true;
	}

	// 如果列表中最后一桢时间小于添加的桢
	std::list <COTEKeyFrame*>::iterator end = m_KeyFrameList.end();		
	if((*(--end))->m_Time < K->m_Time)
	{
		m_KeyFrameList.push_back(K);
		return true;
	}


	// 添加桢 位于第一桢根最后一桢之间
	std::list <COTEKeyFrame*>::iterator it = m_KeyFrameList.begin();
	for(; it != m_KeyFrameList.end(); it ++)
	{		
		// 如果存在更晚的桢则添加到其后
		if(((*it)->m_Time - K->m_Time) > 0.001f)
		{
			m_KeyFrameList.insert(it, K);
			return true; 
		}
	}

	return false;
}
// -------------------------------------------
//删除关键桢
void COTETrack::RemoveKeyFrameAt(float Time)
{
	std::list <COTEKeyFrame*>::iterator it = m_KeyFrameList.begin();
	while(it != m_KeyFrameList.end())
	{
		if(abs((*it)->m_Time - Time) < 0.001f)
		{
			OTE_TRACE("删除关键桢: " << Time)
			m_KeyFrameList.erase(it);
			break;
		}
		++ it;
	}
}
// -------------------------------------------
//查询某一关键桢
COTEKeyFrame* COTETrack::GetKeyFrameAt(float Time)
{
	std::list <COTEKeyFrame*>::iterator it = m_KeyFrameList.begin();
	while(it != m_KeyFrameList.end())
	{
		if(abs((*it)->m_Time - Time) < 0.001f)
		{
			return *it;
		}
		++ it;
	}

	return NULL;
}

// -------------------------------------------
//查询某一桢
COTEKeyFrame* COTETrack::GetFrameAt(float Time)
{
	static COTEKeyFrame keyFrame;

	std::list <COTEKeyFrame*>::iterator it = m_KeyFrameList.begin();
	std::list <COTEKeyFrame*>::iterator endIt = m_KeyFrameList.end();

	if(it != endIt)
	{
		// 如果早于第一桢			
		if((*it)->m_Time >= Time)
		{
			return (*it);
		}
		// 如果晚于最后一桢
		if((*(-- endIt))->m_Time <= Time)
		{
			return (*endIt);
		}	
		
		// 之间的情况

		while(it != m_KeyFrameList.end())
		{
			if(abs((*it)->m_Time - Time) < 0.001f)
			{
				return (*it);
			}

			if((*it)->m_Time > Time && (*(--it))->m_Time < Time)
			{
				std::list <COTEKeyFrame*>::iterator i = it++;

				GetKeyAt(*i, *it, Time, &keyFrame);
				return &keyFrame;
			}

			++ it;
		}
	}
	
	//throw;
	return NULL;
}

// -------------------------------------------
//得到中间桢

void COTETrack::GetKeyAt(const COTEKeyFrame* K1, const COTEKeyFrame* K2,  float Time, COTEKeyFrame* Ki)
{
	// 边界	

	if(Time < K1->m_Time)
		Time = K1->m_Time;
	if(Time > K2->m_Time)
		Time = K2->m_Time;
	
	Ki->m_Time = Time;	

	// 混合

	COTEKeyFrame::BindFrames(Time, K1, K2, this, Ki);

}

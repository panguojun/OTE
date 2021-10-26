#include "oteruntimeanalytics.h"
#include "OTED3DManager.h"

using namespace OTE;

#define MAX_REPORT_SIZE 2000			// 最大的记录调用次数记录

// ==========================================================
// COTERunTimeAnalytics
// ==========================================================
class CRunTimeAnalyticsFrameListener : public Ogre::FrameListener
{
public:
	static CRunTimeAnalyticsFrameListener* s_pInst;
	
	CRunTimeAnalyticsFrameListener() :
	m_FrameTKCnt(0)
	{		
	}

	virtual ~CRunTimeAnalyticsFrameListener()
	{
	}

public:	 

	void showDebugOverlay(bool show){}

	// --------------------------------------------
	bool frameStarted(const Ogre::FrameEvent& evt)
	{		
		return true;
	}

	// --------------------------------------------
	bool frameEnded(const Ogre::FrameEvent& evt)
	{	
		m_FrameTKCnt ++;
		return true;
	}

public:	 

	// 帧计数

	unsigned int		m_FrameTKCnt;
};

CRunTimeAnalyticsFrameListener* CRunTimeAnalyticsFrameListener::s_pInst = NULL;

// ==========================================================
// COTERunTimeAnalytics
// ==========================================================
std::map<std::string, COTERunTimeAnalytics*> COTERunTimeAnalytics::s_AnalyticsTable;

COTERunTimeAnalytics::COTERunTimeAnalytics(const std::string& rName)
{
	m_Name = rName;
}

COTERunTimeAnalytics::~COTERunTimeAnalytics(void)
{	
}

// ----------------------------------------------------------
void COTERunTimeAnalytics::BeginAnalytics(const std::string& rFile, const std::string& rFunction)
{
	std::stringstream ss;
	ss << rFile << " " << rFunction;

	BeginAnalytics(ss.str());	
}

// ----------------------------------------------------------
void COTERunTimeAnalytics::BeginAnalytics(const std::string& rName)
{
	// 有必要初始化？ 

	if(!CRunTimeAnalyticsFrameListener::s_pInst)
	{
		CRunTimeAnalyticsFrameListener::s_pInst = new CRunTimeAnalyticsFrameListener();
		Ogre::Root::getSingleton().addFrameListener(CRunTimeAnalyticsFrameListener::s_pInst);
	}

	COTERunTimeAnalytics* rts = _GetAnalytics(rName);
	if(!rts)
	{
		rts = new COTERunTimeAnalytics(rName);

		_AddAnalytics(rName, rts);
	}	
	
	rts->m_BeginTK = ::GetTickCount();	

}

// ----------------------------------------------------------
void COTERunTimeAnalytics::EndAnalytics(const std::string& rFile, const std::string& rFunction)
{
	static char buff[256];
	sprintf(buff, "%s %s", rFile.c_str(), rFunction.c_str());
	EndAnalytics(buff);	
}

// ----------------------------------------------------------
void COTERunTimeAnalytics::EndAnalytics(const std::string& rName)
{
	COTERunTimeAnalytics* rts = _GetAnalytics(rName);
	if(!rts)
	{
		OTE_MSG_ERR("性能分析器 没有找到 这个分析单元 : " << rName);
		return;
	}	

	// 过大删除老的记录

	if(rts->m_DTimeList.size() >= MAX_REPORT_SIZE)
	{		
		rts->m_DTimeList.pop_front();		
	}

	rts->m_DTimeList.push_back(
		std::pair<int, float>(CRunTimeAnalyticsFrameListener::s_pInst->m_FrameTKCnt, (::GetTickCount() - rts->m_BeginTK) / 1000.0f)
		);

}

// ----------------------------------------------------------
void COTERunTimeAnalytics::Init()
{
	if(!CRunTimeAnalyticsFrameListener::s_pInst)
	{
		CRunTimeAnalyticsFrameListener::s_pInst = new CRunTimeAnalyticsFrameListener();
		Ogre::Root::getSingleton().addFrameListener(CRunTimeAnalyticsFrameListener::s_pInst);
	}

}

// ----------------------------------------------------------
void COTERunTimeAnalytics::_AddAnalytics(const std::string& rName, COTERunTimeAnalytics* rta)
{
	// 已经存在的

	COTERunTimeAnalytics* oldAts = NULL;
	std::map<std::string, COTERunTimeAnalytics*>::iterator it =  s_AnalyticsTable.find(rName);
	if(it != s_AnalyticsTable.end())
	{
		oldAts = it->second;
		OTE_TRACE("已经存在这个分析器单元！添加失败！")
		return;
	}
	
	// 添加到列表 

	s_AnalyticsTable[rName] = rta;

}

// ----------------------------------------------------------
COTERunTimeAnalytics* COTERunTimeAnalytics::_GetAnalytics(const std::string& rName)
{
	std::map<std::string, COTERunTimeAnalytics*>::iterator it =  s_AnalyticsTable.find(rName);
	if(it == s_AnalyticsTable.end())
	{
		return NULL;
	}

	return it->second;
}

// ----------------------------------------------------------
void  COTERunTimeAnalytics::ReportAnalytics()
{
	float totalDTime = 0;
	float maxDTime = 0.0f;
	float minDTime = MAX_FLOAT;

	std::list< std::pair<int, float> >::iterator it = m_DTimeList.begin();
	while(it != m_DTimeList.end())
	{
		float dTime = (*it ++).second;

		totalDTime += dTime;

		if(maxDTime < dTime)
			maxDTime = dTime;

		if(minDTime > dTime)
			minDTime = dTime;
	}

	m_TimeCost = totalDTime / float(m_DTimeList.size());			// 平均占用时间 
	
	if(COTED3DManager::s_FrameTime > 0)
		m_TimePercent =	m_TimeCost / COTED3DManager::s_FrameTime;	// 占用时间百分比 	

	m_WorstTimeCost	=	maxDTime;									// 最长调用时间使用

	m_BestTimeCost	=	minDTime;									// 最短调用时间使用
	
}

// ----------------------------------------------------------
void COTERunTimeAnalytics::PrintAnalyticsReport()
{
	if(m_DTimeList.empty())
		return;

	// 先分析

	ReportAnalytics();

	// 打印信息

	OTE_LOGMSG("单元名称： "     << m_Name);

	unsigned int invakNum = m_DTimeList.size();

	OTE_LOGMSG("调用记录次数： " << invakNum)

	int frameCnt = m_DTimeList.back().first - m_DTimeList.front().first + 1;

	OTE_LOGMSG("调用帧数量： "	 << frameCnt)

	OTE_LOGMSG("平均调用时间： " << m_TimeCost << " 平均帧时间比重： " <<  (m_TimeCost * invakNum / frameCnt) / COTED3DManager::s_FrameTime << " 一次调用所占帧时间比重：" << m_TimeCost / COTED3DManager::s_FrameTime)

	OTE_LOGMSG("最差调用时间： " << m_WorstTimeCost)

	OTE_LOGMSG("最好调用时间： " << m_BestTimeCost)

	
}

// ----------------------------------------------------------
void COTERunTimeAnalytics::_PrintAnalyticsReport()
{
	if(s_AnalyticsTable.empty())
		return;

	OTE_LOGMSG("======= 性能分析结果报告 ========")

	std::map<std::string, COTERunTimeAnalytics*>::iterator it =  s_AnalyticsTable.begin();
	while(it != s_AnalyticsTable.end())
	{
		it->second->PrintAnalyticsReport();
		++ it;
	}	

	OTE_LOGMSG("=================================")	
}

// ----------------------------------------------------------
void COTERunTimeAnalytics::Clear()
{
	std::map<std::string, COTERunTimeAnalytics*>::iterator it =  s_AnalyticsTable.begin();
	while(it != s_AnalyticsTable.end())
	{
		delete it->second;
		++ it;
	}	
	s_AnalyticsTable.clear();
}


// ----------------------------------------------------------
void COTERunTimeAnalytics::Destory()
{
	// 摧毁之前打印信息

	_PrintAnalyticsReport();

	Clear();
}



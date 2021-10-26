#pragma once
#include "OTEStaticInclude.h"

namespace OTE
{

// *********************************************
// COTERunTimeAnalytics
// 性能分析器 
// 用于分析运行性能的类
// *********************************************

class _OTEExport COTERunTimeAnalytics
{
public:

	COTERunTimeAnalytics(const std::string& rName);

	~COTERunTimeAnalytics(void);

public:

	// 使用接口 

	static void BeginAnalytics(const std::string& rFile, const std::string& rFunction);

	static void BeginAnalytics(const std::string& rName);

	static void EndAnalytics(const std::string& rFile, const std::string& rFunction);

	static void EndAnalytics(const std::string& rName);


	// 初始化 与 摧毁  

	static void Init();

	static void Clear();

	static void Destory();

public:

	// 添加 

	static void _AddAnalytics(const std::string& rName, COTERunTimeAnalytics* rta);

	static COTERunTimeAnalytics* _GetAnalytics(const std::string& rName);

	// 打印分析报告

	static void _PrintAnalyticsReport();

protected:

	// 收集信息

	void BeginAnalytics();

	void EndAnalytics();

	// 分析 准备报告

	void  ReportAnalytics();

	// 打印信息 

	void PrintAnalyticsReport();

protected:

	// 分析结果 

	static std::map<std::string, COTERunTimeAnalytics*> s_AnalyticsTable;

	// 时间 数据 统计 

	std::list< std::pair<int, float> >	m_DTimeList;

	DWORD								m_BeginTK;	// 开始标记点时间

	/// 分析各项

	std::string	m_Name;							// 名称

	float m_TimeCost;							// 平均时间 

	float m_TimePercent;						// 时间百分比(timeCost/帧时间)

	float m_WorstTimeCost;						// 最长调用时间使用

	float m_BestTimeCost;						// 最短调用时间使用

};


// *********************************************
// 程序使用的宏 

#	ifdef	TIMECOST_ANS

#		define	TIMECOST_START	{COTERunTimeAnalytics::BeginAnalytics(__FILE__,__FUNCTION__);}

#		define	__TCS	TIMECOST_START

#		define	TIMECOST_END	{COTERunTimeAnalytics::EndAnalytics(__FILE__,__FUNCTION__);  }

#		define	__TCE	TIMECOST_END

#		define	TIMECOST_START_NAMED(name)	{COTERunTimeAnalytics::BeginAnalytics(name);}

#		define	TIMECOST_END_NAMED(name)	{COTERunTimeAnalytics::EndAnalytics(name);  }

#	else

#		define	TIMECOST_START	

#		define	__TCS	TIMECOST_START

#		define	TIMECOST_END	

#		define	__TCE	TIMECOST_END

#		define	TIMECOST_START_NAMED(name)	

#		define	TIMECOST_END_NAMED(name)

#	endif

}

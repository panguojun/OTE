#pragma once
#include "OTEStaticInclude.h"

namespace OTE
{

// *********************************************
// COTERunTimeAnalytics
// ���ܷ����� 
// ���ڷ����������ܵ���
// *********************************************

class _OTEExport COTERunTimeAnalytics
{
public:

	COTERunTimeAnalytics(const std::string& rName);

	~COTERunTimeAnalytics(void);

public:

	// ʹ�ýӿ� 

	static void BeginAnalytics(const std::string& rFile, const std::string& rFunction);

	static void BeginAnalytics(const std::string& rName);

	static void EndAnalytics(const std::string& rFile, const std::string& rFunction);

	static void EndAnalytics(const std::string& rName);


	// ��ʼ�� �� �ݻ�  

	static void Init();

	static void Clear();

	static void Destory();

public:

	// ��� 

	static void _AddAnalytics(const std::string& rName, COTERunTimeAnalytics* rta);

	static COTERunTimeAnalytics* _GetAnalytics(const std::string& rName);

	// ��ӡ��������

	static void _PrintAnalyticsReport();

protected:

	// �ռ���Ϣ

	void BeginAnalytics();

	void EndAnalytics();

	// ���� ׼������

	void  ReportAnalytics();

	// ��ӡ��Ϣ 

	void PrintAnalyticsReport();

protected:

	// ������� 

	static std::map<std::string, COTERunTimeAnalytics*> s_AnalyticsTable;

	// ʱ�� ���� ͳ�� 

	std::list< std::pair<int, float> >	m_DTimeList;

	DWORD								m_BeginTK;	// ��ʼ��ǵ�ʱ��

	/// ��������

	std::string	m_Name;							// ����

	float m_TimeCost;							// ƽ��ʱ�� 

	float m_TimePercent;						// ʱ��ٷֱ�(timeCost/֡ʱ��)

	float m_WorstTimeCost;						// �����ʱ��ʹ��

	float m_BestTimeCost;						// ��̵���ʱ��ʹ��

};


// *********************************************
// ����ʹ�õĺ� 

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

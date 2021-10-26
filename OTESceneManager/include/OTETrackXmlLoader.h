#pragma once

#include "OTEStaticInclude.h"
#include "tinyxml.h"
#include "OTETrack.h"

namespace OTE
{
// ***********************************************
// 轨道加载器
// ***********************************************

class _OTEExport COTETrackXmlLoader : public COTETrackLoader
{

public:
	COTETrackXmlLoader();	
	COTETrackXmlLoader(const std::string& TrackFile);
	~COTETrackXmlLoader();

public:
	void Init(const std::string& TrackFile);
	void Destroy();

public:
	
	//把TRACK信息写到xml
	virtual void TrackWriteXml(int id);

	//保存Track文件
	virtual void SaveFile(const std::string& fileName = "");
			
	//加载指定ID的Track
	virtual COTETrack* LoadTrack(unsigned int id);

	//移除Track
	virtual void RemoveTrack(int id);

	bool IsInit()
	{
		return m_pTrackDoc != NULL;
	}

protected:

	void TrackWriteXml(int id, COTETrack* track);
	
protected:

	//Track资源文件
	TiXmlDocument*	m_pTrackDoc;

};

}
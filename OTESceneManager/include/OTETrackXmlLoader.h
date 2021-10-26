#pragma once

#include "OTEStaticInclude.h"
#include "tinyxml.h"
#include "OTETrack.h"

namespace OTE
{
// ***********************************************
// ���������
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
	
	//��TRACK��Ϣд��xml
	virtual void TrackWriteXml(int id);

	//����Track�ļ�
	virtual void SaveFile(const std::string& fileName = "");
			
	//����ָ��ID��Track
	virtual COTETrack* LoadTrack(unsigned int id);

	//�Ƴ�Track
	virtual void RemoveTrack(int id);

	bool IsInit()
	{
		return m_pTrackDoc != NULL;
	}

protected:

	void TrackWriteXml(int id, COTETrack* track);
	
protected:

	//Track��Դ�ļ�
	TiXmlDocument*	m_pTrackDoc;

};

}
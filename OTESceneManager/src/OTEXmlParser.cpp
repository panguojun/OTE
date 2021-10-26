#include "otexmlparser.h"
#include "tinyxml.h"
#include "OTESceneXmlLoader.h"

// -------------------------------------------------
// ��xml�ļ�����

bool COTEXmlParser::ParseXmlFile(const char* rXmlFile, const char* rTargetName)
{
	Ogre::DataStreamPtr stream = RESMGR_LOAD(rXmlFile);
	
	if(stream.isNull())
	{
		OTE_LOGMSG("ParseXmlFile û���ҵ��ļ�: " << rXmlFile)
		return false;
	}

	return ParseXmlString(stream->getAsString().c_str(), rTargetName);
	
}

// -------------------------------------------------
// ��һ��xml�ַ�������

bool COTEXmlParser::ParseXmlString(const char* rXmlFile, const char* rTargetName)
{
	// �������

	if(rTargetName[0] == '\0')
	{
		return OTE::COTESceneXmlLoader::GetInstance()->ParseSceneXml(rXmlFile);
	}
	
	// ɽ��

	else if(strncmp(rTargetName, "Terrain", 5))
	{
		// todo
	}
	
	// ��ɫ/���

	else
	{
		
	}

	return false;
}



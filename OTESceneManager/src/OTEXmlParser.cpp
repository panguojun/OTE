#include "otexmlparser.h"
#include "tinyxml.h"
#include "OTESceneXmlLoader.h"

// -------------------------------------------------
// 从xml文件创建

bool COTEXmlParser::ParseXmlFile(const char* rXmlFile, const char* rTargetName)
{
	Ogre::DataStreamPtr stream = RESMGR_LOAD(rXmlFile);
	
	if(stream.isNull())
	{
		OTE_LOGMSG("ParseXmlFile 没有找到文件: " << rXmlFile)
		return false;
	}

	return ParseXmlString(stream->getAsString().c_str(), rTargetName);
	
}

// -------------------------------------------------
// 从一段xml字符串创建

bool COTEXmlParser::ParseXmlString(const char* rXmlFile, const char* rTargetName)
{
	// 场景物件

	if(rTargetName[0] == '\0')
	{
		return OTE::COTESceneXmlLoader::GetInstance()->ParseSceneXml(rXmlFile);
	}
	
	// 山体

	else if(strncmp(rTargetName, "Terrain", 5))
	{
		// todo
	}
	
	// 角色/物件

	else
	{
		
	}

	return false;
}



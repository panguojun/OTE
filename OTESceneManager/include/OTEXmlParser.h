#pragma once
/************************************************************
 ************************************************************

 引擎外部使用XML解析接口类

 *************************************************************
 *************************************************************/
class __declspec(dllexport) COTEXmlParser
{
public:
	
	static bool ParseXmlFile(const char* rXmlFile, const char* rTargetName);

	static bool ParseXmlString(const char* rXmlFile, const char* rTargetName);

};

#pragma once
/************************************************************
 ************************************************************

 �����ⲿʹ��XML�����ӿ���

 *************************************************************
 *************************************************************/
class __declspec(dllexport) COTEXmlParser
{
public:
	
	static bool ParseXmlFile(const char* rXmlFile, const char* rTargetName);

	static bool ParseXmlString(const char* rXmlFile, const char* rTargetName);

};

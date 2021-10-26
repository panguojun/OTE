#pragma once
#include "OTEStaticInclude.h"
#include "OTEReactorBase.h"
#include "tinyxml.h"

namespace OTE
{
// ***********************************
// class COTEReactorXmlLoader
// 生化反应 加载器
// 
// ***********************************

class COTEReactorXmlLoader
{

public:
	COTEReactorXmlLoader();	
	COTEReactorXmlLoader(const std::string& ReactorFile);
	~COTEReactorXmlLoader();

public:
	void Init(const std::string& ReactorFile);
	void Destroy();

public:
	
	// 把Reactor信息写到xml
	void ReactorWriteXml(int id);

	// 保存Track文件
	void SaveFile(const std::string& FileName = "");
			
	// 加载指定ID的
	CReactorBase* LoadReactor(unsigned int id);

	// 移除
	void RemoveReactor(int id);

protected:

	void ReactorWriteXml(int ID, CReactorBase* Reactor);
	
protected:

	// Xml资源文件
	TiXmlDocument*		m_pXmlDoc;

};

}

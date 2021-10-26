#pragma once

#include "OTEStaticInclude.h"
#include "OTEAction.h"
#include "OTEActorEntity.h"
#include "tinyxml.h"

namespace OTE
{
// ************************************************ 
// COTEActionXmlLoader
// ************************************************ 

struct ActionDataMap_t
{
	HashMap<int, COTEActionData*> DataMap;	
	TiXmlDocument*	pTiXmlDoc;
};

class _OTEExport COTEActionXmlLoader
{
public:

	static COTEActionXmlLoader* GetSingleton(void);

	static void DestroySingleton();

	// 读取行动数据	

	void ReadActionData(const std::string& rActionFile = "");

	// 保存行动数据

	void SaveAllActionData();

	void SaveActionData(const std::string& rActionFile = "");

	void SaveActionData(COTEActionData* pActionData, const std::string& rActionFile = "");

	void SaveActionData(COTEActionData* pActionData, TiXmlElement* BaseElm);

	// 查询行动数据

	COTEActionData* GetActionData(int ActID);

	COTEActionData* GetActionData(const std::string& rActionName);	

	// 创建行动数据

	COTEActionData* CreateActionData(const std::string& rActionName, const std::string& rActionFile = "");

	// 删除行动数据

	void DeleteActionData(const std::string& rActionName, const std::string& rActionFile = "");

	void RenameActionData(const std::string& rActionName, const std::string& rNewActionName);

protected:

	std::string AutoFixActionFileName(const std::string& rActionFile);

	COTEActionXmlLoader();
	~COTEActionXmlLoader();

public:

	// 行动数据列表

	HashMap<std::string, ActionDataMap_t>	m_ActionDataMap;

protected:

	static COTEActionXmlLoader*			s_pSingleton;
};

}
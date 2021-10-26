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

	// ��ȡ�ж�����	

	void ReadActionData(const std::string& rActionFile = "");

	// �����ж�����

	void SaveAllActionData();

	void SaveActionData(const std::string& rActionFile = "");

	void SaveActionData(COTEActionData* pActionData, const std::string& rActionFile = "");

	void SaveActionData(COTEActionData* pActionData, TiXmlElement* BaseElm);

	// ��ѯ�ж�����

	COTEActionData* GetActionData(int ActID);

	COTEActionData* GetActionData(const std::string& rActionName);	

	// �����ж�����

	COTEActionData* CreateActionData(const std::string& rActionName, const std::string& rActionFile = "");

	// ɾ���ж�����

	void DeleteActionData(const std::string& rActionName, const std::string& rActionFile = "");

	void RenameActionData(const std::string& rActionName, const std::string& rNewActionName);

protected:

	std::string AutoFixActionFileName(const std::string& rActionFile);

	COTEActionXmlLoader();
	~COTEActionXmlLoader();

public:

	// �ж������б�

	HashMap<std::string, ActionDataMap_t>	m_ActionDataMap;

protected:

	static COTEActionXmlLoader*			s_pSingleton;
};

}
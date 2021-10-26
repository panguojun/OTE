#pragma once
#include "PluginDll.h"
#include "OTEPluginInterface.h"
#include <vector>
#include <windows.h>

// ************************************************
// COTEPluginManager
// ************************************************
namespace OTE
{

typedef void (*DLL_START_PLUGIN)(void);
typedef void (*DLL_STOP_PLUGIN)(void);

class _OTEExport COTEPluginManager  
{
public:
	// Plugin
	void LoadPlugins(const char* szPlgCfgName);

	void StartPlugins();

	void EndPlugins();

	void UnloadPlugins();

	void Clear();

	int GetPluginNum()
	{
		return (int)m_PluginList.size();
	}
    
	// Plugin Interface
	void AddPluginInterface(COTEPluginInterface* pPI)
	{
		m_PluginInterfaceList.push_back(pPI);
	}

	void RemovePluginInterface(int Index)
	{
		if( Index < (int)m_PluginInterfaceList.size())
			m_PluginInterfaceList.erase(m_PluginInterfaceList.begin() + Index);	
	}

	void RemovePluginInterface(COTEPluginInterface* pPI)
	{
		for(unsigned int i = 0; i < m_PluginInterfaceList.size(); i++)
		{
			if(pPI == m_PluginInterfaceList[i])
			{
				RemovePluginInterface(i);
				break;
			}
		}
	}

	void RemoveAllPluginInterfaces()
	{
		m_PluginInterfaceList.clear();	
	}

	int GetPluginInterfaceNum()
	{
		return (int)m_PluginInterfaceList.size();
	}

	COTEPluginInterface* GetPluginInterface(int Index)
	{
		if( Index < (int)m_PluginInterfaceList.size())
			return m_PluginInterfaceList[Index];
		return NULL;
	}

	void SendCustomMessage(const std::string& msg)
	{
		for(unsigned int i = 0; i < m_PluginInterfaceList.size(); i++)
		{
			m_PluginInterfaceList[i]->OnCustomMessage(msg);
		}
	}

	const std::string& GetUserString(const std::string& name)
	{
		std::map<std::string, std::string>::iterator it = m_UserStringMap.find(name);
		if(it != m_UserStringMap.end())
		{
			return it->second;
		}

		static std::string sEmptyString = "";
		return sEmptyString;
	}
	
	void SetUserString(const std::string& name, const std::string& val)
	{
		m_UserStringMap[name] = val;
	}

public:

	static COTEPluginManager* GetInstance();

protected:

	COTEPluginManager();
	~COTEPluginManager();

public:

	std::vector<COTEPluginInterface*>	m_PluginInterfaceList;

	// 窗口句柄 

	HWND								m_hMainWnd;
	
protected:
	
	static COTEPluginManager			sPluginManager;

	// 插件列表

	std::vector<CPluginDll*>			m_PluginList;

	// 用户数据 

	std::map<std::string, std::string>	m_UserStringMap;

};

}
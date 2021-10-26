//////////////////////////////////////////////////////////////////////
#include <io.h>
#include <sstream>
#include "OTEPluginManager.h"
#include "OTEQTSceneManager.h"
// ---------------------------------------------------------
using namespace OTE;

// ---------------------------------------------------------
COTEPluginManager COTEPluginManager::sPluginManager;
COTEPluginManager* COTEPluginManager::GetInstance()
{
	return &sPluginManager;
}
// ---------------------------------------------------------
COTEPluginManager::COTEPluginManager()
{	
}

COTEPluginManager::~COTEPluginManager()
{	
	Clear();
}

// ---------------------------------------------------------
void COTEPluginManager::Clear()
{
	for( unsigned int i = 0; i < m_PluginList.size(); i ++ )
	{
		m_PluginList.at(i)->Unload();
		delete m_PluginList.at(i);
	}	
	m_PluginList.clear();

}
// ---------------------------------------------------------
void COTEPluginManager::LoadPlugins(const char* szPlgCfgName)
{	
	Clear();

	FILE* file = fopen(szPlgCfgName, "r");
	if(!file)
	{
		std::stringstream ss;
		ss << "Faild to open plugin config file: " << szPlgCfgName << "!\n";
		//::MessageBox(NULL, ss.str().c_str(), "Error", MB_OK | MB_ICONWARNING);
		::OutputDebugString(ss.str().c_str ());
		return;
	}

	char szLine[256];
	while (fgets (szLine, 256, file) )	
	{	
		if (!strncmp (szLine, "//", 2))
			continue;

		char szLN[128];
		sscanf(szLine, "%s", szLN);

		if(std::string(szLine).find("dll", 0) != std::string::npos)
			m_PluginList.push_back(new CPluginDll(szLN));
	}

	fclose(file);

	// load plugin
	for( unsigned int i = 0; i < m_PluginList.size(); i ++ )
	{
		m_PluginList.at(i)->Load();
	}

	if(m_PluginList.size() > 0)
		SCENE_MGR->m_IsEditor = true;
}

// ---------------------------------------------------------
void COTEPluginManager::StartPlugins()
{	
	for( unsigned int i = 0; i < m_PluginList.size(); i ++ )
	{
		// Call startup function
		DLL_START_PLUGIN pFunc = (DLL_START_PLUGIN)m_PluginList.at(i)->GetSymbol("dllStartPlugin");
		if (!pFunc)
		{
			std::stringstream ss;
			ss << "Can't find plugin's startup function in plugin : " << m_PluginList.at(i)->m_Name << ".";
			::MessageBox(NULL, ss.str().c_str(), "Error",  MB_OK | MB_ICONWARNING);
		}
		else
		{
			pFunc();
		}
	}
}
// ---------------------------------------------------------
void COTEPluginManager::EndPlugins()
{
	for( unsigned int i = 0; i < m_PluginList.size(); i ++ )
	{
		// Call starup function
		DLL_STOP_PLUGIN pFunc = (DLL_STOP_PLUGIN)m_PluginList.at(i)->GetSymbol("dllStopPlugin");
		if (!pFunc)
		{
			std::stringstream ss;
			ss << "Can't find plugin's end function in plugin : " << m_PluginList.at(i)->m_Name << ".";
			::MessageBox(NULL, ss.str().c_str(), "Error",  MB_OK | MB_ICONWARNING);
		
		}
		else
		{
			pFunc();
		}	

	}

	SCENE_MGR->m_IsEditor = false;
}

// ---------------------------------------------------------
void COTEPluginManager::UnloadPlugins()
{
	for( unsigned int i = 0; i < m_PluginList.size(); i ++ )
		m_PluginList.at(i)->Unload();	
}
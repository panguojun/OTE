#include "OTEActionXmlLoader.h"
#include "OTEKeyFrame.h"
#include "OTETrack.h"
#include "OTEQTSceneManager.h"
#include "OTEActorActionMgr.h"

// ---------------------------------------------
using namespace Ogre;
using namespace OTE;

// ---------------------------------------------
COTEActionXmlLoader* COTEActionXmlLoader::s_pSingleton = NULL;
COTEActionXmlLoader* COTEActionXmlLoader::GetSingleton(void)
{
	if(!s_pSingleton)
	{	
		s_pSingleton = new COTEActionXmlLoader();		
	}
    return s_pSingleton;
}

// ---------------------------------------------
void COTEActionXmlLoader::DestroySingleton()
{
	SAFE_DELETE(s_pSingleton)
}


// ---------------------------------------------
//构造函数
COTEActionXmlLoader::COTEActionXmlLoader()
{
	ReadActionData();
}
// ---------------------------------------------
//析构函数
COTEActionXmlLoader::~COTEActionXmlLoader(void)
{	
	HashMap<std::string, ActionDataMap_t>::iterator actDmIt = m_ActionDataMap.begin();
	while(actDmIt != m_ActionDataMap.end())	
	{
		HashMap<int, COTEActionData*>::iterator it = actDmIt->second.DataMap.begin();
		while(it != actDmIt->second.DataMap.end())
		{
			delete it->second;

			++ it;
		}

		delete	actDmIt->second.pTiXmlDoc;

		++ actDmIt;
	}

}


// ---------------------------------------------
COTEActionData* COTEActionXmlLoader::GetActionData(int ActID)
{
	HashMap<std::string, ActionDataMap_t>::iterator actDmIt = m_ActionDataMap.begin();
	while(actDmIt != m_ActionDataMap.end())	
	{
		HashMap<int, COTEActionData*>::iterator it = actDmIt->second.DataMap.find(ActID);
		if(it != actDmIt->second.DataMap.end())
		{
			return it->second;
		}

		++ actDmIt;
	}
	
	OTE_TRACE("没有找到Action数据 id : " << ActID);
	return NULL;
}

// ---------------------------------------------
COTEActionData* COTEActionXmlLoader::GetActionData(const std::string& rActionName)
{
	// 读取属性

	int actID = 0;
	std::string tailName;
	if(COTEActionData::GetIDFromName(rActionName, actID, tailName))		
		return GetActionData(actID);

	OTE_MSG("行动名称必须是[NUMBER][STRING]格式!", "错误")

	return NULL;
}

// ---------------------------------------------
COTEActionData* COTEActionXmlLoader::CreateActionData(const std::string& rActionName, const std::string& rActionFile)
{
	std::string actionFile = AutoFixActionFileName(rActionFile);

	int actID = 0;
	std::string tailName;
	if(!COTEActionData::GetIDFromName(rActionName, actID, tailName))
		return NULL;

	HashMap<std::string, ActionDataMap_t>::iterator actDmIt = m_ActionDataMap.find(actionFile);
	if(actDmIt != m_ActionDataMap.end())	
	{
		COTEActionData* pActData = new COTEActionData();
		pActData->m_ResName = rActionName;
	
		(actDmIt->second).DataMap[actID] = pActData;
		OTE_TRACE("创建一个行动数据 actID : " << actID << " ActionName: " << rActionName)

		return pActData;
	}	
	else
	{
		OTE_MSG("CreateActionData失败! 没有找到Action数据文件。 actionFile : " << actionFile, "失败" )
	}
	return NULL;
}

// ---------------------------------------------
std::string COTEActionXmlLoader::AutoFixActionFileName(const std::string& rActionFile)
{
	if(rActionFile.empty())
	{
		const Ogre::String& filename = OTE::OTEHelper::GetOTESetting("Misc", "ActionFile");
		
		Ogre::String filePath, fullName;
		OTEHelper::GetFullPath(filename, filePath, fullName);

		return fullName;
	}
	return rActionFile;
}

// ---------------------------------------------
void COTEActionXmlLoader::DeleteActionData(const std::string& rActionName, const std::string& rActionFile)
{
	std::string actionFile = AutoFixActionFileName(rActionFile);

	int actID = 0;
	std::string tailName;
	if(!COTEActionData::GetIDFromName(rActionName, actID, tailName))
		return;

	HashMap<std::string, ActionDataMap_t>::iterator actDmIt = m_ActionDataMap.find(actionFile);
	if(actDmIt != m_ActionDataMap.end())	                










	{
		HashMap<int, COTEActionData*>::iterator it = (actDmIt->second).DataMap.find(actID);
		
		if(it != (actDmIt->second).DataMap.end())
		{
			// 在文件中清除
	
			TiXmlElement* elem = actDmIt->second.pTiXmlDoc->RootElement()->FirstChildElement("Action");
			while(elem)
			{
				if(strcmp(elem->Attribute("Name"), rActionName.c_str()) == 0 )
				{
					actDmIt->second.pTiXmlDoc->RootElement()->RemoveChild(elem);
					elem = actDmIt->second.pTiXmlDoc->RootElement()->FirstChildElement("Action");
					continue;
				}
				elem = elem->NextSiblingElement();
			}		
			
			// 从列表中删除

			OTE_TRACE("删除行动数据: acionData" << it->second->m_ResName)
			delete it->second;
			(actDmIt->second).DataMap.erase(it);
		}
	}	
}


// ---------------------------------------------
// 读取行动

void COTEActionXmlLoader::ReadActionData(const std::string& rActionFile)
{
	std::string actionFile = AutoFixActionFileName(rActionFile);

	OTE_TRACE("读取行动 ActionFile : " << rActionFile << "    开始 ...")

	m_ActionDataMap[actionFile] = ActionDataMap_t();
	ActionDataMap_t& adm = m_ActionDataMap[actionFile];

	adm.pTiXmlDoc = new TiXmlDocument(actionFile);	
	adm.pTiXmlDoc->LoadFile();	
	
	if(!adm.pTiXmlDoc->RootElement())
	{	
		return;
	}

	TiXmlElement *elem = adm.pTiXmlDoc->RootElement()->FirstChildElement("Action");

	while(elem)
	{	
		COTEActionData* pActData = new COTEActionData();

		// 读取属性

		pActData->m_ResName = elem->Attribute("Name");		
		
		int actID = 0;
		std::string tailName;
		if(!COTEActionData::GetIDFromName(pActData->m_ResName, actID, tailName))
		{
			OTE_MSG_ERR("读取行动错误！");
			throw;
		}

		adm.DataMap[actID] = pActData;

		pActData->m_AniName = elem->Attribute("AnimationName");				

		pActData->m_AniSpeed = atof(elem->Attribute("AnimationSpeed"));
		pActData->m_Length = atof(elem->Attribute("Length"));
		pActData->m_InureTime = atof(elem->Attribute("InureTime"));
	
		if(strcmp(elem->Attribute("isAutoPlay"), "true") == 0)
			pActData->m_IsAutoPlay = true;
		else
			pActData->m_IsAutoPlay = false;								

		// 读取效果列表

		TiXmlElement* te = elem->FirstChildElement();
		while(te)				
		{						
			if(strcmp(te->Value(), "Effect") == 0)
			{					
				OTE::ActMagicData_t* m = ActMagicData_t::CreateActMagic();

				strcpy(m->MagicFile, te->Attribute("MagicFile"));
				
				if(strcmp(te->Attribute("InterType"), "Reactor") == 0)						
					m->InterType = ActMagicData_t::eReactor;						
				else						
					m->InterType = ActMagicData_t::eTrack;	
				
				strcpy(m->InterData, te->Attribute("InterData"));	

				m->PlaySpeed = atof(te->Attribute("PlaySpeed"));	
				m->PlayTime = atof(te->Attribute("PlayTime"));

				//将相关特效与轨道加入行动中
				
				pActData->AddEffectData(m, atoi(te->Attribute("InterID")));							
					
			}

			// 加载声音

			else if(strcmp(te->Value(), "Sound") == 0)
			{
				const char* sf = te->Attribute("SoundFile");
				if(sf)
				{
					pActData->m_3DSoundName = sf;							
					pActData->m_3DSoundVolume = atof(te->Attribute("volume"));
					pActData->m_Is3DSoundLoop = strcmp(te->Attribute("IsLoop"), "true") == 0;				
				}
			}
			te = te->NextSiblingElement();			
		}

		OTE_TRACE("  读取ActionData:" << pActData->m_ResName);
		
		elem = elem->NextSiblingElement();
	}

	OTE_TRACE("读取行动 ActionFile : " << rActionFile << "    完成.")
}

// ---------------------------------------------
void COTEActionXmlLoader::SaveAllActionData()
{
	HashMap<std::string, ActionDataMap_t>::iterator actDmIt = m_ActionDataMap.begin();
	while(actDmIt != m_ActionDataMap.end())
	{
		SaveActionData(actDmIt->first);

		++ actDmIt;
	}
}

// ---------------------------------------------
void COTEActionXmlLoader::SaveActionData(COTEActionData* pActionData, const std::string& rActionFile)
{
	int actID = 0;
	std::string tailName;
	if(!COTEActionData::GetIDFromName(pActionData->m_ResName, actID, tailName))
		return;

	std::string actionFile = AutoFixActionFileName(rActionFile);

	HashMap<std::string, ActionDataMap_t>::iterator actDmIt = m_ActionDataMap.find(actionFile);
	if(actDmIt != m_ActionDataMap.end())	
	{		
		TiXmlElement *elem = actDmIt->second.pTiXmlDoc->RootElement();		
		if(!elem)
		{
			elem = new TiXmlElement("Actions");
			actDmIt->second.pTiXmlDoc->InsertEndChild(*elem);
		}

		HashMap<int, COTEActionData*>::iterator it = actDmIt->second.DataMap.find(actID);
		if(it != actDmIt->second.DataMap.end())
		{
			SaveActionData(pActionData, elem);
		}

		// 保存行动文件

		actDmIt->second.pTiXmlDoc->SaveFile();
	}

}

// ---------------------------------------------
void COTEActionXmlLoader::SaveActionData(COTEActionData* pActionData, TiXmlElement* BaseElm)
{
	TiXmlDocument* pXmlDoc = BaseElm->GetDocument();

	// 先清除
	
	TiXmlElement* elem = BaseElm->FirstChildElement("Action");
	while(elem)
	{
		if(strcmp(elem->Attribute("Name"), pActionData->m_ResName.c_str()) == 0 )
		{
			BaseElm->RemoveChild(elem);
			elem = BaseElm->FirstChildElement("Action");
			continue;
		}
		elem = elem->NextSiblingElement();
	}		

	// 将行动逐个写入文件	

	elem = new TiXmlElement("Action");

	/// 属性

	elem->SetAttribute("Name",pActionData->m_ResName);		
	elem->SetAttribute("AnimationName", pActionData->m_AniName);

	char sinfo[32];
	sprintf(sinfo, "%f", pActionData->m_AniSpeed);
	elem->SetAttribute("AnimationSpeed", sinfo);
	sprintf(sinfo, "%f", pActionData->m_Length);
	elem->SetAttribute("Length", sinfo);
	sprintf(sinfo, "%f", pActionData->m_InureTime);
	elem->SetAttribute("InureTime", sinfo);
	
	if(pActionData->m_IsAutoPlay)
		elem->SetAttribute("isAutoPlay", "true");
	else
		elem->SetAttribute("isAutoPlay", "false");		

	// 轨道+反应器数据存盘

	MagBindTrackData_t::iterator i = pActionData->m_MagicInterMapData.begin();
	while(i != pActionData->m_MagicInterMapData.end())
	{
		OTE::ActMagicData_t* m = i->second;
	
		TiXmlElement* te = new TiXmlElement("Effect");

		te->SetAttribute("MagicFile", m->MagicFile);

		if(m->InterType == ActMagicData_t::eTrack)
			te->SetAttribute("InterType", "Track");
		else if(m->InterType == ActMagicData_t::eReactor)
			te->SetAttribute("InterType", "Reactor");

		te->SetAttribute("InterData", m->InterData);
		te->SetAttribute("InterID", m->InterID);
		sprintf(sinfo, "%f", m->PlaySpeed);
		te->SetAttribute("PlaySpeed",sinfo);
		sprintf(sinfo, "%f", m->PlayTime);
		te->SetAttribute("PlayTime",sinfo);

		elem->InsertEndChild(*te);
						
		i++;
	}		
	
	// 声音

	if(!pActionData->m_3DSoundName.empty())
	{
		TiXmlElement* te = new TiXmlElement("Sound");

		te->SetAttribute("SoundFile", pActionData->m_3DSoundName);	

		sprintf(sinfo, "%f", pActionData->m_3DSoundVolume);
		te->SetAttribute("volume", sinfo);	

		if(pActionData->m_Is3DSoundLoop)
			te->SetAttribute("IsLoop", "true");	
		else
			te->SetAttribute("IsLoop", "false");		
		
		elem->InsertEndChild(*te);
	}	

	BaseElm->InsertEndChild(*elem);
	
	OTE_TRACE("保存ActionData:" << pActionData->m_ResName)
	

}

// ---------------------------------------------
//保存行动
void COTEActionXmlLoader::SaveActionData(const std::string& rActionFile)
{
	std::string actionFile = AutoFixActionFileName(rActionFile);

	HashMap<std::string, ActionDataMap_t>::iterator actDmIt = m_ActionDataMap.find(actionFile);
	if(actDmIt != m_ActionDataMap.end())	
	{	
		// 清理

		//if(isClearPre && actDmIt->second.pTiXmlDoc)
		//	actDmIt->second.pTiXmlDoc->Clear();

		HashMap<int, COTEActionData*>::iterator it = actDmIt->second.DataMap.begin();
		while(it != actDmIt->second.DataMap.end())
		{
			SaveActionData(it->second, actionFile);

			++ it;
		}
	}

	OTE_TRACE("保存行动 ActionFile : " << actionFile << " 完成.")

}

// ---------------------------------------------
//重命名

void COTEActionXmlLoader::RenameActionData(
	const std::string& rActionName, const std::string& rNewActionName)
{	
	int actID = 0, newActID = 0;
	std::string tailName;

	if(!COTEActionData::GetIDFromName(rActionName, actID, tailName))
	{
		OTE_MSG("重命名失败！ 老行动名格式错误。 ActionName" << rActionName, "失败")
		return;
	}

	if(!COTEActionData::GetIDFromName(rNewActionName, newActID, tailName))
	{
		OTE_MSG("重命名失败！ 新行动名格式错误。 ActionName" << rNewActionName, "失败")
		return;
	}

	HashMap<std::string, ActionDataMap_t>::iterator actDmIt = m_ActionDataMap.begin();
	while(actDmIt != m_ActionDataMap.end())	
	{
		HashMap<int, COTEActionData*>::iterator it = (actDmIt->second).DataMap.find(actID);
		
		if(it != (actDmIt->second).DataMap.end())
		{
			COTEActionData* ad = it->second;		

			OTE_TRACE("重命名: " << ad->m_ResName << "->" << rNewActionName)

			ad->m_ResName = rNewActionName;
			(actDmIt->second).DataMap.erase(it);

			(actDmIt->second).DataMap[newActID] = ad;
		}

		++ actDmIt;
	}

	
}
#include "oteactoractionmgr.h"
#include "OTEActionXmlLoader.h"

using namespace Ogre;
using namespace OTE;

// --------------------------------------------------------
// 添加行动
COTEAction* COTEActorActionMgr::AddAction(COTEActorEntity* ae, const std::string& actName)
{
	HashMap<std::string, COTEAction*>::iterator it = ae->m_ActionList.find(actName);
	if(it != ae->m_ActionList.end())
	{
		OTE_MSG("添加行动已经存在 ACTOR: " << ae->GetName() << " ACTION:" << actName, "失败")
		return it->second;
	}


	COTEAction* act = COTEAction::CreateAction(ae);
	act->m_Name = actName;
	act->m_ResName = actName;

	COTEActionData* actData = COTEActionXmlLoader::GetSingleton()->GetActionData(actName);
	if(actData)
		actData->Clone(act);
	else
	{
		OTE_TRACE_WARNING("添加行动资源不存在 ACTOR: " << ae->GetName() << " ACTION:" << actName)
	}

	ae->m_ActionList[act->m_Name] = act;

	OTE_TRACE("添加行动 ACTOR: " << ae->GetName() << " ACTION:" << act->GetName())

	return act;
}

// -------------------------------------------------
//删除行动
void COTEActorActionMgr::RemoveAction(COTEActorEntity* ae, const std::string& actName)
{
	HashMap<std::string, COTEAction*>::iterator it = ae->m_ActionList.find(actName);
	if(it != ae->m_ActionList.end())
	{
		COTEAction* act = it->second;	

		if(ae->m_CurAction == act)
			ae->m_CurAction = NULL;

		OTE_TRACE("删除行动 ACTOR: " << ae->GetName() << " ACTION:" << actName)
		delete act;
		ae->m_ActionList.erase(it);
	}		
}	

// -------------------------------------------------
//得到指定的行动
COTEAction* COTEActorActionMgr::GetAction(COTEActorEntity* ae, const std::string& actName)
{
	HashMap<std::string, COTEAction*>::iterator it = ae->m_ActionList.find(actName);
	if(it != ae->m_ActionList.end())
	{	
		return it->second;	
	}
	
	return NULL;
}

// -------------------------------------------------
//更改行动名称
void COTEActorActionMgr::RenameAction(COTEActorEntity* ae, const std::string& rOldName, const std::string& rNewName)
{
	HashMap<std::string, COTEAction*>::iterator it = ae->m_ActionList.find(rOldName);
	if(it != ae->m_ActionList.end())
	{	
		COTEAction* act = it->second;
		if(ae->m_CurAction == act)
			ae->m_CurAction = NULL;

		ae->m_ActionList.erase(it);	

		act->SetName(rNewName);
		ae->m_ActionList[rNewName] = act;
			
		OTE_TRACE("更改行动名称 ACTOR: " << ae->GetName() << " ACTION:" << rOldName << "->" << rNewName)
	}
}

// ----------------------------------------------------
//指定当前行动

COTEAction*  COTEActorActionMgr::SetAction(
						COTEActorEntity* ae, 
						const std::string& actName, 
						COTEAction::PlayMode pMode, 
						std::list<Ogre::MovableObject*>* pTagList)
{	
	if(actName.empty())
	{
		ae->m_CurAction = NULL;
		return NULL;
	}

	if(!COTEActorActionMgr::GetAction(ae, actName))
	{
		COTEActorActionMgr::AddAction(ae, actName);
	}

	// 行动已经在播放

	if(  ae->m_CurAction && 
		 ae->m_CurAction->m_Name == actName && 
	    (ae->m_CurAction->m_Mode == pMode ||
		(ae->m_CurAction->m_Mode == COTEAction::eSTOP &&
		pMode != COTEAction::eCONTINUES))
		) 
	{
		if(ae->m_CurAction->m_Mode == COTEAction::eSTOP)
			ae->m_CurAction->Play(pMode);

		return ae->m_CurAction;
	}

	OTE_TRACE(ae->GetName() << " SetAction: " << actName)

	return ForceSetAction(ae, actName, pMode, pTagList);
}

// ----------------------------------------------------
//强制指定当前行动 

COTEAction*  COTEActorActionMgr::ForceSetAction(COTEActorEntity* ae,
												const std::string& actName, 
												COTEAction::PlayMode pMode, 
												std::list<Ogre::MovableObject*>* pTagList)
{
	// 停止当前的行动

	HashMap<std::string, COTEAction*>::iterator it = ae->m_ActionList.find(actName);
	if(it != ae->m_ActionList.end())
	{		
		COTEAction* act = ae->m_CurAction;
		ae->m_CurAction = it->second;
		if(act)
		{
			act->Stop(false);					
		}				
	}	
	else
	{
		OTE_TRACE("当前行动没有找到! ACTOR: " << ae->GetName() << " ACTION:" << actName)
		return NULL;
	}

	// 绑定 一个以上的目标

	if(ae->m_CurAction && pTagList)
	{		
		std::list<Ogre::MovableObject*>::iterator i = pTagList->begin();
		while(i != pTagList->end())
		{
			ae->m_CurAction->BindEffectsToTarget(*i);
			i++;
		}
	}

	if(ae->m_CurAction)
	{		
		ae->m_CurAction->Play(pMode);
		//OTE_TRACE("指定当前行动 ACTOR: " << ae->GetName() << " ACTION:" << actName)
	}	

	return ae->m_CurAction;
}
#include "oteactoractionmgr.h"
#include "OTEActionXmlLoader.h"

using namespace Ogre;
using namespace OTE;

// --------------------------------------------------------
// ����ж�
COTEAction* COTEActorActionMgr::AddAction(COTEActorEntity* ae, const std::string& actName)
{
	HashMap<std::string, COTEAction*>::iterator it = ae->m_ActionList.find(actName);
	if(it != ae->m_ActionList.end())
	{
		OTE_MSG("����ж��Ѿ����� ACTOR: " << ae->GetName() << " ACTION:" << actName, "ʧ��")
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
		OTE_TRACE_WARNING("����ж���Դ������ ACTOR: " << ae->GetName() << " ACTION:" << actName)
	}

	ae->m_ActionList[act->m_Name] = act;

	OTE_TRACE("����ж� ACTOR: " << ae->GetName() << " ACTION:" << act->GetName())

	return act;
}

// -------------------------------------------------
//ɾ���ж�
void COTEActorActionMgr::RemoveAction(COTEActorEntity* ae, const std::string& actName)
{
	HashMap<std::string, COTEAction*>::iterator it = ae->m_ActionList.find(actName);
	if(it != ae->m_ActionList.end())
	{
		COTEAction* act = it->second;	

		if(ae->m_CurAction == act)
			ae->m_CurAction = NULL;

		OTE_TRACE("ɾ���ж� ACTOR: " << ae->GetName() << " ACTION:" << actName)
		delete act;
		ae->m_ActionList.erase(it);
	}		
}	

// -------------------------------------------------
//�õ�ָ�����ж�
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
//�����ж�����
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
			
		OTE_TRACE("�����ж����� ACTOR: " << ae->GetName() << " ACTION:" << rOldName << "->" << rNewName)
	}
}

// ----------------------------------------------------
//ָ����ǰ�ж�

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

	// �ж��Ѿ��ڲ���

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
//ǿ��ָ����ǰ�ж� 

COTEAction*  COTEActorActionMgr::ForceSetAction(COTEActorEntity* ae,
												const std::string& actName, 
												COTEAction::PlayMode pMode, 
												std::list<Ogre::MovableObject*>* pTagList)
{
	// ֹͣ��ǰ���ж�

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
		OTE_TRACE("��ǰ�ж�û���ҵ�! ACTOR: " << ae->GetName() << " ACTION:" << actName)
		return NULL;
	}

	// �� һ�����ϵ�Ŀ��

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
		//OTE_TRACE("ָ����ǰ�ж� ACTOR: " << ae->GetName() << " ACTION:" << actName)
	}	

	return ae->m_CurAction;
}
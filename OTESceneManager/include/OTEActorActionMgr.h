#pragma once
#include "OTEActorEntity.h"

namespace OTE
{
// ************************************
// COTEActorActionMgr
// ��ɫ�ж��������ʵ��
// ************************************

class _OTEExport COTEActorActionMgr
{
public:

	// ����ж�
	static COTEAction* AddAction(COTEActorEntity* ae, const std::string& actName);

	// ɾ���ж�
	static void RemoveAction(COTEActorEntity* ae, const std::string& actName);

	// �õ�ָ�����ж�
	static COTEAction* GetAction(COTEActorEntity* ae, const std::string& actName);

	// ָ����ǰ�ж�
	static COTEAction* SetAction(COTEActorEntity* ae, const std::string& actName, COTEAction::PlayMode pMode = COTEAction::eCONTINUES, std::list<Ogre::MovableObject *> *pTagList = NULL);
	
	// ǿ�в����ж�
	static COTEAction*  ForceSetAction(COTEActorEntity* ae, const std::string& actName, COTEAction::PlayMode pMode, std::list<Ogre::MovableObject *> *pTagList);
	
	// �����ж�����
	static void RenameAction(COTEActorEntity* ae, const std::string& rOldName, const std::string& rNewName);




};


}

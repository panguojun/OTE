#pragma once
#include "OTEActorEntity.h"

namespace OTE
{
// ************************************
// COTEActorActionMgr
// 角色行动功能相关实现
// ************************************

class _OTEExport COTEActorActionMgr
{
public:

	// 添加行动
	static COTEAction* AddAction(COTEActorEntity* ae, const std::string& actName);

	// 删除行动
	static void RemoveAction(COTEActorEntity* ae, const std::string& actName);

	// 得到指定的行动
	static COTEAction* GetAction(COTEActorEntity* ae, const std::string& actName);

	// 指定当前行动
	static COTEAction* SetAction(COTEActorEntity* ae, const std::string& actName, COTEAction::PlayMode pMode = COTEAction::eCONTINUES, std::list<Ogre::MovableObject *> *pTagList = NULL);
	
	// 强行播放行动
	static COTEAction*  ForceSetAction(COTEActorEntity* ae, const std::string& actName, COTEAction::PlayMode pMode, std::list<Ogre::MovableObject *> *pTagList);
	
	// 更改行动名称
	static void RenameAction(COTEActorEntity* ae, const std::string& rOldName, const std::string& rNewName);




};


}

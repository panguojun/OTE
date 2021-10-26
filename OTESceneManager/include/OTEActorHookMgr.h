#pragma once
#include "OTEActorEntity.h"

namespace OTE
{
// ************************************
// COTEActorHookMgr
// 挂点功能相关实现
// ************************************

class _OTEExport COTEActorHookMgr
{
public:
	
	//添加挂点
	
	static void AddHook(
		COTEActorEntity* ae, 
		const std::string& HookName, const std::string& BoneName, 
		Ogre::Vector3 OffestPos = Ogre::Vector3::ZERO,
		Ogre::Quaternion OffestQuat = Ogre::Quaternion::IDENTITY,
		Ogre::Vector3 Scale = Ogre::Vector3::UNIT_SCALE
		);

	//得到挂点
	
	static Hook_t* GetHook(COTEActorEntity* ae, const std::string& HookName);
	
	//删除挂点
	
	static void RemoveHook(COTEActorEntity* ae, const std::string& HookName);

	//绑定装备到挂点
	
	static bool AttachObjectToHook(COTEActorEntity* ae, const std::string& HookName, 
					    const std::string& rObjResFile, const std::string& rObjEntityName = "");
	
	static bool AttachObjectToHook(COTEActorEntity* ae, const std::string& HookName, Ogre::MovableObject* mo);
	
	//解除挂点上绑定的装备
	
	static bool DettachObjectFromHook(COTEActorEntity* ae, const std::string& HookName,	 Ogre::MovableObject* mo);
	
	static bool DettachAllObjectsFromHook(COTEActorEntity* ae, const std::string& HookName, bool deleteMag = false);

	//更新装备
	
	static void UpdateHook(COTEActorEntity* ae, const std::string& HookName);

};


}


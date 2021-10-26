#pragma once
#include "OTEActorEntity.h"

namespace OTE
{
// ************************************
// COTEActorHookMgr
// �ҵ㹦�����ʵ��
// ************************************

class _OTEExport COTEActorHookMgr
{
public:
	
	//��ӹҵ�
	
	static void AddHook(
		COTEActorEntity* ae, 
		const std::string& HookName, const std::string& BoneName, 
		Ogre::Vector3 OffestPos = Ogre::Vector3::ZERO,
		Ogre::Quaternion OffestQuat = Ogre::Quaternion::IDENTITY,
		Ogre::Vector3 Scale = Ogre::Vector3::UNIT_SCALE
		);

	//�õ��ҵ�
	
	static Hook_t* GetHook(COTEActorEntity* ae, const std::string& HookName);
	
	//ɾ���ҵ�
	
	static void RemoveHook(COTEActorEntity* ae, const std::string& HookName);

	//��װ�����ҵ�
	
	static bool AttachObjectToHook(COTEActorEntity* ae, const std::string& HookName, 
					    const std::string& rObjResFile, const std::string& rObjEntityName = "");
	
	static bool AttachObjectToHook(COTEActorEntity* ae, const std::string& HookName, Ogre::MovableObject* mo);
	
	//����ҵ��ϰ󶨵�װ��
	
	static bool DettachObjectFromHook(COTEActorEntity* ae, const std::string& HookName,	 Ogre::MovableObject* mo);
	
	static bool DettachAllObjectsFromHook(COTEActorEntity* ae, const std::string& HookName, bool deleteMag = false);

	//����װ��
	
	static void UpdateHook(COTEActorEntity* ae, const std::string& HookName);

};


}


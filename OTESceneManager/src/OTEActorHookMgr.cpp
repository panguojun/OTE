#include "OTEActorHookMgr.h"
#include "OTETrackManager.h"
#include "OTEMagicManager.h"

using namespace Ogre;
using namespace OTE;

// --------------------------------------------------------
//添加挂点

void COTEActorHookMgr::AddHook(COTEActorEntity* ae, 
							   const std::string& HookName, 
							   const std::string& BoneName, 
							   Ogre::Vector3 OffestPos, 
							   Ogre::Quaternion OffestQuat, 
							   Ogre::Vector3 Scale)
{
	HashMap<std::string, Hook_t*>::iterator i = ae->m_PotHookList.find(HookName);
	if(i == ae->m_PotHookList.end())
	{
		Hook_t* pHook = new Hook_t(HookName, BoneName);

		pHook->OffestPos		= OffestPos;
		pHook->OffestQuaternion = OffestQuat;
		pHook->Scale			= Scale;
		
		ae->m_PotHookList[HookName] = pHook;			
	}
	else
	{
        MessageBox(NULL, "已存在同名挂点", "错误", MB_OK);
	}
}
// -------------------------------------------------
//得到挂点
Hook_t* COTEActorHookMgr::GetHook(COTEActorEntity* ae, const std::string& HookName)
{
	HashMap<std::string, Hook_t*>::iterator i = ae->m_PotHookList.find(HookName);
	if(i != ae->m_PotHookList.end())
	{
		return ae->m_PotHookList[HookName];		
	}

	return NULL;
}

// -------------------------------------------------	
//删除挂点
void COTEActorHookMgr::RemoveHook(COTEActorEntity* ae, const std::string& HookName)
{
	HashMap<std::string, Hook_t*>::iterator i = ae->m_PotHookList.find(HookName);
	if(i != ae->m_PotHookList.end())
	{	
		SAFE_DELETE(i->second)
	
        ae->m_PotHookList.erase(i);			
	}
}

// -------------------------------------------------
//解除挂点上绑定的装备
bool COTEActorHookMgr::DettachObjectFromHook(COTEActorEntity* ae, 
											 const std::string& HookName, 
											 Ogre::MovableObject* mo)
{
	Hook_t *pHook = GetHook(ae, HookName);
	if(pHook)
	{
		std::list<Ogre::MovableObject*>::iterator it = pHook->HookEntityList.begin();
		while(it != pHook->HookEntityList.end())
		{		
			if((*it) == mo)
			{
				//MAGIC_MGR->DeleteMagic((*it)->getName());
				pHook->HookEntityList.erase(it);
				it = pHook->HookEntityList.begin();
			}
			else
			{
				++ it;
			}
		}		
	}
	return true;
}

// -------------------------------------------------
bool COTEActorHookMgr::DettachAllObjectsFromHook(COTEActorEntity* ae, const std::string& HookName, bool deleteMag)
{

	Hook_t* pHook = GetHook(ae, HookName);
	if(pHook)
	{
		std::list<Ogre::MovableObject*>::iterator it = pHook->HookEntityList.begin();
		while(it != pHook->HookEntityList.end())
		{		
			if(deleteMag)
				MAGIC_MGR->DeleteMagic((*it)->getName());
			pHook->HookEntityList.erase(it);	
			it = pHook->HookEntityList.begin();	
		}		
	}
	return true;
}

// -------------------------------------------------
//绑定装备到挂点
bool COTEActorHookMgr::AttachObjectToHook(COTEActorEntity* ae, const std::string& HookName, 
										  const std::string& rObjResFile, const std::string& rObjEntityName)
{
    WAIT_LOADING_RES(ae)
	Hook_t *pHook = GetHook(ae, HookName);
	if(pHook)
	{	
		if(!pHook->HookEntityList.empty())
		{
			// 暂时只能挂一个
			//OTE_MSG_ERR("挂点暂时只能挂一个")
			return false;
		}

		Ogre::MovableObject* mo;

		if(rObjEntityName.empty())
			mo = MAGIC_MGR->AutoCreateMagic(rObjResFile, false);
		else
			mo = MAGIC_MGR->CreateMagic(rObjResFile, rObjEntityName, false);

		SCENE_MGR->AttachObjToSceneNode(mo);

		return AttachObjectToHook(ae, HookName, mo);
	}

	OTE_MSG_ERR("没有找到这个挂点: " << HookName)
	return false;
}


// -------------------------------------------------
//绑定装备到挂点
bool COTEActorHookMgr::AttachObjectToHook(COTEActorEntity* ae, const std::string& HookName, Ogre::MovableObject* mo)
{

	Hook_t *pHook = GetHook(ae, HookName);
	if(pHook)
	{			
		pHook->HookEntityList.push_back(mo);

		// 使用轨道动画实现挂点
		
		// int id = OTEHelper::HashString(ae->GetName() + HookName); // 轨道id与角色跟挂点名称相关
		static int sid = 100000; // 为了避免冲突		 
		sid ++; // 每次都动态创建一个新轨道
			
		COTEKeyFrame* kf;

		if(!COTETrackManager::GetInstance()->IsTrackPresent(sid))
		{
			COTETrack* track = COTETrackManager::GetInstance()->CreateNewTrack(sid);
			if(!pHook->BoneName.empty())
				track->SetBindBone(pHook->BoneName);

			kf = COTEKeyFrame::CreateNewKeyFrame();			
			track->AddKeyFrame(kf);

			pHook->BindTrack = track;		// 邦定track
		}
		else
		{
			COTETrack* track = COTETrackManager::GetInstance()->GetTrack(sid, NULL);
			kf = track->GetFirstKey();	
			
		}	

		if(kf)
		{
			// setting

			kf->m_Position		= pHook->OffestPos;
			kf->m_Quaternion	= pHook->OffestQuaternion;
			kf->m_Scaling		= pHook->Scale;				

			OTE_TRACE("绑定装备到挂点  装备: " << mo->getName() << " 挂点: " << HookName)
		}

		return true;
	}
	
	OTE_MSG_ERR("没有找到所需要的挂点: " << HookName)

	return false;
}

// -------------------------------------------------
//挂点上的装备

void COTEActorHookMgr::UpdateHook(COTEActorEntity* ae, const std::string& HookName)
{

	Hook_t *pHook = GetHook(ae, HookName);
	if(pHook)
	{
		if(!pHook->BindTrack)
			return;

		COTEKeyFrame* kf = pHook->BindTrack->GetFirstKey();		
		if(!kf)
		{
			OTE_MSG_ERR("UpdateHook时对应轨道没有关键桢!")
			return;
		}
		
		// 更新桢位置

		kf->m_Position		= pHook->OffestPos;
		kf->m_Quaternion	= pHook->OffestQuaternion;
		kf->m_Scaling		= pHook->Scale;		

		Ogre::Vector3 pos, scl;
		Ogre::Quaternion q;
		std::list<Ogre::MovableObject*>::iterator it = pHook->HookEntityList.begin();
		while(it != pHook->HookEntityList.end())
		{
			Ogre::SceneNode* sn = (*it)->getParentSceneNode();	
			
			if(sn)
			{

				// 轨道动画

				COTEActTrackUsage::BindTrans(ae, pHook->BindTrack, kf, pos, q, scl);

				sn->setOrientation(q);
				sn->setPosition(pos);	
				sn->setScale(scl);
			}

			++ it;
		}	
	
	}
	else
	{
		//OTE_MSG_ERR("挂点不存在!")	
	}

}


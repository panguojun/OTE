#include "OTEActorHookMgr.h"
#include "OTETrackManager.h"
#include "OTEMagicManager.h"

using namespace Ogre;
using namespace OTE;

// --------------------------------------------------------
//��ӹҵ�

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
        MessageBox(NULL, "�Ѵ���ͬ���ҵ�", "����", MB_OK);
	}
}
// -------------------------------------------------
//�õ��ҵ�
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
//ɾ���ҵ�
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
//����ҵ��ϰ󶨵�װ��
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
//��װ�����ҵ�
bool COTEActorHookMgr::AttachObjectToHook(COTEActorEntity* ae, const std::string& HookName, 
										  const std::string& rObjResFile, const std::string& rObjEntityName)
{
    WAIT_LOADING_RES(ae)
	Hook_t *pHook = GetHook(ae, HookName);
	if(pHook)
	{	
		if(!pHook->HookEntityList.empty())
		{
			// ��ʱֻ�ܹ�һ��
			//OTE_MSG_ERR("�ҵ���ʱֻ�ܹ�һ��")
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

	OTE_MSG_ERR("û���ҵ�����ҵ�: " << HookName)
	return false;
}


// -------------------------------------------------
//��װ�����ҵ�
bool COTEActorHookMgr::AttachObjectToHook(COTEActorEntity* ae, const std::string& HookName, Ogre::MovableObject* mo)
{

	Hook_t *pHook = GetHook(ae, HookName);
	if(pHook)
	{			
		pHook->HookEntityList.push_back(mo);

		// ʹ�ù������ʵ�ֹҵ�
		
		// int id = OTEHelper::HashString(ae->GetName() + HookName); // ���id���ɫ���ҵ��������
		static int sid = 100000; // Ϊ�˱����ͻ		 
		sid ++; // ÿ�ζ���̬����һ���¹��
			
		COTEKeyFrame* kf;

		if(!COTETrackManager::GetInstance()->IsTrackPresent(sid))
		{
			COTETrack* track = COTETrackManager::GetInstance()->CreateNewTrack(sid);
			if(!pHook->BoneName.empty())
				track->SetBindBone(pHook->BoneName);

			kf = COTEKeyFrame::CreateNewKeyFrame();			
			track->AddKeyFrame(kf);

			pHook->BindTrack = track;		// �track
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

			OTE_TRACE("��װ�����ҵ�  װ��: " << mo->getName() << " �ҵ�: " << HookName)
		}

		return true;
	}
	
	OTE_MSG_ERR("û���ҵ�����Ҫ�Ĺҵ�: " << HookName)

	return false;
}

// -------------------------------------------------
//�ҵ��ϵ�װ��

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
			OTE_MSG_ERR("UpdateHookʱ��Ӧ���û�йؼ���!")
			return;
		}
		
		// ������λ��

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

				// �������

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
		//OTE_MSG_ERR("�ҵ㲻����!")	
	}

}


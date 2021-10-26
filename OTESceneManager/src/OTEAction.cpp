#include "OTEStaticInclude.h"
#include "OTEActorEntity.h"
#include "OTEAction.h"

#include "OTEMagicManager.h"
#include "OTETrackManager.h"
#include "OTETrackManager.h"

#include "OTEBillBoardSet.h"
#include "OTEParticlesystem.h"
#include "OTEActorEntity.h"

#include "OTEReactorManager.h"
#include "OTEActionReactorUsage.h"

#include "OTEFModsystem.h"

using namespace Ogre;
using namespace OTE;

// --------------------------------------------------------
static int g_MagIDCount = 0;

// ================================================= 
// COTEActionData
// =================================================

bool COTEActionData::GetIDFromName(const std::string rActionName, int& rID, std::string& rTail)
{
	int ActID = 0;
	char name[32] = {0};

	if(sscanf(rActionName.c_str(), "%d%s", &ActID, name) != 2)
	{
		OTE_MSG("�ж����Ʊ�����[NUMBER][STRING]��ʽ! rActionName = " << rActionName, "����" )
		return false;
	}

	rID = ActID;
	rTail = name;

	return true;
}

// ��¡

bool COTEActionData::CloneInterDatas(COTEActionData* srcAct, COTEActionData* desAct)
{
	OTE_ASSERT(srcAct && desAct && srcAct != desAct);	

	std::string actName = desAct->m_ResName; // ��Դ��

	int actID = 0;
	std::string tailName;
	if(!COTEActionData::GetIDFromName(actName, actID, tailName))
		return false;
	
	int cnt = 0;
	
	MagBindTrackData_t::iterator i = srcAct->m_MagicInterMapData.begin();
	while(i != srcAct->m_MagicInterMapData.end())
	{
		OTE_ASSERT(cnt < 100)
		int interID = actID * 100 + cnt ++; // clone �� id���� actID * 100 + ������� ���	

		ActMagicData_t* desm = ActMagicData_t::CreateActMagic();
		*desm = *i->second;
		desm->ID = g_MagIDCount ++;

		desAct->m_MagicInterMapData[desm->ID] = desm;

		++ i;
	}

	return true;
}

// --------------------------------------------------------
void COTEActionData::Clone(COTEActionData* des)
{
	des->m_ResName			=	m_ResName;	
	des->m_AniName			=	m_AniName;									
	des->m_AniSpeed			=	m_AniSpeed;									
	des->m_IsAutoCreate 	=	m_IsAutoCreate;									
	des->m_InureTime		=	m_InureTime;									
	des->m_Length			=	m_Length;								
	des->m_IsAutoPlay		=	m_IsAutoPlay;
	des->m_CurrentDelay		= 	m_CurrentDelay;
	des->m_3DSoundName  	=	m_3DSoundName;	
	des->m_3DSoundVolume	=	m_3DSoundVolume;
	des->m_Is3DSoundLoop	=	m_Is3DSoundLoop;

	CloneInterDatas(this, des);		

}

// --------------------------------------------------------
void COTEActionData::AddEffectData(ActMagicData_t* Mag, MagBindTrackData_t& magicDataMap)
{
	MagBindTrackData_t::iterator it = magicDataMap.find(Mag->ID);
	if(it != magicDataMap.end())
	{
		OTE_TRACE_ERR("�ظ���� ID : " << Mag->ID)		
		return;
	}
	
	magicDataMap[Mag->ID] = Mag;
}

// --------------------------------------------------------
void COTEActionData::AddEffectData(ActMagicData_t* Mag, int InterId)
{	
	Mag->ID = g_MagIDCount ++; // ��̬������ЧID
	Mag->pMovableObj = NULL;
	Mag->InterID = InterId;

	AddEffectData(Mag, m_MagicInterMapData);

	OTE_TRACE("���Effect, Mag :" << Mag->MagicFile << " Inter id = " << InterId)
}

// --------------------------------------------------------
void COTEActionData::RemoveEffectData(int magID, MagBindTrackData_t& magicDataMap)
{
	MagBindTrackData_t::iterator it = magicDataMap.find(magID);
	if(it != magicDataMap.end())
	{			
		ActMagicData_t::DeleteActMagic(it->second);
		magicDataMap.erase(it);
	}
}

// --------------------------------------------------------
void COTEActionData::RemoveEffectData(int magID)
{
	RemoveEffectData(magID, m_MagicInterMapData);

	OTE_TRACE("�Ƴ�Effect, Mag :" << magID)
}

// ================================================= 
// COTEAction
// =================================================

//���캯��
COTEAction::COTEAction(COTEActorEntity* creator) : COTEActionData()
{		
   	m_Creator = creator;
	m_Time = 0.0f;
	m_3DSoundPtr = 0;
	m_TrackCtrller = NULL;
}

// --------------------------------------------------------
//��������
COTEAction::~COTEAction()
{	
	ClearEffects();

	// �����Դ�󶨱�

	MagBindTrackData_t::iterator i = m_MagicInterMapData.begin();
	while(i != m_MagicInterMapData.end())
	{
		ActMagicData_t* m = i->second;
        m_MagicInterMapData.erase(i);
		i = m_MagicInterMapData.begin();
		ActMagicData_t::DeleteActMagic(m);
	}
	m_MagicInterMapData.clear();

	// ���������

	if(m_TrackCtrller)
	{
		COTEReactorManager::GetSingleton()->RemoveReactor(m_TrackCtrller);
		m_TrackCtrller = NULL;
	}
}

// --------------------------------------------------------
// ����
COTEAction* COTEAction::CreateAction(COTEActorEntity* ae)
{
	return new COTEAction(ae);
}


// --------------------------------------------------------
// Ч����һЩ���Թ���
// --------------------------------------------------------

//�õ�һ��Ч�����ٶ�
float COTEAction::GetPlaySpeed(int magID)
{
	MagBindTrackData_t::iterator it = m_MagicInterMapData.find(magID);
	if(it != m_MagicInterMapData.end())
	{		
		return it->second->PlaySpeed;	
	}

	return 1.0f;
}

// --------------------------------------------------------
//�趨һ��Ч�����ٶ�
void COTEAction::SetPlaySpeed(int magID, float speed)
{
	MagBindTrackData_t::iterator it = m_MagicInterMapData.find(magID);
	if(it != m_MagicInterMapData.end())
	{		
		it->second->PlaySpeed = speed;	
	}	
}	

// --------------------------------------------------------
//�õ�һ��Ч��������ʱ��
float COTEAction::GetPlayTime(int magID)
{
	MagBindTrackData_t::iterator it = m_MagicInterMapData.find(magID);
	if(it != m_MagicInterMapData.end())
	{		
		return it->second->PlayTime;	
	}

	return 0.0f;	
}

// --------------------------------------------------------
//�趨һ��Ч��������ʱ��
void COTEAction::SetPlayTime(int magID, float time)
{
	MagBindTrackData_t::iterator it = m_MagicInterMapData.find(magID);
	if(it != m_MagicInterMapData.end())
	{		
		it->second->PlayTime = time;	
	}	
}

// --------------------------------------------------------
bool COTEAction::SetAnimation(const std::string& aniName)
{
	OTE_ASSERT(m_Creator);

	m_AniName = aniName;

	if(!m_Creator->IsAnimationStatePresent(m_AniName))
	{
		OTE_TRACE_WARNING("�������ɫ: " << m_Creator->GetName() << " �������ж������Ķ���: " << m_AniName)
		return false;
	}
	else
	{
		m_Creator->SetCurrentAnim(m_AniName.c_str());
	}
	return true;
}

// --------------------------------------------------------
//ֹͣ��Ч����

void COTEAction::ClearEffects()
{	
	// �����

	UnbindReactors();

	// �����̬�����

	//UnbindEffectsFromTarget();

	// ��Magicȫ��ɾ��

	MagBindInterList_t::iterator it = m_MagicInterMap.begin();
	while(it != m_MagicInterMap.end())
	{
		Ogre::MovableObject* mo = it->first;
		m_MagicInterMap.erase(it);
		it = m_MagicInterMap.begin();
	
		if(mo != m_Creator)
			COTEMagicManager::GetSingleton()->DeleteMagic(mo->getName().c_str());	
	}
	m_MagicInterMap.clear();
	
	// ����󶨱�־

	MagBindTrackData_t::iterator i = m_MagicInterMapData.begin();
	while(i != m_MagicInterMapData.end())
	{
		ActMagicData_t *m = i->second;		
		m->pMovableObj = NULL;
		i++;
	}
	
	if(m_Creator && m_Creator->m_pTrackOrgSceneNode)
	{
		m_Creator->getParentSceneNode()->setPosition(m_Creator->m_pTrackOrgSceneNode->getWorldPosition());
		m_Creator->getParentSceneNode()->setScale(m_Creator->m_pTrackOrgSceneNode->getScale());
		m_Creator->getParentSceneNode()->setOrientation(m_Creator->m_pTrackOrgSceneNode->getOrientation());
		
		SCENE_MGR->getRootSceneNode()->removeAndDestroyChild(m_Creator->m_pTrackOrgSceneNode->getName());
		m_Creator->m_pTrackOrgSceneNode = NULL;
	}
}

// --------------------------------------------------------	
//��ͣ�ж�

void COTEAction::Pause()
{
	OTE_ASSERT(m_Creator);

	m_Mode = ePAUSE;

	//��ͣ����
	if(!m_AniName.empty())
	{
		if(m_Creator->m_pCurAnimState)
		{
			m_Creator->EnalbeAnim(true);
			m_Creator->SetAnimSpeed(0.0f);
		}
	}
}


// --------------------------------------------------------
//���һ��Magic��Track ��ʵ��İ
void COTEAction::AddMagicTrack(Ogre::MovableObject* Mag, COTETrack* Track)
{
	OTE_ASSERT(m_Creator);

	if(!Mag) // � action�İ�������ж����
	{
		Mag = m_Creator;	
		if(!m_Creator->m_pTrackOrgSceneNode)
		{
			m_Creator->m_pTrackOrgSceneNode = (Ogre::SceneNode*)SCENE_MGR->getRootSceneNode()->createChild();

			m_Creator->m_pTrackOrgSceneNode->setPosition(m_Creator->getParentSceneNode()->getWorldPosition());
			m_Creator->m_pTrackOrgSceneNode->setScale(m_Creator->getParentSceneNode()->getScale());
			m_Creator->m_pTrackOrgSceneNode->setOrientation(m_Creator->getParentSceneNode()->getOrientation());
		}
	}	

	MagBindInterList_t::iterator it = m_MagicInterMap.find(Mag);
	if(it != m_MagicInterMap.end())
	{
		::MessageBox(NULL, "�ظ���ͬһ��Ч��", "Error", MB_OK);
		return;
	}

	m_MagicInterMap[Mag] = Track;

	// �

	SceneNode* sn = Mag->getParentSceneNode();

	/*if(Track->m_AttBoneName.empty() || !m_Creator->hasSkeleton())
	{		
		if(sn)
		{
			sn->setPosition(m_Creator->getParentSceneNode()->getPosition());
		}
		else
		{
			if(Mag->getMovableType() == "Camera")
				((Ogre::Camera*)Mag)->setPosition(m_Creator->getParentSceneNode()->getPosition());
		}
	}
	else
	{		
		Ogre::Node *nd = m_Creator->getSkeleton()->getBone(Track->m_AttBoneName);
		if(sn)
			sn->setPosition(nd->getPosition());
		else
		{
			if(Mag->getMovableType() == "Camera")
				((Ogre::Camera*)Mag)->setPosition(nd->getPosition());

		}
	}*/

	COTEActTrackUsage::UpdateTrackBinding(this, Track, Mag, 0.0f);

	if(Mag->GetBindReactor())
	{
		COTEActionReactorUsage::Bind(Mag, (CReactorBase*)Mag->GetBindReactor());	
	}

}

// --------------------------------------------------------
//�õ�mag
Ogre::MovableObject* COTEAction::GetMagByInterID(int InterId)
{
	MagBindInterList_t::iterator it = m_MagicInterMap.begin();
	while(it != m_MagicInterMap.end())
	{			
		if(it->second->m_nID == InterId)
			return it->first;
		++ it;
	}
	return NULL;
}
// --------------------------------------------------------
//�õ�InterID
int COTEAction::GetInterIDByMag(Ogre::MovableObject* mo)
{
	MagBindInterList_t::iterator it = m_MagicInterMap.find(mo);
	if(it != m_MagicInterMap.end())
	{			
		return it->second->m_nID;		
	}
	return -1;
}

// --------------------------------------------------------
//������������ָ��ʱ���
void COTEAction::MoveFrameTo(float time)
{
	if(time > m_Length)
		return;

	FlushRes();

	m_Mode = ePAUSE;
	m_Time = time;

	//��ͣ����

	if(!m_AniName.empty())
	{
		OTE_ASSERT(m_Creator);

		if(!m_Creator->IsAnimationStatePresent(m_AniName))
		{
			OTE_TRACE_WARNING("�������ɫ" << m_Creator->GetName() << " �������ж������Ķ��� " << m_AniName)
		}
		else
		{
			m_Creator->SetCurrentAnim(m_AniName.c_str());
			if(m_Creator->m_pCurAnimState)
			{
				m_Creator->EnalbeAnim(true);
				m_Creator->m_pCurAnimState->setTimePosition(time);
				m_Creator->SetAnimSpeed(0.0f);
			}
		}
	}

	UpdateTrackMagicPos();
}

// --------------------------------------------------------
//�����ж�
void COTEAction::Play(PlayMode mode)
{
	OTE_ASSERT(m_Creator);

	m_PlayStepFlag = 0;

	m_CurrentDelay = 0;

	m_Mode = mode;

	// ���������

	if(m_TrackCtrller)	
		m_TrackCtrller->SetLength(m_Length);	

	// ���Ŷ���

	if(!m_AniName.empty())
	{
		if(!m_Creator->IsAnimationStatePresent(m_AniName))
		{
			OTE_TRACE_WARNING("�������ɫ" << m_Creator->GetName() << " �������ж������Ķ��� " << m_AniName)
		}
		else
		{
			m_Creator->SetCurrentAnim(m_AniName.c_str());
			if(m_Creator->m_pCurAnimState)
			{
				m_Creator->m_pCurAnimState->setLoop(m_Mode == eCONTINUES);
				m_Creator->SetAnimSpeed(m_AniSpeed);
				m_Creator->EnalbeAnim(true);
			}
		}
	}
	
	// ��������

	if(!m_3DSoundName.empty() && m_3DSoundPtr == 0)
	{
		FMOD::Sound* sd = COTEFMODSystem::s_Inst.Create3DSound(m_3DSoundName, this->m_Is3DSoundLoop);
		m_3DSoundPtr = (void*)sd; // ��ָ�뵱���	
						
		Ogre::Vector3 actorPos = m_Creator->getParentSceneNode()->getPosition();
		FMOD_VECTOR pos = {actorPos.x, actorPos.y, actorPos.z};
		FMOD_VECTOR vel = {0, 0, 0};

		COTEFMODSystem::s_Inst.Play3DSoundAt(sd, pos, vel, m_3DSoundVolume);
	}

	// �¼�

	m_Creator->OnActionBegin();
}
// --------------------------------------------------------
//ֹͣ�ж�
void COTEAction::Stop(bool isInvorkCB)
{
	m_Time = 0.0f;
	m_Mode = eSTOP;

	if(m_TrackCtrller)
		m_TrackCtrller->Clear();

	//ֹͣ����
	if(!m_AniName.empty())
	{
		OTE_ASSERT(m_Creator);
		if(m_Creator->m_pCurAnimState)
		{
			m_Creator->EnalbeAnim(false);
			m_Creator->m_pCurAnimState->setTimePosition(0.0f);
		}
	}

	ClearEffects();

	if(!m_3DSoundName.empty() && m_3DSoundPtr != 0)
	{
		COTEFMODSystem::s_Inst.Stop3DSound((FMOD::Sound*)m_3DSoundPtr);
		
		m_3DSoundPtr = 0;
	}

	//// �¼�

	//if(isInvorkCB)
	//{
	//	OTE_ASSERT(m_Creator);
	//	m_Creator->OnActionEnd();
	//}
}

// ------------------------------------------------------
void COTEAction::FlushRes(bool forceLoad)
{
	OTE_ASSERT(m_Creator);

	//������Ч�����б�	

	MagBindTrackData_t::iterator i = m_MagicInterMapData.begin();
	while(i != m_MagicInterMapData.end())
	{
		ActMagicData_t*	m = i->second;
		if(!m->pMovableObj)
		{		
			if(m_Time >= m->PlayTime || forceLoad) // �����ͺ󲥷ŵ����
			{
				// ����б�			

				if(m->InterType == ActMagicData_t::eTrack)
				{
					COTETrack*	tk = COTETrackManager::GetInstance()->GetTrack(m->InterID, COTEActTrackUsage::GetTrackXmlLoader());
					if(tk)
					{
						if(tk->GetFrameNum() > 0)
						{						
							if(strncmp(m->MagicFile, "__self_", strlen("__self_")) == 0) // ����
							{
								m->pMovableObj = this->m_Creator;
								AddMagicTrack(NULL, tk);
							}
							else
							{
								m->pMovableObj = COTEMagicManager::GetSingleton()->AutoCreateMagic(m->MagicFile);
								OTE_ASSERT(m->pMovableObj);
								SCENE_MGR->AttachObjToSceneNode(m->pMovableObj);
								
								AddMagicTrack(m->pMovableObj, tk);				}														
						}
					}
					else
					{	
						OTE_MSG_ERR("�����Դ������ COTEAction::FlushRes, TrackID = " << m->InterID)
					}
				}

				// ��Ӧ��

				else if(m->InterType == ActMagicData_t::eReactor)
				{		
					if(strncmp(m->MagicFile, "__self_", strlen("__self_")) == 0) // ����
					{
						m->pMovableObj = m_Creator;										
					}
					else
					{
						// ����magic
						
						m->pMovableObj = COTEMagicManager::GetSingleton()->AutoCreateMagic(m->MagicFile);	
						SCENE_MGR->AttachObjToSceneNode(m->pMovableObj);
						m->pMovableObj->getParentSceneNode()->setPosition(m_Creator->getParentSceneNode()->getPosition());

					}
					assert(m->pMovableObj);

					CReactorBase* rb = COTEReactorManager::GetSingleton()->GetReactor(m->InterID);
					if(!rb)					
						rb = COTEReactorManager::GetSingleton()->CreateReactor(m->InterData, m->InterID, m->pMovableObj);				

					if(rb)
					{								
						if(m_MagicInterMap.find(m->pMovableObj) == m_MagicInterMap.end())
						{
							m_MagicInterMap[m->pMovableObj] = rb;
						}
						else
						{
							OTE_MSG_ERR("�����ظ���, InterID = " << m->InterID)						
						}						
					}

				}
			}			
		}
		i++;
	}

	
	// ������

	BindReactors();

}

// ------------------------------------------------------
//������Ч������λ��

void COTEAction::UpdateTrackMagicPos()
{
	assert(m_Creator);
	
	if(!m_Creator->getParentSceneNode())
		return;		

	// ����Ч�ƶ�������󶨹����Ӧ�Ĺؼ�֡��λ����

	MagBindInterList_t::iterator it = m_MagicInterMap.begin();
	while(it != m_MagicInterMap.end())
	{
		Ogre::MovableObject* mo = it->first;

		if(it->second->GetInterType() == "Track")
		{
			COTETrack*	tk = (COTETrack*)it->second;

			assert(tk->m_nID != 0);

			// ����
			// TODO һЩ������ظ���

			COTEActTrackUsage::UpdateTrackBinding(this, tk, mo, m_Time);

		}
		
		it++;
	}

}

// --------------------------------------------------------
//֡����
// --------------------------------------------------------
COTEAction::PlayMode COTEAction::Proccess(float time)
{ 	
	assert(m_Creator);

	if(m_Mode == eCONTINUES || m_Mode == ePLAY_ONCE)
	{	
		if(m_CurrentDelay > 0)				
			m_CurrentDelay -= time;					

		if(m_CurrentDelay <= 0)
			m_Time += time;

		if(m_Time >= m_Length)
		{
			m_Time = 0.0f;

			if(m_Mode != eCONTINUES)
			{
				Stop();
				return m_Mode;
			}
		}

		// ��Դ����

		FlushRes();	

		// ��̬�������

		if(m_TrackCtrller)
			m_TrackCtrller->Update(NULL, time);		

		// ���¹��

		UpdateTrackMagicPos();		

		// ���·�Ӧ��

		UpdateReactors(time);
		

		// ��������λ��

		if(m_3DSoundPtr)
		{
			Ogre::Vector3 actorPos = m_Creator->getParentSceneNode()->getPosition();
			FMOD_VECTOR pos = {actorPos.x, actorPos.y, actorPos.z};
			FMOD_VECTOR vel = {0, 0, 0};	

			COTEFMODSystem::s_Inst.Set3DSoundPos((FMOD::Sound*)m_3DSoundPtr, pos, vel);
		}

		// ������Ч���㴥������

		if(m_Time >= m_InureTime && !(m_PlayStepFlag & 0xF0000000))
		{
			m_Creator->OnActionInure(); 			
			m_PlayStepFlag |= 0xF0000000;
		}
	}
	
	return m_Mode;
}

// ------------------------------------------
// ���Ӧ��
void COTEAction::BindReactors()
{
	MagBindInterList_t::iterator it = m_MagicInterMap.begin();
	while(it != m_MagicInterMap.end())
	{
		if(!it->second->m_IsBind && it->second->GetInterType() == "Reactor")
		{			
			COTEActionReactorUsage::Bind(it->first, (CReactorBase*)it->second);	
			it->second->m_IsBind = true;
		}

		++ it;
	}
}

// ------------------------------------------
// ��Ӧ������

void COTEAction::UnbindReactors()
{
	MagBindInterList_t::iterator it = m_MagicInterMap.begin();
	while(it != m_MagicInterMap.end())
	{
		if(it->second->m_IsBind && it->second->GetInterType() == "Reactor")
		{			
			COTEActionReactorUsage::Unbind(it->first, (CReactorBase*)it->second);
			it->second->m_IsBind = false;			
		}
		++ it;
	}
}

// ------------------------------------------
// ���·�Ӧ��
void COTEAction::UpdateReactors(float dTime)
{
	MagBindInterList_t::iterator it = m_MagicInterMap.begin();
	while(it != m_MagicInterMap.end())
	{		
		if(it->second->GetInterType() == "Reactor")
		{
			COTEActionReactorUsage::Proccess(it->first, (CReactorBase*)it->second, dTime);	
		
		}
		
		++ it;
	}
}


// --------------------------------------------------------
// ��̬���
// --------------------------------------------------------

COTETrackCtrller* COTEAction::GetTrackCtrllerInstance()
{
	if(!m_TrackCtrller)
	{		
		m_TrackCtrller = (COTETrackCtrller*)COTEReactorManager::GetSingleton()->CreateReactor("TC.Target", -1);
		assert(m_TrackCtrller && m_Creator);
		m_TrackCtrller->m_Creator = (COTEEntity*)m_Creator;	
	}
	return m_TrackCtrller;
}	

// --------------------------------------------------------
void COTEAction::BindEffectToTarget(int MagID, Ogre::MovableObject* Target)
{		
	FlushRes();

	MagBindTrackData_t::iterator it = m_MagicInterMapData.find(MagID);
	if(it != m_MagicInterMapData.end())
	{		
		ActMagicData_t* md = it->second;
		
		md->pMovableObj->m_UserFlags |= MOVABLE_UD_BINDTOTARGET;			
		GetTrackCtrllerInstance()->AddMagicTarget(md->pMovableObj, Target);
		GetTrackCtrllerInstance()->Init(NULL); // ��ʱ����
	}
}


// --------------------------------------------------------
void COTEAction::BindEffectsToTarget(Ogre::MovableObject* Target)
{	
	FlushRes();

	MagBindInterList_t::iterator it = m_MagicInterMap.begin();
	while(it != m_MagicInterMap.end())
	{			
		it->first->m_UserFlags |= MOVABLE_UD_BINDTOTARGET;

		// �ָ�һ�¶���λ��
		if(it->second->GetInterType() == "Track")
			it->first->getParentSceneNode()->setPosition(this->m_Creator->GetParentSceneNode()->getPosition());
			//COTEActTrackUsage::UpdateTrackBinding(this, (COTETrack*)it->second, it->first, 0.0f);				
	
		GetTrackCtrllerInstance()->AddMagicTarget(it->first, Target); 
		++ it;
	}
	GetTrackCtrllerInstance()->Init(NULL); // ��ʱ����
}


// --------------------------------------------------------
void COTEAction::UnbindEffectFromTarget(int InterID)
{		
//	FlushRes();

	MagBindInterList_t::iterator it = m_MagicInterMap.begin();
	while(it != m_MagicInterMap.end())
	{			
		if(it->second->m_nID == InterID)
		{
			it->first->m_UserFlags &= ~MOVABLE_UD_BINDTOTARGET;

			break;
		}
		++ it;
	}	
}


// --------------------------------------------------------
void COTEAction::UnbindEffectsFromTarget()
{	
//	FlushRes();

	MagBindInterList_t::iterator it = m_MagicInterMap.begin();
	while(it != m_MagicInterMap.end())
	{			
		it->first->m_UserFlags &= ~MOVABLE_UD_BINDTOTARGET;
		++ it;
	}	
}
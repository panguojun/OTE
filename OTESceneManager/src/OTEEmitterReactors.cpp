#include "oteemitterreactors.h"
#include "OTEQTSceneManager.h"
#include "OTEMagicManager.h"
#include "OTEBillBoardSet.h"
#include "OTEReactorManager.h"

// -------------------------------------------------

using namespace OTE;
using namespace Ogre;

// =================================================
// COTEEmiterAffectorReactorBase
// =================================================

// 触发

void COTEEmiterAffectorReactorBase::Init(Ogre::MovableObject* mo)
{	
	COTEParticleSystemRes* psr = COTEParticleSystemManager::GetInstance()->GetParticleSystemRes(m_ParticleResName);	
	if(!psr)
	{
		COTEParticleSystemXmlSerializer xmlSerializer;
		xmlSerializer.Read(m_ParticleResName, m_ParticleResName);
		psr = xmlSerializer.GetParticleSystemRes();		
	}

	m_EmitterAffectorData = new EmitterAffectorData_t(
		(COTESimpleEmitter*)psr->m_Emitter, 
		psr->m_ParticleSystemAffector);	

	float sideDens = sqrt(float(psr->m_PoolSize) / 6);
	
	int cnt = 0;	

	for(float x = 0; x <= sideDens;  x += sideDens)	
	for(float y = 0; y <  int(sideDens);  y += 1.0f)		
	for(float z = 0; z <  int(sideDens);  z += 1.0f)	
	{	
		if(++ cnt > psr->m_PoolSize)
			break;

		CreateElement(
			mo,		 
			Ogre::Vector3(x - sideDens / 2.0f, y - sideDens / 2.0f, z - sideDens / 2.0f));
	}

	for(float y = 0; y <= sideDens; y += sideDens)	
	for(float x = 0; x < int(sideDens);  x += 1.0f)		
	for(float z = 0; z < int(sideDens);  z += 1.0f)	
	{
		if(++ cnt > psr->m_PoolSize)
			break;

		CreateElement(
			mo,
			Ogre::Vector3(x - sideDens / 2.0f, y - sideDens / 2.0f, z - sideDens / 2.0f));
	}

	for(float z = 0; z <= sideDens; z += sideDens)
	for(float x = 0; x < int(sideDens);  x += 1.0f)
	for(float y = 0; y < int(sideDens);  y += 1.0f)				
	{
		if(++ cnt > psr->m_PoolSize)
			break;

		CreateElement(
			mo,		
			Ogre::Vector3(x - sideDens / 2.0f, y - sideDens / 2.0f, z - sideDens / 2.0f));
	}

}

// -------------------------------------------------
// 更新

void COTEEmiterAffectorReactorBase::Update(Ogre::MovableObject* mo, float dTime)
{
	std::vector<ParticleElement_t>::iterator it = m_ElmList.begin();
	while(it != m_ElmList.end())
	{
		ParticleElement_t& elm = *it;
		if(!elm.MO->isVisible() || elm.TimeEsp >= m_EmitterAffectorData->time_to_live)
		{
			elm.MO->setVisible(true);
			elm.TimeEsp -= int(elm.TimeEsp / m_EmitterAffectorData->time_to_live) * m_EmitterAffectorData->time_to_live;
			elm.MO->getParentSceneNode()->setPosition(mo->getParentSceneNode()->getWorldPosition() + elm.OffsetPos);
			
			InitElement(mo, elm);
		}
		else
		{
			elm.MO->getParentSceneNode()->setPosition(
							elm.MO->getParentSceneNode()->getWorldPosition() +
							(elm.Velocity +	m_EmitterAffectorData->direction * m_EmitterAffectorData->strength * elm.TimeEsp) * dTime);
			elm.TimeEsp += dTime;

			UpdateElement(mo, elm);
		}

		++ it;
	}	

	mo->setVisible(false);	
}

// -------------------------------------------------
void COTEEmiterAffectorReactorBase::Clear()
{
	std::vector<ParticleElement_t>::iterator it = m_ElmList.begin();
	while(it != m_ElmList.end())
	{
		MAGIC_MGR->DeleteMagic((*it).MO->getName());

		++ it;
	}
	m_ElmList.clear();

	SAFE_DELETE(m_EmitterAffectorData)

}

// =================================================
// COTEBBSEmiterAffector
// =================================================

void COTEBBSEmiterAffector::CreateElement(Ogre::MovableObject* mo, const Ogre::Vector3& rPos)
{
	m_ElmList.resize(m_ElmList.size() + 1);
	ParticleElement_t& elm = m_ElmList[m_ElmList.size() - 1];

	COTEBillBoardSet* bbs = (COTEBillBoardSet*)MAGIC_MGR->AutoCreateMagic(".BBSCom", false);
	bbs->setMaterialName(bbs->getMaterialName());
	SCENE_MGR->AttachObjToSceneNode(bbs);
	
	// 反应器
	
	if(mo->GetBindReactor() && mo->GetBindReactor() != this)
	{
		CReactorBase* rb = COTEReactorManager::GetSingleton()->CreateReactor(((CReactorBase*)mo->GetBindReactor())->m_ResName, -1, bbs);
		bbs->SetBindReactor(rb);
		rb->Init(bbs);
	}

	elm.MO = bbs;
	elm.OffsetPos = rPos;
	elm.MO->setVisible(false);
	
	elm.TimeEsp = Ogre::Math::RangeRandom(0, m_EmitterAffectorData->time_to_live);
	elm.Velocity = Ogre::Vector3(
		Ogre::Math::RangeRandom(m_EmitterAffectorData->direction_min.x * m_EmitterAffectorData->velocity_min, m_EmitterAffectorData->direction_max.x * m_EmitterAffectorData->velocity_max),
		Ogre::Math::RangeRandom(m_EmitterAffectorData->direction_min.y * m_EmitterAffectorData->velocity_min, m_EmitterAffectorData->direction_max.y * m_EmitterAffectorData->velocity_max),
		Ogre::Math::RangeRandom(m_EmitterAffectorData->direction_min.z * m_EmitterAffectorData->velocity_min, m_EmitterAffectorData->direction_max.z * m_EmitterAffectorData->velocity_max)
		);   // 方向 * 速度

	elm.Size = 	Ogre::Math::RangeRandom(m_EmitterAffectorData->size_min, m_EmitterAffectorData->size_max);		

}

// -------------------------------------------------
void COTEBBSEmiterAffector::InitElement(Ogre::MovableObject* mo, ParticleElement_t& pe)
{	
	COTEBillBoardSet* bbs = (COTEBillBoardSet*)pe.MO;
	COTEBillBoardSet* bbs1 = (COTEBillBoardSet*)mo;
	bbs->setDefaultDimensions(pe.Size, pe.Size);
}

// -------------------------------------------------
void COTEBBSEmiterAffector::UpdateElement(Ogre::MovableObject* mo, ParticleElement_t& pe)
{
	COTEBillBoardSet* bbs = (COTEBillBoardSet*)pe.MO;
	COTEBillBoardSet* bbs1 = (COTEBillBoardSet*)mo;	

	float size = m_EmitterAffectorData->colorData.a * pe.Size;
	bbs->setDefaultDimensions(size, size);
}


// =================================================
// COTEEntityEmiterAffector
// =================================================

void COTEEntityEmiterAffector::CreateElement(Ogre::MovableObject* mo, const Ogre::Vector3& rPos)
{
	m_ElmList.resize(m_ElmList.size() + 1);
	ParticleElement_t& elm = m_ElmList[m_ElmList.size() - 1];

	COTEActorEntity* ae = (COTEActorEntity*)MAGIC_MGR->AutoCreateMagic(((COTEActorEntity*)mo)->m_pObjData->MeshFile, false);
	SCENE_MGR->AttachObjToSceneNode(ae);

	// 反应器

	if(mo->GetBindReactor())
	{
		CReactorBase* rb = COTEReactorManager::GetSingleton()->CreateReactor(((CReactorBase*)mo->GetBindReactor())->m_FactoryName, -1, ae);
		ae->SetBindReactor(rb);
		rb->Init(ae);
	}

	elm.MO = ae;
	elm.OffsetPos = rPos;
	elm.MO->setVisible(false);
	
	elm.TimeEsp = Ogre::Math::RangeRandom(0, m_EmitterAffectorData->time_to_live);
	elm.Velocity = Ogre::Vector3(
		Ogre::Math::RangeRandom(m_EmitterAffectorData->direction_min.x * m_EmitterAffectorData->velocity_min, m_EmitterAffectorData->direction_max.x * m_EmitterAffectorData->velocity_max),
		Ogre::Math::RangeRandom(m_EmitterAffectorData->direction_min.y * m_EmitterAffectorData->velocity_min, m_EmitterAffectorData->direction_max.y * m_EmitterAffectorData->velocity_max),
		Ogre::Math::RangeRandom(m_EmitterAffectorData->direction_min.z * m_EmitterAffectorData->velocity_min, m_EmitterAffectorData->direction_max.z * m_EmitterAffectorData->velocity_max)
		);   // 方向 * 速度

	elm.Size = 	Ogre::Math::RangeRandom(m_EmitterAffectorData->size_min, m_EmitterAffectorData->size_max);		

}

// -------------------------------------------------
void COTEEntityEmiterAffector::InitElement(Ogre::MovableObject* mo, ParticleElement_t& pe)
{	

}

// -------------------------------------------------
void COTEEntityEmiterAffector::UpdateElement(Ogre::MovableObject* mo, ParticleElement_t& pe)
{
	float size = m_EmitterAffectorData->colorData.a * pe.Size;
	mo->getParentSceneNode()->setScale(size, size, size);
}
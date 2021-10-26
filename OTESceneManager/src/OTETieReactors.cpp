#include "OTETieReactors.h"
#include "OTEParticlesystem.h"
#include "OTEQTSceneManager.h"
#include "OTEActorEquipmentMgr.h"
#include "OTEMagicManager.h"
#include "OTEBillBoardSet.h"
#include "OTEBillBoardChain.h"

// -------------------------------------------------

using namespace OTE;
using namespace Ogre;

// =================================================
// COTEParticleTieReactor
// =================================================

// ����

void COTEParticleTieReactor::Init(Ogre::MovableObject* mo)
{
	m_EspTime = 0;
	m_LastCreatePos = Ogre::Vector3(-1.0f, -1.0f, -1.0f); // ��Чλ��	
	m_LastObj = mo;
	
	if(mo)
		m_MaterialName = ((COTEParticleSystem*)mo)->getMaterialName();
}

// -------------------------------------------------
void COTEParticleTieReactor::ClearContent()
{
	std::list<TieDummyElm_t>::iterator it = m_DummyGraveList.begin();
	while(it != m_DummyGraveList.end())
	{
		MAGIC_MGR->DeleteMagic((*it).ElmName);
		++ it;
	}
	it = m_DummyList.begin();
	while(it != m_DummyList.end())
	{
		MAGIC_MGR->DeleteMagic((*it).ElmName);
		++ it;
	}

	m_DummyGraveList.clear();
	m_DummyList.clear();
}

// -------------------------------------------------
// ����

void COTEParticleTieReactor::Update(Ogre::MovableObject* mo, float dTime)
{
	TIMECOST_START	
	
	if(m_LastObj != mo)
	{
		ClearContent();
		m_LastObj = mo;
	}

	const float c_RemainTime = 0.5f;
	const float c_CreateStep = 2.0f * mo->getBoundingRadius();

	COTEParticleSystem* ps = (COTEParticleSystem*)mo;

	// ����ʱ������
	
	std::list<TieDummyElm_t>::iterator it = m_DummyList.begin();
	while(it != m_DummyList.end())
	{
		float deltaTime = m_EspTime - (*it).CreateTime;
		if(deltaTime > c_RemainTime)
		{
			m_DummyGraveList.push_back(*it);
			((COTEParticleSystem*)MAGIC_MGR->GetMagic((*it).ElmName))->SetVisible(false);
			
			m_DummyList.erase(it);
			it = m_DummyList.begin();
			continue;
		}
		else
		{

		}

		++ it;
	}
	

	// create dummy

	const Ogre::Vector3& pos = mo->getParentSceneNode()->getPosition();
	if( (m_LastCreatePos - pos).length() > c_CreateStep)
	{
		if(m_LastCreatePos != Ogre::Vector3( -1.0f, -1.0f, -1.0f ))
		{
			Ogre::MovableObject* nmo;
			if(m_DummyGraveList.empty())
			{
				nmo = MAGIC_MGR->AutoCreateMagic( ps->GetResPtr()->m_Name, false );
				SCENE_MGR->AttachObjToSceneNode(nmo);

				if(!m_MaterialName.empty())
					((COTEParticleSystem*)nmo)->setMaterialName(m_MaterialName);

				m_DummyList.push_back(TieDummyElm_t(nmo->getName(), m_EspTime));
			}
			else
			{	
				std::list<TieDummyElm_t>::iterator git = m_DummyGraveList.begin();
				nmo = MAGIC_MGR->GetMagic((*git).ElmName);
				(*git).CreateTime = m_EspTime;

				m_DummyList.push_back(*git);
				m_DummyGraveList.erase(git);
			}

			nmo->getParentSceneNode()->setPosition(m_LastCreatePos);
			nmo->getParentSceneNode()->setScale(mo->getParentSceneNode()->getScale());
			nmo->getParentSceneNode()->setOrientation(mo->getParentSceneNode()->getOrientation());
			((COTEParticleSystem*)nmo)->SetVisible(true);	

		}
		m_LastCreatePos = pos;
	}	

	m_EspTime += dTime;

	TIMECOST_END
}


// =================================================
// COTEParticleTieReactorEx
// =================================================

// ����

void COTEParticleTieReactorEx::Init(Ogre::MovableObject* mo)
{
	OTE_ASSERT(mo);

	m_BBS = (COTEBillBoardSet*)MAGIC_MGR->AutoCreateMagic(".BBSCom");
	SCENE_MGR->AttachObjToSceneNode(m_BBS);
	m_BBS->setPoolSize(80);
	m_BBSName = m_BBS->getName();

	const Ogre::Vector3& pos = mo->getParentSceneNode()->getWorldPosition();
	m_BBS->getParentSceneNode()->setPosition(pos);

	std::string texName = ((COTEParticleSystem*)mo)->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName();

	m_BBS->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName(texName);
	
	COTESimpleEmitter* emitter = (COTESimpleEmitter*)((COTEParticleSystem*)mo)->GetResPtr()->m_Emitter;	
	
	m_MinSize = emitter->GetfloatVal("��С�ߴ�");
	m_MaxSize = emitter->GetfloatVal("���ߴ�");

	m_BBS->setDefaultDimensions(m_MinSize, m_MaxSize);

	m_MinVelocity = emitter->GetfloatVal("��С����");
	m_MaxVelocity = emitter->GetfloatVal("�������");	
	
	m_MinDir = emitter->GetVector3Val("��С����");
	m_MaxDir = emitter->GetVector3Val("�����");

	m_LifeTime = emitter->GetfloatVal("����");

	m_EmitRate = emitter->GetIntVal("������");

	// ʹ�����Ӳ���ʾ

	((COTEParticleSystem*)mo)->mCreateTime = -1.0f;

	m_EspTime = 0;
	m_LastCreatePos = pos;
	m_LastFramePosition = pos;
	m_LastObj = mo;	
}

// -------------------------------------------------
void COTEParticleTieReactorEx::ClearContent()
{
	m_DummyGraveList.clear();
	m_DummyList.clear();

	if(m_BBS)
	{
		MAGIC_MGR->DeleteMagic(m_BBSName);
		m_BBS = NULL;
	}
}

// -------------------------------------------------
// ����

void COTEParticleTieReactorEx::Update(Ogre::MovableObject* mo, float dTime)
{
	TIMECOST_START
	
	COTEBillBoardSet* bbs = m_BBS;
	COTEParticleSystem* ps = (COTEParticleSystem*)mo;
	
	if(m_LastObj != mo)
	{
		ClearContent();	
		bbs->clear();
		m_LastObj = mo;
	}

	/// �������ϵͳ��̬����

	// �ߴ�

	COTEParticleSystemAffector* affector = ps->GetResPtr()->m_ParticleSystemAffector;	
	const Ogre::ColourValue& cor = affector->GetCorVal("colorData");
	float size_factor = cor.a;

	// ��ɫ 

	Ogre::ColourValue ncor = cor;
	ncor.a = 1.0f;

	m_BBS->SetColour(ncor);

	// ��ͼ

	std::string texName = ((COTEParticleSystem*)mo)->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName();
	m_BBS->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName(texName);

	// create

	const float c_RemainTime = m_LifeTime;
	const float c_CreateStep = m_MaxSize / 10.0f;	
	
	const Ogre::Vector3& pos = mo->getParentSceneNode()->getWorldPosition();	

	if( (m_LastCreatePos - pos).length() > c_CreateStep &&
		m_EspTime - m_LastCreateTime > m_LifeTime / m_EmitRate)
	{
		Ogre::Billboard* bb;
				
		if(m_DummyGraveList.empty())
		{				
			bb = bbs->createBillboard(
				Ogre::Vector3::ZERO
				);					
			m_DummyList.push_back(
				TieDummyElm_t(
				bbs->getNumBillboards() - 1, 
				m_EspTime)
				);
		}			
		else
		{	
			std::list<TieDummyElm_t>::iterator git = m_DummyGraveList.begin();
			OTE_ASSERT((*git).ID >= 0);

			bb = bbs->getBillboard( (*git).ID );
			(*git).CreateTime = m_EspTime;	
			bb->setPosition(Ogre::Vector3::ZERO);

			m_DummyList.push_back(*git);
			m_DummyGraveList.erase(git);
		}	

		m_LastCreatePos = pos;
		m_LastCreateTime = m_EspTime;
	}	

	// ����ʱ�����

	Ogre::Vector3 dMoved = pos - m_LastFramePosition;

	m_BBS->getParentSceneNode()->setPosition(pos);

	std::list<TieDummyElm_t>::iterator it = m_DummyList.begin();
	while(it != m_DummyList.end())
	{
		float deltaTime = m_EspTime - (*it).CreateTime;

		Ogre::Billboard* bb = bbs->getBillboard((*it).ID);

		// ɾ��

		if(deltaTime > c_RemainTime)
		{
			m_DummyGraveList.push_back(*it);

			bb->setDimensions(0.0f, 0.0f);

			m_DummyList.erase(it);
			it = m_DummyList.begin();

			continue;
		}
		else
		{				
			// ��ɫ

			float alpha = m_LifeTime - deltaTime > 0.0f ? (m_LifeTime - deltaTime) / m_LifeTime : 0.0f;
			bb->setColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f, alpha * alpha));

			// λ�� 

			float random = Ogre::Math::RangeRandom(0, 1.0f);	

			bb->setPosition(bb->getPosition() - dMoved + 
				(m_MinDir * random + m_MaxDir * (1.0f - random)) * 
				(m_MinVelocity * random + m_MaxVelocity * (1.0f - random)) * dTime);			

			// �ߴ�

			float size = m_MaxSize * size_factor;
			bb->setDimensions(size, size);			
		}

		++ it;
	}		
	
	m_LastFramePosition = pos;
	m_EspTime += dTime;
	

	TIMECOST_END
}

// =================================================
// COTEEntityTieReactor
// =================================================

// ����

void COTEEntityTieReactor::Init(Ogre::MovableObject* mo)
{
	m_EspTime = 0;
	m_LastCreatePos = Ogre::Vector3(-1.0f, -1.0f, -1.0f); // ��Чλ��
	m_LastObj = mo;
}

// -------------------------------------------------
void COTEEntityTieReactor::ClearContent()
{
	std::list<TieDummyElm_t>::iterator it = m_DummyGraveList.begin();
	while(it != m_DummyGraveList.end())
	{
		SCENE_MGR->RemoveEntity((*it).ElmName);
		++ it;
	}
	it = m_DummyList.begin();
	while(it != m_DummyList.end())
	{
		SCENE_MGR->RemoveEntity((*it).ElmName);
		++ it;
	}

	m_DummyGraveList.clear();
	m_DummyList.clear();
}

// -------------------------------------------------
// ����

void COTEEntityTieReactor::Update(Ogre::MovableObject* mo, float dTime)
{
	TIMECOST_START
	
	if(m_LastObj != mo)
	{
		ClearContent();
		m_LastObj = mo;
	}

	const float c_RemainTime = 0.5f;
	const float c_CreateStep = 2.0f * mo->getBoundingRadius();

	// ����ʱ������
	
	std::list<TieDummyElm_t>::iterator it = m_DummyList.begin();
	while(it != m_DummyList.end())
	{
		float deltaTime = m_EspTime - (*it).CreateTime;
		if(deltaTime > c_RemainTime)
		{
			m_DummyGraveList.push_back(*it);
			SCENE_MGR->GetEntity((*it).ElmName)->getParentSceneNode()->setVisible(false);

			m_DummyList.erase(it);
			it = m_DummyList.begin();
			continue;
		}
		else
		{
			COTEActorEquipmentMgr::SetColor(
				SCENE_MGR->GetEntity((*it).ElmName),
				Ogre::ColourValue(1.0f, 1.0f, 1.0f, 1.0f / (2.0f + deltaTime)
				) );				
		}

		++ it;
	}	

	// create entity dummy

	const Ogre::Vector3& pos = mo->getParentSceneNode()->getPosition();
	if(	(m_LastCreatePos - pos).length() > c_CreateStep
		)
	{
		if(m_LastCreatePos != Ogre::Vector3( -1.0f, -1.0f, -1.0f ))
		{			
			Ogre::MovableObject* nmo;
			if(m_DummyGraveList.empty())
			{
				nmo = MAGIC_MGR->AutoCreateMagic( ((Entity*)mo)->getMesh()->getName());
				SCENE_MGR->AttachObjToSceneNode(nmo);
				m_DummyList.push_back(TieDummyElm_t(nmo->getName(), m_EspTime));
				
			}			
			else
			{	
				std::list<TieDummyElm_t>::iterator git = m_DummyGraveList.begin();
				nmo = SCENE_MGR->GetEntity((*git).ElmName);
				(*git).CreateTime = m_EspTime;
				
				m_DummyList.push_back(*git);
				m_DummyGraveList.erase(git);
			}

			nmo->getParentSceneNode()->setPosition(m_LastCreatePos);
			nmo->getParentSceneNode()->setScale(mo->getParentSceneNode()->getScale());
			nmo->getParentSceneNode()->setOrientation(mo->getParentSceneNode()->getOrientation());		
			nmo->setVisible(true);	
			
		}

		m_LastCreatePos = pos;
	}	
	
	m_EspTime += dTime;

	TIMECOST_END
}

// =================================================
// COTEBillBoardTieReactor
// =================================================

// ����

void COTEBillBoardTieReactor::Init(Ogre::MovableObject* mo)
{
	m_EspTime = 0;
	m_LastCreatePos = mo->getParentSceneNode()->getWorldPosition();
	m_LastFramePosition = m_LastCreatePos;
	m_LastObj = mo;

	COTEBillBoardSet* bbs = (COTEBillBoardSet*)mo;
	bbs->setPoolSize(50);
}

// -------------------------------------------------
void COTEBillBoardTieReactor::ClearContent()
{
	m_DummyGraveList.clear();
	m_DummyList.clear();	
}

// -------------------------------------------------
// ����

void COTEBillBoardTieReactor::Update(Ogre::MovableObject* mo, float dTime)
{
	TIMECOST_START
	
	COTEBillBoardSet* bbs = (COTEBillBoardSet*)mo;

	const Ogre::Vector3& pos = bbs->getWorldPosition();
	Ogre::Vector3 dMoved = pos - m_LastFramePosition;

	if(m_LastObj != mo)
	{
		ClearContent();	
		bbs->clear();
		m_LastObj = mo;
	}

	const float c_RemainTime = 0.5f;
	const float c_CreateStep = mo->getBoundingRadius() / 5.0f;

	// create dummy

	if(	(m_LastCreatePos - pos).length() > c_CreateStep)
	{
		Ogre::Billboard* bb;					
		if(m_DummyGraveList.empty())
		{				
			bb = bbs->createBillboard(Ogre::Vector3::ZERO);					
			m_DummyList.push_back(
				TieDummyElm_t(
				bbs->getNumBillboards() - 1, 
				m_EspTime)
				);
			bb->setDimensions(bbs->getDefaultWidth(), bbs->getDefaultHeight());	
		}			
		else
		{	
			std::list<TieDummyElm_t>::iterator git = m_DummyGraveList.begin();
			OTE_ASSERT((*git).ID >= 0);

			bb = bbs->getBillboard( (*git).ID );
			(*git).CreateTime = m_EspTime;				

			bb->setDimensions(bbs->getDefaultWidth(), bbs->getDefaultHeight());				

			m_DummyList.push_back(*git);
			m_DummyGraveList.erase(git);
		}
		
		m_LastCreatePos = pos;
	}	

	// ����ʱ�����

	
	
	std::list<TieDummyElm_t>::iterator it = m_DummyList.begin();
	while(it != m_DummyList.end())
	{
		float deltaTime = m_EspTime - (*it).CreateTime;

		Ogre::Billboard* bb = bbs->getBillboard((*it).ID);

		// ɾ��

		if(deltaTime > c_RemainTime)
		{
			m_DummyGraveList.push_back(*it);
			bb->setDimensions(0.0f, 0.0f);	
			bb->setPosition(Ogre::Vector3::ZERO);
			m_DummyList.erase(it);
			it = m_DummyList.begin();
			continue;
		}
		else
		{			
			bb->setColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f, 1.0f / (2.0f + deltaTime)));
			bb->setPosition(bb->getPosition() - dMoved);	
		}

		++ it;
	}		
	
	m_LastFramePosition = pos;
	m_EspTime += dTime;
	

	TIMECOST_END
}


// =================================================
// COTEBillBoardTieReactorEx
// =================================================

// ����

void COTEBillBoardTieReactorEx::Init(Ogre::MovableObject* mo)
{
	m_EspTime = 0;
	m_LastCreatePos = mo->getParentSceneNode()->getWorldPosition();
	m_LastFramePosition = m_LastCreatePos;
	m_LastObj = mo;
}

// -------------------------------------------------
// ����

void COTEBillBoardTieReactorEx::Update(Ogre::MovableObject* mo, float dTime)
{
	TIMECOST_START
	
	COTEBillBoardSet* bbs = (COTEBillBoardSet*)mo;
	
	if(m_LastObj != mo)
	{
		ClearContent();	
		bbs->clear();
		m_LastObj = mo;
	}

	const float c_RemainTime = 0.5f;
	const float c_CreateStep = bbs->getDefaultWidth() / 2.0f;

	// create dummy
	
	const Ogre::Vector3& pos = bbs->getWorldPosition();

	if(	(m_LastCreatePos - pos).length() > c_CreateStep)
	{
		Ogre::Billboard* bb;
				
		if(m_DummyGraveList.empty())
		{				
			bb = bbs->createBillboard(
				Ogre::Vector3(
				Ogre::Math::RangeRandom(-0.5f, 0.5f), 
				Ogre::Math::RangeRandom(-0.5f, 0.5f),
				Ogre::Math::RangeRandom(-0.5f, 0.5f)
				)
				);					
			m_DummyList.push_back(
				TieDummyElm_t(
				bbs->getNumBillboards() - 1, 
				m_EspTime)
				);
			bb->setDimensions(bbs->getDefaultWidth(), bbs->getDefaultHeight());

		}			
		else
		{	
			std::list<TieDummyElm_t>::iterator git = m_DummyGraveList.begin();
			OTE_ASSERT((*git).ID >= 0);

			bb = bbs->getBillboard( (*git).ID );
			(*git).CreateTime = m_EspTime;				

			bb->setDimensions(bbs->getDefaultWidth(), bbs->getDefaultHeight());				
			bb->setPosition(
				Ogre::Vector3(
				Ogre::Math::RangeRandom(-0.2f, 0.2f), 
				Ogre::Math::RangeRandom(-0.2f, 0.2f),
				Ogre::Math::RangeRandom(-0.2f, 0.2f)
				)
				);	
			m_DummyList.push_back(*git);
			m_DummyGraveList.erase(git);
		}		
		m_LastCreatePos = pos;
	}	

	// ����ʱ�����

	Ogre::Vector3 dMoved = pos - m_LastFramePosition;
	
	std::list<TieDummyElm_t>::iterator it = m_DummyList.begin();
	while(it != m_DummyList.end())
	{
		float deltaTime = m_EspTime - (*it).CreateTime;

		Ogre::Billboard* bb = bbs->getBillboard((*it).ID);

		// ɾ��

		if(deltaTime > c_RemainTime)
		{
			m_DummyGraveList.push_back(*it);
			bb->setDimensions(0.0f, 0.0f);
			bb->setColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f, 0.0f));
			bb->setPosition(Ogre::Vector3::ZERO);
			m_DummyList.erase(it);
			it = m_DummyList.begin();
			continue;
		}
		else
		{			
			bb->setColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f, 1.0f / (2.0f + deltaTime)));
			bb->setPosition(bb->getPosition() - dMoved + 
				Ogre::Vector3(
					Ogre::Math::RangeRandom(-0.1f, 0.1f), 
					Ogre::Math::RangeRandom(-0.1f, 0.1f),
					Ogre::Math::RangeRandom(-0.1f, 0.1f)
					) * dTime
					);	
			float dSize = 1.0f + 0.5f * deltaTime;
			bb->setDimensions(bbs->getDefaultWidth() * dSize, bbs->getDefaultHeight() * dSize);	
				
		}

		++ it;
	}		
	
	m_LastFramePosition = pos;
	m_EspTime += dTime;
	

	TIMECOST_END
}

// =================================================
// COTEBillBoardChainTieReactor
// =================================================

// ����

void COTEBillBoardChainTieReactor::Init(Ogre::MovableObject* mo)
{
	m_EspTime = 0;	
	m_LastCreatePos = mo->getParentSceneNode()->getPosition();
	m_LastFramePosition = m_LastCreatePos;
	m_LastObj = mo;
}

// -------------------------------------------------
void COTEBillBoardChainTieReactor::ClearContent()
{
	m_DummyGraveList.clear();
	m_DummyList.clear();

}


// -------------------------------------------------
// ����

void COTEBillBoardChainTieReactor::Update(Ogre::MovableObject* mo, float dTime)
{
	TIMECOST_START
	
	COTEBillBoardChain* bbc = (COTEBillBoardChain*)mo;
	const Ogre::Vector3& pos = bbc->getWorldPosition();
	Ogre::Vector3 dMoved = pos - m_LastFramePosition;

	if(dMoved.length() < 0.001f/*�ٶ�Ϊ��*/ * dTime)
	{
		m_LastCreatePos = pos; 
		m_LastFramePosition = pos;
		m_EspTime = 0.0f;
		ClearContent();	
		bbc->clearAllChains();
		return;
		
	}

	if(m_LastObj != mo)
	{		
		ClearContent();	
		bbc->clearAllChains();
		m_LastObj = mo;
	}	


	const float c_CreateStep = 0.1f;
	const int c_NumChainElm = 12;
				
	// create dummy	
	if( (m_LastCreatePos - pos).length() > c_CreateStep)
	{
		if(bbc->getNumChainElements(0) <= c_NumChainElm)
		{		
			while(bbc->getNumChainElements(0) <= c_NumChainElm)
			{					
				bbc->addChainElement(0, 
					COTEBillBoardChain::Element(
					Ogre::Vector3::ZERO, 
					0.2f, 
					0.0f, 
					Ogre::ColourValue::Blue)
					);	

				int elmInd = bbc->getNumChainElements(0) - 1;

				m_DummyList.push_back(TieDummyElm_t(elmInd, m_EspTime));				

				COTEBillBoardChain::Element& nmo = bbc->getChainElement(0, elmInd );

				nmo.texCoord = float(elmInd) / c_NumChainElm;	
				nmo.position = Ogre::Vector3::ZERO;				
				nmo.colour.a = 1.0f;		
			
			}
			
		}
		else
		{	
			// ����

			if(m_DummyList.size() > 0)
			{					
				std::list<TieDummyElm_t>::iterator it = m_DummyList.begin();
				if((m_LastCreatePos - pos).length() > c_CreateStep)
				{
					std::list<TieDummyElm_t>::iterator nit = it; nit ++;
					while(nit != m_DummyList.end())
					{
						COTEBillBoardChain::Element& emo = bbc->getChainElement(0, (*it).ID );	
						COTEBillBoardChain::Element& nemo = bbc->getChainElement(0, (*nit).ID );												
					
						Ogre::Vector3 position = emo.position;	

						emo.position = nemo.position;					
											
						nemo.texCoord = float((*nit).ID) / c_NumChainElm;	
						nemo.colour.a = 1.0f - nemo.texCoord;	

						nemo.position = position;

						++ it;
						++ nit;
					}

					COTEBillBoardChain::Element& emo = bbc->getChainElement(0, (*it).ID );				
					emo.position = Ogre::Vector3::ZERO;

				}				
			}
		}

		m_LastCreatePos = pos;

	}
	else
	{
		std::list<TieDummyElm_t>::iterator it = m_DummyList.begin();
		while(it != m_DummyList.end())
		{
			COTEBillBoardChain::Element& emo = bbc->getChainElement(0, (*it).ID );	
								
			emo.texCoord -= dMoved.length() / c_CreateStep * (1.0f / c_NumChainElm);	// uv����
			if(emo.texCoord < 0.0f)
				emo.texCoord = 0.0f;
			else if(emo.texCoord > 1.0f)
				emo.texCoord = 1.0f;

			++ it;						
		}
		-- it;				
	
		COTEBillBoardChain::Element& emo = bbc->getChainElement(0, (*it).ID );					

		emo.texCoord = 1.0f;
		//emo.colour.a = 1.0f;	

	}

	// λ�� 

	std::list<TieDummyElm_t>::iterator it  = m_DummyList.begin();
	while(it != m_DummyList.end())
	{
		COTEBillBoardChain::Element& emo = bbc->getChainElement(0, (*it).ID );	
		emo.position = emo.position - dMoved;

		++ it;
	}

	m_EspTime += dTime;
	m_LastFramePosition = pos;

	TIMECOST_END
}



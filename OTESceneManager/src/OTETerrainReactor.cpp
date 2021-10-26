#include "oteterrainreactor.h"
#include "OTETilePage.h"
#include "OTETerrainHeightMgr.h"
#include "OTEMagicManager.h"
#include "OTEWater.h"

using namespace OTE;
using namespace Ogre;

// =================================================
// COTEFootPrintReactor
// =================================================
COTEFootPrintReactor::COTEFootPrintReactor() :
m_BBS(NULL)
{

}

// -------------------------------------------------
// 初始化
void COTEFootPrintReactor::Init(Ogre::MovableObject* mo)
{
	OTE_ASSERT(mo);

	m_EspTime = 0;
	m_LastCreatePos = mo->getParentSceneNode()->getWorldPosition();
	m_LastFramePosition = m_LastCreatePos;
		
	m_DummyGraveList.clear();
	m_DummyList.clear();

	if(!m_BBS)
	{	
		m_BBS = (COTEBillBoardSet*)MAGIC_MGR->AutoCreateMagic("__FootPrint__.BillBoardY");
		SCENE_MGR->AttachObjToSceneNode(m_BBS);
		m_BBS->getBillboard(0)->setDimensions(0, 0); // 第一个不可见 
	}
}

// -------------------------------------------------
// 更新
void COTEFootPrintReactor::Update(Ogre::MovableObject* mo, float dTime)
{
	if(!m_BBS || !COTETilePage::GetCurrentPage())
		return;	

	TIMECOST_START

	const float c_RemainTime = 5.0f;
	const float c_CreateStep = 1.0f;	
	
	COTEBillBoardSet* bbs = m_BBS;

	static Ogre::MovableObject* sLastObj = mo;
	static int sLastSurface = -1;	

	if(sLastObj != mo)
	{
		m_DummyGraveList.clear();
		m_DummyList.clear();
		m_BBS->clear();
		sLastObj = mo;
	}	

	// create dummy	 

	Ogre::Vector3 pos = mo->getParentSceneNode()->getWorldPosition();		
	unsigned int waterValue;
	COTELiquidManager::GetSingleton()->GetLiquidValue(waterValue, pos.x, pos.z);
	pos.y = GET_WATERREALHGT(waterValue);

	if(pos.y > 0)
	{
		if(sLastSurface != 1)
		{
			m_DummyGraveList.clear();
			m_DummyList.clear();
			m_BBS->clear();
			sLastSurface = 1;
			m_BBS->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName("txtt40105.tga");
		}

		// 水面		

		pos.y += 0.05f;
		bbs->getParentSceneNode()->setPosition(pos);		
	}
	else
	{
		if(sLastSurface != 0)
		{
			m_DummyGraveList.clear();
			m_DummyList.clear();
			m_BBS->clear();
			sLastSurface = 0;
			m_BBS->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName("189_zg.dds");
		}

		// 一般地面

		pos.y = COTETilePage::GetCurrentPage()->GetHeightAt(pos.x, pos.z) + 0.05f;
		bbs->getParentSceneNode()->setPosition(pos);

	}

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
		}			
		else
		{	
			std::list<TieDummyElm_t>::iterator git = m_DummyGraveList.begin();
			OTE_ASSERT((*git).ID > 0);

			bb = bbs->getBillboard( (*git).ID );
			(*git).CreateTime = m_EspTime;				

			bb->setDimensions(bbs->getDefaultWidth(), bbs->getDefaultHeight());				

			m_DummyList.push_back(*git);
			m_DummyGraveList.erase(git);
		}
		
		m_LastCreatePos = pos;
	}	

	// 按照时间更新

	Ogre::Vector3 dMoved = pos - m_LastFramePosition;
	
	std::list<TieDummyElm_t>::iterator it = m_DummyList.begin();
	while(it != m_DummyList.end())
	{
		float deltaTime = m_EspTime - (*it).CreateTime;

		Ogre::Billboard* bb = bbs->getBillboard((*it).ID);

		// 删除
	
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
			bb->setPosition(bb->getPosition() - dMoved);	
		}

		++ it;
	}		
	
	m_LastFramePosition = pos;
	m_EspTime += dTime;	

	TIMECOST_END
}

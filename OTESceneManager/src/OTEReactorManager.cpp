#include "OTECommon.h"
#include "OTEReactorManager.h"
#include "OTEEntityReactor.h"
#include "OTETransBangReactor.h"
#include "OTEActorMoveAIReactor.h"
#include "OTETerrainReactor.h"
#include "OTETieReactors.h"
#include "oteemitterreactors.h"
#include "OTEActorStreightMoveAIReactor.h"
#include "OTEBallPhysicsCtrller.h"


// ------------------------------------
using namespace OTE;
using namespace Ogre;

// ------------------------------------
COTEReactorManager* COTEReactorManager::s_pSingleton = NULL;
COTEReactorManager* COTEReactorManager::GetSingleton()
{	
	if(!s_pSingleton) 
	{
		s_pSingleton = new COTEReactorManager();
		s_pSingleton->Init();
	}
	
	return s_pSingleton;
}

// ------------------------------------
void COTEReactorManager::Init()
{		
	m_ReactorFactoryList["TC.Target"				] = new COTETrackCtrllerFactory();
	m_ReactorFactoryList["TC.Target.1"				] = new COTEParabolaTrackCtrllerFactory();	

	m_ReactorFactoryList["VS.Curve"					] = new COTEEntityReactorFactory();

	m_ReactorFactoryList["CPU.Bang"					] = new COTETransBangReactorFactory();
	m_ReactorFactoryList["CPU.FreeDrop"				] = new COTEFreeDropReactorFactory();

	m_ReactorFactoryList["TIE.Entity"				] = new COTEEntityTieReactorFactory();
	m_ReactorFactoryList["TIE.PS"					] = new COTEParticleTieReactorFactory();
	m_ReactorFactoryList["TIE.PS.1"					] = new COTEParticleTieReactorExFactory();
	
	m_ReactorFactoryList["TIE.BBS"					] = new COTEBillBoardTieReactorFactory();
	m_ReactorFactoryList["TIE.BBS.1"				] = new COTEBillBoardTieReactorExFactory();
	
	m_ReactorFactoryList["TIE.BBC"					] = new COTEBillBoardChainTieReactorFactory();

	m_ReactorFactoryList["TER.FootPrint"			] = new COTEFootPrintReactorFactory();

	m_ReactorFactoryList["AI.Move"					] = new COTEActorMoveAIReactorFactory();
	m_ReactorFactoryList["AI.MoveToTarget"			] = new COTEActorMoveToTargetAIReactorFactory();
	m_ReactorFactoryList["AI.StreightMove"			] = new COTEActorStreightMoveAIReactorFactory();
	m_ReactorFactoryList["AI.NpcStreightMove"		] = new COTENpcActorStreightMoveAIReactorFactory();
	
	
	m_ReactorFactoryList["EMT.Bas"					] = new COTEEmiterReactorFactory();	

	m_ReactorFactoryList["PYS.Ball"					] = new COTEBallPhysicsCtrllerFactory();
	

}

// ------------------------------------
void COTEReactorManager::DestroySingleton()
{
	if(s_pSingleton)
	{
		s_pSingleton->RemoveAllReactors();
		delete s_pSingleton;
		s_pSingleton = NULL;
	}
}

// --------------------------------------------------------
// 从文件名分析类型

std::string COTEReactorManager::GetReactorName(const std::string& rResName) 
{	
	// todo 大小写兼容问题
	
	if(rResName.find	(".ps.xml") != std::string::npos)
		return "EMT.Bas";

	return rResName;
}


// ------------------------------------
CReactorBase* COTEReactorManager::CreateReactor(const std::string& rResName, int id, Ogre::MovableObject* mo)
{
	std::string type = GetReactorName(rResName);

	CReactorBase* reactor = NULL;		
	HashMap<std::string, CReactorFactoryBase*>::iterator it = m_ReactorFactoryList.find(type);
	if(it != m_ReactorFactoryList.end())
	{		
		reactor = it->second->Create(mo, rResName);	 
		if(reactor)
		{
			reactor->m_FactoryName = it->first; // 记录创建工厂的名字
			reactor->m_ResName = rResName;

			reactor->Init(mo);	// 这里初始化
		}
			
	}

	if(!reactor)
	{
		OTE_MSG("反应器创建失败 可能是没有这个类型对应的工厂 : " << type, "失败")
	}
	else
	{
		AddReactor(reactor, id);
	}

	return reactor;
}

// ------------------------------------
void COTEReactorManager::DestroyReactor(CReactorBase* reactor)
{	
	if(reactor->m_FactoryName.empty())
		return;

	HashMap<std::string, CReactorFactoryBase*>::iterator it = m_ReactorFactoryList.find(reactor->m_FactoryName);
	if(it != m_ReactorFactoryList.end())
	{
		(it->second)->Delete(reactor);

		//OTE_TRACE("摧毁反应器 :" << it->first)
	}
	else
	{	
		OTE_MSG("反应器摧毁失败 FactoryName:" << reactor->m_FactoryName, "失败")
	}
}


// ------------------------------------
CReactorBase*	COTEReactorManager::GetReactor(int id)
{
	HashMap<int, CReactorBase*>::iterator it = m_ReactorList.find(id);
	if(it != m_ReactorList.end())
	{			
		return it->second;	
	}

	return NULL;		
}

// ------------------------------------
bool COTEReactorManager::AddReactor(CReactorBase* reactor, int id)
{
	if(id == -1) // 自动分配ID
	{
		static int sActorAutoID = -1;
		id = sActorAutoID --;
	}

	if(GetReactor(id))
	{
		OTE_TRACE("重复添加反应器 id: " << id)
		return false;
	}

	m_ReactorList[id] = reactor;
	//OTE_TRACE("添加反应器 id: " << id)

	return true;
}

// ------------------------------------
void COTEReactorManager::RemoveReactor(CReactorBase* reactor)
{
	HashMap<int, CReactorBase*>::iterator it = m_ReactorList.begin();
	while(it != m_ReactorList.end())
	{
		if(it->second == reactor)
		{
			//delete it->second;
			DestroyReactor(it->second);
			m_ReactorList.erase(it);
			break;
		}
		++ it;
	}
}

// ------------------------------------
void COTEReactorManager::RemoveReactor(int id)
{
	HashMap<int, CReactorBase*>::iterator it = m_ReactorList.find(id);
	if(it != m_ReactorList.end())
	{
		//delete it->second;
		DestroyReactor(it->second);
		m_ReactorList.erase(it);		
	}
}

// ------------------------------------
void COTEReactorManager::RemoveAllReactors()
{
	HashMap<int, CReactorBase*>::iterator it = m_ReactorList.begin();
	while(it != m_ReactorList.end())
	{	
		//delete it->second;
		DestroyReactor(it->second);
		++ it;
	}
	m_ReactorList.clear();
}



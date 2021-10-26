#include "OTEMagicManager.h"
#include "OTEParticlesystem.h"
#include "OTEBillBoardSet.h"
#include "OTEBillBoardChain.h"
#include "OTEReactorManager.h"
#include "OTEActionReactorUsage.h"

// --------------------------------------------------------
using namespace OTE;
using namespace Ogre;

namespace OTE
{
// ========================================================
// COTECameraFactory
// ========================================================
class COTECameraFactory : public CMagicFactoryBase
{
public:

	virtual Ogre::MovableObject* Create(const std::string& ResName,	const std::string& Name, bool cloneMat)
	{
		return SCENE_CAM;
	}
	
	virtual void Delete(const std::string& Name)
	{
		// do nothing
	}

};

}

// ========================================================
// COTEMagicManager
// ========================================================
unsigned int g_MagicCount = 0;

// --------------------------------------------------------
// 从文件名分析类型

std::string COTEMagicManager::GetMagType(const std::string& rResName) 
{	
	// todo 大小写兼容问题
	if(rResName.find		(".obj") != std::string::npos || rResName.find(".mesh") != std::string::npos)
        return "OTEEntity";	
	else if(rResName.find	(".ps.xml") != std::string::npos)
		return "PS";
	else if(rResName.find	(".BBC") != std::string::npos)
		return "BBC";
	else if(rResName.find	(".BBS") != std::string::npos)
		return "BBS";	
	else if(rResName.find	(".Cam") != std::string::npos)
		return "Cam";

	return "";
}

// --------------------------------------------------------
COTEMagicManager* COTEMagicManager::s_pSingleton = NULL;
COTEMagicManager* COTEMagicManager::GetSingleton()
{
	if(!s_pSingleton) 
	{
		s_pSingleton = new COTEMagicManager();
		s_pSingleton->Init();
	}
	return s_pSingleton; 
}

// --------------------------------------------------------
void COTEMagicManager::DestroySingleton()
{	
	if(!s_pSingleton)
		return;

	// 清理工厂

	HashMap<std::string, CMagicFactoryBase*>::iterator it = s_pSingleton->m_MagFactoryList.begin();
	while(it != s_pSingleton->m_MagFactoryList.end())
	{
		delete it->second;
		++ it;
	}
	s_pSingleton->m_MagFactoryList.clear();

	SAFE_DELETE(s_pSingleton)	
}

// --------------------------------------------------------
void OnRenderFinished(int ud)
{
	COTEMagicManager::GetSingleton()->_UpdateGrave();
}

// --------------------------------------------------------
void COTEMagicManager::Init()
{
	// 注册工厂

	m_MagFactoryList["PS"]			= new COTEParticleSystemFactory();
	m_MagFactoryList["BBS"]			= new COTEBillBoardSetFactory();
	m_MagFactoryList["BBC"]			= new COTEBillBoardChainFactory();
	m_MagFactoryList["OTEEntity"]	= new COTEEntityFactory();
	m_MagFactoryList["Cam"]			= new COTECameraFactory();

	// 渲染回调

	SCENE_MGR->AddListener(COTEQTSceneManager::eRenderCB, OnRenderFinished);	
	SCENE_MGR->AddListener(COTEQTSceneManager::eSceneClear, OnRenderFinished);
}

// --------------------------------------------------------
//清理
void COTEMagicManager::Clear()
{
	if(!s_pSingleton)
		return;	

	while(!s_pSingleton->m_MagDataList.empty())	
		s_pSingleton->DeleteMagic(((std::string)(s_pSingleton->m_MagDataList.begin()->first)).c_str(), true);

	s_pSingleton->m_MagDataList.clear();
}

// --------------------------------------------------------
//根据时间自动产生名称
Ogre::MovableObject* COTEMagicManager::AutoCreateMagic(const std::string& ResName, bool cloneMat)
{	
	std::stringstream ss;
	ss << g_MagicCount ++ << "_"  << ResName;

	return CreateMagic(ResName, ss.str(), cloneMat);	
}

// --------------------------------------------------------
// 创建Magic实体
Ogre::MovableObject* COTEMagicManager::CreateMagic(
								const std::string& ResName, 
								const std::string& Name, 
								bool cloneMat)
{
	
	std::map<std::string, MagicData_t*>::iterator it = m_MagDataList.find(Name);
	if(it != m_MagDataList.end())
	{
		return ((MagicData_t*)(it->second))->Entity;
	}		

	MagicData_t* md = LoadMagic(ResName, Name, cloneMat);

	m_MagDataList[Name] = md;
	return md->Entity;

}

// --------------------------------------------------------
// 加载Magic实体

MagicData_t* COTEMagicManager::LoadMagic(const std::string& ResName, 
										 const std::string& Name, 										 
										 bool cloneMat)
{
	OTE_ASSERT(!ResName.empty() && !Name.empty());		

	MagicData_t* md = new MagicData_t();		

	// 是否绑定反应器
	
	if( ResName.find('@') != std::string::npos )
	{	
		md->MagicFile = OTEHelper::ScanfStringAt(ResName, 1, '@');	

		md->Type = GetMagType(md->MagicFile);

		OTE_ASSERT(!md->Type.empty());	

		Create(md->MagicFile, Name, md, cloneMat);
		
		// 常绑定反应器
		
		std::string reactorName = OTEHelper::ScanfStringAt(ResName + "@", 2, '@');
		CReactorBase* ra = COTEReactorManager::GetSingleton()->CreateReactor(reactorName, -1, md->Entity);
		if(ra)
		{
			md->Entity->SetBindReactor(ra);	
		}

		OTE_TRACE("Magic(" << md->MagicFile << ") 帮定常反应器: " << reactorName)
	}
	else
	{
		md->MagicFile = ResName;

		md->Type = GetMagType(md->MagicFile);

		OTE_ASSERT(!md->Type.empty());	

		Create(md->MagicFile, Name, md, cloneMat);
	}

	return md;	
}

// --------------------------------------------------------
//创建
void COTEMagicManager::Create(const std::string& ResName, const std::string& Name, MagicData_t* pMagData, bool cloneMat)
{
	HashMap<std::string, CMagicFactoryBase*>::iterator it = m_MagFactoryList.find(pMagData->Type);
	if(it != m_MagFactoryList.end())
	{
		pMagData->Entity = it->second->Create(ResName, Name, cloneMat);
		//OTE_TRACE("创建Magic: Res = " << ResName << " Name = " << Name);
	}
	else
	{
		OTE_MSG_ERR("创建magic不存在这个类型的工厂 Type = " << pMagData->Type)		
	}
}

// --------------------------------------------------------
//获取Magic实体
Ogre::MovableObject* COTEMagicManager::GetMagic(const std::string& Name)
{
	OTE_ASSERT(!Name.empty());

	std::map<std::string, MagicData_t*>::iterator it = m_MagDataList.find(Name);
	if(it != m_MagDataList.end())
	{
		return ((MagicData_t*)(it->second))->Entity;
	}

	return NULL;
}


// --------------------------------------------------------
// 创建MagicList
void COTEMagicManager::GetMagicList(std::vector<Ogre::MovableObject*>* pMagList)
{
	OTE_ASSERT(pMagList);

	std::map<std::string, MagicData_t*>::iterator it = m_MagDataList.begin();
	while(it != m_MagDataList.end())
	{
		if(it->second->Entity)
			pMagList->push_back(it->second->Entity);
		it++;
	}
}

// --------------------------------------------------------
//删除实体
void COTEMagicManager::DeleteMagic(const std::string& Name, bool Immediately)
{	
	OTE_ASSERT(!Name.empty());

	if(SCENE_MGR->IsObjFocused(Name))
		SCENE_MGR->ClearCurFocusObj();

	std::map<std::string, MagicData_t*>::iterator it = m_MagDataList.find(Name);
	if(it != m_MagDataList.end())
	{
		MagicData_t* Ent = (MagicData_t*)(it->second);
		m_MagDataList.erase(it);
		
		m_GraveList.push_back(Ent);
	}
	else
	{
		OTE_TRACE_ERR("移除Magic失败! Name = " << Name << " 原因：这个特效在列表中不存在");
	}

	if(Immediately)
	{
		_UpdateGrave();
	}

}

// --------------------------------------------------------
// 更新坟墓列表(真正的删除操作)

void COTEMagicManager::_UpdateGrave()
{
	std::list<MagicData_t*>::iterator it = m_GraveList.begin();
	while(it != m_GraveList.end())
	{
		MagicData_t* Ent = *it;	
		std::string Name = Ent->Entity->getName();

		HashMap<std::string, CMagicFactoryBase*>::iterator factoryIt = m_MagFactoryList.find(Ent->Type);
		if(factoryIt != m_MagFactoryList.end())
		{
			// 先移除常帮定的反应器

			if(Ent->Entity->GetBindReactor())	
			{
				COTEActionReactorUsage::Unbind(Ent->Entity, (CReactorBase*)Ent->Entity->GetBindReactor());
				
				COTEReactorManager::GetSingleton()->RemoveReactor((CReactorBase*)Ent->Entity->GetBindReactor());				
				
				Ent->Entity->SetBindReactor(NULL);
			}
			
			// 删除

			OTE_TRACE("移除Magic: " << Name)
			factoryIt->second->Delete(Name);
			
			//OTE_TRACE("移除Magic: Name = " << Name);			
		}
		else
		{
			OTE_TRACE_ERR("移除Magic失败! Name = " << Name << " 原因：没找到对应的工厂")
		}

		delete Ent;

		++ it;	
	}

	m_GraveList.clear();

}

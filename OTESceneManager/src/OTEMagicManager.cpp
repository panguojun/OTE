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
// ���ļ�����������

std::string COTEMagicManager::GetMagType(const std::string& rResName) 
{	
	// todo ��Сд��������
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

	// ������

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
	// ע�Ṥ��

	m_MagFactoryList["PS"]			= new COTEParticleSystemFactory();
	m_MagFactoryList["BBS"]			= new COTEBillBoardSetFactory();
	m_MagFactoryList["BBC"]			= new COTEBillBoardChainFactory();
	m_MagFactoryList["OTEEntity"]	= new COTEEntityFactory();
	m_MagFactoryList["Cam"]			= new COTECameraFactory();

	// ��Ⱦ�ص�

	SCENE_MGR->AddListener(COTEQTSceneManager::eRenderCB, OnRenderFinished);	
	SCENE_MGR->AddListener(COTEQTSceneManager::eSceneClear, OnRenderFinished);
}

// --------------------------------------------------------
//����
void COTEMagicManager::Clear()
{
	if(!s_pSingleton)
		return;	

	while(!s_pSingleton->m_MagDataList.empty())	
		s_pSingleton->DeleteMagic(((std::string)(s_pSingleton->m_MagDataList.begin()->first)).c_str(), true);

	s_pSingleton->m_MagDataList.clear();
}

// --------------------------------------------------------
//����ʱ���Զ���������
Ogre::MovableObject* COTEMagicManager::AutoCreateMagic(const std::string& ResName, bool cloneMat)
{	
	std::stringstream ss;
	ss << g_MagicCount ++ << "_"  << ResName;

	return CreateMagic(ResName, ss.str(), cloneMat);	
}

// --------------------------------------------------------
// ����Magicʵ��
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
// ����Magicʵ��

MagicData_t* COTEMagicManager::LoadMagic(const std::string& ResName, 
										 const std::string& Name, 										 
										 bool cloneMat)
{
	OTE_ASSERT(!ResName.empty() && !Name.empty());		

	MagicData_t* md = new MagicData_t();		

	// �Ƿ�󶨷�Ӧ��
	
	if( ResName.find('@') != std::string::npos )
	{	
		md->MagicFile = OTEHelper::ScanfStringAt(ResName, 1, '@');	

		md->Type = GetMagType(md->MagicFile);

		OTE_ASSERT(!md->Type.empty());	

		Create(md->MagicFile, Name, md, cloneMat);
		
		// ���󶨷�Ӧ��
		
		std::string reactorName = OTEHelper::ScanfStringAt(ResName + "@", 2, '@');
		CReactorBase* ra = COTEReactorManager::GetSingleton()->CreateReactor(reactorName, -1, md->Entity);
		if(ra)
		{
			md->Entity->SetBindReactor(ra);	
		}

		OTE_TRACE("Magic(" << md->MagicFile << ") �ﶨ����Ӧ��: " << reactorName)
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
//����
void COTEMagicManager::Create(const std::string& ResName, const std::string& Name, MagicData_t* pMagData, bool cloneMat)
{
	HashMap<std::string, CMagicFactoryBase*>::iterator it = m_MagFactoryList.find(pMagData->Type);
	if(it != m_MagFactoryList.end())
	{
		pMagData->Entity = it->second->Create(ResName, Name, cloneMat);
		//OTE_TRACE("����Magic: Res = " << ResName << " Name = " << Name);
	}
	else
	{
		OTE_MSG_ERR("����magic������������͵Ĺ��� Type = " << pMagData->Type)		
	}
}

// --------------------------------------------------------
//��ȡMagicʵ��
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
// ����MagicList
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
//ɾ��ʵ��
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
		OTE_TRACE_ERR("�Ƴ�Magicʧ��! Name = " << Name << " ԭ�������Ч���б��в�����");
	}

	if(Immediately)
	{
		_UpdateGrave();
	}

}

// --------------------------------------------------------
// ���·�Ĺ�б�(������ɾ������)

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
			// ���Ƴ����ﶨ�ķ�Ӧ��

			if(Ent->Entity->GetBindReactor())	
			{
				COTEActionReactorUsage::Unbind(Ent->Entity, (CReactorBase*)Ent->Entity->GetBindReactor());
				
				COTEReactorManager::GetSingleton()->RemoveReactor((CReactorBase*)Ent->Entity->GetBindReactor());				
				
				Ent->Entity->SetBindReactor(NULL);
			}
			
			// ɾ��

			OTE_TRACE("�Ƴ�Magic: " << Name)
			factoryIt->second->Delete(Name);
			
			//OTE_TRACE("�Ƴ�Magic: Name = " << Name);			
		}
		else
		{
			OTE_TRACE_ERR("�Ƴ�Magicʧ��! Name = " << Name << " ԭ��û�ҵ���Ӧ�Ĺ���")
		}

		delete Ent;

		++ it;	
	}

	m_GraveList.clear();

}

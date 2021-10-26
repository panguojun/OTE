#include "oteactorequipmentmgr.h"
#include "OTEQTSceneManager.h"
#include "OTEBinMeshSerializer.h"
#include "OTEEntityRender.h"

using namespace Ogre;
using namespace OTE;

// -------------------------------------------------
// ��ʾװ��

void COTEActorEquipmentMgr::ShowEquipment(
	COTEActorEntity* ae, const std::string& EquipmentName, bool isVisble)
{
	const std::string& subEntName = GetSubEntityName(ae, EquipmentName);
	if(!subEntName.empty())
	{
		ae->GetSubEntity(subEntName)->setVisible(isVisble);
		OTE_TRACE("ShowEquipment. subName : " << EquipmentName << " subEntName: " << subEntName << (isVisble ? " true" : " false"))
	}	
	else
	{
		OTE_TRACE_ERR("ShowEquipment û���ҵ�װ�� subName : " << EquipmentName)
	}
}


// -------------------------------------------------
// ��ʾ����װ��

void COTEActorEquipmentMgr::ShowAllEquipment(COTEActorEntity* ae, bool isVisble)
{
	HashMap<std::string, std::string>::iterator it = ae->m_EquipmentMapEx.begin();
	while(it != ae->m_EquipmentMapEx.end())
	{		
		ae->GetSubEntity(it->second)->setVisible(isVisble);
		++ it;
	}
}

// --------------------------------------------------
// ����װ��

void COTEActorEquipmentMgr::SetupEquipments(COTEActorEntity* ae, bool HasMesh)
{
	// ���SubEntityList
	if(!HasMesh)
		ae->ClearSubEntityList();

	ae->BuildSubEntityList(ae->mMesh, &ae->mSubEntityList, HasMesh);	

    if (ae->hasSkeleton())
    {	
		float timePos = ae->m_pCurAnimState ? ae->m_pCurAnimState->getTimePosition() : 0;

		bool isCurAnimEnabled = ae->m_pCurAnimState ? ae->m_pCurAnimState->getEnabled() : false;
	
		ae->mMesh->_initAnimationState(ae->mAnimationState);
		if(ae->m_pCurAnimState)
		{
			ae->m_pCurAnimState->setEnabled(isCurAnimEnabled);
			ae->m_pCurAnimState->setTimePosition(timePos);
		}
        ae->prepareTempBlendBuffers();
    }

	ae->m_EquipmentNeedUpdate = false;
}

// -------------------------------------------------
void COTEActorEquipmentMgr::CloneMaterial(COTEActorEntity* ae, 
										  const std::string& EquipmentName, 
										  Ogre::MaterialPtr& newMat)
{
	COTESubEntity* se = NULL;
	
	HashMap<std::string, std::string>::iterator it = ae->m_EquipmentMapEx.find(EquipmentName);
	if(it != ae->m_EquipmentMapEx.end())
	{
		se = ae->GetSubEntity(it->second);
	}

	if(!se)
	{
		OTE_MSG("û���ҵ���ģ�ͣ�", "ʧ��")
		return;
	}

	Ogre::MaterialPtr mat = se->getMaterial();

	// clone ����

	std::stringstream newMatName;
	std::string matName = mat->getName();
	newMatName << matName;
	if(matName.find(ae->getName()) == std::string::npos)
	{
		newMatName << " " << ae->getName();
	}

	newMat = Ogre::MaterialManager::getSingleton().getByName(newMatName.str());
	if(newMat.isNull())
	{			
		newMat = mat->clone(newMatName.str());	
		OTE_TRACE("Clone���� : " << newMatName.str())
	}	

	se->setMaterialName(newMatName.str());	

	ae->m_EquipmentMapEx[EquipmentName] = newMatName.str();

	OTE_TRACE("���Ʋ��ʣ� EquipmentName : " << EquipmentName << ", " << matName << "->" << newMatName.str());
}

// -------------------------------------------------
//�ı��������װ�����ڵ�ʵ����ɫ

void COTEActorEquipmentMgr::SetColor(COTEActorEntity* ae, Ogre::ColourValue color)
{
	std::vector<COTESubEntity*>* subEntList = ae->GetSubEntityList();
			
	std::vector<COTESubEntity*>::iterator ita = subEntList->begin();

	while(ita != subEntList->end())
	{
		Ogre::MaterialPtr mptr = (*ita)->getMaterial();					

		if(!mptr->isLoaded())
			mptr->touch();

		mptr->setAmbient(color);			
		mptr->setDiffuse(color);

		++ ita;	
	}

	//����

	HashMap<std::string, Hook_t*>::iterator ip = ae->m_PotHookList.begin();
	while(ip != ae->m_PotHookList.end())
	{
		Hook_t* phook = ip->second;

		std::list<Ogre::MovableObject*>::iterator it = phook->HookEntityList.begin();
		while(it != phook->HookEntityList.end())
		{		
			SetColor((COTEActorEntity*)(*it), color);
			++ it;
		}

		ip++;
	}

}

// --------------------------------------------------
// װ������ɫ

void COTEActorEquipmentMgr::SetSubmeshColor(COTEActorEntity* ae, 
											const std::string& EquipmentName, 
											const Ogre::ColourValue& color)
{	
	// ���Ʋ���

	Ogre::MaterialPtr newMat;
	CloneMaterial(ae, EquipmentName, newMat);

	// ������ɫ

	newMat->setAmbient(color);					
	newMat->setDiffuse(color);		
}

// -------------------------------------------------
//�ı�װ������ͼ

void COTEActorEquipmentMgr::SetSubmeshTexture( COTEActorEntity* ae, 
											   const std::string& EquipmentName, 
											   const std::string& TextureName)
{
	// ���Ʋ���

	Ogre::MaterialPtr newMat;
	CloneMaterial(ae, EquipmentName, newMat);

	// ��ͼ

	if(newMat->getBestTechnique(0)->getNumPasses() == 0)
	{
		newMat->getBestTechnique(0)->createPass();
	}
	Ogre::Pass* pass = newMat->getBestTechnique(0)->getPass(0);
	if(pass->getNumTextureUnitStates() == 0)
	{
		pass->createTextureUnitState();
	}				

	pass->getTextureUnitState(0)->setTextureName(TextureName);	
	
}


// -------------------------------------------------
//����Mesh
void COTEActorEquipmentMgr::ExportMesh(COTEActorEntity* ae, const std::string& ResName)
{
	Ogre::MeshSerializer msr;
	msr.exportMesh(ae->getMesh().getPointer(), ResName);

	OTE_LOGMSG("����Mesh: " << ResName)
	OTE_TRACE("����Mesh: " << ResName)
}

// -------------------------------------------------
//����SubMesh
void COTEActorEquipmentMgr::ExportSubMesh(COTEActorEntity* ae, const std::string& SubMeshName, const std::string& ResName)
{
	Ogre::SubMesh *pSubMesh = ae->getMesh().getPointer()->getSubMesh(SubMeshName);
	if(!pSubMesh)
	{
		OTE_MSG("��SubMeshδ�ҵ�!", "����")
		return;
	}

	COTEBinMeshSerizlizer bsr;
	bsr.WriteSubMesh(pSubMesh, SubMeshName, ResName.c_str());

	OTE_LOGMSG("����SubMesh: " << ResName)
	OTE_TRACE("����SubMesh: " << ResName)
}

// -------------------------------------------------
//���SubMesh
Ogre::String COTEActorEquipmentMgr::AddSubMesh(COTEActorEntity* ae, const std::string& EquipmentName, const std::string& ResName)
{
	if(ae->getMesh().isNull())
	{
		OTE_TRACE("���SubMeshʧ�ܣ� ��ɫû�м�����ɣ� ")
		return "";
	}
	
	if(!CHECK_RES(ResName))
	{
		OTE_TRACE("û���ҵ�submesh�ļ��� " << ResName)
		return "";
	}

	// ÿ����ͼ���submesh ��Ҫ��һ���ļ� �ļ����¼submesh������ ����Ƿ��Ѿ���mesh�У�

	COTEBinMeshSerizlizer bsr;
	
	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(ResName);

	Ogre::String name = bsr.ReadSubMesh(stream, ae->getMesh().getPointer());

	ae->m_EquipmentMapEx[EquipmentName] = name;

	OTE_LOGMSG("���SubMesh. ResName : " << ResName << " submesh: " << name)
	OTE_TRACE("���SubMesh. ResName : " << ResName << " submesh: " << name)

	return name;
}

// -------------------------------------------------
//ɾ��SubMesh

void COTEActorEquipmentMgr::RemoveSubMesh(COTEActorEntity* ae, const std::string& EquipmentName)
{	
	const std::string& subEntName = COTEActorEquipmentMgr::GetSubEntityName(ae, EquipmentName);
	if(subEntName.empty())	
	{
		OTE_MSG("û���ҵ�subentity! subname : " << EquipmentName, "ʧ��")
		return;
	}

	Mesh::SubMeshNameMap::iterator i = ae->getMesh()->mSubMeshNameMap.find(subEntName);
	if(i != ae->getMesh()->mSubMeshNameMap.end())
	{
		Ogre::ushort n = i->second;
		Mesh::SubMeshList::iterator it = ae->getMesh()->mSubMeshList.begin() + n;
		SubMesh *s = *it;

		ae->getMesh()->mSubMeshList.erase(it);
		ae->getMesh()->mSubMeshNameMap.erase(i);

		for(i = ae->getMesh()->mSubMeshNameMap.begin(); i != ae->getMesh()->mSubMeshNameMap.end(); ++ i)
		{
			if(i->second > n)
				i->second = i->second - 1;
		}

		bool isAni = ae->m_pCurAnimState ? ae->m_pCurAnimState->getEnabled() : false;
		if(isAni)
			ae->m_pCurAnimState->setEnabled(true);

		delete s;

		OTE_TRACE("ɾ��SubMesh: " << EquipmentName)
		OTE_LOGMSG("ɾ��SubMesh: " << EquipmentName)
	}
	else
	{
		OTE_MSG("û���ҵ�SubMesh! subEntName : " << subEntName, "ʧ��")
	}	
}

// -------------------------------------------------
//����SubMeshName
void COTEActorEquipmentMgr::SetSubMeshName(COTEActorEntity* ae, const std::string& OldName, const std::string& NewName)
{
	Ogre::Mesh::SubMeshNameMap::iterator i = ae->getMesh()->mSubMeshNameMap.find(OldName) ;
	if (i == ae->getMesh()->mSubMeshNameMap.end())
	{
		OTE_MSG("��SubMesh������!", "����")
		return;
	}

	Ogre::ushort n = i->second;
	ae->getMesh()->mSubMeshNameMap.erase(i);
	ae->getMesh()->nameSubMesh(NewName, n);

	OTE_TRACE("����SubMeshName. OldName : " << OldName << " NewName: " << NewName)
}

// -------------------------------------------------
const std::string& COTEActorEquipmentMgr::GetSubEntityName(COTEActorEntity* ae, const std::string& EquipmentName)
{
	static std::string retName;
	HashMap<std::string, std::string>::iterator it = ae->m_EquipmentMapEx.find(EquipmentName);
	if(it != ae->m_EquipmentMapEx.end())
	{		
		retName = it->second.c_str();		
	}
	else
	{
		retName = "";
	}

	return retName;
}


// -------------------------------------------------
COTESubEntity* COTEActorEquipmentMgr::GetSubEntity(COTEActorEntity* ae, const std::string& EquipmentName)
{
	const std::string& subEntName = GetSubEntityName(ae, EquipmentName);
	if(!subEntName.empty())
	{
		return (COTESubEntity*)ae->GetSubEntity(subEntName);
	}	

	OTE_TRACE("GetSubEntity û���ҵ�װ�� subName : " << EquipmentName)
	
	return NULL;
}
#include "oteactorequipmentmgr.h"
#include "OTEQTSceneManager.h"
#include "OTEBinMeshSerializer.h"
#include "OTEEntityRender.h"

using namespace Ogre;
using namespace OTE;

// -------------------------------------------------
// 显示装备

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
		OTE_TRACE_ERR("ShowEquipment 没有找到装备 subName : " << EquipmentName)
	}
}


// -------------------------------------------------
// 显示所有装备

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
// 更新装备

void COTEActorEquipmentMgr::SetupEquipments(COTEActorEntity* ae, bool HasMesh)
{
	// 清除SubEntityList
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
		OTE_MSG("没有找到子模型！", "失败")
		return;
	}

	Ogre::MaterialPtr mat = se->getMaterial();

	// clone 材质

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
		OTE_TRACE("Clone材质 : " << newMatName.str())
	}	

	se->setMaterialName(newMatName.str());	

	ae->m_EquipmentMapEx[EquipmentName] = newMatName.str();

	OTE_TRACE("复制材质： EquipmentName : " << EquipmentName << ", " << matName << "->" << newMatName.str());
}

// -------------------------------------------------
//改变包括所有装备在内的实体颜色

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

	//武器

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
// 装备的颜色

void COTEActorEquipmentMgr::SetSubmeshColor(COTEActorEntity* ae, 
											const std::string& EquipmentName, 
											const Ogre::ColourValue& color)
{	
	// 复制材质

	Ogre::MaterialPtr newMat;
	CloneMaterial(ae, EquipmentName, newMat);

	// 设置颜色

	newMat->setAmbient(color);					
	newMat->setDiffuse(color);		
}

// -------------------------------------------------
//改变装备的贴图

void COTEActorEquipmentMgr::SetSubmeshTexture( COTEActorEntity* ae, 
											   const std::string& EquipmentName, 
											   const std::string& TextureName)
{
	// 复制材质

	Ogre::MaterialPtr newMat;
	CloneMaterial(ae, EquipmentName, newMat);

	// 贴图

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
//导出Mesh
void COTEActorEquipmentMgr::ExportMesh(COTEActorEntity* ae, const std::string& ResName)
{
	Ogre::MeshSerializer msr;
	msr.exportMesh(ae->getMesh().getPointer(), ResName);

	OTE_LOGMSG("导出Mesh: " << ResName)
	OTE_TRACE("导出Mesh: " << ResName)
}

// -------------------------------------------------
//导出SubMesh
void COTEActorEquipmentMgr::ExportSubMesh(COTEActorEntity* ae, const std::string& SubMeshName, const std::string& ResName)
{
	Ogre::SubMesh *pSubMesh = ae->getMesh().getPointer()->getSubMesh(SubMeshName);
	if(!pSubMesh)
	{
		OTE_MSG("该SubMesh未找到!", "错误")
		return;
	}

	COTEBinMeshSerizlizer bsr;
	bsr.WriteSubMesh(pSubMesh, SubMeshName, ResName.c_str());

	OTE_LOGMSG("导出SubMesh: " << ResName)
	OTE_TRACE("导出SubMesh: " << ResName)
}

// -------------------------------------------------
//添加SubMesh
Ogre::String COTEActorEquipmentMgr::AddSubMesh(COTEActorEntity* ae, const std::string& EquipmentName, const std::string& ResName)
{
	if(ae->getMesh().isNull())
	{
		OTE_TRACE("添加SubMesh失败！ 角色没有加载完成！ ")
		return "";
	}
	
	if(!CHECK_RES(ResName))
	{
		OTE_TRACE("没有找到submesh文件： " << ResName)
		return "";
	}

	// 每次试图添加submesh 都要读一下文件 文件里记录submesh的名称 检查是否已经在mesh中！

	COTEBinMeshSerizlizer bsr;
	
	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(ResName);

	Ogre::String name = bsr.ReadSubMesh(stream, ae->getMesh().getPointer());

	ae->m_EquipmentMapEx[EquipmentName] = name;

	OTE_LOGMSG("添加SubMesh. ResName : " << ResName << " submesh: " << name)
	OTE_TRACE("添加SubMesh. ResName : " << ResName << " submesh: " << name)

	return name;
}

// -------------------------------------------------
//删除SubMesh

void COTEActorEquipmentMgr::RemoveSubMesh(COTEActorEntity* ae, const std::string& EquipmentName)
{	
	const std::string& subEntName = COTEActorEquipmentMgr::GetSubEntityName(ae, EquipmentName);
	if(subEntName.empty())	
	{
		OTE_MSG("没有找到subentity! subname : " << EquipmentName, "失败")
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

		OTE_TRACE("删除SubMesh: " << EquipmentName)
		OTE_LOGMSG("删除SubMesh: " << EquipmentName)
	}
	else
	{
		OTE_MSG("没有找到SubMesh! subEntName : " << subEntName, "失败")
	}	
}

// -------------------------------------------------
//更改SubMeshName
void COTEActorEquipmentMgr::SetSubMeshName(COTEActorEntity* ae, const std::string& OldName, const std::string& NewName)
{
	Ogre::Mesh::SubMeshNameMap::iterator i = ae->getMesh()->mSubMeshNameMap.find(OldName) ;
	if (i == ae->getMesh()->mSubMeshNameMap.end())
	{
		OTE_MSG("该SubMesh不存在!", "错误")
		return;
	}

	Ogre::ushort n = i->second;
	ae->getMesh()->mSubMeshNameMap.erase(i);
	ae->getMesh()->nameSubMesh(NewName, n);

	OTE_TRACE("更改SubMeshName. OldName : " << OldName << " NewName: " << NewName)
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

	OTE_TRACE("GetSubEntity 没有找到装备 subName : " << EquipmentName)
	
	return NULL;
}
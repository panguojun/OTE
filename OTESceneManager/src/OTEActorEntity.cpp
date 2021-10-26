#include "OTEActorEntity.h"
#include "OTEQTSceneManager.h"
#include "OTEBinMeshSerializer.h"
#include "OgreMeshSerializer.h"
#include "OTEActionXmlLoader.h"
#include "OgreTagPoint.h"
#include "D3DToolbox.h"

#include "OTEObjDataManager.h"
#include "OTEEntityRender.h"

#include "OTEActorHookMgr.h"
#include "OTEActorActionMgr.h"
#include "OTEActorEquipmentMgr.h"
#include "OTEEntityPhysicsMgr.h"
#include "OTEReactorManager.h"
#include "OTED3DManager.h"
#include "OgreResourceBackgroundQueue_Win.h"

#include "tinyxml.h"

// -------------------------------------------------
using namespace Ogre;
using namespace OTE;

// -------------------------------------------------
COTEActorEntity::COTEActorEntity(const std::string& szName, Ogre::MeshPtr meshPtr, bool isCloneMesh) :
COTEEntity(szName, meshPtr),
m_pCurAnimState(NULL),
m_AnimSpeed(1.0f),
m_IsAutoPlay(true),
m_NeedRender(false),
m_CurAction(NULL),
m_pTrackOrgSceneNode(NULL),
m_pGPUReactor(NULL),
m_EquipmentNeedUpdate(true),
m_pObjData(NULL),
m_ProccessTicket(RESLOAD_BEGIN),
m_IsCloneMesh(isCloneMesh),
m_IsMeshExist(false)
{	
}

// -------------------------------------------------
COTEActorEntity::COTEActorEntity(bool isCloneMesh) :
COTEEntity(),
m_pCurAnimState(NULL),
m_AnimSpeed(1.0f),
m_IsAutoPlay(true),
m_NeedRender(false),
m_CurAction(NULL),
m_pTrackOrgSceneNode(NULL),
m_pGPUReactor(NULL),
m_EquipmentNeedUpdate(true),
m_pObjData(NULL),
m_IsCloneMesh(isCloneMesh),
m_IsMeshExist(false)
{	
	mAnimationState = NULL;
}

// -------------------------------------------------
COTEActorEntity::~COTEActorEntity()
{
	OTE_TRIGGER_EVENT("OnBeforeDestroy");

	if(m_CurAction)
		m_CurAction->Stop(false);
	
	// 清理挂点

	HashMap<std::string, Hook_t*>::iterator i = m_PotHookList.begin();
	while(i != m_PotHookList.end())
	{	
		//COTEActorHookMgr::DettachAllObjectsFromHook(this, i->first);
		delete i->second;
		i++;
	}
	m_PotHookList.clear();

	// 清理行动列表

	HashMap<std::string, COTEAction*>::iterator it = m_ActionList.begin();
	while(it != m_ActionList.end())
	{
		delete it->second;
		it++;
	}
	m_ActionList.clear();

	OTE_TRIGGER_EVENT("OnBeforeEnd");
}
// -------------------------------------------------
void COTEActorEntity::Init()
{
	SCENE_MGR->AddListener(COTEQTSceneManager::eBeforeRenderObjs, OnBeforeRenderObjs);
}
// -------------------------------------------------------------
// 动画
// -------------------------------------------------------------

void COTEActorEntity::SetCurrentAnim(const std::string& szAnimName)
{
	if(!hasSkeleton())	
		return;	

	Ogre::Animation::setDefaultInterpolationMode(Ogre::Animation::IM_LINEAR);
    Ogre::Animation::setDefaultRotationInterpolationMode(Ogre::Animation::RIM_LINEAR);

	// 动画复位
    EnalbeAnim(false);

	if(IsAnimationStatePresent(szAnimName))
	{
		m_pCurAnimState = getAnimationState(szAnimName);
		
		if(m_pCurAnimState)	
			m_pCurAnimState->setEnabled(true);	

//		OTE_TRACE(this->GetName() <<" 设置当前动画" << szAnimName)
	}
}

// -------------------------------------------------------------
void COTEActorEntity::EnalbeAnim(bool bEnable)
{
	if(!hasSkeleton())	
		return;

	if(m_pCurAnimState)
		m_pCurAnimState->setEnabled(bEnable);		
}
// -------------------------------------------------------------
bool COTEActorEntity::IsEnableAnim()
{
	return m_pCurAnimState && m_pCurAnimState->getEnabled();	
}

// -------------------------------------------------------------
// 创建/加载 相关
// -------------------------------------------------------------

void COTEActorEntity::WaitForLoadRes()
{	
	// 这种方法容易造成死锁

	/*while(m_ProccessTicket > RESLOAD_BEGIN &&
		!ResourceBackgroundQueue::GetSingleton()->isProcessComplete(m_ProccessTicket))
	{
		::Sleep(100);
	}
	
	SetupRes();*/
	
}
// -------------------------------------------------------------
// 创建角色
COTEActorEntity* COTEActorEntity::CreateActorEntity(const std::string& ObjFile,
													const std::string& name,
													bool isCloneMesh)
{		
	// OBJ

	ObjData_t* oin = COTEObjDataManager::GetInstance()->GetObjData(ObjFile);
	if(oin == NULL)
	{
		OTE_MSG("没有找到角色资源数据", "失败")
		return NULL;
	}	

	COTEActorEntity* ae = CreateActorEntity(oin, name, isCloneMesh);

	return ae;
}

COTEActorEntity* COTEActorEntity::CreateActorEntity(ObjData_t* oin,	const std::string& name, bool isCloneMesh)
{	
	// 创建实体

	COTEActorEntity* ae = new COTEActorEntity(isCloneMesh);
	ae->setName(name);
	ae->m_pObjData = oin;
	ae->m_ResFile = oin->MeshFile;

	ae->SetupRes();

	// 请求线程加载资源
	//
	////OTE_TRACE("开始加载角色实体: " << name << " ...")
	//if(oin->bHasMesh)
	//	ResourceBackgroundQueue::GetSingleton()->load(oin->MeshFile, 
	//	"Mesh", 
	//	Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
	//	NULL,
	//	ae
	//	);
	//if(oin->bHasSkeleton)
	//{
	//	ResourceBackgroundQueue::GetSingleton()->load(oin->SkeletonFile, "Skeleton", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, NULL);		
	//
	//}
	//ae->m_ProccessTicket = ResourceBackgroundQueue::GetSingleton()->mNextTicketID; // 最后一个ticket来判断是否加载完

	return ae;
}

// -------------------------------------------------------------
void COTEActorEntity::SetupRes()
{
	TIMECOST_START

	ObjData_t* oin = m_pObjData;
	COTEActorEntity* ae = this;

	// 加载Mesh

	Ogre::MeshPtr mesh;
	m_IsMeshExist = false;
	if(oin->bHasMesh) //  没有子mesh
	{
		mesh = Ogre::MeshManager::getSingleton().getByName(oin->MeshFile);
		if(mesh.isNull())
		{			
			mesh = Ogre::MeshManager::getSingleton().load(oin->MeshFile,
									"PakFileSystem"/*Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME*/);
			mesh.getPointer()->SetManualLoaded(true);			
		}
		m_IsMeshExist = true; 
	}
	else
	{
		mesh = Ogre::MeshManager::getSingleton().getByName(oin->MeshFile);
		if(mesh.isNull())
		{
			mesh = Ogre::MeshManager::getSingleton().create(oin->MeshFile,
									"PakFileSystem"/*Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME*/);

			mesh->setVertexBufferPolicy(Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, true);
			mesh->setIndexBufferPolicy (Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, true);				
			
			mesh->_setBounds(Ogre::AxisAlignedBox(-0.5f, 0, -0.5f, 0.5f, 2.0f, 0.5f), true);			
		}
	}	

	// 贴图

	int numSubMeshes = mesh->getNumSubMeshes();
    for (int i = 0; i < numSubMeshes; ++ i)
    {
		Ogre::SubMesh* subMesh = mesh->getSubMesh(i);
		if(!subMesh->TextureName.empty() &&
			Ogre::TextureManager::getSingleton().getByName(subMesh->TextureName).isNull())
		{
			Ogre::TextureManager::getSingleton().load(subMesh->TextureName, "PakFileSystem");
			OTE_TRACE("Load Texture: " << subMesh->TextureName)
		}

		subMesh->setMaterialName(MaterialManager::getSingleton().CreateMatFromType(subMesh->MaterialTypeName, subMesh->TextureName)->getName());
	}

	// assert(mesh->hasSkeleton() == oin->bHasSkeleton && "mesh文件格式必须与obj匹配");

	// 复制mesh

	if(m_IsCloneMesh)
		mesh = mesh->clone(mesh->getName() + this->getName());

	// 骨骼与动画
	
	if(oin->bHasSkeleton || mesh->hasSkeleton())
	{
		oin->bHasSkeleton = true;
		if(!mesh->getSkeletonName().empty())		
			oin->SkeletonFile = mesh->getSkeletonName();

		OTE_TRACE("骨骼文件: " << oin->SkeletonFile)
		if(oin->SkeletonFile.empty())
		{
			OTE_MSG("骨骼文件名为空!", "错误")
		}
		else
		{			
			SetupAnimations(mesh, oin->SkeletonFile, &oin->AniFileList);
		}
	}

	init(this->getName(), mesh);	


	if(!m_IsMeshExist)
	{
		// submeshes
		
		std::list<std::string>::iterator it = oin->SubMeshList.begin();
		while(it != oin->SubMeshList.end())
		{
			COTEActorEquipmentMgr::AddSubMesh(ae, (*it).c_str(), (*it).c_str());
			++ it;
		}
		
		mesh.getPointer()->SetManualLoaded(true);
	}	

	if(ae)
	{	
		// 更新装备

		COTEActorEquipmentMgr::SetupEquipments(ae, m_IsMeshExist);

		if(ae->m_pCurAnimState)
			ae->m_pCurAnimState->setEnabled(true);

		//加载挂点资源

		std::list<HookData_t*>::iterator ip = oin->HookList.begin();
		while(ip != oin->HookList.end())
		{
			HookData_t *pi = *ip;
			COTEActorHookMgr::AddHook(ae, pi->Name.c_str(), pi->BoneName.c_str(), pi->Position, pi->Roation, pi->Scale);
			ip++;
		}
		
		SetupEntityRes();	
	}

	// 包围盒更新
	if (mParentNode)
         mParentNode->needUpdate();
	
	//加载行动

	//COTEActionXmlLoader::GetSingleton()->ReadActionData(ae);	

	//OTE_TRACE("创建角色实体:  " << ae->GetName())

	m_ProccessTicket = RESLOAD_FINISHED;	

	//OTE_TRACE("加载角色实体: " << getName() << " 完成")

	// 加载完成时间
		
	OTE_TRIGGER_EVENT_NOLOG("OnResLoaded");

	TIMECOST_END
}

// --------------------------------------------------------
// 加载动画

void COTEActorEntity::SetupAnimations(const Ogre::MeshPtr mesh, const Ogre::String& skeletonFile, 
											std::map<std::string, std::string>* aniFileList)
{		
	// 加载骨骼		
		
	Ogre::SkeletonPtr skeleton = Ogre::SkeletonManager::getSingleton().getByName(skeletonFile);
	if(skeleton.isNull())
	{
		skeleton = Ogre::SkeletonManager::getSingleton().load(skeletonFile,
								Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		skeleton.getPointer()->ManualSetLoaded(true);
	}

	OTE_LOGMSG("完成加载 骨骼:  " << skeletonFile );

	// 加载动画

	std::map<std::string, std::string>::iterator ia = aniFileList->begin();
	while(ia != aniFileList->end())
	{
		if(skeleton->getAnimation(ia->first.c_str()) == NULL)
		{				
			Ogre::DataStreamPtr stream = 
				Ogre::ResourceGroupManager::getSingleton().openResource(ia->second.c_str());

			Ogre::SkeletonSerializer BINssr;
			BINssr.readAnimation(stream, skeleton.getPointer());

			OTE_LOGMSG("完成加载 动画:  " << ia->second );	
		}
		ia ++;
	}					
	
	mesh->_notifySkeleton(skeleton);
}


// --------------------------------------------------------
// SubEntity
// --------------------------------------------------------

COTESubEntity* COTEActorEntity::GetSubEntity(const Ogre::String& SubEntityName)
{
	COTEEntity::OTESubEntityList::iterator it = mSubEntityList.begin();
	while(it != mSubEntityList.end())
	{
		// 包含这个材质名

		if( (*it)->getMaterialName() == SubEntityName )
			return (COTESubEntity*)(*it);
		++ it;
	}

	OTE_TRACE_ERR("COTEActorEntity::GetSubEntity 失败! SubEntityName : " << SubEntityName)
	return NULL;
}

// --------------------------------------------------------
void COTEActorEntity::ClearSubEntityList()
{
	clearSubEntityList();	
}

// --------------------------------------------------------
void COTEActorEntity::BuildSubEntityList(MeshPtr& mesh, COTEEntity::OTESubEntityList* sublist, bool HasMesh)
{	 
	// Create SubEntities

	if(HasMesh)
	{
		int numSubMeshes = mesh->getNumSubMeshes();
		for (int i = 0; i < numSubMeshes; ++i)
		{
			SubMesh* subMesh = mesh->getSubMesh(i);		
			COTESubEntity* subEnt = new COTESubEntity(this, subMesh);		

			if (subMesh->isMatInitialised())
				subEnt->setMaterialName(subMesh->getMaterialName());
			sublist->push_back(subEnt);
		
			m_EquipmentMapEx[subMesh->getMaterialName()] = subMesh->getMaterialName();
		}
	}
	else
	{
		char subMeshName[64];

		HashMap<std::string, std::string>::iterator it = m_EquipmentMapEx.begin();
		while(it != m_EquipmentMapEx.end())
		{
			sscanf(it->second.c_str(), "%s", subMeshName);
			SubMesh* subMesh = mesh->getSubMesh(subMeshName);		

			COTESubEntity* subEnt = new COTESubEntity(this, subMesh);

			if (subMesh->isMatInitialised())
				subEnt->setMaterialName(it->second);

			sublist->push_back(subEnt);	

			OTE_TRACE("创建subentity. COTEEntity : " << this->GetName() << " Equipment : " << it->first <<  " SubEntity : " << it->second)

			++ it;
		}
	}
}

// -------------------------------------------------
// 材质相关
// -------------------------------------------------
void COTEActorEntity::SetMaterials(const MatEff_t& me)
{	
	std::vector<COTESubEntity*>::iterator ita = mSubEntityList.begin();

	while(ita != mSubEntityList.end())
	{
		// 如果已经存在

		std::list< std::pair<COTESubEntity*, Ogre::Material*> >::iterator it = m_MaterList.begin();	

		bool isHave = false;
		while (it != m_MaterList.end())
		{
			if( (*it).first == *ita )
			{
				isHave = true;
				break;
			}

			++ it;			
		}		

		Ogre::Material* mptr = (*ita)->getMaterial().getPointer();	

		std::stringstream ss;
		ss << getName() << "_BumpMat_" << m_MaterList.size();							

		if(!mptr->isLoaded())
			mptr->touch();

		Ogre::MaterialPtr mp = Ogre::MaterialManager::getSingleton().getByName(ss.str());
		if(mp.isNull())
		{
			mp = mptr->clone(ss.str());
			OTE_TRACE("Clone材质 : " << ss.str())
		}
		else
			mptr->copyDetailsTo(mp);

		if(!isHave)			
			m_MaterList.push_back(std::pair<COTESubEntity*, Ogre::Material*>(*ita, mptr));

		(*ita)->setMaterialName(ss.str());

		OTE_TRACE("Clone 材质: " << mptr->getName() << "->" << mp->getName())

		mp->setLightingEnabled(true);		
		
		mp->setAmbient(me.Ambient);
		mp->setDiffuse(me.Diffuse);			

		mp->setDepthWriteEnabled(me.DepthWriteEnabled);

		++ ita;
	}
}

	
// -------------------------------------------------
// 重置材质

void COTEActorEntity::ResetMaterialChange()
{
	std::list< std::pair<COTESubEntity*, Ogre::Material*> >::iterator it = m_MaterList.begin();	
	while (it != m_MaterList.end())
	{
		(*it).first->setMaterialName( (*it).second->getName() );
		++ it;			
	}
	m_MaterList.clear();

	OTE_TRACE("重置材质")
}

// -------------------------------------------------
// 更新
void COTEActorEntity::_notifyCurrentCamera(Ogre::Camera* cam)
{
	// 装备更新 多线程技术

	/*if(m_ProccessTicket > 0 &&
		ResourceBackgroundQueue::GetSingleton()->isProcessComplete(m_ProccessTicket))*/
	{
		//SetupRes();
		if(m_EquipmentNeedUpdate)
		{
			COTEActorEquipmentMgr::SetupEquipments(this, m_IsMeshExist);
		}
	}
	/*if(m_EquipmentNeedUpdate)
	{
		COTEActorEquipmentMgr::UpdateEquipments(this);
	}*/

	// 状态更新

	//if(m_ProccessTicket == RESLOAD_FINISHED/* && !getMesh()->isManuallyLoaded()*/)
		UpdateState(COTED3DManager::s_FrameTime);

	m_NeedRender = true;
}

//// ----------------------------------------------------
//void COTEActorEntity::_updateRenderQueue(Ogre::RenderQueue *queue)
//{
//	const Ogre::Vector3& camPos = SCENE_CAM->getPosition();
//	if(camPos - GetParentSceneNode()->getPosition()).squaredLength() < 20000)
//	{
//		COTEEntity::_updateRenderQueue(queue);
//	}
//
//}

// -------------------------------------------------
//更新实体状态

bool COTEActorEntity::UpdateState(float fDtime)
{	
	if(!m_IsAutoPlay || !m_NeedRender)
		return false;	

	m_NeedRender = false;	

	// 反应器更新

	if(m_BindReactor && !(((CReactorBase*)m_BindReactor)->m_StateFlag & 0xFF000000))
	{
		((CReactorBase*)m_BindReactor)->Update(this, fDtime);
	}

	// 动画播放

	if(m_pCurAnimState && m_pCurAnimState->getEnabled())
	{
		m_pCurAnimState->addTime(fDtime * m_AnimSpeed);	
	}

	// 动作更新

	bool ret = false;

	if(m_CurAction)
	{
		ret = (m_CurAction->Proccess(fDtime) == COTEAction::eSTOP);
		if(ret)
		{
			if (!m_SolidList.empty())
			{
				std::list<ActionPlay_t>::iterator it = m_SolidList.begin();
				COTEActorActionMgr::SetAction(this, it->first.c_str(), it->second);

				if (it->second != COTEAction::eCONTINUES)
					ret = false;
				m_SolidList.erase(it);
				if (ret)
					m_bSolidEnd = true;
			}
		}				
	}	

	// 更新

	HashMap<std::string, Hook_t*>::iterator i = m_PotHookList.begin();
	while(i != m_PotHookList.end())
	{			
		COTEActorHookMgr::UpdateHook(this, i->first);
		i++;
	}

	return ret;
}
// -------------------------------------------------
// 在渲染之前更新

void COTEActorEntity::OnBeforeRenderObjs(int ud)
{
	// 更新物件 及其挂点

	std::map<std::string, COTEEntity*>::iterator i = SCENE_MGR->m_EntityList.begin();
	std::map<std::string, COTEEntity*>::iterator iEnd = SCENE_MGR->m_EntityList.end();

	for (; i != iEnd; ++i)
	{		
		if(((COTEActorEntity*)i->second)->m_NeedRender)
			((COTEActorEntity*)i->second)->UpdateState(COTED3DManager::s_FrameTime);	
	}
}

// -------------------------------------------------
// 清理场景

void COTEActorEntity::Clear()
{
	COTEEntityPhysicsMgr::ReleaseIntersectionCach();

	// 清理正在播放的行动

	std::map<std::string, COTEEntity*>::iterator i = SCENE_MGR->m_EntityList.begin();
	std::map<std::string, COTEEntity*>::iterator iEnd = SCENE_MGR->m_EntityList.end();

	for (; i != iEnd; ++i)
	{
		Ogre::Mesh* ptr = ((COTEEntity*)i->second)->getMesh().getPointer();
		if(ptr && !ptr->isManuallyLoaded())
		{	
			COTEActorEntity* ae = (COTEActorEntity*)i->second;
			if(ae->m_CurAction)
			{
				ae->m_CurAction->Stop(false);
				ae->m_CurAction = NULL; // 设置为空

				COTEActorActionMgr::SetAction(ae, "");
			}
			ae->m_pCurAnimState = NULL;
		}
			
	}

	OTE_TRACE("COTEActorEntity::OnSceneClear")

}

// ====================================================
// COTEEntityFactory
// ====================================================

Ogre::MovableObject* COTEEntityFactory::Create(const std::string& ResName,	const std::string& Name, bool cloneMat)
{
	Ogre::MovableObject* mo = SCENE_MGR->CreateEntity(
											ResName, Name, 
											Ogre::Vector3::ZERO,
											Ogre::Quaternion::IDENTITY,
											Ogre::Vector3::UNIT_SCALE,
											"default",
											false
											);

	if(cloneMat)
	{
		((COTEActorEntity*)mo)->CloneMaterials();
	}
	return mo;
}

// ----------------------------------------------------
void COTEEntityFactory::Delete(const std::string& Name)
{
	SCENE_MGR->RemoveEntity(Name);
}

// =============================================
// COTEEntityBoneRender
// =============================================

Ogre::String	COTEEntityBoneRender::s_SelectedBoneName;
COTEActorEntity*	COTEEntityBoneRender::s_CurrentActorEntity = NULL;
void COTEEntityBoneRender::SetSelectedBoneName(const Ogre::String& boneName)
{
	s_SelectedBoneName = boneName;
}

// 渲染角色骨胳
void COTEEntityBoneRender::RenderEntityBones(COTEActorEntity* ae)
{
	assert(ae);	
	if(ae->hasSkeleton())
	{
		s_CurrentActorEntity = ae;
		RenderBones(ae->getSkeleton()->getRootBone()->getChildIterator());	
	}
}

// ----------------------------------------------
// 渲染骨胳
void COTEEntityBoneRender::RenderBoneNode(Ogre::Node* boneNode)
{
	Ogre::Camera* cam = SCENE_CAM;

	// ----------------------------------------------------
	// 保存现场

	// 矩阵
	
	D3DXMATRIX matrix, oldWorldMat, oldViewMat, oldPrjMat;
	g_pDevice->GetTransform(D3DTS_WORLD,		&oldWorldMat); // SAVE TRANSFORM
	g_pDevice->GetTransform(D3DTS_VIEW,			&oldViewMat); 
	g_pDevice->GetTransform(D3DTS_PROJECTION ,	&oldPrjMat); 

	// 设置世界矩阵到默认

	D3DXMatrixIdentity(&matrix);
	g_pDevice->SetTransform(D3DTS_WORLD,		&matrix);

	// ----------------------------------------------------
	// 设置渲染状态

	g_pDevice->SetVertexShader(0);
	g_pDevice->SetPixelShader(0);

	g_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	/*
	g_pDevice->SetRenderState( D3DRS_AMBIENT, FALSE );
	g_pDevice->SetRenderState( D3DRS_COLORVERTEX, FALSE );*/

	// ZBuffer
	g_pDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	
	// ----------------------------------------------------
	// DRAW

	DWORD tColor = 0xFF808FFF;	

	if(s_SelectedBoneName == boneNode->getName())
	{
		tColor = 0xFFFFFF00;
	}
	
	Ogre::Vector3 pos = s_CurrentActorEntity->getParentSceneNode()->getWorldPosition() +
		s_CurrentActorEntity->getParentSceneNode()->getOrientation() * boneNode->getWorldPosition();

	DrawWireBall(g_pDevice, D3DXVECTOR3(pos.x, pos.y, pos.z), BONE_SHOW_SIZE, tColor);	
	
	// ----------------------------------------------------
	// 还原现场
	
	g_pDevice->SetTransform( D3DTS_WORLD,			&oldWorldMat);
	g_pDevice->SetTransform( D3DTS_VIEW,			&oldViewMat); 
	g_pDevice->SetTransform( D3DTS_PROJECTION,		&oldPrjMat); 
	
}

// ----------------------------------------------
void COTEEntityBoneRender::RenderBones(Ogre::Node::ChildNodeIterator it)
{
	while (it.hasMoreElements())
	{	
		Ogre::Node* n = it.getNext();

		if(n->getName().find("Unnamed") != -1)
			continue;

		RenderBoneNode(n);

		RenderBones(n->getChildIterator());
	}
}

// =============================================
// COTEEntityBonePicker
// =============================================
COTEActorEntity*		COTEEntityBonePicker::s_CurrentActorEntity = NULL;
Node* COTEEntityBonePicker::PickEntityBones(COTEActorEntity* ae, const Ogre::Ray* ray)
{
	s_CurrentActorEntity = ae;
	return PickBones(ae->getSkeleton()->getRootBone()->getChildIterator(), ray);
}

// ----------------------------------------------
Node* COTEEntityBonePicker::PickBones(Ogre::Node::ChildNodeIterator it, const Ogre::Ray* ray)
{
	while (it.hasMoreElements())
	{	
		Ogre::Node* n = it.getNext();

		if(n->getName().find("Unnamed") != -1)
			continue;

		if(PickBoneNode(n, ray))
			return n;

		Ogre::Node* pickNode = PickBones(n->getChildIterator(), ray);
		if(pickNode)
			return pickNode;

	}

	return NULL;
}

// ----------------------------------------------
bool COTEEntityBonePicker::PickBoneNode(Ogre::Node* boneNode, const Ogre::Ray* ray)
{
	Ogre::Vector3 pos = s_CurrentActorEntity->getParentSceneNode()->getWorldPosition() + s_CurrentActorEntity->getParentSceneNode()->getOrientation() * boneNode->getWorldPosition();

	if(ray->intersects(Ogre::Sphere(pos, BONE_SHOW_SIZE)).first)
	{
		return true;
	}
	return false;
}


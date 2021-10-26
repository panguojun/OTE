#include "OTEQTSceneManager.h"
#include "OTEObjDataManager.h"
#include "OTECollisionManager.h"
#include "OTETerrainserializer.h"
#include "OTEMagicManager.h"
#include "OTETrackXmlLoader.h"
#include "OTETrackManager.h"
#include "OTEActionXmlLoader.h"
#include "otevegmanager.h"
#include "OgreD3D9RenderSystem.h"
#include "OTEPluginManager.h"
#include "OTEFMODSystem.h"
#include "OTERenderLight.h"
#include "OTEWater.h"
#include "OTEBillBoardSet.h"
#include "OTEParticlesystem.h"
#include "OTED3DManager.h"
#include "OTEReactorManager.h"
#include "OTEBillBoardChain.h"
#include "OgreResourceBackgroundQueue_Win.h"
#include "OTEShadowRender.h"
#include "OTESky.h"
#include "OTEGrass.h"

#include "DymMagicToOTE.h"
#include "MagicEntity.h"

using namespace Ogre;
namespace OTE
{

// ********************************************
// CFrameListener
// ********************************************
class CFrameListener : public Ogre::FrameListener
{
public:
	static CFrameListener* s_pInst;

	CFrameListener()
	{
		m_IsDeviceLost = false;
	}
	virtual ~CFrameListener(){}

	bool m_IsDeviceLost;

public:	 

	void showDebugOverlay(bool show){}

	// --------------------------------------------
	bool frameStarted(const Ogre::FrameEvent& evt)
	{		
		// 这里防止设备丢失后

		g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, true );

		m_IsDeviceLost = ( (Ogre::D3D9RenderSystem*)(Ogre::Root::getSingleton().getRenderSystem( ) ) )->isDeviceLost();
		if(m_IsDeviceLost) // 设备丢失
		{			
			std::list<COTEQTSceneManager::InvalidDeviceCB_t>* pList = &SCENE_MGR->m_InvalidDeviceCBList;
			std::list<COTEQTSceneManager::InvalidDeviceCB_t>::iterator it = pList->begin();
			while(it != pList->end())
			{
				(*(*it))();

				++ it;
			}
		}	
		
		static float sEspTime = 0.0f;
		sEspTime += COTED3DManager::s_FrameTime;
		g_dymMagicToOTE.FrameMove(sEspTime);

		return true;
	}

	// --------------------------------------------
	bool frameEnded(const Ogre::FrameEvent& evt)
	{		
#   ifdef GAME_DEBUG

		char buff[32];

		sprintf(buff, "[FPS]: %.2f", 1.0f / evt.timeSinceLastFrame);

		SCENE_MGR->m_RenderInfoList["FPS"] = buff;

		HashMap<std::string, std::string>*	list = &SCENE_MGR->m_RenderInfoList;
		HashMap<std::string, std::string>::iterator it = list->begin();

		std::stringstream ss;

		while(it != list->end())
		{
			ss << it->second << " ";

			it ++;
		}

		//Ogre::RenderWindow* rw = (Ogre::RenderWindow*)Ogre::Root::getSingleton().getRenderTarget("OTE RenderWindow");
		//ss << " Tris = " << rw->getTriangleCount() << " Ents = " << SCENE_MGR->GetEntityCount();

		

		//::SetWindowText(OTE::COTEPluginManager::GetInstance()->m_hMainWnd, ss.str().c_str());

#   endif	

		// 更新　D3DEffRender
		
		COTED3DManager::s_FrameTime = evt.timeSinceLastFrame;
		COTED3DManager::s_ElapsedTime += COTED3DManager::s_FrameTime;

		//// 更新D3D矩阵	

		//COTED3DManager::_notifyUpdater();

		return true;

	}

};

CFrameListener* CFrameListener::s_pInst = NULL;


//	==============================================
//  COTEQTSceneManager
//  基于四叉树的场景管理器实现
//	==============================================

static int s_EntityCount = 0;		// 实体计数器 

COTEQTSceneManager::COTEQTSceneManager(const Ogre::AxisAlignedBox &box, int depth) 
:  CQuadTreeSceneManager(box, depth),
m_IsEditor(false), 
m_SelMovableObject(NULL),
m_MainCamera(NULL),
m_MainLight(NULL)
{		
}

COTEQTSceneManager::~COTEQTSceneManager(void)
{
	OTE_TRACE("============================================")
	OTE_TRACE("OTE 摧毁场景管理器")
	OTE_TRACE("============================================")

	OTE_LOGMSG("============================================")
	OTE_LOGMSG("OTE 摧毁场景管理器")
	OTE_LOGMSG("============================================")

	/// 清理作用单体对象

	COTEReactorManager::DestroySingleton();

	COTEActionXmlLoader::DestroySingleton();

	// 清理地形

	COTETilePage::DestroyAllPages();

	/// 清理单体对象	

	COTEMagicManager::DestroySingleton();	
	
	COTEGrass::DestroySingleton(); // 草

	/// 清理	

	COTERenderLight::Destroy();

	COTEObjDataManager::Destroy();	

	COTETrackManager::Destroy(); 

	COTEActTrackUsage::Destroy();

	COTEVegManager::Destroy(); // 草

	COTELiquidManager::DestroySingleton(); // 水效果

	COTEFMODSystem::s_Inst.Clear(); // 声音

	SAFE_DELETE(CFrameListener::s_pInst)

	COTERenderTexture::Destory();	

	// 多线程

	ResourceBackgroundQueue::DestroySingleton();

	COTERunTimeAnalytics::Destory();
}

//	----------------------------------------------
Ogre::SceneNode* COTEQTSceneManager::GetSceneNode(const Ogre::String& strName)
{		
	SceneNodeList::const_iterator i = mSceneNodes.find(strName);

	if (i == mSceneNodes.end())
	{
		return NULL;
	}

	return i->second;
}


//	----------------------------------------------
Ogre::Camera* COTEQTSceneManager::createCamera( const Ogre::String &name )
{	
	return CQuadTreeSceneManager::createCamera( name );
}

//	----------------------------------------------
void COTEQTSceneManager::_renderScene(Ogre::Camera *cam, Ogre::Viewport *vp, bool includeOverlays)
{
if(::GetKeyState(VK_F11) & 0x80)
{
OTE_TRACE("**** render start ... ****")
}

	// 处理设备丢失

	if(CFrameListener::s_pInst && CFrameListener::s_pInst->m_IsDeviceLost)
	{
		std::list<RestoreDeviceCB_t>* pList = &m_RestoreDeviceCBList;
		std::list<RestoreDeviceCB_t>::iterator it = pList->begin();
		while(it != pList->end())
		{
			(*(*it))();

			++ it;
		}		
	}	
	
	// 更新场景光

	COTERenderLight::GetInstance()->AdvancedFrameTime(COTED3DManager::s_FrameTime);
	
	Ogre::SceneManager::_renderScene(cam, vp, includeOverlays);		

	g_dymMagicToOTE.Render(cam);

	// 滞后删除

	_UpdateGrave();	

if(::GetKeyState(VK_F11) & 0x80)
{
OTE_TRACE("**** render end ****")
}

}

//	----------------------------------------------
void COTEQTSceneManager::_TestEventCB(int Event, int UD)
{		
	HashMap<int, RenderListenerList_t>::iterator it = m_EventListeners.find(Event);

	if(it != m_EventListeners.end())
	{	
		RenderListenerList_t::iterator ita = it->second.begin();		

		while(ita != it->second.end())
		{				
			(*(*ita))(UD);	

			++ ita;
		}	
	}
}

//	----------------------------------------------
void COTEQTSceneManager::_renderVisibleObjects(Ogre::Camera *cam)
{	
	// 更新

	COTED3DManager::_notifyUpdater();

	_TestEventCB(COTEQTSceneManager::eBeforeRenderObjs, int(cam));

	Ogre::SceneManager::_renderVisibleObjects(cam);	

	_TestEventCB(COTEQTSceneManager::eEndRenderObjs,	int(cam));

	// 渲染回调

	if(cam == SCENE_CAM)
	{				
		_TestEventCB(COTEQTSceneManager::eRenderCB, 0);				
	}	
}

//	----------------------------------------------
// Listener
//	----------------------------------------------
void COTEQTSceneManager::AddListener(int key, EventListener_t pListenFun)
{	
	HashMap<int, RenderListenerList_t>::iterator it = m_EventListeners.find(key);
	if(it == m_EventListeners.end())
	{
		m_EventListeners[key] = RenderListenerList_t();
	}
	else
	{
		RenderListenerList_t& rl = it->second;

		RenderListenerList_t::iterator ita = rl.begin();
		while(ita != rl.end())
		{
			if(*ita == pListenFun)
			{					
				return;
			}

			++ ita;
		}
	}

	m_EventListeners[key].push_back(pListenFun);

	OTE_TRACE("添加引擎回调函数 : keyName = " << key << " 地址 = " << (int)pListenFun)

	OTE_LOGMSG("添加引擎回调函数 : keyName = " << key << " 地址 = " << (int)pListenFun)

}

//	----------------------------------------------
void COTEQTSceneManager::RemoveListener(EventListener_t pListenFun)
{
	HashMap<int, RenderListenerList_t>::iterator it = m_EventListeners.begin();
	while(it != m_EventListeners.end())
	{
		RenderListenerList_t& rl = it->second;

		RenderListenerList_t::iterator ita = rl.begin();
		while(ita != rl.end())
		{
			if(*ita == pListenFun)
			{
				OTE_TRACE("移除引擎回调函数 : keyName = " << it->first << " 地址 = " << (int)pListenFun)
				
				OTE_LOGMSG("移除引擎回调函数 : keyName = " << it->first << " 地址 = " << (int)pListenFun)
			
				rl.erase(ita);
				break;
			}

			++ ita;
		}
		++ it;
	}	
}

//	----------------------------------------------
//	Entity
//	----------------------------------------------

COTEActorEntity* COTEQTSceneManager::CreateEntity(	const std::string& File, 
													const std::string& Name,
													const Ogre::Vector3& rPosition, 
													const Ogre::Quaternion& rQuaternion,
													const Ogre::Vector3& rScaling, 
													const std::string& group,
													bool  vAttachToSceneNode,
													bool  isCloneMesh)
{	
	OTE_TRACE("创建实体: " << Name)
	//OTE_LOGMSG("创建实体" << Name)

	std::map<Ogre::String, COTEEntity* >::iterator it = m_EntityList.find( Name );
    if( it == m_EntityList.end() )
    {
		Ogre::String fileName;
		if(File.find(".obj") != std::string::npos)
		{
			fileName = File;
		}
		else if(File.find(".mesh") != std::string::npos)
		{
			fileName = File;

			fileName.replace(fileName.find(".mesh"), 5, ".obj");  
		}
		else
		{
			OGRE_EXCEPT(Ogre::Exception::ERR_RT_ASSERTION_FAILED, 
                "文件类型不支持.",
                "COTEQTSceneManager::CreateEntity");
		}

		COTEActorEntity* e = COTEActorEntity::CreateActorEntity(fileName, Name, isCloneMesh);

		if(e == NULL)
		{
			OTE_LOGMSG("在创建实体" << Name << "时出错，文件名为" << File)		

			return NULL;
		}	

		// 场景中物件节点被分成几个组 以便管理
		if(vAttachToSceneNode)
		{			
			AttachObjToSceneNode(e, group);	
		
			/// 节点位置

			e->GetParentSceneNode()->setPosition(rPosition);	
			e->GetParentSceneNode()->setOrientation(rQuaternion);		
			e->GetParentSceneNode()->setScale(rScaling);				
		}	
		m_EntityList[Name] = e;			

		s_EntityCount ++;	// 无论如何把实体计数累加

		return e;
	}

	OTE_TRACE("场景中已经存在同名实体: " << Name)

	return (COTEActorEntity*)it->second;
}

//	----------------------------------------------
COTEActorEntity* COTEQTSceneManager::AutoCreateEntity(const std::string& ResName, const std::string& Type, bool isCloneMesh)
{	
	std::stringstream ss;

	if(!COTETilePage::s_CurrentPage)
		ss << Type << "_" << ResName << "_" << s_EntityCount ++;
	else
		ss << Type << "_" << COTETilePage::s_CurrentPage->m_PageName << "_" << ResName << "_" << s_EntityCount ++;

	return CreateEntity(
							ResName, ss.str(),
							Ogre::Vector3::ZERO,
							Ogre::Quaternion::IDENTITY,
							Ogre::Vector3::UNIT_SCALE,
							"default",
							true,
							isCloneMesh
							);
}

//	----------------------------------------------
void COTEQTSceneManager::RemoveEntity(const OTE::COTEEntity* pEntity, bool Immediately)
{
	if(m_SelMovableObject == pEntity)
		m_SelMovableObject = NULL;

	m_GraveList.push_back(pEntity->getName());

	if(Immediately)
		_UpdateGrave();
}

//	----------------------------------------------
void COTEQTSceneManager::RemoveEntity(const Ogre::String& rName, bool Immediately)
{
	if(m_SelMovableObject && m_SelMovableObject->getName() == rName)
		m_SelMovableObject = NULL;

	m_GraveList.push_back(rName);

	if(Immediately)
		_UpdateGrave();
}

// --------------------------------------------------------
// 更新坟墓列表(真正的删除操作)

void COTEQTSceneManager::_UpdateGrave()
{
	std::list<std::string>::iterator it = m_GraveList.begin();
	while(it != m_GraveList.end())
	{
		const std::string& rName = *it;

		std::map<Ogre::String, COTEEntity* >::iterator i = m_EntityList.find(rName);
		if (i != m_EntityList.end())
		{	
			OTE_TRACE("移除实体" << rName)

			Ogre::SceneNode* sn = i->second->getParentSceneNode();
			delete (i->second);

			if(sn)
			{
				sn->getParentSceneNode()->removeAndDestroyChild(sn->getName());	
			}
			
			m_EntityList.erase(i);
		}

		m_GraveList.erase(it);
		it = m_GraveList.begin();
	}
}

//	----------------------------------------------
//清除所有实体
void COTEQTSceneManager::RemoveAllEntities()
{
	// 选中物体
	m_SelMovableObject = NULL;	

	// 移除静态实体

	std::map<std::string, COTEEntity*>::iterator i = m_EntityList.begin();
	std::map<std::string, COTEEntity*>::iterator iEnd = m_EntityList.end();

	for (; i != iEnd; ++i)
	{
		Ogre::SceneNode* sn = i->second->getParentSceneNode();
		delete (i->second);

		if(sn)
		{
			sn->getParentSceneNode()->removeAndDestroyChild(sn->getName());	
		}			
	}
	m_EntityList.clear();

	OTE_TRACE("移除所有实体")

	OTE_LOGMSG("移除所有实体")
}

//	----------------------------------------------
void COTEQTSceneManager::GetEntityList(std::vector<Ogre::MovableObject*>* elist)
{
	std::map<std::string, COTEEntity*>::iterator i = m_EntityList.begin();
	std::map<std::string, COTEEntity*>::iterator iEnd = m_EntityList.end();

	for (; i != iEnd; ++i)
	{
		if(((Ogre::MovableObject*)i->second)->isInScene())
		{
			elist->push_back((Ogre::MovableObject*)(i->second));
		}			
	}
}	

//	----------------------------------------------
void COTEQTSceneManager::SelectAllEntities(bool isSelect)
{
	std::map<std::string, COTEEntity*>::iterator i = m_EntityList.begin();
	std::map<std::string, COTEEntity*>::iterator iEnd = m_EntityList.end();

	for (; i != iEnd; ++i)
	{
		if(((Ogre::MovableObject*)i->second)->isInScene())
		{
			((Ogre::SceneNode*)i->second->getParentNode())->showBoundingBox(isSelect);
		}
	}
}

//	----------------------------------------------
COTEActorEntity* COTEQTSceneManager::SelectEntity(const char* strName, bool isSelect)
{
	COTEActorEntity* e = (COTEActorEntity*)GetEntity(strName);
	if(e != NULL)
		((Ogre::SceneNode*)e->getParentNode())->showBoundingBox(isSelect);
	
	if(isSelect)
	{
		SetCurFocusObj(e);		
	}
	else
	{
		ClearCurFocusObj();
	}		

	return e;
}

// -----------------------------------------------
// 显示/隐藏物体
void COTEQTSceneManager::ShowEntities(bool isVisible, const std::string& group)
{
	if(group == "all") 
	{
		std::map<std::string, COTEEntity*>::iterator i = m_EntityList.begin();
		std::map<std::string, COTEEntity*>::iterator iEnd = m_EntityList.end();

		for (; i != iEnd; ++i)
		{
			if(((Ogre::MovableObject*)i->second)->isInScene())
			{
				((Ogre::SceneNode*)i->second->getParentNode())->setVisible(isVisible);
			}
		}
	}
	else
	{
		// 分组

		if(m_EntSNRootMap[group])
		{
			m_EntSNRootMap[group]->setVisible(isVisible);	

		}
	}
}


//	----------------------------------------------
// 公告牌
//	----------------------------------------------

Ogre::BillboardSet* COTEQTSceneManager::CreateBillBoardSet(const std::string& name, int poolSize)
{			  
	// Check name not used
    if (mBillboardSets.find(name) != mBillboardSets.end())
    {
        OTE_MSG( "Billboard " << name << " 已经存在！", "失败")
		return NULL;
    }

	OTE::COTEBillBoardSet* bbs = new OTE::COTEBillBoardSet( name, poolSize );	

    mBillboardSets[name] = bbs;

	//OTE_TRACE("CreateBillBoardSet : " << name)

    return bbs;
}

//	----------------------------------------------
void COTEQTSceneManager::DeleteBillBoardSet(const std::string& name)
{
	Ogre::BillboardSet* bbs = getBillboardSet(name);

	DetachObjAndDestroyNode(bbs);

	removeBillboardSet(bbs);

	// 删除材质

	if(Ogre::MaterialManager::getSingleton().resourceExists(name))
	{
		Ogre::MaterialManager::getSingleton().remove(name);
		//OTE_TRACE("移除材质 : " << name)
	}

	//OTE_TRACE("DeleteBillBoardSet : " << name)
}


//	----------------------------------------------
//  场景结点 邦定/删除
//	----------------------------------------------
void COTEQTSceneManager::AttachObjToSceneNode(Ogre::MovableObject* MO, const std::string& Group)
{
	// 创建绑定场景结点

	AttachObjToSceneNode(MO, MO->getName(), Group);	
}
//	----------------------------------------------
void COTEQTSceneManager::AttachObjToSceneNode(Ogre::MovableObject* MO, const std::string& rNodeName, const std::string& Group)
{	
	if(MO->isAttached())
	{
		OTE_TRACE_WARNING("AttachObjToSceneNode : " << MO->getName() << ", 已经帮定！")
		return ;
	}

	Ogre::SceneNode* root = m_EntSNRootMap[Group];
	if(!root)
	{	
		root = (Ogre::SceneNode*)getRootSceneNode()->createChildSceneNode(Group + "_root");
		m_EntSNRootMap[Group] = root;		
	}

	Ogre::SceneNode* sn = root->createChildSceneNode(rNodeName);
	sn->attachObject(MO);
	//OTE_TRACE("AttachObjToSceneNode : " << MO->getName())
}

//	----------------------------------------------
void COTEQTSceneManager::DetachObjAndDestroyNode(Ogre::MovableObject* MO)
{
	//OTE_TRACE("DetachObjAndDestroyNode : " << MO->getName())

	Ogre::SceneNode* sn = MO->getParentSceneNode();			  

	if(sn)
	{			
		sn->getParentSceneNode()->removeChild(sn);

		destroySceneNode(sn->getName());	
	}	
}

//	----------------------------------------------
void COTEQTSceneManager::WalkEntities(void(*pCBFun)(Ogre::MovableObject*), const std::string& IgnoreKeyName)
{
	if(!pCBFun)
		return;

	std::vector<Ogre::MovableObject*> eList;
	GetEntityList(&eList);

	std::vector<Ogre::MovableObject*>::iterator it = eList.begin();
	while(it != eList.end())
	{			
		Ogre::MovableObject* e = (*it);
		if(	e->getName().find("sky_") != std::string::npos ||
			(!IgnoreKeyName.empty() && e->getName().find(IgnoreKeyName))
			)
		{
			++ it;
			continue;
		}		
		
		WAIT_LOADING_RES(e)
		
		(*pCBFun)(e);

		++ it;
	}	
}

//	----------------------------------------------
// world / scene
//	----------------------------------------------
void COTEQTSceneManager::InitWorld()
{
	OTE_TRACE("============================================")
	OTE_TRACE("OTE 初始化")
	OTE_TRACE("============================================")

	// 场景的大小

	Resize(Ogre::AxisAlignedBox( -256, -50, -256, 256, 500, 256 ) );

	// Device

	g_pDevice = ( (Ogre::D3D9RenderSystem*)(Ogre::Root::getSingleton().getRenderSystem( ) ) )->mpD3DDevice;

	// 声音系统初始化

	COTEFMODSystem::s_Inst.InitSystem();	

	// 桢监听器　FrameListener

	CFrameListener::s_pInst = new CFrameListener();

	Ogre::Root::getSingleton().addFrameListener(CFrameListener::s_pInst);

#ifndef __ZG

	// 天空盒

	COTESky::GetInstance()->Create();

#endif

	// 光

	COTERenderLight::GetInstance()->CreateLights();	

	//加载行动

	COTEActionXmlLoader::GetSingleton()->ReadActionData();	

	COTERunTimeAnalytics::Init();

	COTEActorEntity::Init();

	// 特效
	
	g_dymMagicToOTE.init(g_pDevice);
	
}

//	----------------------------------------------
void COTEQTSceneManager::clearScene(void)
{		
	// 缓存

	m_MainCamera = NULL;

	m_MainLight = NULL;

	// 回调

	_TestEventCB(COTEQTSceneManager::eSceneClear, 0);

	m_SelMovableObject = NULL;	// 清理场景

	COTEActorEntity::Clear();	

	COTEMagicManager::Clear();				

	COTERenderLight::Clear();	

	COTETilePage::DestroyAllPages();

	COTEAutoEntityMgr::Clear();

	RemoveAllEntities(); // 实体

	// 公告牌儿	

	removeAllBillboardSets();	
	
	// 粒子系统

	COTEParticleSystemManager::Clear();

	COTEBillBoardChain::Clear();

	// 特效

	COTEMagicEntity::DeleteAllMagics();

	// 清理场景节点

	m_EntSNRootMap.clear();

	getRootSceneNode()->removeAndDestroyAllChildren();

	CQuadTreeSceneManager::clearScene();

	// mesh

	Ogre::MeshManager::getSingleton().unloadAll();	

	Ogre::MeshManager::getSingleton().removeAll();	

	// 特效清理 TODO
	
	OTE_TRACE("--------------------------------------------")
	OTE_TRACE("OTE 清理场景完成")
	OTE_TRACE("--------------------------------------------")	
}


}
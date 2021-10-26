#pragma once

#include "OTEStaticInclude.h"
#include "OTEQuadtreeSceneManager.h"
#include "OTETilePage.h"
#include "OTEActorEntity.h"
#include "OTERunTimeAnalytics.h"
#include "MagicEntity.h"

namespace OTE
{

// ***********************************************
// COTEQTSceneManager
// 四叉树场景管理器的应用版本
// ***********************************************

class _OTEExport COTEQTSceneManager : public CQuadTreeSceneManager
{
friend class COTEActorEntity;
friend class CFrameListener;

public:

	// 事件类型

	enum{eBeforeRenderObjs, eEndRenderObjs, eRenderCB, eSceneClear, eSelectEntity, eUnSelectEntity};

	typedef void(*EventListener_t)(int UD);		// 场景事件回调函数

	typedef void(*InvalidDeviceCB_t)();			// 全屏问题之 清理回调函数
	typedef void(*RestoreDeviceCB_t)();			// 全屏问题之 重新创建回调函数

public:
	COTEQTSceneManager(
		const Ogre::AxisAlignedBox &box = Ogre::AxisAlignedBox( -256, -10, -256, 256, 100, 256 ), 
		int depth = 3);

	virtual ~COTEQTSceneManager(void);

public:

	// 初始化场景　

	void InitWorld();

	Ogre::RenderSystem* GetRenderSystem()
	{
		return mDestRenderSystem;
	}
	
	// 渲染
	virtual void _renderScene(Ogre::Camera *cam, Ogre::Viewport *vp, bool includeOverlays);
	
	// 摄像机
	virtual Ogre::Camera* createCamera( const Ogre::String &name );		

	// 清理	

	virtual void clearScene(void);	

	virtual void _renderVisibleObjects(Ogre::Camera *cam);

public:

	/// 实体的管理

	COTEActorEntity* CreateEntity(
		const Ogre::String& File, const Ogre::String& Name,
		const Ogre::Vector3& rPosition = Ogre::Vector3::ZERO,
		const Ogre::Quaternion& rQuaternion = Ogre::Quaternion::IDENTITY,
		const Ogre::Vector3& rScaling = Ogre::Vector3::UNIT_SCALE,
		const std::string& Group = "default",
		bool  vAttachToSceneNode = true,
		bool  isCloneMesh = false
		);

	// 自动创建

	COTEActorEntity* AutoCreateEntity(const std::string& resName, const std::string& type = "Ent", bool isCloneMesh = false);

	// 获取实体

	COTEActorEntity* GetEntity(const Ogre::String& name)
	{		
		std::map<Ogre::String, COTEEntity* >::iterator i = m_EntityList.find(name);
		if (i == m_EntityList.end())
		{
			return NULL;
		}	
		return (COTEActorEntity*)(i->second);		
	}

	//实体是否存在

	bool IsEntityPresent(const Ogre::String& name)
	{		
		if (m_EntityList.find(name) == m_EntityList.end())
		{
			return false;
		}
		return true;
	}

	// 移除实体

	void RemoveEntity(const Ogre::String& rName, bool Immediately = true);

	void RemoveEntity(const OTE::COTEEntity* pEntity, bool Immediately = true);
	
	virtual void RemoveAllEntities();	

	// 得到场景中的实体列表

	void GetEntityList(std::vector<Ogre::MovableObject*>* elist);
	
	// 选择实体

	COTEActorEntity* SelectEntity(const char* strName, bool isSelect = true);	

	void SelectAllEntities(bool isSelect);
	
	// 显示隐藏

	void ShowEntities(bool isVisible, const std::string& group = "default");

	size_t GetEntityCount()
	{
		return m_EntityList.size();
	}

	// 场景节点管理
 
	Ogre::SceneNode* GetSceneNode(const Ogre::String& strName);

	Ogre::MovableObject* GetCurObjSection()
	{
		return m_SelMovableObject; 
	}

	/// 物体选择

	void SetCurFocusObj(Ogre::MovableObject* pMO)
	{		//ClearCurFocusObj();
		m_SelMovableObject = pMO;
		m_SelMovableObject->getParentSceneNode()->showBoundingBox(true);

		_TestEventCB(eSelectEntity, int(pMO));
	}

	void ClearCurFocusObj()
	{		
		if(m_SelMovableObject)
		{
			_TestEventCB(eUnSelectEntity, int(m_SelMovableObject));
			m_SelMovableObject->getParentSceneNode()->showBoundingBox(false);
			m_SelMovableObject = NULL;
		}

		m_SelMovableObjects.clear();
	}

	bool IsObjFocused(Ogre::MovableObject* pMO)
	{
		return m_SelMovableObject == pMO;
	}

	bool IsObjFocused(const std::string& Name)
	{
		if(m_SelMovableObject)
			return m_SelMovableObject->getName() == Name;
		return false;
	}
	
	// 结点绑定

	void AttachObjToSceneNode(Ogre::MovableObject* MO, const std::string& Group = "default");
	
	void AttachObjToSceneNode(Ogre::MovableObject* MO, const std::string& rNodeName, const std::string& Group);

	void DetachObjAndDestroyNode(Ogre::MovableObject* MO);	

	void DetachObjAndDestroyNode(const std::string& rNodeName);	

	// 工作流
	
	void WalkEntities(void(*pCBFun)(Ogre::MovableObject*), const std::string& IgnoreKeyName = "");

	// 用户数据

	std::string GetUserData(const std::string& key)
	{
		return m_UserDataMap[key];
	}

	void SetUserData(const std::string& key, const std::string& val)
	{
		m_UserDataMap[key] = val;
	}

public:

	/// 回调函数

	void AddListener(int event, EventListener_t pListenFun);	

	void RemoveListener(EventListener_t pListenFun);

	void RemoveAllListeners()
	{
		m_EventListeners.clear();
	}

	// 公告牌

	Ogre::BillboardSet* CreateBillBoardSet(const std::string& name, int poolSize = 1);	

	void DeleteBillBoardSet(const std::string& name);

public:

	/// 处理设备丢失

	void AddInvalidDeviceCB(InvalidDeviceCB_t cbf)
	{
		// 查找是否存在

		std::list<InvalidDeviceCB_t>::iterator it = m_InvalidDeviceCBList.begin();
		while(it != m_InvalidDeviceCBList.end())
		{
			if((*it) == cbf)
				return;
			++ it;
		}
		m_InvalidDeviceCBList.push_back(cbf);
	}

	void AddRestoreDeviceCB(RestoreDeviceCB_t cbf)
	{
		// 查找是否存在

		std::list<RestoreDeviceCB_t>::iterator it = m_RestoreDeviceCBList.begin();
		while(it != m_RestoreDeviceCBList.end())
		{
			if((*it) == cbf)
				return;
			++ it;
		}
		m_RestoreDeviceCBList.push_back(cbf);
	}

protected:

	void _UpdateGrave();
	
	void _TestEventCB(int Event, int UD = 0);

public:		

	// 物件结点

	HashMap<std::string, Ogre::SceneNode*> 		m_EntSNRootMap;

	// 实体列表

	std::map<Ogre::String, COTEEntity* >		m_EntityList;		

	// 编辑状态

	bool										m_IsEditor;	

#   ifdef GAME_DEBUG
	HashMap<std::string, std::string>			m_RenderInfoList;
#   endif

	// 主摄像机
	
	Ogre::Camera*								m_MainCamera;

	// 主光源

	Ogre::Light*								m_MainLight;	

	std::list<Ogre::MovableObject*>				m_SelMovableObjects;

protected:
	
	// 用户数据

	std::map<Ogre::String, Ogre::String >		m_UserDataMap;

	// 场景事件响应回调函数

	typedef std::vector<EventListener_t>	RenderListenerList_t;	
	
	HashMap<int, RenderListenerList_t>		m_EventListeners;		

	// 处理设备丢失

	std::list<InvalidDeviceCB_t>					m_InvalidDeviceCBList;

	std::list<RestoreDeviceCB_t>					m_RestoreDeviceCBList;

	Ogre::MovableObject*							m_SelMovableObject; // 选中物体
	
	std::list<std::string>							m_GraveList;		// 坟墓
};


}

// ***********************************************
// 一些常用宏
// ***********************************************

inline OTE::COTEQTSceneManager*	GetOTESceneManager()
{
	static OTE::COTEQTSceneManager* __s_SceneMgr__  = ((OTE::COTEQTSceneManager*)Ogre::Root::getSingleton().getSceneManager(Ogre::ST_GENERIC));
	//assert(__s_SceneMgr__);
	return __s_SceneMgr__;
}

#define SCENE_MGR	GetOTESceneManager()

#define SCENE_MGR_OGRE	GetOTESceneManager()

// ***********************************************
inline Ogre::Camera*	GetOTEMainCamera()
{
	SCENE_MGR->m_MainCamera == NULL ? SCENE_MGR->m_MainCamera = SCENE_MGR->getCamera("CamMain") : NULL;
	return SCENE_MGR->m_MainCamera;
}

#define SCENE_CAM	GetOTEMainCamera()	

// ***********************************************
inline Ogre::Light*	GetOTEMainLight()
{		
	SCENE_MGR->m_MainLight == NULL ? SCENE_MGR->m_MainLight = SCENE_MGR->getLight("MainLight") : NULL;
	return SCENE_MGR->m_MainLight;
}

#define SCENE_MAINLIGHT	GetOTEMainLight()	

// ***********************************************
inline Ogre::D3D9RenderSystem*	GetOTERender()
{
	static Ogre::D3D9RenderSystem* __s_RenderSystem__  = ( (Ogre::D3D9RenderSystem*)(Ogre::Root::getSingleton().getRenderSystem( ) ) );
	//assert(__s_Light__);
	return __s_RenderSystem__;
}

#define	RENDER	GetOTERender()	
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
// �Ĳ���������������Ӧ�ð汾
// ***********************************************

class _OTEExport COTEQTSceneManager : public CQuadTreeSceneManager
{
friend class COTEActorEntity;
friend class CFrameListener;

public:

	// �¼�����

	enum{eBeforeRenderObjs, eEndRenderObjs, eRenderCB, eSceneClear, eSelectEntity, eUnSelectEntity};

	typedef void(*EventListener_t)(int UD);		// �����¼��ص�����

	typedef void(*InvalidDeviceCB_t)();			// ȫ������֮ ����ص�����
	typedef void(*RestoreDeviceCB_t)();			// ȫ������֮ ���´����ص�����

public:
	COTEQTSceneManager(
		const Ogre::AxisAlignedBox &box = Ogre::AxisAlignedBox( -256, -10, -256, 256, 100, 256 ), 
		int depth = 3);

	virtual ~COTEQTSceneManager(void);

public:

	// ��ʼ��������

	void InitWorld();

	Ogre::RenderSystem* GetRenderSystem()
	{
		return mDestRenderSystem;
	}
	
	// ��Ⱦ
	virtual void _renderScene(Ogre::Camera *cam, Ogre::Viewport *vp, bool includeOverlays);
	
	// �����
	virtual Ogre::Camera* createCamera( const Ogre::String &name );		

	// ����	

	virtual void clearScene(void);	

	virtual void _renderVisibleObjects(Ogre::Camera *cam);

public:

	/// ʵ��Ĺ���

	COTEActorEntity* CreateEntity(
		const Ogre::String& File, const Ogre::String& Name,
		const Ogre::Vector3& rPosition = Ogre::Vector3::ZERO,
		const Ogre::Quaternion& rQuaternion = Ogre::Quaternion::IDENTITY,
		const Ogre::Vector3& rScaling = Ogre::Vector3::UNIT_SCALE,
		const std::string& Group = "default",
		bool  vAttachToSceneNode = true,
		bool  isCloneMesh = false
		);

	// �Զ�����

	COTEActorEntity* AutoCreateEntity(const std::string& resName, const std::string& type = "Ent", bool isCloneMesh = false);

	// ��ȡʵ��

	COTEActorEntity* GetEntity(const Ogre::String& name)
	{		
		std::map<Ogre::String, COTEEntity* >::iterator i = m_EntityList.find(name);
		if (i == m_EntityList.end())
		{
			return NULL;
		}	
		return (COTEActorEntity*)(i->second);		
	}

	//ʵ���Ƿ����

	bool IsEntityPresent(const Ogre::String& name)
	{		
		if (m_EntityList.find(name) == m_EntityList.end())
		{
			return false;
		}
		return true;
	}

	// �Ƴ�ʵ��

	void RemoveEntity(const Ogre::String& rName, bool Immediately = true);

	void RemoveEntity(const OTE::COTEEntity* pEntity, bool Immediately = true);
	
	virtual void RemoveAllEntities();	

	// �õ������е�ʵ���б�

	void GetEntityList(std::vector<Ogre::MovableObject*>* elist);
	
	// ѡ��ʵ��

	COTEActorEntity* SelectEntity(const char* strName, bool isSelect = true);	

	void SelectAllEntities(bool isSelect);
	
	// ��ʾ����

	void ShowEntities(bool isVisible, const std::string& group = "default");

	size_t GetEntityCount()
	{
		return m_EntityList.size();
	}

	// �����ڵ����
 
	Ogre::SceneNode* GetSceneNode(const Ogre::String& strName);

	Ogre::MovableObject* GetCurObjSection()
	{
		return m_SelMovableObject; 
	}

	/// ����ѡ��

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
	
	// ����

	void AttachObjToSceneNode(Ogre::MovableObject* MO, const std::string& Group = "default");
	
	void AttachObjToSceneNode(Ogre::MovableObject* MO, const std::string& rNodeName, const std::string& Group);

	void DetachObjAndDestroyNode(Ogre::MovableObject* MO);	

	void DetachObjAndDestroyNode(const std::string& rNodeName);	

	// ������
	
	void WalkEntities(void(*pCBFun)(Ogre::MovableObject*), const std::string& IgnoreKeyName = "");

	// �û�����

	std::string GetUserData(const std::string& key)
	{
		return m_UserDataMap[key];
	}

	void SetUserData(const std::string& key, const std::string& val)
	{
		m_UserDataMap[key] = val;
	}

public:

	/// �ص�����

	void AddListener(int event, EventListener_t pListenFun);	

	void RemoveListener(EventListener_t pListenFun);

	void RemoveAllListeners()
	{
		m_EventListeners.clear();
	}

	// ������

	Ogre::BillboardSet* CreateBillBoardSet(const std::string& name, int poolSize = 1);	

	void DeleteBillBoardSet(const std::string& name);

public:

	/// �����豸��ʧ

	void AddInvalidDeviceCB(InvalidDeviceCB_t cbf)
	{
		// �����Ƿ����

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
		// �����Ƿ����

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

	// ������

	HashMap<std::string, Ogre::SceneNode*> 		m_EntSNRootMap;

	// ʵ���б�

	std::map<Ogre::String, COTEEntity* >		m_EntityList;		

	// �༭״̬

	bool										m_IsEditor;	

#   ifdef GAME_DEBUG
	HashMap<std::string, std::string>			m_RenderInfoList;
#   endif

	// �������
	
	Ogre::Camera*								m_MainCamera;

	// ����Դ

	Ogre::Light*								m_MainLight;	

	std::list<Ogre::MovableObject*>				m_SelMovableObjects;

protected:
	
	// �û�����

	std::map<Ogre::String, Ogre::String >		m_UserDataMap;

	// �����¼���Ӧ�ص�����

	typedef std::vector<EventListener_t>	RenderListenerList_t;	
	
	HashMap<int, RenderListenerList_t>		m_EventListeners;		

	// �����豸��ʧ

	std::list<InvalidDeviceCB_t>					m_InvalidDeviceCBList;

	std::list<RestoreDeviceCB_t>					m_RestoreDeviceCBList;

	Ogre::MovableObject*							m_SelMovableObject; // ѡ������
	
	std::list<std::string>							m_GraveList;		// ��Ĺ
};


}

// ***********************************************
// һЩ���ú�
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
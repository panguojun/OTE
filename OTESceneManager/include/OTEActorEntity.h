#pragma once

#include "OTEStaticInclude.h"
#include "OTEAction.h"
#include "OTEReactorBase.h"
#include "EventObjBase.h"
#include "OTEEntity.h"
#include "DymProperty.h"

namespace OTE
{

// 利用 Movalbe object userdata 作FLAG 盛方一些信息

#define		MOVABLE_UD_BINDTOTARGET			0x0000000F		// 绑定到目标

// ********************************************************
// Hook_t
// 挂点系统
// ********************************************************

struct Hook_t
{
	/// 属性(将保存到文件里)

	std::string	  		HookName;
	std::string	  		BoneName;

	Ogre::Vector3	 	OffestPos;
	Ogre::Quaternion 	OffestQuaternion;
	Ogre::Vector3	 	Scale;

	// 状态

	COTETrack*			BindTrack;		// 邦定轨道动画

	std::list<Ogre::MovableObject*> HookEntityList;

	/// 操作

	Hook_t(const std::string& rName, const std::string& rBoneName) :
	BindTrack(NULL)
	{
		HookName			=	rName;
		BoneName			=	rBoneName;			
		OffestPos			=	Ogre::Vector3::ZERO;
		OffestQuaternion	=	Ogre::Quaternion::IDENTITY;
		Scale				=	Ogre::Vector3::UNIT_SCALE;	
	}
};

// ********************************************************
// HookData_t
// 挂点数据
// ********************************************************

struct HookData_t
{
	std::string				Name;
	std::string				BoneName;
	Ogre::Vector3			Position;
	Ogre::Quaternion		Roation;
	Ogre::Vector3			Scale;

	HookData_t(	const std::string& rName, 
			    const std::string& rBoneName, 
				Ogre::Vector3 vPos = Ogre::Vector3::ZERO,
				Ogre::Quaternion vRot = Ogre::Quaternion::IDENTITY, 
				Ogre::Vector3 vScale = Ogre::Vector3::UNIT_SCALE )
	{
		Name		=	rName;
		BoneName	=	rBoneName;
		Position	=	vPos;
		Roation		=	vRot;
		Scale		=	vScale;
	}
};

// ********************************************************	
// Obj　对象数据
// ********************************************************

struct ObjData_t
{
	// 属性数据

	std::string							FilePath;
	std::string							MeshFile;
	std::list<std::string>				SubMeshList;
	std::string							SkeletonFile;

	// 状态

	bool								bHasMesh;
	bool								bHasSkeleton;	

	std::map<std::string, std::string>	AniFileList;
	std::list<HookData_t*>				HookList;

	ObjData_t()
	{
		FilePath = "";
		MeshFile = "";
		SkeletonFile = "";
		bHasSkeleton = false;
	}
};

// ********************************************************
// COTEActorEntity
// ********************************************************
#define		RESLOAD_FINISHED		-2
#define		RESLOAD_BEGIN			-1

class _OTEExport COTEActorEntity : public COTEEntity, public CEventObjBase, public CDymProperty
{
friend class COTEActorHookMgr;
friend class COTEActorEquipmentMgr;	
friend class COTEEntityPhysicsMgr;
friend class COTESubEntity;
friend class COTEActionReactorUsage;
friend class COTEShadowMap;

public:
	//构造函数
	COTEActorEntity(const std::string& szName, Ogre::MeshPtr meshPtr, bool isCloneMesh = false);

	COTEActorEntity(bool isCloneMesh = false);

	//析构函数
    virtual ~COTEActorEntity();

public:

	// 是否资源加载完璧
	bool IsResLoaded()
	{
		return this->m_ProccessTicket == RESLOAD_FINISHED;
	}

	void WaitForLoadRes();

	// 事件对象名字
	virtual std::string GetName()
	{
		return getName();
	}

	// 得到角色实体的场景属性(如果有本体轨道动画　则认为是其起点)

	Ogre::SceneNode* GetParentSceneNode()
	{
		return m_pTrackOrgSceneNode ? m_pTrackOrgSceneNode : this->getParentSceneNode();
	}	
	
	/// 朝向

	const Ogre::Vector3& GetDirection()
	{
		static Ogre::Vector3 sDir;
		sDir = GetParentSceneNode()->getOrientation() * Ogre::Vector3::UNIT_Z;
		return sDir;		
	}

	void SetDirection(const Ogre::Vector3& rDir)
	{
		const Ogre::Vector3& curDir = GetDirection();
		
		Ogre::Real cos = curDir.dotProduct(rDir);
		Ogre::Real sin = curDir.crossProduct(rDir).dotProduct(Ogre::Vector3::UNIT_Y);
		if(cos < 0.9) // 为了防止抖动
		{
			GetParentSceneNode()->setOrientation(
				Ogre::Quaternion(Ogre::Math::ATan2(sin, cos), Ogre::Vector3::UNIT_Y) * GetParentSceneNode()->getOrientation());
		}	
	}

	/// 面朝角度

	Ogre::Radian GetRadian(const Ogre::Vector3& rAxis = Ogre::Vector3::UNIT_X)
	{
		Ogre::Vector3 curDir = GetDirection();	
		Ogre::Real cos = curDir.dotProduct(rAxis);
		Ogre::Real sin = curDir.crossProduct(rAxis).dotProduct(Ogre::Vector3::UNIT_Y);

		return Ogre::Math::ATan2(sin, cos);		
	}

	void SetRadian(const Ogre::Radian& rRad)
	{	
		GetParentSceneNode()->setOrientation(Ogre::Quaternion(
			rRad, Ogre::Vector3::UNIT_Y) * GetParentSceneNode()->getOrientation());
	}	

	void SetupRes();

	//动画控制

	bool IsAnimationStatePresent(const Ogre::String& name)
    {       
		if(!mAnimationState)
			return false;

		Ogre::AnimationStateSet::iterator i = mAnimationState->find(name);

        if (i == mAnimationState->end())
        {
            return false;
        }
        return true;
    }	

	void SetCurrentAnim(const std::string& szAnimName);

	void EnalbeAnim(bool bEnable = true);

	bool IsEnableAnim();

	// 更新
	virtual void _notifyCurrentCamera(Ogre::Camera* cam);	

	//virtual void _updateRenderQueue(Ogre::RenderQueue *queue);

	/// 动画速度

	float GetAnimSpeed()
	{
		return m_AnimSpeed;
	}

	void SetAnimSpeed(float speed)
	{
		m_AnimSpeed = speed;
	}

	/// 行动

	COTEAction* GetCurAction()
	{
		return m_CurAction;
	}
	void SetCurAction(COTEAction* action)
	{
		m_CurAction = action;
	}

	// 更新实体状态

	bool UpdateState(float fDtime);	

	// 子实体

	OTESubEntityList* GetSubEntityList()
	{
		return &mSubEntityList;
	}		
	
	// 设置材质相关

	struct MatEff_t
	{
		MatEff_t()
		{         
			SourceBF	=	Ogre::SBF_ONE;	
			DestBF		=	Ogre::SBF_ZERO;
			DepthWriteEnabled = true;
		}
		Ogre::SceneBlendFactor		SourceBF;
		Ogre::SceneBlendFactor		DestBF;
		Ogre::ColourValue			Ambient;
		Ogre::ColourValue			Diffuse;
		bool						DepthWriteEnabled;
	};

	void SetMaterials(const MatEff_t& me);		

	void ResetMaterialChange();	

	// 渲染回调

	typedef		void(*pRenderCB_t)(COTEActorEntity* ae); 

	void AddBeforeRenderCB(pRenderCB_t rcb)
	{
		for(unsigned int i = 0; i < m_BeforeRenderCBList.size(); i ++)
		{
			if(m_BeforeRenderCBList[i] == rcb)
			{
				OTE_TRACE_WARNING("这个物件的渲染前回调已经存在! " << this->GetName())
				return;
			}
		}

		m_BeforeRenderCBList.push_back(rcb);
	}

	void AddEndRenderCB(pRenderCB_t rcb)
	{
		for(unsigned int i = 0; i < m_EndRenderCBList.size(); i ++)
		{
			if(m_EndRenderCBList[i] == rcb)
			{
				OTE_TRACE_WARNING("这个物件的渲染后回调已经存在! " << this->GetName())
				return;
			}
		}

		m_EndRenderCBList.push_back(rcb);
	}
	
public:	

	// 创建角色

	static COTEActorEntity* CreateActorEntity(const std::string& ObjFile, const std::string& name, bool isCloneMesh = false);
	
	static void OnBeforeRenderObjs(int ud);

	static void Init();

	// 清理场景

	static void Clear();

	
public: /// 行动事件响应		
	
	// 行动开始

	void OnActionBegin()
	{
		if(this->m_CurAction)
		{
			OTE_TRIGGER_EVENT_WITHUSERSTRING_NOLOG("OnActionBegin", this->m_CurAction->GetName());
		}
	}

	// 行动结束

	void OnActionEnd()
	{
		if(this->m_CurAction)
		{
			OTE_TRIGGER_EVENT_WITHUSERSTRING_NOLOG("OnActionEnd", this->m_CurAction->GetName());
		}
	}

	// 行动奏效

	void OnActionInure()
	{
		if(this->m_CurAction)
		{
			OTE_TRIGGER_EVENT_WITHUSERSTRING_NOLOG("OnActionInure", this->m_CurAction->GetName());
		}
	}
	// 击中目标

	void OnHitTarget()
	{
		if(this->m_CurAction)
		{
			OTE_TRIGGER_EVENT_WITHUSERSTRING_NOLOG("OnHitTarget", this->m_CurAction->GetName() );
		}
	}

	// 脱离目标

	void OnMissTarget()
	{
		if(this->m_CurAction)
		{
			OTE_TRIGGER_EVENT_WITHUSERSTRING_NOLOG("OnMissTarget", this->m_CurAction->GetName());
		}
	}

	// 被击中

	void OnBeHit(COTEActorEntity* attacker)
	{
		CEventObjBase::EventArg_t arg;
		arg.strObjName = GetName();	
		arg.strObjEventName = "OnBeHit";
		arg.vParamList.push_back(this);
		arg.vParamList.push_back(attacker);
		
		TriggerEvent("OnBeHit", arg);		
	}

protected:
	
	COTESubEntity* GetSubEntity(const Ogre::String& SubEntityName);

	void ClearSubEntityList();

	void BuildSubEntityList(Ogre::MeshPtr& mesh, COTEEntity::OTESubEntityList* sublist, bool HasMesh = false);
	
	static COTEActorEntity* CreateActorEntity(	ObjData_t* oin,
												const std::string& name,
												bool isCloneMesh = false
												);		

	static void SetupAnimations(const Ogre::MeshPtr mesh, const Ogre::String& skeletonFile, std::map<std::string, std::string>* aniFileList);

public:

	float										m_AnimSpeed;			// 动画速度
	COTEAction*									m_CurAction;			// 角色的当前行动
	std::string									m_ResFile;				// 角色的资源文件名		
	Ogre::AnimationState*						m_pCurAnimState;		// 当前的动画状态		
	HashMap<std::string, Hook_t*>				m_PotHookList;			// 挂点列表	
	HashMap<std::string, COTEAction*>			m_ActionList;			// 行动列表

	HashMap<std::string, std::string>			m_EquipmentMapEx;		// submesh装备列表

	typedef std::pair<std::string, COTEAction::PlayMode>    ActionPlay_t;

	std::list<ActionPlay_t>						m_SolidList;			// 连播行动列表
	std::string									m_SolidListName;		// 连播行动列表名
	bool										m_bSolidEnd;			// 连播状态

	bool										m_IsAutoPlay;			// 自动播放	
	bool										m_NeedRender;			// 需要渲染
	ObjData_t*									m_pObjData;				// 数据												
	
	Ogre::SceneNode*							m_pTrackOrgSceneNode;	// 用于本体轨道邦定

	// 用于shader参数更新的生化反应器接口
	
	CReactorBase*								m_pGPUReactor;
		
	// 回调函数 用于渲染特殊效果

	std::vector<pRenderCB_t>					m_BeforeRenderCBList;	// 渲染回调函数

	std::vector<pRenderCB_t>					m_EndRenderCBList;		// 渲染回调函数

	// 动态属性

	OTE::CDymProperty							m_DymProperty;			

protected:
#ifdef OGRE_THREAD_SUPPORT
	Ogre::CriSection OGRE_AUTO_MUTEX_NAME;
#endif
	std::list< std::pair<COTESubEntity*, Ogre::Material*>  >		m_MaterList;	// 用于选中之类效果更改与还原 

	bool										m_EquipmentNeedUpdate;  // 装备需要更新										

	// 暂时多线程加载相关

	long										m_ProccessTicket;	

	bool										m_IsCloneMesh;			// 是否复制mesh 	

	bool										m_IsMeshExist;			// .MESH文件是否存在, 主要是场景，npc与主角的区别

};

// **********************************************
// CMagicFactoryBase
// **********************************************
class _OTEExport CMagicFactoryBase
{
public:

	virtual Ogre::MovableObject* Create(const std::string& ResName,	const std::string& Name, bool cloneMat) = 0;
	
	virtual void Delete(const std::string& Name) = 0;

	virtual void GetTypeList(std::list<std::string>& rTypeList){}

	virtual void GetTypeDescList(std::list<std::string>& rTypeDescList){}

	virtual void GetDescription(std::string& rDesc){}
};

// ****************************************************
// COTEEntityFactory
// ****************************************************
class COTEEntityFactory : public CMagicFactoryBase
{
public:

	virtual Ogre::MovableObject* Create(const std::string& ResName,	const std::string& Name, bool cloneMat);

	virtual void Delete(const std::string& Name);

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "模型实体特效";
	}
};

// ****************************************************
#define		BONE_SHOW_SIZE		0.05f

// ****************************************************
// COTEEntityBoneRender
// 用于显示骨胳
// ****************************************************

class _OTEExport COTEEntityBoneRender
{
public:
	static void RenderEntityBones(COTEActorEntity* ae);		

	static void SetSelectedBoneName(const Ogre::String& boneName);

protected:
	
	static void RenderBones(Ogre::Node::ChildNodeIterator it);

	static void RenderBoneNode(Ogre::Node* boneNode);

	static Ogre::String			s_SelectedBoneName;

	static COTEActorEntity*		s_CurrentActorEntity;

};

// ****************************************************
// COTEEntityBonePicker
// 用于显示骨胳
// ****************************************************
class _OTEExport COTEEntityBonePicker
{
public:
	
	static Ogre::Node* PickEntityBones(COTEActorEntity* ae, const Ogre::Ray* ray);	// 最好放到别处

protected:

	static Ogre::Node* PickBones(Ogre::Node::ChildNodeIterator it, const Ogre::Ray* ray);

	static bool PickBoneNode(Ogre::Node* boneNode, const Ogre::Ray* ray);

	static COTEActorEntity*		s_CurrentActorEntity;

};

}

// ****************************************************
// 一些常用宏

#define		IS_ACTOR_VALID(actor) (actor && ((COTEActorEntity*)actor)->IsResLoaded())

#define		WAIT_LOADING_RES(ae)	((COTEActorEntity*)ae)->WaitForLoadRes();

#define		RES_LOADED_CB(ae, cb)	((COTEActorEntity*)ae)->AddEventHandler("OnResLoaded", cb);

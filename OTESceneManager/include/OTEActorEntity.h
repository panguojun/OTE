#pragma once

#include "OTEStaticInclude.h"
#include "OTEAction.h"
#include "OTEReactorBase.h"
#include "EventObjBase.h"
#include "OTEEntity.h"
#include "DymProperty.h"

namespace OTE
{

// ���� Movalbe object userdata ��FLAG ʢ��һЩ��Ϣ

#define		MOVABLE_UD_BINDTOTARGET			0x0000000F		// �󶨵�Ŀ��

// ********************************************************
// Hook_t
// �ҵ�ϵͳ
// ********************************************************

struct Hook_t
{
	/// ����(�����浽�ļ���)

	std::string	  		HookName;
	std::string	  		BoneName;

	Ogre::Vector3	 	OffestPos;
	Ogre::Quaternion 	OffestQuaternion;
	Ogre::Vector3	 	Scale;

	// ״̬

	COTETrack*			BindTrack;		// ��������

	std::list<Ogre::MovableObject*> HookEntityList;

	/// ����

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
// �ҵ�����
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
// Obj����������
// ********************************************************

struct ObjData_t
{
	// ��������

	std::string							FilePath;
	std::string							MeshFile;
	std::list<std::string>				SubMeshList;
	std::string							SkeletonFile;

	// ״̬

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
	//���캯��
	COTEActorEntity(const std::string& szName, Ogre::MeshPtr meshPtr, bool isCloneMesh = false);

	COTEActorEntity(bool isCloneMesh = false);

	//��������
    virtual ~COTEActorEntity();

public:

	// �Ƿ���Դ�������
	bool IsResLoaded()
	{
		return this->m_ProccessTicket == RESLOAD_FINISHED;
	}

	void WaitForLoadRes();

	// �¼���������
	virtual std::string GetName()
	{
		return getName();
	}

	// �õ���ɫʵ��ĳ�������(����б���������������Ϊ�������)

	Ogre::SceneNode* GetParentSceneNode()
	{
		return m_pTrackOrgSceneNode ? m_pTrackOrgSceneNode : this->getParentSceneNode();
	}	
	
	/// ����

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
		if(cos < 0.9) // Ϊ�˷�ֹ����
		{
			GetParentSceneNode()->setOrientation(
				Ogre::Quaternion(Ogre::Math::ATan2(sin, cos), Ogre::Vector3::UNIT_Y) * GetParentSceneNode()->getOrientation());
		}	
	}

	/// �泯�Ƕ�

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

	//��������

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

	// ����
	virtual void _notifyCurrentCamera(Ogre::Camera* cam);	

	//virtual void _updateRenderQueue(Ogre::RenderQueue *queue);

	/// �����ٶ�

	float GetAnimSpeed()
	{
		return m_AnimSpeed;
	}

	void SetAnimSpeed(float speed)
	{
		m_AnimSpeed = speed;
	}

	/// �ж�

	COTEAction* GetCurAction()
	{
		return m_CurAction;
	}
	void SetCurAction(COTEAction* action)
	{
		m_CurAction = action;
	}

	// ����ʵ��״̬

	bool UpdateState(float fDtime);	

	// ��ʵ��

	OTESubEntityList* GetSubEntityList()
	{
		return &mSubEntityList;
	}		
	
	// ���ò������

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

	// ��Ⱦ�ص�

	typedef		void(*pRenderCB_t)(COTEActorEntity* ae); 

	void AddBeforeRenderCB(pRenderCB_t rcb)
	{
		for(unsigned int i = 0; i < m_BeforeRenderCBList.size(); i ++)
		{
			if(m_BeforeRenderCBList[i] == rcb)
			{
				OTE_TRACE_WARNING("����������Ⱦǰ�ص��Ѿ�����! " << this->GetName())
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
				OTE_TRACE_WARNING("����������Ⱦ��ص��Ѿ�����! " << this->GetName())
				return;
			}
		}

		m_EndRenderCBList.push_back(rcb);
	}
	
public:	

	// ������ɫ

	static COTEActorEntity* CreateActorEntity(const std::string& ObjFile, const std::string& name, bool isCloneMesh = false);
	
	static void OnBeforeRenderObjs(int ud);

	static void Init();

	// ������

	static void Clear();

	
public: /// �ж��¼���Ӧ		
	
	// �ж���ʼ

	void OnActionBegin()
	{
		if(this->m_CurAction)
		{
			OTE_TRIGGER_EVENT_WITHUSERSTRING_NOLOG("OnActionBegin", this->m_CurAction->GetName());
		}
	}

	// �ж�����

	void OnActionEnd()
	{
		if(this->m_CurAction)
		{
			OTE_TRIGGER_EVENT_WITHUSERSTRING_NOLOG("OnActionEnd", this->m_CurAction->GetName());
		}
	}

	// �ж���Ч

	void OnActionInure()
	{
		if(this->m_CurAction)
		{
			OTE_TRIGGER_EVENT_WITHUSERSTRING_NOLOG("OnActionInure", this->m_CurAction->GetName());
		}
	}
	// ����Ŀ��

	void OnHitTarget()
	{
		if(this->m_CurAction)
		{
			OTE_TRIGGER_EVENT_WITHUSERSTRING_NOLOG("OnHitTarget", this->m_CurAction->GetName() );
		}
	}

	// ����Ŀ��

	void OnMissTarget()
	{
		if(this->m_CurAction)
		{
			OTE_TRIGGER_EVENT_WITHUSERSTRING_NOLOG("OnMissTarget", this->m_CurAction->GetName());
		}
	}

	// ������

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

	float										m_AnimSpeed;			// �����ٶ�
	COTEAction*									m_CurAction;			// ��ɫ�ĵ�ǰ�ж�
	std::string									m_ResFile;				// ��ɫ����Դ�ļ���		
	Ogre::AnimationState*						m_pCurAnimState;		// ��ǰ�Ķ���״̬		
	HashMap<std::string, Hook_t*>				m_PotHookList;			// �ҵ��б�	
	HashMap<std::string, COTEAction*>			m_ActionList;			// �ж��б�

	HashMap<std::string, std::string>			m_EquipmentMapEx;		// submeshװ���б�

	typedef std::pair<std::string, COTEAction::PlayMode>    ActionPlay_t;

	std::list<ActionPlay_t>						m_SolidList;			// �����ж��б�
	std::string									m_SolidListName;		// �����ж��б���
	bool										m_bSolidEnd;			// ����״̬

	bool										m_IsAutoPlay;			// �Զ�����	
	bool										m_NeedRender;			// ��Ҫ��Ⱦ
	ObjData_t*									m_pObjData;				// ����												
	
	Ogre::SceneNode*							m_pTrackOrgSceneNode;	// ���ڱ������

	// ����shader�������µ�������Ӧ���ӿ�
	
	CReactorBase*								m_pGPUReactor;
		
	// �ص����� ������Ⱦ����Ч��

	std::vector<pRenderCB_t>					m_BeforeRenderCBList;	// ��Ⱦ�ص�����

	std::vector<pRenderCB_t>					m_EndRenderCBList;		// ��Ⱦ�ص�����

	// ��̬����

	OTE::CDymProperty							m_DymProperty;			

protected:
#ifdef OGRE_THREAD_SUPPORT
	Ogre::CriSection OGRE_AUTO_MUTEX_NAME;
#endif
	std::list< std::pair<COTESubEntity*, Ogre::Material*>  >		m_MaterList;	// ����ѡ��֮��Ч�������뻹ԭ 

	bool										m_EquipmentNeedUpdate;  // װ����Ҫ����										

	// ��ʱ���̼߳������

	long										m_ProccessTicket;	

	bool										m_IsCloneMesh;			// �Ƿ���mesh 	

	bool										m_IsMeshExist;			// .MESH�ļ��Ƿ����, ��Ҫ�ǳ�����npc�����ǵ�����

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
		rDesc = "ģ��ʵ����Ч";
	}
};

// ****************************************************
#define		BONE_SHOW_SIZE		0.05f

// ****************************************************
// COTEEntityBoneRender
// ������ʾ�Ǹ�
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
// ������ʾ�Ǹ�
// ****************************************************
class _OTEExport COTEEntityBonePicker
{
public:
	
	static Ogre::Node* PickEntityBones(COTEActorEntity* ae, const Ogre::Ray* ray);	// ��÷ŵ���

protected:

	static Ogre::Node* PickBones(Ogre::Node::ChildNodeIterator it, const Ogre::Ray* ray);

	static bool PickBoneNode(Ogre::Node* boneNode, const Ogre::Ray* ray);

	static COTEActorEntity*		s_CurrentActorEntity;

};

}

// ****************************************************
// һЩ���ú�

#define		IS_ACTOR_VALID(actor) (actor && ((COTEActorEntity*)actor)->IsResLoaded())

#define		WAIT_LOADING_RES(ae)	((COTEActorEntity*)ae)->WaitForLoadRes();

#define		RES_LOADED_CB(ae, cb)	((COTEActorEntity*)ae)->AddEventHandler("OnResLoaded", cb);

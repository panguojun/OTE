#pragma once
#include "OTEStaticInclude.h"
#include "OgreD3D9RenderSystem.h" 
#include "OgreRenderQueuelistener.h"
#include "OgreVector2.h"
#include "OgreGpuProgram.h"
#include "ogreframelistener.h"
#include "dymproperty.h"
#include "OTEBillBoardSet.h"

#define CG_BASE_ID		100

namespace OTE
{
class COTEParticleSystem;
class CEmitter;

// **************************************
// COTEParticleSystemAffector
// 等效于Ogre 中 Affector的概念
// **************************************

class _OTEExport COTEParticleSystemAffector : public CDymProperty
{
public:	
	
	// 更新

	bool UpdateGPUParams( 
		const Ogre::GpuProgramParameters::AutoConstantEntry& constantEntry, 
		Ogre::GpuProgramParameters* params, 
		const COTEParticleSystem* ps) const; 		

};

// **************************************
// CEmitter
// **************************************

class CEmitter
{
public:
	virtual void Emite(COTEParticleSystem* ps) = 0;
	virtual void Show(COTEParticleSystem* ps, bool isVisible) = 0;
};

// **************************************
// COTESimpleEmitter
// **************************************

class _OTEExport COTESimpleEmitter : public CEmitter, public CDymProperty
{
public:	
	virtual void Emite(COTEParticleSystem* ps);

	virtual void Show(COTEParticleSystem* ps, bool isVisible = true);
	
};


// **************************************
// COTEParticleSystemRes
// 资源
// **************************************
class _OTEExport COTEParticleSystemRes
{
friend class COTEParticleSystem;
public:

	COTEParticleSystemRes(const Ogre::String& name,
						  const Ogre::String& emitterTypeName  = "SimpleEmitter", 
						  const Ogre::String& affectorTypeName = "SimpleAffector");
	
	~COTEParticleSystemRes();	

	// 公告板

	struct Billboard_t
	{
		Ogre::ColourValue colour;
		Ogre::Vector2	  size;
		Ogre::Radian	  rotation;
		Ogre::Vector3	  position;
	};

	// 生成顶点

	void genVertices(
		const Billboard_t& bb, 
		float period,						/*周期*/
		int groupIndex,						/*粒子系统一簇之中公告板索引*/
		int groupSize,						/*簇的容量*/
		const Ogre::Vector3& dirMin, const Ogre::Vector3& dirMax,
		float velMin, float velMax,
		const Ogre::ColourValue& corRangeMin, const Ogre::ColourValue& corRangeMax,
		float sizeMin, float sizeMax
		);	

	void CreateBuffer();

	void ClearBuffer();	

	void beginBillboards(void);

	void endBillboards(void);

	bool IsInit()
	{
		return mVertexData != NULL;
	}

public:

	/// 属性成员

	CEmitter*					m_Emitter;

	COTEParticleSystemAffector*	m_ParticleSystemAffector;

	Ogre::String				m_MaterialName;

	Ogre::String				m_TextureName;

	int							m_PoolSize;

	int							mNumVisibleBillboards;

	float						m_LiftTime;			// 粒子的生命周期

	/// The vertex position data for all billboards in this set.
	Ogre::VertexData*			mVertexData;
	/// Shortcut to main buffer (positions, colours, texture coords)
	Ogre::HardwareVertexBufferSharedPtr mMainBuf;

    //unsigned short* mpIndexes;
    Ogre::IndexData*			mIndexData;

	float*						mLockPtr;

	// res create time
	float						m_CreateTime;

	Ogre::String				m_Name;

};

// ***********************************************
// COTEParticleSystem
// 
// 我们把一个粒子系统分为  公告板 -〉簇 -〉粒子系统 三级，
// 其中 "簇" 是指在同一时刻各个粒子的相位各不相同，而且均匀的分布在整个周期[0, T]范围内而组成一个独立的单元.
// 而一个粒子系统是由多个这样的 "簇" 组成的
// ***********************************************

class COTEParticleSystem : public Ogre::Renderable, public Ogre::MovableObject
{
friend class COTESimpleEmitter;
friend class COTEParticleSystemManager;
friend class COTEParticleTieReactorEx;
public:
	COTEParticleSystem(const Ogre::String& name, COTEParticleSystemRes* res);
	~COTEParticleSystem(void);

public:	

	virtual void setMaterialName(const Ogre::String& name);

    /** Sets the name of the material to be used for this billboard set.
        @returns The name of the material that is used for this set.
    */
    virtual const Ogre::String& getMaterialName(void) const
	{
		return mMaterialName;	
	}

    /** Overridden from MovableObject
        @see
            MovableObject
    */
	virtual void _notifyCurrentCamera(Ogre::Camera* cam);


    /** Overridden from MovableObject
        @see
            MovableObject
    */
    virtual const Ogre::AxisAlignedBox& getBoundingBox(void) const 
	{ 
		return mAABB; 
	}

    /** Overridden from MovableObject
        @see
            MovableObject
    */
    virtual Ogre::Real getBoundingRadius(void) const { return mBoundingRadius; }

    /** Overridden from MovableObject
        @see
            MovableObject
    */
    virtual void _updateRenderQueue(Ogre::RenderQueue* queue);

    /** @copydoc Renderable::getWorldOrientation */
    virtual const Ogre::Quaternion& getWorldOrientation(void) const;
    
	/** @copydoc Renderable::getWorldPosition */
    virtual const Ogre::Vector3& getWorldPosition(void) const;

	virtual void getWorldTransforms(Ogre::Matrix4* xform) const;

    virtual void getRenderOperation(Ogre::RenderOperation& op);	

	virtual const Ogre::MaterialPtr& getMaterial(void) const
	{
		return mpMaterial;
	}

    /** Overridden from MovableObject */
    virtual const Ogre::String& getName(void) const
	{
		return m_Name;
	}


    /** Overridden from MovableObject */
	virtual const Ogre::String& getMovableType(void) const
	{
		return msMovableType;
	}

	virtual void _updateCustomGpuParameter(
		const Ogre::GpuProgramParameters::AutoConstantEntry& constantEntry,
            Ogre::GpuProgramParameters* params) const;

    /** Overridden, see Renderable */
    Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const;

    /** @copydoc Renderable::getLights */
    const Ogre::LightList& getLights(void) const;	

	size_t GetPoolSize()
	{
		return m_ResPtr->m_PoolSize;
	}	

	COTEParticleSystemRes*	GetResPtr()
	{
		return m_ResPtr;
	}

	void	SetCreateTime(float crtTime)
	{
		mCreateTime = crtTime;
	}

	void SetVisible(bool isVisible);
	
protected:

	static Ogre::String		msMovableType;

	Ogre::AxisAlignedBox	mAABB;

    Ogre::Real				mBoundingRadius;

	Ogre::String			mMaterialName;

	Ogre::MaterialPtr		mpMaterial;	

	Ogre::String			m_Name;

	float					mCreateTime;

	mutable Ogre::Vector3	m_EntityInfo; // (x: 时间， y: 随机函数， z: ?)


	COTEParticleSystemRes*	m_ResPtr;

	bool					m_IsVisible;

};

// **************************************
// COTEParticleSystemManager
// **************************************

class _OTEExport COTEParticleSystemManager : public Ogre::FrameListener
{
friend class COTEParticleSystemXmlSerializer;
public:	
	static COTEParticleSystemManager*		s_pInst;
	
public:

	static COTEParticleSystemManager* GetInstance();

	COTEParticleSystemManager(){}

	virtual ~COTEParticleSystemManager(){}	

	static void Init();	

	static void Destroy();

	static void Clear();

	COTEParticleSystem* CreateParticleSystem(const Ogre::String& name, 
											const Ogre::String& resName, bool cloneMat);

	COTEParticleSystem* CreateParticleSystem(const Ogre::String& name, 
											COTEParticleSystemRes* psr, bool cloneMat);

	COTEParticleSystemRes* CreateParticleSystemRes(const Ogre::String& name)
	{		
		m_PSResList[name] = new COTEParticleSystemRes(name);		
		return m_PSResList[name];
	}

	void WriteXml(	const Ogre::String& name, 
					const Ogre::String& resName);

	COTEParticleSystemRes* GetParticleSystemRes(const Ogre::String& name)
	{		
		return m_PSResList[name];		
	}

	void RemoveParticleSystem(const Ogre::String& name);
	
	void RemoveAllParticleSystems();	

	void Render();		

	bool frameStarted(const Ogre::FrameEvent& evt);

    bool frameEnded(const Ogre::FrameEvent& evt);  

public:

	HashMap<std::string, COTEParticleSystem*>				m_ParticleSystems;	// 实体列表

	HashMap<Ogre::String, COTEParticleSystemRes*>			m_PSResList;		// 资源列表
	
};


// **************************************
// COTEParticleSystemFactory
// **************************************
class COTEParticleSystemFactory : public CMagicFactoryBase
{
public:

	virtual Ogre::MovableObject* Create(const std::string& ResName,	const std::string& Name, bool cloneMat);
	
	virtual void Delete(const std::string& Name);

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "粒子系统特效";
	}

};

}

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
// ��Ч��Ogre �� Affector�ĸ���
// **************************************

class _OTEExport COTEParticleSystemAffector : public CDymProperty
{
public:	
	
	// ����

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
// ��Դ
// **************************************
class _OTEExport COTEParticleSystemRes
{
friend class COTEParticleSystem;
public:

	COTEParticleSystemRes(const Ogre::String& name,
						  const Ogre::String& emitterTypeName  = "SimpleEmitter", 
						  const Ogre::String& affectorTypeName = "SimpleAffector");
	
	~COTEParticleSystemRes();	

	// �����

	struct Billboard_t
	{
		Ogre::ColourValue colour;
		Ogre::Vector2	  size;
		Ogre::Radian	  rotation;
		Ogre::Vector3	  position;
	};

	// ���ɶ���

	void genVertices(
		const Billboard_t& bb, 
		float period,						/*����*/
		int groupIndex,						/*����ϵͳһ��֮�й��������*/
		int groupSize,						/*�ص�����*/
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

	/// ���Գ�Ա

	CEmitter*					m_Emitter;

	COTEParticleSystemAffector*	m_ParticleSystemAffector;

	Ogre::String				m_MaterialName;

	Ogre::String				m_TextureName;

	int							m_PoolSize;

	int							mNumVisibleBillboards;

	float						m_LiftTime;			// ���ӵ���������

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
// ���ǰ�һ������ϵͳ��Ϊ  ����� -���� -������ϵͳ ������
// ���� "��" ��ָ��ͬһʱ�̸������ӵ���λ������ͬ�����Ҿ��ȵķֲ�����������[0, T]��Χ�ڶ����һ�������ĵ�Ԫ.
// ��һ������ϵͳ���ɶ�������� "��" ��ɵ�
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

	mutable Ogre::Vector3	m_EntityInfo; // (x: ʱ�䣬 y: ��������� z: ?)


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

	HashMap<std::string, COTEParticleSystem*>				m_ParticleSystems;	// ʵ���б�

	HashMap<Ogre::String, COTEParticleSystemRes*>			m_PSResList;		// ��Դ�б�
	
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
		rDesc = "����ϵͳ��Ч";
	}

};

}

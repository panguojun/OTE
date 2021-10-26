#pragma once
#include "OTEStaticInclude.h"
#include "ogresubentity.h"
#include "ogrenode.h"
#include "OgreAxisAlignedBox.h"
#include "OTEEntity.h"

#ifdef __OTE
#include "OTETile.h"
#endif

// ****************************************************
// ��ɫ��Ⱦ��� 

namespace OTE
{
class COTEActorEntity;
class COTEAutoEntity;

// ****************************************************
// COTESubEntity
// ****************************************************

class _OTEExport COTESubEntity : Ogre::Renderable
{
friend class COTEEntity;
public:

	COTESubEntity(){}
	COTESubEntity(COTEEntity* parent, Ogre::SubMesh* subMeshBasis);	
	virtual ~COTESubEntity();

public:

#ifdef __ZH

	void ManualSet(COTEEntity* parent, void* parentNode, Ogre::SubMesh* subMeshBasis);

#endif

	 /** Gets the name of the Material in use by this instance.
        */
    const Ogre::String& getMaterialName() const
	{
		return mMaterialName;
	}

    /** Sets the name of the Material to be used.
        @remarks
            By default a SubEntity uses the default Material that the SubMesh
            uses. This call can alter that so that the Material is different
            for this instance.
    */
    void setMaterialName( const Ogre::String& name );

    /** Tells this SubEntity whether to be visible or not. */
    virtual void setVisible(bool visible)
	{
		mVisible = visible;
	}

    /** Returns whether or not this SubEntity is supposed to be visible. */
    virtual bool isVisible(void) const
	{
		return mVisible;
	}

    /** Accessor method to read mesh data.
    */
    Ogre::SubMesh* getSubMesh(void)
	{
		return mSubMesh;
	}

    /** Overridden - see Renderable.
    */
    const Ogre::MaterialPtr& getMaterial(void) const
	{
		return mpMaterial;
	}

    /** Overridden - see Renderable.
    */
    Ogre::Technique* getTechnique(void) const
	{
		return mpMaterial->getBestTechnique(mMaterialLodIndex);
	}

    /** Overridden - see Renderable.
    */
    void getRenderOperation(Ogre::RenderOperation& op);

    /** Overridden - see Renderable.
    */
	void getWorldTransforms(Ogre::Matrix4* xform) const;
    /** @copydoc Renderable::getWorldOrientation */
    const Ogre::Quaternion& getWorldOrientation(void) const;
    /** @copydoc Renderable::getWorldPosition */
    const Ogre::Vector3& getWorldPosition(void) const;
    /** Overridden - see Renderable.
    */
    bool getNormaliseNormals(void) const;      
    /** Overridden - see Renderable.
    */
    unsigned short getNumWorldTransforms(void) const;
    /** Overridden, see Renderable */
    Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const;

    /** @copydoc Renderable::getLights */
    const Ogre::LightList& getLights(void) const;
    /// Get the temporary blended vertex data for this subentity
    const Ogre::VertexData* getBlendedVertexData(void) { return mBlendedVertexData; }
    /** @copydoc Renderable::getCastsShadows */
    bool getCastsShadows(void) const;

public:	

	void endRenderOperation();

#ifdef __OTE

	// ����Ч��
	
	void CreateVertexEffect(const Ogre::String& hlslName, bool isCloneMat = true);

	void ClearEffect();

#endif

protected:
    /** Internal method for preparing this Entity for use in animation. */
    void prepareTempBlendBuffers(void);

protected:

    /// Pointer to parent.
    COTEEntity* mParentEntity;

	void*		m_ParentNode;

    /// Name of Material in use by this SubEntity.
    Ogre::String mMaterialName;

    /// Cached pointer to material.
    Ogre::MaterialPtr mpMaterial;

    // Pointer to the SubMesh defining geometry.
    Ogre::SubMesh* mSubMesh;

    /// Is this SubEntity visible?
    bool mVisible;

    Ogre::SceneDetailLevel mRenderDetail;
	/// The LOD number of the material to use, calculated by Entity::_notifyCurrentCamera
	unsigned short mMaterialLodIndex;

    /// blend buffer details for dedicated geometry
    Ogre::VertexData* mBlendedVertexData;
    /// Quick lookup of buffers
    Ogre::TempBlendedBufferInfo mTempBlendedBuffer;


};


// ****************************************************
// ****************************************************
// �±���һ���Ż�����, ��ͼ�Ѳ���һ����submesh�ϳɵ�һ��

// COTEAutoMesh
// �������Զ��ϳɵ�һ��mesh
// �ѱȽϽӽ�������ͬ���ʵĶ��������һ��ϲ���һ��mesh
// ���������mesh ֻ�൱��ogre �� submesh
// ****************************************************
// ****************************************************

class COTEAutoMesh : public Ogre::SubMesh
{
friend class COTEAutoSubEntity;
public:

	COTEAutoMesh();

	virtual ~COTEAutoMesh()
	{
		SAFE_DELETE(vertexData)
		SAFE_DELETE(indexData)
		m_VertexBuffer.setNull();
		m_IndexBuffer.setNull();
	}
	
	// ��ӵ���β

	bool Append(Ogre::SubMesh* sm, const Ogre::Matrix4* matrix);

	// ������� 
	
	void AppendFinished();

protected:

	OGRE_AUTO_MUTEX

	// ������mesh

	void Create(int poolSize);

	// ���ʵ�� 

	bool AppendImp(Ogre::SubMesh* sm, const Ogre::Matrix4* matrix);

	// ����buffer ָ��

	Ogre::HardwareVertexBufferSharedPtr		m_VertexBuffer;	

	Ogre::HardwareIndexBufferSharedPtr		m_IndexBuffer;	

	Ogre::AxisAlignedBox					m_AABB;	

	// ��¼�ȴ���ӵ�submesh�б�

	typedef std::pair<Ogre::SubMesh*, const Ogre::Matrix4*>	SubMeshMatrix_t;
	std::list<SubMeshMatrix_t>				m_SubMeshMatrixList;

};

// ****************************************************
// COTEAutoSubEntity
// �ڲ���������Ⱦ��������С���ʵ���������Ⱦ
// ****************************************************

class COTEAutoSubEntity : public Ogre::Renderable, public Ogre::MovableObject
{
friend class COTEAutoEntity;
friend class COTEAutoEntityMgr;

public:
	COTEAutoSubEntity(const Ogre::String& name, COTEAutoEntity* parent);
	virtual ~COTEAutoSubEntity(void);

public:	

	virtual void setMaterialName(const Ogre::String& name)
	{
		if(mpMaterial->getName() == name)
			return;

		mpMaterial = Ogre::MaterialManager::getSingleton().getByName(name);

		assert(!mpMaterial.isNull());
	}

    /** Sets the name of the material to be used for this billboard set.
        @returns The name of the material that is used for this set.
    */
    virtual const Ogre::String& getMaterialName(void) const
	{
		if(mpMaterial.isNull())
		{
			static Ogre::String sName = "";
			return sName;
		}
		return mpMaterial->getName();	
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
	virtual const Ogre::AxisAlignedBox& getBoundingBox(void) const;

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
		static Ogre::String sMovableType = "AutoEntity";
		return sMovableType;
	}	

    /** Overridden, see Renderable */
    Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const;

    /** @copydoc Renderable::getLights */
    const Ogre::LightList& getLights(void) const;	
	
	// ���

	bool AppendSubMesh(Ogre::SubMesh* subMesh, const Ogre::Matrix4& worldMatrix);

	// ������� 
	
	void AppendFinished();

protected:

	void CreateAutoMesh();

protected:

	COTEAutoMesh*			m_AutoMesh;	

    Ogre::Real				mBoundingRadius;

	Ogre::MaterialPtr		mpMaterial;	

	Ogre::String			m_Name;

	COTEAutoEntity*			m_Parent;

	unsigned int			m_SubMeshCount;				// �ϲ��˶���submesh������Ϣ��ӡ

	unsigned long			m_FrameCount;				// ������Ⱦ

};

// ****************************************************
// COTEAutoEntity
// COTEAutoSubEntity�Ĺ�����
// �ⲿ���ýӿ���
// ****************************************************

class COTEAutoEntity
{
friend class COTEAutoSubEntity;
friend class COTEAutoEntityMgr;

public:
	
	// װ�غ���

	typedef void (*EntitySetupHndler_t)(const Ogre::AxisAlignedBox& aabb);

public:
	COTEAutoEntity(float x, float y, float z);
	~COTEAutoEntity();

public:	
	
	// ��Ⱦ��Ԫ

	struct RenderEntity_t
	{
		Ogre::String		MeshName;
		Ogre::Matrix4		WorldMatrix;
		Ogre::Quaternion	Rotation;
		long				ProccessTicket;
		Ogre::MeshPtr		MeshResPtr;
	};

public:	

	// ֱ�����mesh��Դ	
	
	void AppendMesh(const Ogre::String& meshName, const Ogre::Matrix4& worldMatrix, const Ogre::Quaternion& rotation);
	
	// ������Ӻ�װ
	
	void SetupMeshes(EntitySetupHndler_t* setupHdler);

	// ��ӵ���Ⱦ����

	void AddToRenderQueue(Ogre::RenderQueue* queue);

	bool IsResLoaded()
	{
		return m_RenderEntityList.empty();
	}

protected:

	// ֱ�����mesh��Դ

	bool AppendMesh(const Ogre::Mesh* mesh, const Ogre::Matrix4& worldMatrix, const Ogre::Quaternion& rotation);

	// ֱ�Ӽ�����Դ

	void SetupRes();

public:
	
	std::map<std::string, COTEAutoSubEntity*>		m_AutoEntityMap;	// <submeshָ��, autoentity>

	std::vector<RenderEntity_t*>					m_RenderEntityList;	// ������Ⱦ��ʵ���б�		

protected:	

	Ogre::Vector3							m_Position;					// λ��	

	Ogre::AxisAlignedBox					m_AABB;						// BOX 

};

// ****************************************************
// COTEAutoEntityMgr
// ������������

// ע�⣺
// ʹ��ʱ �ظ����� AppendMesh/AppendEntity 
// ��������� SetupMeshes
// ****************************************************

class _OTEExport COTEAutoEntityMgr
{
public:
	static COTEAutoEntityMgr*	GetInstance();	
	
	void Init();
		
	// ����

	static void Clear();

	static void Destroy();

	// ���/����

	void AppendMesh(const Ogre::String& meshName, const Ogre::Matrix4& worldMatrix, const Ogre::Quaternion& rotation);

	void AppendMesh(const Ogre::String& meshName, 
		const Ogre::Vector3& position, const Ogre::Vector3& scale, const Ogre::Quaternion& orientation);

	// ������Ӻ�װ

	void SetupMeshes(COTEAutoEntity::EntitySetupHndler_t* setupHdler);

	// ��Ⱦɽ��tileʱ����

	static void RenderTileEntities(
					std::vector<void*>* pObjList,
					Ogre::RenderQueue* queue);

	// ���ڵ���

	void LogMsg();

protected:
	
	COTEAutoEntityMgr();

	void SetupMeshesImp();

	// ��Ⱦ����

	static void OnRenderScene(int id);

private:

	static COTEAutoEntityMgr			s_Inst;

	HashMap<Ogre::String, COTEAutoEntity*>		m_GroupTileMap;

	bool										m_IsInit;

	enum {ePrepaired, eAppendFinished, eLoadFinished};

	int											m_FinishedState; 

	// ��ʵ�尲װ������ĳʵ���б���Ļص�����

	COTEAutoEntity::EntitySetupHndler_t*		m_EntitySetupHandler;

};


#ifdef __ZH

// ****************************************************
// COTERender
// ��Ⱦ��
// ****************************************************

class COTERender
{
public:

	static void FrameStarted(float dTimeSinceLstFrm);

	static void FrameEnded(float dTimeSinceLstFrm);
};

#endif

}

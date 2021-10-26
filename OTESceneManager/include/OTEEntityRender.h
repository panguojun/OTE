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
// 角色渲染相关 

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

	// 附加效果
	
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
// 下边是一种优化策略, 试图把材质一样的submesh合成到一起

// COTEAutoMesh
// 是引擎自动合成的一种mesh
// 把比较接近并且相同材质的对象打组在一起合并成一个mesh
// 不过这里的mesh 只相当于ogre 的 submesh
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
	
	// 添加到结尾

	bool Append(Ogre::SubMesh* sm, const Ogre::Matrix4* matrix);

	// 结束添加 
	
	void AppendFinished();

protected:

	OGRE_AUTO_MUTEX

	// 创建子mesh

	void Create(int poolSize);

	// 添加实现 

	bool AppendImp(Ogre::SubMesh* sm, const Ogre::Matrix4* matrix);

	// 顶点buffer 指针

	Ogre::HardwareVertexBufferSharedPtr		m_VertexBuffer;	

	Ogre::HardwareIndexBufferSharedPtr		m_IndexBuffer;	

	Ogre::AxisAlignedBox					m_AABB;	

	// 记录等待添加的submesh列表

	typedef std::pair<Ogre::SubMesh*, const Ogre::Matrix4*>	SubMeshMatrix_t;
	std::list<SubMeshMatrix_t>				m_SubMeshMatrixList;

};

// ****************************************************
// COTEAutoSubEntity
// 内部创建的渲染对象，用于小物件实体的批量渲染
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
	
	// 添加

	bool AppendSubMesh(Ogre::SubMesh* subMesh, const Ogre::Matrix4& worldMatrix);

	// 结束添加 
	
	void AppendFinished();

protected:

	void CreateAutoMesh();

protected:

	COTEAutoMesh*			m_AutoMesh;	

    Ogre::Real				mBoundingRadius;

	Ogre::MaterialPtr		mpMaterial;	

	Ogre::String			m_Name;

	COTEAutoEntity*			m_Parent;

	unsigned int			m_SubMeshCount;				// 合并了多少submesh用于信息打印

	unsigned long			m_FrameCount;				// 用于渲染

};

// ****************************************************
// COTEAutoEntity
// COTEAutoSubEntity的管理类
// 外部调用接口类
// ****************************************************

class COTEAutoEntity
{
friend class COTEAutoSubEntity;
friend class COTEAutoEntityMgr;

public:
	
	// 装载函数

	typedef void (*EntitySetupHndler_t)(const Ogre::AxisAlignedBox& aabb);

public:
	COTEAutoEntity(float x, float y, float z);
	~COTEAutoEntity();

public:	
	
	// 渲染单元

	struct RenderEntity_t
	{
		Ogre::String		MeshName;
		Ogre::Matrix4		WorldMatrix;
		Ogre::Quaternion	Rotation;
		long				ProccessTicket;
		Ogre::MeshPtr		MeshResPtr;
	};

public:	

	// 直接添加mesh资源	
	
	void AppendMesh(const Ogre::String& meshName, const Ogre::Matrix4& worldMatrix, const Ogre::Quaternion& rotation);
	
	// 结束添加后安装
	
	void SetupMeshes(EntitySetupHndler_t* setupHdler);

	// 添加到渲染队列

	void AddToRenderQueue(Ogre::RenderQueue* queue);

	bool IsResLoaded()
	{
		return m_RenderEntityList.empty();
	}

protected:

	// 直接添加mesh资源

	bool AppendMesh(const Ogre::Mesh* mesh, const Ogre::Matrix4& worldMatrix, const Ogre::Quaternion& rotation);

	// 直接加载资源

	void SetupRes();

public:
	
	std::map<std::string, COTEAutoSubEntity*>		m_AutoEntityMap;	// <submesh指针, autoentity>

	std::vector<RenderEntity_t*>					m_RenderEntityList;	// 用于渲染的实体列表		

protected:	

	Ogre::Vector3							m_Position;					// 位置	

	Ogre::AxisAlignedBox					m_AABB;						// BOX 

};

// ****************************************************
// COTEAutoEntityMgr
// 物件打组管理器

// 注意：
// 使用时 重复调用 AppendMesh/AppendEntity 
// 结束后调用 SetupMeshes
// ****************************************************

class _OTEExport COTEAutoEntityMgr
{
public:
	static COTEAutoEntityMgr*	GetInstance();	
	
	void Init();
		
	// 清理

	static void Clear();

	static void Destroy();

	// 添加/创建

	void AppendMesh(const Ogre::String& meshName, const Ogre::Matrix4& worldMatrix, const Ogre::Quaternion& rotation);

	void AppendMesh(const Ogre::String& meshName, 
		const Ogre::Vector3& position, const Ogre::Vector3& scale, const Ogre::Quaternion& orientation);

	// 结束添加后安装

	void SetupMeshes(COTEAutoEntity::EntitySetupHndler_t* setupHdler);

	// 渲染山体tile时调用

	static void RenderTileEntities(
					std::vector<void*>* pObjList,
					Ogre::RenderQueue* queue);

	// 用于调试

	void LogMsg();

protected:
	
	COTEAutoEntityMgr();

	void SetupMeshesImp();

	// 渲染场景

	static void OnRenderScene(int id);

private:

	static COTEAutoEntityMgr			s_Inst;

	HashMap<Ogre::String, COTEAutoEntity*>		m_GroupTileMap;

	bool										m_IsInit;

	enum {ePrepaired, eAppendFinished, eLoadFinished};

	int											m_FinishedState; 

	// 把实体安装到场景某实体列表里的回调函数

	COTEAutoEntity::EntitySetupHndler_t*		m_EntitySetupHandler;

};


#ifdef __ZH

// ****************************************************
// COTERender
// 渲染器
// ****************************************************

class COTERender
{
public:

	static void FrameStarted(float dTimeSinceLstFrm);

	static void FrameEnded(float dTimeSinceLstFrm);
};

#endif

}

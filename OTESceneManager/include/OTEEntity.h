#pragma once

#include "OTEStaticInclude.h"
#include "OTEEntityRender.h"

namespace OTE
{    
// ***************************************************
// COTEEntity
// 实体对象
// ***************************************************

class _OTEExport COTEEntity	: public Ogre::MovableObject
{
	friend class Ogre::SceneManager;
	friend class COTEEntityPhysicsMgr;
	friend class COTESubEntity;
	friend class COTEActionReactorUsage;
	friend class COTEShadowMap;

	friend class COTESceneMgr;	

public:
    /// Contains the child objects (attached to bones) indexed by name
    typedef std::map<Ogre::String, Ogre::MovableObject*> OTEChildObjectList;
			
	/** List of SubEntities (point to SubMeshes).
    */
    typedef std::vector<COTESubEntity*> OTESubEntityList;
    OTESubEntityList mSubEntityList;
	
	typedef std::set<COTEEntity*> EntitySet;
    
	// constructor

    COTEEntity();

	void init(const Ogre::String& name, Ogre::MeshPtr& mesh);

protected:
    /** Private constructor - specify name (the usual constructor used).
    */
    COTEEntity( const Ogre::String& name, Ogre::MeshPtr& mesh);	

    /** Name of the entity; used for location in the scene.
    */
    Ogre::String mName;

    /** The Mesh that this COTEEntity is based on.
    */
    Ogre::MeshPtr mMesh;

    /// State of animation for animable meshes
    Ogre::AnimationStateSet* mAnimationState;

    /// Shared class-level name for Movable type
    static Ogre::String msMovableType;


    /// Temp blend buffer details for shared geometry
    Ogre::TempBlendedBufferInfo mTempBlendedBuffer;
    /// Temp blend buffer details for shared geometry
    Ogre::VertexData* mSharedBlendedVertexData;

    /** Internal method - given vertex data which could be from the Mesh or 
        any submesh, finds the temporary blend copy. */
    const Ogre::VertexData* findBlendedVertexData(const Ogre::VertexData* orig);
    /** Internal method - given vertex data which could be from the Mesh or 
    any SubMesh, finds the corresponding SubEntity. */
    COTESubEntity* findSubEntityForVertexData(const Ogre::VertexData* orig);

    /** Internal method for extracting metadata out of source vertex data
        for fast assignment of temporary buffers later. */
    void extractTempBufferInfo(Ogre::VertexData* sourceData, Ogre::TempBlendedBufferInfo* info);
    /** Internal method to clone vertex data definitions but to remove blend buffers. */
    Ogre::VertexData* cloneVertexDataRemoveBlendInfo(const Ogre::VertexData* source);
    /** Internal method for preparing this COTEEntity for use in animation. */
    void prepareTempBlendBuffers(void);

    /// Cached bone matrices, including any world transform
    Ogre::Matrix4 *mBoneMatrices;
    unsigned short mNumBoneMatrices;
    /// Records the last frame in which animation was updated
    unsigned long mFrameAnimationLastUpdated;

        /// Perform all the updates required for an animated entity
    void updateAnimation(void);

    /// Records the last frame in which the bones was updated
    /// It's a pointer because it can be shared between different entities with
    /// a shared skeleton.
    unsigned long *mFrameBonesLastUpdated;
    
    /// Private method to cache bone matrices from skeleton
    void cacheBoneMatrices(void);

    /// Flag determines whether or not to display skeleton
    bool mDisplaySkeleton;
    /// Flag indicating whether hardware skinning is supported by this entities materials
    bool mHardwareSkinning;
    /// Flag indicating whether we have a vertex program in use on any of our subentities
    bool mVertexProgramInUse;


    /** This COTEEntity's personal copy of the skeleton, if skeletally animated
    */
    Ogre::SkeletonInstance* mSkeletonInstance;

	/** Builds a list of SubEntities based on the SubMeshes contained in the Mesh. */
	void buildSubEntityList(Ogre::MeshPtr& mesh, OTESubEntityList* sublist);

	//Clear the list of SubEntities
	void clearSubEntityList();		

    /// Trigger reevaluation of the kind of vertex processing in use
    void reevaluateVertexProcessing(void);

protected:
    OTEChildObjectList mChildObjectList;

	/// Bounding box that 'contains' all the mesh of each child entity
	Ogre::AxisAlignedBox *mFullBoundingBox;

    bool mNormaliseNormals;
    
public:
    /** Default destructor.
    */
    virtual ~COTEEntity();

    /** Gets the Mesh that this COTEEntity is based on.
    */
    Ogre::MeshPtr& getMesh(void);

    /** Gets a pointer to a SubEntity, ie a part of an COTEEntity.
    */
    COTESubEntity* getSubEntity(unsigned int index);

	/** Gets a pointer to a SubEntity by name
		@remarks - names should be initialized during a Mesh creation.
	*/
	COTESubEntity* getSubEntity( const Ogre::String& name );

    /** Retrieves the number of SubEntity objects making up this entity.
    */
    unsigned int getNumSubEntities(void) const;

    /** Sets the material to use for the whole of this entity.
        @remarks
            This is a shortcut method to set all the materials for all
            subentities of this entity. Only use this method is you want to
            set the same material for all subentities or if you know there
            is only one. Otherwise call getSubEntity() and call the same
            method on the individual SubEntity.
    */
    void setMaterialName(const Ogre::String& name);

    /** Overridden - see MovableObject.
    */
    void _notifyCurrentCamera(Ogre::Camera* cam);

    /// Overridden - see MovableObject.
    void setRenderQueueGroup(Ogre::RenderQueueGroupID queueID);
    
    /** Overridden - see MovableObject.
    */
    const Ogre::AxisAlignedBox& getBoundingBox(void) const;

    /// merge all the child object Bounds a return it
	Ogre::AxisAlignedBox getChildObjectsBoundingBox(void) const;

    /** Overridden - see MovableObject.
    */
    void _updateRenderQueue(Ogre::RenderQueue* queue);

    /** Overridden from MovableObject */
    const Ogre::String& getName(void) const;

	void setName(const Ogre::String& name){mName = name;} // romeo

    /** Overridden from MovableObject */
    const Ogre::String& getMovableType(void) const;

    /** For entities based on animated meshes, gets the AnimationState object for a single animation.
    @remarks
        You animate an entity by updating the animation state objects. Each of these represents the
        current state of each animation available to the entity. The AnimationState objects are
        initialised from the Mesh object.
    */
    Ogre::AnimationState* getAnimationState(const Ogre::String& name);
    /** For entities based on animated meshes, gets the AnimationState objects for all animations.
    @remarks
        You animate an entity by updating the animation state objects. Each of these represents the
        current state of each animation available to the entity. The AnimationState objects are
        initialised from the Mesh object.
    */
    Ogre::AnimationStateSet* getAllAnimationStates(void);

    /** Tells the COTEEntity whether or not it should display it's skeleton, if it has one.
    */
    void setDisplaySkeleton(bool display);

    /** @see MovableObject::getBoundingRadius */
	Ogre::Real getBoundingRadius(void) const;
    /** If set to true, this forces normals of this entity to be normalised
        dynamically by the hardware.
    @remarks
        This option can be used to prevent lighting variations when scaling an
        COTEEntity using a SceneNode - normally because this scaling is hardware
        based, the normals get scaled too which causes lighting to become inconsistent.
        However, this has an overhead so only do this if you really need to.
    */
    void setNormaliseNormals(bool normalise) { mNormaliseNormals = normalise; }

    /** Returns true if this entity has auto-normalisation of normals set. */
    bool getNormaliseNormals(void) const {return mNormaliseNormals; }


	/** Internal method for retrieving bone matrix information. */
	const Ogre::Matrix4* _getBoneMatrices(void) { return mBoneMatrices;}
	/** Internal method for retrieving bone matrix information. */
    unsigned short _getNumBoneMatrices(void) { return mNumBoneMatrices; }
    /** Returns whether or not this entity is skeletally animated. */
    bool hasSkeleton(void) { return mSkeletonInstance != 0; }
    /** Get this COTEEntity's personal skeleton instance. */
    Ogre::SkeletonInstance* getSkeleton(void) { return mSkeletonInstance; }
    /** Returns whether or not hardware skinning is enabled.
    @remarks
        Because fixed-function indexed vertex blending is rarely supported
        by existing graphics cards, hardware skinning can only be done if
        the vertex programs in the materials used to render an entity support
        it. Therefore, this method will only return true if all the materials
        assigned to this entity have vertex programs assigned, and all those
        vertex programs must support 'include_skeletal_animation true'.
    */
    bool isHardwareSkinningEnabled(void) { return mHardwareSkinning; }

    /** Overridden from MovableObject */
    void _notifyAttached(Ogre::Node* parent, bool isTagPoint = false);

	void ManualSetBounding( const Ogre::AxisAlignedBox& aabb)
	{
		*mFullBoundingBox = aabb;
	}	

	// 给每个实体以独立的材质

	void CloneMaterials();
	
	void SetupEntityRes();

	// 加载光照图 

	bool CheckLightmapValid();

	void LoadLightmap(const std::string& rLightmap);

	// 顶点色

	void ReadVertexColorData(const Ogre::String& fileName);

};

// 检查是否支持光照图

#define  CHECK_LM_VALID(entity)		(entity)->getMesh()->getSubMesh(0)->vertexData->vertexDeclaration->findElementBySemantic(VES_TEXTURE_COORDINATES, 1)


#ifdef __ZH

// ****************************************************
// COTESceneMgr
// 一个简单的场景管理器
// ****************************************************
class COTESceneMgr
{
public:

	COTEEntity* CreateEntity(
		const Ogre::String& File, const Ogre::String& Name,
		const Ogre::Vector3& rPosition = Ogre::Vector3::ZERO,
		const Ogre::Quaternion& rQuaternion = Ogre::Quaternion::IDENTITY,
		const Ogre::Vector3& rScaling = Ogre::Vector3::UNIT_SCALE,
		const std::string& Group = "default",
		bool  vAttachToSceneNode = true);

	void AttachObjToSceneNode(Ogre::MovableObject* MO, const std::string& rNodeName, const std::string& Group = "default");

	void ClearScene();

public:

	// 实体列表

	std::map<Ogre::String, COTEEntity* >		m_EntityList;

	static		COTESceneMgr					s_Inst;

};

#endif

}

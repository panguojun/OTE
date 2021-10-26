#pragma once
#include "OTEStaticInclude.h"
#include "OgreMovableObject.h"
#include "OgreRenderable.h"
#include "OTEActorEntity.h"

namespace OTE
{

// ***********************************************************
// class COTEBillBoardChain
// 着各类是从Ogre源代码里 "拿来"的, 不过修改了不少 以适应需求
// ***********************************************************

class _OTEExport COTEBillBoardChain : public Ogre::MovableObject, public Ogre::Renderable
{
public:

	/** Contains the data of an element of the COTEBillBoardChain.
	*/
	class _OTEExport Element
	{

	public:

		Element();

		Element(Ogre::Vector3 position,
			Ogre::Real width,
			Ogre::Real texCoord,
			Ogre::ColourValue colour);

		Ogre::Vector3 position;
		Ogre::Real width;
		/// U or V texture coord depending on options
		Ogre::Real texCoord;
		Ogre::ColourValue colour;

	};
	typedef std::vector<Element> ElementList;

	/** Constructor (don't use directly, use factory) 
	@param name The name to give this object
	@param maxElements The maximum number of elements per chain
	@param numberOfChains The number of separate chain segments contained in this object
	@param useTextureCoords If true, use texture coordinates from the chain elements
	@param useVertexColours If true, use vertex colours from the chain elements
	@param dynamic If true, buffers are created with the intention of being updated
	*/
	COTEBillBoardChain(const Ogre::String& name, size_t maxElements = 20, size_t numberOfChains = 1, 
		bool useTextureCoords = true, bool useColours = true, bool dynamic = true);
	/// destructor
	virtual ~COTEBillBoardChain();

	/** Set the maximum number of chain elements per chain 
	*/
	virtual void setMaxChainElements(size_t maxElements);
	/** Get the maximum number of chain elements per chain 
	*/
	virtual size_t getMaxChainElements(void) const { return mMaxElementsPerChain; }
	/** Set the number of chain segments (this class can render multiple chains
		at once using the same material). 
	*/
	virtual void setNumberOfChains(size_t numChains);
	/** Get the number of chain segments (this class can render multiple chains
	at once using the same material). 
	*/
	virtual size_t getNumberOfChains(void) const { return mChainCount; }

	/** Sets whether texture coordinate information should be included in the
		final buffers generated.
	@note You must use either texture coordinates or vertex colour since the
		vertices have no normals and without one of these there is no source of
		colour for the vertices.
	*/
	virtual void setUseTextureCoords(bool use);
	/** Gets whether texture coordinate information should be included in the
		final buffers generated.
	*/
	virtual bool getUseTextureCoords(void) const { return mUseTexCoords; }
	
	virtual const Ogre::Quaternion& getWorldOrientation(void) const;
   
	virtual const Ogre::Vector3& getWorldPosition(void) const;

	virtual const Ogre::String& getName(void) const	 
	{
		return m_Name;
	}
	/** The direction in which texture coordinates from elements of the
		chain are used.
	*/
	enum TexCoordDirection
	{
		/// Tex coord in elements is treated as the 'u' texture coordinate
		TCD_U,
		/// Tex coord in elements is treated as the 'v' texture coordinate
		TCD_V
	};
	/** Sets the direction in which texture coords specified on each element
		are deemed to run along the length of the chain.
	@param dir The direction, default is TCD_U.
	*/
	virtual void setTextureCoordDirection(TexCoordDirection dir);
	/** Gets the direction in which texture coords specified on each element
		are deemed to run.
	*/
	virtual TexCoordDirection getTextureCoordDirection(void) { return mTexCoordDir; }

	/** Set the range of the texture coordinates generated across the width of
		the chain elements.
	@param start Start coordinate, default 0.0
	@param end End coordinate, default 1.0
	*/
	virtual void setOtherTextureCoordRange(Ogre::Real start, Ogre::Real end);
	/** Get the range of the texture coordinates generated across the width of
		the chain elements.
	*/
	virtual const Ogre::Real* getOtherTextureCoordRange(void) const { return mOtherTexCoordRange; }

	/** Sets whether vertex colour information should be included in the
		final buffers generated.
	@note You must use either texture coordinates or vertex colour since the
		vertices have no normals and without one of these there is no source of
		colour for the vertices.
	*/
	virtual void setUseVertexColours(bool use);
	/** Gets whether vertex colour information should be included in the
		final buffers generated.
	*/
	virtual bool getUseVertexColours(void) const { return mUseVertexColour; }

	/** Sets whether or not the buffers created for this object are suitable
		for dynamic alteration.
	*/
	virtual void setDynamic(bool dyn);

	/** Gets whether or not the buffers created for this object are suitable
		for dynamic alteration.
	*/
	virtual bool getDynamic(void) const { return mDynamic; }
	
	/** Add an element to the 'head' of a chain.
	@remarks
		If this causes the number of elements to exceed the maximum elements
		per chain, the last element in the chain (the 'tail') will be removed
		to allow the additional element to be added.
	@param chainIndex The index of the chain
	@param billboardChainElement The details to add
	*/
	virtual void addChainElement(size_t chainIndex, 
		const Element& billboardChainElement);
	/** Remove an element from the 'tail' of a chain.
	@param chainIndex The index of the chain
	*/
	virtual void removeChainElement(size_t chainIndex);
	/** Update the details of an existing chain element.
	@param chainIndex The index of the chain
	@param elementIndex The element index within the chain, measured from 
		the 'head' of the chain
	@param billboardChainElement The details to set
	*/
	virtual void updateChainElement(size_t chainIndex, size_t elementIndex, 
		const Element& billboardChainElement);
	/** Get the detail of a chain element.
	@param chainIndex The index of the chain
	@param elementIndex The element index within the chain, measured from
		the 'head' of the chain
	*/
	virtual Element& getChainElement(size_t chainIndex, size_t elementIndex);

	/** Returns the number of chain elements. */
	virtual size_t getNumChainElements(size_t chainIndex) const;

	/** Remove all elements of a given chain (but leave the chain intact). */
	virtual void clearChain(size_t chainIndex);
	/** Remove all elements from all chains (but leave the chains themselves intact). */
	virtual void clearAllChains(void);

	/// Get the material name in use
	virtual const Ogre::String& getMaterialName(void) const { return mMaterialName; }
	/// Set the material name to use for rendering
	virtual void setMaterialName(const Ogre::String& name);


	// Overridden members follow
	void _notifyCurrentCamera(Ogre::Camera* cam);
	Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const;
	Ogre::Real getBoundingRadius(void) const;
	const Ogre::AxisAlignedBox& getBoundingBox(void) const;
	const Ogre::MaterialPtr& getMaterial(void) const;
	const Ogre::String& getMovableType(void) const;
	void _updateRenderQueue(Ogre::RenderQueue *);
	void getRenderOperation(Ogre::RenderOperation &);
	void getWorldTransforms(Ogre::Matrix4 *) const;
	const Ogre::LightList& getLights(void) const;
	
	// 管理

	typedef std::map<Ogre::String, COTEBillBoardChain* > OTEBillBoardChainList_t;

    /** for easy memory management and lookup.
    */
    static OTEBillBoardChainList_t s_BillBoardChainSets;

	// 创建/删除接口

	static COTEBillBoardChain* Create(const std::string& name, bool cloneMat = true);

	static void Clear();

	static void Delete(const std::string& name);

protected:

	/// Maximum length of each chain
	size_t mMaxElementsPerChain;
	/// Number of chains
	size_t mChainCount;
	/// Use texture coords?
	bool mUseTexCoords;
	/// Use vertex colour?
	bool mUseVertexColour;
	/// Dynamic use?
	bool mDynamic;
	/// Vertex data
	Ogre::VertexData* mVertexData;
	/// Index data (to allow multiple unconnected chains)
	Ogre::IndexData* mIndexData;
	/// Is the vertex declaration dirty?
	bool mVertexDeclDirty;
	/// Do the buffers need recreating?
	bool mBuffersNeedRecreating;
	/// Do the bounds need redefining?
	mutable bool mBoundsDirty;
	/// Is the index buffer dirty?
	bool mIndexContentDirty;
	/// AABB
	mutable Ogre::AxisAlignedBox mAABB;
	/// Bounding radius
	mutable Ogre::Real mRadius;
	/// Material 
	Ogre::String mMaterialName;
	Ogre::MaterialPtr mMaterial;
	/// Texture coord direction
	TexCoordDirection mTexCoordDir;
	/// Other texture coord range
	Ogre::Real mOtherTexCoordRange[2];

	Ogre::String	m_Name;


	/// The list holding the chain elements
	ElementList mChainElementList;

	/** Simple struct defining a chain segment by referencing a subset of
		the preallocated buffer (which will be mMaxElementsPerChain * mChainCount
		long), by it's chain index, and a head and tail value which describe
		the current chain. The buffer subset wraps at mMaxElementsPerChain
		so that head and tail can move freely. head and tail are inclusive,
		when the chain is empty head and tail are filled with high-values.
	*/
	struct ChainSegment
	{
		/// The start of this chains subset of the buffer
		size_t start;
		/// The 'head' of the chain, relative to start
		size_t head;
		/// The 'tail' of the chain, relative to start
		size_t tail;
	};
	typedef std::vector<ChainSegment> ChainSegmentList;
	ChainSegmentList mChainSegmentList;

	/// Setup the STL collections
	virtual void setupChainContainers(void);
	/// Setup vertex declaration
	virtual void setupVertexDeclaration(void);
	// Setup buffers
	virtual void setupBuffers(void);
	/// Update the contents of the vertex buffer
	virtual void updateVertexBuffer(Ogre::Camera* cam);
	/// Update the contents of the index buffer
	virtual void updateIndexBuffer(void);
	virtual void updateBoundingBox(void) const;

	/// Chain segment has no elements
	static const size_t SEGMENT_EMPTY;
};


// **************************************
// COTEBillBoardChainFactory
// **************************************
class COTEBillBoardChainFactory : public CMagicFactoryBase
{
public:

	virtual Ogre::MovableObject* Create(const std::string& ResName,	const std::string& Name, bool cloneMat);

	virtual void Delete(const std::string& Name);

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "条带特效";
	}
};

} 
#include "otebillboardchain.h"
#include "OTEQTSceneManager.h"
#include "OTED3DManager.h"

using namespace Ogre;
using namespace OTE;

const size_t COTEBillBoardChain::SEGMENT_EMPTY = 0xffffffff;
COTEBillBoardChain::OTEBillBoardChainList_t COTEBillBoardChain::s_BillBoardChainSets;
//-----------------------------------------------------------------------
COTEBillBoardChain::Element::Element()
{
}
//-----------------------------------------------------------------------
COTEBillBoardChain::Element::Element(Vector3 _position,
	Real _width,
	Real _texCoord,
	ColourValue _colour) :
position(_position),
	width(_width),
	texCoord(_texCoord),
	colour(_colour)
{
}
//-----------------------------------------------------------------------
COTEBillBoardChain::COTEBillBoardChain(const String& name, size_t maxElements,
	size_t numberOfChains, bool useTextureCoords, bool useColours, bool dynamic) :		
	mMaxElementsPerChain(maxElements),
	mChainCount(numberOfChains),
	mUseTexCoords(useTextureCoords),
	mUseVertexColour(useColours),
	mDynamic(dynamic),
	mVertexDeclDirty(true),
	mBuffersNeedRecreating(true),
	mBoundsDirty(true),
	mIndexContentDirty(true),
	mRadius(0.0f),
	mTexCoordDir(TCD_U),
	m_Name(name)
{	
	mVertexData = new VertexData();
	mIndexData = new IndexData();

	mOtherTexCoordRange[0] = 0.0f;
	mOtherTexCoordRange[1] = 1.0f;

	setupChainContainers();

	mVertexData->vertexStart = 0;
	// index data set up later
	// set basic white material
	this->setMaterialName("BillBoardChainMat");

	mAABB.setExtents(-5.0f, -5.0f, -5.0f, 5.0f, 5.0f, 5.0f);

}
//-----------------------------------------------------------------------
COTEBillBoardChain::~COTEBillBoardChain()
{
	SAFE_DELETE (mVertexData)
	SAFE_DELETE (mIndexData)
}
//-----------------------------------------------------------------------
void COTEBillBoardChain::setupChainContainers(void)
{
	// Allocate enough space for everything
	mChainElementList.resize(mChainCount * mMaxElementsPerChain);
	mVertexData->vertexCount = mChainElementList.size() * 2;

	// Configure chains
	mChainSegmentList.resize(mChainCount);
	for (size_t i = 0; i < mChainCount; ++i)
	{
		ChainSegment& seg = mChainSegmentList[i];
		seg.start = i * mMaxElementsPerChain;
		seg.tail = seg.head = SEGMENT_EMPTY;

	}


}
//-----------------------------------------------------------------------
void COTEBillBoardChain::setupVertexDeclaration(void)
{
	if (mVertexDeclDirty)
	{
		VertexDeclaration* decl = mVertexData->vertexDeclaration;
		//decl->removeAllElements(); // ???

		size_t offset = 0;
		// Add a description for the buffer of the positions of the vertices
		decl->addElement(0, offset, VET_FLOAT3, VES_POSITION);
		offset += VertexElement::getTypeSize(VET_FLOAT3);

		if (mUseVertexColour)
		{
			decl->addElement(0, offset, VET_COLOUR, VES_DIFFUSE);
			offset += VertexElement::getTypeSize(VET_COLOUR);
		}

		if (mUseTexCoords)
		{
			decl->addElement(0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES);
			offset += VertexElement::getTypeSize(VET_FLOAT2);
		}

		if (!mUseTexCoords && !mUseVertexColour)
		{
			OTE_MSG_ERR("COTEBillBoardChain::setupVertexDeclaration 调用失败")
		}
		mVertexDeclDirty = false;
	}
}
//-----------------------------------------------------------------------
void COTEBillBoardChain::setupBuffers(void)
{
	setupVertexDeclaration();
	if (mBuffersNeedRecreating)
	{
		// Create the vertex buffer (always dynamic due to the camera adjust)
		HardwareVertexBufferSharedPtr pBuffer =
			HardwareBufferManager::getSingleton().createVertexBuffer(
			mVertexData->vertexDeclaration->getVertexSize(0),
			mVertexData->vertexCount,
			HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

		// (re)Bind the buffer
		// Any existing buffer will lose its reference count and be destroyed
		mVertexData->vertexBufferBinding->setBinding(0, pBuffer);

		mIndexData->indexBuffer =
			HardwareBufferManager::getSingleton().createIndexBuffer(
				HardwareIndexBuffer::IT_16BIT,
				mChainCount * mMaxElementsPerChain * 6, // max we can use
				mDynamic? HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY : HardwareBuffer::HBU_STATIC_WRITE_ONLY);
		// NB we don't set the indexCount on IndexData here since we will
		// probably use less than the maximum number of indices

		mBuffersNeedRecreating = false;
	}
}
//-----------------------------------------------------------------------
void COTEBillBoardChain::setMaxChainElements(size_t maxElements)
{
	mMaxElementsPerChain = maxElements;
	setupChainContainers();
    mBuffersNeedRecreating = mIndexContentDirty = true;
}
//-----------------------------------------------------------------------
void COTEBillBoardChain::setNumberOfChains(size_t numChains)
{
	mChainCount = numChains;
	setupChainContainers();
    mBuffersNeedRecreating = mIndexContentDirty = true;
}
//-----------------------------------------------------------------------
void COTEBillBoardChain::setUseTextureCoords(bool use)
{
	mUseTexCoords = use;
	mVertexDeclDirty = mBuffersNeedRecreating = true;
    mIndexContentDirty = true;
}
//-----------------------------------------------------------------------
void COTEBillBoardChain::setTextureCoordDirection(COTEBillBoardChain::TexCoordDirection dir)
{
	mTexCoordDir = dir;
}
//-----------------------------------------------------------------------
void COTEBillBoardChain::setOtherTextureCoordRange(Real start, Real end)
{
	mOtherTexCoordRange[0] = start;
	mOtherTexCoordRange[1] = end;
}
//-----------------------------------------------------------------------
void COTEBillBoardChain::setUseVertexColours(bool use)
{
	mUseVertexColour = use;
	mVertexDeclDirty = mBuffersNeedRecreating = true;
    mIndexContentDirty = true;
}
//-----------------------------------------------------------------------
void COTEBillBoardChain::setDynamic(bool dyn)
{
	mDynamic = dyn;
	mBuffersNeedRecreating = mIndexContentDirty = true;
}
//-----------------------------------------------------------------------
const Quaternion& COTEBillBoardChain::getWorldOrientation(void) const
{
	return mParentNode->_getDerivedOrientation();
}

const Vector3& COTEBillBoardChain::getWorldPosition(void) const
{
	return mParentNode->_getDerivedPosition();
}

//-----------------------------------------------------------------------
void COTEBillBoardChain::addChainElement(size_t chainIndex,
	const COTEBillBoardChain::Element& dtls)
{

	if (chainIndex >= mChainCount)
	{
		OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
			"chainIndex out of bounds",
			"COTEBillBoardChain::addChainElement");
	}
	ChainSegment& seg = mChainSegmentList[chainIndex];
	if (seg.head == SEGMENT_EMPTY)
	{
		// Tail starts at end, head grows backwards
		seg.tail = mMaxElementsPerChain - 1;
		seg.head = seg.tail;
		mIndexContentDirty = true;
	}
	else
	{
		if (seg.head == 0)
		{
			// Wrap backwards
			seg.head = mMaxElementsPerChain - 1;
		}
		else
		{
			// Just step backward
			--seg.head;
		}
		// Run out of elements?
		if (seg.head == seg.tail)
		{
			// Move tail backwards too, losing the end of the segment and re-using
			// it in the head
			if (seg.tail == 0)
				seg.tail = mMaxElementsPerChain - 1;
			else
				--seg.tail;
		}
	}

	// Set the details
	mChainElementList[seg.start + seg.head] = dtls;

	mIndexContentDirty = true;
	mBoundsDirty = true;
	// tell parent node to update bounds
	if (mParentNode)
		mParentNode->needUpdate();

}
//-----------------------------------------------------------------------
void COTEBillBoardChain::removeChainElement(size_t chainIndex)
{
	if (chainIndex >= mChainCount)
	{
		OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
			"chainIndex out of bounds",
			"COTEBillBoardChain::removeChainElement");
	}
	ChainSegment& seg = mChainSegmentList[chainIndex];
	if (seg.head == SEGMENT_EMPTY)
		return; // do nothing, nothing to remove


	if (seg.tail == seg.head)
	{
		// last item
		seg.head = seg.tail = SEGMENT_EMPTY;
	}
	else if (seg.tail == 0)
	{
		seg.tail = mMaxElementsPerChain - 1;
	}
	else
	{
		--seg.tail;
	}

	// we removed an entry so indexes need updating
	mIndexContentDirty = true;
	mBoundsDirty = true;
	// tell parent node to update bounds
	if (mParentNode)
		mParentNode->needUpdate();

}
//-----------------------------------------------------------------------
void COTEBillBoardChain::clearChain(size_t chainIndex)
{
	if (chainIndex >= mChainCount)
	{
		OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
			"chainIndex out of bounds",
			"COTEBillBoardChain::clearChain");
	}
	ChainSegment& seg = mChainSegmentList[chainIndex];

	// Just reset head & tail
	seg.tail = seg.head = SEGMENT_EMPTY;

	// we removed an entry so indexes need updating
	mIndexContentDirty = true;
	mBoundsDirty = true;
	// tell parent node to update bounds
	if (mParentNode)
		mParentNode->needUpdate();

}
//-----------------------------------------------------------------------
void COTEBillBoardChain::clearAllChains(void)
{
	for (size_t i = 0; i < mChainCount; ++i)
	{
		clearChain(i);
	}

}
//-----------------------------------------------------------------------
void COTEBillBoardChain::updateChainElement(size_t chainIndex, size_t elementIndex,
	const COTEBillBoardChain::Element& dtls)
{
	if (chainIndex >= mChainCount)
	{
		OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
			"chainIndex out of bounds",
			"COTEBillBoardChain::updateChainElement");
	}
	ChainSegment& seg = mChainSegmentList[chainIndex];
	if (seg.head == SEGMENT_EMPTY)
	{
		OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
			"Chain segment is empty",
			"COTEBillBoardChain::updateChainElement");
	}

	size_t idx = seg.head + elementIndex;
	// adjust for the edge and start
	idx = (idx % mMaxElementsPerChain) + seg.start;

	mChainElementList[idx] = dtls;

	mBoundsDirty = true;
	// tell parent node to update bounds
	if (mParentNode)
		mParentNode->needUpdate();


}
//-----------------------------------------------------------------------
COTEBillBoardChain::Element&
COTEBillBoardChain::getChainElement(size_t chainIndex, size_t elementIndex)
{

	if (chainIndex >= mChainCount)
	{
		OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
			"chainIndex out of bounds",
			"COTEBillBoardChain::getChainElement");
	}
	const ChainSegment& seg = mChainSegmentList[chainIndex];

	size_t idx = seg.head + elementIndex;
	// adjust for the edge and start
	idx = (idx % mMaxElementsPerChain) + seg.start;

	return mChainElementList[idx];
}
//-----------------------------------------------------------------------
size_t COTEBillBoardChain::getNumChainElements(size_t chainIndex) const
{
	if (chainIndex >= mChainCount)
	{
		OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
			"chainIndex out of bounds",
			"COTEBillBoardChain::getNumChainElements");
	}
	const ChainSegment& seg = mChainSegmentList[chainIndex];

	return seg.tail - seg.head + 1;
}
//-----------------------------------------------------------------------
void COTEBillBoardChain::updateBoundingBox(void) const
{
	if (mBoundsDirty)
	{
		mAABB.setNull();
		Vector3 widthVector;
		for (ChainSegmentList::const_iterator segi = mChainSegmentList.begin();
			segi != mChainSegmentList.end(); ++segi)
		{
			const ChainSegment& seg = *segi;

			if (seg.head != SEGMENT_EMPTY)
			{

				for(size_t e = seg.head; ; ++e) // until break
				{
					// Wrap forwards
					if (e == mMaxElementsPerChain)
						e = 0;

					const Element& elem = mChainElementList[seg.start + e];

					widthVector.x = widthVector.y = widthVector.z = elem.width;
					mAABB.merge(elem.position - widthVector);
					mAABB.merge(elem.position + widthVector);

					if (e == seg.tail)
						break;

				}
			}

		}

		// Set the current radius
		if (mAABB.isNull())
		{
			mRadius = 0.0f;
		}
		else
		{
			mRadius = Math::Sqrt(
			std::max(mAABB.getMinimum().squaredLength(),
					mAABB.getMaximum().squaredLength())
				);
		}

		mBoundsDirty = false;
	}
}
//-----------------------------------------------------------------------
void COTEBillBoardChain::updateVertexBuffer(Camera* cam)
{
	setupBuffers();
	HardwareVertexBufferSharedPtr pBuffer =
		mVertexData->vertexBufferBinding->getBuffer(0);
	void* pBufferStart = pBuffer->lock(HardwareBuffer::HBL_DISCARD);

	const Vector3& camPos = cam->getDerivedPosition();
	Vector3 eyePos = mParentNode->_getDerivedOrientation().Inverse() *
		(camPos - mParentNode->_getDerivedPosition()) / mParentNode->_getDerivedScale();

	Vector3 chainTangent;
	for (ChainSegmentList::iterator segi = mChainSegmentList.begin();
		segi != mChainSegmentList.end(); ++segi)
	{
		ChainSegment& seg = *segi;

		// Skip 0 or 1 element segment counts
		if (seg.head != SEGMENT_EMPTY && seg.head != seg.tail)
		{
			size_t laste = seg.head;
			for (size_t e = seg.head; ; ++e) // until break
			{
				// Wrap forwards
				if (e == mMaxElementsPerChain)
					e = 0;

				Element& elem = mChainElementList[e + seg.start];
				assert (((e + seg.start) * 2) < 65536 && "Too many elements!");
				uint16 baseIdx = static_cast<uint16>((e + seg.start) * 2);

				// Determine base pointer to vertex #1
				void* pBase = static_cast<void*>(
					static_cast<char*>(pBufferStart) +
						pBuffer->getVertexSize() * baseIdx);

				// Get index of next item
				size_t nexte = e + 1;
				if (nexte == mMaxElementsPerChain)
					nexte = 0;

				if (e == seg.head)
				{
					// No laste, use next item
					chainTangent = mChainElementList[nexte + seg.start].position - elem.position;
				}
				else if (e == seg.tail)
				{
					// No nexte, use only last item
					chainTangent = elem.position - mChainElementList[laste + seg.start].position;
				}
				else
				{
					// A mid position, use tangent across both prev and next
					chainTangent = mChainElementList[nexte + seg.start].position - mChainElementList[laste + seg.start].position;

				}

				Vector3 vP1ToEye = eyePos - elem.position;
				Vector3 vPerpendicular = chainTangent.crossProduct(vP1ToEye);
				vPerpendicular.normalise();
				vPerpendicular *= (elem.width * 0.5);

				Vector3 pos0 = elem.position - vPerpendicular;
				Vector3 pos1 = elem.position + vPerpendicular;

				float* pFloat = static_cast<float*>(pBase);
				// pos1
				*pFloat++ = pos0.x;
				*pFloat++ = pos0.y;
				*pFloat++ = pos0.z;

				pBase = static_cast<void*>(pFloat);

				if (mUseVertexColour)
				{
					RGBA* pCol = static_cast<RGBA*>(pBase);
					Root::getSingleton().convertColourValue(elem.colour, pCol);
					pCol++;
					pBase = static_cast<void*>(pCol);
				}

				if (mUseTexCoords)
				{
					pFloat = static_cast<float*>(pBase);
					if (mTexCoordDir == TCD_U)
					{
						*pFloat++ = elem.texCoord;
						*pFloat++ = mOtherTexCoordRange[0];
					}
					else
					{
						*pFloat++ = mOtherTexCoordRange[0];
						*pFloat++ = elem.texCoord;
					}
					pBase = static_cast<void*>(pFloat);
				}

				// pos2
				pFloat = static_cast<float*>(pBase);
				*pFloat++ = pos1.x;
				*pFloat++ = pos1.y;
				*pFloat++ = pos1.z;
				pBase = static_cast<void*>(pFloat);

				if (mUseVertexColour)
				{
					RGBA* pCol = static_cast<RGBA*>(pBase);
					Root::getSingleton().convertColourValue(elem.colour, pCol);
					pCol++;
					pBase = static_cast<void*>(pCol);
				}

				if (mUseTexCoords)
				{
					pFloat = static_cast<float*>(pBase);
					if (mTexCoordDir == TCD_U)
					{
						*pFloat++ = elem.texCoord;
						*pFloat++ = mOtherTexCoordRange[1];
					}
					else
					{
						*pFloat++ = mOtherTexCoordRange[1];
						*pFloat++ = elem.texCoord;
					}
					pBase = static_cast<void*>(pFloat);
				}

				if (e == seg.tail)
					break; // last one

				laste = e;

			} // element
		} // segment valid?

	} // each segment



	pBuffer->unlock();


}
//-----------------------------------------------------------------------
void COTEBillBoardChain::updateIndexBuffer(void)
{

	setupBuffers();
	if (mIndexContentDirty)
	{

		uint16* pShort = static_cast<uint16*>(
			mIndexData->indexBuffer->lock(HardwareBuffer::HBL_DISCARD));
		mIndexData->indexCount = 0;
		// indexes
		for (ChainSegmentList::iterator segi = mChainSegmentList.begin();
			segi != mChainSegmentList.end(); ++segi)
		{
			ChainSegment& seg = *segi;

			// Skip 0 or 1 element segment counts
			if (seg.head != SEGMENT_EMPTY && seg.head != seg.tail)
			{
				// Start from head + 1 since it's only useful in pairs
				size_t laste = seg.head;
				while(1) // until break
				{
					size_t e = laste + 1;
					// Wrap forwards
					if (e == mMaxElementsPerChain)
						e = 0;
					// indexes of this element are (e * 2) and (e * 2) + 1
					// indexes of the last element are the same, -2
					assert (((e + seg.start) * 2) < 65536 && "Too many elements!");
					uint16 baseIdx = static_cast<uint16>((e + seg.start) * 2);
					uint16 lastBaseIdx = static_cast<uint16>((laste + seg.start) * 2);
					*pShort++ = lastBaseIdx;
					*pShort++ = lastBaseIdx + 1;
					*pShort++ = baseIdx;
					*pShort++ = lastBaseIdx + 1;
					*pShort++ = baseIdx + 1;
					*pShort++ = baseIdx;

					mIndexData->indexCount += 6;


					if (e == seg.tail)
						break; // last one

					laste = e;

				}
			}

		}
		mIndexData->indexBuffer->unlock();

		mIndexContentDirty = false;
	}

}
//-----------------------------------------------------------------------
void COTEBillBoardChain::_notifyCurrentCamera(Camera* cam)
{
	// 反应器更新

	if(m_BindReactor && !(((CReactorBase*)m_BindReactor)->m_StateFlag & 0xFF000000))
	{
		((CReactorBase*)m_BindReactor)->Update(this, COTED3DManager::s_FrameTime);		
	}

	updateVertexBuffer(cam);
}
//-----------------------------------------------------------------------
Real COTEBillBoardChain::getSquaredViewDepth(const Camera* cam) const
{
	Vector3 min, max, mid, dist;
	min = mAABB.getMinimum();
	max = mAABB.getMaximum();
	mid = ((max - min) * 0.5) + min;
	dist = cam->getDerivedPosition() - mid;

	return dist.squaredLength();
}
//-----------------------------------------------------------------------
Real COTEBillBoardChain::getBoundingRadius(void) const
{
	return mRadius;
}
//-----------------------------------------------------------------------
const AxisAlignedBox& COTEBillBoardChain::getBoundingBox(void) const
{
	//updateBoundingBox();
	return mAABB;
}
//-----------------------------------------------------------------------
const MaterialPtr& COTEBillBoardChain::getMaterial(void) const
{
	return mMaterial;
}
//-----------------------------------------------------------------------
void COTEBillBoardChain::setMaterialName(const String& name)
{
	mMaterialName = name;
	mMaterial = MaterialManager::getSingleton().getByName(mMaterialName);

	if (mMaterial.isNull())
	{
		
		mMaterial = MaterialManager::getSingleton().getByName("BaseWhite");
		if (mMaterial.isNull())
		{
			OTE_MSG("没有找到材质 : " << name, "失败")
		}
	}
	// Ensure new material loaded (will not load again if already loaded)
	mMaterial->load();
}
//-----------------------------------------------------------------------
const String& COTEBillBoardChain::getMovableType(void) const
{
	static String sType = "BBC";
	return sType;
}
//-----------------------------------------------------------------------
void COTEBillBoardChain::_updateRenderQueue(RenderQueue* queue)
{
	updateIndexBuffer();

	if (mIndexData->indexCount > 0)
	{
        if (mRenderQueueIDSet)
            queue->addRenderable(this, mRenderQueueID);
        else
            queue->addRenderable(this);
	}

}
//-----------------------------------------------------------------------
void COTEBillBoardChain::getRenderOperation(RenderOperation& op)
{
	op.indexData = mIndexData;
	op.operationType = RenderOperation::OT_TRIANGLE_LIST;
	op.srcRenderable = this;
	op.useIndexes = true;
	op.vertexData = mVertexData;
}
//-----------------------------------------------------------------------
void COTEBillBoardChain::getWorldTransforms(Matrix4* xform) const
{
	*xform = _getParentNodeFullTransform();
}
//-----------------------------------------------------------------------
const LightList& COTEBillBoardChain::getLights(void) const
{
	return getParentSceneNode()->findLights(this->getBoundingRadius());
}

//-----------------------------------------------------------------------
COTEBillBoardChain* COTEBillBoardChain::Create(const std::string& name, bool cloneMat)
{	
	// Check name not used
    if (s_BillBoardChainSets.find(name) != s_BillBoardChainSets.end())
    {
        OTE_MSG_ERR( "BillBoardChain " << name << " 已经存在！")
		return NULL;
    }

	COTEBillBoardChain* bbc = new COTEBillBoardChain(	name, 
														20, 
														1, 
														true,
														true, 
														true );
	s_BillBoardChainSets[name] = bbc;

	// Clone 材质

	if(cloneMat)
	{
		Ogre::MaterialPtr mptr = Ogre::MaterialManager::getSingleton().getByName(name);
		if(mptr.isNull())
		{
			mptr = Ogre::MaterialManager::getSingleton().getByName("BillBoardChainMat");
			if(mptr.isNull())
			{
				OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "材质没有找到!", "COTEBillBoardChain::Create");							
			}
			if(!mptr->isLoaded())
				mptr->touch();

			mptr->clone(name);
			OTE_TRACE("Clone材质 : " << name)
		}
		bbc->setMaterialName(name);	
	}
	else
	{
		bbc->setMaterialName("BillBoardChainMat");	
	}

	OTE_TRACE("创建 BillBoardChain:" << name)

	return bbc;
}

//-----------------------------------------------------------------------
void COTEBillBoardChain::Delete(const std::string& name)
{
	OTE_TRACE("移除 BillBoardChain:" << name)

	COTEBillBoardChain::OTEBillBoardChainList_t::iterator it = s_BillBoardChainSets.find(name);
	if(it != s_BillBoardChainSets.end())
	{
		// 删除材质

		if(Ogre::MaterialManager::getSingleton().resourceExists(name))
		{
			Ogre::MaterialManager::getSingleton().remove(name);
			//OTE_TRACE("移除材质 : " << name)
		}
		
		delete it->second;
		s_BillBoardChainSets.erase(it);
	}

}

//-----------------------------------------------------------------------
void COTEBillBoardChain::Clear()
{
	COTEBillBoardChain::OTEBillBoardChainList_t::iterator it = s_BillBoardChainSets.begin();
	while(it != s_BillBoardChainSets.end())	
		Delete(it->first);	

	OTE_TRACE("移除所有 BillBoardChain")
}


// ======================================
// COTEBillBoardChainFactory
// ======================================
Ogre::MovableObject* COTEBillBoardChainFactory::Create(const std::string& ResName, const std::string& Name, bool cloneMat)
{	
	return COTEBillBoardChain::Create(Name, cloneMat);
}

//-----------------------------------------------------------------------
void COTEBillBoardChainFactory::Delete(const std::string& Name)
{
	return COTEBillBoardChain::Delete(Name);
}
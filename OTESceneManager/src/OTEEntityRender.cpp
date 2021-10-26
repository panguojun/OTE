#include "OTEEntityRender.h"
#include "OTED3DManager.h"

#ifdef	__OTE

#include "OTEQTSceneManager.h"
#include "OTEEntityReactor.h" 
#include "OTEShader.h"
#include "OTEFog.h"
#include "OTEMathLib.h"
#include "OgreResourceBackgroundQueue_Win.h"
#include "OgreDefaultHardwareBufferManager.h"

#endif 

#ifdef __ZH

//#include "ZHRenderObj.h"

#endif 

using namespace Ogre;
using namespace OTE;

// =============================================
// COTESubEntity
// =============================================
COTESubEntity::COTESubEntity (COTEEntity* parent, SubMesh* subMeshBasis)
    :mParentEntity(parent), mSubMesh(subMeshBasis)
{
    mpMaterial = MaterialManager::getSingleton().getByName("BaseWhite");
    mMaterialLodIndex = 0;
    mRenderDetail = SDL_SOLID;
    mVisible = true;
    mBlendedVertexData = NULL;
	m_ParentNode = NULL;

}

// ----------------------------------------------
COTESubEntity::~COTESubEntity()
{
#ifdef __OTE

	if(this->mParentEntity->hasSkeleton() && 
		!mTempBlendedBuffer.destPositionBuffer.isNull() && 
		mTempBlendedBuffer.destPositionBuffer->getShadowBuffer())
	{
		DefaultHardwareBufferManager::FlagDefaultHardwareVertexBufferPool(
			((DefaultHardwareVertexBuffer*)mTempBlendedBuffer.destPositionBuffer->getShadowBuffer())->getData(),
			mTempBlendedBuffer.destPositionBuffer->getShadowBuffer()->getSizeInBytes());
		HardwareBufferManager::getSingleton()._forceReleaseBufferCopies(mTempBlendedBuffer.srcPositionBuffer);
		
	}

#else

	if(mParentEntity && mParentEntity->hasSkeleton())
	{
		HardwareBufferManager::getSingleton()._forceReleaseBufferCopies(mTempBlendedBuffer.srcPositionBuffer);
	}

	// 释放材质?

	//if(mpMaterial.useCount() <= 1)
	{
		const Ogre::String& matName = mpMaterial->getName();		
		Ogre::MaterialManager::getSingleton().remove(matName);	

	}
	mpMaterial.setNull();
	

#endif

	if (mBlendedVertexData)
        delete mBlendedVertexData;

}

#ifdef __ZH

// ----------------------------------------------
void COTESubEntity::ManualSet(COTEEntity* parent, void* parentNode, Ogre::SubMesh* subMeshBasis)
{	
	mParentEntity = parent;
	m_ParentNode = parentNode;
	mSubMesh = subMeshBasis;
	setMaterialName("");
}

#endif

#ifdef __OTE

// ----------------------------------------------
void COTESubEntity::CreateVertexEffect(const Ogre::String& hlslName, bool isCloneMat)
{
	Ogre::Pass* pass = getMaterial()->getTechnique(0)->getPass(0);
	if(pass->hasVertexProgram())
		return;

	// 创建vertex shader

	GpuProgramPtr gpptr = Ogre::HighLevelGpuProgramManager::getSingleton().getByName(hlslName);
	if(gpptr.isNull())
	{
		gpptr = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(
			hlslName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			"hlsl", GPT_VERTEX_PROGRAM);
	}

	// clone 材质

	if(isCloneMat)
	{
		Ogre::Material* mptr = getMaterial().getPointer();	

		std::stringstream ss;
		ss << mParentEntity->getName() << mptr->getName();							

		if(!mptr->isLoaded())
			mptr->touch();

		Ogre::MaterialPtr mp = Ogre::MaterialManager::getSingleton().getByName(ss.str());
		if(mp.isNull())
		{
			mp = mptr->clone(ss.str());
			OTE_TRACE("Clone材质 : " << ss.str())
		}
		//else
			//mptr->copyDetailsTo(mp);

		setMaterialName(ss.str());

		OTE_TRACE("Clone 材质: " << mptr->getName() << "->" << mp->getName())
	}

	// 设置vertex shader

	pass = getMaterial()->getTechnique(0)->getPass(0);
	pass->setVertexProgram(hlslName);

	// 设置vertex shader 参数

	Ogre::GpuProgramParametersSharedPtr gppPtr = pass->getVertexProgramParameters(); 
	gppPtr->setNamedAutoConstant("worldViewProjectionMatrix", Ogre::GpuProgramParameters::AutoConstantType::ACT_WORLDVIEWPROJ_MATRIX);

}

// ----------------------------------------------
void COTESubEntity::ClearEffect()
{
	Ogre::Pass* pass = getMaterial()->getTechnique(0)->getPass(0);
	if(!pass->hasVertexProgram())
		return;

	pass->setVertexProgram("");			
}

#endif

// ----------------------------------------------
void COTESubEntity::setMaterialName( const String& name)
{
#ifdef __OTE

	mpMaterial = MaterialManager::getSingleton().getByName(name);

	mMaterialName = mpMaterial->getName();

    if (mpMaterial.isNull())
    {
        OTE_LOGMSG("Can't assign material " << name << 
            " to SubEntity of " << mParentEntity->getName() << " because this "
            "Material does not exist. Have you forgotten to define it in a "
            ".material script?")

        mpMaterial = MaterialManager::getSingleton().getByName("BaseWhite");
        if (mpMaterial.isNull())
        {
            OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Can't assign default material "
                "to SubEntity of " + mParentEntity->getName() + ". Did "
                "you forget to call MaterialManager::initialise()?",
                "SubEntity.setMaterialName");
        }
    }
    // Ensure new material loaded (will not load again if already loaded)
    mpMaterial->load();

#else
		
	mpMaterial = MaterialManager::getSingleton().CreateMatFromTypeEx(mSubMesh->getMatTypeName(), mSubMesh->getMatResName());

	mMaterialName = mpMaterial->getName();

#endif

    // tell parent to reconsider material vertex processing options
	if(mParentEntity)
		mParentEntity->reevaluateVertexProcessing();
}

// ----------------------------------------------
void COTESubEntity::getRenderOperation(RenderOperation& op)
{

#ifdef __OTE

	__TCS

    // Do we need to use software skinned vertex data?
    if (mParentEntity->hasSkeleton() && !mParentEntity->mHardwareSkinning)
    {		
        op.vertexData = mSubMesh->useSharedVertices ? 
            mParentEntity->mSharedBlendedVertexData : mBlendedVertexData;			
    }

	if(!mParentEntity->isAttached()) // 标示物件已经渲染完毕
		this->mParentEntity->setUserObject(0);	



	// shader

	static COTEEntity* sLastEntity;
	if(sLastEntity != this->mParentEntity || mParentEntity->getSubEntity(0) == this) // 当前ParentEntity有变化 或者 frame move时更新 
	{	
		// 骨胳动画

		if(mParentEntity->hasSkeleton() && 
				mParentEntity->isHardwareSkinningEnabled())
		{	
			unsigned short mNumBoneMatrices = this->mParentEntity->_getNumBoneMatrices();
			const Matrix4 *mBoneMatrices = this->mParentEntity->_getBoneMatrices(); 

			// 传递参数

			HRESULT hr;
			assert( (g_pDevice != NULL) && "g_pDevice == NULL");
			if (FAILED(hr = g_pDevice->SetVertexShaderConstantF(4, (float*)mBoneMatrices, mNumBoneMatrices * 4))) // 4 × 60个register
			{
				OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, 
					"Unable to upload vertex shader float parameters", 
					"COTESubEntity::getRenderOperation");
			}
		}
		else

		// 反应器

		{	
			COTEActorEntity* ae = (COTEActorEntity*)mParentEntity;

			// 附加Ｅｆｆｅｃｔ　放在在这里为了不跟骨胳动画冲突

			if(ae->m_pGPUReactor)
			{
				((COTEShaderCtrller*)ae->m_pGPUReactor)->UpdateParams();							
			}
		}
		
		sLastEntity = this->mParentEntity;
	} 

	// 用户注册之渲染函数

	COTEActorEntity* ae = (COTEActorEntity*)mParentEntity;
	for(unsigned int i = 0; i < ae->m_BeforeRenderCBList.size(); i ++)
	{
		(*ae->m_BeforeRenderCBList[i])(ae);
	}

	__TCE

#endif

		
	// Use LOD

    mSubMesh->_getRenderOperation(op, 0);


}

// ----------------------------------------------
// 结束渲染
void COTESubEntity::endRenderOperation()
{
#ifdef __OTE

	COTEActorEntity* ae = (COTEActorEntity*)mParentEntity;

	// 用户注册之渲染函数
	
	for(unsigned int i = 0; i < ae->m_EndRenderCBList.size(); i ++)
	{
		(*ae->m_EndRenderCBList[i])(ae);
	}

#endif

}

// ----------------------------------------------
void COTESubEntity::getWorldTransforms(Matrix4* xform) const
{
//#ifdef __ZH
//	if(m_ParentNode)
//		*xform = ((CZHRenderObj*)m_ParentNode)->getWorldTrans();
//	return;
//#endif

	if (mParentEntity && !mParentEntity->mNumBoneMatrices)
    {
        *xform = mParentEntity->_getParentNodeFullTransform();
    }
    else
    {  
         *xform = Matrix4::IDENTITY;       
    }
}

// ----------------------------------------------
const Quaternion& COTESubEntity::getWorldOrientation(void) const
{	
//#ifdef __ZH
//	if(m_ParentNode)
//		return ((CZHRenderObj*)m_ParentNode)->mRot;
//#endif

	return mParentEntity ? mParentEntity->mParentNode->_getDerivedOrientation() : Quaternion::IDENTITY;
}

// ----------------------------------------------
const Vector3& COTESubEntity::getWorldPosition(void) const
{	
//#ifdef __ZH
//	if(m_ParentNode)
//		return ((CZHRenderObj*)m_ParentNode)->mWorldPos;
//#endif

	return mParentEntity ? mParentEntity->mParentNode->_getDerivedPosition() : Ogre::Vector3::ZERO;
}

// ---------------------------------------------
unsigned short COTESubEntity::getNumWorldTransforms(void) const
{
    if (!mParentEntity						||
		!mParentEntity->mNumBoneMatrices	||
        !mParentEntity->isHardwareSkinningEnabled())
    {
        // No skeletal animation, or software skinning (pretransformed)
        return 1;
    }
    else
    {
        // Hardware skinning, pass all matrices
        return mParentEntity->mNumBoneMatrices;
    }
}

// ---------------------------------------------
Real COTESubEntity::getSquaredViewDepth(const Camera* cam) const
{
	return mParentEntity ? mParentEntity->mParentNode->getSquaredViewDepth(cam) : 0.0f;
}

// ---------------------------------------------
bool COTESubEntity::getNormaliseNormals(void) const
{
	return mParentEntity ? mParentEntity->mNormaliseNormals : false;
}

// ---------------------------------------------
const LightList& COTESubEntity::getLights(void) const
{	
	if(!mParentEntity)
	{
		static Ogre::LightList s_LL;
		return s_LL;
	}
    return mParentEntity->getParentSceneNode()->findLights(mParentEntity->getBoundingRadius());
}

// ---------------------------------------------
void COTESubEntity::prepareTempBlendBuffers(void)
{
    if (mBlendedVertexData) 
    {
        delete mBlendedVertexData;
        mBlendedVertexData = 0;
    }
	if(mParentEntity)
	{
		// Clone without copying data
		mBlendedVertexData = 
			mParentEntity->cloneVertexDataRemoveBlendInfo(mSubMesh->vertexData);
		mParentEntity->extractTempBufferInfo(mBlendedVertexData, &mTempBlendedBuffer);
	}
}

// ---------------------------------------------
bool COTESubEntity::getCastsShadows(void) const
{
	if(mParentEntity)
		return mParentEntity->getCastShadows();
	else
		return false;
}

/************************************************************
 ************************************************************

 模块名称： 用于优化模型渲染，特别是小物件比较多的时候

 *************************************************************
 *************************************************************/


// =============================================
// COTEAutoMesh
// =============================================

#define		GROUP_SIZE				128			// 一个autoentity的尺寸（可以理解为矩形, 物件的中心点在其中)

// ----------------------------------------------
int			g_RenderCount = 0;

//Ogre::HardwareVertexBufferSharedPtr COTEAutoMesh::m_VertexBuffer;
//Ogre::HardwareIndexBufferSharedPtr  COTEAutoMesh::m_IndexBuffer;

// ---------------------------------------------
COTEAutoMesh::COTEAutoMesh() : 
SubMesh()
{
	m_AABB.setExtents(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT, -MAX_FLOAT, -MAX_FLOAT, -MAX_FLOAT);
}

// ---------------------------------------------
void COTEAutoMesh::Create(int poolSize)
{	
	// vertex data

	vertexData = new VertexData();
    vertexData->vertexDeclaration = 
        HardwareBufferManager::getSingleton().createVertexDeclaration();
    vertexData->vertexBufferBinding = 
        HardwareBufferManager::getSingleton().createVertexBufferBinding();

    // Map in position data

	int offset = 0;
    vertexData->vertexDeclaration->addElement(0, offset, VET_FLOAT3, VES_POSITION); 
    offset += VertexElement::getTypeSize(VET_FLOAT3);
    vertexData->vertexDeclaration->addElement(0, offset, VET_COLOUR, VES_DIFFUSE); 
    offset += VertexElement::getTypeSize(VET_COLOUR);
    vertexData->vertexDeclaration->addElement(0, offset, VET_FLOAT3, VES_NORMAL); 
    offset += VertexElement::getTypeSize(VET_FLOAT3);
    // texture coord sets
    vertexData->vertexDeclaration->addElement(0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);

	// 顶点缓存

	if(m_VertexBuffer.isNull())
		m_VertexBuffer = 
				HardwareBufferManager::getSingleton().createVertexBuffer(
					vertexData->vertexDeclaration->getVertexSize(0), 
					poolSize, 
					HardwareBuffer::HBU_STATIC_WRITE_ONLY);	

	vertexData->vertexBufferBinding->setBinding(0, m_VertexBuffer);
	vertexData->vertexStart = 0;
	vertexData->vertexCount = 0;

	// 索引

	indexData = new IndexData();
	if(m_IndexBuffer.isNull())
		m_IndexBuffer = HardwareBufferManager::getSingleton().
								createIndexBuffer(HardwareIndexBuffer::IT_16BIT,
									poolSize,
									HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	indexData->indexBuffer = m_IndexBuffer;
    indexData->indexStart  = 0;
	indexData->indexCount  = 0;
}

// ----------------------------------------------
bool COTEAutoMesh::Append(Ogre::SubMesh* sm, const Ogre::Matrix4* worldMatrix)
{
	m_SubMeshMatrixList.push_back(SubMeshMatrix_t(sm, worldMatrix));
	return true;
}

// ----------------------------------------------
void COTEAutoMesh::AppendFinished()
{
	// 计算尺寸

	int poolSize  = 0;

	std::list<SubMeshMatrix_t>::iterator it = m_SubMeshMatrixList.begin();
	while(it != m_SubMeshMatrixList.end())
	{
		poolSize += it->first->vertexData->vertexCount;	
		
		++ it;
	}

	// 创建顶点缓存

	Create(poolSize);

	OTE_TRACE("Create AutoMesh Size:" << poolSize)
	
	// 将列表中submesh添加到automesh中

	it = m_SubMeshMatrixList.begin();
	while(it != m_SubMeshMatrixList.end())
	{
		AppendImp(it->first, it->second);

		++ it;
	}
	m_SubMeshMatrixList.clear();
}

// ----------------------------------------------
bool COTEAutoMesh::AppendImp(Ogre::SubMesh* sm, const Ogre::Matrix4* worldMatrix)
{
	OGRE_LOCK_AUTO_MUTEX

	// 材质

	this->setMaterialName(sm->getMaterialName());

	// 合并缓存

	HardwareVertexBufferSharedPtr tSrcBufPtr = sm->vertexData->vertexBufferBinding->getBuffer(0);

	// 查询是否超出范围（无论顶点 还是 索引）
	// 可能存在不同格式的顶点 !

	if(	//m_VertexBuffer->getSizeInBytes() < (vertexData->vertexStart + vertexData->vertexCount) * m_VertexBuffer->getVertexSize() + tSrcBufPtr->getSizeInBytes()	||
		m_IndexBuffer ->getSizeInBytes() < (indexData->indexStart   + indexData ->indexCount ) * m_IndexBuffer ->getIndexSize()  + sm->indexData->indexBuffer->getSizeInBytes())
	{
		//OTE_TRACE("vertexPool: " << m_VertexBuffer->getSizeInBytes() << " demand size: " << (vertexData->vertexStart + vertexData->vertexCount) * m_VertexBuffer->getVertexSize() + tSrcBufPtr->getSizeInBytes())
		//OTE_TRACE("indexPool: "  << m_IndexBuffer ->getSizeInBytes() << " demand size: " << (indexData->indexStart   + indexData ->indexCount ) * m_IndexBuffer ->getIndexSize()  + sm->indexData->indexBuffer->getSizeInBytes())
		
		OTE_TRACE("在场景物件自动融合的过程中 超出池的尺寸范围!")
  		return false;
	}

	/////////////////// 顶点 ///////////////////
 
	// lock		
	unsigned char* pSrcBuffer = static_cast<unsigned char*>(tSrcBufPtr->lock(HardwareBuffer::HBL_READ_ONLY));
	unsigned char* pDesBuffer = static_cast<unsigned char*>(m_VertexBuffer->lock(
											(vertexData->vertexStart + vertexData->vertexCount) * m_VertexBuffer->getVertexSize(),
											tSrcBufPtr->getNumVertices() * m_VertexBuffer->getVertexSize(), 
											HardwareBuffer::HBL_DISCARD));
	
	const VertexElement* srcPoselem  = sm->vertexData->vertexDeclaration->findElementBySemantic(VES_POSITION);  		
	const VertexElement* srcDiffelem = sm->vertexData->vertexDeclaration->findElementBySemantic(VES_DIFFUSE);	
    const VertexElement* srcNormelem = sm->vertexData->vertexDeclaration->findElementBySemantic(VES_NORMAL);
	const VertexElement* srcTexelem0 = sm->vertexData->vertexDeclaration->findElementBySemantic(VES_TEXTURE_COORDINATES, 0);  
	
	const VertexElement* desPoselem  = vertexData->vertexDeclaration->findElementBySemantic(VES_POSITION);  	
	const VertexElement* desDiffelem = vertexData->vertexDeclaration->findElementBySemantic(VES_DIFFUSE);	
    const VertexElement* desNormelem = vertexData->vertexDeclaration->findElementBySemantic(VES_NORMAL);
	const VertexElement* desTexelem0 = vertexData->vertexDeclaration->findElementBySemantic(VES_TEXTURE_COORDINATES, 0);  

	float *pSrcPos, *pSrcNormal, *pSrcTex1;
	float *pDesPos, *pDesNormal, *pDesTex1;
	RGBA *pSrcDiff, *pDesDiff;

	Ogre::Vector3 tOrgPos;
	Ogre::Vector3 minCorner = m_AABB.getMinimum();
	Ogre::Vector3 maxCorner = m_AABB.getMaximum();

	for(unsigned int i = 0; i < sm->vertexData->vertexCount; ++ i)
	{
		srcPoselem ->baseVertexPointerToElement(pSrcBuffer, &pSrcPos);	
		srcDiffelem->baseVertexPointerToElement(pSrcBuffer, &pSrcDiff);
		srcNormelem->baseVertexPointerToElement(pSrcBuffer, &pSrcNormal);
		srcTexelem0->baseVertexPointerToElement(pSrcBuffer, &pSrcTex1); 

		desPoselem ->baseVertexPointerToElement(pDesBuffer, &pDesPos);	
		desDiffelem->baseVertexPointerToElement(pDesBuffer, &pDesDiff);
		desNormelem->baseVertexPointerToElement(pDesBuffer, &pDesNormal);	
		desTexelem0->baseVertexPointerToElement(pDesBuffer, &pDesTex1); 		
	
		// 位置

		tOrgPos.x  = *pSrcPos++;	
		tOrgPos.y  = *pSrcPos++;	
		tOrgPos.z  = *pSrcPos++;				
		tOrgPos    = *worldMatrix * tOrgPos;
		*pDesPos++ = tOrgPos.x;
		*pDesPos++ = tOrgPos.y;
		*pDesPos++ = tOrgPos.z;
		
		// 保卫盒	

		tOrgPos.x > maxCorner.x ? maxCorner.x = tOrgPos.x : NULL;
		tOrgPos.y > maxCorner.y ? maxCorner.y = tOrgPos.y : NULL;
		tOrgPos.z > maxCorner.z ? maxCorner.z = tOrgPos.z : NULL;

		tOrgPos.x < minCorner.x ? minCorner.x = tOrgPos.x : NULL;
		tOrgPos.y < minCorner.y ? minCorner.y = tOrgPos.y : NULL;
		tOrgPos.z < minCorner.z ? minCorner.z = tOrgPos.z : NULL;

		// 颜色 
		*pDesDiff++ = *pSrcDiff++;	

		// 发线
		*pDesNormal++ = *pSrcNormal++;	
		*pDesNormal++ = *pSrcNormal++;		
		*pDesNormal++ = *pSrcNormal++;		

		// 纹理坐标
		*pDesTex1++ = *pSrcTex1++;	
		*pDesTex1++ = *pSrcTex1++;		
	
		pDesBuffer +=  m_VertexBuffer->getVertexSize();		
		pSrcBuffer +=  tSrcBufPtr    ->getVertexSize();	
	}	

	m_AABB.setMinimum(minCorner);
	m_AABB.setMaximum(maxCorner);

	// unlock

	m_VertexBuffer->unlock();
	tSrcBufPtr->unlock();

	/////////////////// 索引 ///////////////////	
	
	// lock	

	Ogre::uint16* pSrc16Buffer = static_cast<Ogre::uint16*>(sm->indexData->indexBuffer->lock(HardwareBuffer::HBL_READ_ONLY));
	Ogre::uint16* pDes16Buffer = static_cast<Ogre::uint16*>(m_IndexBuffer->lock(
											(indexData->indexStart + indexData->indexCount) * indexData->indexBuffer->getIndexSize(),
											sm->indexData->indexBuffer->getSizeInBytes(), 
											HardwareBuffer::HBL_DISCARD));
	
	for(size_t j = 0; j < sm->indexData->indexCount; ++ j)
	{						
		*pDes16Buffer ++  = (*pSrc16Buffer ++) + (vertexData->vertexStart + vertexData->vertexCount);
	}

	// unlock

	m_IndexBuffer->unlock();
	sm->indexData->indexBuffer->unlock();	
	
	// 更新"渲染尺寸"

	vertexData->vertexCount += tSrcBufPtr->getNumVertices();
	//s_VerPoolCount  += tSrcBufPtr->getNumVertices();
	indexData->indexCount += sm->indexData->indexBuffer->getNumIndexes();
	//s_IndPoolCount += sm->indexData->indexBuffer->getNumIndexes();
	
	return true;
}


// =============================================
// COTEAutoSubEntity
// =============================================
COTEAutoSubEntity::COTEAutoSubEntity(const Ogre::String& name, COTEAutoEntity* parent) :
m_Name(name), 
mBoundingRadius(1.0f),
m_Parent(parent),
m_AutoMesh(NULL),
m_SubMeshCount(0),
m_FrameCount(0)
{	
	CreateAutoMesh();
	
}

COTEAutoSubEntity::~COTEAutoSubEntity()
{	
	SAFE_DELETE(m_AutoMesh)
}

// ---------------------------------------------
void COTEAutoSubEntity::CreateAutoMesh()
{
	m_AutoMesh = new COTEAutoMesh();	// 设置默认顶点池尺寸
}

// ---------------------------------------------
Real COTEAutoSubEntity::getSquaredViewDepth(const Camera* const cam) const
{  
    return SCENE_MGR_OGRE->getRootSceneNode()->getSquaredViewDepth(cam);
}

// ---------------------------------------------
const LightList& COTEAutoSubEntity::getLights(void) const
{
    static Ogre::LightList s_LL;
	return s_LL;
}

// -------------------------------------- 
void COTEAutoSubEntity::_updateRenderQueue(RenderQueue* queue)
{	
	if( (::GetKeyState(VK_CONTROL) & 0x80) && 
		(::GetKeyState('A') & 0x80)		 )
		return;

	if(!mpMaterial.isNull())
	{
		queue->addRenderable(this);
		g_RenderCount ++;
	}
}
// -------------------------------------- 
const Ogre::AxisAlignedBox& COTEAutoSubEntity::getBoundingBox(void) const 
{ 
	return m_AutoMesh->m_AABB;
}

// ----------------------------------------------------------
void COTEAutoSubEntity::getWorldTransforms( Ogre::Matrix4* xform ) const
{
	*xform = Ogre::Matrix4::IDENTITY;	 
}

// ----------------------------------------------------------
const Ogre::Quaternion& COTEAutoSubEntity::getWorldOrientation(void) const
{
	return Quaternion::IDENTITY;
}


//-----------------------------------------------------------------------
const Vector3& COTEAutoSubEntity::getWorldPosition(void) const
{
	return Vector3::ZERO;	
}

// ---------------------------------------------
void COTEAutoSubEntity::_notifyCurrentCamera(Ogre::Camera* cam)
{	
}

// ---------------------------------------------
void COTEAutoSubEntity::getRenderOperation(RenderOperation& op)
{
	op.operationType = RenderOperation::OT_TRIANGLE_LIST;
	op.useIndexes = true; 
	op.srcRenderable = this;
	op.indexData = m_AutoMesh->indexData;
	op.vertexData = m_AutoMesh->vertexData;
}


// ---------------------------------------------
bool COTEAutoSubEntity::AppendSubMesh(Ogre::SubMesh* subMesh, const Ogre::Matrix4& worldMatrix)
{
	if( m_AutoMesh->getMaterialName().empty() ||
		m_AutoMesh->getMaterialName() == subMesh->getMaterialName())
	{
		mpMaterial = Ogre::MaterialManager::getSingleton().getByName(subMesh->getMaterialName());
		
		if(mpMaterial.isNull())
		{
			OTE_MSG_ERR("COTEAutoSubEntity::AppendSubMesh 失败！没有找到材质" << subMesh->getMaterialName().c_str())
		}
		mpMaterial->load();
		m_SubMeshCount ++;
		return m_AutoMesh->Append(subMesh, &worldMatrix);			
	}
	return false;
}

// ---------------------------------------------
void COTEAutoSubEntity::AppendFinished()
{
	if(!m_AutoMesh)
		return;

	m_AutoMesh->AppendFinished();
}

// ======================================================================
// COTEAutoEntity
// ======================================================================
COTEAutoEntity::COTEAutoEntity(float x, float y, float z) :
m_Position(x, y, z)
{
}

// ----------------------------------------------------------------------
COTEAutoEntity::~COTEAutoEntity()
{
	std::map<std::string, COTEAutoSubEntity*>::iterator it = m_AutoEntityMap.begin();
	while(it != m_AutoEntityMap.end())
	{
		delete it->second;
		++ it;
	}
	m_AutoEntityMap.clear();
}

// ----------------------------------------------------------------------
void COTEAutoEntity::AppendMesh(const Ogre::String& meshName, const Ogre::Matrix4& worldMatrix, const Ogre::Quaternion& rotation)
{	
#ifdef __OTE

	ResourceBackgroundQueue::GetSingleton()->load(meshName, 
		"Mesh", 
		"PakFileSystem"/*Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME*/		
		);	
	
	RenderEntity_t* re = new RenderEntity_t();
	re->ProccessTicket	 = ResourceBackgroundQueue::GetSingleton()->mNextTicketID; // 最后一个ticket来判断是否加载完
	re->MeshName	= meshName;
	re->WorldMatrix = worldMatrix;
	re->Rotation	= rotation;

	m_RenderEntityList.push_back(re);

#endif	


#ifdef __ZH
	
	RenderEntity_t* re = new RenderEntity_t();
	re->ProccessTicket	 = 0; // 这个暂时为零
	re->MeshName	= meshName;
	re->WorldMatrix = worldMatrix;
	re->Rotation	= rotation;

	m_RenderEntityList.push_back(re);

#endif

}


// ----------------------------------------------------------------------
void COTEAutoEntity::SetupRes()
{
#ifdef __OTE

	std::vector<RenderEntity_t*>::iterator it = m_RenderEntityList.begin();
	while(it != m_RenderEntityList.end())
	{
		if(ResourceBackgroundQueue::GetSingleton()->isProcessComplete((*it)->ProccessTicket))
		{
			(*it)->MeshResPtr = Ogre::MeshManager::getSingleton().getByName((*it)->MeshName);

			if((*it)->MeshResPtr.isNull())
			{
				(*it)->MeshResPtr = Ogre::MeshManager::getSingleton().load(
					(*it)->MeshName,
					"PakFileSystem"/*ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME*/);
			}

			// 贴图

			int numSubMeshes = (*it)->MeshResPtr->getNumSubMeshes();
			for (int i = 0; i < numSubMeshes; ++ i)
			{
				Ogre::SubMesh* subMesh = (*it)->MeshResPtr->getSubMesh(i);
				if(!subMesh->TextureName.empty() &&
					Ogre::TextureManager::getSingleton().getByName(subMesh->TextureName).isNull())
				{
					Ogre::TextureManager::getSingleton().load(subMesh->TextureName, "PakFileSystem");
					OTE_TRACE("Load Texture: " << subMesh->TextureName)
				}

				subMesh->setMaterialName(MaterialManager::getSingleton().CreateMatFromType(subMesh->MaterialTypeName, subMesh->TextureName)->getName());
			}

			AppendMesh((*it)->MeshResPtr.getPointer(), (*it)->WorldMatrix, (*it)->Rotation);

			m_RenderEntityList.erase(it);	

			it = m_RenderEntityList.begin();

			continue;
		}		
		++ it;	
	}

#endif

#ifdef __ZH

	std::vector<RenderEntity_t*>::iterator it = m_RenderEntityList.begin();
	while(it != m_RenderEntityList.end())
	{		
		(*it)->MeshResPtr = Ogre::MeshManager::getSingleton().getByName((*it)->MeshName);
		
		if((*it)->MeshResPtr.isNull())
		{
			//meshPtr = Ogre::MeshManager::getSingleton().load(
			//	(*it)->MeshName,
			//	ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			MESH_RESMGR_LOAD((*it)->MeshName);
		}

		AppendMesh((*it)->MeshResPtr.getPointer(), (*it)->WorldMatrix, (*it)->Rotation);

		m_RenderEntityList.erase(it);	

		it = m_RenderEntityList.begin();

		continue;
		
		++ it;	
	}

#endif

}

// ----------------------------------------------------------------------
bool COTEAutoEntity::AppendMesh(const Ogre::Mesh* mesh, const Ogre::Matrix4& worldMatrix, const Ogre::Quaternion& orientation)
{	
	bool empty = true;
	int numSubMeshes = mesh->getNumSubMeshes();
	for (int i = 0; i < numSubMeshes; ++i)
	{
		SubMesh* subMesh = mesh->getSubMesh(i);	
		
		if(subMesh)
		{
			empty = false;
			//int id = int(subMesh);
			if(m_AutoEntityMap.find(subMesh->getMaterialName()) == m_AutoEntityMap.end())
			{				
				COTEAutoSubEntity* ae = new COTEAutoSubEntity(subMesh->getMaterialName(), this);

				// 放到结点上 这种方法不是很完美!

				//SCENE_MGR->AttachObjToSceneNode(ae);
				//ae->getParentSceneNode()->setPosition(worldMatrix[0][3], 0, worldMatrix[2][3]);		
				
				// 放在山体上
				// 这里我们采用了一种粗糙的方法,就是把小物件放到tile上,
				// 注意:这种对应本来是多对多的 我们简化成多对一,
				// 考虑到小物件体积比较小所以影响不是很大!	
				

				/*COTETile* tile = COTETilePage::s_CurrentPage->GetTileAt(worldMatrix[0][3], worldMatrix[2][3]);
				if(tile)
				{	
					tile->m_ObjectList.push_back(ae);					
				}	*/
				
				// 保持总是可见
				
				m_AutoEntityMap[subMesh->getMaterialName()] = ae;
			}
			
			if(!m_AutoEntityMap[subMesh->getMaterialName()]->AppendSubMesh(subMesh, worldMatrix))
			{
				OTE_TRACE("AppendSubMesh 失败！" << subMesh->getMaterialName())
				return false;			
			}
		}
	}	

	if(empty)
	{
		OTE_TRACE("AppendMesh 失败！" << mesh->getName())
	}

	return empty == false;
}

// ----------------------------------------------------------------------
void COTEAutoEntity::AddToRenderQueue(Ogre::RenderQueue* queue)
{
	std::map<std::string, COTEAutoSubEntity*>::iterator it = m_AutoEntityMap.begin();
	while(it != m_AutoEntityMap.end())
	{
		it->second->_updateRenderQueue(queue);
		++ it;
	}
}

//-----------------------------------------------------------------------
void COTEAutoEntity::SetupMeshes(EntitySetupHndler_t* setupHdler)
{	
	std::map<std::string, COTEAutoSubEntity*>::iterator it = m_AutoEntityMap.begin();
	while(it != m_AutoEntityMap.end())
	{
		it->second->AppendFinished();
		m_AABB.merge(it->second->getBoundingBox());

		// 把渲染对象邦定到地皮上 

		const Ogre::AxisAlignedBox& aabb = it->second->getBoundingBox();

#ifdef __OTE
		
		// tile尺寸

		float tileSize = (COTETilePage::s_CurrentPage->m_TileSize) * COTETilePage::s_CurrentPage->m_Scale.x;


		const Vector3& _min = aabb.getMinimum();
		const Vector3& _max = aabb.getMaximum();

		/*for(float x = _min.x; x < _max.x; x += tileSize)
		for(float z = _min.z; z < _max.z; z += tileSize)*/
		{
			COTETile* tile = COTETilePage::s_CurrentPage->GetTileAt(aabb.getCenter().x, aabb.getCenter().z);
			if(tile)		
			{
				tile->m_ObjectList.push_back(it->second);					
			}
			
		}

		++ it;
	
#else
		// 在外部装载

		if(setupHdler)
			(*setupHdler)(aabb);

#endif

}

}


// ======================================================================
// COTEAutoEntityMgr
// ======================================================================
COTEAutoEntityMgr	COTEAutoEntityMgr::s_Inst;
// ----------------------------------------------------------------------
COTEAutoEntityMgr* COTEAutoEntityMgr::GetInstance()
{
	if(!s_Inst.m_IsInit)
		s_Inst.Init();

	return &s_Inst;
}

// ----------------------------------------------------------------------
COTEAutoEntityMgr::COTEAutoEntityMgr() :
m_IsInit(false),
m_FinishedState(eLoadFinished),
m_EntitySetupHandler(NULL)
{

}

// ----------------------------------------------------------------------
void COTEAutoEntityMgr::Init()
{
#ifdef __OTE
	
	SCENE_MGR_OGRE->AddListener(COTEQTSceneManager::eRenderCB, OnRenderScene);

#endif

	m_IsInit = true;
}

// ----------------------------------------------------------------------
void COTEAutoEntityMgr::Destroy()
{
	Clear();
}

// ----------------------------------------------------------------------
void COTEAutoEntityMgr::Clear()
{
	s_Inst.LogMsg(); // 打印一些信息

	HashMap<Ogre::String, COTEAutoEntity*>::iterator it = s_Inst.m_GroupTileMap.begin();
	while(it != s_Inst.m_GroupTileMap.end())
	{
		delete it->second;
		++ it;
	}
	s_Inst.m_GroupTileMap.clear();
}

// ----------------------------------------------------------------------
void COTEAutoEntityMgr::OnRenderScene(int id)
{
//#ifdef __OTE
//
//	char buff[32];
//	sprintf(buff, "[AutoEntities]: %d", g_RenderCount);
//	SCENE_MGR_OGRE->m_RenderInfoList["AutoEntities"] = buff;
//
//#endif

	g_RenderCount = 0;

	if(s_Inst.m_FinishedState == eLoadFinished)
		return;

	int loadedCnt = 0;
	HashMap<Ogre::String, COTEAutoEntity*>::iterator it = s_Inst.m_GroupTileMap.begin();
	while(it != s_Inst.m_GroupTileMap.end())
	{		
		it->second->SetupRes();
		if(it->second->IsResLoaded())
			loadedCnt ++;

		++ it;
	}

	if(s_Inst.m_FinishedState == eAppendFinished && 
				loadedCnt == s_Inst.m_GroupTileMap.size())
	{
		s_Inst.m_FinishedState = eLoadFinished;		
		s_Inst.SetupMeshesImp();		
	}
}

// ----------------------------------------------------------------------
void COTEAutoEntityMgr::AppendMesh(const Ogre::String& meshName, const Ogre::Matrix4& worldMatrix, const Ogre::Quaternion& orientation)
{	
	m_FinishedState = ePrepaired; 

	// 根据位置组织名称

	std::stringstream ss;

	int x = int(worldMatrix[0][3]) / GROUP_SIZE;
	int z = int(worldMatrix[2][3]) / GROUP_SIZE;

	ss << "[" << x << "," << z << "]";

	Ogre::Vector3 tCenterPos(x * GROUP_SIZE, 0.0f, z * GROUP_SIZE);

	if(m_GroupTileMap.find(ss.str()) == m_GroupTileMap.end())
	{
		m_GroupTileMap[ss.str()] = new COTEAutoEntity(tCenterPos.x, tCenterPos.y, tCenterPos.z);

		OTE_TRACE("Create AutoEntityGroup: " << ss.str())
	}

	OTE_TRACE("Add To AutoEntityGroup" << ss.str() << ": " << meshName)

	COTEAutoEntity* aeg = m_GroupTileMap[ss.str()];

	aeg->AppendMesh(meshName, worldMatrix, orientation);
	
}

// ----------------------------------------------------- 
inline void MakeTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation, 
			Matrix4& destMatrix)
{
	destMatrix = Matrix4::IDENTITY;
	// Ordering:
	//    1. Scale
	//    2. Rotate
	//    3. Translate

	// Parent scaling is already applied to derived position
	// Own scale is applied before rotation
	Matrix3 rot3x3, scale3x3;
	orientation.ToRotationMatrix(rot3x3);
	scale3x3 = Matrix3::ZERO;
	scale3x3[0][0] = scale.x;
	scale3x3[1][1] = scale.y;
	scale3x3[2][2] = scale.z;

	destMatrix = rot3x3 * scale3x3;
	destMatrix.setTrans(position);
}

// ----------------------------------------------------------------------
void COTEAutoEntityMgr::AppendMesh(const Ogre::String& meshName, 
	const Ogre::Vector3& position, const Ogre::Vector3& scale, const Ogre::Quaternion& orientation)
{
	Ogre::Matrix4 worldMatrix;
	MakeTransform(position, scale, orientation, worldMatrix);
	
	AppendMesh(meshName, worldMatrix, orientation);

}

//-----------------------------------------------------------------------
void COTEAutoEntityMgr::SetupMeshes(COTEAutoEntity::EntitySetupHndler_t* setupHdler)
{
	m_EntitySetupHandler = setupHdler;
	m_FinishedState = eAppendFinished;
}

//-----------------------------------------------------------------------
void COTEAutoEntityMgr::SetupMeshesImp()
{
	HashMap<Ogre::String, COTEAutoEntity*>::iterator it = m_GroupTileMap.begin();
	while(it != m_GroupTileMap.end())
	{
		COTEAutoEntity* ae = it->second;
		
		ae->SetupMeshes(m_EntitySetupHandler);
		++ it;
	}
}

// ----------------------------------------------------------------------
void COTEAutoEntityMgr::RenderTileEntities(std::vector<void*>* pObjList, RenderQueue* queue)
{
	static unsigned long s_FrameCount = 1;

	std::vector<void*>::iterator it = pObjList->begin();
	while(it != pObjList->end())
	{
		COTEAutoSubEntity* ase = (COTEAutoSubEntity*)(*it);
		if(ase->m_FrameCount != s_FrameCount)
		{			
			queue->addRenderable(ase);
			g_RenderCount ++;		

			ase->m_FrameCount = s_FrameCount;			
		}
		
		++ it;
	}

	s_FrameCount ++;

}

// ----------------------------------------------------------------------
void COTEAutoEntityMgr::LogMsg() 
{
	if(s_Inst.m_GroupTileMap.empty())
		return;

	OTE_LOGMSG("======== 静态实体优化报告 ==========")
	OTE_LOGMSG("AutoEntity总共的group数量: " << s_Inst.m_GroupTileMap.size())

	HashMap<Ogre::String, COTEAutoEntity*>::iterator it = s_Inst.m_GroupTileMap.begin();
	while(it != s_Inst.m_GroupTileMap.end())
	{
		COTEAutoEntity* ae = it->second;
		
		OTE_LOGMSG("AutoEntity: "<< ae << " num of subentities: " << ae->m_AutoEntityMap.size());

		std::map<std::string, COTEAutoSubEntity*>::iterator ita = ae->m_AutoEntityMap.begin();
		while(ita != ae->m_AutoEntityMap.end())
		{
			OTE_LOGMSG("AutoSubEntity: "<< ita->second->getName() << " SubMeshCount: " << ita->second->m_SubMeshCount);
			++ ita;
		}

		++ it;
	}
	OTE_LOGMSG("====================================")

}

#ifdef __ZH

#include "OTESky.h"

// ======================================================================
// COTERender
// ======================================================================
void COTERender::FrameStarted(float dTimeSinceLstFrm)
{
	int cam = (int)SCENE_MGR_OGRE->getCamera("PlayerCam");
	if(cam == 0)
		return;
	
	COTESky::GetInstance()->OnRender(cam);

	static DWORD sOldTC = ::GetTickCount();
	COTED3DManager::s_FrameTime = (::GetTickCount() - sOldTC) / 1000.0f;
	COTED3DManager::s_ElapsedTime += COTED3DManager::s_FrameTime;
	sOldTC = ::GetTickCount();

}

//-----------------------------------------------------------------------
void COTERender::FrameEnded(float dTimeSinceLstFrm)
{	
}

//-----------------------------------------------------------------------
void COTERender::OnDestroyScene()
{
	COTESky::GetInstance()->Destroy();
}

#endif
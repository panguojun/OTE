#include "OTEEntity.h"
#include "OgreHardwareBufferManager.h"
#include "OgreSkeletonInstance.h"
#include "OgreStringConverter.h"
#include "OTEEntityRender.h"
#include "OgreD3D9Texture.h"

#ifdef __OTE
#include "OTEQTSceneManager.h"
#endif
//-----------------------------------------------------------------------
using namespace Ogre;
using namespace OTE ;

//-----------------------------------------------------------------------
String COTEEntity::msMovableType = "OTEEntity";
//-----------------------------------------------------------------------
COTEEntity::COTEEntity() 
{
    mFullBoundingBox = new AxisAlignedBox(-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f);
    mNormaliseNormals = false;
    mFrameBonesLastUpdated = new unsigned long;
    *mFrameBonesLastUpdated = 0;
    mFrameAnimationLastUpdated = 0;
    mHardwareSkinning = false;
    mSkeletonInstance = 0;
	mSharedBlendedVertexData = NULL;
}
//-----------------------------------------------------------------------
COTEEntity::COTEEntity( const String& name, MeshPtr& mesh)       
{
    mFullBoundingBox = new AxisAlignedBox;
    mHardwareSkinning = false;
    mSharedBlendedVertexData = NULL;

	init(name, mesh);
}

//-----------------------------------------------------------------------
void COTEEntity::init(const String& name, MeshPtr& mesh)
{
	mName = name;
	mMesh = mesh;
		
	// Is mesh skeletally animated?
    if (mMesh->hasSkeleton() && !mMesh->getSkeleton().isNull())
    {
        mSkeletonInstance = new SkeletonInstance(mMesh->getSkeleton());
        mSkeletonInstance->load();
    }
    else
    {
        mSkeletonInstance = 0;
    }
#ifndef __ZG

    // Build main subentity list
	//if (mMesh->isManuallyLoaded()) // by romeo
		buildSubEntityList(mesh, &mSubEntityList);

#endif

    // Initialise the AnimationState, if Mesh has animation

    if (hasSkeleton())
    {
        mAnimationState = new AnimationStateSet();
		if(!mFrameBonesLastUpdated)			
			mFrameBonesLastUpdated = new unsigned long;
        *mFrameBonesLastUpdated = 0;

        mesh->_initAnimationState(mAnimationState);
        mNumBoneMatrices = mSkeletonInstance->getNumBones();
        mBoneMatrices = new Matrix4[mNumBoneMatrices];
        prepareTempBlendBuffers();
    }
    else
    {
        mBoneMatrices = 0;
        mNumBoneMatrices = 0;
        mAnimationState = 0;
        mFrameBonesLastUpdated  = 0;

    }

    reevaluateVertexProcessing();

    mDisplaySkeleton = false;

    mFrameAnimationLastUpdated = 0;

    setCastShadows(false);  
}

//-----------------------------------------------------------------------
COTEEntity::~COTEEntity()
{
    // Delete submeshes
    OTESubEntityList::iterator i, iend;
    iend = mSubEntityList.end();
    for (i = mSubEntityList.begin(); i != iend; ++i)
    {
        // Delete COTESubEntity
        delete *i;
    }
   
    delete mFullBoundingBox;   

    // Detach all child objects, do this manually to avoid needUpdate() call 
    // which can fail because of deleted items
    //detachAllObjectsImpl();

    if (mSkeletonInstance) 
	{        
        delete mSkeletonInstance;
        delete [] mBoneMatrices;
        delete mAnimationState;
        delete mFrameBonesLastUpdated;
        
    }
}
//-----------------------------------------------------------------------
MeshPtr& COTEEntity::getMesh(void)
{
    return mMesh;
}
//-----------------------------------------------------------------------
const String& COTEEntity::getName(void) const
{
    return mName;
}
//-----------------------------------------------------------------------
COTESubEntity* COTEEntity::getSubEntity(unsigned int index)
{
    if (index >= mSubEntityList.size())
        OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
        "Index out of bounds.",
        "COTEEntity::getSubEntity");
    return mSubEntityList[index];
}
//-----------------------------------------------------------------------
COTESubEntity* COTEEntity::getSubEntity(const String& name)
{
    ushort index = mMesh->_getSubMeshIndex(name);
    return getSubEntity(index);
}
//-----------------------------------------------------------------------
unsigned int COTEEntity::getNumSubEntities(void) const
{
    return static_cast< unsigned int >( mSubEntityList.size() );
}

//-----------------------------------------------------------------------
void COTEEntity::setMaterialName(const String& name)
{
    // Set for all subentities
    OTESubEntityList::iterator i;
    for (i = mSubEntityList.begin(); i != mSubEntityList.end(); ++i)
    {
        (*i)->setMaterialName(name);
    }

}
//-----------------------------------------------------------------------
void COTEEntity::_notifyCurrentCamera(Camera* cam)
{    
    // Notify any child objects
    OTEChildObjectList::iterator child_itr = mChildObjectList.begin();
    OTEChildObjectList::iterator child_itr_end = mChildObjectList.end();
    for( ; child_itr != child_itr_end; child_itr++)
    {
        (*child_itr).second->_notifyCurrentCamera(cam);
    }

}
//-----------------------------------------------------------------------
const AxisAlignedBox& COTEEntity::getBoundingBox(void) const
{
    // Get from Mesh
	if(!mMesh.isNull())
	{
		*mFullBoundingBox = mMesh->getBounds();
		 mFullBoundingBox->merge(getChildObjectsBoundingBox());
	}

    // Don't scale here, this is taken into account when world BBox calculation is done

    return *mFullBoundingBox;
}
//-----------------------------------------------------------------------
AxisAlignedBox COTEEntity::getChildObjectsBoundingBox(void) const
{
    AxisAlignedBox aa_box;
    AxisAlignedBox full_aa_box;
    full_aa_box.setNull();

    OTEChildObjectList::const_iterator child_itr = mChildObjectList.begin();
    OTEChildObjectList::const_iterator child_itr_end = mChildObjectList.end();
    for( ; child_itr != child_itr_end; child_itr++)
    {
        aa_box = child_itr->second->getBoundingBox();
      //  TagPoint* tp = (TagPoint*)child_itr->second->getParentNode();
        // Use transform local to skeleton since world xform comes later
        //aa_box.transform(tp->_getFullLocalTransform());

        full_aa_box.merge(aa_box);
    }

    return full_aa_box;
}
//-----------------------------------------------------------------------
void COTEEntity::_updateRenderQueue(RenderQueue* queue)
{
	if(mMesh.isNull())
		return;    

    // Add each visible COTESubEntity to the queue
    OTESubEntityList::iterator i, iend;
    iend = mSubEntityList.end();
    for (i = mSubEntityList.begin(); i != iend; ++i)
    {
        if((*i)->isVisible())  
        {
            if(mRenderQueueIDSet) 
            {
                queue->addRenderable(*i, mRenderQueueID);
            } 
            else 
            {
                queue->addRenderable(*i);
            }
        }
    }

    // Since we know we're going to be rendered, take this opportunity to 
    // update the animation
    if (hasSkeleton())
    {
        updateAnimation();

        //--- pass this point,  we are sure that the transformation matrix of each bone and tagPoint have been updated			
        OTEChildObjectList::iterator child_itr = mChildObjectList.begin();
        OTEChildObjectList::iterator child_itr_end = mChildObjectList.end();
        for( ; child_itr != child_itr_end; child_itr++)
        {
            if ((*child_itr).second->isVisible())
                (*child_itr).second->_updateRenderQueue(queue);
        }
    }

    // HACK to display bones
    // This won't work if the entity is not centered at the origin
    // TODO work out a way to allow bones to be rendered when COTEEntity not centered
    if (mDisplaySkeleton && hasSkeleton())
    {
        int numBones = mSkeletonInstance->getNumBones();
        for (int b = 0; b < numBones; ++b)
        {
            Bone* bone = mSkeletonInstance->getBone(b);
            if(mRenderQueueIDSet) 
            {
                    queue->addRenderable(bone, mRenderQueueID);
            } else {
                    queue->addRenderable(bone);
            }
        }
    }
}

//-----------------------------------------------------------------------
AnimationState* COTEEntity::getAnimationState(const String& name)
{
    if (!mAnimationState)
    {
        OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "COTEEntity is not animated", 
            "COTEEntity::getAnimationState");
    }
    AnimationStateSet::iterator i = mAnimationState->find(name);

    if (i == mAnimationState->end())
    {
        OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "No animation entry found named " + name, 
            "COTEEntity::getAnimationState");
    }

    return &(i->second);
}
//-----------------------------------------------------------------------
AnimationStateSet* COTEEntity::getAllAnimationStates(void)
{
    return mAnimationState;
}
//-----------------------------------------------------------------------
const String& COTEEntity::getMovableType(void) const
{
    return msMovableType;
}
//-----------------------------------------------------------------------
void COTEEntity::updateAnimation(void)
{
    // We only do these tasks if they have not already been done for 
    // this frame 
    Root& root = Root::getSingleton();
    unsigned long currentFrameNumber = root.getCurrentFrameNumber();
    if (mFrameAnimationLastUpdated != currentFrameNumber)
    {
        cacheBoneMatrices();

        // Software blend?
        bool hwSkinning = isHardwareSkinningEnabled();
        if (!hwSkinning ||
            root._getCurrentSceneManager()->getShadowTechnique() == SHADOWTYPE_STENCIL_ADDITIVE ||
            root._getCurrentSceneManager()->getShadowTechnique() == SHADOWTYPE_STENCIL_MODULATIVE)
        {
            // Ok, we need to do a software blend
            // Blend normals in s/w only if we're not using h/w skinning,
            // since shadows only require positions
            bool blendNormals = !hwSkinning;
            // Firstly, check out working vertex buffers
            if (mSharedBlendedVertexData)
            {
                // Blend shared geometry
                // NB we suppress hardware upload while doing blend if we're
                // hardware skinned, because the only reason for doing this
                // is for shadow, which need only be uploaded then
                mTempBlendedBuffer.checkoutTempCopies(true, blendNormals);
                mTempBlendedBuffer.bindTempCopies(mSharedBlendedVertexData, 
                    mHardwareSkinning);
                Mesh::softwareVertexBlend(mMesh->sharedVertexData, 
                    mSharedBlendedVertexData, mBoneMatrices, blendNormals);
            }
            OTESubEntityList::iterator i, iend;
            iend = mSubEntityList.end();
            for (i = mSubEntityList.begin(); i != iend; ++i)
            {
                // Blend dedicated geometry
                COTESubEntity* se = *i;
                if (se->isVisible() && se->mBlendedVertexData)
                {
                    se->mTempBlendedBuffer.checkoutTempCopies(true, blendNormals);
                    se->mTempBlendedBuffer.bindTempCopies(se->mBlendedVertexData, 
                        mHardwareSkinning);
                    Mesh::softwareVertexBlend(se->mSubMesh->vertexData, 
                        se->mBlendedVertexData, mBoneMatrices, blendNormals);
                }
            }
        }

        // Trigger update of bounding box if necessary
        if (!mChildObjectList.empty())
            mParentNode->needUpdate();
        mFrameAnimationLastUpdated = currentFrameNumber;
    }
}
//-----------------------------------------------------------------------
void COTEEntity::cacheBoneMatrices(void)
{
    Root& root = Root::getSingleton();
    unsigned long currentFrameNumber = root.getCurrentFrameNumber();		
    if (*mFrameBonesLastUpdated  != currentFrameNumber) 
	{
        // Use normal mesh
        Mesh* theMesh = mMesh.getPointer();     

        mSkeletonInstance->setAnimationState(*mAnimationState);
        mSkeletonInstance->_getBoneMatrices(mBoneMatrices);
        *mFrameBonesLastUpdated  = currentFrameNumber;
      
        //--- Update the child object's transforms
        OTEChildObjectList::iterator child_itr = mChildObjectList.begin();
        OTEChildObjectList::iterator child_itr_end = mChildObjectList.end();
        for( ; child_itr != child_itr_end; child_itr++)
        {
            (*child_itr).second->getParentNode()->_update(true, true);
        }     

        // Apply our current world transform to these too, since these are used as
        // replacement world matrices
        unsigned short i;
        Matrix4 worldXform = _getParentNodeFullTransform();
        mNumBoneMatrices = mSkeletonInstance->getNumBones();

        for (i = 0; i < mNumBoneMatrices; ++i)
        {
            mBoneMatrices[i] = worldXform * mBoneMatrices[i];
        }
    }
}

//-----------------------------------------------------------------------
void COTEEntity::setDisplaySkeleton(bool display)
{
    mDisplaySkeleton = display;
}

//-----------------------------------------------------------------------
void COTEEntity::buildSubEntityList(MeshPtr& mesh, OTESubEntityList* sublist)
{
    // Create SubEntities
    unsigned short i, numSubMeshes;
    SubMesh* subMesh;
    COTESubEntity* subEnt;

    numSubMeshes = mesh->getNumSubMeshes();
    for (i = 0; i < numSubMeshes; ++i)
    {
        subMesh = mesh->getSubMesh(i);
        subEnt = new COTESubEntity(this, subMesh);
        if (subMesh->isMatInitialised())
            subEnt->setMaterialName(subMesh->getMaterialName());
        sublist->push_back(subEnt);
    }
}

//-----------------------------------------------------------------------
void COTEEntity::clearSubEntityList()
{
	OTESubEntityList::iterator i, iend;
    iend = mSubEntityList.end();

    for( i = mSubEntityList.begin(); i != iend; ++i ) 
    { 
        delete (*i);
    } 

	mSubEntityList.clear();
}

//-----------------------------------------------------------------------
Real COTEEntity::getBoundingRadius(void) const
{
    Real rad = mMesh->getBoundingSphereRadius();
    // Scale by largest scale factor
    if (mParentNode)
    {
        const Vector3& s = mParentNode->_getDerivedScale();
        rad *= std::max(s.x, std::max(s.y, s.z));
    }
    return rad;
}

//-----------------------------------------------------------------------
void COTEEntity::prepareTempBlendBuffers(void)
{
    if (mSharedBlendedVertexData) 
    {
        delete mSharedBlendedVertexData;
        mSharedBlendedVertexData = 0;
    }

    if (hasSkeleton())
    {
        // Shared data
        if (mMesh->sharedVertexData)
        {            
            mSharedBlendedVertexData = 
                cloneVertexDataRemoveBlendInfo(mMesh->sharedVertexData);
            extractTempBufferInfo(mSharedBlendedVertexData, &mTempBlendedBuffer);
        }

        OTESubEntityList::iterator i, iend;
        iend = mSubEntityList.end();
        for (i = mSubEntityList.begin(); i != iend; ++i)
        {
            COTESubEntity* s = *i;
            s->prepareTempBlendBuffers();
        }
    }
}

//-----------------------------------------------------------------------
void COTEEntity::extractTempBufferInfo(VertexData* sourceData, TempBlendedBufferInfo* info)
{
    VertexDeclaration* decl = sourceData->vertexDeclaration;
    VertexBufferBinding* bind = sourceData->vertexBufferBinding;
    const VertexElement *posElem = decl->findElementBySemantic(VES_POSITION);
    const VertexElement *normElem = decl->findElementBySemantic(VES_NORMAL);

    assert(posElem && "Positions are required");

    info->posBindIndex = posElem->getSource();
    info->srcPositionBuffer = bind->getBuffer(info->posBindIndex);

    if (!normElem)
    {
        info->posNormalShareBuffer = false;
        info->srcNormalBuffer.setNull();
    }
    else
    {
        info->normBindIndex = normElem->getSource();
        if (info->normBindIndex == info->posBindIndex)
        {
            info->posNormalShareBuffer = true;
            info->srcNormalBuffer.setNull();
        }
        else
        {
            info->posNormalShareBuffer = false;
            info->srcNormalBuffer = bind->getBuffer(info->normBindIndex);
        }
    }
}
//-----------------------------------------------------------------------
VertexData* COTEEntity::cloneVertexDataRemoveBlendInfo(const VertexData* source)
{
    // Clone without copying data
    VertexData* ret = source->clone(false);
    const VertexElement* blendIndexElem = 
        source->vertexDeclaration->findElementBySemantic(VES_BLEND_INDICES);
    const VertexElement* blendWeightElem = 
        source->vertexDeclaration->findElementBySemantic(VES_BLEND_WEIGHTS);
    // Remove blend index
    if (blendIndexElem)
    {
        // Remove buffer reference
        ret->vertexBufferBinding->unsetBinding(blendIndexElem->getSource());

    }
    if (blendWeightElem && 
        blendWeightElem->getSource() != blendIndexElem->getSource())
    {
        // Remove buffer reference
        ret->vertexBufferBinding->unsetBinding(blendWeightElem->getSource());
    }
    // remove elements from declaration
    ret->vertexDeclaration->removeElement(VES_BLEND_INDICES);
    ret->vertexDeclaration->removeElement(VES_BLEND_WEIGHTS);

    // Copy reference to wcoord buffer
    if (!source->hardwareShadowVolWBuffer.isNull())
        ret->hardwareShadowVolWBuffer = source->hardwareShadowVolWBuffer;

    return ret;
}

//-----------------------------------------------------------------------
void COTEEntity::reevaluateVertexProcessing(void)
{
    // init
    mHardwareSkinning = false; 
    mVertexProgramInUse = false; // assume false because we just assign this
    bool firstPass = true;

    OTESubEntityList::iterator i, iend;
    iend = mSubEntityList.end();
    for (i = mSubEntityList.begin(); i != iend; ++i, firstPass = false)
    {
        const MaterialPtr& m = (*i)->getMaterial();
        // Make sure it's loaded
        m->load();
        Technique* t = m->getBestTechnique();
        if (!t)
        {
            // No supported techniques
            continue;
        }
        Pass* p = t->getPass(0);
        if (!p)
        {
            // No passes, invalid
            continue;
        }
        if (p->hasVertexProgram())
        {
            // If one material uses a vertex program, set this flag 
            // Causes some special processing like forcing a separate light cap
            mVertexProgramInUse = true;

            // All materials must support skinning for us to consider using
            // hardware skinning - if one fails we use software
            if (firstPass)
            {
                mHardwareSkinning = p->getVertexProgram()->isSkeletalAnimationIncluded();
            }
            else
            {
                mHardwareSkinning = mHardwareSkinning &&
                    p->getVertexProgram()->isSkeletalAnimationIncluded();
            }
        }
    }

}

//-----------------------------------------------------------------------
const VertexData* COTEEntity::findBlendedVertexData(const VertexData* orig)
{
    if (orig == mMesh->sharedVertexData)
    {
        return mSharedBlendedVertexData;
    }
    OTESubEntityList::iterator i, iend;
    iend = mSubEntityList.end();
    for (i = mSubEntityList.begin(); i != iend; ++i)
    {
        COTESubEntity* se = *i;
        if (orig == se->getSubMesh()->vertexData)
        {
            return se->getBlendedVertexData();
        }
    }
    // None found
    OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, 
        "Cannot find blended version of the vertex data specified.",
        "COTEEntity::findBlendedVertexData");

	return NULL;
}
//-----------------------------------------------------------------------
COTESubEntity* COTEEntity::findSubEntityForVertexData(const VertexData* orig)
{
    if (orig == mMesh->sharedVertexData)
    {
        return 0;
    }

    OTESubEntityList::iterator i, iend;
    iend = mSubEntityList.end();
    for (i = mSubEntityList.begin(); i != iend; ++i)
    {
        COTESubEntity* se = *i;
        if (orig == se->getSubMesh()->vertexData)
        {
            return se;
        }
    }

    // None found
    return 0;
}
//-----------------------------------------------------------------------
void COTEEntity::_notifyAttached(Node* parent, bool isTagPoint)
{
    MovableObject::_notifyAttached(parent, isTagPoint);  

}

//-----------------------------------------------------------------------
void COTEEntity::setRenderQueueGroup(RenderQueueGroupID queueID)
{
    MovableObject::setRenderQueueGroup(queueID);  
}

// -------------------------------------------------
void COTEEntity::CloneMaterials()
{	
	int NumEnts = this->getNumSubEntities();
	for(int i = 0; i < NumEnts; i ++)
	{
		COTESubEntity* se = this->getSubEntity(i);	

		Ogre::MaterialPtr mptr = se->getMaterial();					

		if(!mptr->isLoaded())
			mptr->touch();

		std::stringstream ss;
		ss << mptr->getName() << i << getName();

		std::string matName = ss.str();
		Ogre::MaterialPtr mp = Ogre::MaterialManager::getSingleton().getByName(matName);
		if(mp.isNull())
		{
			mp = mptr->clone(matName);	
		}
		else
			mptr->copyDetailsTo(mp);

		se->setMaterialName(matName);			
	}
}

//-----------------------------------------------------------------------
bool COTEEntity::CheckLightmapValid()
{
	bool lightmapValid = true;

	int NumEnts = this->getNumSubEntities();
	for(int i = 0; i < NumEnts; i ++)
	{
		COTESubEntity* se = this->getSubEntity(i);
	
		// 存在两层材质吗？

		if(!se->getSubMesh()->vertexData->vertexDeclaration->findElementBySemantic(
															VES_TEXTURE_COORDINATES, 1))
		{
			lightmapValid = false;
			break;
		}
	}
	
	return lightmapValid;
}


//-----------------------------------------------------------------------
void COTEEntity::LoadLightmap(const std::string& rLightmap)
{	
	bool isMaterialCloned = false;

	int NumEnts = this->getNumSubEntities();
	for(int i = 0; i < NumEnts; i ++)
	{
		COTESubEntity* se = this->getSubEntity(i);
	
		// 存在两层材质吗？

		if(!se->getSubMesh()->vertexData->vertexDeclaration->findElementBySemantic(
															VES_TEXTURE_COORDINATES, 1))
			continue;

		// 先clone材质

		if(!isMaterialCloned)
			CloneMaterials();

		isMaterialCloned = true;

		// 第一层颜色一倍处理	
		
		Ogre::TextureUnitState* t = se->getMaterial()->getBestTechnique()->getPass(0)->getTextureUnitState(0);
		t->setColourOperation(LBO_REPLACE);	

		// 第二层

		t = se->getMaterial()->getBestTechnique()->getPass(0)->createTextureUnitState();
		t->setColourOperationEx(LBX_MODULATE_X2, LBS_TEXTURE, LBS_CURRENT);	

		

		// 光照图 设置在第二层

		t->setTextureCoordSet(1);
		t->setTextureName(rLightmap);

	}

	if(isMaterialCloned)
	{
		Ogre::TexturePtr texPtr = Ogre::TextureManager::getSingleton().getByName(			
			rLightmap
			);
		if(texPtr.isNull())
		{
	
			// 加载光照图

			//texPtr = TextureManager::getSingleton().load(
			//			rLightmap,
			//			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			//			TEX_TYPE_2D, 0);
			texPtr = TEX_RESMGR_LOAD(rLightmap);
		}

		Ogre::D3D9Texture* d3dtex = (Ogre::D3D9Texture*)texPtr.getPointer();	
		//d3dtex->getNormTexture()->GenerateMipSubLevels(); // MINMAPS
	}
}

// --------------------------------------------------------
// 读入顶点色数据

void COTEEntity::ReadVertexColorData(const Ogre::String& fileName)
{
	Ogre::DataStreamPtr stream = RESMGR_LOAD(fileName);	

	if(stream.isNull())
	{
		OTE_MSG_ERR("读入顶点色数据: " << fileName << " 失败！")
		return;
	}

	Mesh::SubMeshIterator smIt = getMesh()->getSubMeshIterator();
	while(smIt.hasMoreElements())
	{
		SubMesh * sm =smIt.getNext();
		if (!sm->useSharedVertices)
		{					
			const VertexElement* diffElem = sm->vertexData->vertexDeclaration->findElementBySemantic(VES_DIFFUSE);
			if(!diffElem)
				return;

			HardwareVertexBufferSharedPtr vbuf = sm->vertexData->vertexBufferBinding->getBuffer(diffElem->getSource());

			int numOfVerts = vbuf->getNumVertices();

			DWORD* pBuff = new DWORD[numOfVerts];		

			stream->read((char*)pBuff, numOfVerts * sizeof(DWORD));	

			char* vertex = static_cast<char*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));
			DWORD* pDiff;

			for(int i = 0; i < numOfVerts; ++ i)
			{
				diffElem->baseVertexPointerToElement(vertex + i * vbuf->getVertexSize(), (void**)&pDiff);	
				*pDiff = pBuff[i];				
			}	

			vbuf->unlock();	

			delete []pBuff;

		} 		
	}	

	//OTE_LOGMSG("读入顶点色数据: " << fileName);
}

// ----------------------------------------------
// 迪归调用搜索文件目录
#include <io.h>
void ScanfLightmapDir(const char* cPathName, std::map<std::string, std::string>& filesPathMap)
{
    //find the first file	
    _finddata_t tRec;
	static char tMypath[512];
	strcpy(tMypath, cPathName);
	strcat(tMypath,"\\*");

    intptr_t hFile=_findfirst(tMypath, &tRec);
    if(hFile == -1)
        return;
 
    //search all files recursively.
    do
    {
        if(   strlen(tRec.name) == 1 && tRec.name[0] == '.'
            ||strlen(tRec.name) == 2 && tRec.name[0] == '.' && tRec.name[1] == '.')
            continue;

        static char fullPath[512];
        strcpy(fullPath, cPathName);

        if(tRec.attrib & _A_SUBDIR)
        {
			strcat(fullPath, "\\");
			strcat(fullPath, tRec.name);
            ScanfLightmapDir(fullPath, filesPathMap);
        }
        else
        { 		
			if(!(tRec.attrib & _A_HIDDEN) && strstr(tRec.name, ".bmp") != 0)
			{
				filesPathMap[tRec.name] = fullPath;
				OTE_TRACE(tRec.name << " " << fullPath)
			}			
        }
    }
    while(_findnext( hFile, &tRec ) == 0);
    //close search handle
    _findclose(hFile);
}

// --------------------------------------------------------
void COTEEntity::SetupEntityRes()
{
	COTESubEntity* se = this->getSubEntity(0);

	// 存在两层材质吗？

	if(!se->getSubMesh()->vertexData->vertexDeclaration->findElementBySemantic(
														VES_TEXTURE_COORDINATES, 1))
		return;

	// 自动加载光照图

	static bool sIsScanfed = false;
	static std::map<std::string, std::string>	sLightmapFileMap;
	if(!sIsScanfed)
	{
		Ogre::String dfullpath;
		OTEHelper::GetFullPath(std::string(OTEHelper::GetOTESetting("ResConfig", "EntityLightmapFilePath")), dfullpath);	

		if(!dfullpath.empty())
		{			
			OTE_TRACE("---------------- 光照图文件列表 --------------------")
			ScanfLightmapDir(dfullpath.c_str(), sLightmapFileMap);		
			OTE_TRACE("----------------------------------------------------")			
		}

		sIsScanfed = true;
	}

	std::string lightmapName = getName() + "_LM.bmp";
	if(!sLightmapFileMap.empty() && sLightmapFileMap.find(lightmapName) != sLightmapFileMap.end())
	{
		LoadLightmap(lightmapName);
	}
	else
	{
		//// 自动加载顶点色

		//std::string verCorName = getName() + ".vcr";
		//if(CHECK_RES(verCorName))
		//{
		//	ReadVertexColorData(verCorName);
		//}
	}

}

#ifdef __ZH

// ======================================================================
// COTESceneMgr
// ======================================================================
COTESceneMgr	COTESceneMgr::s_Inst;
COTEEntity* COTESceneMgr::CreateEntity(	const std::string& File, 
										const std::string& Name,
										const Ogre::Vector3& rPosition, 
										const Ogre::Quaternion& rQuaternion,
										const Ogre::Vector3& rScaling, 
										const std::string& group,
										bool  vAttachToSceneNode	)
{		

	Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().getByName(Name);
	if(mesh.isNull())
	{
		mesh = MESH_RESMGR_LOAD(File);				
	}
	COTEEntity* e = new COTEEntity(Name, mesh);
	
	// 场景中物件节点被分成几个组 以便管理
	if(vAttachToSceneNode)
	{			
		AttachObjToSceneNode(e, Name, group);	
	
		/// 节点位置

		e->getParentSceneNode()->setPosition(rPosition);	
		e->getParentSceneNode()->setOrientation(rQuaternion);		
		e->getParentSceneNode()->setScale(rScaling);				
	}	
	m_EntityList[Name] = e;

	return e;

}

//	----------------------------------------------
void COTESceneMgr::AttachObjToSceneNode(Ogre::MovableObject* MO, 
										const std::string& rNodeName, 
										const std::string& Group)
{	
	Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManager(Ogre::ST_GENERIC);
	Ogre::SceneNode* sn = (Ogre::SceneNode*)sm->getRootSceneNode()->createChildSceneNode(rNodeName);

	sn->attachObject(MO);	
}
//-----------------------------------------------------------------------
void COTESceneMgr::ClearScene()
{
	// 移除静态实体

	std::map<std::string, COTEEntity*>::iterator i = m_EntityList.begin();
	std::map<std::string, COTEEntity*>::iterator iEnd = m_EntityList.end();

	for (; i != iEnd; ++i)
	{
		Ogre::SceneNode* sn = i->second->getParentSceneNode();
		delete (i->second);

		if(sn)
		{
			sn->getParentSceneNode()->removeAndDestroyChild(sn->getName());	
		}			
	}
	m_EntityList.clear();
}

//-----------------------------------------------------------------------
void COTESceneMgr::RemoveEntity(const Ogre::String& Name)
{
	std::map<std::string, COTEEntity*>::iterator i = m_EntityList.find(Name);
	if(i != m_EntityList.end())
	{
		RemoveEntity(i->second);
	}
}

//-----------------------------------------------------------------------
void COTESceneMgr::RemoveEntity(COTEEntity* ent)
{
	// 移除静态实体

	Ogre::SceneNode* sn = ent->getParentSceneNode();

	// 移除

	Ogre::MeshManager::getSingleton().remove(ent->getMesh()->getName());

	delete ent;

	if(sn)
	{
		sn->getParentSceneNode()->removeAndDestroyChild(sn->getName());	
	}	

}


#endif
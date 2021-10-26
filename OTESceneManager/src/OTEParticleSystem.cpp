#include "OTEParticlesystem.h"
#include "OTEQTSceneManager.h"
#include "OTETilePage.h"
#include "OgreD3D9RenderSystem.h" 
#include "OgreD3D9HardwareVertexBuffer.h" 
#include "OgreD3D9HardwareIndexBuffer.h" 
#include "OTEParticlesystemxmlserializer.h"
#include "OTED3DManager.h"

using namespace Ogre;
using namespace OTE;

// =============================================
// COTEParticleSystemRes
// =============================================
COTEParticleSystemRes::COTEParticleSystemRes(
						const Ogre::String& name,
						const Ogre::String& emitterTypeName, 
						const Ogre::String& affectorTypeName) :
						mVertexData(NULL),	mIndexData(NULL)
{

	if(emitterTypeName == "SimpleEmitter")
	{
		m_Emitter = new COTESimpleEmitter();
	}
	if(affectorTypeName == "SimpleAffector")
	{
		m_ParticleSystemAffector = new COTEParticleSystemAffector();
	}

	m_MaterialName = "particleMat";

	m_Name = name;
}
// ---------------------------------------
void COTEParticleSystemRes::ClearBuffer()
{
	if(mVertexData)
	{
		delete mVertexData;
		mVertexData = NULL;
	}
	if(mIndexData)
	{
		delete mIndexData;
		mIndexData = NULL;
	}
}

// ---------------------------------------
COTEParticleSystemRes::~COTEParticleSystemRes()
{
	ClearBuffer();

	if(m_Emitter)
	{
		m_Emitter->Show(NULL, false);
		delete m_Emitter;
	}

	if(m_ParticleSystemAffector)
		delete m_ParticleSystemAffector;
}

// ---------------------------------------
void COTEParticleSystemRes::CreateBuffer()
{
	mVertexData = new VertexData();
    mIndexData  = new IndexData();

	mVertexData->vertexCount = m_PoolSize * 4;
    mVertexData->vertexStart = 0;

    // Vertex declaration
    VertexDeclaration* decl = mVertexData->vertexDeclaration;
    VertexBufferBinding* binding = mVertexData->vertexBufferBinding;

    int offset = 0;
    decl->addElement(0, offset, VET_FLOAT3, VES_POSITION);
    offset += VertexElement::getTypeSize(VET_FLOAT3);
	decl->addElement(0, offset, VET_FLOAT3, VES_NORMAL);
    offset += VertexElement::getTypeSize(VET_FLOAT3);
    decl->addElement(0, offset, VET_COLOUR, VES_DIFFUSE);
    offset += VertexElement::getTypeSize(VET_COLOUR);
    decl->addElement(0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);
    offset += VertexElement::getTypeSize(VET_FLOAT2);
	decl->addElement(0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 1);

    mMainBuf = 
        HardwareBufferManager::getSingleton().createVertexBuffer(
            decl->getVertexSize(0),
            mVertexData->vertexCount, 
            HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
    // bind position and diffuses
    binding->setBinding(0, mMainBuf);


    mIndexData->indexStart = 0;
    mIndexData->indexCount = m_PoolSize * 6;

    mIndexData->indexBuffer = HardwareBufferManager::getSingleton().
        createIndexBuffer(HardwareIndexBuffer::IT_16BIT,
            mIndexData->indexCount,
            HardwareBuffer::HBU_STATIC_WRITE_ONLY);

    /* Create indexes (will be the same every frame)
        Using indexes because it means 1/3 less vertex transforms (4 instead of 6)

        Billboard layout relative to camera:

        2-----3
        |    /|
        |  /  |
        |/    |
        0-----1
    */

    ushort* pIdx = static_cast<ushort*>(
        mIndexData->indexBuffer->lock(0,
            mIndexData->indexBuffer->getSizeInBytes(),
            HardwareBuffer::HBL_DISCARD) );

    for(
        int idx, idxOff, bboard = 0;
        bboard < m_PoolSize;
        ++bboard )
    {
        // Do indexes
        idx    = bboard * 6;
        idxOff = bboard * 4;

        pIdx[idx] = static_cast<unsigned short>(idxOff); // + 0;, for clarity
        pIdx[idx+1] = static_cast<unsigned short>(idxOff + 1);
        pIdx[idx+2] = static_cast<unsigned short>(idxOff + 3);
        pIdx[idx+3] = static_cast<unsigned short>(idxOff + 0);
        pIdx[idx+4] = static_cast<unsigned short>(idxOff + 3);
        pIdx[idx+5] = static_cast<unsigned short>(idxOff + 2);

    }

    mIndexData->indexBuffer->unlock();
}

//-----------------------------------------------------------------------
void COTEParticleSystemRes::beginBillboards(void)
{
	if(!mVertexData)
		CreateBuffer();

    // Init num visible  
	mNumVisibleBillboards = 0;	
	m_CreateTime = ::GetTickCount() / 1000.0f;

    mLockPtr = static_cast<float*>( 
        mMainBuf->lock(HardwareBuffer::HBL_DISCARD) );

}

//-----------------------------------------------------------------------
// ��������Ҫ�ĳ�ʼ���ݷ�������

void COTEParticleSystemRes::genVertices(const Billboard_t& bb,
										float period, int groupIndex, int groupSize,
										const Ogre::Vector3& dirMin, const Ogre::Vector3& dirMax,
										float velMin, float velMax,
										const Ogre::ColourValue& corRangeMin, const Ogre::ColourValue& corRangeMax,
										float sizeMin, float sizeMax
									)
{
	mNumVisibleBillboards ++;

	float timeOffset = (period / groupSize) * groupIndex;					// period / groupSize ���ȵ���λ�� ������/���������			
	OTE_ASSERT(timeOffset < 100 && period < 100);							// ���ڲ����� 100s �൱�� ����2����
	float period_timeOffset = int(100 * period) * 100.0f + timeOffset;		// �����ڼ���ʱ��ƫ�ƺϲ���һ�����

	float size = Ogre::Math::RangeRandom(sizeMin, sizeMax);

	Ogre::Vector3 vel_dir = Ogre::Vector3(
		Ogre::Math::RangeRandom(dirMin.x * velMin, dirMax.x * velMax),
		Ogre::Math::RangeRandom(dirMin.y * velMin, dirMax.y * velMax),
		Ogre::Math::RangeRandom(dirMin.z * velMin, dirMax.z * velMax)
		);   // ���� * �ٶ�

	RGBA colour;
	Ogre::ColourValue cor = Ogre::ColourValue(
		Ogre::Math::RangeRandom(corRangeMin.r, corRangeMax.r),
		Ogre::Math::RangeRandom(corRangeMin.g, corRangeMax.g),
		Ogre::Math::RangeRandom(corRangeMin.b, corRangeMax.b)
		);

    Root::getSingleton().convertColourValue(cor, &colour);
	RGBA* pCol;
    static float basicTexData[8] = {
        0.0, 1.0,
        1.0, 1.0,
        0.0, 0.0,
        1.0, 0.0 };
    static float rotTexDataBase[8] = {
        -0.5, 0.5,
            0.5, 0.5,
        -0.5,-0.5,
            0.5,-0.5 };
    static float rotTexData[8];

	float* pTexData;

    // Texcoords
   /* if (mFixedTextureCoords)
    {
		pTexData = basicTexData;
    }
    else*/
    {
        const Real  cos_rot  ( Math::Cos(bb.rotation) );
        const Real  sin_rot  ( Math::Sin(bb.rotation) );

        rotTexData[0] = (cos_rot * rotTexDataBase[0]) + (sin_rot * rotTexDataBase[1]) + 0.5;
        rotTexData[1] = (sin_rot * rotTexDataBase[0]) - (cos_rot * rotTexDataBase[1]) + 0.5;

        rotTexData[2] = (cos_rot * rotTexDataBase[2]) + (sin_rot * rotTexDataBase[3]) + 0.5;
        rotTexData[3] = (sin_rot * rotTexDataBase[2]) - (cos_rot * rotTexDataBase[3]) + 0.5;

        rotTexData[4] = (cos_rot * rotTexDataBase[4]) + (sin_rot * rotTexDataBase[5]) + 0.5;
        rotTexData[5]= (sin_rot * rotTexDataBase[4]) - (cos_rot * rotTexDataBase[5]) + 0.5;

        rotTexData[6] = (cos_rot * rotTexDataBase[6]) + (sin_rot * rotTexDataBase[7]) + 0.5;
        rotTexData[7] = (sin_rot * rotTexDataBase[6]) - (cos_rot * rotTexDataBase[7]) + 0.5;
		
		pTexData = rotTexData;
    }

    // Left-top
	// Positions
    *mLockPtr++ = bb.position.x;
    *mLockPtr++ = bb.position.y;
    *mLockPtr++ = bb.position.z;

	// normals
	*mLockPtr++ = vel_dir.x;
    *mLockPtr++ = vel_dir.y;
    *mLockPtr++ = vel_dir.z;

	// Colour
	// Convert float* to RGBA*
    pCol = static_cast<RGBA*>(static_cast<void*>(mLockPtr));
    *pCol++ = colour;	
    // Update lock pointer
    mLockPtr = static_cast<float*>(static_cast<void*>(pCol));
	// Texture coords
	*mLockPtr++ = *pTexData++;
	*mLockPtr++ = *pTexData++;	

	*mLockPtr++ = period_timeOffset;	// ʱ��
	*mLockPtr++ = size;					// �ߴ�	

	// Right-top
	// Positions

    *mLockPtr++ = bb.position.x;
    *mLockPtr++ = bb.position.y;
    *mLockPtr++ = bb.position.z;

	// normals
	*mLockPtr++ = vel_dir.x;
    *mLockPtr++ = vel_dir.y;
    *mLockPtr++ = vel_dir.z;

	// Colour
	// Convert float* to RGBA*
    pCol = static_cast<RGBA*>(static_cast<void*>(mLockPtr));
    *pCol++ = colour;
    // Update lock pointer
    mLockPtr = static_cast<float*>(static_cast<void*>(pCol));
	// Texture coords
	*mLockPtr++ = *pTexData++;
	*mLockPtr++ = *pTexData++;

	*mLockPtr++ = period_timeOffset;		// ʱ��
	*mLockPtr++ = size;						// �ߴ�

	// Left-bottom
	// Positions
    *mLockPtr++ = bb.position.x;
    *mLockPtr++ = bb.position.y;
    *mLockPtr++ = bb.position.z;
	// normals
	*mLockPtr++ = vel_dir.x;
    *mLockPtr++ = vel_dir.y;
    *mLockPtr++ = vel_dir.z;

	// Colour
	// Convert float* to RGBA*
    pCol = static_cast<RGBA*>(static_cast<void*>(mLockPtr));
    *pCol++ = colour;
    // Update lock pointer
    mLockPtr = static_cast<float*>(static_cast<void*>(pCol));
	// Texture coords
	*mLockPtr++ = *pTexData++;
	*mLockPtr++ = *pTexData++;

	*mLockPtr++ = period_timeOffset;		// ʱ��
	*mLockPtr++ = size;						// �ߴ�

	// Right-bottom
	// Positions
    *mLockPtr++ = bb.position.x;
    *mLockPtr++ = bb.position.y;
    *mLockPtr++ = bb.position.z;

	// normals
	*mLockPtr++ = vel_dir.x;
    *mLockPtr++ = vel_dir.y;
    *mLockPtr++ = vel_dir.z;

	// Colour
	// Convert float* to RGBA*
    pCol = static_cast<RGBA*>(static_cast<void*>(mLockPtr));
    *pCol++ = colour;
    // Update lock pointer
    mLockPtr = static_cast<float*>(static_cast<void*>(pCol));
	// Texture coords
	*mLockPtr++ = *pTexData++;
	*mLockPtr++ = *pTexData++;

	*mLockPtr++ = period_timeOffset;		// ʱ��
	*mLockPtr++ = size;						// �ߴ�

}

//-----------------------------------------------------------------------
void COTEParticleSystemRes::endBillboards(void)
{
	mMainBuf->unlock();
}

// =============================================
// COTEParticleSystem
// =============================================
Ogre::String COTEParticleSystem::msMovableType = "PS";

// ---------------------------------------
COTEParticleSystem::COTEParticleSystem(const Ogre::String& name, COTEParticleSystemRes* res) :
m_Name(name), 
mBoundingRadius(1),
mCreateTime(0),
m_ResPtr(res)
{
	mAABB.setExtents(-5.0f, -5.0f, -5.0f, 5.0f, 5.0f, 5.0f);
}

COTEParticleSystem::~COTEParticleSystem(void)
{	
}

// -------------------------------------- 
void COTEParticleSystem::setMaterialName(const String& name)
{
	mMaterialName = name;

	mpMaterial = MaterialManager::getSingleton().getByName(name);

	if (mpMaterial.isNull())
		OGRE_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Could not find material " + name,
			"BillboardSet::setMaterialName" );

	if(!m_ResPtr->m_TextureName.empty())
	{
		mpMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName(m_ResPtr->m_TextureName);
	}
}

// -------------------------------------- 
void COTEParticleSystem::_notifyCurrentCamera(Ogre::Camera* cam)
{	
	// ��Ӧ������

	if(m_BindReactor && !(((CReactorBase*)m_BindReactor)->m_StateFlag & 0xFF000000))
	{
		((CReactorBase*)m_BindReactor)->Update(this, COTED3DManager::s_FrameTime);	
	}

	if(mCreateTime == 0 && m_ResPtr->m_Emitter)
	{
		if(!m_ResPtr->IsInit())
			m_ResPtr->m_Emitter->Emite(this);

		mCreateTime = ::GetTickCount() / 1000.0f;
		m_EntityInfo.x = mCreateTime;
	}

}

// -------------------------------------- 
void COTEParticleSystem::_updateRenderQueue(RenderQueue* queue)
{	
	if(mCreateTime >= 0.0f)
		queue->addRenderable(this);
}

//-----------------------------------------------------------------------
void COTEParticleSystem::getWorldTransforms( Matrix4* xform ) const
{     
     *xform = _getParentNodeFullTransform(); 
}

//-----------------------------------------------------------------------
const Quaternion& COTEParticleSystem::getWorldOrientation(void) const
{
    return mParentNode->_getDerivedOrientation();
}

//-----------------------------------------------------------------------
const Vector3& COTEParticleSystem::getWorldPosition(void) const
{
    return mParentNode->_getDerivedPosition();
}

//-----------------------------------------------------------------------
void COTEParticleSystem::getRenderOperation(Ogre::RenderOperation& op)
{
	op.operationType = RenderOperation::OT_TRIANGLE_LIST;
	op.useIndexes = true;

	op.vertexData = m_ResPtr->mVertexData;
	op.vertexData->vertexCount = m_ResPtr->mNumVisibleBillboards * 4;
	op.vertexData->vertexStart = 0;

	op.indexData = m_ResPtr->mIndexData;
	op.indexData->indexCount = m_ResPtr->mNumVisibleBillboards * 6;
	op.indexData->indexStart = 0;	
}

//-----------------------------------------------------------------------
void COTEParticleSystem::SetVisible(bool isVisible)
{	
	if(!m_IsVisible && isVisible && mCreateTime >= 0.0f)
	{
		mCreateTime = 0;
		m_ResPtr->m_Emitter->Emite(this);
	}

	m_IsVisible = isVisible;

	Ogre::MovableObject::setVisible(isVisible);
}

//-----------------------------------------------------------------------
// ����cg�������
void COTEParticleSystem::_updateCustomGpuParameter( const GpuProgramParameters::AutoConstantEntry& constantEntry, GpuProgramParameters* params) const
{	
	if( CG_BASE_ID == constantEntry.data ) // ʵ��֮��ʱ����λ��
	{
		m_EntityInfo.y = Ogre::Math::RangeRandom(0.0f, 1.0f); // �����	

		params->setNamedConstant("ENTITY_INFO", m_EntityInfo);		
	}	

	else if(!m_ResPtr->m_ParticleSystemAffector || !m_ResPtr->m_ParticleSystemAffector->UpdateGPUParams(constantEntry, params, this))
	{
		Renderable::_updateCustomGpuParameter(constantEntry, params);
	}
}

//-----------------------------------------------------------------------
Real COTEParticleSystem::getSquaredViewDepth(const Camera* const cam) const
{  
    return mParentNode->getSquaredViewDepth(cam);
}

//-----------------------------------------------------------------------
const LightList& COTEParticleSystem::getLights(void) const
{
    return getParentSceneNode()->findLights(this->getBoundingRadius());
}

// =============================================
// COTEParticleSystemAffector
// =============================================

bool COTEParticleSystemAffector::UpdateGPUParams(const GpuProgramParameters::AutoConstantEntry& constantEntry, Ogre::GpuProgramParameters* params, const COTEParticleSystem* ps) const
{
	std::map<Ogre::String, std::pair<int, Ogre::Real> >::iterator floatIt = m_floats.begin();
	
	while(floatIt != m_floats.end())
	{
		if( (floatIt->second).first == constantEntry.data )
		{			
			params->setNamedConstant(floatIt->first, (floatIt->second).second);
			return true;
		}

		++ floatIt;
	}
	
	std::map<Ogre::String, std::pair<int, Ogre::Vector3> >::iterator vector3It = m_vector3s.begin();
	while(vector3It != m_vector3s.end())
	{
		if( (vector3It->second).first == constantEntry.data )
		{			
			params->setNamedConstant(vector3It->first, (vector3It->second).second);
			return true;
		}

		++ vector3It;
	}  

	std::map<Ogre::String, std::pair<int, int> >::iterator intIt = m_ints.begin();
	while(intIt != m_ints.end())
	{
		if( (intIt->second).first == constantEntry.data )
		{			
			params->setNamedConstant(intIt->first, (int)(intIt->second).second);
			return true;
		}

		++ intIt;
	} 
	std::map<Ogre::String, std::pair<int, Ogre::ColourValue> >::iterator corIt = m_colors.begin();
	while(corIt != m_colors.end())	
	{
		if( (corIt->second).first == constantEntry.data )
		{			
			params->setNamedConstant(corIt->first, (corIt->second).second);
			return true;
		}

		++ corIt;
	} 

	return false;

}

// =============================================
// COTESimpleEmitter
// =============================================
#define GEN_PARTICLES(ix, iy, iz) {if(count >= ps->GetPoolSize())	break;COTEParticleSystemRes::Billboard_t bb;bb.position.x = width  / sideDens * ix;bb.position.y = height / sideDens * iy;bb.position.z = depth  / sideDens * iz;ps->GetResPtr()->genVertices(bb, time_to_live,(count ++) % emission_rate,	emission_rate,direction_min,				direction_max,	velocity_min,	velocity_max,	colour_range_start,colour_range_end,size_min,	size_max);}

void COTESimpleEmitter::Emite(COTEParticleSystem* ps)
{	
	if(ps->mCreateTime == 0)
	{
		/// ����CG����
		//  ���ձ�������Ӧ
	
		if(!(ps->getMaterial()->m_UserFlag & 0xFF000000))
		{
			Ogre::GpuProgramParametersSharedPtr gppPtr = ps->getMaterial()->getTechnique(0)->getPass(0)->getVertexProgramParameters(); 			

			gppPtr->setNamedAutoConstant("ENTITY_INFO", Ogre::GpuProgramParameters::AutoConstantType::ACT_CUSTOM, CG_BASE_ID);
			
			std::map<Ogre::String, std::pair<int, Ogre::Real> >::iterator floatIt = ps->GetResPtr()->m_ParticleSystemAffector->m_floats.begin();
			while(floatIt != ps->GetResPtr()->m_ParticleSystemAffector->m_floats.end())
			{				
				gppPtr->setNamedAutoConstant(floatIt->first, Ogre::GpuProgramParameters::AutoConstantType::ACT_CUSTOM, (floatIt->second).first);

				++ floatIt;
			}
			
			std::map<Ogre::String, std::pair<int, Ogre::Vector3> >::iterator vector3It = ps->GetResPtr()->m_ParticleSystemAffector->m_vector3s.begin();
			while(vector3It != ps->GetResPtr()->m_ParticleSystemAffector->m_vector3s.end())
			{			
				gppPtr->setNamedAutoConstant(vector3It->first, Ogre::GpuProgramParameters::AutoConstantType::ACT_CUSTOM, (vector3It->second).first);

				++ vector3It;
			}  

			std::map<Ogre::String, std::pair<int, int> >::iterator intIt = ps->GetResPtr()->m_ParticleSystemAffector->m_ints.begin();
			while(intIt != ps->GetResPtr()->m_ParticleSystemAffector->m_ints.end())
			{				
				gppPtr->setNamedAutoConstant(intIt->first, Ogre::GpuProgramParameters::AutoConstantType::ACT_CUSTOM, (intIt->second).first);
				
				++ intIt;
			}

			std::map<Ogre::String, std::pair<int, Ogre::ColourValue> >::iterator corIt = ps->GetResPtr()->m_ParticleSystemAffector->m_colors.begin();
			while(corIt != ps->GetResPtr()->m_ParticleSystemAffector->m_colors.end())
			{				
				gppPtr->setNamedAutoConstant(corIt->first, Ogre::GpuProgramParameters::AutoConstantType::ACT_CUSTOM, (corIt->second).first);
				
				++ corIt;
			}

			ps->getMaterial()->m_UserFlag |= 0xFF000000;			
		}

	}

	// ����billboards

	float sideDens = sqrt(float(ps->GetPoolSize()) / 6);
	int count = 0;

	float   angle = GetfloatVal("������");
    int		emission_rate = GetIntVal("������");
	float	time_to_live  = GetfloatVal("����");	
	float	width  = GetfloatVal("������_���");
	float	height = GetfloatVal("������_�߶�");
	float	depth  = GetfloatVal("������_����");	

	float	velocity_min = GetfloatVal("��С����");
	float	velocity_max = GetfloatVal("�������");	

	float	size_min = GetfloatVal("��С�ߴ�");
	float	size_max = GetfloatVal("���ߴ�");

	Ogre::Vector3 direction_min = GetVector3Val("��С����");
	Ogre::Vector3 direction_max = GetVector3Val("�����");

	Ogre::ColourValue colour_range_start = GetCorVal("��С��ɫ");
	Ogre::ColourValue colour_range_end   = GetCorVal("�����ɫ");

	ps->GetResPtr()->beginBillboards();	
		
		for(float x = 0; x <= sideDens;  x += sideDens)	
		for(float y = 0; y <  sideDens;  y += 1.0f)		
		for(float z = 0; z <  sideDens;  z += 1.0f)	
			GEN_PARTICLES((x - sideDens / 2.0f), (y - sideDens / 2.0f), (z - sideDens / 2.0f))

		for(float y = 0; y <= sideDens; y += sideDens)	
		for(float x = 0; x < sideDens;  x += 1.0f)		
		for(float z = 0; z < sideDens;  z += 1.0f)	
			GEN_PARTICLES((x - sideDens / 2.0f), (y - sideDens / 2.0f), (z - sideDens / 2.0f))

		for(float z = 0; z <= sideDens; z += sideDens)
		for(float x = 0; x < sideDens;  x += 1.0f)
		for(float y = 0; y < sideDens;  y += 1.0f)				
			GEN_PARTICLES((x - sideDens / 2.0f), (y - sideDens / 2.0f), (z - sideDens / 2.0f))
	
	ps->GetResPtr()->endBillboards();	

}

// --------------------------------------
void COTESimpleEmitter::Show(COTEParticleSystem* ps, bool isVisible)
{	return;	
	COTEActorEntity* ae = SCENE_MGR->GetEntity("emitter_helper");

	if(isVisible)
	{			
		if(!ae)
			ae = SCENE_MGR->CreateEntity("������������.mesh", "emitter_helper");
		
		ae->getParentSceneNode()->setPosition(ps->getWorldPosition());
		
		float	width  = GetfloatVal("������_���");
		float	height = GetfloatVal("������_�߶�");
		float	depth  = GetfloatVal("������_����");	
		
		ae->getParentSceneNode()->setScale(Ogre::Vector3(width, height, depth));
	}
	else
	{
		if(ae)
			SCENE_MGR->RemoveEntity(ae->getName());
	}
}

// =============================================
// COTEParticleSystemManager
// =============================================
COTEParticleSystemManager*		COTEParticleSystemManager::s_pInst = NULL;

// --------------------------------------
COTEParticleSystemManager* COTEParticleSystemManager::GetInstance()
{
	if(!COTEParticleSystemManager::s_pInst)
	{
		COTEParticleSystemManager::Init();
	}
	return COTEParticleSystemManager::s_pInst;
}

// --------------------------------------
void COTEParticleSystemManager::Init()
{
	if(s_pInst)
		return;
	
	s_pInst = new COTEParticleSystemManager();
	//Ogre::Root::getSingleton().addFrameListener(s_pInst);

}
// ---------------------------------------------
COTEParticleSystem* COTEParticleSystemManager::CreateParticleSystem(
											const Ogre::String& name, 
											const Ogre::String& resName,
											bool cloneMat)
{
	COTEParticleSystemRes* psr = GetParticleSystemRes(resName);	
	if(!psr)
	{
		COTEParticleSystemXmlSerializer xmlSerializer;
		xmlSerializer.Read(resName, resName);
		psr = xmlSerializer.GetParticleSystemRes();
	}

	return CreateParticleSystem(name, psr, cloneMat);

}

// ---------------------------------------------
void COTEParticleSystemManager::WriteXml(	const Ogre::String& name, 
										const Ogre::String& resName)
{
	COTEParticleSystemXmlSerializer psser;
	psser.Save(name, resName);

}

// ---------------------------------------------
void COTEParticleSystemManager::Destroy()
{	
	if(s_pInst)
	{
		//Ogre::Root::getSingleton().removeFrameListener(s_pInst);

		delete s_pInst;
		s_pInst = NULL;
	}
}

// ---------------------------------------------
void COTEParticleSystemManager::Clear()
{
	if(s_pInst)
	{	
		HashMap<Ogre::String, COTEParticleSystemRes*>::iterator it = s_pInst->m_PSResList.begin();
		while(it != s_pInst->m_PSResList.end())
		{
			if(it->second)
				delete it->second;
			++ it;
		}
		s_pInst->m_PSResList.clear();

		s_pInst->RemoveAllParticleSystems();		
	}
}

// ---------------------------------------------
COTEParticleSystem* COTEParticleSystemManager::CreateParticleSystem(
											const Ogre::String& name, 
											COTEParticleSystemRes* psr,
											bool cloneMat)
{
	COTEParticleSystem* ps = new COTEParticleSystem(name, psr);		

	ps->setRenderQueueGroup(SCENE_MGR->getWorldGeometryRenderQueue());		

	// Clone ����

	if(cloneMat)
	{
		Ogre::MaterialPtr mptr = Ogre::MaterialManager::getSingleton().getByName(name);
		if(mptr.isNull())
		{
			mptr = Ogre::MaterialManager::getSingleton().getByName(psr->m_MaterialName);
			if(mptr.isNull())
			{
				OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "����ϵͳ��������û���ҵ�!", "CreateParticleSystem");							
			}
			if(!mptr->isLoaded())
				mptr->touch();
			
			mptr->clone(name);	
			OTE_TRACE("Clone���� : " << name)
		}
		ps->setMaterialName(name);
	}
	else
	{
		ps->setMaterialName(psr->m_MaterialName);
	}	
	ps->getMaterial()->m_UserFlag = 0;

	m_ParticleSystems[name] = ps;

	OTE_TRACE("��������ϵͳ : " << name)

	return ps;
}



// ---------------------------------------------
void COTEParticleSystemManager::RemoveParticleSystem(const Ogre::String& name)
{
	HashMap<std::string, COTEParticleSystem*>::iterator it = m_ParticleSystems.find(name);
	if(it != m_ParticleSystems.end())
	{
		Ogre::SceneNode* sn = it->second->getParentSceneNode();			

		if(sn)
		{				
			sn->getParentSceneNode()->removeChild(sn);
			SCENE_MGR->destroySceneNode(sn->getName());				
		}	
		delete it->second;	

		m_ParticleSystems.erase(it);		

		OTE_TRACE("�Ƴ�����ϵͳ : " << name)
	}

	// ɾ������

	//if(Ogre::MaterialManager::getSingleton().resourceExists(name))
	//{
	//	Ogre::MaterialManager::getSingleton().remove(name);
	//	//OTE_TRACE("�Ƴ����� : " << name)
	//}

}

void COTEParticleSystemManager::RemoveAllParticleSystems()
{
	HashMap<std::string, COTEParticleSystem*>::iterator it = m_ParticleSystems.begin();
	while(it != m_ParticleSystems.end())
	{
		RemoveParticleSystem(it->first);
		it = m_ParticleSystems.begin();
	}
}

// ---------------------------------------------
bool COTEParticleSystemManager::frameStarted(const Ogre::FrameEvent& evt)
{
	return true;
}

// ---------------------------------------------
bool COTEParticleSystemManager::frameEnded(const Ogre::FrameEvent& evt)
{
	return true;
}

// ======================================
// COTEParticleSystemFactory
// ======================================
Ogre::MovableObject* COTEParticleSystemFactory::Create(const std::string& ResName,	const std::string& Name, bool cloneMat)
{
	return COTEParticleSystemManager::GetInstance()->CreateParticleSystem(Name, ResName, cloneMat);
}

// ---------------------------------------------
void COTEParticleSystemFactory::Delete(const std::string& Name)
{
	COTEParticleSystemManager::GetInstance()->RemoveParticleSystem(Name);
}
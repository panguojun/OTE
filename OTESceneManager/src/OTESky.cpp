#include "otesky.h"

#ifdef __OTE
#include "OTEMagicManager.h"
#include "OTEQTSceneManager.h"
#endif

using namespace Ogre;
using namespace OTE;

// -----------------------------------------------

#define MAX_SKYNAME_LEN		64					// ������Ƴ���

// ===============================================
// ���
// ===============================================

COTESky		COTESky::s_Inst;
COTESky*	COTESky::GetInstance()
{	
	return &s_Inst;	
}

// -----------------------------------------------
COTESky::COTESky() :
#ifdef __OTE
m_SunEffect(NULL),
#endif
m_IsVisible(true),
m_ReflectionHgt(0.0f),
m_SkyBaseEntity(NULL),
m_SkyTopBaseEntity(NULL),
m_SkyCylinderEntity(NULL),
m_SkyCloudEntity(NULL),
m_SkyMoveCloudEntity(NULL)
{
	
}

// -----------------------------------------------
// ����

void COTESky::Create()
{
	// 񷶥

	m_SkyBaseEntity = SCENE_MGR->CreateEntity("����.mesh", "sky_frame", 
									Ogre::Vector3::ZERO,
									Ogre::Quaternion::IDENTITY,
									Ogre::Vector3::UNIT_SCALE,
									"default",
									false
									);	

	WAIT_LOADING_RES(m_SkyBaseEntity)	

	m_SkyTopBaseEntity = SCENE_MGR->CreateEntity("񷶥.mesh", "sky_frametop", 
									Ogre::Vector3::ZERO,
									Ogre::Quaternion::IDENTITY,
									Ogre::Vector3::UNIT_SCALE,
									"default",
									false
									);	

	WAIT_LOADING_RES(m_SkyTopBaseEntity)

	// Բ����

	m_SkyCylinderEntity = SCENE_MGR->CreateEntity("Ͱ.mesh", "sky_Cyl", 
									Ogre::Vector3::ZERO,
									Ogre::Quaternion::IDENTITY,
									Ogre::Vector3::UNIT_SCALE,
									"default",
									false
									);	

	WAIT_LOADING_RES(m_SkyCylinderEntity)

	
	// �Ʋ�
	
	m_SkyCloudEntity = SCENE_MGR->CreateEntity("��ֹ.mesh", "sky_Cld", 
									Ogre::Vector3::ZERO,
									Ogre::Quaternion::IDENTITY,
									Ogre::Vector3::UNIT_SCALE,
									"default",
									false
									);	

	WAIT_LOADING_RES(m_SkyCloudEntity)

	m_SkyMoveCloudEntity = SCENE_MGR->CreateEntity("��̬.mesh", "sky_MovCld", 
									Ogre::Vector3::ZERO,
									Ogre::Quaternion::IDENTITY,
									Ogre::Vector3::UNIT_SCALE,
									"default",
									false
									);	

	WAIT_LOADING_RES(m_SkyMoveCloudEntity)

	SetSkyCloudRollSpeed(2.0f);

#ifdef __OTE

	SCENE_MGR->AddListener(COTEQTSceneManager::eBeforeRenderObjs, OnRender);

	SCENE_MGR->AddListener(COTEQTSceneManager::eSceneClear, Destroy);

	//// ̫��

	//m_SunBBS = (OTE::COTEBillBoardSet*)MAGIC_MGR->CreateMagic("sky_sun.BBSCom", "sky_sun", false); 
	//m_SunBBS->setMaterialName("SunBillBoardMat");

	//SCENE_MGR->AttachObjToSceneNode(m_SunBBS);

	//m_SunBBS->SetScale(Ogre::Vector3(80.0f, 80.0f, 80.0f));	
	//m_SunBBS->SetColour(Ogre::ColourValue(1.0f, 1.0f, 0.9f));

	//// ̫������ 

	//SAFE_DELETE(m_SunEffect)
	//m_SunEffect = new COTESunEffect();

#endif	
	

}
		
// -----------------------------------------------
void COTESky::SetSkyTextures(
						const Ogre::String& rBaseTexture,					
						const Ogre::String& rStaticCloudTexture,	
						const Ogre::String& rDymCloudTexture,
						const Ogre::String& rTopTexture
						)
{
	if(!rBaseTexture.empty())
		m_BaseTexture			=	rBaseTexture		;		// ������ͼ
	if(!rStaticCloudTexture.empty())
		m_StaticCloudTexture	=	rStaticCloudTexture	;		// ��̬�Ʋ�
	if(!rDymCloudTexture.empty())
		m_DymCloudTexture		=	rDymCloudTexture	;		// ��̬�Ʋ�
	if(!rTopTexture.empty())
		m_TopTexture			=	rTopTexture			;		// �����Ʋ�

	// ����

	if(!s_Inst.m_SkyBaseEntity)
	{
		s_Inst.Create();
	}	

	// ��װ!

	COTEEntity::OTESubEntityList::iterator it;

	if(!rBaseTexture.empty())
	{
		it = m_SkyBaseEntity->mSubEntityList.begin();
		while(it != m_SkyBaseEntity->mSubEntityList.end())
		{
			(*it)->getMaterial()->getBestTechnique()->getPass(0)->getTextureUnitState(0)->setTextureName(rBaseTexture);	
			++ it;
		}
	}
	if(!rStaticCloudTexture.empty())
	{
		it = m_SkyCloudEntity->mSubEntityList.begin();
		while(it != m_SkyCloudEntity->mSubEntityList.end())
		{
			(*it)->getMaterial()->getBestTechnique()->getPass(0)->getTextureUnitState(0)->setTextureName(rStaticCloudTexture);	
			++ it;
		}
	}
	if(!rDymCloudTexture.empty())
	{
		it = m_SkyMoveCloudEntity->mSubEntityList.begin();
		while(it != m_SkyMoveCloudEntity->mSubEntityList.end())
		{
			(*it)->getMaterial()->getBestTechnique()->getPass(0)->getTextureUnitState(0)->setTextureName(rDymCloudTexture);	
			++ it;
		}
	}
	if(!rTopTexture.empty())
	{
		it = m_SkyTopBaseEntity->mSubEntityList.begin();
		while(it != m_SkyTopBaseEntity->mSubEntityList.end())
		{
			(*it)->getMaterial()->getBestTechnique()->getPass(0)->getTextureUnitState(0)->setTextureName(rTopTexture);	
			++ it;
		}
	}
}

// -----------------------------------------------
void COTESky::Destroy(int ca)
{
	s_Inst.Clear(false);
}

// -----------------------------------------------
void COTESky::Clear(bool invalidDevice)
{
	if(!invalidDevice)
	{
#ifdef __OTE
		SAFE_DELETE(m_SunEffect)
#endif

		if(m_SkyBaseEntity)			{SCENE_MGR->RemoveEntity(m_SkyBaseEntity); m_SkyBaseEntity = NULL;			}
		if(m_SkyTopBaseEntity)		{SCENE_MGR->RemoveEntity(m_SkyTopBaseEntity);m_SkyTopBaseEntity = NULL;		}
		if(m_SkyCylinderEntity)		{SCENE_MGR->RemoveEntity(m_SkyCylinderEntity); m_SkyCylinderEntity = NULL;	}
		if(m_SkyCloudEntity)		{SCENE_MGR->RemoveEntity(m_SkyCloudEntity); m_SkyCloudEntity = NULL;		}
		if(m_SkyMoveCloudEntity)	{SCENE_MGR->RemoveEntity(m_SkyMoveCloudEntity); m_SkyMoveCloudEntity = NULL;}

	}
}

// -----------------------------------------------
void COTESky::InvalidateDevice()
{	
	Clear(true);	
}

// -----------------------------------------------
void COTESky::OnRender(int ca)
{
	#define SUBMESHTEXTRUE		tu->getTextureName()

	Ogre::Camera* cam = (Ogre::Camera*)ca;

	if( !(cam->m_RenderFlag & Ogre::Camera::RF_SKY) )
		return;

	if(IS_DEVICELOST) // �豸��ʧ
	{
		COTESky::s_Inst.InvalidateDevice();	
		return;
	}

	if(!s_Inst.m_SkyBaseEntity)
		s_Inst.Create();

	if(!COTESky::s_Inst.m_IsVisible) 
		return;

	// ����任

	D3DXMATRIX world, oldWorld, view, oldView;
	D3DXMatrixIdentity(&world);	
	world.m[3][1] = 0.0f;

	g_pDevice->GetTransform(D3DTS_WORLD,		&oldWorld);
	g_pDevice->SetTransform(D3DTS_WORLD,		&world);

	COTED3DManager::_notifyUpdater();
	view = *COTED3DManager::GetD3DViewMatrix(cam);

	g_pDevice->GetTransform(D3DTS_VIEW,			&oldView);	
	
	view.m[3][0] = 0.0f;	view.m[3][1] = 0.0f;	view.m[3][2] = 0.0f;	view.m[3][3] = 1.0f;
		
	g_pDevice->SetTransform(D3DTS_VIEW,			&view);
	g_pDevice->SetTransform(D3DTS_PROJECTION,   &COTED3DManager::s_PrjMat);

	// ������Ⱦ״̬

	g_pDevice->SetVertexShader(NULL);
	g_pDevice->SetPixelShader (NULL);
	
	g_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE);

	RENDERSYSTEM->_disableTextureUnitsFrom(0);

	RENDERSYSTEM->_setDepthBufferCheckEnabled(true);
	RENDERSYSTEM->_setDepthBufferWriteEnabled(true);
	RENDERSYSTEM->_setDepthBias(0);

	RENDERSYSTEM->setLightingEnabled(false);

	RENDERSYSTEM->_setAlphaRejectSettings(CMPF_ALWAYS_PASS, 0);		

	// �ر�ģ��

	g_pDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE);
	g_pDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);


	// ��͸������

	g_pDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD);
	g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	true			);
	g_pDevice->SetRenderState( D3DRS_SRCBLEND,	D3DBLEND_SRCALPHA		);
    g_pDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA	);

	// alpha����
	
	g_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, 1					);
	g_pDevice->SetRenderState(D3DRS_ALPHAFUNC,	D3DCMP_GREATER			);
	g_pDevice->SetRenderState(D3DRS_ALPHAREF,	3						);

	// ����ɫ

	g_pDevice->SetRenderState( D3DRS_LIGHTING, FALSE);
	g_pDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
	g_pDevice->SetRenderState( D3DRS_COLORVERTEX, TRUE);
	g_pDevice->SetRenderState(D3DRS_AMBIENT,				 0xffffffff			);
	g_pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE,	 D3DTA_TEXTURE		);

	// texture stage settings
	g_pDevice->SetTextureStageState(0,   D3DTSS_COLOROP,	   D3DTOP_DISABLE		);
	//g_pDevice->SetTextureStageState(0,   D3DTSS_COLOROP,     D3DTOP_ADD			);  
	//g_pDevice->SetTextureStageState(0,   D3DTSS_COLORARG1,   D3DTA_TEXTURE		);  	
	//g_pDevice->SetTextureStageState(0,   D3DTSS_COLORARG2,   D3DTA_DIFFUSE		); 

	g_pDevice->SetTextureStageState(0,   D3DTSS_ALPHAOP,	 D3DTOP_SELECTARG1	);
	g_pDevice->SetTextureStageState(0,   D3DTSS_ALPHAARG1,	 D3DTA_TEXTURE		);	

	// ��Ⱦ񷶥

	static Ogre::RenderOperation ro;
		
	g_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,		 D3DTOP_MODULATE );	
	g_pDevice->SetRenderState(D3DRS_FOGENABLE, 0 );

	COTEEntity::OTESubEntityList::iterator it = COTESky::s_Inst.m_SkyBaseEntity->mSubEntityList.begin();
	while(it != COTESky::s_Inst.m_SkyBaseEntity->mSubEntityList.end())
	{			
		Ogre::TextureUnitState* tu = (*it)->getMaterial()->getBestTechnique()->getPass(0)->getTextureUnitState(0);
		RENDERSYSTEM->_setTexture(0, true, SUBMESHTEXTRUE);

		(*it)->getRenderOperation(ro);	

		RENDERSYSTEM->_render(ro);

		++ it;
	}	

	it = COTESky::s_Inst.m_SkyTopBaseEntity->mSubEntityList.begin();
	while(it != COTESky::s_Inst.m_SkyTopBaseEntity->mSubEntityList.end())
	{			
		Ogre::TextureUnitState* tu = (*it)->getMaterial()->getBestTechnique()->getPass(0)->getTextureUnitState(0);
		RENDERSYSTEM->_setTexture(0, true, SUBMESHTEXTRUE);
	
		(*it)->getRenderOperation(ro);	

		RENDERSYSTEM->_render(ro);

		++ it;
	}		

	// ��Ⱦ�Ʋ�		

	it = COTESky::s_Inst.m_SkyCloudEntity->mSubEntityList.begin();
	while(it != COTESky::s_Inst.m_SkyCloudEntity->mSubEntityList.end())
	{			
		Ogre::TextureUnitState* tu = (*it)->getMaterial()->getBestTechnique()->getPass(0)->getTextureUnitState(0);
	
		RENDERSYSTEM->_setTexture(0, true, SUBMESHTEXTRUE);

		(*it)->getRenderOperation(ro);	

		RENDERSYSTEM->_render(ro);
		++ it;
	}

	D3DXMATRIX mat;	
	D3DXMatrixRotationY( &mat, float(D3DX_PI / 180.0f) * s_Inst.m_CloudRollSpeed * COTED3DManager::s_ElapsedTime );
	D3DXMatrixMultiply(&mat, &world, &mat);
	g_pDevice->SetTransform(D3DTS_WORLD,			&mat);

	it = COTESky::s_Inst.m_SkyMoveCloudEntity->mSubEntityList.begin();
	while(it != COTESky::s_Inst.m_SkyMoveCloudEntity->mSubEntityList.end())
	{			
		Ogre::TextureUnitState* tu = (*it)->getMaterial()->getBestTechnique()->getPass(0)->getTextureUnitState(0);
				
		RENDERSYSTEM->_setTexture(0, true, SUBMESHTEXTRUE);

		(*it)->getRenderOperation(ro);	

		RENDERSYSTEM->_render(ro);
		++ it;
	}

	// ��Ⱦ���Ͳ	

	g_pDevice->SetTransform(D3DTS_WORLD,			&world);

	g_pDevice->SetRenderState(D3DRS_FOGENABLE, true );

	it = COTESky::s_Inst.m_SkyCylinderEntity->mSubEntityList.begin();
	while(it != COTESky::s_Inst.m_SkyCylinderEntity->mSubEntityList.end())
	{			
		Ogre::TextureUnitState* tu = (*it)->getMaterial()->getBestTechnique()->getPass(0)->getTextureUnitState(0);
			
		RENDERSYSTEM->_setTexture(0, true, SUBMESHTEXTRUE);

		static Ogre::RenderOperation ro;	
		(*it)->getRenderOperation(ro);
		
		RENDERSYSTEM->_render(ro);
		++ it;
	}
	
	/// ��ԭ

	// ��Ⱦ״̬

	g_pDevice->SetTextureStageState(0,	 D3DTSS_COLOROP,	 D3DTOP_MODULATE2X  ); 
	g_pDevice->SetTextureStageState(0,   D3DTSS_COLORARG1,   D3DTA_TEXTURE		);  	
	g_pDevice->SetTextureStageState(0,   D3DTSS_COLORARG2,   D3DTA_CURRENT		); 


	// �Ѿ����û�
	
	g_pDevice->SetTransform(D3DTS_WORLD,					 &oldWorld			);
	g_pDevice->SetTransform(D3DTS_VIEW,						 &oldView			);

	// ̫��		

	/*if(SCENE_MAINLIGHT)
	{
		s_Inst.m_SunBBS->getParentSceneNode()->setPosition(
			SCENE_CAM->getPosition() - 
			SCENE_MAINLIGHT->getDirection() * 400.0f);

		s_Inst.m_SunEffect->Update();
	}*/

	//COTESky::s_Inst.m_pEffect->EndPass();	 
	//COTESky::s_Inst.m_pEffect->End();

}

// -----------------------------------------------
void COTESky::SetSunTexture(const Ogre::String& rSunTexture)
{
#ifdef __OTE
	m_SunBBS->getMaterial()->getBestTechnique()->getPass(0)->getTextureUnitState(0)->setTextureName(rSunTexture);	
#endif
}

// -----------------------------------------------
void COTESky::SetSunColor(const Ogre::ColourValue& rSunColor)
{
#ifdef __OTE
	m_SunBBS->SetColour(rSunColor);
#endif
}

// ===============================================
// COTESkyMgr
// ��չ�����
// ===============================================
COTESkyMgr	COTESkyMgr::s_Inst;
COTESkyMgr* COTESkyMgr::GetInstance()
{
	return &s_Inst;
}

// -----------------------------------------------
/// �ļ�����

void COTESkyMgr::CreateFromFile(const Ogre::String& rFileName)
{	
	if(!rFileName.empty())
	{
		if(!CHECK_RES(rFileName))
		{
			OTE_MSG_ERR("�����������ʧ��! FILE: " << rFileName)			
			return;
		}

		Ogre::DataStreamPtr stream = RESMGR_LOAD(rFileName);
		
		if(stream.isNull())
		{
			OTE_MSG_ERR("�����������ʧ��! FILE: " << rFileName)
			return;
		}

		// ������ͼ 

		char _BaseTexture[MAX_SKYNAME_LEN];
		char _StaticCloudTexture[MAX_SKYNAME_LEN];
		char _DymCloudTexture[MAX_SKYNAME_LEN];
		char _TopTexture[MAX_SKYNAME_LEN];

		stream->read(_BaseTexture, MAX_SKYNAME_LEN);		
		stream->read(_StaticCloudTexture, MAX_SKYNAME_LEN);	
		stream->read(_DymCloudTexture, MAX_SKYNAME_LEN);	
		stream->read(_TopTexture, MAX_SKYNAME_LEN);	

		// ������պ���ͼ

		COTESky::GetInstance()->SetSkyTextures(	_BaseTexture, 
												_StaticCloudTexture,
												_DymCloudTexture,
												_TopTexture
												);	

		// �Ʋ��˶��ٶ� 

		float _CloudRollSpeed;
		stream->read(&_CloudRollSpeed, sizeof(float));	

		// �����Ʋ��˶��ٶ�

		COTESky::GetInstance()->SetSkyCloudRollSpeed(_CloudRollSpeed);

	}
}

// -----------------------------------------------
void COTESkyMgr::SaveToFile(const Ogre::String& rFileName)
{
	FILE* file = fopen(rFileName.c_str(), "wb");
	if(!file)
	{
		OTE_MSG("����������ļ�ʧ�ܣ�", "ʧ��"); 
		return;
	}

	// ������ͼ

	if( COTESky::GetInstance()->m_BaseTexture.length()		  > MAX_SKYNAME_LEN		||
		COTESky::GetInstance()->m_StaticCloudTexture.length() > MAX_SKYNAME_LEN		||
		COTESky::GetInstance()->m_DymCloudTexture.length()    > MAX_SKYNAME_LEN		||
		COTESky::GetInstance()->m_TopTexture.length()         > MAX_SKYNAME_LEN		)
	{
		OTE_MSG("��ͼ���ƹ���(����С��64��Ӣ���ַ�)!", "����")
		fclose(file);	
		return;
	}

	fwrite( COTESky::GetInstance()->m_BaseTexture.c_str(),		  MAX_SKYNAME_LEN, 1, file );	
	fwrite( COTESky::GetInstance()->m_StaticCloudTexture.c_str(), MAX_SKYNAME_LEN, 1, file );	
	fwrite( COTESky::GetInstance()->m_DymCloudTexture.c_str(),    MAX_SKYNAME_LEN, 1, file );	
	fwrite( COTESky::GetInstance()->m_TopTexture.c_str(),         MAX_SKYNAME_LEN, 1, file );

	// �����ٶ�

	fwrite( &COTESky::GetInstance()->m_CloudRollSpeed, sizeof(float), 1, file );	

	fclose(file);	
}


#ifdef __OTE

// ===============================================
// ̫������Ч��������
// ===============================================
COTESunEffBillBoardSet::COTESunEffBillBoardSet(const std::string& name, int poolSize) 
: COTEBillBoardSet(name, poolSize)
{

}

// -----------------------------------------------
void COTESunEffBillBoardSet::getRenderOperation(RenderOperation& op)
{
	COTEBillBoardSet::getRenderOperation(op);

	// ���ݲ���

	HRESULT hr;
	assert( (g_pDevice != NULL) && "g_pDevice == NULL");

	const Ogre::Vector3& lightDir = SCENE_MAINLIGHT->getDirection();
	const Ogre::Vector3& cam_direction = SCENE_CAM->getDirection();
	const Ogre::Vector3& cam_position = SCENE_CAM->getPosition();

	if (FAILED(hr = g_pDevice->SetVertexShaderConstantF(5, lightDir.val, 1		))	||
		FAILED(hr = g_pDevice->SetVertexShaderConstantF(6, cam_direction.val, 1	))
		//FAILED(hr = g_pDevice->SetVertexShaderConstantF(7, cam_position.val, 1	))
		)
	{
		OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, 
			"��shader���ݲ���ʱʧ�ܣ�", 
			"COTESunEffBillBoardSet::getRenderOperation");
	}	

}

// -----------------------------------------------
void COTESunEffBillBoardSet::_notifyCurrentCamera(Camera* cam)
{
	COTEBillBoardSet::_notifyCurrentCamera(cam);

}

// ===============================================
// ̫������Ч��
// ===============================================

COTESunEffect::COTESunEffect()
{	
	Create();
}

// -----------------------------------------------
void COTESunEffect::Create()
{
	Ogre::Billboard* bb;

	// uv��ת����

	static float c_rotTexDataBase[8] = {
										-0.5f,	 0.5f,
										 0.5f,	 0.5f,
										-0.5f,	-0.5f,
										 0.5f,	-0.5f };

	// �ϲ�Ч��

	m_SunEffectUpper = new COTESunEffBillBoardSet( "sky_sun_uppereff", 4 );	
	m_SunEffectUpper->m_BillBoardType = COTEBillBoardSet::eFree;	
	m_SunEffectUpper->setMaterialName("SunEffectBillBoardMat");
	
	SCENE_MGR->AttachObjToSceneNode(m_SunEffectUpper);

	m_SunEffectUpper->SetColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f));	

	// uv����
	
	static float c_basicTexData1[8]  =	{
			0.0f,			1.0f / 3.0f,
			1.0f / 3.0f,	1.0f / 3.0f,
			0.0f,			0.0f,
			1.0f / 3.0f,	0.0f		};

	static float c_basicTexData2[8]  =	{
			1.0f / 3.0f,	1.0f / 3.0f,
			2.0f / 3.0f,	1.0f / 3.0f,
			1.0f / 3.0f,	0.0f,
			2.0f / 3.0f,	0.0f	};	

	static float c_basicTexData3[8]  =	{
			2.0f / 3.0f,	1.0f / 3.0f,
			3.0f / 3.0f,	1.0f / 3.0f,
			2.0f / 3.0f,	0.0f,
			3.0f / 3.0f,	0.0f		};	


	bb = m_SunEffectUpper->createBillboard(0.0f, 0.0f, -40.0f);
	bb->setDimensions(25.0f, 25.0f);
	bb->m_RotTexDataBase		= c_rotTexDataBase;
	bb->m_BasicTexData			= c_basicTexData1;

	bb = m_SunEffectUpper->createBillboard(0.0f, 0.0f, -20.0f);
	bb->setDimensions(15.0f, 15.0f);
	bb->m_RotTexDataBase		= c_rotTexDataBase;
	bb->m_BasicTexData			= c_basicTexData1;

	bb = m_SunEffectUpper->createBillboard(0.0f, 0.0f, -10.0f);
	bb->setDimensions(10.0f, 10.0f);
	bb->m_RotTexDataBase		= c_rotTexDataBase;
	bb->m_BasicTexData			= c_basicTexData3;

	bb = m_SunEffectUpper->createBillboard(0.0f, 0.0f, -5.0f);
	bb->setDimensions(2.0f, 2.0f);
	bb->m_RotTexDataBase		= c_rotTexDataBase;
	bb->m_BasicTexData			= c_basicTexData2;

	// �²�Ч��

	m_SunEffectLower = new COTESunEffBillBoardSet( "sky_sun_lowereff", 6 );	
	m_SunEffectLower->m_BillBoardType = COTEBillBoardSet::eFree;	
	m_SunEffectLower->setMaterialName("SunEffectBillBoardMat");
	
	SCENE_MGR->AttachObjToSceneNode(m_SunEffectLower);

	m_SunEffectLower->SetColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f));

	// uv����

	static float c_basicTexData4[8]  =	{
			0.0f,			2.0f / 3.0f,
			1.0f / 3.0f,	2.0f / 3.0f,
			0.0f,			1.0f / 3.0f,
			1.0f / 3.0f,	1.0f / 3.0f	};

	static float c_basicTexData5[8]  =	{
			1.0f / 3.0f,	2.0f / 3.0f,
			2.0f / 3.0f,	2.0f / 3.0f,
			1.0f / 3.0f,	1.0f / 3.0f,
			2.0f / 3.0f,	1.0f / 3.0f };	

	static float c_basicTexData6[8]  =	{
			2.0f / 3.0f,	2.0f / 3.0f,
			3.0f / 3.0f,	2.0f / 3.0f,
			2.0f / 3.0f,	1.0f / 3.0f,
			3.0f / 3.0f,	1.0f / 3.0f	};	


	bb = m_SunEffectLower->createBillboard(0.0f, 0.0f, 40.0f);
	bb->setDimensions(3.0f, 3.0f);
	bb->m_RotTexDataBase	= c_rotTexDataBase;
	bb->m_BasicTexData		= c_basicTexData4;

	bb = m_SunEffectLower->createBillboard(0.0f, 0.0f, 30.0f);
	bb->setDimensions(5.0f, 5.0f);
	bb->m_RotTexDataBase	= c_rotTexDataBase;
	bb->m_BasicTexData		= c_basicTexData5;

	bb = m_SunEffectLower->createBillboard(0.0f, 0.0f, 15.0f);
	bb->setDimensions(5.0f, 5.0f);
	bb->m_RotTexDataBase	= c_rotTexDataBase;
	bb->m_BasicTexData		= c_basicTexData6;

	bb = m_SunEffectLower->createBillboard(0.0f, 0.0f, 10.0f);
	bb->setDimensions(4.0f, 4.0f);
	bb->m_RotTexDataBase	= c_rotTexDataBase;
	bb->m_BasicTexData		= c_basicTexData4;

	bb = m_SunEffectLower->createBillboard(0.0f, 0.0f, 8.0f);
	bb->setDimensions(3.0f, 3.0f);
	bb->m_RotTexDataBase	= c_rotTexDataBase;
	bb->m_BasicTexData		= c_basicTexData4;

	bb = m_SunEffectLower->createBillboard(0.0f, 0.0f, 4.0f);
	bb->setDimensions(2.0f, 2.0f);
	bb->m_RotTexDataBase	= c_rotTexDataBase;
	bb->m_BasicTexData		= c_basicTexData4;
}

// -----------------------------------------------
void COTESunEffect::Update()
{
	Ogre::Vector3 pos;

	const Ogre::Vector3& lgtdir = SCENE_MAINLIGHT->getDirection();
	const Ogre::Vector3& camdir = SCENE_CAM->getDirection();

	float delta = lgtdir.dotProduct(- camdir);
	if(delta > 0.7f)
	{
		pos = SCENE_CAM->getPosition() + camdir * 10.0f;
	}
	else
	{
		// ������ɼ��Ͱ�������̫����λ����

		pos = SCENE_CAM->getPosition() - camdir * 200.0f;
	}
		
	m_SunEffectLower->getParentSceneNode()->setPosition(pos);
	m_SunEffectUpper->getParentSceneNode()->setPosition(pos);
}

// -----------------------------------------------
COTESunEffect::~COTESunEffect()
{	
	SCENE_MGR->DetachObjAndDestroyNode(m_SunEffectLower);
	SAFE_DELETE(m_SunEffectLower)

	SCENE_MGR->DetachObjAndDestroyNode(m_SunEffectUpper);
	SAFE_DELETE(m_SunEffectUpper)
}

#endif
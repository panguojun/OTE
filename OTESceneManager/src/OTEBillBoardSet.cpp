#include "otebillboardset.h"
#include "OTED3DManager.h"
#include "OTEQTSceneManager.h"
#include "OTED3DManager.h"

using namespace Ogre;
using namespace OTE;

// ------------------------------------------
COTEBillBoardSet::COTEBillBoardSet(void) :
m_Scale(Ogre::Vector3::UNIT_SCALE),
m_Colour(Ogre::ColourValue::White)
{
}

// ------------------------------------------
COTEBillBoardSet::COTEBillBoardSet(const std::string& name, int poolSize) :
Ogre::BillboardSet(name, poolSize)
{
	
}

COTEBillBoardSet::~COTEBillBoardSet(void)
{
}

//-----------------------------------------------------------------------
void COTEBillBoardSet::SetScale(const Vector3& s)
{
	mDefaultWidth	= s.x;
	mDefaultHeight	= s.y;
}

//-----------------------------------------------------------------------
void COTEBillBoardSet::SetAngle(const Ogre::Radian& angle)
{
	m_Angle = angle;
}

//-----------------------------------------------------------------------
void COTEBillBoardSet::SetRotation(const Ogre::Vector3& angles, const Ogre::Quaternion& quat)
{
	switch (m_BillBoardType)
    {
    case eCommon: // 普通公告牌儿
		{		
			SetAngle(Ogre::Radian(Ogre::Degree(angles.z)));		
		}
        break;
    case eRotY: // 绕Y轴旋转的
		{	
			SetAngle(Ogre::Radian(Ogre::Degree(angles.y)));	
		}        
        break;
    case eFaceY: // 朝向Y轴的
        {	
			SetAngle(Ogre::Radian(Ogre::Degree(angles.y)));	
		
		}  
        break;
	case eFree: // 自由方向
		{
			m_Quaternion = quat;
		}

    }
}

//-----------------------------------------------------------------------
void COTEBillBoardSet::SetColour(const ColourValue& c)
{
	m_Colour = c;
	
}

// -------------------------------------- 
void COTEBillBoardSet::_notifyCurrentCamera(Ogre::Camera* cam)
{
	// 反应器更新

	if(m_BindReactor && !(((CReactorBase*)m_BindReactor)->m_StateFlag & 0xFF000000))
	{
		((CReactorBase*)m_BindReactor)->Update(this, COTED3DManager::s_FrameTime);	
	}
	BillboardSet::_notifyCurrentCamera(cam);
	
}

//-----------------------------------------------------------------------
void COTEBillBoardSet::getRenderOperation(RenderOperation& op)
{
	BillboardSet::getRenderOperation(op);

	// 传递参数

	HRESULT hr;
	OTE_ASSERT( (g_pDevice != NULL) && "g_pDevice == NULL");
	if (FAILED(hr = g_pDevice->SetPixelShaderConstantF(0, m_Colour.val, 1)))
	{
		OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, 
			"向shader传递参数时失败！", 
			"COTEBillBoardSet::getRenderOperation");
	}

	if(getMaterial()->getBestTechnique()->getPass(0)->getFogMode() == Ogre::FOG_NONE)
	{
		RENDERSYSTEM->_setFog(Ogre::FOG_NONE);
	}
	
	// 这里防止设备丢失后

	g_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
}

//-----------------------------------------------------------------------
// pX, pY 分别是 公告牌垂直两个边的方向
void COTEBillBoardSet::genBillboardAxes(Camera* cam, Vector3* pX, Vector3 *pY, const Billboard* bb)
{
    // Default behaviour is that billboards are in local node space
    // so orientation of camera (in world space) must be reverse-transformed 
    // into node space to generate the axes

	Quaternion transform;
    Quaternion invTransform;

    if (!mWorldSpace && mParentNode)
    {
        transform = mParentNode->_getDerivedOrientation();
		invTransform = transform.Inverse();
    }	

    Quaternion camQ;

    switch (m_BillBoardType)
    {
    case eCommon: // 普通公告牌儿
		{
			// Get camera world axes for X and Y (depth is irrelevant)
			camQ = cam->getDerivedOrientation();
			if (!mWorldSpace)
			{
				// Convert into billboard local space
				camQ = invTransform * camQ;
			}

			camQ = camQ * Quaternion(- m_Angle, Ogre::Vector3::UNIT_Z);

			*pX = camQ * Vector3::UNIT_X;
			*pY = camQ * Vector3::UNIT_Y;
			
		}
        break;
    case eRotY: // 绕Y轴旋转的
		{	
			// Get camera world axes for X and Y (depth is irrelevant)
			camQ = cam->getDerivedOrientation();
			if (!mWorldSpace)
			{
				// Convert into billboard local space
				camQ = invTransform * camQ;
			}	

			camQ = camQ * Quaternion(- m_Angle, Ogre::Vector3::UNIT_Y);

			*pX = camQ * Vector3::UNIT_X;
			*pY = Vector3::UNIT_Y;
		}        
        break;
    case eFaceY: // 朝向Y轴的
        {	
			transform = transform * Quaternion(- m_Angle, Ogre::Vector3::UNIT_Y);

			*pX = transform * Vector3::UNIT_X;
			*pY = transform * Vector3::UNIT_Z;	
		
		}  
        break;
	case eFree: // 自由方向
		{	
			transform = transform * m_Quaternion;
			*pX = transform * Vector3::UNIT_X;
			*pY = transform * Vector3::UNIT_Y;	
		}

    }
}

// ======================================
// COTEBillBoardSetFactory
// ======================================
Ogre::MovableObject* COTEBillBoardSetFactory::Create(const std::string& ResName, const std::string& Name, bool cloneMat)
{
	COTEBillBoardSet* bbs = (COTEBillBoardSet*)SCENE_MGR->CreateBillBoardSet(Name, 1);
			
	bbs->createBillboard(0.0f, 0.0f, 0.0f);
	bbs->setDefaultDimensions(1.0f, 1.0f);
	bbs->_notifyCurrentCamera(SCENE_CAM);

	// 创建绑定场景结点

	//AttachObjToSceneNode(bbs);	

	// Clone 材质

	if(cloneMat)
	{
		Ogre::MaterialPtr mptr = Ogre::MaterialManager::getSingleton().getByName(Name);
		if(mptr.isNull())
		{
			mptr = Ogre::MaterialManager::getSingleton().getByName("BillBoardMat");
			if(mptr.isNull())
			{
				OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, "公告牌材质： BillBoardMat没有找到!", "COTEMagicManager::Create");							
			}
			if(!mptr->isLoaded())
				mptr->touch();

			mptr->clone(Name);
			//OTE_TRACE("Clone材质 : " << Name)
		}
		bbs->setMaterialName(Name);	
	}
	else
	{
		bbs->setMaterialName("BillBoardMat");	
		
	}

	if(ResName.find(".BBSRY") != std::string::npos)
	{
		bbs->m_BillBoardType = COTEBillBoardSet::eRotY;			
	}
	else if(ResName.find(".BBSY") != std::string::npos)
	{
		bbs->m_BillBoardType = COTEBillBoardSet::eFaceY;
	}
	else if(ResName.find(".BBSFree") != std::string::npos)
	{
		bbs->m_BillBoardType = COTEBillBoardSet::eFree;	
	}
	else
	{
		bbs->m_BillBoardType = COTEBillBoardSet::eCommon;	
	}
	OTE_TRACE("创建公告牌特效 : " << Name)

	return bbs;
}

//-----------------------------------------------------------------------
void COTEBillBoardSetFactory::GetTypeList(std::list<std::string>& rTypeList)
{
	rTypeList.push_back("BBSCom");
	rTypeList.push_back("BBSRY");
	rTypeList.push_back("BBSY");
	rTypeList.push_back("BBSFree");
}

//-----------------------------------------------------------------------
void COTEBillBoardSetFactory::GetTypeDescList(std::list<std::string>& rTypeDescList)
{
	rTypeDescList.push_back("普通公告牌");
	rTypeDescList.push_back("绕Y公告牌");
	rTypeDescList.push_back("朝向Y公告牌");
	rTypeDescList.push_back("自由公告牌");
}

//-----------------------------------------------------------------------
void COTEBillBoardSetFactory::Delete(const std::string& Name)
{
	SCENE_MGR->DeleteBillBoardSet(Name);
}

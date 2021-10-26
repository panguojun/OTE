#include "OTETerrainTexturePic.h"
#include "OgreMaterialManager.h"
#include "OgreMaterial.h"
#include "OgreOverlayManager.h"
#include "OgreTechnique.h"
#include "OgreTextureManager.h"

using namespace Ogre;
using namespace OTE;

// -------------------------------------------------------
CTerrainTexturePic::CTerrainTexturePic() :
CATCHDIFFERENCE(48),
m_pGuiOverlay(NULL),
m_terrainT(NULL),
m_t(NULL),
m_pContainer(NULL),
m_pPanel(NULL),
m_pBorder(NULL),
m_pSubContainerBorder(NULL),
m_pSubPanel(NULL),
m_pSubBorder(NULL),
m_pSubContainerBorder2(NULL),
m_pSubPanel2(NULL),
m_pSubBorder2(NULL),
m_zOrder(600),
m_b_isShow(true), 
m_SelectUnitWidth(32), 
m_SelectUnitHeight(32),
m_DefaultSelectIndexX1(4),
m_DefaultSelectIndexY1(2),
m_DefaultSelectIndexX2(4),
m_DefaultSelectIndexY2(0),
m_borderPicFileName("ogrebordercentersmall.tga"),
m_selectBackPic("borderCenterclarity.tga"),
m_TextureFileName("ogrebordercentersmall.tga")
{
	
}

CTerrainTexturePic::~CTerrainTexturePic()
{	
	/*if(m_pGuiOverlay)
	{
		OverlayManager::getSingletonPtr()->destroy(m_pGuiOverlay);
		OverlayManager::getSingletonPtr()->destroyOverlayElement(m_pBorder);
		OverlayManager::getSingletonPtr()->destroyOverlayElement(m_pPanel);
		OverlayManager::getSingletonPtr()->destroyOverlayElement(m_pSubPanel);
		OverlayManager::getSingletonPtr()->destroyOverlayElement(m_pSubBorder);
		OverlayManager::getSingletonPtr()->destroyOverlayElement(m_pSubPanel2);
		OverlayManager::getSingletonPtr()->destroyOverlayElement(m_pSubBorder2);
		OverlayManager::getSingletonPtr()->destroyOverlayElement(m_pContainer);
		OverlayManager::getSingletonPtr()->destroyOverlayElement(m_pSubContainerBorder);
		OverlayManager::getSingletonPtr()->destroyOverlayElement(m_pSubContainerBorder2);
	}*/

	m_terrainT = NULL;
	m_t = NULL;
	m_pContainer = NULL;
	m_pPanel = NULL;
	m_pBorder = NULL;
	m_pSubContainerBorder = NULL;
	m_pSubPanel = NULL;
	m_pSubBorder = NULL;
	m_pSubContainerBorder2 = NULL;
	m_pSubPanel2 = NULL;
	m_pSubBorder2 = NULL;
}

// -------------------------------------------------------
void CTerrainTexturePic::Create(std::string strFileName, int posX, int posY, float width, float height)
{
	if(strFileName != "")
		m_TextureFileName = strFileName;

	m_posX = posX;
	m_posY = posY;
	m_picWidth = width;
	m_picHeight = height;

		
	m_pGuiOverlay = OverlayManager::getSingletonPtr()->create("GuiOverlay"); 
	m_pGuiOverlay->setZOrder(m_zOrder);
	
	LoadTerrainMaterialPic();

	//纹理边框材质
	m_pBorderMat = MaterialManager::getSingleton().getByName("materialBorder");
	if(m_pBorderMat.isNull())
	{
		m_pBorderMat = MaterialManager::getSingleton().create("materialBorder", 
						ResourceGroupManager::getSingleton().getWorldResourceGroupName(), true);
		m_pass = m_pBorderMat->getTechnique(0)->getPass(0);
		//m_pBorderMat->setSceneBlending(SBT_TRANSPARENT_ALPHA);
		m_t = m_pass->createTextureUnitState(m_borderPicFileName);//ogrebordercenter.png
		m_t->setColourOperationEx(LBX_MODULATE_X2, LBS_TEXTURE, LBS_TEXTURE);
		m_t->setTextureAddressingMode(Ogre::TextureUnitState::TextureAddressingMode::TAM_CLAMP);//
	}

	m_pPanel = (PanelOverlayElement *)OverlayManager::getSingletonPtr()->createOverlayElement("Panel", "GUITexture");
	m_pPanel->initialise();
	m_pPanel->setMetricsMode(GMM_PIXELS);
	m_pPanel->setPosition(m_posX, m_posY);
	m_pPanel->setMaterialName(m_pPanMat->getName());
	//m_pPanel->setTiling(1.0, 1.0);//0.625
	//m_pPanel->setHorizontalAlignment(GHA_LEFT);
	//m_pPanel->setTransparent(true);
	//m_pPanel->setUV(0.625, 0.0, 1.0, 1.0);
	m_pPanel->setWidth(m_picWidth);
	m_pPanel->setHeight(m_picHeight);	

	//边框
	m_pBorder = (BorderPanelOverlayElement *)OverlayManager::getSingletonPtr()->createOverlayElement("BorderPanel", "GUIBorder");
	m_pBorder->setMetricsMode(GMM_PIXELS);
	m_pBorder->setBorderMaterialName(m_pBorderMat->getName());
	m_pBorder->setBorderSize(1, 1, 1, 1);
	m_pBorder->setBottomBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pBorder->setBottomLeftBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pBorder->setBottomRightBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pBorder->setTopBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pBorder->setTopLeftBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pBorder->setTopRightBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pBorder->setRightBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pBorder->setLeftBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pBorder->setWidth(m_picWidth);
	m_pBorder->setHeight(m_picHeight);

	
	m_pContainer = (Ogre::OverlayContainer*) OverlayManager::getSingletonPtr()->createOverlayElement("Panel", "GUIContainer");
	
	m_pGuiOverlay->add2D(m_pContainer);
	m_pContainer->addChild(m_pPanel);
	m_pPanel->addChild(m_pBorder);


	//选择框背景
	m_pSubbackMat = MaterialManager::getSingleton().getByName("materialSubBorder");
	if(m_pSubbackMat.isNull())
	{
		m_pSubbackMat = MaterialManager::getSingleton().create("materialSubBorder", 
						ResourceGroupManager::getSingleton().getWorldResourceGroupName(), true);
		m_pSubbackMat->setSceneBlending(SBT_TRANSPARENT_ALPHA);
		m_pass = m_pSubbackMat->getTechnique(0)->getPass(0);
		m_t = m_pass->createTextureUnitState(m_selectBackPic);
		m_t->setTextureAddressingMode(Ogre::TextureUnitState::TextureAddressingMode::TAM_CLAMP);
	}
	
	//选择框1
	m_pSubPanel = (PanelOverlayElement *)OverlayManager::getSingletonPtr()->createOverlayElement("Panel", "GUISelectSub");
	m_pSubPanel->setMetricsMode(GMM_PIXELS);
	m_pSubPanel->setPosition(m_pPanel->getLeft() + m_SelectUnitWidth * m_DefaultSelectIndexX1, m_pPanel->getTop() + m_SelectUnitHeight * m_DefaultSelectIndexY1);
	//m_pSubPanel->setPosition(160, 654);
	m_pSubPanel->setMaterialName(m_pSubbackMat->getName());
	m_pSubPanel->setWidth(m_SelectUnitWidth);
	m_pSubPanel->setHeight(m_SelectUnitHeight);
	
	m_pSubBorder = (BorderPanelOverlayElement *)OverlayManager::getSingletonPtr()->createOverlayElement("BorderPanel", "GUISubBorder");
	m_pSubBorder->setMetricsMode(GMM_PIXELS);
	m_pSubBorder->setBorderMaterialName(m_pBorderMat->getName());
	m_pSubBorder->setBorderSize(1, 1, 1, 1);
	m_pSubBorder->setBottomBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pSubBorder->setBottomLeftBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pSubBorder->setBottomRightBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pSubBorder->setTopBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pSubBorder->setTopLeftBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pSubBorder->setTopRightBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pSubBorder->setRightBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pSubBorder->setLeftBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pSubBorder->setWidth(m_SelectUnitWidth);
	m_pSubBorder->setHeight(m_SelectUnitHeight);


	m_pSubContainerBorder = (Ogre::OverlayContainer*) OverlayManager::getSingletonPtr()->createOverlayElement("Panel", "GUIContainerSelectBorder");
	
	m_pGuiOverlay->add2D(m_pSubContainerBorder);
	m_pSubContainerBorder->addChild(m_pSubPanel);//
	m_pSubPanel->addChild(m_pSubBorder);
	
	//选择框1end

	//选择框2
	m_pSubPanel2 = (PanelOverlayElement *)OverlayManager::getSingletonPtr()->createOverlayElement("Panel", "GUISelectSub2");
	m_pSubPanel2->setMetricsMode(GMM_PIXELS);
	m_pSubPanel2->setPosition(m_pPanel->getLeft() + m_SelectUnitWidth * m_DefaultSelectIndexX2, m_pPanel->getTop() + m_SelectUnitHeight * m_DefaultSelectIndexY2);
	//m_pSubPanel->setPosition(160, 654);
	m_pSubPanel2->setMaterialName(m_pSubbackMat->getName());
	m_pSubPanel2->setWidth(m_SelectUnitWidth);
	m_pSubPanel2->setHeight(m_SelectUnitHeight);
	
	m_pSubBorder2 = (BorderPanelOverlayElement *)OverlayManager::getSingletonPtr()->createOverlayElement("BorderPanel", "GUISubBorder2");
	m_pSubBorder2->setMetricsMode(GMM_PIXELS);
	m_pSubBorder2->setBorderMaterialName(m_pBorderMat->getName());
	m_pSubBorder2->setBorderSize(1, 1, 1, 1);
	m_pSubBorder2->setBottomBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pSubBorder2->setBottomLeftBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pSubBorder2->setBottomRightBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pSubBorder2->setTopBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pSubBorder2->setTopLeftBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pSubBorder2->setTopRightBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pSubBorder2->setRightBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pSubBorder2->setLeftBorderUV(0.0, 1.0, 0.0, 1.0);
	m_pSubBorder2->setWidth(m_SelectUnitWidth);
	m_pSubBorder2->setHeight(m_SelectUnitHeight);


	m_pSubContainerBorder2 = (Ogre::OverlayContainer*) OverlayManager::getSingletonPtr()->createOverlayElement("Panel", "GUIContainerSelectBorder2");
	
	m_pGuiOverlay->add2D(m_pSubContainerBorder2);
	m_pSubContainerBorder2->addChild(m_pSubPanel2);//
	m_pSubPanel2->addChild(m_pSubBorder2);
	//选择框2end

	m_pGuiOverlay->show();

}

// -------------------------------------------------------
void CTerrainTexturePic::LoadTerrainMaterialPic()
{
	
	m_pPanMat = MaterialManager::getSingleton().getByName("material");
	
	
	if(m_pPanMat.isNull())
	{
		//纹理图片材质
		m_pPanMat = MaterialManager::getSingleton().create("material", 
						ResourceGroupManager::getSingleton().getWorldResourceGroupName(), true);
		
	}

	m_pass = m_pPanMat->getTechnique(0)->getPass(0);

	//Ogre::ResourcePtr texUnitStateThirdResource = TextureManager::getSingleton().getByName(m_TextureFileName);
	if(!m_terrainT)
	{
		m_terrainT = m_pass->createTextureUnitState(m_TextureFileName);		
	}
	else
	{
		//m_terrainT = m_pass->createTextureUnitState();//如果每次这样创建一个空的再设置图片名，次数过多就会崩掉。所以为经常要变的TextureUnitState单独建立一个，并且只改它就行了。
		m_terrainT->setTextureName(m_TextureFileName);		
		m_terrainT->setTextureCoordSet(0);
	}
	m_terrainT->setColourOperationEx(LBX_MODULATE_X2, LBS_TEXTURE, LBS_TEXTURE);

	//m_pPanMat->setSceneBlending(SBT_TRANSPARENT_ALPHA);

}

// -------------------------------------------------------
void CTerrainTexturePic::GetSelectAreaIndex(int x, int y, int& resultX, int& resultY)
{
	resultX = int((x - m_pPanel->getLeft()) / m_SelectUnitWidth);
	resultY = int((y + CATCHDIFFERENCE - m_pPanel->getTop()) / m_SelectUnitHeight);
}

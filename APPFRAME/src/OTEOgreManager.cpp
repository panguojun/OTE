// ======================================================================
//COTEOgreManager 
//By Romeo April 21,05
// ======================================================================
#include "stdafx.h"
#include <windows.h>

#include "OgrePrerequisites.h"
#include "Ogre.h"

#include <OgreHardwareBufferManager.h>
#include "OgreHardwarePixelBuffer.h"

#include "OTEFrameListener.h"
#include "OTEOgreManager.h"

#include "OTEQTSceneManager.h"
#include "PackArchiveInterface.h"

// -----------------------------------------------------------------------

//singlton
////////////////////////////////////////////////////////////////////////////
COTEOgreManager* COTEOgreManager::m_pInstance=0;
COTEOgreManager* COTEOgreManager::Instance()
{
	if(m_pInstance==0)
        m_pInstance=new COTEOgreManager; 
	return m_pInstance;
}
//con/dis
/////////////////////////////////////////////////////////////////////////////
COTEOgreManager::COTEOgreManager()
{
	m_pCamera = 0;
    m_pFrameListener = 0;
    m_pRoot = 0;
	m_pSceneMgr = 0 ;
	m_pWindow = 0;
}

COTEOgreManager::~COTEOgreManager()
{
}

void COTEOgreManager::DestroyOgre()
{
	if (m_pFrameListener)
	{
        delete m_pFrameListener;
		m_pFrameListener=0;
	}
    if (m_pRoot)
	{ 
        delete m_pRoot;
		m_pRoot=0;
	}
}
// Start render
////////////////////////////////////////////////////////////////////////////////
void COTEOgreManager::StartRender()
{ 
	if(m_pRoot)
		m_pRoot->startRendering();	
    DestroyOgre();	

	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}
// Initialize OGRE
////////////////////////////////////////////////////////////////////////////////
bool COTEOgreManager::Init(HWND handle)
{
	if(m_pRoot)
		return false;

	m_pRoot = new Ogre::Root();
    setupResources();
	// tries to restore the config, on failure it shows the "Configure Dialog"
    if(m_pRoot->restoreConfig() != true)
        m_pRoot->showConfigDialog();
	
	// Render target
	if(handle)
	{
		m_pWindow = m_pRoot->initialise(false);
		Ogre::NameValuePairList misc;
		misc["externalWindowHandle"] = Ogre::StringConverter::toString((Ogre::uint)handle);		

		m_pWindow = m_pRoot->createRenderWindow("OTE RenderWindow", 1024, 768, false, &misc);
		//m_pWindow->setAutoUpdated(false);	
		
	}
	else
	{
		m_pWindow = m_pRoot->initialise(true);
	}

  	//Create

    chooseSceneManager();
	createFrameListener();

    createCamera();

    createViewports();
    loadResources();
	
    createScene();

	// All set up, activate.

	m_pWindow->setActive(true);

    return true;
}
// Exit
///////////////////////////////////////////////////////////////////////////////
void COTEOgreManager::OgrExit()
{
	m_pRoot->queueEndRendering(); 

	m_pFrameListener->m_IsClosed = true;
}

void COTEOgreManager::chooseSceneManager(void)
{
    // Get the SceneManager, in this case a generic one
	m_pSceneMgr = m_pRoot->getSceneManager(Ogre::ST_GENERIC );
}

void COTEOgreManager::createCamera(void)
{
    // Create the camera
    m_pCamera = m_pSceneMgr->createCamera("CamMain");

	m_pCamera->setPosition(0, 6.3, 13.1);
	m_pCamera->lookAt(Ogre::Vector3(0, 0, 0));

    m_pCamera->setNearClipDistance( 0.1f );
    m_pCamera->setFarClipDistance( 600.0f );	
	
}

void COTEOgreManager::createFrameListener(void)
{
    m_pFrameListener= new COTEFrameListener(m_pWindow, m_pCamera);	
    m_pFrameListener->showDebugOverlay(true);
    m_pRoot->addFrameListener(m_pFrameListener);

}

void COTEOgreManager::createViewports(void)
{    
	// 渲染场景摄像机
	
	Ogre::Viewport* vp = m_pWindow->addViewport(m_pCamera);
	vp->setBackgroundColour(Ogre::ColourValue(0.5, 0.5, 0.5));   

    m_pCamera->setAspectRatio(
        float(vp->getActualWidth()) / float(vp->getActualHeight()));
}

// Method which will define the source
// of resources (other than current folder)
////////////////////////////////////////////////////////////////////////////////
void COTEOgreManager::setupResources(void)
{
	// 打包文件
	
	Ogre::ArchiveManager::getSingleton().addArchiveFactory(new OTE::PakFileArchiveFactory() );

    //// Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load("resources.cfg");

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
           Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }

}
//create scene
///////////////////////////////////////////////////////////////////////////////////
//#include "MagicEntity.h"
void COTEOgreManager::createScene(void)
{	
	// 视窗颜色
    Ogre::ColourValue fadeColour(0.5, 0.5, 0.5);
	m_pWindow->getViewport(0)->setBackgroundColour(fadeColour);		
	
	// 摄像机　Infinite far plane?
	/*if (m_pRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_INFINITE_FAR_PLANE))
    {
        m_pCamera->setFarClipDistance(0);	
	} 	*/ 

	// 初始化
	((OTE::COTEQTSceneManager*)m_pSceneMgr)->InitWorld();	

	// tttttttttttttttttttttt
	
	//OTE::COTEMagicEntity::CreateMagicEntity("asdf", "生命之语mz.effect");
	
}

// Optional override method where you can perform resource group loading
// Must at least do ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
///////////////////////////////////////////////////////////////////////////////////
void COTEOgreManager::loadResources(void)
{
	// Initialise, parse scripts etc
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	//Ogre::ResourceBackgroundQueue::getSingleton().initialiseAllResourceGroups();
}


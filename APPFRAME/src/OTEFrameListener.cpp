// ===================================================================
// COTEFrameListener
// By Romeo April 21,05
// ===================================================================
#include "stdafx.h"
#include "OTEInterfaceInclude.h"
#include "OgrePrerequisites.h"
#include "Ogre.h"
#include <OgreHardwareBufferManager.h>
#include "OgreHardwarePixelBuffer.h"
//#include "OgreEventListeners.h"
#include "OgreStringConverter.h"
#include "OgreException.h"
#include "OgreSceneManager.h"
#include "ogreframelistener.h"
#include "oteframelistener.h"
#include "OTEOgreManager.h"

//
//Constructor/Dis
/////////////////////////////////////////////////////////////////////
COTEFrameListener::COTEFrameListener(Ogre::RenderWindow* win, Ogre::Camera* cam)
{
    m_pWindow = win; 
	m_IsClosed = false;
}

COTEFrameListener::~COTEFrameListener()
{
	
}

//Stats
//////////////////////////////////////////////////////////////////////
void COTEFrameListener::updateStats()
{
    //static String currFps = "Current FPS: ";
    //static String avgFps = "Average FPS: ";
    //static String bestFps = "Best FPS: ";
    //static String worstFps = "Worst FPS: ";
    //static String tris = "Triangle Count: ";

    //// update stats when necessary
    //try {
    //    OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
    //    OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
    //    OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
    //    OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

      //  const Ogre::RenderTarget::FrameStats& stats = m_pWindow->getStatistics();

    //    guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
    //    guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
    //    guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS)
    //        +" "+StringConverter::toString(stats.bestFrameTime)+" ms");
    //    guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
    //        +" "+StringConverter::toString(stats.worstFrameTime)+" ms");

    //    OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
    //    guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));

    //    OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
    //    guiDbg->setCaption(m_pWindow->getDebugText());
    //}
    //catch(...)
    //{
    //    // ignore
    //}
}

//Show/hide debug overlay
//////////////////////////////////////////////////////////////////////////
void COTEFrameListener::showDebugOverlay(bool show)
{
    //if (m_DebugOverlay)
    //{
    //    if (show)
    //    {
    //        m_DebugOverlay->show();
    //    }
    //    else
    //    {
    //        m_DebugOverlay->hide();
    //    }
    //}
}

// Override frameStarted event to process that (don't care about frameEnded)
//////////////////////////////////////////////////////////////////////////

#   ifndef GAME_DEBUG 

	#include "OTEOgreManager.h"

	extern HMODULE ghModule;
	typedef bool (*pFunGameCoreStart) (Ogre::RenderWindow* mainWin);
	typedef void (*pFunGameCoreEnd) ();

#   endif 

bool COTEFrameListener::frameStarted(const Ogre::FrameEvent& evt)
{	
	
	//char buff[32];

	//sprintf(buff, "捉鬼 FPS : %.2f, Tris: %d", 1.0f / evt.timeSinceLastFrame, m_pWindow->getTriangleCount());

	//::SetWindowText(::GetActiveWindow(), buff);
	
	if(m_pWindow->isClosed() || m_IsClosed)
	{

#   ifdef GAME_DEBUG

		 return false;

#   else

		// 退出客户端
		if(ghModule)
		{
			pFunGameCoreEnd pFunCoreEnd = (pFunGameCoreEnd)::GetProcAddress( ghModule, "StopDll" );
			(*pFunCoreEnd)();			
		}

		// Ogre退出
		COTEOgreManager::Instance()->OgrExit();	//exit first 

		if(ghModule)
			::FreeLibrary(ghModule);

		return false;

#   endif 
       	
	}
	

	return true;
}

// Frame end
//////////////////////////////////////////////////////////////////////////
bool COTEFrameListener::frameEnded(const Ogre::FrameEvent& evt)
{

    return true;
}
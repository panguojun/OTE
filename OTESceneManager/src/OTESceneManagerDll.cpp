#include "OTEPluginManager.h"
#include "OTEQTScenemanager.h"
#include <OgreRoot.h>

namespace OTE
{ 
	COTEQTSceneManager*	gpGeneraSMgr = NULL;

	// 四叉树管理场景
	extern "C" void _OTEExport dllStartPlugin( void )
	{
		// Create new scene managers
		gpGeneraSMgr = new COTEQTSceneManager();
		
		// Register  managers
		Ogre::Root::getSingleton().setSceneManager( Ogre::ST_GENERIC, gpGeneraSMgr); 

	}

	extern "C" void _OTEExport dllStopPlugin( void )
	{
		// Delete scene managers
		if(gpGeneraSMgr)
		{
			delete gpGeneraSMgr;
			gpGeneraSMgr = NULL;
		}

	}

}

// -------------------------------------------------
// ETAct调用接口
// -------------------------------------------------

extern "C" int __declspec(dllexport) Algorithm_Start(float x, float y) 
{
	return 0;
}

// -------------------------------------------------
extern "C" int __declspec(dllexport) Algorithm_Selection(float x, float y) 
{
	return 1;
}

// -------------------------------------------------
extern "C" int __declspec(dllexport) Algorithm_WorkFun(float x, float y, int depth) 
{	
	return sin(x + y) * 1000;
}

// ======================================================================
// COTEOgreManager 
// By Romeo April 21,05
// ======================================================================
#pragma once

/////////////////////////////////////////////////////////////////////////
class COTEFrameListener;
class COTEOgreManager
{
public:	
	//singleton interfaces
	static COTEOgreManager*				 m_pInstance;	
    Ogre::Camera						*m_pCamera;
	Ogre::RenderWindow					*m_pWindow;	

protected:

	Ogre::Root                       *m_pRoot;

    Ogre::SceneManager		         *m_pSceneMgr;	

    COTEFrameListener                *m_pFrameListener;


public:
	static COTEOgreManager* Instance();
	COTEOgreManager(void);
	virtual ~COTEOgreManager(void);
	Ogre::SceneManager* GetSceneManager()
	{
		return m_pSceneMgr;
	}
	COTEFrameListener* GetFrameListener()
	{
		return m_pFrameListener;
	}
    void DestroyOgre();
	void OgrExit(); 
    void StartRender();    
    bool Init(HWND handle);

protected:	
    void chooseSceneManager(void);
    void createCamera(void);
    void createFrameListener(void);
	void createScene(void) ; 
    void destroyScene(void){}
    void createViewports(void);

    void setupResources(void);

	virtual void loadResources(void);

};

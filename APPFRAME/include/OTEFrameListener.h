// ======================================================================
// COTEFrameListener 
// By Romeo April 21,05
// ======================================================================
#pragma once

#include <windows.h>

class Ogre::FrameListener;
struct Ogre::FrameEvent;
class Ogre::RenderWindow;
struct Ogre::RenderTarget::FrameStats;
class Ogre::Camera;

class COTEFrameListener : public Ogre::FrameListener
{
public:
    // Constructor takes a RenderWindow because it uses that to determine input context
	COTEFrameListener(Ogre::RenderWindow* win, Ogre::Camera* cam);
    virtual ~COTEFrameListener(); 

    void showDebugOverlay(bool show);

    // Override frameStarted event to process that (don't care about frameEnded)
	bool frameStarted(const Ogre::FrameEvent& evt);
    bool frameEnded(const Ogre::FrameEvent& evt);  

public:	
	Ogre::RenderWindow*				m_pWindow;
	bool							m_IsClosed;
	
protected:
    void updateStats(void);
};

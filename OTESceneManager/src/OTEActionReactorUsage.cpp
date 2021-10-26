#include "oteactionreactorusage.h"
#include "OTEEntityRender.h"
#include "OTEEntityReactor.h"
#include "OTETransBangReactor.h"

using namespace Ogre;
using namespace OTE;

// --------------------------------------
void COTEActionReactorUsage::Bind(Ogre::MovableObject* mo, CReactorBase* er)
{
	// bind -> trigger
	er->Init(mo);
	
}

// --------------------------------------
void COTEActionReactorUsage::Proccess(Ogre::MovableObject* mo, CReactorBase* er, float dTime)
{
	er->Update(mo, dTime);
}

// --------------------------------------
void COTEActionReactorUsage::Unbind(Ogre::MovableObject* mo, CReactorBase* er)
{	
	er->Clear();
}
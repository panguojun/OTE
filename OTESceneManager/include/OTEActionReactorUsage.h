#pragma once
#include "OTEStaticInclude.h"
#include "OTEReactorBase.h"
#include "OTEAction.h"
#include "OTEActorEntity.h"

namespace OTE
{
	// ***********************************
	// class COTEActionReactorUsage
	// 反应器的行动绑定实现
	// ***********************************
	class COTEActionReactorUsage
	{
	public:

		static void Bind(Ogre::MovableObject* mo, CReactorBase* er);

		static void Unbind(Ogre::MovableObject* mo, CReactorBase* er);

		static void Proccess(Ogre::MovableObject* mo, CReactorBase* er, float dTime);

	};

}
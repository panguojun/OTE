#pragma once
#include "otereactorbase.h"
#include "OTEBillBoardSet.h"
#include "OTETieReactors.h"

namespace OTE
{
// ***********************************************
// COTEFootPrintReactor 
// ***********************************************
class _OTEExport COTEFootPrintReactor : public COTETieReactorBase
{
public:

	COTEFootPrintReactor();

	// 初始化
	virtual void Init(Ogre::MovableObject* mo);
	// 更新
	virtual void Update(Ogre::MovableObject* mo, float dTime);

protected:

	COTEBillBoardSet*		m_BBS; 

	Ogre::Vector3			m_LastFramePosition;

};


// ***********************************************
// COTEFootPrintReactorFactory 
// ***********************************************
class COTEFootPrintReactorFactory : public CReactorFactoryBase
{
public:
	virtual bool ValidBind(const std::string& rObjType)
	{ 		
		return rObjType == "OTEEntity";
	}

	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
	{
		if(ValidBind(mo->getMovableType()))
			return new COTEFootPrintReactor();
		return NULL;
	}
	
	virtual void Delete(CReactorBase* rb)
	{
		SAFE_DELETE(rb)
	}

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "山体上的脚印";
	}
};

}

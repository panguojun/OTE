#pragma once
#include "OTEReactorBase.h"
#include "dymproperty.h"
#include "OTEShader.h"

namespace OTE
{
// ***********************************
// class COTEEntityReactor
// 用于作用在角色身上的反应器
// ***********************************

class _OTEExport COTEEntityReactor : public CReactorBase, public COTEShaderCtrller
{
public:

	COTEEntityReactor() : 
	m_HLSLName("chem_eff_1.hlsl"),
	m_BindEntity(NULL)
	{}

	~COTEEntityReactor();

	// 初始化
	virtual void Init(Ogre::MovableObject* mo);

	// 更新参数　向ｓｈａｄｅｒ传递参数
	virtual void UpdateParams();

protected:

	std::string			m_HLSLName;	// HLSL名称

	Ogre::MovableObject*	m_BindEntity;
};



// ***********************************************
// COTEEntityReactorFactory 
// ***********************************************
class COTEEntityReactorFactory : public CReactorFactoryBase
{
public:

	virtual bool ValidBind(const std::string& rObjType)
	{ 		
		return rObjType == "OTEEntity";
	}

	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
	{
		if(ValidBind(mo->getMovableType() ))
			return new COTEEntityReactor();	
	
		return NULL;
	}
	
	virtual void Delete(CReactorBase* rb)
	{
		SAFE_DELETE(rb)
	}

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "实体扭曲";
	}

};

}

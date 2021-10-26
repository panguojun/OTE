#pragma once
#include "OTEReactorBase.h"
#include "dymproperty.h"
#include "OTEShader.h"

namespace OTE
{
// ***********************************
// class COTEEntityReactor
// ���������ڽ�ɫ���ϵķ�Ӧ��
// ***********************************

class _OTEExport COTEEntityReactor : public CReactorBase, public COTEShaderCtrller
{
public:

	COTEEntityReactor() : 
	m_HLSLName("chem_eff_1.hlsl"),
	m_BindEntity(NULL)
	{}

	~COTEEntityReactor();

	// ��ʼ��
	virtual void Init(Ogre::MovableObject* mo);

	// ���²������������򴫵ݲ���
	virtual void UpdateParams();

protected:

	std::string			m_HLSLName;	// HLSL����

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
		rDesc = "ʵ��Ť��";
	}

};

}

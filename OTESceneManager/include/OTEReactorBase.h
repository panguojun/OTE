#pragma once
#include "OTETrack.h"
#include "OTECommon.h"

namespace OTE
{
// ***********************************
// class CReactorBase
// ������ӦЧ����
// ������ڹ���������Եģ���Ҫ��ʹ��
// �ֳɿ⺯��������shader����
// ��ĳ������ʵ�̶ֹ�Ч��
// ������"����(inter)" ��һ��
// ***********************************

class _OTEExport CReactorBase : public CInterBase
{
public:

	// ����
	virtual void Init(Ogre::MovableObject* mo){}

	// ����
	virtual void Update(Ogre::MovableObject* mo, float dTime){}

	// ����
	virtual void Clear(){}

public:

	CReactorBase() : 
	CInterBase(),
	m_StateFlag(0)
	{		
		m_IsBind = false; // ��Ӧ����Ҫ��̬��
	}

	virtual ~CReactorBase(){}

	virtual const std::string&	GetInterType()
	{
		static std::string sType = "Reactor";
		return sType;
	}

public:

	std::string		m_FactoryName;	// �������Ĺ�����
	std::string		m_ResName;		// ����������Դ��
	unsigned int    m_StateFlag;	// һЩ״̬
	
};

// **********************************************
// CReactorFactoryBase
// **********************************************
class _OTEExport CReactorFactoryBase
{
public:

	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName) = 0;
	
	virtual void Delete(CReactorBase* rb) = 0;

	virtual bool ValidBind(const std::string& rMovableType){ return true; }

	virtual void GetDescription(std::string& rDesc){}	

	virtual void GetTypeList(std::list<std::string>& rTypeList){}

	virtual void GetTypeDescList(std::list<std::string>& rTypeList){}
};


}

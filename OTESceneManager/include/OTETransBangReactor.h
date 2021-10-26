#pragma once
#include "OTETransBangReactor.h"
#include "OTEReactorBase.h"
#include "OTEActorEntity.h" 

namespace OTE
{
// ***********************************
// class COTETransBangReactor
// �𶯷�Ӧ��
// ***********************************

class _OTEExport COTETransBangReactor : public CReactorBase
{
public:
	// ��ʼ��
	virtual void Init(Ogre::MovableObject* mo);
	// ����
	virtual void Update(Ogre::MovableObject* mo, float dTime);

protected:

	float				m_InterTime;
	
};


// ***********************************************
// COTETransBangReactorFactory 
// ***********************************************
class COTETransBangReactorFactory : public CReactorFactoryBase
{
public:

	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
	{		
		return new COTETransBangReactor();
	}
	
	virtual void Delete(CReactorBase* rb)
	{
		SAFE_DELETE(rb)
	}

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "��1";
	}
};

// ***********************************
// class COTEFreeDropReactor
// �𶯷�Ӧ��
// ***********************************

class _OTEExport COTEFreeDropReactor : public CReactorBase
{
public:	
	COTEFreeDropReactor() : m_InterTime(0.0f){}
	// ��ʼ��
	virtual void Init(Ogre::MovableObject* mo);
	// ����
	virtual void Update(Ogre::MovableObject* mo, float dTime);

	void SetInterTime(float interTime){m_InterTime = interTime;}

	void SetGravity(const Ogre::Vector3& g){m_Gravity = g;}

	void SetPlane(float d, const Ogre::Vector3& n){m_Plane = Ogre::Plane(n, d);}

protected:

	// ״̬ 
	
	Ogre::Vector3		m_Vec;			// �˶��ٶ�

	float				m_TimeEsp;		// ����ʱ��

	// ����

	float				m_InterTime;	// ����ʱ��
	
	Ogre::Vector3		m_Gravity;		// �������ٶ�

	Ogre::Plane         m_Plane;		// ��ײƽ�� 
	
};


// ***********************************************
// COTEFreeDropReactorFactory 
// ***********************************************
class COTEFreeDropReactorFactory : public CReactorFactoryBase
{
public:

	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
	{		
		return new COTETransBangReactor();
	}
	
	virtual void Delete(CReactorBase* rb)
	{
		SAFE_DELETE(rb)
	}

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "��������";
	}
};

}

#pragma once

#include "OTEStaticInclude.h"
#include "OTEReactorBase.h"

namespace OTE
{		
	// ***********************************
	// COTEBallPhysicsCtrller
	// ��������ѧ
	// ***********************************
	class _OTEExport COTEBallPhysicsCtrller : public CReactorBase
	{
	public:

		// ��ʼ��
		virtual void Init(Ogre::MovableObject* mo);

		// ����
		virtual void Update(Ogre::MovableObject* mo, float dTime);	

	public:

		// ����

		void Hit(const Ogre::Vector3& normal, float strength = 0);

		// �����ٶ�

		void SetVec(const Ogre::Vector3& vec);
		
		const Ogre::Vector3& GetVec(){return m_Vec;}

		// Ħ��

		void SetFra(float frac){m_Frac = frac;}
		
		float GetFra(){return m_Frac;}

		// ����ٶ�

		void SetMaxVec(float maxVec){m_MaxVec = maxVec;}
		
		float GetMaxVec(){return m_MaxVec;}

		// ˮƽ��

		void SetGroundHgt(float groundHgt){m_GroundHgt = groundHgt;}

		float GetGroundHgt(){return m_GroundHgt;}

	protected:

		// �ٶ�

		Ogre::Vector3		m_Vec;

		// Ħ��

		float				m_Frac;

		// ����ٶ�����

		float				m_MaxVec;

		// ˮƽ�߸߶�

		float				m_GroundHgt;

	};

	
// ***********************************************
// COTEBallPhysicsCtrllerFactory 
// ***********************************************
class COTEBallPhysicsCtrllerFactory : public CReactorFactoryBase
{
public:

	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
	{
		if(!mo || mo->getMovableType() == "OTEEntity")
			return new COTEBallPhysicsCtrller();
		return NULL;
	}
	
	virtual void Delete(CReactorBase* rb)
	{
		SAFE_DELETE(rb)
	}

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "�������1";
	}
};


}
#pragma once
#include "otereactorbase.h"
#include "OTEActorEntity.h"

namespace OTE
{
// ***********************************************
// COTEActorMoveAIReactor 
// ***********************************************
class _OTEExport COTEActorStreightMoveAIReactor : public CReactorBase
{
public:
	// ����
	virtual void Update(Ogre::MovableObject* mo, float dTime);	

public:

	// ��ʼ��
	void Init(Ogre::MovableObject* mo, float maxMoveVec, float maxMoveAcc, float maxFrasc);

	// �����ƶ���Χ
	void SetMoveRange(const Ogre::Vector3& pt1, const Ogre::Vector3& pt2)
	{
		m_Pt1 = pt1;
		m_Pt2 = pt2;
	}

	// �ƶ��ٶ�
	Ogre::Vector3 GetMoveVec()
	{
		if(m_MoveVec.length() > m_MaxMoveVec)
			m_MoveVec = m_MoveVec.normalisedCopy() * m_MaxMoveVec;
		return m_MoveVec;
	}

	// Ħ����
	void SetMaxFrasc(float maxFrasc)
	{
		m_MaxFrasc = maxFrasc;
	}

	// ����
	void MoveLeft(Ogre::MovableObject* mo);

    // ����
	void MoveRight(Ogre::MovableObject* mo);

    // ֹͣ
	void Stop(float maxFrasc = -1.0f);

public:

	///	����

	// ��һ��
	Ogre::Vector3		m_Pt1;		

	// �ڶ���
	Ogre::Vector3		m_Pt2;

	/// ״̬

	// ���ٶ�
	Ogre::Vector3       m_MoveAcc;	

	// �ٶ�
	Ogre::Vector3       m_MoveVec;	

	// Ħ������
	float				m_Frasc;

protected:
	
	/// ���ֵ

	float				m_MaxMoveVec;

	float				m_MaxMoveAcc;

	float				m_MaxFrasc;

};


// ***********************************************
// COTEActorStreightMoveAIReactorFactory 
// ***********************************************
class COTEActorStreightMoveAIReactorFactory : public CReactorFactoryBase
{
public:

	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
	{
		if(!mo || mo->getMovableType() == "OTEEntity")
			return new COTEActorStreightMoveAIReactor();
		return NULL;
	}
	
	virtual void Delete(CReactorBase* rb)
	{
		SAFE_DELETE(rb)
	}

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "��ɫ�ƶ�����3";
	}
};

// ***********************************************
// COTENpcActorMoveAIReactor 
// ***********************************************
class _OTEExport COTENpcActorStreightMoveAIReactor : public COTEActorStreightMoveAIReactor
{
public:	

	// ����

	virtual void Update(Ogre::MovableObject* mo, float dTime);	

	/// Ŀ��λ��(�˹�����)

	void SetTargetPos(const Ogre::Vector3& targetPos){m_TargetPos = targetPos;}

	const Ogre::Vector3& GetTargetPos(){return m_TargetPos;}

protected:

	// �˹�����

	void NpcThink(Ogre::MovableObject* );

protected:
	
	// Ŀ��λ��
	Ogre::Vector3	m_TargetPos;

};


// ***********************************************
// COTENpcActorStreightMoveAIReactorFactory 
// ***********************************************
class COTENpcActorStreightMoveAIReactorFactory : public CReactorFactoryBase
{
public:

	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
	{
		if(!mo || mo->getMovableType() == "OTEEntity")
			return new COTENpcActorStreightMoveAIReactor();
		return NULL;
	}
	
	virtual void Delete(CReactorBase* rb)
	{
		SAFE_DELETE(rb)
	}

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "��ɫ�ƶ����ƣ�";
	}
};

}

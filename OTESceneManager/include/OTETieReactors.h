#pragma once
#include "otereactorbase.h"
#include "OTEBillBoardSet.h"

namespace OTE
{
struct TieDummyElm_t
{
	TieDummyElm_t(const std::string& elmName, float createTime)
	{ 
		ElmName = elmName;
		CreateTime = createTime;
		
	}
	TieDummyElm_t(int id, float createTime)
	{
		ID = id;
		CreateTime = createTime;
	}

	int				ID;
	std::string		ElmName;
	float			CreateTime;
	
};

// ***********************************************************
// class COTETieReactorBase
// ***********************************************************
class COTETieReactorBase : public CReactorBase
{
protected:
	
	// ��λ��

	Ogre::Vector3		m_LastCreatePos;
	
	// ʱ��

	float				m_EspTime;
	
	// ��ӰЧ��Ԫ��	

	std::list<TieDummyElm_t>		m_DummyList;

	std::list<TieDummyElm_t>		m_DummyGraveList;

	Ogre::MovableObject*			m_LastObj;

};


// ***********************************************************
// class COTEParticleTieReactor
// ��������������ϵͳ�ϵķ�Ӧ����
// �ܲ�����ӰЧ��������������ϵͳĬ��Ч����
// ***********************************************************
class COTEParticleTieReactor : public COTETieReactorBase
{
public:
	// ����
	virtual void Init(Ogre::MovableObject* mo);
	//// ����
	virtual void Update(Ogre::MovableObject* mo, float dTime);	

	void ClearContent();

protected:

	std::string			m_MaterialName;


};

// ***********************************************
// COTEParticleTieReactorFactory 
// ***********************************************
class COTEParticleTieReactorFactory : public CReactorFactoryBase
{
public:
	
	virtual bool ValidBind(const std::string& rObjType)
	{ 		
		return rObjType == "PS";
	}

	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
	{
		if(ValidBind(mo->getMovableType()))
			return new COTEParticleTieReactor();
		return NULL;
	}
	
	virtual void Delete(CReactorBase* rb)
	{
		if(rb)
			((COTEParticleTieReactor*)rb)->ClearContent();
		SAFE_DELETE(rb)
	}
	
	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "���Ӳ�Ӱ";
	}

};

// ***********************************************************
// class COTEParticleTieReactorEx
// ***********************************************************

class COTEParticleTieReactorEx : public COTEParticleTieReactor
{
public:
	// ����
	virtual void Init(Ogre::MovableObject* mo);
	//// ����
	virtual void Update(Ogre::MovableObject* mo, float dTime);	

	void ClearContent();

protected:

	Ogre::Vector3			m_LastFramePosition;

	COTEBillBoardSet*		m_BBS;							// ������βЧ����bbs
	
	std::string				m_BBSName;						// ��βЧ����bbs��

	float					m_MinVelocity, m_MaxVelocity;	// ��β������������

	float					m_MinSize, m_MaxSize;			// ��β�������ӳߴ�

	Ogre::Vector3			m_MinDir, m_MaxDir;				// ��β���������˶�����

	float					m_LifeTime;						// ��������

	int						m_EmitRate;						// ���ӷ�����

	float					m_LastCreateTime;				// ��һ�δ�����ʱ��

};

// ***********************************************
// COTEParticleTieReactorExFactory 
// ***********************************************
class COTEParticleTieReactorExFactory : public CReactorFactoryBase
{
public:
		
	virtual bool ValidBind(const std::string& rObjType)
	{ 		
		return rObjType == "PS";
	}

	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
	{
		if(ValidBind(mo->getMovableType()))
			return new COTEParticleTieReactorEx();
		return NULL;
	}
	
	virtual void Delete(CReactorBase* rb)
	{
		if(rb)
			((COTEParticleTieReactorEx*)rb)->ClearContent();
		SAFE_DELETE(rb)
	}
	
	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "������βЧ��";
	}

};

// ***********************************************************
// class COTEEntityTieReactor
// ��������������ϵͳ�ϵķ�Ӧ����
// �ܲ�����ӰЧ��������������ϵͳĬ��Ч����
// ***********************************************************

class COTEEntityTieReactor :	public COTETieReactorBase
{
public:
	// ����
	virtual void Init(Ogre::MovableObject* mo);
	//// ����
	virtual void Update(Ogre::MovableObject* mo, float dTime);

private:

	void ClearContent();
};


// ***********************************************
// COTEEntityTieReactorFactory 
// ***********************************************
class COTEEntityTieReactorFactory : public CReactorFactoryBase
{
public:

	virtual bool ValidBind(const std::string& rObjType)
	{ 		
		return rObjType == "OTEEntity";
	}

	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
	{
		if(ValidBind(mo->getMovableType()))
			return new COTEEntityTieReactor();
		return NULL;
	}

	virtual void Delete(CReactorBase* rb)
	{
		SAFE_DELETE(rb)
	}

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "ʵ���Ӱ";
	}
};


// ***********************************************************
// class COTEBillBoardTieReactor
// ***********************************************************

class COTEBillBoardTieReactor :	public COTETieReactorBase
{
public:
	// ����
	virtual void Init(Ogre::MovableObject* mo);
	//// ����
	virtual void Update(Ogre::MovableObject* mo, float dTime);

protected:

	void ClearContent();

protected:

	Ogre::Vector3			m_LastFramePosition;

};

// ***********************************************
// COTEBillBoardTieReactorFactory 
// ***********************************************
class COTEBillBoardTieReactorFactory : public CReactorFactoryBase
{
public:

	virtual bool ValidBind(const std::string& rObjType)
	{ 		
		return rObjType == "BBS";
	}

	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
	{
		if(ValidBind(mo->getMovableType()))
			return new COTEBillBoardTieReactor();
		return NULL;
	}
	
	virtual void Delete(CReactorBase* rb)
	{
		SAFE_DELETE(rb)
	}

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "�����Ʋ�Ӱ";
	}

};


// ***********************************************************
// class COTEBillBoardTieReactorEx
// ***********************************************************

class COTEBillBoardTieReactorEx : public COTEBillBoardTieReactor
{
public:
	// ����
	virtual void Init(Ogre::MovableObject* mo);
	//// ����
	virtual void Update(Ogre::MovableObject* mo, float dTime);	

protected:

	Ogre::Vector3			m_LastFramePosition;

};

// ***********************************************
// COTEBillBoardTieReactorExFactory 
// ***********************************************
class COTEBillBoardTieReactorExFactory : public CReactorFactoryBase
{
public:

	virtual bool ValidBind(const std::string& rObjType)
	{ 		
		return rObjType == "BBS";
	}
	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
	{
		if(ValidBind(mo->getMovableType()))
			return new COTEBillBoardTieReactorEx();
		return NULL;
	}
	
	virtual void Delete(CReactorBase* rb)
	{
		SAFE_DELETE(rb)
	}

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "�����Ʋ�Ӱ2";
	}

};

// ***********************************************************
// class COTEBillBoardChainTieReactor
// ***********************************************************

class COTEBillBoardChainTieReactor :	public COTETieReactorBase
{
public:
	// ����
	virtual void Init(Ogre::MovableObject* mo);
	//// ����
	virtual void Update(Ogre::MovableObject* mo, float dTime);	

protected:

	void ClearContent();

protected:

	Ogre::Vector3			m_LastFramePosition;
};


// ***********************************************
// COTEBillBoardTieReactorFactory 
// ***********************************************
class COTEBillBoardChainTieReactorFactory : public CReactorFactoryBase
{
public:
	virtual bool ValidBind(const std::string& rObjType)
	{ 		
		return rObjType == "BBC";
	}

	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
	{
		if(ValidBind(mo->getMovableType()))
			return new COTEBillBoardChainTieReactor();
		return NULL;
	}
	
	virtual void Delete(CReactorBase* rb)
	{
		SAFE_DELETE(rb)
	}

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "������Ӱ";
	}
};

}
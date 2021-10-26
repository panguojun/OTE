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
	
	// 老位置

	Ogre::Vector3		m_LastCreatePos;
	
	// 时间

	float				m_EspTime;
	
	// 残影效果元素	

	std::list<TieDummyElm_t>		m_DummyList;

	std::list<TieDummyElm_t>		m_DummyGraveList;

	Ogre::MovableObject*			m_LastObj;

};


// ***********************************************************
// class COTEParticleTieReactor
// 用于作用在粒子系统上的反应器，
// 能产生残影效果（本来是粒子系统默认效果）
// ***********************************************************
class COTEParticleTieReactor : public COTETieReactorBase
{
public:
	// 触发
	virtual void Init(Ogre::MovableObject* mo);
	//// 更新
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
		rDesc = "粒子参影";
	}

};

// ***********************************************************
// class COTEParticleTieReactorEx
// ***********************************************************

class COTEParticleTieReactorEx : public COTEParticleTieReactor
{
public:
	// 触发
	virtual void Init(Ogre::MovableObject* mo);
	//// 更新
	virtual void Update(Ogre::MovableObject* mo, float dTime);	

	void ClearContent();

protected:

	Ogre::Vector3			m_LastFramePosition;

	COTEBillBoardSet*		m_BBS;							// 用于托尾效果的bbs
	
	std::string				m_BBSName;						// 托尾效果的bbs名

	float					m_MinVelocity, m_MaxVelocity;	// 托尾部分粒子速率

	float					m_MinSize, m_MaxSize;			// 托尾部分粒子尺寸

	Ogre::Vector3			m_MinDir, m_MaxDir;				// 托尾部分粒子运动方向

	float					m_LifeTime;						// 粒子寿命

	int						m_EmitRate;						// 粒子发射率

	float					m_LastCreateTime;				// 上一次创生的时间

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
		rDesc = "粒子托尾效果";
	}

};

// ***********************************************************
// class COTEEntityTieReactor
// 用于作用在粒子系统上的反应器，
// 能产生残影效果（本来是粒子系统默认效果）
// ***********************************************************

class COTEEntityTieReactor :	public COTETieReactorBase
{
public:
	// 触发
	virtual void Init(Ogre::MovableObject* mo);
	//// 更新
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
		rDesc = "实体参影";
	}
};


// ***********************************************************
// class COTEBillBoardTieReactor
// ***********************************************************

class COTEBillBoardTieReactor :	public COTETieReactorBase
{
public:
	// 触发
	virtual void Init(Ogre::MovableObject* mo);
	//// 更新
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
		rDesc = "公告牌参影";
	}

};


// ***********************************************************
// class COTEBillBoardTieReactorEx
// ***********************************************************

class COTEBillBoardTieReactorEx : public COTEBillBoardTieReactor
{
public:
	// 触发
	virtual void Init(Ogre::MovableObject* mo);
	//// 更新
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
		rDesc = "公告牌参影2";
	}

};

// ***********************************************************
// class COTEBillBoardChainTieReactor
// ***********************************************************

class COTEBillBoardChainTieReactor :	public COTETieReactorBase
{
public:
	// 触发
	virtual void Init(Ogre::MovableObject* mo);
	//// 更新
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
		rDesc = "条带参影";
	}
};

}
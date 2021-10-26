#pragma once
#include "OTETrack.h"
#include "OTECommon.h"

namespace OTE
{
// ***********************************
// class CReactorBase
// 生化反应效果器
// 是相对于轨道动画而言的，主要是使用
// 现成库函数（或者shader）来
// 对某个对象实现固定效果
// 它属于"作用(inter)" 的一种
// ***********************************

class _OTEExport CReactorBase : public CInterBase
{
public:

	// 触发
	virtual void Init(Ogre::MovableObject* mo){}

	// 更新
	virtual void Update(Ogre::MovableObject* mo, float dTime){}

	// 清理
	virtual void Clear(){}

public:

	CReactorBase() : 
	CInterBase(),
	m_StateFlag(0)
	{		
		m_IsBind = false; // 反应器需要动态绑定
	}

	virtual ~CReactorBase(){}

	virtual const std::string&	GetInterType()
	{
		static std::string sType = "Reactor";
		return sType;
	}

public:

	std::string		m_FactoryName;	// 创建它的工厂名
	std::string		m_ResName;		// 创建它的资源名
	unsigned int    m_StateFlag;	// 一些状态
	
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

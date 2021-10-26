#include "otereactorbase.h"
#include "OTEParticlesystem.h"
#include "OTEBillBoardSet.h"
#include "OTEParticleSystemXmlSerializer.h"

namespace OTE
{

// 粒子元素

struct ParticleElement_t
{
	Ogre::MovableObject* MO;
	float TimeEsp;
	Ogre::Vector3 Velocity;
	Ogre::Vector3 OffsetPos;
	float Size;
};

// 发射/影响器数据

struct EmitterAffectorData_t
{
	// 发射器属性

	int		emission_rate; 
	float	time_to_live ;

	float	width ;
	float	height;
	float	depth ;

	float	velocity_min; 
	float	velocity_max; 

	float	size_min;
	float	size_max;

	Ogre::Vector3 direction_min;
	Ogre::Vector3 direction_max;

	Ogre::ColourValue colour_range_start;
	Ogre::ColourValue colour_range_end; 

	// 影响器属性

	Ogre::Vector3 direction; 
	float strength;
	Ogre::ColourValue colorData; 

	EmitterAffectorData_t(COTESimpleEmitter* pemitter, COTEParticleSystemAffector* affector)
	{
		// 发射器属性

		emission_rate = pemitter->GetIntVal("发射率");
		time_to_live  = pemitter->GetfloatVal("寿命");	

		width  = pemitter->GetfloatVal("发射器_宽度");
		height = pemitter->GetfloatVal("发射器_高度");
		depth  = pemitter->GetfloatVal("发射器_长度");	

		velocity_min = pemitter->GetfloatVal("最小速率");
		velocity_max = pemitter->GetfloatVal("最大速率");	

		size_min = pemitter->GetfloatVal("最小尺寸");
		size_max = pemitter->GetfloatVal("最大尺寸");

		direction_min = pemitter->GetVector3Val("最小方向");
		direction_max = pemitter->GetVector3Val("最大方向");

		colour_range_start = pemitter->GetCorVal("最小颜色");
		colour_range_end   = pemitter->GetCorVal("最大颜色");

		// 影响器属性
		
		direction = affector->GetVector3Val("direction");

		strength = affector->GetfloatVal("strength");

		colorData = affector->GetCorVal("colorData");
	}



};

// ***********************************************************
// class COTEEmiterReactor
// ***********************************************************
class COTEEmiterAffectorReactorBase : public CReactorBase
{
public:
	// 初始化
	virtual void Init(Ogre::MovableObject* mo);

	// 更新
	virtual void Update(Ogre::MovableObject* mo, float dTime);	
		
	// 清理
	virtual void Clear();

protected:

	virtual void CreateElement(Ogre::MovableObject* mo, const Ogre::Vector3& rPos) = 0;

	virtual void InitElement(Ogre::MovableObject* mo, ParticleElement_t& pe) = 0;

	virtual void UpdateElement(Ogre::MovableObject* mo, ParticleElement_t& pe) = 0;


public:

	std::string				m_ParticleResName;

protected:

	std::vector<ParticleElement_t>			m_ElmList;

	EmitterAffectorData_t*					m_EmitterAffectorData;

};

// ***********************************************************
// COTEBBSEmiterAffector
// ***********************************************************
class COTEBBSEmiterAffector : public COTEEmiterAffectorReactorBase
{
protected:
	
	virtual void CreateElement(Ogre::MovableObject* mo, const Ogre::Vector3& rPos);

	virtual void InitElement(Ogre::MovableObject* mo, ParticleElement_t& pe);

	virtual void UpdateElement(Ogre::MovableObject* mo, ParticleElement_t& pe);
};
// ***********************************************************
// COTEEntityEmiterAffector
// ***********************************************************
class COTEEntityEmiterAffector : public COTEEmiterAffectorReactorBase
{
protected:
	
	virtual void CreateElement(Ogre::MovableObject* mo, const Ogre::Vector3& rPos);

	virtual void InitElement(Ogre::MovableObject* mo, ParticleElement_t& pe);

	virtual void UpdateElement(Ogre::MovableObject* mo, ParticleElement_t& pe);
};

// ***********************************************
// COTEEmiterReactorFactory 
// ***********************************************
class COTEEmiterReactorFactory : public CReactorFactoryBase
{
public:	

	virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
	{			
		COTEEmiterAffectorReactorBase* emitter = NULL;

		if(mo->getMovableType() == "BBS")
		{
			emitter = new COTEBBSEmiterAffector();	
			emitter->m_ParticleResName = rResName;			
		}
		else if(mo->getMovableType() == "OTEEntity")
		{			
			emitter = new COTEEntityEmiterAffector();	
			emitter->m_ParticleResName = rResName;	
		}

		return emitter;
	}
	
	virtual void Delete(CReactorBase* rb)
	{
		SAFE_DELETE(rb)
	}

	virtual void GetDescription(std::string& rDesc)
	{
		rDesc = "万能发射器";
	}
};


}

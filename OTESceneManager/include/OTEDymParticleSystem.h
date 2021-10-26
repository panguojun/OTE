#pragma once
#include "OTEStaticInclude.h"

// ********************************************
// COTEParticle
// 粒子单体， 粒子可以任意实体
// ********************************************
class COTEParticle
{
public:

	float			m_LifeTime;		// 寿命

	void*			m_EntityPtr;	// 粒子可以是任何实体

public:
	
};

// ********************************************
// COTEEmitter
// 发射器，种类比较多
// ********************************************
class COTEEmitter
{
public:
	void Emiter(){}
};

// ********************************************
// COTEAffector
// 影响器 
// ********************************************
class COTEAffector
{
public:
	void Affector(){}
};


// ********************************************
// COTEDymParticleSystem
// 粒子系统首先是一个粒子容器，
// 另外粒子系统要把发射器，影响器也包含进来。
// 一个粒子系统的实体对应一个完整的粒子效果。
// ********************************************
class COTEDymParticleSystem
{
public:
	
	void Init();

	void Destroy();

	void Proccess();

public:

	COTEDymParticleSystem(void){}
	~COTEDymParticleSystem(void){}

public:

	// 系统内粒子列表

	std::vector<COTEParticle*>	m_Particles;
};

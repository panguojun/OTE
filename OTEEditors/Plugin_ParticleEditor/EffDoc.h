#pragma once
#include "OTEParticleSystem.h"
// *************************************
// 特效数据
// *************************************

class CEffDoc
{
public:
	static CEffDoc	s_Inst;
	
public:
	CEffDoc(void);
	~CEffDoc(void);

public:
	OTE::COTEParticleSystem*		m_ParticleSystem;
	
	std::string					m_ParticleSystemTempName;

};

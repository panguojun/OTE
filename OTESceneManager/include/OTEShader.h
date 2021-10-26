#pragma once
#include "OTEStaticInclude.h"
#include "OgreD3D9RenderSystem.h"

namespace OTE
{

// ***********************************
// COTEShaderParams
// Shader 参数 
// ***********************************
class _OTEExport COTEShaderParams
{
public:
	typedef std::pair<int, std::string>		ParamElm_t;

public:	

	static void PopulateParameterNames(Ogre::D3D9HLSLProgram* d3d9HLSLProgram, std::list<ParamElm_t>& rParamList);

	static void ProcessParamElement(
								Ogre::D3D9HLSLProgram* d3d9HLSLProgram, 
								D3DXHANDLE parent, 
								Ogre::String prefix,
								unsigned int index, 
								ParamElm_t& rParamElem);

};

// ***********************************
// COTEShaderCtrller
// Shader 控制器 
// ***********************************

class COTEShaderCtrller
{
public:
	// 参数
	//COTEShaderParams	m_ShaderParams;

public:
		
	// 更新参数　向ｓｈａｄｅｒ传递参数
	virtual void UpdateParams(){}
};

}
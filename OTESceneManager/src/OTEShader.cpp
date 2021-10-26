#include "oteshader.h"
#include "OgreD3D9HLSLProgram.h"

using namespace Ogre;
using namespace OTE;

//-----------------------------------------------------------------------
void COTEShaderParams::PopulateParameterNames(D3D9HLSLProgram* d3d9HLSLProgram, std::list<ParamElm_t>& rParamList)
{	
	if(!d3d9HLSLProgram->mpConstTable)
		return;

	rParamList.clear();

    // Get contents of the constant table
    D3DXCONSTANTTABLE_DESC desc;
    HRESULT hr = d3d9HLSLProgram->mpConstTable->GetDesc(&desc);

    if (FAILED(hr))
    {
        OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, 
            "Cannot retrieve constant descriptions from HLSL program.", 
            "D3D9HLSLProgram::PopulateParameterNames");
    }
    // Iterate over the constants
    for (unsigned int i = 0; i < desc.Constants; ++i)
    {
        // Recursively descend through the structure levels
        // Since D3D9 has no nice 'leaf' method like Cg (sigh)
		rParamList.resize(rParamList.size() + 1);	
		ParamElm_t& paramElem = rParamList.back();
        ProcessParamElement(d3d9HLSLProgram, NULL, "", i, paramElem);	
    }    
}

//-----------------------------------------------------------------------
void COTEShaderParams::ProcessParamElement(
								D3D9HLSLProgram* d3d9HLSLProgram, 
								D3DXHANDLE parent, 
								Ogre::String prefix,
								unsigned int index, 
								ParamElm_t& rParamElem)
{
    D3DXHANDLE hConstant = d3d9HLSLProgram->mpConstTable->GetConstant(parent, index);

    // Since D3D HLSL doesn't deal with naming of array and struct parameters
    // automatically, we have to do it by hand

    D3DXCONSTANT_DESC desc;
    unsigned int numParams = 1;
    HRESULT hr = d3d9HLSLProgram->mpConstTable->GetConstantDesc(hConstant, &desc, &numParams);
    if (FAILED(hr))
    {
        OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, 
            "Cannot retrieve constant description from HLSL program.", 
            "D3D9HLSLProgram::processParamElement");
    }

    String paramName = desc.Name;
    // trim the odd '$' which appears at the start of the names in HLSL
    if (paramName.at(0) == '$')
        paramName.erase(paramName.begin());		

    // If it's an array, elements will be > 1
    for (unsigned int e = 0; e < desc.Elements; ++e)
	{
		if (desc.Class == D3DXPC_STRUCT)
		{				
			// work out a new prefix for nested members, if it's an array, we need an index
			if (desc.Elements > 1)
				prefix = prefix + paramName + "[" + StringConverter::toString(e) + "].";
			else
				prefix = prefix + paramName + ".";
			// Cascade into struct
			for (unsigned int i = 0; i < desc.StructMembers; ++i)
			{
				ProcessParamElement(d3d9HLSLProgram, hConstant, prefix, i, rParamElem);
			}
				
		}
		else
		{
			// Process params
			if (desc.Type == D3DXPT_FLOAT || desc.Type == D3DXPT_INT || desc.Type == D3DXPT_BOOL)
			{
				size_t paramIndex = desc.RegisterIndex;
				String name = prefix + paramName;
				// If this is an array, need to append element index
				if (desc.Elements > 1)
					name += "[" + StringConverter::toString(e) + "]";

				//params->_mapParameterNameToIndex(name, paramIndex);
				rParamElem.first = paramIndex;
				rParamElem.second = name;				
			}
		}
	}
    
        
}

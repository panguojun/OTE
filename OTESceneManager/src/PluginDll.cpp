#include "PluginDll.h"

using namespace OTE;

// ----------------------------------------------
CPluginDll::CPluginDll( const char* strName )
{
	m_Name = strName;	       
	m_hInst = NULL;
}

// ----------------------------------------------
bool CPluginDll::Load()
{
	m_hInst = (HINSTANCE__*)DYNLIB_LOAD( m_Name.c_str());

	return true; 
}
// ----------------------------------------------
void CPluginDll::Unload()
{
	DYNLIB_UNLOAD( m_hInst );

}
// ----------------------------------------------
void* CPluginDll::GetSymbol( const char* strSymName )
{
    return (void*)DYNLIB_GETSYM( m_hInst, strSymName );
}
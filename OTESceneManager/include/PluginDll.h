#pragma once

#include <vector> 
#include <string>
#include <windows.h>

// *********************************************************
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#    define DYNLIB_LOAD( a ) LoadLibrary( a )
#    define DYNLIB_UNLOAD( a ) FreeLibrary( a )
#    define DYNLIB_GETSYM( a, b ) GetProcAddress( a, b )
#endif

// *********************************************************

namespace OTE
{
// *********************************************************
// CPluginDll
// *********************************************************

class CPluginDll  
{
public:
	bool Load();
	void Unload();

	bool StartPlugins();
	void* GetSymbol( const char* strSymName );

public:
	std::string		m_Name;	        
    HINSTANCE__*   	m_hInst;
		   
public:
	CPluginDll(const char* strName);

};

}


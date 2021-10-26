#pragma once

// -----------------------------------------------------------------------
// Windows Settings
// -----------------------------------------------------------------------

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

// -----------------------------------------------------------------------
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
#   ifdef PLUGIN_OTE_EXPORTS 
#       define _OTEExport __declspec(dllexport) 
#   else 
#       define _OTEExport __declspec(dllimport) 
#   endif 
#else 
#   define _OTEExport 
#endif 

// -----------------------------------------------------------------------
// 关于发布/调试版本
// -----------------------------------------------------------------------

#	define	GAME_DEBUG		// 带编辑器的版本

#	define	GAME_DEBUG1		// 发布版本与编辑版本控制

//#	define	TIMECOST_ANS	// 性能分析

#	define	__OTE			// 模块可能在其他地方应用，这里的表示在OTE中使用

//#	define	__ZG			// 捉鬼

#	define	__OTEZH			// 纵横

// -----------------------------------------------------------------------
// Static heads
// -----------------------------------------------------------------------
#include "Ogre.h"
#include <windows.h>

// -----------------------------------------------------------------------
// 常用宏
// -----------------------------------------------------------------------

/// 信息输出 

#	define	OTE_MSG_FAIL(msg)	{std::stringstream __ss__; __ss__ << msg; ::MessageBox(NULL, __ss__.str().c_str(), "失败", MB_OK); }
#	define	OTE_MSG_ERR(msg)	{std::stringstream __ss__; __ss__ << msg; ::MessageBox(NULL, __ss__.str().c_str(), "错误", MB_ICONERROR); }

#	define	OTE_MSG(msg, title)	{std::stringstream __ss__; __ss__ << msg; ::MessageBox(NULL, __ss__.str().c_str(), title, MB_OK); }
#	define	OTE_MSG_TYPE(msg, title, type)	{std::stringstream __ss__; __ss__ << msg; ::MessageBox(NULL, __ss__.str().c_str(), title, type); }

#	define	OTE_LOGMSG(msg) {std::stringstream __ss__; __ss__ << "<OTE LOG> " << msg; Ogre::LogManager::getSingleton().logMessage(__ss__.str());}

#	define	OTE_TRACE(msg) {std::stringstream __ss__; __ss__ << "<OTE TRACE> " << msg << "\n"; ::OutputDebugString(__ss__.str().c_str());OTE_LOGMSG(msg)}
#	define	OTE_TRACE_ERR(msg) {std::stringstream __ss__; __ss__ << "<OTE TRACE !ERR> " << msg << "\n"; ::OutputDebugString(__ss__.str().c_str());OTE_LOGMSG(msg)}
#	define	OTE_TRACE_WARNING(msg) {std::stringstream __ss__; __ss__ << "<OTE TRACE !WARNING> " << msg << "\n"; ::OutputDebugString(__ss__.str().c_str());OTE_LOGMSG(msg)}


/// 错误报告

#   define  OTE_ASSERT(a)  if(!(a)) {OTE_MSG_ERR("ASSERT失败! " << #a << " 错误位于 FILE: " << __FILE__ << " LINE: " << __LINE__ <<  " FUNCTION: " << __FUNCTION__) throw;}

// 资源组

#define RES_GROUP		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME  // "PakFileSystem"

/// commonly used macros

#	ifndef	SAFE_DELETE
#	define	SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#	endif

#	ifndef	SAFE_DELETE_ARRAY
#	define	SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#	endif

#	ifndef	SAFE_RELEASE
#	define	SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#	endif

/// mapping

#	define MAPPING(keyType, valType, mapping)	static HashMap<keyType, valType>		mapping;
#	define ADDMAPPING(key, val, mapping)		mapping[key] = val;
#	define GETMAPPING(key, mapping)				mapping[key]

#	define _MAPPING(keyType, valType)			MAPPING(keyType, valType, _mapping)
#	define _ADDMAPPING(key, val)				ADDMAPPING(key, val, _mapping)
#	define _GETMAPPING(key)						GETMAPPING(key, _mapping)

/// 字符串相关宏

#	define  LINK_STR(inS, outS)	std::string outS; {std::stringstream __ss__; __ss__ << inS; outS = __ss__.str();}

/// 资源管理

#define TEX_RESMGR_LOAD(fileName)	Ogre::TextureManager::getSingleton().load(fileName,		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME)

#define RESMGR_LOAD(fileName)		Ogre::ResourceGroupManager::getSingleton().openResource(fileName,	Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME)

#define RES_MGR		Ogre::ResourceGroupManager::getSingleton()

#define CHECK_RES(fileName)	RES_MGR.resourceExists(ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, fileName)

#define OPEN_RES(fileName) Ogre::ResourceGroupManager::getSingleton().openResource(fileName);

/// 数值

#define CG_BASE_ID		100

#define	MAX_FLOAT		1000000.0f

#define	MIN_FLOAT		0.0000001f

// -----------------------------------------------------------------------
// 方便的宏
// -----------------------------------------------------------------------

#define  OTE_OBJ		Ogre::MovableObject				// 可移动对象

#define  OTE_ACTOR		OTE::COTEActorEntity			// 角色

#define  OTE_STATIC		OTE::COTEActorEntity			// 场景物件

#define  USING_OTE		using namespace Ogre;	using namespace OTE;


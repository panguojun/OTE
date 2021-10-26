// Plugin_ZHFileConvertor.h : Plugin_ZHFileConvertor DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// 主符号


// CPlugin_ZHFileConvertorApp
// 有关此类实现的信息，请参阅 Plugin_ZHFileConvertor.cpp
//

class CPlugin_ZHFileConvertorApp : public CWinApp
{
public:
	CPlugin_ZHFileConvertorApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

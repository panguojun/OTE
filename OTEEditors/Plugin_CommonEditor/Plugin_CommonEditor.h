// Plugin_CommonEditor.h : Plugin_CommonEditor DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// 主符号


// CPlugin_CommonEditorApp
// 有关此类实现的信息，请参阅 Plugin_CommonEditor.cpp
//

class CPlugin_CommonEditorApp : public CWinApp
{
public:
	CPlugin_CommonEditorApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

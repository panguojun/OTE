// Plugin_MaterialEditor.h : Plugin_MaterialEditor DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// 主符号


// CPlugin_MaterialEditorApp
// 有关此类实现的信息，请参阅 Plugin_MaterialEditor.cpp
//

class CPlugin_MaterialEditorApp : public CWinApp
{
public:
	CPlugin_MaterialEditorApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

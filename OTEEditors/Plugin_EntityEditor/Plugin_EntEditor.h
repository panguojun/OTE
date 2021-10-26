// Plugin_MeshEditor.h : Plugin_MeshEditor DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// 主符号


// CPlugin_EntEditorApp
// 有关此类实现的信息，请参阅 Plugin_MeshEditor.cpp
//

class CPlugin_EntEditorApp : public CWinApp
{
public:
	CPlugin_EntEditorApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

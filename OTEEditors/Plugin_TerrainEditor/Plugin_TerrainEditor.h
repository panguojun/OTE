// Plugin_TerrainEditor.h : Plugin_TerrainEditor DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// 主符号


// CPlugin_TerrainEditorApp
// 有关此类实现的信息，请参阅 Plugin_TerrainEditor.cpp
//

class CPlugin_TerrainEditorApp : public CWinApp
{
public:
	CPlugin_TerrainEditorApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

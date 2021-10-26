// Plugin_ActorEditor.h : Plugin_ActorEditor DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// 主符号


// CPlugin_ActorEditorApp
// 有关此类实现的信息，请参阅 Plugin_ActorEditor.cpp
//

class CPlugin_ActorEditorApp : public CWinApp
{
public:
	CPlugin_ActorEditorApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()

};

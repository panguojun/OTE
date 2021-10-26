// Plugin_ActorEditor.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "Plugin_ActorEditor.h"
#include ".\plugin_actoreditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPlugin_ActorEditorApp

BEGIN_MESSAGE_MAP(CPlugin_ActorEditorApp, CWinApp)

END_MESSAGE_MAP()


// CPlugin_ActorEditorApp 构造

CPlugin_ActorEditorApp::CPlugin_ActorEditorApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CPlugin_ActorEditorApp 对象

CPlugin_ActorEditorApp theApp;


// CPlugin_ActorEditorApp 初始化

BOOL CPlugin_ActorEditorApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}


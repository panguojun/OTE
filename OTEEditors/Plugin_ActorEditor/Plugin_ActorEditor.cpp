// Plugin_ActorEditor.cpp : ���� DLL �ĳ�ʼ�����̡�
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


// CPlugin_ActorEditorApp ����

CPlugin_ActorEditorApp::CPlugin_ActorEditorApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CPlugin_ActorEditorApp ����

CPlugin_ActorEditorApp theApp;


// CPlugin_ActorEditorApp ��ʼ��

BOOL CPlugin_ActorEditorApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}


// Plugin_CommonEditor.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "Plugin_ParticleEditor.h"

#define OGRE_DEBUG_MEMORY_MANAGER 1
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

// CPlugin_CommonEditorApp

BEGIN_MESSAGE_MAP(CPlugin_ParticleEditorApp, CWinApp)
END_MESSAGE_MAP()


// CPlugin_CommonEditorApp ����

CPlugin_ParticleEditorApp::CPlugin_ParticleEditorApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CPlugin_CommonEditorApp ����

CPlugin_ParticleEditorApp theApp;


// CPlugin_CommonEditorApp ��ʼ��

BOOL CPlugin_ParticleEditorApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

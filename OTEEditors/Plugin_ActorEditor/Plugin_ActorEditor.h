// Plugin_ActorEditor.h : Plugin_ActorEditor DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// CPlugin_ActorEditorApp
// �йش���ʵ�ֵ���Ϣ������� Plugin_ActorEditor.cpp
//

class CPlugin_ActorEditorApp : public CWinApp
{
public:
	CPlugin_ActorEditorApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()

};

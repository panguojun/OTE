// Plugin_MeshEditor.h : Plugin_MeshEditor DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// CPlugin_EntEditorApp
// �йش���ʵ�ֵ���Ϣ������� Plugin_MeshEditor.cpp
//

class CPlugin_EntEditorApp : public CWinApp
{
public:
	CPlugin_EntEditorApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

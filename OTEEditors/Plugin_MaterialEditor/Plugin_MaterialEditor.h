// Plugin_MaterialEditor.h : Plugin_MaterialEditor DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// CPlugin_MaterialEditorApp
// �йش���ʵ�ֵ���Ϣ������� Plugin_MaterialEditor.cpp
//

class CPlugin_MaterialEditorApp : public CWinApp
{
public:
	CPlugin_MaterialEditorApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

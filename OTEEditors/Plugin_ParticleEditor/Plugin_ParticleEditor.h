// Plugin_CommonEditor.h : Plugin_CommonEditor DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// CPlugin_CommonEditorApp
// �йش���ʵ�ֵ���Ϣ������� Plugin_CommonEditor.cpp
//

class CPlugin_ParticleEditorApp : public CWinApp
{
public:
	CPlugin_ParticleEditorApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

// Plugin_TerrainEditor.h : Plugin_TerrainEditor DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// CPlugin_TerrainEditorApp
// �йش���ʵ�ֵ���Ϣ������� Plugin_TerrainEditor.cpp
//

class CPlugin_TerrainEditorApp : public CWinApp
{
public:
	CPlugin_TerrainEditorApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

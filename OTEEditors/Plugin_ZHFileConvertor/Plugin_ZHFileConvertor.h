// Plugin_ZHFileConvertor.h : Plugin_ZHFileConvertor DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ������


// CPlugin_ZHFileConvertorApp
// �йش���ʵ�ֵ���Ϣ������� Plugin_ZHFileConvertor.cpp
//

class CPlugin_ZHFileConvertorApp : public CWinApp
{
public:
	CPlugin_ZHFileConvertorApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

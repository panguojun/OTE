#   ifdef GAME_DEBUG

	#include "stdafx.h" 

	// AppFrame.h : AppFrame Ӧ�ó������ͷ�ļ�
	//
	#pragma once

	#ifndef __AFXWIN_H__
		#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h�� 
	#endif

	#include "resource.h"       // ������


	// CAppFrameApp:
	// �йش����ʵ�֣������ AppFrame.cpp
	//

	class CAppFrameApp : public CWinApp
	{
	public:
		CAppFrameApp();
		~CAppFrameApp()
		{

		}

	// ��д
	public:
		virtual BOOL InitInstance();

	// ʵ��
		DECLARE_MESSAGE_MAP()
	};

	extern CAppFrameApp theApp;

#   endif 

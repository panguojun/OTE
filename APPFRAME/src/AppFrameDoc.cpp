#include "stdafx.h"

#   ifdef GAME_DEBUG
	
	#include "AppFrame.h"

	#include "AppFrameDoc.h"

	#ifdef _DEBUG
	#define new DEBUG_NEW
	#endif


	// CAppFrameDoc

	IMPLEMENT_DYNCREATE(CAppFrameDoc, CDocument)

	BEGIN_MESSAGE_MAP(CAppFrameDoc, CDocument)
	END_MESSAGE_MAP()


	// CAppFrameDoc 构造/析构

	CAppFrameDoc::CAppFrameDoc()
	{
		// TODO: 在此添加一次性构造代码

	}

	CAppFrameDoc::~CAppFrameDoc()
	{
	}

	BOOL CAppFrameDoc::OnNewDocument()
	{
		if (!CDocument::OnNewDocument())
			return FALSE;

		// TODO: 在此添加重新初始化代码
		// (SDI 文档将重用该文档)

		return TRUE;
	}




	// CAppFrameDoc 序列化

	void CAppFrameDoc::Serialize(CArchive& ar)
	{
		if (ar.IsStoring())
		{
			// TODO: 在此添加存储代码
		}
		else
		{
			// TODO: 在此添加加载代码
		}
	}


	// CAppFrameDoc 诊断

	#ifdef _DEBUG
	void CAppFrameDoc::AssertValid() const
	{
		CDocument::AssertValid();
	}

	void CAppFrameDoc::Dump(CDumpContext& dc) const
	{
		CDocument::Dump(dc);
	}
	#endif //_DEBUG


	// CAppFrameDoc 命令

# endif
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


	// CAppFrameDoc ����/����

	CAppFrameDoc::CAppFrameDoc()
	{
		// TODO: �ڴ����һ���Թ������

	}

	CAppFrameDoc::~CAppFrameDoc()
	{
	}

	BOOL CAppFrameDoc::OnNewDocument()
	{
		if (!CDocument::OnNewDocument())
			return FALSE;

		// TODO: �ڴ�������³�ʼ������
		// (SDI �ĵ������ø��ĵ�)

		return TRUE;
	}




	// CAppFrameDoc ���л�

	void CAppFrameDoc::Serialize(CArchive& ar)
	{
		if (ar.IsStoring())
		{
			// TODO: �ڴ���Ӵ洢����
		}
		else
		{
			// TODO: �ڴ���Ӽ��ش���
		}
	}


	// CAppFrameDoc ���

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


	// CAppFrameDoc ����

# endif
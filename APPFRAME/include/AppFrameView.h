#pragma once
#include "MainFrm.h"

class CAppFrameView : public CView
{
protected: // �������л�����
	CAppFrameView();
	DECLARE_DYNCREATE(CAppFrameView)

// ����
public:
	CAppFrameDoc* GetDocument() const;

// ����
public:

// ��д
public:

	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	

// ʵ��
public:
	virtual ~CAppFrameView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
	
#endif

	afx_msg void OnLButtonDown( UINT, CPoint ); 
	afx_msg void OnLButtonDblClk( UINT, CPoint ); 	
	afx_msg void OnLButtonUp( UINT, CPoint ); 
	afx_msg void OnRButtonDown( UINT, CPoint ); 
	afx_msg void OnRButtonUp( UINT, CPoint ); 
	afx_msg void OnMouseMove( UINT, CPoint );
	afx_msg BOOL OnMouseWheel( UINT, short, CPoint );

private:

	int					m_nMousePosX;
	int					m_nMousePosY;

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // AppFrameView.cpp �ĵ��԰汾
inline CAppFrameDoc* CAppFrameView::GetDocument() const
   { return reinterpret_cast<CAppFrameDoc*>(m_pDocument); }
#endif


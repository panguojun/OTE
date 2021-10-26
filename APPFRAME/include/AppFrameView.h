#pragma once
#include "MainFrm.h"

class CAppFrameView : public CView
{
protected: // 仅从序列化创建
	CAppFrameView();
	DECLARE_DYNCREATE(CAppFrameView)

// 属性
public:
	CAppFrameDoc* GetDocument() const;

// 操作
public:

// 重写
public:

	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	

// 实现
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

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // AppFrameView.cpp 的调试版本
inline CAppFrameDoc* CAppFrameView::GetDocument() const
   { return reinterpret_cast<CAppFrameDoc*>(m_pDocument); }
#endif


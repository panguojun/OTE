#pragma once

class CMainFrame : public CFrameWnd
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CToolBar				m_wndToolBar;

protected:  // 控件条嵌入成员
	CStatusBar				m_wndStatusBar;	
	
	int						m_nMousePosX;
	int						m_nMousePosY;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy( ); 
	afx_msg void OnActivate(UINT nState, CWnd * pWndOther, BOOL bMinmized);
	afx_msg void OnToolBar(UINT nID);	

	DECLARE_MESSAGE_MAP()

public:

	void ClearPlugins();
	
	void LoadPlugins(const char* szPluginCfg);

	void InitOgre();

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnToolTipText(NMHDR*   pnmhdr,LRESULT   *result);
	
};



#pragma once
#include "afxcmn.h"
#include "Resource.h"

// CSliderEditor 对话框

class CSliderEditor : public CDialog
{
	DECLARE_DYNAMIC(CSliderEditor)

public:
	typedef void (*pCBFun_t) (float v, CString ud);
	typedef void (*pCBUpdateFun_t) ();

	static CSliderEditor	s_Inst;
	CSliderEditor(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSliderEditor();

// 对话框数据
	enum { IDD = IDD_SLIDER_EDITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:	

	void Show(pCBFun_t pCBFun, pCBUpdateFun_t pCBUFun, CString ud, float v, float min, float max, CRect rt, float scl = 10.0f);

	void Hide();

	BOOL OnInitDialog();

	virtual void OnOK();

public:

	pCBFun_t	m_pCBFun;
	pCBUpdateFun_t	m_pCBUpdateFun;
	CSliderCtrl m_SliderCtrl;
	CString		m_UserData;
	float		m_Scaler;

	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};

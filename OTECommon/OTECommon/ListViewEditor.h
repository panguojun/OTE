#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "resource.h"

// CListViewEditor �Ի���

class CListViewEditor : public CDialog
{
	DECLARE_DYNAMIC(CListViewEditor)

public:
	typedef void (*NotifyFuncHdl_t)(int, const char*);
	typedef void (*OKFuncHdl_t)();
	typedef void (*CancelFuncHdl_t)();

public:
	enum{eNumberEditor, eTextEditor};
	CListViewEditor(int nType, const CString& rText, const CRect& rRect, int NumUpper = 100, int NumLower = 0, CWnd* pParent = NULL, NotifyFuncHdl_t fun = NULL, 
		OKFuncHdl_t okfun = NULL);   // ��׼���캯��
	virtual ~CListViewEditor();

// �Ի�������
	enum { IDD = IDD_DLG_LVE };
	
	// ȷ��

	void OprOK();

protected:
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();


	DECLARE_MESSAGE_MAP()
public:
	CEdit		m_EditBox;
	CRect		m_Rect;

public:
	static CString s_Text;
	
public:
	afx_msg void OnEnChangeEdit1();
	CSpinButtonCtrl m_SpinBtn;
	int			m_PtrOffset;

	int			m_userState;

protected:
	int			m_Type;
	int			m_NumUpper;
	int			m_NumLower;

public:	

	NotifyFuncHdl_t		m_pNotifyFuncHdl;
	OKFuncHdl_t			m_pOKFunHdl;

public:
	afx_msg void OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult);

	void SetCallBackFun(NotifyFuncHdl_t fun);

	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnEnKillfocusEdit1();
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

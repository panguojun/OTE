#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CListViewEditors 对话框

class CListViewEditors : public CDialog
{
	DECLARE_DYNAMIC(CListViewEditors)

public:
	enum{eNumberEditor, eTextEditor};
	CListViewEditors(int nType, const CString& rText, const CRect& rRect, int NumUpper = 100, int NumLower = 0, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CListViewEditors();

// 对话框数据
	enum { IDD = IDD_DLG_LVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditBox;
	CSpinButtonCtrl m_SpinBtn;
	afx_msg void OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEdit1();

public :
	CRect		m_Rect;
	static CString s_Text;

protected :
	int			m_Type;
	int			m_NumUpper;
	int			m_NumLower;
};

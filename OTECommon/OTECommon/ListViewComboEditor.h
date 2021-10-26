#pragma once
#include "afxwin.h"
#include <string>

// CListViewComboEditor 对话框

class CListViewComboEditor : public CDialog
{
	DECLARE_DYNAMIC(CListViewComboEditor)

public:
	CListViewComboEditor(const CString& rText, const CRect& rRect, const char* szStringList, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CListViewComboEditor();

public:
	CRect			m_Rect;
	std::string		mStringList;
// 对话框数据
	enum { IDD = IDD_DIALOG8 };
public:
	static CString	s_Text;
	static int		s_SelIndex;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	void OnOK();

public:
	CComboBox m_ComBox;
};

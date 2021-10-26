#pragma once
#include "afxwin.h"
#include <string>

// CListViewComboEditor �Ի���

class CListViewComboEditor : public CDialog
{
	DECLARE_DYNAMIC(CListViewComboEditor)

public:
	CListViewComboEditor(const CString& rText, const CRect& rRect, const char* szStringList, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CListViewComboEditor();

public:
	CRect			m_Rect;
	std::string		mStringList;
// �Ի�������
	enum { IDD = IDD_DIALOG8 };
public:
	static CString	s_Text;
	static int		s_SelIndex;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	void OnOK();

public:
	CComboBox m_ComBox;
};

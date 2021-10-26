#pragma once
#include "afxwin.h"


// CAddEffEntityDlg �Ի���

class CAddEffEntityDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddEffEntityDlg)

public:
	CAddEffEntityDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAddEffEntityDlg();

// �Ի�������
	enum { IDD = IDD_ADDENT };

	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnCbnSelchangeCombo1();

	CListBox m_SubTypeList;
	CComboBox m_TypeCombo;
	afx_msg void OnLbnSelchangeList2();
	afx_msg void OnLbnDblclkList2();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnCbnDropdownCombo1();
	afx_msg void OnCbnSelchangeCombo2();
	CComboBox m_ReactorTypeListCombo;
	CListBox m_ReactorSubTypeList;
	afx_msg void OnLbnSelchangeList3();
	afx_msg void OnBnClickedButton3();
};

#pragma once
#include "resource.h"
#include "afxwin.h"

// CReactorLibDlg 对话框

class CReactorLibDlg : public CDialog
{
	DECLARE_DYNAMIC(CReactorLibDlg)

public:
	CReactorLibDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CReactorLibDlg();

// 对话框数据
	enum { IDD = IDD_REACTORLIBDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnLbnSelchangeList3();
	CComboBox m_ReactorFactoryCombo;
	CListBox m_ReactorSubTypeList;
	afx_msg void OnCbnSelchangeCombo2();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

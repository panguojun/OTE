#pragma once
#include "resource.h"
#include "afxwin.h"

// CReactorLibDlg �Ի���

class CReactorLibDlg : public CDialog
{
	DECLARE_DYNAMIC(CReactorLibDlg)

public:
	CReactorLibDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CReactorLibDlg();

// �Ի�������
	enum { IDD = IDD_REACTORLIBDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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

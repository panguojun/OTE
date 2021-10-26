#pragma once
#include "afxwin.h"
#include "Resource.h"

// CSectorDlg 对话框

class CSectorDlg : public CDialog
{
	DECLARE_DYNAMIC(CSectorDlg)

public:
	CSectorDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSectorDlg();

// 对话框数据
	enum { IDD = IDD_SECTORLOADER };
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

public:
	void AutoLoadScene();
	void SaveScene();

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_SectorNameList;
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
